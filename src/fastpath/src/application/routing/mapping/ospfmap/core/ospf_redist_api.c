/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  ospf_redist_api.c
*
* @purpose   OSPF redistribution APIs
*
* @component OSPF Mapping Layer
*
* @comments  This file includes getters & setters for OSPF
*            variables related to route redistribution.
*
* @create    06/30/2003
*
* @author    rrice
*
* @end
*             
**********************************************************************/


#include "l7_ospfinclude.h"
#include "l7_ospf_api.h"

extern L7_ospfMapCfg_t    *pOspfMapCfgData;

/*********************************************************************
* @purpose  Set configuration parameters for OSPF's redistribution 
*           of routes from other sources. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistribute - Whether OSPF redistributes from sourceProto
* @param    distList - Number of access list used to filter routes from
*                      sourceProto.
* @param    redistMetric - Metric OSPF advertises for routes from
*                          sourceProto
* @param    metType - Metric type OSPF advertises for routes from 
*                     sourceProto
* @param    tag - OSPF tag advertised with routes from sourceProto
* @param    subnets - OSPF only redistributes subnetted routes if this
*                     option is set to true.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if a) sourceProto does not identify a protocol from
*                      which OSPF may redistribute, or b) redistMetric 
*                      is out of range.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributionSet(L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL redistribute,
                                 L7_uint32 distList,
                                 L7_int32 redistMetric,
                                 L7_OSPF_EXT_METRIC_TYPES_t metType,
                                 L7_uint32 tag,
                                 L7_BOOL subnets)
{
    L7_BOOL change = L7_FALSE;
    /* set to TRUE if change causes OSPF to register with RTO */
    L7_BOOL registered = L7_FALSE; 
    ospfRedistCfg_t *redistCfg;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }

    if ((redistMetric != FD_OSPF_REDIST_METRIC) &&
        ((redistMetric < L7_OSPF_REDIST_METRIC_MIN) || 
         (redistMetric > L7_OSPF_REDIST_METRIC_MAX))) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }

    if (redistCfg->redistribute != redistribute) {
        if (redistribute) {
            /* turning on redistribution for this source. If OSPF not
             * already an ASBR, need to note change in status. 
             */
            if (ospfMapIsAsbr() == L7_FALSE) {
                ospfMapAsbrStatusApply(L7_ENABLE);
                registered = L7_TRUE;
            }
        }       
        redistCfg->redistribute = redistribute;
        if (redistribute == L7_FALSE) {   
            /* turned off redistribution for this source. If OSPF
             * doesn't redistribute from another source, router is
             * no longer an ASBR.
             */
            if (ospfMapIsAsbr() == L7_FALSE) {
                ospfMapAsbrStatusApply(L7_DISABLE);
            }
        }
        change = L7_TRUE;
    }
    if (redistCfg->distList != distList) {
        redistCfg->distList = distList;
        change = L7_TRUE;
    }
    if (redistCfg->redistMetric != redistMetric) {
        redistCfg->redistMetric = redistMetric;
        change = L7_TRUE;
    }
    if (redistCfg->metType != metType) {
        redistCfg->metType = metType;
        change = L7_TRUE;
    }
    if (redistCfg->tag != tag) {
        redistCfg->tag = tag;
        change = L7_TRUE;
    }
    if (redistCfg->subnets != subnets) {
        redistCfg->subnets = subnets;
        change = L7_TRUE;
    }

    if (change) {
        ospfDataChangedSet(__FUNCTION__);
        /* If change caused router to become an ASBR and register with 
         * RTO, then RTO will send a full set of routes. Don't need to 
         * walk the routing table here. Otherwise, reevaluate all routes. */
        if (!registered)
          ospfMapExtenRedistReevaluate();
    }

    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Specify whether OSPF redistributes from a specific
*           source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistribute - Whether OSPF redistributes from sourceProto
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributeSet(L7_REDIST_RT_INDICES_t sourceProto,
                               L7_BOOL redistribute)
{
    ospfRedistCfg_t *redistCfg;
    L7_BOOL registered = L7_FALSE;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    
    if (redistCfg->redistribute != redistribute) {
        if (redistribute) {
            /* turning on redistribution for this source. If OSPF not
             * already an ASBR, need to note change in status. 
             */
            if (ospfMapIsAsbr() == L7_FALSE) {
                ospfMapAsbrStatusApply(L7_ENABLE);
                registered = L7_TRUE;
            }
        }       
        redistCfg->redistribute = redistribute;
        
        if (redistribute == L7_FALSE) {   
            /* turned off redistribution for this source. If OSPF
             * doesn't redistribute from another source, router is
             * no longer an ASBR.
             */
            if (ospfMapIsAsbr() == L7_FALSE) {
                ospfMapAsbrStatusApply(L7_DISABLE);
            }
        }
        ospfDataChangedSet(__FUNCTION__);
        if (!registered)
          ospfMapExtenRedistReevaluate();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get whether OSPF redistributes from a specific
*           source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistribute - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributeGet(L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL *redistribute)
{
    ospfRedistCfg_t *redistCfg;

    if (redistribute == L7_NULLPTR) {
        return L7_FAILURE;
    }

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    
    *redistribute = redistCfg->redistribute;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  For a given source of redistributed routes, revert the
*           redistribution configuration to the defaults.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    Defaults as follows:
*              redistribute: FALSE
*              distribute list: 0
*              metric: not configured
*              metric type: external type 2
*              tag: 0
*              subnets: FALSE   
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistributeRevert(L7_REDIST_RT_INDICES_t sourceProto)
{
    L7_RC_t rc = L7_SUCCESS;
    ospfRedistCfg_t *redistCfg;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }

    /* if turning off redistribution for this source */
    if (redistCfg->redistribute == L7_TRUE) {

        ospfDataChangedSet(__FUNCTION__);
        rc = ospfRedistCfgInit(redistCfg);

        if (ospfMapIsAsbr() == L7_FALSE) {
            ospfMapAsbrStatusApply(L7_DISABLE);
        }

        ospfMapExtenRedistReevaluate();
    }

    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return rc;
}

/*********************************************************************
* @purpose  Specify an access list that OSPF uses to filter routes
*           from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    distList - Number of access list used to filter routes from
*                      sourceProto.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDistListSet(L7_REDIST_RT_INDICES_t sourceProto,
                             L7_uint32 distList)
{
    ospfRedistCfg_t *redistCfg;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }

    if (redistCfg->distList != distList) {
        redistCfg->distList = distList;
        ospfDataChangedSet(__FUNCTION__);
        ospfMapExtenRedistReevaluate();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove a distribute list from serving as a route filter
*           for routes from a given source protocol. Call this function 
*           when the user removes the distribute list option. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDistListClear(L7_REDIST_RT_INDICES_t sourceProto)
{
    /* An access list number of 0 indicates no distribute list is 
       configured. */
    return ospfMapDistListSet(sourceProto, FD_OSPF_DIST_LIST_OUT);
}

/*********************************************************************
* @purpose  Get the number of the access list that OSPF uses to filter routes
*           from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    distList - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @returns  L7_NOT_EXIST if no distribute list is configured.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapDistListGet(L7_REDIST_RT_INDICES_t sourceProto,
                           L7_uint32 *distList)
{
    ospfRedistCfg_t *redistCfg;

    if (distList == L7_NULLPTR) {
        return L7_FAILURE;
    }

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    
    *distList = redistCfg->distList;
    if (*distList == FD_OSPF_DIST_LIST_OUT) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_NOT_EXIST;
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the metric that OSPF advetises for routes learned from
*           another protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistMetric - Metric OSPF advertises for routes from
*                          sourceProto
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if a) sourceProto does not identify a protocol from
*                      which OSPF may redistribute, or b) redistMetric is
*                      out of range.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricSet(L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_int32 redistMetric)
{
    ospfRedistCfg_t *redistCfg;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    if ((redistMetric != FD_OSPF_REDIST_METRIC) && 
        ((redistMetric < L7_OSPF_REDIST_METRIC_MIN) || 
         (redistMetric > L7_OSPF_REDIST_METRIC_MAX))) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    if (redistCfg->redistMetric != redistMetric) {
        redistCfg->redistMetric = redistMetric;    
        ospfDataChangedSet(__FUNCTION__);
        ospfMapExtenRedistReevaluate();   
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear the redistribution metric for a given source protocol.
*           Call this function when the redistribute metric option is
*           removed from the configuration. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricClear(L7_REDIST_RT_INDICES_t sourceProto)
{
    /* Indicates no redistribution metric is configured. */
    return ospfMapRedistMetricSet(sourceProto, FD_OSPF_REDIST_METRIC);
}

/*********************************************************************
* @purpose  Get the metric that OSPF advetises for routes learned from
*           another protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    redistMetric - return value.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
* @returns  L7_NOT_EXIST if no redistribution metric is configured
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricGet(L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_int32 *redistMetric)
{
    ospfRedistCfg_t *redistCfg;
    L7_RC_t rc = L7_SUCCESS;

    if (redistMetric == L7_NULLPTR) {
        return L7_FAILURE;
    }

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    } 
    if (redistCfg->redistMetric == FD_OSPF_REDIST_METRIC) {
        rc = L7_NOT_EXIST;
    }
    *redistMetric = redistCfg->redistMetric;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return rc;
}

/*********************************************************************
* @purpose  Set the metric that OSPF uses when it redistributes routes 
*           from a given source protocol.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    metType - Metric type OSPF advertises for routes from 
*                     sourceProto
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricTypeSet(L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_OSPF_EXT_METRIC_TYPES_t metType)
{
    ospfRedistCfg_t *redistCfg;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    if (redistCfg->metType != metType) {
        redistCfg->metType = metType;
        ospfDataChangedSet(__FUNCTION__);
        ospfMapExtenRedistReevaluate();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Revert to the default the metric that OSPF uses when it  
*           redistributes routes from a given source protocol.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricTypeRevert(L7_REDIST_RT_INDICES_t sourceProto)
{
    return ospfMapRedistMetricTypeSet(sourceProto, 
                                      FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_METRIC_TYPE);
}

/*********************************************************************
* @purpose  Get the metric that OSPF uses when it redistributes routes 
*           from a given source protocol.
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    metType - return value.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistMetricTypeGet(L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_OSPF_EXT_METRIC_TYPES_t *metType)
{
    ospfRedistCfg_t *redistCfg;

    if (metType == L7_NULLPTR) {
        return L7_FAILURE;
    }

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    *metType = redistCfg->metType;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Specify the tag that OSPF uses when it advertises routes
*           learned from a given protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    tag - OSPF tag advertised with routes from sourceProto
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTagSet(L7_REDIST_RT_INDICES_t sourceProto,
                      L7_uint32 tag)
{
    ospfRedistCfg_t *redistCfg;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    
    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    if (redistCfg->tag != tag) {
        redistCfg->tag = tag;
        ospfDataChangedSet(__FUNCTION__);
        ospfMapExtenRedistReevaluate();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the tag that OSPF uses when it advertises routes
*           learned from a given protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    tag - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapTagGet(L7_REDIST_RT_INDICES_t sourceProto,
                      L7_uint32 *tag)
{
    ospfRedistCfg_t *redistCfg;

    if (tag == L7_NULLPTR) {
        return L7_FAILURE;
    }

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    
    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    
    *tag = redistCfg->tag;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Specify whether OSPF is allowed to redistribute subnetted 
*           routes from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    subnets - OSPF only redistributes subnetted routes if this
*                     option is set to true.
*
* @returns  L7_SUCCESS if configuration is successfully stored.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistSubnetsSet(L7_REDIST_RT_INDICES_t sourceProto,
                                  L7_BOOL subnets)
{
    ospfRedistCfg_t *redistCfg;

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    
    if (redistCfg->subnets != subnets) {
        redistCfg->subnets = subnets;
        ospfDataChangedSet(__FUNCTION__);
        ospfMapExtenRedistReevaluate();
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get whether OSPF is allowed to redistribute subnetted 
*           routes from a given source protocol. 
*
* @param    sourceProto - Other protocol from which OSPF will redistribute.
*                         Valid values are: REDIST_RT_RIP
*                                           REDIST_RT_STATIC
*                                           REDIST_RT_LOCAL (connected)
*                                           REDIST_RT_BGP
* 
* @param    subnets - return value.
*
* @returns  L7_SUCCESS if successful.
* @returns  L7_FAILURE if sourceProto does not identify a protocol from
*                      which OSPF may redistribute
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapRedistSubnetsGet(L7_REDIST_RT_INDICES_t sourceProto,
                                L7_BOOL *subnets)
{
    ospfRedistCfg_t *redistCfg;

    if (subnets == L7_NULLPTR) {
        return L7_FAILURE;
    }

    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    
    redistCfg = ospfMapRedistCfgGet(sourceProto);
    if (redistCfg == L7_NULL) {
        osapiSemaGive(ospfMapCtrl_g.cfgSema);
        return L7_FAILURE;
    }
    
    *subnets = redistCfg->subnets;
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Indicates whether a sourceIndex is a valid
*           source from which OSPF may redistribute.
*
* @param    sourceIndex - Corresponds to a source for redistributed routes.
*
* @returns  L7_SUCCESS if sourceIndex is a valid
*                      source of routes for OSPF to redistribute.
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapRouteRedistributeGet(L7_REDIST_RT_INDICES_t sourceIndex)
{
    L7_RC_t rc;
    osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 
    if (ospfMapIsValidSourceProto(sourceIndex)) {
        rc = L7_SUCCESS;
    }
    else {
        rc = L7_FAILURE;
    }
    osapiSemaGive(ospfMapCtrl_g.cfgSema);
    return rc;
}

/*********************************************************************
* @purpose  Given a route redistribution source, set nextSourceIndex
*           to the next valid source of redistributed routes.
*
* @param    sourceIndex - A source for redistributed routes.
* @param    nextSourceIndex - The source index following sourceIndex. 
*
* @returns  L7_SUCCESS if there is a valid nextSourceIndex
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.  
*
* @end
*********************************************************************/
L7_RC_t ospfMapRouteRedistributeGetNext(L7_REDIST_RT_INDICES_t sourceIndex,
                                        L7_REDIST_RT_INDICES_t *nextSourceIndex)
{
    if (nextSourceIndex == L7_NULLPTR) {
        return L7_FAILURE;
    }
    *nextSourceIndex = sourceIndex + 1;
    while (*nextSourceIndex != REDIST_RT_LAST) {
        if (ospfMapIsValidSourceProto(*nextSourceIndex)) {
           return L7_SUCCESS;
        }
        (*nextSourceIndex) += 1;

    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Flush and reoriginate all self-originated external LSAs.
*
* @param    void
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_NOT_EXIST if OSPF is not initialized
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapSelfOrigExtLsaFlush(void)
{
  L7_RC_t rc;
  L7_uint32 areaIndex;

  if (ospfMapOspfInitialized() != L7_TRUE)
  {
      return L7_NOT_EXIST;
  }

  osapiSemaTake(ospfMapCtrl_g.cfgSema, L7_WAIT_FOREVER); 

  /* Flush */
  if (ospfMapExtenPurgeExternalLsas() != L7_SUCCESS)
    return L7_FAILURE;

  /* Trigger reorigination */
  rc = ospfMapExtenRedistReevaluate();

  /* Reoriginate default T5 LSA if configured to do so. */
  if (pOspfMapCfgData->defRouteCfg.origDefRoute) 
  {
    ospfMapExtenDefRouteOrig();
  }

  /* For each NSSA area, originate a T7 default if configured to do so. */
  for (areaIndex = 1;  areaIndex < L7_OSPF_MAX_AREAS; areaIndex++)
  {
    if (pOspfMapCfgData->area[areaIndex].inUse == L7_FALSE)
      continue;

    if ((pOspfMapCfgData->area[areaIndex].extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA) &&
        pOspfMapCfgData->area[areaIndex].nssaCfg.defInfoOrig)
    {
      ospfMapExtenNSSADefaultInfoOriginateSet(pOspfMapCfgData->area[areaIndex].area, L7_FALSE);
      ospfMapExtenNSSADefaultInfoOriginateSet(pOspfMapCfgData->area[areaIndex].area, L7_TRUE);
    }
  }

  osapiSemaGive(ospfMapCtrl_g.cfgSema);
  return rc;
}

