/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rip_map_util.c
*
* @purpose   RIP Utility Functions
*
* @component RIP Mapping Layer
*
* @comments  none
*
* @create    05/21/2001
*
* @author    gpaussa
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_ripinclude.h"
#include "bspapi.h"


extern ripMapCnfgrState_t   ripMapCnfgrState; 
extern ripMapCfg_t          *pRipMapCfgData;
extern L7_uint32            *pRipMapMapTbl;
extern void                 *ripMapSemaphore;

static void ripMapIntfBuildTestConfigData(ripMapCfgIntf_t *pCfg);

/*---------------------------------------------------------------------
 *                  SUPPORT FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Take global RIP configuration semaphore
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void ripMapSemaTake(void)
{
  osapiSemaTake(ripMapSemaphore, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  Give global RIP configuration semaphore
*
* @param    void
*
* @returns  void
*
* @notes    none
*       
* @end
*********************************************************************/
void ripMapSemaGive(void)
{
  osapiSemaGive(ripMapSemaphore);
}

/*********************************************************************
* @purpose  Enable the RIP Routing Function
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapAdminModeEnable(void)
{
  L7_uint32     i, max;
  L7_uint32     intIfNum;
  L7_REDIST_RT_INDICES_t   index;

  static const char *routine_name = "ripMapAdminModeEnable()";

  /* Check if IP Routing has been enabled */
  if (ipMapRtrAdminModeGet() == L7_DISABLE)
    return L7_SUCCESS;

  /* apply the RIP configuration change */
  if (ripMapExtenRipAdminModeSet(L7_ENABLE) != L7_SUCCESS)
    return L7_FAILURE;

  /* Store the applied configuration */
  pRipInfo->ripAdminMode = L7_ENABLE;

  /* enable router interfaces which are configured for routing */
  max = ipmRouterIfMaxCountGet();
  for (i = RIP_MAP_RTR_INTF_FIRST; i <= max; i++)
  {
    /* NOTE:  This call fails if router intf not created yet */
    if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
    {
      if (ripMapIntfMayParticipate(intIfNum) == L7_TRUE)
      {
        /* ...enable interface RIP operation */
        if (ripMapIntfAdminModeEnable(intIfNum) != L7_SUCCESS)
        {
          LOG_MSG("ripMapAdminModeEnable: State change callback failed " \
                  "for router intf %d\n", i);
        }
      }
    }
  }
  if (ripMapSplitHorizonModeApply(pRipMapCfgData->ripSplitHorizon) != L7_SUCCESS)
  {
    LOG_MSG("ripMapAdminModeEnable: Could not apply split horizon mode.\n");
  }
  if (ripMapAutoSummarizationModeApply(pRipMapCfgData->ripAutoSummarization) != L7_SUCCESS)
  {
    LOG_MSG("ripMapAdminModeEnable: Could not apply auto-summarization mode.\n");
  }
  if (ripMapHostRoutesAcceptModeApply(pRipMapCfgData->ripHostRoutesAccept) != L7_SUCCESS)
  {
    LOG_MSG("ripMapAdminModeEnable: Could not apply host routes accept mode.\n");
  }  

  /* Register RIP to be notified of ACL changes. 
   * RIP uses ACLs to filter routes for redistribution. 
   */
  if (ripMapAclCallbackIsRegistered(ripMapAclCallback) == L7_FALSE) 
  {
    if (ripMapAclCallbackRegister(ripMapAclCallback, "ripMapAclCallback") == L7_FAILURE) 
    {
      LOG_MSG("\n%s %d: %s: ERROR:  Failed to register for ACL changes.\n",
              __FILE__, __LINE__, routine_name);
    }
  }

  /* No need to apply the default metric. Since it is used by default route
     advertise mode apply and route redistribution parms apply functions
     anyway, we have nothing to do about it.
  */
  
  /* No need to apply the route filter. Since it is used by route redistribution parms
     apply function, we have nothing to do about it.
  */

  /* Apply the default route adv mode. */
  if (pRipMapCfgData->ripDefRtAdv.defRtAdv == L7_ENABLE)
  {
    if (ripMapDefaultRouteAdvertiseModeApply(pRipMapCfgData->ripDefRtAdv.defRtAdv) != L7_SUCCESS)
    {
      LOG_MSG("ripMapAdminModeEnable: Could not apply Default Route Advertise mode.\n");
    }
  }

  /* Apply the route redist parms for all protocols */
  for (index = REDIST_RT_FIRST+1; index < REDIST_RT_LAST; index ++)
  {
    if ((index == REDIST_RT_RIP) || 
        (pRipMapCfgData->ripRtRedist[index].redistribute != L7_ENABLE))
      continue;

    if (ripMapRouteRedistributeModeApply(index, 
                                         pRipMapCfgData->ripRtRedist[index].redistribute) 
        != L7_SUCCESS)
    {
      LOG_MSG("ripMapAdminModeEnable: Could not apply Route Redist for routes from -- protocol.\n");
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Disable the RIP Routing Function
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapAdminModeDisable(void)
{
  L7_uint32     i, max;
  L7_uint32     intIfNum;
  L7_REDIST_RT_INDICES_t   index;

  static const char *routine_name = "ripMapAdminModeDisable()";

  /* NOTE:  Return success if RIP has not been initialized. 
   *        This allows for dynamic configurability of RIP prior to
   *        initializing RIP.  The API function stores the configuration 
   *        value.  When RIP is eventually enabled by the user, RIP
   *        will use the configured values.
   */
  if (ripMapIsInitialized() != L7_TRUE)
    return L7_SUCCESS;

  /* Disable the interfaces before disabling RIP globally to ensure that the RIP vendor task
    is enabled to send signals back to the RIP mapping task. */  
  max = ipmRouterIfMaxCountGet();
  for (i = RIP_MAP_RTR_INTF_FIRST; i <= max; i++)
  {
    if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
    {
      ripMapCfgIntf_t *pCfg = L7_NULL;
      /* Check configuration to see if RIP is enabled for the interface... */
      /* do not use ripMapIntfAdminModeGet, external API takes semaphore */
      if ((ripMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) &&
          (pCfg->adminMode == L7_ENABLE))
      {
        /* ...disable interface RIP operation */
        if (ripMapIntfAdminModeDisable(intIfNum) != L7_SUCCESS)
        {
          LOG_MSG("ripMapAdminModeDisable: State change callback failed " \
                  "for router intf %d\n", i);
        }
      }
    }
  }

  /* apply the RIP configuration change */
  if (ripMapExtenRipAdminModeSet(L7_DISABLE) != L7_SUCCESS)
    return L7_FAILURE;
    
  /* Stop receiving notifications for ACL changes. */
  if (ripMapAclCallbackIsRegistered(ripMapAclCallback) == L7_TRUE) 
  {
    if (ripMapAclCallbackUnregister(ripMapAclCallback) == L7_FAILURE) 
    {
      LOG_MSG("\n%s %d: %s: ERROR:  Failed to de-register for ACL changes.\n",
              __FILE__, __LINE__, routine_name);
    }
  }

  /* No need to apply the default metric. Since it is used by default route
     advertise mode apply and route redistribution parms apply functions
     anyway, we have nothing to do about it.
  */

  /* No need to apply the route filter. Since it is used by route redistribution parms
     apply function, we have nothing to do about it.
  */

  /* Apply the default route adv mode. */
  if (pRipInfo->ripDefRtAdv == L7_ENABLE)
  {
    if (ripMapDefaultRouteAdvertiseModeApply(L7_DISABLE) != L7_SUCCESS)
    {
      LOG_MSG("ripMapAdminModeDisable: Could not apply Default Route Advertise mode.\n");
    }
  }

  /* Apply the route redist parms for all protocols */
  for (index = REDIST_RT_FIRST+1; index < REDIST_RT_LAST; index ++)
  {
    if ((index == REDIST_RT_RIP) || 
        (pRipInfo->ripRtRedistMode[index] != L7_ENABLE))
      continue;
  
    pRipInfo->ripRtRedistMode[index] = L7_DISABLE;
    if(ripMapExtenRipRouteRedistributeModeSet(index, L7_DISABLE) != L7_SUCCESS)
    {
      LOG_MSG("ripMapAdminModeDisable: Could not disable Route Redist for routes from -- protocol %d\n", index);
    }
  }

  /* Reevaluate whether RIP should be registered with RTO for best route changes. */
  ripRtoRegisterUpdate();

  /* Store the applied configuration */
  pRipInfo->ripAdminMode = L7_DISABLE;

  /* Flush all routes in the internal rip tables */
  /* TODO: Dead code
  if (ripMapRouteTableFlushCallBack() != L7_SUCCESS)
  {
    LOG_MSG("ripMapAdminModeDisable: RIP route table flush failed \n");
  }
  */
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Apply the RIP split horizon mode
*
* @param    val   @b{(input)} Split Horizon mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapSplitHorizonModeApply(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  if (ripMapIsInitialized() != L7_TRUE)
    return L7_SUCCESS;

  /* If RIP is not active right now, then do not apply it*/
  if (pRipInfo->ripAdminMode != L7_ENABLE)
    return L7_SUCCESS;

  /* apply the configuration change */
  rc = ripMapExtenRipSplitHorizonModeSet(val);

  return rc;
}

/*********************************************************************
* @purpose  Apply the RIP auto-summarization mode
*
* @param    val   @b{(input)} Auto-Summarization mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapAutoSummarizationModeApply(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  if (ripMapIsInitialized() != L7_TRUE)
    return L7_SUCCESS;

  /* If RIP is not active right now, then do not apply it*/
  if (pRipInfo->ripAdminMode != L7_ENABLE)
    return L7_SUCCESS;

  /* apply the configuration change */
  rc = ripMapExtenRipAutoSummarizationModeSet(val);

  return rc;
}


/*********************************************************************
* @purpose  Apply the RIP host route accept mode
*
* @param    val         @b{(input)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapHostRoutesAcceptModeApply(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  if (ripMapIsInitialized() != L7_TRUE)
    return L7_SUCCESS;

  /* If RIP is not active right now, then do not apply it*/
  if (pRipInfo->ripAdminMode != L7_ENABLE)
    return L7_SUCCESS;

  /* apply the configuration change */
  rc = ripMapExtenRipHostRoutesAcceptModeSet(val);

  return rc;
}

/*********************************************************************
* @purpose  Apply the default metric
*
* @param    val         @b{(input)} Default Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapDefaultMetricApply(L7_uint32 val)
{
  L7_RC_t rc1 = L7_SUCCESS;
  L7_RC_t rc2 = L7_SUCCESS;
  L7_REDIST_RT_INDICES_t   index;

  if (val == L7_RIP_METRIC_INVALID)
    val = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;

  /* If def route adv mode is enabled, apply it. */
  if (pRipInfo->ripDefRtAdv == L7_ENABLE)
  {
    rc1 = ripMapDefaultRouteAdvertiseModeApply(L7_ENABLE);
  }

  /* The exten function goes thru all the redistributed protocols and modifies
     the metric.
  */   
  for (index = REDIST_RT_FIRST+1; index < REDIST_RT_LAST; index ++)
  {
    if ((index == REDIST_RT_RIP) || (pRipInfo->ripRtRedistMode[index] != L7_ENABLE))
      continue;

    break;
  }
  if (index < REDIST_RT_LAST)
    rc2 = ripMapExtenRipDefaultMetricSet(val); 

  if ((rc1 != L7_SUCCESS) || (rc2 != L7_SUCCESS))
    return L7_FAILURE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply the default route advertise mode
*
* @param    val         @b{(input)} Default Route Advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapDefaultRouteAdvertiseModeApply(L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Nothing to do if admin mode is disabled */
  if (pRipInfo->ripAdminMode != L7_ENABLE)
    return L7_SUCCESS;

  if (val == L7_ENABLE)
    rc = ripMapDefaultRouteAdvertiseModeEnable();
  else
    rc = ripMapDefaultRouteAdvertiseModeDisable();

  return rc;
}

/*********************************************************************
* @purpose  This function is used to start advertising the default route
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapDefaultRouteAdvertiseModeEnable()
{
  L7_uint32 metric;
  L7_RC_t rc = L7_SUCCESS;

  pRipInfo->ripDefRtAdv = L7_ENABLE;
  if (ripMapDefaultMetricGet(&metric) == L7_SUCCESS) 
  {
    if ((metric < L7_RIP_CONF_METRIC_MIN) || (metric > L7_RIP_CONF_METRIC_MAX))
      metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
  }
  else
  {
    metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
  }

  rc = ripMapExtenRipDefaultRouteAdvertiseSet(L7_ENABLE, metric);

  return rc;
}


/*********************************************************************
* @purpose  This function is used to stop advertising the default route
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapDefaultRouteAdvertiseModeDisable()
{
  L7_RC_t rc = L7_SUCCESS;
  
  pRipInfo->ripDefRtAdv = L7_DISABLE;
  /* The second parm can be ignored here. so it is arbitrary 0. */
  rc = ripMapExtenRipDefaultRouteAdvertiseSet(L7_DISABLE, 0);
  return rc;
}


/*********************************************************************
* @purpose  Apply the Route Redistribute Filter
*
* @param    protocol         @b{(input)} The source protocol
* @param    val              @b{(input)} ACL used for filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeFilterApply(L7_REDIST_RT_INDICES_t protocol, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;

  /* The below mode is set only if admin  mode is enabled and redist is enabled. */
  /* Nothing to do if mode is disabled */
  if (pRipInfo->ripRtRedistMode[protocol] != L7_ENABLE)
    return L7_SUCCESS;
  
  rc = ripMapExtenRipRouteRedistributeFilterSet(protocol, val);

  return rc;
}

/*********************************************************************
* @purpose  Determine whether RIP is registered with RTO for best route changes.
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL ripIsRegisteredWithRto(void)
{
  return pRipInfo->registeredWithRto;
}

/*********************************************************************
* @purpose  Determine whether RIP is configured to redistribute routes
*           from any source.
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL ripRedistributes(void)
{
  L7_uint32 index;
  for (index = REDIST_RT_FIRST + 1; index < REDIST_RT_LAST; index++)
  {
    if (pRipInfo->ripRtRedistMode[index] == L7_ENABLE)
      return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether RIP should be registered with RTO 
*           for best route changes.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    RIP only listens to best route changes if RIP is enabled 
*           globally and RIP is configured to redistribute something.
*       
* @end
*********************************************************************/
L7_BOOL ripShouldListenToBestRoutes(void)
{
  return ((pRipMapCfgData && (pRipMapCfgData->ripAdminMode == L7_ENABLE)) &&
          ripRedistributes());
}

/*********************************************************************
* @purpose  If configuration change requires a change in whether RIP
*           is registered with RTO, register or deregister as necessary.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    RIP only listens to best route changes if RIP is enabled 
*           globally and RIP is configured to redistribute something.
*       
* @end
*********************************************************************/
L7_RC_t ripRtoRegisterUpdate(void)
{
  if (ripShouldListenToBestRoutes())
  {
    if (!ripIsRegisteredWithRto())
    {
      /* Register with RTO to receive best route callback */
      if (rtoBestRouteClientRegister("RIP Redist", 
                                     ripMapBestRouteChangeCallback) != L7_SUCCESS)
      {
        L7_LOG(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
               "RIP failed to register with RTO for best route changes.");
        return L7_FAILURE;
      }
      pRipInfo->registeredWithRto = L7_TRUE;
    }
  }
  else
  {
    /* should not be registered */
    if (ripIsRegisteredWithRto())
    {
      if (rtoBestRouteClientDeregister("RIP Redist", 
                                       ripMapBestRouteChangeCallback) != L7_SUCCESS)
      {
        L7_LOG(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
               "RIP failed to deregister with RTO for best route changes.");
        return L7_FAILURE;
      }
      pRipInfo->registeredWithRto = L7_FALSE;

      /* Since we are deregistering with RTO, purge all the routes we learned
       * earlier from RTO */
      ripMapExtenRipPurgeRedistRoutes();
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply the route redistribute parms
*
* @param    protocol         @b{(input)} The source protocol
* @param    metric           @b{(input)} Metric used to redistribute
* @param    matchType        @b{(input)} MatchType used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeParmsApply(L7_REDIST_RT_INDICES_t protocol, 
                                          L7_uint32 metric, 
                                          L7_OSPF_METRIC_TYPES_t matchType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 tempMetric;

  /* The below mode is set only if admin  mode is enabled and redist is enabled. */
  /* Nothing to do if mode is disabled */
  if (pRipInfo->ripRtRedistMode[protocol] != L7_ENABLE)
    return L7_SUCCESS;
  
  if ((metric < L7_RIP_CONF_METRIC_MIN) || (metric > L7_RIP_CONF_METRIC_MAX))
  {
    if (ripMapDefaultMetricGet(&tempMetric) == L7_SUCCESS) 
    {
      if ((tempMetric < L7_RIP_CONF_METRIC_MIN) || (tempMetric > L7_RIP_CONF_METRIC_MAX))
        metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
      else
        metric = tempMetric;
    }
    else
    {
      metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
    }
  }

  rc = ripMapExtenRipRouteRedistributeParmsSet(protocol, metric, matchType);

  return rc;
}

/*********************************************************************
* @purpose  Apply the route redistribute metric
*
* @param    protocol         @b{(input)} The source protocol
* @param    metric           @b{(input)} Metric used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeMetricApply(L7_REDIST_RT_INDICES_t protocol, 
                                           L7_uint32 metric)
{
  L7_uint32 tempMetric;
  L7_RC_t rc = L7_SUCCESS;

  /* The below mode is set only if admin  mode is enabled and redist is enabled. */
  /* Nothing to do if mode is disabled */
  if (pRipInfo->ripRtRedistMode[protocol] != L7_ENABLE)
    return L7_SUCCESS;
  
  if ((metric < L7_RIP_CONF_METRIC_MIN) || (metric > L7_RIP_CONF_METRIC_MAX))
  {
    if (ripMapDefaultMetricGet(&tempMetric) == L7_SUCCESS) 
    {
      if ((tempMetric < L7_RIP_CONF_METRIC_MIN) || (tempMetric > L7_RIP_CONF_METRIC_MAX))
        metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
      else
        metric = tempMetric;
    }
    else
    {
      metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
    }
  }

  rc = ripMapExtenRipRouteRedistributeMetricSet(protocol, metric);

  return rc;
}

/*********************************************************************
* @purpose  Apply the route redistribute matchType
*
* @param    protocol         @b{(input)} The source protocol
* @param    matchType        @b{(input)} MatchType used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeMatchTypeApply(L7_REDIST_RT_INDICES_t protocol, 
                                              L7_OSPF_METRIC_TYPES_t matchType)
{
  L7_RC_t rc = L7_SUCCESS;
  
  /* The below mode is set only if admin  mode is enabled and redist is enabled. */
  /* Nothing to do if mode is disabled */
  if (pRipInfo->ripRtRedistMode[protocol] != L7_ENABLE)
    return L7_SUCCESS;

  rc = ripMapExtenRipRouteRedistributeMatchTypeSet(protocol, matchType);

  return rc;
}

/*********************************************************************
* @purpose  Apply the route redistribute mode
*
* @param    protocol         @b{(input)} The source protocol
* @param    mode             @b{(input)} Enable / Disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeModeApply(L7_REDIST_RT_INDICES_t protocol, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;
  
  /* Nothing to do if admin mode is disabled */
  if (pRipInfo->ripAdminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }

  pRipInfo->ripRtRedistMode[protocol] = mode;
  rc = ripMapExtenRipRouteRedistributeModeSet(protocol, mode);

  /* Reevaluate whether RIP should be registered with RTO for best route changes. */
  ripRtoRegisterUpdate();

  return rc;
}


/*---------------------------------------------------------------------
 *                 SUPPORT FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Set the RIP admin mode for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} Administrative mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If mode is L7_ENABLE, assumes caller has already verified
*           that all configuration conditions are met for RIP to be
*           active on this interface.
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAdminModeApply(L7_uint32 intIfNum, L7_uint32 mode)
{
  if ((mode == L7_ENABLE) && 
      (pRipIntfInfo[intIfNum].state == RIP_MAP_INTF_STATE_UP))
    return L7_SUCCESS;
  
  if ((mode == L7_DISABLE) && 
      (pRipIntfInfo[intIfNum].state == RIP_MAP_INTF_STATE_DOWN))
    return L7_SUCCESS;
  
  if (mode == L7_ENABLE)
  {
    if (dtlIpv4LocalMulticastAddrAdd(intIfNum, L7_IP_RIPV2_RTRS_ADDR) 
        != L7_SUCCESS)
    {
      LOG_MSG("ripMapIntfAdminModeApply: Could not add RIP multicast addr\n");
      /* avoid bringing RIP interface up */
      return L7_FAILURE;
    }
  }
  if (mode == L7_DISABLE)
  {
    /* delete RIPv2 router multicast address from NPU */
    if (dtlIpv4LocalMulticastAddrDelete(intIfNum, L7_IP_RIPV2_RTRS_ADDR) 
        != L7_SUCCESS)
    {
      LOG_MSG("ripMapIntfAdminModeApply: Could not delete RIP multicast addr\n");
      /* continue to be sure RIP marks interface down */
    }
  }
  
  if (mode == L7_ENABLE)
  {
    if (ripMapIntfIsActive(intIfNum) == L7_TRUE)
    {
      ripMapVendIntfUp(intIfNum);
    }
  }
  else
  {
    ripMapVendIntfDown(intIfNum);
  }
                          
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP authentication type for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    authType    @b{(input)} Authentication type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthTypeApply(L7_uint32 intIfNum, L7_uint32 authType)
{
  /* don't apply config unless intf is created (return w/success for pre-config) */
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
    return L7_SUCCESS;

  return(ripMapExtenIntfAuthTypeSet(intIfNum, L7_NULL, authType));
}

/*********************************************************************
* @purpose  Set the RIP authentication key for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    key         @b{(input)} Authentication key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The authentication key is passed as the maximum 16 characters
*           with appropriate zero padding on the right hand side.
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeyApply(L7_uint32 intIfNum, L7_uchar8 *key)
{
  /* don't apply config unless intf is created (return w/success for pre-config) */
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
    return L7_SUCCESS;

  return(ripMapExtenIntfAuthKeySet(intIfNum, L7_NULL, key));
}

/*********************************************************************
* @purpose  Set the RIP authentication key id for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    authType    @b{(input)} Authentication key id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeyIdApply(L7_uint32 intIfNum, L7_uint32 authKeyId)
{
  /* don't apply config unless intf is created (return w/success for pre-config) */
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
    return L7_SUCCESS;

  return(ripMapExtenIntfAuthKeyIdSet(intIfNum, L7_NULL, authKeyId));
}

/*********************************************************************
* @purpose  Set the RIP version used for sending updates on the specified 
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    version     @b{(input)} RIP version used for sends
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfVerSendApply(L7_uint32 intIfNum, L7_uint32 version)
{
  /* don't apply config unless intf is created (return w/success for pre-config) */
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
    return L7_SUCCESS;

  return(ripMapExtenIntfVerSendSet(intIfNum, L7_NULL, version));
}

/*********************************************************************
* @purpose  Set the RIP version used for receiving updates from the specified 
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    version     @b{(input)} RIP version allowed for receives
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfVerRecvApply(L7_uint32 intIfNum, L7_uint32 version)
{
  /* don't apply config unless intf is created (return w/success for pre-config) */
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
    return L7_SUCCESS;

  return(ripMapExtenIntfVerRecvSet(intIfNum, L7_NULL, version));
}


/*---------------------------------------------------------------------
 *                      OTHER UTILITY FUNCTIONS
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Determine if the RIP component has been initialized
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    ripMapSemaphore must be locked when calling this function
* @notes    This is often used to determine whether a configuration 
*           value can be applied or not.
*       
* @end
*********************************************************************/
L7_BOOL ripMapIsInitialized(void)
{
  if (pRipInfo == L7_NULL)
  {
    /* RIP component was not started during system initialization */
    LOG_MSG("WARNING:  Invalid build - RIP component was not started\n");
    L7_assert(1);
    return L7_FALSE;
  }

  /* return value based on current RIP initialization state */
  return(pRipInfo->ripInitialized);
}

/*********************************************************************
* @purpose  Determines if the specified internal interface is created
*           for RIP and is currently active, meaning the link is up and
*           in forwarding state
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_BOOL ripMapIntfIsActive(L7_uint32 intIfNum)
{
  L7_uint32 activeState;

  if ((nimGetIntfActiveState(intIfNum, &activeState) == L7_SUCCESS) &&
      (activeState == L7_ACTIVE))
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determines if the specified router interface is operable
*           for RIP operation
*
* @param    rtrIfNum    @b{(input)} Router Interface Number
* @param    *pIntIfNum  @b{(output)} location to store internal
*                         interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    ripMapSemaphore must be locked when calling this function
* @notes    The meaning of 'operable' here is that the router interface  
*           is made available to the RIP protocol, namely, it is 
*           detected for use by ifinit().
*       
* @end
*********************************************************************/
L7_BOOL ripMapRtrIntfIsOperable(L7_uint32 rtrIfNum, L7_uint32 *pIntIfNum)
{
  L7_uint32        intIfNum;
  L7_uint32        mode;
  ripMapCfgIntf_t *pCfg = L7_NULL;

  /* do not use ripMapIntfAdminModeGet, external API takes semaphore */
  if ((ipMapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) == L7_SUCCESS) &&
      (ripMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE) &&
      (pCfg->adminMode == L7_ENABLE))
  {
    if (ipMapRtrIntfModeGet(intIfNum, &mode) == L7_SUCCESS && mode == L7_ENABLE)
    {
      *pIntIfNum = intIfNum;
      return L7_TRUE;
    }
  }

  *pIntIfNum = 0;
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determines if RIP protocol may participate in
*           RIP operation
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Returns true if all of the following conditions are true
*           
*           - Routing is enabled
*           - RIP is enabled
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_BOOL ripMapProtocolMayParticipate(void)
{
  L7_uint32 mode;

  if (ipMapRtrAdminModeGet() == L7_ENABLE)
  {
    if (ripMapAdminModeGet(&mode) == L7_SUCCESS && mode == L7_ENABLE)
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;   
}

/*********************************************************************
* @purpose  Determines if an interface has been acquired by another 
*           component and is therefore unavailable to RIP.
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_TRUE if interface has been acquired
* @returns  L7_FALSE
*
* @notes    Checks interface acquisition for:
*              - port mirroring
*              - LAGs
*
*           The interface is not considered to be acquired in this 
*           context if it has been acquired for routing. For this 
*           reason, the function checks specific acquiring components
*           rather than checking for a non-zero acquired mask.
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_BOOL ripMapIntfAcquired(L7_uint32 intIfNum)
{
  if (COMPONENT_ACQ_ISMASKBITSET(pRipIntfInfo[intIfNum].acquiredList, 
                                 L7_PORT_MIRROR_COMPONENT_ID) ||
      COMPONENT_ACQ_ISMASKBITSET(pRipIntfInfo[intIfNum].acquiredList, 
                                 L7_DOT3AD_COMPONENT_ID))
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}

/*********************************************************************
* @purpose  Determines if the specified interface is configured to
*           participate in RIP.
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Returns true if all of the following conditions are true
*           
*           - Routing is enabled
*           - RIP is enabled
*           - Routing is enabled on the interface
*           - RIP is enabled on the interface
*           - A non-zero IP address exists 
*           - The interface has not been acquired, say by a LAG or probe
*
*           This function is only concerned with configuration information,
*           not with state information (e.g., is the interface active).
* @notes    ripMapSemaphore must be locked when calling this function
*       
* @end
*********************************************************************/
L7_BOOL ripMapIntfMayParticipate(L7_uint32 intIfNum)
{
  L7_uint32        mode;
  L7_IP_ADDR_t     ipAddr, ipMask;
  ripMapCfgIntf_t *pCfg;
  
  /* routing enabled? */
  if (ipMapRtrAdminModeGet() != L7_ENABLE)
  {
    return L7_FALSE;
  }

  /* RIP enabled? */
  if (ripMapAdminModeGet(&mode) != L7_SUCCESS || mode != L7_ENABLE)
  {
    return L7_FALSE;
  }

  /* routing enabled on interface? */
  if ((ipMapRtrIntfOperModeGet(intIfNum, &mode) != L7_SUCCESS) || 
      (mode != L7_ENABLE))
  {
    return L7_FALSE;
  }

  /* RIP enabled on interface? */
  /* do not use ripMapIntfAdminModeGet, external API takes semaphore */
  if ((ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE) ||
      (pCfg->adminMode != L7_ENABLE))
  {
    return L7_FALSE;
  }

  /* non-zero address on interface? */
  if ((ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) != L7_SUCCESS) ||
      ((ipAddr == 0) || (ipMask == 0)))
  {
    return L7_FALSE;
  }

  /* check if currently acquired */
  if (ripMapIntfAcquired(intIfNum) == L7_TRUE)
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Dumps the rip configuration information onto the display
*           screen
*
* @param    void
*
* @notes    none
*       
* @end
*********************************************************************/
void ripMapCfgDataShow(void)
{
  L7_uint32 i;
  L7_uint32 intIfNum;
  nimUSP_t usp;
  L7_uchar8 c = 'n';
  L7_uchar8 keyStr[L7_AUTH_MAX_KEY_RIP+1];
  ripRtRedistCfg_t *pRedist;
  char *redistProtoStr[] = { "---", "local", "static", "---", "ospf", "bgp", "---" };
  char *splitHorizonType = "None";  

  ripMapSemaTake();

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nDo you wish to see all interfaces? ");
  osapiFileRead(bspapiConsoleFdGet(), &c, sizeof(c));

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n+--------------------------------------------------------+");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n|              RIP CONFIGURATION DATA                    |");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n+--------------------------------------------------------+");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n\nHeader Information:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nfilename...............................%s", pRipMapCfgData->cfgHdr.filename);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nversion................................%d", pRipMapCfgData->cfgHdr.version);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\ncomponentID............................%d", pRipMapCfgData->cfgHdr.componentID);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\ntype...................................%d", pRipMapCfgData->cfgHdr.type);
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nlength.................................%d", pRipMapCfgData->cfgHdr.length);

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n\nRIP specific Information:");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nripAdminMode......................%s",
                 (pRipMapCfgData->ripAdminMode == L7_ENABLE) ? "enable" : "disable");

  if (pRipMapCfgData->ripSplitHorizon == L7_RIP_SPLITHORIZON_NONE)
    splitHorizonType = "None";
  else if (pRipMapCfgData->ripSplitHorizon == L7_RIP_SPLITHORIZON_SIMPLE)
    splitHorizonType = "Simple";
  else if (pRipMapCfgData->ripSplitHorizon == L7_RIP_SPLITHORIZON_POISONREVERSE)
    splitHorizonType = "Poison Reverse";

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nripSplitHorizon...................%s",
                 splitHorizonType);
  
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nripAutoSummarization..............%s",
                 (pRipMapCfgData->ripAutoSummarization == L7_ENABLE) ? "enable" : "disable");
  
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nripHostRoutesAccept...............%s",
                 (pRipMapCfgData->ripHostRoutesAccept == L7_ENABLE) ? "enable" : "disable");
  
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nDefault route advertisement:  %s",
                 (pRipMapCfgData->ripDefRtAdv.defRtAdv == 0) ? "Off" : "On");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nDefault Metric:  %d", pRipMapCfgData->ripDefaultMetric);
  
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nredist  redist   metric   OSPF  filter  filter  ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nproto    flag             type   mode     ID    ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n------------------------------------------------");
  for (i = REDIST_RT_FIRST+1; i < REDIST_RT_LAST; i++)
  {
    if (i == REDIST_RT_RIP)
      continue;
    pRedist = &(pRipMapCfgData->ripRtRedist[i]);
    SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n%-6.6s     %1u    %8u  0x%2.2x     %1u      %2u    ",
                   redistProtoStr[i], pRedist->redistribute, pRedist->metric, pRedist->matchType,                                                   
                   pRedist->ripRtRedistFilter.mode, pRedist->ripRtRedistFilter.filter);
  }
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n");

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n--------------------------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nslot.   admin    auth    auth key   auth     ver    ver       intf  auth              ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nport    mode     type    length     key id   send   recv      cost  key               ");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n--------------------------------------------------------------------------------------");

  for (i = 1; i < L7_RIP_MAP_INTF_MAX_COUNT; i++)
  {
    if ((pRipMapCfgData->intf[i].adminMode == L7_ENABLE) || (c == 'y') || (c == 'Y'))
    {

      if (nimIntIfFromConfigIDGet(&pRipMapCfgData->intf[i].configId, &intIfNum) == L7_SUCCESS)
      {

          memset((void *)&usp, 0, sizeof(nimUSP_t));
          if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
          {
              SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n%d/%d/%d", 
                             usp.unit, usp.slot, usp.port);
          }
          else
          {

              SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n%0/0/0");
          }
      }
      if (pRipMapCfgData->intf[i].adminMode == L7_ENABLE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\t  E");
      }
      else
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\t  D");
      }

      if (pRipMapCfgData->intf[i].authType == L7_AUTH_TYPE_NONE)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\t None");
      }
      else if (pRipMapCfgData->intf[i].authType == L7_AUTH_TYPE_SIMPLE_PASSWORD)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\tSimple");
      }
      else if (pRipMapCfgData->intf[i].authType == L7_AUTH_TYPE_MD5)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\t MD5");
      }

      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "       %d", pRipMapCfgData->intf[i].authKeyLen);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "         %d", pRipMapCfgData->intf[i].authKeyId);

      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "      %d", pRipMapCfgData->intf[i].verSend);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "       %d", pRipMapCfgData->intf[i].verRecv);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "        %d", pRipMapCfgData->intf[i].defaultMetric);
      SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "        %d", pRipMapCfgData->intf[i].intfCost);

      memcpy(keyStr, pRipMapCfgData->intf[i].authKey, (size_t)L7_AUTH_MAX_KEY_RIP);
      *(keyStr+L7_AUTH_MAX_KEY_RIP) = L7_EOS;
      if (strlen(keyStr) > 0)
      {
        SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "    \"%s\"", keyStr);
      }
    }
  }

  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n-----------------------------------------------------------------------------------------");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nKey:");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\nE: Enable\tD: Disable");
  SYSAPI_PRINTF( SYSAPI_APPLICATION_LOGGING_RIPMAP,  "\n");

  ripMapSemaGive();
  return;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_BOOL ripMapIntfIsConfigurable(L7_uint32 intIfNum, ripMapCfgIntf_t **pCfg)
{
  nimConfigID_t configId;
  L7_uint32 index = 0;

  if (RIPMAP_IS_READY != L7_TRUE)
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  if (ripMapIntfIsValid(intIfNum) == L7_FALSE)
    return L7_FALSE;

  index = pRipMapMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(pRipMapCfgData->intf[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between ospfMapCfg and ospfMapMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      LOG_MSG("Error accessing RIPMAP config data for interface %d in rippMapIntfIsConfigurable.\n", intIfNum);
      return L7_FALSE;
    }
  }

  if (pCfg)
  {
    *pCfg = &pRipMapCfgData->intf[index];
  }

  return L7_TRUE;
}


/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to policy interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
* @notes    ripMapSemaphore must be locked when calling this function
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL ripMapIntfConfigEntryGet(L7_uint32 intIfNum, ripMapCfgIntf_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (RIPMAP_IS_READY != L7_TRUE)
    return L7_FALSE;

  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pRipMapCfgData->intf[i].configId, &configIdNull))
      {
        pRipMapMapTbl[intIfNum] = i;
        *pCfg = &pRipMapCfgData->intf[i];
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Set the state of an interface for RIP.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    state   @b{(input)}  the new state of the interface
*
* @returns  void
*
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
void ripIntfStateSet(L7_uint32 intIfNum, ripMapIntfState_t state)
{
    pRipIntfInfo[intIfNum].state = state;
}



/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void ripMapBuildTestConfigData(void)
{

  L7_REDIST_RT_INDICES_t   index;
  L7_uint32 cfgIndex;
  ripMapCfgIntf_t *pCfg;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

 
  /* generic RIP cfg */
  pRipMapCfgData->ripAdminMode = L7_DISABLE;
  pRipMapCfgData->ripSplitHorizon = L7_RIP_SPLITHORIZON_POISONREVERSE;
  pRipMapCfgData->ripAutoSummarization = L7_DISABLE;
  pRipMapCfgData->ripHostRoutesAccept = L7_DISABLE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    pCfg = &pRipMapCfgData->intf[cfgIndex];
    ripMapIntfBuildTestConfigData(pCfg);
  }

  pRipMapCfgData->ripDefaultMetric = 0xabcdabcd;
  pRipMapCfgData->ripDefRtAdv.defRtAdv = L7_ENABLE;
  for (index = REDIST_RT_FIRST+1; index < REDIST_RT_LAST; index ++)
  {
    pRipMapCfgData->ripRtRedist[index].redistribute = L7_ENABLE;
    pRipMapCfgData->ripRtRedist[index].metric = 0xabcd0000 + index;
    pRipMapCfgData->ripRtRedist[index].matchType = L7_OSPF_METRIC_TYPE_NSSA_EXT1;
    pRipMapCfgData->ripRtRedist[index].ripRtRedistFilter.mode = L7_ENABLE;
    pRipMapCfgData->ripRtRedist[index].ripRtRedistFilter.filter = 0xaabbab00 + index;

  }


 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;  
   sysapiPrintf("Built test config data\n");


}



/*********************************************************************
* @purpose  Build default RIP config data for an intf 
*
* @param    configId     configID associated with an interface
* @param    *pCfg        pointer to RIP interface configuration structure
*
* @returns  void
*
* @notes    none 
*
* @end
*********************************************************************/
void ripMapIntfBuildTestConfigData(ripMapCfgIntf_t *pCfg)
{
  L7_uint32 k;
  L7_uint32 intIfNum;


  /* Ensure at least one entry in each array has a unique value to validate
     accurate migration */

  if (nimIntIfFromConfigIDGet(&pCfg->configId, &intIfNum) != L7_SUCCESS)
      intIfNum = 0xFF;


  pCfg->adminMode     = L7_ENABLE;
  pCfg->authType      = L7_AUTH_TYPE_SIMPLE_PASSWORD;

  for (k = 0; k < L7_AUTH_MAX_KEY_RIP; k++)
    pCfg->authKey[k]  = 0xA0 + k;

  pCfg->authKeyLen    = 4;
  pCfg->authKeyId     = 6;
  pCfg->verSend       = L7_RIP_CONF_SEND_RIPV1_DEMAND;
  pCfg->verRecv       = L7_RIP_CONF_RECV_DO_NOT_RECEIVE;
  pCfg->intfCost      = 7 + intIfNum;
}




/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void ripMapConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(pRipMapCfgData->cfgHdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    ripMapCfgDataShow();


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "REDIST_RT_LAST - %d\n", REDIST_RT_LAST);
    sysapiPrintf( "L7_AUTH_MAX_KEY_RIP - %d\n", L7_AUTH_MAX_KEY_RIP);
    sysapiPrintf( "L7_RIP_MAP_INTF_MAX_COUNT - %d\n", L7_RIP_MAP_INTF_MAX_COUNT);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("pRipMapCfgData->checkSum : %u\n", pRipMapCfgData->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/


