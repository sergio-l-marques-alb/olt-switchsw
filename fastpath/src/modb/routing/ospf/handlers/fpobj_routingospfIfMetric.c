/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfIfMetric.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfIfMetric-object.xml
*
* @create  29 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfIfMetric_obj.h"
#include "usmdb_mib_ospf_api.h"


/*******************************************************************************
* @function fpObjGet_routingospfIfMetric_IpAddress
*
* @purpose Get 'IpAddress'
 *@description  [IpAddress] he IP address of this OSPF interface. On row
* creation, this can be derived from the instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfIfMetric_IpAddress (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddressValue;
  xLibIpV4_t nextObjIpAddressValue;
  xLibU32_t objAddressLessIfValue;
  xLibU32_t objTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_IpAddress,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objIpAddressValue, 0, sizeof (objIpAddressValue));
    memset (&objAddressLessIfValue, 0, sizeof (objAddressLessIfValue));
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    owa.l7rc = usmDbOspfIfMetricEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objAddressLessIfValue,
                         &objTOSValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);
    nextObjIpAddressValue = objIpAddressValue;
    memset (&objAddressLessIfValue, 0, sizeof (objAddressLessIfValue));
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    do
    {
      owa.l7rc = usmDbOspfIfMetricEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objAddressLessIfValue,
                         &objTOSValue);
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
* @function fpObjGet_routingospfIfMetric_AddressLessIf
*
* @purpose Get 'AddressLessIf'
 *@description  [AddressLessIf] For the purpose of easing the instancing of
* addressed and addressless interfaces; This variable takes the value 0
* on interfaces with IP Addresses, and the value of ifIndex for
* in- terfaces having no IP Address. On row crea- tion, this can be
* derived from the instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfIfMetric_AddressLessIf (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddressValue;
  xLibU32_t nextObjIpAddressValue;
  xLibU32_t objAddressLessIfValue;
  xLibU32_t nextObjAddressLessIfValue;
  xLibU32_t objTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_IpAddress,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);

  /* retrieve key: AddressLessIf */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_AddressLessIf,
                          (xLibU8_t *) & objAddressLessIfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    nextObjIpAddressValue = objIpAddressValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objAddressLessIfValue, 0, sizeof (objAddressLessIfValue));
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    owa.l7rc = usmDbOspfIfMetricEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objAddressLessIfValue,
                         &objTOSValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressLessIfValue, owa.len);
    nextObjIpAddressValue = objIpAddressValue;
    nextObjAddressLessIfValue = objAddressLessIfValue;
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    do
    {
      owa.l7rc = usmDbOspfIfMetricEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objAddressLessIfValue,
                         &objTOSValue);
    }
    while ((objIpAddressValue == nextObjIpAddressValue)
           && (objAddressLessIfValue == nextObjAddressLessIfValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objIpAddressValue != nextObjIpAddressValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjAddressLessIfValue = objAddressLessIfValue;
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAddressLessIfValue, owa.len);

  /* return the object value: AddressLessIf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAddressLessIfValue,
                           sizeof (objAddressLessIfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfIfMetric_TOS
*
* @purpose Get 'TOS'
 *@description  [TOS] The type of service metric being referenced. On row
* creation, this can be derived from the instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfIfMetric_TOS (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddressValue;
  xLibU32_t nextObjIpAddressValue;
  xLibU32_t objAddressLessIfValue;
  xLibU32_t nextObjAddressLessIfValue;
  xLibU32_t objTOSValue;
  xLibU32_t nextObjTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_IpAddress,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);

  /* retrieve key: AddressLessIf */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_AddressLessIf,
                          (xLibU8_t *) & objAddressLessIfValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressLessIfValue, owa.len);

  /* retrieve key: TOS */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_TOS, (xLibU8_t *) & objTOSValue, &owa.len);
  nextObjIpAddressValue = objIpAddressValue;
  nextObjAddressLessIfValue = objAddressLessIfValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    owa.l7rc = usmDbOspfIfMetricEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objAddressLessIfValue,
                         &objTOSValue);
    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTOSValue, owa.len);

    owa.l7rc = usmDbOspfIfMetricEntryNext(L7_UNIT_CURRENT, &objIpAddressValue, &objAddressLessIfValue,
                         &objTOSValue);

  }

  if ((objIpAddressValue != nextObjIpAddressValue)
      || (objAddressLessIfValue != nextObjAddressLessIfValue) || (owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_routingospfIfMetric_IpAddress_AddressLessIf_TOS
*
* @purpose Get 'IpAddress + AddressLessIf + TOS +'
*
* @description [IpAddress]: he IP address of this OSPF interface. On row creation,
*              this can be derived from the instance. 
*              [AddressLessIf]: For the purpose of easing the instancing of
*              addressed and addressless interfaces; This variable takes
*              the value 0 on interfaces with IP Addresses, and the value
*              of ifIndex for in- terfaces having no IP Address. On row crea-
*              tion, this can be derived from the instance. 
*              [TOS]: The type of service metric being referenced. On row
*              creation, this can be derived from the instance. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfIfMetric_IpAddress_AddressLessIf_TOS (void *wap,
                                                                   void *bufp[],
                                                                   xLibU16_t
                                                                   keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaIpAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpAddressValue, nextObjIpAddressValue;
  fpObjWa_t owaAddressLessIf = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressLessIfValue, nextObjAddressLessIfValue;
  fpObjWa_t owaTOS = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTOSValue, nextObjTOSValue;
  void *outIpAddress = (void *) bufp[--keyCount];
  void *outAddressLessIf = (void *) bufp[--keyCount];
  void *outTOS = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outIpAddress);
  FPOBJ_TRACE_ENTER (outAddressLessIf);
  FPOBJ_TRACE_ENTER (outTOS);

  /* retrieve key: IpAddress */
  owaIpAddress.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_IpAddress,
                                   (xLibU8_t *) & objIpAddressValue,
                                   &owaIpAddress.len);
  if (owaIpAddress.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: AddressLessIf */
    owaAddressLessIf.rc =
      xLibFilterGet (wap, XOBJ_routingospfIfMetric_AddressLessIf,
                     (xLibU8_t *) & objAddressLessIfValue,
                     &owaAddressLessIf.len);
    if (owaAddressLessIf.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: TOS */
      owaTOS.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_TOS,
                                 (xLibU8_t *) & objTOSValue, &owaTOS.len);
    }
  }
  else
  {
     objIpAddressValue =0;
     nextObjIpAddressValue = 0;
     objAddressLessIfValue = 0;
     nextObjAddressLessIfValue = 0;
     objTOSValue = 0;
     nextObjTOSValue = 0;
  }

  nextObjIpAddressValue = objIpAddressValue;
  nextObjAddressLessIfValue = objAddressLessIfValue;
  nextObjTOSValue = objTOSValue;

  FPOBJ_TRACE_CURRENT_KEY (outIpAddress, &objIpAddressValue, owaIpAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outAddressLessIf, &objAddressLessIfValue,
                           owaAddressLessIf.len);
  FPOBJ_TRACE_CURRENT_KEY (outTOS, &objTOSValue, owaTOS.len);

  owa.rc = usmDbOspfIfMetricEntryNext(L7_UNIT_CURRENT, &nextObjIpAddressValue, &nextObjAddressLessIfValue,
                         &nextObjTOSValue);


  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outIpAddress, owaIpAddress);
    FPOBJ_TRACE_EXIT (outAddressLessIf, owaAddressLessIf);
    FPOBJ_TRACE_EXIT (outTOS, owaTOS);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outIpAddress, &nextObjIpAddressValue,
                           owaIpAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outAddressLessIf, &nextObjAddressLessIfValue,
                           owaAddressLessIf.len);
  FPOBJ_TRACE_CURRENT_KEY (outTOS, &nextObjTOSValue, owaTOS.len);

  /* return the object value: IpAddress */
  xLibBufDataSet (outIpAddress,
                  (xLibU8_t *) & nextObjIpAddressValue,
                  sizeof (nextObjIpAddressValue));

  /* return the object value: AddressLessIf */
  xLibBufDataSet (outAddressLessIf,
                  (xLibU8_t *) & nextObjAddressLessIfValue,
                  sizeof (nextObjAddressLessIfValue));

  /* return the object value: TOS */
  xLibBufDataSet (outTOS,
                  (xLibU8_t *) & nextObjTOSValue, sizeof (nextObjTOSValue));
  FPOBJ_TRACE_EXIT (outIpAddress, owaIpAddress);
  FPOBJ_TRACE_EXIT (outAddressLessIf, owaAddressLessIf);
  FPOBJ_TRACE_EXIT (outTOS, owaTOS);
  return XLIBRC_SUCCESS;
}

#endif
/*******************************************************************************
* @function fpObjGet_routingospfIfMetric_Value
*
* @purpose Get 'Value'
*
* @description [Value]: The metric of using this type of service on this interface.
*              The default value of the TOS 0 Metric is 10^8 / ifSpeed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfIfMetric_Value (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIfValue;
  /*fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;*/
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLessIf */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_AddressLessIf,
                           (xLibU8_t *) & keyAddressLessIfValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIfValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfMetricValueGet (L7_UNIT_CURRENT, keyAddressLessIfValue,
                                        0, &objValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Value */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objValueValue,
                           sizeof (objValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfIfMetric_Value
*
* @purpose Set 'Value'
*
* @description [Value]: The metric of using this type of service on this interface.
*              The default value of the TOS 0 Metric is 10^8 / ifSpeed.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfIfMetric_Value (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objValueValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIfValue;
  /*fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;*/
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Value */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objValueValue, owa.len);

  /* retrieve key: AddressLessIf */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_AddressLessIf,
                           (xLibU8_t *) & keyAddressLessIfValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIfValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfIfMetricValueSet (L7_UNIT_CURRENT, 0,
                                        keyAddressLessIfValue,
                                        0, objValueValue);
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
* @function fpObjGet_routingospfIfMetric_Status
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
xLibRC_t fpObjGet_routingospfIfMetric_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIfValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AddressLessIf */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_AddressLessIf,
                           (xLibU8_t *) & keyAddressLessIfValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIfValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIfMetricStatusGet (L7_UNIT_CURRENT, 0,
                                         keyAddressLessIfValue,
                                         0, &objStatusValue);
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
* @function fpObjSet_routingospfIfMetric_Status
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
xLibRC_t fpObjSet_routingospfIfMetric_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIfValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
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
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_IpAddress,
                           (xLibU8_t *) & keyIpAddressValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddressValue, kwa1.len);

  /* retrieve key: AddressLessIf */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_AddressLessIf,
                           (xLibU8_t *) & keyAddressLessIfValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIfValue, kwa2.len);

  /* retrieve key: TOS */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfIfMetric_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa3.len);

  /* call the usmdb only for add and delete */

 /* WE ARE NOT SUPPORTING SET FUNCTIONS */ 
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
   #if 0
    /* Create a row */
    owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyIpAddressValue,
                                keyAddressLessIfValue,
                                keyTOSValue, objStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
   #endif
  
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
   #if 0
    owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyIpAddressValue,
                                keyAddressLessIfValue,
                                keyTOSValue, objStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
   #endif
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}
