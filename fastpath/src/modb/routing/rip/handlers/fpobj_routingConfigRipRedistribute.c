/*******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 ********************************************************************************
 *
 * @filename fpobj_routingConfigRipRedistribute.c
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
#include "_xe_routingConfigRipRedistribute_obj.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_util_api.h"
#ifdef L7_QOS_FLEX_PACKAGE_ACL
#include "usmdb_qos_acl_api.h"
#endif

/*******************************************************************************
 * @function fpObjGet_routingConfigRipRedistribute_Source
 *
 * @purpose Get 'Source'
 *
 * @description [Source]: Source route, from which RIP can redistribute routes.This
 *              object also acts as the identifier for the RIP redistribution
 *              Table entry. 
 *
 * @note  This is KEY Object
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_Source (void *wap, void *bufp)
{
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objSourceValue;
    xLibU32_t nextObjSourceValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    owa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & objSourceValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
        nextObjSourceValue = 0;
        owa.l7rc = usmDbRip2RouteRedistributeSourceGetNext(L7_UNIT_CURRENT, 
                &nextObjSourceValue);
    }
    else
    {
        FPOBJ_TRACE_CURRENT_KEY (bufp, &objSourceValue, owa.len);
        owa.l7rc = usmDbRip2RouteRedistributeSourceGetNext(L7_UNIT_CURRENT, 
                &objSourceValue);
        nextObjSourceValue = objSourceValue;
    }
    if ((owa.l7rc != L7_SUCCESS) || (objSourceValue == REDIST_RT_BGP))
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
 * @function fpObjGet_routingConfigRipRedistribute_Mode
 *
 * @purpose Get 'Mode'
 *
 * @description [Mode]: User enabels/disables route-redistribution for a particular
 *              source protocol. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_Mode (void *wap, void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objModeValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc = usmDbRip2RouteRedistributeModeGet (L7_UNIT_CURRENT, 
            keySourceValue, &objModeValue);
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
 * @function fpObjSet_routingConfigRipRedistribute_Mode
 *
 * @purpose Set 'Mode'
 *
 * @description [Mode]: User enabels/disables route-redistribution for a particular
 *              source protocol. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_Mode (void *wap, void *bufp)
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
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    owa.l7rc = usmDbRip2RouteRedistributeModeSet (L7_UNIT_CURRENT, 
                                                  keySourceValue,objModeValue);
    if(objModeValue == L7_DISABLE)
    { 
      owa.l7rc |= usmDbRip2RouteRedistributeParmsClear(L7_UNIT_CURRENT, keySourceValue);
      owa.l7rc |= usmDbRip2RRouteRedistributeFilterClear(L7_UNIT_CURRENT, keySourceValue);
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
 * @function fpObjGet_routingConfigRipRedistribute_Metric
 *
 * @purpose Get 'Metric'
 *
 * @description [Metric]: Metric used for redistributing a particular source
 *              protocol route into RIP. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_Metric (void *wap, void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMetricValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMetricGet (L7_UNIT_CURRENT, keySourceValue,
                &objMetricValue);
    if(owa.l7rc == L7_ERROR)
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
 * @function fpObjSet_routingConfigRipRedistribute_Metric
 *
 * @purpose Set 'Metric'
 *
 * @description [Metric]: Metric used for redistributing a particular source
 *              protocol route into RIP. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_Metric (void *wap, void *bufp)
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
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    if(objMetricValue != L7_NULL)
    {
      owa.l7rc = usmDbRip2RouteRedistributeMetricSet (L7_UNIT_CURRENT, keySourceValue,
                                                      L7_ENABLE, objMetricValue);
    }
    else
    {
      owa.l7rc = usmDbRip2RouteRedistributeMetricSet (L7_UNIT_CURRENT, keySourceValue,
                                                      L7_DISABLE, objMetricValue);
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
 * @function fpObjGet_routingConfigRipRedistribute_MetricConfigured
 *
 * @purpose Get 'MetricConfigured'
 *
 * @description [MetricConfigured]: Flag to determine whether RIP redistribute-metric
 *              is configured or not. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_MetricConfigured (void *wap,
        void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMetricConfiguredValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMetricGet (L7_UNIT_CURRENT, keySourceValue,
                &objMetricConfiguredValue);
    if(owa.l7rc == L7_ERROR)
    {
        owa.l7rc = L7_SUCCESS;
        owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
        objMetricConfiguredValue = 0;
    }
    else if (owa.l7rc != L7_SUCCESS)
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
 * @function fpObjSet_routingConfigRipRedistribute_MetricConfigured
 *
 * @purpose Set 'MetricConfigured'
 *
 * @description [MetricConfigured]: Flag to determine whether RIP redistribute-metric
 *              is configured or not. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_MetricConfigured (void *wap,
        void *bufp)
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
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }

    if(objMetricConfiguredValue == L7_TRUE)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    } 

    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMetricClear (L7_UNIT_CURRENT, keySourceValue);
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
 * @function fpObjGet_routingConfigRipRedistribute_MatchInternal
 *
 * @purpose Get 'MatchInternal'
 *
 * @description [MatchInternal]: If this flag is true, RIP redistributes OSPF
 *              internal routes. It will be non-applicable, when source
 *              protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_MatchInternal (void *wap,
        void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchInternalValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMatchTypeGet (L7_UNIT_CURRENT, keySourceValue,
                &objMatchInternalValue);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    objMatchInternalValue = 
        (objMatchInternalValue & L7_OSPF_METRIC_TYPE_INTERNAL) ?
        L7_ENABLE : L7_DISABLE;

    /* return the object value: MatchInternal */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchInternalValue,
            sizeof (objMatchInternalValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_routingConfigRipRedistribute_MatchInternal
 *
 * @purpose Set 'MatchInternal'
 *
 * @description [MatchInternal]: If this flag is true, RIP redistributes OSPF
 *              internal routes. It will be non-applicable, when source
 *              protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_MatchInternal (void *wap,
        void *bufp)
{
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchInternalValue;
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve object: MatchInternal */
    owa.rc = xLibBufDataGet (bufp,
            (xLibU8_t *) & objMatchInternalValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, &objMatchInternalValue, owa.len);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }


    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    if(keySourceValue == REDIST_RT_OSPF)
    {
      owa.l7rc = usmDbRip2RouteRedistributeMatchTypeSet (L7_UNIT_CURRENT, 
                                                         keySourceValue, 
                                                         objMatchInternalValue, 
                                                         L7_OSPF_METRIC_TYPE_INTERNAL);
    }
    else
    {
      owa.l7rc = L7_SUCCESS;
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
 * @function fpObjGet_routingConfigRipRedistribute_MatchExternal1
 *
 * @purpose Get 'MatchExternal1'
 *
 * @description [MatchExternal1]: If this flag is true, RIP redistributes OSPF
 *              External1 routes. It will be non-applicable, when source
 *              protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_MatchExternal1 (void *wap,
        void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchExternal1Value;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMatchTypeGet (L7_UNIT_CURRENT, keySourceValue,
                &objMatchExternal1Value);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    objMatchExternal1Value =
        (objMatchExternal1Value & L7_OSPF_METRIC_TYPE_EXT1) ?
        L7_ENABLE : L7_DISABLE;

    /* return the object value: MatchExternal1 */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchExternal1Value,
            sizeof (objMatchExternal1Value));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_routingConfigRipRedistribute_MatchExternal1
 *
 * @purpose Set 'MatchExternal1'
 *
 * @description [MatchExternal1]: If this flag is true, RIP redistributes OSPF
 *              External1 routes. It will be non-applicable, when source
 *              protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_MatchExternal1 (void *wap,
        void *bufp)
{
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchExternal1Value;
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve object: MatchExternal1 */
    owa.rc = xLibBufDataGet (bufp,
            (xLibU8_t *) & objMatchExternal1Value, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, &objMatchExternal1Value, owa.len);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */    
    if(keySourceValue == REDIST_RT_OSPF)
    {
      owa.l7rc = usmDbRip2RouteRedistributeMatchTypeSet (L7_UNIT_CURRENT, 
                                                         keySourceValue,
                                                         objMatchExternal1Value, 
                                                         L7_OSPF_METRIC_TYPE_EXT1);
    }
    else
    {
      owa.l7rc = L7_SUCCESS;
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
 * @function fpObjGet_routingConfigRipRedistribute_MatchExternal2
 *
 * @purpose Get 'MatchExternal2'
 *
 * @description [MatchExternal2]: If this flag is true, RIP redistributes OSPF
 *              External2 routes. It will be non-applicable, when source
 *              protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_MatchExternal2 (void *wap,
        void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchExternal2Value;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMatchTypeGet (L7_UNIT_CURRENT, keySourceValue,
                &objMatchExternal2Value);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    objMatchExternal2Value =
        (objMatchExternal2Value & L7_OSPF_METRIC_TYPE_EXT2) ?
        L7_ENABLE : L7_DISABLE;

    /* return the object value: MatchExternal2 */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchExternal2Value,
            sizeof (objMatchExternal2Value));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_routingConfigRipRedistribute_MatchExternal2
 *
 * @purpose Set 'MatchExternal2'
 *
 * @description [MatchExternal2]: If this flag is true, RIP redistributes OSPF
 *              External2 routes. It will be non-applicable, when source
 *              protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_MatchExternal2 (void *wap,
        void *bufp)
{
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchExternal2Value;
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;

    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve object: MatchExternal2 */
    owa.rc = xLibBufDataGet (bufp,
            (xLibU8_t *) & objMatchExternal2Value, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, &objMatchExternal2Value, owa.len);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    if(keySourceValue == REDIST_RT_OSPF)
    {
      owa.l7rc = usmDbRip2RouteRedistributeMatchTypeSet (L7_UNIT_CURRENT, 
                                                         keySourceValue,
                                                         objMatchExternal2Value, 
                                                         L7_OSPF_METRIC_TYPE_EXT2);
    }
    else
    {
      owa.l7rc = L7_SUCCESS;
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
 * @function fpObjGet_routingConfigRipRedistribute_MatchNSSAExternal1
 *
 * @purpose Get 'MatchNSSAExternal1'
 *
 * @description [MatchNSSAExternal1]: If this flag is true, RIP redistributes
 *              OSPF NSSA-External1 routes. It will be non-applicable, when
 *              source protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_MatchNSSAExternal1 (void *wap,
        void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchNSSAExternal1Value;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMatchTypeGet (L7_UNIT_CURRENT, keySourceValue,
                &objMatchNSSAExternal1Value);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    objMatchNSSAExternal1Value =
        (objMatchNSSAExternal1Value & L7_OSPF_METRIC_TYPE_NSSA_EXT1) ?
        L7_ENABLE : L7_DISABLE;

    /* return the object value: MatchNSSAExternal1 */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchNSSAExternal1Value,
            sizeof (objMatchNSSAExternal1Value));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_routingConfigRipRedistribute_MatchNSSAExternal1
 *
 * @purpose Set 'MatchNSSAExternal1'
 *
 * @description [MatchNSSAExternal1]: If this flag is true, RIP redistributes
 *              OSPF NSSA-External1 routes. It will be non-applicable, when
 *              source protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_MatchNSSAExternal1 (void *wap,
        void *bufp)
{
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchNSSAExternal1Value;
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve object: MatchNSSAExternal1 */
    owa.rc = xLibBufDataGet (bufp,
            (xLibU8_t *) & objMatchNSSAExternal1Value, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, &objMatchNSSAExternal1Value, owa.len);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    if(keySourceValue == REDIST_RT_OSPF)
    {
      owa.l7rc = usmDbRip2RouteRedistributeMatchTypeSet (L7_UNIT_CURRENT,
                                                         keySourceValue,
                                                         objMatchNSSAExternal1Value,
                                                         L7_OSPF_METRIC_TYPE_NSSA_EXT1);
    }
    else
    {
      owa.l7rc = L7_SUCCESS;
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
 * @function fpObjGet_routingConfigRipRedistribute_MatchNSSAExternal2
 *
 * @purpose Get 'MatchNSSAExternal2'
 *
 * @description [MatchNSSAExternal2]: If this flag is true, RIP redistributes
 *              OSPF NSSA-External2 routes. It will be non-applicable, when
 *              source protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_MatchNSSAExternal2 (void *wap,
        void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchNSSAExternal2Value;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeMatchTypeGet (L7_UNIT_CURRENT, keySourceValue,
                &objMatchNSSAExternal2Value);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    objMatchNSSAExternal2Value =
        (objMatchNSSAExternal2Value & L7_OSPF_METRIC_TYPE_NSSA_EXT2) ?
        L7_ENABLE : L7_DISABLE;

    /* return the object value: MatchNSSAExternal2 */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMatchNSSAExternal2Value,
            sizeof (objMatchNSSAExternal2Value));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}


/*******************************************************************************
 * @function fpObjSet_routingConfigRipRedistribute_MatchNSSAExternal2
 *
 * @purpose Set 'MatchNSSAExternal2'
 *
 * @description [MatchNSSAExternal2]: If this flag is true, RIP redistributes
 *              OSPF NSSA-External2 routes. It will be non-applicable, when
 *              source protocol is other than ospf. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_MatchNSSAExternal2 (void *wap,
        void *bufp)
{
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objMatchNSSAExternal2Value;
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve object: MatchNSSAExternal2 */
    owa.rc = xLibBufDataGet (bufp,
            (xLibU8_t *) & objMatchNSSAExternal2Value, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, &objMatchNSSAExternal2Value, owa.len);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    if(keySourceValue == REDIST_RT_OSPF)
    {
      owa.l7rc = usmDbRip2RouteRedistributeMatchTypeSet (L7_UNIT_CURRENT, 
                                                         keySourceValue,
                                                         objMatchNSSAExternal2Value,
                                                         L7_OSPF_METRIC_TYPE_NSSA_EXT2);
    }
    else
    {
      owa.l7rc = L7_SUCCESS;
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
 * @function fpObjGet_routingConfigRipRedistribute_DistList
 *
 * @purpose Get 'DistList'
 *
 * @description [DistList]: Access-list number of the access-list, which filters
 *              routes received from a source-protocol. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_DistList (void *wap, void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objDistListValue;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeFilterGet (L7_UNIT_CURRENT, keySourceValue,
                &objDistListValue);
    if(owa.l7rc == L7_ERROR)
    {
        owa.l7rc = L7_SUCCESS;
        owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
        objDistListValue = 0;
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
 * @function fpObjSet_routingConfigRipRedistribute_DistList
 *
 * @purpose Set 'DistList'
 *
 * @description [DistList]: Access-list number of the access-list, which filters
 *              routes received from a source-protocol. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_DistList (void *wap, void *bufp)
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
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    if(objDistListValue == L7_NULL)
    {
      owa.l7rc = usmDbRip2RRouteRedistributeFilterClear(L7_UNIT_CURRENT, keySourceValue);
    }
    else
    {
      owa.l7rc = usmDbRip2RouteRedistributeFilterSet(L7_UNIT_CURRENT, keySourceValue, 
                                                     L7_ENABLE, objDistListValue);
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
 * @function fpObjGet_routingConfigRipRedistribute_DistListConfigured
 *
 * @purpose Get 'DistListConfigured'
 *
 * @description [DistListConfigured]: Flag to determine whether Access-list
 *              is configured or not. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_DistListConfigured (void *wap,
        void *bufp)
{
    fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t keySourceValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    xLibU32_t objDistListConfiguredValue;
    xLibU32_t temp_val;
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: Source */
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* get the value from application */
    owa.l7rc =
        usmDbRip2RouteRedistributeFilterGet (L7_UNIT_CURRENT, keySourceValue,
                &temp_val);
    if (owa.l7rc == L7_SUCCESS)
    {
       objDistListConfiguredValue = L7_TRUE;
    }
    else if (owa.l7rc == L7_ERROR)
    {
       objDistListConfiguredValue = L7_FALSE;
       owa.l7rc = L7_SUCCESS;
       owa.rc = XLIBRC_SUCCESS;
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
 * @function fpObjSet_routingConfigRipRedistribute_DistListConfigured
 *
 * @purpose Set 'DistListConfigured'
 *
 * @description [DistListConfigured]: Flag to determine whether Access-list
 *              is configured or not. 
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_DistListConfigured (void *wap,
        void *bufp)
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
    kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
            (xLibU8_t *) & keySourceValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
        kwa.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa);
        return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

    /* set the value in application */
    owa.l7rc =
        usmDbRip2RRouteRedistributeFilterClear (L7_UNIT_CURRENT, keySourceValue);
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
* @function fpObjGet_routingConfigRipRedistribute_Match
*
* @purpose Get 'Match'
 *@description  [Match] If this flag is true, RIP redistributes OSPF internal
* routes. It will be non-applicable, when source protocol is other
* than ospf.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_Match (void *wap, void *bufp)
{
#define REM_STR_LEN(a) (sizeof(a)-strlen(a))
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objMatchValue;
  xLibU32_t objMatchType;
  
  xLibU32_t keySourceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Source */
  owa.len = sizeof (keySourceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
                          (xLibU8_t *) & keySourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, owa.len);

  memset(objMatchValue, 0x00, sizeof(objMatchValue));
  if (keySourceValue == XUI_REDIST_RT_OSPF)
  {
    /* get the value from application */
    owa.l7rc = usmDbRip2RouteRedistributeMatchTypeGet (L7_UNIT_CURRENT, keySourceValue, &objMatchType);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if (objMatchType & L7_OSPF_METRIC_TYPE_INTERNAL)
    {
      osapiStrncat(objMatchValue, "Internal ", REM_STR_LEN(objMatchValue));
      osapiStrncat(objMatchValue,"&lt;br&gt;", REM_STR_LEN(objMatchValue));
    }

    if (objMatchType & L7_OSPF_METRIC_TYPE_EXT1)
    {
      osapiStrncat(objMatchValue, "External Type 1 ", REM_STR_LEN(objMatchValue));
      osapiStrncat(objMatchValue,"&lt;br&gt;", REM_STR_LEN(objMatchValue));
    }

    if (objMatchType & L7_OSPF_METRIC_TYPE_EXT2)
    {
      osapiStrncat(objMatchValue, "External Type 2 ", REM_STR_LEN(objMatchValue));
      osapiStrncat(objMatchValue,"&lt;br&gt;", REM_STR_LEN(objMatchValue));
    }

    if (objMatchType & L7_OSPF_METRIC_TYPE_NSSA_EXT1)
    {
      osapiStrncat(objMatchValue, "NSSA External Type 1 ", REM_STR_LEN(objMatchValue));
      osapiStrncat(objMatchValue,"&lt;br&gt;", REM_STR_LEN(objMatchValue));
    }

    if (objMatchType & L7_OSPF_METRIC_TYPE_NSSA_EXT2)
    {
      osapiStrncat(objMatchValue, "NSSA External Type 2", REM_STR_LEN(objMatchValue));
      osapiStrncat(objMatchValue,"&lt;br&gt;", REM_STR_LEN(objMatchValue));
    }
  }
  else 
  {
    osapiSnprintf(objMatchValue, sizeof(objMatchValue), "%s", "N.A.");
  }

  FPOBJ_TRACE_VALUE (bufp, objMatchValue, strlen (objMatchValue));

  /* return the object value: Match */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMatchValue, strlen (objMatchValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_routingConfigRipRedistribute_DistListName
*
* @purpose Get 'DistListName'
*
* @description [DistListName]: Access-list number of the access-list, which filters
*              routes received from a source-protocol.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingConfigRipRedistribute_DistListName (void *wap,
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
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
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
    owa.l7rc = usmDbRip2RouteRedistributeFilterGet (L7_UNIT_CURRENT, keySourceValue,
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
* @function fpObjGet_routingConfigRipRedistribute_DistListName
*
* @purpose Get 'DistListName'
*
* @description [DistListName]: Access-list number of the access-list, which filters
*              routes received from a source-protocol.
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_routingConfigRipRedistribute_DistListName (void *wap,
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
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
                          (xLibU8_t *) & keySourceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceValue, kwa.len);

/* retrieve object: DistListName */
  owa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_DistListName,
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
* @function fpObjSet_routingConfigRipRedistribute_DistListName
*
* @purpose Set 'DistListName'
*
* @description [DistListName]: Access-list number of the access-list, which filters
*              routes received from a source-protocol.
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingConfigRipRedistribute_DistListName (void *wap,
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
  kwa.rc = xLibFilterGet (wap, XOBJ_routingConfigRipRedistribute_Source,
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
      owa.l7rc = usmDbRip2RouteRedistributeFilterSet (L7_UNIT_CURRENT, keySourceValue, L7_ENABLE,
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

