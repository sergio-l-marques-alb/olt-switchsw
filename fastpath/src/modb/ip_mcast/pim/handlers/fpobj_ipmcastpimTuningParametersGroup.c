
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimTuningParametersGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimTuningParametersGroup_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimTuningParametersGroup_pimKeepalivePeriod
*
* @purpose Get 'pimKeepalivePeriod'
*
* @description [pimKeepalivePeriod] The duration of the Keepalive Timer.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimTuningParametersGroup_pimKeepalivePeriod (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimKeepalivePeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimKeepalivePeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimKeepalivePeriodValue, sizeof (objpimKeepalivePeriodValue));

  /* return the object value: pimKeepalivePeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimKeepalivePeriodValue,
                           sizeof (objpimKeepalivePeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimTuningParametersGroup_pimKeepalivePeriod
*
* @purpose Set 'pimKeepalivePeriod'
*
* @description [pimKeepalivePeriod] The duration of the Keepalive Timer.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimTuningParametersGroup_pimKeepalivePeriod (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimKeepalivePeriodValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimKeepalivePeriod */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimKeepalivePeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimKeepalivePeriodValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimKeepalivePeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimTuningParametersGroup_pimRegisterSuppressionTime
*
* @purpose Get 'pimRegisterSuppressionTime'
*
* @description [pimRegisterSuppressionTime] The duration of the Register Suppression Timer.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimTuningParametersGroup_pimRegisterSuppressionTime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimRegisterSuppressionTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimRegisterSuppressionTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimRegisterSuppressionTimeValue,
                     sizeof (objpimRegisterSuppressionTimeValue));

  /* return the object value: pimRegisterSuppressionTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimRegisterSuppressionTimeValue,
                           sizeof (objpimRegisterSuppressionTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimTuningParametersGroup_pimRegisterSuppressionTime
*
* @purpose Set 'pimRegisterSuppressionTime'
*
* @description [pimRegisterSuppressionTime] The duration of the Register Suppression Timer.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimTuningParametersGroup_pimRegisterSuppressionTime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimRegisterSuppressionTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimRegisterSuppressionTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimRegisterSuppressionTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimRegisterSuppressionTimeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimRegisterSuppressionTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
