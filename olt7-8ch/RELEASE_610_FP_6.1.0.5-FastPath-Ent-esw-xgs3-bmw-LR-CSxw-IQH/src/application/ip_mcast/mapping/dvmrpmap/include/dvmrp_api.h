/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
 **********************************************************************
 *
 * @filename dvmrp_api.h
 *
 * @purpose Contains structures,constants,macros, for dvmrp protocol
 *
 * @component
 *
 * @notes
 *
 * @create 07/03/2006
 *
 * @author Prakash/Shashidhar
 * @end
 *
 **********************************************************************/
#ifndef _DVMRP_API_H_
#define _DVMRP_API_H_

#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "osapi_support.h"
#include "mfc_api.h"
#include "l7_mgmd_api.h"

#define DVMRP_MAJOR_VERSION     3

typedef struct _dvmrp_adminmode_s {
  L7_uint32      rtrIfNum;
  L7_uint32      mode;
  L7_inet_addr_t *ipAddr;
  L7_inet_addr_t *netMask;
} dvmrp_adminmode_t;

typedef struct _dvmrp_metric_s {
  L7_uint32 rtrIfNum;
  L7_uint32 metric;
} dvmrp_metric_t;

/*Timer Block structure*/
typedef struct  dvmtp_timer_block_s
{
  L7_int32     data1;
  void *data2;
  void *data3;
  void *data4;  
}dvmrp_timer_event_t;

/*********************************************************************
*
* @purpose  De-Inits the DVMRP protocol
*
* @param    dvmrpcb    @b{ (input) }Pointer to the DVMRP Control Block
*
* @returns  None
*
* @notes    De-inits all the DVMRP tables.
*
* @end
*********************************************************************/
void dvmrpClearInit(void *dvmrpcb);


/*********************************************************************
 * @purpose  This function initializes the DVMRP task.
 *
 * @param   
 *     
 * @returns  L7_SUCCESS - Protocol has been initialized properly.
 *
 * @returns  L7_ERROR   - Protocol has not been initialized properly.
 *
 * @notes    This function initializes all the control block parameters
 *       
 * @end
 *********************************************************************/
L7_int32 dvmrp_init (void **dvmrpmapcb);

/*********************************************************************
* @purpose  This function process DVMRP Events.
*
* @param    familyType     @b{ (input) } address Family. 
* @param    event          @b{ (input) } Event.
* @param    pMsg           @b{ (input) } Message.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  This function recieves all the messages and events and 
*            call the appropriate handler
* @end
*********************************************************************/
L7_RC_t dvmrpEventProcess(L7_uchar8 familyType, L7_uint32 event, 
                          void *pMsg);

/*********************************************************************
 * @purpose  This function creates IGMP raw socket to be used by DVMRP for
 *           it's message RX/TX.
 *
 * @param    None

 * @return   L7_SUCCESS 
 * @return   L7_FAILURE
 *
 *
 * @notes   None
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpSocketInfoApply(void *dvmrpcb);

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table
*
* @param    numRoutes   @b{(output)}  Number of Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpNumRoutesGet(L7_uint32 *numRoutes);

/*********************************************************************
  * @purpose  Gets the number of routes in the DVMRP routing table with
  *           non-infinite metric
  *
  * @param    reachable    -  @b{(output)}  Number of routes with non-infinite
  *                                         metrics 
  *
  * @returns  L7_SUCCESS
  * @returns  L7_FAILURE
  *
  * @notes    none
  *
  * @end
  *********************************************************************/
extern L7_RC_t dvmrpReachableRoutesGet(L7_uint32 *reachable);

/*********************************************************************
* @purpose  Gets the DVMRP Generation ID used by the router
*
* @param    rtrIfNum       -  @b{(input)}  Router Interface Number
* @param    genId          -  @b{(output)} Generation ID.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpIntfGenerationIDGet(L7_int32 rtrIfNum, L7_ulong32 *genId);

/*********************************************************************
* @purpose  Get the entry status.
*
* @param    rtrIfNum   -  @b{(input)}   Router interface number
* @param    status      -  @b{(output)}  status
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpIntfStatusGet(L7_uint32 rtrIfNum,L7_int32 *status);


/*********************************************************************
* @purpose  Get the number of invalid packets received on this interface.
*
* @param    rtrIfNum  -  @b{(input)} Router interface number
* @param    badPkts    -  @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS     if success
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpIntfRcvBadPktsGet(L7_uint32 rtrIfNum,L7_ulong32 *badPkts);

/*********************************************************************
* @purpose  Get the number of invalid routes received on this interface.
*
* @param    rtrIfNum   -  @b{(input)}   Router interface number
* @param    badRts     -  @b{(output)}  Bad Routes
*
* @returns  L7_SUCCESS      if success
* @returns  L7_ERROR        if interface does not exist
* @returns  L7_FAILURE      if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpIntfRcvBadRoutesGet(L7_uint32 rtrIfNum,L7_ulong32 *badRts);

/*********************************************************************
* @purpose  Get the number of routes sent on this interface.
*
* @param    rtrIfNum  -   @b{(input)}    Router interface number
* @param    sent       -  @b{(output)}   Sent Routes
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpIntfSentRoutesGet(L7_uint32 rtrIfNum, L7_ulong32 *sent);

/*********************************************************************
* @purpose  Get if the entry of Interfaces is valid or not.
*
* @param    rtrIfNum   -  @b{(input)} Router Interface Number
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpIntfGet(L7_uint32 rtrIfNum);

/*********************************************************************
* @purpose  Get the state for the specified neighbour of the specified 
*           interface.
*
* @param    rtrIfNum   -  @b{(input)}  Router interface number
* @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
* @param    state      -  @b{(output)}  state
*
* @returns  L7_SUCCESS    if success
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE    if failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpNeighborStateGet(L7_uint32 rtrIfNum, 
                                     L7_inet_addr_t *nbrIpAddr, 
                                     L7_ulong32 *state);

/*********************************************************************
* @purpose  Check whether router interface running DVMRP 'rtrIfNum' is a leaf
*
* @param    rtrIfNum  -  @b{(input)}  Router interface number
*
* @returns  L7_SUCCESS       if leaf
* @returns  L7_FAILURE       if interface is not leaf
*
* @notes    needs only rtrIfNum.
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpIsInterfaceLeaf(L7_uint32 rtrIfNum);

/*********************************************************************
* @purpose  Chech whether DVMRP is a leaf router
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
L7_uint32 dvmrpRouterIsLeaf(void);

/*********************************************************************
* @purpose  Get the DVMRP Uptime for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum   -  @b{(input)}  Router Interface Number
* @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
* @param    nbrUpTime  -  @b{(output)} Neighbor Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpNeighborUpTimeGet(L7_uint32 rtrIfNum, 
                                      L7_inet_addr_t *nbrIpAddr, 
                                      L7_ulong32 *nbrUpTime);

/*********************************************************************
* @purpose  Get the DVMRP Expiry time for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum   -  @b{(input)}  Router Interface Number
* @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
* @param    nbrExpTime -  @b{(output)}  Neighbor Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR,     if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpNeighborExpiryTimeGet(L7_uint32 rtrIfNum, 
                                   L7_inet_addr_t *nbrIpAddr, 
                                   L7_ulong32 *nbrExpTime);

/*********************************************************************
* @purpose  Get the DVMRP Generation ID for the specified neighbour
*           of the specified interface
*
* @param    rtrIfNum     -  @b{(input)} router Interface Number
* @param    nbrIpAddr    -  @b{(input)} IP Address of the neighbour
* @param    nbrGenId     -  @b{(output)} Neighbor Generation ID
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR,       if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t dvmrpNeighborGenIdGet(L7_uint32 rtrIfNum, 
                L7_inet_addr_t *nbrIpAddr,                                                     L7_ulong32 *nbrGenId);
/*********************************************************************
 * @purpose  Get the DVMRP Major Version for the specified neighbour
 *           of the specified interface
 *
 * @param    rtrIfNum      -  @b{(input)}  Router Interface Number
 * @param    nbrIpAddr     -  @b{(input)}  IP Address of the neighbour
 * @param    majorVersion  -  @b{(output)} Neighbor Major Version
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR,        if interface does not exist
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborMajorVersionGet(L7_uint32 rtrIfNum, 
                      L7_inet_addr_t *nbrIpAddr, L7_ushort16 *majorVersion);

/*********************************************************************
 * @purpose  Get the DVMRP Minor Version for the specified neighbour
 *           of the specified interface
 *
 * @param    rtrIfNum      -  @b{(input)}  Router Interface Number
 * @param    nbrIpAddr     -  @b{(input)}  IP Address of the neighbour
 * @param    minorVersion  -  @b{(output)} Neighbor Minor Version
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR,        if interface does not exist
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborMinorVersionGet(L7_uint32 rtrIfNum, 
                     L7_inet_addr_t *nbrIpAddr, L7_ushort16 *minorVersion);


/*********************************************************************
 * @purpose  Get the DVMRP capabilities for the specified neighbour
 *           of the specified interface
 *
 * @param    rtrIfNum     -  @b{(input)}  Router Interface Number
 * @param    nbrIpAddr    -  @b{(input)}  IP Address of the neighbour
 * @param    capabilities -  @b{(output)} Neighbor Capabilities
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborCapabilitiesGet(L7_uint32 rtrIfNum, 
                       L7_inet_addr_t *nbrIpAddr, L7_ushort16 *capabilities);


/*********************************************************************
 * @purpose  Get the number of routes received for the specified neighbour
 *           of the specified interface.
 *
 * @param    rtrIfNum   -  @b{(input)}  Router interface number
 * @param    nbrIpAddr  -  @b{(input)}  IP Address of the neighbour
 * @param    rcvRoutes  -  @b{(output)}  Received Routes
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR,     if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborRcvRoutesGet(L7_uint32 rtrIfNum, 
                          L7_inet_addr_t *nbrIpAddr, L7_ulong32 *rcvRoutes);


/*********************************************************************
 * @purpose  Get the number of invalid packets received for the specified 
 *           neighbour of the specified interface.
 *
 * @param    rtrIfNum  -  @b{(input)}   Router interface number
 * @param    nbrIpAddr -  @b{(input)}   IP Address of the neighbour
 * @param    badPkts   -  @b{(output)}  Bad Packets
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborRcvBadPktsGet(L7_uint32 rtrIfNum, 
                             L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badPkts);

/*********************************************************************
 * @purpose  Get the number of invalid routes received for the specified 
 *           neighbour of the specified interface.
 *
 * @param    rtrIfNum   -  @b{(input)}   internal interface number
 * @param    nbrIpAddr  -  @b{(input)}   IP Address of the neighbour
 * @param    badRts     -  @b{(output)}  Bad Routes
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborRcvBadRoutesGet(L7_uint32 rtrIfNum, 
                          L7_inet_addr_t *nbrIpAddr, L7_ulong32 *badRts);



/*********************************************************************
 * @purpose  Get an entry of Neighbors.
 *
 * @param    rtrIfNum     -  @b{(input)}   internal interface number
 * @param    nbrIpAddr    -  @b{(input)}   IP Address of the neighbour
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR      if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    needs only rtrIfNum.
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborEntryGet(L7_uint32 *rtrIfNum, 
                                L7_inet_addr_t *nbrIpAddr);


/*********************************************************************
 * @purpose  Get the next entry on this interface.
 *
 * @param    intIfNum    -  @b{(input/output)}   internal interface number
 * @param    nbrIpAddr   -  @b{(input)}  IP Address of the neighbour
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR      if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    needs only intIfNum.
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpNeighborEntryNextGet(L7_uint32 *intIfNum, 
                L7_inet_addr_t *nbrIpAddr);

/*********************************************************************
 * @purpose  Gets the IP address of neighbor which is the source for
 *           the packets for a specified source address.
 *
 * @param    srcIpAddr  -  @b{(input)}   source IP Address
 * @param    srcMask    -  @b{(input)}   source Mask
 * @param    addr       -  @b{(output)}  IP Address
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteUpstreamNeighborGet(L7_inet_addr_t *srcIpAddr, 
                L7_inet_addr_t *srcMask, L7_inet_addr_t *addr);


/*********************************************************************
 * @purpose  Gets the Interface index for a specific route for a specified 
 *           source address.
 *
 * @param    srcIpAddr  -  @b{(input)}  source IP Address
 * @param    srcMask    -  @b{(input)}  source Mask
 * @param    IfIndex    -  @b{(output)}  Interface Index which is passed back 
 *                                      to the calling function
 *
 * @returns  L7_SUCCESS   if success
 * @returns  L7_ERROR,    if source address does not exist
 * @returns  L7_FAILURE   if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteIfIndexGet(L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, 
                       L7_uint32 *IfIndex);

/*********************************************************************
 * @purpose  Gets the distance in hops for a specified source address.
 *
 * @param    srcIpAddr  -  @b{(input)}   source IP Address
 * @param    srcMask    -  @b{(input)}   source Mask
 * @param    metric     -  @b{(output)}   metric
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteMetricGet(L7_inet_addr_t *srcIpAddr, 
                              L7_inet_addr_t *srcMask,
                              L7_ushort16 *metric);

/*********************************************************************
 * @purpose  Gets the route expiry time for a specified source address.
 *
 * @param    srcIpAddr   -  @b{(input)}  source IP Address
 * @param    srcMask     -  @b{(input)}  source Mask
 * @param    rtExpTime   -  @b{(output)} route expiry time
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteExpiryTimeGet(L7_inet_addr_t *srcIpAddr, 
                L7_inet_addr_t *srcMask, L7_ulong32 *rtExpTime);

/*********************************************************************
 * @purpose  Gets the time when a route was learnt for a specified source 
 *           address.
 *
 * @param    srcIpAddr -  @b{(input)}   source IP Address
 * @param    srcMask   -  @b{(input)}   source Mask
 * @param    rtUpTime  -  @b{(output)}  route up time
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteUptimeGet(L7_inet_addr_t *srcIpAddr, 
                            L7_inet_addr_t *srcMask,
                            L7_ulong32 *rtUpTime);



/*********************************************************************
 * @purpose  Get an entry of Routes.
 *
 * @param    srcIpAddr -  @b{(input)}   source IP Address
 * @param    srcMask   -  @b{(input)}   source Mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 * 
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteEntryGet(L7_inet_addr_t *srcIpAddr, 
                L7_inet_addr_t *srcMask);

/*********************************************************************
 * @purpose  Get the next entry of Routes.
 *
 * @param    srcIpAddr -  @b{(input)}   source IP Address
 * @param    srcMask   -  @b{(input)}   source Mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 * 
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteEntryNextGet(L7_inet_addr_t *srcIpAddr,
               L7_inet_addr_t *srcMask);

/*********************************************************************
 * @purpose  Gets the type of next hop router - leaf or branch,
 *           for a specified source address and specified interface.
 *
 * @param    nextHopSrc     -  @b{(input)}  Next Hop source IP Address
 * @param    nextHopSrcMask -  @b{(input)}  Next Hop source Mask
 * @param    nextHopRtrIfIndex -  @b{(input)}  Next Hop Interface Index
 * @param    nextHopType    -  @b{(output)} Next Hop Type
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteNextHopTypeGet(L7_inet_addr_t *nextHopSrc, 
                L7_inet_addr_t *nextHopSrcMask, L7_uint32 nextHopRtrIfIndex, 
                L7_uint32 *nextHopType);


/*********************************************************************
 * @purpose  Get an entry of next hops.
 *
 * @param    nextHopSrc     -  @b{(input)}   Next Hop source IP Address
 * @param    nextHopSrcMask -  @b{(input)}   Next Hop source Mask
 * @param    rtrIfNum       -  @b{(input)}   Router Interface Number
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    needs only intIfNum.
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteNextHopEntryGet(L7_inet_addr_t *nextHopSrc, 
                   L7_inet_addr_t *nextHopSrcMask, L7_uint32 *rtrIfNum);

/*********************************************************************
 * @purpose  Get the next entry of nexthops.
 *
 * @param    nextHopSrc       -  @b{(input)}          Next Hop source IP Address
 * @param    nextHopSrcMask   -  @b{(input)}          Next Hop source Mask
 * @param    nextHopIfIndex   -  @b{(input/output)}   Next hop Index
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteNextHopEntryNextGet(L7_inet_addr_t *nextHopSrc, 
                                             L7_inet_addr_t *nextHopSrcMask, 
                                             L7_uint32 *nextHopIfIndex);

/*********************************************************************
 * @purpose  Gets the prune expiry time for a group and specified source 
 *           address.
 *
 * @param    pruneGrp       -  @b{(input)}   prune group
 * @param    pruneSrc       -  @b{(input)}   prune source
 * @param    pruneSrcMask   -  @b{(input)}   prune source mask
 * @param    pruneExpTime   -  @b{(output)}  prune expiry time
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR,   if source address does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpPruneExpiryTimeGet(L7_inet_addr_t *pruneGrp, 
                        L7_inet_addr_t *pruneSrc, 
                        L7_inet_addr_t *pruneSrcMask, 
                        L7_ulong32 *pruneExpTime);


/*********************************************************************
 * @purpose  Get an entry of prunes.
 *
 * @param    pruneGrp       -  @b{(input)}   prune group
 * @param    pruneSrc       -  @b{(input)}   prune source
 * @param    pruneSrcMask   -  @b{(input)}   prune source mask
 *
 * 
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpPruneEntryGet(L7_inet_addr_t *pruneGrp,
                                  L7_inet_addr_t *pruneSrc, 
                                  L7_inet_addr_t *pruneSrcMask);

/*********************************************************************
 * @purpose  Get the next entry of prunes.
 *
 *
 * @param    pruneGrp       -  @b{(input)}   prune group
 * @param    pruneSrc       -  @b{(input)}   prune source
 * @param    pruneSrcMask   -  @b{(input)}   prune source mask
 * 
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpPruneEntryNextGet(L7_inet_addr_t *pruneGrp, 
                L7_inet_addr_t *pruneSrc, L7_inet_addr_t *pruneSrcMask);


/*********************************************************************
 * @purpose  Get the Route Flags
 *
 * @param    ipMRouteGroup       -  @b{(input)}  Group Address
 * @param    ipMRouteSource      -  @b{(input)}  Source Address
 * @param    ipMRouteSourceMask  -  @b{(input)}  Source Mask
 * @param    flags               -  @b{(output)} Flags
 *
 * @returns  L7_SUCCESS       if success
 * @returns  L7_ERROR         if interface does not exist
 * @returns  L7_FAILURE       if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteFlagsGet(L7_inet_addr_t *ipMRouteGroup, 
                              L7_inet_addr_t *ipMRouteSource,
                              L7_inet_addr_t  *ipMRouteSourceMask, 
                              L7_uint32*  flags);


/*********************************************************************
 * @purpose  Get the Route source
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    rpfAddr             -  @b{(output)} RPF address of (S,G)
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteRtAddrGet(L7_inet_addr_t *ipMRtGrp, 
                               L7_inet_addr_t *ipMRtSrc, 
                               L7_inet_addr_t *ipMRtSrcMsk,
                               L7_inet_addr_t *rpfAddr);

/*********************************************************************
 * @purpose  Get the Route source Mask
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    rtMask              -  @b{(output)} Mask pf route entry.
 *
 * @returns  L7_SUCCESS        if success
 * @returns  L7_ERROR          if interface does not exist
 * @returns  L7_FAILURE        if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteRtMaskGet(L7_inet_addr_t *ipMRtGrp, 
                              L7_inet_addr_t *ipMRtSrc,
                              L7_inet_addr_t *ipMRtSrcMsk, 
                              L7_inet_addr_t *rtMask);

/*********************************************************************
 * @purpose  Get the Route source Type
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    rtType              -  @b{(output)} Route Type.
 *
 * @returns  L7_SUCCESS    if success
 * @returns  L7_ERROR,     if interface does not exist
 * @returns  L7_FAILURE    if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteRtTypeGet (L7_inet_addr_t *ipMRtGrp, 
                                L7_inet_addr_t *ipMRtSrc,
                                L7_inet_addr_t *ipMRtSrcMsk, 
                                L7_uint32 *rtType);

/*********************************************************************
 * @purpose  Get the Route rpf address
 *
 * @param    ipMRtGrp            -  @b{(input)}  Group Address
 * @param    ipMRtSrc            -  @b{(input)}  Source Address
 * @param    ipMRtSrcMask        -  @b{(input)}  Source Mask
 * @param    prfAddr             -  @b{(output)} RPF address.
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteRpfAddrGet (L7_inet_addr_t *ipMRtGrp, 
                                 L7_inet_addr_t *ipMRtSrc,
                                 L7_inet_addr_t *ipMRtSrcMsk, 
                                 L7_inet_addr_t *rpfAddr);

/*********************************************************************
 * @purpose  Get the MRoute Entry
 *
 * @param    ipMRouteGroup      -  @b{(input)}  Group address
 * @param    ipMRouteSource     -  @b{(input)}  source IP Address
 * @param    ipMRouteSourceMask -  @b{(input)}  source IP mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteEntryGet(L7_inet_addr_t *ipMRouteGroup, 
                              L7_inet_addr_t *ipMRouteSource, 
                              L7_inet_addr_t  *ipMRouteSourceMask);

/*********************************************************************
 * @purpose  Get the Next MRoute Entry
 *
 * @param    ipMRtGrp      -  @b{(input)}      Group address
 * @param    ipMRtSrc      -  @b{(input)}      source IP Address
 * @param    ipMRtSrcMask  -  @b{(input)}      source IP mask
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteEntryNextGet(L7_inet_addr_t *ipMRtGrp, 
                                  L7_inet_addr_t *ipMRtSrc, 
                                  L7_inet_addr_t  *ipMRtSrcMask);

/*********************************************************************
 * @purpose  Get the output i/f id of the given source, group pair
 *
 * @param    ipMRtGrp      -  @b{(input)}      Group address
 * @param    ipMRtSrc      -  @b{(input)}      source IP Address
 * @param    ipMRtSrcMask  -  @b{(input)}      source IP mask
 * @param    intIfNum      -  @b{(output)}     output i/f id
 * 
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR, if interface does not exist
 * @returns  L7_FAILURE  if failure
 *
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpRouteOutIntfEntryNextGet(L7_inet_addr_t *ipMRtGrp, 
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask, 
                                      L7_uint32 *intIfNum);

/*********************************************************************
 * @purpose  This function is used to get the number of multicast route entries in the 
 *                  cache table.
 *
 * @param     None
 * @returns   entryCount - no of multicast route entries
 * @notes    None
 *
 * @end
 *********************************************************************/
extern L7_RC_t dvmrpIpMRouteEntryCountGet (L7_uint32 *entryCount);

/*********************************************************************
* @purpose  This function is used to Enable/Disable DVMRP  a given interface
*
* @param    dvmrpCbHandle  -  @b{(input)} DVMRP control block Handle.
* @param    dvmrpMetric    -  @b{(input)} dvmrp Metic.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
extern L7_RC_t dvmrp_interface_metric_set(MCAST_CB_HNDL_t dvmrpCbHandle,
                                          dvmrp_metric_t *dvmrpMetric);

/*********************************************************************
* @purpose  This function is used to enable/disable interface admin mode.
*
* @param    dvmrpCbHandle  -  @b{(input)} DVMRP control block Handle.
* @param    dvmrpMode      -  @b{(input)} dvmrp  Mode.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
extern L7_RC_t dvmrp_interface_adminmode_set(MCAST_CB_HNDL_t dvmrpCbHandle,
                                   dvmrp_adminmode_t *dvmrpAdminMode);
/*********************************************************************
* @purpose  This function is used to enable/disable Global admin mode.
*
* @param    dvmrpCbHandle   -  @b{(input)} DVMRP control block Handle.
* @param    globalAdminMode -  @b{(input)} dvmrp  Mode.
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*       
* @end
*********************************************************************/
L7_RC_t dvmrp_global_adminmode_set(MCAST_CB_HNDL_t dvmrpCbHandle,
                                      L7_uint32       globalAdminMode);

extern L7_RC_t dvmrpMemoryInit (L7_uchar8 addrFamily);

extern L7_RC_t dvmrpMemoryDeInit (L7_uchar8 addrFamily);

#endif
