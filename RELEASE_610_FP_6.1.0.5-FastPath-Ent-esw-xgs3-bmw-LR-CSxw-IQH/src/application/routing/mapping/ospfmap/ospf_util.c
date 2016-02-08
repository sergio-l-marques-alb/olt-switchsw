/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ospf Utility functions
*
* @purpose   OSPF Utility Functions
*
* @component Ospf Mapping Layer
*
* @comments  none
*
* @create    03/13/2001
*
* @author    anayar
*
* @end
*
**********************************************************************/

#include "l7_ospfinclude.h"
#include "defaultconfig.h"
#include "sysnet_api_ipv4.h"
#include "l7_socket.h"

extern L7_ospfMapCfg_t     *pOspfMapCfgData;
extern ospfInfo_t         *pOspfInfo;
extern ospfAreaInfo_t     *pOspfAreaInfo;
extern ospfIntfInfo_t     *pOspfIntfInfo;
extern ospfNetworkAreaInfo_t *pOspfNetworkAreaInfo;
extern L7_uint32 tosToTosIndex[L7_RTR_MAX_TOS];
extern L7_uint32 tosIndexToTos[L7_RTR_MAX_TOS_INDICES];
extern ospfNetworkAreaInfo_t  *networkAreaListHead_g, *networkAreaListFree_g;

static L7_RC_t ospfMapStubStatusSet ( L7_uint32 areaId, L7_int32 TOS,
                                      L7_int32 val);
static L7_RC_t _ospfMapIntfModeAndAreaCompute(L7_uint32 intIfNum);
static L7_RC_t ospfMapIntfAreaAssign(L7_uint32 intIfNum, L7_uint32 areaId, 
                                     L7_BOOL advSecondaries);

/*
**********************************************************************
*                    API FUNCTIONS  -  GLOBAL CONFIG
**********************************************************************
*/
/*********************************************************************
* @purpose  Enable the Ospf Routing Function
*
* @param    mode  @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAdminModeEnable(void)
{
   L7_uint32 intIfNum, i, routerId;
   sysnetPduIntercept_t sysnetPduIntercept;
   L7_RC_t rc;

   static const char *routine_name = "ospfMapOspfAdminModeEnable()";

   OSPFMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

   if (ospfMapRouterIDGet(&routerId) == L7_SUCCESS)
     if(routerId <= 0)
       return L7_FAILURE;

/*------------------------------------------------------*/
/*    Initialize OSPF stack if not already initialized  */
/*------------------------------------------------------*/
   if(ospfMapOspfInitialized() != L7_TRUE)
     return ospfMapAppsInit();

    rc = ospfMapRawSockInit();
    if(rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID, 
              "Failed to create OSPFv2 socket. Error code %u. "
              "OSPFv2 cannot be enabled.", rc);
      return rc;
    }

/*--------------------------------------------------------------*/
/*     Proceed to enable OSPF only if routing has been enabled  */
/*--------------------------------------------------------------*/
   if(ipMapRtrAdminModeGet() != L7_ENABLE)
   {
     /* Its observed that ipMapLockTake() in ipMapRtrAdminModeGet() is
      * failing due to pthread_mutex_lock failure that happens sometimes
      * due to some reasons. We need to retry the pthread_mutex_lock again
      * for it to succeed in such situations. Hence calling
      * ipMapRtrAdminModeGet() again.
      */
     if(ipMapRtrAdminModeGet() != L7_ENABLE)
     {
       return L7_SUCCESS;
     }
   }

/*------------------------------------------------------*/
/*    Register for SysNet packet intercept              */
/*------------------------------------------------------*/
    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_MCAST_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_OSPF_FILTER_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = ospfMapMcastIntercept;
    strcpy(sysnetPduIntercept.interceptFuncName, "ospfMapMcastIntercept");
    if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
    {
      /* NOTE: Don't care about bad rc here -- can happen if this function
       *       called multiple times in succession.
       */
    }

/*-----------------------------------------*/
/*    Apply the OSPF Configuration Change  */
/*-----------------------------------------*/
    if (ospfMapExtenOspfAdminModeSet(L7_ENABLE) == L7_SUCCESS)
    {
      /* apply the new router id if changed by the user */
      ospfMapRouterIDApply(routerId);

      /*------------------------------------------------------------------------------*/
      /* Enable router interfaces which are configured for routing */
      /*------------------------------------------------------------------------------*/
      for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
      {
         if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
         {
             if (ospfMayEnableInterface(intIfNum) == L7_TRUE)
             {
                 ospfMapIntfAdminModeEnable(intIfNum);
             }
         }
      } /* for (i=0; i < L7_RTR_MAX_RTR_INTERFACES; i++)  */

      /* Register OSPF to be notified of ACL changes.
       * OSPF uses ACLs to filter routes for redistribution.
       */
      if (ospfMapAclCallbackIsRegistered(ospfMapAclCallback) == L7_FALSE) {
          if (ospfMapAclCallbackRegister(ospfMapAclCallback, "ospfMapAclCallback") == L7_FAILURE) {
              OSPFMAP_ERROR("\n%s %d: %s: ERROR:  Failed to register for ACL changes.\n",
                            __FILE__, __LINE__, routine_name);
          }
      }

      /* If OSPF configured to redistribute, register for best route changes. */
      if (ospfMapIsAsbr()) {
        /* OSPF evaluates best routes as candidates for redistribution. */
        if ((rtoBestRouteClientRegister("OSPF Redist", 
                                        ospfMapExtenRouteCallback)) == L7_SUCCESS)
        {
          pOspfInfo->registeredWithRto = L7_TRUE;
        }
        else
        {
          OSPFMAP_ERROR("\n%s %d: %s: ERROR:  Failed to register for RTO changes.\n",
                        __FILE__, __LINE__, routine_name);
        }
      }

      /* Originate default route if configured to do so. */
      if (pOspfMapCfgData->defRouteCfg.origDefRoute) {
          ospfMapExtenDefRouteOrig();
      }
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable the Ospf Routing Function
*
* @param    mode  @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapOspfAdminModeDisable(void)
{
  L7_uint32 i, intIfNum, mode;
  sysnetPduIntercept_t sysnetPduIntercept;

  static const char *routine_name = "ospfMapOspfAdminModeDisable()";

  OSPFMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

  /* Return success if OSPF has not been initialized.
     This allows for dynamic configurability of OSPF. */
    if(ospfMapOspfInitialized() != L7_TRUE)
        return L7_SUCCESS;


/*------------------------------------------------------*/
/*    De-register for SysNet packet intercept           */
/*------------------------------------------------------*/
    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_MCAST_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_OSPF_FILTER_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = ospfMapMcastIntercept;
    strcpy(sysnetPduIntercept.interceptFuncName, "ospfMapMcastIntercept");
    if (sysNetPduInterceptDeregister(&sysnetPduIntercept) != L7_SUCCESS)
    {
      /* NOTE: Don't care about bad rc here -- can happen if this function
       *       called multiple times in succession.
       */
    }

/*-----------------------------------------*/
/*    Apply the OSPF Configuration Change  */
/*-----------------------------------------*/

    if (ospfIsRegisteredWithRto())
    {
      rtoBestRouteClientDeregister("OSPF Redist", ospfMapExtenRouteCallback);
      pOspfInfo->registeredWithRto = L7_FALSE;
    }

    /* Stop receiving notifications for ACL changes. */
    if (ospfMapAclCallbackIsRegistered(ospfMapAclCallback) == L7_TRUE) {
        if (ospfMapAclCallbackUnregister(ospfMapAclCallback) == L7_FAILURE) {
            OSPFMAP_ERROR("\n%s %d: %s: ERROR:  Failed to de-register for ACL changes.\n",
                          __FILE__, __LINE__, routine_name);
        }
    }

    /* Purge all external LSAs. */
    ospfMapExtenPurgeExternalLsas();

    /* Disable interfaces before disabling globally so one-way hellos
       * are sent on each interface. */
    for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
    {
       if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
       {
         /* Check if ospf is enabled on the interface */
         if(ospfMapIntfEffectiveAdminModeGet(intIfNum, &mode) == L7_SUCCESS &&
            mode == L7_ENABLE)
         {
           ospfMapIntfAdminModeDisable(intIfNum);
         }
       }
    } 
 
    ospfMapExtenOspfAdminModeSet(L7_DISABLE);

    if (ospfMapRawSockDelete() != L7_SUCCESS)
    {
      OSPFMAP_ERROR("%s %d: %s: ERROR: OSPF_MAP: Socket deletion error\n", 
                      __FILE__, __LINE__, routine_name);
    }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ospf Router ID
*
* @param    routerID   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapRouterIDApply(L7_uint32 routerID)
{
  L7_uint32 vendorAdminStat;
  L7_uint32 adminMode;

  static const char *routine_name = "ospfMapRouterIDApply()";

  OSPFMAP_TRACE("%s %d: %s : routerID = %x\n", __FILE__, __LINE__, routine_name, routerID);

  if(routerID > 0)
  {
    if(ospfMapOspfInitialized() == L7_TRUE)
    {
      /* OSPF may need to be enabled if we are configuring a router id
         after a previous reset */
      if((ospfMapOspfAdminModeGet(&adminMode) == L7_SUCCESS) &&
         (ospfMapExtenAdminStatGet(&vendorAdminStat) == L7_SUCCESS))
        if((vendorAdminStat != L7_ENABLE) && (adminMode == L7_ENABLE))
          return ospfMapOspfAdminModeEnable();

      return (ospfMapExtenRouterIDSet(routerID));
    }
    else
      return ospfMapAppsInit();
  }

  return L7_FAILURE;
}

 /*********************************************************************
* @purpose  Sets the ASBR status of the router.
*
* @param    mode   @b{(input)}     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapAsbrStatusApply(L7_uint32 mode)
{
  L7_uint32 adminMode;

  static const char *routine_name = "ospfMapAsbrStatusApply()";

  OSPFMAP_TRACE("%s %d: %s : mode = %s \n", __FILE__, __LINE__, routine_name,
              mode?"enable":"disable");

  if (mode == L7_ENABLE)
  {
    if ((ospfMapOspfAdminModeGet(&adminMode) == L7_SUCCESS) && (adminMode == L7_ENABLE))
    {
      /* register for best route changes */
      if ((rtoBestRouteClientRegister("OSPF Redist", 
                                      ospfMapExtenRouteCallback)) == L7_SUCCESS)
      {
        pOspfInfo->registeredWithRto = L7_TRUE;
      }
      else
      {
        OSPFMAP_ERROR("\n%s %d: %s: ERROR:  Failed to register for RTO changes.\n",
                      __FILE__, __LINE__, routine_name);
      }
    }
  }
  else if ((mode == L7_DISABLE) && ospfIsRegisteredWithRto())
  {
    rtoBestRouteClientDeregister("OSPF Redist", ospfMapExtenRouteCallback);
    pOspfInfo->registeredWithRto = L7_FALSE;
  }

  /* Return success if OSPF has not been initialized.
     This allows for dynamic configurability of OSPF. */
  if(ospfMapOspfInitialized() == L7_TRUE)
    return (ospfMapExtenAsbrStatusSet(mode) );

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether the router is an ASBR.
*
* @param    none
*
* @returns  L7_TRUE if router is an ASBR
* @returns  L7_FALSE if router is not an ASBR
*
* @notes
*
* @notes    User no longer configures a single ASBR
*           admin mode parameter. Instead, the user configures
*           redistribution independently for each source. Router is also
*           considered an ASBR if OSPF is configured to originate a 
*           default route.
*
* @end
*********************************************************************/
L7_BOOL ospfMapIsAsbr()
{
    L7_REDIST_RT_INDICES_t source;
    if (pOspfMapCfgData->defRouteCfg.origDefRoute) {
        return L7_TRUE;
    }
    for (source = REDIST_RT_FIRST + 1; source < REDIST_RT_LAST; source++) {
        if (pOspfMapCfgData->redistCfg[source].redistribute) {
            return L7_TRUE;
        }
    }
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Configures if Ospf is compatible with RFC 1583
*
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If enabled, preference rules remain those as defined by
* @notes    RFC 1583.  If disabled, preference rules are as stated
* @notes    in Section 16.4.1 of the OSPFv2 standard. When disabled,
* @notes    greater protection is provided against routing loops.
*
* @end
*********************************************************************/
L7_RC_t ospfMapRfc1583CompatibilityApply(L7_uint32 mode)
{
  static const char *routine_name = "ospfMapRfc1583CompatibilityApply()";

  OSPFMAP_TRACE("%s %d: %s : mode = %s \n", __FILE__, __LINE__, routine_name,
              mode?"enable":"disable");

  if(ospfMapOspfInitialized() == L7_TRUE)
    return(ospfMapExtenRfc1583CompatibilitySet( mode) );

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures the value of MtuIgnore flag
*
* @param    val        @b{(input)}  L7_TRUE or L7_FLASE
* @param    intIfNum   @b{(input)} L7_uint32
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If true, Mtu value is ignored while forming adjacencies
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMtuIgnoreApply(L7_uint32 intIfNum, L7_BOOL val)
{
  static const char *routine_name = "ospfMapMtuIgnoreApply()";

  OSPFMAP_TRACE("%s %d: %s : mode = %s \n", __FILE__, __LINE__, routine_name,
              val?"true":"false");

  /* Return success if OSPF has not been initialized.
  ** This allows for dynamic configurability of OSPF.
  */
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  return ospfMapExtenIntfMtuIgnoreSet(intIfNum, val);
}

/*********************************************************************
* @purpose  Configures the value of MaxIpMTUsize
*
* @param    ipMtu   @b{(input)} L7_uint32
* @param    intIfNum   @b{(input)} L7_uint32
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    MaxMTUsize is used in DD packets
*
* @end
*********************************************************************/
L7_RC_t ospfMapInftIpMtuApply(L7_uint32 intIfNum, L7_uint32 ipMtu)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  return(ospfMapExtenIntfIpMtuSet(intIfNum, ipMtu));
}

/*********************************************************************
* @purpose Apply interface cost metric
*
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfBandwidthApply(L7_uint32 intIfNum)
{
  ospfCfgCkt_t *pCfg;
  L7_uint32 ifMetric;

    if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if(ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
            return L7_SUCCESS;
      if (pCfg->metric == L7_OSPF_INTF_METRIC_NOT_CONFIGURED)
      {
        if (ospfMapIntfMetricCalculate(intIfNum, &ifMetric) == L7_SUCCESS)
          return(ospfMapExtenIfMetricValueSet( intIfNum,L7_TOS_NORMAL_SERVICE,ifMetric));
      }
    }
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Sets the Exit OverFlow Interval
*
* @param    val         seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of seconds, that after entering Overflow
*             state, a router will attempt to leave OverflowState.
*             This allows the router to again originate non-default
*             AS-external-LSAs.  When set to 0, the router will
*             not leave OverflowState until restarted."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExitOverflowIntervalApply ( L7_int32 val )
{
  /* Return success if OSPF has not been initialized.
     This allows for dynamic configurability of OSPF. */
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenExitOverflowIntervalSet(val);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the delay time between when OSPF receives a topology
*           change and when it starts an SPF calculation
*
* @param    spfDelay    @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between when OSPF receives
*             a topology change and when it starts an SPF calculation."
*
* @end
*********************************************************************/
L7_RC_t ospfMapSpfDelayApply(L7_int32 spfDelay)
{
  /* Return success if OSPF has not been initialized.
     This allows for dynamic configurability of OSPF. */
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenSpfDelaySet(spfDelay);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the minimum time (in seconds) between two consecutive
*           SPF calculations
*
* @param    spfHoldtime @b{(input)}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The length of time in seconds between two consecutive
*             SPF calculations."
*
* @end
*********************************************************************/
L7_RC_t ospfMapSpfHoldtimeApply(L7_int32 spfHoldtime)
{  
  /* Return success if OSPF has not been initialized.
     This allows for dynamic configurability of OSPF. */
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenSpfHoldtimeSet(spfHoldtime);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the maximum number of non-default AS-external-LSAs entries
*           that can be stored in the link-state database.
*
* @param    val         number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The default value is -1. If the value is -1, then there
*           is no limit. When the number of non-default AS-external-LSAs
*           in a router's link-state database reaches ExtLsdbLimit, the
*           router enters overflow state. The router never holds more than
*           ExtLsdbLimit non-default AS-external-LSAs in it database
*           ExtLsdbLimit MUST be set identically in all routers attached
*           to the OSPF backbone and/or any regular OSPF area. (that is,
*           OSPF stub areas and NSSAs are excluded."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbLimitApply ( L7_int32 val )
{
  /* Return success if ospf is not initialized this allows
     dynamic configurability of ospf */
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenExtLsdbLimitSet(val);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply a configuration change to the maximum number of next 
*           hops that OSPF can report to a given destination.
*          
* @param    maxPaths         
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapMaxPathsApply(L7_uint32 maxPaths)
{
    if (ospfMapOspfInitialized() == L7_TRUE)
    {
        return ospfMapExtenMaxPathsSet(maxPaths);
    }
    return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Apply a configuration change to all the interfaces.
 *
 * @param    no
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapAutoCostRefBwApply(void)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_uint32 ifMetric;
  ospfCfgCkt_t *pCfg;
  int i;
  
  if (ospfMapOspfInitialized() == L7_TRUE)
  {
     for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
     {
        if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
        {
           if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
           {
              if (pCfg->metric == L7_OSPF_INTF_METRIC_NOT_CONFIGURED)
              {
                 if (ospfMapIntfMetricCalculate(intIfNum, &ifMetric) == L7_SUCCESS)
                    rc = ospfMapExtenIfMetricValueSet(intIfNum,L7_TOS_NORMAL_SERVICE,ifMetric);
              } 
           }
        }
     }  
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Apply a configuration change to all the interfaces.
 *
 * @param    defaultPassiveMode @b{(input)} default PassiveMode of an interface
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if failure
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ospfMapPassiveModeApply(L7_BOOL defaultPassiveMode)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  ospfCfgCkt_t *pCfg;
  
  for (intIfNum = 1; intIfNum < L7_OSPF_INTF_MAX_COUNT; intIfNum++)
  {
     if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
     {
        /* default setting overrides the interface level setting */
        if(((pCfg->passiveMode == L7_OSPF_INTF_PASSIVE) && (defaultPassiveMode == L7_FALSE)) ||
           ((pCfg->passiveMode == L7_OSPF_INTF_NOT_PASSIVE) && (defaultPassiveMode == L7_TRUE)))
        {
           pCfg->passiveMode = defaultPassiveMode;
           rc = ospfMapExtenIfPassiveModeSet(intIfNum, defaultPassiveMode);
        } 
     }
  }  
  return L7_SUCCESS;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  INTERFACE CONFIG
**********************************************************************
*/
/*********************************************************************
* @purpose  Determines whether all conditions have been met for
*           OSPF to be functional on a specific interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE if all conditions have been met
*           L7_FALSE otherwise
*
* @notes    The conditions checked are as follows:
*             a) OSPF is enabled
*             b) routing is enabled
*             c) OSPF is enabled on the interface
*             d) routing is enabled on the interface
*             e) an IP address has been configured on the interface
*             f) OSPF is initialized
*             g) the interface has not been acquired (e.g., for a LAG)
*
* @end
*********************************************************************/
L7_BOOL ospfMayEnableInterface(L7_uint32 intIfNum)
{
    L7_uint32 ospfAdminMode;
    L7_uint32 rtrIntfMode;
    L7_IP_ADDR_t ipAddr;
    L7_IP_ADDR_t ipMask;
    L7_uint32 ospfNetType;

    /* is OSPF enabled? */
    if ((ospfMapOspfAdminModeGet(&ospfAdminMode) != L7_SUCCESS) ||
        (ospfAdminMode != L7_ENABLE))
        return L7_FALSE;

    /* is routing enabled? */
    if (ipMapRtrAdminModeGet() != L7_ENABLE)
        return L7_FALSE;

    /* is OSPF enabled on the interface? */
    if ((ospfMapIntfEffectiveAdminModeGet(intIfNum, &ospfAdminMode) != L7_SUCCESS) ||
        (ospfAdminMode != L7_ENABLE))
        return L7_FALSE;

    /* Is there an IP address on the interface? */
    if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) != L7_SUCCESS)
    {
            return L7_FALSE;
    }
    if ((ipAddr == 0) || (ipMask == 0))
    {
      if (!ipMapIntfIsUnnumbered(intIfNum))
      {
        return L7_FALSE;
      }
      /* Interface is unnumbered. Make sure OSPF network type is p2p. */
      if ((ospfMapIntfTypeGet(intIfNum, &ospfNetType) != L7_SUCCESS) ||
          (ospfNetType != L7_OSPF_INTF_PTP))
      {
        return L7_FALSE;
      }
    }

     /* Is routing enabled on the interface */
    if ((ipMapRtrIntfOperModeGet(intIfNum, &rtrIntfMode) != L7_SUCCESS) || 
        (rtrIntfMode != L7_ENABLE))
    {
      return L7_FALSE;
    }

    /* Is OSPF initialized? */
    if (ospfMapOspfInitialized() != L7_TRUE)
      return L7_FALSE;

    /* check if interface has been acquired by a LAG or probe */
    if (pOspfIntfInfo[intIfNum].intfAcquired == L7_TRUE)
      return L7_FALSE;

    return L7_TRUE;
}

/*********************************************************************
* @purpose  Enables the OSPF routing function for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    Assumes
*           2. Interface has been created and configured for ospf at
*              the ospf vendor layer
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAdminModeEnable(L7_uint32 intIfNum)
{
  L7_uint32 activeState;
  L7_uint32 rtrIfNum, mode, ipAddr, ipMask;
  L7_RC_t rc = L7_FAILURE;

  static const char *routine_name = "ospfMapIntfAdminModeEnable()";

  OSPFMAP_TRACE("%s %d: %s : intIfNum = %d \n", __FILE__, __LINE__, routine_name,
                intIfNum);

  /* If OSPF enabled */
  if (ospfMapOspfAdminModeGet(&mode) == L7_SUCCESS &&
      mode == L7_ENABLE)
  {
    /* Verify if the interface is configured for routing */
    if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      /* If the specified interface does not exist try to create it */
      if (ospfMapOspfIntfExists(intIfNum) == L7_FALSE)
      {
        /* Get the associated ip address */
        if (ipMapIntfIsUnnumbered(intIfNum))
        {
          ipAddr = 0;
          ipMask = 0;
          rc = L7_SUCCESS;
        }
        else
        {
          rc = ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask);
        }
        if (rc == L7_SUCCESS)
        {
          if (ospfMapOspfIntfCreate(intIfNum, ipAddr, ipMask) == L7_SUCCESS)
          {
            pOspfIntfInfo[intIfNum].ospfInitialized = L7_TRUE;
          }
          else {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
            L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
                    "Failed to initialize OSPF on interface %s.", ifName);
            return L7_FAILURE;
          }
        }
      } 

      if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE)
      {
        if (ospfMapExtenIntfAdminModeSet( intIfNum, L7_ENABLE) == L7_SUCCESS)
        {
          if (ospfMapExtenIfStatusSet(intIfNum, L7_OSPF_ROW_CHANGE) != L7_SUCCESS)
          {
            OSPFMAP_ERROR("ospfMapOspfAdminModeEnable: Could not add any secondary IP addrs\n");
          }

          /* If OSPF is configured to redistribute local routes, suppress
           * redistribution of network on this interface, since OSPF will now
           * advertise the local route as an internal route.
           */
          if (ospfMapLocalRedistSuppress(intIfNum) != L7_SUCCESS) {
            OSPFMAP_ERROR("ospfMapIntfAdminModeEnable: Failed to suppress redistribution of local network\n");
          }

          if (nimGetIntfActiveState(intIfNum, &activeState) == L7_SUCCESS &&
              activeState == L7_ACTIVE)
          {
            if (ospfMapOspfIntfUp(intIfNum) == L7_SUCCESS)
              ospfApplyVirtualLinkConfigData();
          }
        }
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disables the OSPF routing function for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAdminModeDisable(L7_uint32 intIfNum)
{
  L7_uint32 ospfEnabled;
  static const char *routine_name = "ospfMapIntfAdminModeDisable()";

  OSPFMAP_TRACE("%s %d: %s : intIfNum = %d \n", __FILE__, __LINE__, routine_name,
              intIfNum);


  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  ospfMapOspfIntfDown(intIfNum);

  ospfMapOspfIntfDelete(intIfNum);

  /* When OSPF is disabled on an interface, we may need to begin
   * redistributing the local network. This must be done after the 
   * preceeding statements.
   */
  if ((ospfMapOspfAdminModeGet(&ospfEnabled) == L7_SUCCESS) &&
      (ospfEnabled == L7_ENABLE))
  {
      ospfMapExtenRedistReevaluate();
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ospf Area Id for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAreaIdApply(L7_uint32 intIfNum, L7_uint32 area)
{
  L7_uint32 index;

  static const char *routine_name = "ospfMapIntfAreaIdApply()";

  OSPFMAP_TRACE("%s %d: %s : intIfNum = %d , area = %x\n", __FILE__, __LINE__, routine_name,
              intIfNum, area);

  /* Per RFC 1850 recommendataion, automatically create the areas required
     for the interface */
  if(ospfMapAreaCreate(area, &index) == L7_SUCCESS)
  {
      /* If ospf is initialized, and the routing interface is initialized,
        then apply the area configuration to the underlying OSPF stack */
    if(ospfMapOspfInitialized() == L7_TRUE)
    {
      if (ospfMapOspfIntfInitialized(intIfNum) == L7_TRUE)
      {
        /* Now call the vendor to create the specified area */
        return ospfMapExtenIntfAreaIdPack(intIfNum, area);
      }
    }
  }

  /* We cannot set the area id unless ospf has been initialized,
     i.e. vendor stack has been initialized */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the SecondariesFlag for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    secondaries @b{(input)} secondariesFlag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfSecondariesFlagApply(L7_uint32 intIfNum, L7_uint32 secondaries)
{
  static const char *routine_name = "ospfMapIntfSecondariesFlagApply()";

  OSPFMAP_TRACE("%s %d: %s : intIfNum = %d , secondaries = %x\n", __FILE__, __LINE__, routine_name,
              intIfNum, secondaries);

  ospfMapIntfModeAndAreaCompute(intIfNum);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the OSPF interface type.
*
* @param    intIfNum    internal interface number
* @param    type        interface type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTypeApply(L7_uint32 intIfNum, L7_uint32 type)
{
  if (ospfMapOspfIntfExists(intIfNum) == L7_TRUE)
  {
    /* interface already up. If should remain enabled, notify 
     * protocol of interface type change. */
    if (ospfMayEnableInterface(intIfNum))
    {
      return ospfMapExtenIfTypeSet(intIfNum, type);
    }
    else
    {
      /* Interface type change takes interface down */
      return ospfMapIntfAdminModeDisable(intIfNum);
    }
  }
  else
  {
    /* Interface was down. See if type change brings interface up. */
    if (ospfMayEnableInterface(intIfNum))
    {
      return ospfMapIntfAdminModeEnable(intIfNum);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Ospf Priority for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    priority   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPriorityApply(L7_uint32 intIfNum, L7_uint32 priority)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  return ( ospfMapExtenIntfPrioritySet( intIfNum,  priority)  );
}

/*********************************************************************
* @purpose  Sets the default metric for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    metric value   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricApply(L7_uint32 intIfNum, L7_uint32 value)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  value = (value == 0) ? FD_OSPF_INTF_DEFAULT_METRIC : value;

  return ( ospfMapExtenIfMetricValueSet( intIfNum, L7_TOS_NORMAL_SERVICE, value));
}

/*********************************************************************
* @purpose  Sets the passive mode for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    passive mode @b{(input)} if the interface is passive
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfPassiveModeApply(L7_uint32 intIfNum, L7_BOOL value)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  return ospfMapExtenIfPassiveModeSet(intIfNum, value);
}

/*********************************************************************
* @purpose  Sets the Ospf Hello Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfHelloIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  return (ospfMapExtenIntfHelloIntervalSet( intIfNum,  seconds) );
}

/*********************************************************************
* @purpose  Sets the Ospf Dead Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDeadIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  return (ospfMapExtenIntfDeadIntervalSet( intIfNum,  seconds) );
}

/*********************************************************************
* @purpose  Sets the Ospf Retransmit Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfRxmtIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
      return L7_SUCCESS;

  return ( ospfMapExtenIntfRxmtIntervalSet( intIfNum,  seconds) );
}

/*********************************************************************
* @purpose  Sets the Ospf Nbma Poll Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfNbmaPollIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  return (ospfMapExtenIntfNbmaPollIntervalSet( intIfNum,  seconds));
}

/*********************************************************************
* @purpose  Sets the Ospf Transit Delay for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfTransitDelayApply(L7_uint32 intIfNum, L7_uint32 seconds)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  return ( ospfMapExtenIntfTransitDelaySet( intIfNum,  seconds));
}



/*********************************************************************
* @purpose  Sets the LSA Ack Interval for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    seconds   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist or
*                     if interface has not been enabled for routing
* @returns  L7_FAILURE
*
* @notes    " The length of time, in seconds, between LSA Acknowledgement
*             packet transmissions. The value must be less than
*             retransmit interval."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfLsaAckIntervalApply(L7_uint32 intIfNum, L7_uint32 seconds)
{
  ospfCfgCkt_t *pCfg;

  if (ospfMapMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    pCfg->lsaAckInterval = seconds;
    pOspfMapCfgData->cfgHdr.dataChanged = L7_TRUE;
    return( ospfMapExtenIntfLsaAckIntervalSet( intIfNum,  seconds) );
  }

  return L7_ERROR;

}


/*********************************************************************
* @purpose  Sets the Ospf Authentication Type for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    authType   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    The auth type must be applied BEFORE the auth key in order
*           for the key to be interpreted properly.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthTypeApply(L7_uint32 intIfNum, L7_uint32 authType)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  return ( ospfMapExtenIntfAuthTypeSet( intIfNum,  authType)  );
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key for the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    key         @b{(input)} authentication key
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    The auth key must be applied AFTER the auth type in order
*           for the key to be interpreted properly.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthKeyApply(L7_uint32 intIfNum, L7_uchar8 *key)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  return (ospfMapExtenIntfAuthKeySet( intIfNum, key ));
}

/*********************************************************************
* @purpose  Sets the Ospf Authentication Key Id for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    keyId    @b{(input)} key identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfAuthKeyIdApply(L7_uint32 intIfNum, L7_uint32 keyId)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  return ( ospfMapExtenIntfAuthKeyIdSet( intIfNum, keyId) );
}



/*********************************************************************
* @purpose  Set the ospf demand value
*
* @param    intIfNum    Internal Interface Number
* @param    val         pass L7_TRUE or L7_FALSE as truth value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " Indicates whether Demand OSPF procedures (hello
*             supression to FULL neighbors and setting the DoNotAge
*             flag on propogated LSAs) should be performed on
*             this interface."
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfDemandApply(L7_int32 intIfNum, L7_int32 val )
{
  if (ospfMapOspfInitialized() == L7_TRUE)
    return ospfMapExtenIntfDemandAgingSet(intIfNum,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the Ospf Virtual Transit Area Id for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfVirtTransitAreaIdApply(L7_uint32 intIfNum, L7_uint32 area)
{
  L7_uint32 index;

  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  if(ospfMapAreaIndexGet(area, &index) != L7_SUCCESS)
  {
    /* If we got here we did not find the specified area id
       Find a free index to store the information associated
       with the new area */
    if(ospfMapAreaCreate(area, &index) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  return ( ospfMapExtenIntfVirtTransitAreaIdSet( intIfNum,  area)  );

}

/*********************************************************************
* @purpose  Sets the Ospf Virtual Neighbor for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    neighbor   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfVirtIntfNeighborApply(L7_uint32 intIfNum, L7_uint32 areaId,
                                         L7_uint32 neighbor)
{
  if (ospfMapOspfIntfExists(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  /* We don't need to call the vendor code here.  These changes only get
     applied when the interface is specifically configured as a virtual
     interface */
  /*return (ospfMapExtenIntfVirtIntfNeighborSet( intIfNum, areaId, neighbor) );*/

  return L7_SUCCESS;
}

/*
**********************************************************************
*                    API FUNCTIONS  -  AREA CONFIG
**********************************************************************
*/

/*********************************************************************
* @purpose  Delete the area summary.
*
* @param    AreaId      type
* @param    val         variable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    If it is noAreaSummary, router will neither originate nor propagate
*           summary LSAs into stub area. It will rely on its default route.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaSummaryApply(L7_uint32 areaId, L7_int32 val)
{
  L7_uint32 index;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    {
      /* If we got here we did not find the specified area id
         Find a free index to store the information associated
         with the new area */
      if(ospfMapAreaCreate(areaId, &index) != L7_SUCCESS)
        return L7_FAILURE;
    }

    if (ospfMapExtenAreaExists(areaId) == L7_SUCCESS)
    {
       return ospfMapExtenAreaSummarySet(areaId, val);
    }
  }

  /* Return success if OSPF has not been initialized.
     This allows for dynamic configurability of OSPF. */
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Apply the Area's external routing capability
*
* @param    AreaId      type
* @param    val         user configuration     (L7_OSPF_AREA_EXT_ROUTING_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_ERROR    if invalid val
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaImportAsExternApply(L7_uint32 areaId, L7_int32 prevConf,
                                       L7_int32 val)
{
  L7_uint32 rtrIntf, intIfNum, intfArea;

  /* Return success if OSPF has not been initialized.
  **  This allows for dynamic configurability of OSPF
  **                  OR
  ** If the area has not been instantiated in the vendor code, exit
  ** successfully.  This configuration change will be applied when
  ** the area gets created.
  */
  if((ospfMapOspfInitialized() != L7_TRUE) ||
     (ospfMapExtenAreaExists(areaId) != L7_SUCCESS))
     return L7_SUCCESS;

  switch(val)
  {
  case L7_OSPF_AREA_IMPORT_EXT:
    if(prevConf == L7_OSPF_AREA_IMPORT_NO_EXT)
      ospfMapStubStatusSet(areaId, L7_TOS_NORMAL_SERVICE, L7_OSPF_ROW_DESTROY);
    else if(prevConf == L7_OSPF_AREA_IMPORT_NSSA)
      ospfMapExtenNSSAStatusSet(areaId, L7_OSPF_ROW_DESTROY);
    break;

  case L7_OSPF_AREA_IMPORT_NO_EXT:
    /* We can only configure a regular area to a stub */
    if(prevConf == L7_OSPF_AREA_IMPORT_EXT)
      ospfMapAreaIsStubAreaApply(areaId);
    break;

  case L7_OSPF_AREA_IMPORT_NSSA:
    /* We can only configure a regular area to a NSSA */
    if(prevConf == L7_OSPF_AREA_IMPORT_EXT)
      ospfMapExtenNSSAStatusSet(areaId, L7_OSPF_ROW_CREATE_AND_GO);
    break;

  default:
  /* Invalid type */
    return L7_FAILURE;
    break;
  }

  /* We could not set the parm in the vendor code but since it has
   * been updated in the LVL7 configuration this will be applied later
   */
  if(ospfMapExtenImportAsExternSet(areaId, val) != L7_SUCCESS)
    return L7_SUCCESS;

  /* Toggle interface down & up modes for all the interfaces in this
     area to force adjacencies. */
  for(rtrIntf = 1; rtrIntf < L7_RTR_MAX_RTR_INTERFACES; rtrIntf++)
  {
    if(ipMapRtrIntfToIntIfNum(rtrIntf, &intIfNum) == L7_SUCCESS &&
       ospfMapOspfIntfInitialized(intIfNum) == L7_TRUE)
    {
      if(ospfMapIntfEffectiveAreaIdGet(intIfNum, &intfArea) == L7_SUCCESS &&
         intfArea == areaId)
      {
        ospfMapOspfIntfDown(intIfNum);
        ospfMapOspfIntfUp(intIfNum);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the entry status.
*
* @param    areaId      type
* @param    TOS         entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This variable displays the status of the entry.  Setting
*             it to 'invalid' has the effect of rendering it inoperable
*             The internal effect (row removal) is implementation
*             dependent."
*
* @end
*********************************************************************/
static L7_RC_t ospfMapStubStatusSet ( L7_uint32 areaId, L7_int32 TOS,
                                      L7_int32 val)
{
  L7_RC_t rc;

  if (ospfMapAreaEntryExists(areaId)  == L7_SUCCESS)
  {
    if (ospfMapTosIsValid(TOS) == L7_TRUE)
    {
      rc = ospfMapExtenStubAreaStatusSet(areaId,TOS,val);
      return rc;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Configures if the specified Area is a stub area
*
* @param    areaID  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if area has not been configured
* @returns  L7_FAILURE
*
* @notes    Area must have been created prior to this call
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaIsStubAreaApply(L7_uint32 areaId)
{
  L7_RC_t   rc;
  L7_uint32 index;

  rc = L7_SUCCESS;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    if(ospfMapAreaIndexGet(areaId, &index) != L7_SUCCESS)
    {
      /* Area should have already been created */
        return L7_FAILURE;
    }

    return ospfMapExtenStubAreaMetricCfgSet(areaId, L7_TOS_NORMAL_SERVICE,
                                            pOspfMapCfgData->area[index].stubCfg[L7_TOS_NORMAL_SERVICE].stubMetric,
                                            pOspfMapCfgData->area[index].stubCfg[L7_TOS_NORMAL_SERVICE].stubMetricType);
  }

  /* Return success if OSPF has not been initialized (i.e. enabled).
   This allows for dynamic configurability of OSPF. */
  return rc;
}



/*********************************************************************
* @purpose  Set the stub metric configuration
*
* @param    areaId      areaID
* @param    TOS         TOS         (L7_TOSTYPES)
* @param    metric      metric
* @param    metricType  metric type (L7_OSPF_STUB_METRIC_TYPE_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Assumes parms passed have already been validity checked
*
* @end
*********************************************************************/
L7_RC_t ospfMapStubMetricCfgApply ( L7_uint32 areaId, L7_uint32 TOS,
                                    L7_uint32 metric, L7_uint32 metricType )
{
    L7_uint32 areaIndex;

    if((ospfMapAreaIndexGet(areaId, &areaIndex) == L7_SUCCESS))
    {
      if (ospfMapOspfInitialized() == L7_TRUE)
        return ospfMapExtenStubAreaMetricCfgSet(areaId, TOS,metric, metricType);

      return L7_SUCCESS;
    }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Process the cleaning up of an area from the OSPF stack
*
* @param    area   @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaFreeApply( L7_uint32 area)
{
  L7_uint32 i;


  static const char *routine_name = "ospfMapAreaFreeApply()";

  OSPFMAP_TRACE("%s %d: %s :  area = %x\n", __FILE__, __LINE__, routine_name,
               area);


  if(ospfMapOspfInitialized() == L7_TRUE)
  {
      /* Only apply the configuration if the area exists in core OSPF */
      if (ospfMapExtenAreaExists(area) == L7_SUCCESS)
      {
          /*-------------------------------------------*/
          /* Clean up area range configuration, if any */
          /*-------------------------------------------*/
          for (i=0; i < L7_OSPF_MAX_AREA_RANGES; i++)
          {
              if (pOspfMapCfgData->areaRange[i].inUse == L7_TRUE)
              {
                 if (pOspfMapCfgData->areaRange[i].area == area)
                 {
                   ospfMapAreaAggregateRangeDeleteApply (pOspfMapCfgData->areaRange[i].area,
                                                         pOspfMapCfgData->areaRange[i].lsdbType,
                                                         pOspfMapCfgData->areaRange[i].ipAddr,
                                                         pOspfMapCfgData->areaRange[i].subnetMask);
                 }
              }
          }  /*    for (i=0; i < L7_OSPF_MAX_AREA_RANGES; i++) */



          /*-------------------------------------------------------*/
          /* There is no need to clean up area stub  configuration  */
          /*-------------------------------------------------------*/


          /*-------------------------------------------*/
          /* Free the area in the underlying stack     */
          /*-------------------------------------------*/
          ospfMapExtenAreaFree(area);

      }   /* ospfMapExtenAreaExists */
  }  /*  ospfMapOspfInitialized */


  /* Return success if OSPF has not been initialized.
   This allows for dynamic configurability of OSPF. */
  return L7_SUCCESS;

}



/*********************************************************************
* @purpose  Create an area range entry.
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
* @param    advertise   advertise aggregate area (L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if area id does not exist in configuration
* @returns  L7_ERROR    if core OSPF stack fails to apply change
*
* @notes    " A range of IP Addresses specified by an IP Address/IP
*             network mask pair.  For example, calss B address range
*             of X.X.X.X with a network mask of 255.255.0.0 includes
*             all IP addresses from X.X.0.0 to X.X.255.255."
*
*             obsolete
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateRangeApply ( L7_uint32 areaId, L7_uint32 lsdbType,
                                L7_uint32 ipAddr, L7_uint32 netMask,
                                L7_uint32 advertise )
{
  L7_ospfAreaAddrRangeInfo_t areaAddrRange;

  if(ospfMapOspfInitialized() == L7_TRUE)
  {
    memset(&areaAddrRange, 0, sizeof(areaAddrRange));
    areaAddrRange.descr.areaId        = areaId;
    areaAddrRange.descr.lsdbType      = lsdbType;
    areaAddrRange.descr.ipAddr        = ipAddr;
    areaAddrRange.descr.netMask       = netMask;
    areaAddrRange.advertiseMode       = advertise;

    return(ospfMapExtenAreaRangeSet( areaId,  &areaAddrRange));
  }

  /* Return success if OSPF has not been initialized.
   This allows for dynamic configurability of OSPF. */
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Delete an area range entry.
*
* @param    areaId      type
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A range of IP Addresses specified by an IP Address/IP
*             network mask pair.  For example, calss B address range
*             of X.X.X.X with a network mask of 255.255.0.0 includes
*             all IP addresses from X.X.0.0 to X.X.255.255."
*
*             obsolete
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateRangeDeleteApply ( L7_uint32 areaId, L7_uint32 lsdbType,
                                      L7_uint32 ipAddr, L7_uint32 netMask )
{

    L7_ospfAreaAddrRangeInfo_t areaAddrRange;

    if(ospfMapOspfInitialized() == L7_TRUE)
    {
      memset(&areaAddrRange, 0, sizeof(areaAddrRange));
      areaAddrRange.descr.areaId        = areaId;
      areaAddrRange.descr.lsdbType      = lsdbType;
      areaAddrRange.descr.ipAddr        = ipAddr;
      areaAddrRange.descr.netMask       = netMask;

      /* Only apply the configuration if the area exists in core OSPF */
      if (ospfMapExtenAreaExists(areaId) == L7_SUCCESS)
      {
          return ospfMapExtenAreaRangeDelete(areaId, &areaAddrRange);
      }
    }

    /* Return success if OSPF has not been initialized.
     This allows for dynamic configurability of OSPF. */
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the effective admin mode on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    adminMode  @b{(output)} effective admin mode
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAdminModeGet(L7_uint32 intIfNum,
                                         L7_uint32 *adminMode)
{
    *adminMode = pOspfIntfInfo[intIfNum].adminMode;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the effective admin mode on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    adminMode  @b{(input)} effective admin mode
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAdminModeSet(L7_uint32 intIfNum,
                                         L7_uint32 adminMode)
{
    pOspfIntfInfo[intIfNum].adminMode = adminMode;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the effective areaId on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    areaId     @b{(output)} effective areaId
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAreaIdGet(L7_uint32 intIfNum,
                                      L7_uint32 *areaId)
{
    *areaId = pOspfIntfInfo[intIfNum].areaId;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the effective areaId on the interface
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    areaId     @b{(input)} effective areaId
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveAreaIdSet(L7_uint32 intIfNum,
                                      L7_uint32 areaId)
{
    pOspfIntfInfo[intIfNum].areaId = areaId;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the effective secondaries flag on the interface
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    secondariesFlag @b{(output)} effective secondaries flag
*                                        that indicates what all secondary
*                                        addresses are advertised
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveSecondariesFlagGet(L7_uint32 intIfNum,
                                               L7_uint32 *secondariesFlag)
{
    *secondariesFlag = pOspfIntfInfo[intIfNum].secondariesFlag;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the effective secondaries flag on the interface
*
* @param    intIfNum        @b{(input)} internal interface number
* @param    secondariesFlag @b{(input)} effective secondaries flag
*                                       that indicates what all secondary
*                                       addresses are advertised
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfEffectiveSecondariesFlagSet(L7_uint32 intIfNum,
                                               L7_uint32 secondariesFlag)
{
    pOspfIntfInfo[intIfNum].secondariesFlag = secondariesFlag;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find a specific network area object.
*
* @param  ipAddr        @b{(input)} network configured for ospf
* @param  wildcardMask  @b{(input)} wildcard mask
* @param  areaId        @b{(input)} areaid for this network
*
* @returns  L7_SUCCESS  if the specified network area object is found
* @returns  L7_FAILURE  otherwise
*
* @notes    
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaGet (L7_uint32 ipAddr, L7_uint32 wildcardMask,
                               L7_int32 areaId)
{
  ospfNetworkAreaInfo_t *p_NetworkArea = networkAreaListHead_g;

  while(p_NetworkArea != L7_NULLPTR)
  {
    if((p_NetworkArea->ipAddr == ipAddr) &&
       (p_NetworkArea->wildcardMask == wildcardMask) &&
       (p_NetworkArea->areaId == areaId))
      return L7_SUCCESS;

    p_NetworkArea = p_NetworkArea->next;
  }
  return L7_FAILURE;
}

/***************************************************************************
* @purpose  Get next network area configured for OSPF. 
*
* @param  ipAddr     @b{(input/output)} network of the current object
* @param  wildcardMask @b{(input/output)} wildcardMask of the current object
* @param  areaId     @b{(input/output)}  areaId of the current object
*
* @returns  L7_SUCCESS  if a next network area object exists
* @returns  L7_FAILURE  if no next object exists
* @returns  L7_ERROR if input parameters are invalid or previous network
*                    area object is not found
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaGetNext(L7_uint32 *ipAddr, L7_uint32 *wildcardMask, 
                                  L7_int32 *areaId)
{
  ospfNetworkAreaInfo_t *p_NetworkArea = networkAreaListHead_g;

  if((*ipAddr == 0) && (*wildcardMask == 0) && (*areaId == 0))
  {
    /* This is the query for the first network area object */
    if(p_NetworkArea != L7_NULLPTR)
    {
      *ipAddr       = p_NetworkArea->ipAddr;
      *wildcardMask = p_NetworkArea->wildcardMask;
      *areaId       = p_NetworkArea->areaId;
      return L7_SUCCESS;
    }
    return L7_ERROR;
  }

  while(p_NetworkArea != L7_NULLPTR)
  {
    if((p_NetworkArea->ipAddr == *ipAddr) &&
       (p_NetworkArea->wildcardMask == *wildcardMask) &&
       (p_NetworkArea->areaId == *areaId))
    {
      if(p_NetworkArea->next != L7_NULLPTR)
      {
        *ipAddr       = p_NetworkArea->next->ipAddr;
        *wildcardMask = p_NetworkArea->next->wildcardMask;
        *areaId       = p_NetworkArea->next->areaId;
        return L7_SUCCESS;
      }
      else
      {
        /* The next network area object doesn't exist */
        return L7_FAILURE;
      }
    }

    p_NetworkArea = p_NetworkArea->next;
  }

  /* The previous network area object doesn't exist */
  return L7_ERROR;
}

/*********************************************************************
* @purpose  Create/Update a network area entry.
*
* @param    ipAddr        @b{(input)} ip address
* @param    wildcardMask  @b{(input)} wild card mask
* @param    areaId        @b{(input)} area associated with the address
*
* @returns  L7_ALREADY_CONFIGURED if entry already exists in the list
* @returns  L7_SUCCESS  if entry is successfully added to the list
* @returns  L7_FAILURE  if entry can't be added to the list
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaUpdate (L7_uint32 ipAddr,
                                  L7_uint32 wildcardMask,
                                  L7_uint32 areaId)
{
  ospfNetworkAreaInfo_t *p_NetworkArea, *p_PrevNetworkArea = L7_NULLPTR;

  p_NetworkArea = networkAreaListHead_g;

  while(p_NetworkArea != L7_NULLPTR)
  {
    if((p_NetworkArea->ipAddr != ipAddr) ||
       (p_NetworkArea->wildcardMask != wildcardMask))
    {
      p_PrevNetworkArea = p_NetworkArea;
      p_NetworkArea = p_NetworkArea->next;
    }
    else
    {
      if(p_NetworkArea->areaId == areaId)
        return L7_ALREADY_CONFIGURED; /* no change in the entry */

      p_NetworkArea->areaId = areaId; /* change in areaid */
      return L7_SUCCESS;
    }
  }

  if(p_NetworkArea == L7_NULLPTR)
  {
    if(networkAreaListFree_g == L7_NULLPTR)
    {
      /* There are no entries on the freelist */
      return L7_FAILURE;
    }
    else
    {
      /* there were no entries on the available list */
      if(p_PrevNetworkArea == L7_NULLPTR)
        p_NetworkArea = networkAreaListHead_g = networkAreaListFree_g;
      else
        p_NetworkArea = p_PrevNetworkArea->next = networkAreaListFree_g;

      /* update the free list header */
      networkAreaListFree_g = networkAreaListFree_g->next;

      /* There is a free entry available */
      p_NetworkArea->ipAddr = ipAddr;
      p_NetworkArea->wildcardMask = wildcardMask;
      p_NetworkArea->areaId = areaId;
      p_NetworkArea->next = L7_NULLPTR;

      return L7_SUCCESS;
    }
  }

  /* shouldn't reach here */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete a network area entry.
*
* @param    ipAddr        @b{(input)} ip address
* @param    wildcardMask  @b{(input)} wild card mask
* @param    areaId        @b{(input)} area associated with the address
*
* @returns  L7_SUCCESS  if entry is successfully deleted from the list
* @returns  L7_FAILURE  if entry doesn't exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ospfMapNetworkAreaDelete (L7_uint32 ipAddr,
                                  L7_uint32 wildcardMask,
                                  L7_uint32 areaId)
{
  ospfNetworkAreaInfo_t *p_NetworkArea, *p_PrevNetworkArea = L7_NULLPTR;

  p_NetworkArea = networkAreaListHead_g;

  if(p_NetworkArea == L7_NULLPTR)
    return L7_FAILURE;

  while(p_NetworkArea != L7_NULLPTR)
  {
    if((p_NetworkArea->ipAddr == ipAddr) &&
       (p_NetworkArea->wildcardMask == wildcardMask) &&
       (p_NetworkArea->areaId == areaId))
    {
      if(p_PrevNetworkArea)
        p_PrevNetworkArea->next = p_NetworkArea->next;
      else
        networkAreaListHead_g = p_NetworkArea->next;

      if(networkAreaListFree_g)
      {
        p_NetworkArea->next = networkAreaListFree_g;
        networkAreaListFree_g = p_NetworkArea;
      }
      else
      {
        networkAreaListFree_g = p_NetworkArea;
        networkAreaListFree_g->next = L7_NULLPTR;
      }
      return L7_SUCCESS;
    }
    p_PrevNetworkArea = p_NetworkArea;
    p_NetworkArea = p_NetworkArea->next;
  }

  /* The entry was not found in the list */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the area corresponding to the matching "network area"
*           command for an interface
*
* @param    ipAddr        @b{(input)} ip address
* @param    matchingArea  @b{(output)} area associated with the address
* @param    matchFound    @b{(output)} flag indicating if matching area is found
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void ospfMapMatchingNetworkFind(L7_uint32 ipAddr, L7_uint32 *matchingArea,
                                L7_BOOL *matchFound)
{
  L7_int32 matchingNetwork = 0;
  L7_uint32 matchingWildcardMask = 0;
  ospfNetworkAreaInfo_t *p_NetworkArea = networkAreaListHead_g;

  *matchFound = L7_FALSE;

  while(p_NetworkArea != L7_NULLPTR)
  {
    /* interface address is matched by "network" */
    if((p_NetworkArea->ipAddr & ~(p_NetworkArea->wildcardMask)) ==
       (ipAddr & ~(p_NetworkArea->wildcardMask)))
    {
      /* first area in the list for the matching "network" command (or)
       * if a more specific matching "network" for the 
       * first matching area is found, take it */
      if((*matchFound == L7_FALSE) ||
         ((matchingNetwork & ~matchingWildcardMask) <
          (p_NetworkArea->ipAddr & ~(p_NetworkArea->wildcardMask))))
      {
        *matchingArea         = p_NetworkArea->areaId;
        matchingNetwork       = p_NetworkArea->ipAddr;
        matchingWildcardMask  = p_NetworkArea->wildcardMask; 
        *matchFound           = L7_TRUE;
      }
    }
    p_NetworkArea = p_NetworkArea->next;
  }
}

/*********************************************************************
* @purpose  Compute the bitset for the advertisability of the
*           secondary addresses on the interface under the influence
*           of "network area" commands covering this interface
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    matchingArea  @b{(output)} area associated with the address
* @param    secondaries   @b{(output)} bitset for the secondaries
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void ospfMapIntfSecondariesFlagCompute(L7_uint32 intIfNum,
                                       L7_uint32 matchingArea,
                                       L7_uint32 *secondaries)
{
  L7_uint32 i;
  L7_uint32 area;
  L7_rtrIntfIpAddr_t addrs[L7_L3_NUM_IP_ADDRS];
  L7_BOOL   matchFound = L7_FALSE;
  *secondaries = 0;

  /* matching Area is the area the primary address on this interface
   * is enabled in.
   * we need to find the list of secondary address that are covered
   * by any "network area" commands in the same matchingArea
   */
  if (ipMapRtrIntfIpAddrListGet(intIfNum, addrs) == L7_SUCCESS)
  {
    for (i = 1; i < L7_L3_NUM_IP_ADDRS; i++)
    {
      if (addrs[i].ipAddr != 0)
      {
        ospfMapMatchingNetworkFind(addrs[i].ipAddr, &area, &matchFound);
        if((matchFound == L7_TRUE) && (area == matchingArea))
        {
          /* secondary address on this interface is in the same
           * ospf area as the primary address
           */
          *secondaries |= (1<<i);
        }
      }
    }
  }
}

/*********************************************************************
* @purpose  For a given interface, determine whether OSPF is enabled, 
*           and if so, what area it is in. Also determine whether 
*           OSPF advertises reachability to secondary networks on 
*           the interface.
*
* @param    intIfNum  @b{(input)} internal interface number
*                                 if 0, run the algorithm on all interfaces
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    "ip ospf area" takes precedence over "network area" commands.
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfModeAndAreaCompute(L7_uint32 intIfNum)
{
  L7_uint32 i;

  static const char *routine_name = "ospfMapIntfModeAndAreaCompute()";
  OSPFMAP_TRACE("%s %d: %s : \n", __FILE__, __LINE__, routine_name);

  if (intIfNum)
  {
    /* single interface */
    return _ospfMapIntfModeAndAreaCompute(intIfNum);
  }
  else
  {
    /* all routing interfaces */
    for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++)
    {
      if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
      {
        _ospfMapIntfModeAndAreaCompute(intIfNum);
      }
    }
  }
      
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Implementation function for ospfMapIntfModeAndAreaCompute().
*
* @param    intIfNum  @b{(input)} internal interface number
*                                 Must be non-zero
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t _ospfMapIntfModeAndAreaCompute(L7_uint32 intIfNum)
{
  L7_uint32 prevAdminMode, cfgAdminMode;
  L7_uint32 secondaries, ipAddr, ipMask;
  L7_uint32 areaId;
  L7_BOOL   matchFound = L7_FALSE;

  if (intIfNum == 0)
    return L7_FAILURE;
      
  /* Get the previous effective admin mode */
  ospfMapIntfEffectiveAdminModeGet(intIfNum, &prevAdminMode);

  if (ospfMapIntfAdminModeGet(intIfNum, &cfgAdminMode) != L7_SUCCESS)
    return L7_FAILURE;

  if (cfgAdminMode == L7_ENABLE)
  {
    /* intf admin mode set with "ip ospf area" command */
    ospfMapIntfAreaIdGet(intIfNum, &areaId);
    ospfMapIntfSecondariesFlagGet(intIfNum, &secondaries);
    return ospfMapIntfAreaAssign(intIfNum, areaId, secondaries);
  }
  
  /* no "ip ospf area" command for this interface. Compare primary
   * IP address to "network" commands. */
  if ((ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &ipMask) == L7_SUCCESS) && 
      ((ipAddr != 0) && (ipMask != 0)))
  {
    /* Can only compare "network" command to numbered interfaces. */
    ospfMapMatchingNetworkFind(ipAddr, &areaId, &matchFound);
  }

  if (matchFound == L7_TRUE)
  {
    ospfMapIntfSecondariesFlagCompute(intIfNum, areaId, &secondaries);
    return ospfMapIntfAreaAssign(intIfNum, areaId, secondaries);
  }

  /* "ip ospf area" is not configured on the interface and
   * there is no matching "network area", hence disable ospf
   * on the interface */
  if (prevAdminMode == L7_ENABLE)
  {
    ospfMapIntfEffectiveAdminModeSet(intIfNum, L7_DISABLE);
    ospfMapIntfAdminModeDisable(intIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the effective admin mode, area ID, and secondaries 
*           advertisement for a given interface.
*
* @param  intIfNum       @b{(input)} internal interface number
*                                    Must be non-zero
* @param  areaId         @b{(input)} area interface is now in
* @param  advSecondaries @b{(input)} L7_TRUE if OSPF advertises secondary
*                                    subnets on this interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t ospfMapIntfAreaAssign(L7_uint32 intIfNum,  
                                     L7_uint32 areaId, L7_BOOL advSecondaries)
{
  L7_uint32 prevAdminMode;
  L7_uint32 prevSecondaries;
  L7_uint32 prevAreaId;

  if (intIfNum == 0)
    return L7_FAILURE;
      
  /* Get the previous effective (i) adminMode (ii) areaId (iii) secondariesFlag.
   * All of them could have been effective either due to
   * "ip ospf area" or "network area" commands
   */
  ospfMapIntfEffectiveAdminModeGet(intIfNum, &prevAdminMode);
  ospfMapIntfEffectiveAreaIdGet(intIfNum, &prevAreaId);
  ospfMapIntfEffectiveSecondariesFlagGet(intIfNum, &prevSecondaries);

  /* Area ID*/
  if (areaId != prevAreaId)
  {
    ospfMapIntfEffectiveAreaIdSet(intIfNum, areaId);
    ospfMapIntfAreaIdApply(intIfNum, areaId);
  }

  /* Secondaries */
  if (advSecondaries != prevSecondaries)
  {
    ospfMapIntfEffectiveSecondariesFlagSet(intIfNum, advSecondaries);
    ospfMapExtenIfStatusSet(intIfNum, L7_OSPF_ROW_CHANGE);
  }

  /* Admin mode */
  if (prevAdminMode == L7_DISABLE)
  {
    ospfMapIntfEffectiveAdminModeSet(intIfNum, L7_ENABLE);

    /* If conditions are met to enable ospf on the interface */
    if (ospfMayEnableInterface(intIfNum) == L7_TRUE)
      ospfMapIntfAdminModeEnable(intIfNum);
  }
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the "network area" commands operational data
*           from the "network area" commands stored in the config
*
* @param    none
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void ospfMapNetworkAreaConfigRead(void)
{
  L7_uint32 i;
  ospfNetworkAreaInfo_t *p_NetworkArea, *p_PrevNetworkArea = L7_NULLPTR;

  p_NetworkArea = pOspfNetworkAreaInfo;

  networkAreaListHead_g = L7_NULLPTR;

  /* Initialize the "network area" operational data 
   * from the configuration data
   */
  for (i = 0; i < NUM_NETWORK_AREA_CMDS; i++)
  {
    p_NetworkArea[i].ipAddr       = pOspfMapCfgData->networkArea[i].ipAddress;
    p_NetworkArea[i].wildcardMask = pOspfMapCfgData->networkArea[i].wildcardMask;
    p_NetworkArea[i].areaId       = pOspfMapCfgData->networkArea[i].areaId;

    if(i < NUM_NETWORK_AREA_CMDS-1)
      p_NetworkArea[i].next         = &p_NetworkArea[i+1];
  }
  p_NetworkArea[NUM_NETWORK_AREA_CMDS-1].next = L7_NULLPTR;

  /* Initialize the FreeList and Non-free list headers */
  p_NetworkArea = pOspfNetworkAreaInfo;
  
  if(p_NetworkArea->ipAddr != 0)
    networkAreaListHead_g = p_NetworkArea;

  do
  {
    if(p_NetworkArea->ipAddr != 0)
    {
      p_PrevNetworkArea = p_NetworkArea;
      p_NetworkArea = p_NetworkArea->next;
    }
    else
    {
      break;
    }
  }
  while(p_NetworkArea != L7_NULLPTR);

  if(p_PrevNetworkArea)
    p_PrevNetworkArea->next = L7_NULLPTR;
  networkAreaListFree_g = p_NetworkArea;
}

/*********************************************************************
* @purpose  Determine if the interface exists to the OSPF component
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ospfMapOspfIntfExists(L7_uint32 intIfNum)
{
   if (ospfMapOspfInitialized() == L7_TRUE)
      return ospfMapOspfIntfInitialized(intIfNum);

   return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the OSPF component has been initialized
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ospfMapOspfInitialized(void)
{
  if (pOspfInfo == L7_NULL)
  {
    /* OSPF component was not initialized during system initialization */
    LOG_MSG("WARNING:  Invalid build - OSPF component was not initialized\n");
    L7_assert(1);
    return L7_FALSE;
  }

  return (pOspfInfo->ospfInitialized);
}

/*********************************************************************
* @purpose  Determine if OSPF is registered with RTO for best route changes.
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ospfIsRegisteredWithRto(void)
{
  if (pOspfInfo == L7_NULL)
  {
    return L7_FALSE;
  }

  return (pOspfInfo->registeredWithRto);
}

/*********************************************************************
* @purpose  Determine whether OSPF originates a default route.
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ospfMapOriginatingDefaultRoute(void)
{
  if (pOspfInfo == L7_NULL)
  {
    /* OSPF component was not initialized during system initialization */
    LOG_MSG("WARNING:  Invalid build - OSPF component was not initialized\n");
    L7_assert(1);
    return L7_FALSE;
  }

  return (pOspfInfo->originatingDefaultRoute);
}

/*********************************************************************
* @purpose  Determine if the OSPF interface has been initialized
*
* @param    void
*
* @returns  L7_TRUE: ospf initialized on the interface
* @returns  L7_FALSE: ospf is not initialized on the interface.
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ospfMapOspfIntfInitialized(L7_uint32 intIfNum)
{
  if(pOspfIntfInfo == L7_NULLPTR)
  {
    /* OSPF component was not initialized during system initialization */
    LOG_MSG("WARNING:  Invalid build - OSPF component was not initialized\n");
    L7_assert(1);
    return L7_FALSE;
  }

  return (pOspfIntfInfo[intIfNum].ospfInitialized);
}

/*********************************************************************
* @purpose  Calculates the interface metric based on the interface speed.
*
* @param    intIfNum   internal interface number
* @param    ifMetric*  storage to return the calculated metric.
*
* @returns  L7_SUCCESS
*
* @notes    This function calculates the metric based on the speed of the
*           link as received from the network interface manager.  In the
*           event that NIM returns a failure a speed of 100full is
*           assumed.
*
*           This function may be used to calculate the metric ONLY IF
*           there is no user configured value for the metric.
*           i.e pOspfMapCfgData->ckt[usp.slot][usp.port].metric ==
*                             L7_OSPF_INTF_METRIC_NOT_CONFIGURED
*
* @end
*********************************************************************/
L7_RC_t ospfMapIntfMetricCalculate(L7_uint32 intIfNum, L7_uint32 *ifMetric)
{
  L7_RC_t rc;
  L7_uint32 intfBandwidth;

  rc=ipMapIntfBandwidthGet(intIfNum, &intfBandwidth);
  
  if(rc == L7_SUCCESS)
  {  
    if(intfBandwidth >= pOspfMapCfgData->rtr.autoCostRefBw*1000)
    {
      /* Default metric shall be 1 for all bandwidths > reference bandwidth */
      *ifMetric = 1;
    }
    else
    {
      *ifMetric = (pOspfMapCfgData->rtr.autoCostRefBw*1000)/intfBandwidth;
    }
    return L7_SUCCESS;
  }
   return L7_FAILURE;  
}

/*********************************************************************
* @purpose  Determines whether sourceProto is a valid source of routes
*           for OSPF to redistribute.
*
* @param    sourceProto - source of routes
*
* @returns  L7_TRUE if valid
* @returns  L7_FAILURE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ospfMapIsValidSourceProto(L7_REDIST_RT_INDICES_t sourceProto)
{
   if ((sourceProto == REDIST_RT_RIP) && (cnfgrIsComponentPresent(L7_RIP_MAP_COMPONENT_ID) == L7_TRUE))
   {
      return L7_TRUE;
   }
   else if ((sourceProto == REDIST_RT_BGP) && (cnfgrIsComponentPresent(L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE))
   {
      return L7_TRUE;
   }
   else if((sourceProto == REDIST_RT_LOCAL) ||
        (sourceProto == REDIST_RT_STATIC))
   {
      return L7_TRUE;
   }
   else
   {
     return L7_FALSE;
   }
}

/*********************************************************************
* @purpose  Get the redistribution configuration for a source of routes.
*
* @param    sourceProto - route source
*
* @returns  Pointer to the redistribution configuration, if one exists.
* @returns  NULL otherwise
*
* @notes
*
* @end
*********************************************************************/
ospfRedistCfg_t *ospfMapRedistCfgGet(L7_REDIST_RT_INDICES_t sourceProto)
{
    if (ospfMapIsValidSourceProto(sourceProto) == L7_FALSE) {
        return L7_NULL;
    }
    return &pOspfMapCfgData->redistCfg[sourceProto];
}

/*********************************************************************
* @purpose  Determine if prefix/mask is a subnetted network.
*
* @param    prefix - a network prefix
* @param    mask - associated network mask
*
* @returns  L7_TRUE if prefix/mask is a subnetted network
* @returns  L7_FAILURE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ospfMapIsSubnet(L7_uint32 prefix, L7_uint32 mask)
{
    if (IN_CLASSA(prefix)) {
        /* class A address */
        return (mask & IN_CLASSA_HOST) ? L7_TRUE : L7_FALSE;
    }
    if (IN_CLASSB(prefix)) {
        /* class B address */
        return (mask & IN_CLASSB_HOST) ? L7_TRUE : L7_FALSE;
    }
    if (IN_CLASSC(prefix)) {
        /* class C address */
        return (mask & IN_CLASSC_HOST) ? L7_TRUE : L7_FALSE;
    }
    /* OSPF shouldn't be trying to redistribute a Class D or Class E
    prefix.  */
    {
        L7_uchar8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        (void) osapiInetNtoa(prefix & mask, destAddrStr);
        LOG_MSG("ERROR:  OSPF attempt to redistribute invalid prefix %s\n",
                destAddrStr);
    }
    return L7_TRUE;
}

/*********************************************************************
* @purpose  Make sure the network on a given interface is not being
*           redistributed.
*
* @param    intIfNum - internal interface number of local interface
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if an error occurs
*
* @notes    This is called when OSPF is enabled on an interface. If
*           OSPF is configured to redistribute local (connected) routes,
*           we must stop redistributing the local network, since OSPF
*           will begin advertising it as an internal route.
*
* @end
*********************************************************************/
L7_RC_t ospfMapLocalRedistSuppress(L7_uint32 intIfNum)
{
    ospfRedistCfg_t *redistCfg;
	L7_rtrIntfIpAddr_t	addrs[L7_L3_NUM_IP_ADDRS];

    /* get the redistribution parameters for local routes */
    redistCfg = ospfMapRedistCfgGet(REDIST_RT_LOCAL);
    if (redistCfg == NULL) {
        return L7_FAILURE;
    }
    if (redistCfg->redistribute) {
        /* OSPF does redistribute local routes */
        if (ipMapRtrIntfIpAddrListGet(intIfNum, addrs) == L7_SUCCESS)
        {
            L7_uint32 j;
            for (j=0; j < L7_L3_NUM_IP_ADDRS; j++)
            {
                if (addrs[j].ipAddr != L7_NULL_IP_ADDR)
                {
                    if (ospfMapPreviouslyRedistributed((addrs[j].ipAddr & addrs[j].ipMask), 
                                                       addrs[j].ipMask)) 
                    {
                        ospfMapWithdrawRedistRoute((addrs[j].ipAddr & addrs[j].ipMask), 
                                                   addrs[j].ipMask);
                    }
                }   
            }
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  If conflicting area range exists for the given network 
*           address and netmask
*
* @param    areaId      Area
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
*
* @returns  L7_TRUE     if conflicting
* @returns  L7_FALSE    if not conflicting
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ospfMapAreaAggregateRangeConflictExists(L7_uint32 areaId,
                                                L7_uint32 lsdbType,
                                                L7_uint32 ipAddr,
                                                L7_uint32 netMask)
{
  L7_uint32 i;

  for (i=0; i < L7_OSPF_MAX_AREA_RANGES; i++)
  {
    if (pOspfMapCfgData->areaRange[i].inUse == L7_TRUE)
    {
      if (pOspfMapCfgData->areaRange[i].area == areaId)
      {
        if(pOspfMapCfgData->areaRange[i].ipAddr == ipAddr)
        {
          if(pOspfMapCfgData->areaRange[i].subnetMask == netMask)
          {
            if (pOspfMapCfgData->areaRange[i].lsdbType != lsdbType)
            {
              /* conflict exists */
              return L7_TRUE;
            }
          }
        }
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Find entry in area range configuration file
*
* @param    areaId      Area
* @param    lsdbType    LSDB Type (L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t)
* @param    ipAddr      ip address
* @param    netMask     mask to specify area range
* @param    matchType   find existing entry (L7_MATCH_EXACT) or first free entry (L7_MATCH_FREE)
* @param    *index      pointer to index parameter (L7_uint32 parm)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
* @returns  L7_ALREADY_CONFIGURED  if entry already exists.
*
* @notes    The index of the existing entry is passed back, if one exists.
*           Else, the index of the first empty entry is passed back, if one exists.
*
*
* @end
*********************************************************************/
L7_RC_t ospfMapAreaAggregateRangeCfgIndexFind ( L7_uint32 areaId,
                                                L7_uint32 lsdbType,
                                                L7_uint32 ipAddr,
                                                L7_uint32 netMask,
                                                L7_uint32 matchType,
                                                L7_uint32 *index)

{

  L7_IP_ADDR_t  newAreaRange;
  L7_BOOL empty_entry_found;
  L7_BOOL match_found;
  L7_uint32 entry_to_use;
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;


  /* Miscellaneous initializaton */
  empty_entry_found = L7_FALSE;
  match_found       = L7_FALSE;
  entry_to_use      = L7_FALSE;

  newAreaRange      = ipAddr & netMask;

  /* Find existing area configuration, if any */
  for (i=0; i < L7_OSPF_MAX_AREA_RANGES; i++)
  {
      if (pOspfMapCfgData->areaRange[i].inUse == L7_TRUE)
      {
         if (pOspfMapCfgData->areaRange[i].area == areaId)
         {
             if (pOspfMapCfgData->areaRange[i].lsdbType == lsdbType)
             {
                if(pOspfMapCfgData->areaRange[i].ipAddr == ipAddr)
                {
                   if(pOspfMapCfgData->areaRange[i].subnetMask == netMask)
                   {
                      match_found  = L7_TRUE;
                      entry_to_use = i;
                      rc = L7_ALREADY_CONFIGURED;
                      break;
                   }
                }
             }
         }
      }
      else
      {
          if (empty_entry_found == L7_FALSE)
          {
            empty_entry_found = L7_TRUE;
            entry_to_use = i;
            rc = L7_SUCCESS;
          }
      }
  }

  if ( (match_found == L7_FALSE) && (empty_entry_found == L7_FALSE) )
    {
        /* No available entries */
        return L7_FAILURE;

    }

  if (matchType == L7_MATCH_EXACT)
  {
      if (match_found == L7_FALSE)
      {
          /* Entry does not exist in configuration */
          return L7_FAILURE;
      }
  }

  *index = entry_to_use;

  return rc;

}

/*********************************************************************
* @purpose  common function to convert internal interface number to
*           ospf interface entry
*
* @param    intIfNum      @b{(input)} internal interface number
*
* @returns  pointer to ospf interface entry
*
* @notes    none
*
* @end
*********************************************************************/
t_IFO * ifoPtrGet(L7_uint32 intIfNum)
{
  t_IFO         *p_IFO = (t_IFO *) NULL;
  ospfMapLocalMgr_t *p_lm = NULL;

  if (ospfMapLmLookup(intIfNum, &p_lm) == L7_SUCCESS)
    p_IFO = (t_IFO *) p_lm->IFO_Id;

  return p_IFO;
}

