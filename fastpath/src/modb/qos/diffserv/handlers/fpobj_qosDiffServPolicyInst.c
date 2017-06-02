/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServPolicyInst.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to diffserv-object.xml
*
* @create  1 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_qosDiffServPolicyInst_obj.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_common.h"

xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t 
len);

#ifdef RADHA
/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyInst_PolicyIndex_PolicyInstIndex
*
* @purpose Get 'PolicyIndex + PolicyInstIndex +'
*
* @description [PolicyIndex]: The identifier for DiffServ Policy table entry.
*              
*              [PolicyInstIndex]: The identifier for policy-class Instance
*              table entry within a policy. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyIndex_PolicyInstIndex (void *wap,
                                                                     void
                                                                     *bufp[],
                                                                     xLibU16_t
                                                                     keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaPolicyIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyIndexValue, nextObjPolicyIndexValue;
  fpObjWa_t owaPolicyInstIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstIndexValue, nextObjPolicyInstIndexValue;
  void *outPolicyIndex = (void *) bufp[--keyCount];
  void *outPolicyInstIndex = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outPolicyIndex);
  FPOBJ_TRACE_ENTER (outPolicyInstIndex);

  /* retrieve key: PolicyIndex */
  owaPolicyIndex.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                   (xLibU8_t *) & objPolicyIndexValue, &owaPolicyIndex.len);
  if (owaPolicyIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: PolicyInstIndex */
    owaPolicyInstIndex.rc =
      xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                     (xLibU8_t *) & objPolicyInstIndexValue,
                     &owaPolicyInstIndex.len);
  }
  else
  {
    /*failed to get policy index. hence try to get the first policy index avaialble */
    objPolicyIndexValue = 0;
    objPolicyInstIndexValue = 0;
    nextObjPolicyIndexValue = 0;
    nextObjPolicyInstIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (outPolicyIndex, &objPolicyIndexValue,
                           owaPolicyIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outPolicyInstIndex, &objPolicyInstIndexValue,
                           owaPolicyInstIndex.len);
  owa.rc =
    usmDbDiffServPolicyInstGetNext (L7_UNIT_CURRENT, objPolicyIndexValue,
                         objPolicyInstIndexValue, &nextObjPolicyIndexValue,
                         &nextObjPolicyInstIndexValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outPolicyIndex, owaPolicyIndex);
    FPOBJ_TRACE_EXIT (outPolicyInstIndex, owaPolicyInstIndex);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outPolicyIndex, &nextObjPolicyIndexValue,
                           owaPolicyIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outPolicyInstIndex, &nextObjPolicyInstIndexValue,
                           owaPolicyInstIndex.len);

  /* return the object value: PolicyIndex */
  xLibBufDataSet (outPolicyIndex,
                  (xLibU8_t *) & nextObjPolicyIndexValue,
                  sizeof (nextObjPolicyIndexValue));

  /* return the object value: PolicyInstIndex */
  xLibBufDataSet (outPolicyInstIndex,
                  (xLibU8_t *) & nextObjPolicyInstIndexValue,
                  sizeof (nextObjPolicyInstIndexValue));
  FPOBJ_TRACE_EXIT (outPolicyIndex, owaPolicyIndex);
  FPOBJ_TRACE_EXIT (outPolicyInstIndex, owaPolicyInstIndex);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyInst_PolicyIndex
*
* @purpose Get 'PolicyIndex'
*
* @description [PolicyIndex] The identifier for DiffServ Policy table entry.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyIndex (void *wap, void *bufp)
{

  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyIndexValue = objPolicyInstIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyInstGetNext(L7_UNIT_CURRENT,
                                     objPolicyIndexValue,
                                     objPolicyInstIndexValue, &nextObjPolicyIndexValue,
                                     &nextObjPolicyInstIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);
    objPolicyInstIndexValue = 0;
    nextObjPolicyInstIndexValue = 0;
    do
    {
      objPolicyInstIndexValue = nextObjPolicyInstIndexValue;
      
      owa.l7rc = usmDbDiffServPolicyInstGetNext (L7_UNIT_CURRENT,
                                      objPolicyIndexValue,
                                      objPolicyInstIndexValue, &nextObjPolicyIndexValue,
                                      &nextObjPolicyInstIndexValue);
    }
    while ((objPolicyIndexValue == nextObjPolicyIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyIndexValue, owa.len);

  /* return the object value: PolicyIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyIndexValue,
                           sizeof (objPolicyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyInst_PolicyInstIndex
*
* @purpose Get 'PolicyInstIndex'
*
* @description [PolicyInstIndex] The identifier for policy-class Instance table entry within a policy.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyInstIndex (void *wap, void *bufp)
{
  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  xLibU32_t objPolicyInstIndexValue;
  xLibU32_t nextObjPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                          (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);

  /* retrieve key: PolicyInstIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                          (xLibU8_t *) & objPolicyInstIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyInstIndexValue = 0;
    owa.l7rc = usmDbDiffServPolicyInstGetNext (L7_UNIT_CURRENT,
                                     objPolicyIndexValue,
                                     objPolicyInstIndexValue, &nextObjPolicyIndexValue,
                                     &nextObjPolicyInstIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyInstIndexValue, owa.len);

    owa.l7rc = usmDbDiffServPolicyInstGetNext (L7_UNIT_CURRENT,
                                    objPolicyIndexValue,
                                    objPolicyInstIndexValue, &nextObjPolicyIndexValue,
                                    &nextObjPolicyInstIndexValue);

  }

  if ((objPolicyIndexValue != nextObjPolicyIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPolicyInstIndexValue, owa.len);

  /* return the object value: PolicyInstIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPolicyInstIndexValue,
                           sizeof (objPolicyInstIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyInst_PolicyInstClassIndex
*
* @purpose Get 'PolicyInstClassIndex'
*
* @description [PolicyInstClassIndex]: Index of the DiffServ Class Table row,
*              whose class definition is used as the policy class instance.
*              The class index identifies the classifier definition
*              used by the policy to establish an instance of that class (to
*              which policy attr 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyInstClassIndex (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstClassIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyInstClassIndexGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                          keyPolicyInstIndexValue,
                                          &objPolicyInstClassIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyInstClassIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstClassIndexValue,
                           sizeof (objPolicyInstClassIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyInst_PolicyInstClassIndex
*
* @purpose Set 'PolicyInstClassIndex'
*
* @description [PolicyInstClassIndex]: Index of the DiffServ Class Table row,
*              whose class definition is used as the policy class instance.
*              The class index identifies the classifier definition
*              used by the policy to establish an instance of that class (to
*              which policy attr 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyInst_PolicyInstClassIndex (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstClassIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  xLibU32_t tempPolicyInstId;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PolicyInstClassIndex */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPolicyInstClassIndexValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyInstClassIndexValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);
  
 /* check if the given class Index is valid */
 if(usmDbDiffServClassGet(L7_UNIT_CURRENT, objPolicyInstClassIndexValue) != L7_SUCCESS )
 {
    owa.rc = XLIBRC_DIFFSERV_CLASS_NOT_EXISTS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
 }

  /*check if the given class index is already part of this policy-instindex group */
  if (( usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, keyPolicyIndexValue, objPolicyInstClassIndexValue,&tempPolicyInstId) == L7_SUCCESS) &&
  	       (tempPolicyInstId == keyPolicyInstIndexValue) )
  {
    owa.rc = XLIBRC_DIFFSERV_CLASS_ALREADY_ADDED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyInstClassIndexSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                          keyPolicyInstIndexValue,
                                          objPolicyInstClassIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DIFFSERV_CLASS_ADD_FAILED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyInst_PolicyInstAttrIndexNextFree
*
* @purpose Get 'PolicyInstAttrIndexNextFree'
*
* @description [PolicyInstAttrIndexNextFree]: This object contains an unused
*              value for agentDiffServPolicyAttrIndex, or a zero to indicate
*              that none exists. This index is maintained on a per-policy-class-instance
*              basis. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyInstAttrIndexNextFree (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstAttrIndexNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyAttrIndexNext (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                      keyPolicyInstIndexValue,
                                      &objPolicyInstAttrIndexNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if(objPolicyInstAttrIndexNextFreeValue == 0 )
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /*ADD TABLE fULL ERROR CODE */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyInstAttrIndexNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstAttrIndexNextFreeValue,
                    sizeof (objPolicyInstAttrIndexNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyInst_PolicyInstStorageType
*
* @purpose Get 'PolicyInstStorageType'
*
* @description [PolicyInstStorageType]: Storage-type for this conceptual row.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyInstStorageType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyInstStorageTypeGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           &objPolicyInstStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyInstStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstStorageTypeValue,
                           sizeof (objPolicyInstStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyInst_PolicyInstStorageType
*
* @purpose Set 'PolicyInstStorageType'
*
* @description [PolicyInstStorageType]: Storage-type for this conceptual row.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyInst_PolicyInstStorageType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstStorageTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PolicyInstStorageType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPolicyInstStorageTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyInstStorageTypeValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyInstStorageTypeSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           objPolicyInstStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServPolicyInst_PolicyInstRowStatus
*
* @purpose Get 'PolicyInstRowStatus'
*
* @description [PolicyInstRowStatus]: The status of this conceptual row. Entries
*              can not be deleted until all rows in agentDiffServPolicyAttrTable
*              with corresponding values of agentDiffServPolicyIndex
*              and agentDiffServPolicyInstIndex have been deleted
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyInstRowStatus (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyInstRowStatusGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                         keyPolicyInstIndexValue,
                                         &objPolicyInstRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyInstRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstRowStatusValue,
                           sizeof (objPolicyInstRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_qosDiffServPolicyInst_PolicyInstRowStatus
*
* @purpose Set 'PolicyInstRowStatus'
*
* @description [PolicyInstRowStatus]: The status of this conceptual row. Entries
*              can not be deleted until all rows in agentDiffServPolicyAttrTable
*              with corresponding values of agentDiffServPolicyIndex
*              and agentDiffServPolicyInstIndex have been deleted
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicyInst_PolicyInstRowStatus (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
	xLibU32_t nextPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  xLibU32_t nextPolicyIndex;
  xLibU32_t nextPolicyInstIndex;
  xLibU32_t policyAttrIndex;
  xLibU32_t totalAttribCount;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PolicyInstRowStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPolicyInstRowStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyInstRowStatusValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);
	
  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    keyPolicyInstIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);
  
  /* call the usmdb only for add and delete */
  if ( (keyPolicyInstIndexValue == 0) && ((objPolicyInstRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO ) ||
  	   (objPolicyInstRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT )))
  {
    owa.l7rc = usmDbDiffServPolicyInstIndexNext(L7_UNIT_CURRENT, keyPolicyIndexValue, 
    &nextPolicyIndexValue);
		if(owa.l7rc != L7_SUCCESS)
		{
		  owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
		}
		
    /* Create a row */
    owa.l7rc = usmDbDiffServPolicyInstCreate(USMDB_UNIT_CURRENT,
					keyPolicyIndexValue,
					nextPolicyIndexValue, 
          ((objPolicyInstRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ? L7_TRUE : L7_FALSE));
    if (owa.l7rc != L7_SUCCESS)
    {
      if (owa.l7rc == L7_REQUEST_DENIED)
      {
        owa.rc = XLIBRC_DIFFSERV_CHANGE_REQ_DENY_APP_POLICY_INST_LIMIT;
      }
      else
      {
        owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      }
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
		else
		{
    xLibFilterSet(wap,XOBJ_qosDiffServPolicyInst_PolicyInstIndex,0,(xLibU8_t *) &nextPolicyIndexValue,sizeof(nextPolicyIndexValue));

		}

    /*owa.l7rc =
      usmDbDiffServPolicyInstRowStatusSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           objPolicyInstRowStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE; 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }*/
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objPolicyInstRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
   kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);  
		if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
    /* Delete the existing row */
     policyAttrIndex = 0;
     totalAttribCount = 0;
     while ((usmDbDiffServPolicyAttrGetNext(L7_UNIT_CURRENT,keyPolicyIndexValue, keyPolicyInstIndexValue, policyAttrIndex,
                &nextPolicyIndex, &nextPolicyInstIndex, &policyAttrIndex) == L7_SUCCESS) &&
                (nextPolicyIndex == keyPolicyIndexValue) &&
                (nextPolicyInstIndex == keyPolicyInstIndexValue))
    {
      if( policyAttrIndex == 0 )
         break;
      else
        totalAttribCount++;
    }

    if ( totalAttribCount > 0 )
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Add error string */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
	
    owa.l7rc =
      usmDbDiffServPolicyInstRowStatusSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
                                           keyPolicyInstIndexValue,
                                           objPolicyInstRowStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
*	@function fpObjGet_qosDiffServPolicyInst_PolicyInstClassName
*
* @purpose Get 'PolicyInstClassName'
*
* @description [PolicyInstClassName]: Provides the anem of the class assigned to policy instance
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyInstClassName (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPolicyInstClassName;
  xLibU32_t tempClassInstanceIndex;
	xLibU32_t len;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServPolicyInstClassIndexGet(L7_UNIT_CURRENT, keyPolicyIndexValue,
                              keyPolicyInstIndexValue,&tempClassInstanceIndex);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	len = sizeof(objPolicyInstClassName);
	memset(objPolicyInstClassName,0x0,len);
	owa.l7rc = usmDbDiffServClassNameGet( L7_UNIT_CURRENT,
		                                   tempClassInstanceIndex,  objPolicyInstClassName, &len);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyInstAttrIndexNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *)  objPolicyInstClassName,
                    strlen (objPolicyInstClassName));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/******************************************************************************
*
* @function fpObjGet_qosDiffServPolicyInst_PolicyInstClassNameList
*
* @purpose Get 'PolicyInstClassNameList'
 *@description  [PolicyInstClassNameList] Name of the DiffServ Class Table row,
* whose class definition is used as the policy class instance. The
* class index identifies the classifier definition used by the
* policy to establish an instance of that class (to which policy attr
* @notes
*
* @return
*******************************************************************************/

xLibRC_t fpObjGet_qosDiffServPolicyInst_PolicyInstClassNameList (void *wap, 
void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyInstIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPolicyInstClassName;
  xLibU32_t tempClassInstanceIndex;
	xLibU32_t len;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyIndex,
                           (xLibU8_t *) & keyPolicyIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa1.len);

  /* retrieve key: PolicyInstIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicyInst_PolicyInstIndex,
                           (xLibU8_t *) & keyPolicyInstIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyInstIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServPolicyInstClassIndexGet(L7_UNIT_CURRENT, keyPolicyIndexValue,
                              keyPolicyInstIndexValue,&tempClassInstanceIndex);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	len = sizeof(objPolicyInstClassName);
	memset(objPolicyInstClassName,0x0,len);
	owa.l7rc = usmDbDiffServClassNameGet( L7_UNIT_CURRENT,
		                                   tempClassInstanceIndex,  objPolicyInstClassName, &len);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyInstAttrIndexNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *)  objPolicyInstClassName,
                    strlen (objPolicyInstClassName));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/******************************************************************************
*
* @function fpObjSet_qosDiffServPolicyInst_PolicyInstClassNameList
*
* @purpose Set 'PolicyInstClassNameList'
 *@description  [PolicyInstClassNameList] Name of the DiffServ Class Table row,
* whose class definition is used as the policy class instance. The
* class index identifies the classifier definition used by the
* policy to establish an instance of that class (to which policy attr
* @notes
*
* @return
*******************************************************************************/

xLibRC_t fpObjSet_qosDiffServPolicyInst_PolicyInstClassNameList (void *wap, 
void *bufp)
{

/* Just return success for now */
  return XLIBRC_SUCCESS;
}

