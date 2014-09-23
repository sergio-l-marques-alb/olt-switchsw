/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServAssignQueue.c
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
#include "_xe_qosDiffServAssignQueue_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosDiffServAssignQueue_Index
*
* @purpose Get 'Index'
*
* @description [Index]: An index that enumerates the Assign Queue entries.
*              Managers obtain new values for row creation in this table
*              by reading agentDiffServAssignQueueNextFree. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAssignQueue_Index (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIndexValue;
  xLibU32_t nextObjIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAssignQueue_Index,
                          (xLibU8_t *) & objIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIndexValue = 0;
    nextObjIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIndexValue, owa.len);
  owa.l7rc = usmDbDiffServAssignQueueGetNext (L7_UNIT_CURRENT, objIndexValue,
                                                &nextObjIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIndexValue, owa.len);

  /* return the object value: Index */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIndexValue,
                           sizeof (objIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAssignQueue_Qnum
*
* @purpose Get 'Qnum'
*
* @description [Qnum]: The queue identifier value. This value indicates the
*              QoS queue number to which the traffic stream for this data
*              path is directed for egress from the device. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAssignQueue_Qnum (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQnumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAssignQueue_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServAssignQueueQnumGet (L7_UNIT_CURRENT, keyIndexValue,
                                              &objQnumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Qnum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQnumValue,
                           sizeof (objQnumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAssignQueue_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAssignQueue_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAssignQueue_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServAssignQueueStorageGet (L7_UNIT_CURRENT, keyIndexValue,
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
* @function fpObjGet_qosDiffServAssignQueue_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The status of this conceptual row. All writable objects
*              in this row may be modified at any time. Setting this
*              variable to 'destroy' when the MIB contains one or more RowPointers
*              pointing to it results in destruction being delayed
*              unti 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAssignQueue_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Index */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAssignQueue_Index,
                          (xLibU8_t *) & keyIndexValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServAssignQueueStatusGet (L7_UNIT_CURRENT, keyIndexValue,
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
