/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  mgmd_vend_api.h
*
* @purpose   MGMD vendor-specific API function prototypes
*
* @component Mgmd Vendor APIs
*
* @comments  none
*
* @create    April 06, 2006
*
* @author    akamlesh
*
* @end
*
**********************************************************************/

#ifndef _MGMD_API_H
#define _MGMD_API_H

typedef enum _mgmd_counter
{
  L7_MGMD_TOTAL_PKTS_RECV,
  L7_MGMD_TOTAL_PKTS_SENT,
  L7_MGMD_QUERIES_RECV,
  L7_MGMD_QUERIES_SENT,
  L7_MGMD_REPORTS_RECV,
  L7_MGMD_REPORTS_SENT,
  L7_MGMD_LEAVES_RECV,
  L7_MGMD_LEAVES_SENT,
  L7_MGMD_BAD_CKSM_PKTS,
  L7_MGMD_MALFORM_PKTS  
} mgmd_counter;

typedef struct mgmdIntfConfig_s
{
  L7_uint32 version;
  L7_uint32 robustness;
  L7_uint32 queryInterval;
  L7_uint32 queryResponseInterval;
  L7_uint32 startupQueryInterval;
  L7_uint32 startupQueryCount;
  L7_uint32 lastMemQueryInterval;
  L7_uint32 lastMemQueryCount;
  L7_uint32 unsolicitedReportInterval;
}mgmdIntfConfig_t;

typedef struct mgmdMapEventParams_s
{
  L7_uint32  rtrIfNum;
  L7_uchar8  familyType;
  L7_uint32  intfType;   /* MGMD or PROXY interface */
  L7_uint32  eventType;
  L7_uint32  eventInfo;  
  mgmdIntfConfig_t *mgmdIntfInfo;
} mgmdMapEventParams_t;

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
*********************************************************************/
L7_RC_t mgmdIntfQuerierGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, 
                                          L7_inet_addr_t *querierIP);


/*********************************************************************
* @purpose  Gets the Querier Up Time for the specified interface
*
* @param    rtrIfNum        @b{(input)}   Router Interface Number
* @param    querierUpTime   @b{(output)}  Querier  Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfQuerierUpTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, 
                                                  L7_uint32* querierUpTime);

/*********************************************************************
* @purpose  Gets the Querier Expiry Time for the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    querierExpiryTime   @b{(output)}  Querier  Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfQuerierExpiryTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, 
                                                  L7_uint32* querierExpiryTime);

/*********************************************************************
* @purpose  Gets the Wrong Version Queries for the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    wrongVerQueries     @b{(output)}  Wrong Version Queries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfWrongVersionQueriesGet(MCAST_CB_HNDL_t mgmdCB, 
                        L7_uint32 rtrIfNum, L7_uint32* wrongVerQueries);

/*********************************************************************
* @purpose  Gets the Number of Joins on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    intfJoins           @b{(output)}  Number of Joins on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfJoinsGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, 
                                                L7_uint32* intfJoins);

/*********************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    rtrIfNum            @b{(input)}   Router Interface Number
* @param    numOfGroups         @b{(output)}  Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfGroupsGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, 
                                             L7_uint32* numOfGroups);

/*********************************************************************
* @purpose  Gets the Querier Status for the specified interface
*
* @param    rtrIfNum    @b{(input)}   Router Interface Number
* @param    status      @b{(output)}  Querier  Status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfQuerierStatusGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, 
                                                         L7_uint32 *status);

/**************************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}  Router Interface Number
* @param    *ipAddr           @b{(output)} last reporter ip
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheLastReporterGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                                    L7_uint32 rtrIfNum, L7_inet_addr_t *ipAddr);

/**************************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache Table for
*           the specified group address & the specified interface
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *upTime           @b{(output)} Up time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheUpTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                                    L7_uint32 rtrIfNum, L7_uint32 *upTime);

/**************************************************************************************
* @purpose  Gets the  Expiry time before the specified entry in Cache Table will be aged out
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *expTime          @b{(output)} Expiry Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheExpiryTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                                      L7_uint32 rtrIfNum, L7_uint32 *expTime);

/**************************************************************************************
* @purpose  Gets the  time remaining until the router assumes there are no longer
* @purpose  any MGMD version 1 Hosts on the specified interface
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *version1Time     @b{(output)} version1 Host Expiry Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheVersion1HostTimerGet(MCAST_CB_HNDL_t mgmdCB, 
                                      L7_inet_addr_t *group, 
                                      L7_uint32 rtrIfNum, 
                                      L7_uint32 *version1Time);

/**************************************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    ipaddr            @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exists
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheEntryGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                                                    L7_uint32 rtrIfNum);

/**************************************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given multi 
*           ipaddress & an interface number.
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exists
*
* @notes    if rtrIfNum = 0 & group = 0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheEntryNextGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                                                       L7_uint32 *rtrIfNum);

/***************************************************************************
* @purpose  Gets the time remaining until the router assumes there are no
* @purpose  longer any MGMD version 2 Hosts for the specified group on the
* @purpose  specified interface
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *version2Time     @b{(output)} version1 Host Expiry Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheVersion2HostTimerGet(MCAST_CB_HNDL_t mgmdCB, 
                                      L7_inet_addr_t *group, 
                                      L7_uint32 rtrIfNum, 
                                      L7_uint32 *version2Time);

/***************************************************************************
* @purpose  Gets the compatibility mode (v1, v2 or v3) for the specified
* @purpose  group on the specified interface
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *groupCompatMode  @b{(output)} group compatibility mode
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheGroupCompatModeGet(MCAST_CB_HNDL_t mgmdCB, 
                                    L7_inet_addr_t *group, 
                                    L7_uint32 rtrIfNum, 
                                    L7_uint32 *groupCompatMode);

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
* @purpose  group on the specified interface
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    *sourceFilterMode @b{(output)} source filter mode
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheGroupSourceFilterModeGet(MCAST_CB_HNDL_t mgmdCB, 
                                          L7_inet_addr_t *group, 
                                          L7_uint32 rtrIfNum, 
                                          L7_uint32 *sourceFilterMode);

/*****************************************************************************
* @purpose  To validate if the specified interface contains a cache entry for
* @purpose  the specified group or not
*
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    group             @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes    Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheIntfEntryGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 rtrIfNum, 
                                                  L7_inet_addr_t *group);

/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
* @purpose  interface
*
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    ipaddr            @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes    if rtrIfNum=0 & group=0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdCacheIntfEntryNextGet(MCAST_CB_HNDL_t mgmdCB, L7_uint32 *rtrIfNum, 
                                                       L7_inet_addr_t *group);

/*****************************************************************************
* @purpose  Get the Next valid Group and Interface entry in the order of (Grp, Intf)
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    group             @b{(inout)}   multicast group ip address
* @param    rtrIfNum          @b{(inout)}   Router Interface Number
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
*
* @end
*********************************************************************/

L7_RC_t mgmdGrpIntfEntryNextGet(MCAST_CB_HNDL_t mgmdCBHndl, L7_inet_addr_t *searchKeyGroup, L7_uint32 *searchKeyRtrIfNum);

/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
* @purpose  group address and interface or not
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    host              @b{(input)}  source address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcListEntryGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                                 L7_uint32 rtrIfNum, L7_inet_addr_t *host);

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
* @purpose  interface and source address
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    host              @b{(input)}  source address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @notes    if rtrIfNum=0, group=0 & host=0, then return the first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcListEntryNextGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                                   L7_uint32 *rtrIfNum, L7_inet_addr_t *host);

/*******************************************************************************
* @purpose  Get the amount of time until the specified Source Entry is aged out
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    host              @b{(input)}  source address
* @param    srcExpiryTime     @b{(output)} source expiry time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcExpiryTimeGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                             L7_uint32 rtrIfNum, L7_inet_addr_t *host, 
                             L7_uint32 *srcExpiryTime);

/*******************************************************************************
* @purpose  Get the number of source records for the specified group and interface
*
* @param    group             @b{(input)}  multicast group ip address
* @param    rtrIfNum          @b{(input)}   Router Interface Number
* @param    srcCount          @b{(output)} number of sources
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmdSrcCountGet(MCAST_CB_HNDL_t mgmdCB, L7_inet_addr_t *group, 
                        L7_uint32 rtrIfNum, L7_uint32 *srcCount);

/*********************************************************************
*
* @purpose  De-Initialize the MGMD protocol control block
*
* @param    MGMD Control Block

*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This module assumes that mgmdCB was initialized and memory
*           allocated for mgmdCB
*
* @end
*********************************************************************/
L7_RC_t mgmdCtrlBlockDeInit (MCAST_CB_HNDL_t cbHandle);

/*********************************************************************
*
* @purpose  Initialize the MGMD protocol control block and
*           elements within MGMD CB
*
* @param    Nothing
*
* @returns  if success, MCAST_CB_HNDL_t - pointer to MGMD CB
* @returns  if failure, NULL
*
* @notes
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t mgmdCtrlBlockInit (L7_uchar8 familyType);

/*********************************************************************
*
* @purpose  To Initialize the Memory for MGMD Protocol
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
L7_RC_t mgmdMemoryInit (L7_uchar8 addrFamily);

/*********************************************************************
*
* @purpose  To De-Initialize the Memory for MGMD Protocol
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
L7_RC_t mgmdMemoryDeInit (L7_uchar8 addrFamily);

/*********************************************************************
*
* @purpose  MGMD Event process.
*
* @param    event   @b{ (input) } Event.
* @param    pMsg    @b{ (input) } Message.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mgmd_event_process (L7_uchar8 familyType, L7_uint32 event, void *pMsg);

/*********************************************************************
* @purpose  Gets thecorresponding counter value
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
* @param    counterType           @b{(output)}  Type of the counter
*                 value                         value of the counter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdCounterGet(MCAST_CB_HNDL_t mgmdCBHndl,L7_uint32 counterType,L7_uint32* value);

/*********************************************************************
* @purpose  Clears all the global counters
*
* @param    mgmdCBHndl          @b{(input)}   MGMD CB Handle
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdTrafficCountersClear(MCAST_CB_HNDL_t mgmdCBHndl);

/*********************************************************************
* @purpose  Clears all mld interface counters
*
* @param    mgmdCBHndl        @b{(input)}   MGMD CB Handle
* @param    rtrIntf           @b{(input)}   router Interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdCountersClear(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIntf);

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
L7_RC_t mgmdProtocolConfigSet(mgmdMapEventParams_t *eventParams);

#endif /* _MGMD_API_H */


