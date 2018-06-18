/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_dvmrp_api.h
*
* @purpose    DVMRP Mapping Layer APIs
*
* @component  DVMRP Mapping Layer
*
* @comments   none
*
* @create     02/06/2002
*
* @author     M Pavan K Chakravarthi
* @end
*
**********************************************************************/
/********************************************************************
 *                   
 ********************************************************************/

#ifndef _L7_DVMRP_API_H_
#define _L7_DVMRP_API_H_

#include "l3_mcast_commdefs.h"

/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */

/*********************************************************************
* @purpose  Set the DVMRP administrative mode
*
* @param    mode        @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the DVMRP administrative mode
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapDvmrpAdminModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set the administrative mode of a DVMRP routing interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfAdminModeSet(L7_uint32 intIfNum, 
                                 L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of a DVMRP routing interface
*
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
L7_RC_t dvmrpMapIntfAdminModeGet(L7_uint32 intIfNum, 
                                 L7_uint32 *mode);
/*********************************************************************
* @purpose  Gets the DVMRP Vendor Version 
*
* @param    versionString    @b{(input)} version string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapVersionStringGet(L7_uchar8 *versionString);

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table
*
* @param    numRoutes   @b{(output)} number of routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNumRoutesGet(L7_uint32 *numRoutes);

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table with
*           non-infinite metric
*
* @param    reachable    @b{(output)} reachable routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapReachableRoutesGet(L7_uint32 *reachable);

/*********************************************************************
* @purpose  Gets the IP address of the DVMRP interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddr      @b{(output)} Ip Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfLocalAddressGet(L7_uint32 intIfNum, 
                                    L7_inet_addr_t *ipAddr);

/*********************************************************************
* @purpose  Gets the DVMRP Generation ID used by the router
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    genId       @b{(output)}  Generation Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfGenerationIDGet(L7_uint32 intIfNum, 
                                    L7_ulong32 *genId);

/*********************************************************************
* @purpose  Gets the DVMRP Interface Metric for the specified interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    metric      @b{(output)}  Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfMetricGet(L7_uint32 intIfNum, 
                              L7_ushort16 *metric);

/*********************************************************************
* @purpose  Sets the DVMRP Interface Metric for the specified interface
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
* @param    metric      @b{(input)}  Metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfMetricSet(L7_uint32 intIfNum, 
                              L7_ushort16 metric);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    status      @b{(output)}  status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfStatusGet(L7_uint32 intIfNum, 
                              L7_int32 *status);

/*********************************************************************
* @purpose  Determine if the DVMRP component has been initialized for
*           the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapIntfIsOperational(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the number of invalid packets received on this interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    badPkts     @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfRcvBadPktsGet(L7_uint32 intIfNum, 
                                  L7_ulong32 *badPkts);

/*********************************************************************
* @purpose  Get the number of invalid routes received on this interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    badRts      @b{(output)} Bad Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfRcvBadRoutesGet(L7_uint32 intIfNum,
                                    L7_ulong32 *badRts);

/*********************************************************************
* @purpose  Get the number of routes sent on this interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    sent        @b{(output)} Sent Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfSentRoutesGet(L7_uint32 intIfNum, 
                                  L7_ulong32 *sent);

/*********************************************************************
* @purpose  Get an entry of Interfaces.
*
* @param    pIntIfNum         @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfEntryGet(L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the next entry of interfaces.
*
* @param    pIntIfNum    @b{(inout)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIntfEntryNextGet(L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the DVMRP Uptime for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrUpTime    @b{(output)} Neighbor Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborUpTimeGet(L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
                                  L7_ulong32 *nbrUpTime);
/*********************************************************************
* @purpose  Get the DVMRP Expiry time for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrExpTime   @b{(output)} Neighbor Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborExpiryTimeGet(L7_uint32 intIfNum, 
     L7_inet_addr_t *nbrIpAddr, L7_ulong32 *nbrExpTime);
/*********************************************************************
* @purpose  Get the DVMRP Generation ID for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrGenId     @b{(output)} Neighbor Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborGenIdGet(L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
                                 L7_ulong32 *nbrGenId);
/*********************************************************************
* @purpose  Get the DVMRP Major Version for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    majorVersion @b{(output)} Neighbor Major Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborMajorVersionGet(L7_uint32 intIfNum,
     L7_inet_addr_t *nbrIpAddr, L7_ushort16 *majorVersion);

/*********************************************************************
* @purpose  Get the DVMRP Minor Version for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    minorVersion @b{(output)} Neighbor Minor Version
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborMinorVersionGet(L7_uint32 intIfNum, 
       L7_inet_addr_t *nbrIpAddr, L7_ushort16 *minorVersion);
/*********************************************************************
* @purpose  Get the DVMRP capabilities for the specified neighbour
*           of the specified interface
*
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    capabilities @b{(output)} Neighbor Capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborCapabilitiesGet(L7_uint32 intIfNum,  
      L7_inet_addr_t *nbrIpAddr, L7_ushort16 *capabilities);
/*********************************************************************
* @purpose  Get the number of routes received for the specified neighbour
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    rcvRoutes   @b{(output)} Received Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborRcvRoutesGet(L7_uint32 intIfNum, 
     L7_inet_addr_t *nbrIpAddr, L7_ulong32 *rcvRoutes);
/*********************************************************************
* @purpose  Get the number of invalid packets received for the specified 
*           neighbour of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badPkts     @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborRcvBadPktsGet(L7_uint32 intIfNum, 
    L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badPkts);
/*********************************************************************
* @purpose  Get the number of invalid routes received for the 
*           specified neighbour of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badRts      @b{(output)} Bad Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapNeighborRcvBadRoutesGet(L7_uint32 intIfNum, 
             L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badRts);
/*********************************************************************
* @purpose  Get the state for the specified neighbour
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    state       @b{(output)} state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborStateGet(L7_uint32 intIfNum, 
              L7_inet_addr_t *nbrIpAddr, L7_ulong32 *state);
/*********************************************************************
* @purpose  Get if the entry of Neighbors is valid or not.
*
* @param    nbrIfIndex  @b{(input)} Neighbor Interface Index.
* @param    nbrAddr     @b{(output)} IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborGet(L7_uint32 nbrIfIndex, 
                            L7_inet_addr_t *nbrAddr);
/*********************************************************************
* @purpose  Get an entry of Neighbors.
*
* @param    pIntIfNum   @b{(output)} Internal Interface Number
* @param    nbrIpAddr   @b{(output)} IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborEntryGet(L7_uint32 *pIntIfNum, 
                                 L7_inet_addr_t *nbrIpAddr);
/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    pIntIfNum    @b{(output)} Internal Interface Number
* @param    nbrIpAddr    @b{(output)} IP Address of the neighbour
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapNeighborEntryNextGet(L7_uint32 *pIntIfNum, 
                                 L7_inet_addr_t *nbrIpAddr);
/*********************************************************************
* @purpose  Gets the IP address of neighbor which is the source for
*           the packets for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    addr         @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteUpstreamNeighborGet(L7_inet_addr_t *srcIpAddr, 
           L7_inet_addr_t *srcMask, L7_inet_addr_t *addr);
/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    IfIndex      @b{(output)} Interface Index which is passed back 
*                                    to the calling function
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapRouteIfIndexGet(L7_inet_addr_t *srcIpAddr, 
                                 L7_inet_addr_t *srcMask, L7_uint32 *IfIndex);

/*********************************************************************
* @purpose  Gets the distance in hops for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    metric       @b{(output)} metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteMetricGet(L7_inet_addr_t *srcIpAddr, 
          L7_inet_addr_t *srcMask, L7_ushort16 *metric);
/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtExpTime    @b{(output)} route expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteExpiryTimeGet(L7_inet_addr_t *srcIpAddr, 
            L7_inet_addr_t *srcMask, L7_ulong32 *rtExpTime);
/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtUpTime     @b{(output)} route up time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteUptimeGet(L7_inet_addr_t *srcIpAddr, 
        L7_inet_addr_t *srcMask, L7_ulong32 *rtUptime);
/*********************************************************************
* @purpose  Get if the entry of Routes is valid or not.
*
* @param    routeSrc          @b{(input)} source IP Address
* @param    routeSrcMask      @b{(input)} source Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteGet(L7_inet_addr_t *routeSrc, 
                         L7_inet_addr_t *routeSrcMask);
/*********************************************************************
* @purpose  Get an entry of Routes.
*
* @param    srcIpAddr     @b{(input)} source IP Address
* @param    srcMask       @b{(input)} source Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteEntryGet(L7_inet_addr_t *srcIpAddr, 
                              L7_inet_addr_t *srcMask);
/*********************************************************************
* @purpose  Get the next entry of Routes.
*
* @param    srcIpAddr   @b{(inout)}  source IP Address
* @param    srcMask     @b{(inout)}  source Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteEntryNextGet(L7_inet_addr_t *srcIpAddr, 
                                  L7_inet_addr_t *srcMask);
/*********************************************************************
* @purpose  Gets the type of next hop router - leaf or branch,
*           for a specified source address and specified interface.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(input)} Next Hop Interface Index
* @param    nextHopType       @b{(output)} Next Hop Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteNextHopTypeGet(L7_inet_addr_t *nextHopSrc,
                                    L7_inet_addr_t *nextHopSrcMask,
                                    L7_uint32 nextHopIfIndex,
                                    L7_uint32 *nextHopType);

/*********************************************************************
* @purpose  Get if the entry of next hops is valid or not.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(input)} Next Hop Interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapRouteNextHopGet(L7_inet_addr_t *nextHopSrc,
                                 L7_inet_addr_t *nextHopSrcMask,
                                 L7_uint32 nextHopIfIndex);

/*********************************************************************
* @purpose  Get an entry of next hops.
*
* @param    nextHopSrc        @b{(inout)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(inout)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(inout)} Next Hop Interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t  dvmrpMapRouteNextHopEntryGet(L7_inet_addr_t *nextHopSrc,
                                      L7_inet_addr_t *nextHopSrcMask,
                                      L7_uint32 *nextHopIfIndex);

/*********************************************************************
* @purpose  Get the next entry of nexthops.
*
* @param    nextHopSrc        @b{(inout)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(inout)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(inout)} Next Hop Interface Index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteNextHopEntryNextGet(L7_inet_addr_t *nextHopSrc,
                                         L7_inet_addr_t *nextHopSrcMask,
                                         L7_uint32 *nextHopIfIndex);

/*********************************************************************
* @purpose  Gets the prune expiry time for a group and
*           specified source address.
*
* @param    pruneGrp        @b{(input)} prune group
* @param    pruneSrc        @b{(input)} prune source
* @param    pruneSrcMask    @b{(input)} prune source mask
* @param    pruneExpTime    @b{(output)} prune expiry time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapPruneExpiryTimeGet(L7_inet_addr_t *pruneGrp,
                                   L7_inet_addr_t *pruneSrc,
                                   L7_inet_addr_t *pruneSrcMask,
                                   L7_ulong32 *pruneExpTime);

/*********************************************************************
* @purpose  Get an entry of prunes.
*
* @param    pruneGrp        @b{(input)} prune group
* @param    pruneSrc        @b{(input)} prune source
* @param    pruneSrcMask    @b{(input)} prune source mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapPruneEntryGet(L7_inet_addr_t *pruneGrp,
                              L7_inet_addr_t *pruneSrc,
                              L7_inet_addr_t *pruneSrcMask);

/*********************************************************************
* @purpose  Get the next entry of prunes.
*
* @param    pruneGrp        @b{(inout)} prune group
* @param    pruneSrc        @b{(inout)} prune source
* @param    pruneSrcMask    @b{(inout)} prune source mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapPruneEntryNextGet(L7_inet_addr_t *pruneGrp,
                                  L7_inet_addr_t *pruneSrc,
                                  L7_inet_addr_t *pruneSrcMask);

/*********************************************************************
* @purpose  Set DVMRP Mapping Layer tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapTraceModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    retVal  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapTraceModeGet(L7_uint32 *retVal);


/*********************************************************************
* @purpose  Determine if the interface type is valid in DVMRP
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
L7_BOOL dvmrpIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface type is valid in DVMRP
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
L7_BOOL dvmrpIntfIsValid(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose Check whether dvmrp Is Operational.
*
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapDvmrpIsOperational ();

/*********************************************************************
* @purpose  Get the Ip Mroute Flags.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    flags                 @b{(output)} Flags
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteFlagsGet(L7_inet_addr_t *ipMRouteGroup,
                                 L7_inet_addr_t *ipMRouteSource,
                                 L7_inet_addr_t  *ipMRouteSourceMask,
                                 L7_uint32 *  flags);
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtAddr                @b{(output)} rtAddr 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRtAddressGet(L7_inet_addr_t *ipMRouteGroup,
                                     L7_inet_addr_t *ipMRouteSource,
                                     L7_inet_addr_t  *ipMRouteSourceMask,
                                     L7_inet_addr_t*  rtAddr);
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtMask                @b{(output)} rtMask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRtMaskGet(L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSource,
                                  L7_inet_addr_t  *ipMRouteSourceMask,
                                  L7_inet_addr_t*  rtMask);
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtType                @b{(output)} rtType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRtTypeGet(L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSource,
                                  L7_inet_addr_t  *ipMRouteSourceMask,
                                  L7_uint32 *rtType);
/*********************************************************************
* @purpose  Get the Ip Mroute Expiry Time.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    expire                @b{(output)} expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteExpiryTimeGet(L7_inet_addr_t *ipMRouteGroup,
                                  L7_inet_addr_t *ipMRouteSource,
                                  L7_inet_addr_t  *ipMRouteSourceMask,
                                  L7_uint32 *expire);

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Address.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rpfAddr               @b{(output)} rpfAddr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteRpfAddrGet(L7_inet_addr_t *ipMRouteGroup,
                                   L7_inet_addr_t *ipMRouteSource,
                                   L7_inet_addr_t  *ipMRouteSourceMask,
                                   L7_inet_addr_t *rpfAddr);
/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count Get.
*
* @param    entryCount     @b{(output)} entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteEntryCountGet(L7_uint32 *entryCount);

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count Get.
*
* @param    heCount     @b{(output)} entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteHighestEntryCountGet(L7_uint32 *heCount);
/*********************************************************************
* @purpose  Get the Ip Mroute Entry
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteEntryGet(L7_inet_addr_t *ipMRouteGroup,
                                 L7_inet_addr_t *ipMRouteSource,
                                 L7_inet_addr_t  *ipMRouteSourceMask);
/*********************************************************************
* @purpose  Get the Ip Mroute Entry Next
*
* @param    ipMRouteGroup         @b{(inout)} Group Address.
* @param    ipMRouteSource        @b{(inout)} Source Address.
* @param    ipMRouteSourceMask    @b{(inout)} Source Mask.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapIpMRouteEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                     L7_inet_addr_t *ipMRouteSource,
                                     L7_inet_addr_t  *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the next out interface
*
* @param    ipMRouteGroup        @b{(inout)}  Group Address.
* @param    ipMRouteSource       @b{(inout)}  Source Address.
* @param    ipMRouteSourceMask   @b{(inout)}  Source Mask.
* @param    outIntf              @b{(inout)}  out interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapRouteOutIntfEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                         L7_inet_addr_t *ipMRouteSource,
                                         L7_inet_addr_t *ipMRouteSourceMask,
                                         L7_uint32 *outIntf);
/*********************************************************************
* @purpose  Send event to DVMRP Vendor
*
* @param    familyType     @b{(input)} Address Family type
* @param    eventType      @b{(input)} Event Type 
* @param    msgLen         @b{(input)} Message Length
* @param    eventMsg       @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapEventChangeCallback(L7_uchar8 family, L7_uint32  eventType, 
                                  L7_uint32  msgLen, void *eventMsg);
/*********************************************************************
* @purpose  To send the event to Message Queue
*
* @param    familyType   @b{(input)} address family
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
L7_RC_t dvmrpMapMessageQueueSend(L7_uchar8 familyType, L7_uint32 eventType,
                                 L7_uint32 msgLen, L7_VOIDPTR pMsg);



#endif /* _L7_DVMRP_API_H_ */
