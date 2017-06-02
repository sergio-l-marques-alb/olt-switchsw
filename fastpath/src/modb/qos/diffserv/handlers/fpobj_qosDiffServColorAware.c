/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServColorAware.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to diffserv-object.xml
*
* @create  2 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_qosDiffServColorAware_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosDiffServColorAware_diffServMeterId
*
* @purpose Get 'diffServMeterId'
*
* @description [diffServMeterId]: An index that enumerates the Meter entries.
*              Managers obtain new values for row creation in this table
*              by reading diffServMeterNextFree. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServColorAware_diffServMeterId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServMeterIdValue;
  xLibU32_t nextObjdiffServMeterIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServColorAware_diffServMeterId,
                          (xLibU8_t *) & objdiffServMeterIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objdiffServMeterIdValue = 0;
    nextObjdiffServMeterIdValue = 0;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServMeterIdValue, owa.len);
  owa.l7rc =
      usmDbDiffServMeterGetNext (L7_UNIT_CURRENT, objdiffServMeterIdValue,
                                 &nextObjdiffServMeterIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdiffServMeterIdValue, owa.len);

  /* return the object value: diffServMeterId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdiffServMeterIdValue,
                           sizeof (objdiffServMeterIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServColorAware_Level
*
* @purpose Get 'Level'
*
* @description [Level]: The metering conformance level of this table entry.
*              A policing meter represents either a conforming or an exceeding
*              level of traffic in the traffic stream for this data
*              path. The color characteristics used by the meter are relevant
*              to this 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServColorAware_Level (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLevelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServColorAware_diffServMeterId,
                          (xLibU8_t *) & keydiffServMeterIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMeterIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServColorAwareLevelGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
                                     &objLevelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Level */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLevelValue,
                           sizeof (objLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServColorAware_Mode
*
* @purpose Get 'Mode'
*
* @description [Mode]: The color mode indicator for the meter. When set to
*              blind(1), metering is performed in a color-blind manner such
*              that any packet markings are ignored by the meter. The other
*              values listed for this object indicate the packet field
*              used f 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServColorAware_Mode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServColorAware_diffServMeterId,
                          (xLibU8_t *) & keydiffServMeterIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMeterIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServColorAwareModeGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
                                    &objModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Mode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objModeValue,
                           sizeof (objModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServColorAware_Value
*
* @purpose Get 'Value'
*
* @description [Value]: The value of the color aware table entry. This value
*              is used to compare the field indicated in agentDiffServColorAwareMode
*              against packets in a traffic stream along the
*              data path supplying the meter for a given conformance level.
*              This obj 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServColorAware_Value (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServColorAware_diffServMeterId,
                          (xLibU8_t *) & keydiffServMeterIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMeterIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServColorAwareValueGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
                                     &objValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Value */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objValueValue,
                           sizeof (objValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
