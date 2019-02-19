/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  ip_1213_api.h
*
* @purpose   Ip Api functions for rfc 1213
*
* @component ip statistics support
*
* @comments  This is a common header file for Switching package &
*            Routing package versions of the file ip_1213_api.c
*
* @create    06/26/2001
*                       
* @author    anayar
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef _IP_1213_API_H
#define _IP_1213_API_H_

/* Begin Function Prototypes */
/*********************************************************************
* @purpose Gets the IP Router Admin Mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLE, L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAIlURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t ipmIpRtrAdminModeGet(L7_uint32 *val);

/*********************************************************************
* @purpose Set the IP Router Admin Mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} L7_ENABLE, L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t ipmIpRtrAdminModeSet(L7_uint32 val);

/*********************************************************************
* @purpose  Sets Ip default Time To Live
*
* @param    *val  @b{(output)} Default TTL value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*       
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t ipmIpDefaultTTLGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Sets Ip default Time To Live
*
* @param    ttl  @b{(input)} Default ttl value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*       
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t ipmIpDefaultTTLSet(L7_uint32 ttl);

/*********************************************************************
* @purpose  Get the total number of input datagrams received from interfaces
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments (ip packets handled by only the NP + ip packets handled
*           by the Operating System IP stack)   = 
*         
*           ( DAPI_STATS_RECEIVED_L3_IN +
*            vxworks ipInReceives -
*            DAPI_STATS_RECEIVED_L3_IP_TO_CPU -
*            DAPI_STATS_RECEIVED_L3_MAC_MCAST_DISCARDS )
*                                                        
* @end
*********************************************************************/
extern L7_RC_t ipmIpInReceivesGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of input datagrams discarded due to headers
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder )  
*         
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpInHdrErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of input datagrams discarded because
*           the IP address in their IP header's destination
*           field was not a valid address to be received at
*           this entity.
*          
* @param    *val    @b{(output)} datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder )  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpInAddrErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of input datagrams for which this
*           entity was not their final IP destination, as a
*           result of which an attempt was made to find a
*           route to forward them to that final destination.
*          
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets forwarded by the NP + packets forwarded by the 
*           Operating System IP stack + packets forwarded by the
*           IP Mapping layer forwarder )  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpForwDatagramsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of locally-addressed datagrams
*           received successfully but discarded because of an
*           unknown or unsupported protocol.
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments ( packets discarded by the Operating System IP stack 
*           + packets discarded by the IP Mapping layer forwarder )  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpInUnknownProtosGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of input IP datagrams for which no
*           problems were encountered to prevent their
*           continued processing, but which were discarded
*
*
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder + packets discarded prior to
*           receipt in the forwarder layer)  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpInDiscardsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the total number of input datagrams successfully
*           delivered to IP user-protocols (including ICMP).
*          
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*          
* @comments (packets delivered to the Operating System IP stack's user protocols
*           + packets delivered to the IP Mapping layer forwarder's
*           user protocols (but not delivered to the IP stack))
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpInDeliversGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the total number of IP datagrams which local IP
*           user-protocols (including ICMP) supplied to IP in
*           requests for transmission.  
*          
* @param    *val   @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that this counter
*           does not include any datagrams counted in
*           ipForwDatagrams
*
* @comments (packets submitted by the Operating System IP stack
*           + packets submitted by the IP Mapping layer forwarder's
*           user protocols (but which bypass the OS IP stack))
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpOutRequestsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of output IP datagrams for which no
*           problem was encountered to prevent their
*           transmission to their destination, but which were
*           discarded (e.g., for lack of buffer space).  
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpOutDiscardsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams discarded because no
*           route could be found to transmit them to their
*           destination.  
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments (packets discarded by the NP + packets discarded by the 
*           Operating System IP stack + packets discarded by the
*           IP Mapping layer forwarder + packets discarded prior to
*           receipt in the forwarder layer)  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpOutNoRoutesGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the maximum number of seconds which received
*           fragments are held while they are awaiting
*           reassembly at this entity
*
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpReasmTimeoutGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP fragments received which needed
*           to be reassembled at this entity
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpReasmReqdsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams successfully re-assembled
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpReasmOKsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of failures detected by the IP re-
*           assembly algorithm (for whatever reason: timed
*           out, errors, etc).  
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpReasmFailsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           successfully fragmented at this entity
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpFragOKsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           discarded because they needed to be fragmented at
*           this entity but could not be, e.g., because their
*           Don't Fragment flag was set.
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpFragFailsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagram fragments that have
*           been generated as a result of fragmentation at
*           this entity.
*          
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpFragCreatesGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of routing entries which were chosen
*           to be discarded even though they are valid.  
*
* @param    *val  @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRoutingDiscardsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the number of routing entries which were chosen
*           to be discarded even though they are valid.  
*
* @param    val    @b{(input)} Address
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRoutingDiscardsSet(L7_uint32 val);

/*********************************************************************
* @purpose  Determines if the Ip Address entry corresponding to 
*           this ip address exists
*          
* @param    ipAddr     @b{(input)} The ip address to which this entry's
*                               addressing information pertains.
* @param    intIfNum   @b{(input)} Internal interface number for entry
*
* @returns  L7_SUCCESS  if the specified address entry exists
* @returns  L7_FAILURE  if the specified address entry does not exist
*
* @comments Each entry contains addressing information for one of
*           this entity's IP addresses. 
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpAdEntAddrGet(L7_uint32 ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the ip address from the entry's addressing information
*           which is next to the entry to which this ip address pertains
*          
* @param    *ipAddr   @b{(input)} The ip address to which this entry's
*                   addressing information pertains. @line
*                     @b{(output)} The ip address of the next entry
* @param    *intIfNum  @b{(input)} Internal interface number for entry
*                      @b{(output)} Internal interface of next entry
*
* @returns  L7_SUCCESS  if the next route entry exists
* @returns  L7_FAILURE  if the next route entry does not exist
*
* @comments Each entry contains addressing information for one of
*           this entity's IP addresses. 
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpAdEntAddrGetNext(L7_uint32* ipAddr, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Gets the index value which uniquely identifies the
*           interface to which this entry is applicable
*          
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *ifIndex   @b{(output)} The ifindex of this entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The ifIndex is the index value which uniquely identifies
*           the interface to which this entry is applicable. The interface
*           identified by a particular value of this index is the same
*           interface as identified by the same value of ifIndex.
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpEntIfIndexGet(L7_uint32 intIfNum, L7_uint32 *ifIndex);

/*********************************************************************
* @purpose  Gets the subnet mask associated with the IP address of
*           this entry.  
*
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *netmask   @b{(output)} The subnet mask associated with the
*                                ip address of this entry. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The value of the mask is an IP
*           address with all the network bits set to 1 and all
*           the hosts bits set to 0."  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpAdEntNetMaskGet (L7_uint32 intIfNum, L7_uint32 *netmask);

/*********************************************************************
* @purpose  Gets the value of the least-significant bit in the IP
*           broadcast address used for sending datagrams on
*           the (logical) interface associated with the IP
*           address of this entry
*
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *bcastAddr @b{(output)} bcast address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The standard specifies that any host id consisting of all
*           1s is reserved for broadcast. 
*           bcast addr = (ipaddr & netmask) | ~netmask 
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpAdEntBCastGet (L7_uint32 intIfNum, L7_uint32 *bcastAddr);

/*********************************************************************
* @purpose  Gets the size of the largest IP datagram which this
*           entity can re-assemble from incoming IP fragmented
*           datagrams received on this interface
*
* @param    ipAddr     @b{(input)} the ip address associated with this
*                               entry.
* @param    *maxSize   @b{(output)} size of largest re-assembled datagram
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   This return code is not used currently
*
* @comments As the OS performs reassembly this value will be retrieved
*           from the OS.
*           For Linux: The value will be obtained from the file
*           /proc/sys/net/ipv4/ipfrag_high_thresh
*           For VxWorks: The value will be obtained from the mib
*           using the call m2IpAddrTblEntryGet()
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpAdEntReasmMaxSizeGet(L7_uint32 ipAddr, L7_uint32 *maxSize);

/*********************************************************************
* @purpose  Determines if a route entry exists for the specified route
*           destination.
*
* @param    routeDest  @b{(input)} Route destination for this entry.
*
* @returns  L7_SUCCESS If the corresponding entry exists.
* @returns  L7_FAILURE If the corresponding entry does not exist
*
* @comment   
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteEntryGet(L7_uint32 routeDest);

/*********************************************************************
* @purpose  Determines if the next route entry exists for the specified route
*           destination.
*
* @param    *routeDest  @b{(input)} Route destination for this entry. @line
*                       @b{(output)} Route destination of next entry.
*
* @returns  L7_SUCCESS If the corresponding entry exists
* @returns  L7_FAILURE If the corresponding entry does not exist
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteEntryGetNext(L7_uint32 *routeDest);

/*********************************************************************
* @purpose  Gets the index value which uniquely identifies the
*           local interface through which the next hop of this
*           route should be reached.  
*
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *intIfNum  @b{(output)} internal interface number for this entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteIfIndexGet(L7_uint32 routeDest, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Gets the primary routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric1   @b{(output)} route metric 1
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric1Get(L7_uint32 routeDest, L7_uint32 *metric1);

/*********************************************************************
* @purpose  Sets the primary routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric1    @b{(input)} route metric 1
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric1Set(L7_uint32 routeDest, L7_uint32 metric1);

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric2   @b{(output)} Route metric 2
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric2Get(L7_uint32 routeDest, L7_uint32 *metric2);

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric2    @b{(input)} Route
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric2Set(L7_uint32 routeDest, L7_uint32 metric2);

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric3   @b{(output)} route metric 3
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric3Get(L7_uint32 routeDest, L7_uint32 *metric3);

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric3    @b{(input)} Route metric 3
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric3Set(L7_uint32 routeDest, L7_uint32 metric3);

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric4   @b{(output)} route metric 4
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric4Get(L7_uint32 routeDest, L7_uint32 *metric4);

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
* @param    intIfNum   @b{(input)} the internal interface number associated
*                               with this entry
* @param    metric4    @b{(input)} route metric 4
*
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric4Set(L7_uint32 routeDest, L7_uint32 *metric4);

/*********************************************************************
* @purpose  Gets the IP address of the next hop of this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    nextHop    @b{(output)} ip address of the next hop of this
*                                route entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteNextHopGet(L7_uint32 routeDest, L7_uint32 *nextHop);

/*********************************************************************
* @purpose  Sets the IP address of the next hop of this route  
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    nextHop    @b{(input)} ip address of the next hop of this
*                                route entry
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteNextHopSet(L7_uint32 routeDest, L7_uint32 nextHop);

/*********************************************************************
* @purpose  Gets the type of route.   
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeType  @b{(output)} The type of route.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that the values direct(3) and indirect(4) refer to 
*           the notion of direct and indirect routing in the IP
*           architecture.  
*
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteTypeGet(L7_uint32 routeDest, L7_uint32 *routeType);

/*********************************************************************
* @purpose  Set the type of route.    
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeType  @b{(input)} The type of route.
*
* @returns  L7_NOT_SUPPORTED
*
* @comments Note that the values
*           direct(3) and indirect(4) refer to the notion of
*           direct and indirect routing in the IP
*           architecture. 
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteTypeSet(L7_uint32 routeDest, L7_uint32 routeType);

/*********************************************************************
* @purpose  Gets the routing mechanism via which this route was
*           learned.  
*
* @param    routeDest   @b{(input)} route destination for this entry
* @param    *routeProto @b{(output)} protocol via which this route
*                               was learned
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteProtoGet(L7_uint32 routeDest, L7_uint32 *routeProto);

/*********************************************************************
* @purpose  Gets the number of seconds since this route was last
*           updated or otherwise determined to be correct
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *routeAge  @b{(output)} seconds since the route was last updated
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteAgeGet(L7_uint32 routeDest, L7_uint32 *routeAge);

/*********************************************************************
* @purpose  Sets the number of seconds since this route was last
*           updated or otherwise determined to be correct
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeAge   @b{(input)} Seconds since this route was last
*                               updated
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteAgeSet(L7_uint32 routeDest, L7_uint32 routeAge);

/*********************************************************************
* @purpose  Gets the mask to be logical-ANDed with the
*           destination address before being compared to the
*           value in the ipRouteDest field
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *routeMask @b{(output)} route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMaskGet(L7_uint32 routeDest, L7_uint32 *routeMask);

/*********************************************************************
* @purpose  Sets the mask to be logical-ANDed with the
*           destination address before being compared to the
*           value in the ipRouteDest field
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeMask  @b{(input)} route mask
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMaskSet(L7_uint32 routeDest, L7_uint32 routeMask);

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric5   @b{(output)} route metric 5
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric5Get(L7_uint32 routeDest, L7_uint32 *metric5);

/*********************************************************************
* @purpose  Sets an alternate routing metric for this route
*
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric5   @b{(output)} route metric 5
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteMetric5Set(L7_uint32 routeDest, L7_uint32 *metric5);

/*********************************************************************
* @purpose  Gets reference to MIB definitions specific to the
*           particular routing protocol which is responsible
*           for this route 
*
* @param    intIfNum   @b{(input)} internal interface number associated
*                               with this entry
* @param    *oid1      @b{(output)} object id
* @param    *oid2      @b{(output)} object id
*
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpRouteInfoGet(L7_uint32 routeDest, L7_uint32 *oid1, L7_uint32* oid2);

/*********************************************************************
* @purpose  determines if the entry exists in the ipNetToMedia table
*          
* @param    ipNetToMediaIfIndex        @b{(input)} interface number
* @param    ipNetToMediaNetAddress     @b{(input)} network address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpNetToMediaEntryGet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress);

/*********************************************************************
* @purpose  Retrieves the next valid ARP entry for the ipNetToMedia table
*          
* @param    *ipNetToMediaIfIndex        @b{(input)} internal interface number of current entry @line
*                                       @b{(output)} internal interface number of next entry
* @param    *ipNetToMediaNetAddress     @b{(input)} network address of current entry @line
*                                       @b{(output)} network address of next entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Utility function to wrap the 
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpNetToMediaEntryArpNextGet(L7_uint32 *ipNetToMediaIfIndex, L7_uint32 *ipNetToMediaNetAddress);

/*********************************************************************
* @purpose  Get the next valid entry in the ipNetToMedia table
*          
* @param    *ipNetToMediaIfIndex        @b{(input)} internal interface number of current entry @line
*                                       @b{(output)} internal interface number of next entry
* @param    *ipNetToMediaNetAddress     @b{(input)} network address of current entry @line
*                                       @b{(output)} network address of next entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpNetToMediaEntryNextGet(L7_int32 *ipNetToMediaIfIndex, L7_uint32 *ipNetToMediaNetAddress);

/*********************************************************************
* @purpose  Get the ipNetToMediaPhysAddress object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    *physAddress               @b{(output)} physAddress buffer
* @param    *addrLength                @b{(output)} physAddress buffer length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpNetToMediaPhysAddressGet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 *addrLength);

/*********************************************************************
* @purpose  Set the ipNetToMediaPhysAddress object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    physAddress                @b{(input)}  physAddress buffer
* @param    addrLength                 @b{(input)}  physAddress buffer length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpNetToMediaPhysAddressSet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 addrLength);

/*********************************************************************
* @purpose  get the ipNetToMediaType object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    *ipNetToMediaType          @b{(output)} ipNetToMediaType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpNetToMediaTypeGet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 *ipNetToMediaType);

/*********************************************************************
* @purpose  set the ipNetToMediaType object
*          
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    ipNetToMediaType           @b{(input)}  ipNetToMediaType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIpNetToMediaTypeSet(L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 ipNetToMediaType);

/*********************************************************************
* @purpose  Get the total number of ICMP messages which the entity 
*           received.
*
* @param    *val   @b{(output)} Number of received icmp messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The total number of ICMP messages which the entity
*           received.  Note that this counter includes all those 
*           counted by icmpInErrors.
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInMsgsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*
* @param    *val  @b{(output)} Number of received icmp messages with errors
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Destination Unreachable messages
*           received.
*
* @param    *val   @b{(output)}  Number of icmp dest unreach messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of ICMP Destination Unreachable messages
*           received.
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInDestUnreachsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Time Exceeded messages received.
*
* @param    *val   @b{(output)} Number of icmp time exceeded messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of ICMP Time Exceeded messages received.
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInTimeExcdsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Parameter Problem messages received
*
* @param    *val   @b{(output)} Number of received icmp parameter
*                       problem messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInParmProbsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Source Quench messages.
*
* @param    *val   @b{(output)} Number of received icmp sorce quench messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInSrcQuenchsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Redirect messages received.
*
* @param    *val   @b{(output)} Number of received icmp redirect messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInRedirectsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Echo (request) messages received. 
*
* @param    *val   @b{(output)} Number of received icmp echo request messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInEchosGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Echo Reply messages received
*
* @param    *val   @b{(output)} Number of received icmp echo response messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInEchoRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp (request) messages received
*
* @param    *val   @b{(output)} Number of received icmp timestamp messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInTimestampsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp Reply messages
*
* @param    *val  @b{(output)} Number of received icmp timestamp response messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInTimestampRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Address Mask Request messages received
*
* @param    *val  @b{(output)} Number of received icmp address mask req messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInAddrMasksGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Address Mask Reply messages received
*
* @param    *val   @b{(output)} Number of received icmp address mask replies messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpInAddrMaskRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of ICMP messages which this entity
*           attempted to send.
*
* @param    *val  @b{(output)} Number of icmp messages sent
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that this counter includes all those counted by
*           icmpOutErrors
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutMsgsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP messages which this entity did not
*           send due to problems discovered within ICMP.
*
* @param    *val    @b{(output)} Number of icmp messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This includes problems such as lack of buffers. This
*           value should not include errors discovered outside of
*           the ICMP layer such as the inability of IP to route the
*           resultant datagram. In some implementations there may
*           be no types of error which contribute to this counter's
*           value.
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Destination Unreachable messages
*           sent
*
* @param    *val  @b{(output)} Number of sent icmp destination
*                          unreachable messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutDestUnreachsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Time Exceeded messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp time
*                          exceeded messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutTimeExcdsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Parameter Problem messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp parameter
*                          problem messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutParmProbsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Source Quench messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp source quench messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutSrcQuenchsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Redirect messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp redirect messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments For a host, this object will always be zero, since
*           hosts do not send redirects.
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutRedirectsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Echo (request) messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp echo request messages
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutEchosGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Echo Reply messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp echo reply messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutEchoRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp (request) messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp timestamp request messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutTimestampsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Timestamp Reply messages sent
*
* @param    *val  @b{(output)} Number of sent icmp timestamp reply messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutTimestampRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Address mask Request messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp address mask request
*                          messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutAddrMasksGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Get the number of ICMP Address Mask Reply messages sent.
*
* @param    *val  @b{(output)} Number of sent icmp address mask reply messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t ipmIcmpOutAddrMaskRepsGet(L7_uint32 *val);

/* End Function Prototypes */

#endif /* _IP_1213_API_H_ */

