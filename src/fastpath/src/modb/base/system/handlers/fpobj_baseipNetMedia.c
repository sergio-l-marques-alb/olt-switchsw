/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseipNetMedia.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  11 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseipNetMedia_obj.h"
#include "usmdb_1213_api.h"
#if 0
/*******************************************************************************
* @function fpObjGet_baseipNetMedia_ipNetToMediaIfIndex_ipNetToMediaNetAddress
*
* @purpose Get 'ipNetToMediaIfIndex + ipNetToMediaNetAddress +'
*
* @description [ipNetToMediaIfIndex]: ToDO: Add Help 
*              [ipNetToMediaNetAddress]: ToDO: Add Help 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_baseipNetMedia_ipNetToMediaIfIndex_ipNetToMediaNetAddress (void *wap,
                                                                    void
                                                                    *bufp[],
                                                                    xLibU16_t
                                                                    keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaipNetToMediaIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipNetToMediaIfIndexValue, nextObjipNetToMediaIfIndexValue;
  fpObjWa_t owaipNetToMediaNetAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipNetToMediaNetAddressValue, nextObjipNetToMediaNetAddressValue;
  void *outipNetToMediaIfIndex = (void *) bufp[--keyCount];
  void *outipNetToMediaNetAddress = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outipNetToMediaIfIndex);
  FPOBJ_TRACE_ENTER (outipNetToMediaNetAddress);

  /* retrieve key: ipNetToMediaIfIndex */
  owaipNetToMediaIfIndex.rc =
    xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaIfIndex,
                   (xLibU8_t *) & objipNetToMediaIfIndexValue,
                   &owaipNetToMediaIfIndex.len);
  if (owaipNetToMediaIfIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: ipNetToMediaNetAddress */
    owaipNetToMediaNetAddress.rc =
      xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaNetAddress,
                     (xLibU8_t *) & objipNetToMediaNetAddressValue,
                     &owaipNetToMediaNetAddress.len);
  }
  else
  {
  objipNetToMediaIfIndexValue = 0;
  objipNetToMediaNetAddressValue = 0;
  nextObjipNetToMediaIfIndexValue = 0;
  nextObjipNetToMediaNetAddressValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (outipNetToMediaIfIndex, &objipNetToMediaIfIndexValue,
                           owaipNetToMediaIfIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outipNetToMediaNetAddress,
                           &objipNetToMediaNetAddressValue,
                           owaipNetToMediaNetAddress.len);
  owa.rc =
    usmDbIpNetToMediaEntryNextGet (L7_UNIT_CURRENT, &objipNetToMediaIfIndexValue,
                         &objipNetToMediaNetAddressValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outipNetToMediaIfIndex, owaipNetToMediaIfIndex);
    FPOBJ_TRACE_EXIT (outipNetToMediaNetAddress, owaipNetToMediaNetAddress);
    return owa.rc;
  }
  nextObjipNetToMediaIfIndexValue = objipNetToMediaIfIndexValue;
  nextObjipNetToMediaNetAddressValue = objipNetToMediaNetAddressValue;
  
  FPOBJ_TRACE_CURRENT_KEY (outipNetToMediaIfIndex,
                           &nextObjipNetToMediaIfIndexValue,
                           owaipNetToMediaIfIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outipNetToMediaNetAddress,
                           &nextObjipNetToMediaNetAddressValue,
                           owaipNetToMediaNetAddress.len);

  /* return the object value: ipNetToMediaIfIndex */
  xLibBufDataSet (outipNetToMediaIfIndex,
                  (xLibU8_t *) & nextObjipNetToMediaIfIndexValue,
                  sizeof (nextObjipNetToMediaIfIndexValue));

  /* return the object value: ipNetToMediaNetAddress */
  xLibBufDataSet (outipNetToMediaNetAddress,
                  (xLibU8_t *) & nextObjipNetToMediaNetAddressValue,
                  sizeof (nextObjipNetToMediaNetAddressValue));
  FPOBJ_TRACE_EXIT (outipNetToMediaIfIndex, owaipNetToMediaIfIndex);
  FPOBJ_TRACE_EXIT (outipNetToMediaNetAddress, owaipNetToMediaNetAddress);
  return XLIBRC_SUCCESS;
}

#endif


/*******************************************************************************
* @function fpObjGet_baseipNetMedia_ipNetToMediaIfIndex
*
* @purpose Get 'ipNetToMediaIfIndex'
*
* @description [ipNetToMediaIfIndex] ToDO: Add Help
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseipNetMedia_ipNetToMediaIfIndex (void *wap, void *bufp)
{

  xLibU32_t objipNetToMediaIfIndexValue;
  xLibU32_t nextObjipNetToMediaIfIndexValue;
  xLibIpV4_t objipNetToMediaNetAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipNetToMediaIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaIfIndex,
                          (xLibU8_t *) & objipNetToMediaIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objipNetToMediaIfIndexValue = objipNetToMediaNetAddressValue = 0;
    owa.l7rc =
      usmDbIpNetToMediaEntryNextGet (L7_UNIT_CURRENT, &objipNetToMediaIfIndexValue,
                          &objipNetToMediaNetAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipNetToMediaIfIndexValue, owa.len);
    nextObjipNetToMediaIfIndexValue = objipNetToMediaIfIndexValue;
    do
    {
      objipNetToMediaNetAddressValue = 0;
      owa.l7rc = usmDbIpNetToMediaEntryNextGet (L7_UNIT_CURRENT, &objipNetToMediaIfIndexValue,
                          &objipNetToMediaNetAddressValue);
    }
    while ((objipNetToMediaIfIndexValue == nextObjipNetToMediaIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  nextObjipNetToMediaIfIndexValue = objipNetToMediaIfIndexValue; 
  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipNetToMediaIfIndexValue, owa.len);

  /* return the object value: ipNetToMediaIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipNetToMediaIfIndexValue,
                           sizeof (objipNetToMediaIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseipNetMedia_ipNetToMediaNetAddress
*
* @purpose Get 'ipNetToMediaNetAddress'
*
* @description [ipNetToMediaNetAddress] ToDO: Add Help
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseipNetMedia_ipNetToMediaNetAddress (void *wap, void *bufp)
{

  xLibU32_t objipNetToMediaIfIndexValue;
  xLibIpV4_t nextObjipNetToMediaIfIndexValue;
  xLibIpV4_t objipNetToMediaNetAddressValue;
  xLibIpV4_t nextObjipNetToMediaNetAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipNetToMediaIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaIfIndex,
                          (xLibU8_t *) & objipNetToMediaIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipNetToMediaIfIndexValue, owa.len);

  /* retrieve key: ipNetToMediaNetAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaNetAddress,
                          (xLibU8_t *) objipNetToMediaNetAddressValue, &owa.len);

  nextObjipNetToMediaIfIndexValue = objipNetToMediaIfIndexValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objipNetToMediaNetAddressValue = 0;
    owa.l7rc =
      usmDbIpNetToMediaEntryNextGet (L7_UNIT_CURRENT, &objipNetToMediaIfIndexValue,
                          &objipNetToMediaNetAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipNetToMediaNetAddressValue, owa.len);

    owa.l7rc =
      usmDbIpNetToMediaEntryNextGet (L7_UNIT_CURRENT, &objipNetToMediaIfIndexValue,
                          &objipNetToMediaNetAddressValue);
  }

  if ((objipNetToMediaIfIndexValue != nextObjipNetToMediaIfIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjipNetToMediaNetAddressValue = objipNetToMediaNetAddressValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipNetToMediaNetAddressValue, owa.len);

  /* return the object value: ipNetToMediaNetAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjipNetToMediaNetAddressValue,
                           sizeof (objipNetToMediaNetAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseipNetMedia_ipNetToMediaPhysAddress
*
* @purpose Get 'ipNetToMediaPhysAddress'
*
* @description [ipNetToMediaPhysAddress]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseipNetMedia_ipNetToMediaPhysAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaNetAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objipNetToMediaPhysAddressValue;
  xLibU32_t addrLength;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipNetToMediaIfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaIfIndex,
                           (xLibU8_t *) & keyipNetToMediaIfIndexValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaIfIndexValue, kwa1.len);

  /* retrieve key: ipNetToMediaNetAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaNetAddress,
                           (xLibU8_t *) & keyipNetToMediaNetAddressValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaNetAddressValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbIpNetToMediaPhysAddressGet (L7_UNIT_CURRENT,
                                     keyipNetToMediaIfIndexValue,
                                     keyipNetToMediaNetAddressValue,
                                     objipNetToMediaPhysAddressValue, &addrLength);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipNetToMediaPhysAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objipNetToMediaPhysAddressValue,
                           strlen (objipNetToMediaPhysAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseipNetMedia_ipNetToMediaPhysAddress
*
* @purpose Set 'ipNetToMediaPhysAddress'
*
* @description [ipNetToMediaPhysAddress]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseipNetMedia_ipNetToMediaPhysAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objipNetToMediaPhysAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaNetAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipNetToMediaPhysAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objipNetToMediaPhysAddressValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objipNetToMediaPhysAddressValue, owa.len);

  /* retrieve key: ipNetToMediaIfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaIfIndex,
                           (xLibU8_t *) & keyipNetToMediaIfIndexValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaIfIndexValue, kwa1.len);

  /* retrieve key: ipNetToMediaNetAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaNetAddress,
                           (xLibU8_t *) & keyipNetToMediaNetAddressValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaNetAddressValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbIpNetToMediaPhysAddressSet (L7_UNIT_CURRENT,
                                     keyipNetToMediaIfIndexValue,
                                     keyipNetToMediaNetAddressValue,
                                     objipNetToMediaPhysAddressValue,
                                     sizeof(objipNetToMediaPhysAddressValue));
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
* @function fpObjGet_baseipNetMedia_ipNetToMediaType
*
* @purpose Get 'ipNetToMediaType'
*
* @description [ipNetToMediaType]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseipNetMedia_ipNetToMediaType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaNetAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipNetToMediaTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipNetToMediaIfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaIfIndex,
                           (xLibU8_t *) & keyipNetToMediaIfIndexValue,
                           &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaIfIndexValue, kwa1.len);

  /* retrieve key: ipNetToMediaNetAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaNetAddress,
                           (xLibU8_t *) & keyipNetToMediaNetAddressValue,
                           &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaNetAddressValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbIpNetToMediaTypeGet (L7_UNIT_CURRENT, keyipNetToMediaIfIndexValue,
                              keyipNetToMediaNetAddressValue,
                              &objipNetToMediaTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipNetToMediaType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipNetToMediaTypeValue,
                           sizeof (objipNetToMediaTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseipNetMedia_ipNetToMediaType
*
* @purpose Set 'ipNetToMediaType'
*
* @description [ipNetToMediaType]: ToDO: Add Help 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseipNetMedia_ipNetToMediaType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipNetToMediaTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaIfIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipNetToMediaNetAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipNetToMediaType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objipNetToMediaTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipNetToMediaTypeValue, owa.len);

  /* retrieve key: ipNetToMediaIfIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaIfIndex,
                           (xLibU8_t *) & keyipNetToMediaIfIndexValue,
                           &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaIfIndexValue, kwa1.len);

  /* retrieve key: ipNetToMediaNetAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseipNetMedia_ipNetToMediaNetAddress,
                           (xLibU8_t *) & keyipNetToMediaNetAddressValue,
                           &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipNetToMediaNetAddressValue, kwa2.len);

  /* set the value in application */
  owa.l7rc =
    usmDbIpNetToMediaTypeSet (L7_UNIT_CURRENT, keyipNetToMediaIfIndexValue,
                              keyipNetToMediaNetAddressValue,
                              objipNetToMediaTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
