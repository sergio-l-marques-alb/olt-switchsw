/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseadvancekey.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  12 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseadvancekey_obj.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_keying_api.h"

/*******************************************************************************
* @function fpObjGet_baseadvancekey_FeatureKeyingIndex
*
* @purpose Get 'FeatureKeyingIndex'
*
* @description A value corresponding to a keyable feature.When this table is 
*              walked, only values associated with keyable features are returned.This 
*              value must be equivalent to valid value of FeatureKeyingIndex. 
*              
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseadvancekey_FeatureKeyingIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFeatureKeyingIndexValue;
  xLibU32_t nextObjFeatureKeyingIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FeatureKeyingIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseadvancekey_FeatureKeyingIndex,
                          (xLibU8_t *) & objFeatureKeyingIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbFeatureGetFirst(L7_UNIT_CURRENT,&nextObjFeatureKeyingIndexValue) ;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objFeatureKeyingIndexValue, owa.len);	
    owa.l7rc =
      usmDbFeatureGetNext (L7_UNIT_CURRENT, objFeatureKeyingIndexValue,
                                  &nextObjFeatureKeyingIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjFeatureKeyingIndexValue, owa.len);

  /* return the object value: FeatureKeyingIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjFeatureKeyingIndexValue,
                           sizeof (objFeatureKeyingIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseadvancekey_FeatureKeyingName
*
* @purpose Get 'FeatureKeyingName'
*
* @description The abbreviated name of this component.This is also equivalent 
*              to FeatureKeyingName 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseadvancekey_FeatureKeyingName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyFeatureKeyingIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objFeatureKeyingNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FeatureKeyingIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseadvancekey_FeatureKeyingIndex,
                          (xLibU8_t *) & keyFeatureKeyingIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFeatureKeyingIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbComponentNameGet ( keyFeatureKeyingIndexValue,
                                    objFeatureKeyingNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FeatureKeyingName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objFeatureKeyingNameValue,
                           strlen (objFeatureKeyingNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseadvancekey_FeatureKeyingStatus
*
* @purpose Get 'FeatureKeyingStatus'
*
* @description Returns a value of (1) if the feature is enabled for management, 
*              (2) if disabled. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseadvancekey_FeatureKeyingStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyFeatureKeyingIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFeatureKeyingStatusValue;
  L7_BOOL mode;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: FeatureKeyingIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseadvancekey_FeatureKeyingIndex,
                          (xLibU8_t *) & keyFeatureKeyingIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyFeatureKeyingIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbFeatureKeyGet (L7_UNIT_CURRENT, keyFeatureKeyingIndexValue,
                                 &mode,&objFeatureKeyingStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FeatureKeyingStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFeatureKeyingStatusValue,
                           sizeof (objFeatureKeyingStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseadvancekey_FeatureKeyingDisableKey
*
* @purpose Set 'FeatureKeyingDisableKey'
*
* @description Hexadecimal Key-string entered to disable an advance functionality. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseadvancekey_FeatureKeyingDisableKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objFeatureKeyingDisableKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FeatureKeyingDisableKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objFeatureKeyingDisableKeyValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objFeatureKeyingDisableKeyValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbFeatureKeyLicenseValidate (L7_UNIT_CURRENT,
                                    objFeatureKeyingDisableKeyValue,L7_FALSE);
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
* @function fpObjSet_baseadvancekey_FeatureKeyingEnableKey
*
* @purpose Set 'FeatureKeyingEnableKey'
*
* @description Hexadecimal Key-string entered to enable an advance functionality. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseadvancekey_FeatureKeyingEnableKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objFeatureKeyingEnableKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FeatureKeyingEnableKey */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objFeatureKeyingEnableKeyValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objFeatureKeyingEnableKeyValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbFeatureKeyLicenseValidate (L7_UNIT_CURRENT,
                                    objFeatureKeyingEnableKeyValue,L7_TRUE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
