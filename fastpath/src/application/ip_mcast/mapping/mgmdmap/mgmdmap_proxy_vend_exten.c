/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  mgmd_proxy_vend_exten.c
*
* @purpose   MGMD Proxy vendor-specific functions
*
* @component Mgmd Mapping Layer
*
* @comments  none
*
* @create    07/03/2005
*
* @author    ryadagiri
*
* @end
*
**********************************************************************/
#include "l7_mgmdmap_include.h"

/*******************************************************************
* @purpose  Sets the Proxy Interface Version for the specified interface
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
L7_RC_t mgmdMapProxyExtenIntfVersionSet(mgmdMapCB_t *mgmdMapCbPtr,
                            L7_uint32 intIfNum, L7_uint32 version)
{
  mgmdMapEventParams_t  eventParams;
  L7_uint32             rtrIfNum = L7_NULL;

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
  eventParams.familyType = mgmdMapCbPtr->familyType;
  eventParams.eventInfo = version;
  eventParams.eventType = MGMD_VERSION_SET;
  eventParams.intfType = MGMD_PROXY_INTERFACE;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS," given version = %d", version);
  if (mgmdProxyProtocolConfigSet(&eventParams) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Proxy Interface Version Set Failed.\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Updates vendor with configured unsolicited report interval
*
* @param    mgmdMapCB                 @b{(input)} Mapping Control Block.
* @param    intIfNum                  @b{(input)} Internal Interface Number
* @param    unsolicitedReportInterval @b{(input)} configured unsolicited
*                                                 report interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is specific to Proxy interface and hence applied only if the
*           proxy is enabled on the given interface 
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenIntfUnsolicitedReportIntervalSet(mgmdMapCB_t *mgmdMapCbPtr,
                                                     L7_uint32 intIfNum,
                                                     L7_uint32 unsolicitedReportInterval)
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
  eventParams.eventType = MGMD_UNSOLICITED_REPORT_INTERVAL_SET;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventInfo = unsolicitedReportInterval;
  return mgmdProxyProtocolConfigSet(&eventParams);
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
L7_RC_t  mgmdMapProxyExtenIntfRobustnessSet(mgmdMapCB_t *mgmdMapCbPtr,
                                            L7_uint32 intIfNum,
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
  eventParams.eventType = MGMD_ROBUSTNESS_SET;
  eventParams.rtrIfNum = rtrIfNum;
  eventParams.eventInfo = robustness;
  return mgmdProxyProtocolConfigSet(&eventParams);

}
/*********************************************************************
* @purpose  Updates Proxy MRT table for interface down event
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
L7_RC_t mgmdMapProxyExtenIntfDownMRTUpdate(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum = L7_NULL;

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS,
                 "Function Entered, intIfNum\n", intIfNum);

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, 
                   "Failed to convert to rtrIfNum for intIfNum = %d\n",intIfNum);
    return L7_FAILURE;
  }
  mgmdProxyIntfDownMRTUpdate(mgmdMapCbPtr->cbHandle, rtrIfNum);
  return L7_SUCCESS;
}
/****************************************************************************
* @pupose  Gets the Proxy Version1 Querier Time for the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    timer               @b{(output)}  Querier  Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
****************************************************************************/
L7_RC_t mgmdMapProxyExtenIntfVer1QuerierTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                    L7_uint32 intIfNum, L7_uint32* timer)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyIntfVer1QuerierTimeGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                         timer);
}

/*****************************************************************************
* @purpose  Gets the Proxy Version2 Querier Time for the specified interface
*
* @param    mgmdMapCbPtr  @b{(input)}     Mapping Control Block.
* @param    intIfNum      @b{(input)}     Internal Interface Number
* @param    timer         @b{(output)}    Querier  Expiry Time
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
******************************************************************************/
L7_RC_t mgmdMapProxyExtenIntfVer2QuerierTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                    L7_uint32 intIfNum, L7_uint32* timer)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyIntfVer2QuerierTimeGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                         timer);
}


/************************************************************************
*  @purpose  Gets the Proxy Restart Count for the specified interface
*
*  @param    mgmdMapCbPtr       @b{(input)}   Mapping Control Block.
*  @param    intIfNum           @b{(input)}   Internal Interface Number
*  @param    count              @b{(output)}  count
* 
*  @returns  L7_SUCCESS
*  @returns  L7_FAILURE
* 
* @comments  none
* 
*  @end
*************************************************************************/
L7_RC_t mgmdMapProxyExtenIntfRestartCountGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 intIfNum, L7_uint32* count)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyIntfRestartCountGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                      count);
}


/***************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache
*           Table for the specified group address & the specified interface
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    multipaddr       @b{(input)}  multicast group ip address
* @param    intIfNum         @b{(input)}  internal Interface number
* @param    upTime           @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t mgmdMapProxyExtenCacheUpTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_inet_addr_t *multipaddr,
                                        L7_uint32 intIfNum,
                                        L7_uint32 *upTime)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyCacheUpTimeGet(mgmdMapCbPtr->cbHandle, multipaddr, 
                                 rtrIfNum, upTime);
}
                                                                                                                                               
/***********************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    multipaddr       @b{(input)}  multicast group ip address
* @param    intIfNum         @b{(input)}  internal Interface number
* @param    ipAddr           @b{(output)} last reporter ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyExtenCacheLastReporterGet(mgmdMapCB_t *mgmdMapCbPtr,
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
  return mgmdProxyCacheLastReporterGet(mgmdMapCbPtr->cbHandle, multipaddr, 
                             rtrIfNum, ipAddr);
}

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
*           group on the specified interface
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    groupAddr        @b{(input)}  multicast group ip address
* @param    intIfNum         @b{(input)}  internal Interface number
* @param    groupFilterMode  @b{(output)} source filter mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Proxy Cache Table
*
* @end
******************************************************************************/
L7_RC_t mgmdMapProxyExtenCacheGroupFilterModeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                                 L7_inet_addr_t *groupAddr,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32 *groupFilterMode)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }

  return mgmdProxyCacheGroupFilterModeGet(mgmdMapCbPtr->cbHandle, groupAddr,
                        rtrIfNum, groupFilterMode);
}

/**********************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    ipaddr            @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments Mgmd Cache Table
*
* @end
q
*********************************************************************/
L7_RC_t mgmdMapProxyExtenCacheEntryGet(mgmdMapCB_t *mgmdMapCbPtr,
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
  return mgmdProxyCacheEntryGet(mgmdMapCbPtr->cbHandle, ipaddr, 
                                rtrIfNum);
}
                                                                                                                                               
/**********************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given 
*           multi ipaddress & an interface number.
*
* @param    mgmdMapCbPtr @b{(input)}  Mapping Control Block.
* @param    ipaddr       @b{(input)}  multicast group ip address
* @param    pIntIfNum    @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments if intIfNum = 0 & multipaddr = 0, 
*           then return the first valid entry in the cache table
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyExtenCacheEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_inet_addr_t *ipaddr,
                                           L7_uint32 *pIntIfNum)
{
  L7_uint32  rtrIfNum = L7_NULL;
  L7_RC_t    retCode = L7_FAILURE;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      *pIntIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  retCode = mgmdProxyCacheEntryNextGet(mgmdMapCbPtr->cbHandle, 
                                       ipaddr, &rtrIfNum);
  
  return retCode;
}


/***********************************************************************
* @purpose  Gets the Proxy v3 queries received on the specified interface
*
* @param    mgmdMapCbPtr       @b{(input)}   Mapping Control Block.
* @param    intIfNum           @b{(input)}   Internal Interface Number
* @param    v3QueriesRcvd      @b{(output)}  v3QueriesRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenV3QueriesRecvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum,
                                           L7_uint32 *v3QueriesRcvd)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV3QueriesRecvdGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                    v3QueriesRcvd);
}

/**************************************************************************** 
* @purpose  Gets the Proxy v3 reports received  for the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v3ReportsRcvd       @b{(output)}  v3ReportsRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
****************************************************************************/
L7_RC_t mgmdMapProxyExtenV3ReportsRecvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum,
                                           L7_uint32* v3ReportsRcvd)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV3ReportsRecvdGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                    v3ReportsRcvd);
}

/*********************************************************************
* @purpose  Gets the Proxy v3 reports Sent for the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v3ReportsSent       @b{(output)}  v3ReportsSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenV3ReportsSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_uint32* v3ReportsSent)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV3ReportsSentGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                   v3ReportsSent);
}

/*************************************************************************
* @purpose  Gets the Proxy v2 queries received on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v2QueriesRcvd       @b{(output)}  v2QueriesRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t mgmdMapProxyExtenV2QueriesRecvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum,
                                           L7_uint32* v2QueriesRcvd)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV2QueriesRecvdGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                    v2QueriesRcvd);
}

/*************************************************************************
* @purpose  Gets the Proxy v2 reports received on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v2ReportsRcvd       @b{(output)}  v2ReportsRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenV2ReportsRecvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum,
                                           L7_uint32* v2ReportsRcvd)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV2ReportsRecvdGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                    v2ReportsRcvd);
}

/***********************************************************************
* @purpose  Gets the Proxy v2 reports sent on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v2ReportsSent       @b{(output)}  v2ReportsSent 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenV2ReportsSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_uint32* v2ReportsSent)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV2ReportsSentGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                   v2ReportsSent);
}


/*************************************************************************
* @purpose  Gets the Proxy v2 leaves received on the specified interface
*
* @param    mgmdMapCbPtr   @b{(input)}    Mapping Control Block.
* @param    intIfNum       @b{(input)}   Internal Interface Number
* @param    v2LeavesRcvd   @b{(output)}   v2LeavesRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenV2LeavesRecvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_uint32 *v2LeavesRcvd)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV2LeavesRecvdGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                   v2LeavesRcvd);
}

/************************************************************************
* @purpose  Gets the Proxy v2 leaves sent on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v2LeavesSent        @b{(output)}  v2LeavesSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*************************************************************************/
L7_RC_t mgmdMapProxyExtenV2LeavesSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_uint32 intIfNum,
                                         L7_uint32* v2LeavesSent)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV2LeavesSentGet(mgmdMapCbPtr->cbHandle, rtrIfNum,
                                  v2LeavesSent);
}

/***************************************************************************
* @purpose  Gets the Proxy v1 queries received on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v1QueriesRcvd       @b{(output)}  v1QueriesRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenV1QueriesRecvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum,
                                           L7_uint32* v1QueriesRcvd)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV1QueriesRecvdGet(mgmdMapCbPtr->cbHandle, rtrIfNum,
                                    v1QueriesRcvd);
}

/*************************************************************************
* @purpose  Gets the Proxy v1 reports received on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v1ReportsRcvd       @b{(output)}  v1ReportsRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t mgmdMapProxyExtenV1ReportsRecvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 intIfNum,
                                           L7_uint32* v1ReportsRcvd)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV1ReportsRecvdGet(mgmdMapCbPtr->cbHandle, rtrIfNum,
                                    v1ReportsRcvd);
}

/************************************************************************
* @purpose  Gets the Proxy v1 reports sent on the specified interface
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    intIfNum            @b{(input)}   Internal Interface Number
* @param    v1ReportsSent       @b{(output)}  v1ReportsSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t mgmdMapProxyExtenV1ReportsSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_uint32 *v1ReportsSent)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyV1ReportsSentGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                   v1ReportsSent);
}

/*********************************************************************
* @purpose  Get the Proxy interface number configured for
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    intIfNum         @b{(input)}  internal interface number
* @param    ifindex          @b{(output)} index of the proxy interface
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenInterfaceGet(mgmdMapCB_t *mgmdMapCbPtr,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *ifindex)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyInterfaceGet(mgmdMapCbPtr->cbHandle, rtrIfNum, ifindex);
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
* @comments Mgmd Proxy Cache Table
*
* @end
******************************************************************************/
L7_RC_t mgmdMapProxyExtenInverseCacheEntryGet(mgmdMapCB_t *mgmdMapCbPtr,
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
  return mgmdProxymgmdProxyInterfaceGet(mgmdMapCbPtr->cbHandle, rtrIfNum, 
                                        groupAddr);
}

/***************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
* @purpose  interface
*
* @param    mgmdMapCbPtr      @b{(inout)}  Mapping Control Block.
* @param    pIntIfNum         @b{(inout)}  internal Interface number
* @param    ipaddr            @b{(inout)}  multicast group ip address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0 & groupAddr=0, 
            then return the first valid entry in the cache table
*
* @end
***************************************************************************/
L7_RC_t mgmdMapProxyExtenInverseCacheEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
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
  retCode = mgmdProxyInverseCacheEntryNextGet(mgmdMapCbPtr->cbHandle, 
                                              &rtrIfNum, ipaddr);
  
  if (mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,
                                      rtrIfNum, pIntIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return retCode;
}

/*********************************************************************
* @purpose  To validate if the specified source address exists for 
*           the specified group address and interface or not
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    groupAddr        @b{(input)}  multicast group ip address
* @param    intIfNum         @b{(input)}  internal Interface number
* @param    hostAddr         @b{(input)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenSrcListEntryGet(mgmdMapCB_t *mgmdMapCbPtr,
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
  return mgmdProxySrcListEntryGet(mgmdMapCbPtr->cbHandle, groupAddr, 
                                  rtrIfNum, hostAddr);
}

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
* @purpose  interface and source address
*
* @param    ipaddr         @b{(inout)}  multicast group ip address
* @param    pIntIfNum      @b{(inout)}  internal Interface number
* @param    hostAddr       @b{(inout)}  source address
* @param    mgmdMapCBPtr   @b{(inout)}  Mapping Control Block.
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if currentHostIp = 0 return the first source address in that 
*           group address 
*
* @end
******************************************************************************/
L7_RC_t mgmdMapProxyExtenSrcListEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                             L7_inet_addr_t *ipaddr, 
                                             L7_uint32 *pIntIfNum, 
                                             L7_inet_addr_t *hostAddr)
{
  L7_uint32  rtrIfNum = L7_NULL;
  L7_RC_t    retCode = L7_FAILURE;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      *pIntIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  retCode = mgmdProxySrcListEntryNextGet(mgmdMapCbPtr->cbHandle, 
                                         ipaddr, &rtrIfNum, hostAddr);
  
  return retCode;
}

/*****************************************************************************
* @purpose  Get the amount of time until the specified Source Entry is aged out
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @param    srcExpiryTime     @b{(output)} source expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t mgmdMapProxyExtenSrcListExpiryTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                              L7_uint32 intIfNum,
                                              L7_inet_addr_t *groupAddr,
                                              L7_inet_addr_t *hostAddr,
                                              L7_uint32 *srcExpiryTime)
{
  L7_uint32  rtrIfNum;
  L7_RC_t    retCode = L7_FAILURE;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                  intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  retCode = mgmdProxySrcListExpiryTimeGet(mgmdMapCbPtr->cbHandle, 
                                          groupAddr, rtrIfNum, hostAddr, srcExpiryTime);
  return retCode;
}

/*********************************************************************
* @purpose  Resets the proxy statistics parameter 
*
* @param    mgmdMapCbPtr   @b{(input)}   Mapping Control Block.
* @param    intIfNum       @b{(input)}   Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenResetHostStatus(mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_uint32 intIfNum)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyResetHostStatus(mgmdMapCbPtr->cbHandle, rtrIfNum);
}

#ifdef MGMD_PROXY_FP_NOT_SUPPORTED
/*****************************************************************************
* @purpose  Disable or Enable traffic from downstream to upstream 
*           interface or vice versa 
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    mode           @b{(input)} enable or disable 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
******************************************************************************/
L7_RC_t mgmdMapExtenProxyUpstreamFwdDisable(mgmdMapCB_t *mgmdMapCbPtr,
                                            L7_uint32 intIfNum, L7_uint32 mode)
{
  if (mgmdMapMgmdProxyIntfInitialized(mgmdMapCbPtr->familyType,
      intIfNum) == L7_FAILURE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "mgmdMapMgmdProxyIntfInitialized Failed\n");
    return L7_FAILURE;
  }

  if (mgmdMapCbPtr->pMgmdMapCfgData == L7_NULLPTR)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid mgmd config data\n");
    return L7_FAILURE;
  }

  L7_assert(mgmdMapCbPtr->pMgmdMapCfgData);

  /* confirm about checking mgmd_host_info in MGMD vendor CB */

  if (mode == L7_ENABLE && mgmdMapCbPtr->
      pMgmdMapCfgData->mgmd_proxy_UpStreamDisable == L7_FALSE)
  {
    mgmdMapCbPtr->pMgmdMapCfgData->mgmd_proxy_UpStreamDisable = L7_TRUE;
  }
  else if(mode == L7_DISABLE && mgmdMapCbPtr->
      pMgmdMapCfgData->mgmd_proxy_UpStreamDisable == L7_TRUE)
  {
    mgmdMapCbPtr->pMgmdMapCfgData->mgmd_proxy_UpStreamDisable = L7_FALSE;
  }
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Gets the proxy forwarding status- enabled or disabled
*
* @param    mgmdMapCbPtr   @b{(input)}   Mapping Control Block.
* @param    intIfNum       @b{(input)}   Internal Interface Number
* @param    mode           @b{(output)}  Denotes proxy forwarding is enabled or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
******************************************************************************/
L7_RC_t mgmdMapExtenProxyUpstreamFwdDisableGet(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum, 
                                               L7_BOOL *mode)
{
  if(mgmdMapMgmdProxyIntfInitialized(mgmdMapCbPtr->familyType,
                                     intIfNum) == L7_FAILURE)
  {
     MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get proxy interface number configured for\n");
     return L7_FAILURE;
  }
  *mode = mgmdMapCbPtr->pMgmdMapCfgData->mgmd_proxy_UpStreamDisable;
  return L7_SUCCESS;
}
#endif
/*****************************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    mgmdMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    intIfNum           @b{(input)}  Internal Interface Number
* @param    numOfGroups        @b{(output)} Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
******************************************************************************/
L7_RC_t mgmdMapProxyExtenIntfGroupsGet(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum,
                                       L7_int32 *numOfGroups)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyIntfGroupsGet(mgmdMapCbPtr->cbHandle, 
                                rtrIfNum, numOfGroups);
}

/*********************************************************************
* @purpose  To get the state of the member 
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    hostState         @b{(output)} state of the group address 
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenProxyGroupStatusGet (mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_inet_addr_t *groupAddr,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *hostState)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxyGroupStatusGet(mgmdMapCbPtr->cbHandle, groupAddr, 
                                 rtrIfNum, hostState);
}

/**********************************************************************
* @purpose  Get the number of source records for the specified group 
*           and interface
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
**********************************************************************/
L7_RC_t mgmdMapProxyExtenSrcCountGet(mgmdMapCB_t *mgmdMapCbPtr,
                                     L7_inet_addr_t *groupAddr,
                                     L7_uint32 intIfNum,
                                     L7_uint32 *srcCount)
{
  L7_uint32  rtrIfNum = L7_NULL;

  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "IntIfNumToRtrIntf Conversion Failed\n");
    return L7_FAILURE;
  }
  return mgmdProxySrcCountGet(mgmdMapCbPtr->cbHandle, groupAddr, 
                              rtrIfNum, srcCount);
}

/*********************************************************************
* @purpose  Gets the Querier for the specified interface
*
* @param    mgmdMapCbPtr   @b{(input)}   Mapping Control Block.
* @param    intIfNum       @b{(input)}   Internal Interface Number
* @param    querierIP      @b{(output)}  Querier  IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyExtenIntfQuerierGet(mgmdMapCB_t *mgmdMapCbPtr,
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
  return mgmdProxyIntfQuerierGet(mgmdMapCbPtr->cbHandle, 
                                 rtrIfNum, querierIP);
}

/*********************************************************************
* @purpose  Gets the status of the row in the host cache table
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    multipaddr    @b{(input)}   Internal Group Address
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    status        @b{(output)}  Enabled or Disabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenCacheStatusGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_inet_addr_t *multipaddr,
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
  return mgmdProxyCacheStatusGet(mgmdMapCbPtr->cbHandle, multipaddr, 
                                 rtrIfNum, status);
}

/*********************************************************************
* @purpose  Get the MGMD  IP Multicast route table Next entry
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    ipMRouteGroup        @b{(inout)}  Multicast Group address
* @param    ipMRouteSource       @b{(inout)}  Source address
* @param    ipMRouteSourceMask   @b{(inout)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenIpMRouteEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                         L7_inet_addr_t* ipMRouteGroup, 
                                         L7_inet_addr_t* ipMRouteSource,
                                         L7_inet_addr_t* ipMRouteSourceMask)
{
  return mgmdProxyIpMRouteEntryNextGet(mgmdMapCbPtr->cbHandle, ipMRouteGroup,
                                       ipMRouteSource, ipMRouteSourceMask);
}

/*********************************************************************
* @purpose  Gets the minimum amount of time remaining before  this
*           entry will be aged out.
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    ipMRtGrp       @b{(input)} Multicast Group address
* @param    ipMRouteSrc    @b{(input)} Source address
* @param    ipMRtSrcMask   @b{(input)} Mask Address
* @param    expire         @b{(output)} expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The value of 0 indicates that the entry is not
*           subject to aging.
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyExtenIpMRouteExpiryTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                               L7_inet_addr_t *ipMRtGrp,
                                               L7_inet_addr_t *ipMRtSrc,
                                               L7_inet_addr_t *ipMRtSrcMask,
                                               L7_uint32       *expire)
{
  return mgmdProxyIpMRouteExpiryTimeGet(mgmdMapCbPtr->cbHandle, ipMRtGrp,
                                        ipMRtSrc, ipMRtSrcMask, expire);
}

/*********************************************************************
* @purpose  Gets the time since the multicast routing information
*           represented by this entry was learned by the router.
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    ipMRtGrp      @b{(input)} Multicast Group address
* @param    ipMRouteSrc   @b{(input)} Source address
* @param    ipMRtSrcMask  @b{(input)} Mask Address
* @param    upTime        @b{(output)} uptime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyExtenIpMRouteUpTimeGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_inet_addr_t *ipMRtGrp,
                                           L7_inet_addr_t *ipMRtSrc,
                                           L7_inet_addr_t *ipMRtSrcMask,
                                           L7_uint32       *upTime)
{
  return mgmdProxyIpMRouteUpTimeGet(mgmdMapCbPtr->cbHandle, ipMRtGrp,
                                    ipMRtSrc, ipMRtSrcMask, upTime);
}

/**********************************************************************
* @purpose  Gets the Rpf address for the given index
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    ipMRtGrp        @b{(input)} mcast group address
* @param    ipMRtSrc        @b{(input)} mcast source
* @param    ipMRtSrcMask    @b{(input)} source mask
* @param    rpfAddr         @b{(output)} RPF address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenIpMRouteRpfAddrGet(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_inet_addr_t *ipMRtGrp,
                                           L7_inet_addr_t *ipMRtSrc,
                                           L7_inet_addr_t *ipMRtSrcMask,
                                           L7_inet_addr_t *rpfAddress)
{
  return mgmdProxyIpMRouteRpfAddrGet(mgmdMapCbPtr->cbHandle, ipMRtGrp,
                                     ipMRtSrc, ipMRtSrcMask, rpfAddress);
}

/**********************************************************************
* @purpose  Gets the number of MRT entries in Proxy table.
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    count           @b{(input)} Number of MRT entries
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyExtenIpMRouteEntryCountGet(mgmdMapCB_t *mgmdMapCbPtr, 
                                               L7_uint32 *count)
{
  return mgmdProxyIpMrouteEntryCountGet(mgmdMapCbPtr->cbHandle, count);
}

/*********************************************************************
* @purpose  Gets the interface index on which IP datagrams sent by
*           these sources to this multicast address are received.
*           corresponding to the index received.
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    ipMRtGrp       @b{(input)}  Multicast Group address
* @param    ipMRouteSrc    @b{(input)}  Source address
* @param    ipMRtSrcMask   @b{(input)}  Mask Address
* @param    intIfNum       @b{(output)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A value of 0 indicates that datagrams are not subject
*           to an incoming interface check, but may be accepted on
*           multiple interfaces.
*
* @end
**********************************************************************/
L7_RC_t mgmdMapProxyExtenIpMRouteIfIndexGet(mgmdMapCB_t *mgmdMapCbPtr,
                                            L7_inet_addr_t *ipMRtGrp,
                                            L7_inet_addr_t *ipMRtSrc,
                                            L7_inet_addr_t *ipMRtSrcMask,
                                            L7_uint32      *intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 rtrIfNum =0;
  rc= mgmdProxyIpMRouteIfIndexGet(mgmdMapCbPtr->cbHandle, ipMRtGrp,
                                  ipMRtSrc, ipMRtSrcMask, &rtrIfNum);

  if(rc == L7_SUCCESS)
  {
    if (rtrIfNum == 0)
    {
      *intIfNum = 0;
      return L7_SUCCESS;
    }

    if(mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,rtrIfNum,intIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to convert rtrIfNum(%d) to intIfNum",rtrIfNum);
      return L7_FAILURE;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    mgmdMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    ipMRtGrp           @b{(input)} mcast group
* @param    ipMRtSrc           @b{(input)} mcast source
* @param    ipMRtSrcMask       @b{(input)} source mask
* @param    intIfNum           @b{(inout)} interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mgmdMapProxyExtenRouteOutIntfEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                                  L7_inet_addr_t *ipMRtGrp,
                                                  L7_inet_addr_t *ipMRtSrc,
                                                  L7_inet_addr_t *ipMRtSrcMask,
                                                  L7_uint32      *outIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 rtrIfNum =0, intIfNum;

  if(*outIfNum != 0)
  {
    if(mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,*outIfNum,&rtrIfNum) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to convert intIfNum(%d) to rtrIfNum",*outIfNum);
      return L7_FAILURE;
    }

  }

  rc = mgmdProxyRouteOutIntfEntryNextGet(mgmdMapCbPtr->cbHandle, ipMRtGrp, 
                                         ipMRtSrc, ipMRtSrcMask, &rtrIfNum);

  if(rc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if(mcastIpMapRtrIntfToIntIfNum(mgmdMapCbPtr->familyType,rtrIfNum,&intIfNum) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to convert rtrIfNum(%d) to intIfNum",rtrIfNum);
    return L7_FAILURE;
  }

  *outIfNum = intIfNum;

  return rc;
}


/*********************************************************************
* @purpose  Get the MGMD Version configured for the router Interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    rtrIfNum     @b{(input)} router Interface Number
* @param    version      @b{(output)} MGMD Version on the interface
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolProxyInterfaceVersionGet(L7_uchar8 familyType, 
                                        L7_uint32 rtrIfNum,
                                        L7_uint32* version)
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
  if (mgmdMapProxyInterfaceVersionGet(familyType, intIfNum, version) == L7_SUCCESS)
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
* @purpose  Get the Unsolicited Report Interval configured for 
*           the router Interface
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
L7_RC_t mgmdMapProtocolProxyInterfaceUnsolicitedIntervalGet(L7_uchar8 familyType,
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
  return mgmdMapProxyInterfaceUnsolicitedIntervalGet(familyType,intIfNum,interval);
}
                                                                                           

/*********************************************************************
* @purpose  To get the vendor Proxy CB handle based on family Type
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
L7_RC_t mgmdMapProtocolProxyCtrlBlockGet(L7_uchar8 familyType, 
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
      (mgmdMapCbPtr->proxyCbHandle != L7_NULLPTR))
  {
    *cbHandle = mgmdMapCbPtr->proxyCbHandle;
    return L7_SUCCESS;
  }

  MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "Invalid Vendor CB for Family - %d",
                 familyType);

  *cbHandle = L7_NULLPTR;
  return L7_FAILURE;
}

