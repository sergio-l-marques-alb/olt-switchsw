/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfHost.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfHosts-object.xml
*
* @create  28 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfHost_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfHost_IpAddress
*
* @purpose Get 'IpAddress'
 *@description  [IpAddress] The IP Address of the Host.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfHost_IpAddress (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddressValue;
  xLibIpV4_t nextObjIpAddressValue;
  xLibU32_t objTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objIpAddressValue, 0, sizeof (objIpAddressValue));
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    owa.l7rc = usmDbOspfHostEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objTOSValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);
    nextObjIpAddressValue = objIpAddressValue;
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    do
    {
      owa.l7rc = usmDbOspfHostEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objTOSValue);
    }
    while ((objIpAddressValue == nextObjIpAddressValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjIpAddressValue = objIpAddressValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIpAddressValue, owa.len);

  /* return the object value: IpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIpAddressValue, sizeof (objIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfHost_TOS
*
* @purpose Get 'TOS'
 *@description  [TOS] The Type of Service of the route being configured.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfHost_TOS (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddressValue;
  xLibU32_t nextObjIpAddressValue;
  xLibU32_t objTOSValue;
  xLibU32_t nextObjTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);

  /* retrieve key: TOS */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfHost_TOS, (xLibU8_t *) & objTOSValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objTOSValue, 0, sizeof (objTOSValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTOSValue, owa.len);
  }

  nextObjIpAddressValue = objIpAddressValue;
  owa.l7rc = usmDbOspfHostEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objTOSValue);
  
  if ((objIpAddressValue != nextObjIpAddressValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjTOSValue = objTOSValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTOSValue, owa.len);

  /* return the object value: TOS */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjTOSValue, sizeof (objTOSValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_routingospfHost_IpAddress_TOS
*
* @purpose Get 'IpAddress + TOS +'
*
* @description [IpAddress]: The IP Address of the Host. 
*              [TOS]: The Type of Service of the route being configured. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfHost_IpAddress_TOS (void *wap, void *bufp[],
                                                 xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaIpAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpAddressValue, nextObjIpAddressValue;
  fpObjWa_t owaTOS = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTOSValue, nextObjTOSValue;
  void *outIpAddress = (void *) bufp[--keyCount];
  void *outTOS = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outIpAddress);
  FPOBJ_TRACE_ENTER (outTOS);

  /* retrieve key: IpAddress */
  owaIpAddress.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                                   (xLibU8_t *) & objIpAddressValue,
                                   &owaIpAddress.len);
  if (owaIpAddress.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: TOS */
    owaTOS.rc = xLibFilterGet (wap, XOBJ_routingospfHost_TOS,
                               (xLibU8_t *) & objTOSValue, &owaTOS.len);
  }
  else
  {
    objIpAddressValue = 0;
    nextObjIpAddressValue = 0;
    objTOSValue = 0;
    nextObjTOSValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (outIpAddress, &objIpAddressValue, owaIpAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outTOS, &objTOSValue, owaTOS.len);

  owa.rc = usmDbOspfHostEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objTOSValue);

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outIpAddress, owaIpAddress);
    FPOBJ_TRACE_EXIT (outTOS, owaTOS);
    return owa.rc;
  }

  nextObjIpAddressValue = objIpAddressValue;
  nextObjTOSValue = objTOSValue;

  FPOBJ_TRACE_CURRENT_KEY (outIpAddress, &nextObjIpAddressValue,
                           owaIpAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outTOS, &nextObjTOSValue, owaTOS.len);

  /* return the object value: IpAddress */
  xLibBufDataSet (outIpAddress,
                  (xLibU8_t *) & nextObjIpAddressValue,
                  sizeof (nextObjIpAddressValue));

  /* return the object value: TOS */
  xLibBufDataSet (outTOS,
                  (xLibU8_t *) & nextObjTOSValue, sizeof (nextObjTOSValue));
  FPOBJ_TRACE_EXIT (outIpAddress, owaIpAddress);
  FPOBJ_TRACE_EXIT (outTOS, owaTOS);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_routingospfHost_Metric
*
* @purpose Get 'Metric'
*
* @description [Metric]: The Metric to be advertised. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfHost_Metric (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                           (xLibU8_t *) & keyIpAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddressValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfHost_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfHostMetricGet (L7_UNIT_CURRENT, keyIpAddressValue,
                                     keyTOSValue, &objMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Metric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMetricValue,
                           sizeof (objMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfHost_Metric
*
* @purpose Set 'Metric'
*
* @description [Metric]: The Metric to be advertised. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfHost_Metric (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Metric */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMetricValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricValue, owa.len);

  /* retrieve key: IpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                           (xLibU8_t *) & keyIpAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddressValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfHost_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfHostMetricSet (L7_UNIT_CURRENT, keyIpAddressValue,
                                     keyTOSValue, objMetricValue);
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
* @function fpObjGet_routingospfHost_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfHost_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                           (xLibU8_t *) & keyIpAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddressValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfHost_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfHostStatusGet (L7_UNIT_CURRENT, keyIpAddressValue,
                                     keyTOSValue, &objStatusValue);
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


/*******************************************************************************
* @function fpObjSet_routingospfHost_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfHost_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: IpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                           (xLibU8_t *) & keyIpAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddressValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfHost_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfHostStatusSet (L7_UNIT_CURRENT, keyIpAddressValue,
                                     keyTOSValue, objStatusValue);
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
* @function fpObjGet_routingospfHost_AreaID
*
* @purpose Get 'AreaID'
*
* @description [AreaID]: The Area the Host Entry is to be found within. By
*              default, the area that a subsuming OSPF in- terface is in,
*              or 0.0.0.0 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfHost_AreaID (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfHost_IpAddress,
                           (xLibU8_t *) & keyIpAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddressValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfHost_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfHostAreaIDGet (L7_UNIT_CURRENT, keyIpAddressValue,
                                     keyTOSValue, &objAreaIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AreaID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAreaIDValue,
                           sizeof (objAreaIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
