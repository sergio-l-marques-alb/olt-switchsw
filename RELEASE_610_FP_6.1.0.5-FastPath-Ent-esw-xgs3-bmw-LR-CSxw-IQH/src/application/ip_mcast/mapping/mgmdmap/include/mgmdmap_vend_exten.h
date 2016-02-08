/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    mgmd_vend_exten.h
*
* @purpose     MGMD  vendor-specific API functions
*
* @component   MGMD Mapping Layer
*
* @comments
*
* @create      02/06/2002
*
* @author      ramakrishna
*
* @end
*
**********************************************************************/

#ifndef _MGMD_VEND_EXTEN_H_
#define _MGMD_VEND_EXTEN_H_

#include "mgmd.h"

/*---------------------------------------------------*/
/* MGMD Mapping Layer vendor API function prototypes */
/*---------------------------------------------------*/

/*-------------------*/
/* mgmd_vend_exten.c */
/*-------------------*/

/*********************************************************************
* @purpose  Sets the Mgmd Admin mode
*
* @param    mgmdMapCbPtr   @b{(input)} Mapping Control Block.
* @param    mode           @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenMgmdAdminModeSet(mgmdMapCB_t *mgmdMapCbPtr,
                                     L7_uint32 mode, L7_BOOL doInit);

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
                                L7_uint32 mode,L7_MGMD_INTF_MODE_t intfType);

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
                                            L7_BOOL mode);

/*******************************************************************
* @purpose  Sets the igmp router Interface Version 
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
                            L7_uint32 intIfNum, L7_uint32 version);

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
L7_RC_t  mgmdMapExtenIntfRobustnessSet(mgmdMapCB_t *mgmdMapCbPtr, 
                                       L7_uint32 intIfNum,
                                       L7_uint32 robustness);
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
L7_RC_t  mgmdMapExtenIntfQueryIntervalSet(mgmdMapCB_t *mgmdMapCbPtr, 
                                          L7_uint32 intIfNum,
                                          L7_uint32 queryInterval);
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
L7_RC_t  mgmdMapExtenIntfQueryMaxRespTimeSet(mgmdMapCB_t *mgmdMapCbPtr,
                                             L7_uint32 intIfNum,
                                             L7_uint32 queryMaxRespTime);
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
                                                 L7_uint32 startupQueryInterval);
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
                                             L7_uint32 startupQueryCount);
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
                                                   L7_uint32 lastMemQueryInterval);
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
                                                L7_uint32 lastMemQueryCount);
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
                                   L7_inet_addr_t *querierIP);

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
                                         L7_uint32* querierUpTime);

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
                                             L7_uint32* querierExpiryTime);

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
                                               L7_uint32* wrongVerQueries);

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
L7_RC_t mgmdMapExtenIntfJoinsGet(mgmdMapCB_t *mgmdMapCbPtr,L7_uint32 intIfNum, 
                                 L7_uint32* intfJoins);

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
                                  L7_uint32* numOfGroups);

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
                                         L7_uint32 *status);

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
                                     L7_uint32 *pIntIfNum);

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
                                         L7_inet_addr_t *ipAddr);

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
                                   L7_uint32 intIfNum,  L7_uint32 *upTime);

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
                                       L7_uint32 *expTime);

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
                                              L7_uint32 *version1Time);

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
                                  L7_uint32 intIfNum);

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
                                      L7_uint32 *pIntIfNum);

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
                                              L7_uint32 *version2Time);

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
                                            L7_uint32 *groupCompatMode);

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
                                                  L7_uint32 *sourceFilterMode);

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
                                      L7_inet_addr_t *groupAddr);

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
                                          L7_inet_addr_t *ipaddr);

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
                                    L7_inet_addr_t *hostAddr);

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
                                        L7_inet_addr_t *hostAddr);

/*****************************************************************************
* @purpose  Get the Next valid Group and Interface entry in the order of (Grp, Intf)
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    ipaddr            @b{(inout)}   multicast group ip address
* @param    pIntIfNum         @b{(inout)}   Internal Interface Number
*
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenGrpIntfEntryNextGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_inet_addr_t *ipaddr,
                                        L7_uint32 *pIntIfNum);

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
                  L7_uint32 *srcExpiryTime);

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
            L7_uint32 *srcCount);

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Received for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    validPkts        @b{(output)} Valid Packets Received.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenValidPacketsRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_uint32 *validPkts);

/********************************************************************
* @purpose  Get the Valid MLD Pakcets Sent for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    pktsSent         @b{(output)} Valid Packets Sent.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenValidPacketsSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                        L7_uint32 *pktsSent);

/********************************************************************
* @purpose  Get the Queries Received for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    quriesRcvd       @b{(output)} Queries Rcvd.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenQueriesRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *quriesRcvd);

/********************************************************************
* @purpose  Get the Queries Sent for the router.
*
* @param    mgmdMapCbPtr     @b{(input)} Mapping Control Block.
* @param    quriesSent       @b{(output)}Queries Sent.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenQueriesSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *quriesSent);

/********************************************************************
* @purpose  Get the Reports Received for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    reportsRcvd      @b{(output)} Reports Received.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenReportsRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *reportsRcvd);

/********************************************************************
* @purpose  Get the Reports Sent for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    reportsSent      @b{(output)} Reports Sent.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @domments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenReportsSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                   L7_uint32 *reportsSent);

/********************************************************************
* @purpose  Get the Leaves Received for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    leavesRcvd       @b{(output)} Leaves Received.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenLeavesRcvdGet(mgmdMapCB_t *mgmdMapCbPtr,
                                  L7_uint32 *leavesRcvd);

/********************************************************************
* @purpose  Get the Leaves Sent for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    leavesSent       @b{(output)} Leaves Sent.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenLeavesSentGet(mgmdMapCB_t *mgmdMapCbPtr,
                                  L7_uint32 *leavesSent);

/********************************************************************
* @purpose  Get the BadCheckSum Pkts for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    badChkSum        @b{(output)} Bad Checksum Packets.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenBadCheckSumPktsGet(mgmdMapCB_t *mgmdMapCbPtr,
                                       L7_uint32 *badChkSum);

/********************************************************************
* @purpose  Get the Malformed Pkts for the router.
*
* @param    mgmdMapCbPtr     @b{(input)}  Mapping Control Block.
* @param    malfrmedPkts     @b{(output)} Malformed packets.
*
* @returns  L7_SUCCESS       if an extry exists
* @returns  L7_FAILURE       if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapExtenMalformedPktsGet(mgmdMapCB_t *mgmdMapCbPtr,
                                     L7_uint32 *malfrmedPkts);

/********************************************************************
* @purpose  Clear the Mld Counters for MLD.
*
* @param    mgmdMapCbPtr      @b{(input)}  Mapping Control Block.
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
                                  L7_uint32 intIfNum);

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
L7_RC_t mgmdMapExtenTrafficCountersClear(mgmdMapCB_t *mgmdMapCbPtr);

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
                                   L7_uint32* version);

/*********************************************************************
* @purpose  Get the interface query interval for the specific router Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    interval    @b{(output)} Interface Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceQueryIntervalGet(L7_uchar8 familyType, 
                                                 L7_uint32 rtrIfNum, 
                                                 L7_uint32 *interval);
/*********************************************************************
* @purpose  Get the Query Max Response Time configured for the 
*           router Interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    rtrIfNum     @b{(input)} router Interface Number
* @param    maxRespTime  @b{(output)} Max Response Time on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceQueryMaxResponseTimeSecondsGet(L7_uchar8 familyType,
               L7_uint32 intIfNum, L7_uint32* maxRespTime);

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
               L7_uint32 rtrIfNum, L7_uint32* maxRespTime);

/*********************************************************************
* @purpose  Get the Robustness variable configured for the router Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    robustness  @b{(output)} Robustness variable on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceRobustnessGet(L7_uchar8 familyType, L7_uint32 intIfNum,
                                              L7_uint32* robustness) ;
/*********************************************************************
* @purpose  Get the Last Member Query Interval configured for the Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    interval    @b{(output)} Last Member Query Interval on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceLastMembQueryIntervalSecondsGet(L7_uchar8 familyType,
                                                         L7_uint32 intIfNum, 
                                                         L7_uint32* interval);
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
                                                         L7_uint32* interval);
/*********************************************************************
* @purpose  Set the Last Member Query Count for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    queryCount  @b{(input)} Last Member Query Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceLastMembQueryCountGet(L7_uchar8 familyType,
                                                      L7_uint32 intIfNum, 
                                                      L7_uint32 *queryCount);
/*********************************************************************
* @purpose  Get the Startup Query Interval configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    interval    @b{(output)} Startup Query Interval on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceStartupQueryIntervalGet(L7_uchar8 familyType,
                                                        L7_uint32 intIfNum,
                                                        L7_uint32* interval);
/*********************************************************************
* @purpose  Get the Startup Query Count configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} router Interface Number
* @param    queryCount  @b{(output)} Startup Query Count on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProtocolInterfaceStartupQueryCountGet(L7_uchar8 familyType,
                                                     L7_uint32 intIfNum,
                                                     L7_uint32* queryCount);

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
                                    MCAST_CB_HNDL_t *cbHandle);

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
                                    MCAST_CB_HNDL_t *cbHandle);

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
                                                 mgmdGroupInfo_t  *grpInfo);


/*********************************************************************
* @purpose  Obtain Multicast HeapID
* 
* @param    family @b{(input)} IP Address family
*
* @returns  heapId, on success
* @returns  L7_NULL, on failure
*
* @comments This heapID is to be used for all the memory allocations for the
*           given IP address family within the Multicast module
*
* @end
*********************************************************************/
L7_uint32 mgmdMapHeapIdGet (L7_uint32 family);

#endif
