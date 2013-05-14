/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServAlg.c
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
#include "_xe_qosdiffServAlg_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_diffServAlgDropId
*
* @purpose Get 'diffServAlgDropId'
*
* @description [diffServAlgDropId]: An index that enumerates the Algorithmic
*              Dropper entries. Managers obtain new values for row creation
*              in this table by reading diffServAlgDropNextFree. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_diffServAlgDropId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServAlgDropIdValue;
  xLibU32_t nextObjdiffServAlgDropIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & objdiffServAlgDropIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objdiffServAlgDropIdValue = 0;
    nextObjdiffServAlgDropIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServAlgDropIdValue, owa.len);
  owa.l7rc =
      usmDbDiffServAlgDropGetNext (L7_UNIT_CURRENT, objdiffServAlgDropIdValue,
                                   &nextObjdiffServAlgDropIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdiffServAlgDropIdValue, owa.len);

  /* return the object value: diffServAlgDropId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdiffServAlgDropIdValue,
                           sizeof (objdiffServAlgDropIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropType
*
* @purpose Get 'DropType'
*
* @description [DropType]: The type of algorithm used by this dropper. The
*              value other(1) requires further specification in some other
*              MIB module. In the tailDrop(2) algorithm, diffServAlgDropQThreshold
*              represents the maximum depth of the queue, pointed
*              to by 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgDropTypeGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                 &objDropTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropTypeValue,
                           sizeof (objDropTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropNext
*
* @purpose Get 'DropNext'
*
* @description [DropNext]: This selects the next Differentiated Services Functional
*              Data Path Element to handle traffic for this data
*              path. This RowPointer should point to an instance of one of:
*              diffServClfrEntry diffServMeterEntry diffServAction 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropNext (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDropNextValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbDiffServAlgDropNextGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                 objDropNextValue); */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropNext */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDropNextValue,
                           strlen (objDropNextValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropQMeasure
*
* @purpose Get 'DropQMeasure'
*
* @description [DropQMeasure]: Points to an entry in the diffServQTable to
*              indicate the queue that a drop algorithm is to monitor when
*              deciding whether to drop a packet. If the row pointed to
*              does not exist, the algorithmic dropper element is considered
*              inactive. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropQMeasure (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDropQMeasureValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbDiffServAlgDropQMeasureGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                     objDropQMeasureValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropQMeasure */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDropQMeasureValue,
                           strlen (objDropQMeasureValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropQThreshold
*
* @purpose Get 'DropQThreshold'
*
* @description [DropQThreshold]: A threshold on the depth in bytes of the
*              queue being measured at which a trigger is generated to the
*              dropping algorithm, unless diffServAlgDropType is alwaysDrop(5)
*              where this object is ignored. For the tailDrop(2) or
*              headDrop(3) a 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropQThreshold (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropQThresholdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgDropQThresholdGet (L7_UNIT_CURRENT,
                                       keydiffServAlgDropIdValue,
                                       &objDropQThresholdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropQThreshold */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropQThresholdValue,
                           sizeof (objDropQThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropSpecific
*
* @purpose Get 'DropSpecific'
*
* @description [DropSpecific]: Points to a table entry that provides further
*              detail regarding a drop algorithm. Entries with diffServAlgDropType
*              equal to other(1) may have this point to a table
*              defined in another MIB module. Entries with diffServAlgDropType
*              equa 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropSpecific (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDropSpecificValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbDiffServAlgDropSpecificGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                     objDropSpecificValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropSpecific */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDropSpecificValue,
                           strlen (objDropSpecificValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropOctets
*
* @purpose Get 'DropOctets'
*
* @description [DropOctets]: The number of octets that have been deterministically
*              dropped by this drop process. Discontinuities in the
*              value of this counter can occur at re- initialization of
*              the management system and at other times as indicated by the
*              value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropOctets (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropOctetsValue;

  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgDropOctetsGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                   &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropOctetsValue,
                           sizeof (objDropOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropPkts
*
* @purpose Get 'DropPkts'
*
* @description [DropPkts]: The number of packets that have been deterministically
*              dropped by this drop process. Discontinuities in the
*              value of this counter can occur at re- initialization of
*              the management system and at other times as indicated by the
*              value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropPktsValue;

  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgDropPacketsGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                    &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropPktsValue,
                           sizeof (objDropPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_RandomDropOctets
*
* @purpose Get 'RandomDropOctets'
*
* @description [RandomDropOctets]: The number of octets that have been randomly
*              dropped by this drop process. This counter applies, therefore,
*              only to random droppers. Discontinuities in the value
*              of this counter can occur at re- initialization of the
*              management sy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_RandomDropOctets (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRandomDropOctetsValue;

  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgRandomDropOctetsGet (L7_UNIT_CURRENT,
                                         keydiffServAlgDropIdValue,
                                         &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RandomDropOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRandomDropOctetsValue,
                           sizeof (objRandomDropOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_RandomDropPkts
*
* @purpose Get 'RandomDropPkts'
*
* @description [RandomDropPkts]: The number of packets that have been randomly
*              dropped by this drop process. This counter applies, therefore,
*              only to random droppers. Discontinuities in the value
*              of this counter can occur at re- initialization of the
*              management sy 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_RandomDropPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRandomDropPktsValue;

  xLibU32_t tempLowValue;
  xLibU32_t tempHighValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgRandomDropPacketsGet (L7_UNIT_CURRENT,
                                          keydiffServAlgDropIdValue,
                                          &tempLowValue,&tempHighValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RandomDropPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRandomDropPktsValue,
                           sizeof (objRandomDropPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropStorage
*
* @purpose Get 'DropStorage'
*
* @description [DropStorage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropStorage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgDropStorageGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                    &objDropStorageValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropStorage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropStorageValue,
                           sizeof (objDropStorageValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServAlg_DropStatus
*
* @purpose Get 'DropStatus'
*
* @description [DropStatus]: The status of this conceptual row. All writable
*              objects in this row may be modified at any time. Setting
*              this variable to 'destroy' when the MIB contains one or more
*              RowPointers pointing to it results in destruction being
*              delayed until t 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServAlg_DropStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServAlgDropIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDropStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServAlgDropId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosdiffServAlg_diffServAlgDropId,
                          (xLibU8_t *) & keydiffServAlgDropIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServAlgDropIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAlgDropStatusGet (L7_UNIT_CURRENT, keydiffServAlgDropIdValue,
                                   &objDropStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DropStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDropStatusValue,
                           sizeof (objDropStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
