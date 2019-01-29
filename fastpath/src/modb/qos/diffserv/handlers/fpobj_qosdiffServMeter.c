/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServMeter.c
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
#include "_xe_qosdiffServMeter_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosdiffServMeter_diffServMeterId
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
xLibRC_t fpObjGet_qosdiffServMeter_diffServMeterId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServMeterIdValue;
  xLibU32_t nextObjdiffServMeterIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServMeter_diffServMeterId,
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
* @function fpObjGet_qosdiffServMeter_SucceedNext
*
* @purpose Get 'SucceedNext'
*
* @description [SucceedNext]: If the traffic does conform, this selects the
*              next Differentiated Services Functional Data Path element
*              to handle traffic for this data path. This RowPointer should
*              point to an instance of one of: diffServClfrEntry diffSe
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMeter_SucceedNext (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSucceedNextValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServMeter_diffServMeterId,
                          (xLibU8_t *) & keydiffServMeterIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMeterIdValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbDiffServMeterSucceedNextGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
                                      objSucceedNextValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SucceedNext */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSucceedNextValue,
                           strlen (objSucceedNextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMeter_FailNext
*
* @purpose Get 'FailNext'
*
* @description [FailNext]: If the traffic does not conform, this selects the
*              next Differentiated Services Functional Data Path element
*              to handle traffic for this data path. This RowPointer should
*              point to an instance of one of: diffServClfrEntry di 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMeter_FailNext (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objFailNextValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServMeter_diffServMeterId,
                          (xLibU8_t *) & keydiffServMeterIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMeterIdValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbDiffServMeterFailNextGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
                                   objFailNextValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FailNext */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objFailNextValue,
                           strlen (objFailNextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMeter_Specific
*
* @purpose Get 'Specific'
*
* @description [Specific]: This indicates the behavior of the meter by pointing
*              to an entry containing detailed parameters. Note that
*              entries in that specific table must be managed explicitly.
*              For example, diffServMeterSpecific may point to an entry in
*              diffS 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMeter_Specific (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSpecificValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServMeter_diffServMeterId,
                          (xLibU8_t *) & keydiffServMeterIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMeterIdValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbDiffServMeterSpecificGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
                                   objSpecificValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Specific */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSpecificValue,
                           strlen (objSpecificValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMeter_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMeter_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServMeter_diffServMeterId,
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
    usmDbDiffServMeterStorageGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
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
* @function fpObjGet_qosdiffServMeter_Status
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
xLibRC_t fpObjGet_qosdiffServMeter_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMeterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMeterId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServMeter_diffServMeterId,
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
    usmDbDiffServMeterStatusGet (L7_UNIT_CURRENT, keydiffServMeterIdValue,
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
