/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    pimdm_map_vend_exten.h
*
* @purpose     PIMDM MRT vendor-specific API functions
*
* @component   PIMDM Mapping Layer
*
* @comments    Does not contain any references to vendor headers or types.
*
* @create      10/2/2006
*
* @author      gkiran
*
* @end
*
**********************************************************************/
#ifndef _PIMDM_VEND_EXTEN_H_
#define _PIMDM_VEND_EXTEN_H_

#include "l7_common.h"
#include "l3_addrdefs.h"
#include "pimdm_map_util.h"

/* NOTE: Make sure L7_AUTH_MAX_KEY_PIMDM definition matches this value */
/*#define PIMDM_MAP_EXTEN_AUTH_MAX_KEY   8   */ /* vendor code maximum */

/*---------------------------------------------------*/
/* PIMDM Mapping Layer vendor API function prototypes */
/*---------------------------------------------------*/

/*********************************************************************
* @purpose  Sets the PIMDM Admin mode
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    mode             @b{(input)} L7_ENABLE or L7_DISABLE
* @param    pimdmDoInit      @b{(input)} Flag to check whether PIM-DM
*                                        Memory can be Initialized/
                                         De-Initialized.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Controls the vendor PIMDM task through the use of a semaphore,
*           taking and holding it while disabled and releasing it while
*           enabled.
*
* @comments A flag is used to allow repeated calls for the same admin
*           state without hanging the process due to an already-taken
*           semaphore.
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenPimdmAdminModeSet(pimdmMapCB_t *pimdmMapCbPtr,
                                       L7_uint32 mode, L7_BOOL pimdmDoInit);

/*********************************************************************
* @purpose  Set the PIMDM administrative mode for the specified interface
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    mode           @b{(input)} Admin mode (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIntfAdminModeSet(pimdmMapCB_t *pimdmMapCbPtr,
                              L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Set the Hello interval of a PIMDM routing interface
*
* @param    pimdmMapCbPtr    @b{(input)}  Mapping Control Block.
* @param    intIfNum         @b{(input)}  Internal Interface Number
* @param    helloIntvl       @b{(output)} Hello interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The frequency at which PIM Hello messages are transmitted on this interface
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIntfHelloIntervalSet(pimdmMapCB_t *pimdmMapCbPtr, 
                            L7_uint32 intIfNum, L7_uint32 helloIntvl);

/*********************************************************************
* @purpose  Get the DR IP address for the specified specified interface
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    dsgRtrIpAddr    @b{(output)}IP Address of the Designated Router
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface Not configurable
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIntfDRGet(pimdmMapCB_t *pimdmMapCbPtr, 
        L7_uint32 intIfNum, L7_inet_addr_t *dsgRtrIpAddr);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)}  Mapping Control Block.
* @param    nbrIpAddr        @b{(input)}  Neighbhour IP Address.
* @param    nbrUpTime        @b{(output)}  The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrUpTimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
                            L7_inet_addr_t *nbrIpAddr, L7_uint32 *nbrUpTime);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Uptime for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)}  Mapping Control Block.
* @param    nbrIpAddr        @b{(input)}  Neighbhour IP Address.
* @param    nbrIpAddr        @b{(input)}  internal interface number
* @param    nbrUpTime        @b{(output)}  The time since the neighbour is up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrUpTimeByIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                           L7_inet_addr_t *nbrIpAddr,L7_uint32 intIfNum, 
                                           L7_uint32 *nbrUpTime);

/*********************************************************************
* @purpose  Get the Neighbor count specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum         @b{(input)}  Interface Number
* @param    nbrCount         @b{(output)} neighbour count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrCountGet(pimdmMapCB_t *pimdmMapCbPtr, 
                   L7_uint32 intIfNum, L7_uint32 *nbrCount);

/*********************************************************************
* @purpose  Get the Pimdm Interface index of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    nbrIpAddr        @b{(input)} Neighbor Ip address
* @param    pIntIfNum        @b{(output)} interface Num.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrIntfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                L7_inet_addr_t *nbrIpAddr, L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Get the PIMDM neighbor entry validity
*           on the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    nbrIpAddr        @b{(output)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                 L7_inet_addr_t *nbrIpAddr);

/*********************************************************************
* @purpose  Get the next Pimdm Neighbor Interface
*
* @param    pimdmMapCbPtr    @b{(inout)} Mapping Control Block.
* @param    ngbrIpAddr       @b{(inout)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                     L7_inet_addr_t* nbrIpAddr);

/*********************************************************************
* @purpose  Get the PIMDM neighbor entry validity on the specified interface
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    nbrIpAddr      @b{(input)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryByIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr,
                                          L7_uint32 intIfNum,
                                          L7_inet_addr_t* nbrIpAddr);

/*********************************************************************
* @purpose  Get the PIMDM Next neighbor entry by interface index on 
*           the specified interface
*
* @param    pimdmMapCbPtr    @b{(inout)} Mapping Control Block.
* @param    pimdmMapCbPtr    @b{(inout)} interface number.
* @param    ngbrIpAddr       @b{(inout)} Neighbor Ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrEntryByIfIndexNextGet(pimdmMapCB_t *pimdmMapCbPtr,
                                     L7_uint32 *outIntIfNum,
                                     L7_inet_addr_t* nbrIpAddr);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    ngbrIpAddr       @b{(input)} Neighbor Ip address
* @param    nbrExpireTime    @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrExpireTimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
              L7_inet_addr_t *nbrIpAddr, L7_uint32 *nbrExpireTime);

/*********************************************************************
* @purpose  Get the PIMDM Neighbor Expire time for the specified neighbour
*           of the specified interface
*
* @param    pimdmMapCbPtr    @b{(input)} Mapping Control Block.
* @param    ngbrIpAddr       @b{(input)} Neighbor Ip address
* @param    intIfNum         @b{(input)} internal interface number
* @param    nbrExpireTime    @b{(output)} Get the neighbor expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Time before this neighbor entry status expires
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenNbrExpireTimeByIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                               L7_inet_addr_t *nbrIpAddr,
                                               L7_uint32 intIfNum, 
                                               L7_uint32 *nbrExpireTime);

/*********************************************************************
* @purpose  Get the assert metric
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    assertMetric         @b{(output)} Assert metric
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteAssertMetricGet(pimdmMapCB_t *pimdmMapCbPtr, 
            L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
            L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetric);

/*********************************************************************
* @purpose  Get the assert metric preference
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    assertMetricPref     @b{(output)} Assert metric preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteAssertMetricPrefGet(pimdmMapCB_t *pimdmMapCbPtr, 
     L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
     L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetricPref);

/*********************************************************************
* @purpose  Get the RPT bit status
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    RPTBitStatus         @b{(output)} RPT bit status
*     
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteAssertRPTBitGet(pimdmMapCB_t *pimdmMapCbPtr, 
     L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
     L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *RPTBitStatus);

/*********************************************************************
* @purpose  Get the Upstream assert timer status
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    upstreamAssertTimer  @b{(output)} Upstream assert timer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteUpstreamAssertTimerGet(pimdmMapCB_t *pimdmMapCbPtr,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *upstreamAssertTimer);

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table  entry
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteEntryGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast route table Next entry
*
* @param    pimdmMapCbPtr        @b{(input)} Mapping Control Block.
* @param    ipMRouteGroup        @b{(inout)}  Multicast Group address
* @param    ipMRouteSource       @b{(inout)}  Source address
* @param    ipMRouteSourceMask   @b{(inout)}  Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t* ipMRouteGroup, L7_inet_addr_t* ipMRouteSource, 
    L7_inet_addr_t* ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table entry
*
* @param    pimdmMapCbPtr              @b{(input)} Mapping Control Block.
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopRtrIfNum    @b{(input)}  Next Hop RtrIfIndex
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteNextHopEntryGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteNextHopGroup, L7_inet_addr_t *ipMRouteNextHopSource,
    L7_inet_addr_t  *ipMRouteNextHopSourceMask, L7_uint32 ipMRouteNextHopRtrIfNum,
    L7_inet_addr_t *ipMRouteNextHopAddress);

/*********************************************************************
* @purpose  Get the PIMDM  Next Hop table Next entry
*
* @param  pimdmMapCbPtr              @b{(input)} Mapping Control Block.
* @param  ipMRouteNextHopGroup       @b{(inout)} Multicast Group address
* @param  ipMRouteNextHopSource      @b{(inout)}  Source address
* @param  ipMRouteNextHopSourceMask  @b{(inout)}  Mask Address
* @param  ipMRouteNextHopRtrIfNum    @b{(inout)}  Next Hop RtrIfIndex
* @param  ipMRouteNextHopAddress     @b{(inout)}  Next Hop Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteNextHopEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
   L7_inet_addr_t* ipMRouteNextHopGroup, L7_inet_addr_t* ipMRouteNextHopSource, 
   L7_inet_addr_t* ipMRouteNextHopSourceMask, L7_uint32* 
   ipMRouteNextHopRtrIfNum, L7_inet_addr_t* ipMRouteNextHopAddress);

/*********************************************************************
* @purpose  Get the Next Hop Prune reason on the specified interface
*
* @param    pimdmMapCbPtr              @b{(input)}  Mapping Control Block.
* @param    ipMRouteNextHopGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteNextHopSource      @b{(input)}  Source address
* @param    ipMRouteNextHopSourceMask  @b{(input)}  Mask Address
* @param    ipMRouteNextHopRtrIfNum    @b{(input)}  Next Hop RtrIfIndex
* @param    ipMRouteNextHopAddress     @b{(input)}  Next Hop Address
* @param    PruneReason                @b{(output)} Prune reason
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Prune is generated in router durig assert/flood-prune and IGMP no member cases
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteNextHopPruneReasonGet(pimdmMapCB_t *pimdmMapCbPtr,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask,L7_uint32 ipMRouteNextHopRtrIfNum,
    L7_inet_addr_t *ipMRouteNextHopAddress,L7_uint32 *pruneReason);

/*********************************************************************
* @purpose  Get the PIMDM  IP Multicast Flags
*
* @param    pimdmMapCbPtr        @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup        @b{(input)}  Multicast Group address
* @param    ipMRouteSource       @b{(input)}  Source address
* @param    ipMRouteSourceMask   @b{(input)}  Mask Address
* @param    flags                @b{(output)} flags.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteFlagsGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32*  flags);

/*********************************************************************
* @purpose  Get the PIMDM  IP multicast table  Out going interface list
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtrIfNum            @b{(input)}  Input Rtr interface index
* @param    state               @b{(input)}  interface status
* @param    mode                @b{(output)} PIM mode
* @param    upTime              @b{(output)} entry uptime
* @param    expiryTime          @b{(output)} entry expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteOIFNextGet
  (pimdmMapCB_t *pimdmMapCbPtr, L7_inet_addr_t *ipMRouteGroup,
   L7_inet_addr_t *ipMRouteSource, L7_inet_addr_t  *ipMRouteSourceMask, 
   L7_uint32* rtrIfNum,L7_uint32* state, L7_uint32* mode, L7_uint32* upTime,
   L7_uint32* expiryTime);

/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    pimdmMapCbPtr   @b{(input)} Mapping Control Block.
* @param    grpIpAddr       @b{(input)} Group IP Address
* @param    srcIpAddr       @b{(input)} source IP Address
* @param    srcMask         @b{(input)} source Mask
* @param    rtrIfNum        @b{(output)}Rtr Interface Index which is passed back 
*                                       to the calling function
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteIfIndexGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr,
  L7_inet_addr_t *srcMask, L7_uint32 *rtrIfNum);
/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    pimdmMapCbPtr @b{(input)} Mapping Control Block.
* @param    grpIpAddr     @b{(input)} Group IP Address
* @param    srcIpAddr     @b{(input)} source IP Address
* @param    srcMask       @b{(input)} source Mask
* @param    rtUptime      @b{(output)}route up time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteUptimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
 L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask,
 L7_uint32 *rtUpTime);
/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    pimdmMapCbPtr  @b{(input)} Mapping Control Block.
* @param    grpIpAddr      @b{(input)} Group IP Address
* @param    srcIpAddr      @b{(input)} source IP Address
* @param    srcMask        @b{(input)} source Mask
* @param    rtExpTime      @b{(output)} route expiry time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteExpiryTimeGet(pimdmMapCB_t *pimdmMapCbPtr, 
 L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask,
 L7_uint32 *rtExpTime);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtAddr              @b{(output)}  rt Address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtAddrGet(pimdmMapCB_t *pimdmMapCbPtr, 
 L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
 L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rpfAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtMask              @b{(output)}  rt mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtMaskGet(pimdmMapCB_t *pimdmMapCbPtr, 
   L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
   L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtMask);
/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtType              @b{(output)}  rt Type.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtTypeGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtType);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Proto.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rtProto             @b{(output)} Route Protocol
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRtProtoGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtProto);

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Addr.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Mask Address
* @param    rpfAddr             @b{(output)} rpfAddr.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteRpfAddrGet(pimdmMapCB_t *pimdmMapCbPtr, 
  L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
  L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    entryCount          @b{(output)}  entryCount
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteEntryCountGet(pimdmMapCB_t *pimdmMapCbPtr, 
                                                        L7_uint32 *entryCount);

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count.
*
* @param    pimdmMapCbPtr      @b{(input)}  Mapping Control Block.
* @param    heCount            @b{(output)}  entryCount
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpMRouteHighestEntryCountGet(pimdmMapCB_t *pimdmMapCbPtr,
                                                  L7_uint32 *heCount);
/*********************************************************************
* @purpose  Delete Routes
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteDelete(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource);
/*********************************************************************
* @purpose  Delete Routes
*
* @param    none
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenRouteDeleteAll();

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Addr.
*
* @param    pimdmMapCbPtr       @b{(input)}  Mapping Control Block.
* @param    ipMRouteGroup       @b{(input)}  Multicast Group address
* @param    ipMRouteSource      @b{(input)}  Source address
* @param    ipMRouteSourceMask  @b{(input)}  Source Mask
* @param    outIntf             @b{(input)} Out interface.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapExtenIpRouteOutIntfEntryNextGet(pimdmMapCB_t *pimdmMapCbPtr, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *outIfNum);

#endif /* _PIMDM_VEND_EXTEN_H_ */
