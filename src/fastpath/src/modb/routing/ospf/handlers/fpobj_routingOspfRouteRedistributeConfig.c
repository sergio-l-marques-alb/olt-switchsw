/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingOspfRouteRedistributeConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfConfig-object.xml
*
* @create  24 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingOspfRouteRedistributeConfig_obj.h"
#include "usmdb_ospf_api.h"
#include "usmdb_util_api.h"
#ifdef L7_QOS_FLEX_PACKAGE_ACL
#include "usmdb_qos_acl_api.h"
#endif
/*******************************************************************************
* @function fpObjGet_routingOspfRouteRedistributeConfig_Source
*
* @purpose Get 'Source'
*
* @description [Source]: Source protocol, from which OSPF can redistribute
*              routes. This object also acts as the identifier for the OSPF
*              redistribution Table entry. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_Source (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSourceValue;
  xLibU32_t nextObjSourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & objSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objSourceValue = 0;
    owa.l7rc = usmDbOspfRouteRedistributeGetNext(L7_UNIT_CURRENT, objSourceValue, &nextObjSourceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSourceValue, owa.len);
    owa.l7rc =
      usmDbOspfRouteRedistributeGetNext (L7_UNIT_CURRENT, objSourceValue,
                                         &nextObjSourceValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSourceValue, owa.len);

  /* return the object value: Source */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSourceValue,
                           sizeof (objSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingOspfRouteRedistributeConfig_Mode
*
* @purpose Get 'Mode'
*
* @description [Mode]: User enabels/disables route-redistribution for a particular
*              source protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_Mode (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfRedistributeGet (L7_UNIT_CURRENT, keySourceValue,
                                       &objModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Mode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objModeValue,
                           sizeof (objModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingOspfRouteRedistributeConfig_Mode
*
* @purpose Set 'Mode'
*
* @description [Mode]: User enabels/disables route-redistribution for a particular
*              source protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_Mode (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Mode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objModeValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* set the value in application */

  if(objModeValue == L7_FALSE)
  {
     owa.l7rc = 
     usmDbOspfRedistributeRevert(L7_UNIT_CURRENT, keySourceValue);
  } 
  else
  {   
    owa.l7rc = usmDbOspfRedistributeSet(L7_UNIT_CURRENT, keySourceValue, objModeValue);
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
* @function fpObjGet_routingOspfRouteRedistributeConfig_Metric
*
* @purpose Get 'Metric'
*
* @description [Metric]: Metric used for redistributing a particular source
*              protocol route into OSPF. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_Metric (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfRedistMetricGet (L7_UNIT_CURRENT, keySourceValue,
                                       &objMetricValue);
  if(owa.l7rc == L7_NOT_EXIST)
  {
    owa.l7rc = L7_SUCCESS;
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    objMetricValue = 0;
  }
  else if (owa.l7rc != L7_SUCCESS)
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
* @function fpObjSet_routingOspfRouteRedistributeConfig_Metric
*
* @purpose Set 'Metric'
*
* @description [Metric]: Metric used for redistributing a particular source
*              protocol route into OSPF. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_Metric (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Metric */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMetricValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfRedistMetricSet (L7_UNIT_CURRENT, keySourceValue,
                                       objMetricValue);
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
* @function fpObjGet_routingOspfRouteRedistributeConfig_MetricConfigured
*
* @purpose Get 'MetricConfigured'
*
* @description [MetricConfigured]: Flag to determine whether OSPF redistribute-metric
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_MetricConfigured (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricConfiguredValue;
  L7_uint32 temp_val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfRedistMetricGet (L7_UNIT_CURRENT, keySourceValue,
                                       &temp_val);

  
  if (owa.l7rc == L7_SUCCESS)
  {
     objMetricConfiguredValue = L7_TRUE;
  }
  else if(owa.l7rc == L7_NOT_EXIST)
  {
    owa.l7rc = L7_SUCCESS;
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    objMetricConfiguredValue = L7_FALSE;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MetricConfigured */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMetricConfiguredValue,
                           sizeof (objMetricConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingOspfRouteRedistributeConfig_MetricConfigured
*
* @purpose Set 'MetricConfigured'
*
* @description [MetricConfigured]: Flag to determine whether OSPF redistribute-metric
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_MetricConfigured (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricConfiguredValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MetricConfigured */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMetricConfiguredValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricConfiguredValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  if(objMetricConfiguredValue == L7_TRUE)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbOspfRedistMetricClear (L7_UNIT_CURRENT, keySourceValue);

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
* @function fpObjGet_routingOspfRouteRedistributeConfig_MetricType
*
* @purpose Get 'MetricType'
*
* @description [MetricType]: Metric Type of routes,which OSPF redistributes
*              from other source protocols. It can be 1(external type 1)
*              or 2(external type 2). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_MetricType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfRedistMetricTypeGet (L7_UNIT_CURRENT, keySourceValue,
                                           &objMetricTypeValue);
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
* @function fpObjSet_routingOspfRouteRedistributeConfig_MetricType
*
* @purpose Set 'MetricType'
*
* @description [MetricType]: Metric Type of routes,which OSPF redistributes
*              from other source protocols. It can be 1(external type 1)
*              or 2(external type 2). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_MetricType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MetricType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMetricTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricTypeValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfRedistMetricTypeSet (L7_UNIT_CURRENT, keySourceValue,
                                           objMetricTypeValue);
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
* @function fpObjGet_routingOspfRouteRedistributeConfig_Tag
*
* @purpose Get 'Tag'
*
* @description [Tag]: It specifies the tag field in routes redistributed by
*              OSPF. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_Tag (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTagValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfTagGet (L7_UNIT_CURRENT, keySourceValue, &objTagValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Tag */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTagValue,
                           sizeof (objTagValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingOspfRouteRedistributeConfig_Tag
*
* @purpose Set 'Tag'
*
* @description [Tag]: It specifies the tag field in routes redistributed by
*              OSPF. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_Tag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTagValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Tag */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTagValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTagSet (L7_UNIT_CURRENT, keySourceValue, objTagValue);
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
* @function fpObjGet_routingOspfRouteRedistributeConfig_Subnets
*
* @purpose Get 'Subnets'
*
* @description [Subnets]: If this value is false, then OSPF will not redistribute
*              subnetted routes. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_Subnets (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSubnetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfRedistSubnetsGet (L7_UNIT_CURRENT, keySourceValue,
                                        &objSubnetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Subnets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSubnetsValue,
                           sizeof (objSubnetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingOspfRouteRedistributeConfig_Subnets
*
* @purpose Set 'Subnets'
*
* @description [Subnets]: If this value is false, then OSPF will not redistribute
*              subnetted routes. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_Subnets (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSubnetsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Subnets */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSubnetsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSubnetsValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfRedistSubnetsSet (L7_UNIT_CURRENT, keySourceValue,
                                        objSubnetsValue);
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
* @function fpObjGet_routingOspfRouteRedistributeConfig_DistListName
*
* @purpose Get 'DistListName'
*
* @description [DistListName]: Access-list number of the access-list, which filters
*              routes received from a source-protocol.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_DistListName (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  xLibU32_t aclIndx;
#endif
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDistListNameValue;

  FPOBJ_TRACE_ENTER (bufp);
  memset (objDistListNameValue, 0, sizeof(objDistListNameValue));
/* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbComponentPresentCheck (L7_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE)
  {
    /* get the value from application */
    owa.l7rc = usmDbOspfDistListGet (L7_UNIT_CURRENT, keySourceValue,
                                   &aclIndx);
    if (owa.l7rc == L7_SUCCESS)
    {
       if (usmDbQosAclNameGet (L7_UNIT_CURRENT, aclIndx, objDistListNameValue)!= L7_SUCCESS)
       {
          owa.l7rc = L7_FAILURE;
       }
    }
  }
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DistList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDistListNameValue,
                           sizeof (objDistListNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_routingOspfRouteRedistributeConfig_DistListName
*
* @purpose Get 'DistListName'
*
* @description [DistListName]: Access-list number of the access-list, which filters
*              routes received from a source-protocol.
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_routingOspfRouteRedistributeConfig_DistListName (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  xLibU32_t aclIndx;
#endif
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDistListNameValue;
  FPOBJ_TRACE_ENTER (bufp);
  memset (objDistListNameValue, 0, sizeof(objDistListNameValue));

/* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

/* retrieve object: DistListName */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_DistListName,
                          (xLibU8_t *) objDistListNameValue, &owa.len);

  owa.l7rc = L7_FAILURE;
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbComponentPresentCheck (L7_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE)
  {
     if (owa.rc != XLIBRC_SUCCESS)
     {
       aclIndx = 0;
       owa.l7rc =usmDbQosAclNamedIndexGetFirst(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, &aclIndx);
       if (owa.l7rc == L7_SUCCESS)
       {
          owa.l7rc = usmDbQosAclNameGet(L7_UNIT_CURRENT, aclIndx, objDistListNameValue);
       }
    }
    else
    {
      FPOBJ_TRACE_VALUE (bufp, objDistListNameValue, owa.len);
      owa.l7rc = L7_FAILURE;
      if ((owa.l7rc =usmDbQosAclNameToIndex(L7_UNIT_CURRENT, objDistListNameValue, &aclIndx))== L7_SUCCESS)
      {
        owa.l7rc =usmDbQosAclNamedIndexGetNext(L7_UNIT_CURRENT, L7_ACL_TYPE_IP, aclIndx, &aclIndx);
        if (owa.l7rc == L7_SUCCESS)
        {
           owa.l7rc = usmDbQosAclNameGet(L7_UNIT_CURRENT, aclIndx, objDistListNameValue);
        }
     }
   }
  }
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DistList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDistListNameValue,
                           sizeof (objDistListNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingOspfRouteRedistributeConfig_DistListName
*
* @purpose Set 'DistListName'
*
* @description [DistListName]: Access-list number of the access-list, which filters
*              routes received from a source-protocol.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_DistListName (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDistListNameValue;
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  xLibU32_t aclIndx;
#endif
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DistList */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDistListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDistListValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);
  owa.l7rc = L7_FAILURE;
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  if (usmDbComponentPresentCheck (L7_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE)
  {
    if (usmDbQosAclNameToIndex(L7_UNIT_CURRENT, objDistListNameValue, &aclIndx)== L7_SUCCESS)
    {
      /* set the value in application */
      owa.l7rc = usmDbOspfDistListSet (L7_UNIT_CURRENT, keySourceValue,
                                   aclIndx);
   
    }
  }
#endif
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
* @function fpObjGet_routingOspfRouteRedistributeConfig_DistList
*
* @purpose Get 'DistList'
*
* @description [DistList]: Access-list number of the access-list, which filters
*              routes received from a source-protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_DistList (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDistListValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfDistListGet (L7_UNIT_CURRENT, keySourceValue,
                                   &objDistListValue);
  if (owa.l7rc == L7_NOT_EXIST)
  {
    owa.rc = XLIBRC_FAILURE ;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;  
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DistList */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDistListValue,
                           sizeof (objDistListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingOspfRouteRedistributeConfig_DistList
*
* @purpose Set 'DistList'
*
* @description [DistList]: Access-list number of the access-list, which filters
*              routes received from a source-protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_DistList (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDistListValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DistList */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDistListValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDistListValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfDistListSet (L7_UNIT_CURRENT, keySourceValue,
                                   objDistListValue);
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
* @function fpObjGet_routingOspfRouteRedistributeConfig_DistListConfigured
*
* @purpose Get 'DistListConfigured'
*
* @description [DistListConfigured]: Flag to determine whether Access-list
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfRouteRedistributeConfig_DistListConfigured (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDistListConfiguredValue;
  xLibU32_t temp_val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfDistListGet (L7_UNIT_CURRENT, keySourceValue,
                                   &temp_val);
  if (owa.l7rc == L7_SUCCESS)
  {
     objDistListConfiguredValue = L7_TRUE;
  }
  else if (owa.l7rc == L7_NOT_EXIST)
  {
     objDistListConfiguredValue = L7_FALSE;
     owa.l7rc = L7_SUCCESS;
     owa.rc = XLIBRC_SUCCESS ;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DistListConfigured */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDistListConfiguredValue,
                           sizeof (objDistListConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingOspfRouteRedistributeConfig_DistListConfigured
*
* @purpose Set 'DistListConfigured'
*
* @description [DistListConfigured]: Flag to determine whether Access-list
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingOspfRouteRedistributeConfig_DistListConfigured (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDistListConfiguredValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DistListConfigured */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDistListConfiguredValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDistListConfiguredValue, owa.len);

  /* retrieve key: Source */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingOspfRouteRedistributeConfig_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

  if(objDistListConfiguredValue == L7_TRUE)
  {
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbOspfDistListClear (L7_UNIT_CURRENT, keySourceValue);
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
