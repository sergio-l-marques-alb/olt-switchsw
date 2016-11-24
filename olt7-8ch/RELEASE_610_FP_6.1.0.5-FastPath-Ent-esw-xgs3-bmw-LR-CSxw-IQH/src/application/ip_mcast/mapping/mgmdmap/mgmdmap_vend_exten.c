/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  mgmd_vend_exten.c
*
* @purpose   MGMD vendor-specific functions
*
* @component Mgmd Mapping Layer
*
* @comments  none
*
* @create    02/09/2002
*
* @author    ramakrishna
*
* @end
*
**********************************************************************/
#include "l7_mgmdmap_include.h"
#include "dtl_l3_mcast_api.h"

/* Local Prototype */
static L7_RC_t mgmdAsyncGlobal (mgmdMapCB_t *mgmdMapCbPtr, 
                                L7_uint32 mode);
static L7_RC_t mgmdAsyncInterface (mgmdMapCB_t *mgmdMapCbPtr, 
                           L7_uint32 intIfNum, L7_uint32 mode,
                           L7_MGMD_INTF_MODE_t intfType,
                           mgmdIntfConfig_t *mgmdIntfInfo);
/*********************************************************************
* @purpose  Sets the Mgmd Admin mode
*
* @param    mgmdMapCbPtr    @b{(input)}   Mapping Control Block.
* @param    mode            @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenMgmdAdminModeSet(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 mode, L7_BOOL doInit)
{
  L7_uint32 mcastMode = L7_DISABLE;
  L7_uint32 proxyIntfMode = L7_DISABLE;
  L7_uint32 intIfNum;
  L7_uint32 rtrIfNum;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n Function Entered\n");
  if (mode == L7_ENABLE)
  {
    if (doInit == L7_TRUE)
    {
      if (mgmdCnfgrMgmdInitPhase1DynamicProcess (mgmdMapCbPtr) != L7_SUCCESS)
      {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                        "Vendor Layer Dynamic Init Failed for Family - %d",
                        mgmdMapCbPtr->familyType);
        return L7_FAILURE;
      }
    }
    if (mgmdMapMgmdInitialized(mgmdMapCbPtr) == L7_TRUE)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Vendor MGMD already initialized");
      return L7_SUCCESS;
    }
    if ((mcastIpMapRtrAdminModeGet(mgmdMapCbPtr->familyType) != L7_ENABLE) ||
        ((mcastMapMcastAdminModeGet(&mcastMode) ==L7_SUCCESS)&&(mcastMode != L7_ENABLE)))
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, " Routing or Multicast is not enabled ");
      return L7_SUCCESS;
    }
    if (mgmdAsyncGlobal(mgmdMapCbPtr, L7_ENABLE) != L7_SUCCESS)
    {
        MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "mgmdAsyncGlobal Failed\n");
        if (doInit == L7_TRUE)
        {
          /* De-Initialize the protocol's Memory & Control Block */
          mgmdCnfgrMgmdFiniPhase1DynamicProcess (mgmdMapCbPtr);
        }
        return L7_FAILURE;
    }
    
    if (mgmdMapCbPtr->familyType == L7_AF_INET)
    {
      dtlRouterMulticastIgmpModeSet(L7_ENABLE, mgmdMapCbPtr->familyType);
    }
    else
    {
      dtlRouterMulticastIgmpModeSet(L7_ENABLE, mgmdMapCbPtr->familyType);
    }
    
    mgmdMapCbPtr->pMgmdInfo->initialized = L7_TRUE;

    for (rtrIfNum = 1; rtrIfNum <= MCAST_MAX_INTERFACES; rtrIfNum ++)
    {
      if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                       rtrIfNum, &intIfNum) != L7_SUCCESS)
      {
        continue;
      }
      if (mgmdMapProxyInterfaceModeGet (mgmdMapCbPtr->familyType, intIfNum,
                                        &proxyIntfMode) == L7_SUCCESS)
      {
        if (proxyIntfMode == L7_ENABLE)
        {
          if (mgmdMapUIEventSend (mgmdMapCbPtr, MGMD_INTF_MODE_SET, mode,
                                  intIfNum, MGMD_PROXY_INTERFACE) != L7_SUCCESS)
          {
            MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Proxy Enable Event "
                            "Post Failed for eventType:%d", MGMD_INTF_MODE_SET);
            break;
          }
        }
      }
    }
  }
  else  /* L7_DISABLE */
  {
    if (mgmdMapMgmdInitialized(mgmdMapCbPtr) == L7_TRUE)
    {
      if (mgmdAsyncGlobal(mgmdMapCbPtr, L7_DISABLE) != L7_SUCCESS)
      {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "mgmdAsyncGlobal Failed\n");
        return L7_FAILURE;
      }

      if (mgmdMapCbPtr->familyType == L7_AF_INET)
      {
        /* Remove the classifier added to the NPU */
        dtlRouterMulticastIgmpModeSet(L7_DISABLE, mgmdMapCbPtr->familyType);
      } else
      {
        dtlRouterMulticastIgmpModeSet(L7_DISABLE, mgmdMapCbPtr->familyType);
      }
      mgmdMapCbPtr->pMgmdInfo->initialized = L7_FALSE;
    }
    if (doInit == L7_TRUE)
    {
      /* De-Initialize the protocol's Memory & Control Block */
      mgmdCnfgrMgmdFiniPhase1DynamicProcess (mgmdMapCbPtr);
    }

  }
  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nCode Flow: SUCCESS\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the Mgmd Admin mode for the specified interface
*
* @param    mgmdMapCbPtr   @b{(input)}   Mapping Control Block.
* @param    intIfNum       @b{(input)}   Internal Interface Number
* @param    mode           @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfModeSet(mgmdMapCB_t *mgmdMapCbPtr,
                                L7_uint32 intIfNum, 
                                L7_uint32 mode,
                                L7_MGMD_INTF_MODE_t intfType)
{
  L7_uint32          rtrIfNum = L7_NULL;
  mgmdIntfCfgData_t *intfCfg = L7_NULLPTR;
  L7_inet_addr_t allRoutersAddr;
  mgmdIntfConfig_t   mgmdIntfInfo;
  MCAST_CB_HNDL_t cbHandle;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n Function Entered\n");

  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum, &intfCfg) != L7_TRUE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntfIsConfigurable Failed\n");
    return L7_FAILURE;
  }
  
  if (intfCfg == L7_NULL)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "intfCfg is NULL\n");
    return L7_FAILURE;
  }

  if ( ((mgmdMapCbPtr->familyType == L7_AF_INET) && (intfCfg->version == L7_MGMD_VERSION_3))
      && ((mgmdMapCbPtr->familyType == L7_AF_INET6) && (intfCfg->version == L7_MGMD_VERSION_2)) )
  {
    inetIgmpv3RouterAddressInit (mgmdMapCbPtr->familyType, &(allRoutersAddr));
    if (mcastLocalMulticastAddrUpdate(mgmdMapCbPtr->familyType,intIfNum,
                                      &allRoutersAddr,mode) != L7_SUCCESS)
    {
      L7_uchar8 addr[IPV6_DISP_ADDR_LEN];
      LOG_MSG ("MGMD All Routers Address - %s Set to the DTL Mcast List Failed; "
               "Mode - %d, intIfNum - %d", inetAddrPrint(&allRoutersAddr,addr),
               mode, intIfNum);
    }
  }

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_SUCCESS;
  }

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n Code Flow\n");

  if (intfType == MGMD_PROXY_INTERFACE &&
      mgmdMapProtocolProxyCtrlBlockGet(mgmdMapCbPtr->familyType, &cbHandle) == L7_FAILURE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Proxy Conversion Failed\n");
    return L7_SUCCESS;
  }

  memset(&mgmdIntfInfo, 0, sizeof(mgmdIntfConfig_t));
  mgmdIntfInfo.version = intfCfg->version;
  mgmdIntfInfo.robustness = intfCfg->robustness;
  mgmdIntfInfo.queryInterval = intfCfg->queryInterval;
  mgmdIntfInfo.queryResponseInterval = intfCfg->responseInterval;
  mgmdIntfInfo.startupQueryInterval = intfCfg->startupQueryInterval;
  mgmdIntfInfo.startupQueryCount = intfCfg->startupQueryCount;
  mgmdIntfInfo.lastMemQueryInterval = intfCfg->lastMemQueryInterval;
  mgmdIntfInfo.lastMemQueryCount = intfCfg->lastMemQueryCount;
  mgmdIntfInfo.unsolicitedReportInterval = intfCfg->unsolicitedReportInterval;

  if (mode == L7_ENABLE)
  {
    /* Apply all the config params whenever the interface initially comes up */
    if (mgmdAsyncInterface(mgmdMapCbPtr, 
                           rtrIfNum, L7_ENABLE, intfType,
                           &mgmdIntfInfo) != L7_SUCCESS)
    {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "mgmdAsyncInterface Failed\n");
        return L7_SUCCESS;
    }

    if (intfType == MGMD_PROXY_INTERFACE)
    {
      mgmdMapCbPtr->pMgmdInfo->mgmdProxyEnabled = L7_ENABLE;
    }
    else if (intfType == MGMD_ROUTER_INTERFACE)
    {
      mgmdMapCbPtr->pMgmdInfo->
                    mgmdIntfInfo[rtrIfNum].mgmdInitialized = L7_TRUE;
    }

  } else if (mode == L7_DISABLE)
  {

    if (mgmdAsyncInterface(mgmdMapCbPtr, rtrIfNum, 
                           L7_DISABLE, intfType,
                           &mgmdIntfInfo) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "mgmdAsyncInterface Failed\n");
      return L7_SUCCESS;
    }

    if (intfType == MGMD_PROXY_INTERFACE)
    {
      mgmdMapCbPtr->pMgmdInfo->mgmdProxyEnabled = L7_DISABLE;
    }
    else if(intfType == MGMD_ROUTER_INTERFACE)
    {
      mgmdMapCbPtr->pMgmdInfo->
                    mgmdIntfInfo[rtrIfNum].mgmdInitialized = L7_FALSE;
    }
   
  }

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\n Code Flow:SUCCESS\n");
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Enables/Disables router-alert check in vendor code when IGMP
*           control packets are used.
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenMgmdRouterAlertCheckSet(mgmdMapCB_t *mgmdMapCbPtr,
                                            L7_BOOL mode)
{
  mgmdMapEventParams_t eventParams;

  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.eventType = MGMD_ROUTER_ALERT_CHECK_SET;
  eventParams.eventInfo = mode;
  return mgmdProtocolConfigSet(&eventParams);
}
/*******************************************************************
* @purpose  Sets the mgmd router Interface Version 
*
* @param    mgmdMapCbPtr   @b{(input)}   Mapping Control Block.
* @param    intIfNum       @b{(input)}   Internal Interface Number
* @param    version        @b{(input)}   MGMD Version on the Interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfVersionSet(mgmdMapCB_t *mgmdMapCbPtr,
                            L7_uint32 intIfNum, L7_uint32 version)
{
  mgmdMapEventParams_t eventParams;
  L7_uint32                rtrIfNum = L7_NULL;
  L7_inet_addr_t allRoutersAddr;
  L7_uchar8 addr[IPV6_DISP_ADDR_LEN];

  if (mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType, 
      intIfNum) != L7_SUCCESS)
  {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, " IGMP is not enabled on intIfNum = %d \n", intIfNum);
      return L7_FAILURE;
  }

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }

  if (mgmdMapCbPtr->familyType == L7_AF_INET6)
  {
    version = version + 1;
  }
  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventType = MGMD_VERSION_SET;
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.eventInfo = version;
  eventParams.intfType = MGMD_ROUTER_INTERFACE;

  if (version == L7_MGMD_VERSION_3)
  {
    inetIgmpv3RouterAddressInit (mgmdMapCbPtr->familyType, &(allRoutersAddr));
    if (mcastLocalMulticastAddrUpdate(mgmdMapCbPtr->familyType,intIfNum,
                                      &allRoutersAddr,L7_ENABLE) != L7_SUCCESS)
    {
      L7_LOGF ( L7_LOG_SEVERITY_NOTICE, L7_FLEX_MCAST_MAP_COMPONENT_ID,
               "MGMD All Routers Address - %s Add to the DTL Mcast List Failed"
               " MGMD All Routers Address addition to the local multicast list"
               " Failed. As a result of this, MGMD Multicast packets with this"
               " address will not be received at the application.",
               inetAddrPrint(&allRoutersAddr,addr));
    }
  }
  else
  {
    if (mcastLocalMulticastAddrUpdate(mgmdMapCbPtr->familyType,intIfNum,
                                      &allRoutersAddr, L7_DISABLE) != L7_SUCCESS)
    {
      L7_LOGF ( L7_LOG_SEVERITY_NOTICE, L7_FLEX_MCAST_MAP_COMPONENT_ID,
               "MGMD All Routers Address - %s Delete from the DTL Mcast List Failed"
               " MGMD All Routers Address deletion from the local multicast list Failed."
               " As a result of this, MGMD Multicast packets are still received at the"
               " application though MGMD is disabled.",
               inetAddrPrint(&allRoutersAddr,addr));
    }
  }

  if (mgmdProtocolConfigSet(&eventParams) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Proxy Interface Version Set Failed.\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Updates vendor with configured robustness.
*
* @param    mgmdMapCB   @b{(input)} Mapping Control Block.
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    robustness  @b{(input)} configured robustness
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapExtenIntfRobustnessSet(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum,
                                       L7_uint32 robustness)
{
  L7_uint32 rtrIfNum = 0;
  mgmdMapEventParams_t eventParams;
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventType = MGMD_ROBUSTNESS_SET;
  eventParams.eventInfo = robustness;
  return mgmdProtocolConfigSet(&eventParams);

}
/*********************************************************************
* @purpose  Updates vendor with configured queryInterval
*
* @param    mgmdMapCB      @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    queryInterval  @b{(input)} configured query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapExtenIntfQueryIntervalSet(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum,
                                          L7_uint32 queryInterval)
{
  L7_uint32 rtrIfNum = 0;
  mgmdMapEventParams_t eventParams;
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.eventType = MGMD_QUERY_INTERVAL_SET;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventInfo = queryInterval;
  return mgmdProtocolConfigSet(&eventParams);
}
/*********************************************************************
* @purpose  Updates vendor with configured query Maximum response time
*
* @param    mgmdMapCB         @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    queryMaxRespTime  @b{(input)} configured query Maximum
*                                         response time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapExtenIntfQueryMaxRespTimeSet(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum,
                                             L7_uint32 queryMaxRespTime)
{
  L7_uint32 rtrIfNum = 0;
  mgmdMapEventParams_t eventParams;
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }

  /* These conversions needed for vendor as the packet formats differ
     for both IGMP and MLD */
  if (mgmdMapCbPtr->familyType == L7_AF_INET)
  {
    queryMaxRespTime = queryMaxRespTime / 10;
  }
  else if (mgmdMapCbPtr->familyType == L7_AF_INET6)
  {
    queryMaxRespTime = queryMaxRespTime / 1000;
  }

  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.eventType = MGMD_QUERY_MAX_RESPONSE_TIME_SET;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventInfo = queryMaxRespTime;
  return mgmdProtocolConfigSet(&eventParams);
}
/*********************************************************************
* @purpose  Updates vendor with configured startup Query Interval
*
* @param    mgmdMapCB            @b{(input)} Mapping Control Block.
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    startupQueryInterval @b{(input)} configured startup queryInterval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapExtenIntfStartupQueryIntervalSet(mgmdMapCB_t *mgmdMapCbPtr,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32 startupQueryInterval)
{
  L7_uint32 rtrIfNum = 0;
  mgmdMapEventParams_t eventParams;
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventType = MGMD_STARTUP_QUERY_INTERVAL_SET;
  eventParams.eventInfo = startupQueryInterval;
  return mgmdProtocolConfigSet(&eventParams);
}
/*********************************************************************
* @purpose  Updates vendor with configured startup Query count
*
* @param    mgmdMapCB         @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} Internal Interface Number
* @param    startupQueryCount @b{(input)} configured startup query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfStartupQueryCountSet(mgmdMapCB_t *mgmdMapCbPtr,
                                             L7_uint32 intIfNum,
                                             L7_uint32 startupQueryCount)
{
  L7_uint32 rtrIfNum = 0;
  mgmdMapEventParams_t eventParams;
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventType = MGMD_STARTUP_QUERY_COUNT_SET;
  eventParams.eventInfo = startupQueryCount;
  return mgmdProtocolConfigSet(&eventParams);
}
/*********************************************************************
* @purpose  Updates vendor with configured Last member query interval
*
* @param    mgmdMapCB            @b{(input)} Mapping Control Block.
* @param    intIfNum             @b{(input)} Internal Interface Number
* @param    lastMemQueryInterval @b{(input)} configured last member
*                                            query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfLastMemberQueryIntervalSet(mgmdMapCB_t *mgmdMapCbPtr, 
                                                   L7_uint32 intIfNum,
                                                   L7_uint32 lastMemQueryInterval)
{
  L7_uint32 rtrIfNum = 0;
  mgmdMapEventParams_t eventParams;
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  if (mgmdMapCbPtr->familyType == L7_AF_INET)
  {
    lastMemQueryInterval = lastMemQueryInterval / 10;
  }
  else if (mgmdMapCbPtr->familyType == L7_AF_INET6)
  {
    lastMemQueryInterval = lastMemQueryInterval / 1000;
  }
  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventType = MGMD_LASTMEMBER_QUERY_INTERVAL_SET;
  eventParams.eventInfo = lastMemQueryInterval;
  return mgmdProtocolConfigSet(&eventParams);
}
/*********************************************************************
* @purpose  Updates vendor with configured Last member query count
*
* @param    mgmdMapCB          @b{(input)} Mapping Control Block.
* @param    intIfNum           @b{(input)} Internal Interface Number
* @param    lastMemQueryCount  @b{(input)} configured last member
*                                          query count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfLastMemberQueryCountSet(mgmdMapCB_t *mgmdMapCbPtr,
                                                L7_uint32 intIfNum,
                                                L7_uint32 lastMemQueryCount)
{
  L7_uint32 rtrIfNum = 0;
  mgmdMapEventParams_t eventParams;
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventType = MGMD_LASTMEMBER_QUERY_COUNT_SET;
  eventParams.eventInfo = lastMemQueryCount;
  return mgmdProtocolConfigSet(&eventParams);
}

/*********************************************************************
* @purpose  Gets the Querier for the specified interface
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    querierIP     @b{(output)}  Querier  IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfQuerierGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 intIfNum, 
                                   L7_inet_addr_t *querierIP)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }

  return mgmdIntfQuerierGet(mgmdMapCbPtr->cbHandle, rtrIfNum, querierIP);
}

/*********************************************************************
* @purpose  Gets the Querier Up Time for the specified interface
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    querierUpTime   @b{(output)}Querier  Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfQuerierUpTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_uint32 intIfNum, 
                                         L7_uint32* querierUpTime)
{
  L7_uint32  rtrIfNum= L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }

  return mgmdIntfQuerierUpTimeGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                  querierUpTime);
}

/*********************************************************************
* @purpose  Gets the Querier Expiry Time for the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    querierExpiryTime   @b{(output)}  Querier  Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfQuerierExpiryTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                             L7_uint32 intIfNum, 
                                             L7_uint32* querierExpiryTime)
{
  L7_uint32  rtrIfNum= L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }

  return mgmdIntfQuerierExpiryTimeGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                      querierExpiryTime);
}

/*********************************************************************
* @purpose  Gets the Wrong Version Queries for the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    wrongVerQueries     @b{(output)}  Wrong Version Queries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfWrongVersionQueriesGet(mgmdMapCB_t *mgmdMapCbPtr,
                                               L7_uint32 intIfNum, 
                                               L7_uint32* wrongVerQueries)
{
  L7_uint32  rtrIfNum= L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdIntfWrongVersionQueriesGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                        wrongVerQueries);
}

/*********************************************************************
* @purpose  Gets the Number of Joins on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    intfJoins           @b{(output)}  Number of Joins on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfJoinsGet(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum, 
                                 L7_uint32* intfJoins)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdIntfJoinsGet(mgmdMapCbPtr->cbHandle, rtrIfNum, intfJoins);
}

/*********************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    numOfGroups         @b{(output)}  Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfGroupsGet(mgmdMapCB_t *mgmdMapCbPtr,
                                  L7_uint32 intIfNum, 
                                  L7_uint32* numOfGroups)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdIntfGroupsGet(mgmdMapCbPtr->cbHandle, rtrIfNum, numOfGroups);
}


/*********************************************************************
* @purpose  Gets the Querier Status for the specified interface
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    status        @b{(output)}  Querier  Status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfQuerierStatusGet(mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *status)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdIntfQuerierStatusGet(mgmdMapCbPtr->cbHandle, rtrIfNum, status);
}

/******************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    ipAddr            @b{(output)} last reporter ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheLastReporterGet(mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_inet_addr_t *multipaddr,
                                         L7_uint32 intIfNum,  
                                         L7_inet_addr_t *ipAddr)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheLastReporterGet(mgmdMapCbPtr->cbHandle, multipaddr, 
                                  rtrIfNum, ipAddr);
}


/******************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache Table for
*           the specified group address & the specified interface
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    upTime            @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheUpTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_inet_addr_t *multipaddr,
                                   L7_uint32 intIfNum,  L7_uint32 *upTime)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheUpTimeGet(mgmdMapCbPtr->cbHandle, multipaddr, rtrIfNum, 
                            upTime);
}

/******************************************************************************
* @purpose  Gets the  Expiry time before the specified entry in Cache Table will be aged out
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    expTime           @b{(output)} Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheExpiryTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                       L7_inet_addr_t *multipaddr,
                                       L7_uint32 intIfNum,  
                                       L7_uint32 *expTime)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheExpiryTimeGet(mgmdMapCbPtr->cbHandle, multipaddr,
                                rtrIfNum, expTime);
}

/******************************************************************************
* @purpose  Gets the  time remaining until the router assumes there are no longer
*           any MGMD version 1 Hosts on the specified interface
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    version1Time      @b{(output)} version1 Host Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheVersion1HostTimerGet(mgmdMapCB_t *mgmdMapCbPtr,
                                              L7_inet_addr_t *multipaddr,
                                              L7_uint32 intIfNum,  
                                              L7_uint32 *version1Time)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheVersion1HostTimerGet(mgmdMapCbPtr->cbHandle, multipaddr, 
                                       rtrIfNum, version1Time);
}


/******************************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    mgmdMapCbPtr      @b{(input)} Mapping Control Block.
* @param    ipaddr            @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheEntryGet(mgmdMapCB_t *mgmdMapCbPtr,
                                  L7_inet_addr_t *ipaddr, 
                                  L7_uint32 intIfNum)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheEntryGet(mgmdMapCbPtr->cbHandle, ipaddr, rtrIfNum);
}

/**************************************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given multi ipaddress & an interface number.
*
* @param    mgmdMapCbPtr  @b{(input)}  Mapping Control Block.
* @param    ipaddr        @b{(input)}  multicast group ip address
* @param    pIntIfNum      @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments if intIfNum = 0 & multipaddr = 0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                      L7_inet_addr_t *ipaddr, 
                                      L7_uint32 *pIntIfNum)
{
  L7_uint32  rtrIfNum = L7_NULL;
  L7_RC_t    retCode = L7_FAILURE;
  L7_uint32  tmpIntIfNum = *pIntIfNum;

  rtrIfNum = L7_NULL;
  if (tmpIntIfNum != 0)
  {
    if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
        tmpIntIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
      return L7_FAILURE;
    }
  }

  retCode = mgmdCacheEntryNextGet (mgmdMapCbPtr->cbHandle, 
                                   ipaddr, &rtrIfNum);
  if (retCode == L7_SUCCESS)
  {
    if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                        rtrIfNum, pIntIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "RtrIntfNumToIntIf Conversion Failed\n");
      return L7_FAILURE;
    }
  }
  return retCode;
}


/***************************************************************************
* @purpose  Gets the time remaining until the router assumes there are no
*           longer any MGMD version 2 Hosts for the specified group on the
*           specified interface
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    version2Time      @b{(output)} version1 Host Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheVersion2HostTimerGet(mgmdMapCB_t *mgmdMapCbPtr,
                                              L7_inet_addr_t *groupAddr,
                                              L7_uint32 intIfNum,  
                                              L7_uint32 *version2Time)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheVersion2HostTimerGet(mgmdMapCbPtr->cbHandle, groupAddr, 
                                       rtrIfNum, version2Time);
}

/***************************************************************************
* @purpose  Gets the compatibility mode (v1, v2 or v3) for the specified
*           group on the specified interface
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupCompatMode   @b{(output)} group compatibility mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheGroupCompatModeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                            L7_inet_addr_t *groupAddr,
                                            L7_uint32 intIfNum,  
                                            L7_uint32 *groupCompatMode)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheGroupCompatModeGet(mgmdMapCbPtr->cbHandle, groupAddr, 
                                     rtrIfNum, groupCompatMode);
}

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
*           group on the specified interface
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    sourceFilterMode  @b{(output)} source filter mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheGroupSourceFilterModeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                                  L7_inet_addr_t *groupAddr,
                                                  L7_uint32 intIfNum,  
                                                  L7_uint32 *sourceFilterMode)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
     MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheGroupSourceFilterModeGet(mgmdMapCbPtr->cbHandle, 
                                  groupAddr, rtrIfNum, sourceFilterMode);

}

/*****************************************************************************
* @purpose  To validate if the specified interface contains a cache entry for
*           the specified group or not
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheIntfEntryGet(mgmdMapCB_t *mgmdMapCbPtr,
                                      L7_uint32 intIfNum, 
                                      L7_inet_addr_t *groupAddr)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdCacheIntfEntryGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                               groupAddr);
}

/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
*           interface
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    pIntIfNum         @b{(input)}  internal Interface number
* @param    ipaddr            @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0 & groupAddr=0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCacheIntfEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                          L7_uint32 *pIntIfNum, 
                                          L7_inet_addr_t *ipaddr)
{
  L7_uint32  rtrIfNum = L7_NULL;
  L7_RC_t    retCode = L7_FAILURE;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      *pIntIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  retCode = mgmdCacheIntfEntryNextGet(mgmdMapCbPtr->cbHandle, &rtrIfNum, 
                                      ipaddr);
  
  if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                      rtrIfNum, pIntIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "RtrIntfNumToIntIf Conversion Failed\n");
    return L7_FAILURE;
  }
  return retCode;
}
/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
*           group address and interface or not
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenSrcListEntryGet(mgmdMapCB_t *mgmdMapCbPtr,
                                    L7_inet_addr_t *groupAddr, 
                                    L7_uint32 intIfNum, 
                                    L7_inet_addr_t *hostAddr)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdSrcListEntryGet(mgmdMapCbPtr->cbHandle, groupAddr, rtrIfNum, 
                             hostAddr);
}

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
*           interface and source address
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    ipaddr            @b{(input)}  multicast group ip address
* @param    pIntIfNum         @b{(input)}  internal Interface number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0, then return the 
*           first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenSrcListEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_inet_addr_t *ipaddr, 
                                        L7_uint32 *pIntIfNum, 
                                        L7_inet_addr_t *hostAddr)
{
  L7_uint32  rtrIfNum = L7_NULL;
  L7_RC_t    retCode = L7_FAILURE;

  if (*pIntIfNum == 0)
  {
    /* First entry */
    rtrIfNum = 0;
    retCode = mgmdSrcListEntryNextGet (mgmdMapCbPtr->cbHandle, ipaddr, 
                                  &rtrIfNum, hostAddr);
  }
  else
  {
    if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                        *pIntIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
      return L7_FAILURE;
    }
    retCode = mgmdSrcListEntryNextGet (mgmdMapCbPtr->cbHandle, ipaddr, 
                                       &rtrIfNum, hostAddr);
  }
  
  if (retCode == L7_SUCCESS)
  {
    if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                        rtrIfNum, pIntIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "RtrIntfNumToIntIf Conversion Failed\n");
      return L7_FAILURE;
    }
  }
  return retCode;
}

/*****************************************************************************
* @purpose  Get the Next valid Group and Interface entry in the order of (Grp, Intf)
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    ipaddr             @b{(inout)}   multicast group ip address
* @param    pIntIfNum          @b{(inout)}   Internal Interface Number
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0 & groupAddr=0  then return the 
*           first valid entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenGrpIntfEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_inet_addr_t *ipaddr,
                                        L7_uint32 *pIntIfNum)
{
  L7_uint32  rtrIfNum = L7_NULL;
  L7_RC_t    retCode = L7_FAILURE;

  if (*pIntIfNum == 0)
  {
    /* First entry */
    rtrIfNum = 1;
    retCode = mgmdGrpIntfEntryNextGet (mgmdMapCbPtr->cbHandle, ipaddr,
                                  &rtrIfNum);
  }
  else
  {
    if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                        *pIntIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
      return L7_FAILURE;
    }
    retCode =  mgmdGrpIntfEntryNextGet (mgmdMapCbPtr->cbHandle, ipaddr,
                                  &rtrIfNum);
  }

  if (retCode == L7_SUCCESS)
  {
    if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                        rtrIfNum, pIntIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "RtrIntfNumToIntIf Conversion Failed\n");
      return L7_FAILURE;
    }
  }
  return retCode;

}
/*******************************************************************************
* @purpose  Get the amount of time until the specified Source Entry is aged out
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    hostAddr          @b{(input)}  source address
* @param    srcExpiryTime     @b{(output)} source expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenSrcExpiryTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                  L7_inet_addr_t *groupAddr,
                  L7_uint32 intIfNum, L7_inet_addr_t *hostAddr,
                  L7_uint32 *srcExpiryTime)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdSrcExpiryTimeGet (mgmdMapCbPtr->cbHandle, groupAddr, rtrIfNum, 
                            hostAddr, srcExpiryTime);
}

/*******************************************************************************
* @purpose  Get the number of source records for the specified group and interface
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    srcCount          @b{(output)} number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenSrcCountGet(mgmdMapCB_t *mgmdMapCbPtr,
            L7_inet_addr_t *groupAddr, L7_uint32 intIfNum,
            L7_uint32 *srcCount)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdSrcCountGet (mgmdMapCbPtr->cbHandle, groupAddr, 
                          rtrIfNum, srcCount);
}

/*********************************************************************
* @purpose  Gets the Next Valid Interface for MGMD
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    pIntIfNum     @b{(inout)}   Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenIntfEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                     L7_uint32 *pIntIfNum)
{
  L7_uint32 tmpIntIfNum = L7_NULL;
  L7_uint32 mode = L7_DISABLE;

  for (tmpIntIfNum = *pIntIfNum+1; 
       tmpIntIfNum <= L7_MAX_INTERFACE_COUNT; tmpIntIfNum++)
  {
    /* Check if mgmd is enabled on the interface */
    if ((mgmdMapInterfaceModeGet(mgmdMapCbPtr->familyType, 
        tmpIntIfNum, &mode)== L7_SUCCESS) && mode == L7_ENABLE)
    {
      *pIntIfNum = tmpIntIfNum;
      return L7_SUCCESS;
    }
  }

  MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD not enabled on the interface\n");
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  To send interface admin Mode Set event to MGMD(MGMD/MLD) task.
*
* @param    mgmdMapCbPtr  @b{(input)} Mapping Control Block.
* @param    rtrIfNum      @b{(input)} Rtr Interface Number
* @param    mode          @b{(input)} admin Mode (ENABLE/DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t mgmdAsyncInterface(mgmdMapCB_t *mgmdMapCbPtr, 
                                  L7_uint32 rtrIfNum, L7_uint32 mode,
                                  L7_MGMD_INTF_MODE_t intfType,
                                  mgmdIntfConfig_t *mgmdIntfInfo)
{
  mgmdMapEventParams_t eventParams;

  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.eventInfo = mode;
  eventParams.intfType = intfType;
  eventParams.eventType = MGMD_INTF_MODE_SET;
  eventParams.mgmdIntfInfo = mgmdIntfInfo;

  /* These conversions needed for vendor as the packet formats differ
     for both IGMP and MLD */
  if (mgmdMapCbPtr->familyType == L7_AF_INET)
  {
    mgmdIntfInfo->queryResponseInterval = mgmdIntfInfo->queryResponseInterval / 10;
    mgmdIntfInfo->lastMemQueryInterval = mgmdIntfInfo->lastMemQueryInterval / 10;
  }
  else if (mgmdMapCbPtr->familyType == L7_AF_INET6)
  {
    mgmdIntfInfo->version = mgmdIntfInfo->version + 1;
    mgmdIntfInfo->queryResponseInterval = mgmdIntfInfo->queryResponseInterval / 1000;
    mgmdIntfInfo->lastMemQueryInterval = mgmdIntfInfo->lastMemQueryInterval / 1000;
  }
 
  if (mgmdProtocolConfigSet(&eventParams) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Interface Admin mode Set Failed.\n");
    return L7_FAILURE;
  } 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To send interface admin Mode Set event to MGMD(MGMD/MLD) task.
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    mode            @b{(input)} admin Mode (ENABLE/DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t mgmdAsyncGlobal(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 mode)
{
  mgmdMapEventParams_t eventParams;

  memset(&eventParams, 0, sizeof(mgmdMapEventParams_t));
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.eventType = MGMD_ADMIN_MODE_SET;
  eventParams.eventInfo = mode;
  return mgmdProtocolConfigSet(&eventParams);
}

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Received for the router.
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    validPkts         @b{(output)} Valid Packets Received.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenValidPacketsRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_uint32 *validPkts)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_TOTAL_PKTS_RECV, validPkts);
}

/********************************************************************
* @purpose  Get the Valid MLD Pakcets Sent for the router.
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    pktsSent          @b{(output)} Valid Packets Sent.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenValidPacketsSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_uint32 *pktsSent)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_TOTAL_PKTS_SENT, pktsSent);
}

/********************************************************************
* @purpose  Get the Queries Received for the router.
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    quriesRcvd        @b{(output)} Queries Rcvd.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenQueriesRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *quriesRcvd)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_QUERIES_RECV, quriesRcvd);
}
/********************************************************************
* @purpose  Get the Queries Sent for the router.
*
* @param    mgmdMapCbPtr      @b{(input)} Mapping Control Block.
* @param    quriesSent        @b{(output)}Queries Sent.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenQueriesSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *quriesSent)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_QUERIES_SENT, quriesSent);
}

/********************************************************************
* @purpose  Get the Reports Received for the router.
*
* @param    mgmdMapCbPtr      @b{(input)} Mapping Control Block.
* @param    reportsRcvd       @b{(output)} Reports Received.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenReportsRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *reportsRcvd)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_REPORTS_RECV, reportsRcvd);
}

/********************************************************************
* @purpose  Get the Reports Sent for the router.
*
* @param    mgmdMapCbPtr      @b{(input)} Mapping Control Block.
* @param    reportsSent       @b{(output)} Reports Sent.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenReportsSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *reportsSent)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_REPORTS_SENT, reportsSent);
}

/********************************************************************
* @purpose  Get the Leaves Received for the router.
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    leavesRcvd        @b{(output)} Leaves Received.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenLeavesRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                  L7_uint32 *leavesRcvd)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_LEAVES_RECV, leavesRcvd);
}

/********************************************************************
* @purpose  Get the Leaves Sent for the router.
*
* @param    mgmdMapCbPtr      @b{(input)} Mapping Control Block.
 @param     leavesSent        @b{(output)} Leaves Sent.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenLeavesSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                  L7_uint32 *leavesSent)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_LEAVES_SENT, leavesSent);
}

/********************************************************************
* @purpose  Get the BadCheckSum Pkts for the router.
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    badChkSum         @b{(output)} Bad Checksum Packets.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenBadCheckSumPktsGet(mgmdMapCB_t *mgmdMapCbPtr,
                                       L7_uint32 *badChkSum)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_BAD_CKSM_PKTS, badChkSum);
}

/********************************************************************
* @purpose  Get the Malformed Pkts for the router.
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    malfrmedPkts      @b{(output)} Malformed packets.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenMalformedPktsGet(mgmdMapCB_t *mgmdMapCbPtr,
                                     L7_uint32 *malfrmedPkts)
{
  return mgmdCounterGet(mgmdMapCbPtr->cbHandle, 
                        L7_MGMD_MALFORM_PKTS, malfrmedPkts);
}

/********************************************************************
* @purpose  Clear the Mld Counters for MLD.
*
* @param    mgmdMapCbPtr      @b{(input)} Mapping Control Block.
* @param    intIfNum          @b{(input)} interface number.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenCountersClear(mgmdMapCB_t *mgmdMapCbPtr, 
                                  L7_uint32 intIfNum)
{
  L7_uint32  rtrIfNum = L7_NULL;

  /* intIfNum zero means clearing mgmd counters 
   * for all interfaces.
   */
  if (intIfNum != L7_NULL)
  {
    if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                    intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
      return L7_FAILURE;
    }
  }
  return mgmdCountersClear(mgmdMapCbPtr->cbHandle, rtrIfNum);
}

/********************************************************************
* @purpose  Clear the Traffic Counters for MLD.
*
* @param    mgmdMapCbPtr       @b{(input)}  Mapping Control Block.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenTrafficCountersClear(mgmdMapCB_t *mgmdMapCbPtr)
{
  return mgmdTrafficCountersClear(mgmdMapCbPtr->cbHandle);
}

/*********************************************************************
* @purpose  Get the MGMD Version configured for the router Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    version     @b{(output)} MGMD Version on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceVersionGet(L7_uchar8 familyType,
                                   L7_uint32 rtrIfNum, 
                                   L7_uint32* version)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
  L7_uint32 intIfNum = 0;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }

  if (mcastIpMapRtrIntfToIntIfNum(familyType, rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  if (mgmdMapInterfaceVersionGet(familyType, intIfNum, version) == L7_SUCCESS)
  {
    if (familyType == L7_AF_INET6)
    {
      *version = *version + 1;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get the interface query interval for the specific router Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    interval    @b{(output)} Interface Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceQueryIntervalGet(L7_uchar8 familyType, 
                                                 L7_uint32 rtrIfNum, 
                                                 L7_uint32 *interval)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
  L7_uint32 intIfNum = 0;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }

  if (mcastIpMapRtrIntfToIntIfNum(familyType, rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  return mgmdMapInterfaceQueryIntervalGet(familyType, intIfNum, interval);
}


/*********************************************************************
* @purpose  Get the Query Max Response Time configured for the 
*           router Interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    rtrIfNum     @b{(input)} router Interface Number
* @param    maxRespTime  @b{(output)} Max Response Time on the interface 
*                                     in seconds.  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceQueryMaxResponseTimeSecondsGet(L7_uchar8 familyType,
               L7_uint32 rtrIfNum, L7_uint32* maxRespTime)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
  L7_uint32 intIfNum = 0;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }

  if (mgmdMapInterfaceQueryMaxResponseTimeGet(familyType,intIfNum,maxRespTime) == L7_SUCCESS)
  {
    if (familyType == L7_AF_INET)
    {
      *maxRespTime = *maxRespTime / 10;
    }
    else if (familyType == L7_AF_INET6)
    {
      *maxRespTime = *maxRespTime / 1000;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the Query Max Response Time configured for the 
*           router Interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    rtrIfNum     @b{(input)} router Interface Number
* @param    maxRespTime  @b{(output)} Max Response Time on the interface
*                                     in milli-seconds.  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceQueryMaxResponseTimeMilliSecsGet(L7_uchar8 familyType,
               L7_uint32 rtrIfNum, L7_uint32* maxRespTime)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
  L7_uint32 intIfNum = 0;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }

  return mgmdMapInterfaceQueryMaxResponseTimeGet(familyType,intIfNum,maxRespTime);
}

/*********************************************************************
* @purpose  Get the Robustness variable configured for the router Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    robustness  @b{(output)} Robustness variable on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceRobustnessGet(L7_uchar8 familyType, L7_uint32 rtrIfNum,
                                              L7_uint32* robustness)
{
  L7_uint32 intIfNum = 0;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  return mgmdMapInterfaceRobustnessGet(familyType,intIfNum,robustness);
}
/*********************************************************************
* @purpose  Get the Last Member Query Interval configured for the Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    interval    @b{(output)} Last Member Query Interval on the interface
*                                    in seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceLastMembQueryIntervalSecondsGet(L7_uchar8 familyType,
                                                         L7_uint32 rtrIfNum, 
                                                         L7_uint32* interval)
{
  L7_uint32 intIfNum = 0;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  if (mgmdMapInterfaceLastMembQueryIntervalGet(familyType,intIfNum,interval)== L7_SUCCESS)
  {
    if (familyType == L7_AF_INET)
    {
      *interval = *interval / 10;
    }
    else if (familyType == L7_AF_INET6)
    {
      *interval = *interval / 1000;
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
/*********************************************************************
* @purpose  Get the Last Member Query Interval configured for the Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    interval    @b{(output)} Last Member Query Interval on the interface
*                                    in milli-seconds.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceLastMembQueryIntervalMilliSecsGet(L7_uchar8 familyType,
                                                         L7_uint32 rtrIfNum, 
                                                         L7_uint32* interval)
{
  L7_uint32 intIfNum = 0;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  return mgmdMapInterfaceLastMembQueryIntervalGet(familyType,intIfNum,interval);
}

/*********************************************************************
* @purpose  Set the Last Member Query Count for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    queryCount  @b{(input)} Last Member Query Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceLastMembQueryCountGet(L7_uchar8 familyType,
                                                      L7_uint32 rtrIfNum, 
                                                      L7_uint32 *queryCount)
{
  L7_uint32 intIfNum = 0;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  return mgmdMapInterfaceLastMembQueryCountGet(familyType,intIfNum,queryCount);
}

/*********************************************************************
* @purpose  Get the Startup Query Interval configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    interval    @b{(output)} Startup Query Interval on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceStartupQueryIntervalGet(L7_uchar8 familyType,
                                                        L7_uint32 rtrIfNum,
                                                        L7_uint32* interval)
{
  L7_uint32 intIfNum = 0;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }
  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  return mgmdMapInterfaceStartupQueryIntervalGet(familyType,intIfNum,interval);
}

/*********************************************************************
* @purpose  Get the Startup Query Count configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    queryCount  @b{(output)} Startup Query Count on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceStartupQueryCountGet(L7_uchar8 familyType,
                                                     L7_uint32 rtrIfNum,
                                                     L7_uint32* queryCount)
{
  L7_uint32 intIfNum = 0;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }

  if (mcastIpMapRtrIntfToIntIfNum(familyType,rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,   "Could not Map the Rtr Inter face interface number \n");
    return L7_FAILURE;
  }
  return mgmdMapInterfaceStartupQueryCountGet(familyType,intIfNum,queryCount);
}


/*********************************************************************
* @purpose  To get the vendor CB handle based on family Type
*
* @param    familyType    @b{(input)} Address Family type.
* @param    cbHandle      @b{(output)} cbHandle
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolCtrlBlockGet(L7_uchar8 familyType, 
                                    MCAST_CB_HNDL_t *cbHandle)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "\nFunction Entered\n");
  if (mgmdMapCtrlBlockGet(familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                    "Invalid Mapping CB for Family - %d", familyType);
    /* Failed to get control block */
    *cbHandle = L7_NULLPTR;
    return L7_FAILURE;
  }
  if ((mgmdMapCbPtr != L7_NULLPTR) &&
      (mgmdMapCbPtr->cbHandle != L7_NULLPTR))
  {
    *cbHandle = mgmdMapCbPtr->cbHandle;
    return L7_SUCCESS;
  }

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "Invalid Vendor CB for Family - %d",
                 familyType);

  *cbHandle = L7_NULLPTR;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Routine to Notify MRP protocols of the Membership events.
*
* @param    mgmdCB         @b{ (input) }  MGMD control block
* @param    event          @b{ (input) }  event type.
* @param    grpInfo        @b{ (input) }  Group change data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolMRPMembershipEventsNotify(L7_uchar8   family, 
                                                 mgmMrtEvents_t  event, 
                                                 mgmdGroupInfo_t  *grpInfo)
{
  L7_int32  index; 
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
  L7_RC_t   rc = L7_SUCCESS;
  mrp_source_record_t **mgmdMrpMainSrcRec = L7_NULLPTR;
  mgmdMrpEventInfo_t mgmdMrpEventInfo;
  L7_int32 srcIndex = 0;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_EVENTS," Entered ");

  if (mgmdMapCtrlBlockGet(family, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    /* Failed to get control block */
    return L7_FAILURE;
  }

  memset (&mgmdMrpEventInfo, 0, sizeof (mgmdMrpEventInfo_t));

  /* Fill in the details */
  mgmdMrpEventInfo.rtrIfNum = grpInfo->rtrIfNum;
  mgmdMrpEventInfo.mode = grpInfo->mode;
  inetCopy (&mgmdMrpEventInfo.group, &grpInfo->group);
  mgmdMrpEventInfo.numSrcs = grpInfo->numSrcs;

  /* Fill in the sourceList details in case of MGMDv3/MLDv2 */
  if (grpInfo->numSrcs > 0)
  {
    mgmdMrpMainSrcRec = (mrp_source_record_t**)
                         osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                      sizeof(mrp_source_record_t*) * grpInfo->numSrcs);
    mgmdMrpEventInfo.sourceList = mgmdMrpMainSrcRec;

    for (srcIndex = 0; srcIndex < grpInfo->numSrcs; srcIndex++)
    {
      mgmdMrpEventInfo.sourceList[srcIndex] = grpInfo->sourceList[srcIndex];
    }
  }

  /* Notify the registered MRP components */
  for (index = 0; index < L7_MRP_MAXIMUM; index++)
  {
    if ((mgmdMapCbPtr->mgmdInfo.mgmdGrpInfoNotifyList[index].notifyMembershipChange)
         != L7_NULLPTR)
    {
      /* Since right now only one protocol will be active, if atleast one 
         callback function returned L7_SUCCESS then this function should 
         return L7_SUCCESS.In future,if multiple protocols can be made 
         active then this logic has to be changed.     
       */
      if(((*mgmdMapCbPtr->mgmdInfo.mgmdGrpInfoNotifyList[index].notifyMembershipChange)
             (family, event, sizeof(mgmdMrpEventInfo_t), &mgmdMrpEventInfo))
          != L7_SUCCESS)
      {
        rc = L7_FAILURE;

        /* Free the MCAST MGMD Events Source List Buffers */
        mcastMgmdEventsBufferPoolFree (family, &mgmdMrpEventInfo);
      }
    }
  }/*End-of-For-Loop*/

  return rc;
}


/*********************************************************************
* @purpose  Obtain MGMD HeapID
* 
* @param    family @b{(input)} IP Address family
*
* @returns  heapId, on success
* @returns  L7_NULL, on failure
*
* @comments This heapID is to be used for all the memory allocations for the
*           given IP address family within the MGMD module
*
* @end
*********************************************************************/
L7_uint32 mgmdMapHeapIdGet (L7_uint32 family)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(family, &mgmdMapCbPtr) == L7_SUCCESS)
  {
    return (mgmdMapCbPtr->mgmdHeapId);
  }

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
                  "\n MGMDMAP : Failed to get heap ID for family %d\n", family);
  return L7_NULL;
}


