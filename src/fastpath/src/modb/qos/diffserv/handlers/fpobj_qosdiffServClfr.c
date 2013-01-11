/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServClfr.c
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
#include "_xe_qosdiffServClfr_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosdiffServClfr_Id
*
* @purpose Get 'Id'
*
* @description [Id]: An index that enumerates the classifier entries. Managers
*              should obtain new values for row creation in this table
*              by reading diffServClfrNextFree. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfr_Id (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIdValue;
  xLibU32_t nextObjIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfr_Id,
                          (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIdValue = 0;
    nextObjIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIdValue, owa.len);
  owa.l7rc = usmDbDiffServClfrGetNext (L7_UNIT_CURRENT, objIdValue,
                                         &nextObjIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIdValue, owa.len);

  /* return the object value: Id */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIdValue,
                           sizeof (objIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServClfr_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServClfr_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfr_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClfrStorageGet (L7_UNIT_CURRENT, keyIdValue,
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
* @function fpObjGet_qosdiffServClfr_Status
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
xLibRC_t fpObjGet_qosdiffServClfr_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServClfr_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServClfrStatusGet (L7_UNIT_CURRENT, keyIdValue,
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
