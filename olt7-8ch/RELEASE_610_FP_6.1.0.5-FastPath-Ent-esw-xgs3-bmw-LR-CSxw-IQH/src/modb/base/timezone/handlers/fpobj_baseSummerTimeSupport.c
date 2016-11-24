
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSummerTimeSupport.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  10 October 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseSummerTimeSupport_obj.h"
#include "timezone_exports.h"
#include "usmdb_timezone_api.h"



/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringStartWeek
*
* @purpose Get 'RecurringStartWeek'
 *@description  [RecurringStartWeek] Starting week number.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringStartWeek (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartWeekValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_START_WEEK, (void *)&objStartWeekValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartWeekValue, sizeof (objStartWeekValue));

  /* return the object value: StartWeek */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartWeekValue, sizeof (objStartWeekValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringStartWeek
*
* @purpose Set 'RecurringStartWeek'
 *@description  [RecurringStartWeek] Starting week number.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringStartWeek (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartWeekValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartWeek */
  owa.len = sizeof (objStartWeekValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartWeekValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartWeekValue, owa.len);

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringStartDay
*
* @purpose Get 'RecurringStartDay'
 *@description  [RecurringStartDay] Startng day for summertime.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringStartDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartDayValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_START_DAY, (void *)&objStartDayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartDayValue, sizeof (objStartDayValue));

  /* return the object value: StartDay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartDayValue, sizeof (objStartDayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringStartDay
*
* @purpose Set 'RecurringStartDay'
 *@description  [RecurringStartDay] Startng day for summertime.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringStartDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartDayValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartDay */
  owa.len = sizeof (objStartDayValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartDayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartDayValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringStartMonth
*
* @purpose Get 'RecurringStartMonth'
 *@description  [RecurringStartMonth] Starting month for the summer time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringStartMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
   owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_START_MONTH, (void *)&objStartMonthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartMonthValue, sizeof (objStartMonthValue));

  /* return the object value: StartMonth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartMonthValue, sizeof (objStartMonthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringStartMonth
*
* @purpose Set 'RecurringStartMonth'
 *@description  [RecurringStartMonth] Starting month for the summer time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringStartMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartMonth */
  owa.len = sizeof (objStartMonthValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartMonthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartMonthValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringEndWeek
*
* @purpose Get 'RecurringEndWeek'
 *@description  [RecurringEndWeek] Ending week number for the summertime .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringEndWeek (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndWeekValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_END_WEEK, (void *)&objEndWeekValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndWeekValue, sizeof (objEndWeekValue));

  /* return the object value: EndWeek */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndWeekValue, sizeof (objEndWeekValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringEndWeek
*
* @purpose Set 'RecurringEndWeek'
 *@description  [RecurringEndWeek] Ending week number for the summertime .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringEndWeek (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndWeekValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndWeek */
  owa.len = sizeof (objEndWeekValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndWeekValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndWeekValue, owa.len);

 /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */

}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringEndDay
*
* @purpose Get 'RecurringEndDay'
 *@description  [RecurringEndDay] Ending day for the summertime .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringEndDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndDayValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_END_DAY, (void *)&objEndDayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndDayValue, sizeof (objEndDayValue));

  /* return the object value: EndDay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndDayValue, sizeof (objEndDayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringEndDay
*
* @purpose Set 'RecurringEndDay'
 *@description  [RecurringEndDay] Ending day for the summertime .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringEndDay (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndDayValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndDay */
  owa.len = sizeof (objEndDayValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndDayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndDayValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringEndMonth
*
* @purpose Get 'RecurringEndMonth'
 *@description  [RecurringEndMonth] Ending month for the summertime .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringEndMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_END_MONTH, (void *)&objEndMonthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndMonthValue, sizeof (objEndMonthValue));

  /* return the object value: EndMonth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndMonthValue, sizeof (objEndMonthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringEndMonth
*
* @purpose Set 'RecurringEndMonth'
 *@description  [RecurringEndMonth] Ending month for the summertime .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringEndMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

   xLibU32_t objEndMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndDay */
  owa.len = sizeof (objEndMonthValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndMonthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndMonthValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringStartTimeHours
*
* @purpose Get 'RecurringStartTimeHours'
 *@description  [RecurringStartTimeHours] Hours of the Starting time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_START_HOUR, (void *)&objStartTimeHoursValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartTimeHoursValue, sizeof (objStartTimeHoursValue));

  /* return the object value: StartTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeHoursValue,
                           sizeof (objStartTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringStartTimeHours
*
* @purpose Set 'RecurringStartTimeHours'
 *@description  [RecurringStartTimeHours] Hours of the Starting time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartTimeHours */
  owa.len = sizeof (objStartTimeHoursValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartTimeHoursValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringStartTimeMinutes
*
* @purpose Get 'RecurringStartTimeMinutes'
 *@description  [RecurringStartTimeMinutes] Minutes of the starting time for the
* summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_START_MINUTE, (void *)&objStartTimeMinutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartTimeMinutesValue, sizeof (objStartTimeMinutesValue));

  /* return the object value: StartTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeMinutesValue,
                           sizeof (objStartTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringStartTimeMinutes
*
* @purpose Set 'RecurringStartTimeMinutes'
 *@description  [RecurringStartTimeMinutes] Minutes of the starting time for the
* summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartTimeMinutes */
  owa.len = sizeof (objStartTimeMinutesValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartTimeMinutesValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringEndTimeHours
*
* @purpose Get 'RecurringEndTimeHours'
 *@description  [RecurringEndTimeHours] Hours of the ending time for summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_END_HOUR, (void *)&objEndTimeHoursValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndTimeHoursValue, sizeof (objEndTimeHoursValue));

  /* return the object value: EndTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeHoursValue,
                           sizeof (objEndTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringEndTimeHours
*
* @purpose Set 'RecurringEndTimeHours'
 *@description  [RecurringEndTimeHours] Hours of the ending time for summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndTimeHours */
  owa.len = sizeof (objEndTimeHoursValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndTimeHoursValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringEndTimeMinutes
*
* @purpose Get 'RecurringEndTimeMinutes'
 *@description  [RecurringEndTimeMinutes] Minutes of the ending time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_END_MINUTE, (void *)&objEndTimeMinutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndTimeMinutesValue, sizeof (objEndTimeMinutesValue));

  /* return the object value: EndTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeMinutesValue,
                           sizeof (objEndTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringEndTimeMinutes
*
* @purpose Set 'RecurringEndTimeMinutes'
 *@description  [RecurringEndTimeMinutes] Minutes of the ending time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndTimeMinutes */
  owa.len = sizeof (objEndTimeMinutesValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndTimeMinutesValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringTimeOffSet
*
* @purpose Get 'RecurringTimeOffSet'
 *@description  [RecurringTimeOffSet] Number of minutes to add during summer time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringTimeOffSet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTimeOffSetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_OFFSET_MINUTES, (void *)&objTimeOffSetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTimeOffSetValue, sizeof (objTimeOffSetValue));

  /* return the object value: TimeOffSet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeOffSetValue, sizeof (objTimeOffSetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringTimeOffSet
*
* @purpose Set 'RecurringTimeOffSet'
 *@description  [RecurringTimeOffSet] Number of minutes to add during summer time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringTimeOffSet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTimeOffSetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TimeOffSet */
  owa.len = sizeof (objTimeOffSetValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTimeOffSetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTimeOffSetValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_RecurringZone
*
* @purpose Get 'RecurringZone'
 *@description  [RecurringZone] The acronym of the time zone to be used when summer time
* is in effect.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_RecurringZone (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objZoneValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStRecurringGet (L7_UNIT_CURRENT, L7_ZONE_ACRONYM, (void *)objZoneValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objZoneValue, strlen (objZoneValue));

  /* return the object value: Zone */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objZoneValue, strlen (objZoneValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_RecurringZone
*
* @purpose Set 'RecurringZone'
 *@description  [RecurringZone] The acronym of the time zone to be used when summer time
* is in effect.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_RecurringZone (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objZoneValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Zone */
  owa.len = sizeof (objZoneValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objZoneValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objZoneValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}






/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartMonth
*
* @purpose Get 'NonRecurringStartMonth'
 *@description  [NonRecurringStartMonth] Starrt month for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringStartMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_START_MONTH, (void *)&objStartMonthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartMonthValue, sizeof (objStartMonthValue));

  /* return the object value: StartMonth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartMonthValue, sizeof (objStartMonthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringStartMonth
*
* @purpose Set 'NonRecurringStartMonth'
 *@description  [NonRecurringStartMonth] Starrt month for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringStartMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartMonth */
  owa.len = sizeof (objStartMonthValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartMonthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartMonthValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartDate
*
* @purpose Get 'NonRecurringStartDate'
 *@description  [NonRecurringStartDate] start date for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringStartDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartDateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_START_DAY, (void *)&objStartDateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartDateValue, sizeof (objStartDateValue));

  /* return the object value: StartDate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartDateValue, sizeof (objStartDateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartDate
*
* @purpose Get 'NonRecurringStartDate'
 *@description  [NonRecurringStartDate] start date for the summertime support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringStartDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartDateValue;
  
  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof(objStartDateValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartDate,
                          (xLibU8_t *) &objStartDateValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objStartDateValue = 1;
  }
  else 
  {
    objStartDateValue++;
  }
  if (objStartDateValue > 31)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }    

  FPOBJ_TRACE_VALUE (bufp, &objStartDateValue, sizeof (objStartDateValue));

  /* return the object value: StartDate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartDateValue, sizeof (objStartDateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringStartDate
*
* @purpose Set 'NonRecurringStartDate'
 *@description  [NonRecurringStartDate] start date for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringStartDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartDateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartDate */
  owa.len = sizeof (objStartDateValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartDateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartDateValue, owa.len);

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartYear
*
* @purpose Get 'NonRecurringStartYear'
 *@description  [NonRecurringStartYear] Starting year for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringStartYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartYearValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_START_YEAR, (void *)&objStartYearValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartYearValue, sizeof (objStartYearValue));

  /* return the object value: StartYear */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartYearValue, sizeof (objStartYearValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartYear
*
* @purpose Get 'NonRecurringStartYear'
 *@description  [NonRecurringStartYear] Starting year for the summertime support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringStartYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartYearValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(objStartYearValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartYear,
                          (xLibU8_t *) &objStartYearValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objStartYearValue = L7_START_SUMMER_TIME_YEAR;
  }
  else
  {
    objStartYearValue++;
  }
  if (objStartYearValue > L7_END_SUMMER_TIME_YEAR)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartYearValue, sizeof (objStartYearValue));

  /* return the object value: StartYear */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartYearValue, sizeof (objStartYearValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringStartYear
*
* @purpose Set 'NonRecurringStartYear'
 *@description  [NonRecurringStartYear] Starting year for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringStartYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartYearValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartYear */
  owa.len = sizeof (objStartYearValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartYearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartYearValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndMonth
*
* @purpose Get 'NonRecurringEndMonth'
 *@description  [NonRecurringEndMonth] Ending month for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringEndMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_END_MONTH, (void *)&objEndMonthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndMonthValue, sizeof (objEndMonthValue));

  /* return the object value: EndMonth */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndMonthValue, sizeof (objEndMonthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringEndMonth
*
* @purpose Set 'NonRecurringEndMonth'
 *@description  [NonRecurringEndMonth] Ending month for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringEndMonth (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndMonthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndMonth */
  owa.len = sizeof (objEndMonthValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndMonthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndMonthValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndDate
*
* @purpose Get 'NonRecurringEndDate'
 *@description  [NonRecurringEndDate] ending date for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringEndDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndDateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_END_DAY, (void *)&objEndDateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndDateValue, sizeof (objEndDateValue));

  /* return the object value: EndDate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndDateValue, sizeof (objEndDateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndDate
*
* @purpose Get 'NonRecurringEndDate'
 *@description  [NonRecurringEndDate] ending date for the summertime support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringEndDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndDateValue;

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof(objEndDateValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndDate,
                          (xLibU8_t *) &objEndDateValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objEndDateValue = 1;
  }
  else
  {
    objEndDateValue++;
  }
  if (objEndDateValue > 31)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objEndDateValue, sizeof (objEndDateValue));

  /* return the object value: EndDate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndDateValue, sizeof (objEndDateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringEndDate
*
* @purpose Set 'NonRecurringEndDate'
 *@description  [NonRecurringEndDate] ending date for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringEndDate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndDateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndDate */
  owa.len = sizeof (objEndDateValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndDateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndDateValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndYear
*
* @purpose Get 'NonRecurringEndYear'
 *@description  [NonRecurringEndYear] Ending year for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringEndYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndYearValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_END_YEAR, (void *)&objEndYearValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndYearValue, sizeof (objEndYearValue));

  /* return the object value: EndYear */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndYearValue, sizeof (objEndYearValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndYear
*
* @purpose Get 'NonRecurringEndYear'
 *@description  [NonRecurringEndYear] Ending year for the summertime support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringEndYear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndYearValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(objEndYearValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndYear,
                          (xLibU8_t *) &objEndYearValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objEndYearValue = L7_START_SUMMER_TIME_YEAR;
  }
  else
  {
    objEndYearValue++;
  }
  if (objEndYearValue > L7_END_SUMMER_TIME_YEAR)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objEndYearValue, sizeof (objEndYearValue));

  /* return the object value: EndYear */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndYearValue, sizeof (objEndYearValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringEndYear
*
* @purpose Set 'NonRecurringEndYear'
 *@description  [NonRecurringEndYear] Ending year for the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringEndYear (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndYearValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartYear */
  owa.len = sizeof (objEndYearValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndYearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndYearValue, owa.len);

   /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */

}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeHours
*
* @purpose Get 'NonRecurringStartTimeHours'
 *@description  [NonRecurringStartTimeHours] Hours of the starting time for the NonRecurring summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_START_HOUR, (void *)&objStartTimeHoursValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartTimeHoursValue, sizeof (objStartTimeHoursValue));

  /* return the object value: StartTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeHoursValue,
                           sizeof (objStartTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeHours
*
* @purpose Get 'NonRecurringStartTimeHours'
 *@description  [NonRecurringStartTimeHours] Hours of the starting time for the NonRecurring summertime
* support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(objStartTimeHoursValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartTimeHours,
                          (xLibU8_t *) &objStartTimeHoursValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objStartTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else
  {
    objStartTimeHoursValue++;
  }
  if (objStartTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartTimeHoursValue, sizeof (objStartTimeHoursValue));

  /* return the object value: StartTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeHoursValue,
                           sizeof (objStartTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeHours
*
* @purpose Get 'NonRecurringStartTimeHours'
 *@description  [NonRecurringStartTimeHours] Hours of the starting time for the NonRecurring summertime
* support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_RecurringStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(objStartTimeHoursValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringStartTimeHours,
                          (xLibU8_t *) &objStartTimeHoursValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objStartTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else
  {
    objStartTimeHoursValue++;
  }
  if (objStartTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartTimeHoursValue, sizeof (objStartTimeHoursValue));

  /* return the object value: StartTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeHoursValue,
                           sizeof (objStartTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringStartTimeHours
*
* @purpose Set 'NonRecurringStartTimeHours'
 *@description  [NonRecurringStartTimeHours] Hours of the starting time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringStartTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartTimeHours */
  owa.len = sizeof (objStartTimeHoursValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartTimeHoursValue, owa.len);

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeMinutes
*
* @purpose Get 'NonRecurringStartTimeMinutes'
 *@description  [NonRecurringStartTimeMinutes] Minutes of the starting time for the
* summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_START_MINUTE, (void *)&objStartTimeMinutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStartTimeMinutesValue, sizeof (objStartTimeMinutesValue));

  /* return the object value: StartTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeMinutesValue,
                           sizeof (objStartTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeMinutes
*
* @purpose Get 'NonRecurringStartTimeMinutes'
 *@description  [NonRecurringStartTimeMinutes] Minutes of the starting time for the
* summertime support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof(objStartTimeMinutesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartTimeMinutes,
                          (xLibU8_t *) &objStartTimeMinutesValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objStartTimeMinutesValue =L7_TIMEZONE_MINS_MIN;
  }
  else
  {
    objStartTimeMinutesValue++;
  }
  if (objStartTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objStartTimeMinutesValue, sizeof (objStartTimeMinutesValue));

  /* return the object value: StartTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeMinutesValue,
                           sizeof (objStartTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringStartTimeMinutes
*
* @purpose Get 'NonRecurringStartTimeMinutes'
 *@description  [NonRecurringStartTimeMinutes] Minutes of the starting time for the
* summertime support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_RecurringStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);
  owa.len = sizeof(objStartTimeMinutesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringStartTimeMinutes,
                          (xLibU8_t *) &objStartTimeMinutesValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objStartTimeMinutesValue =L7_TIMEZONE_MINS_MIN;
  }
  else
  {
    objStartTimeMinutesValue++;
  }
  if (objStartTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objStartTimeMinutesValue, sizeof (objStartTimeMinutesValue));

  /* return the object value: StartTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartTimeMinutesValue,
                           sizeof (objStartTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringStartTimeMinutes
*
* @purpose Set 'NonRecurringStartTimeMinutes'
 *@description  [NonRecurringStartTimeMinutes] Minutes of the starting time for the
* summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringStartTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartTimeMinutes */
  owa.len = sizeof (objStartTimeMinutesValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartTimeMinutesValue, owa.len);

  /* if row status object is specified and eual to delete return success */

 /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeHours
*
* @purpose Get 'NonRecurringEndTimeHours'
 *@description  [NonRecurringEndTimeHours] Hours of the ending time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_END_HOUR, (void *)&objEndTimeHoursValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndTimeHoursValue, sizeof (objEndTimeHoursValue));

  /* return the object value: EndTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeHoursValue,
                           sizeof (objEndTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeHours
*
* @purpose Get 'NonRecurringEndTimeHours'
 *@description  [NonRecurringEndTimeHours] Hours of the ending time for the summertime
* support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);
   owa.len = sizeof(objEndTimeHoursValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndTimeHours,
                          (xLibU8_t *) &objEndTimeHoursValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objEndTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else
  {
    objEndTimeHoursValue++;
  }
  if (objEndTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objEndTimeHoursValue, sizeof (objEndTimeHoursValue));

  /* return the object value: EndTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeHoursValue,
                           sizeof (objEndTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeHours
*
* @purpose Get 'NonRecurringEndTimeHours'
 *@description  [NonRecurringEndTimeHours] Hours of the ending time for the summertime
* support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_RecurringEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);
   owa.len = sizeof(objEndTimeHoursValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringEndTimeHours,
                          (xLibU8_t *) &objEndTimeHoursValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objEndTimeHoursValue = L7_TIMEZONE_UHOURS_MIN;
  }
  else
  {
    objEndTimeHoursValue++;
  }
  if (objEndTimeHoursValue > L7_TIMEZONE_UHOURS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objEndTimeHoursValue, sizeof (objEndTimeHoursValue));

  /* return the object value: EndTimeHours */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeHoursValue,
                           sizeof (objEndTimeHoursValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringEndTimeHours
*
* @purpose Set 'NonRecurringEndTimeHours'
 *@description  [NonRecurringEndTimeHours] Hours of the ending time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringEndTimeHours (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeHoursValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndTimeHours */
  owa.len = sizeof (objEndTimeHoursValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndTimeHoursValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndTimeHoursValue, owa.len);

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeMinutes
*
* @purpose Get 'NonRecurringEndTimeMinutes'
 *@description  [NonRecurringEndTimeMinutes] Minutes of the ending time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_END_MINUTE, (void *)&objEndTimeMinutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objEndTimeMinutesValue, sizeof (objEndTimeMinutesValue));

  /* return the object value: EndTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeMinutesValue,
                           sizeof (objEndTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeMinutes
*
* @purpose Get 'NonRecurringEndTimeMinutes'
 *@description  [NonRecurringEndTimeMinutes] Minutes of the ending time for the summertime
* support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_NonRecurringEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(objEndTimeMinutesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndTimeMinutes,
                          (xLibU8_t *) &objEndTimeMinutesValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objEndTimeMinutesValue =L7_TIMEZONE_MINS_MIN;
  }
  else
  {
    objEndTimeMinutesValue++;
  }
  if (objEndTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objEndTimeMinutesValue, sizeof (objEndTimeMinutesValue));

  /* return the object value: EndTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeMinutesValue,
                           sizeof (objEndTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringEndTimeMinutes
*
* @purpose Get 'NonRecurringEndTimeMinutes'
 *@description  [NonRecurringEndTimeMinutes] Minutes of the ending time for the summertime
* support.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_baseSummerTimeSupport_RecurringEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(objEndTimeMinutesValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringEndTimeMinutes,
                          (xLibU8_t *) &objEndTimeMinutesValue , &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    objEndTimeMinutesValue =L7_TIMEZONE_MINS_MIN;
  }
  else
  {
    objEndTimeMinutesValue++;
  }
  if (objEndTimeMinutesValue > L7_TIMEZONE_MINS_MAX)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_VALUE (bufp, &objEndTimeMinutesValue, sizeof (objEndTimeMinutesValue));

  /* return the object value: EndTimeMinutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndTimeMinutesValue,
                           sizeof (objEndTimeMinutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringEndTimeMinutes
*
* @purpose Set 'NonRecurringEndTimeMinutes'
 *@description  [NonRecurringEndTimeMinutes] Minutes of the ending time for the summertime
* support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringEndTimeMinutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEndTimeMinutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: EndTimeMinutes */
  owa.len = sizeof (objEndTimeMinutesValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEndTimeMinutesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEndTimeMinutesValue, owa.len);

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringTimeOffSet
*
* @purpose Get 'NonRecurringTimeOffSet'
 *@description  [NonRecurringTimeOffSet] Number of minutes to add during summer time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringTimeOffSet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTimeOffSetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_OFFSET_MINUTES, (void *)&objTimeOffSetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTimeOffSetValue, sizeof (objTimeOffSetValue));

  /* return the object value: TimeOffSet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeOffSetValue, sizeof (objTimeOffSetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringTimeOffSet
*
* @purpose Set 'NonRecurringTimeOffSet'
 *@description  [NonRecurringTimeOffSet] Number of minutes to add during summer time.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringTimeOffSet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTimeOffSetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TimeOffSet */
  owa.len = sizeof (objTimeOffSetValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTimeOffSetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTimeOffSetValue, owa.len);

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_NonRecurringZone
*
* @purpose Get 'NonRecurringZone'
 *@description  [NonRecurringZone] The acronym of the time zone to be used when summer time
* is in effect.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_NonRecurringZone (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objZoneValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
 owa.l7rc = usmDbStNonRecurringGet (L7_UNIT_CURRENT,  L7_ZONE_ACRONYM,  (void *) objZoneValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objZoneValue, strlen (objZoneValue));

  /* return the object value: Zone */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objZoneValue, strlen (objZoneValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_NonRecurringZone
*
* @purpose Set 'NonRecurringZone'
 *@description  [NonRecurringZone] The acronym of the time zone to be used when summer time
* is in effect.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_NonRecurringZone (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objZoneValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Zone */
  owa.len = sizeof (objZoneValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objZoneValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objZoneValue, owa.len);

  /* set the value in application */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS; /* value being set in Status object */
	
}


/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupport_Status
*
* @purpose Get 'Status'
 *@description  [Status] Status of the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupport_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStatusValue;
  xLibU32_t objTempStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbStModeGet (L7_UNIT_CURRENT, &objTempStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch(objTempStatusValue)
  {
		case L7_XUI_NO_SUMMER_TIME:
			objStatusValue = L7_XUI_NO_SUMMER_TIME;
			break;
		case L7_XUI_SUMMER_TIME_RECURRING:
			objStatusValue = L7_SUMMER_TIME_RECURRING;
			break;
		case L7_XUI_SUMMER_TIME_RECURRING_EU:
			objStatusValue = L7_SUMMER_TIME_RECURRING_EU;
			break;
		case L7_XUI_SUMMER_TIME_RECURRING_USA:
			objStatusValue = L7_SUMMER_TIME_RECURRING_USA;
			break;
		case L7_XUI_SUMMER_TIME_NON_RECURRING:
			objStatusValue = L7_SUMMER_TIME_NON_RECURRING;
			break;
		default:
		   owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, sizeof (objStatusValue));

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue, sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSummerTimeSupport_Status
*
* @purpose Set 'Status'
 *@description  [Status] Status of the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupport_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStatusValue;

  xLibU32_t objStartWeekValue;
  xLibU32_t objStartDayValue;
  xLibU32_t objStartMonthValue;

  xLibU32_t objEndWeekValue;
  xLibU32_t objEndDayValue;
  xLibU32_t objEndMonthValue;

  xLibU32_t objStartTimeHoursValue;
  xLibU32_t objStartTimeMinutesValue;

  xLibU32_t objEndTimeHoursValue;
  xLibU32_t objEndTimeMinutesValue;

  xLibU32_t objTimeOffSetValue;
  xLibStr256_t objZoneValue;

  xLibU32_t objNonRecuringStartMonthValue;
  xLibU32_t objNonRecuringStartDateValue;
  xLibU32_t objNonRecuringStartYearValue;

  xLibU32_t objNonRecuringEndMonthValue;
  xLibU32_t objNonRecuringEndDateValue;
  xLibU32_t objNonRecuringEndYearValue;

   xLibU32_t objNonRecuringStartTimeHoursValue;
  xLibU32_t objNonRecuringStartTimeMinutesValue;

  xLibU32_t objNonRecuringEndTimeHoursValue;
  xLibU32_t objNonRecuringEndTimeMinutesValue;

  xLibU32_t objNonRecuringTimeOffSetValue;
  xLibStr256_t objNonRecuringZoneValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.len = sizeof (objStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);
	
 switch(objStatusValue)
 {
    case L7_XUI_NO_SUMMER_TIME:
		    owa.l7rc = usmDbStModeSet(L7_UNIT_CURRENT, L7_NO_SUMMER_TIME);
			/*Set the time objects to default and set the status to disable */
			break;
	case L7_XUI_SUMMER_TIME_RECURRING:
		   /*Pull in the required objects and set them to user defined values */

			owa.len = sizeof (objStartWeekValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringStartWeek,
			                        (xLibU8_t *) & objStartWeekValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objStartDayValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringStartDay,
			                        (xLibU8_t *) & objStartDayValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objStartMonthValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringStartMonth,
			                        (xLibU8_t *) & objStartMonthValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objEndWeekValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringEndWeek,
			                        (xLibU8_t *) & objEndWeekValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objEndDayValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringEndDay,
			                        (xLibU8_t *) & objEndDayValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objEndMonthValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringEndMonth,
			                        (xLibU8_t *) & objEndMonthValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}
			owa.len = sizeof (objStartTimeHoursValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringStartTimeHours,
			                        (xLibU8_t *) & objStartTimeHoursValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objStartTimeMinutesValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringStartTimeMinutes,
			                        (xLibU8_t *) & objStartTimeMinutesValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objEndTimeHoursValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringEndTimeHours,
			                        (xLibU8_t *) & objEndTimeHoursValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objEndTimeMinutesValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringEndTimeMinutes,
			                        (xLibU8_t *) & objEndTimeMinutesValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objTimeOffSetValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringTimeOffSet,
			                        (xLibU8_t *) & objTimeOffSetValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objZoneValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringZone,
			                        (xLibU8_t *)  objZoneValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			if (fpObjUtil__ValidateSummerTime(objStartMonthValue, objStartDayValue, 0, objStartWeekValue, 
				        objEndMonthValue, objEndDayValue, 0, objEndWeekValue, L7_TRUE) != L7_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

		    if ((usmDbStModeSet(L7_UNIT_CURRENT, L7_SUMMER_TIME_RECURRING) != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_MONTH, &objStartMonthValue)      != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_DAY, &objStartDayValue)          != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_WEEK, &objStartWeekValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_HOUR, &objStartTimeHoursValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_MINUTE, &objStartTimeMinutesValue)       != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_MONTH, &objEndMonthValue)       != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_DAY, &objEndDayValue)           != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_WEEK, &objEndWeekValue)         != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_HOUR, &objEndTimeHoursValue)         != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_MINUTE, &objEndTimeMinutesValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &objTimeOffSetValue) != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_ZONE_ACRONYM, objZoneValue)     != L7_SUCCESS))
			{
				owa.l7rc = L7_FAILURE;
				break;
			}
			 
			break;
	case L7_XUI_SUMMER_TIME_RECURRING_EU:
		   /*set the values to default */
			objStartMonthValue  = L7_SUMMER_TIME_EU_START_MONTH;
			objStartDayValue    = L7_SUMMER_TIME_DAY;
			objStartWeekValue   = L7_SUMMER_TIME_EU_START_WEEK;
			objStartTimeHoursValue   = L7_SUMMERTIME_EU_START_HOUR;
			objStartTimeMinutesValue    = 0;
			objEndMonthValue = L7_SUMMER_TIME_EU_END_MONTH;
			objEndDayValue   = L7_SUMMER_TIME_DAY;
			objEndWeekValue  = L7_SUMMER_TIME_EU_END_WEEK;
			objEndTimeHoursValue  = L7_SUMMERTIME_EU_END_HOUR;
			objEndTimeMinutesValue   = 0;

			owa.len = sizeof (objTimeOffSetValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringTimeOffSet,
			                        (xLibU8_t *) & objTimeOffSetValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objZoneValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringZone,
			                        (xLibU8_t *)  objZoneValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}
			
			if ((usmDbStModeSet(L7_UNIT_CURRENT, L7_SUMMER_TIME_RECURRING_EU) != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_MONTH, &objStartMonthValue)      != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_DAY, &objStartDayValue)          != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_WEEK, &objStartWeekValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_HOUR, &objStartTimeHoursValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_MINUTE, &objStartTimeMinutesValue)       != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_MONTH, &objEndMonthValue)       != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_DAY, &objEndDayValue)           != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_WEEK, &objEndWeekValue)         != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_HOUR, &objEndTimeHoursValue)         != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_MINUTE, &objEndTimeMinutesValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &objTimeOffSetValue) != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_ZONE_ACRONYM, objZoneValue)     != L7_SUCCESS))
			{
				owa.l7rc = L7_FAILURE;
				break;
			}
			
			break;
	case L7_XUI_SUMMER_TIME_RECURRING_USA:
		    /*set the values to default */
			objStartMonthValue  = L7_SUMMER_TIME_USA_START_MONTH;
			objStartDayValue    = L7_SUMMER_TIME_DAY;
			objStartWeekValue   = L7_SUMMER_TIME_USA_START_WEEK;
			objStartTimeHoursValue   = L7_SUMMERTIME_USA_HOUR;
			objStartTimeMinutesValue    = 0;
			objEndMonthValue = L7_SUMMER_TIME_USA_END_MONTH;
			objEndDayValue   = L7_SUMMER_TIME_DAY;
			objEndWeekValue  = L7_SUMMER_TIME_USA_END_WEEK;
			objEndTimeHoursValue  = L7_SUMMERTIME_USA_HOUR;
			objEndTimeMinutesValue   = 0;

			owa.len = sizeof (objTimeOffSetValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringTimeOffSet,
			                        (xLibU8_t *) & objTimeOffSetValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objZoneValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_RecurringZone,
			                        (xLibU8_t *)  objZoneValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			if ((usmDbStModeSet(L7_UNIT_CURRENT, L7_SUMMER_TIME_RECURRING_USA) != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_MONTH, &objStartMonthValue)      != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_DAY, &objStartDayValue)          != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_WEEK, &objStartWeekValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_HOUR, &objStartTimeHoursValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_START_MINUTE, &objStartTimeMinutesValue)       != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_MONTH, &objEndMonthValue)       != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_DAY, &objEndDayValue)           != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_WEEK, &objEndWeekValue)         != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_HOUR, &objEndTimeHoursValue)         != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_END_MINUTE, &objEndTimeMinutesValue)        != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &objTimeOffSetValue) != L7_SUCCESS) ||
				(usmDbStRecurringSet(L7_UNIT_CURRENT, L7_ZONE_ACRONYM, objZoneValue)     != L7_SUCCESS))
			{
				owa.l7rc = L7_FAILURE;
				break;
			}
			break;
	case L7_XUI_SUMMER_TIME_NON_RECURRING:
           /*Pull in the required objects and set them to user defined values */
					 

	
			owa.len = sizeof (objNonRecuringStartMonthValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartMonth,
			                        (xLibU8_t *) & objNonRecuringStartMonthValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringStartDateValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartDate,
			                        (xLibU8_t *) & objNonRecuringStartDateValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringStartYearValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartYear,
			                        (xLibU8_t *) & objNonRecuringStartYearValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringEndMonthValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndMonth,
			                        (xLibU8_t *) & objNonRecuringEndMonthValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringEndDateValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndDate,
			                        (xLibU8_t *) & objNonRecuringEndDateValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringEndYearValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndYear,
			                        (xLibU8_t *) & objNonRecuringEndYearValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}
			owa.len = sizeof (objNonRecuringStartTimeHoursValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartTimeHours,
			                        (xLibU8_t *) & objNonRecuringStartTimeHoursValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringStartTimeMinutesValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringStartTimeMinutes,
			                        (xLibU8_t *) & objNonRecuringStartTimeMinutesValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringEndTimeHoursValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndTimeHours,
			                        (xLibU8_t *) & objNonRecuringEndTimeHoursValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringEndTimeMinutesValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringEndTimeMinutes,
			                        (xLibU8_t *) & objNonRecuringEndTimeMinutesValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringTimeOffSetValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringTimeOffSet,
			                        (xLibU8_t *) & objNonRecuringTimeOffSetValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			owa.len = sizeof (objNonRecuringZoneValue);
			owa.rc = xLibFilterGet (wap, XOBJ_baseSummerTimeSupport_NonRecurringZone,
			                        (xLibU8_t *)  objNonRecuringZoneValue, &owa.len);
			if (owa.rc != XLIBRC_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

			if (fpObjUtil__ValidateSummerTime(
				    objNonRecuringStartMonthValue, objNonRecuringStartDateValue, objNonRecuringStartYearValue, 0, 
				    objNonRecuringEndMonthValue, objNonRecuringEndDateValue, objNonRecuringEndYearValue, 0, L7_FALSE) != L7_SUCCESS)
			{
			  owa.l7rc = L7_FAILURE;
			  break;
			}

		    if ((usmDbStModeSet(L7_UNIT_CURRENT, L7_SUMMER_TIME_NON_RECURRING)!=L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_START_MONTH, &objNonRecuringStartMonthValue)   != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_START_DAY, &objNonRecuringStartDateValue)       != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_START_YEAR, &objNonRecuringStartYearValue)     != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_START_HOUR, &objNonRecuringStartTimeHoursValue)     != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_START_MINUTE, &objNonRecuringStartTimeMinutesValue)    != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_END_MONTH, &objNonRecuringEndMonthValue)    != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_END_DAY, &objNonRecuringEndDateValue)        != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_END_YEAR, &objNonRecuringEndYearValue)      != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_END_HOUR, &objNonRecuringEndTimeHoursValue)      != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_END_MINUTE, &objNonRecuringEndTimeMinutesValue)     != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &objNonRecuringTimeOffSetValue) != L7_SUCCESS) ||
		        (usmDbStNonRecurringSet(L7_UNIT_CURRENT, L7_ZONE_ACRONYM, objNonRecuringZoneValue)  != L7_SUCCESS))
		    {
					  owa.l7rc = L7_FAILURE;
					  break;
		    }			
			break;
	default:
	       break;			
 }

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

