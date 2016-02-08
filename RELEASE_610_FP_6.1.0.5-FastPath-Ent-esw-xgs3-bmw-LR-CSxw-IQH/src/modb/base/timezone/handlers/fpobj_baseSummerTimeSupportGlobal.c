
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSummerTimeSupportGlobal.c
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
#include "_xe_baseSummerTimeSupportGlobal_obj.h"

/*******************************************************************************
* @function fpObjGet_baseSummerTimeSupportGlobal_Status
*
* @purpose Get 'Status'
 *@description  [Status] Status of the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSummerTimeSupportGlobal_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  /*owa.l7rc = usmDbStFlagGet (L7_UNIT_CURRENT, &objStatusValue);*/
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
* @function fpObjSet_baseSummerTimeSupportGlobal_Status
*
* @purpose Set 'Status'
 *@description  [Status] Status of the summertime support.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSummerTimeSupportGlobal_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStatusValue;

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

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbStModeSet (L7_UNIT_CURRENT, objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


