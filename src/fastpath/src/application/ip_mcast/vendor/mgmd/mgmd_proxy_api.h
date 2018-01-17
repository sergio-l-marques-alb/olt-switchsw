/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  mgmd_proxy_vend_api.h
*
* @purpose   MGMD Proxy vendor-specific API function prototypes
*
* @component Mgmd Mapping Layer
*
* @comments  none
*
* @create    April 11, 2006
*
* @author    akamlesh
*
* @end
*
**********************************************************************/

#include "mgmd_api.h"
/*********************************************************************
*
* @purpose  To Initialize the Memory for MGMD Proxy Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyMemoryInit (L7_uchar8 addrFamily);

/*********************************************************************
*
* @purpose  To Deinitialize the Memory for MGMD Proxy Protocol
*
* @param    addrFamily @b{ (input) } Address Family Identifier
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyMemoryDeInit (L7_uchar8 addrFamily);
/*****************************************************************
* @purpose  Proxy Init functionality
*
* @param    mgmdCB    @b{ (input) }   MGMD Control Block
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t mgmdProxyCtrlBlockInit (L7_uchar8 addrFamily);
/*****************************************************************
* @purpose  Proxy DeInit functionality
*
* @param    mgmdCB    @b{ (input) }   MGMD Control Block
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCtrlBlockDeInit (void *mgmdHostCB ); 
                      
/*********************************************************************
* @purpose  Handles the interface routing change
*
* @param    mgmdCB            @b{ (input) }  MGMD Control Block
* @param    rtrIfNum          @b{ (input) }  the disabled router interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void mgmdProxyIntfDownMRTUpdate(MCAST_CB_HNDL_t mgmdCBHndl,L7_uint32 rtrIfNum);
             
/****************************************************************************
* @pupose  Gets the Proxy Version1 Querier Time for the specified interface
*
* @param    rtrIfNum              @b{(input)}   Router Interface Number
* @param    Timer  @b{(output)}   Querier  Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t mgmdProxyIntfVer1QuerierTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, L7_uint32* timer);

/****************************************************************************
* @pupose  Gets the Proxy Version1 Querier Time for the specified interface
*
* @param    rtrIfNum              @b{(input)}   Router Interface Number
* @param    Timer  @b{(output)}   Querier  Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t mgmdProxyIntfVer2QuerierTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, L7_uint32* timer);

/************************************************************************
*  @purpose  Gets the Proxy Restart Count for the specified interface
*
*  @param    rtrIfNum             @b{(input)}   Router Interface Number
*  @param    count  @b{(output)}  count
* 
*  @returns  L7_SUCCESS
*  @returns  L7_FAILURE
* 
*  @notes    none
* 
*  @end
*************************************************************************/
L7_RC_t mgmdProxyIntfRestartCountGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* count);

/***********************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    *ipAddr           @b{(output)} last reporter ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
**********************************************************************/
L7_RC_t mgmdProxyCacheLastReporterGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *ipAddr);

/***************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache
*           Table for the specified group address & the specified interface
*
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    *upTime           @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t mgmdProxyCacheUpTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *multipaddr,L7_uint32 rtrIfNum,
                                L7_uint32 *upTime);

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
* @purpose  group on the specified interface
*
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *groupFilterMode  @b{(output)} source filter mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Proxy Cache Table
*
* @end
******************************************************************************/
L7_RC_t mgmdProxyCacheGroupFilterModeGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *groupAddr,L7_uint32 rtrIfNum,
                                         L7_uint32 *groupFilterMode);

/**********************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    ipaddr            @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCacheEntryGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group ,L7_uint32 rtrIfNum);

/**********************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given 
*           multi ipaddress & an interface number.
*
* @param    multipaddr   @b{(input)}  multicast group ip address
* @param    rtrIfNum     @b{(input)}  Router Interface Number
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes    if rtrIfNum = 0 & multipaddr = 0, 
*           then return the first valid entry in the cache table
*
* @end
**********************************************************************/
L7_RC_t mgmdProxyCacheEntryNextGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, L7_uint32 *rtrIfNum);

/***********************************************************************
* @purpose  Gets the Proxy v3 queries received on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v3QueriesRcvd       @b{(output)} v3QueriesRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV3QueriesRecvdGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32 *v3QueriesRcvd);

/**************************************************************************** 
* @purpose  Gets the Proxy v3 reports received  for the specified interface
*
* @param    rtrIfNum             @b{(input)}   Router Interface Number
* @param    v3ReportsRcvd        @b{(output)}  v3ReportsRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
****************************************************************************/
L7_RC_t mgmdProxyV3ReportsRecvdGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v3ReportsRcvd);

/*********************************************************************
* @purpose  Gets the Proxy v3 reports Sent for the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v3ReportsSent       @b{(output)}  v3ReportsSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV3ReportsSentGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v3ReportsSent);

/*************************************************************************
* @purpose  Gets the Proxy v2 queries received on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2QueriesRcvd       @b{(output)}  v2QueriesRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t mgmdProxyV2QueriesRecvdGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v2QueriesRcvd);

/*************************************************************************
* @purpose  Gets the Proxy v2 reports received on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2ReportsRcvd       @b{(output)}  v2ReportsRcvd
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV2ReportsRecvdGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, L7_uint32* v2ReportsRcvd);

/***********************************************************************
* @purpose  Gets the Proxy v2 reports sent on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2ReportsSent       @b{(output)}  v2ReportsSent 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV2ReportsSentGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v2ReportsSent);

/*************************************************************************
* @purpose  Gets the Proxy v2 leaves received on the specified interface
*
* @param    rtrIfNum        @b{(input)}   Router Interface Number
* @param    v2LeavesRcvd    @b{(output)} v2LeavesRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV2LeavesRecvdGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32 *v2LeavesRcvd);

/************************************************************************
* @purpose  Gets the Proxy v2 leaves sent on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v2LeavesSent        @b{(output)}  v2LeavesSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*************************************************************************/
L7_RC_t mgmdProxyV2LeavesSentGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v2LeavesSent);

/***************************************************************************
* @purpose  Gets the Proxy v1 queries received on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v1QueriesRcvd       @b{(output)} v1QueriesRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyV1QueriesRecvdGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v1QueriesRcvd);

/*************************************************************************
* @purpose  Gets the Proxy v1 reports received on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v1ReportsRcvd       @b{(output)} v1ReportsRcvd 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t mgmdProxyV1ReportsRecvdGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v1ReportsRcvd);

/************************************************************************
* @purpose  Gets the Proxy v1 reports sent on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    v1ReportsSent       @b{(output)}  v1ReportsSent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**************************************************************************/
L7_RC_t mgmdProxyV1ReportsSentGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* v1ReportsSent);

/*********************************************************************
* @purpose  Get the Proxy interface number configured for
*
* @param    rtrIfNum @b{(input)}   Router Interface Number
* @param    ifindex  @(output) index of the proxy interface
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyInterfaceGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_uint32* ifIndex);

/*********************************************************************
* @purpose  To validate if the specified source address exists for 
*           the specified group address and interface or not
*
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxySrcListEntryGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                 L7_inet_addr_t *host);

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
* @purpose  interface and source address
*
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes    if currentHostIp = 0 return the first source address in that 
*           group address 
*
* @end
******************************************************************************/
L7_RC_t mgmdProxySrcListEntryNextGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, L7_uint32 *rtrIfNum,
                                     L7_inet_addr_t *host);

/*********************************************************************
* @purpose  To get the source expiry time
*
* @param    mgmdCBHndl        @b{(input)}  MGMD CB Handle
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    hostAddr          @b{(input)}  source address
* @param    expiryTime        @b{(output)} source Expiry Time
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxySrcListExpiryTimeGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *group,L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *host, L7_uint32 *srcExpiryTime);

/*********************************************************************
* @purpose  Resets the proxy statistics parameter 
*
* @param    rtrIfNum             @b{(input)}   Router Interface Number
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyResetHostStatus(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum);

/*****************************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    numOfGroups         @b{(output)} Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
******************************************************************************/
L7_RC_t mgmdProxyIntfGroupsGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_int32 *numOfGroups);

/*********************************************************************
* @purpose  To get the state of the member 
*
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    hostState         @b((output)) state of the group address 
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyGroupStatusGet (MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *groupAddr,L7_uint32 rtrIfNum,
                                 L7_uint32 *hostState);

/**********************************************************************
* @purpose  Get the number of source records for the specified group 
*           and interface
*
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    srcCount          @b{(output)} number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t mgmdProxySrcCountGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *groupAddr,L7_uint32 rtrIfNum,L7_uint32 *srcCount);

/*********************************************************************
* @purpose  Gets the Querier for the specified interface
*
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    *querierIP  @b{(output)}  Querier  IP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t mgmdProxyIntfQuerierGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum,L7_inet_addr_t *querierIP);

/*********************************************************************
* @purpose  Gets the status of the row in the host cache table
*
* @param    multipaddrr @b{(input)}   Internal Group Address
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    *status     @b{(output)}  Enabled or Disabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCacheStatusGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, L7_uint32 rtrIfNum,
                                L7_uint32 *status);

/*********************************************************************
* @purpose  Gets the next entry for Proxy Interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyInterfaceEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 *rtrIfNum);

/*********************************************************************
* @purpose  Gets the next entry for Proxy Interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    groupAddr   @b{(output)}  Internal Group Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxymgmdProxyInterfaceGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum, L7_inet_addr_t *groupAddr);

/*********************************************************************
* @purpose  Gets the next entry for Proxy Interface
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    ipaddr      @b{(output)}  Internal Group Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyInverseCacheEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 *rtrIfNum, L7_inet_addr_t *ipaddr);

/*********************************************************************
* @purpose  Get the MGMD  IP Multicast route table Next entry
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                      L7_inet_addr_t* ipMRouteGroup, 
                                      L7_inet_addr_t* ipMRouteSource,
                                      L7_inet_addr_t* ipMRouteSourceMask);




/*********************************************************************
* @purpose  Gets the minimum amount of time remaining before  this
*           entry will be aged out.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteExpiryTimeGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                               L7_inet_addr_t *ipMRtGrp,
                                               L7_inet_addr_t *ipMRtSrc,
                                               L7_inet_addr_t *ipMRtSrcMask,
                                               L7_uint32       *expire);

/*********************************************************************
* @purpose  Gets the time since the multicast routing information
*           represented by this entry was learned by the router.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteUpTimeGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                   L7_inet_addr_t *ipMRtGrp,
                                   L7_inet_addr_t *ipMRtSrc,
                                   L7_inet_addr_t *ipMRtSrcMask,
                                   L7_uint32       *upTime);

/**********************************************************************
* @purpose  Gets the Rpf address for the given index
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
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
L7_RC_t mgmdProxyIpMRouteRpfAddrGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_inet_addr_t *rpfAddress);

/**********************************************************************
* @purpose  Gets the number of MRT entries in Proxy table.
*
* @param    mgmdCBHndl   @b{(input)} MGMD CB Handle
* @param    count        @b{(input)} Number of MRT entries in Proxy table
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyIpMrouteEntryCountGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                       L7_uint32 *count);
/*********************************************************************
* @purpose  Gets the interface index on which IP datagrams sent by
*           these sources to this multicast address are received.
*           corresponding to the index received.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    ipMRtGrp       @b{(input)}  Multicast Group address
* @param    ipMRouteSrc    @b{(input)}  Source address
* @param    ipMRtSrcMask   @b{(input)}  Mask Address
* @param    rtrIfNum       @b{(output)} router interface number
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
L7_RC_t mgmdProxyIpMRouteIfIndexGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_uint32      *rtrIfNum);

/*********************************************************************
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    mgmdCBHndl  @b{(input)}   MGMD CB Handle
* @param    ipMRtGrp           @b{(input)} mcast group
* @param    ipMRtSrc           @b{(input)} mcast source
* @param    ipMRtSrcMask       @b{(input)} source mask
* @param    rtrIfNum           @b{(inout)} router interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mgmdProxyRouteOutIntfEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl,
                                          L7_inet_addr_t *ipMRtGrp,
                                          L7_inet_addr_t *ipMRtSrc,
                                          L7_inet_addr_t *ipMRtSrcMask,
                                          L7_uint32      *rtrIfNum);
/**********************************************************************
* @purpose  To enable/disable multicast data reception in stack for the 
*           specified interface  
*
* @param    mgmdCB            @b{ (input) }   MGMD Control Block
* @param    rtrIfNum          @b{ (input) }   Router interface Number
* @param    mode              @b{ (input) }   L7_ENABLE/ L7_DISABLE
*
* @returns  None
*
* @notes    This function is specific to IPv6 only.
* @end
*********************************************************************/
L7_RC_t mgmd_proxy_MfcIntfEventQueue(L7_uchar8 family, L7_uint32 rtrIfNum, 
                                     L7_uint32 mode);
/*********************************************************************
* @purpose  Processes all configuration events
*
* @param    mgmd_map_events_params_t @b{ (input) } mapping event info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyProtocolConfigSet(mgmdMapEventParams_t *eventParams);
