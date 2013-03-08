/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ipmap_2096_api.c
*
* @purpose   Api functions for the IP Forwarding Table MIB, as 
*            specified in RFC 2096
*
* @component Ip Mapping Layer
*
* @comments  none
*
* @create    05/29/01
*
* @author    anayar
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "l7_common.h"
#include "rto_api.h"
#include "l7_ipinclude.h"

/*********************************************************************
* @purpose  Gets the number of current ipCidrRouteTable entries that
*           are not invalid
*
* @param    val         @b{(output)} Pointer to the number of entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    read-only
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpCidrRouteNumberGet(L7_int32 *val)
{
  L7_uint32 i = 0;
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  /* iterate best routes only */
  while (rtoNextRouteGet(&routeEntry, L7_TRUE) == L7_SUCCESS)
  {
    /* add num hops */
    i += routeEntry.ecmpRoutes.numOfRoutes;
  }

  *val = i;

  return L7_SUCCESS;
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
* @notes    A particular route to a particular destination with
*           a particular next hop. 
*
*           The current implementation ignores ipCidrRouteTos.
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpCidrRouteEntryGet(L7_uint32 ipCidrRouteDest,
                                 L7_uint32 ipCidrRouteMask,
                                 L7_uint32 ipCidrRouteTos,
                                 L7_uint32 ipCidrRouteNextHop)
{
  L7_uint32 i = 0;
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if (rtoPrefixFind(ipCidrRouteDest, ipCidrRouteMask, &routeEntry) == L7_SUCCESS)
  {
    /* check each next hop */
    for (i = 0; i < routeEntry.ecmpRoutes.numOfRoutes; i++)
      if(routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr == ipCidrRouteNextHop)
        return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determines if the next route entry exists
*
* Index values of the current entry:
* @param    ipCidrRouteDest         @b{(input/output)} The destination ip address
* @param    ipCidrRouteMask         @b{(input/output)} The associated route mask
* @param    ipCidrRouteTos          @b{(input/output)} The policy identifier
* @param    ipCidrRouteNextHop      @b{(input/output)} The next hop address
*
* @returns  L7_SUCCESS  If the next route entry exists
* @returns  L7_FAILURE  If the next route entry does not exist
*
* @notes    The current implementation ignores ipCidrRouteTos.
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpCidrRouteEntryGetNext(L7_uint32* ipCidrRouteDest,
                                     L7_uint32* ipCidrRouteMask,
                                     L7_uint32* ipCidrRouteTos,
                                     L7_uint32* ipCidrRouteNextHop)
{
  L7_uint32 i = 0;
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if (rtoPrefixFind(*ipCidrRouteDest, *ipCidrRouteMask, &routeEntry) == L7_SUCCESS)  
  {
    if(0 == *ipCidrRouteNextHop)
    {
      i = 0; /* first "next hop" */
    }
    else
    {
      /* check each next hop (to find the next bigger one!) */
      for (i = 0; i < routeEntry.ecmpRoutes.numOfRoutes; i++)
        if(routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr > *ipCidrRouteNextHop)
          break;
    }

    /* valid? */
    if(i < routeEntry.ecmpRoutes.numOfRoutes)
    {
      *ipCidrRouteDest = routeEntry.ipAddr;
      *ipCidrRouteMask = routeEntry.subnetMask;
      *ipCidrRouteTos = 0;
      *ipCidrRouteNextHop = routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr;
      return L7_SUCCESS;
    }
	
  }
  
  /* Get next route */
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));
  routeEntry.ipAddr     = *ipCidrRouteDest;
  routeEntry.subnetMask = *ipCidrRouteMask;

  /* get the next entry */
  while(rtoNextRouteGet(&routeEntry, L7_TRUE) == L7_SUCCESS)  /* best routes only */
  {
    if(routeEntry.ipAddr != *ipCidrRouteDest || 
       routeEntry.subnetMask != *ipCidrRouteMask)
    {
      *ipCidrRouteDest = routeEntry.ipAddr;
      *ipCidrRouteMask = routeEntry.subnetMask;
      *ipCidrRouteTos = 0;
      *ipCidrRouteNextHop = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;
    
      return L7_SUCCESS;
    }
  }
  
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the ifIndex value for this route entry
*
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
L7_RC_t ipMapIpCidrRouteIfIndexGet(L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteIfIndex)
{
  L7_uint32 i = 0;
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if (rtoPrefixFind(ipCidrRouteDest, ipCidrRouteMask, &routeEntry) == L7_SUCCESS)
  {
    /* check each next hop */
    for (i = 0; i < routeEntry.ecmpRoutes.numOfRoutes; i++)
    {
      if(routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr == ipCidrRouteNextHop)
      {
        *ipCidrRouteIfIndex = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the type of route entry
*
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
L7_RC_t ipMapIpCidrRouteTypeGet(L7_uint32 ipCidrRouteDest,
                                L7_uint32 ipCidrRouteMask,
                                L7_uint32 ipCidrRouteTos,
                                L7_uint32 ipCidrRouteNextHop,
                                L7_int32* ipCidrRouteType)
{
  L7_uint32 i = 0;
  L7_uint32 intf_if_index = 0;
  L7_uint32 intf_ip_addr;
  L7_uint32 intf_net_mask;
  L7_RC_t rc = L7_FAILURE;
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if (rtoPrefixFind(ipCidrRouteDest, ipCidrRouteMask, &routeEntry) == L7_SUCCESS)
  {
    /* check each next hop */
    for (i = 0; i < routeEntry.ecmpRoutes.numOfRoutes; i++)
    {
      if(routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr == ipCidrRouteNextHop)
      {
        intf_if_index = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum;
        rc =  L7_SUCCESS;
        break;
      }
    }
  }

  /* found entry? */
  if(rc != L7_SUCCESS)
    return rc;

  if (routeEntry.ipAddr != ipCidrRouteDest)
  {
    if (routeEntry.flags & L7_RTF_DISCARD)
    {
      *ipCidrRouteType = L7_IP_CIDR_ROUTE_TYPE_BLACKHOLE;
      rc = L7_SUCCESS;
    } 
    else if (routeEntry.flags & L7_RTF_REJECT)
    {
      *ipCidrRouteType = L7_IP_CIDR_ROUTE_TYPE_REJECT;
      rc = L7_SUCCESS;
    }
  }
  else
  {
    rc = ipMapRtrIntfIpAddressGet(intf_if_index,
                  (L7_IP_ADDR_t *)&intf_ip_addr,
                  (L7_IP_MASK_t *)&intf_net_mask);

    if (rc != L7_SUCCESS)
      return L7_FAILURE;

    if ((routeEntry.ipAddr & intf_net_mask) ==
      (intf_ip_addr & intf_net_mask))
    {
      *ipCidrRouteType = L7_IP_CIDR_ROUTE_TYPE_LOCAL;
      rc = L7_SUCCESS;
    } else
    {
      *ipCidrRouteType = L7_IP_CIDR_ROUTE_TYPE_REMOTE;
      rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Gets the routing mechanism via which this route was
*           learned.
*
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
L7_RC_t ipMapIpCidrRouteProtoGet(L7_uint32 ipCidrRouteDest,
                                 L7_uint32 ipCidrRouteMask,
                                 L7_uint32 ipCidrRouteTos,
                                 L7_uint32 ipCidrRouteNextHop,
                                 L7_int32* ipCidrRouteProto)
{
  L7_uint32 i = 0;
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if (rtoPrefixFind(ipCidrRouteDest, ipCidrRouteMask, &routeEntry) == L7_SUCCESS)
  {
    /* check each next hop */
    for (i = 0; i < routeEntry.ecmpRoutes.numOfRoutes; i++)
    {
      if(routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr == ipCidrRouteNextHop)
      {
        switch (routeEntry.protocol)
        {
        case RTO_LOCAL:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_LOCAL;
          break;
  
        case RTO_STATIC:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_NETMGMT;
          break;
  
          case RTO_MPLS:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_OTHER;
          break;
  
          case RTO_OSPF_INTRA_AREA:
          case RTO_OSPF_INTER_AREA:
          case RTO_OSPF_TYPE1_EXT:
          case RTO_OSPF_TYPE2_EXT:
          case RTO_OSPF_NSSA_TYPE1_EXT:
          case RTO_OSPF_NSSA_TYPE2_EXT:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_OSPF;
          break;
  
          case RTO_RIP:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_RIP;
          break;
  
          case RTO_DEFAULT:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_NETMGMT;
          break;
  
          case RTO_IBGP:
          case RTO_EBGP:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_BGP;
          break;
  
        default:
          *ipCidrRouteProto = L7_IP_CIDR_ROUTE_PROTO_OTHER;
          break;
        }

        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the number of seconds since this route was last
*           updated or otherwise determined to be correct.
*
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
L7_RC_t ipMapIpCidrRouteAgeGet(L7_uint32 ipCidrRouteDest,
                               L7_uint32 ipCidrRouteMask,
                               L7_uint32 ipCidrRouteTos,
                               L7_uint32 ipCidrRouteNextHop,
                               L7_uint32* ipCidrRouteAge)
{
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if (rtoPrefixFind(ipCidrRouteDest, ipCidrRouteMask, &routeEntry) == L7_SUCCESS)
  {
    *ipCidrRouteAge = simSystemUpTimeGet() - routeEntry.updateTime;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the route info
*
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
L7_RC_t ipMapIpCidrRouteInfoGet(L7_uint32 ipCidrRouteDest,
                                L7_uint32 ipCidrRouteMask,
                                L7_uint32 ipCidrRouteTos,
                                L7_uint32 ipCidrRouteNextHop,
                                L7_int32* ipCidrRouteInfo)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
* @purpose  Gets the Autonomous system number of the next hop.
*
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
L7_RC_t ipMapIpCidrRouteNextHopASGet(L7_uint32 ipCidrRouteDest,
                                     L7_uint32 ipCidrRouteMask,
                                     L7_uint32 ipCidrRouteTos,
                                     L7_uint32 ipCidrRouteNextHop,
                                     L7_int32* ipCidrRouteNextHopAS)
{
  return L7_NOT_IMPLEMENTED_YET;
}

/*********************************************************************
* @purpose  Gets the primary routing metric for this route
*
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
L7_RC_t ipMapIpCidrRouteMetric1Get(L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric1)
{
  L7_uint32 i = 0;
  L7_routeEntry_t routeEntry;
  bzero((char *)&routeEntry, sizeof(L7_routeEntry_t));

  if (rtoPrefixFind(ipCidrRouteDest, ipCidrRouteMask, &routeEntry) == L7_SUCCESS)
  {
    /* check each next hop */
    for (i = 0; i < routeEntry.ecmpRoutes.numOfRoutes; i++)
    {
      if(routeEntry.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr == ipCidrRouteNextHop)
      {
        *ipCidrRouteMetric1 = routeEntry.metric;
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route
*
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
L7_RC_t ipMapIpCidrRouteMetric2Get(L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric2)
{
  *ipCidrRouteMetric2 = -1;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
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
L7_RC_t ipMapIpCidrRouteMetric3Get(L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric3)
{
  *ipCidrRouteMetric3 = -1;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
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
L7_RC_t ipMapIpCidrRouteMetric4Get(L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric4)
{
  *ipCidrRouteMetric4 = -1;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
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
L7_RC_t ipMapIpCidrRouteMetric5Get(L7_uint32 ipCidrRouteDest,
                                   L7_uint32 ipCidrRouteMask,
                                   L7_uint32 ipCidrRouteTos,
                                   L7_uint32 ipCidrRouteNextHop,
                                   L7_int32* ipCidrRouteMetric5)
{
  *ipCidrRouteMetric5 = -1;
  return L7_SUCCESS;
}

