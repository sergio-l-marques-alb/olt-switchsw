/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l7_mcast_api.h
*
* @purpose   MCAST vendor-specific function prototypes
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    05/17/2001
*
* @author    Vijay S.
*
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#ifndef _MCASTMAP_API_H_
#define _MCASTMAP_API_H_

#include "l3_comm_structs.h"
#include "l3_mcast_commdefs.h"
#include "l7_mgmd_api.h"
#include "async_event_api.h"
#include "l7handle_api.h"
#include "rto_api.h"

typedef struct mcastRPFInfo_s {
  L7_uint32                 rpfIfIndex; /* RPF's Router Interface Index */
  L7_inet_addr_t            rpfNextHop;
  L7_RTO_PROTOCOL_INDICES_t rpfRouteProtocol;
  L7_inet_addr_t            rpfRouteAddress;
  L7_uint32                 rpfRouteMetricPref;
  L7_uint32                 rpfRouteMetric;
  RTO_ROUTE_EVENT_t         status; /* RTO_ADD_ROUTE, RTO_DELETE_ROUTE, RTO_CHANGE_ROUTE */
  L7_uint32                 prefixLength;
} mcastRPFInfo_t;

typedef enum
{
    MCAST_RESERVED_STATE_CHANGE = 0,
    MCAST_RTR_ADMIN_MODE_ENABLE_PENDING, /* 1: Admin mode is being enabled*/
    MCAST_RTR_ADMIN_MODE_ENABLED,        /* 2: Admin mode is enabled     */
    MCAST_RTR_ADMIN_MODE_DISABLE_PENDING,/* 3: Admin mode is being disabled*/
    MCAST_RTR_ADMIN_MODE_DISABLED,       /* 4: Admin mode is disabled     */
}MCAST_RTR_EVENT_MODE_CHANGE_t;

typedef L7_RC_t (*mcastMapStaticMRouteNotifyFn_t)(L7_uchar8 addrFamily,
                            L7_uint32 eventType, L7_uint32 msgLen, void *pData);

/*********************************************************************
* @purpose  Determine if the MCAST component has been initialized
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL mcastMapMcastIsOperational(L7_uchar8 family);
/**********************************************************************
* @purpose  Set the MCAST administrative mode
*
* @param    mode        @b{(input)}  admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapMcastAdminModeSet(L7_uint32 mode);

/***************************************************************
*
* @purpose  Get the MCAST administrative mode
*
* @param    mode       @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t mcastMapMcastAdminModeGet(L7_uint32 *mode);
/***************************************************************
* @purpose   Gets the multicast protocal running on the router.
*
* @param     currentMcastProtocol  @b{(output)} mcast protocol
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
***************************************************************/
L7_RC_t mcastMapIpCurrentMcastProtocolGet(L7_uint32 family, 
                                          L7_MCAST_IANA_PROTO_ID_t *currentMcastProtocol);
L7_BOOL mcastMapMcastProtocolIsOperational(L7_uint32 family, 
                            L7_MCAST_IANA_PROTO_ID_t currentMcastProtocol);

/***************************************************************
* @purpose   Sets the multicast protocal running on the router.
*
* @param    family                @b{(input)} IP address family
* @param    currentMcastProtocol  @b{(input)} mcast protocol
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
***************************************************************/
L7_RC_t mcastMapIpCurrentMcastProtocolSet(L7_uint32 family, L7_uint32 currentMcastProtocol);

/**********************************************************************
*
* @purpose  Get the IP Multicast route table  entry count
*
* @param    count          @b{(output)}   Number of Entires in Mroute Table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
**********************************************************************/
L7_RC_t mcastMapIpMRouteEntryCountGet(L7_uchar8 family, L7_uint32 *count);

/*********************************************************************
*
* @purpose  Get the IP Multicast route table  Highest entry count
*
* @param    count          @b{(output)}   Highest Entry Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
**********************************************************************/
L7_RC_t mcastMapIpMRouteHighestEntryCountGet(L7_uint32 *count);

/**********************************************************************
*
* @purpose  Get the IP Multicast route table  Maximum Size
*
* @param    size           @b{(output)}   Maximum Size of Mroute Table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
**********************************************************************/
L7_RC_t mcastMapIpMRouteTableMaxSizeGet(L7_uint32 *size);

/*********************************************************************
* @purpose     Gets the address of the upstream neighbor from which
*              IP datagrams from these sources to this multicast
*              address are received.
*
* @param       ipMRtGroup   @b{(input)} mroute group
* @param       ipMRtSrc     @b{(input)} mroute source
* @param       inMRtSrcMask @b{(input)} mroute mask
* @param       upstmNbr     @b{(output)} upstream neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments returns 0 if upstream neighbor is unknown.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteUpstreamNbrGet(L7_uchar8 family,
                                       L7_inet_addr_t *ipMRtGroup,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *inMRtSrcMask,
                                       L7_inet_addr_t *upstmNbr);
/*********************************************************************
* @purpose     Gets the interface index on which IP datagrams sent by
*              these sources to this multicast address are received.
*              corresponding to the index received.
*
* @param    ipMRouteGroup     @b{(input)}  Multicast Group address
* @param    ipMRouteSrc       @b{(input)}  Source address
* @param    ipMRouteSrcMask   @b{(input)}  Mask Address
* @param    inIfIndex         @b{(output)}  interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    A value of 0 indicates that datagrams are not subject
*           to an incoming interface check, but may be accepted on
*           multiple interfaces.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteIfIndexGet(L7_uchar8 family,
                                   L7_inet_addr_t *ipMRouteGroup,
                                   L7_inet_addr_t *ipMRouteSrc,
                                   L7_inet_addr_t *ipMRouteSrcMask,
                                   L7_uint32 *inIfIndex);

/*********************************************************************
* @purpose     Gets the time since the multicast routing information
*              represented by this entry was learned by the router.
*
* @param    ipMRouteGroup    @b{(input)} Multicast Group address
* @param    ipMRouteSrc      @b{(input)} Source address
* @param    ipMRouteSrcMask  @b{(input)} Mask Address
* @param    upTime           @b{(output)} uptime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteUpTimeGet(L7_uchar8 family,
                                  L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSrc,
                                  L7_inet_addr_t *ipMRouteSrcMask,
                                  L7_uint32 *upTime);

/*********************************************************************
* @purpose     Gets the minimum amount of time remaining before  this
*              entry will be aged out.
*
* @param    ipMRouteGroup     @b{(input)} Multicast Group address
* @param    ipMRouteSrc       @b{(input)} Source address
* @param    ipMRouteSrcMask   @b{(input)} Mask Address
* @param    expire            @b{(output)} expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    The value of 0 indicates that the entry is not
*           subject to aging.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteExpiryTimeGet(L7_uchar8 family,
                                      L7_inet_addr_t *ipMRouteGroup,
                                      L7_inet_addr_t *ipMRouteSrc,
                                      L7_inet_addr_t *ipMRouteSrcMask,
                                      L7_uint32 *expire);

/**********************************************************************
* @purpose     Gets multicast routing protocol
*
* @param    ipMRouteGroup      @b{(input)} Multicast Group address
* @param    ipMRouteSrc        @b{(input)} Source address
* @param    ipMRouteSrcMask    @b{(input)} Mask Address
* @param    protocol           @b{(output)} protocol value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    The multicast routing protocol via which this multicast
*           forwarding entry was learned.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteProtocolGet(L7_uchar8 family,
                                    L7_inet_addr_t *ipMRouteGroup,
                                    L7_inet_addr_t *ipMRouteSrc,
                                    L7_inet_addr_t *ipMRouteSrcMask,
                                    L7_uint32 *protocol);


/**********************************************************************
* @purpose     Gets the routing protocol used to find the upstream or
*              parent interface.
*
* @param    ipMRtGrp        @b{(input)} mcast group
* @param    ipMRtSrc        @b{(input)} mcast source 
* @param    inMRtSrcMask    @b{(input)} mcast mask
* @param    protocol        @b{(output)} protocol value
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments
*
* @end
********************************************************************/
L7_RC_t mcastMapIpMRouteRtProtoGet(L7_inet_addr_t *ipMRtGrp,
                                   L7_inet_addr_t *ipMRtSrc,
                                   L7_inet_addr_t *inMRtSrcMask,
                                   L7_uint32 *protocol);

/*********************************************************************
* @purpose    Gets outgoing interfaces for the given source address,
*             group address and source mask.
*
* @param    ipMRtGrp           @b{(inout)} mcast group
* @param    ipMRtSrc           @b{(inout)} mcast source
* @param    ipMRtSrcMask       @b{(inout)} source mask
* @param    intIfNum           @b{(inout)} interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mcastMapIpMRouteOutIntfEntryNextGet(L7_uchar8 family,
                                            L7_inet_addr_t *ipMRtGrp,
                                            L7_inet_addr_t *ipMRtSrc,
                                            L7_inet_addr_t *ipMRtSrcMask,
                                            L7_uint32 *intIfNum);

/**********************************************************************
* @purpose   Gets the address field of the route used to find the
*            upstream or parent interface
*
* @param    ipMRtGrp           @b{(input)} mcast group
* @param    ipMRtSrc           @b{(input)} mcast source
* @param    ipMRtSrcMask       @b{(input)} mcast mask
* @param    rtAddr             @b{(output)} route address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteRtAddressGet(L7_inet_addr_t *ipMRtGrp,
                                     L7_inet_addr_t *ipMRtSrc,
                                     L7_inet_addr_t *inMRtSrcMask,
                                     L7_inet_addr_t *rtAddr);

/**********************************************************************
* @purpose     Gets the mask associated with the route used to find the
*              upstream or parent interface.
*
* @param    ipMRtGrp           @b{(input)} mcast group address
* @param    ipMRtSrc           @b{(input)} mcast source
* @param    ipMRtSrcMask       @b{(input)} mcast source mask
* @param    rtMask             @b{(output)} mroute mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments    none
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteRtMaskGet(L7_inet_addr_t *ipMRtGrp,
                                  L7_inet_addr_t *ipMRtSrc,
                                  L7_inet_addr_t *inMRtSrcMask,
                                  L7_inet_addr_t *rtMask);

/**********************************************************************
* @purpose     Gets the type of route used (unicast or multicast)
*
* @param    ipMRtGrp            @b{(input)} mcast group address
* @param    ipMRtSrc            @b{(input)} mcast source
* @param    ipMRtSrcMask        @b{(input)} source mask
* @param    rtType              @b{(output)} route type
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments       The reason the given route was placed in the MRIB.
*
* @end
*********************************************************************/
L7_RC_t mcastMapIpMRouteRtTypeGet(L7_inet_addr_t *ipMRtGrp,
                                  L7_inet_addr_t *ipMRtSrc,
                                  L7_inet_addr_t *inMRtSrcMask,
                                  L7_uint32 *rtType);

/**********************************************************************
* @purpose     Gets the Rpf address for the given index
*
* @param    ipMRtGrp           @b{(input)} mcast group address
* @param    ipMRtSrc           @b{(input)} mcast source
* @param    ipMRtSrcMask       @b{(input)} source mask
* @param    rpfAddr            @b{(output)} RPF address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
* @returns     rpfAddr
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapIpMRouteRpfAddressGet(L7_uchar8 family,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_inet_addr_t *rpfAddr);

/**********************************************************************
* @purpose   Gets PIM specific flags related to a multicast state entry
*
* @param     ipMRouteGroup        @b{(input)} mroute group
* @param     ipMRouteSource       @b{(input)} mroute source
* @param     ipMRouteSourceMask   @b{(input)} mroute mask
* @param     flag                 @b{(output)} flag
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  Flag is '0' for RPT (Shared tree) and '1' for SPT(Source Tree)
*
* @end
*********************************************************************/
L7_RC_t mcastMapIpMRouteFlagsGet(L7_uchar8 family,
                                 L7_inet_addr_t *ipMRouteGroup, 
                                 L7_inet_addr_t *ipMRouteSource,
                                 L7_inet_addr_t *ipMRouteSourceMask, 
                                 L7_uint32 *flag);

/**********************************************************************
* @purpose  Get the IP Multicast route table  entry
*
* @param    ipMRouteGroup      @b{(input)} Multicast Group address
* @param    ipMRouteSource     @b{(input)} Source address
* @param    ipMRouteSourceMask @b{(input)} Mask Address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteEntryGet(L7_uchar8 family,
                                 L7_inet_addr_t *ipMRouteGroup,
                                 L7_inet_addr_t *ipMRouteSource,
                                 L7_inet_addr_t *ipMRouteSourceMask);

/**********************************************************************
* @purpose  Get the IP Multicast route table Next entry
*
* @param    ipMRtGrp           @b{(inout)} mcast group
* @param    ipMRtSrc           @b{(inout)} mcast source
* @param    ipMRtSrcMask       @b{(inout)} source mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteEntryNextGet(L7_uchar8 family,
                                     L7_inet_addr_t *ipMRtGrp,
                                     L7_inet_addr_t *ipMRtSrc,
                                     L7_inet_addr_t *ipMRtSrcMask);

/*********************************************************************
* @purpose  Checks whether the entry corresponding to the given source
*           group, and source mask is there in the next hop routing 
*           table
*
* @param    ipMRtNextHopGrp       @b{(input)} next hop group
* @param    ipMRtNextHoptSrc      @b{(input)} next hop source
* @param    ipMRtNextHopSrcMask   @b{(input)} next hop source mask
* @param    ipMRtNextHopIfIndex   @b{(input)} next hop index
* @param    ipMRtNextHopAddr      @b{(input)} next hop address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapNextHopMRouteEntryGet(L7_inet_addr_t *ipMRtNextHopGrp,
                                        L7_inet_addr_t *ipMRtNextHopSrc,
                                        L7_inet_addr_t *ipMRtNextHopSrcMask,
                                        L7_uint32 ipMRtNextHopIfIndex,
                                        L7_inet_addr_t *ipMRtNextHopAddr);

/*********************************************************************
* @purpose  Gets the next entry in the next hop routing table for the
*           given source, group and source mask.
*
* @param    ipMRtNextHopIfIndex  @b{(input)} next hop index
* @param    ipMRtNextHopGrp      @b{(inout)} next hop group
* @param    ipMRtNextHoptSrc     @b{(inout)} next hop source
* @param    ipMRtNextHopSrcMask  @b{(inout)} next hop source mask
* @param    ipMRtNextHopAddr     @b{(inout)} next hop address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapNextHopMRouteEntryNextGet(L7_uint32 ipMRtNextHopIfIndex,
                                            L7_inet_addr_t *ipMRtNextHopGrp,
                                            L7_inet_addr_t *ipMRtNextHopSrc,
                                            L7_inet_addr_t *ipMRtNextHopSrcMask,
                                            L7_inet_addr_t *ipMRtNextHopAddr);

/**********************************************************************
* @purpose  Set the Ttl threshold of the specified interface
*
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    ttl                 @b{(input)} TTL threshold
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIntfTtlThresholdSet(L7_uint32 intIfNum, L7_uint32 ttl);

/*********************************************************************
* @purpose  Get the Ttl threshold of the specified interface
*
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    ttl                 @b{(output)} TTL threshold
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIntfTtlThresholdGet(L7_uint32 intIfNum, L7_uint32 *ttl);

/*********************************************************************
* @purpose     Gets the multicast protocol running on the interface
*
* @param    intfNum             @b{(input)} interface number
* @param    protocol            @b{(output)} protocol value
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteInterfaceProtocolGet(L7_uint32 intIfNum,
                                             L7_uint32 *protocol);

/*********************************************************************
* @purpose     Checks the entry corresponding to intfNum is in 
*              route table or not.
*
* @param    intfNum             @b{(input)} interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteIntfEntryGet(L7_uint32 intfNum);

/******************************************************************
* @purpose     Gets the next interface entry in the routing table.
*
* @param    intfNum            @b{(inout)} interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mcastMapIpMRouteIntfEntryNextGet(L7_uint32 *intfNum);

/***************************************************************
*
* @purpose  Adds/Updates an entry in the Static MRoute Table
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(input)} Source IP Address
* @param    srcMask    @b{(input)} Source IP Address  Mask
* @param    rpfAddr    @b{(input)} RPF nexthop IP Address
* @param    intIfNum   @b{(input)} Internal Interface Num
* @param    preference @b{(input)} Route preference for the prefix
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
**************************************************************/
L7_RC_t
mcastMapStaticMRouteSet (L7_uchar8 addrFamily,
                         L7_inet_addr_t *srcAddr,
                         L7_inet_addr_t *srcMask,
                         L7_inet_addr_t *rpfAddr,
                         L7_uint32 intIfNum,
                         L7_uint32 preference);

/***************************************************************
*
* @purpose  Removes an entry from the Static MRoute Table
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(input)} Source IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
**************************************************************/
L7_RC_t
mcastMapStaticMRouteReset (L7_uchar8 addrFamily,
                           L7_inet_addr_t *srcAddr,
                           L7_inet_addr_t *srcMask);

/************************************************************************
* @purpose  finds the best matching static mroute entry for a given source.
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(inout)} source address
* @param    srcMask    @b{(output)} source mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteMatchingEntryGet (L7_uchar8 addrFamily,
                                      L7_inet_addr_t *srcAddr,
                                      L7_inet_addr_t *srcMask);

/*********************************************************************
* @purpose  Verifies the entry corresponding to given source address is
*           in the static route table.
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(input)} source address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteEntryGet (L7_uchar8 addrFamily,
                              L7_inet_addr_t *srcAddr,
                              L7_inet_addr_t *srcMask);

/*********************************************************************
* @purpose  Returns next entry's source address in the static route
*           table for the given source address.
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{inout)}  source address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteEntryNextGet (L7_uchar8 addrFamily,
                                  L7_inet_addr_t *srcAddr,
                                  L7_inet_addr_t *srcMask);

/*********************************************************************
* @purpose  Gets RPF address of interface corresponding to the given
*           source address in the static route table
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    srcAddr    @b{(input)}  source address
* @param    rpfAddr    @b{(output)} RPF address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteRpfAddrGet (L7_uchar8 addrFamily,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_inet_addr_t *rpfAddr);

/***************************************************************
*
* @purpose  Gets the metric value corresponding to given source
*           address in the static route table
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    ipSrc      @b{(input)}  Source IP Address
* @param    preference @b{(output)} Preference for this route
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*
***************************************************************/
L7_RC_t
mcastMapStaticMRoutePreferenceGet (L7_uchar8 addrFamily,
                                   L7_inet_addr_t *srcAddr,
                                   L7_inet_addr_t *srcMask,
                                   L7_uint32 *preference);

/***************************************************************
*
* @purpose  Gets the RPF interface id corresponding to given source
*           address in the static route table
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    ipSrc      @b{(input)}  Source IP Address
* @param    intIfNum   @b{(output)} Internal Interface Number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*
***************************************************************/
L7_RC_t
mcastMapStaticMRouteRpfInterfaceGet (L7_uchar8 addrFamily,
                                     L7_inet_addr_t *srcAddr,
                                     L7_inet_addr_t *srcMask,
                                     L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Gets the total number of entries in the Static MRoute
*           table.
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    numEntries @b{(output)} Total Entry Count
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteEntryCountGet (L7_uchar8 addrFamily,
                                   L7_uint32 *numEntries);

/*********************************************************************
* @purpose  Gets the indication of whether the outgoing interface and
*           next hop represented by the given entry is currently being
*           used to forward IP datagrams.
*
* @param    ipMRtNextHopGrp       @b{(input)} next hop group
* @param    ipMRtNextHoptSrc      @b{(input)} next hop source
* @param    ipMRtNextHopSrcMask   @b{(input)} next hop source mask
* @param    ipMRtNextHopIfIndex    @b{(input)} next hop index
* @param    ipMRtNextHopAddr      @b{(input)} next hop address
* @param    state                 @b{(output)} next hop state
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments    If state is forwarding indicates it is currently being
*           used and if the value 'pruned' indicates it is not
*
* @end
**********************************************************************/
L7_RC_t mcastMapNextHopMRouteStateGet(L7_inet_addr_t *ipMRtNextHopGrp,
                                L7_inet_addr_t *ipMRtNextHopSrc,
                                L7_inet_addr_t *ipMRtNextHopSrcMask,
                                L7_uint32 ipMRtNextHopIfIndex,
                                L7_inet_addr_t *ipMRtNextHopAddr,
                                L7_uint32 *state);

/**********************************************************************
* @purpose Register a callback function to be called when and 
*          MCAST interface event occurs.
*
* @param   routerProtocol_ID    @b{(input)} Protocol ID  
* @param   notify              @b{(input)} function pointer. 
* @param   event                @b{(input)} event
* @param   pdata               @b{(input)} data
* @param   pEventInfo          @b{(input)} event info
*
* @returns   L7_SUCCESS   
* @returns   L7_FAILURE   
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t mcastMapRegisterAdminModeEventChange(L7_MRP_TYPE_t protocolId, 
                                        L7_RC_t (*notify)( 
                                        L7_uint32 event, 
                                        void *pdata,
                                        ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo));

/**************************************************************************
* @purpose Unregister a callback function to be called when and MCAST
*          interface event occurs 
* 
* @param   routerProtocol_ID    @b{(input)} Protocol ID  
*
* @returns   L7_SUCCESS   
* @returns   L7_FAILURE   
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t mcastMapDeregisterAdminModeEventChange(L7_MRP_TYPE_t protocolId);

/*********************************************************************
* @purpose  Free mcast Buffer Pool allocated.
*
* @param    family      @b{(input)} family type
* @param    buffer      @b{(input)} Buffer pointer.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastCtrlPktBufferPoolFree(L7_uchar8 family,L7_uchar8 *buffer);

/*********************************************************************
* @purpose  Get the mcast V6 control pkt Buffer Pool allocated.
*
* @param    none 
*
* @returns  Buffer Pool Id 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 mcastCtrlPktBufferPoolIdGet(L7_uchar8 family);

/*********************************************************************
* @purpose  Free mcast data pkt Buffer Pool allocated.
*
* @param    buffer      @b{(input)} Buffer pointer.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastV6DataBufferPoolFree(L7_uchar8 *buffer);
/*********************************************************************
* @purpose  Get the mcast data pkt Buffer Pool allocated.
*
* @param    none
*
* @returns  Buffer Pool Id 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 mcastV6DataBufferPoolIdGet();
/*********************************************************************
* @purpose  Register a routine to be called when admin scope changes.
*
* @param    registrar_ID        @b{(input)} routine registrar id
* @param    notify_adminscope_change_fnptr @b{(input)} ptr to a routine
*                          to be invoked for admin scope
*                          changes.  Each routine has the following parameters:
*                          (event(MCAST_ADMINSCOPE_ADD, MCAST_ADMINSCOPE_DELETE
*                           )).
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapRegisterAdminScopeEventChange(L7_MRP_TYPE_t protocolId,
                                   L7_RC_t (*notify_adminscope_change_fnptr)
                                   (L7_uint32 event_type,
                                    L7_uint32 msg_len,
                                    void *adminInfo));
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
L7_RC_t mcastMapMRPAdminScopeInfoGet(L7_uchar8 familyType, L7_uint32 rtrIfNum);

/*********************************************************************
* @purpose  De-Register a routine to be called when admin scope changes
*           occur.
*
* @param    registrar_ID   @b{(input)} routine registrar id
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDeregisterAdminScopeEventChange(L7_MRP_TYPE_t protocolId);

/**********************************************************************
* @purpose    Adds an entry to the Admin Scope Boundary List for the
*             given interface
*
* @param      intIfNum     @b{(input)}  Internal Interface Number
* @param      grpAddr      @b{(input)}  Group IP Address
* @param      grpMask      @b{(input)}  Group IP Address Mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
* @returns   L7_ERROR
* @returns   L7_ALREADY_CONFIGURED
* @returns   L7_TABLE_IS_FULL
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapAdminScopeBoundarySet(L7_uint32 intIfNum,
                                  L7_inet_addr_t *grpAddr,
                                  L7_inet_addr_t *grpMask);

/**********************************************************************
* @purpose  Deletes an entry from the Admin Scope Boundary List for
*           the given interface
*
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    grpAddr        @b{(input)}  Group IP Address
* @param    grpMask         @b{(input)}  Group IP Address Mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryReset(L7_uint32 intIfNum,
                                     L7_inet_addr_t *grpAddr,
                                     L7_inet_addr_t *grpMask);

/*********************************************************************
* @purpose  Checks whether the admin scope entry exists for the
*           given interface
*
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    grpAddr         @b{(input)}  Group IP Address
* @param    grpMask         @b{(input)}  Group IP Address Mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments    
*
* @end
**********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryEntryGet(L7_uint32 intIfNum,
                                       L7_inet_addr_t *grpAddr,
                                       L7_inet_addr_t *grpMask);
/*********************************************************************
* @purpose  Gets the next entry in the Admin Scope Boundary list
*           for the given interface
*
* @param    intIfNum        @b{(inout)}  Internal Interface Number
* @param    grpAddr         @b{(inout)}  Group IP Address
* @param    grpMask         @b{(inout)}  Group IP Address Mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments    
*
* @end
**********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryEntryNextGet(L7_uint32 *intIfNum,
                                           L7_inet_addr_t *grpAddr,
                                           L7_inet_addr_t *grpMask);

/*********************************************************************
 * @purpose  Gets whether the specified group address is administratively
 *           scoped boundary on a particular router interface
 *
 * @param    intIfNum        @b{(input)}     Router Interface Number
 * @param    ipGroup         @b{(input)}     Group IP Address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 **********************************************************************/
L7_BOOL mcastMapProtocolIsAdminScopedAddress(L7_uchar8 family, 
                                             L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *pGrpAddr);

/******************************************************************************
* @purpose    To check if the mcast address is admin scoped
*
* @param    pGrpAddr    @b{(input)} group address
* @param    pOif        @b{(input)} interface mask with scoped interfaces 
*                                   for the given group.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments 	 
*		
* @end
******************************************************************************/
L7_RC_t mcastMapGroupScopedInterfacesGet(L7_inet_addr_t *pGrpAddr,
                                         L7_uchar8  *pOlifList);

/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mcastMapProtocolMsgSend(L7_uint32  eventType, L7_VOIDPTR pMsg,
                                 L7_uint32 msgLen);


/*********************************************************************
* @purpose  Determine if the interface type is valid in MCAST
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
L7_BOOL mcastIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in MCAST
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
L7_BOOL mcastIntfIsValid(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Addition of a Packet FD to the Packet Receiver 
*
* @param    sockFd      @b{(input)} Socket FD on which the application wants to 
*                                   poll for control packets   
* @param    eventType   @b{(input)} Event Type the application is interested in                 
* @param    buffPoolID  @b{(input)} Buffer pool to be used for pkt receive
* @param    msgQID      @b{(input)} Msg Q ID to send the message to
* @param    pktRcvr     @b{(input)} Data structure for the packer receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to add socket Fds in the packet receiver's structure.
            The fdlist is a sorted one.FD with highest value is the 1st element.  
*
* @end
*
*********************************************************************/
L7_RC_t mcastMapPktRcvrSocketFdRegister (L7_int32 sockFd, L7_uint32 evTypeId,
                                         L7_uint32 bufPoolId,
                                         L7_IPV6_PKT_RCVR_ID_TYPE_t id);

/*********************************************************************
*
* @purpose  Deletion of a Packet FD from the Packet Receiver
*
* @param    sockFd  @b{(input)}Socket FD to be removed
* @param    pktRcvr @b{(input)}Data structure for the packet Receiver
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to remove a socket Fd from the packet 
*           receiver structure.
*
* @end
*
*********************************************************************/
L7_RC_t mcastMapPktRcvrSocketFdDeRegister (L7_IPV6_PKT_RCVR_ID_TYPE_t id);

/*********************************************************************
* @purpose  Obtain APP Timer Buffer Pool ID       
* 
* @param    bufPoolId   @b{(inout)} Buffer Pool Id
*
* @returns  App Timer Buffer Pool Id
*
* @comments This buffer is for internal use only by the APP Timer.
*
* @end
*********************************************************************/
L7_RC_t mcastMapGetAppTimerBufPoolId (L7_uint32 *bufPoolId);

/*********************************************************************
* @purpose  Obtain MCAST Handle List Memory Handle
* 
* @param    family @b{(input)} IP Address family
* @param    mcastHandleListHandle @b{(output)} Handle to the Handle List
*
* @returns  L7_SUCCESS, on success
* @returns  L7_FAILURE, on failure
*
* @comments This buffer is for internal use only by the Handle List of
*           the MCAST Component.
*
* @end
*********************************************************************/
L7_RC_t mcastMapGetHandleListHandle (L7_uint32 family, void** mcastHandleListMemHandle);

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
L7_uint32 mcastMapHeapIdGet (L7_uint32 family);

/*********************************************************************
* @purpose  Checks if any contradictory configuration made for 
*           unnumbered interface.
*
* @param    intIfNum      @b{(input)} Internal interface number
*
* @returns   L7_TRUE
* @returns   L7_FALSE
*
* @comments  An interface should not be configured as unnumbered for 
*            an DVMRP/IGMP/PIMDM configured interface.
*
* @end
*********************************************************************/
L7_BOOL mcastIntfIsConfigured(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MCAST MGMD Events Buffer Pool ID
*
* @param    none 
*
* @returns  Buffer Pool Id 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32
mcastMgmdEventsBufferPoolIdGet(L7_uchar8 family);

/*********************************************************************
* @purpose  Free MCAST MGMD Events Buffer Pool Buffer.
*
* @param    family         @b{(input)} family type
* @param    mgmdGroupInfo  @b{(input)} Buffer pointer.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
mcastMgmdEventsBufferPoolFree (L7_uchar8 family,
                               mgmdMrpEventInfo_t *mgmdGroupInfo);

/*********************************************************************
*
* @purpose  Register a routine to be called when changes occur within
*           the Multicast Static MRoute module that necessitates
*           communication with other modules for the given protocol.
*
* @param    addrFamily    @b{(input)} Address Family Identifier
* @param    protocolId    @b{(input)} Protocol ID (A identified constant in L7_MRP_TYPE_t )
* @param    *notifyFn     @b{(input)} pointer to a routine to be invoked
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The registration from MRPs is reentrant, yet the notify list is not 
*            protected as function pointer location is fixed for each 
*
* @end
*
*********************************************************************/
L7_RC_t
mcastMapStaticMRouteEventRegister (L7_uchar8 addrFamily,
                                   L7_MRP_TYPE_t protocolId,
                                   mcastMapStaticMRouteNotifyFn_t notifyFn);

/*********************************************************************
*
* @purpose  De-Register a routine to be called when changes occur within
*           the Multicast Static MRoute module that necessitates
*           communication with other modules for the given protocol.
*
* @param    addrFamily    @b{(input)} Address Family Identifier
* @param    protocolId    @b{(input)} Protocol ID (A identified constant in L7_MRP_TYPE_t )
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  The registration from MRPs is reentrant, yet the notify list is not 
*            protected as function pointer location is fixed for each 
*
* @end
*
*********************************************************************/
L7_RC_t
mcastMapStaticMRouteEventDeRegister (L7_uchar8 addrFamily,
                                     L7_MRP_TYPE_t protocolId);

#endif /* _MCASTMAP_API_H_ */

