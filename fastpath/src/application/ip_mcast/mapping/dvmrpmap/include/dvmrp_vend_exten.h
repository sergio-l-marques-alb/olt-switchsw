/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    dvmrp_vend_exten.h
*
* @purpose     DVMRP MRT vendor-specific API functions
*
* @component   DVMRP Mapping Layer
*
* @comments    Does not contain any references to vendor headers or types.
*
* @create      02/18/2002
*
* @author      M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/

#ifndef _DVMRP_VEND_EXTEN_H_
#define _DVMRP_VEND_EXTEN_H_

#define DVMRPMAP_IP_ADDRESS_LENGTH_VERSION_4   32

/* NOTE: Make sure L7_AUTH_MAX_KEY_DVMRP definition matches this value
#define DVMRP_MAP_EXTEN_AUTH_MAX_KEY   8    vendor code maximum */


/*---------------------------------------------------*/
/* DVMRP Mapping Layer vendor API function prototypes */
/*---------------------------------------------------*/

/*-------------------*/
/* dvmrp_vend_exten.c */
/*-------------------*/

/*********************************************************************
* @purpose  Set the DVMRP administrative mode for the router
*
* @param    mode        @b{(input)} Admin mode (L7_ENABLE or L7_DISABLE)
* @param    dvmrpDoInit @b{(input)} Flag to check whether DVMRP
*                                   Memory can be Initialized/
*                                   De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenDvmrpAdminModeSet(L7_uint32 mode,
                                       L7_BOOL dvmrpDoInit);

/*********************************************************************
* @purpose  Sets the DVMRP Admin mode for the specified interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    mode   @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfAdminModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table
*
* @param    numRoutes    @b{(output)} number of routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNumRoutesGet(L7_uint32 *numRoutes);

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table with
*           non-infinite metric
*
* @param    reachable    @b{(reachable)} reachable routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenReachableRoutesGet(L7_uint32 *reachable);

/*********************************************************************
* @purpose  Set the metric used for default routes in DVMRP updates
*           originated on the specified interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(output)} Default metric value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfMetricSet(L7_uint32 intIfNum, L7_ushort16 metric);

/*********************************************************************
* @purpose  Gets the DVMRP Generation ID used by the router
*
* @param    intIfNum @b{(input)} internal interface number
* @param    genId    @b{(output)} Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfGenerationIDGet(L7_int32 intIfNum, L7_ulong32 *genId);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    status     @b{(output)} status
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfStatusGet(L7_uint32 intIfNum,L7_int32 *status);

/*********************************************************************
* @purpose  Get the number of invalid packets received on this interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    badPkts     @b{(output)} bad packets received
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfRcvBadPktsGet(L7_uint32 rtrIfNum,L7_ulong32 *badPkts);

/*********************************************************************
* @purpose  Get the number of invalid routes received on this interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    badRts      @b{(output)} bad routes received
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfRcvBadRoutesGet(L7_uint32 rtrIfNum,L7_ulong32 *badRts);

/*********************************************************************
* @purpose  Get the number of routes sent on this interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    sent      @b{(output)} sent routes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfSentRoutesGet(L7_uint32 rtrIfNum, L7_ulong32 *sent);

/*********************************************************************
* @purpose  Get if the entry of Interfaces is valid or not.
*
* @param    rtrIfNum    @b{(input)} router interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfGet(L7_uint32 rtrIfNum);

/*********************************************************************
* @purpose  Get an entry of Interfaces.
*
* @param    pIntIfNum   @b{(output)}  DVMRP Interface table
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfEntryGet(L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    pIntIfNum   @b{(inout)}  internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfEntryNextGet(L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the DVMRP Uptime for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrUpTime    @b{(output)} Neighbor Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborUpTimeGet(L7_uint32 rtrIfNum,
                                       L7_inet_addr_t *nbrIpAddr,
                                       L7_ulong32 *nbrUpTime);

/*********************************************************************
* @purpose  Get the DVMRP Expiry time for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrExpTime   @b{(output)} Neighbor Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborExpiryTimeGet(L7_uint32 rtrIfNum,
                                           L7_inet_addr_t *nbrIpAddr,
                                           L7_ulong32 *nbrExpTime);

/*********************************************************************
* @purpose  Get the DVMRP Generation ID for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    nbrGenId     @b{(output)} Neighbor Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborGenIdGet(L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *nbrIpAddr,
                                      L7_ulong32 *nbrGenId);

/*********************************************************************
* @purpose  Get the DVMRP Major Version for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    majorVersion @b{(output)} Neighbor Major Version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborMajorVersionGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ushort16 *majorVersion);

/*********************************************************************
* @purpose  Get the DVMRP Minor Version for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)}  IP Address of the neighbour
* @param    minorVersion @b{(output)} Neighbor Minor Version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborMinorVersionGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ushort16 *minorVersion);

/*********************************************************************
* @purpose  Get the DVMRP capabilities for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    capabilities @b{(output)} Neighbor Capabilities
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborCapabilitiesGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ushort16 *capabilities);

/*********************************************************************
* @purpose  Get the number of routes received for the specified neighbour
*           of the specified interface.
*
* @param    rtrIfNum    @b{(input)} router interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    rcvRoutes   @b{(output)} Received Routes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborRcvRoutesGet(L7_uint32 rtrIfNum,
                                          L7_inet_addr_t *nbrIpAddr,
                                          L7_ulong32 *rcvRoutes);

/*********************************************************************
* @purpose  Get the number of invalid packets received for the specified neighbour
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badPkts     @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborRcvBadPktsGet(L7_uint32 intIfNum,
                                           L7_inet_addr_t *nbrIpAddr,
                                           L7_ulong32 *badPkts);

/*********************************************************************
* @purpose  Get the number of invalid routes received for the specified neighbour
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    badRts      @b{(output)} Bad Routes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborRcvBadRoutesGet(L7_uint32 rtrIfNum,
                                             L7_inet_addr_t *nbrIpAddr,
                                             L7_ulong32 *badRts);

/*********************************************************************
* @purpose  Get the state for the specified neighbour
*           of the specified interface.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    state       @b{(output)} state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborStateGet(L7_uint32 rtrIfNum,
                                      L7_inet_addr_t *nbrIpAddr,
                                      L7_ulong32 *state);

/*********************************************************************
* @purpose  Get if the entry of Neighbors is valid or not.
*
* @param    nbrIfIndex  @b{(input)}  neighbor interface index
* @param    nbrAddr     @b{(output)} neighbor address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborGet(L7_uint32 nbrIfIndex, 
                                 L7_inet_addr_t *nbrAddr);

/*********************************************************************
* @purpose  Get an entry of Neighbors.
*
* @param    pRtrIfNum    @b{(input)} Router interface number
* @param    nbrIpAddr    @b{(output)} neigbor address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborEntryGet(L7_uint32 *pRtrIfNum, 
                                      L7_inet_addr_t *nbrIpAddr);

/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    pRtrIfNum    @b{(input)} Router interface number
* @param    nbrIpAddr    @b{(output)} neighbor address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenNeighborEntryNextGet(L7_uint32 *pRtrIfNum, 
                                          L7_inet_addr_t *nbrIpAddr);

/*********************************************************************
* @purpose  Gets the IP address of neighbor which is the source for
*           the packets for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    addr         @b{(output)} neighbor ip address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteUpstreamNeighborGet(L7_inet_addr_t *srcIpAddr,
                                              L7_inet_addr_t *srcMask,
                                              L7_inet_addr_t *addr);

/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source ip address
* @param    srcMask      @b{(input)} source Mask
* @param    rtrIfIndex      @b{(output)} router Interface Index which is passed
*                                     back to the calling function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteIfIndexGet(L7_inet_addr_t *srcIpAddr,
                                     L7_inet_addr_t *srcMask,
                                     L7_uint32 *rtrIfIndex);

/*********************************************************************
* @purpose  Gets the distance in hops for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    metric       @b{(output)} metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteMetricGet(L7_inet_addr_t *srcIpAddr,
                                    L7_inet_addr_t *srcMask, L7_ushort16 *metric);

/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtExpTime    @b{(output)} route expiry time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteExpiryTimeGet(L7_inet_addr_t *srcIpAddr,
                                        L7_inet_addr_t *srcMask,
                                        L7_ulong32 *rtExpTime);

/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtUpTime     @b{(output)} route up time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteUptimeGet(L7_inet_addr_t *srcIpAddr,
                                    L7_inet_addr_t *srcMask,
                                    L7_ulong32 *rtUpTime);

/*********************************************************************
* @purpose  Get if the entry of Routes is valid or not.
*
* @param    routeSrc     @b{(input)} route source
* @param    routeSrcMask @b{(input)} route source mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteGet(L7_inet_addr_t *routeSrc, 
                              L7_inet_addr_t *routeSrcMask);

/*********************************************************************
* @purpose  Get an entry of Routes.
*
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteEntryGet(L7_inet_addr_t *srcIpAddr, 
                                   L7_inet_addr_t *srcMask);

/*********************************************************************
* @purpose  Get the next entry of Routes.
*
* @param    srcIpAddr    @b{(inout)} source IP Address
* @param    srcMask      @b{(inout)} source Mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteEntryNextGet(L7_inet_addr_t *srcIpAddr, 
                                       L7_inet_addr_t *srcMask);

/*********************************************************************
* @purpose  Gets the type of next hop router - leaf or branch,
*           for a specified source address and specified interface.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopRtrIfIndex    @b{(input)} Next Hop Interface Index
* @param    nextHopType       @b{(output)} Next Hop Type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopTypeGet(L7_inet_addr_t *nextHopSrc,
                                         L7_inet_addr_t *nextHopSrcMask,
                                         L7_uint32 nextHopRtrIfIndex,
                                         L7_uint32 *nextHopType);

/*********************************************************************
* @purpose  Get if the entry of next hops is valid or not.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(input)} Next Hop Interface Index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopGet(L7_inet_addr_t *nextHopSrc,
                                     L7_inet_addr_t *nextHopSrcMask,
                                     L7_uint32 nextHopIfIndex);

/*********************************************************************
* @purpose  Get an entry of next hops.
*
* @param    nextHopSrc        @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex    @b{(output)} Next Hop Interface Index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopEntryGet(L7_inet_addr_t *nextHopSrc,
                                          L7_inet_addr_t *nextHopSrcMask,
                                          L7_uint32 *nextHopIfIndex);

/*********************************************************************
* @purpose  Get the next entry of nexthops.
*
* @param    nextHopSrc        @b{(inout)} Next Hop source IP Address
* @param    nextHopSrcMask    @b{(inout)} Next Hop source Mask
* @param    nextHopRtrIfIndex    @b{(output)} Next Hop Interface Index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenRouteNextHopEntryNextGet(L7_inet_addr_t *nextHopSrc,
                                              L7_inet_addr_t *nextHopSrcMask,
                                              L7_uint32 *nextHopRtrIfIndex);

/*********************************************************************
* @purpose  Gets the prune expiry time for a group and
*           specified source address.
*
* @param    pruneGrp        @b{(input)} prune group
* @param    pruneSrc        @b{(input)} prune source
* @param    pruneSrcMask    @b{(input)} prune source mask
* @param    pruneExpTime    @b{(output)} prune expiry time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenPruneExpiryTimeGet(L7_inet_addr_t *pruneGrp,
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
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenPruneEntryGet(L7_inet_addr_t *pruneGrp,
                                   L7_inet_addr_t *pruneSrc,
                                   L7_inet_addr_t *pruneSrcMask);

/*********************************************************************
* @purpose  Get the next entry of prunes.
*
* @param    pruneGrp        @b{(inout)} prune group
* @param    pruneSrc        @b{(inout)} prune source
* @param    pruneSrcMask    @b{(inout)} prune source mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenPruneEntryNextGet(L7_inet_addr_t *pruneGrp,
                                       L7_inet_addr_t *pruneSrc,
                                       L7_inet_addr_t *pruneSrcMask);

/*********************************************************************
* @purpose  Get the next entry of nbr on intIfNum.
*
* @param    pIntIfNum     @b{(input)} internal interface number
* @param    nbrIpAddr     @b{(output)} Nbr IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIntfNbrEntryNextGet(L7_uint32 *pIntIfNum, 
                                         L7_inet_addr_t *nbrIpAddr);

/*********************************************************************
* @purpose  Chech whether router interface running DVMRP 'intIfNum' is a leaf
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes    needs only intIfNum.
*
* @end
*********************************************************************/
L7_uint32 dvmrpMapExtenDvmrpIsInterfaceLeaf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Chech whether the router running DVMRP is a leaf router
*
* @param    void
*
* @returns  L7_TRUE  if leaf router
* @returns  L7_FALSE, if not a leaf router
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dvmrpMapExtenDvmrpRouterIsLeaf(void);

/*********************************************************************
* @purpose  Get the Ip Mroute Flags.
*
* @param    ipMRouteGroup     @b{(input)} Group Address.
* @param    ipMRouteSource    @b{(input)} Source Address.
* @param    ipMRouteMask      @b{(input)} Mask Address.
* @param    flags             @b{(output)} Flags
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dvmrpMapExtenIpMRouteFlagsGet(L7_inet_addr_t *ipMRouteGroup, 
                                      L7_inet_addr_t *ipMRouteSource, 
                                      L7_inet_addr_t  *ipMRouteSourceMask, 
                                      L7_uint32*  flags);
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rpfAddr               @b{(output)} rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRtAddrGet(L7_inet_addr_t *ipMRouteGroup,
                                       L7_inet_addr_t *ipMRouteSource,
                                       L7_inet_addr_t  *ipMRouteSourceMask,
                                       L7_inet_addr_t*  rpfAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rtMask                @b{(output)} rtMask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRtMaskGet(L7_inet_addr_t *ipMRouteGroup,
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
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRtTypeGet(L7_inet_addr_t *ipMRouteGroup,
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
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteExpiryTimeGet(L7_inet_addr_t *ipMRouteGroup,
                                       L7_inet_addr_t *ipMRouteSource,
                                       L7_inet_addr_t  *ipMRouteSourceMask,
                                       L7_uint32 *expire);

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Addr.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
* @param    rpfAddr               @b{(output)} rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteRpfAddrGet(L7_inet_addr_t *ipMRouteGroup,
                                        L7_inet_addr_t *ipMRouteSource,
                                        L7_inet_addr_t  *ipMRouteSourceMask,
                                        L7_inet_addr_t *rpfAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count.
*
* @param    entryCount      @b{(output)}  rpfAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteEntryCountGet(L7_uint32 *entryCount);

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count.
*
* @param    heCount     @b{(output)}  highest Entry Count
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteHighestEntryCountGet(L7_uint32 *heCount);

/*********************************************************************
* @purpose  Get the Ip Mroute Entry.
*
* @param    ipMRouteGroup         @b{(input)} Group Address.
* @param    ipMRouteSource        @b{(input)} Source Address.
* @param    ipMRouteSourceMask    @b{(input)} Source Mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteEntryGet(L7_inet_addr_t *ipMRouteGroup,
                                      L7_inet_addr_t *ipMRouteSource,
                                      L7_inet_addr_t  *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the Ip Mroute Entry.
*
* @param    ipMRouteGroup         @b{(inout)} Group Address.
* @param    ipMRouteSource        @b{(inout)} Source Address.
* @param    ipMRouteSourceMask    @b{(inout)} Source Mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpMRouteEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                          L7_inet_addr_t *ipMRouteSource,
                                          L7_inet_addr_t  *ipMRouteSourceMask);
/*********************************************************************
* @purpose  Get the next mroute entry out interface
*
* @param    ipMRouteGroup         @b{(inout)} Group Address.
* @param    ipMRouteSource        @b{(inout)} Source Address.
* @param    ipMRouteSourceMask    @b{(inout)} Source Mask.
* @param    outIfNum              @b{(inout)} interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapExtenIpRouteOutIntfEntryNextGet(L7_inet_addr_t *ipMRouteGroup,
                                                L7_inet_addr_t *ipMRouteSource,
                                                L7_inet_addr_t  *ipMRouteSourceMask,
                                                L7_uint32 *outIfNum);


#endif /* _DVMRP_VEND_EXTEN_H_ */
