/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingConfigRip2Interface.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to RipConfig-object.xml
*
* @create  21 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingConfigRip2Interface_obj.h"
#include "usmdb_mib_ripv2_api.h"

/*******************************************************************************
* @function fpObjGet_routingConfigRip2Interface_ConfAddress
*
* @purpose Get 'ConfAddress'
*
* @description [ConfAddress]: The IP Address of this system on the indicated
*              subnet. For unnumbered interfaces, the value 0.0.0.N, where
*              the least significant 24 bits (N) is the ifIndex for the
*              IP Interface in network byte order. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2Interface_ConfAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfAddressValue;
  xLibU32_t nextObjConfAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ConfAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2Interface_ConfAddress,
                          (xLibU8_t *) & objConfAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjConfAddressValue = 0;
    owa.l7rc = usmDbRip2IfConfEntryNext(L7_UNIT_CURRENT, &nextObjConfAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objConfAddressValue, owa.len);
    owa.l7rc = usmDbRip2IfConfEntryNext(L7_UNIT_CURRENT, &objConfAddressValue);
    nextObjConfAddressValue = objConfAddressValue;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjConfAddressValue, owa.len);

  /* return the object value: ConfAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjConfAddressValue,
                           sizeof (objConfAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingConfigRip2Interface_ConfAuthKeyId
*
* @purpose Get 'ConfAuthKeyId'
*
* @description [ConfAuthKeyId]: The identifier for the authentication key
*              used on this interface. This field is only meaningful when
*              the RIPv2-MIB rip2IfConfAuthType is md5(3); otherwise, the
*              value is not used. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRip2Interface_ConfAuthKeyId (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyConfAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfAuthKeyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ConfAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2Interface_ConfAddress,
                          (xLibU8_t *) & keyConfAddressValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConfAddressValue, kwa.len);

  #if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyConfAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  #endif
  /* get the value from application */
  owa.l7rc = usmDbRip2IfConfAuthKeyIdGet (L7_UNIT_CURRENT, keyConfAddressValue,
                                          &objConfAuthKeyIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ConfAuthKeyId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConfAuthKeyIdValue,
                           sizeof (objConfAuthKeyIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRip2Interface_ConfAuthKeyId
*
* @purpose Set 'ConfAuthKeyId'
*
* @description [ConfAuthKeyId]: The identifier for the authentication key
*              used on this interface. This field is only meaningful when
*              the RIPv2-MIB rip2IfConfAuthType is md5(3); otherwise, the
*              value is not used. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRip2Interface_ConfAuthKeyId (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objConfAuthKeyIdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyConfAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ConfAuthKeyId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objConfAuthKeyIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objConfAuthKeyIdValue, owa.len);

  /* retrieve key: ConfAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRip2Interface_ConfAddress,
                          (xLibU8_t *) & keyConfAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConfAddressValue, kwa.len);

  #if 0
  if (usmDbRip2IfIPAddrToIntIf(L7_UNIT_CURRENT, keyConfAddressValue, &interface) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  #endif
  /* set the value in application */
  owa.l7rc = usmDbRip2IfConfAuthKeyIdSet (L7_UNIT_CURRENT, keyConfAddressValue,
                                          objConfAuthKeyIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
