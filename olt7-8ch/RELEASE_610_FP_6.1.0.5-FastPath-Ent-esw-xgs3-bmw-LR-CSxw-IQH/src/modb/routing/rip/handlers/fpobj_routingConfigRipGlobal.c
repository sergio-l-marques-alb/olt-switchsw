/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingConfigRipGlobal.c
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
#include "_xe_routingConfigRipGlobal_obj.h"
#include "usmdb_mib_ripv2_api.h"

/*******************************************************************************
* @function fpObjGet_routingConfigRipGlobal_HostRoutesAcceptMode
*
* @purpose Get 'HostRoutesAcceptMode'
*
* @description [HostRoutesAcceptMode]: Sets the RIP host route acceptance
*              mode in the router. A value of enable(1) means that host routes
*              advertized to this RIP router will be accepted (learned)
*              into its routing table. When set to disable(2), host routes
*              advertized in RIP updates from neighboring routers are
*              ignored. The default host routes accept mode is enable(1).
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipGlobal_HostRoutesAcceptMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHostRoutesAcceptModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRip2HostRoutesAcceptModeGet (L7_UNIT_CURRENT,
                                      &objHostRoutesAcceptModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHostRoutesAcceptModeValue,
                     sizeof (objHostRoutesAcceptModeValue));

  /* return the object value: HostRoutesAcceptMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHostRoutesAcceptModeValue,
                           sizeof (objHostRoutesAcceptModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRipGlobal_HostRoutesAcceptMode
*
* @purpose Set 'HostRoutesAcceptMode'
*
* @description [HostRoutesAcceptMode]: Sets the RIP host route acceptance
*              mode in the router. A value of enable(1) means that host routes
*              advertized to this RIP router will be accepted (learned)
*              into its routing table. When set to disable(2), host routes
*              advertized in RIP updates from neighboring routers are
*              ignored. The default host routes accept mode is enable(1).
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipGlobal_HostRoutesAcceptMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHostRoutesAcceptModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HostRoutesAcceptMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objHostRoutesAcceptModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHostRoutesAcceptModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbRip2HostRoutesAcceptModeSet (L7_UNIT_CURRENT,
                                      objHostRoutesAcceptModeValue);
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
* @function fpObjGet_routingConfigRipGlobal_DefaultInfoOriginate
*
* @purpose Get 'DefaultInfoOriginate'
*
* @description [DefaultInfoOriginate]: Flag to determine, whether RIP can
*              advertise a default-route learned from another protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipGlobal_DefaultInfoOriginate (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultInfoOriginateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRip2DefaultRouteAdvertiseModeGet (L7_UNIT_CURRENT,
                                           &objDefaultInfoOriginateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDefaultInfoOriginateValue,
                     sizeof (objDefaultInfoOriginateValue));

  /* return the object value: DefaultInfoOriginate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDefaultInfoOriginateValue,
                           sizeof (objDefaultInfoOriginateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRipGlobal_DefaultInfoOriginate
*
* @purpose Set 'DefaultInfoOriginate'
*
* @description [DefaultInfoOriginate]: Flag to determine, whether RIP can
*              advertise a default-route learned from another protocol. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipGlobal_DefaultInfoOriginate (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultInfoOriginateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultInfoOriginate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDefaultInfoOriginateValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDefaultInfoOriginateValue, owa.len);

  /* set the value in application */
  if (objDefaultInfoOriginateValue == FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE)
  {
    owa.l7rc = usmDbRip2DefaultRouteAdvertiseModeClear(L7_UNIT_CURRENT);
  }
  else
  {
    owa.l7rc = usmDbRip2DefaultRouteAdvertiseModeSet (L7_UNIT_CURRENT, objDefaultInfoOriginateValue);
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
* @function fpObjGet_routingConfigRipGlobal_AdminState
*
* @purpose Get 'AdminState'
*
* @description [AdminState]: Sets the administrative mode of RIP in the router.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipGlobal_AdminState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbRip2AdminModeGet (L7_UNIT_CURRENT, &objAdminStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStateValue, sizeof (objAdminStateValue));

  /* return the object value: AdminState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminStateValue,
                           sizeof (objAdminStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRipGlobal_AdminState
*
* @purpose Set 'AdminState'
*
* @description [AdminState]: Sets the administrative mode of RIP in the router.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipGlobal_AdminState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminState */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminStateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStateValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbRip2AdminModeSet (L7_UNIT_CURRENT, objAdminStateValue);
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
* @function fpObjGet_routingConfigRipGlobal_DefaultMetricConfigured
*
* @purpose Get 'DefaultMetricConfigured'
*
* @description [DefaultMetricConfigured]: Flag to determine whether RIP default-metric
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipGlobal_DefaultMetricConfigured (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultMetricConfiguredValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRip2DefaultMetricGet (L7_UNIT_CURRENT,
                               &objDefaultMetricConfiguredValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDefaultMetricConfiguredValue,
                     sizeof (objDefaultMetricConfiguredValue));

  /* return the object value: DefaultMetricConfigured */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDefaultMetricConfiguredValue,
                           sizeof (objDefaultMetricConfiguredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRipGlobal_DefaultMetricConfigured
*
* @purpose Set 'DefaultMetricConfigured'
*
* @description [DefaultMetricConfigured]: Flag to determine whether RIP default-metric
*              is configured or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipGlobal_DefaultMetricConfigured (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultMetricConfiguredValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultMetricConfigured */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDefaultMetricConfiguredValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDefaultMetricConfiguredValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbRip2DefaultMetricClear (L7_UNIT_CURRENT);
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
* @function fpObjGet_routingConfigRipGlobal_DefaultMetric
*
* @purpose Get 'DefaultMetric'
*
* @description [DefaultMetric]: Default metric of redistributed routes, when
*              RIP redistributes from other protocols. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipGlobal_DefaultMetric (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRip2DefaultMetricGet (L7_UNIT_CURRENT, &objDefaultMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDefaultMetricValue,
                     sizeof (objDefaultMetricValue));

  /* return the object value: DefaultMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDefaultMetricValue,
                           sizeof (objDefaultMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRipGlobal_DefaultMetric
*
* @purpose Set 'DefaultMetric'
*
* @description [DefaultMetric]: Default metric of redistributed routes, when
*              RIP redistributes from other protocols. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipGlobal_DefaultMetric (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDefaultMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultMetric */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDefaultMetricValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDefaultMetricValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbRip2DefaultMetricSet (L7_UNIT_CURRENT, L7_ENABLE, objDefaultMetricValue);
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
* @function fpObjGet_routingConfigRipGlobal_SplitHorizonMode
*
* @purpose Get 'SplitHorizonMode'
*
* @description [SplitHorizonMode]: Sets the RIP split horizon operating mode
*              in the router. A value of none(1) means split horizon processing
*              is disabled. When set to simple(2), the simple split
*              horizon technique is used. When set to poisonReverse(3),
*              the split horizon with poison reverse technique is used. The
*              default split horizon mode is simple(2). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipGlobal_SplitHorizonMode (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSplitHorizonModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRip2SplitHorizonModeGet (L7_UNIT_CURRENT, &objSplitHorizonModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSplitHorizonModeValue,
                     sizeof (objSplitHorizonModeValue));

  /* return the object value: SplitHorizonMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSplitHorizonModeValue,
                           sizeof (objSplitHorizonModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRipGlobal_SplitHorizonMode
*
* @purpose Set 'SplitHorizonMode'
*
* @description [SplitHorizonMode]: Sets the RIP split horizon operating mode
*              in the router. A value of none(1) means split horizon processing
*              is disabled. When set to simple(2), the simple split
*              horizon technique is used. When set to poisonReverse(3),
*              the split horizon with poison reverse technique is used. The
*              default split horizon mode is simple(2). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipGlobal_SplitHorizonMode (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSplitHorizonModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SplitHorizonMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSplitHorizonModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSplitHorizonModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbRip2SplitHorizonModeSet (L7_UNIT_CURRENT, objSplitHorizonModeValue);
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
* @function fpObjGet_routingConfigRipGlobal_AutoSummaryMode
*
* @purpose Get 'AutoSummaryMode'
*
* @description [AutoSummaryMode]: Sets the RIP auto summarization mode in
*              the router. A value of enable(1) means that routes advertized
*              by this RIP router are combined, or summarized, whenever
*              possible into aggregates. When set to disable(2), routes are
*              not aggregated in RIP updates generated by this router.
*              The default auto summary mode is enable(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipGlobal_AutoSummaryMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoSummaryModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbRip2AutoSummarizationModeGet (L7_UNIT_CURRENT,
                                       &objAutoSummaryModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoSummaryModeValue,
                     sizeof (objAutoSummaryModeValue));

  /* return the object value: AutoSummaryMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoSummaryModeValue,
                           sizeof (objAutoSummaryModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingConfigRipGlobal_AutoSummaryMode
*
* @purpose Set 'AutoSummaryMode'
*
* @description [AutoSummaryMode]: Sets the RIP auto summarization mode in
*              the router. A value of enable(1) means that routes advertized
*              by this RIP router are combined, or summarized, whenever
*              possible into aggregates. When set to disable(2), routes are
*              not aggregated in RIP updates generated by this router.
*              The default auto summary mode is enable(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipGlobal_AutoSummaryMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoSummaryModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoSummaryMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAutoSummaryModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoSummaryModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbRip2AutoSummarizationModeSet (L7_UNIT_CURRENT,
                                       objAutoSummaryModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
