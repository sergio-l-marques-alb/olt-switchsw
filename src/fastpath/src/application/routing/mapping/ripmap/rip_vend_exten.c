/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  rip_vend_exten.c
*
* @purpose   RIP vendor-specific functions
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



#include "rip_vend_ctrl.h"
#include "rip_util.h"
#include "rip_map_table.h"

#include <net/if.h>

extern ripMapCfg_t       *pRipMapCfgData;


/* private function prototypes */
static L7_RC_t ripMapExtenInterfaceFind(L7_uint32 intIfNum,
                                        L7_uint32 ipAddress,
                                        struct interface *p,
                                        struct interface **ifp);


/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  GLOBAL CONFIG
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Sets the RIP Admin mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Controls the vendor RIP task through the use of a semaphore,
*           taking and holding it while disabled and releasing it while
*           enabled.
*
* @notes    A flag is used to allow repeated calls for the same admin
*           state without hanging the process due to an already-taken
*           semaphore.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipAdminModeSet(L7_uint32 mode)
{
/*
  if (mode == L7_ENABLE)
  {
    if (ripMapCtrl_g.taskCtrl.semAvail == L7_FALSE)
    {
      if (osapiSemaGive(ripMapCtrl_g.taskCtrl.semId) == L7_SUCCESS)
        ripMapCtrl_g.taskCtrl.semAvail = L7_TRUE;
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
               "ripMapExtenRipAdminModeSet: semaphore give error\n");
        return L7_FAILURE;
      }
    }
  }

  else
  {
    if (ripMapCtrl_g.taskCtrl.semAvail == L7_TRUE)
    {
      if (osapiSemaTake(ripMapCtrl_g.taskCtrl.semId, L7_WAIT_FOREVER)
          == L7_SUCCESS)
        ripMapCtrl_g.taskCtrl.semAvail = L7_FALSE;
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "ripMapExtenRipAdminModeSet: semaphore take error\n");
      }
    }
  }
*/
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
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipSplitHorizonModeSet(L7_uint32 val)
{
  /* Set the split horizon mode in vendor code */
  ripPipeMsg_t  msg;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Send a message to the pipe. */
    msg.msgId = L7_RIP_SPLITHORIZON_MODE_CHANGE;
    switch (val)
    {
      case L7_RIP_SPLITHORIZON_NONE:
        msg.arg1 = SPHZ_NONE;
        break;
      case L7_RIP_SPLITHORIZON_SIMPLE:
        msg.arg1 = SPHZ_SIMPLE;
        break;
      case L7_RIP_SPLITHORIZON_POISONREVERSE:
        msg.arg1 = SPHZ_POISONREVERSE;
        break;
      default:
        msg.arg1 = SPHZ_POISONREVERSE;
        break;
    }
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
  }
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
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipAutoSummarizationModeSet(L7_uint32 val)
{
  /* Set the auto summarization mode in vendor code */
  ripPipeMsg_t  msg;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Send a message to the pipe. */
    msg.msgId = L7_RIP_AUTOSUMMARY_MODE_CHANGE;
    switch (val)
    {
      case L7_DISABLE:
        msg.arg1 = AUTOSUMMARY_OFF;
        break;
      case L7_ENABLE:
        msg.arg1 = AUTOSUMMARY_ON;
        break;
      default:
        msg.arg1 = AUTOSUMMARY_ON;
        break;
    }
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
  }
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
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipHostRoutesAcceptModeSet(L7_uint32 val)
{
  ripPipeMsg_t  msg;
  L7_BOOL flashUpdateNeeded = L7_FALSE;

  /* Set the host routes accept mode in vendor code */
  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Send a message to the pipe. */
    msg.msgId = L7_RIP_HOSTROUTESACCEPT_MODE_CHANGE;
    switch (val)
    {
      case L7_DISABLE:
        msg.arg1 = HOSTROUTESACCEPT_OFF;
        break;
      case L7_ENABLE:
        msg.arg1 = HOSTROUTESACCEPT_ON;
        break;
      default:
        msg.arg1 = HOSTROUTESACCEPT_ON;
        break;
    }
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);

    /* Acquire the ripmap internal redist route table resource and get the routes */
    if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
      LOG_ERROR(1);

    (void) ripMapExtenRipRouteRedistributeRecalculate(&flashUpdateNeeded);

    /* Release the ripmap internal redist route table resource. */
    if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not release the RIP route table semaphore.\n");
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the default metric in the protocol code
*
* @param    val         @b{(input)} Default Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipDefaultMetricSet(L7_uint32 val)
{
  ripPipeMsg_t  msg;

  /* Send the default route metric change message to vendor code */
  if (ripMapIsInitialized() == L7_TRUE)
  {
    msg.msgId = L7_RIP_DEFAULT_METRIC_CHANGE;
    msg.arg1 = val;
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the RIP default route advertisement mode and metric
*
* @param    mode         @b{(input)} Default Route advertisement mode
* @param    metric       @b{(input)} Default Route metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipDefaultRouteAdvertiseSet(L7_uint32 mode, L7_uint32 metric)
{
  /* Send a message to RIP protocol about this. */
  ripPipeMsg_t  msg;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Send a message to the pipe. */
    msg.msgId = L7_RIP_DEFAULT_ROUTE_ADV_CHANGE;
    switch (mode)
    {
      case L7_DISABLE:
        msg.arg1 = DEF_ROUTE_ADV_OFF;
        break;
      case L7_ENABLE:
        msg.arg1 = DEF_ROUTE_ADV_ON;
        msg.arg2 = metric;
        break;
      default:
        msg.arg1 = DEF_ROUTE_ADV_OFF;
        break;
    }
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the Route Redistribute Filter
*
* @param    protocol         @b{(input)} The source protocol
* @param    val              @b{(input)} ACL used for filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeFilterSet(L7_REDIST_RT_INDICES_t protocol, L7_uint32 val)
{
  L7_BOOL flashUpdateNeeded = L7_FALSE;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Acquire the ripmap internal redist route table resource and get the routes */
    if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
      LOG_ERROR(1);

    (void) ripMapExtenRipRouteRedistributeRecalculate(&flashUpdateNeeded);

    /* Release the ripmap internal redist route table resource. */
    if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not release the RIP route table semaphore.\n");
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Applies the changed ACL filter to evaluate the routes to be
*           redistributed.
*
* @param    val              @b{(input)} ACL used for filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteRedistFilterChange(L7_uint32 val)
{
  L7_BOOL flashUpdateNeeded = L7_FALSE;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Acquire the ripmap internal redist route table resource and get the routes */
    if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
      LOG_ERROR(1);

    (void) ripMapExtenRipRouteRedistributeRecalculate(&flashUpdateNeeded);

    /* Release the ripmap internal redist route table resource. */
    if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not release the RIP route table semaphore.\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the route redistribute parms
*
* @param    protocol         @b{(input)} The source protocol
* @param    metric           @b{(input)} Metric used to redistribute
* @param    matchType        @b{(input)} MatchType used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeParmsSet(L7_REDIST_RT_INDICES_t protocol,
                                                L7_uint32 metric,
                                                L7_OSPF_METRIC_TYPES_t matchType)
{
  ripPipeMsg_t     msg;
  L7_BOOL flashUpdateNeeded = L7_FALSE;
  rip_redist_proto rip_proto;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Acquire the ripmap internal redist route table resource and get the routes */
    if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
      LOG_ERROR(1);

    (void) ripMapExtenRipRouteRedistributeRecalculate(&flashUpdateNeeded);

    /* Release the ripmap internal redist route table resource. */
    if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not release the RIP route table semaphore.\n");

    rip_proto = RIP_RT_REDIST_FIRST;
    if (ripMapVendRouteRedistProtocolConvert(protocol, &rip_proto) == L7_SUCCESS)
    {
      msg.msgId = L7_RIP_ROUTE_REDISTRIBUTE_METRIC_CHANGE;
      msg.arg1 = metric;
      msg.rip_proto = rip_proto;
      osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
    }
    else
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not convert ripMap redistribution protocol to rip redistribution protocol.\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the route redistribute metric
*
* @param    protocol         @b{(input)} The source protocol
* @param    metric           @b{(input)} Metric used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeMetricSet(L7_REDIST_RT_INDICES_t protocol,
                                                 L7_uint32 metric)
{
  ripPipeMsg_t  msg;
  rip_redist_proto rip_proto;

  /* Send the default route metric change message to vendor code */
  if (ripMapIsInitialized() == L7_TRUE)
  {
    rip_proto = RIP_RT_REDIST_FIRST;
    if (ripMapVendRouteRedistProtocolConvert(protocol, &rip_proto) == L7_SUCCESS)
    {
      msg.msgId = L7_RIP_ROUTE_REDISTRIBUTE_METRIC_CHANGE;
      msg.arg1 = metric;
      msg.rip_proto = rip_proto;
      osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
    }
    else
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not convert ripMap redistribution protocol to rip redistribution protocol.\n");
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the route redistribute matchType
*
* @param    protocol         @b{(input)} The source protocol
* @param    matchType        @b{(input)} MatchType used to redistribute
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeMatchTypeSet(L7_REDIST_RT_INDICES_t protocol,
                                                    L7_OSPF_METRIC_TYPES_t matchType)
{
  L7_BOOL flashUpdateNeeded = L7_FALSE;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Acquire the ripmap internal redist route table resource and get the routes */
    if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
      LOG_ERROR(1);

    (void) ripMapExtenRipRouteRedistributeRecalculate(&flashUpdateNeeded);

    /* Release the ripmap internal redist route table resource. */
    if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not release the RIP route table semaphore.\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the route redistribute mode
*
* @param    protocol         @b{(input)} The source protocol
* @param    mode             @b{(input)} Enable / Disable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeModeSet(L7_REDIST_RT_INDICES_t protocol,
                                               L7_uint32 mode)
{
  L7_BOOL flashUpdateNeeded = L7_FALSE;

  if (ripMapIsInitialized() == L7_TRUE)
  {
    /* Acquire the ripmap internal redist route table resource and get the routes */
    if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
      LOG_ERROR(1);

    (void) ripMapExtenRipRouteRedistributeRecalculate(&flashUpdateNeeded);

    /* Release the ripmap internal redist route table resource. */
    if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not release the RIP route table semaphore.\n");
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Recalculate the redistribution route table
*
* @param    *flashUpdateNeeded     @b{(input)} Flash update needed flag.
*
* @returns  L7_SUCCESS
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeRecalculate(L7_BOOL *flashUpdateNeeded)
{
  ripMapRouteData_t routeData, *rtData;
  L7_RC_t rc;
  L7_uint32 metric;

  *flashUpdateNeeded = L7_FALSE;

  rtData = &routeData;
  /* Go thru the whole ripMap route table.
  */
  memset(&routeData, 0, sizeof(ripMapRouteData_t));
  while (ripMapRouteTableRouteGetNext(rtData, &rtData) == L7_SUCCESS)
  {
    rc = ripMapExtenRipRouteRedistributeRouteCheck(rtData->network, rtData->subnetMask, rtData->gateway, rtData->protocol, rtData->protoSpecific);
    if ((rc == L7_SUCCESS) && (rtData->routeAdded == L7_FALSE))
    {
      if (ripMapRouteRedistributeMetricGet(rtData->protocol, &metric) != L7_SUCCESS)
      {
        if (ripMapDefaultMetricGet(&metric) != L7_SUCCESS)
          metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
      }
      if (ripMapExtenRipRouteRedistRouteAdd(rtData->network, rtData->subnetMask,
                                           rtData->gateway, rtData->protocol,
                                           metric) == L7_SUCCESS)
      {
        *flashUpdateNeeded = L7_TRUE;
        rtData->routeAdded = L7_TRUE;
      }
    }
    else if ((rc == L7_FAILURE) && (rtData->routeAdded == L7_TRUE))
    {
      if (ripMapExtenRipRouteRedistRouteDelete(rtData->network, rtData->subnetMask,
                                              rtData->gateway, rtData->protocol)
          == L7_SUCCESS)
      {
        *flashUpdateNeeded = L7_TRUE;
        rtData->routeAdded = L7_FALSE;
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Purge all the redistributed external routes from RIP
*           map datastructures and in the RIP vendor code
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipPurgeRedistRoutes(void)
{
  ripMapRouteData_t routeData, *rtData;

  memset(&routeData, 0, sizeof(ripMapRouteData_t));

  /* Acquire the ripmap internal redist route table resource */
  if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
    LOG_ERROR(1);

  while (ripMapRouteTableRouteGetNext(&routeData, &rtData) == L7_SUCCESS)
  {
    memcpy(&routeData, rtData, sizeof(routeData));
    if(ripMapRouteTableRouteDelete(rtData->network, rtData->subnetMask) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "%s: Could not Delete route %x/%x from ripMap route table.\n",
                          __FUNCTION__, routeData.network, routeData.subnetMask);
    }
  }

  /* Release the ripmap internal redist route table resource. */
  if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "%s: Could not release the RIP route table semaphore.\n", __FUNCTION__);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if route can be redistributed.
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
* @param    gateway            @b{(input)} Gateway ip address
* @param    protocol           @b{(input)} Type of route. Type L7_REDIST_RT_INDICES_t
* @param    protoSpecific      @b{(input)} Type of route. Type L7_RTO_PROTOCOL_INDICES_t
*
* @returns  L7_SUCCESS    If the route can be redistributed
* @returns  L7_FAILURE    If the route cannot be redistributed
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistributeRouteCheck(L7_IP_ADDR_t network,
                                                  L7_IP_MASK_t subnetMask,
                                                  L7_IP_ADDR_t gateway,
                                                  L7_REDIST_RT_INDICES_t protocol,
                                                  L7_RTO_PROTOCOL_INDICES_t protoSpecific)
{
  L7_BOOL                 pass = L7_FALSE;
  L7_OSPF_METRIC_TYPES_t  ospfMatchType, matchType;
  L7_uint32               hostMode, filter, rc;


  /* We want to delete the route from protocol route table, if one of the conditions
   * is satisfied:
   * 1. Redist mode for the protocol is disabled.
   * 2. The route doesn't pass the filter.
   * 3. The ospf route is not one of the redistributed matchtypes.
   * 4. If it is a host route and hostRouteAcceptMode is disabled.
   *
   * Otherwise, we add the route to the protocol route table.
   */
  /* One pass loop. */
  do
  {
    if (pRipInfo->ripRtRedistMode[protocol] != L7_ENABLE)
    {
      pass = L7_FALSE;
      break;
    }
    if (ripMapRouteRedistributeFilterGet(protocol, &filter) == L7_SUCCESS)
    {
      if (ripMapAclRouteFilter(filter, network, subnetMask) != L7_SUCCESS)
      {
        pass = L7_FALSE;
        break;
      }
    }
    if (protocol == REDIST_RT_OSPF)
    {
      if (ripMapExtenRouteRedistOspfMatchTypeConvert(protoSpecific, &ospfMatchType) == L7_SUCCESS)
      {
        rc = ripMapRouteRedistributeMatchTypeGet(protocol, &matchType);
        if (rc == L7_SUCCESS)
        {
          if ((ospfMatchType & matchType) != ospfMatchType)
          {
            pass = L7_FALSE;
            break;
          }
        }
        else if (rc == L7_ERROR)
        {
          pass = L7_FALSE;
          break;
        }
      }
    }
    if ((subnetMask == HOST_MASK) && (ripMapHostRoutesAcceptModeGet(&hostMode) == L7_SUCCESS))
    {
      if (hostMode == L7_DISABLE)
      {
        pass = L7_FALSE;
        break;
      }
    }
    pass = L7_TRUE;
  } while (0);

  if (pass == L7_TRUE)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Add the route to be redistributed into rip
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
* @param    gateway            @b{(input)} Gateway ip address
* @param    protoSpecific      @b{(input)} Type of route. Type L7_RTO_PROTOCOL_INDICES_t
* @param    rtFlags            @b{(input)} Route Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteAdd(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask,
                            L7_IP_ADDR_t gateway,
                            L7_RTO_PROTOCOL_INDICES_t protoSpecific,
                            L7_uint32 rtFlags)
{
  L7_RC_t rc = L7_SUCCESS;
  ripMapRouteData_t routeData;
  L7_REDIST_RT_INDICES_t proto;
  L7_uint32 metric;
  L7_BOOL ripInit, flashUpdateNeeded = L7_FALSE;

  static const char *routine_name = "ripMapExtenRouteAdd()";

  if (ripMapExtenRouteRedistRouteTypeConvert(protoSpecific, &proto, rtFlags) != L7_SUCCESS)
    return L7_SUCCESS;

  routeData.network = network;
  routeData.subnetMask = subnetMask;
  routeData.gateway = gateway;
  routeData.protoSpecific = protoSpecific;
  routeData.protocol = proto;
  routeData.routeAdded = L7_FALSE;

  rc = ripMapExtenRipRouteRedistributeRouteCheck(network,subnetMask,gateway,proto,protoSpecific);

  ripInit = ripMapIsInitialized();

  /* Acquire the ripmap internal redist route table resource */
  if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
    LOG_ERROR(1);

  if ((ripInit == L7_TRUE) && (rc == L7_SUCCESS))
  {
    if (ripMapRouteRedistributeMetricGet(proto, &metric) != L7_SUCCESS)
    {
      if (ripMapDefaultMetricGet(&metric) != L7_SUCCESS)
        metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
    }
    if (ripMapExtenRipRouteRedistRouteAdd(network, subnetMask, gateway, proto, metric)
        == L7_SUCCESS)
    {
      flashUpdateNeeded = L7_TRUE;
      routeData.routeAdded = L7_TRUE;
    }
  }

  /* Add it to ripMap route table. */
  if (ripMapRouteTableRouteAdd(&routeData) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "%s: Could not add route into ripMap route table.\n", routine_name);
  }

  /* Release the ripmap internal redist route table resource. */
  if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "%s: Could not release the RIP route table semaphore.\n", routine_name);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete the route that was redistributed into rip
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments RTO can't provide the next hop or protocol of a deleted route.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteDelete(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask)
{
  ripMapRouteData_t *routeData;
  L7_BOOL ripInit, flashUpdateNeeded = L7_FALSE;

  static const char *routine_name = "ripMapExtenRouteDelete()";

  ripInit = ripMapIsInitialized();

  /* Acquire the ripmap internal redist route table resource */
  if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
    LOG_ERROR(1);

  /* Search ripMap route table for this route. */
  if (ripMapRouteTableRouteSearch(network, subnetMask, &routeData) == L7_SUCCESS)
  {
    if ((routeData->routeAdded == L7_TRUE) && (ripInit == L7_TRUE))
    {
      if (ripMapExtenRipRouteRedistRouteDelete(network, subnetMask,
                                               routeData->gateway,
                                               routeData->protocol) == L7_SUCCESS)
      {
        routeData->routeAdded = L7_FALSE;
        flashUpdateNeeded = L7_TRUE;
      }
    }
    /* Delete it from ripMap route table. */
    if (ripMapRouteTableRouteDelete(network, subnetMask) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "%s: Could not Delete route from ripMap route table.\n", routine_name);
    }
  }

  /* Release the ripmap internal redist route table resource. */
  if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "%s: Could not release the RIP route table semaphore.\n", routine_name);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Modify the route that was redistributed into rip
*
* @param    network            @b{(input)} Network ip address
* @param    subnetMask         @b{(input)} subnet mask of the network
* @param    gateway            @b{(input)} Gateway ip address
* @param    protoSpecific      @b{(input)} Type of route. Type L7_RTO_PROTOCOL_INDICES_t
* @param    rtFlags            @b{(input)} Route Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteChange(L7_IP_ADDR_t network, L7_IP_MASK_t subnetMask,
                               L7_IP_ADDR_t gateway,
                               L7_RTO_PROTOCOL_INDICES_t protoSpecific,
                               L7_uint32 rtFlags)
{
  ripMapRouteData_t *routeData, tempData;
  L7_REDIST_RT_INDICES_t proto = REDIST_RT_FIRST;
  L7_uint32 metric;
  L7_BOOL ripInit, flashUpdateNeeded = L7_FALSE, protoRouteAdded;
  L7_RC_t rcRouteCheck = L7_SUCCESS, rcRtTypeConv = L7_SUCCESS, rcRtTblSrch = L7_SUCCESS;

  static const char *routine_name = "ripMapExtenRouteChange()";

  ripInit = ripMapIsInitialized();
  /* Check if the new one cannot be redistributed. */
  rcRtTypeConv = ripMapExtenRouteRedistRouteTypeConvert(protoSpecific, &proto, rtFlags);
  if (rcRtTypeConv == L7_SUCCESS)
    rcRouteCheck = ripMapExtenRipRouteRedistributeRouteCheck(network,subnetMask,gateway,proto,protoSpecific);

  /* Acquire the ripmap internal redist route table resource */
  if (ripMapRouteTableResourceAcquire() != L7_SUCCESS)
    LOG_ERROR(1);

  /* Search ripMap route table for this route and check if it was added. */
  rcRtTblSrch = ripMapRouteTableRouteSearch(network, subnetMask, &routeData);
  if (rcRtTblSrch == L7_SUCCESS)
  {
    if ((routeData->routeAdded == L7_TRUE) && (ripInit == L7_TRUE))
    {
      /* Old route was added to rip protocol. So delete it. */
      if (ripMapExtenRipRouteRedistRouteDelete(network, subnetMask, routeData->gateway, routeData->protocol)
          == L7_SUCCESS)
      {
        flashUpdateNeeded = L7_TRUE;
        routeData->routeAdded = L7_FALSE;
      }
    }
  }
  /* The new route is a valid route to be redistributed. */
  if (rcRtTypeConv == L7_SUCCESS)
  {
    protoRouteAdded = L7_FALSE;
    if ((ripInit == L7_TRUE) && (rcRouteCheck == L7_SUCCESS))
    {
      if (ripMapRouteRedistributeMetricGet(proto, &metric) != L7_SUCCESS)
      {
        if (ripMapDefaultMetricGet(&metric) != L7_SUCCESS)
          metric = L7_RIP_DEFAULT_METRIC_DEFAULT_DECONFIG;
      }
      if (ripMapExtenRipRouteRedistRouteAdd(network, subnetMask, gateway, proto, metric)
          == L7_SUCCESS)
      {
        flashUpdateNeeded = L7_TRUE;
        protoRouteAdded = L7_TRUE;
      }
    }
    if (rcRtTblSrch == L7_SUCCESS)
    {
      /* If old route exists in ripMap route table, then modify the contents. */
      if (routeData->gateway != gateway)
        routeData->gateway = gateway;
      if (routeData->protoSpecific != protoSpecific)
        routeData->protoSpecific = protoSpecific;
      if (routeData->protocol != proto)
        routeData->protocol = proto;
      routeData->routeAdded = protoRouteAdded;
    }
    else
    {
      /* If old route doesn't exist in ripMap route table, then add it. */
      tempData.network = network;
      tempData.subnetMask = subnetMask;
      tempData.gateway = gateway;
      tempData.protoSpecific = protoSpecific;
      tempData.protocol = proto;
      tempData.routeAdded = protoRouteAdded;
      if (ripMapRouteTableRouteAdd(&tempData) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
                "%s: Could not add route to ripMap route table.\n", routine_name);
      }
    }
  }
  else
  {
    if (rcRtTblSrch == L7_SUCCESS)
    {
      /* Delete it from ripMap route table if the new route cannot be redistributed. */
      if (ripMapRouteTableRouteDelete(network, subnetMask) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
                "%s: Could not Delete route from ripMap route table.\n", routine_name);
      }
    }
  }

  /* Release the ripmap internal redist route table resource. */
  if (ripMapRouteTableResourceRelease() != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "%s: Could not release the RIP route table semaphore.\n", routine_name);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Adds the redistributed route into the rip protocol route table
*
* @param    dst                @b{(input)} Network ip address
* @param    mask               @b{(input)} subnet mask of the network
* @param    gw                 @b{(input)} Gateway ip address
* @param    proto              @b{(input)} Type of route. Type L7_REDIST_RT_INDICES_t
* @param    metric             @b{(input)} Metric of the route
*
* @returns  L7_SUCCESS
*
* @comments Sends a message to the protocol task to add the route.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistRouteAdd(L7_IP_ADDR_t dst, L7_IP_MASK_t mask,
                                          L7_IP_ADDR_t gw, L7_REDIST_RT_INDICES_t proto, L7_uint32 metric)
{
  ripPipeMsg_t     msg;
  rip_redist_proto rip_proto;

  rip_proto = RIP_RT_REDIST_FIRST;
  if (ripMapVendRouteRedistProtocolConvert(proto, &rip_proto) != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not convert ripMap redistribution protocol to rip redistribution protocol.\n");

  /* Send the default route metric change message to vendor code */
  if (ripMapIsInitialized() == L7_TRUE)
  {
    msg.msgId = L7_RIP_ROUTE_REDISTRIBUTE_ROUTE_ADD;
    msg.arg1 = dst;
    msg.arg2 = mask;
    msg.arg3 = gw;
    msg.rip_proto = rip_proto;
    msg.arg4 = metric;
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Deletes the redistributed route from the rip protocol route table
*
* @param    dst                @b{(input)} Network ip address
* @param    mask               @b{(input)} subnet mask of the network
* @param    gw                 @b{(input)} Gateway ip address
* @param    proto              @b{(input)} Type of route. Type L7_REDIST_RT_INDICES_t
*
* @returns  L7_SUCCESS
*
* @comments Sends a message to the protocol task to delete the route.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRipRouteRedistRouteDelete(L7_IP_ADDR_t dst, L7_IP_MASK_t mask,
                                             L7_IP_ADDR_t gw, L7_REDIST_RT_INDICES_t proto)
{
  ripPipeMsg_t     msg;
  rip_redist_proto rip_proto;

  rip_proto = RIP_RT_REDIST_FIRST;
  if (ripMapVendRouteRedistProtocolConvert(proto, &rip_proto) != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not convert ripMap redistribution protocol to rip redistribution protocol.\n");

  /* Send the default route metric change message to vendor code */
  if (ripMapIsInitialized() == L7_TRUE)
  {
    msg.msgId = L7_RIP_ROUTE_REDISTRIBUTE_ROUTE_DELETE;
    msg.arg1 = dst;
    msg.arg2 = mask;
    msg.arg3 = gw;
    msg.rip_proto = rip_proto;
    osapiPipeWrite(intfCom.intfPipe, (L7_char8 *) &msg, RIP_INTF_PIPE_MSG_LEN);
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Convert the route type from rto route type to redistribution route type.
*
* @param    routeType         @b{(input)} RTO route type
* @param    protocol          @b{(output)} Redistribution route type
* @param    rtFlags           @b{(input)} Route Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteRedistRouteTypeConvert(L7_uint32 routeType,
                                               L7_REDIST_RT_INDICES_t *protocol,
                                               L7_uint32 rtFlags)
{

  if (protocol == L7_NULLPTR)
    return L7_FAILURE;

  /* Don't redistribute reject routes */
  if (rtFlags & L7_RTF_REJECT)
    return L7_FAILURE;

  switch (routeType)
  {
    case RTO_OSPF_INTRA_AREA:
    case RTO_OSPF_INTER_AREA:
    case RTO_OSPF_TYPE1_EXT:
    case RTO_OSPF_TYPE2_EXT:
    case RTO_OSPF_NSSA_TYPE1_EXT:
    case RTO_OSPF_NSSA_TYPE2_EXT:
      *protocol = REDIST_RT_OSPF;
      break;

    case RTO_IBGP:
    case RTO_EBGP:
      *protocol = REDIST_RT_BGP;
      break;

    case RTO_LOCAL:
      *protocol = REDIST_RT_LOCAL;
      break;

    case RTO_STATIC:
      *protocol = REDIST_RT_STATIC;
      break;

    default:
      return L7_FAILURE;
      break;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Convert the route type from rto ospf route type to redistribution ospf route type.
*
* @param    rtoOspfType         @b{(input)} RTO ospf route type
* @param    *redistOspfType     @b{(output)} Redistribution ospf route type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteRedistOspfMatchTypeConvert(L7_uint32 rtoOspfType, L7_OSPF_METRIC_TYPES_t *redistOspfType)
{
  if (redistOspfType == L7_NULLPTR)
    return L7_FAILURE;

  switch (rtoOspfType)
  {
    case RTO_OSPF_INTRA_AREA:
    case RTO_OSPF_INTER_AREA:
      *redistOspfType = L7_OSPF_METRIC_TYPE_INTERNAL;
      break;

    case RTO_OSPF_TYPE1_EXT:
      *redistOspfType = L7_OSPF_METRIC_TYPE_EXT1;
      break;

    case RTO_OSPF_TYPE2_EXT:
      *redistOspfType = L7_OSPF_METRIC_TYPE_EXT2;
      break;

    case RTO_OSPF_NSSA_TYPE1_EXT:
      *redistOspfType = L7_OSPF_METRIC_TYPE_NSSA_EXT1;
      break;

    case RTO_OSPF_NSSA_TYPE2_EXT:
      *redistOspfType = L7_OSPF_METRIC_TYPE_NSSA_EXT2;
      break;

    default:
      return L7_FAILURE;
      break;
  }

  return L7_SUCCESS;
}

/*
----------------------------------------------------------------------
                     SET FUNCTIONS  -  INTERFACE CONFIG
----------------------------------------------------------------------
*/

/* ######################################################################
 * NOTE:  All intf config 'set' functions return with a successful return
 *        code in the event the desired RIP interface structure is not
 *        found.  This allows for pre-configuration of RIP routing
 *        interfaces (the config gets picked up in rip_ifinit() when the
 *        interface structure is created).
 *
 * NOTE:  All intf config functions accept a pointer to a RIP vendor
 *        interface structure, if known.  Otherwise, this p parm must be
 *        set to L7_NULL.  The intIfNum parm is required in either case.
 *        (This mechanism allows these same functions to be used both when
 *        handling config requests from the user interface as well as when
 *        the vendor RIP code asks for its interface config information.)
 * ######################################################################
 */

/*********************************************************************
* @purpose  Set the RIP authentication type for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    authType    @b{(input)} Authentication type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only supporting one authorization type/key at this time.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfAuthTypeSet(L7_uint32 intIfNum, void *p, L7_uint32 authType)
{
  struct interface   *ifp;
  u_int16_t          ripAuthType;
  L7_uchar8          blankKey[RIP_AUTH_PW_LEN] = { 0 };
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32          count = L7_NULL;

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }

  for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
  {
    if (ipAddrList[count].ipAddr == L7_NULL_IP_ADDR)
    {
      continue;
    }
    if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[count].ipAddr, p, &ifp) != L7_SUCCESS)
    {
      return L7_SUCCESS;
    }

    /* convert the system auth type into the RIP auth type code */
    switch (authType)
    {
    case L7_AUTH_TYPE_NONE:
      ripAuthType = osapiNtohs(RIP_AUTH_NONE);
      break;

    case L7_AUTH_TYPE_SIMPLE_PASSWORD:
      ripAuthType = osapiNtohs(RIP_AUTH_PW);
      break;

    case L7_AUTH_TYPE_MD5:
      ripAuthType = osapiNtohs(RIP_AUTH_MD5);
      break;

    default:
      return L7_FAILURE;
    }

    /* NOTE:  Only supporting one authorization type/key at this time */
    ifp->int_auth[0].type = ripAuthType;

    /* now that the auth type was set, set the auth key to a 'blank' value */
    if (ripMapExtenIntfAuthKeySet(intIfNum, ifp, blankKey) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Could not set blank authen key for interface %s", ifName);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP authentication key for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    key         @b{(input)} Authentication key
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The authentication key is always passed using the maximum
*           allowed key length, and is already padded with zeroes on
*           the right hand side.
*
* @notes    Only supporting one authorization type/key at this time.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfAuthKeySet(L7_uint32 intIfNum, void *p, L7_uchar8 *key)
{
  struct interface  *ifp;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32          count = L7_NULL;

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }

  for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
  {
    if (ipAddrList[count].ipAddr == L7_NULL_IP_ADDR)
    {
      continue;
    }
    if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[count].ipAddr, p, &ifp) != L7_SUCCESS)
      return L7_SUCCESS;

    /* compile-time check that RIP mapping layer and vendor code match */
#if (L7_AUTH_MAX_KEY_RIP != RIP_AUTH_PW_LEN)
#error RIP authentication key max length definitions mismatch!
#endif

    /* set this key to be valid 'forever'
     * NOTE:  This is needed until key management is added to the overall system
     *        design (including support for more than one key ID per interface)
     */
    memcpy(ifp->int_auth[0].key, key, RIP_AUTH_PW_LEN);
    ifp->int_auth[0].start = (time_t)0;
    ifp->int_auth[0].end = (time_t)-1;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP authentication key id for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    keyId       @b{(input)} Authentication key id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only supporting one authorization type/key at this time.
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfAuthKeyIdSet(L7_uint32 intIfNum, void *p, L7_uint32 keyId)
{
  struct interface  *ifp;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32          count = L7_NULL;

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }
  for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
  {
    if (ipAddrList[count].ipAddr == L7_NULL_IP_ADDR)
    {
      continue;
    }

    if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[count].ipAddr, p, &ifp) != L7_SUCCESS)
      return L7_SUCCESS;

    ifp->int_auth[0].keyid = (int8_t)keyId;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP version used for sending updates on the specified
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    version     @b{(input)} RIP version used for sends
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfVerSendSet(L7_uint32 intIfNum, void *p, L7_uint32 version)
{
  struct interface  *ifp;
  u_int         flags;
  L7_BOOL       mcast;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32          count = L7_NULL;

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }
  for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
  {
    if (ipAddrList[count].ipAddr == L7_NULL_IP_ADDR)
    {
      continue;
    }
    if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[count].ipAddr, p, &ifp) != L7_SUCCESS)
      return L7_SUCCESS;

    /* NOTE:  Normally, RIPv1 is sent broadcast while RIPv2 is sent multicast.
     *        RIPv1 compatibility mode, however, sends RIPv2 using broadcast.
     */
    mcast = L7_FALSE;                     /* override as necessary */

    /* translate the version parm into RIP int_state flag value */
    switch (version)
    {
    case L7_RIP_CONF_SEND_DO_NOT_SEND:
      flags = (IS_NO_RIPV1_OUT | IS_NO_RIPV2_OUT);
      break;

    case L7_RIP_CONF_SEND_RIPV1:
      flags = IS_NO_RIPV2_OUT;
      break;

    case L7_RIP_CONF_SEND_RIPV1_COMPAT:
      flags = IS_NO_RIPV1_OUT;
      break;

    case L7_RIP_CONF_SEND_RIPV2:
      flags = IS_NO_RIPV1_OUT;
      mcast = L7_TRUE;
      break;

    default:
      return L7_FAILURE;
    }

    ifp->int_state = (ifp->int_state & ~IS_NO_RIP_OUT) | flags;

    /* note the negative state flag */
    if (mcast == L7_TRUE)
      ifp->int_state &= ~IS_NO_RIP_MCAST;
    else
      ifp->int_state |= IS_NO_RIP_MCAST;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the RIP version used for receiving updates from the specified
*           interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    version     @b{(input)} RIP version allowed for receives
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfVerRecvSet(L7_uint32 intIfNum, void *p, L7_uint32 version)
{
  struct interface  *ifp;
  u_int              flags;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32          count = L7_NULL;

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }

  for (count = 0; count < L7_L3_NUM_IP_ADDRS; count++)
  {
    if (ipAddrList[count].ipAddr == L7_NULL_IP_ADDR)
    {
      continue;
    }
    if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[count].ipAddr, p, &ifp) != L7_SUCCESS)
      return L7_SUCCESS;

    /* translate the version parm into RIP int_state flag value */
    switch (version)
    {
    case L7_RIP_CONF_RECV_RIPV1:
      flags = IS_NO_RIPV2_IN;
      break;

    case L7_RIP_CONF_RECV_RIPV2:
      flags = IS_NO_RIPV1_IN;
      break;

    case L7_RIP_CONF_RECV_RIPV1_RIPV2:
      flags = 0;
      break;

    case L7_RIP_CONF_RECV_DO_NOT_RECEIVE:
      flags = (IS_NO_RIPV1_IN | IS_NO_RIPV2_IN);
      break;

    default:
      return L7_FAILURE;
    }

    ifp->int_state = (ifp->int_state & ~IS_NO_RIP_IN) | flags;
  }
  return L7_SUCCESS;
}


/*
----------------------------------------------------------------------
                     GET FUNCTIONS  -  GLOBAL
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Get the number of route changes made by RIP to the
*           IP Route Database
*
* @param    *val        @b{(output)} RIP route changes count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenRouteChangesGet(L7_uint32 *val)
{
  *val = (L7_uint32)rip_glob_mibstats.route_changes;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of responses sent to RIP queries from other systems
*
* @param    *val        @b{(output)} RIP query response count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenQueriesGet(L7_uint32 *val)
{
  *val = (L7_uint32)rip_glob_mibstats.queries;
  return L7_SUCCESS;
}


/*
----------------------------------------------------------------------
                     GET FUNCTIONS  -  INTERFACE
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Get the number of RIP response packets received by the RIP
*           process which were subsequently discarded for any reason
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Receive Bad Packet count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfRcvBadPacketsGet(L7_uint32 intIfNum, void *p, L7_uint32 *val)
{
  struct interface  *ifp;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }

  if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[0].ipAddr, p, &ifp) != L7_SUCCESS)
  {
    *val = 0;
    return L7_FAILURE;
  }

  *val = (L7_uint32)ifp->int_mibstats.ibadpkts;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of routes contained in valid RIP packets that
*           were ignored for any reason
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Receive Bad Routes count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfRcvBadRoutesGet(L7_uint32 intIfNum, void *p, L7_uint32 *val)
{
  struct interface  *ifp;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }
  if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[0].ipAddr, p, &ifp) != L7_SUCCESS)
  {
    *val = 0;
    return L7_FAILURE;
  }

  *val = (L7_uint32)ifp->int_mibstats.ibadroutes;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of triggered RIP updates actually sent
*           on this interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Sent Triggered Updates count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfSentUpdatesGet(L7_uint32 intIfNum, void *p, L7_uint32 *val)
{
  struct interface  *ifp;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }
  if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[0].ipAddr, p, &ifp) != L7_SUCCESS)
  {
    *val = 0;
    return L7_FAILURE;
  }

  *val = (L7_uint32)ifp->int_mibstats.otrupdates;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the current operational state of the specified RIP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} RIP interface state (L7_UP or L7_DOWN)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfStateGet(L7_uint32 intIfNum, void *p, L7_uint32 *val)
{
  struct interface  *ifp;
  L7_uint32         rtrIfNum;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }

  if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[0].ipAddr , p, &ifp) == L7_SUCCESS)
    *val = (ifp->int_if_flags & IFF_UP) ? L7_UP : L7_DOWN;

  else
  {
    /* check if this is even a router interface to determine how to respond */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      *val = 0;
      return L7_FAILURE;
    }
    *val = L7_DOWN;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the state flags of the specified RIP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} RIP interface state flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfStateFlagsGet(L7_uint32 intIfNum, void *p, L7_uint32 *val)
{
  struct interface  *ifp;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }
  if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[0].ipAddr, p, &ifp) != L7_SUCCESS)
  {
    *val = 0;
    return L7_FAILURE;
  }

  *val = (L7_uint32)ifp->int_if_flags;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of times this RIP interface has changed
*           its state (from up to down)
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    p           @b{(input)} RIP vendor interface struct ptr (or L7_NULL)
* @param    *val        @b{(output)} Link Events Counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapExtenIntfLinkEventsCtrGet(L7_uint32 intIfNum, void *p, L7_uint32 *val)
{
  struct interface  *ifp;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];

  if (ipMapRtrIntfIpAddrListGet(intIfNum, ipAddrList) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "Could not get Ip address list for interface %s", ifName);
    return L7_FAILURE;
  }

  if (ripMapExtenInterfaceFind(intIfNum, ipAddrList[0].ipAddr, p, &ifp) != L7_SUCCESS)
  {
    *val = 0;
    return L7_FAILURE;
  }

  *val = (L7_uint32)ifp->int_transitions;
  return L7_SUCCESS;
}


/*
----------------------------------------------------------------------
                         PRIVATE HELPER FUNCTIONS
----------------------------------------------------------------------
*/

/*********************************************************************
* @purpose  Obtains a pointer to the RIP interface structure for the
*           specified internal interface number
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddress   @b{(input)} IP Address of interface to search
* @param    p           @b{(input)} RIP vendor interface struct ptr, or L7_NULL
* @param    **ifp       @b{(output)} RIP interface structure pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if routing interface does not exist
* @returns  L7_FAILURE
*
* @notes    Only intended to be used within this file.
*
* @notes    The p parm is provided to handle cases where the RIP interface
*           structure being referenced is not yet in the ifnet linked list.
*           This typically occurs when an interface is being created.
*
* @end
*********************************************************************/
static L7_RC_t ripMapExtenInterfaceFind(L7_uint32 intIfNum,
                                        L7_uint32 ipAddress,
                                        struct interface *p,
                                        struct interface **ifp)
{
  L7_uint32     rtrIfNum;

  /* check that the vendor RIP application has been initialized */
  if (ripMapIsInitialized() != L7_TRUE)
    return L7_FAILURE;


  /* if a passed-in interface pointer is available, just feed it back */
  if (p != L7_NULL)
  {
    *ifp = p;
    return L7_SUCCESS;
  }

  /* get the routing interface number to be used as the search value */
  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    return L7_ERROR;

  /* walk through each RIP interface structure, comparing its index value */
  for (p = ifnet; p != 0; p = p->int_next)
  {
    if (p->int_index == rtrIfNum && p->int_addr == ipAddress)
    {
      *ifp = p;
      return L7_SUCCESS;
    }
  }
  return L7_ERROR;
}

