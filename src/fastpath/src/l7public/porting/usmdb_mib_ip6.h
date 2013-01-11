
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   usmdb_mib_ip6.h
*
* @purpose    Ip6 Mapping layer external function prototypes
*
* @component  Ip6 Mapping Layer
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/



#ifndef USMDB_MIB_IP6_H
#define USMDB_MIB_IP6_H

#include "ipstk_mib_api.h"


/*********************************************************************
* @purpose  get NDisc info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6NetToMediaEntryGet( L7_uint32 searchType, ipstkIpv6NetToMediaEntry_t *vars);

/*********************************************************************
* @purpose  get interface stats from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6IfStatsGet( L7_uint32 searchType, ipstkIpv6IfStats_t *vars);


/*********************************************************************
* @purpose  get interface icmp stats from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6IfIcmpStatsGet( L7_uint32 searchType, ipstkIpv6IfIcmpStats_t *vars);

/*********************************************************************
* @purpose  get count of ipv6 neighbors
*
* @param    nbrCount @b{(output)} number of ipv6 neighbors
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6NbrCountGet(L7_uint32 *nbrCount);


/*********************************************************************
* @purpose  get v6 general group info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6GeneralGroupGet( ipstkIpv6GeneralGroup_t *vars);

/*********************************************************************
* @purpose  get interface info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6IfEntryGet( L7_uint32 searchType, ipstkIpv6IfEntry_t *vars);

/*********************************************************************
* @purpose  get interface info from stack: private extensions
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6IfEntryExtGet( L7_uint32 searchType, ipstkIpv6IfEntryExt_t *vars);


/*********************************************************************
* @purpose  Get SNMP addresses
*
* @param    type         NEXT or EXACT
*           addrEnt      in/out ptr to snmp address entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6AddrEntryGet(L7_uint32 type, ipstkIpv6AddrEntry_t *addrEnt);

/*********************************************************************
* @purpose  Get SNMP addresses
*
* @param    type         NEXT or EXACT
*           addrEnt      in/out ptr to snmp address entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6AddrEntryGetNext(L7_uint32 type, ipstkIpv6AddrEntry_t *addrEnt);

/*********************************************************************
* @purpose  Get SNMP address prefixes
*
* @param    type         NEXT or EXACT
*           pfxEnt      in/out ptr to snmp address prefix entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpv6AddrPrefixEntryGet(L7_uint32 type, ipstkIpv6AddrPrefixEntry_t *pfxEnt);



/*********************************************************************
* @purpose  Return next best route in the routing table. 
*
* @param    type         (input)  EXACT or NEXT
* @param    nextRoute    (input/output) Next best route in the database.
*
* @returns  L7_SUCCESS  Next route found. 
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop. Nexthop instance identified by ipv6RouteIndex.
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpRto6NextBestRouteGet (L7_uint32 type, ipstkIpv6RouteEntry_t *nextRoute);

/*********************************************************************
* @purpose  Return next route (best or non-best) in the routing table.
*
* @param    type         (input)  EXACT or NEXT
* @param    nextRoute    (input/output) Next route in the database.
*
* @returns  L7_SUCCESS  Next route found.
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop. Nexthop instance identified by ipv6RouteIndex.
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbRto6AllNextRouteGet (L7_uint32 type, ipstkIpv6RouteEntry_t *nextRoute);

/*********************************************************************
* @purpose  Return next best static route in the config table. 
*
* @param    nextRoute    (input/output) Next best route in the database.
*
* @returns  L7_SUCCESS  Next route found. 
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop. 
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpIpv6StaticRouteGet (ipstkIpv6RouteEntry_t *nextRt);

/*********************************************************************
* @purpose  Return next best static route in the config table.
*
* @param    nextRoute    (input/output) Next best route in the database.
*
* @returns  L7_SUCCESS  Next route found.
* @returns  L7_ERROR    Next route is not found.
*
* @notes    Returns a single nexthop.
*           Per normal snmp operation, if operation is NEXT, returns exact match.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbSnmpIpv6StaticRouteGetNext (ipstkIpv6RouteEntry_t *nextRt);

/*********************************************************************
* @purpose  Enable or disable forwarding on the router
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6MapRtrForwardingModeSet(L7_uint32 mode);


#endif /* USMDB_MIB_IP6_H */

