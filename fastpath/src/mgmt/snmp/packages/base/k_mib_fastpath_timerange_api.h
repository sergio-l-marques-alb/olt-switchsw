/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename  k_mib_fastpath_timerange_api.h
*
* @purpose   Time Ranges - MIB implementation
*
* @create    19 Dec 2009
*
* @author    Siva Mannem
*
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "usmdb_timerange_api.h"
#include "timerange_exports.h"


/*********************************************************************
* @purpose  Get the  the time range entry
*
* @param    UnitIndex          @b{(input)}        the unit for this operation
* @param    timeRangeEntryData @b{(input/output)} timeRangeEntry data
* @param    nominator          @b{(MIB element)}  the MIB element on which the get
*                                                 operation is performed.
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangeEntryGet(L7_uint32 UnitIndex, timeRangeEntry_t *timeRangeEntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_char8  snmp_buffer[SNMP_BUFFER_LEN];
  timeRangeStatus_t status;
  if (usmDbTimeRangeIndexCheckValid(UnitIndex, timeRangeEntryData->timeRangeIndex) == L7_SUCCESS)
  {
    CLR_VALID(nominator, timeRangeEntryData->valid);

    /*
     * if ( nominator != -1 ) condition is added to all the case statemen
     * for storing all the values to support the undo functionality.
     */
    switch (nominator)
    {
      case -1:
      case I_timeRangeIndex:
        rc = L7_SUCCESS;
        if( nominator != -1) break;
        /* else pass through */

      case I_timeRangeName:
        memset(snmp_buffer, 0, (size_t)SNMP_BUFFER_LEN);
        if ((usmDbTimeRangeNameGet(UnitIndex,
                                   timeRangeEntryData->timeRangeIndex,
                                   snmp_buffer) == L7_SUCCESS) &&
            (SafeMakeOctetString(&timeRangeEntryData->timeRangeName,
                                 snmp_buffer, 
                                 strlen((char *)snmp_buffer)) == L7_TRUE))
        {
          rc = L7_SUCCESS;
          SET_VALID(I_timeRangeName, timeRangeEntryData->valid);
        }
        if (nominator != -1 ) break;
        /* else pass through */

      case I_timeRangeOperState:
        if (usmDbTimeRangeStatusGet(UnitIndex,
                                   timeRangeEntryData->timeRangeIndex,
                                   &status) == L7_SUCCESS)
        {
          rc = L7_SUCCESS;
          if (TIMERANGE_STATUS_ACTIVE == status)
          {
            timeRangeEntryData->timeRangeOperState = D_timeRangeOperState_active;
          }
          else if (TIMERANGE_STATUS_INACTIVE == status)
          {
            timeRangeEntryData->timeRangeOperState = D_timeRangeOperState_inactive;
          }
          SET_VALID(I_timeRangeOperState, timeRangeEntryData->valid);
        }
        if (nominator != -1 ) break;

      case I_timeRangeStatus:
        /* all entries are active */
        rc = L7_SUCCESS;
        timeRangeEntryData->timeRangeStatus = D_timeRangeStatus_active;
        SET_VALID(I_timeRangeStatus, timeRangeEntryData->valid);
        break;
        /* last valid nominator */

      default:
        rc = L7_FAILURE;
        break;
    }

    if (nominator >= 0 && rc == L7_SUCCESS)
      SET_VALID(nominator, timeRangeEntryData->valid);
    else if (nominator == -1)
      rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the next time range entry
*
* @param    UnitIndex          @b{(input)}        the unit for this operation
* @param    timeRangeEntryData @b{(input/output)} timeRangeEntry data
* @param    nominator          @b{(MIB element)}  the MIB element on which the get
*                                                 operation is performed.
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangeEntryGetNext(L7_uint32 UnitIndex, timeRangeEntry_t *timeRangeEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;

  while (usmDbTimeRangeIndexNextGet(UnitIndex, 
                                    timeRangeEntryData->timeRangeIndex, 
                                    &timeRangeEntryData->timeRangeIndex) == L7_SUCCESS)
  {
    if (snmpTimeRangeEntryGet(UnitIndex, timeRangeEntryData, nominator) == L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the absolute time entry given its timeRangeIndex and
*           timeRangeAbsoluteEntryIndex
*
* @param    UnitIndex          @b{(input)}                the unit for this operation
* @param    timeRangeAbsoluteEntryData @b{(input/output)} timeRangeAbsoluteEntry data
* @param    nominator          @b{(MIB element)}          the MIB element on which the get
*                                                         operation is performed.
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangeAbsoluteEntryGet(L7_uint32 UnitIndex, timeRangeAbsoluteEntry_t *timeRangeAbsoluteEntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  timeRangeEntryType_t entryType;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;

  if (usmDbTimeRangeIndexCheckValid(UnitIndex, timeRangeAbsoluteEntryData->timeRangeIndex) == L7_SUCCESS)
  {
    if ((usmDbTimeRangeEntryTypeGet(UnitIndex,
                        timeRangeAbsoluteEntryData->timeRangeIndex,
                        timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex,
                        &entryType) == L7_SUCCESS) && 
        (TIMERANGE_ABSOLUTE_ENTRY == entryType))
    {
      CLR_VALID(nominator, timeRangeAbsoluteEntryData->valid);

      /*
       * if ( nominator != -1 ) condition is added to all the case statemen
       * for storing all the values to support the undo functionality.
       */
      switch (nominator)
      {
        case -1:
        case I_timeRangeAbsoluteEntryIndex_timeRangeIndex:
        case I_timeRangeAbsoluteEntryIndex:
          rc = L7_SUCCESS;
          if( nominator != -1) break;
          /* else pass through */

        case I_timeRangeAbsoluteStartDateAndTime:
          if (usmDbTimeRangeIsFieldConfigured(UnitIndex,
                               timeRangeAbsoluteEntryData->timeRangeIndex,
                               timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex,
                               TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
          {
            rc = usmDbTimeRangeEntryStartDateAndTimeGet(UnitIndex,
                                timeRangeAbsoluteEntryData->timeRangeIndex,
                                timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex,
                                &entryType,
                                &startDateAndTime);

            if (rc == L7_SUCCESS)
            {
              if (SafeMakeTimeRangeAbsoluteDateAndTime(
                    &timeRangeAbsoluteEntryData->timeRangeAbsoluteStartDateAndTime,
                    startDateAndTime.date.absoluteDate.year,
                    startDateAndTime.date.absoluteDate.month,
                    startDateAndTime.date.absoluteDate.day,
                    startDateAndTime.hour,
                    startDateAndTime.minute) == L7_TRUE)
              {
                SET_VALID(I_timeRangeAbsoluteStartDateAndTime, timeRangeAbsoluteEntryData->valid);
              }
            }
          }
          if (nominator != -1 ) break;
          /* else pass through */
 
       case I_timeRangeAbsoluteEndDateAndTime:
          if (usmDbTimeRangeIsFieldConfigured(UnitIndex,
                               timeRangeAbsoluteEntryData->timeRangeIndex,
                               timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex,
                               TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
          {
            rc = usmDbTimeRangeEntryEndDateAndTimeGet(UnitIndex,
                                timeRangeAbsoluteEntryData->timeRangeIndex,
                                timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex,
                                &entryType,
                                &endDateAndTime);


            if (rc == L7_SUCCESS)
            {
              if (SafeMakeTimeRangeAbsoluteDateAndTime(
                    &timeRangeAbsoluteEntryData->timeRangeAbsoluteEndDateAndTime,
                    endDateAndTime.date.absoluteDate.year,
                    endDateAndTime.date.absoluteDate.month,
                    endDateAndTime.date.absoluteDate.day,
                    endDateAndTime.hour,
                    endDateAndTime.minute) == L7_TRUE)
              {
                SET_VALID(I_timeRangeAbsoluteEndDateAndTime, timeRangeAbsoluteEntryData->valid);
              }
            }
          }
          if (nominator != -1 ) break;
          /* else pass through */

        case I_timeRangeAbsoluteStatus:
          /* all entries are active */
          rc = L7_SUCCESS;
          timeRangeAbsoluteEntryData->timeRangeAbsoluteStatus = D_timeRangeAbsoluteStatus_active;
          SET_VALID(I_timeRangeAbsoluteStatus, timeRangeAbsoluteEntryData->valid);
          break;
          /* last valid nominator */

        default:
          rc = L7_FAILURE;
          break;
      }

      if (nominator >= 0 && rc == L7_SUCCESS)
        SET_VALID(nominator, timeRangeAbsoluteEntryData->valid);
      else if (nominator == -1)
        rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the next absolute time entry given its timeRangeIndex and
*           timeRangeAbsoluteEntryIndex
*
* @param    UnitIndex                  @b{(input)}        the unit for this operation
* @param    timeRangeAbsoluteEntryData @b{(input/output)} timeRangeAbsoluteEntry data
* @param    nominator                  @b{(MIB element)}  the MIB element on which the get
*                                                         operation is performed.
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangeAbsoluteEntryGetNext(L7_uint32 UnitIndex, timeRangeAbsoluteEntry_t *timeRangeAbsoluteEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  timeRangeEntryType_t entryType;

  while (1)
  {
    if (usmDbTimeRangeEntryGetNext(UnitIndex, 
          timeRangeAbsoluteEntryData->timeRangeIndex, 
          timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex, 
          &timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex) == L7_SUCCESS)
    {
      if ((usmDbTimeRangeEntryTypeGet(UnitIndex,
          timeRangeAbsoluteEntryData->timeRangeIndex,
          timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex,
          &entryType) == L7_SUCCESS) && 
        (TIMERANGE_ABSOLUTE_ENTRY == entryType))
      {
        if (snmpTimeRangeAbsoluteEntryGet(UnitIndex, timeRangeAbsoluteEntryData, nominator) == L7_SUCCESS)
        {
          rc = L7_SUCCESS;
          break;
        }
      }
    }
    else
    {
      /* no more rules in current entries, so move on to next list */
      if (usmDbTimeRangeIndexNextGet(UnitIndex, timeRangeAbsoluteEntryData->timeRangeIndex, 
                                     &timeRangeAbsoluteEntryData->timeRangeIndex) == L7_SUCCESS)
        timeRangeAbsoluteEntryData->timeRangeAbsoluteEntryIndex = 0;
      else
        break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the periodic time entry given its timeRangeIndex and
*           timeRangePeriodicEntryIndex
*
* @param    UnitIndex                  @b{(input)}        the unit for this operation
* @param    timeRangePeriodicEntryData @b{(input/output)} timeRangePeriodicEntry data
* @param    nominator                  @b{(MIB element)}  the MIB element on which the get
*                                                         operation is performed.
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/

L7_RC_t
snmpTimeRangePeriodicEntryGet(L7_uint32 UnitIndex, timeRangePeriodicEntry_t *timeRangePeriodicEntryData, L7_int32 nominator)
{
  L7_RC_t   rc = L7_FAILURE;
  timeRangeEntryType_t entryType;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  L7_uchar8 i, dayMask;

  if (usmDbTimeRangeIndexCheckValid(UnitIndex, timeRangePeriodicEntryData->timeRangeIndex) == L7_SUCCESS)
  {
    if ((usmDbTimeRangeEntryTypeGet(UnitIndex,
                        timeRangePeriodicEntryData->timeRangeIndex,
                        timeRangePeriodicEntryData->timeRangePeriodicEntryIndex,
                        &entryType) == L7_SUCCESS) && 
        (TIMERANGE_PERIODIC_ENTRY == entryType))
    {
      CLR_VALID(nominator, timeRangePeriodicEntryData->valid);

      /*
       * if ( nominator != -1 ) condition is added to all the case statemen
       * for storing all the values to support the undo functionality.
       */
      switch (nominator)
      {
        case -1:
        case I_timeRangePeriodicEntryIndex_timeRangeIndex:
        case I_timeRangePeriodicEntryIndex:
          rc = L7_SUCCESS;
          if( nominator != -1) break;
          /* else pass through */
        case I_timeRangePeriodicStartDay:
        case I_timeRangePeriodicStartTime:
          if (usmDbTimeRangeIsFieldConfigured(UnitIndex,
                               timeRangePeriodicEntryData->timeRangeIndex,
                               timeRangePeriodicEntryData->timeRangePeriodicEntryIndex,
                               TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
          {
            rc = usmDbTimeRangeEntryStartDateAndTimeGet(UnitIndex,
                                timeRangePeriodicEntryData->timeRangeIndex,
                                timeRangePeriodicEntryData->timeRangePeriodicEntryIndex,
                                &entryType,
                                &startDateAndTime);

            if (rc == L7_SUCCESS && nominator == I_timeRangePeriodicStartDay)
            {
              dayMask = 0;
              for (i = 1; i<=7 ; i++)
              {
                if (startDateAndTime.date.daysOfTheWeek.dayMask & (1 << i))
                {
                  dayMask |= (0x80 >> i);
                }
              }
              if(L7_TRUE == SafeMakeOctetString(
                    &timeRangePeriodicEntryData->timeRangePeriodicStartDay,
                    &(dayMask),
                    1))
              {
                SET_VALID(I_timeRangePeriodicStartDay, timeRangePeriodicEntryData->valid);
              }
            }
            if (rc == L7_SUCCESS && nominator == I_timeRangePeriodicStartTime)
            {
              if (SafeMakeTimeRangePeriodicTime(
                       &timeRangePeriodicEntryData->timeRangePeriodicStartTime,
                       startDateAndTime.hour,
                       startDateAndTime.minute) == L7_TRUE)
              {
                SET_VALID(I_timeRangePeriodicStartTime, timeRangePeriodicEntryData->valid);
              }
            } 
          }
          if (nominator != -1 ) break;
          /* else pass through */
 
        case I_timeRangePeriodicEndDay:
        case I_timeRangePeriodicEndTime:
          if (usmDbTimeRangeIsFieldConfigured(UnitIndex,
                               timeRangePeriodicEntryData->timeRangeIndex,
                               timeRangePeriodicEntryData->timeRangePeriodicEntryIndex,
                               TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
          {
            rc = usmDbTimeRangeEntryEndDateAndTimeGet(UnitIndex,
                                timeRangePeriodicEntryData->timeRangeIndex,
                                timeRangePeriodicEntryData->timeRangePeriodicEntryIndex,
                                &entryType,
                                &endDateAndTime);

            if (rc == L7_SUCCESS && nominator == I_timeRangePeriodicEndDay)
            {
              dayMask = 0;
              for (i = 1; i<=7 ; i++)
              {
                if (endDateAndTime.date.daysOfTheWeek.dayMask & (1 << i))
                {
                  dayMask |= (0x80 >> i);
                }
              }

              if(L7_TRUE == SafeMakeOctetString(
                    &timeRangePeriodicEntryData->timeRangePeriodicEndDay,
                    &(dayMask),
                    1))
              {
                SET_VALID(I_timeRangePeriodicEndDay, timeRangePeriodicEntryData->valid);
              }
            }
            if (rc == L7_SUCCESS && nominator == I_timeRangePeriodicEndTime)
            {
               if (SafeMakeTimeRangePeriodicTime(
                       &timeRangePeriodicEntryData->timeRangePeriodicEndTime,
                       endDateAndTime.hour,
                       endDateAndTime.minute) == L7_TRUE)
              {
                SET_VALID(I_timeRangePeriodicEndTime, timeRangePeriodicEntryData->valid);
              }
            }
          }
          if (nominator != -1 ) break;
          /* else pass through */

        case I_timeRangePeriodicStatus:
          /* all entries are active */
          rc = L7_SUCCESS;
          timeRangePeriodicEntryData->timeRangePeriodicStatus = D_timeRangePeriodicStatus_active;
          SET_VALID(I_timeRangePeriodicStatus, timeRangePeriodicEntryData->valid);
          break;
          /* last valid nominator */

        default:
          rc = L7_FAILURE;
          break;
      }

      if (nominator >= 0 && rc == L7_SUCCESS)
        SET_VALID(nominator, timeRangePeriodicEntryData->valid);
      else if (nominator == -1)
        rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the next periodic time entry given its timeRangeIndex and
*           timeRangePeriodicEntryIndex
*
* @param    UnitIndex                  @b{(input)}        the unit for this operation
* @param    timeRangePeriodicEntryData @b{(input/output)} timeRangePeriodicEntry data
* @param    nominator                  @b{(MIB element)}  the MIB element on which the get
*                                                         operation is performed.
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangePeriodicEntryGetNext(L7_uint32 UnitIndex, timeRangePeriodicEntry_t *timeRangePeriodicEntryData, L7_int32 nominator)
{
  L7_RC_t rc = L7_FAILURE;
  timeRangeEntryType_t entryType;

  while (1)
  {
    if (usmDbTimeRangeEntryGetNext(UnitIndex, 
          timeRangePeriodicEntryData->timeRangeIndex, 
          timeRangePeriodicEntryData->timeRangePeriodicEntryIndex, 
          &timeRangePeriodicEntryData->timeRangePeriodicEntryIndex) == L7_SUCCESS)
    {
      if ((usmDbTimeRangeEntryTypeGet(UnitIndex,
          timeRangePeriodicEntryData->timeRangeIndex,
          timeRangePeriodicEntryData->timeRangePeriodicEntryIndex,
          &entryType) == L7_SUCCESS) && 
        (TIMERANGE_PERIODIC_ENTRY == entryType))
      {
        if (snmpTimeRangePeriodicEntryGet(UnitIndex, timeRangePeriodicEntryData, nominator) == L7_SUCCESS)
        {
          rc = L7_SUCCESS;
          break;
        }
      }
    }
    else
    {
      /* no more rules in current entries, so move on to next list */
      if (usmDbTimeRangeIndexNextGet(UnitIndex, timeRangePeriodicEntryData->timeRangeIndex, 
                                     &timeRangePeriodicEntryData->timeRangeIndex) == L7_SUCCESS)
        timeRangePeriodicEntryData->timeRangePeriodicEntryIndex = 0;
      else
        break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Add a time entry into a time range given its timeRangeIndex and
*           timeRangeEntryIndex and entryType.
*
* @param    UnitIndex           @b{(input)} the unit for this operation
* @param    timeRangeIndex      @b{(input)} timeRangeIndex
* @param    timeRangeEntryIndex @b{(input)} time range entry index
* @param    entryType           @b{(input)} time range entry type
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangeEntryAdd(L7_uint32 UnitIndex,
                      L7_uint32 timeRangeIndex,
                      L7_uint32 timeRangeEntryIndex,
                      timeRangeEntryType_t entryType)
{
  L7_RC_t rc = L7_SUCCESS;
  
  rc = usmDbTimeRangeEntryAdd(UnitIndex, timeRangeIndex,
                                  timeRangeEntryIndex, entryType);
  return rc;
}

/*********************************************************************
* @purpose  Set the rowstatus of a time range given its timeRangeIndex
*
* @param    UnitIndex           @b{(input)} the unit for this operation
* @param    timeRangeIndex      @b{(input)} timeRangeIndex
* @param    val                 @b{(input)} RowStatus Value
*                                           D_timeRangeStatus_active or 
*                                           D_timeRangeStatus_destroy
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangeStatusSet(L7_uint32 UnitIndex, L7_uint32 timeRangeIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_timeRangeStatus_active:
    break;

  case D_timeRangeStatus_destroy:
    rc = usmDbTimeRangeDelete(UnitIndex, timeRangeIndex);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the rowstatus of a an absolute  or periodic 
*           time range entry given its timeRangeIndex
*           and timeRangeEntryIndex
*
* @param    UnitIndex           @b{(input)} the unit for this operation
* @param    timeRangeIndex      @b{(input)} timeRangeIndex
* @param    timeRangeEntryIndex @b{(input)} timeRangeEntryIndex
* @param    val                 @b{(input)} RowStatus Value
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
snmpTimeRangeEntryStatusSet(L7_uint32 UnitIndex, L7_uint32 timeRangeIndex, L7_uint32 timeRangeEntryIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_timeRangePeriodicStatus_active:
/*  case D_timeRangeAbsoluteStatus_active: */
    break;

  case D_timeRangePeriodicStatus_destroy:
/*  case D_timeRangeAbsoluteStatus_destroy: */
    rc = usmDbTimeRangeEntryRemove(UnitIndex, timeRangeIndex, timeRangeEntryIndex); 
    break;

  default:
    rc = L7_FAILURE;
    break;
  }

  return rc;
}

OctetString *MakeTimeRangeAbsoluteDateAndTime_track(L7_uint32 year,
                                                    L7_uchar8 month,
                                                    L7_uchar8 day,
                                                    L7_uchar8 hour,
                                                    L7_uchar8 minute,
                                                    L7_char8 *file, 
                                                    L7_uint32 line)
{
  L7_uchar8 buff[6];
  buff[0] = (L7_char8)(year >> 8);
  buff[1] = (L7_char8)year;
  buff[2] = (L7_char8)month;
  buff[3] = (L7_char8)day;
  buff[4] = (L7_char8)hour;
  buff[5] = (L7_char8)minute;
  return MakeOctetString_track(buff, 6, file, line);
}

L7_BOOL
SafeMakeTimeRangeAbsoluteDateAndTime_track(OctetString **os_ptr,
                                          L7_uint32 year,
                                          L7_uchar8 month,
                                          L7_uchar8 day,
                                          L7_uchar8 hour,
                                          L7_uchar8 minute,
                                          L7_char8 *file, 
                                          L7_uint32 line)
{ 
  OctetString *temp_os_ptr;

  temp_os_ptr = MakeTimeRangeAbsoluteDateAndTime_track(year,
                                          month,
                                          day,
                                          hour,
                                          minute,
                                          file, 
                                          line);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }
  return L7_FALSE;
}

OctetString *MakeTimeRangePeriodicTime_track(L7_uchar8 hour,
                                      L7_uchar8 minute,
                                      L7_char8 *file, 
                                      L7_uint32 line)
{
  L7_uchar8 buff[2];
    
  buff[0] = (L7_char8)hour;
  buff[1] = (L7_char8)minute;
  return MakeOctetString_track(buff, 2, file, line);
}


L7_BOOL
SafeMakeTimeRangePeriodicTime_track(OctetString **os_ptr,
                                      L7_uchar8 hour,
                                      L7_uchar8 minute,
                                      L7_char8 *file, 
                                      L7_uint32 line)
{ 
  OctetString *temp_os_ptr;

  temp_os_ptr = MakeTimeRangePeriodicTime_track(hour, minute, file, line);

  if (temp_os_ptr != NULL)
  {
    FreeOctetString(*os_ptr);
    *os_ptr = temp_os_ptr;
    return L7_TRUE;
  }
  return L7_FALSE;
}
