/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rip_map_api.c
*
* @purpose   RIP API functions
*
* @component RIP Mapping Layer
*
* @comments  The only "set" commands which are queued to the RIP processing
*            thread are RIP admin mode and RIP interface admin mode commands.
*            These are queued because they are fairly heavy and because they 
*            require synchronization with the RIP vendor task.  All other set
*            and get commands are protected with read-write locks. (WPJ_STACK_TBD)
*           
*            Additionally, configuration is restored under the RIP processing
*            thread.
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


extern ripMapCfg_t  *pRipMapCfgData;
extern void         *ripMapSemaphore;


static L7_uint32 ripMapRipRouteRedistributeMatchTypeNew(L7_REDIST_RT_INDICES_t protocol, 
                                                        L7_uint32 matchMode, 
                                                        L7_uint32 matchType);

/*:ignore*/
/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the RIP administrative mode 
*
* @param    *mode   @b{(output)} admin mode L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*      
* @end
*********************************************************************/
L7_RC_t ripMapRipAdminModeGet(L7_uint32 *mode)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  rc = ripMapAdminModeGet(mode);
  ripMapSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Get the RIP administrative mode 
*
* @param    *mode   @b{(output)} admin mode L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments internal function, ripMapSemaphore must be locked
*      
* @end
*********************************************************************/
L7_RC_t ripMapAdminModeGet(L7_uint32 *mode)
{
  if (mode == L7_NULLPTR)
    return L7_FAILURE;

  if (pRipMapCfgData == L7_NULL)
  {
    *mode = 0;                /* neither enabled nor disabled */
    return L7_FAILURE;
  }

  *mode = pRipMapCfgData->ripAdminMode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP administrative mode
*
* @param    mode   @b{(input)} admin mode L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipAdminModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  ripMapSemaTake();

  /* assumes current admin mode already established, so nothing more to do here */
  if (mode == pRipMapCfgData->ripAdminMode)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripAdminMode = mode;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  ripMapSemaGive();

  rc = ripMapAdminModeQueue(mode);

  return rc;
}

/*********************************************************************
* @purpose  Get the number of route changes made by RIP to the 
*           IP Route Database
*
* @param    *val    @b{(output)} RIP route changes count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteChangesGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  ripMapSemaTake();
  if (ripMapIsInitialized() != L7_TRUE)
  {
    *val = 0;
    return L7_SUCCESS;
  }
  rc = ripMapExtenRouteChangesGet(val);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the number of responses sent to RIP queries from other systems
*
* @param    *val   @b{(output)} RIP query response count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapQueriesGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  ripMapSemaTake();
  if (ripMapIsInitialized() != L7_TRUE)
  {
    *val = 0;
    return L7_SUCCESS;
  }
  rc = ripMapExtenQueriesGet(val);
  ripMapSemaGive();

  return rc;
}


/*********************************************************************
* @purpose  Get the RIP split horizon mode
*
* @param    *val   @b{(output)} Split Horizon mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipSplitHorizonModeGet(L7_uint32 *val)
{
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  ripMapSemaTake();
  *val = pRipMapCfgData->ripSplitHorizon;
  ripMapSemaGive();

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the RIP split horizon mode
*
* @param    val   @b{(input)} Split Horizon mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipSplitHorizonModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((val != L7_RIP_SPLITHORIZON_NONE) && (val != L7_RIP_SPLITHORIZON_SIMPLE) &&
      (val != L7_RIP_SPLITHORIZON_POISONREVERSE))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (val == pRipMapCfgData->ripSplitHorizon)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripSplitHorizon = val;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapSplitHorizonModeApply(val);
  ripMapSemaGive();
  
  return rc;
}


/*********************************************************************
* @purpose  Get the RIP auto-summarization mode
*
* @param    *val   @b{(output)} Auto-Summarization mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipAutoSummarizationModeGet(L7_uint32 *val)
{
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  ripMapSemaTake();
  *val = pRipMapCfgData->ripAutoSummarization;
  ripMapSemaGive();
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the RIP auto-summarization mode
*
* @param    val   @b{(input)} Auto-Summarization mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipAutoSummarizationModeSet(L7_uint32 val)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((val != L7_ENABLE) && (val != L7_DISABLE))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (val == pRipMapCfgData->ripAutoSummarization)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripAutoSummarization = val;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapAutoSummarizationModeApply(val);
  ripMapSemaGive();

  return rc;
}


/*********************************************************************
* @purpose  Get the RIP host route accept mode
*
* @param    *val        @b{(output)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipHostRoutesAcceptModeGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  rc = ripMapHostRoutesAcceptModeGet(val); 
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the RIP host route accept mode
*
* @param    *val        @b{(output)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments internal function, ripMapSemaphore must be locked
*       
* @end
*********************************************************************/
L7_RC_t ripMapHostRoutesAcceptModeGet(L7_uint32 *val)
{
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  *val = pRipMapCfgData->ripHostRoutesAccept;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP host route accept mode
*
* @param    val         @b{(input)} Host Routes Accept mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipHostRoutesAcceptModeSet(L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((val != L7_ENABLE) && (val != L7_DISABLE))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (val == pRipMapCfgData->ripHostRoutesAccept)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripHostRoutesAccept = val;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapHostRoutesAcceptModeApply(val);
  ripMapSemaGive();

  return rc;
}


/*********************************************************************
* @purpose  Get the default metric value
*
* @param    *val         @b{(output)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric is not configured.
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipDefaultMetricGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  rc = ripMapDefaultMetricGet(val); 
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the default metric value
*
* @param    *val         @b{(output)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric is not configured.
*
* @comments internal function, ripMapSemaphore must be locked
*       
* @end
*********************************************************************/
L7_RC_t ripMapDefaultMetricGet(L7_uint32 *val)
{
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  /* SHould I return failure if metric was not configured. */
  if ((pRipMapCfgData->ripDefaultMetric < L7_RIP_CONF_METRIC_MIN) || 
      (pRipMapCfgData->ripDefaultMetric > L7_RIP_CONF_METRIC_MAX))
  {
    return L7_ERROR;
  }
  *val = pRipMapCfgData->ripDefaultMetric;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the default metric value
*
* @param    mode         @b{(input)} enable/disable default metric value
* @param    val          @b{(input)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  config entry point, must lock ripMapSemaphore to read/modify
* @comments  The purpose of the parameter mode is to specify whether metric is
*            being configured or deconfigured. If mode is disable, the metric is
*            set to invalid value and not to default value. If mode is disable the
*            val parameter is ignored. If mode is enable, this function checks if val
*            is within allowable parameters.
*            This function returns failure in the followign cases:
*                 1. If mode is L7_ENABLE and val is outside of range
*                 2. If the metric cannot be applied.
*                 
* @end
*********************************************************************/
L7_RC_t ripMapRipDefaultMetricSet(L7_uint32 mode, L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  if ((mode == L7_ENABLE) && 
      ((val < L7_RIP_CONF_METRIC_MIN) || (val > L7_RIP_CONF_METRIC_MAX)))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (mode == L7_ENABLE)
  {
    if (val == pRipMapCfgData->ripDefaultMetric)
    {
      ripMapSemaGive();
      return L7_SUCCESS;
    }
    pRipMapCfgData->ripDefaultMetric = val;
  }
  else if (mode == L7_DISABLE)
  {
    if (L7_RIP_METRIC_INVALID == pRipMapCfgData->ripDefaultMetric)
    {
      ripMapSemaGive();
      return L7_SUCCESS;
    }
    pRipMapCfgData->ripDefaultMetric = L7_RIP_METRIC_INVALID;
    val = L7_RIP_METRIC_INVALID;
  }
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapDefaultMetricApply(val);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Clears the default metric value. Sets it to its default value.
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipDefaultMetricClear()
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (FD_RIP_DEFAULT_DEFAULT_METRIC == pRipMapCfgData->ripDefaultMetric)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripDefaultMetric = FD_RIP_DEFAULT_DEFAULT_METRIC;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapDefaultMetricApply(FD_RIP_DEFAULT_DEFAULT_METRIC);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the RIP default route advertise mode
*
* @param    *val        @b{(output)} Default route advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipDefaultRouteAdvertiseModeGet(L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  rc = ripMapDefaultRouteAdvertiseModeGet(val);
  ripMapSemaGive();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the RIP default route advertise mode
*
* @param    *val        @b{(output)} Default route advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments internal function, ripMapSemaphore must be locked
*       
* @end
*********************************************************************/
L7_RC_t ripMapDefaultRouteAdvertiseModeGet(L7_uint32 *val)
{
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  *val = pRipMapCfgData->ripDefRtAdv.defRtAdv;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP default route advertise mode
*
* @param    val         @b{(input)} Default route advertise mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipDefaultRouteAdvertiseModeSet(L7_uint32 val)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((val != L7_ENABLE) && (val != L7_DISABLE))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (val == pRipMapCfgData->ripDefRtAdv.defRtAdv)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripDefRtAdv.defRtAdv = val;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapDefaultRouteAdvertiseModeApply(val);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Clear the RIP default route advertise mode. Sets it to its default value.
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipDefaultRouteAdvertiseModeClear()
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE == pRipMapCfgData->ripDefRtAdv.defRtAdv)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripDefRtAdv.defRtAdv = FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapDefaultRouteAdvertiseModeApply(FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the filter used for filtering redistributed routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route.
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *val        @b{(output)} ACL number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If filter is not configured.
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeFilterGet(L7_REDIST_RT_INDICES_t protocol, 
                                            L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  
  ripMapSemaTake();
  rc = ripMapRouteRedistributeFilterGet(protocol, val); 
  ripMapSemaGive();
  
  return rc;
}

/*********************************************************************
* @purpose  Get the filter used for filtering redistributed routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route.
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *val        @b{(output)} ACL number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If filter is not configured.
*
* @comments internal function, ripMapSemaphore must be locked
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeFilterGet(L7_REDIST_RT_INDICES_t protocol, 
                                         L7_uint32 *val)
{
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if (val == L7_NULLPTR)
    return L7_FAILURE;

  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;
  
  if (pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.mode == L7_DISABLE)
  {
    return L7_ERROR;
  }
  
  *val = pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.filter;
  
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Set the filter used for filtering redistributed routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    flag        @b{(input)} enable/disable route filter
* @param    val         @b{(input)} ACL number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  config entry point, must lock ripMapSemaphore to read/modify
* @comments  The purpose of the parameter mode is to specify whether filter is
*            being configured or deconfigured. If mode is disable, the filter is
*            set to invalid value and not to default value. If mode is disable the
*            val parameter is ignored. If mode is enable, this function checks if val
*            is within aloowable parameters.
*            This function returns failure in the followign cases:
*                 1. If mode is L7_ENABLE and val is outside of range
*                 2. If the filter cannot be applied.
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeFilterSet(L7_REDIST_RT_INDICES_t protocol, 
                                            L7_uint32 mode, 
                                            L7_uint32 val)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  /* TODO: Check for validity of the ACL using outcalls file*/
  /* Replace 1 with the range checking for ACL */
  /*if ((mode == L7_ENABLE) && (check_range_ACL()))
      return L7_FAILURE;
   */
  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (mode == L7_ENABLE)
  {
    if ((L7_ENABLE == pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.mode) && 
        (val == pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.filter))
    {
      ripMapSemaGive();
      return L7_SUCCESS;
    }
    pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.filter = val;
    pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.mode = L7_ENABLE;
  }
  else if (mode == L7_DISABLE)
  {
    if ((L7_DISABLE == pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.mode) && 
        (L7_RIP_ROUTE_REDIST_FILTER_INVALID == pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.filter))
    {
      ripMapSemaGive();
      return L7_SUCCESS;
    }
    pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.filter = L7_RIP_ROUTE_REDIST_FILTER_INVALID;
    pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.mode = L7_DISABLE;
    val = L7_RIP_ROUTE_REDIST_FILTER_INVALID;
  }
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapRouteRedistributeFilterApply(protocol, val);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Clear the filter used for filtering redistributed routes from
*           the specified source protocol. Sets it to its default value.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeFilterClear(L7_REDIST_RT_INDICES_t protocol)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if ((FD_RIP_DEFAULT_REDIST_ROUTE_FILTER == pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.filter) &&
      (FD_RIP_DEFAULT_REDIST_ROUTE_FILTER_MODE == pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.mode))
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.filter = FD_RIP_DEFAULT_REDIST_ROUTE_FILTER;
  pRipMapCfgData->ripRtRedist[protocol].ripRtRedistFilter.mode = FD_RIP_DEFAULT_REDIST_ROUTE_FILTER_MODE;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = ripMapRouteRedistributeFilterApply(protocol, FD_RIP_DEFAULT_REDIST_ROUTE_FILTER);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Check if the source protocol exists in route redistribution table
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeSourceGet(L7_REDIST_RT_INDICES_t protocol)
{
  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP))
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the next source protocol in route redistribution table
*
* @param    *protocol    @b{(input)} Source protocol of redist route
*                                    Type L7_REDIST_RT_INDICES_t.
*                        @b{(output)} Next valid Source protocol of redist route
*                                     Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeSourceGetNext(L7_REDIST_RT_INDICES_t *protocol)
{
  if (protocol == L7_NULLPTR)
    return L7_FAILURE;

  /* If it is the last source protocol or anything greater, return failure*/
  if (*protocol >= REDIST_RT_BGP)
    return L7_FAILURE;
  
  /* If it is anything less than the first source protocol, return the first one*/
  if (*protocol <= REDIST_RT_FIRST)
  {
    *protocol = REDIST_RT_LOCAL;
    return L7_SUCCESS;
  }

  /* If it is static source, return ospf, since we cannot redistribute from ourself */
  if (*protocol == REDIST_RT_STATIC)
  {
    *protocol = REDIST_RT_OSPF;
    return L7_SUCCESS;
  }

  /* Anything else, increment by 1.*/
  *protocol = *protocol + 1;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the metric and match type used for redistributing routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *metric     @b{(output)} Metric used for redistributing routes
* @param    *matchType  @b{(output)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric or matchType is not configured.
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeParmsGet(L7_REDIST_RT_INDICES_t protocol, 
                                           L7_uint32 *metric, 
                                           L7_OSPF_METRIC_TYPES_t *matchType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32     ospfRtType;
  
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;

  
  /* TODO: Should I check for the redist enabled flag for this protocol to return
     any value stored in the config. If redistFlag is disabled, then no vlaue will be
     returned. If enabled, the config vlaues will be returned.
  */
  ospfRtType = L7_OSPF_METRIC_TYPE_INTERNAL | L7_OSPF_METRIC_TYPE_EXT1 | 
               L7_OSPF_METRIC_TYPE_EXT2 | L7_OSPF_METRIC_TYPE_NSSA_EXT1 | 
               L7_OSPF_METRIC_TYPE_NSSA_EXT2;

  ripMapSemaTake();
  if ((pRipMapCfgData->ripRtRedist[protocol].metric >= L7_RIP_CONF_METRIC_MIN) && 
      (pRipMapCfgData->ripRtRedist[protocol].metric <= L7_RIP_CONF_METRIC_MAX))
  {
    if (metric == L7_NULLPTR)
    {
      ripMapSemaGive();
      return L7_FAILURE;
    }
    *metric = pRipMapCfgData->ripRtRedist[protocol].metric;
  }
  else
  {
    rc = L7_ERROR;
  }

  if(protocol == REDIST_RT_OSPF)
  {
    if (((pRipMapCfgData->ripRtRedist[protocol].matchType | ospfRtType) == ospfRtType) && 
        (pRipMapCfgData->ripRtRedist[protocol].matchType >= L7_OSPF_METRIC_TYPE_INTERNAL))
    {
      if (matchType == L7_NULLPTR)
      {
        ripMapSemaGive();
        return L7_FAILURE;
      }
      *matchType = pRipMapCfgData->ripRtRedist[protocol].matchType;
    }
    else
    {
      rc = L7_ERROR;
    }
  }
  
  ripMapSemaGive();
  return rc;
}


/*********************************************************************
* @purpose  Set the route redistribution mode metric and match type used
*           for redistributing routes from the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    parmsMode   @b{(input)} Specify if parms are enabled/disabled for this protocol
* @param    metric      @b{(input)} Metric used for redistributing routes
* @param    matchType   @b{(input)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  config entry point, must lock ripMapSemaphore to read/modify
* @comments  The purpose of the parameter parmsMode is to specify whether parms are
*            being configured or deconfigured. If parmsMode is disable, the parms are
*            set to invalid values and not to default values. If parmsMode is disable, only those 
*            specified in the matchType are disabled. If parmsMode is disable the
*            metric parameter is ignored. If parmsMode is enable, this function checks if
*            metric is within allowable parameters and enables only those specified 
*            in the matchType argument. Range checking of matchType is done no
*            matter what parmsMode is and argument matchType is applicable only if argument
*            protocol is OSPF, otherwise it is ignored.
*            This function doesnot activate  or deactivate route redistribution for
*            the specified protocol. There's another function that has to be called to
*            activate or deactivate it.
*            This function returns failure in the following cases:
*                 1. If parmsMode is L7_ENABLE and parms are outside of ranges
*                 2. If the parms cannot be applied.
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeParmsSet(L7_REDIST_RT_INDICES_t protocol, 
                                           L7_uint32 parmsMode, 
                                           L7_uint32 metric, 
                                           L7_uint32 matchType)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 ospfRtType;


  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  ospfRtType = L7_OSPF_METRIC_TYPE_INTERNAL | L7_OSPF_METRIC_TYPE_EXT1 | 
               L7_OSPF_METRIC_TYPE_EXT2 | L7_OSPF_METRIC_TYPE_NSSA_EXT1 | 
               L7_OSPF_METRIC_TYPE_NSSA_EXT2;
  
  /* Test the validity of the parameters */
  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;
  if ((parmsMode != L7_ENABLE) && (parmsMode != L7_DISABLE))
    return L7_FAILURE;
  if ((parmsMode == L7_ENABLE) && 
      ((metric < L7_RIP_CONF_METRIC_MIN) || (metric > L7_RIP_CONF_METRIC_MAX)))
    return L7_FAILURE;
  if ((protocol == REDIST_RT_OSPF) &&
      (((matchType | ospfRtType) != ospfRtType) || 
       (matchType < L7_OSPF_METRIC_TYPE_INTERNAL)))
    return L7_FAILURE;


  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (parmsMode == L7_DISABLE)
  {
    metric = L7_RIP_METRIC_INVALID;
  }
  if (protocol == REDIST_RT_OSPF)
  {
    matchType = ripMapRipRouteRedistributeMatchTypeNew(protocol, parmsMode, matchType);
  }

  if ((protocol != REDIST_RT_OSPF) || 
      ((protocol == REDIST_RT_OSPF) && 
       (matchType == pRipMapCfgData->ripRtRedist[protocol].matchType)))
  {
    if (metric == pRipMapCfgData->ripRtRedist[protocol].metric)
    {
      ripMapSemaGive();
      return L7_SUCCESS;
    }
    /* To minimize processing time, applying metric only will not go through
       the whole mapping layer route table.
    */   
    pRipMapCfgData->ripRtRedist[protocol].metric = metric;
    pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    rc = ripMapRouteRedistributeMetricApply(protocol, metric);
    ripMapSemaGive();
    return rc;
  }
  else
  {
    pRipMapCfgData->ripRtRedist[protocol].metric = metric;
    pRipMapCfgData->ripRtRedist[protocol].matchType = matchType;
  }

  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapRouteRedistributeParmsApply(protocol, metric, matchType);
  ripMapSemaGive();
  
  return rc;
}


/*********************************************************************
* @purpose  Clear the route redistribution mode metric and match type used used 
*           for redistributing routes from the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeParmsClear(L7_uint32 protocol)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* Test the validity of the parameters */
  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;

  /* TODO: What should I check for here to see if all default values are already present
     and return success.
  */
  ripMapSemaTake();
  if (pRipMapCfgData->ripRtRedist[protocol].metric == FD_RIP_DEFAULT_REDIST_ROUTE_METRIC)
  {
    if ((protocol != REDIST_RT_OSPF) || 
        ((protocol == REDIST_RT_OSPF) && 
         (pRipMapCfgData->ripRtRedist[protocol].matchType == FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE)))
    {
      ripMapSemaGive();
      return L7_SUCCESS;
    }
  }
  
  pRipMapCfgData->ripRtRedist[protocol].metric = FD_RIP_DEFAULT_REDIST_ROUTE_METRIC;
  pRipMapCfgData->ripRtRedist[protocol].matchType = FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapRouteRedistributeParmsApply(protocol, FD_RIP_DEFAULT_REDIST_ROUTE_METRIC, 
                                         FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE);
  ripMapSemaGive();
  
  return rc;
}


/*********************************************************************
* @purpose  Get the metric for redistributing routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *metric     @b{(output)} Metric used for redistributing routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric is not configured.
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeMetricGet(L7_uint32 protocol, L7_uint32 *metric)
{
  L7_RC_t rc = L7_FAILURE;
  
  ripMapSemaTake();
  rc = ripMapRouteRedistributeMetricGet(protocol, metric); 
  ripMapSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Get the metric for redistributing routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *metric     @b{(output)} Metric used for redistributing routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If metric is not configured.
*
* @comments internal function, ripMapSemaphore must be locked
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeMetricGet(L7_uint32 protocol, L7_uint32 *metric)
{
  L7_RC_t rc = L7_SUCCESS;

  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;
  
  /* TODO: Should I check for the redist enabled flag for this protocol to return
     any value stored in the config. If redistFlag is disabled, then no vlaue will be
     returned. If enabled, the config vlaues will be returned.
  */
  if ((pRipMapCfgData->ripRtRedist[protocol].metric >= L7_RIP_CONF_METRIC_MIN) && 
      (pRipMapCfgData->ripRtRedist[protocol].metric <= L7_RIP_CONF_METRIC_MAX))
  {
    if (metric == L7_NULLPTR)
    {
      return L7_FAILURE;
    }
    *metric = pRipMapCfgData->ripRtRedist[protocol].metric;
  }
  else
  {
    rc = L7_ERROR;
  }
  
  return rc;
}

/*********************************************************************
* @purpose  Set the route redistribution mode and metric used 
*           for redistributing routes from the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    metricMode  @b{(input)} Specify if metric is enabled/disabled for this protocol
* @param    *metric     @b{(input)} Metric used for redistributing routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  config entry point, must lock ripMapSemaphore to read/modify
* @comments  The purpose of the parameter metricMode is to specify whether metric is
*            being configured or deconfigured. If metricMode is disable, the metric is
*            set to invalid value and not to default value. If metricMode is disable the
*            metric parameter is ignored. If metricMode is enable, this function checks if
*            metric is within allowable parameters.
*            This function doesnot activate  or deactivate route redistribution for
*            the specified protocol. There's another function that has to be called to
*            activate or deactivate it.
*            This function returns failure in the followign cases:
*                 1. If mode is L7_ENABLE and metric is outside of range
*                 2. If the metric cannot be applied.
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeMetricSet(L7_REDIST_RT_INDICES_t protocol, 
                                            L7_uint32 metricMode, 
                                            L7_uint32 metric)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* Test the validity of the parameters */
  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;

  if((metricMode != L7_ENABLE) && (metricMode != L7_DISABLE))
    return L7_FAILURE;

  if ((metricMode == L7_ENABLE) && 
      ((metric < L7_RIP_CONF_METRIC_MIN) || (metric > L7_RIP_CONF_METRIC_MAX)))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  if (metricMode == L7_DISABLE)
    metric = L7_RIP_METRIC_INVALID;

  ripMapSemaTake();
  if (metric == pRipMapCfgData->ripRtRedist[protocol].metric)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }
  pRipMapCfgData->ripRtRedist[protocol].metric = metric;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  rc = ripMapRouteRedistributeMetricApply(protocol, metric);
  ripMapSemaGive();

  return rc;
}


/*********************************************************************
* @purpose  Clear the route redistribution metric used 
*           for redistributing routes from the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeMetricClear(L7_REDIST_RT_INDICES_t protocol)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* Test the validity of the parameters */
  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;

  ripMapSemaTake();
  if (pRipMapCfgData->ripRtRedist[protocol].metric == FD_RIP_DEFAULT_REDIST_ROUTE_METRIC)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }
  
  pRipMapCfgData->ripRtRedist[protocol].metric = FD_RIP_DEFAULT_REDIST_ROUTE_METRIC;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapRouteRedistributeMetricApply(protocol, FD_RIP_DEFAULT_REDIST_ROUTE_METRIC);
  ripMapSemaGive();
  
  return rc;
}


/*********************************************************************
* @purpose  Get the match type used for redistributing routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *matchType  @b{(output)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If matchType is not configured.
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeMatchTypeGet(L7_REDIST_RT_INDICES_t protocol, 
                                               L7_uint32 *matchType)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  rc = ripMapRouteRedistributeMatchTypeGet (protocol, matchType);  
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the match type used for redistributing routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *matchType  @b{(output)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR     If matchType is not configured.
*
* @comments internal function, ripMapSemaphore must be locked
*       
* @end
*********************************************************************/
L7_RC_t ripMapRouteRedistributeMatchTypeGet(L7_REDIST_RT_INDICES_t protocol, 
                                            L7_uint32 *matchType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32     ospfRtType;
  
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if(protocol != REDIST_RT_OSPF)
    return L7_FAILURE;

  /* TODO: Should I check for the redist enabled flag for this protocol to return
     any value stored in the config. If redistFlag is disabled, then no vlaue will be
     returned. If enabled, the config vlaues will be returned.
  */

  ospfRtType = L7_OSPF_METRIC_TYPE_INTERNAL | L7_OSPF_METRIC_TYPE_EXT1 | 
               L7_OSPF_METRIC_TYPE_EXT2 | L7_OSPF_METRIC_TYPE_NSSA_EXT1 | 
               L7_OSPF_METRIC_TYPE_NSSA_EXT2;

  if (((pRipMapCfgData->ripRtRedist[protocol].matchType | ospfRtType) == ospfRtType) && 
      (pRipMapCfgData->ripRtRedist[protocol].matchType >= L7_OSPF_METRIC_TYPE_INTERNAL))
  {
    if (matchType == L7_NULLPTR)
    {
      return L7_FAILURE;
    }
    *matchType = pRipMapCfgData->ripRtRedist[protocol].matchType;
  }
  else
  {
    rc = L7_ERROR;
  }
  
  return rc;
}

/*********************************************************************
* @purpose  Set the route redistribution match type used
*           for redistributing routes from the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    matchMode   @b{(input)} Specify if matchType is enabled/disabled for this protocol
* @param    matchType   @b{(input)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  config entry point, must lock ripMapSemaphore to read/modify
* @comments  The purpose of the parameter matchMode is to specify whether matchType is
*            being configured or deconfigured. If matchMode is disable, only those 
*            specified in the matchType are disabled. Then if matchType is same as 
*            stored config, the config is set to invalid value and not to default value. 
*            If matchMode is enable, this function checks if matchType is within allowable
*            parameters. It replaces the configured matchType with argument matchType.
*            This function doesnot activate  or deactivate route redistribution for
*            the specified protocol. There's another function that has to be called to
*            activate or deactivate it.
*            This function returns failure in the followign cases:
*                 1. If matchMode is L7_ENABLE and matchType is outside of range
*                 2. If the matchType cannot be applied.
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeMatchTypeSet(L7_REDIST_RT_INDICES_t protocol, 
                                               L7_uint32 matchMode, 
                                               L7_uint32 matchType)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     ospfRtType;

  
  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  ospfRtType = L7_OSPF_METRIC_TYPE_INTERNAL | L7_OSPF_METRIC_TYPE_EXT1 | 
               L7_OSPF_METRIC_TYPE_EXT2 | L7_OSPF_METRIC_TYPE_NSSA_EXT1 | 
               L7_OSPF_METRIC_TYPE_NSSA_EXT2;
  /* Test the validity of the parameters */
  if (protocol != REDIST_RT_OSPF)
    return L7_FAILURE;
  if((matchMode != L7_ENABLE) && (matchMode != L7_DISABLE))
    return L7_FAILURE;
  if (((matchType | ospfRtType) != ospfRtType) || 
        (matchType < L7_OSPF_METRIC_TYPE_INTERNAL))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  matchType = ripMapRipRouteRedistributeMatchTypeNew(protocol, matchMode, matchType);

  if (matchType == pRipMapCfgData->ripRtRedist[protocol].matchType)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }
  pRipMapCfgData->ripRtRedist[protocol].matchType = matchType;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  rc = ripMapRouteRedistributeMatchTypeApply(protocol, matchType);
  ripMapSemaGive();
  
  return rc;
}

/*********************************************************************
* @purpose  Get the new route redistribution match type value based on
*           existing match type for the source protocol and the specified mode.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    matchMode   @b{(input)} Specify if matchType is enabled/disabled for this protocol
* @param    matchType   @b{(input)} MatchType used for filtering redistributing 
*                                    OSPF routes
*
* @returns  L7_uint32   New matchType value
*
* @comments  Caller must lock ripMapSemaphore prior to calling this function.
*            The caller is also responsible for any range checking and value
*            validation of the input parameters so that it does not get 
*            duplicated here.
*
* @comments  This is an internal helper function used to calculate the new 
*            redistribution matchType.  It does not actually update the RIP
*            matchType configuraton parameter, but calculates the new matchType
*            value for the protocol based on the mode.  Since only OSPF 
*            routes redistributed through RIP use the matchType, an "invalid" 
*            matchType value is returned for any other protocol.
*       
* @end
*********************************************************************/
static L7_uint32 ripMapRipRouteRedistributeMatchTypeNew(L7_REDIST_RT_INDICES_t protocol, 
                                                        L7_uint32 matchMode, 
                                                        L7_uint32 matchType)
{
  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_RIP_ROUTE_REDIST_MATCHTYPE_INVALID;

  /* return invalid matchType value if protocol is not OSPF */
  if (protocol != REDIST_RT_OSPF)
    return L7_RIP_ROUTE_REDIST_MATCHTYPE_INVALID;

  if (matchMode == L7_DISABLE)
  {
    matchType = (pRipMapCfgData->ripRtRedist[protocol].matchType ^ matchType) &
                pRipMapCfgData->ripRtRedist[protocol].matchType;
  }
  else  /* (matchMode == L7_ENABLE) */
  {
    matchType |= pRipMapCfgData->ripRtRedist[protocol].matchType;
  }

  /* If matchType becomes 0, use invalid value */
  if (matchType == 0)
    matchType = L7_RIP_ROUTE_REDIST_MATCHTYPE_INVALID;
  
  return matchType;
}


/*********************************************************************
* @purpose  Clear the route redistribution match type used
*           for redistributing routes from the specified source protocol.
*           Sets it to the default value.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeMatchTypeClear(L7_REDIST_RT_INDICES_t protocol)
{
  L7_RC_t rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* Test the validity of the parameters */
  if (protocol != REDIST_RT_OSPF) 
    return L7_FAILURE;

  ripMapSemaTake();
  if (pRipMapCfgData->ripRtRedist[protocol].matchType == FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }
  
  pRipMapCfgData->ripRtRedist[protocol].matchType = FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapRouteRedistributeMatchTypeApply(protocol, FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE);
  ripMapSemaGive();
  
  return rc;
}


/*********************************************************************
* @purpose  Get the redistribution mode for redistributing routes from
*           the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    *mode       @b{(output)} Redistribution mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeModeGet(L7_REDIST_RT_INDICES_t protocol, 
                                          L7_uint32 *mode)
{
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;
  
  if (mode == L7_NULLPTR)
    return L7_FAILURE;

  ripMapSemaTake();
  *mode = pRipMapCfgData->ripRtRedist[protocol].redistribute;
  ripMapSemaGive();
  
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the route redistribution mode for the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
* @param    mode        @b{(input)} Specify if mode is enabled/disabled for this protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  config entry point, must lock ripMapSemaphore to read/modify
* @comments  This function does not reset other redistribution parms to their default
*            values or to invalid values. It only disables/enables route redistribution
*            for routes from specified protocol.
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeModeSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 mode)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* Test the validity of the parameters */
  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;
  if((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  /* If the existing configuration is same as the one being set, then nothing
     to do here. Just return success */
  ripMapSemaTake();
  if (mode == pRipMapCfgData->ripRtRedist[protocol].redistribute)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  pRipMapCfgData->ripRtRedist[protocol].redistribute = mode;  
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  /* If mode is enable, and is all redist is enabled, register with rto to get best routes. */

  rc = ripMapRouteRedistributeModeApply(protocol, mode);
  ripMapSemaGive();

  /* If mode is disable, and is all redist is disbled, deregister with rto to get best routes. */

  return rc;
}


/*********************************************************************
* @purpose  Clear the route redistribution mode for the specified source protocol.
*
* @param    protocol    @b{(input)} Source protocol of redist route
*                                   Type L7_REDIST_RT_INDICES_t.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapRipRouteRedistributeModeClear(L7_REDIST_RT_INDICES_t protocol)
{
  L7_RC_t       rc = L7_FAILURE;

  /* this fails if RIP component not started by configurator */
  if (pRipMapCfgData == L7_NULL)
    return L7_FAILURE;

  /* Test the validity of the parameters */
  if ((protocol <= REDIST_RT_FIRST) || (protocol >= REDIST_RT_LAST) || (protocol == REDIST_RT_RIP)) 
    return L7_FAILURE;

  ripMapSemaTake();
  if (pRipMapCfgData->ripRtRedist[protocol].redistribute == FD_RIP_DEFAULT_REDIST_ROUTE_MODE)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }
  
  pRipMapCfgData->ripRtRedist[protocol].redistribute = FD_RIP_DEFAULT_REDIST_ROUTE_MODE;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  /* If mode is enable, and is all redist is enabled, register with rto to get best routes. */
  
  rc = ripMapRouteRedistributeModeApply(protocol, FD_RIP_DEFAULT_REDIST_ROUTE_MODE);
  ripMapSemaGive();

  /* If mode is disable, and is all redist is disbled, deregister with rto to get best routes. */
  
  return rc;
}

/*:ignore*/
/*---------------------------------------------------------------------
 *              API FUNCTIONS  -  INTERFACE ENTRY GET/NEXT
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the routing internal interface number for the specified
*           IP address
*
* @param    ipAddr      @b{(input)}  Routing interface IP address
* @param    *intIfNum   @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfEntryGet(L7_uint32 ipAddr, L7_uint32 *intIfNum)
{
  L7_uint32 ifState;

  if (ipMapIpAddressToIntf((L7_IP_ADDR_t)ipAddr, intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if ((ipMapRtrIntfOperModeGet(*intIfNum, &ifState) != L7_SUCCESS) ||
      (ifState != L7_ENABLE))
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next sequential routing internal interface number
*           after the interface corresponding to the specified IP address
*
* @param    ipAddr      @b{(input)}  Routing interface IP address
* @param    *intIfNum   @b{(output)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
* @comments The first routing interface is returned if the specified IP 
*           address does not match any existing interface.
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfEntryNext(L7_uint32 ipAddr, L7_uint32 *intIfNum)
{
#define RIP_MAP_SAVEIP_INIT   (-1)
  L7_uint32     saveIp, saveIntIfNum;
  L7_uint32     tempIntIfNum;
  L7_uint32     i, max;
  L7_IP_ADDR_t  rtrIp;
  L7_IP_MASK_t  rtrMask;

  saveIp = RIP_MAP_SAVEIP_INIT;
  saveIntIfNum = 0;

  ripMapSemaTake();
  max = ipmRouterIfMaxCountGet();
  for (i = RIP_MAP_RTR_INTF_FIRST; i <= max ; i++)
  {
    /* make sure this is a valid RIP interface */
    if (ripMapRtrIntfIsOperable(i, &tempIntIfNum) != L7_TRUE)
      continue;

    if ((ipMapRtrIntfIpAddressGet(tempIntIfNum, &rtrIp, &rtrMask) == L7_SUCCESS)
        && (rtrIp != 0))
    {
      if ((rtrIp > ipAddr) && (rtrIp < saveIp))
      {
        saveIp = rtrIp;
        saveIntIfNum = tempIntIfNum;
      }
    }
  }
  if (saveIp != RIP_MAP_SAVEIP_INIT)
  {
    *intIfNum = saveIntIfNum;
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  ripMapSemaGive();
  return L7_FAILURE;

#undef RIP_MAP_SAVEIP_INIT
}


/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE STATS
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the number of RIP response packets received by the RIP 
*           process which were subsequently discarded for any reason
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *count      @b{(output)} Receive Bad Packet count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfRcvBadPacketsGet(L7_uint32 intIfNum, L7_uint32 *count)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }
  rc = ripMapExtenIntfRcvBadPacketsGet(intIfNum, L7_NULL, count); 
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the number of routes contained in valid RIP packets that 
*           were ignored for any reason
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *count      @b{(output)} Receive Bad Routes count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfRcvBadRoutesGet(L7_uint32 intIfNum, L7_uint32 *count)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }
  rc = ripMapExtenIntfRcvBadRoutesGet(intIfNum, L7_NULL, count); 
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the number of triggered RIP updates actually sent 
*           on this interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *count      @b{(output)} Sent Triggered Updates count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfSentUpdatesGet(L7_uint32 intIfNum, L7_uint32 *count)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }
  rc = ripMapExtenIntfSentUpdatesGet(intIfNum, L7_NULL, count); 
  ripMapSemaGive();

  return rc;
}

/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE CONFIG
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the administrative mode of a RIP routing interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *mode       @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAdminModeGet(L7_uint32 intIfNum, L7_uint32 *mode)
{
  ripMapCfgIntf_t   *pCfg;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  *mode = pCfg->adminMode;
  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the administrative mode of a RIP routing interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode)
{
  ripMapCfgIntf_t *pCfg;
  L7_RC_t          rc = L7_FAILURE;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_ERROR;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  if (pCfg->adminMode == mode)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }
  
  pCfg->adminMode = mode;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  /* only do the actual mode change processing if the RIP application is 
   * currently initialized (allows pre-config)
   */
  rc = L7_SUCCESS;                              /* init for pre-config case */
  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* apply the admin mode setting */
    rc = ripMapIntfAdminModeQueue(intIfNum,  mode);
  }

  ripMapSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Get the routing domain value for this RIP interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *domain     @b{(output)} Routing domain
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments This MIB object is listed as obsolete.
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfDomainGet(L7_uint32 intIfNum, L7_uint32 *domain)
{
  ripMapSemaTake();
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }
  ripMapSemaGive();

  *domain = 0;  /* per RFC 1724 */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the routing domain value for this RIP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    domain      @b{(input)} Routing domain
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments This MIB object is listed as obsolete.
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfDomainSet(L7_uint32 intIfNum, L7_uint32 domain)
{
  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, L7_NULL) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  /* do not actually set anything, since this MIB object is obsolete */
  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the RIP authentication type for the specified interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *authType   @b{(output)} Interface authentication type
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthTypeGet(L7_uint32 intIfNum, L7_uint32 *authType)
{
  ripMapCfgIntf_t   *pCfg;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  *authType = pCfg->authType;
  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP authentication type for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    authType    @b{(input)} Authentication type
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
* @comments For security reasons, the authentication key is always
*           cleared whenever the auth type is changed.  If setting a 
*           new key value, be sure to do it AFTER setting the auth type. 
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthTypeSet(L7_uint32 intIfNum, L7_uint32 authType)
{
  ripMapCfgIntf_t *pCfg;
  L7_RC_t          rc = L7_FAILURE;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  if (pCfg->authType == authType)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  /* always clear the key when the auth type changes
   *
   * NOTE: This is to reduce the possibility of transmitting an 
   *       MD5 secret key in the clear when the auth type changes
   *       from MD5 to Simple.
   */
  memset(pCfg->authKey, 0, (size_t)L7_AUTH_MAX_KEY_RIP);
  pCfg->authKeyLen = 0;

  pCfg->authType = authType;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapIntfAuthTypeApply(intIfNum, authType); 
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the RIP authentication key for the specified interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *key        @b{(output)} Authentication key string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments The key value is output as an ASCIIZ string of whose length
*           is that of the key without any padding.
*
* @comments RFC 1724 specifies that authentication must not be bypassed by 
*           reading the MIB, but this function assumes that will be taken
*           care of at a higher layer.  DO NOT USE THIS API WITHOUT THE
*           APPROPRIATE SAFEGUARDS TO PREVENT EXPOSURE OF THE AUTHENTICATION
*           KEY!
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeyGet(L7_uint32 intIfNum, L7_uchar8 *key)
{
  ripMapCfgIntf_t *pCfg;
  L7_uint32        keyLen;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  /* output the key contents as a string */
  keyLen = pCfg->authKeyLen;
  memcpy(key, pCfg->authKey, keyLen);
  /* terminate the key value with a '\0' to make it an ASCIIZ string */
  *(key+keyLen) = L7_EOS;

  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP authentication key for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    key         @b{(input)} Pointer to authentication key string
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments The authentication key is passed in here as an ASCII string 
*           for the convenience of the user interface functions.  The MIB
*           key definition, however, is an OCTET STRING, which means it
*           is treated as a byte array, not an ASCII string.  The
*           conversion is done here for use in lower layer code.
*
* @comments The key type is based on the current value of the authType 
*           code.  This implies that the authType field must be set
*           before the authKey.
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeySet(L7_uint32 intIfNum, L7_uchar8 *key)
{
  ripMapCfgIntf_t *pCfg;
  L7_uint32        keyLen, authType, authLenMax;
  L7_uchar8        newKey[L7_AUTH_MAX_KEY_RIP];
  L7_RC_t          rc = L7_FAILURE;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  /* Convert incoming key string to a left-justified array, padded with hex zeroes */
  if(key != L7_NULLPTR)
  {
    keyLen = strlen(key);
  }
  else
  {
    keyLen = 0;
  }

  /* use current auth type from the configuration */
  authType = pCfg->authType;

  /* maximum allowed key length depends on current authType value */
  switch (authType)
  {
    case L7_AUTH_TYPE_NONE:
      authLenMax = 0;
      break;
  
    case L7_AUTH_TYPE_SIMPLE_PASSWORD:
    case L7_AUTH_TYPE_MD5:
      authLenMax = L7_AUTH_MAX_KEY_RIP;
      break;
  
    default:
      ripMapSemaGive();
      return L7_FAILURE;
      break;
  }

  /* The only time keyLen is greater than the max is if the caller 
   * specified a key value that is longer than the allowed maximum
   * for the current auth type -- reject it
   */
  if (keyLen > authLenMax)
  {
    ripMapSemaGive();
    return L7_FAILURE;
  }

  /* build a left-adjusted, padded new key to compare against current key */
  memset(&newKey, 0, (size_t)L7_AUTH_MAX_KEY_RIP);
  if (keyLen > 0)
  {
    memcpy(&newKey, key, (size_t)keyLen);
  }

  /* if incoming key is same as configured key, just return successfully */
  if (memcmp(pCfg->authKey, &newKey, (size_t)L7_AUTH_MAX_KEY_RIP) == 0)
  {
    ripMapSemaGive();
    return L7_SUCCESS;
  }

  /* update config struct with latest key and length */
  memcpy(pCfg->authKey, newKey, (size_t)L7_AUTH_MAX_KEY_RIP);
  pCfg->authKeyLen = keyLen;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  /* from here on, only work with padded full-sized key (max length implied) */
  rc = ripMapIntfAuthKeyApply(intIfNum, pCfg->authKey); 

  ripMapSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Get the RIP authentication key ID for the specified interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *keyId      @b{(output)} Interface authentication key ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeyIdGet(L7_uint32 intIfNum, L7_uint32 *keyId)
{
  ripMapCfgIntf_t   *pCfg;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  *keyId = pCfg->authKeyId;
  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP authentication key ID for the specified interface 
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    keyId       @b{(input)} Authentication key ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfAuthKeyIdSet(L7_uint32 intIfNum, L7_uint32 keyId)
{
  ripMapCfgIntf_t *pCfg;
  L7_RC_t          rc = L7_FAILURE;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  /* check for proper key ID range */
  if ((keyId < L7_RIP_CONF_AUTH_KEY_ID_MIN) || 
      (keyId > L7_RIP_CONF_AUTH_KEY_ID_MAX))
  {
    ripMapSemaGive();
    return L7_FAILURE;
  }

  pCfg->authKeyId = keyId;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapIntfAuthKeyIdApply(intIfNum, keyId); 
  ripMapSemaGive();
                                                    
  return rc;
}

/*********************************************************************
* @purpose  Get the RIP version used for sending updates on the specified 
*           interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *version    @b{(output)} RIP version used for sends
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfVerSendGet(L7_uint32 intIfNum, L7_uint32 *version)
{
  ripMapCfgIntf_t *pCfg;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  *version = pCfg->verSend;
  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP version used for sending updates on the specified 
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    version     @b{(input)} RIP version used for sends
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfVerSendSet(L7_uint32 intIfNum, L7_uint32 version)
{
  ripMapCfgIntf_t *pCfg;
  L7_RC_t          rc = L7_FAILURE;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  pCfg->verSend = version;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapIntfVerSendApply(intIfNum, version); 

  ripMapSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Get the RIP version used for receiving updates from the specified 
*           interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *version    @b{(output)} RIP version allowed for receives
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfVerRecvGet(L7_uint32 intIfNum, L7_uint32 *version)
{
  ripMapCfgIntf_t *pCfg;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  *version = pCfg->verRecv;
  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP version used for receiving updates from the specified 
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    version     @b{(input)} RIP version allowed for receives
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read/modify
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfVerRecvSet(L7_uint32 intIfNum, L7_uint32 version)
{
  ripMapCfgIntf_t *pCfg;
  L7_RC_t          rc = L7_FAILURE;

  ripMapSemaTake();
  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }

  pCfg->verRecv = version;
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  rc = ripMapIntfVerRecvApply(intIfNum, version); 

  ripMapSemaGive();
  return rc;
}

/*********************************************************************
* @purpose  Get the metric used for default routes in RIP updates 
*           originated on the specified interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *metric     @b{(output)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfDefaultMetricGet(L7_uint32 intIfNum, L7_uint32 *metric)
{
    return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Set the metric used for default routes in RIP updates 
*           originated on the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(input)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfDefaultMetricSet(L7_uint32 intIfNum, L7_uint32 metric)
{
  return L7_NOT_SUPPORTED;
}

/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  GLOBAL INFO (NON-MIB)
 *---------------------------------------------------------------------
 */
/*:end ignore*/


/*:ignore*/
/*---------------------------------------------------------------------
 *                 API FUNCTIONS  -  INTERFACE INFO (NON-MIB)
 *---------------------------------------------------------------------
 */
/*:end ignore*/

/*********************************************************************
* @purpose  Get the IP address of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *ipAddr     @b{(output)} Interface IP address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments      
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfIPAddrGet(L7_uint32 intIfNum, L7_uint32 *ipAddr)
{
  L7_IP_ADDR_t  myIpAddr;
  L7_IP_MASK_t  myMask;
  L7_RC_t       rc;

  if ((rc = ipMapRtrIntfIpAddressGet(intIfNum, &myIpAddr, &myMask)) != L7_SUCCESS)
    return rc;

  *ipAddr = (L7_uint32)myIpAddr;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the subnet mask of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *netMask    @b{(output)} Interface subnet mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfSubnetMaskGet(L7_uint32 intIfNum, L7_uint32 *netMask)
{
  L7_IP_ADDR_t  myIpAddr;
  L7_IP_MASK_t  myMask;
  L7_RC_t       rc;

  ripMapSemaTake();
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }
  ripMapSemaGive();

  if ((rc = ipMapRtrIntfIpAddressGet(intIfNum, &myIpAddr, &myMask)) != L7_SUCCESS)
    return rc;

  *netMask = (L7_uint32)myMask;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the interface type for the specified RIP interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *type       @b{(output)} Interface type (see nimIntfTypes for list) 
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments     
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfTypeGet(L7_uint32 intIfNum, L7_uint32 *type)
{
  return(nimGetIntfType(intIfNum, type));
}

/*********************************************************************
* @purpose  Get the current state of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *ifState    @b{(output)} RIP interface state (L7_UP or L7_DOWN)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfStateGet(L7_uint32 intIfNum, L7_uint32 *ifState)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  rc = ripMapExtenIntfStateGet(intIfNum, L7_NULL, ifState);
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the state flags of the specified RIP interface 
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *flags      @b{(output)} RIP interface state flags
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfStateFlagsGet(L7_uint32 intIfNum, L7_uint32 *flags)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }
  rc = ripMapExtenIntfStateFlagsGet(intIfNum, L7_NULL, flags); 
  ripMapSemaGive();

  return rc;
}

/*********************************************************************
* @purpose  Get the number of times the specified RIP interface has changed
*           state from up to down
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    *val        @b{(output)} Interface up-down transition count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, or is not enabled for routing
* @returns  L7_FAILURE
*
* @comments config entry point, must lock ripMapSemaphore to read
*       
* @end
*********************************************************************/
L7_RC_t ripMapIntfLinkEventsCtrGet(L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;

  ripMapSemaTake();
  if (pRipIntfInfo[intIfNum].state != RIP_MAP_INTF_STATE_UP)
  {
    ripMapSemaGive();
    return L7_ERROR;
  }
  rc = ripMapExtenIntfLinkEventsCtrGet(intIfNum, L7_NULL, val); 

  ripMapSemaGive();
  return rc;
}

