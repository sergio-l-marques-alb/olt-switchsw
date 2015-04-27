/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServRedirect.c
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
#include "_xe_qosDiffServRedirect_obj.h"
#include "usmdb_mib_diffserv_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_qosDiffServRedirect_DiffServRedirectId
*
* @purpose Get 'DiffServRedirectId'
*
* @description [DiffServRedirectId]: An index that enumerates the Redirect
*              entries. Managers obtain new values for row creation in this
*              table by reading agentDiffServRedirectNextFree. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServRedirect_DiffServRedirectId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServRedirectIdValue;
  xLibU32_t nextObjDiffServRedirectIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServRedirectId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServRedirect_DiffServRedirectId,
                          (xLibU8_t *) & objDiffServRedirectIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objDiffServRedirectIdValue = 0;
    objDiffServRedirectIdValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDiffServRedirectIdValue, owa.len);
  owa.l7rc =
      usmDbDiffServRedirectGetNext (L7_UNIT_CURRENT, objDiffServRedirectIdValue,
                                    &nextObjDiffServRedirectIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDiffServRedirectIdValue, owa.len);

  /* return the object value: DiffServRedirectId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDiffServRedirectIdValue,
                           sizeof (objDiffServRedirectIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServRedirect_Intf
*
* @purpose Get 'Intf'
*
* @description [Intf]: The external interface number identifier value. This
*              value indicates the egress interface to which the traffic
*              stream for this data path is directed. Normal traffic forwarding
*              decisions are bypassed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServRedirect_Intf (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServRedirectIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfValue;
  xLibU32_t tempIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServRedirectId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServRedirect_DiffServRedirectId,
                          (xLibU8_t *) & keyDiffServRedirectIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServRedirectIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServRedirectIntfGet (L7_UNIT_CURRENT, keyDiffServRedirectIdValue,
                                  &tempIntfValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(usmDbExtIfNumFromIntIfNum(tempIntfValue,&objIntfValue)  != L7_SUCCESS)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  /* return the object value: Intf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfValue,
                           sizeof (objIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServRedirect_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServRedirect_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServRedirectIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServRedirectId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServRedirect_DiffServRedirectId,
                          (xLibU8_t *) & keyDiffServRedirectIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServRedirectIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServRedirectStorageGet (L7_UNIT_CURRENT,
                                     keyDiffServRedirectIdValue,
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
* @function fpObjGet_qosDiffServRedirect_Status
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
xLibRC_t fpObjGet_qosDiffServRedirect_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServRedirectIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServRedirectId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServRedirect_DiffServRedirectId,
                          (xLibU8_t *) & keyDiffServRedirectIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServRedirectIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServRedirectStatusGet (L7_UNIT_CURRENT, keyDiffServRedirectIdValue,
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
