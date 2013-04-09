/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServTBParam.c
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
#include "_xe_qosdiffServTBParam_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosdiffServTBParam_diffServTBParamId
*
* @purpose Get 'diffServTBParamId'
*
* @description [diffServTBParamId]: An index that enumerates the Token Bucket
*              Parameter entries. Managers obtain new values for row creation
*              in this table by reading diffServTBParamNextFree. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServTBParam_diffServTBParamId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServTBParamIdValue;
  xLibU32_t nextObjdiffServTBParamIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServTBParamId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServTBParam_diffServTBParamId,
                          (xLibU8_t *) & objdiffServTBParamIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	
    objdiffServTBParamIdValue = 0;
    nextObjdiffServTBParamIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServTBParamIdValue, owa.len);
  owa.l7rc =
      usmDbDiffServTBParamGetNext (L7_UNIT_CURRENT, objdiffServTBParamIdValue,
                                   &nextObjdiffServTBParamIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdiffServTBParamIdValue, owa.len);

  /* return the object value: diffServTBParamId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdiffServTBParamIdValue,
                           sizeof (objdiffServTBParamIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServTBParam_Type
*
* @purpose Get 'Type'
*
* @description [Type]: The Metering algorithm associated with the Token Bucket
*              parameters. zeroDotZero indicates this is unknown. Standard
*              values for generic algorithms: diffServTBParamSimpleTokenBucket,
*              diffServTBParamAvgRate, diffServTBParamSrTCMBlind
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServTBParam_Type (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServTBParamIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTypeValue;
  L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_t tempTbMeterType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServTBParamId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServTBParam_diffServTBParamId,
                          (xLibU8_t *) & keydiffServTBParamIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServTBParamIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServTBParamTypeGet (L7_UNIT_CURRENT, keydiffServTBParamIdValue,
                                 &tempTbMeterType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTypeValue,
                           strlen (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServTBParam_Rate
*
* @purpose Get 'Rate'
*
* @description [Rate]: The token-bucket rate, in kilobits per second (kbps).
*              This attribute is used for: 1. CIR in RFC 2697 for srTCM
*              2. CIR and PIR in RFC 2698 for trTCM 3. CTR and PTR in RFC
*              2859 for TSWTCM 4. AverageRate in RFC 3290. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServTBParam_Rate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServTBParamIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServTBParamId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServTBParam_diffServTBParamId,
                          (xLibU8_t *) & keydiffServTBParamIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServTBParamIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServTBParamRateGet (L7_UNIT_CURRENT, keydiffServTBParamIdValue,
                                 &objRateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Rate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRateValue,
                           sizeof (objRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServTBParam_BurstSize
*
* @purpose Get 'BurstSize'
*
* @description [BurstSize]: The maximum number of bytes in a single transmission
*              burst. This attribute is used for: 1. CBS and EBS in
*              RFC 2697 for srTCM 2. CBS and PBS in RFC 2698 for trTCM 3.
*              Burst Size in RFC 3290. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServTBParam_BurstSize (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServTBParamIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBurstSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServTBParamId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServTBParam_diffServTBParamId,
                          (xLibU8_t *) & keydiffServTBParamIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServTBParamIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServTBParamBurstSizeGet (L7_UNIT_CURRENT,
                                      keydiffServTBParamIdValue,
                                      &objBurstSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BurstSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBurstSizeValue,
                           sizeof (objBurstSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServTBParam_Interval
*
* @purpose Get 'Interval'
*
* @description [Interval]: The time interval used with the token bucket. For:
*              1. Average Rate Meter, the Informal Differentiated Services
*              Model section 5.2.1, - Delta. 2. Simple Token Bucket Meter,
*              the Informal Differentiated Services Model section 5.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServTBParam_Interval (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServTBParamIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServTBParamId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServTBParam_diffServTBParamId,
                          (xLibU8_t *) & keydiffServTBParamIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServTBParamIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServTBParamIntervalGet (L7_UNIT_CURRENT, keydiffServTBParamIdValue,
                                     &objIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Interval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntervalValue,
                           sizeof (objIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServTBParam_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServTBParam_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServTBParamIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServTBParamId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServTBParam_diffServTBParamId,
                          (xLibU8_t *) & keydiffServTBParamIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServTBParamIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServTBParamStorageGet (L7_UNIT_CURRENT, keydiffServTBParamIdValue,
                                    &objStorageValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Storage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStorageValue,
                           sizeof (objStorageValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServTBParam_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The status of this conceptual row. All writable objects
*              in this row may be modified at any time. Setting this
*              variable to 'destroy' when the MIB contains one or more RowPointers
*              pointing to it results in destruction being delayed
*              until t 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServTBParam_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServTBParamIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServTBParamId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServTBParam_diffServTBParamId,
                          (xLibU8_t *) & keydiffServTBParamIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServTBParamIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServTBParamStatusGet (L7_UNIT_CURRENT, keydiffServTBParamIdValue,
                                   &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
