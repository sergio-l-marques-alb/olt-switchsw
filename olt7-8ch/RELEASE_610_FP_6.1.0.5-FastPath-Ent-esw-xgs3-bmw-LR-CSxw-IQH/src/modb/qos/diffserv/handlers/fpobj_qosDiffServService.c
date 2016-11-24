/*******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 ********************************************************************************
 *
 * @filename fpobj_qosDiffServService.c
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
#include "_xe_qosDiffServService_obj.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

#ifdef RADHA
/*******************************************************************************
 * @function fpObjGet_qosDiffServService_IfIndex_IfDirection
 *
 * @purpose Get 'IfIndex + IfDirection +'
 *
 * @description [IfIndex]: External interface number of the interface to which
 *              policy is assigned. 
 *              [IfDirection]: Interface direction to which the policy is assigned.
 *              
 *
 * @note  This is KEY Object
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_IfIndex_IfDirection (void *wap,
    void *bufp[],
    xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue, nextObjIfIndexValue;
  fpObjWa_t owaIfDirection = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfDirectionValue, nextObjIfDirectionValue;

  void *outIfIndex = (void *) bufp[--keyCount];
  void *outIfDirection = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outIfIndex);
  FPOBJ_TRACE_ENTER (outIfDirection);

  /* retrieve key: IfIndex */
  owaIfIndex.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & objIfIndexValue,
      &owaIfIndex.len);
  if (owaIfIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: IfDirection */
    owaIfDirection.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
        (xLibU8_t *) & objIfDirectionValue,
        &owaIfDirection.len);
    if(owaIfDirection.rc != XLIBRC_SUCCESS)
    {
      objIfDirectionValue = 0;
      nextObjIfDirectionValue = 0;
    }
  }
  else
  {
    objIfIndexValue = 0;
    nextObjIfIndexValue = 0;
    objIfDirectionValue = 0;
    nextObjIfDirectionValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (outIfIndex, &objIfIndexValue, owaIfIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outIfDirection, &objIfDirectionValue,
      owaIfDirection.len);

  owa.rc =
    usmDbDiffServServiceGetNext (L7_UNIT_CURRENT, objIfIndexValue, objIfDirectionValue,
        &nextObjIfIndexValue, &nextObjIfDirectionValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outIfIndex, owaIfIndex);
    FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (outIfIndex, &nextObjIfIndexValue, owaIfIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outIfDirection, &nextObjIfDirectionValue,
      owaIfDirection.len);

  /* return the object value: IfIndex */
  xLibBufDataSet (outIfIndex,
      (xLibU8_t *) & nextObjIfIndexValue,
      sizeof (nextObjIfIndexValue));

  /* return the object value: IfDirection */
  xLibBufDataSet (outIfDirection,
      (xLibU8_t *) & nextObjIfDirectionValue,
      sizeof (nextObjIfDirectionValue));
  FPOBJ_TRACE_EXIT (outIfIndex, owaIfIndex);
  FPOBJ_TRACE_EXIT (outIfDirection, owaIfDirection);
  return XLIBRC_SUCCESS;
}

#endif

/*******************************************************************************
 * @function fpObjGet_qosDiffServService_IfIndex
 *
 * @purpose Get 'IfIndex'
 *
 * @description [IfIndex] External interface number of the interface to which policy is assigned.
 *
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_IfIndex (void *wap, void *bufp)
{

  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjIfIndexValue = 0;
    /* objIfIndexValue = objIfDirectionValue = 0;
       nextObjIfIndexValue = nextObjIfDirectionValue = 0;
       owa.l7rc = usmDbDiffServServiceGetNext(L7_UNIT_CURRENT,
       objIfIndexValue,
       objIfDirectionValue, &nextObjIfIndexValue,
       &nextObjIfDirectionValue);*/
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, USM_LAG_MEMBER_INTF, &nextObjIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);
    /* objIfDirectionValue = 0;
       nextObjIfDirectionValue = objIfDirectionValue;
       do
       {
       objIfDirectionValue = nextObjIfDirectionValue;
       owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
       objIfIndexValue,
       objIfDirectionValue, &nextObjIfIndexValue,
       &nextObjIfDirectionValue);
       }
       while ((objIfIndexValue == nextObjIfIndexValue) && (owa.l7rc == L7_SUCCESS)); */
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF, USM_LAG_MEMBER_INTF, objIfIndexValue, &nextObjIfIndexValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfIndexValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfIndexValue, sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_qosDiffServService_IfDirection
 *
 * @purpose Get 'IfDirection'
 *
 * @description [IfDirection] Interface direction to which the policy is assigned.
 *
 * @notes
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_IfDirection (void *wap, void *bufp)
{

  xLibU32_t objIfIndexValue;
  xLibU32_t nextObjIfIndexValue;
  xLibU32_t objIfDirectionValue;
  xLibU32_t nextObjIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & objIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfIndexValue, owa.len);

  /* retrieve key: IfDirection */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & objIfDirectionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIfDirectionValue = 0;
    owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
        objIfIndexValue,
        objIfDirectionValue, &nextObjIfIndexValue,
        &nextObjIfDirectionValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfDirectionValue, owa.len);

    owa.l7rc = usmDbDiffServServiceGetNext (L7_UNIT_CURRENT,
        objIfIndexValue,
        objIfDirectionValue, &nextObjIfIndexValue,
        &nextObjIfDirectionValue);

  }

  if ((objIfIndexValue != nextObjIfIndexValue) || (owa.l7rc != L7_SUCCESS))
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
 * @function fpObjGet_qosDiffServService_PolicyIndex
 *
 * @purpose Get 'PolicyIndex'
 *
 * @description [PolicyIndex]: Index of the Policy Table row whose policy definition
 *              is attached to the interface in the specified direction.
 *              
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_PolicyIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue ;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue ;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  keyIfIndexValue = keyIfDirectionValue = objPolicyIndexValue = 0;
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServicePolicyIndexGet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue,
        &objPolicyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyIndexValue,
      sizeof (objPolicyIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServService_PolicyIndex
 *
 * @purpose Set 'PolicyIndex'
 *
 * @description [PolicyIndex]: Index of the Policy Table row whose policy definition
 *              is attached to the interface in the specified direction.
 *              
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_PolicyIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyIndexValue;
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue; 

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PolicyIndex */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyIndexValue, owa.len);

  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServService_RowStatus,
        (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);


  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /*make sure policy is existing */

  if (usmDbDiffServPolicyGet(L7_UNIT_CURRENT, objPolicyIndexValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServServicePolicyIndexSet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue,
        objPolicyIndexValue);
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
 * @function fpObjGet_qosDiffServService_IfOperStatus
 *
 * @purpose Get 'IfOperStatus'
 *
 * @description [IfOperStatus]: The current operational state of the DiffServ
 *              service interface. If agentDiffServGenStatusAdminMode is
 *              disable(2) then agentDiffServServiceIfOperStatus should be
 *              down(2). If agentDiffServServiceGenStatusAdminMode is ch 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_IfOperStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfOperStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);  

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServiceIfOperStatusGet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue,
        &objIfOperStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IfOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIfOperStatusValue,
      sizeof (objIfOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServService_StorageType
 *
 * @purpose Get 'StorageType'
 *
 * @description [StorageType]: Storage-type for this conceptual row 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_StorageType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServServiceStorageTypeGet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue,
        &objStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStorageTypeValue,
      sizeof (objStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServService_StorageType
 *
 * @purpose Set 'StorageType'
 *
 * @description [StorageType]: Storage-type for this conceptual row 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_StorageType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageTypeValue;
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStorageTypeValue, owa.len);

  /* retrieve row status object value */
  owa.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServService_RowStatus,
        (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    return XLIBRC_SUCCESS;
  }

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  /* set the value in application */
  owa.l7rc =
    usmDbDiffServServiceStorageTypeSet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue,
        objStorageTypeValue);
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
 * @function fpObjGet_qosDiffServService_RowStatus
 *
 * @purpose Get 'RowStatus'
 *
 * @description [RowStatus]: The status of this conceptual row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServServiceRowStatusGet (L7_UNIT_CURRENT, keyIfIndexValue,
      keyIfDirectionValue,
      &objRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue,
      sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServService_RowStatus
 *
 * @purpose Set 'RowStatus'
 *
 * @description [RowStatus]: The status of this conceptual row. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);


  /* call the usmdb only for add and delete */
  if ( (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
  {
    /* Create a row */
    if (usmDbDiffServServiceCreate(USMDB_UNIT_CURRENT, keyIfIndexValue,
          keyIfDirectionValue,   	                                                     
          ((objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ? L7_TRUE : L7_FALSE)) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    owa.l7rc =
      usmDbDiffServServiceRowStatusSet (L7_UNIT_CURRENT, keyIfIndexValue,
          keyIfDirectionValue, objRowStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    if (usmDbDiffServServiceDelete(L7_UNIT_CURRENT,keyIfIndexValue, keyIfDirectionValue ) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    owa.l7rc =
      usmDbDiffServServiceRowStatusSet (L7_UNIT_CURRENT, keyIfIndexValue,
          keyIfDirectionValue, objRowStatusValue);
    if (owa.l7rc != L7_SUCCESS && owa.l7rc!=L7_ERROR)
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


/*********************************************************************
 *
 * @purpose Get the next policy index
 *
 * @param    unit         the unit index
 * @param    polType      the policy type
 * @param    prevPolIndex index of the previous policy, zero gets the first policy index
 * @param   *nextPolIndex next policy index
 *
 * @returns L7_SUCCESS, L7_ERROR or L7_FAILURE
 *
 * @end
 *
 *********************************************************************/
xLibRC_t fpObjUtil_qosDiffServService_PolicyIndexGetNext(L7_uint32 unit, L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t polType, L7_uint32 prevPolIndex, L7_uint32 * nextPolIndex)
{
  L7_uint32 policyIndex = 0;
  L7_uint32 policyType;
  L7_RC_t rc;
  L7_BOOL policyFound;

  policyFound = L7_FALSE;

  rc = usmDbDiffServPolicyGetNext(unit, prevPolIndex, &policyIndex);

  while ((rc == L7_SUCCESS) && (policyFound == L7_FALSE))
  {
    rc = usmDbDiffServPolicyTypeGet(unit, policyIndex, &policyType);
    if (policyType == polType)
    {
      policyFound = L7_TRUE;
      *nextPolIndex = policyIndex;
    }
    else
    {
      prevPolIndex = policyIndex;
      rc = usmDbDiffServPolicyGetNext(unit, prevPolIndex, &policyIndex);
    }
  }

  if(rc != L7_SUCCESS)
    return XLIBRC_FAILURE;
  else
    return XLIBRC_SUCCESS;

}
#if 0
/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS LATER*/

xLibRC_t fpObjList_qosDiffServService_PolicyName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objPolicyNameList  ;
  xLibStr256_t nextPolicyNameList  ;


  xLibU32_t objPolicyIndexValueNext;
  L7_uint32  len = 0;

  xLibBool_t isFirstTime = XLIB_FALSE;

  memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));

  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_PolicyName ,
      (xLibU8_t *) objPolicyNameList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList));
    isFirstTime = XLIB_TRUE;
  }

  /*Get the next valid class name */
  /*1 Convert name to index. 2. get Next valid index. 3. convert classindex to class name. push to fileter
   */

  if(isFirstTime == XLIB_TRUE )
  {
    objPolicyIndexValueNext = 0;
  }
  else
  {
    /*get the index of the class retrieved by the filterget */
    owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
        objPolicyNameList,
        &objPolicyIndexValueNext);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }
  /*Currently we are supporting only IN*/
  if (fpObjUtil_qosDiffServService_PolicyIndexGetNext(L7_UNIT_CURRENT,
        L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN, 
        objPolicyIndexValueNext, &objPolicyIndexValueNext) != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  len = sizeof(nextPolicyNameList);
  memset(nextPolicyNameList, 0, sizeof(nextPolicyNameList));
  usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT, objPolicyIndexValueNext, nextPolicyNameList, &len);

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)nextPolicyNameList, len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS LATER*/

xLibRC_t fpObjGet_qosDiffServService_PolicyName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t selPolicyName  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

  xLibU32_t tempPolicyIndex, len = 0;
	xLibStr256_t objPolicyNameList;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);  

	memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));
  
  owa.l7rc = usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, 
      keyIfIndexValue, (L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t) keyIfDirectionValue, &tempPolicyIndex);
  if (owa.l7rc == L7_SUCCESS)
  {
    len = sizeof(selPolicyName);
    memset(selPolicyName, 0x00, sizeof(selPolicyName));
    owa.l7rc = usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT,tempPolicyIndex, selPolicyName, &len);
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) selPolicyName, len);
    FPOBJ_TRACE_EXIT (bufp, owa);
    owa.rc = XLIBRC_SUCCESS;
    return owa.rc;
  }
  return XLIBRC_FAILURE;
	
}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS OBJECT LATER*/

xLibRC_t fpObjSet_qosDiffServService_PolicyName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPolicyName  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;

  xLibU32_t tempPolicyIndex;
  xLibU32_t status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);
  /*Direction is always IN*/
#if 0
  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);  
#endif

  /* retrieve object: AddToMemberClassList */
  memset(objPolicyName, 0x00, sizeof(objPolicyName));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objPolicyName, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objPolicyName, owa.len);

  /*get the index of the policy retrieved by the filterget */
  owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
      objPolicyName,
      &tempPolicyIndex);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbDiffServServiceGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue) != L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServiceCreate(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, L7_TRUE);
    if (owa.l7rc != L7_SUCCESS)
    {
      /* delete the row entry just created */
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  /* set the policy index */
  if(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServicePolicyIndexSet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, tempPolicyIndex);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  if(owa.l7rc == L7_SUCCESS)
  {
    /* make sure row status becomes active */
    status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    owa.l7rc= usmDbDiffServServiceRowStatusVerify(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, status);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
      /* Log error message*/
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS OBJECT LATER*/

xLibRC_t fpObjList_qosDiffServService_MemberClassList (void *wap, 
    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMemberClassList  ;
  xLibStr256_t nextMemberClassList  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t PolicyIndexValue;
  xLibU32_t keyIfIndexValue;
  xLibU32_t	keyIfDirectionValue  ;
  xLibU32_t classIndexValueNext;
  L7_uint32  policyInstId, len = 0;

  xLibBool_t isFirstTime = XLIB_FALSE;
  xLibBool_t entryFound = XLIB_FALSE;
  L7_BOOL lookForNextAdd = L7_TRUE;


  /* retrieve key: PolicyIndex */
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);


  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);
  memset(objMemberClassList, 0x00, sizeof(
        objMemberClassList ));

  owa.rc = xLibFilterGet (wap, 
      XOBJ_qosDiffServService_MemberClassList ,
      (xLibU8_t *) objMemberClassList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objMemberClassList, 0x00, sizeof(
          objMemberClassList));
    isFirstTime = XLIB_TRUE;
  }
  if(usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue,&PolicyIndexValue)!= L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /*Get the next valid class name */
  /*1 Convert name to index. 2. get Next valid index. 3. convert classindex to 
    class name. push to fileter
   */

  if(isFirstTime == XLIB_TRUE )
  {
    classIndexValueNext = 0;
  }
  else
  {
    /*get the index of the class retrieved by the filterget */
    owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, 
        objMemberClassList,
        &classIndexValueNext);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }

  while (lookForNextAdd == L7_TRUE)
  {   
    if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, &
          classIndexValueNext) == L7_SUCCESS)
    {
      policyInstId = 0;
      if( usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
            PolicyIndexValue, classIndexValueNext, &policyInstId) == L7_SUCCESS )
      {
        len = sizeof(nextMemberClassList);
        memset(nextMemberClassList, 0, sizeof(
              nextMemberClassList));
        usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
            nextMemberClassList, &len);
        entryFound = XLIB_TRUE;
        break;
      }
    }
    else
    {
      lookForNextAdd = L7_FALSE;
    }
  }

  if(entryFound != XLIB_TRUE )
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextMemberClassList, 
      len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS OBJECT LATER*/

xLibRC_t fpObjGet_qosDiffServService_MemberClassList (void *wap, 
    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t nextMemberClassList  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t PolicyIndexValue;

  xLibU32_t classIndexValueNext;
  L7_uint32  policyInstId, len = 0;
  xLibU32_t keyIfIndexValue;
  xLibU32_t	keyIfDirectionValue  ;
  L7_BOOL lookForNextAdd = L7_TRUE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);


  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  if(usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue,&PolicyIndexValue)!= L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  classIndexValueNext = 0;

  while (lookForNextAdd == L7_TRUE)
  {   
    if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, &
          classIndexValueNext) == L7_SUCCESS)
    {
      policyInstId = 0;
      if( usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
            PolicyIndexValue, classIndexValueNext, &policyInstId) == L7_SUCCESS )
      {
        len = sizeof(nextMemberClassList);
        memset(nextMemberClassList, 0, sizeof(
              nextMemberClassList));
        usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext,nextMemberClassList, &len);
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) 
            nextMemberClassList,len);
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
    else
    {
      lookForNextAdd = L7_FALSE;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS OBJECT LATER*/

xLibRC_t fpObjSet_qosDiffServService_MemberClassList(void *wap, void 
*bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMemberClassList;

  xLibU32_t PolicyIndexValue;

  L7_uint32  policyInstId, classIndexValueTemp;
	xLibU32_t keyIfIndexValue;
  xLibU32_t	keyIfDirectionValue;
	  xLibU32_t classIndexValueNext;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);


  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  if(usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue,&PolicyIndexValue)!= L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  classIndexValueNext = 0;


  /* retrieve object: AddToMemberClassList */
  memset(objMemberClassList, 0x00, sizeof(objMemberClassList));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objMemberClassList, &
owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objMemberClassList, owa.len);

  owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, 
                                       objMemberClassList,
                                      &classIndexValueTemp);
  if (owa.l7rc != L7_SUCCESS)
   {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  if (usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
PolicyIndexValue, 
		         classIndexValueTemp, &policyInstId) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS LATER*/

xLibRC_t fpObjList_qosDiffServService_PolicyNameList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objPolicyNameList  ;
  xLibStr256_t nextPolicyNameList  ;


  xLibU32_t objPolicyIndexValueNext;
  L7_uint32  len = 0;

  xLibBool_t isFirstTime = XLIB_FALSE;

  memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));

  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_PolicyNameList ,
      (xLibU8_t *) objPolicyNameList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList));
    isFirstTime = XLIB_TRUE;
  }

  /*Get the next valid class name */
  /*1 Convert name to index. 2. get Next valid index. 3. convert classindex to class name. push to fileter
   */

  if(isFirstTime == XLIB_TRUE )
  {
    objPolicyIndexValueNext = 0;
  }
  else
  {
    /*get the index of the class retrieved by the filterget */
    owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
        objPolicyNameList,
        &objPolicyIndexValueNext);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }
  /*Currently we are supporting only IN*/
  if (fpObjUtil_qosDiffServService_PolicyIndexGetNext(L7_UNIT_CURRENT,
        L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN, 
        objPolicyIndexValueNext, &objPolicyIndexValueNext) != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  len = sizeof(nextPolicyNameList);
  memset(nextPolicyNameList, 0, sizeof(nextPolicyNameList));
  usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT, objPolicyIndexValueNext, nextPolicyNameList, &len);

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)nextPolicyNameList, len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS LATER*/

xLibRC_t fpObjGet_qosDiffServService_PolicyNameList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t selPolicyName  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfDirectionValue;

  xLibU32_t tempPolicyIndex, len = 0;
	xLibStr256_t objPolicyNameList;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);  

	memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));
  
  owa.l7rc = usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, 
      keyIfIndexValue, (L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t) keyIfDirectionValue, &tempPolicyIndex);
  if (owa.l7rc == L7_SUCCESS)
  {
    len = sizeof(selPolicyName);
    memset(selPolicyName, 0x00, sizeof(selPolicyName));
    owa.l7rc = usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT,tempPolicyIndex, selPolicyName, &len);
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) selPolicyName, len);
    FPOBJ_TRACE_EXIT (bufp, owa);
    owa.rc = XLIBRC_SUCCESS;
    return owa.rc;
  }
  return XLIBRC_FAILURE;
	
}

/*THIS IS A REDUNDANT HANDLER.SIMILAR ONE IS PRESENT IN DIFFSERV MODULE. NEED 
TO SEE IF WE CAN REMOVE THIS OBJECT LATER*/

xLibRC_t fpObjSet_qosDiffServService_PolicyNameList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPolicyName  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;

  xLibU32_t tempPolicyIndex;
  xLibU32_t status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);
  /*Direction is always IN*/
#if 0
  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);  
#endif

  /* retrieve object: AddToMemberClassList */
  memset(objPolicyName, 0x00, sizeof(objPolicyName));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objPolicyName, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objPolicyName, owa.len);

  /*get the index of the policy retrieved by the filterget */
  owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
      objPolicyName,
      &tempPolicyIndex);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbDiffServServiceGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue) != L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServiceCreate(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, L7_TRUE);
    if (owa.l7rc != L7_SUCCESS)
    {
      /* delete the row entry just created */
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  /* set the policy index */
  if(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServicePolicyIndexSet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, tempPolicyIndex);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  if(owa.l7rc == L7_SUCCESS)
  {
    /* make sure row status becomes active */
    status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    owa.l7rc= usmDbDiffServServiceRowStatusVerify(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, status);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
      /* Log error message*/
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjList_qosDiffServService_InPolicyNameList
 *
 * @purpose List InPolicyNameList
 *
 * @description Lists the inbound policies currently configured.
 *
 * @return L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServService_InPolicyNameList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objPolicyNameList  ;
  xLibStr256_t nextPolicyNameList  ;


  xLibU32_t objPolicyIndexValueNext;
  L7_uint32  len = 0;

  xLibBool_t isFirstTime = XLIB_FALSE;

  memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));

  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_InPolicyNameList,
      (xLibU8_t *) objPolicyNameList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList));
    isFirstTime = XLIB_TRUE;
  }

  /*Get the next valid class name */
  /*1 Convert name to index. 2. get Next valid index. 3. convert classindex to class name. push to fileter
   */

  if(isFirstTime == XLIB_TRUE )
  {
    objPolicyIndexValueNext = 0;
  }
  else
  {
    /*get the index of the class retrieved by the filterget */
    owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
        objPolicyNameList,
        &objPolicyIndexValueNext);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }
  /*Currently we are supporting only IN*/
  if (fpObjUtil_qosDiffServService_PolicyIndexGetNext(L7_UNIT_CURRENT,
        L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN, 
        objPolicyIndexValueNext, &objPolicyIndexValueNext) != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  len = sizeof(nextPolicyNameList);
  memset(nextPolicyNameList, 0, sizeof(nextPolicyNameList));
  usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT, objPolicyIndexValueNext, nextPolicyNameList, &len);

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)nextPolicyNameList, len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServService_InPolicyNameList
 *
 * @purpose Get InPolicyNameList
 *
 * @description Get the inbound policies currently configured. 
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_InPolicyNameList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t selPolicyName  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  xLibU32_t tempPolicyIndex, len = 0;
	xLibStr256_t objPolicyNameList;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

	memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));
  
  owa.l7rc = usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, 
      keyIfIndexValue, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN, &tempPolicyIndex);
  if (owa.l7rc == L7_SUCCESS)
  {
    len = sizeof(selPolicyName);
    memset(selPolicyName, 0x00, sizeof(selPolicyName));
    owa.l7rc = usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT,tempPolicyIndex, selPolicyName, &len);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) selPolicyName, len);
    FPOBJ_TRACE_EXIT (bufp, owa);
    owa.rc = XLIBRC_SUCCESS;
    return owa.rc;
  }
  return XLIBRC_FAILURE;
}

/*******************************************************************************
 * @function fpObjList_qosDiffServService_OutPolicyNameList
 *
 * @purpose List OutPolicyNameList
 *
 * @description Lists the outbound policies currently configured
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjList_qosDiffServService_OutPolicyNameList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objPolicyNameList  ;
  xLibStr256_t nextPolicyNameList  ;


  xLibU32_t objPolicyIndexValueNext;
  L7_uint32  len = 0;

  xLibBool_t isFirstTime = XLIB_FALSE;

  memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));

  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_OutPolicyNameList,
      (xLibU8_t *) objPolicyNameList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList));
    isFirstTime = XLIB_TRUE;
  }

  /*Get the next valid class name */
  /*1 Convert name to index. 2. get Next valid index. 3. convert classindex to class name. push to fileter
   */

  if(isFirstTime == XLIB_TRUE )
  {
    objPolicyIndexValueNext = 0;
  }
  else
  {
    /*get the index of the class retrieved by the filterget */
    owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
        objPolicyNameList,
        &objPolicyIndexValueNext);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }
  /*Currently we are supporting only IN*/
  if (fpObjUtil_qosDiffServService_PolicyIndexGetNext(L7_UNIT_CURRENT,
        L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT, 
        objPolicyIndexValueNext, &objPolicyIndexValueNext) != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  len = sizeof(nextPolicyNameList);
  memset(nextPolicyNameList, 0, sizeof(nextPolicyNameList));
  usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT, objPolicyIndexValueNext, nextPolicyNameList, &len);

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)nextPolicyNameList, len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServService_OutPolicyNameList
 *
 * @purpose Get OutPolicyNameList
 *
 * @description Get the outbound policies currently configured
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_OutPolicyNameList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t selPolicyName  ;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;

  xLibU32_t tempPolicyIndex, len = 0;
	xLibStr256_t objPolicyNameList;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

	memset(objPolicyNameList, 0x00, sizeof(objPolicyNameList ));
  
  owa.l7rc = usmDbDiffServServicePolicyIndexGet(L7_UNIT_CURRENT, 
      keyIfIndexValue, L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT, &tempPolicyIndex);
  if (owa.l7rc == L7_SUCCESS)
  {
    len = sizeof(selPolicyName);
    memset(selPolicyName, 0x00, sizeof(selPolicyName));
    owa.l7rc = usmDbDiffServPolicyNameGet(L7_UNIT_CURRENT,tempPolicyIndex, selPolicyName, &len);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) selPolicyName, len);
    FPOBJ_TRACE_EXIT (bufp, owa);
    owa.rc = XLIBRC_SUCCESS;
    return owa.rc;
  }
 return XLIBRC_FAILURE;
}

/*******************************************************************************
 * @function fpObjGet_qosDiffServService_GlobalDirection
 *
 * @purpose Get 'GlobalDirection'
 *
 * @description All supported directions.
 *
 * @return L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServService_GlobalDirection (void *wap, void *bufp)
{

  xLibU32_t objDirectionGlobalAllValue;
  xLibU32_t nextObjDirectionGlobalAllValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DirectionGlobalAll */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_GlobalDirection,
      (xLibU8_t *) & objDirectionGlobalAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjDirectionGlobalAllValue = L7_DIFFSERV_POLICY_TYPE_IN;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    if (objDirectionGlobalAllValue == L7_DIFFSERV_POLICY_TYPE_IN)
    {
        if (usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
        {
        nextObjDirectionGlobalAllValue = L7_DIFFSERV_POLICY_TYPE_OUT;
        owa.l7rc = L7_SUCCESS;
    }
    else
    {
        owa.l7rc = L7_FAILURE;
    }
  }
    else
    {
        owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDirectionGlobalAllValue, owa.len);

  /* return the object value: DirectionGlobalAll */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDirectionGlobalAllValue,
      sizeof (objDirectionGlobalAllValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServService_InPolicyAdd
 *
 * @purpose Add specified Policy
 *
 * @description Adds the given outbound policy on the specified interface
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_InPolicyAdd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInPolicyAdd=0;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyPolicyName;

  xLibU32_t tempPolicyIndex;
  xLibU32_t status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);
  /*Direction is always IN*/
#if 0
  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);  
#endif

  /* retrieve key: InPolicyName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_InPolicyNameList,
      (xLibU8_t *) & keyPolicyName, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyName, kwa2.len);

  /* retrieve object: objInPolicyAdd */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objInPolicyAdd, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objInPolicyAdd, owa.len);

  /*get the index of the policy retrieved by the filterget */
  owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
      keyPolicyName,
      &tempPolicyIndex);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbDiffServServiceGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue) != L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServiceCreate(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, L7_TRUE);
    if (owa.l7rc != L7_SUCCESS)
    {
      /* delete the row entry just created */
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  /* set the policy index */
  if(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServicePolicyIndexSet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, tempPolicyIndex);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  if(owa.l7rc == L7_SUCCESS)
  {
    /* make sure row status becomes active */
    status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    owa.l7rc= usmDbDiffServServiceRowStatusVerify(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, status);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
      /* Log error message*/
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServService_InPolicyDel
 *
 * @purpose Deletes specified Policy
 *
 * @description Deletes selected policy to all interfaces in given direction
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_InPolicyDel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  owa.l7rc = usmDbDiffServServiceDelete(L7_UNIT_CURRENT,keyIfIndexValue, keyIfDirectionValue );
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ERROR)
  {
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

#if 0
  owa.l7rc =
    usmDbDiffServServiceRowStatusSet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue, L7_ROW_STATUS_DESTROY);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc!=L7_ERROR)
  {
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
  }
#endif
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServService_OutPolicyAdd
 *
 * @purpose Adds specified out bound Policy
 *
 * @description Adds the given outbound policy on the specified interface
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_OutPolicyAdd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInPolicyAdd=0;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyPolicyName;

  xLibU32_t tempPolicyIndex;
  xLibU32_t status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);
  /*Direction is always IN*/
#if 0
  /* retrieve key: IfDirection */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfDirection,
      (xLibU8_t *) & keyIfDirectionValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfDirectionValue, kwa2.len);  
#endif

  /* retrieve key: InPolicyName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_OutPolicyNameList,
      (xLibU8_t *) & keyPolicyName, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyName, kwa2.len);

  /* retrieve object: objInPolicyAdd */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objInPolicyAdd, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInPolicyAdd, owa.len);

  /*get the index of the policy retrieved by the filterget */
  owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
      keyPolicyName,
      &tempPolicyIndex);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbDiffServServiceGet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue) != L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServiceCreate(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, L7_TRUE);
    if (owa.l7rc != L7_SUCCESS)
    {
      /* delete the row entry just created */
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  /* set the policy index */
  if(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServicePolicyIndexSet(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, tempPolicyIndex);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
    }
  }

  if(owa.l7rc == L7_SUCCESS)
  {
    /* make sure row status becomes active */
    status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    owa.l7rc= usmDbDiffServServiceRowStatusVerify(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue, status);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, keyIfDirectionValue);
      /* Log error message*/
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServService_OutPolicyDel
 *
 * @purpose Delete specified outbound policy
 *
 * @description Deletes the given outbound policy on the specified interface
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_OutPolicyDel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue;
  xLibU32_t keyIfDirectionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_IfIndex,
      (xLibU8_t *) & keyIfIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIfIndexValue, kwa1.len);

  owa.l7rc = usmDbDiffServServiceDelete(L7_UNIT_CURRENT,keyIfIndexValue, keyIfDirectionValue );
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ERROR)
  {
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
#if 0
  owa.l7rc =
    usmDbDiffServServiceRowStatusSet (L7_UNIT_CURRENT, keyIfIndexValue,
        keyIfDirectionValue, L7_ROW_STATUS_DESTROY);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc!=L7_ERROR)
  {
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
  }
#endif
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServService_GlobalPolicyAdd
 *
 * @purpose Add specified policy for all interfaces
 *
 * @description Adds selected policy to all interfaces in given direction
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_GlobalPolicyAdd (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalPolicyAdd=0;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue=L7_ALL_INTERFACES;
  xLibU32_t keyGlobalDirectionValue = L7_DIFFSERV_POLICY_TYPE_IN;
  xLibU32_t directionValue=L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyPolicyName;

  xLibU32_t tempPolicyIndex;
  xLibU32_t status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: GlobalDirectionValue */
  kwa1.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_GlobalDirection,
      (xLibU8_t *) & keyGlobalDirectionValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyGlobalDirectionValue, kwa1.len);

  if (keyGlobalDirectionValue == L7_DIFFSERV_POLICY_TYPE_IN)
  {
    /* retrieve key: InPolicyName */
    kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_InPolicyNameList,
        (xLibU8_t *) & keyPolicyName, &kwa2.len);
    directionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  }
  else if(keyGlobalDirectionValue == L7_DIFFSERV_POLICY_TYPE_OUT)
  {
    /* retrieve key: InPolicyName */
    kwa2.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_OutPolicyNameList,
        (xLibU8_t *) & keyPolicyName, &kwa2.len);
    directionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
  }
    if (kwa2.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyName, kwa2.len);

  /* retrieve object: objGlobalPolicyAdd */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objGlobalPolicyAdd, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalPolicyAdd, owa.len);

  /*get the index of the policy retrieved by the filterget */
  owa.l7rc = usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, 
      keyPolicyName,
      &tempPolicyIndex);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (usmDbDiffServServiceGet(L7_UNIT_CURRENT, keyIfIndexValue, directionValue) != L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServiceCreate(L7_UNIT_CURRENT, keyIfIndexValue, directionValue, L7_TRUE);
    if (owa.l7rc != L7_SUCCESS)
    {
      /* delete the row entry just created */
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, directionValue);
    }
  }

  /* set the policy index */
  if(owa.l7rc == L7_SUCCESS)
  {
    owa.l7rc = usmDbDiffServServicePolicyIndexSet(L7_UNIT_CURRENT, keyIfIndexValue, directionValue, tempPolicyIndex);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, directionValue);
    }
  }

  if(owa.l7rc == L7_SUCCESS)
  {
    /* make sure row status becomes active */
    status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    owa.l7rc= usmDbDiffServServiceRowStatusVerify(L7_UNIT_CURRENT, keyIfIndexValue, directionValue, status);
    if (owa.l7rc != L7_SUCCESS)
    {
      (void)usmDbDiffServServiceDelete(L7_UNIT_CURRENT, keyIfIndexValue, directionValue);
      /* Log error message*/
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * @function fpObjSet_qosDiffServService_GlobalPolicyDel
 *
 * @purpose Deletes specified policy for all interfaces 
 *
 * @Description Deletes selected policy to all interfaces in given direction
 *
 * @return  L7_SUCCESS or L7_FAILURE
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServService_GlobalPolicyDel (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIfIndexValue=L7_ALL_INTERFACES;
  xLibU32_t keyDirectionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  xLibU32_t directionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Direction */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServService_GlobalDirection,
      (xLibU8_t *) & keyDirectionValue, &owa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDirectionValue, owa.len);

  if (keyDirectionValue == L7_DIFFSERV_POLICY_TYPE_IN)
    directionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  else if(keyDirectionValue == L7_DIFFSERV_POLICY_TYPE_OUT)
    directionValue = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;

  owa.l7rc = usmDbDiffServServiceDelete(L7_UNIT_CURRENT,keyIfIndexValue, directionValue);
  if (owa.l7rc != L7_SUCCESS && owa.l7rc != L7_ERROR)
  {
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

