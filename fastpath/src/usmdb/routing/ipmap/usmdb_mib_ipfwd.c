/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  usmdb_mib_ipfwd.c
*
* @purpose   Api functions for the IP Forwarding Table MIB, as 
*            specified in RFC 2096
*
* @component usmdb
*
* @comments  none
*
* @create    06/29/01
*
* @author    anayar
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "usmdb_mib_ipfwd_api.h"
#include "ipmap_2096_api.h"

/*********************************************************************
* @purpose  Gets the number of current ipCidrRouteTable entries that
*           are not invalid
*
* @param    UnitIndex   @b{(input)}   the unit for this operation
* @param    val         @b{(output)}  Pointer to the number of entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    read-only
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteNumberGet(L7_uint32 UnitIndex, L7_int32 *val)
{
  return ipMapIpCidrRouteNumberGet(val);
}

/*------------------------------------------------------------------------------*/
/* IP CIDR Route Table                                                          */
/*                                                                              */
/* The IP CIDR Route Table obsoletes and replaces the ipRoute Table current in  */
/* MIB-I and MIB-II and the IP Forwarding Table. It adds knowledge of the       */
/* autonomous system of the next hop, multiple next hops, and policy routing    */
/* and Classless Inter-Domain Routing.                                          */
/*------------------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Determines if the route entry which corresponds to the 
*           specified index values exists.
*
* @param    ipCidrRouteDest     @b{(input)} The destination ip address
* @param    ipCidrRouteMask     @b{(input)} The associated route mask
* @param    ipCidrRouteTos      @b{(input)} The policy identifier
* @param    ipCidrRouteNextHop  @b{(input)} The next hop address
*
* @returns  L7_SUCCESS  If the specified route entry exists
* @returns  L7_FAILURE  If the specified route entry does not exist
*
* @notes    A particular route to a particular destination under
*           a particular policy. 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteEntryGet(L7_uint32 UnitIndex,
                                 L7_uint32 ipCidrRouteDest,
                                 L7_uint32 ipCidrRouteMask,
                                 L7_uint32 ipCidrRouteTos,
                                 L7_uint32 ipCidrRouteNextHop)
{
  return ipMapIpCidrRouteEntryGet(ipCidrRouteDest, ipCidrRouteMask,
                                  ipCidrRouteTos, ipCidrRouteNextHop);
}

/*********************************************************************
* @purpose  Determines if the next route entry exists
*
* @param    UnitIndex               @b{(input)} the unit for this operation
* @param    ipCidrRouteDest         @b{(input/output)} ptr to destination ip address
* @param    ipCidrRouteMask         @b{(input/output)} ptr to associated route mask
* @param    ipCidrRouteTos          @b{(input/output)} ptr to policy identifier
* @param    ipCidrRouteNextHop      @b{(input/output)} ptr to next hop address
*
* @returns  L7_SUCCESS  If the next route entry exists
* @returns  L7_FAILURE  If the next route entry does not exist
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteEntryGetNext(L7_uint32 UnitIndex,
                                     L7_uint32* ipCidrRouteDest,
                                     L7_uint32* ipCidrRouteMask,
                                     L7_uint32* ipCidrRouteTos,
                                     L7_uint32* ipCidrRouteNextHop)
{
  return ipMapIpCidrRouteEntryGetNext(ipCidrRouteDest, ipCidrRouteMask,
                                      ipCidrRouteTos, ipCidrRouteNextHop);
}

/*********************************************************************
* @purpose  Gets the ifIndex value for this route entry
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteIfIndex  @b{(output)}  Pointer to ifindex value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The ifIndex value identifies the local interface through
*           which the next hop of this route should be reached
*           MAX_ACCESS: read-create
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteIfIndexGet(L7_uint32 UnitIndex,
                                   L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteIfIndex)
{
  return ipMapIpCidrRouteIfIndexGet(ipCidrRouteDest, ipCidrRouteMask,
                                    ipCidrRouteTos, ipCidrRouteNextHop,
                                    ipCidrRouteIfIndex);
}

/*********************************************************************
* @purpose  Gets the type of route entry
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteType     @b{(output)}  Pointer to route entry type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Route types include:
*            L7_IP_CIDR_ROUTE_TYPE_OTHER    1
*            L7_IP_CIDR_ROUTE_TYPE_REJECT   2
*            L7_IP_CIDR_ROUTE_TYPE_LOCAL    3
*            L7_IP_CIDR_ROUTE_TYPE_REMOTE   4
*            L7_IP_CIDR_ROUTE_TYPE_BLACKHOLE 5
*
*           Note that local(3) refers to a route for which the next hop
*           is the final destination; remote(4) refers to a route for
*           which the next hop is not the final destination.
*
*           Routes which do not result in traffic forwarding or 
*           rejection should not be displayed even if the implementation
*           keeps them stored internally.
*
*           Reject(2) refers to a route which, if matched, discards the
*           message as unreachable. This is used in some protocols as a
*           means of correctly aggregating routes.
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteTypeGet(L7_uint32 UnitIndex,
                                L7_uint32 ipCidrRouteDest,
                                L7_uint32 ipCidrRouteMask,
                                L7_uint32 ipCidrRouteTos,
                                L7_uint32 ipCidrRouteNextHop,
                                L7_int32* ipCidrRouteType)
{
  return ipMapIpCidrRouteTypeGet(ipCidrRouteDest, ipCidrRouteMask,
                                 ipCidrRouteTos, ipCidrRouteNextHop,
                                 ipCidrRouteType);
}

/*********************************************************************
* @purpose  Gets the routing mechanism via which this route was
*           learned.
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteProto    @b{(output)}  Pointer to routing protocol
*                                             type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Routing protocols include:
*             L7_IP_CIDR_ROUTE_PROTO_OTHER      1
*             L7_IP_CIDR_ROUTE_PROTO_LOCAL      2
*             L7_IP_CIDR_ROUTE_PROTO_NETMGMT    3
*             L7_IP_CIDR_ROUTE_PROTO_ICMP       4
*
*           The following are dynamic routing protocols:
*             L7_IP_CIDR_ROUTE_PROTO_EGP        5
*             L7_IP_CIDR_ROUTE_PROTO_GGP        6
*             L7_IP_CIDR_ROUTE_PROTO_HELLO      7
*             L7_IP_CIDR_ROUTE_PROTO_RIP        8
*             L7_IP_CIDR_ROUTE_PROTO_ISIS       9
*             L7_IP_CIDR_ROUTE_PROTO_ESIS       10
*             L7_IP_CIDR_ROUTE_PROTO_CISCOIGRP  11
*             L7_IP_CIDR_ROUTE_PROTO_BBNSPFIGP  12
*             L7_IP_CIDR_ROUTE_PROTO_OSPF       13
*             L7_IP_CIDR_ROUTE_PROTO_BGP        14
*             L7_IP_CIDR_ROUTE_PROTO_IDPR       15
*             L7_IP_CIDR_ROUTE_PROTO_CISCOEIGRP 16
*
*           Inclusion of values for gateway routing protocols is not
*           intended to imply that hosts should support those protocols.
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteProtoGet(L7_uint32 UnitIndex,
                                 L7_uint32 ipCidrRouteDest,
                                 L7_uint32 ipCidrRouteMask,
                                 L7_uint32 ipCidrRouteTos,
                                 L7_uint32 ipCidrRouteNextHop,
                                 L7_int32* ipCidrRouteProto)
{
  return ipMapIpCidrRouteProtoGet(ipCidrRouteDest, ipCidrRouteMask,
                                  ipCidrRouteTos, ipCidrRouteNextHop,
                                  ipCidrRouteProto);
}

/*********************************************************************
* @purpose  Gets the number of seconds since this route was last
*           updated or otherwise determined to be correct.
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteAge      @b{(output)}  Pointer to route age
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Note that no semantics of "too old" can be implied except
*           through knowledge of the routing protocol by which the
*           route was learned.
*           Default value: { 0 }
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteAgeGet(L7_uint32 UnitIndex,
                               L7_uint32 ipCidrRouteDest,
                               L7_uint32 ipCidrRouteMask,
                               L7_uint32 ipCidrRouteTos,
                               L7_uint32 ipCidrRouteNextHop,
                               L7_uint32 *ipCidrRouteAge)
{
  return ipMapIpCidrRouteAgeGet(ipCidrRouteDest, ipCidrRouteMask,
                                ipCidrRouteTos, ipCidrRouteNextHop,
                                ipCidrRouteAge);
}

/*********************************************************************
* @purpose  Gets the route info
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteInfo     @b{(output)}  Pointer to route info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A reference to the MIB definitions specific to the
*           particular routing protocol which is responsible for
*           this route, as determined by the value specified in the
*           route's ipCidrRouteProto value. If this information is
*           not present, its value should be set to the OBJECT
*           IDENTIFIER { 0 0 }, which is a syntactically valid
*           object identifier, and any implementation conforming to
*           ASN.1 and the Basic Encoding Rules must be able to
*           generate and recognize this value.
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteInfoGet(L7_uint32 UnitIndex,
                                L7_uint32 ipCidrRouteDest,
                                L7_uint32 ipCidrRouteMask,
                                L7_uint32 ipCidrRouteTos,
                                L7_uint32 ipCidrRouteNextHop,
                                L7_int32* ipCidrRouteInfo)
{
  return ipMapIpCidrRouteInfoGet(ipCidrRouteDest, ipCidrRouteMask,
                                 ipCidrRouteTos, ipCidrRouteNextHop,
                                 ipCidrRouteInfo);
}

/*********************************************************************
* @purpose  Gets the Autonomous system number of the next hop.
*
* @param    UnitIndex             @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest       @b{(input)}   Destination ip address
* @param    ipCidrRouteMask       @b{(input)}   Route mask
* @param    ipCidrRouteTos        @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop    @b{(input)}   Next hop address
* @param    ipCidrRouteNextHopAS  @b{(output)}  Pointer to next hop
*                                 Autonomous system number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The Autonomous system number of the next hop. The
*           semantics of this object are determined by the routing-
*           protocol specified in the route's ipCidrRouteProto value.
*           When this object is unknown or not relevant its value
*           should be set to zero.
*           Default value: { 0 }
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteNextHopASGet(L7_uint32 UnitIndex,
                                     L7_uint32 ipCidrRouteDest,
                                     L7_uint32 ipCidrRouteMask,
                                     L7_uint32 ipCidrRouteTos,
                                     L7_uint32 ipCidrRouteNextHop,
                                     L7_int32* ipCidrRouteNextHopAS)
{
  return ipMapIpCidrRouteNextHopASGet(ipCidrRouteDest, ipCidrRouteMask,
                                      ipCidrRouteTos, ipCidrRouteNextHop,
                                      ipCidrRouteNextHopAS);
}

/*********************************************************************
* @purpose  Gets the primary routing metric for this route
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteMetric1  @b{(output)}  Pointer to route metric 1
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The semantics of this metric are determined by the routing-
*           protocol specified in the route's ipCidrRouteProto vlaue.
*           If this metric is not used, its value should be set to -1
*           Default value: -1
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteMetric1Get(L7_uint32 UnitIndex,
                                   L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric1)
{
  return ipMapIpCidrRouteMetric1Get(ipCidrRouteDest, ipCidrRouteMask,
                                    ipCidrRouteTos, ipCidrRouteNextHop,
                                    ipCidrRouteMetric1);
}

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteMetric2  @b{(output)}  Pointer to route metric 2
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The semantics of this metric are determined by the routing-
*           protocol specified in the route's ipCidrRouteProto vlaue.
*           If this metric is not used, its value should be set to -1
*           Default value: -1
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteMetric2Get(L7_uint32 UnitIndex,
                                   L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric2)
{
  return ipMapIpCidrRouteMetric2Get(ipCidrRouteDest, ipCidrRouteMask,
                                    ipCidrRouteTos, ipCidrRouteNextHop,
                                    ipCidrRouteMetric2);
}

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteMetric3  @b{(output)}  Pointer to route metric 3
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The semantics of this metric are determined by the routing-
*           protocol specified in the route's ipCidrRouteProto vlaue.
*           If this metric is not used, its value should be set to -1
*           Default value: -1
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteMetric3Get(L7_uint32 UnitIndex,
                                   L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric3)
{
  return ipMapIpCidrRouteMetric3Get(ipCidrRouteDest, ipCidrRouteMask,
                                    ipCidrRouteTos, ipCidrRouteNextHop,
                                    ipCidrRouteMetric3);
}

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteMetric4  @b{(output)}  Pointer to route metric 4
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The semantics of this metric are determined by the routing-
*           protocol specified in the route's ipCidrRouteProto vlaue.
*           If this metric is not used, its value should be set to -1
*           Default value: -1
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteMetric4Get(L7_uint32 UnitIndex,
                                   L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric4)
{
  return ipMapIpCidrRouteMetric4Get(ipCidrRouteDest, ipCidrRouteMask,
                                    ipCidrRouteTos, ipCidrRouteNextHop,
                                    ipCidrRouteMetric4);
}

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
* @param    UnitIndex           @b{(input)}   the unit for this operation
* @param    ipCidrRouteDest     @b{(input)}   Destination ip address
* @param    ipCidrRouteMask     @b{(input)}   Route mask
* @param    ipCidrRouteTos      @b{(input)}   Policy identifier
* @param    ipCidrRouteNextHop  @b{(input)}   Next hop address
* @param    ipCidrRouteMetric5  @b{(output)}  Pointer to route metric 5
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The semantics of this metric are determined by the routing-
*           protocol specified in the route's ipCidrRouteProto vlaue.
*           If this metric is not used, its value should be set to -1
*           Default value: -1
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpCidrRouteMetric5Get(L7_uint32 UnitIndex,
                                   L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric5)
{
  return ipMapIpCidrRouteMetric5Get(ipCidrRouteDest, ipCidrRouteMask,
                                    ipCidrRouteTos, ipCidrRouteNextHop,
                                    ipCidrRouteMetric5);
}

