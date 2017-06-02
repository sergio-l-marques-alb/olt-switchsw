/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServGenGetNextIndx.c
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
#include "_xe_qosDiffServGenGetNextIndx_obj.h"
#include "usmdb_mib_diffserv_api.h"

#if 0
/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServQNextFree
*
* @purpose Get 'diffServQNextFree'
*
* @description [diffServQNextFree]: This object contains an unused value for
*              diffServQId, or a zero to indicate that none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServQNextFree (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServQNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServQIndexNext (L7_UNIT_CURRENT, &objdiffServQNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServQNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServQNextFreeValue,
                     sizeof (objdiffServQNextFreeValue));

  /* return the object value: diffServQNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServQNextFreeValue,
                           sizeof (objdiffServQNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif
/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServCountActNextFree
*
* @purpose Get 'diffServCountActNextFree'
*
* @description [diffServCountActNextFree]: This object contains an unused
*              value for diffServCountActId, or a zero to indicate that none
*              exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServCountActNextFree (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServCountActNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServCountActIndexNext (L7_UNIT_CURRENT,
                                    &objdiffServCountActNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServCountActNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServCountActNextFreeValue,
                     sizeof (objdiffServCountActNextFreeValue));

  /* return the object value: diffServCountActNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServCountActNextFreeValue,
                    sizeof (objdiffServCountActNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServMinRateNextFree
*
* @purpose Get 'diffServMinRateNextFree'
*
* @description [diffServMinRateNextFree]: This object contains an unused value
*              for diffServMinRateId, or a zero to indicate that none
*              exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServMinRateNextFree (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServMinRateNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMinRateIndexNext (L7_UNIT_CURRENT,
                                   &objdiffServMinRateNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServMinRateNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServMinRateNextFreeValue,
                     sizeof (objdiffServMinRateNextFreeValue));

  /* return the object value: diffServMinRateNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServMinRateNextFreeValue,
                           sizeof (objdiffServMinRateNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServActionNextFree
*
* @purpose Get 'diffServActionNextFree'
*
* @description [diffServActionNextFree]: This object contains an unused value
*              for diffServActionId, or a zero to indicate that none exist.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServActionNextFree (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServActionNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServActionIndexNext (L7_UNIT_CURRENT,
                                  &objdiffServActionNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServActionNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServActionNextFreeValue,
                     sizeof (objdiffServActionNextFreeValue));

  /* return the object value: diffServActionNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServActionNextFreeValue,
                           sizeof (objdiffServActionNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServAlgDropNextFree
*
* @purpose Get 'diffServAlgDropNextFree'
*
* @description [diffServAlgDropNextFree]: This object contains an unused value
*              for diffServAlgDropId, or a zero to indicate that none
*              exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServAlgDropNextFree (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServAlgDropNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgDropIndexNext (L7_UNIT_CURRENT,
                                   &objdiffServAlgDropNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServAlgDropNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServAlgDropNextFreeValue,
                     sizeof (objdiffServAlgDropNextFreeValue));

  /* return the object value: diffServAlgDropNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServAlgDropNextFreeValue,
                           sizeof (objdiffServAlgDropNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServMultiFieldClfrNextFree
*
* @purpose Get 'diffServMultiFieldClfrNextFree'
*
* @description [diffServMultiFieldClfrNextFree]: This object contains an unused
*              value for diffServMultiFieldClfrId, or a zero to indicate
*              that none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServMultiFieldClfrNextFree (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServMultiFieldClfrNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrIndexNext (L7_UNIT_CURRENT,
                                  &objdiffServMultiFieldClfrNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServMultiFieldClfrNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  	
  FPOBJ_TRACE_VALUE (bufp, &objdiffServMultiFieldClfrNextFreeValue,
                     sizeof (objdiffServMultiFieldClfrNextFreeValue));

  /* return the object value: diffServMultiFieldClfrNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServMultiFieldClfrNextFreeValue,
                    sizeof (objdiffServMultiFieldClfrNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServMaxRateNextFree
*
* @purpose Get 'diffServMaxRateNextFree'
*
* @description [diffServMaxRateNextFree]: This object contains an unused value
*              for diffServMaxRateId, or a zero to indicate that none
*              exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServMaxRateNextFree (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServMaxRateNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMaxRateIndexNext (L7_UNIT_CURRENT,
                                   &objdiffServMaxRateNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServMaxRateNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServMaxRateNextFreeValue,
                     sizeof (objdiffServMaxRateNextFreeValue));

  /* return the object value: diffServMaxRateNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServMaxRateNextFreeValue,
                           sizeof (objdiffServMaxRateNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServRandomDropNextFree
*
* @purpose Get 'diffServRandomDropNextFree'
*
* @description [diffServRandomDropNextFree]: This object contains an unused
*              value for diffServRandomDropId, or a zero to indicate that
*              none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServRandomDropNextFree (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServRandomDropNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServRandomDropIndexNext (L7_UNIT_CURRENT,
                                      &objdiffServRandomDropNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServRandomDropNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServRandomDropNextFreeValue,
                     sizeof (objdiffServRandomDropNextFreeValue));

  /* return the object value: diffServRandomDropNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServRandomDropNextFreeValue,
                    sizeof (objdiffServRandomDropNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif


/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServClfrNextFree
*
* @purpose Get 'diffServClfrNextFree'
*
* @description [diffServClfrNextFree]: This object contains an unused value
*              for diffServClfrId, or a zero to indicate that none exist.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServClfrNextFree (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServClfrNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClfrIndexNext (L7_UNIT_CURRENT, &objdiffServClfrNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   if(objdiffServClfrNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServClfrNextFreeValue,
                     sizeof (objdiffServClfrNextFreeValue));

  /* return the object value: diffServClfrNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServClfrNextFreeValue,
                           sizeof (objdiffServClfrNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServSchedulerNextFree
*
* @purpose Get 'diffServSchedulerNextFree'
*
* @description [diffServSchedulerNextFree]: This object contains an unused
*              value for diffServSchedulerId, or a zero to indicate that
*              none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServSchedulerNextFree (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServSchedulerNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServSchedulerIndexNext (L7_UNIT_CURRENT,
                                     &objdiffServSchedulerNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(objdiffServSchedulerNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServSchedulerNextFreeValue,
                     sizeof (objdiffServSchedulerNextFreeValue));

  /* return the object value: diffServSchedulerNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServSchedulerNextFreeValue,
                    sizeof (objdiffServSchedulerNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServMeterNextFree
*
* @purpose Get 'diffServMeterNextFree'
*
* @description [diffServMeterNextFree]: This object contains an unused value
*              for diffServMeterId, or a zero to indicate that none exist.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServMeterNextFree (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServMeterNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMeterIndexNext (L7_UNIT_CURRENT,
                                 &objdiffServMeterNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServMeterNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServMeterNextFreeValue,
                     sizeof (objdiffServMeterNextFreeValue));

  /* return the object value: diffServMeterNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServMeterNextFreeValue,
                           sizeof (objdiffServMeterNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServClfrElementNextFree
*
* @purpose Get 'diffServClfrElementNextFree'
*
* @description [diffServClfrElementNextFree]: This object contains an unused
*              value for diffServClfrElementId, or a zero to indicate that
*              none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServClfrElementNextFree (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServClfrElementNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClfrElemIndexNext (L7_UNIT_CURRENT,
                                    &objdiffServClfrElementNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServClfrElementNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServClfrElementNextFreeValue,
                     sizeof (objdiffServClfrElementNextFreeValue));

  /* return the object value: diffServClfrElementNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServClfrElementNextFreeValue,
                    sizeof (objdiffServClfrElementNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGenGetNextIndx_diffServTBParamNextFree
*
* @purpose Get 'diffServTBParamNextFree'
*
* @description [diffServTBParamNextFree]: This object contains an unused value
*              for diffServTBParamId, or a zero to indicate that none
*              exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGenGetNextIndx_diffServTBParamNextFree (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServTBParamNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServTBParamIndexNext (L7_UNIT_CURRENT,
                                   &objdiffServTBParamNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objdiffServTBParamNextFreeValue  == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdiffServTBParamNextFreeValue,
                     sizeof (objdiffServTBParamNextFreeValue));

  /* return the object value: diffServTBParamNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdiffServTBParamNextFreeValue,
                           sizeof (objdiffServTBParamNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
