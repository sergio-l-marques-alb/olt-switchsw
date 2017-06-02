
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseTimeZone.c
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
#include "_xe_baseTimeZone_obj.h"
#include "timezone_exports.h"
#include "usmdb_timezone_api.h"
#include "stdlib.h"

/*******************************************************************************
* @function fpObjGet_baseTimeZone_hoursOffset
*
* @purpose Get 'hoursOffset'
 *@description  [hoursOffset]  Hours offset from timezone.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeZone_hoursOffset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS32_t objhoursOffsetValue;
  xLibS32_t objminOffsetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTimeZoneOffsetGet(L7_UNIT_CURRENT, &objminOffsetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  /* don't modify tzMinutes so it may be displayed later */
  objhoursOffsetValue = objminOffsetValue / L7_MINS_PER_HOUR;
  FPOBJ_TRACE_VALUE (bufp, &objhoursOffsetValue, sizeof (objhoursOffsetValue));

  /* return the object value: hoursOffset */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhoursOffsetValue, sizeof (objhoursOffsetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTimeZone_hoursAddOffset
*
* @purpose Get 'hoursOffset'
 *@description  [hoursOffset]  Hours offset from timezone.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeZone_hoursAddOffset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS32_t objhoursOffsetValue;
  xLibS32_t objminOffsetValue;
  xLibS32_t  mode,offset=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTimeZoneOffsetGet(L7_UNIT_CURRENT, &objminOffsetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbStModeGet(USMDB_UNIT_CURRENT, &mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_RECURRING)
  {
   usmDbStRecurringGet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &offset); 
  }
  else if (usmDbStModeGet(USMDB_UNIT_CURRENT, &mode) == L7_SUCCESS && mode == L7_SUMMER_TIME_NON_RECURRING)
  {
    usmDbStNonRecurringGet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &offset);
  }  
  objminOffsetValue= offset + objminOffsetValue;
  /* don't modify tzMinutes so it may be displayed later */
  objhoursOffsetValue = objminOffsetValue / L7_MINS_PER_HOUR;
  FPOBJ_TRACE_VALUE (bufp, &objhoursOffsetValue, sizeof (objhoursOffsetValue));

  /* return the object value: hoursOffset */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhoursOffsetValue, sizeof (objhoursOffsetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseTimeZone_hoursOffset
*
* @purpose Set 'hoursOffset'
 *@description  [hoursOffset]  Hours offset from timezone.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeZone_hoursOffset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS32_t objhoursOffsetValue;
  xLibS32_t objminutesOffsetValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objminutesOffsetValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseTimeZone_MinutesOffset,
                          (xLibU8_t *) & objminutesOffsetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objminutesOffsetValue, owa.len);

  /* retrieve object: hoursOffset */
  owa.len = sizeof (objhoursOffsetValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objhoursOffsetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objhoursOffsetValue, owa.len);

  /* set the value in application */
 owa.l7rc = usmDbTimeZoneOffsetSet(L7_UNIT_CURRENT, 
    (objhoursOffsetValue * L7_MINS_PER_HOUR) + (objhoursOffsetValue < 0 ? 0-objminutesOffsetValue : objminutesOffsetValue));
  if (owa.l7rc != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;   
  }
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTimeZone_MinutesOffset
*
* @purpose Get 'MinutesOffset'
 *@description  [MinutesOffset] Minutes offset for timezone   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeZone_MinutesOffset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMinutesOffsetValue;
  xLibU32_t tempMinutesOffsetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTimeZoneOffsetGet(L7_UNIT_CURRENT, &tempMinutesOffsetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objMinutesOffsetValue = abs(tempMinutesOffsetValue) % L7_MINS_PER_HOUR;
  FPOBJ_TRACE_VALUE (bufp, &objMinutesOffsetValue, sizeof (objMinutesOffsetValue));

  /* return the object value: MinutesOffset */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMinutesOffsetValue,
                           sizeof (objMinutesOffsetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTimeZone_MinutesAddOffset
*
* @purpose Get 'MinutesOffset'
 *@description  [MinutesOffset] Minutes offset for timezone
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeZone_MinutesAddOffset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMinutesOffsetValue;
  xLibU32_t tempMinutesOffsetValue;
  xLibS32_t  mode,offset=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTimeZoneOffsetGet(L7_UNIT_CURRENT, &tempMinutesOffsetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (usmDbStModeGet(USMDB_UNIT_CURRENT, &mode)==L7_SUCCESS && mode==L7_SUMMER_TIME_RECURRING)
  {
   usmDbStRecurringGet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &offset);
  }
  else if (usmDbStModeGet(USMDB_UNIT_CURRENT, &mode) == L7_SUCCESS && mode == L7_SUMMER_TIME_NON_RECURRING)
  {
    usmDbStNonRecurringGet(L7_UNIT_CURRENT, L7_OFFSET_MINUTES, &offset);
  }

  if (usmDbSummerTimeIsInEffect() == L7_TRUE)
  {
    tempMinutesOffsetValue = offset + tempMinutesOffsetValue;
  }

  objMinutesOffsetValue = abs(tempMinutesOffsetValue) % L7_MINS_PER_HOUR;
  FPOBJ_TRACE_VALUE (bufp, &objMinutesOffsetValue, sizeof (objMinutesOffsetValue));

  /* return the object value: MinutesOffset */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMinutesOffsetValue,
                           sizeof (objMinutesOffsetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTimeZone_MinutesOffset
*
* @purpose Set 'MinutesOffset'
 *@description  [MinutesOffset] Minutes offset for timezone   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeZone_MinutesOffset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMinutesOffsetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MinutesOffset */
  owa.len = sizeof (objMinutesOffsetValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMinutesOffsetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMinutesOffsetValue, owa.len);

  owa.rc = XLIBRC_SUCCESS; /*The data is being set in the hoursOffset object */


  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTimeZone_Acronym
*
* @purpose Get 'Acronym'
 *@description  [Acronym] Acronym for the timezone.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeZone_Acronym (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAcronymValue;
  memset(objAcronymValue, 0x00, sizeof(objAcronymValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
	
  owa.l7rc = usmDbTimeZoneAcronymGet (L7_UNIT_CURRENT,  objAcronymValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objAcronymValue, strlen (objAcronymValue));

  /* return the object value: Acronym */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAcronymValue, strlen (objAcronymValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTimeZone_Acronym
*
* @purpose Set 'Acronym'
 *@description  [Acronym] Acronym for the timezone.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTimeZone_Acronym (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAcronymValue;
  memset(objAcronymValue, 0x00, sizeof(objAcronymValue));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Acronym */
  owa.len = sizeof (objAcronymValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAcronymValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAcronymValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbTimeZoneAcronymSet (L7_UNIT_CURRENT,  objAcronymValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;   
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseTimeZone_SummerTimeIsInEffect
*
* @purpose Get 'SummerTimeIsInEffect'
*
* @description
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTimeZone_SummerTimeIsInEffect (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSummerTimeIsInEffectValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  objSummerTimeIsInEffectValue = usmDbSummerTimeIsInEffect();
  FPOBJ_TRACE_VALUE (bufp, &objSummerTimeIsInEffectValue, sizeof (objSummerTimeIsInEffectValue));

  /* return the object value: NsfRestartInProgress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSummerTimeIsInEffectValue,
                           sizeof (objSummerTimeIsInEffectValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

