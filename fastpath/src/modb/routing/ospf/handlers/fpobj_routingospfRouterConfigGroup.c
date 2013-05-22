/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfRouterConfigGroup.c
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
#include "_xe_routingospfRouterConfigGroup_obj.h"
#include "usmdb_ospf_api.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfRouterConfigGroup_InfoOriginateAlways
*
* @purpose Get 'InfoOriginateAlways'
*
* @description [InfoOriginateAlways]: When this flag is true, the router advertises
*              0.0.0.0/0.0.0.0 always. Without this option, OSPF
*              will only advertise 0.0.0.0/0.0.0.0 if the router's forwarding
*              table contains a default route. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_InfoOriginateAlways (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateAlwaysValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfDefaultRouteAlwaysGet (L7_UNIT_CURRENT,
                                    &objInfoOriginateAlwaysValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateAlwaysValue,
                     sizeof (objInfoOriginateAlwaysValue));

  /* return the object value: InfoOriginateAlways */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInfoOriginateAlwaysValue,
                           sizeof (objInfoOriginateAlwaysValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_InfoOriginateAlways
*
* @purpose Set 'InfoOriginateAlways'
*
* @description [InfoOriginateAlways]: When this flag is true, the router advertises
*              0.0.0.0/0.0.0.0 always. Without this option, OSPF
*              will only advertise 0.0.0.0/0.0.0.0 if the router's forwarding
*              table contains a default route. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_InfoOriginateAlways (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateAlwaysValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: InfoOriginateAlways */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objInfoOriginateAlwaysValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateAlwaysValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfDefaultRouteAlwaysSet (L7_UNIT_CURRENT,
                                    objInfoOriginateAlwaysValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_Metric
*
* @purpose Get 'Metric'
*
* @description [Metric]: Default metric of redistributed routes, when OSPF
*              redistributes from other protocols. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_Metric (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfDefaultMetricGet(L7_UNIT_CURRENT, &objMetricValue);

  if (owa.l7rc == L7_NOT_EXIST)
  {
    objMetricValue= 0;
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    owa.l7rc = L7_SUCCESS;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricValue, sizeof (objMetricValue));

  /* return the object value: Metric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMetricValue,
                           sizeof (objMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_Metric
*
* @purpose Set 'Metric'
*
* @description [Metric]: Default metric of redistributed routes, when OSPF
*              redistributes from other protocols. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_Metric (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Metric */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMetricValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricValue, owa.len);

  if(((objMetricValue < L7_OSPF_DEFAULT_METRIC_MIN) ||
      (objMetricValue > L7_OSPF_DEFAULT_METRIC_MAX)) &&
     (objMetricValue != 0))
  {
    owa.rc = XLIBRC_OSPF_DEFAULT_METRIC_INVALID_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objMetricValue == 0)
  {
    owa.l7rc = usmDbOspfDefaultMetricClear(L7_UNIT_CURRENT);
  }
  else
  {
    /* set the value in application */
    owa.l7rc = usmDbOspfDefaultMetricSet(L7_UNIT_CURRENT, objMetricValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_InfoOriginateMetric
*
* @purpose Get 'InfoOriginateMetric'
*
* @description [InfoOriginateMetric]: Metric of the default route,which OSPF
*              advertises when learned from other protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_InfoOriginateMetric (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfDefaultRouteMetricGet(L7_UNIT_CURRENT,
                                    &objInfoOriginateMetricValue);
  if (owa.l7rc == L7_NOT_EXIST)
  {
    objInfoOriginateMetricValue = 0;
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    owa.l7rc = L7_SUCCESS;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateMetricValue,
                     sizeof (objInfoOriginateMetricValue));

  /* return the object value: InfoOriginateMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInfoOriginateMetricValue,
                           sizeof (objInfoOriginateMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_InfoOriginateMetric
*
* @purpose Set 'InfoOriginateMetric'
*
* @description [InfoOriginateMetric]: Metric of the default route,which OSPF
*              advertises when learned from other protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_InfoOriginateMetric (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: InfoOriginateMetric */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objInfoOriginateMetricValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateMetricValue, owa.len);

  if (((objInfoOriginateMetricValue < L7_OSPF_DEFAULT_METRIC_MIN) || 
       (objInfoOriginateMetricValue > L7_OSPF_DEFAULT_METRIC_MAX)) &&
      (objInfoOriginateMetricValue != 0))
  {
    owa.rc = XLIBRC_OSPF_DEFAULT_ROUTE_METRIC_INVALID_RANGE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objInfoOriginateMetricValue == 0)
  {
  owa.l7rc = usmDbOspfDefaultRouteMetricClear(L7_UNIT_CURRENT);
  }
  else
  {
    /* set the value in application */
    owa.l7rc =
      usmDbOspfDefaultRouteMetricSet (L7_UNIT_CURRENT,
                                      objInfoOriginateMetricValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_RFC1583CompatibilityMode
*
* @purpose Get 'RFC1583CompatibilityMode'
*
* @description [RFC1583CompatibilityMode]: From RFC2328: Controls the preference
*              rules used in Section 16.4 when choosing among multiple
*              AS-external-LSAs advertising the same destination. When
*              set to 'enabled', the preference rules remain those specified
*              by RFC 1583 ([Ref9]). When set to 'disabled', the preference
*              rules are those stated in Section 16.4.1, which prevent
*              routing loops when AS- external-LSAs for the same destination
*              have been originated from different areas. Set to 'enabled'
*              by default. In order to minimize the chance of routing
*              loops, all OSPF routers in an OSPF routing domain should
*              have RFC1583Compatibility set identically. When there are
*              routers present that have not been updated with the functionality
*              specified in Section 16.4.1 of this memo, all routers
*              should have RFC1583Compatibility set to 'enabled'. Otherwise,
*              all routers should have RFC1583Compatibility set to 'disabled',
*              preventing all routing loops. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_RFC1583CompatibilityMode (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRFC1583CompatibilityModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfRfc1583CompatibilityGet (L7_UNIT_CURRENT,
                                      &objRFC1583CompatibilityModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRFC1583CompatibilityModeValue,
                     sizeof (objRFC1583CompatibilityModeValue));

  /* return the object value: RFC1583CompatibilityMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objRFC1583CompatibilityModeValue,
                    sizeof (objRFC1583CompatibilityModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_RFC1583CompatibilityMode
*
* @purpose Set 'RFC1583CompatibilityMode'
*
* @description [RFC1583CompatibilityMode]: From RFC2328: Controls the preference
*              rules used in Section 16.4 when choosing among multiple
*              AS-external-LSAs advertising the same destination. When
*              set to 'enabled', the preference rules remain those specified
*              by RFC 1583 ([Ref9]). When set to 'disabled', the preference
*              rules are those stated in Section 16.4.1, which prevent
*              routing loops when AS- external-LSAs for the same destination
*              have been originated from different areas. Set to 'enabled'
*              by default. In order to minimize the chance of routing
*              loops, all OSPF routers in an OSPF routing domain should
*              have RFC1583Compatibility set identically. When there are
*              routers present that have not been updated with the functionality
*              specified in Section 16.4.1 of this memo, all routers
*              should have RFC1583Compatibility set to 'enabled'. Otherwise,
*              all routers should have RFC1583Compatibility set to 'disabled',
*              preventing all routing loops. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_RFC1583CompatibilityMode (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRFC1583CompatibilityModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RFC1583CompatibilityMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRFC1583CompatibilityModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRFC1583CompatibilityModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfRfc1583CompatibilitySet (L7_UNIT_CURRENT,
                                      objRFC1583CompatibilityModeValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_InfoOriginateMetricConfigured
*
* @purpose Get 'InfoOriginateMetricConfigured'
*
* @description [InfoOriginateMetricConfigured]: Flag to determine whether
*              OSPF default-info originate metric is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_routingospfRouterConfigGroup_InfoOriginateMetricConfigured (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateMetricConfiguredValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfDefaultRouteMetricGet(L7_UNIT_CURRENT,
                                    &objInfoOriginateMetricConfiguredValue);

  if (owa.l7rc == L7_NOT_EXIST)
  {
    objInfoOriginateMetricConfiguredValue = 0;
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    owa.l7rc = L7_SUCCESS;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateMetricConfiguredValue,
                     sizeof (objInfoOriginateMetricConfiguredValue));

  /* return the object value: InfoOriginateMetricConfigured */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objInfoOriginateMetricConfiguredValue,
                    sizeof (objInfoOriginateMetricConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_InfoOriginateMetricConfigured
*
* @purpose Set 'InfoOriginateMetricConfigured'
*
* @description [InfoOriginateMetricConfigured]: Flag to determine whether
*              OSPF default-info originate metric is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_routingospfRouterConfigGroup_InfoOriginateMetricConfigured (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateMetricConfiguredValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: InfoOriginateMetricConfigured */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objInfoOriginateMetricConfiguredValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateMetricConfiguredValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfDefaultRouteMetricClear (L7_UNIT_CURRENT);
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
* @function fpObjGet_routingospfRouterConfigGroup_MetricConfigured
*
* @purpose Get 'MetricConfigured'
*
* @description [MetricConfigured]: Flag to determine whether OSPF default-metric
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_MetricConfigured (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricConfiguredValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfDefaultMetricGet (L7_UNIT_CURRENT, &objMetricConfiguredValue);

  if (owa.l7rc == L7_NOT_EXIST)
  {
    objMetricConfiguredValue = 0;
    owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
    owa.l7rc = L7_SUCCESS;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMetricConfiguredValue,
                     sizeof (objMetricConfiguredValue));

  /* return the object value: MetricConfigured */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMetricConfiguredValue,
                           sizeof (objMetricConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_MetricConfigured
*
* @purpose Set 'MetricConfigured'
*
* @description [MetricConfigured]: Flag to determine whether OSPF default-metric
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_MetricConfigured (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricConfiguredValue;
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

  /* set the value in application */
  owa.l7rc =
    usmDbOspfDefaultMetricClear (L7_UNIT_CURRENT);
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
* @function fpObjGet_routingospfRouterConfigGroup_PassiveMode
*
* @purpose Get 'PassiveMode'
*
* @description [PassiveMode]: Flag to determine, whether all the interfaces
*              are passive interfaces by default. A passive interface will
*              not participate in the OSPF adjacency formation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_PassiveMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPassiveModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfPassiveModeGet (L7_UNIT_CURRENT, &objPassiveModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPassiveModeValue, sizeof (objPassiveModeValue));

  /* return the object value: PassiveMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPassiveModeValue,
                           sizeof (objPassiveModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_PassiveMode
*
* @purpose Set 'PassiveMode'
*
* @description [PassiveMode]: Flag to determine, whether all the interfaces
*              are passive interfaces by default. A passive interface will
*              not participate in the OSPF adjacency formation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_PassiveMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPassiveModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PassiveMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPassiveModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPassiveModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfPassiveModeSet (L7_UNIT_CURRENT, objPassiveModeValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_AutoCostRefBw
*
* @purpose Get 'AutoCostRefBw'
*
* @description [AutoCostRefBw]: Auto cost refernece bandwidth of the router
*              interafaces for ospf metric calculations.It can be an integer
*              from 1 to 4294967. The default reference bandwidth is
*              100 Mbps. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_AutoCostRefBw (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoCostRefBwValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfAutoCostRefBwGet (&objAutoCostRefBwValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoCostRefBwValue,
                     sizeof (objAutoCostRefBwValue));

  /* return the object value: AutoCostRefBw */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoCostRefBwValue,
                           sizeof (objAutoCostRefBwValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_AutoCostRefBw
*
* @purpose Set 'AutoCostRefBw'
*
* @description [AutoCostRefBw]: Auto cost refernece bandwidth of the router
*              interafaces for ospf metric calculations.It can be an integer
*              from 1 to 4294967. The default reference bandwidth is
*              100 Mbps. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_AutoCostRefBw (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoCostRefBwValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoCostRefBw */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAutoCostRefBwValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoCostRefBwValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfAutoCostRefBwSet (L7_UNIT_CURRENT, objAutoCostRefBwValue);

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
* @function fpObjGet_routingospfRouterConfigGroup_OpaqueLsaSupport
*
* @purpose Get 'OpaqueLsaSupport'
*
* @description [OpaqueLsaSupport]: The router's support for Opaque LSA types.
*              This object is persistent and when written the entity SHOULD
*              save the change to non-volatile storage. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_OpaqueLsaSupport (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOpaqueLsaSupportValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfOpaqueLsaSupportGet (&objOpaqueLsaSupportValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objOpaqueLsaSupportValue,
                     sizeof (objOpaqueLsaSupportValue));

  /* return the object value: OpaqueLsaSupport */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOpaqueLsaSupportValue,
                           sizeof (objOpaqueLsaSupportValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_OpaqueLsaSupport
*
* @purpose Set 'OpaqueLsaSupport'
*
* @description [OpaqueLsaSupport]: The router's support for Opaque LSA types.
*              This object is persistent and when written the entity SHOULD
*              save the change to non-volatile storage. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_OpaqueLsaSupport (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOpaqueLsaSupportValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: OpaqueLsaSupport */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objOpaqueLsaSupportValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objOpaqueLsaSupportValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfOpaqueLsaSupportSet (objOpaqueLsaSupportValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_InfoOriginateMetricType
*
* @purpose Get 'InfoOriginateMetricType'
*
* @description [InfoOriginateMetricType]: Metric Type of the default route,which
*              OSPF advertises when learned from other protocol. It
*              can be 1(external type 1) or 2(external type 2). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_InfoOriginateMetricType (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateMetricTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfDefaultRouteMetricTypeGet (L7_UNIT_CURRENT,
                                        &objInfoOriginateMetricTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateMetricTypeValue,
                     sizeof (objInfoOriginateMetricTypeValue));

  /* return the object value: InfoOriginateMetricType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInfoOriginateMetricTypeValue,
                           sizeof (objInfoOriginateMetricTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_InfoOriginateMetricType
*
* @purpose Set 'InfoOriginateMetricType'
*
* @description [InfoOriginateMetricType]: Metric Type of the default route,which
*              OSPF advertises when learned from other protocol. It
*              can be 1(external type 1) or 2(external type 2). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_InfoOriginateMetricType (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateMetricTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: InfoOriginateMetricType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objInfoOriginateMetricTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateMetricTypeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfDefaultRouteMetricTypeSet (L7_UNIT_CURRENT,
                                        objInfoOriginateMetricTypeValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_DelayTime
*
* @purpose Get 'DelayTime'
*
* @description [DelayTime]: Delay time (in seconds) between when OSPF receives
*              a topology change and when it starts an SPF calculation.
*              It can be an integer from 0 to 65535. The default time is
*              5 seconds. A value of 0 means that there is no delay; that
*              is, the SPF calculation is started immediately. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_DelayTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDelayTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfspfDelayTimeGet (L7_UNIT_CURRENT, &objDelayTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDelayTimeValue, sizeof (objDelayTimeValue));

  /* return the object value: DelayTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDelayTimeValue,
                           sizeof (objDelayTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_DelayTime
*
* @purpose Set 'DelayTime'
*
* @description [DelayTime]: Delay time (in seconds) between when OSPF receives
*              a topology change and when it starts an SPF calculation.
*              It can be an integer from 0 to 65535. The default time is
*              5 seconds. A value of 0 means that there is no delay; that
*              is, the SPF calculation is started immediately. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_DelayTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDelayTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DelayTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDelayTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDelayTimeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfspfDelaySet (L7_UNIT_CURRENT, objDelayTimeValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_InfoOriginate
*
* @purpose Get 'InfoOriginate'
*
* @description [InfoOriginate]: Flag to determine, whether OSPF can advertise
*              a default-route learned from another protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_InfoOriginate (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfDefaultRouteOrigGet (L7_UNIT_CURRENT, &objInfoOriginateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateValue,
                     sizeof (objInfoOriginateValue));

  /* return the object value: InfoOriginate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInfoOriginateValue,
                           sizeof (objInfoOriginateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_InfoOriginate
*
* @purpose Set 'InfoOriginate'
*
* @description [InfoOriginate]: Flag to determine, whether OSPF can advertise
*              a default-route learned from another protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_InfoOriginate (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInfoOriginateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: InfoOriginate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objInfoOriginateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objInfoOriginateValue, owa.len);

  /* set the value in application */
  if (objInfoOriginateValue)
  {
   owa.l7rc =
    usmDbOspfDefaultRouteOrigSet (L7_UNIT_CURRENT, objInfoOriginateValue);
  }
  else
  {
    owa.l7rc = usmDbOspfDefaultRouteSet(L7_UNIT_CURRENT, objInfoOriginateValue, FD_OSPF_ORIG_DEFAULT_ROUTE_ALWAYS, FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC, FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE);
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
* @function fpObjGet_routingospfRouterConfigGroup_HoldTime
*
* @purpose Get 'HoldTime'
*
* @description [HoldTime]: Minimum time (in seconds) between two consecutive
*              SPF calculations. It can be an integer from 0 to 65535.
*              The default time is 10 seconds. A value of 0 means that there
*              is no delay; that is, two SPF calculations can be done,
*              one immediately after the other. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_HoldTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHoldTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfspfHoldTimeGet (L7_UNIT_CURRENT, &objHoldTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHoldTimeValue, sizeof (objHoldTimeValue));

  /* return the object value: HoldTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHoldTimeValue,
                           sizeof (objHoldTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_HoldTime
*
* @purpose Set 'HoldTime'
*
* @description [HoldTime]: Minimum time (in seconds) between two consecutive
*              SPF calculations. It can be an integer from 0 to 65535.
*              The default time is 10 seconds. A value of 0 means that there
*              is no delay; that is, two SPF calculations can be done,
*              one immediately after the other. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_HoldTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHoldTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HoldTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHoldTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHoldTimeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfspfHoldTimeSet (L7_UNIT_CURRENT, objHoldTimeValue);
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
* @function fpObjGet_routingospfRouterConfigGroup_MaxPaths
*
* @purpose Get 'MaxPaths'
 *@description  [MaxPaths] It sets the number of paths that OSPF can report for
* a given destination where maxpaths is platform-dependent.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfRouterConfigGroup_MaxPaths (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxPathsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOspfMaxPathsGet(&objMaxPathsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMaxPathsValue, sizeof (objMaxPathsValue));

  /* return the object value: MaxPaths */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxPathsValue, sizeof (objMaxPathsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfRouterConfigGroup_MaxPaths
*
* @purpose Set 'MaxPaths'
 *@description  [MaxPaths] It sets the number of paths that OSPF can report for
* a given destination where maxpaths is platform-dependent.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfRouterConfigGroup_MaxPaths (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxPathsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxPaths */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMaxPathsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxPathsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbOspfMaxPathsSet(L7_UNIT_CURRENT, objMaxPathsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

