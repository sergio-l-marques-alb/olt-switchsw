/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_mib_mgmd.c
*
* @purpose  Provide interface to  MGMD components
*
* @component unitmgr
*
* @comments
*
* @create
*
* @author Nitish
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_mib_igmp_api.h"
#include "l7_mgmd_api.h"

/*********************************************************************
* @purpose  Gets the Interface Query Interval
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)} Address Family type
* @param    intIfNum       @b{(input)}  internal Interface number
* @param    queryinterval   @b{(output)}  query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQueryIntervalGet(L7_uint32 UnitIndex, 
       L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *queryInterval)
{
   return mgmdMapInterfaceQueryIntervalGet(family, intIfNum, queryInterval);
   
}

/*********************************************************************
* @purpose  Sets the Interface Query Interval
*
*
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    queryinterval   @b{(input)}  query interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQueryIntervalSet(L7_uint32 UnitIndex, 
                 L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 queryInterval)
{
  return mgmdMapInterfaceQueryIntervalSet(family, intIfNum, queryInterval);
  
}

/*********************************************************************
* @purpose  Gets the Interface MGMD version
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    version         @b{(output)}  MGMD version
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceVersionGet(L7_uint32 UnitIndex, 
                     L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *version)
{
  return mgmdMapInterfaceVersionGet(family, intIfNum, version);
}

/*********************************************************************
* @purpose  Sets the Interface MGMD version
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    version         @b{(input)}  MGMD version
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceVersionSet(L7_uint32 UnitIndex, 
                      L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 version)
{
  return mgmdMapInterfaceVersionSet(family, intIfNum, version);
}

/*********************************************************************
* @purpose  Gets the Interface MGMD Query Max Reponse Time
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    maxresptime     @b{(output)} Max Response Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQueryMaxResponseTimeGet(L7_uint32 UnitIndex, 
                  L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *maxRespTime)
{
  return mgmdMapInterfaceQueryMaxResponseTimeGet(family, intIfNum, maxRespTime);
}

/*********************************************************************
* @purpose  Sets the Interface MGMD Query Max Reponse Time
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    maxresptime     @b{(input)}   Max Response Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQueryMaxResponseTimeSet(L7_uint32 UnitIndex, 
                  L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 maxRespTime)
{
   return mgmdMapInterfaceQueryMaxResponseTimeSet(family, intIfNum, maxRespTime);
}

/*********************************************************************
* @purpose  Gets the MGMD Interface Robustness
*
* @param    UnitIndex       @b{(input)}    Unit
* @param    family          @b{(input)}    Address Family type
* @param    intIfNum        @b{(input)}    internal Interface number
* @param    robust          @b{(output)}   Robustness
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceRobustnessGet(L7_uint32 UnitIndex, 
                       L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *robust)
{
   return mgmdMapInterfaceRobustnessGet(family, intIfNum, robust);
}

/*********************************************************************
* @purpose  Sets the MGMD Interface Robustness
*
* @param    UnitIndex       @b{(input)}    Unit
* @param    family          @b{(input)}    Address Family type
* @param    intIfNum        @b{(input)}    internal Interface number
* @param    robust          @b{(input)}    Robustness
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceRobustnessSet(L7_uint32 UnitIndex, 
                      L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 robust)
{
  return mgmdMapInterfaceRobustnessSet(family, intIfNum, robust);
}

/*********************************************************************
* @purpose  Gets the  MGMD Interface last member Query interval
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    queryInterval   @b{(output)} last member querry interval
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceLastMembQueryIntervalGet(L7_uint32 UnitIndex, 
                L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *queryInterval)
{
  return mgmdMapInterfaceLastMembQueryIntervalGet(family, intIfNum, queryInterval);
}

/*********************************************************************
* @purpose  Sets the  MGMD Interface last member Query interval
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    queryInterval   @b{(input)}  last member querry interval
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceLastMembQueryIntervalSet(L7_uint32 UnitIndex, 
                L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 queryInterval)
{
  return mgmdMapInterfaceLastMembQueryIntervalSet(family, intIfNum, 
                                                  queryInterval);
}



/*********************************************************************
* @purpose  Gets the  MGMD Interface Querrier ip address
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    ipAddr            @b{(output)} Querier Ip address
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQuerierGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                    L7_uint32 intIfNum, L7_inet_addr_t *ipAddr)
{
  return mgmdMapInterfaceQuerierGet(family, intIfNum, ipAddr);
}

/*********************************************************************
* @purpose  Gets the  MGMD Interface Querier upTime
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    upTime            @b{(output)} Querier uptime
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQuerierUpTimeGet(L7_uint32 UnitIndex, 
                      L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *upTime)
{
  return mgmdMapInterfaceQuerierUpTimeGet(family, intIfNum, upTime);
}

/*********************************************************************
* @purpose  Gets the  MGMD Interface Querier Expiry Time
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    expiryTime             @b{(output)} Querier Expiry Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdInterfaceQuerierExpiryTimeGet(L7_uint32 UnitIndex, 
                   L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *expiryTime)
{
  return mgmdMapInterfaceQuerierExpiryTimeGet(family, intIfNum, expiryTime);
}

/**************************************************************************
* @purpose  Gets the  Number of MGMD wrong version queries received on this 
*           interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    wrongVerNum            @b{(output)} Number of Wrong version queries
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdInterfaceWrongVersionQueriesGet(L7_uint32 UnitIndex, 
                 L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *wrongVerNum)
{
  return mgmdMapInterfaceWrongVersionQueriesGet(family, intIfNum, wrongVerNum);
}

/**************************************************************************
* @purpose  Gets the  Number of times Group Membership is added to this 
*           interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    numJoins               @b{(output)} Number of times groupmembers 
*                                               added to this interface
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdInterfaceJoinsGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                     L7_uint32 intIfNum, L7_uint32 *numJoins)
{
  return mgmdMapInterfaceJoinsGet(family, intIfNum, numJoins);
}

/***************************************************************************
* @purpose  Gets Current Number of membership in cache table for this interface
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    numGroups              @b{(output)} current  Number of memberships 
*                                               on this interface
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdInterfaceGroupsGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                     L7_uint32 intIfNum, L7_uint32 *numGroups)
{
  return mgmdMapInterfaceGroupsGet(family, intIfNum, numGroups);
}



/************************************************************************
* @purpose  Get the Status of Entry in the Interface Table
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    status                 @b{(output)} Enable/Disable
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdInterfaceStatusGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                        L7_uint32 intIfNum, L7_uint32 *status)
{
  return mgmdMapInterfaceModeGet(family, intIfNum, status);
}

/*************************************************************************
* @purpose  Set Status of Entry in the Interface Table
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    status                 @b{(input)}  Enable/Disable
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdInterfaceStatusSet(L7_uint32 UnitIndex, L7_uchar8 family, 
     L7_uint32 intIfNum, L7_uint32 status,L7_MGMD_INTF_MODE_t proxy_or_router)
{
  L7_MGMD_ERROR_CODE_t errCode; 
  return mgmdMapInterfaceModeSet(family, intIfNum, status,
                                 proxy_or_router, &errCode);
}

/***************************************************************************
* @purpose  Determines if the entry in the Interface table is valid or not
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @returns  L7_SUCCESS if the entry exists.
* @returns  L7_FAILURE if the entry does not exists.
*
* @comments none
*
* @end
****************************************************************************/
L7_RC_t usmDbMgmdInterfaceEntryGet ( L7_uint32 UnitIndex, L7_uchar8 family, 
                                                          L7_uint32 intIfNum)
{
        L7_RC_t rc = L7_FAILURE;
        L7_uint32 mode;

       rc = mgmdMapInterfaceModeGet(family, intIfNum, &mode);
       if ( (rc == L7_SUCCESS) && (mode == L7_ENABLE))
       {
         return L7_SUCCESS;
       }
       return L7_FAILURE;
}

/**************************************************************************
* @purpose  Get the Next entry in the Interface table subsequent to a given interface number
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(inout)}  internal Interface number
* @returns  L7_SUCCESS if the entry exists.
* @returns  L7_FAILURE if the entry does not exists.
*
* @comments if intIfNum = 0 then the first entry is returned
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdInterfaceEntryNextGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                         L7_uint32 *intIfNum)
{
  return  mgmdMapInterfaceEntryNextGet(family, intIfNum);
}

/**************************************************************************
* @purpose  Create an entry in the Interface table for the given interface number
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)} Address Family type
* @param    intIfNum               @b{(inout)}  internal Interface number
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
****************************************************************************/
L7_RC_t usmDbMgmdInterfaceCreate ( L7_uint32 UnitIndex, L7_uchar8 family, 
                       L7_uint32 intIfNum,L7_MGMD_INTF_MODE_t proxy_or_router)
{
  L7_MGMD_ERROR_CODE_t errCode;
  return mgmdMapInterfaceModeSet(family, intIfNum, L7_ENABLE,proxy_or_router, &errCode);
}

/**************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @param    ipAddr            @b{(output)} last reporter ip
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdCacheLastReporterGet(L7_uint32 UnitIndex, L7_uchar8 family, 
       L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, L7_inet_addr_t *ipAddr)
{
  return  mgmdMapCacheLastReporterGet(family, multipaddr, intIfNum, ipAddr);
}

/**************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache 
*           Table for the specified group address & the specified interface
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @param    upTime            @b{(output)} Up time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdCacheUpTimeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
            L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, L7_uint32 *upTime)
{
  return  mgmdMapCacheUpTimeGet(family, multipaddr, intIfNum, upTime);
}

/***************************************************************************
* @purpose  Gets the  Expiry time before the specified entry in Cache Table will be aged out
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @param    expiryTime        @b{(output)} Expiry Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdCacheExpiryTimeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
        L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, L7_uint32 *expiryTime)
{
  return  mgmdMapCacheExpiryTimeGet(family, multipaddr, intIfNum, expiryTime);
}

/**************************************************************************
* @purpose  Gets the  time remaining until the router assumes there are no longer
* @purpose  any MGMD version 1 Hosts on the specified interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @param    version1Time      @b{(output)} version1 Host Expiry Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*************************************************************************/
L7_RC_t usmDbMgmdCacheVersion1HostTimerGet(L7_uint32 UnitIndex, 
  L7_uchar8 family, L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, 
  L7_uint32 *version1Time)
{
  return  mgmdMapCacheVersion1HostTimerGet(family, multipaddr, 
                                           intIfNum, version1Time);
}

/**************************************************************************
* @purpose  To Validate  the entry exists or not
*
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdCacheEntryGet ( L7_uint32 UnitIndex, L7_uchar8 family, 
                        L7_inet_addr_t *multipaddr, L7_uint32 intIfNum)
{
  return  mgmdMapCacheEntryGet(family, multipaddr, intIfNum );
}

/*************************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given multi 
*           ipaddress & an interface number.
*
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments if intIfNum = 0 & multipaddr = 0, then return the first valid entry in the cache table
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdCacheEntryNextGet ( L7_uint32 UnitIndex, L7_uchar8 family, 
                            L7_inet_addr_t *multipaddr, L7_uint32 *intIfNum)
{
  return  mgmdMapCacheEntryNextGet(family, multipaddr, intIfNum );

}

/***************************************************************************
* @purpose  Gets the time remaining until the router assumes there are no
*           longer any MGMD version 2 Hosts for the specified group on the
*           specified interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    version2Time      @b{(output)} version1 Host Expiry Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*************************************************************************/
L7_RC_t usmDbMgmdCacheVersion2HostTimerGet(L7_uint32 UnitIndex, 
 L7_uchar8 family, L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, 
 L7_uint32 *version2Time)
{
  return  mgmdMapCacheVersion2HostTimerGet(family, groupAddr, 
                                           intIfNum, version2Time);
}

/***************************************************************************
* @purpose  Gets the compatibility mode (v1, v2 or v3) for the specified
* @purpose  group on the specified interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)} Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    groupCompatMode   @b{(output)} group compatibility mode
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdCacheGroupCompatModeGet(L7_uint32 UnitIndex, L7_uchar8 family,
    L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, L7_uint32 *groupCompatMode)
{
  return  mgmdMapCacheGroupCompatModeGet(family, groupAddr, 
                                         intIfNum, groupCompatMode);
}

/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
* @purpose  group on the specified interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    sourceFilterMode  @b{(output)} source filter mode
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
*************************************************************************/
L7_RC_t usmDbMgmdCacheGroupSourceFilterModeGet(L7_uint32 UnitIndex, 
 L7_uchar8 family, L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, 
 L7_uint32 *sourceFilterMode)
{
  return  mgmdMapCacheGroupSourceFilterModeGet(family, groupAddr, 
                                               intIfNum, sourceFilterMode);
}

/*****************************************************************************
* @purpose  To validate if the specified interface contains a cache entry for
*           the specified group or not
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments Mgmd Cache Table
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdCacheIntfEntryGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                            L7_uint32 intIfNum, L7_inet_addr_t *groupAddr)
{
  return  mgmdMapCacheIntfEntryGet(family, intIfNum, groupAddr);
}

/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
* @purpose  interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    ipaddr            @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0 & groupAddr=0, then return the first valid entry in 
*           the cache table
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdCacheIntfEntryNextGet (L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 *intIfNum, L7_inet_addr_t *ipaddr)
{
  return  mgmdMapCacheIntfEntryNextGet(family, intIfNum, ipaddr);
}

/*****************************************************************************
* @purpose  To validate if the specified source address exists for the 
*           specified group address and interface or not
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdSrcListEntryGet(L7_uint32 UnitIndex, L7_uchar8 family, 
      L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, L7_inet_addr_t *hostAddr)
{
  return  mgmdMapSrcListEntryGet(family, groupAddr, intIfNum, hostAddr);
}

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
* @purpose  interface and source address
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0, then return the first
*           valid source list entry
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdSrcListEntryNextGet(L7_uint32 UnitIndex, L7_uchar8 family, 
     L7_inet_addr_t *groupAddr, L7_uint32 *intIfNum, L7_inet_addr_t *hostAddr)
{
  return  (mgmdMapSrcListEntryNextGet(family, groupAddr, intIfNum, hostAddr));
}

/*****************************************************************************
* @purpose  Get the Next  group address and interface
*
* @param    familyType         @b{(input)}  Address Family type
* @param    groupAddr          @b{(inout)}  multicast group ip address
* @param    pIntIfNum          @b{(inout)}  internal Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments if intIfNum=0 and groupAddr=0
*           then return the first valid  entry
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdGrpIntfEntryNextGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *groupAddr,
                                   L7_uint32 *pIntIfNum)
{
  return  (mgmdMapGrpIntfEntryNextGet(familyType, groupAddr, pIntIfNum));
}


/*******************************************************************************
* @purpose  Get the amount of time until the specified Source Entry is aged out
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @param    srcExpiryTime     @b{(output)} source expiry time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdSrcExpiryTimeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
  L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, L7_inet_addr_t *hostAddr, 
  L7_uint32 *srcExpiryTime)
{
  return  mgmdMapSrcExpiryTimeGet(family, groupAddr, intIfNum, 
                                          hostAddr, srcExpiryTime);
}


/*********************************************************************
* @purpose  Gets the  MGMD Interface Querrier ip address
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    ipAddr            @b{(output)} Querier Ip address
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostInterfaceQuerierGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                    L7_uint32 intIfNum, L7_inet_addr_t *ipAddr)
{
  return mgmdMapProxyInterfaceQuerierGet(family, intIfNum, ipAddr);
}


/****************************************************************************
* @purpose  Set Status of Entry in the Interface Table
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    status                 @b{(input)}  Enable/Disable
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*****************************************************************************/
L7_RC_t usmDbMgmdHostInterfaceStatusSet(L7_uint32 UnitIndex, L7_uchar8 family,
     L7_uint32 intIfNum, L7_uint32 status,L7_MGMD_INTF_MODE_t proxy_or_router)
{
  L7_MGMD_ERROR_CODE_t errCode;
  return mgmdMapInterfaceModeSet(family, intIfNum, status,
                                         proxy_or_router, &errCode);
}

/*************************************************************************
* @purpose  Get the Status of Entry in the Interface Table
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)}  Unit
* @param    family                 @b{(input)}  Address Family type
* @param    intIfNum               @b{(input)}  internal Interface number
* @param    status                 @b{(output)} Enable/Disable
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
****************************************************************************/
L7_RC_t usmDbMgmdHostInterfaceStatusGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                        L7_uint32 intIfNum, L7_uint32 *status)
{
  return mgmdMapProxyInterfaceModeGet(family, intIfNum, status);
}

/*********************************************************************
* @purpose  Gets the Interface MGMD version
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    version         @b{(output)} MGMD version
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostInterfaceVersionGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                      L7_uint32 intIfNum, L7_uint32 *version)
{
  return mgmdMapProxyInterfaceVersionGet(family, intIfNum, version);
}

#ifdef MGMD_PROXY_FP_NOT_SUPPORTED

/*********************************************************************
* @purpose  Sets the Interface MGMD version
*
* @param    UnitIndex       @b{(input)}  Unit
* @param    family          @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  internal Interface number
* @param    version         @b{(input)}  MGMD version
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostInterfaceVersionSet(L7_uint32 UnitIndex, L7_uchar8 family,
                                       L7_uint32 intIfNum, L7_uint32 version)
{
  return mgmdMapProxyInterfaceVersionSet(family, intIfNum, version);
}

#endif
/*********************************************************************
* @purpose  Gets the host interface version 1 Querier timer 
*
* @param    UnitIndex   @b{(input)}   unit
* @param    family      @b{(input)}   Address Family type
* @param    intifNum    @b{(input)}   interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostInterfaceVer1QuerierTimerGet (L7_uint32 UnitIndex, 
               L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *v1Queriertime)
{
  return mgmdMapProxyInterfaceVer1QuerierTimerGet (family, intIfNum, 
                                                   v1Queriertime);

}


/*********************************************************************
* @purpose  Gets the host interface version 2 Querier timer 
*
* @param    UnitIndex   @b{(input)} unit
* @param    family      @b{(input)} Address Family type
* @param    intifNum    @b{(input)} interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostInterfaceVer2QuerierTimerGet (L7_uint32 UnitIndex, 
               L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *v2Queriertime)
{
  return mgmdMapProxyInterfaceVer2QuerierTimerGet (family, intIfNum, 
                                                   v2Queriertime);

}



/*****************************************************************************
* @purpose  Determines if the entry in the host Interface table is valid or not
* @purpose  For SNMP
*
* @param    UnitIndex              @b{(input)} Unit
* @param    family                 @b{(input)} Address Family type
* @param    intIfNum               @b{(input)} internal Interface number
* @returns  L7_SUCCESS if the entry exists.
* @returns  L7_FAILURE if the entry does not exists.
*
* @comments none
*
* @end
****************************************************************************/
L7_RC_t usmDbMgmdHostInterfaceEntryGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                                         L7_uint32 intIfNum)
{
        L7_RC_t rc = L7_FAILURE;
        L7_uint32 mode;

       rc = mgmdMapProxyInterfaceModeGet(family, intIfNum, &mode);
       if ( (rc == L7_SUCCESS) && (mode == L7_ENABLE))
       {
            return L7_SUCCESS;
       }
       return L7_FAILURE;
}

/**************************************************************************
* @purpose  Get the Next entry in the host Interface table subsequent to a 
            given interface number
*
* @param    UnitIndex              @b{(input)} Unit
* @param    family                 @b{(input)} Address Family type
* @param    intIfNum              @b{(inout)} internal Interface number
* @returns  L7_SUCCESS if the entry exists.
* @returns  L7_FAILURE if the entry does not exists.
*
* @comments if intIfNum = 0 then the first entry is returned
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdHostInterfaceEntryNextGet ( L7_uint32 UnitIndex, 
                       L7_uchar8 family, L7_uint32 *intIfNum)
{
  return  mgmdMapProxyInterfaceEntryNextGet(family, intIfNum);
}

/****************************************************************************
* @purpose  Gets the  time elapsed since the entry was created in the Cache Table for
*           the specified group address & the specified interface
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @param    upTime            @b{(output)} Up time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
**************************************************************************/
L7_RC_t usmDbMgmdHostCacheUpTimeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
            L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, L7_uint32 *upTime)
{
  return  mgmdMapProxyCacheUpTimeGet(family, multipaddr, intIfNum, upTime);
}



/****************************************************************************
* @purpose  Gets the  Ip Address of the source of last membership report
*           received for the specified group address on the specified interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @param    ipAddr            @b{(output)} last reporter ip
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdHostCacheLastReporterGet(L7_uint32 UnitIndex, 
  L7_uchar8 family, L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, 
  L7_inet_addr_t *ipAddr)
{
  return  mgmdMapProxyCacheLastReporterGet(family, multipaddr, 
                                           intIfNum, ipAddr);
}



/*****************************************************************************
* @purpose  Gets the source filter mode (Include or Exclude) for the specified
* @purpose  group on the specified interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    sourceFilterMode  @b{(output)} source filter mode
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdHostCacheGroupSourceFilterModeGet(L7_uint32 UnitIndex, 
   L7_uchar8 family, L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, 
   L7_uint32 *sourceFilterMode)
{
  return  mgmdMapProxyCacheGroupFilterModeGet(family, groupAddr, 
                                              intIfNum, sourceFilterMode);
}

/****************************************************************************
* @purpose  To get the status of the entry 
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @param    intIfNum          @b{(input)}  internal Interface number 
*
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments Mgmd Cache Table
*
* @end
*************************************************************************/
L7_RC_t usmDbMgmdHostCacheStatusGet ( L7_uint32 UnitIndex, L7_uchar8 family, 
                                 L7_inet_addr_t *ipaddr, L7_uint32 intIfNum)
{
  L7_uint32 status = L7_DISABLE;
    
  if( mgmdMapProxyCacheStatusGet(family, ipaddr, 
                                 intIfNum, &status) == L7_SUCCESS)
  {
    if (status == L7_DISABLE)
       return L7_FAILURE;
    else
       return L7_SUCCESS;
  }
  else
     return L7_FAILURE;
}


/***************************************************************************
* @purpose  To Validate  the entry exists or not
* @
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments Mgmd Cache Table
*
* @end
***************************************************************************/
L7_RC_t usmDbMgmdHostCacheEntryGet ( L7_uint32 UnitIndex, L7_uchar8 family, 
                                 L7_inet_addr_t *ipaddr, L7_uint32 intIfNum)
{
  return  mgmdMapProxyCacheEntryGet(family, ipaddr, intIfNum);
}

/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table subsequent to a given multi ipaddress & an interface number.
* @
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    multiipaddr       @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS   if an extry exists
* @returns  L7_FAILURE   if an entry does not exists
*
* @comments if intIfNum = 0 & multipaddr = 0, then return the first valid entry
*           in the cache table
*
* @end
****************************************************************************/
L7_RC_t usmDbMgmdHostCacheEntryNextGet ( L7_uint32 UnitIndex, L7_uchar8 family,
                               L7_inet_addr_t *multipaddr, L7_uint32 *intIfNum)
{
  return  mgmdMapProxyCacheEntryNextGet(family, multipaddr, intIfNum );

}

/*****************************************************************************
* @purpose  To validate if the specified interface contains a cache entry for
* @purpose  the specified group or not
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments Host Inverse Cache Table
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostInverseCacheEntryGet(L7_uint32 UnitIndex, 
       L7_uchar8 family, L7_uint32 intIfNum, L7_inet_addr_t *groupAddr)
{
  return  mgmdMapProxyInverseCacheEntryGet(family, intIfNum, groupAddr);
}
/*****************************************************************************
* @purpose  Get the Next Entry in Cache Table for the specified group and
* @purpose  interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    ipaddr            @b{(input)}  multicast group ip address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0 & groupAddr=0, then return the first valid entry 
*           in the cache table
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostInverseCacheEntryGetNext (L7_uint32 UnitIndex, 
                 L7_uchar8 family, L7_uint32 *intIfNum, L7_inet_addr_t *ipaddr)
{
  return  mgmdMapProxyInverseCacheEntryNextGet(family, intIfNum, ipaddr);
}
                                                                                                                             
/*****************************************************************************
* @purpose  To validate if the specified source address exists for the specified
* @purpose  group address and interface or not
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostSrcListEntryGet(L7_uint32 UnitIndex, L7_uchar8 family, 
      L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, L7_inet_addr_t * hostAddr)
{
  return  mgmdMapProxySrcListEntryGet(family, groupAddr, intIfNum, hostAddr);
}

/*****************************************************************************
* @purpose  Get the Next Source List Entry for the specified group address,
* @purpose  interface and source address
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0, then return the first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostSrcListEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 family,
     L7_inet_addr_t *groupAddr, L7_uint32 *intIfNum, L7_inet_addr_t *hostAddr)
{
  return  mgmdMapProxySrcListEntryNextGet(family, groupAddr, 
                                                  intIfNum, hostAddr);
}
/*****************************************************************************
* @purpose  Get the Expiry Time of the  Source List Entry for the specified group address,
* @purpose  interface and source address
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
* @param    intIfNum          @b{(input)}  internal Interface number
* @param    groupAddr         @b{(input)}  multicast group ip address
* @param    hostAddr          @b{(input)}  source address
* @returns  L7_SUCCESS        if an extry exists
* @returns  L7_FAILURE        if an entry does not exist
*
* @comments if intIfNum=0, groupAddr=0 & hostAddr=0, then return the first valid source list entry
*
* @end
*********************************************************************/
L7_RC_t usmDbMgmdHostSrcListExpiryTimeGet(L7_uint32 UnitIndex, 
                                          L7_uchar8 family, 
                                          L7_uint32 intIfNum, 
                                          L7_inet_addr_t *groupAddr, 
                                          L7_inet_addr_t *hostAddr, 
                                          L7_uint32 *srcExpiryTime)
{
  return mgmdMapProxySrcListExpiryTimeGet(family, intIfNum, 
                                          groupAddr, hostAddr, srcExpiryTime);
}

/*********************************************************************
* @purpose  Gets the Group status on the given interface
*
* @param    UnitIndex     @b{(input)}   Unit
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
L7_RC_t usmDbMgmdHostGroupStatusGet(L7_uint32 UnitIndex, L7_uchar8 family, 
  L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, L7_uint32 *hostState)
{
  return  mgmdMapProxyGroupStatusGet (family, multipaddr, intIfNum, hostState);
}

/**************************************************************************
* @purpose  Get the number of source records for the specified group and 
*           interface
*
* @param    UnitIndex         @b{(input)}  Unit
* @param    family            @b{(input)}  Address Family type
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

L7_RC_t usmDbMgmdHostInterfaceSrcsGet(L7_uint32 UnitIndex, L7_uchar8 family,
 L7_inet_addr_t *groupAddr, L7_uint32 intIfNum, L7_uint32 *numSrcs)
{
  return mgmdMapProxySrcCountGet(family, groupAddr, intIfNum, numSrcs);
}
