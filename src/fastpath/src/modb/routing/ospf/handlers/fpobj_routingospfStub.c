/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfStub.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfStub-object.xml
*
* @create  2 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfStub_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfStub_AreaId
*
* @purpose Get 'AreaId'
 *@description  [AreaId] The 32 bit identifier for the Stub Area. On creation,
* this can be derived from the instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfStub_AreaId (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibU32_t objTOSValue;
  xLibU32_t nextObjTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  memset (&objAreaIdValue, 0, sizeof (objAreaIdValue));
  memset (&objTOSValue, 0, sizeof (objTOSValue));
  memset (&nextObjAreaIdValue, 0, sizeof (nextObjAreaIdValue));
  memset (&nextObjTOSValue, 0, sizeof (nextObjTOSValue));

  owa.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbOspfStubAreaEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjTOSValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);
    nextObjAreaIdValue = objAreaIdValue;
    memset (&objTOSValue, 0, sizeof (objTOSValue));
    do
    {
      owa.l7rc = usmDbOspfStubAreaEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjTOSValue);
    }
    while ((objAreaIdValue == nextObjAreaIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAreaIdValue, sizeof(nextObjAreaIdValue));

  /* return the object value: AreaId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAreaIdValue, sizeof (nextObjAreaIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfStub_TOS
*
* @purpose Get 'TOS'
*@description  [TOS] The Type of Service associated with the metric. On
* creation, this can be derived from the instance.   
* @notes       
*
* @return
********************************************************************************/
xLibRC_t fpObjGet_routingospfStub_TOS (void *wap, void *bufp)
{
  xLibIpV4_t objAreaIdValue;

  xLibU32_t objTOSValue;
  xLibU32_t nextObjTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  memset (&objAreaIdValue, 0, sizeof (objAreaIdValue));
  memset (&objTOSValue, 0, sizeof (objTOSValue));

  memset (&nextObjTOSValue, 0, sizeof (nextObjTOSValue));

  owa.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);

  /* retrieve key: TOS */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS, (xLibU8_t *) & objTOSValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjTOSValue = 0; 
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTOSValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTOSValue, sizeof(nextObjTOSValue));

  /* return the object value: TOS */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjTOSValue, sizeof (objTOSValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_routingospfStub_AreaId_TOS
*
* @purpose Get 'AreaId + TOS +'
*
* @description [AreaId]: The 32 bit identifier for the Stub Area. On creation,
*              this can be derived from the instance. 
*              [TOS]: The Type of Service associated with the metric. On creation,
*              this can be derived from the instance. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfStub_AreaId_TOS (void *wap, void *bufp[],
                                              xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaAreaId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue, nextObjAreaIdValue;
  fpObjWa_t owaTOS = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTOSValue, nextObjTOSValue;
  void *outAreaId = (void *) bufp[--keyCount];
  void *outTOS = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outAreaId);
  FPOBJ_TRACE_ENTER (outTOS);

  /* retrieve key: AreaId */
  owaAreaId.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                                (xLibU8_t *) & objAreaIdValue, &owaAreaId.len);
  if (owaAreaId.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: TOS */
    owaTOS.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS,
                               (xLibU8_t *) & objTOSValue, &owaTOS.len);
  }
  else
  {
    objAreaIdValue = 0;
    nextObjAreaIdValue = 0;
    objTOSValue = 0;
    nextObjTOSValue = 0;
  } 
  FPOBJ_TRACE_CURRENT_KEY (outAreaId, &objAreaIdValue, owaAreaId.len);
  FPOBJ_TRACE_CURRENT_KEY (outTOS, &objTOSValue, owaTOS.len);

  owa.rc =
    usmDbOspfStubAreaEntryNext(L7_UNIT_CURRENT, &objAreaIdValue, &objTOSValue);

    nextObjAreaIdValue = objAreaIdValue;
    nextObjTOSValue =  objTOSValue;
 
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outAreaId, owaAreaId);
    FPOBJ_TRACE_EXIT (outTOS, owaTOS);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outAreaId, &nextObjAreaIdValue, owaAreaId.len);
  FPOBJ_TRACE_CURRENT_KEY (outTOS, &nextObjTOSValue, owaTOS.len);

  /* return the object value: AreaId */
  xLibBufDataSet (outAreaId,
                  (xLibU8_t *) & nextObjAreaIdValue,
                  sizeof (nextObjAreaIdValue));

  /* return the object value: TOS */
  xLibBufDataSet (outTOS,
                  (xLibU8_t *) & nextObjTOSValue, sizeof (nextObjTOSValue));
  FPOBJ_TRACE_EXIT (outAreaId, owaAreaId);
  FPOBJ_TRACE_EXIT (outTOS, owaTOS);
  return XLIBRC_SUCCESS;
}

#endif

/*******************************************************************************
* @function fpObjGet_routingospfStub_Metric
*
* @purpose Get 'Metric'
*
* @description [Metric]: The metric value applied at the indicated type of
*              service. By default, this equals the least metric at the type
*              of service among the inter- faces to other areas. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfStub_Metric (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfStubMetricGet (L7_UNIT_CURRENT, keyAreaIdValue,
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
* @function fpObjSet_routingospfStub_Metric
*
* @purpose Set 'Metric'
*
* @description [Metric]: The metric value applied at the indicated type of
*              service. By default, this equals the least metric at the type
*              of service among the inter- faces to other areas. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfStub_Metric (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
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

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfStubMetricSet (L7_UNIT_CURRENT, keyAreaIdValue,
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
* @function fpObjGet_routingospfStub_Status
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
xLibRC_t fpObjGet_routingospfStub_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfStubStatusGet (L7_UNIT_CURRENT, keyAreaIdValue,
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
* @function fpObjSet_routingospfStub_Status
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
xLibRC_t fpObjSet_routingospfStub_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  /*fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;*/
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: TOS 
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);*/

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /*rc = usmDbOspfImportAsExternGet(L7_UNIT_CURRENT, keyAreaIdValue, &temp_val);*/

    /* Create a row */

   /* if (rc == L7_SUCCESS && temp_val != L7_OSPF_AREA_IMPORT_NO_EXT)*/
    {
       owa.l7rc = usmDbOspfImportAsExternSet(L7_UNIT_CURRENT, keyAreaIdValue, L7_OSPF_AREA_IMPORT_NO_EXT);
    }
   /* else
    {
       owa.l7rc = L7_FAILURE;
    }*/

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {

    /*rc = usmDbOspfImportAsExternGet(L7_UNIT_CURRENT, keyAreaIdValue, &temp_val); */

    /* Delete the existing row */

   /* if (rc == L7_SUCCESS && temp_val != L7_OSPF_AREA_IMPORT_NO_EXT) */
    {
       owa.l7rc = usmDbOspfImportAsExternSet(L7_UNIT_CURRENT, keyAreaIdValue, L7_OSPF_AREA_IMPORT_EXT);
    }
   /* else
    {
       owa.l7rc = L7_FAILURE;
    }*/

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
* @function fpObjGet_routingospfStub_MetricType
*
* @purpose Get 'MetricType'
*
* @description [MetricType]: This variable displays the type of metric ad-
*              vertised as a default route. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfStub_MetricType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfStubMetricTypeGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                         keyTOSValue, &objMetricTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MetricType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMetricTypeValue,
                           sizeof (objMetricTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfStub_MetricType
*
* @purpose Set 'MetricType'
*
* @description [MetricType]: This variable displays the type of metric ad-
*              vertised as a default route. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfStub_MetricType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTOSValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MetricType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMetricTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricTypeValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfStub_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: TOS */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfStub_TOS,
                           (xLibU8_t *) & keyTOSValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTOSValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfStubMetricTypeSet (L7_UNIT_CURRENT, keyAreaIdValue,
                                         keyTOSValue, objMetricTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
