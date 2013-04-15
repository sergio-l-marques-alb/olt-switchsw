/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  l7_mgmd_api.h
*
* @purpose   The purpose of this file is to have a central location for
* @purpose   all mgmdMap includes and definitions.
*
* @component mgmdMap Mapping Layer
*
* @comments  none
*
* @create    02/05/2002
*
* @author    ramakrishna
* @end
*
**********************************************************************/
#ifndef _MGMDMAP_API_H
#define _MGMDMAP_API_H

#include "l3_comm_structs.h"
#include "async_event_api.h"
#include "ip_1213_api.h"
#include "l3_mcast_commdefs.h"

#define MGMD_MAX_QUERY_SOURCES      L7_MGMD_MAX_QUERY_SOURCES

#define IGMP_IP_ROUTER_ALERT_TYPE   0x94
#define IGMP_IP_ROUTER_ALERT_LENGTH 4

typedef struct mrp_source_record_s
{
  L7_inet_addr_t      sourceAddr;
  L7_uchar8           filterMode; /* INCLUDE or EXCLUDE */
  L7_uchar8           action;      /* ADD or DELETE */
}mrp_source_record_t;

typedef struct mgmdMrpEventInfo_s
{
  L7_uint32           rtrIfNum;
  L7_uint32           mode;
  L7_inet_addr_t      group;
  L7_uint32           numSrcs;
  mrp_source_record_t **sourceList;
}mgmdMrpEventInfo_t;

typedef enum
{
  MGMD_FILTER_MODE_NONE = 0,
  MGMD_FILTER_MODE_INCLUDE = 1,
  MGMD_FILTER_MODE_EXCLUDE = 2
} MGMD_FILTER_MODE_T;

typedef enum mgmMrtEvents_s
{
  MGMD_SOURCE_ADD,
  MGMD_SOURCE_DELETE,
  MGMD_QUERIER,
  MGMD_NON_QUERIER
}mgmMrtEvents_t;

typedef enum _mgmd_version
{
  L7_MGMD_VERSION_1=1,
  L7_MGMD_VERSION_2,
  L7_MGMD_VERSION_3
} mgmd_version; 

typedef enum
{
  MGMD_HOST_NON_MEMBER   = 1,
  MGMD_HOST_DELAY_MEMBER = 2,
  MGMD_HOST_IDLE_MEMBER  = 3
}MGMD_HOST_STATE_T;


/*
***********************************************************************
*                     API FUNCTIONS  -  MGMD UTILITY FUNCTIONS
***********************************************************************
*/

/********************************************************************
* @purpose  Get the MGMD administrative mode
*
* @param    familyType  @b{(input)} Address Family type
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapAdminModeGet(L7_uchar8 familyType, L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the MGMD administrative mode
*
* @param    familyType  @b{(input)} Address Family type
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapAdminModeSet(L7_uchar8 familyType, L7_uint32 mode);

/*********************************************************************
* @purpose  Sets whether the optional Router Alert field is required.
*
* @param    familyType  @b{(input)} Address Family type
* @param    checkRtrAlert       @b{(input)}   True if Router Alert required
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRouterAlertMandatorySet(L7_uchar8 familyType, L7_BOOL checkRtrAlert);

/*********************************************************************
* @purpose  Get whether the optional Router Alert field is required.
*
* @param    familyType  @b{(input)} Address Family type
* @param    checkRtrAlert       @b{(input)}   True if Router Alert required
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRouterAlertMandatoryGet(L7_uchar8 familyType, L7_BOOL *checkRtrAlert);

/*********************************************************************
* @purpose  Get the administrative mode of an MGMD interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceModeGet(L7_uchar8 familyType,
                                L7_uint32 intIfNum, L7_uint32* mode);
/*********************************************************************
* @purpose  Set the administrative mode of an MGMD interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
* @param    proxy_or_router @b{(input)} proxy or router
* @param    errCode     @b{(input)} error code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceModeSet(L7_uchar8 familyType,
                                L7_uint32 intIfNum,
                                L7_uint32 mode,
                                L7_MGMD_INTF_MODE_t proxy_or_router,
                                L7_MGMD_ERROR_CODE_t *errCode);

/*********************************************************************
* @purpose  Get the interface query interval for the specific Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapInterfaceQueryIntervalGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *interval);

/*********************************************************************
* @purpose  Set the query interval value for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(input)} Query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQueryIntervalSet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32 interval);

/*********************************************************************
* @purpose  Get the MGMD Version configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    version     @b{(output)} MGMD Version on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceVersionGet(L7_uchar8 familyType,
                                   L7_uint32 intIfNum,
                                   L7_uint32* version);

/*********************************************************************
* @purpose  Set the MGMD Version for this interface
*
* @param    familyType @b{(input)} Address Family type
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    version    @b{(input)} MGMD Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceVersionSet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                   L7_uint32 version);

/*********************************************************************
* @purpose  Get the Query Max Response Time configured for the Interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    intIfNum     @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapInterfaceQueryMaxResponseTimeGet(L7_uchar8 familyType,
               L7_uint32 intIfNum, L7_uint32* maxRespTime);

/*********************************************************************
* @purpose  Set the Query Max Response Time for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    maxRespTime @b{(input)} Query Max Response Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQueryMaxResponseTimeSet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum, L7_uint32 maxRespTime);

/*********************************************************************
* @purpose  Get the Robustness variable configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapInterfaceRobustnessGet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                      L7_uint32* robustness);

/*********************************************************************
* @purpose  Set the Robustness variable for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    robustness  @b{(input)} Robustness variable
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceRobustnessSet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                      L7_uint32 robustness);

/*********************************************************************
* @purpose  Get the Last Member Query Interval configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapInterfaceLastMembQueryIntervalGet(L7_uchar8 familyType,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32* interval);

/*********************************************************************
* @purpose  Set the Last Member Query Interval for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(input)} Last Member Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceLastMembQueryIntervalSet(L7_uchar8 familyType,
                                                 L7_uint32 intIfNum,
                                                 L7_uint32 interval);

/*********************************************************************
* @purpose  Get the Last Member Query Count configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    queryCount  @b{(output)} Last Member Query Count on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceLastMembQueryCountGet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum,
                                     L7_uint32* queryCount);

/*********************************************************************
* @purpose  Set the Last Member Query Count for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapInterfaceLastMembQueryCountSet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum,
                                     L7_uint32 queryCount);

/*********************************************************************
* @purpose  Get the Startup Query Interval configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapInterfaceStartupQueryIntervalGet(L7_uchar8 familyType,
                                                L7_uint32 intIfNum,
                                                L7_uint32* interval);

/*********************************************************************
* @purpose  Set the Startup Query Interval for this interface
*
* @param    familyType@b{(input)} Address Family type
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    interval  @b{(input)} Startup  Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceStartupQueryIntervalSet(L7_uchar8 familyType,
                                                L7_uint32 intIfNum,
                                                L7_uint32 interval);

/*********************************************************************
* @purpose  Get the Startup Query Count configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapInterfaceStartupQueryCountGet(L7_uchar8 familyType,
                                             L7_uint32 intIfNum,
                                             L7_uint32* queryCount);

/*********************************************************************
* @purpose  Set the Startup Query Count for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    queryCount  @b{(input)} Startup Query Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceStartupQueryCountSet(L7_uchar8 familyType,
                                             L7_uint32 intIfNum,
                                             L7_uint32 queryCount);

/*********************************************************************
* @purpose  Get the Interface Querier on the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    querierIP   @b{(output)} Querier Address on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierGet(L7_uchar8 familyType,
                                   L7_uint32 intIfNum,
                                   L7_inet_addr_t* querierIP);

/*********************************************************************
* @purpose  Get the Interface Querier UpTime on the Interface
*
* @param    familyType      @b{(input)} Address Family type
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    querierUpTime   @b{(output)} Querier UpTime on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierUpTimeGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32* querierUpTime);

/*********************************************************************
* @purpose  Get the Interface Querier Expiry Time on the Interface
*
*                                            the interface
* @param    familyType          @b{(input)} Address Family type
* @param    intIfNum            @b{(input)}  Internal Interface Number
* @param    querierExpiryTime   @b{(output)} Querier ExpiryTime on
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierExpiryTimeGet(L7_uchar8 familyType,
                                             L7_uint32 intIfNum,
                                             L7_uint32* querierExpiryTime);

/*********************************************************************
* @purpose  Get the Interface Wrong Version Queries  on the Interface
*
* @param    familyType        @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  Internal Interface Number
* @param    wrongVerQueries   @b{(output)}Wrong Version Queries on theinterface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceWrongVersionQueriesGet(L7_uchar8 familyType,
                                               L7_uint32 intIfNum,
                                               L7_uint32* wrongVerQueries);

/*********************************************************************
* @purpose  Get the Number Of Joins  on the Interface
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    intfJoins     @b{(output)}Num of Joins on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceJoinsGet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum,
                                 L7_uint32* intfJoins);

/*********************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    familyType          @b{(input)} Address Family type
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    numOfGroups         @b{(output)}Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceGroupsGet(L7_uchar8 familyType,
                                  L7_uint32 intIfNum,
                                  L7_uint32* numOfGroups);

/*********************************************************************
* @purpose  Gets the Querier Status for the specified interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    status      @b{(output)} Querier  Status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceQuerierStatusGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *status);

/*********************************************************************
* @purpose  Gets the Next Valid Interface for MGMD
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceEntryNextGet(L7_uchar8 familyType,
                                     L7_uint32 *intIfNum);

/* cache parameters*/
/*******************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*       received for the specified group address on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    ipAddr            @b{(output)} last reporter ip
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
****************************************************************************/
L7_RC_t mgmdMapCacheLastReporterGet(L7_uchar8 familyType,
                                    L7_inet_addr_t *multipaddr,
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *ipAddr);

/****************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the
*           Cache Table for
*           the specified group address & the specified interface
* @param    familyType        @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    upTime            @b{(output)} Up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheUpTimeGet(L7_uchar8 familyType,
                              L7_inet_addr_t *multipaddr,
                              L7_uint32 intIfNum,
                              L7_uint32 *upTime);

/**************************************************************************************
* @purpose  Gets the  Expiry time before the specified entry in Cache Table will be aged out
*
* @param    familyType        @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    expTime           @b{(output)} Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheExpiryTimeGet(L7_uchar8 familyType,
                                  L7_inet_addr_t *multipaddr,
                                  L7_uint32 intIfNum,
                                  L7_uint32 *expTime);

/**************************************************************************************
* @purpose  Gets the  time remaining until the router assumes there are no
*           longer any MGMD version 1 Hosts on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    version1Time      @b{(output)} version1 Host Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheVersion1HostTimerGet(L7_uchar8 familyType,
                                         L7_inet_addr_t *multipaddr,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *version1Time);

/**************************************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    familyType        @b{(input)}  Address Family type
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheEntryGet (L7_uchar8 familyType,
                              L7_inet_addr_t *multipaddr,
                              L7_uint32 intIfNum);

/**************************************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given multi 
*           ipaddress & an interface number.
* @
*
* @param    familyType        @b{(input)}  Address Family type
* @param    pIntIfNum         @b{(input)}  internal Interface number
* @param    multipaddr        @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum = 0 & multipaddr = 0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheEntryNextGet (L7_uchar8 familyType,
                                  L7_inet_addr_t *multipaddr,
                                  L7_uint32 *pIntIfNum);

/***************************************************************************
* @purpose  Gets the compatibility mode (v1, v2 or v3) for the specified
*           group on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
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
L7_RC_t mgmdMapCacheGroupCompatModeGet(L7_uchar8 familyType,
                                       L7_inet_addr_t *groupAddr,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *groupCompatMode);

/***************************************************************************
* @purpose  Gets the time remaining until the router assumes there are no
*           longer any MGMD version 2 Hosts for the specified group on the
*           specified interface
*
* @param    familyType        @b{(input)}  Address Family type
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
L7_RC_t mgmdMapCacheVersion2HostTimerGet(L7_uchar8 familyType,
                                         L7_inet_addr_t *groupAddr,
                                         L7_uint32 intIfNum,
                                         L7_uint32 *version2Time);

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
*           group on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    sourceFilterMode  @b{(output)} source filter mode
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheGroupSourceFilterModeGet(L7_uchar8 familyType,
                                             L7_inet_addr_t *groupAddr,
                                             L7_uint32 intIfNum,
                                             L7_uint32 *sourceFilterMode);

/*****************************************************************************
* @purpose  To validate if the specified interface contains a cache entry for
*           the specified group or not
*
* @param    familyType        @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheIntfEntryGet(L7_uchar8 familyType,L7_uint32 intIfNum,
                                 L7_inet_addr_t *groupAddr);

/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
*           interface
*
* @param    familyType         @b{(input)}  Address Family type
* @param    pIntIfNum          @b{(input)}  internal Interface number
* @param    ipaddr             @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum=0 & groupAddr=0, then return the first valid
*           entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheIntfEntryNextGet(L7_uchar8 familyType,L7_uint32 *pIntIfNum,
                                     L7_inet_addr_t *ipaddr);

/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
*           group address and interface or not
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapSrcListEntryGet(L7_uchar8 familyType,
                               L7_inet_addr_t *groupAddr,
                               L7_uint32 intIfNum,
                               L7_inet_addr_t *hostAddr);

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
*           interface and source address
*
* @param    familyType         @b{(input)}  Address Family type
* @param    groupAddr          @b{(input)}  multicast group ip address
* @param    pIntIfNum          @b{(input)}  internal Interface number
* @param    hostAddr           @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0,
*           then return the first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapSrcListEntryNextGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *groupAddr,
                                   L7_uint32 *pIntIfNum,
                                   L7_inet_addr_t *hostAddr);

/*****************************************************************************
* @purpose  Get the Next  group address and interface
*
* @param    familyType         @b{(input)}  Address Family type
* @param    groupAddr          @b{(input)}  multicast group ip address
* @param    pIntIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum=0 and groupAddr=0
*           then return the first valid  entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapGrpIntfEntryNextGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *groupAddr,
                                   L7_uint32 *pIntIfNum);

/*************************************************************************
* @purpose  Get the amount of time until the specified Source Entry is aged out
*
* @param    familyType        @b{(input)}  Address Family type
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
L7_RC_t mgmdMapSrcExpiryTimeGet(L7_uchar8 familyType,L7_inet_addr_t *groupAddr,
                                L7_uint32 intIfNum, L7_inet_addr_t *hostAddr,
                                L7_uint32 *srcExpiryTime);

/**************************************************************************
* @purpose  Get the number of source records for the specified group
*           and interface
*
* @param    familyType        @b{(input)}  Address Family type
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
L7_RC_t mgmdMapSrcCountGet(L7_uchar8 familyType,
                           L7_inet_addr_t *groupAddr,
                           L7_uint32 intIfNum,
                           L7_uint32 *srcCount);

/*********************************************************************
* @purpose  Determine if the MGMD interface has been initialized
*
* @param    familyType    @b{(input)} Address Family type
* @param    intIfNum      @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdMapMgmdIsOperational(L7_uchar8 familyType);
L7_RC_t mgmdMapMgmdIntfInitialized(L7_uchar8 familyType,L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine if the interface type is valid in MGMD
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL mgmdIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in MGMD
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL mgmdIntfIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the proxy Interface Querier on the Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    querierIP   @b{(output)} Querier Address on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceQuerierGet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t* querierIP);

/*********************************************************************
* @purpose  Get the version 1 Querier time on the mgmd proxy Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    timer       @b{(output)} Querier Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceVer1QuerierTimerGet (L7_uchar8 familyType,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32* timer);

/*********************************************************************
* @purpose  Get the version 2 Querier time on the mgmd proxy Interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    timer       @b{(output)} Querier Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceVer2QuerierTimerGet (L7_uchar8 familyType,
                                                  L7_uint32 intIfNum,
                                                  L7_uint32* timer);

/*********************************************************************
* @purpose  Gets the Next Valid Interface for MGMD-PROXY
*
* @param    familyType  @b{(input)} Address Family type
* @param    pIntIfNum   @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceEntryNextGet(L7_uchar8 familyType,
                                          L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Gets the Number of Multicast Groups on the specified interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    numOfGroups   @b{(output)} Number of Groups on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceNumMcastGroupsGet(L7_uchar8 familyType,
                                               L7_uint32 intIfNum,
                                               L7_uint32* numOfGroups);

/*********************************************************************
* @purpose  Set the unsolicited Interval for this interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    interval    @b{(input)} Startup  Query Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceUnsolicitedIntervalSet(L7_uchar8 familyType,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32 interval);

/*********************************************************************
* @purpose  Get the Unsolicited Report Interval configured for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
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
L7_RC_t mgmdMapProxyInterfaceUnsolicitedIntervalGet(L7_uchar8 familyType,
                                                    L7_uint32 intIfNum,
                                                    L7_uint32* interval);

/*********************************************************************
* @purpose  Get the Restart count for the Interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    count       @b{(output)} Restart count on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyInterfaceRestartCountGet(L7_uchar8 familyType,
                                              L7_uint32 intIfNum,
                                              L7_uint32* count);

/****************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache 
*           Table for the specified group address & the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
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
L7_RC_t mgmdMapProxyCacheUpTimeGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *multipaddr,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *upTime);

/***************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    familyType        @b{(input)}  Address Family type
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
****************************************************************************/
L7_RC_t mgmdMapProxyCacheLastReporterGet(L7_uchar8 familyType,
                                         L7_inet_addr_t *multipaddr,
                                         L7_uint32 intIfNum,
                                         L7_inet_addr_t *ipAddr);

/***************************************************************************
* @purpose  Gets MGMD Proxy Cache Status
*
* @param    familyType        @b{(input)}  Address Family type
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    status            @b{(output)} status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
****************************************************************************/
L7_RC_t mgmdMapProxyCacheStatusGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *multipaddr,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *status);

/*****************************************************************************
* @purpose  Gets the group filter mode (Include or Exclude) for the specified
*           group on the specified interface
*
* @param    familyType       @b{(input)}  Address Family type
* @param    groupAddr        @b{(input)}  multicast group ip address
* @param    intIfNum         @b{(input)}  internal Interface number
* @param    groupFilterMode  @b{(output)} group filter mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyCacheGroupFilterModeGet(L7_uchar8 familyType,
                                            L7_inet_addr_t *groupAddr,
                                            L7_uint32 intIfNum,
                                            L7_uint32 *groupFilterMode);
/************************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    familyType          @b{(input)} Address Family type
* @param    ipaddr              @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyCacheEntryGet (L7_uchar8 familyType,
                                   L7_inet_addr_t *ipaddr, 
                                   L7_uint32  intIfNum);
/********************************************************************
* @purpose  To Validate  the entry exists or not
*
* @param    familyType        @b{(input)}  Address Family type
* @param    multipaddr        @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCacheEntryGet (L7_uchar8 familyType,
                              L7_inet_addr_t *multipaddr,
                              L7_uint32 intIfNum);

/***********************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given
*           multi ipaddress & an interface number.
*
* @param    familyType         @b{(input)} Address Family type
* @param    multipaddr         @b{(inout)}  multicast group ip address
* @param    pIntIfNum          @b{(inout)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments if intIfNum = 0 & multipaddr = 0, then return
*           the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyCacheEntryNextGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *multipaddr,
                                      L7_uint32 *pIntIfNum);

/**********************************************************************
* @purpose  Get the Proxy interface number configured for
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapMgmdProxyIntfInitialized(L7_uchar8 familyType,
                                         L7_uint32 intIfNum);

/************************************************************************
* @purpose  Get the Proxy v3 queries received on that interface
*
* @param    familyType     @b{(input)}   Address Family type
* @param    intIfNum       @b{(input)}   internal interface number
* @param    v3QueriesRcvd  @b{(output)}  queries received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV3QueriesRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v3QueriesRcvd);

/***********************************************************************
* @purpose  Get the Proxy v3 reports received on that interface
*
* @param    familyType     @b{(input)}   Address Family type
* @param    intIfNum       @b{(input)}   internal interface number
* @param    v3ReportsRcvd  @b{(output)}  Reports received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV3ReportsRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v3ReportsRcvd);

/***********************************************************************
* @purpose  Get the Proxy v3 reports sent on that interface
*
* @param    familyType     @b{(input)} Address Family type
* @param    intIfNum       @b{(input)} internal interface number
* @param    v3ReportsSent  @b{(output)}Reports sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV3ReportsSentGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v3ReportsSent);

/************************************************************************
* @purpose  Get the Proxy v2 queries received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v2QueriesRcvd  @b{(output)} Queries received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*******************************************************************/
L7_RC_t  mgmdMapProxyV2QueriesRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v2QueriesRcvd);

/***********************************************************************
* @purpose  Get the Proxy v2 reports received on that interface
*
* @param    familyType     @b{(input)}   Address Family type
* @param    intIfNum       @b{(input)}   internal interface number
* @param    v2ReportsRcvd  @b{(output)}  Reports received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2ReportsRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v2ReportsRcvd);

/*************************************************************************
* @purpose  Get the Proxy v2 reports sent on that interface
*
* @param    familyType     @b{(input)}   Address Family type
* @param    intIfNum       @b{(input)}   internal interface number
* @param    v2ReportsSent  @b{(output)}  Reports sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2ReportsSentGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v2ReportsSent);

/*************************************************************************
* @purpose  Get the Proxy v2 leaves received on that interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    intIfNum      @b{(input)}  internal interface number
* @param    v2LeavesRcvd  @b{(output)} Leaves Received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2LeavesRecvdGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v2LeavesRcvd);

/**********************************************************************
* @purpose  Get the Proxy v2 leaves sent on that interface
*
* @param    familyType    @b{(input)}  Address Family type
* @param    intIfNum      @b{(input)}  internal interface number
* @param    v2LeavesSent  @b{(output)} Leaves Sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV2LeavesSentGet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum,
                                     L7_uint32 *v2LeavesSent);

/***********************************************************************
* @purpose  Get the Proxy v1 queries received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v1QueriesRcvd  @b{(output)} Queries received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV1QueriesRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v1QueriesRcvd);

/***********************************************************************
* @purpose  Get the Proxy v1 reports received on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v1ReportsRcvd  @b{(output)} Reports received
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV1ReportsRecvdGet(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 *v1ReportsRcvd);

/************************************************************************
* @purpose  Get the Proxy v1 reports sent on that interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  internal interface number
* @param    v1ReportsSent  @b{(output)} Reports sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  mgmdMapProxyV1ReportsSentGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_uint32 *v1ReportsSent);

/*****************************************************************************
* @purpose  To validate if the specified interface contains a inverse cache 
*           entry for the specified group or not
*
* @param    familyType        @b{(input)}   Address Family type
* @param    intIfNum          @b{(input)}   internal Interface number
* @param    groupAddr         @b{(input)}   multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Inverse Cache Table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInverseCacheEntryGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_inet_addr_t *groupAddr);

/*****************************************************************************
* @purpose  Get the Next Entry in inverse Cache Table for the specified group 
*           and interface
*
* @param    familyType         @b{(input)}  Address Family type
* @param    pIntIfNum           @b{(input)}  internal Interface number
* @param    ipaddr             @b{(input)}  multicast group ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments if intIfNum=0 & groupAddr=0, then return the first
8            valid entry in the inverse cache table
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInverseCacheEntryNextGet(L7_uchar8 familyType,
                                             L7_uint32 *pIntIfNum,
                                             L7_inet_addr_t *ipaddr);

/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
*           group address and interface or not
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    hostAddr          @b{(input)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxySrcListEntryGet(L7_uchar8 familyType,
                                    L7_inet_addr_t *groupAddr,
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *hostAddr);

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
*           interface and source address
*
* @param    familyType        @b{(input)}  Address Family type
* @param    groupAddr         @b{(inout)}  multicast group ip address
* @param    pIntIfNum         @b{(inout)}  internal Interface number
* @param    hostAddr          @b{(inout)}  source address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0,
*           then return the first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxySrcListEntryNextGet(L7_uchar8 familyType,
                                        L7_inet_addr_t *groupAddr,
                                        L7_uint32 *pIntIfNum,
                                        L7_inet_addr_t *hostAddr);

/****************************************************************************
* @purpose  Get the amount of time until the specified Source
*           Entry is aged out
*
* @param    UnitIndex         @b{(input)}  Unit
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
*********************************************************************/
L7_RC_t mgmdMapProxySrcListExpiryTimeGet(L7_uchar8 familyType,
                                         L7_uint32 intIfNum,
                                         L7_inet_addr_t *groupAddr,
                                         L7_inet_addr_t *hostAddr,
                                         L7_uint32 *srcExpiryTime);

/*********************************************************************
* @purpose  Get the MGMD Version configured for the Interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
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
L7_RC_t mgmdMapProxyInterfaceVersionGet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum,
                                        L7_uint32* version);

/*********************************************************************
* @purpose  Set the MGMD Proxy Version for this interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    version      @b{(input)} MGMD Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceVersionSet(L7_uchar8 familyType,
                                        L7_uint32 intIfNum,
                                        L7_uint32 version);

/*********************************************************************
* @purpose  Resets the proxy statistics parameter
*
* @param    familyType   @b{(input)}   Address Family type
* @param    intIfNum     @b{(input)}   Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/

L7_RC_t mgmdMapProxyResetHostStatus(L7_uchar8 familyType,
                                    L7_uint32 intIfNum);
#ifdef MGMD_TBD /*  CLI command is not there  */
/*********************************************************************
* @purpose  Sets the specified interface as MGMD QUERIER
*
* @param    familyType   @b{(input)}   Address Family type
* @param    intIfNum     @b{(input)}   Internal Interface Number
* @param    mode         @b{(input)}   mode
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
L7_RC_t  mgmdMapInterfaceQuerierStatusSet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_uint32 mode);
#endif
/*********************************************************************
* @purpose  Disables the down stream traffic to flow into up stream interface
*
* @param    familyType   @b{(input)}   Address Family type
* @param    intIfNum     @b{(input)}   Internal Interface Number
* @param    mode         @b{(input)}   mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyUpstreamFwdDisable(L7_uchar8 familyType,
                                       L7_uint32 intIfNum,
                                       L7_uint32 mode);

/*********************************************************************
* @purpose  Disables the down stream traffic to flow into up stream interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    mode         @b{(input)}  mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyUpstreamFwdDisableGet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_BOOL *mode);

/*********************************************************************
* @purpose  Gets the Group status on the given interface
*
* @param    familyType    @b{(input)}   Address Family type
* @param    multipaddr    @b{(input)}   multicast IP Address
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    hostState     @b{(output)}  host state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/

L7_RC_t mgmdMapProxyGroupStatusGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *multipaddr,
                                   L7_uint32 intIfNum,
                                   L7_uint32 *hostState);

/*******************************************************************************
* @purpose  Get the number of source records for the specified group and 
*           interface
*
* @param    familyType        @b{(input)}  Address Family type
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
L7_RC_t mgmdMapProxySrcCountGet(L7_uchar8 familyType,
                                L7_inet_addr_t *groupAddr,
                                L7_uint32 intIfNum,
                                L7_uint32 *srcCount);

/*********************************************************************
* @purpose  Get the administrative mode of an MGMD-PROXY interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyInterfaceModeGet(L7_uchar8 familyType,
                                     L7_uint32 intIfNum, L7_uint32* mode);

/*********************************************************************
* @purpose  Checks if MGMD-Proxy is Enabled.
*
* @param    familyType          @b{(input)} Address Family type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/

L7_BOOL mgmdMapProxyChkIsEnabled(L7_uchar8 familyType);
/*********************************************************************
* @purpose  Send event to MGMD Vendor
*
* @param    familyType     @b{(input)}      Address Family type
* @param    eventType      @b{(input)}      Event Type 
* @param    msgLen         @b{(input)}      Message Length
* @param    eventMsg       @b{(input)}      Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapEventChangeNotify(L7_uchar8 familyType,
                                 L7_uint32  eventType,
                                 L7_uint32  msgLen,
                                 void *eventMsg);

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Received for the router.
*
* @param    familyType  @b{(input)}   Address Family type
* @param    validPkts   @b{(output)}  Valid Packets Received.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidPacketsRcvdGet(L7_uchar8 familyType,
                                   L7_uint32 *validPkts);

/*********************************************************************
* @purpose  Get the Valid MLD Pakcets Sent for the router.
*
* @param    familyType @b{(input)}    Address Family type
* @param    pktsSent   @b{(output)}   Valid Packets Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidPacketsSentGet(L7_uchar8 familyType,
                                   L7_uint32 *pktsSent);

/*********************************************************************
* @purpose  Get the Queries Received for the router.
*
* @param    familyType  @b{(input)}  Address Family type
* @param    quriesRcvd  @b{(output)} Queries Rcvd.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE

* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidQueriesRcvdGet(L7_uchar8 familyType,
                                   L7_uint32 *quriesRcvd);

/*********************************************************************
* @purpose  Get the Queries Sent for the router.
*
* @param    familyType  @b{(input)}  Address Family type
* @param    quriesSent  @b{(output)} Queries Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapValidQueriesSentGet(L7_uchar8 familyType,
                                   L7_uint32 *quriesSent);

/*********************************************************************
* @purpose  Get the Reports Received for the router.
*
* @param    familyType   @b{(input)}  Address Family type
* @param    reportsRcvd  @b{(output)} Reports Received.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapReportsReceivedGet(L7_uchar8 familyType,
                                  L7_uint32 *reportsRcvd);

/*********************************************************************
* @purpose  Get the Reports Sent for the router.
*
* @param    familyType   @b{(input)}  Address Family type
* @param    reportsSent  @b{(output)} Reports Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapReportsSentGet(L7_uchar8 familyType,
                              L7_uint32 *reportsSent);

/*********************************************************************
* @purpose  Get the Leaves Received for the router.
*
* @param    familyType  @b{(input)}  Address Family type
* @param    leavesRcvd  @b{(output)} Leaves Received.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapLeavesReceivedGet(L7_uchar8 familyType,
                                 L7_uint32 *leavesRcvd);

/*********************************************************************
* @purpose  Get the Leaves Sent for the router.
*
* @param    familyType   @b{(input)}  Address Family type
* @param    leavesSent   @b{(output)} Leaves Sent.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapLeavesSentGet(L7_uchar8 familyType,
                             L7_uint32 *leavesSent);

/*********************************************************************
* @purpose  Get the Bad Checksum packets for the router.
*
* @param    familyType   @b{(input)}  Address Family type.
* @param    badChkSum    @b{(output)} Bad Checksum packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapBadCheckSumPktsGet(L7_uchar8 familyType,
                                  L7_uint32 *badChkSum);

/*********************************************************************
* @purpose  Get the Malformed packets for the router.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    malfrmedPkts  @b{(output)} Malformed packets.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMalformedPktsGet(L7_uchar8 familyType,
                                L7_uint32 *malfrmedPkts);

/*********************************************************************
* @purpose  Clear the  Counters for MLD.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    intIfNum    @b{(input)} interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapCountersClear(L7_uchar8 familyType, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Clear the  Traffic Counters for MLD.
*
* @param    familyType    @b{(input)}  Address Family type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
*
*
* @end
*********************************************************************/
L7_RC_t mgmdMapTrafficCountersClear(L7_uchar8 familyType);

/*********************************************************************
* @purpose  To register Callback routine for MGMD to call MRP's
*
* @param    familyType         @b{(input)}  Address Family type
* @param    routerProtocol_ID  @b{(input)}  protocol ID.
* @param    notifyFn           @b{(input)}  Function Pointer to call
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRegisterMembershipChangeEvent(L7_uchar8 familyType,
                       L7_MRP_TYPE_t  routerProtocol_ID,
                       L7_RC_t (*notifyFn)(L7_uchar8 family, mcastEventTypes_t event, 
                       L7_uint32 msgLen, void *pMsg));

/*********************************************************************
* @purpose  To Deregister Callback routine for MGMD to call MRP's
*
* @param    familyType         @b{(input)}  Address Family type
* @param    routerProtocol_ID  @b{(input)}  protocol ID.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapDeRegisterMembershipChangeEvent(L7_uchar8 familyType,
                                               L7_MRP_TYPE_t  routerProtocol_ID);


/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    familyType  @b{(input)} address Family
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    msg         @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMessageQueueSend(L7_uchar8 familyType, L7_uint32 eventType, 
                                L7_uint32 msgLen, L7_VOIDPTR pMsg);

/*********************************************************************
* @purpose  Send event to MGMD Vendor
*
* @param    familyType    @b{(input)}  Address Family type
* @param    eventType     @b{(input)}  Event Type
* @param    msgLen        @b{(input)}  Message Length
* @param    eventMsg      @b{(input)}  Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapComponentCallback(L7_uchar8 familyType, 
         L7_uint32  eventType, L7_uint32  msgLen, void *eventMsg);

/*********************************************************************
* @purpose  Get the MGMD  IP Multicast route table Next entry
*
* @param    familyType           @b{(input)}  Address Family type
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
L7_RC_t mgmdMapProxyIpMRouteEntryNextGet(L7_uchar8 familyType,
                                         L7_inet_addr_t* ipMRouteGroup, 
                                         L7_inet_addr_t* ipMRouteSource,
                                         L7_inet_addr_t* ipMRouteSourceMask);


/*********************************************************************
* @purpose  Gets the minimum amount of time remaining before  this
*           entry will be aged out.
*
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
L7_RC_t mgmdMapProxyIpMRouteExpiryTimeGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_uint32       *expire);

/*********************************************************************
* @purpose  Gets the time since the multicast routing information
*           represented by this entry was learned by the router.
*
* @param    familyType    @b{(input)}  Address Family type
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
L7_RC_t mgmdMapProxyIpMRouteUpTimeGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_uint32       *upTime);

/**********************************************************************
* @purpose  Gets the Rpf address for the given index
*
* @param    familyType    @b{(input)}  Address Family type
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
L7_RC_t mgmdMapProxyIpMRouteRpfAddrGet(L7_uchar8 familyType,
                                       L7_inet_addr_t *ipMRtGrp,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *ipMRtSrcMask,
                                       L7_inet_addr_t *rpfAddress);

/**********************************************************************
* @purpose  Gets the Proxy MRT table count
*
* @param    familyType      @b{(input)}  Address Family type
* @param    count           @b{(input)} Number of MRT entries
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdMapProxyIpMRouteEntryCountGet(L7_uchar8 familyType, L7_uint32 *count);

/*********************************************************************
* @purpose  Gets the interface index on which IP datagrams sent by
*           these sources to this multicast address are received.
*           corresponding to the index received.
*
* @param    familyType    @b{(input)}  Address Family type
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
L7_RC_t mgmdMapProxyIpMRouteIfIndexGet(L7_uchar8 familyType,
                                       L7_inet_addr_t *ipMRtGrp,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *ipMRtSrcMask,
                                       L7_uint32 *inIfIndex);

/*********************************************************************
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType         @b{(input)}  Address Family type
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
L7_RC_t mgmdMapProxyRouteOutIntfEntryNextGet(L7_uchar8 familyType,
                                             L7_inet_addr_t *ipMRtGrp,
                                             L7_inet_addr_t *ipMRtSrc,
                                             L7_inet_addr_t *ipMRtSrcMask,
                                             L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Retrieve group information learned by MGMD
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} Router Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is used by MRPs to get the group information learned 
*           by MGMD whenever MRP is disabled and enabled at run-time
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMRPGroupInfoGet(L7_uchar8 familyType, L7_uint32 rtrIfNum);


#endif

