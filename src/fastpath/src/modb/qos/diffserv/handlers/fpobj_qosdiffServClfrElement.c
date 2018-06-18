/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServClfrElement.c
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
#include "_xe_qosdiffServClfrElement_obj.h"
#include "usmdb_mib_diffserv_api.h"

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_qosdiffServClfrElement_diffServClfrId_diffServClfrElementId
*
* @purpose Get 'diffServClfrId + diffServClfrElementId +'
*
* @description [diffServClfrId]: An index that enumerates the classifier entries.
*              Managers should obtain new values for row creation
*              in this table by reading diffServClfrNextFree. 
*              [diffServClfrElementId]: An index that enumerates the Classifier
*              Element entries. Managers obtain new values for row creation
*              in this table by reading diffServClfrElementNextFree.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_qosdiffServClfrElement_diffServClfrId_diffServClfrElementId (void *wap,
                                                                      void
                                                                      *bufp[],
                                                                      xLibU16_t
                                                                      keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owadiffServClfrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServClfrIdValue, nextObjdiffServClfrIdValue;
  fpObjWa_t owadiffServClfrElementId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServClfrElementIdValue, nextObjdiffServClfrElementIdValue;
  void *outdiffServClfrId = (void *) bufp[--keyCount];
  void *outdiffServClfrElementId = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outdiffServClfrId);
  FPOBJ_TRACE_ENTER (outdiffServClfrElementId);

  /* retrieve key: diffServClfrId */
  owadiffServClfrId.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                   (xLibU8_t *) & objdiffServClfrIdValue,
                   &owadiffServClfrId.len);
  if (owadiffServClfrId.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: diffServClfrElementId */
    owadiffServClfrElementId.rc =
      xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrElementId,
                     (xLibU8_t *) & objdiffServClfrElementIdValue,
                     &owadiffServClfrElementId.len);
	
    if(owadiffServClfrElementId.rc == XLIBRC_SUCCESS)
    {
       objdiffServClfrElementIdValue = 0;
    }
  }
  else
  {
    objdiffServClfrIdValue = 0;
    objdiffServClfrElementIdValue = 0;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (outdiffServClfrId, &objdiffServClfrIdValue,
                           owadiffServClfrId.len);
  FPOBJ_TRACE_CURRENT_KEY (outdiffServClfrElementId,
                           &objdiffServClfrElementIdValue,
                           owadiffServClfrElementId.len);
  owa.rc =
    usmDbDiffServClfrElemGetNext (L7_UNIT_CURRENT, objdiffServClfrIdValue,
                         objdiffServClfrElementIdValue,
                         &nextObjdiffServClfrIdValue,
                         &nextObjdiffServClfrElementIdValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outdiffServClfrId, owadiffServClfrId);
    FPOBJ_TRACE_EXIT (outdiffServClfrElementId, owadiffServClfrElementId);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outdiffServClfrId, &nextObjdiffServClfrIdValue,
                           owadiffServClfrId.len);
  FPOBJ_TRACE_CURRENT_KEY (outdiffServClfrElementId,
                           &nextObjdiffServClfrElementIdValue,
                           owadiffServClfrElementId.len);

  /* return the object value: diffServClfrId */
  xLibBufDataSet (outdiffServClfrId,
                  (xLibU8_t *) & nextObjdiffServClfrIdValue,
                  sizeof (nextObjdiffServClfrIdValue));

  /* return the object value: diffServClfrElementId */
  xLibBufDataSet (outdiffServClfrElementId,
                  (xLibU8_t *) & nextObjdiffServClfrElementIdValue,
                  sizeof (nextObjdiffServClfrElementIdValue));
  FPOBJ_TRACE_EXIT (outdiffServClfrId, owadiffServClfrId);
  FPOBJ_TRACE_EXIT (outdiffServClfrElementId, owadiffServClfrElementId);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_qosdiffServClfrElement_diffServClfrId
*
* @purpose Get 'diffServClfrId'
*
* @description [diffServClfrId] An index that enumerates the classifier entries. Managers should obtain new values for row creation in this table by reading diffServClfrNextFree.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfrElement_diffServClfrId (void *wap, void *bufp)
{

  xLibU32_t objdiffServClfrIdValue;
  xLibU32_t nextObjdiffServClfrIdValue;
  xLibU32_t objdiffServClfrElementIdValue;
  xLibU32_t nextObjdiffServClfrElementIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServClfrId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                          (xLibU8_t *) & objdiffServClfrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objdiffServClfrIdValue = objdiffServClfrElementIdValue = 0;
    owa.l7rc = usmDbDiffServClfrElemGetNext(L7_UNIT_CURRENT,
                                            objdiffServClfrIdValue,
                                            objdiffServClfrElementIdValue, 
                                            &nextObjdiffServClfrIdValue,
                                            &nextObjdiffServClfrElementIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServClfrIdValue, owa.len);
    objdiffServClfrElementIdValue = 0;
    do
    {
      owa.l7rc = usmDbDiffServClfrElemGetNext(L7_UNIT_CURRENT,
                                              objdiffServClfrIdValue,
                                              objdiffServClfrElementIdValue, 
                                              &nextObjdiffServClfrIdValue,
                                              &nextObjdiffServClfrElementIdValue);
    }
    while ((objdiffServClfrIdValue == nextObjdiffServClfrIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdiffServClfrIdValue, owa.len);

  /* return the object value: diffServClfrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdiffServClfrIdValue,
                           sizeof (objdiffServClfrIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosdiffServClfrElement_diffServClfrElementId
*
* @purpose Get 'diffServClfrElementId'
*
* @description [diffServClfrElementId] An index that enumerates the Classifier Element entries. Managers obtain new values for row creation in this table by reading diffServClfrElementNextFree.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfrElement_diffServClfrElementId (void *wap, void *bufp)
{

  xLibU32_t objdiffServClfrIdValue;
  xLibU32_t nextObjdiffServClfrIdValue;
  xLibU32_t objdiffServClfrElementIdValue;
  xLibU32_t nextObjdiffServClfrElementIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServClfrId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                          (xLibU8_t *) & objdiffServClfrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServClfrIdValue, owa.len);

  /* retrieve key: diffServClfrElementId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrElementId,
                          (xLibU8_t *) & objdiffServClfrElementIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objdiffServClfrElementIdValue = 0;
    owa.l7rc = usmDbDiffServClfrElemGetNext(L7_UNIT_CURRENT,
                                            objdiffServClfrIdValue,
                                            objdiffServClfrElementIdValue, 
                                            &nextObjdiffServClfrIdValue,
                                            &nextObjdiffServClfrElementIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServClfrElementIdValue, owa.len);

    owa.l7rc = usmDbDiffServClfrElemGetNext(L7_UNIT_CURRENT,
                                            objdiffServClfrIdValue,
                                            objdiffServClfrElementIdValue, 
                                            &nextObjdiffServClfrIdValue,
                                            &nextObjdiffServClfrElementIdValue);

  }

  if ((objdiffServClfrIdValue != nextObjdiffServClfrIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdiffServClfrElementIdValue, owa.len);

  /* return the object value: diffServClfrElementId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdiffServClfrElementIdValue,
                           sizeof (objdiffServClfrElementIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosdiffServClfrElement_Precedence
*
* @purpose Get 'Precedence'
*
* @description [Precedence]: The relative order in which classifier elements
*              are applied: higher numbers represent classifier element
*              with higher precedence. Classifier elements with the same
*              order must be unambiguous i.e. they must define non-overlapping
*              patterns, 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfrElement_Precedence (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrElementIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPrecedenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServClfrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                           (xLibU8_t *) & keydiffServClfrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrIdValue, kwa1.len);

  /* retrieve key: diffServClfrElementId */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrElementId,
                   (xLibU8_t *) & keydiffServClfrElementIdValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrElementIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClfrElemPrecedenceGet (L7_UNIT_CURRENT, keydiffServClfrIdValue,
                                        keydiffServClfrElementIdValue,
                                        &objPrecedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Precedence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPrecedenceValue,
                           sizeof (objPrecedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServClfrElement_Next
*
* @purpose Get 'Next'
*
* @description [Next]: This attribute provides one branch of the fan-out functionality
*              of a classifier described in the Informal Differentiated
*              Services Model section 4.1. This selects the next
*              Differentiated Services Functional Data Path Element to hand
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfrElement_Next (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrElementIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objNextValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServClfrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                           (xLibU8_t *) & keydiffServClfrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrIdValue, kwa1.len);

  /* retrieve key: diffServClfrElementId */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrElementId,
                   (xLibU8_t *) & keydiffServClfrElementIdValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrElementIdValue, kwa2.len);

  /* get the value from application */
  /* owa.l7rc =
    usmDbDiffServClfrElemNextGet (L7_UNIT_CURRENT, keydiffServClfrIdValue,
                                  keydiffServClfrElementIdValue, objNextValue); */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Next */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objNextValue,
                           strlen (objNextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServClfrElement_Specific
*
* @purpose Get 'Specific'
*
* @description [Specific]: A pointer to a valid entry in another table, filter
*              table, that describes the applicable classification parameters,
*              e.g. an entry in diffServMultiFieldClfrTable. The
*              value zeroDotZero is interpreted to match anything not matched
*              by 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfrElement_Specific (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrElementIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSpecificValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServClfrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                           (xLibU8_t *) & keydiffServClfrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrIdValue, kwa1.len);

  /* retrieve key: diffServClfrElementId */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrElementId,
                   (xLibU8_t *) & keydiffServClfrElementIdValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrElementIdValue, kwa2.len);

  /* get the value from application */
  /* owa.l7rc =
    usmDbDiffServClfrElemSpecificGet (L7_UNIT_CURRENT, keydiffServClfrIdValue,
                                      keydiffServClfrElementIdValue,
                                      objSpecificValue); */
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
* @function fpObjGet_qosdiffServClfrElement_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfrElement_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrElementIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServClfrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                           (xLibU8_t *) & keydiffServClfrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrIdValue, kwa1.len);

  /* retrieve key: diffServClfrElementId */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrElementId,
                   (xLibU8_t *) & keydiffServClfrElementIdValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrElementIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClfrElemStorageGet (L7_UNIT_CURRENT, keydiffServClfrIdValue,
                                     keydiffServClfrElementIdValue,
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
* @function fpObjGet_qosdiffServClfrElement_Status
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
xLibRC_t fpObjGet_qosdiffServClfrElement_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServClfrElementIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServClfrId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrId,
                           (xLibU8_t *) & keydiffServClfrIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrIdValue, kwa1.len);

  /* retrieve key: diffServClfrElementId */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServClfrElement_diffServClfrElementId,
                   (xLibU8_t *) & keydiffServClfrElementIdValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServClfrElementIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServClfrElemStatusGet (L7_UNIT_CURRENT, keydiffServClfrIdValue,
                                    keydiffServClfrElementIdValue,
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
