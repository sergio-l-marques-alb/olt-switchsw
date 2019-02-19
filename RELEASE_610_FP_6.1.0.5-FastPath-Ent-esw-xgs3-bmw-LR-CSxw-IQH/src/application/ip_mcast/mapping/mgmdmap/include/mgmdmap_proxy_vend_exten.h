/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    mgmd_proxy_vend_exten.h
*
* @purpose     MGMD Proxy vendor-specific API functions
*
* @component   MGMD Mapping Layer
*
* @comments
*
* @create      03/11/2002
*
* @author      gkiran
* @end
*
**********************************************************************/

#ifndef _MGMD_PROXY_VEND_EXTEN_H_
#define _MGMD_PROXY_VEND_EXTEN_H_

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
                            L7_uint32 intIfNum, L7_uint32 version);
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
                                                  L7_uint32 unsolicitedReportInterval);
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
                                            L7_uint32 robustness);
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
                                           L7_uint32 intIfNum);

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
                                    L7_uint32 intIfNum, L7_uint32* timer);

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
                                    L7_uint32 intIfNum, L7_uint32* timer);

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
                                   L7_uint32 intIfNum, L7_uint32* count);

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
                                        L7_uint32 *upTime);
                                                                                                 
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
                                              L7_inet_addr_t *ipAddr);

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
                                                 L7_uint32 *groupFilterMode);

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
                                           L7_uint32 *v3QueriesRcvd);
                                                                                          
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
                                           L7_uint32* v3ReportsRcvd);

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
                                          L7_uint32* v3ReportsSent);

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
                                           L7_uint32* v2QueriesRcvd);

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
                                           L7_uint32* v2ReportsRcvd);

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
                                          L7_uint32* v2ReportsSent);

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
                                          L7_uint32 *v2LeavesRcvd);

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
                                         L7_uint32* v2LeavesSent);

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
                                           L7_uint32* v1QueriesRcvd);

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
                                           L7_uint32* v1ReportsRcvd);

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
                                          L7_uint32 *v1ReportsSent);

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
                                       L7_uint32 intIfNum);

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
                                           L7_uint32 *pIntIfNum);

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
                                      L7_uint32 *ifindex);

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
                                              L7_inet_addr_t *groupAddr);


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
                                                  L7_inet_addr_t *ipaddr);
                                                  

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
                                         L7_inet_addr_t *hostAddr);

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
                                             L7_inet_addr_t *hostAddr);

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
                                              L7_uint32 *srcExpiryTime);

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
                                         L7_uint32 intIfNum);


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
                                            L7_uint32 intIfNum, L7_uint32 mode);

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
                                               L7_BOOL *mode);

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
                                       L7_int32 *numOfGroups);

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
                                         L7_uint32 *hostState);

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
                                     L7_uint32 *srcCount);

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
                                        L7_inet_addr_t *querierIP);

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
                                        L7_uint32 *status);

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
                                         L7_inet_addr_t* ipMRouteSourceMask);


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
                                               L7_uint32       *expire);

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
                                           L7_uint32       *upTime);

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
                                           L7_inet_addr_t *rpfAddress);

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
                                               L7_uint32 *count);

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
                                            L7_uint32      *intIfNum);

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
                                                  L7_uint32      *intIfNum);

/*********************************************************************
* @purpose  Get the MGMD Version configured for the router Interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    rtrIfNum     @b{(input)} router Interface Number
* @param    version      @b{(output)} MGMD Version on the interface
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolProxyInterfaceVersionGet(L7_uchar8 familyType, 
                                        L7_uint32 rtrIfNum,
                                        L7_uint32* version);
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
                                                    L7_uint32* interval);

#endif
