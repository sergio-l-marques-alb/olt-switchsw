/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServDataPath.c
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
#include "_xe_qosdiffServDataPath_obj.h"
#include "usmdb_mib_diffserv_api.h"

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_qosdiffServDataPath_ifIndex_IfDirection
*
* @purpose Get 'ifIndex + IfDirection +'
*
* @description [ifIndex]: A unique value for each interface. Its value ranges
*              between 1 and the value of ifNumber. The value for each
*              interface must remain constant at least from one re-initialization
*              of the entity's network management 
*              [IfDirection]: IfDirection specifies whether the reception
*              or transmission path for this interface is in view. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServDataPath_ifIndex_IfDirection (void *wap,
                                                           void *bufp[],
                                                           xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue, nextObjifIndexValue;
  fpObjWa_t owaIfDirection = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfDirectionValue, nextObjIfDirectionValue;
  void *outifIndex = (void *) bufp[--keyCount];
  void *outIfDirection = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outifIndex);
  FPOBJ_TRACE_ENTER (outIfDirection);

  /* retrieve key: ifIndex */
  owaifIndex.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_ifIndex,
                                 (xLibU8_t *) & objifIndexValue,
                                 &owaifIndex.len);
  if (owaifIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: IfDirection */
    owaIfDirection.rc =
      xLibFilterGet (wap, XOBJ_qosdiffServDataPath_IfDirection,
                     (xLibU8_t *) & objIfDirectionValue, &owaIfDirection.len);
  }
  else
  {
    objifIndexValue = 0;
    objIfDirectionValue = 0;
    nextObjifIndexValue = 0;
    nextObjIfDirectionValue = 0;
  }
  	
  FPOBJ_TRACE_CURRENT_KEY (outifIndex, &objifIndexValue, owaifIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outIfDirection, &objIfDirectionValue,
                           owaIfDirection.len);
  owa.rc =
  	usmDbDiffServDataPathGetNext(L7_UNIT_CURRENT, objifIndexValue, 
  	                                     objIfDirectionValue,
  	                                     &nextObjifIndexValue, 
  	                                     &nextObjIfDirectionValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outifIndex, owaifIndex);
    FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outifIndex, &nextObjifIndexValue, owaifIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outIfDirection, &nextObjIfDirectionValue,
                           owaIfDirection.len);

  /* return the object value: ifIndex */
  xLibBufDataSet (outifIndex,
                  (xLibU8_t *) & nextObjifIndexValue,
                  sizeof (nextObjifIndexValue));

  /* return the object value: IfDirection */
  xLibBufDataSet (outIfDirection,
                  (xLibU8_t *) & nextObjIfDirectionValue,
                  sizeof (nextObjIfDirectionValue));
  FPOBJ_TRACE_EXIT (outifIndex, owaifIndex);
  FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_qosdiffServDataPath_ifIndex
*
* @purpose Get 'ifIndex'
*
* @description [ifIndex] A unique value for each interface. Its value ranges between 1 and the value of ifNumber. The value for each interface must remain constant at least from one re-initialization of the entity's network management
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServDataPath_ifIndex (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU32_t objIfDirectionValue;
  xLibU32_t nextObjIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objifIndexValue = objIfDirectionValue = 0;
    owa.l7rc = usmDbDiffServDataPathGetNext(L7_UNIT_CURRENT,
                                            objifIndexValue,
                                            objIfDirectionValue, 
                                            &nextObjifIndexValue,
                                            &nextObjIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
    objIfDirectionValue = 0;
    do
    {
      owa.l7rc = usmDbDiffServDataPathGetNext(L7_UNIT_CURRENT,
                                              objifIndexValue,
                                              objIfDirectionValue, 
                                              &nextObjifIndexValue,
                                              &nextObjIfDirectionValue);
    }
    while ((objifIndexValue == nextObjifIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue, sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosdiffServDataPath_IfDirection
*
* @purpose Get 'IfDirection'
*
* @description [IfDirection] IfDirection specifies whether the reception or transmission path for this interface is in view.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServDataPath_IfDirection (void *wap, void *bufp)
{

  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  xLibU32_t objIfDirectionValue;
  xLibU32_t nextObjIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);

  /* retrieve key: IfDirection */
  owa.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_IfDirection,
                          (xLibU8_t *) & objIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIfDirectionValue = 0;
    owa.l7rc = usmDbDiffServDataPathGetNext(L7_UNIT_CURRENT,
                                            objifIndexValue,
                                            objIfDirectionValue, 
                                            &nextObjifIndexValue,
                                            &nextObjIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfDirectionValue, owa.len);

    owa.l7rc = usmDbDiffServDataPathGetNext(L7_UNIT_CURRENT,
                                            objifIndexValue,
                                            objIfDirectionValue, 
                                            &nextObjifIndexValue,
                                            &nextObjIfDirectionValue);

  }

  if ((objifIndexValue != nextObjifIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfDirectionValue, owa.len);

  /* return the object value: IfDirection */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfDirectionValue,
                           sizeof (objIfDirectionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosdiffServDataPath_Start
*
* @purpose Get 'Start'
*
* @description [Start]: This selects the first Differentiated Services Functional
*              Data Path Element to handle traffic for this data path.
*              This RowPointer should point to an instance of one of:
*              diffServClfrEntry diffServMeterEntry diffServActio 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServDataPath_Start (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStartValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc = usmDbDiffServDataPathStartGet (L7_UNIT_CURRENT, keyifIndexValue,
                                            keyIfDirectionValue, objStartValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Start */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStartValue,
                           strlen (objStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServDataPath_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServDataPath_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServDataPathStorageGet (L7_UNIT_CURRENT, keyifIndexValue,
                                              keyIfDirectionValue,
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
* @function fpObjGet_qosdiffServDataPath_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The status of this conceptual row. All writable objects
*              in this row may be modified at any time. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServDataPath_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_ifIndex,
                           (xLibU8_t *) & keyifIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosdiffServDataPath_IfDirection,
                           (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServDataPathStatusGet (L7_UNIT_CURRENT, keyifIndexValue,
                                             keyIfDirectionValue,
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
