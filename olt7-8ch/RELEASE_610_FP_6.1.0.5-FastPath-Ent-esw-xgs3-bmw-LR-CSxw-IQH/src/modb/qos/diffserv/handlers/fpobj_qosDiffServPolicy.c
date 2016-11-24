/*******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 ********************************************************************************
 *
 * @filename fpobj_qosDiffServPolicy.c
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
#include "_xe_qosDiffServPolicy_obj.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_mib_diffserv_common.h"

xLibRC_t xLibFilterSet (void *arg, xLibId_t oid, xLibU16_t type, xLibU8_t * val, xLibU16_t 
len);
/*******************************************************************************
 * @function fpObjGet_qosDiffServPolicy_PolicyIndex
 *
 * @purpose Get 'PolicyIndex'
 *
 * @description [PolicyIndex]: The identifier for DiffServ Policy table entry.
 *              
 *
 * @note  This is KEY Object
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicy_PolicyIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyIndexValue;
  xLibU32_t nextObjPolicyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & objPolicyIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPolicyIndexValue = 0;
    nextObjPolicyIndexValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPolicyIndexValue, owa.len);
  owa.l7rc =		
    usmDbDiffServPolicyGetNext (L7_UNIT_CURRENT, objPolicyIndexValue,
        &nextObjPolicyIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if(nextObjPolicyIndexValue == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;
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
 * @function fpObjGet_qosDiffServPolicy_PolicyName
 *
 * @purpose Get 'PolicyName'
 *
 * @description [PolicyName]: Name of the policy 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicy_PolicyName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPolicyNameValue;
  xLibU32_t stringLength = L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* get the value from application */
  memset(objPolicyNameValue,0x00,sizeof(objPolicyNameValue));
  owa.l7rc = usmDbDiffServPolicyNameGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
      objPolicyNameValue, &stringLength);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPolicyNameValue,
      strlen (objPolicyNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServPolicy_PolicyName
 *
 * @purpose Set 'PolicyName'
 *
 * @description [PolicyName]: Name of the policy 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicy_PolicyName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPolicyNameValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue=0;
  xLibU32_t newPolicyId;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRowStatusValue = 0;

  /*retrun success if its a row status is create_and Go */
  kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyRowStatus,
	(xLibU8_t *) &keyRowStatusValue, &kwaRowStatus.len);
  if (kwaRowStatus.rc == XLIBRC_SUCCESS)
  {
	  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRowStatusValue, kwaRowStatus.len);

	  if ( (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
	      (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
	  {
	     return XLIBRC_SUCCESS;
	  }
  }
  else
  {
	  /* retrieve object: PolicyName */
	  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objPolicyNameValue, &owa.len);
	  if (owa.rc != XLIBRC_SUCCESS)
	  {
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }
	  FPOBJ_TRACE_VALUE (bufp, objPolicyNameValue, owa.len);

	  /* retrieve key: PolicyIndex */
	  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
	      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
	  if (kwa.rc != XLIBRC_SUCCESS)
	  {
	    kwa.rc = XLIBRC_FILTER_MISSING;
	    FPOBJ_TRACE_EXIT (bufp, kwa);
	    return kwa.rc;
	  }
	  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

	  /* check if the new name of the policy already exists or not */
	  if (usmDbDiffServPolicyNameToIndex(L7_UNIT_CURRENT, objPolicyNameValue, &newPolicyId) == L7_SUCCESS)
	  {
	    if(newPolicyId != keyPolicyIndexValue)
	    {
		    owa.rc = XLIBRC_POLICY_NAME_EXISTS;    /* TODO: Change if required */
		    FPOBJ_TRACE_EXIT (bufp, owa);
		    return owa.rc;
	    }
	  }
  /* set the value in application */
  owa.l7rc = usmDbDiffServPolicyNameSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
      objPolicyNameValue);
  }
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
 * @function fpObjGet_qosDiffServPolicy_PolicyType
 *
 * @purpose Get 'PolicyType'
 *
 * @description [PolicyType]:  The policy type dictates whether inbound or
 *              outbound policy attribute statements are used in the policy
 *              definition. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicy_PolicyType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDiffServPolicyTypeGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
      &objPolicyTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyTypeValue,
      sizeof (objPolicyTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServPolicy_PolicyType
 *
 * @purpose Set 'PolicyType'
 *
 * @description [PolicyType]:  The policy type dictates whether inbound or
 *              outbound policy attribute statements are used in the policy
 *              definition. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicy_PolicyType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  fpObjWa_t kwaRowStatus = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRowStatusValue = 0;

  /*retrun success if its a row status is create_and Go */
  kwaRowStatus.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyRowStatus,
	(xLibU8_t *) &keyRowStatusValue, &kwaRowStatus.len);
  if (kwaRowStatus.rc != XLIBRC_SUCCESS)
  {
	kwaRowStatus.rc = XLIBRC_FILTER_MISSING;
	FPOBJ_TRACE_EXIT (bufp, kwaRowStatus);
	return kwaRowStatus.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRowStatusValue, kwaRowStatus.len);

  if ( (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (keyRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
  {
     return XLIBRC_SUCCESS;
  }

  /* retrieve object: PolicyType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPolicyTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyTypeValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDiffServPolicyTypeSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
      objPolicyTypeValue);
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
 * @function fpObjGet_qosDiffServPolicy_PolicyInstIndexNextFree
 *
 * @purpose Get 'PolicyInstIndexNextFree'
 *
 * @description [PolicyInstIndexNextFree]: This object contains an unused value
 *              for agentDiffServPolicyInstIndex, or a zero to indicate
 *              that none exists. This index is maintained on a per-policy
 *              basis. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicy_PolicyInstIndexNextFree (void *wap,
    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyInstIndexNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyInstIndexNext (L7_UNIT_CURRENT, keyPolicyIndexValue,
        &objPolicyInstIndexNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objPolicyInstIndexNextFreeValue == 0)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyInstIndexNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyInstIndexNextFreeValue,
      sizeof (objPolicyInstIndexNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjGet_qosDiffServPolicy_PolicyStorageType
 *
 * @purpose Get 'PolicyStorageType'
 *
 * @description [PolicyStorageType]: Storage-type for this conceptual row 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicy_PolicyStorageType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyStorageTypeGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
        &objPolicyStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyStorageTypeValue,
      sizeof (objPolicyStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServPolicy_PolicyStorageType
 *
 * @purpose Set 'PolicyStorageType'
 *
 * @description [PolicyStorageType]: Storage-type for this conceptual row 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicy_PolicyStorageType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyStorageTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PolicyStorageType */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objPolicyStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyStorageTypeValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDiffServPolicyStorageTypeSet (L7_UNIT_CURRENT, keyPolicyIndexValue,
        objPolicyStorageTypeValue);
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
 * @function fpObjGet_qosDiffServPolicy_PolicyRowStatus
 *
 * @purpose Get 'PolicyRowStatus'
 *
 * @description [PolicyRowStatus]: The status of this conceptual row. Entries
 *              can not be deleted until all rows in agentDiffServPolicyInstTable
 *              with corresponding values of agentDiffServPolicyIndex
 *              have been deleted. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_qosDiffServPolicy_PolicyRowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServPolicyRowStatusGet (L7_UNIT_CURRENT, keyPolicyIndexValue,
        &objPolicyRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PolicyRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPolicyRowStatusValue,
      sizeof (objPolicyRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_qosDiffServPolicy_PolicyRowStatus
 *
 * @purpose Set 'PolicyRowStatus'
 *
 * @description [PolicyRowStatus]: The status of this conceptual row. Entries
 *              can not be deleted until all rows in agentDiffServPolicyInstTable
 *              with corresponding values of agentDiffServPolicyIndex
 *              have been deleted. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_qosDiffServPolicy_PolicyRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPolicyRowStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  fpObjWa_t kwaPolicyName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t kwaPolicyNameValue;

  fpObjWa_t kwaPolicyType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyTypeValue = 0;

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PolicyRowStatus */
  owa.rc = xLibBufDataGet (bufp,
      (xLibU8_t *) & objPolicyRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPolicyRowStatusValue, owa.len);

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    keyPolicyIndexValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  /* call the usmdb only for add and delete */
  if ( (objPolicyRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ||
      (objPolicyRowStatusValue == L7_ROW_STATUS_CREATE_AND_WAIT) )
  {
    owa.l7rc = usmDbDiffServPolicyIndexNext(L7_UNIT_CURRENT, &keyPolicyIndexValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Add error string */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
		
    if (usmDbDiffServPolicyCreate(L7_UNIT_CURRENT, keyPolicyIndexValue, 
          ((objPolicyRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO) ? L7_TRUE : L7_FALSE)) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

	  /* get the class name and class type value */
	  kwaPolicyName.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyName,
	                              (xLibU8_t *) kwaPolicyNameValue, &kwaPolicyName.len);
	  if (kwaPolicyName.rc != XLIBRC_SUCCESS)
	  {
		kwaPolicyName.rc = XLIBRC_FILTER_MISSING;
		FPOBJ_TRACE_EXIT (bufp, kwaPolicyName);
		return kwaPolicyName.rc;
	  }
	  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaPolicyNameValue, kwaPolicyName.len);

	  /* get the policy name and policy type value */
	  kwaPolicyType.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyType,
	             (xLibU8_t *) & keyPolicyTypeValue, &kwaPolicyType.len);
	  if (kwaPolicyType.rc != XLIBRC_SUCCESS)
	  {
		keyPolicyTypeValue = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;
	  }
	  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyTypeValue, kwaPolicyType.len);
	  owa.l7rc = usmDbDiffServPolicyNameSet(L7_UNIT_CURRENT, 
			                    keyPolicyIndexValue, kwaPolicyNameValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyDelete(L7_UNIT_CURRENT, keyPolicyIndexValue);
		 owa.rc = XLIBRC_POLICY_NAME_EXISTS;  /* TODO: Change if required */
		 FPOBJ_TRACE_EXIT (bufp, owa);
		 return owa.rc;
      }

      /* set the policy type*/
	  owa.l7rc = usmDbDiffServPolicyTypeSet(L7_UNIT_CURRENT, 
	                       keyPolicyIndexValue, keyPolicyTypeValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        /* delete the row entry just created*/
        usmDbDiffServPolicyDelete(L7_UNIT_CURRENT, keyPolicyIndexValue);
		 owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
		 FPOBJ_TRACE_EXIT (bufp, owa);
		 return owa.rc;
      }

      /* make sure row status becomes active */
      owa.l7rc = usmDbDiffServPolicyRowStatusGet(L7_UNIT_CURRENT,
                               keyPolicyIndexValue, &status);
      if ( (owa.l7rc != L7_SUCCESS) || (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE) )
      {
        usmDbDiffServPolicyDelete(L7_UNIT_CURRENT, keyPolicyIndexValue);
		 owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
		 FPOBJ_TRACE_EXIT (bufp, owa);
		 return owa.rc;
      }
		
	   owa.rc = xLibFilterSet(wap, XOBJ_qosDiffServPolicy_PolicyIndex, 0, 
		 	(xLibU8_t *) &keyPolicyIndexValue, sizeof(keyPolicyIndexValue));		 
		if(owa.rc  != XLIBRC_SUCCESS)
		{
	        usmDbDiffServPolicyDelete(L7_UNIT_CURRENT, keyPolicyIndexValue);
			 owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
			 FPOBJ_TRACE_EXIT (bufp, owa);
			 return owa.rc;
		}
  }
  else if (objPolicyRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
	    /* Delete the existing row */
	  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
	      (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
	  if (kwa.rc != XLIBRC_SUCCESS)
	  {
	    kwa.rc = XLIBRC_FILTER_MISSING;
	    FPOBJ_TRACE_EXIT (bufp, kwa);
	    return kwa.rc;
	   }
		if ( usmDbDiffServPolicyDelete(L7_UNIT_CURRENT, keyPolicyIndexValue) != L7_SUCCESS )
		{
		  owa.rc = XLIBRC_DIFFSERV_POLICY_DELETE_FAILED; 
		  FPOBJ_TRACE_EXIT (bufp, owa);
		  return owa.rc;
		}

	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjList_qosDiffServPolicy_AddToMemberClassList (void *wap, void *
bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAddToMemberClassList  ;
  xLibStr256_t nextAddToMemberClassList  ;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

   xLibU32_t classIndexValueNext;
   L7_uint32  policyInstId, len = 0;

	xLibBool_t isFirstTime = XLIB_FALSE;
	xLibBool_t entryFound = XLIB_FALSE;
	L7_BOOL lookForNextAdd = L7_TRUE;

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  memset(objAddToMemberClassList, 0x00, sizeof(objAddToMemberClassList ));

  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_AddToMemberClassList ,
                          (xLibU8_t *) objAddToMemberClassList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objAddToMemberClassList, 0x00, sizeof(objAddToMemberClassList));
	isFirstTime = XLIB_TRUE;
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
                                       objAddToMemberClassList,
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
	      if ((usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
keyPolicyIndexValue, classIndexValueNext, &policyInstId) != L7_SUCCESS) &&
	          (usmDbDiffServClassValidForPolicyCheck(L7_UNIT_CURRENT, 
classIndexValueNext, keyPolicyIndexValue) == L7_SUCCESS))
	      {
	        len = sizeof(nextAddToMemberClassList);
	        memset(nextAddToMemberClassList, 0, sizeof(nextAddToMemberClassList));
	        usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
nextAddToMemberClassList, &len);
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
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)nextAddToMemberClassList,
                           len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_qosDiffServPolicy_AddToMemberClassList (void *wap, void *
bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t nextAddToMemberClassList  ;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

   xLibU32_t classIndexValueNext;
   L7_uint32  policyInstId, len = 0;

	L7_BOOL lookForNextAdd = L7_TRUE;
	
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  classIndexValueNext = 0;

  while (lookForNextAdd == L7_TRUE)
  {   
	  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, &
classIndexValueNext) == L7_SUCCESS)
	  {
	      if ((usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
keyPolicyIndexValue, classIndexValueNext, &policyInstId) != L7_SUCCESS) &&
	          (usmDbDiffServClassValidForPolicyCheck(L7_UNIT_CURRENT, 
classIndexValueNext, keyPolicyIndexValue) == L7_SUCCESS))
	      {
	          len = sizeof(nextAddToMemberClassList);
	          memset(nextAddToMemberClassList, 0, sizeof(nextAddToMemberClassList
));
	          usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
nextAddToMemberClassList, &len);
             owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) 
nextAddToMemberClassList,len);
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

xLibRC_t fpObjSet_qosDiffServPolicy_AddToMemberClassList(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAddToMemberClassList;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  L7_uint32  policyInstId, classIndexValueTemp;
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t status;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);


  /* retrieve object: AddToMemberClassList */
  memset(objAddToMemberClassList, 0x00, sizeof(objAddToMemberClassList));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)  objAddToMemberClassList, &owa.
len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAddToMemberClassList, owa.len);


  /*get the index of the class retrieved by the filterget */
	 owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, 
                                       objAddToMemberClassList,
                                      &classIndexValueTemp);
	  if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
     }


    if ( usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
keyPolicyIndexValue, 
			classIndexValueTemp, &policyInstId) == L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if ( usmDbDiffServPolicyInstIndexNext(L7_UNIT_CURRENT, keyPolicyIndexValue
, &policyInstId) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    /* create the row entry in the policy instance table */
    if (usmDbDiffServPolicyInstCreate(L7_UNIT_CURRENT, keyPolicyIndexValue, 
policyInstId, L7_TRUE) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    /* set the class index for the specified policy */
    if (usmDbDiffServPolicyInstClassIndexSet(L7_UNIT_CURRENT, 
keyPolicyIndexValue, policyInstId, classIndexValueTemp) != L7_SUCCESS)
    {
      /* delete the row entry just created*/
      usmDbDiffServPolicyInstDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, 
policyInstId);
	  owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyInstRowStatusGet(L7_UNIT_CURRENT, keyPolicyIndexValue, 
policyInstId, &status);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_qosDiffServPolicy_isClassPresent (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t owaIsClassPresent  ;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  xLibU32_t classIndexValueNext;
   L7_uint32  policyInstId;

	L7_BOOL lookForNextAdd = L7_TRUE;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  classIndexValueNext = 0;
  owaIsClassPresent = L7_FALSE;
  while (lookForNextAdd == L7_TRUE)
  {   
	  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, &
classIndexValueNext) == L7_SUCCESS)
	  {
	      if ((usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
keyPolicyIndexValue, classIndexValueNext, &policyInstId) != L7_SUCCESS) &&
	          (usmDbDiffServClassValidForPolicyCheck(L7_UNIT_CURRENT, 
classIndexValueNext, keyPolicyIndexValue) == L7_SUCCESS))
	      {
				owaIsClassPresent = L7_TRUE;
				break;
	      	}
  	 }
	 else
    {
      lookForNextAdd = L7_FALSE;
    }
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &owaIsClassPresent,sizeof(
owaIsClassPresent));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjList_qosDiffServPolicy_RemoveFromMemberClassList (void *wap, 
void *bufp)
{
   fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRemoveFromMemberClassList  ;
  xLibStr256_t nextRemoveFromMemberClassList  ;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue,tempPolicyIndex;

  xLibU32_t classIndex;
  L7_uint32 len = 0;
  static L7_uint32  policyInstId = 0;

  xLibBool_t entryFound = XLIB_FALSE;
  L7_BOOL lookForNextAdd = L7_TRUE;

  /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);
  tempPolicyIndex = keyPolicyIndexValue;

  memset(objRemoveFromMemberClassList, 0x00, sizeof(objRemoveFromMemberClassList));  
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_RemoveFromMemberClassList ,
                          (xLibU8_t *) objRemoveFromMemberClassList, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    policyInstId = 0;
  }
  if (usmDbDiffServPolicyInstGetNext(L7_UNIT_CURRENT, tempPolicyIndex, policyInstId,
                                         &tempPolicyIndex, &policyInstId) != L7_SUCCESS)
  {
    lookForNextAdd = L7_FALSE;
  } 
  while (lookForNextAdd == L7_TRUE && keyPolicyIndexValue == tempPolicyIndex)
  {  
    if (usmDbDiffServPolicyInstClassIndexGet(L7_UNIT_CURRENT, keyPolicyIndexValue,
                                             policyInstId, &classIndex) == L7_SUCCESS)
    {
      len = sizeof(nextRemoveFromMemberClassList);
      memset(nextRemoveFromMemberClassList, 0, len);
      if (usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndex, nextRemoveFromMemberClassList, &len) == L7_SUCCESS)
      {
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

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextRemoveFromMemberClassList, len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc; 
}


xLibRC_t fpObjGet_qosDiffServPolicy_RemoveFromMemberClassList (void *wap, 
void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t nextRemoveFromMemberClassList;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue,tempPolicyIndex;

  L7_uint32  policyInstId, len = 0;

  L7_BOOL lookForNextAdd = L7_TRUE;
  xLibU32_t classIndex;
	
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);
  tempPolicyIndex = keyPolicyIndexValue;
  policyInstId = 0;
  
  if (usmDbDiffServPolicyInstGetNext(L7_UNIT_CURRENT, tempPolicyIndex, policyInstId,
                                         &tempPolicyIndex, &policyInstId) != L7_SUCCESS)
  {
    lookForNextAdd = L7_FALSE;
  } 
  
  while (lookForNextAdd == L7_TRUE && keyPolicyIndexValue == tempPolicyIndex)
  {  
    if (usmDbDiffServPolicyInstClassIndexGet(L7_UNIT_CURRENT, keyPolicyIndexValue,
                                             policyInstId, &classIndex) == L7_SUCCESS)
    {
      len = sizeof(nextRemoveFromMemberClassList);
      memset(nextRemoveFromMemberClassList, 0, len);
      if (usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndex, nextRemoveFromMemberClassList, &len) == L7_SUCCESS)
      {
        owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)nextRemoveFromMemberClassList,len);
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


xLibRC_t fpObjSet_qosDiffServPolicy_RemoveFromMemberClassList(void *wap, void 
*bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRemoveFromMemberClassList;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  L7_uint32  policyInstId, classIndexValueTemp;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);


  /* retrieve object: AddToMemberClassList */
  memset(objRemoveFromMemberClassList, 0x00, sizeof(
objRemoveFromMemberClassList));
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *)  objRemoveFromMemberClassList, &
owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objRemoveFromMemberClassList, owa.len);

  owa.l7rc = usmDbDiffServClassNameToIndex(L7_UNIT_CURRENT, 
                                       objRemoveFromMemberClassList,
                                      &classIndexValueTemp);
  if (owa.l7rc != L7_SUCCESS)
   {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  if (usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
keyPolicyIndexValue, 
		         classIndexValueTemp, &policyInstId) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ( usmDbDiffServPolicyInstDelete(L7_UNIT_CURRENT, keyPolicyIndexValue, 
policyInstId) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


xLibRC_t fpObjGet_qosDiffServPolicy_isMemberPresent (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t owaIsMemberPresent  ;
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

  xLibU32_t classIndexValueNext;
   L7_uint32  policyInstId;

	L7_BOOL lookForNextAdd = L7_TRUE;

  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  classIndexValueNext = 0;
  owaIsMemberPresent = L7_FALSE;
  while (lookForNextAdd == L7_TRUE)
  {   
	   if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, &
classIndexValueNext) == L7_SUCCESS)
	  {
	      policyInstId = 0;
	      if( usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
keyPolicyIndexValue, classIndexValueNext, &policyInstId) == L7_SUCCESS )
	      {
	         owaIsMemberPresent = L7_TRUE;
			  break;
	      }
  	 }
	 else
    {
      lookForNextAdd = L7_FALSE;
    }
  }

  /* return the object value: GroupProtocolIPX */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &owaIsMemberPresent,sizeof(
owaIsMemberPresent));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



xLibRC_t fpObjGet_qosDiffServPolicy_PolicyMemberClassList (void *wap, void *
bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof(xLibStr256_t));
  xLibStr256_t nextPolicyMemberClassList ;

  char policyMemberClassList[500];
	
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPolicyIndexValue;

   xLibU32_t classIndexValueNext;
   L7_uint32  policyInstId, len = 0;

	L7_BOOL lookForNextAdd = L7_TRUE;
   FPOBJ_TRACE_ENTER (bufp);
   memset(policyMemberClassList, 0x00, sizeof(policyMemberClassList));

   /* retrieve key: PolicyIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServPolicy_PolicyIndex,
                          (xLibU8_t *) & keyPolicyIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPolicyIndexValue, kwa.len);

  classIndexValueNext = 0;

  while (lookForNextAdd == L7_TRUE)
  {   
	  if (usmDbDiffServClassGetNext(L7_UNIT_CURRENT, classIndexValueNext, &
classIndexValueNext) == L7_SUCCESS)
	  {
	      policyInstId = 0;
	      if( usmDbDiffServPolicyInstIndexFromClass(L7_UNIT_CURRENT, 
keyPolicyIndexValue, classIndexValueNext, &policyInstId) == L7_SUCCESS )
	      {
	          len = sizeof(nextPolicyMemberClassList);
				memset(nextPolicyMemberClassList, 0, sizeof(nextPolicyMemberClassList));
	          usmDbDiffServClassNameGet(L7_UNIT_CURRENT, classIndexValueNext, 
nextPolicyMemberClassList, &len);
			   osapiStrncat(policyMemberClassList,  nextPolicyMemberClassList, sizeof(
nextPolicyMemberClassList) );
			   osapiStrncat(policyMemberClassList,  ",", sizeof(",") );
	      }
  	 }
	 else
    {
      lookForNextAdd = L7_FALSE;
    }
  }
  len = strlen(policyMemberClassList);
  if(len > 1)
	 len = len-1; /* to remove the extra comma added */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) policyMemberClassList, len);
	
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
	
}

