
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
********************************************************************************
*
* @filename fpobj_baseTimeRange.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  29 December 2009, Tuesday
*
  * @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseTimeRange_obj.h"
#include "timerange_exports.h"
#include "timezone_exports.h"
#include "usmdb_timerange_api.h"

#define L7_MAX_TIME_RANGE_ENTRIES          10

extern L7_uint32 daysInAMonth[];
void MaskToDaysOfWeek1( L7_uchar8 daysOfTheWeek, L7_uchar8 *buf)
{
  if ( daysOfTheWeek & (1 << L7_DAY_SUN ))
  {
    strcat(buf,"SUN");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_MON ))
  {
    strcat(buf,",MON");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_TUE ))
  {
    strcat(buf,",TUE");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_WED ))
  {
    strcat(buf,",WED");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_THU ))
  {
    strcat(buf,",THU");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_FRI ))
  {
    strcat(buf,",FRI");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_SAT ))
  {
    strcat(buf,",SAT");
  }

}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeIndex
*
* @purpose Get 'timeRangeIndex'
 *@description  [timeRangeIndex] The Time Range table index, this instance is
* associated with, When creating a new Time Range. Refer to the
* timeRangeIndexNextFree object to determine the next available
* timeRangeIndex to use.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeIndexValue;
  xLibU32_t nextObjtimeRangeIndexValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(objtimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
              (xLibU8_t *) &objtimeRangeIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
    objtimeRangeIndexValue =  0;
    nextObjtimeRangeIndexValue = 0;
    owa.l7rc = usmDbTimeRangeIndexFirstGet(L7_UNIT_CURRENT, 
                  &nextObjtimeRangeIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, &objtimeRangeIndexValue, owa.len);
    owa.l7rc = usmDbTimeRangeIndexNextGet(L7_UNIT_CURRENT,  objtimeRangeIndexValue,
                  &nextObjtimeRangeIndexValue);
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY(bufp, &nextObjtimeRangeIndexValue, owa.len);

  /* return the object value: timeRangeIndex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&nextObjtimeRangeIndexValue,
                      sizeof(nextObjtimeRangeIndexValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeName
*
* @purpose Get 'timeRangeName'
 *@description  [timeRangeName] The name of this Time Range entry, which must
* consist of1 to 31 alphanumeric characters and uniquely identify
* this Time Range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objtimeRangeNameValue;
  xLibU32_t keytimeRangeIndexValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* get the value from application */
  owa.l7rc = usmDbTimeRangeIndexCheckValid(L7_UNIT_CURRENT, 
                       keytimeRangeIndexValue);
  if (owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbTimeRangeNameGet(L7_UNIT_CURRENT,
                                   keytimeRangeIndexValue,
                                   objtimeRangeNameValue);
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, objtimeRangeNameValue, strlen(objtimeRangeNameValue));

  /* return the object value: timeRangeName */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)objtimeRangeNameValue,
                      strlen(objtimeRangeNameValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeName
*
* @purpose Set 'timeRangeName'
 *@description  [timeRangeName] The name of this Time Range entry, which must
* consist of1 to 31 alphanumeric characters and uniquely identify
* this Time Range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeName (void *wap, void *bufp)
{
  /*This object is set in timeRangeRowStatus*/
  return XLIBRC_SUCCESS;

}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeStatus
*
* @purpose Get 'timeRangeStatus'
 *@description  [timeRangeStatus] Status of this instance.It could be either
* active, createAndGo or destroy   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeStatusValue;
  xLibU32_t keytimeRangeIndexValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* get the value from application */
  owa.l7rc = usmDbTimeRangeStatusGet(L7_UNIT_CURRENT,
                              keytimeRangeIndexValue,
                              &objtimeRangeStatusValue);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeStatusValue, sizeof(objtimeRangeStatusValue));

  /* return the object value: timeRangeStatus */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeStatusValue,
                      sizeof(objtimeRangeStatusValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeRowStatus
*
* @purpose Set 'timeRangeRowStatus'
 *@description  [timeRangeRowStatus] Status of this instance.It could be either
* active, createAndGo or destroy   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeRowStatusValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibStr256_t timeRangeName;
  xLibU16_t  timeRangeNameSize = sizeof(timeRangeName);


  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: timeRangeRowStatus */
  owa.len = sizeof(objtimeRangeRowStatusValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objtimeRangeRowStatusValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeRowStatusValue, owa.len);


  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if((owa.rc != XLIBRC_SUCCESS) &&
     (objtimeRangeRowStatusValue != L7_ROW_STATUS_CREATE_AND_GO))
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* get the value of timeRangeName from the filters  */
  memset(timeRangeName , 0,timeRangeNameSize); 
  owa.rc = xLibFilterGet (wap, XOBJ_baseTimeRange_timeRangeName,
                           (xLibU8_t *)  timeRangeName,
                           &timeRangeNameSize);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  /* call the usmdb only for add and delete */
  if (objtimeRangeRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */         

    /* Check if the name does not exists already */
    owa.l7rc = usmDbTimeRangeNameToIndex(L7_UNIT_CURRENT, timeRangeName,&keytimeRangeIndexValue);
    if(owa.l7rc == L7_SUCCESS)   /*Name already exists*/
    {    
        owa.rc = XLIBRC_TIMERANGE_NAME_ALREADY_EXISTS;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }  
    /* Get the next Valid Index */
    owa.l7rc = usmdbTimeRangeNextAvailableIndexGet(L7_UNIT_CURRENT, &keytimeRangeIndexValue);   
    if(owa.l7rc == L7_TABLE_IS_FULL)
    {
      owa.rc = XLIBRC_TIMERANGE_TABLE_FULL_ERROR;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else if(owa.l7rc == L7_FAILURE)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }    
    owa.l7rc = usmDbTimeRangeCreate(L7_UNIT_CURRENT, keytimeRangeIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      if(owa.l7rc == L7_TABLE_IS_FULL)
      {
        owa.rc = XLIBRC_TIMERANGE_TABLE_FULL_ERROR;
      }
      else
      { 
        owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      } 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc = usmDbTimeRangeNameAdd(L7_UNIT_CURRENT, keytimeRangeIndexValue,timeRangeName);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      usmDbTimeRangeDelete(L7_UNIT_CURRENT, keytimeRangeIndexValue);
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    /* Creation of index successful Push the index into the filter*/
    /* Pass type as 0 */
    owa.rc = xLibFilterSet(wap,XOBJ_baseTimeRange_timeRangeIndex,0,
                           (xLibU8_t *) &keytimeRangeIndexValue,
                           sizeof(keytimeRangeIndexValue)); 
    if(owa.rc !=XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if(objtimeRangeRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbTimeRangeDelete(L7_UNIT_CURRENT, keytimeRangeIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeTableSizeAndMax
*
* @purpose Get 'timeRangeTableSizeAndMax'
 *@description  [timeRangeTableSizeAndMax]  Current Size/Max Size of the
* TimeRange Table   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeTableSizeAndMax (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibStr256_t objtimeRangeTableSizeAndMaxValue;
  xLibU32_t tableSize, tableMax;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbTimeRangeCurrNumGet(L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  owa.l7rc = usmDbTimeRangeMaxNumGet(L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  /* format the string */
  osapiSnprintf(objtimeRangeTableSizeAndMaxValue, sizeof(objtimeRangeTableSizeAndMaxValue),
                "%u / %u", tableSize, tableMax);

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  /* return the object value: timeRangeTableSizeAndMax */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeTableSizeAndMaxValue,
                      sizeof(objtimeRangeTableSizeAndMaxValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeEntryIndex
*
* @purpose Get 'timeRangeEntryIndex'
 *@description  [timeRangeEntryIndex] The index of this entry instance within an
* Time Range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeEntryIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeEntryIndexValue;
  xLibU32_t nextObjtimeRangeEntryIndexValue;
  xLibU32_t keytimeRangeIndexValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(objtimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
              (xLibU8_t *) &objtimeRangeEntryIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
    owa.l7rc = usmDbTimeRangeEntryGetFirst(L7_UNIT_CURRENT,
                                           keytimeRangeIndexValue,
                                           &nextObjtimeRangeEntryIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, &objtimeRangeEntryIndexValue, owa.len);
    owa.l7rc = usmDbTimeRangeEntryGetNext(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    objtimeRangeEntryIndexValue,
                                    &nextObjtimeRangeEntryIndexValue);
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY(bufp, &nextObjtimeRangeEntryIndexValue, owa.len);

  /* return the object value: timeRangeEntryIndex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&nextObjtimeRangeEntryIndexValue,
                      sizeof(nextObjtimeRangeEntryIndexValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeEntryType
*
* @purpose Get 'timeRangeEntryType'
 *@description  [timeRangeEntryType] Type of the timerange entry.It could be
* either absolute or periodic   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeEntryType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeEntryTypeValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryType_t entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

  /* get the value from application */
  owa.l7rc = usmDbTimeRangeEntryTypeGet(L7_UNIT_CURRENT,
                                        keytimeRangeIndexValue,
                                        keytimeRangeEntryIndexValue,
                                        &entryType);
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }
  objtimeRangeEntryTypeValue = (xLibU32_t )entryType;
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeEntryTypeValue, sizeof(objtimeRangeEntryTypeValue));

  /* return the object value: timeRangeEntryType */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeEntryTypeValue,
                      sizeof(objtimeRangeEntryTypeValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeEntryType
*
* @purpose Set 'timeRangeEntryType'
 *@description  [timeRangeEntryType] Type of the timerange entry.It could be
* either absolute or periodic   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeEntryType (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteStartMonth
*
* @purpose Get 'timeRangeAbsoluteStartMonth'
 *@description  [timeRangeAbsoluteStartMonth] Start month for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteStartMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartMonthValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &startDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteStartMonthValue = startDateAndTime.date.absoluteDate.month;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartMonthValue, sizeof(objtimeRangeAbsoluteStartMonthValue));

  /* return the object value: timeRangeAbsoluteStartMonth */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartMonthValue,
                      sizeof(objtimeRangeAbsoluteStartMonthValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteStartMonth
*
* @purpose Set 'timeRangeAbsoluteStartMonth'
 *@description  [timeRangeAbsoluteStartMonth] Start month for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteStartMonth (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteStartDate
*
* @purpose Get 'timeRangeAbsoluteStartDate'
 *@description  [timeRangeAbsoluteStartDate] Start date for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteStartDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartDateValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &startDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteStartDateValue = startDateAndTime.date.absoluteDate.day;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartDateValue, sizeof(objtimeRangeAbsoluteStartDateValue));

  /* return the object value: timeRangeAbsoluteStartDate */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartDateValue,
                      sizeof(objtimeRangeAbsoluteStartDateValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;


}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteStartDate
*
* @purpose List 'timeRangeAbsoluteStartDate'
 *@description  [timeRangeAbsoluteStartDate] Start date for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteStartDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartDateValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteStartDateValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartDate,
              (xLibU8_t *) &objtimeRangeAbsoluteStartDateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteStartDateValue = 1;
  }
  else 
  {
    objtimeRangeAbsoluteStartDateValue++;
  }
  if (objtimeRangeAbsoluteStartDateValue > 31)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartDateValue, owa.len);

  /* return the object value: timeRangeAbsoluteStartDate */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartDateValue,
                       sizeof(objtimeRangeAbsoluteStartDateValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteStartDate
*
* @purpose Set 'timeRangeAbsoluteStartDate'
 *@description  [timeRangeAbsoluteStartDate] Start date for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteStartDate (void *wap, void *bufp)
{

  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;

}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteStartYear
*
* @purpose Get 'timeRangeAbsoluteStartYear'
 *@description  [timeRangeAbsoluteStartYear] Start yearfor the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteStartYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartYearValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &startDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteStartYearValue = startDateAndTime.date.absoluteDate.year;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartYearValue, sizeof(objtimeRangeAbsoluteStartYearValue));

  /* return the object value: timeRangeAbsoluteStartYear */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartYearValue,
                      sizeof(objtimeRangeAbsoluteStartYearValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteStartYear
*
* @purpose List 'timeRangeAbsoluteStartYear'
 *@description  [timeRangeAbsoluteStartYear] Start yearfor the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteStartYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartYearValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteStartYearValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartYear,
              (xLibU8_t *) &objtimeRangeAbsoluteStartYearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteStartYearValue = L7_TIMERANGE_MIN_YEAR;
  }
  else 
  {
    objtimeRangeAbsoluteStartYearValue++;
  }
  if (objtimeRangeAbsoluteStartYearValue > L7_TIMERANGE_MAX_YEAR)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartYearValue, owa.len);

  /* return the object value: timeRangeAbsoluteStartYear */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartYearValue,
                       sizeof(objtimeRangeAbsoluteStartYearValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteStartYear
*
* @purpose Set 'timeRangeAbsoluteStartYear'
 *@description  [timeRangeAbsoluteStartYear] Start yearfor the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteStartYear (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteEndMonth
*
* @purpose Get 'timeRangeAbsoluteEndMonth'
 *@description  [timeRangeAbsoluteEndMonth] End month for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteEndMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndMonthValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &endDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteEndMonthValue = endDateAndTime.date.absoluteDate.month;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndMonthValue, sizeof(objtimeRangeAbsoluteEndMonthValue));

  /* return the object value: timeRangeAbsoluteEndMonth */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndMonthValue,
                      sizeof(objtimeRangeAbsoluteEndMonthValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteEndMonth
*
* @purpose Set 'timeRangeAbsoluteEndMonth'
 *@description  [timeRangeAbsoluteEndMonth] End month for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteEndMonth (void *wap, void *bufp)
{

  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;

}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteEndDate
*
* @purpose Get 'timeRangeAbsoluteEndDate'
 *@description  [timeRangeAbsoluteEndDate] End date for the absolute time range
* entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteEndDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndDateValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &endDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteEndDateValue = endDateAndTime.date.absoluteDate.day;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndDateValue, sizeof(objtimeRangeAbsoluteEndDateValue));

  /* return the object value: timeRangeAbsoluteEndDate */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndDateValue,
                      sizeof(objtimeRangeAbsoluteEndDateValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteEndDate
*
* @purpose List 'timeRangeAbsoluteEndDate'
 *@description  [timeRangeAbsoluteEndDate] End date for the absolute time range
* entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteEndDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndDateValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteEndDateValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndDate,
              (xLibU8_t *) &objtimeRangeAbsoluteEndDateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteEndDateValue = 1;
  }
  else 
  {
    objtimeRangeAbsoluteEndDateValue++;
  }
  if (objtimeRangeAbsoluteEndDateValue > 31)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndDateValue, owa.len);

  /* return the object value: timeRangeAbsoluteEndDate */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndDateValue,
                       sizeof(objtimeRangeAbsoluteEndDateValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteEndDate
*
* @purpose Set 'timeRangeAbsoluteEndDate'
 *@description  [timeRangeAbsoluteEndDate] End date for the absolute time range
* entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteEndDate (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteEndYear
*
* @purpose Get 'timeRangeAbsoluteEndYear'
 *@description  [timeRangeAbsoluteEndYear] End year for the absolute time range
* entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteEndYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndYearValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &endDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteEndYearValue = endDateAndTime.date.absoluteDate.year;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndYearValue, sizeof(objtimeRangeAbsoluteEndYearValue));

  /* return the object value: timeRangeAbsoluteEndYear */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndYearValue,
                      sizeof(objtimeRangeAbsoluteEndYearValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteEndYear
*
* @purpose List 'timeRangeAbsoluteEndYear'
 *@description  [timeRangeAbsoluteEndYear] End year for the absolute time range
* entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteEndYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndYearValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteEndYearValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndYear,
              (xLibU8_t *) &objtimeRangeAbsoluteEndYearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteEndYearValue = L7_TIMERANGE_MIN_YEAR;
  }
  else 
  {
    objtimeRangeAbsoluteEndYearValue++;
  }
  if (objtimeRangeAbsoluteEndYearValue > L7_TIMERANGE_MAX_YEAR)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndYearValue, owa.len);

  /* return the object value: timeRangeAbsoluteEndYear */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndYearValue,
                       sizeof(objtimeRangeAbsoluteEndYearValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteEndYear
*
* @purpose Set 'timeRangeAbsoluteEndYear'
 *@description  [timeRangeAbsoluteEndYear] End year for the absolute time range
* entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteEndYear (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteStartTimeHours
*
* @purpose Get 'timeRangeAbsoluteStartTimeHours'
 *@description  [timeRangeAbsoluteStartTimeHours] Start hour for the absolute
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartTimeHourValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &startDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteStartTimeHourValue = startDateAndTime.hour;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartTimeHourValue, sizeof(objtimeRangeAbsoluteStartTimeHourValue));

  /* return the object value: timeRangeAbsoluteStartTimeHour */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartTimeHourValue,
                      sizeof(objtimeRangeAbsoluteStartTimeHourValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteStartTimeHours
*
* @purpose List 'timeRangeAbsoluteStartTimeHours'
 *@description  [timeRangeAbsoluteStartTimeHours] Start hour for the absolute
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartTimeHoursValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteStartTimeHoursValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartTimeHours,
              (xLibU8_t *) &objtimeRangeAbsoluteStartTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteStartTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else 
  {
    objtimeRangeAbsoluteStartTimeHoursValue++;
  }
  if (objtimeRangeAbsoluteStartTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartTimeHoursValue, owa.len);

  /* return the object value: timeRangeAbsoluteStartTimeHours */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartTimeHoursValue,
                       sizeof(objtimeRangeAbsoluteStartTimeHoursValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteStartTimeHours
*
* @purpose Set 'timeRangeAbsoluteStartTimeHours'
 *@description  [timeRangeAbsoluteStartTimeHours] Start hour for the absolute
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteStartTimeHours (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteStartTimeMinutes
*
* @purpose Get 'timeRangeAbsoluteStartTimeMinutes'
 *@description  [timeRangeAbsoluteStartTimeMinutes] Start minute for the
* absolute time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartTimeMinutesValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &startDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteStartTimeMinutesValue = startDateAndTime.minute;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartTimeMinutesValue, sizeof(objtimeRangeAbsoluteStartTimeMinutesValue));

  /* return the object value: timeRangeAbsoluteStartTimeMinute */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartTimeMinutesValue,
                      sizeof(objtimeRangeAbsoluteStartTimeMinutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteStartTimeMinutes
*
* @purpose List 'timeRangeAbsoluteStartTimeMinutes'
 *@description  [timeRangeAbsoluteStartTimeMinutes] Start minute for the
* absolute time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteStartTimeMinutesValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartTimeMinutes,
              (xLibU8_t *) &objtimeRangeAbsoluteStartTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteStartTimeMinutesValue = L7_TIMEZONE_MINS_MIN;
  }
  else 
  {
    objtimeRangeAbsoluteStartTimeMinutesValue++;
  }
  if (objtimeRangeAbsoluteStartTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartTimeMinutesValue, owa.len);

  /* return the object value: timeRangeAbsoluteStartTimeMinutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartTimeMinutesValue,
                       sizeof(objtimeRangeAbsoluteStartTimeMinutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteStartTimeMinutes
*
* @purpose Set 'timeRangeAbsoluteStartTimeMinutes'
 *@description  [timeRangeAbsoluteStartTimeMinutes] Start minute for the
* absolute time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteStartTimeMinutes (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteEndTimeHours
*
* @purpose Get 'timeRangeAbsoluteEndTimeHours'
 *@description  [timeRangeAbsoluteEndTimeHours] End hour for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndTimeHourValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &endDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteEndTimeHourValue = endDateAndTime.hour;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndTimeHourValue, sizeof(objtimeRangeAbsoluteEndTimeHourValue));

  /* return the object value: timeRangeAbsoluteEndTimeHour */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndTimeHourValue,
                      sizeof(objtimeRangeAbsoluteEndTimeHourValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteEndTimeHours
*
* @purpose List 'timeRangeAbsoluteEndTimeHours'
 *@description  [timeRangeAbsoluteEndTimeHours] End hour for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndTimeHoursValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteEndTimeHoursValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndTimeHours,
              (xLibU8_t *) &objtimeRangeAbsoluteEndTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteEndTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else 
  {
    objtimeRangeAbsoluteEndTimeHoursValue++;
  }
  if (objtimeRangeAbsoluteEndTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndTimeHoursValue, owa.len);

  /* return the object value: timeRangeAbsoluteEndTimeHours */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndTimeHoursValue,
                       sizeof(objtimeRangeAbsoluteEndTimeHoursValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteEndTimeHours
*
* @purpose Set 'timeRangeAbsoluteEndTimeHours'
 *@description  [timeRangeAbsoluteEndTimeHours] End hour for the absolute time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteEndTimeHours (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteEndTimeMinutes
*
* @purpose Get 'timeRangeAbsoluteEndTimeMinutes'
 *@description  [timeRangeAbsoluteEndTimeMinutes] End Minute for the absolute
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndTimeMinutesValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  L7_uint32 absEntryNum;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.l7rc == L7_SUCCESS)
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                        keytimeRangeIndexValue,
                        absEntryNum,
                        TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
    {
      owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    absEntryNum,
                                    &entryType,
                                    &endDateAndTime);
      if (owa.l7rc == L7_SUCCESS)
      {
        objtimeRangeAbsoluteEndTimeMinutesValue = endDateAndTime.minute;
      }
    }
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndTimeMinutesValue, sizeof(objtimeRangeAbsoluteEndTimeMinutesValue));

  /* return the object value: timeRangeAbsoluteEndTimeMinute */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndTimeMinutesValue,
                      sizeof(objtimeRangeAbsoluteEndTimeMinutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangeAbsoluteEndTimeMinutes
*
* @purpose List 'timeRangeAbsoluteEndTimeMinutes'
 *@description  [timeRangeAbsoluteEndTimeMinutes] End Minute for the absolute
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangeAbsoluteEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangeAbsoluteEndTimeMinutesValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndTimeMinutes,
              (xLibU8_t *) &objtimeRangeAbsoluteEndTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangeAbsoluteEndTimeMinutesValue = L7_TIMEZONE_MINS_MIN;
  }
  else 
  {
    objtimeRangeAbsoluteEndTimeMinutesValue++;
  }
  if (objtimeRangeAbsoluteEndTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndTimeMinutesValue, owa.len);

  /* return the object value: timeRangeAbsoluteEndTimeMinutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndTimeMinutesValue,
                       sizeof(objtimeRangeAbsoluteEndTimeMinutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteEndTimeMinutes
*
* @purpose Set 'timeRangeAbsoluteEndTimeMinutes'
 *@description  [timeRangeAbsoluteEndTimeMinutes] End Minute for the absolute
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteEndTimeMinutes (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeEntryStatus
*
* @purpose Set 'timeRangeEntryStatus'
 *@description  [timeRangeEntryStatus] Status of this instance.It could be
* either active, createAndGo or destroy   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeEntryStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeEntryStatusValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;

  xLibU32_t objtimeRangeEntryTypeValue;
  xLibU32_t objtimeRangeAbsoluteStartMonthValue;
  xLibU32_t objtimeRangeAbsoluteStartDateValue;
  xLibU32_t objtimeRangeAbsoluteStartYearValue;
  xLibU32_t objtimeRangeAbsoluteStartTimeHoursValue;
  xLibU32_t objtimeRangeAbsoluteStartTimeMinutesValue;

  xLibU32_t objtimeRangeAbsoluteEndMonthValue;
  xLibU32_t objtimeRangeAbsoluteEndDateValue;
  xLibU32_t objtimeRangeAbsoluteEndYearValue;
  xLibU32_t objtimeRangeAbsoluteEndTimeHoursValue;
  xLibU32_t objtimeRangeAbsoluteEndTimeMinutesValue;

  xLibU32_t objtimeRangePeriodicStartDayValue;
  xLibU32_t objtimeRangePeriodicStartTimeHoursValue;
  xLibU32_t objtimeRangePeriodicStartTimeMinutesValue;

  xLibU32_t objtimeRangePeriodicEndDayValue;
  xLibU32_t objtimeRangePeriodicEndTimeHoursValue;
  xLibU32_t objtimeRangePeriodicEndTimeMinutesValue;

  xLibU32_t objtimeRangeAbsoluteStartDateAndTimeValue;
  xLibU32_t objtimeRangeAbsoluteEndDateAndTimeValue;
  
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  L7_BOOL removeEntry = L7_TRUE;
  
  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: timeRangeEntryStatusValue*/
  owa.len = sizeof(objtimeRangeEntryStatusValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryStatus,
                              (xLibU8_t *) &objtimeRangeEntryStatusValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeEntryStatusValue, owa.len);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

  owa.l7rc = L7_SUCCESS;
  if(objtimeRangeEntryStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
   
    owa.l7rc = usmDbTimeRangeEntryNumRangeCheck(L7_UNIT_CURRENT,
                                         keytimeRangeIndexValue,
                                         keytimeRangeEntryIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_TIMERANGE_ENTRY_ID_OUT_OF_RANGE;
      FPOBJ_TRACE_EXIT(bufp, owa);
      return owa.rc;
    }
   /* retrieve object: timeRangeEntryTypeValue*/
    owa.len = sizeof(objtimeRangeEntryTypeValue);
    owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryType,
                                (xLibU8_t *) &objtimeRangeEntryTypeValue, 
                                &owa.len);
    if(owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING; 
      FPOBJ_TRACE_EXIT(bufp, owa);
      return owa.rc;
    }

    if (objtimeRangeEntryTypeValue == TIMERANGE_ABSOLUTE_ENTRY)
    {
 
      owa.l7rc = usmDbTimeRangeEntryAdd(L7_UNIT_CURRENT, keytimeRangeIndexValue,
                                  keytimeRangeEntryIndexValue, TIMERANGE_ABSOLUTE_ENTRY);
      if (L7_SUCCESS == owa.l7rc)
      {   
        /* retrieve object: timeRangeAbsoluteStartDateAndTimeValue*/
        owa.len = sizeof(objtimeRangeAbsoluteStartDateAndTimeValue);
        owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartDateAndTime,
                                    (xLibU8_t *) &objtimeRangeAbsoluteStartDateAndTimeValue, 
                                    &owa.len);
        if(owa.rc != XLIBRC_SUCCESS)
        {
          owa.rc = XLIBRC_FILTER_MISSING; 
          FPOBJ_TRACE_EXIT(bufp, owa);
          return owa.rc;
        }
        if (objtimeRangeAbsoluteStartDateAndTimeValue == L7_ENABLE)
        {
          removeEntry = L7_FALSE;
          /* retrieve object: timeRangeAbsoluteStartMonthValue*/
          owa.len = sizeof(objtimeRangeAbsoluteStartMonthValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartMonth,
                                      (xLibU8_t *) &objtimeRangeAbsoluteStartMonthValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteStartDateValue*/
          owa.len = sizeof(objtimeRangeAbsoluteStartDateValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartDate,
                                      (xLibU8_t *) &objtimeRangeAbsoluteStartDateValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteStartYearValue*/
          owa.len = sizeof(objtimeRangeAbsoluteStartYearValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartYear,
                                      (xLibU8_t *) &objtimeRangeAbsoluteStartYearValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteStartTimeHoursValue*/
          owa.len = sizeof(objtimeRangeAbsoluteStartTimeHoursValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartTimeHours,
                                      (xLibU8_t *) &objtimeRangeAbsoluteStartTimeHoursValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteStartTimeMinutesValue*/
          owa.len = sizeof(objtimeRangeAbsoluteStartTimeMinutesValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteStartTimeMinutes,
                                      (xLibU8_t *) &objtimeRangeAbsoluteStartTimeMinutesValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }
          if (!((objtimeRangeAbsoluteStartDateValue == L7_LEAP_YEAR_DAY &&
                  objtimeRangeAbsoluteStartMonthValue == L7_MONTH_FEB &&
                  L7_LEAP_YEAR_CHECK(objtimeRangeAbsoluteStartYearValue)) ||
              ((objtimeRangeAbsoluteStartMonthValue >= 1 &&
                objtimeRangeAbsoluteStartDateValue <= daysInAMonth [objtimeRangeAbsoluteStartMonthValue]) &&
               (objtimeRangeAbsoluteStartDateValue >= 1 &&
                objtimeRangeAbsoluteStartDateValue <= daysInAMonth [objtimeRangeAbsoluteStartMonthValue]) &&
               (objtimeRangeAbsoluteStartYearValue >= L7_TIMERANGE_MIN_YEAR &&
                objtimeRangeAbsoluteStartYearValue <= L7_TIMERANGE_MAX_YEAR))))
          {
            usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                      keytimeRangeIndexValue,
                                      keytimeRangeEntryIndexValue);
            owa.rc = XLIBRC_TIMERANGE_INVALID_START_DATE; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }
          
          memset(&startDateAndTime, 0, sizeof(startDateAndTime));
          startDateAndTime.hour = objtimeRangeAbsoluteStartTimeHoursValue;
          startDateAndTime.minute = objtimeRangeAbsoluteStartTimeMinutesValue;
          startDateAndTime.date.absoluteDate.day = objtimeRangeAbsoluteStartDateValue;
          startDateAndTime.date.absoluteDate.month = objtimeRangeAbsoluteStartMonthValue;
          startDateAndTime.date.absoluteDate.year = objtimeRangeAbsoluteStartYearValue;
          owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeAdd(L7_UNIT_CURRENT,
                                                      keytimeRangeIndexValue,
                                                      keytimeRangeEntryIndexValue,
                                                      TIMERANGE_ABSOLUTE_ENTRY,
                                                      &startDateAndTime);
          if (L7_SUCCESS != owa.l7rc)
          {
            usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                      keytimeRangeIndexValue,
                                      keytimeRangeEntryIndexValue);
            owa.rc = XLIBRC_TIMERANGE_ENTRY_START_TIME_ERROR; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }
        }
        /* retrieve object: timeRangeAbsoluteEndDateAndTimeValue*/
        owa.len = sizeof(objtimeRangeAbsoluteEndDateAndTimeValue);
        owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndDateAndTime,
                                    (xLibU8_t *) &objtimeRangeAbsoluteEndDateAndTimeValue, 
                                    &owa.len);
        if(owa.rc != XLIBRC_SUCCESS)
        {
          owa.rc = XLIBRC_FILTER_MISSING; 
          FPOBJ_TRACE_EXIT(bufp, owa);
          return owa.rc;
        }
        if (objtimeRangeAbsoluteEndDateAndTimeValue == L7_ENABLE)
        {
          removeEntry = L7_FALSE;
          /* retrieve object: timeRangeAbsoluteEndMonthValue*/
          owa.len = sizeof(objtimeRangeAbsoluteEndMonthValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndMonth,
                                      (xLibU8_t *) &objtimeRangeAbsoluteEndMonthValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteEndDateValue*/
          owa.len = sizeof(objtimeRangeAbsoluteEndDateValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndDate,
                                      (xLibU8_t *) &objtimeRangeAbsoluteEndDateValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteEndYearValue*/
          owa.len = sizeof(objtimeRangeAbsoluteEndYearValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndYear,
                                      (xLibU8_t *) &objtimeRangeAbsoluteEndYearValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteEndTimeHoursValue*/
          owa.len = sizeof(objtimeRangeAbsoluteEndTimeHoursValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndTimeHours,
                                      (xLibU8_t *) &objtimeRangeAbsoluteEndTimeHoursValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          /* retrieve object: timeRangeAbsoluteEndTimeMinutesValue*/
          owa.len = sizeof(objtimeRangeAbsoluteEndTimeMinutesValue);
          owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeAbsoluteEndTimeMinutes,
                                      (xLibU8_t *) &objtimeRangeAbsoluteEndTimeMinutesValue, 
                                      &owa.len);
          if(owa.rc != XLIBRC_SUCCESS)
          {
            owa.rc = XLIBRC_FILTER_MISSING; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          if (!((objtimeRangeAbsoluteEndDateValue == L7_LEAP_YEAR_DAY &&
                  objtimeRangeAbsoluteEndMonthValue == L7_MONTH_FEB &&
                  L7_LEAP_YEAR_CHECK(objtimeRangeAbsoluteEndYearValue)) ||
              ((objtimeRangeAbsoluteEndMonthValue >= 1 &&
                objtimeRangeAbsoluteEndDateValue <= daysInAMonth [objtimeRangeAbsoluteEndMonthValue]) &&
               (objtimeRangeAbsoluteEndDateValue >= 1 &&
                objtimeRangeAbsoluteEndDateValue <= daysInAMonth [objtimeRangeAbsoluteEndMonthValue]) &&
               (objtimeRangeAbsoluteEndYearValue >= L7_TIMERANGE_MIN_YEAR &&
                objtimeRangeAbsoluteEndYearValue <= L7_TIMERANGE_MAX_YEAR))))
          {
            usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                      keytimeRangeIndexValue,
                                      keytimeRangeEntryIndexValue);
            owa.rc = XLIBRC_TIMERANGE_INVALID_END_DATE; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }

          memset(&endDateAndTime, 0, sizeof(endDateAndTime));
          endDateAndTime.hour = objtimeRangeAbsoluteEndTimeHoursValue;
          endDateAndTime.minute = objtimeRangeAbsoluteEndTimeMinutesValue;
          endDateAndTime.date.absoluteDate.day = objtimeRangeAbsoluteEndDateValue;
          endDateAndTime.date.absoluteDate.month = objtimeRangeAbsoluteEndMonthValue;
          endDateAndTime.date.absoluteDate.year = objtimeRangeAbsoluteEndYearValue;
          owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeAdd(L7_UNIT_CURRENT,
                                                      keytimeRangeIndexValue,
                                                      keytimeRangeEntryIndexValue,
                                                      TIMERANGE_ABSOLUTE_ENTRY,
                                                      &endDateAndTime);
          if (L7_SUCCESS != owa.l7rc)
          {
            usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                      keytimeRangeIndexValue,
                                      keytimeRangeEntryIndexValue);
            owa.rc = XLIBRC_TIMERANGE_ENTRY_END_TIME_ERROR; 
            FPOBJ_TRACE_EXIT(bufp, owa);
            return owa.rc;
          }
        }
        if (removeEntry == L7_TRUE)
        {
          usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                      keytimeRangeIndexValue,
                                      keytimeRangeEntryIndexValue);
          owa.rc = XLIBRC_TIMERANGE_ABS_ENTRY_ADD_ERROR;
          FPOBJ_TRACE_EXIT(bufp, owa);
          return owa.rc;
        }
      }
      else       
      {
        switch (owa.l7rc)
        {
          case L7_ERROR:
            owa.rc = XLIBRC_TIMERANGE_ENTRY_ID_ALREADY_EXISTS; 
            break;
          case L7_TABLE_IS_FULL:
            owa.rc = XLIBRC_TIMERANGE_MAX_ENTRIES_ALREADY_CONFIGURED; 
            break;
          case L7_ALREADY_CONFIGURED:
            owa.rc = XLIBRC_TIMERANGE_ABS_ENTRY_ALREADY_EXISTS; 
            break;
          default:
            owa.rc = XLIBRC_FAILURE;
            break;
        }
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }
    }
    else if (objtimeRangeEntryTypeValue == TIMERANGE_PERIODIC_ENTRY)
    {
      /* retrieve object: timeRangePeriodicStartDayValue*/
      owa.len = sizeof(objtimeRangePeriodicStartDayValue);
      owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicStartDay,
                                  (xLibU8_t *) &objtimeRangePeriodicStartDayValue, 
                                  &owa.len);
      if(owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING; 
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }

      /* retrieve object: timeRangePeriodicStartTimeHoursValue*/
      owa.len = sizeof(objtimeRangePeriodicStartTimeHoursValue);
      owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicStartTimeHours,
                                  (xLibU8_t *) &objtimeRangePeriodicStartTimeHoursValue, 
                                  &owa.len);
      if(owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING; 
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }

      /* retrieve object: timeRangePeriodicStartTimeMinutesValue*/
      owa.len = sizeof(objtimeRangePeriodicStartTimeMinutesValue);
      owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicStartTimeMinutes,
                                  (xLibU8_t *) &objtimeRangePeriodicStartTimeMinutesValue, 
                                  &owa.len);
      if(owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING; 
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }

      /* retrieve object: timeRangePeriodicEndDayValue*/
      owa.len = sizeof(objtimeRangePeriodicEndDayValue);
      owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicEndDay,
                                  (xLibU8_t *) &objtimeRangePeriodicEndDayValue, 
                                  &owa.len);
      if(owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING; 
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }

      /* retrieve object: timeRangePeriodicEndTimeHoursValue*/
      owa.len = sizeof(objtimeRangePeriodicEndTimeHoursValue);
      owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicEndTimeHours,
                                  (xLibU8_t *) &objtimeRangePeriodicEndTimeHoursValue, 
                                  &owa.len);
      if(owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING; 
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }

      /* retrieve object: timeRangePeriodicEndTimeMinutesValue*/
      owa.len = sizeof(objtimeRangePeriodicEndTimeMinutesValue);
      owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicEndTimeMinutes,
                                  (xLibU8_t *) &objtimeRangePeriodicEndTimeMinutesValue, 
                                  &owa.len);
      if(owa.rc != XLIBRC_SUCCESS)
      {
        owa.rc = XLIBRC_FILTER_MISSING; 
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }


      owa.l7rc = usmDbTimeRangeEntryAdd(L7_UNIT_CURRENT, keytimeRangeIndexValue,
                                  keytimeRangeEntryIndexValue, TIMERANGE_PERIODIC_ENTRY);
      if (L7_SUCCESS == owa.l7rc)
      {

        memset(&startDateAndTime, 0, sizeof(startDateAndTime));
        startDateAndTime.hour = objtimeRangePeriodicStartTimeHoursValue;
        startDateAndTime.minute = objtimeRangePeriodicStartTimeMinutesValue;
        startDateAndTime.date.daysOfTheWeek.dayMask = objtimeRangePeriodicStartDayValue;
        owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeAdd(L7_UNIT_CURRENT,
                                                    keytimeRangeIndexValue,
                                                    keytimeRangeEntryIndexValue,
                                                    TIMERANGE_PERIODIC_ENTRY,
                                                    &startDateAndTime);
        if (L7_SUCCESS != owa.l7rc)
        {
          usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    keytimeRangeEntryIndexValue);
          owa.rc = XLIBRC_TIMERANGE_ENTRY_START_TIME_ERROR; 
          FPOBJ_TRACE_EXIT(bufp, owa);
          return owa.rc;
        }

        memset(&endDateAndTime, 0, sizeof(endDateAndTime));
        endDateAndTime.hour = objtimeRangePeriodicEndTimeHoursValue;
        endDateAndTime.minute = objtimeRangePeriodicEndTimeMinutesValue;
        endDateAndTime.date.daysOfTheWeek.dayMask = objtimeRangePeriodicEndDayValue;
        owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeAdd(L7_UNIT_CURRENT,
                                                    keytimeRangeIndexValue,
                                                    keytimeRangeEntryIndexValue,
                                                    TIMERANGE_PERIODIC_ENTRY,
                                                    &endDateAndTime);
        if (L7_SUCCESS != owa.l7rc)
        {
          usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    keytimeRangeEntryIndexValue);
          owa.rc = XLIBRC_TIMERANGE_ENTRY_END_TIME_ERROR; 
          FPOBJ_TRACE_EXIT(bufp, owa);
          return owa.rc;
        }

      }
      else       
      {
        switch (owa.l7rc)
        {
          case L7_ERROR:
            owa.rc = XLIBRC_TIMERANGE_ENTRY_ID_ALREADY_EXISTS; 
            break;
          case L7_TABLE_IS_FULL:
            owa.rc = XLIBRC_TIMERANGE_MAX_ENTRIES_ALREADY_CONFIGURED; 
            break;
          default:
            owa.rc = XLIBRC_FAILURE;
            break;
        }
        FPOBJ_TRACE_EXIT(bufp, owa);
        return owa.rc;
      }
    }
  }
  else if(objtimeRangeEntryStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbTimeRangeEntryRemove(L7_UNIT_CURRENT,
                                         keytimeRangeIndexValue,
                                         keytimeRangeEntryIndexValue);
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangePeriodicStartDay
*
* @purpose Get 'timeRangePeriodicStartDay'
 *@description  [timeRangePeriodicStartDay] The start day for an periodic entry
* in the time range   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangePeriodicStartDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicStartDayValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    keytimeRangeEntryIndexValue,
                                    &entryType,
                                    &startDateAndTime);
  if (owa.l7rc == L7_SUCCESS && entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    objtimeRangePeriodicStartDayValue = startDateAndTime.date.daysOfTheWeek.dayMask;
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicStartDayValue, sizeof(objtimeRangePeriodicStartDayValue));

  /* return the object value: timeRangePeriodicStartDay */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicStartDayValue,
                      sizeof(objtimeRangePeriodicStartDayValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangePeriodicStartDay
*
* @purpose Set 'timeRangePeriodicStartDay'
 *@description  [timeRangePeriodicStartDay] The start day for an periodic entry
* in the time range   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangePeriodicStartDay (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}



/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangePeriodicEndDay
*
* @purpose Get 'timeRangePeriodicEndDay'
 *@description  [timeRangePeriodicEndDay] The end day for an periodic entry
* in the time range   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangePeriodicEndDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicEndDayValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                    keytimeRangeIndexValue,
                                    keytimeRangeEntryIndexValue,
                                    &entryType,
                                    &endDateAndTime);
  if (owa.l7rc == L7_SUCCESS && entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    objtimeRangePeriodicEndDayValue = endDateAndTime.date.daysOfTheWeek.dayMask;
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicEndDayValue, sizeof(objtimeRangePeriodicEndDayValue));

  /* return the object value: timeRangePeriodicEndDay */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicEndDayValue,
                      sizeof(objtimeRangePeriodicEndDayValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangePeriodicEndDay
*
* @purpose Set 'timeRangePeriodicEndDay'
 *@description  [timeRangePeriodicEndDay] The end day for an periodic entry
* in the time range   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangePeriodicEndDay (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangePeriodicStartTimeHours
*
* @purpose Get 'timeRangePeriodicStartTimeHours'
 *@description  [timeRangePeriodicStartTimeHours] Start hour for the Periodic
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangePeriodicStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicStartTimeHourValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                keytimeRangeIndexValue,
                                keytimeRangeEntryIndexValue,
                                &entryType,
                                &startDateAndTime);
  if (owa.l7rc == L7_SUCCESS && entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    objtimeRangePeriodicStartTimeHourValue = startDateAndTime.hour;
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicStartTimeHourValue, sizeof(objtimeRangePeriodicStartTimeHourValue));

  /* return the object value: timeRangePeriodicStartTimeHour */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicStartTimeHourValue,
                      sizeof(objtimeRangePeriodicStartTimeHourValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangePeriodicStartTimeHours
*
* @purpose List 'timeRangePeriodicStartTimeHours'
 *@description  [timeRangePeriodicStartTimeHours] Start hour for the Periodic
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangePeriodicStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicStartTimeHoursValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangePeriodicStartTimeHoursValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicStartTimeHours,
              (xLibU8_t *) &objtimeRangePeriodicStartTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangePeriodicStartTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else 
  {
    objtimeRangePeriodicStartTimeHoursValue++;
  }
  if (objtimeRangePeriodicStartTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicStartTimeHoursValue, owa.len);

  /* return the object value: timeRangePeriodicStartTimeHours */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicStartTimeHoursValue,
                       sizeof(objtimeRangePeriodicStartTimeHoursValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangePeriodicStartTimeHours
*
* @purpose Set 'timeRangePeriodicStartTimeHours'
 *@description  [timeRangePeriodicStartTimeHours] Start hour for the Periodic
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangePeriodicStartTimeHours (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangePeriodicStartTimeMinutes
*
* @purpose Get 'timeRangePeriodicStartTimeMinutes'
 *@description  [timeRangePeriodicStartTimeMinutes] Start minute for the
* Periodic time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangePeriodicStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicStartTimeMinuteValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryDateAndTimeParms_t startDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeEntryStartDateAndTimeGet(L7_UNIT_CURRENT,
                                keytimeRangeIndexValue,
                                keytimeRangeEntryIndexValue,
                                &entryType,
                                &startDateAndTime);
  if (owa.l7rc == L7_SUCCESS && entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    objtimeRangePeriodicStartTimeMinuteValue = startDateAndTime.minute;
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicStartTimeMinuteValue, sizeof(objtimeRangePeriodicStartTimeMinuteValue));

  /* return the object value: timeRangePeriodicStartTimeMinute */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicStartTimeMinuteValue,
                      sizeof(objtimeRangePeriodicStartTimeMinuteValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangePeriodicStartTimeMinutes
*
* @purpose List 'timeRangePeriodicStartTimeMinutes'
 *@description  [timeRangePeriodicStartTimeMinutes] Start minute for the
* Periodic time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangePeriodicStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangePeriodicStartTimeMinutesValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicStartTimeMinutes,
              (xLibU8_t *) &objtimeRangePeriodicStartTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangePeriodicStartTimeMinutesValue = L7_TIMEZONE_MINS_MIN;
  }
  else 
  {
    objtimeRangePeriodicStartTimeMinutesValue++;
  }
  if (objtimeRangePeriodicStartTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicStartTimeMinutesValue, owa.len);

  /* return the object value: timeRangePeriodicStartTimeMinutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicStartTimeMinutesValue,
                       sizeof(objtimeRangePeriodicStartTimeMinutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangePeriodicStartTimeMinutes
*
* @purpose Set 'timeRangePeriodicStartTimeMinutes'
 *@description  [timeRangePeriodicStartTimeMinutes] Start minute for the
* Periodic time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangePeriodicStartTimeMinutes (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangePeriodicEndTimeHours
*
* @purpose Get 'timeRangePeriodicEndTimeHours'
 *@description  [timeRangePeriodicEndTimeHours] End hour for the Periodic time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangePeriodicEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicEndTimeHourValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                keytimeRangeIndexValue,
                                keytimeRangeEntryIndexValue,
                                &entryType,
                                &endDateAndTime);
  if (owa.l7rc == L7_SUCCESS && entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    objtimeRangePeriodicEndTimeHourValue = endDateAndTime.hour;
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicEndTimeHourValue, sizeof(objtimeRangePeriodicEndTimeHourValue));

  /* return the object value: timeRangePeriodicEndTimeHour */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicEndTimeHourValue,
                      sizeof(objtimeRangePeriodicEndTimeHourValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangePeriodicEndTimeHours
*
* @purpose List 'timeRangePeriodicEndTimeHours'
 *@description  [timeRangePeriodicEndTimeHours] End hour for the Periodic time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangePeriodicEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicEndTimeHoursValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangePeriodicEndTimeHoursValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicEndTimeHours,
              (xLibU8_t *) &objtimeRangePeriodicEndTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangePeriodicEndTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else 
  {
    objtimeRangePeriodicEndTimeHoursValue++;
  }
  if (objtimeRangePeriodicEndTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicEndTimeHoursValue, owa.len);

  /* return the object value: timeRangePeriodicEndTimeHours */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicEndTimeHoursValue,
                       sizeof(objtimeRangePeriodicEndTimeHoursValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangePeriodicEndTimeHours
*
* @purpose Set 'timeRangePeriodicEndTimeHours'
 *@description  [timeRangePeriodicEndTimeHours] End hour for the Periodic time
* range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangePeriodicEndTimeHours (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangePeriodicEndTimeMinutes
*
* @purpose Get 'timeRangePeriodicEndTimeMinutes'
 *@description  [timeRangePeriodicEndTimeMinutes] End Minute for the Periodic
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangePeriodicEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicEndTimeMinuteValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryDateAndTimeParms_t endDateAndTime;
  timeRangeEntryType_t             entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

/* get the value from application */
  owa.l7rc = usmDbTimeRangeEntryEndDateAndTimeGet(L7_UNIT_CURRENT,
                                keytimeRangeIndexValue,
                                keytimeRangeEntryIndexValue,
                                &entryType,
                                &endDateAndTime);
  if (owa.l7rc == L7_SUCCESS && entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    objtimeRangePeriodicEndTimeMinuteValue = endDateAndTime.minute;
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

    FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicEndTimeMinuteValue, sizeof(objtimeRangePeriodicEndTimeMinuteValue));

  /* return the object value: timeRangePeriodicEndTimeMinute */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicEndTimeMinuteValue,
                      sizeof(objtimeRangePeriodicEndTimeMinuteValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangePeriodicEndTimeMinutes
*
* @purpose List 'timeRangePeriodicEndTimeMinutes'
 *@description  [timeRangePeriodicEndTimeMinutes] End Minute for the Periodic
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangePeriodicEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER(bufp);

  owa.len = sizeof(objtimeRangePeriodicEndTimeMinutesValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicEndTimeMinutes,
              (xLibU8_t *) &objtimeRangePeriodicEndTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objtimeRangePeriodicEndTimeMinutesValue = L7_TIMEZONE_MINS_MIN;
  }
  else 
  {
    objtimeRangePeriodicEndTimeMinutesValue++;
  }
  if (objtimeRangePeriodicEndTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangePeriodicEndTimeMinutesValue, owa.len);

  /* return the object value: timeRangePeriodicEndTimeMinutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicEndTimeMinutesValue,
                       sizeof(objtimeRangePeriodicEndTimeMinutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangePeriodicEndTimeMinutes
*
* @purpose Set 'timeRangePeriodicEndTimeMinutes'
 *@description  [timeRangePeriodicEndTimeMinutes] End Minute for the Periodic
* time range entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangePeriodicEndTimeMinutes (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteStartDateAndTime
*
* @purpose Get 'timeRangeAbsoluteStartDateAndTime'
 *@description  [timeRangeAbsoluteStartDateAndTime] Mentions whether the Start
* Date and Time is specified    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteStartDateAndTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteStartDateAndTimeValue = L7_DISABLE; 
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryType_t entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

    /* get the value from application */
  if ((usmDbTimeRangeEntryTypeGet(L7_UNIT_CURRENT, 
                                   keytimeRangeIndexValue, 
                                   keytimeRangeEntryIndexValue,
                                   &entryType) == L7_SUCCESS) &&
      (entryType == TIMERANGE_ABSOLUTE_ENTRY))
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                            keytimeRangeIndexValue,
                            keytimeRangeEntryIndexValue,
                            TIMERANGE_ENTRY_START_TIME) == L7_TRUE)
    {
      objtimeRangeAbsoluteStartDateAndTimeValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteStartDateAndTimeValue, sizeof(objtimeRangeAbsoluteStartDateAndTimeValue));

  /* return the object value: timeRangeAbsoluteStartDateAndTime */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteStartDateAndTimeValue,
                      sizeof(objtimeRangeAbsoluteStartDateAndTimeValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteStartDateAndTime
*
* @purpose Set 'timeRangeAbsoluteStartDateAndTime'
 *@description  [timeRangeAbsoluteStartDateAndTime] Mentions whether the Start
* Date and Time is specified    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteStartDateAndTime (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteEndDateAndTime
*
* @purpose Get 'timeRangeAbsoluteEndDateAndTime'
 *@description  [timeRangeAbsoluteEndDateAndTime] Mentions whether the End Date
* and Time is specified    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteEndDateAndTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEndDateAndTimeValue = L7_DISABLE;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t keytimeRangeEntryIndexValue;
  timeRangeEntryType_t entryType;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* retrieve key: timeRangeEntryIndex */
  owa.len = sizeof(keytimeRangeEntryIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeEntryIndex,
                              (xLibU8_t *) &keytimeRangeEntryIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeEntryIndexValue, 
          owa.len);

    /* get the value from application */
  if ((usmDbTimeRangeEntryTypeGet(L7_UNIT_CURRENT, 
                                   keytimeRangeIndexValue, 
                                   keytimeRangeEntryIndexValue,
                                   &entryType) == L7_SUCCESS) &&
      (entryType == TIMERANGE_ABSOLUTE_ENTRY))
  {
    if (usmDbTimeRangeIsFieldConfigured(L7_UNIT_CURRENT,
                            keytimeRangeIndexValue,
                            keytimeRangeEntryIndexValue,
                            TIMERANGE_ENTRY_END_TIME) == L7_TRUE)
    {
      objtimeRangeAbsoluteEndDateAndTimeValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEndDateAndTimeValue, sizeof(objtimeRangeAbsoluteEndDateAndTimeValue));

  /* return the object value: timeRangeAbsoluteEndDateAndTime */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEndDateAndTimeValue,
                      sizeof(objtimeRangeAbsoluteEndDateAndTimeValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_baseTimeRange_timeRangeAbsoluteEndDateAndTime
*
* @purpose Set 'timeRangeAbsoluteEndDateAndTime'
 *@description  [timeRangeAbsoluteEndDateAndTime] Mentions whether the End Date
* and Time is specified    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeRange_timeRangeAbsoluteEndDateAndTime (void *wap, void *bufp)
{
  /*This object is set in rowstatus object timeRangeEntryStatus*/
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeNumPeriodicEntries
*
* @purpose Get 'timeRangeNumPeriodicEntries'
 *@description  [timeRangeNumPeriodicEntries] Current number of periodic entries   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeNumPeriodicEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeNumPeriodicEntriesValue;
  xLibU32_t keytimeRangeIndexValue;
  xLibU32_t absEntryNum;
  L7_BOOL   absEntryPresent = L7_FALSE;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* get the value from application */
  owa.rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &absEntryNum);
  if (owa.rc == L7_SUCCESS)
  {
    absEntryPresent = L7_TRUE;
  }

  owa.rc = usmDbTimeRangeNumEntriesGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, &objtimeRangeNumPeriodicEntriesValue);
  if (owa.rc == L7_SUCCESS  )
  {
    if (absEntryPresent == L7_TRUE && objtimeRangeNumPeriodicEntriesValue != 0)
    {
      objtimeRangeNumPeriodicEntriesValue = objtimeRangeNumPeriodicEntriesValue - 1;
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeNumPeriodicEntriesValue, sizeof(objtimeRangeNumPeriodicEntriesValue));

  /* return the object value: timeRangeNumPeriodicEntries */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeNumPeriodicEntriesValue,
                      sizeof(objtimeRangeNumPeriodicEntriesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeAbsoluteEntryExist
*
* @purpose Get 'timeRangeAbsoluteEntryExist'
 *@description  [timeRangeAbsoluteEntryExist] Specifies whether an absolute time
* entry exists   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeAbsoluteEntryExist (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeAbsoluteEntryExistValue;
  xLibU32_t keytimeRangeIndexValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* get the value from application */
  owa.l7rc = usmDbTimeRangeAbsoluteEntryNumGet(L7_UNIT_CURRENT, keytimeRangeIndexValue,
 &objtimeRangeAbsoluteEntryExistValue);
  if(owa.l7rc == L7_SUCCESS)
  {
    objtimeRangeAbsoluteEntryExistValue = L7_TRUE;
  }
  else
  {
    objtimeRangeAbsoluteEntryExistValue = L7_FALSE;
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeAbsoluteEntryExistValue, sizeof(objtimeRangeAbsoluteEntryExistValue));

  /* return the object value: timeRangeAbsoluteEntryExist */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeAbsoluteEntryExistValue,
                      sizeof(objtimeRangeAbsoluteEntryExistValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangePeriodicStartDay
*
* @purpose List 'timeRangePeriodicStartDay'
 *@description  [timeRangePeriodicStartDay] The start day for an periodic entry
* in the time range   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangePeriodicStartDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicStartDayValue;  
  FPOBJ_TRACE_ENTER(bufp);
    
  owa.len = sizeof(objtimeRangePeriodicStartDayValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicStartDay,
              (xLibU8_t *) &objtimeRangePeriodicStartDayValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  { 
    objtimeRangePeriodicStartDayValue = 2;
  }   
  else
  {
    objtimeRangePeriodicStartDayValue <<= 1;
  }
 
  if (objtimeRangePeriodicStartDayValue > ((1 << L7_DAY_SAT )))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_VALUE(bufp, objtimeRangePeriodicStartDayValue, owa.len);
 
  /* return the object value: timeRangePeriodicStartDay */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicStartDayValue,
                       sizeof(objtimeRangePeriodicStartDayValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}
 
/*******************************************************************************
* @function fpObjList_baseTimeRange_timeRangePeriodicEndDay
*
* @purpose List 'timeRangePeriodicEndDay'
 *@description  [timeRangePeriodicEndDay] The start day for an periodic entry
* in the time range   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseTimeRange_timeRangePeriodicEndDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangePeriodicEndDayValue;  
  FPOBJ_TRACE_ENTER(bufp);
    
  owa.len = sizeof(objtimeRangePeriodicEndDayValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangePeriodicEndDay,
              (xLibU8_t *) &objtimeRangePeriodicEndDayValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  { 
    objtimeRangePeriodicEndDayValue = 2;
  }   
  else
  {
    objtimeRangePeriodicEndDayValue <<= 1;
  }
 
  if (objtimeRangePeriodicEndDayValue > ((1 << L7_DAY_SAT )))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_VALUE(bufp, objtimeRangePeriodicEndDayValue, owa.len);
 
  /* return the object value: timeRangePeriodicEndDay */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangePeriodicEndDayValue,
                       sizeof(objtimeRangePeriodicEndDayValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeTableMaxSizeStatus
*
* @purpose Get 'timeRangeTableMaxSizeStatus'
*
* @description  Status to detect max time ranges are configured or not.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeTableMaxSizeStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_BOOL objtimeRangeTableMaxSizeStatusValue = L7_XUI_TRUE;
  xLibU32_t tableSize, tableMax;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  owa.l7rc = usmDbTimeRangeCurrNumGet(L7_UNIT_CURRENT, &tableSize);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &tableSize, sizeof (tableSize));

  owa.l7rc = usmDbTimeRangeMaxNumGet(L7_UNIT_CURRENT, &tableMax);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &tableMax, sizeof (tableMax));

  if (tableSize < tableMax)
  {
    objtimeRangeTableMaxSizeStatusValue = L7_XUI_TRUE;
  }
  else if (tableSize >= tableMax)
  {
    objtimeRangeTableMaxSizeStatusValue = L7_XUI_FALSE;
  }

  /* return the object value */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtimeRangeTableMaxSizeStatusValue,
                           sizeof (objtimeRangeTableMaxSizeStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTimeRange_timeRangeEntryTableMaxSizeStatus
*
* @purpose Get 'timeRangeEntryTableMaxSizeStatus'
 *@description  [timeRangeEntryTableMaxSizeStatus] Current number of Time Range entries   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeRange_timeRangeEntryTableMaxSizeStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objtimeRangeEntryTableMaxSizeStatusValue = L7_XUI_TRUE;
  xLibU32_t keytimeRangeIndexValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: timeRangeIndex */
  owa.len = sizeof(keytimeRangeIndexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseTimeRange_timeRangeIndex,
                              (xLibU8_t *) &keytimeRangeIndexValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keytimeRangeIndexValue, 
          owa.len);

  /* get the value from application */
  
  owa.rc = usmDbTimeRangeNumEntriesGet(L7_UNIT_CURRENT, keytimeRangeIndexValue, 
                                       &objtimeRangeEntryTableMaxSizeStatusValue);
  if (owa.rc == L7_SUCCESS  )
  {
    if (objtimeRangeEntryTableMaxSizeStatusValue < L7_MAX_TIME_RANGE_ENTRIES)
    {
      objtimeRangeEntryTableMaxSizeStatusValue = L7_XUI_TRUE;
    }
    else if (objtimeRangeEntryTableMaxSizeStatusValue >= L7_MAX_TIME_RANGE_ENTRIES)
    {
      objtimeRangeEntryTableMaxSizeStatusValue = L7_XUI_FALSE;
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;  
  }

  FPOBJ_TRACE_VALUE (bufp, &objtimeRangeEntryTableMaxSizeStatusValue,
                     sizeof(objtimeRangeEntryTableMaxSizeStatusValue));

  /* return the object value: timeRangeNumPeriodicEntries */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objtimeRangeEntryTableMaxSizeStatusValue,
                      sizeof(objtimeRangeEntryTableMaxSizeStatusValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

