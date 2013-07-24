/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  usmdb_mib_dvmrp.c
*
* @purpose   Imlementation of USMDB layer for DVMRP.
*
* @component USMDB
*
* @comments  none
*
* @create    26/02/2002
*
* @author    Mahe.Korukonda
* @end
*
**********************************************************************/

#include "l7_common.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_dvmrp_api.h"
#endif


#include "nimapi.h"
#include "trap_layer3_mcast_api.h"
#include "l7_dvmrp_api.h"
#include "dvmrp_debug_api.h"

/*********************************************************************
* @purpose  Get the administrative mode of DVMRP component in the Router
*
* @param    UnitIndex  @b{(input)}  UnitIndex
* @param    mode       @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *mode)
{
  return dvmrpMapDvmrpAdminModeGet(mode);
}

/*********************************************************************
* @purpose  Sets the administrative mode of DVMRP component in the Router
*
* @param    UnitIndex  @b{(input)} UnitIndex
* @param    mode       @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpAdminModeSet(L7_uint32 UnitIndex, L7_uint32 mode)
{
  return dvmrpMapDvmrpAdminModeSet(mode);
}

/*********************************************************************
* @purpose  Get the administrative mode of a DVMRP routing interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number.
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Interface does not exist, 
*                       or is not enabled for routing.
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfAdminModeGet(L7_uint32 UnitIndex, 
                                   L7_uint32 intIfNum, L7_uint32 *mode)
{
  return dvmrpMapIntfAdminModeGet(intIfNum, mode);
}

/*********************************************************************
* @purpose  Get the operational state of a DVMRP routing interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbDvmrpInterfaceOperationalStateGet(L7_uint32 UnitIndex, 
                                               L7_uint32 intIfNum)
{
  return dvmrpMapIntfIsOperational(intIfNum);
}

/*********************************************************************
* @purpose  Sets the DVMRP Admin mode for the specified interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number.
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfAdminModeSet(L7_uint32 UnitIndex, 
                                   L7_uint32 intIfNum, L7_uint32 mode)
{
  return dvmrpMapIntfAdminModeSet(intIfNum, mode);
}

/*********************************************************************
* @purpose  Gets the DVMRP Version String being used
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    versionString  @b{(output)} Version String.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpVersionStringGet(L7_uint32 UnitIndex, L7_uchar8* versionStr)
{
  return dvmrpMapVersionStringGet(versionStr);
}

/*********************************************************************
* @purpose  Gets the DVMRP Generation ID used by the router
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    genId          @b{(output)} Generation Id.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfGenerationIdGet(L7_uint32 UnitIndex, 
                                      L7_uint32 intIfNum, L7_ulong32* genId)
{
  return dvmrpMapIntfGenerationIDGet(intIfNum, genId);
}

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    numRoutes      @b{(output)} Number of Routes.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNumRoutesGet(L7_uint32 UnitIndex, L7_uint32* numRoutes)
{
  return dvmrpMapNumRoutesGet(numRoutes);
}

/*********************************************************************
* @purpose  Gets the number of routes in the DVMRP routing table with
*           non-infinite metric
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    reachable      @b{(output)} reachable Routes.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpReachableRoutesGet(L7_uint32 UnitIndex, 
                                     L7_uint32* reachableRoutes)
{
  return dvmrpMapReachableRoutesGet(reachableRoutes);
}

/*********************************************************************
* @purpose  Gets the IP address of the DVMRP interface
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    intIfNum       @b{(input)} Internal Interface Number
* @param    addr           @b{(output)} Ip Address.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfLocalAddressGet(L7_uint32 UnitIndex, 
                                      L7_uint32 intIfNum, L7_inet_addr_t *addr)
{
  return dvmrpMapIntfLocalAddressGet(intIfNum, addr);
}

/*********************************************************************
* @purpose  Gets the DVMRP Metric for the specified interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(output)} Metric.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfMetricGet(L7_uint32 UnitIndex, 
                                L7_uint32 intIfNum, L7_ushort16* metric)
{
  return dvmrpMapIntfMetricGet(intIfNum, metric);
}

/*********************************************************************
* @purpose  Sets the DVMRP Metric for the specified interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    metric      @b{(input)} Metric.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfMetricSet(L7_uint32 UnitIndex, 
                                L7_uint32 intIfNum, L7_uint32 metric)
{
  return dvmrpMapIntfMetricSet(intIfNum, metric);
}

/*********************************************************************
* @purpose  Get the number of invalid packets received on this interface.
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    badPkts     @b{(output)} Bad Packets
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfRcvBadPktsGet(L7_uint32 UnitIndex, 
                                    L7_uint32 intIfNum, L7_ulong32* badPkts)
{
  return dvmrpMapIntfRcvBadPktsGet(intIfNum, badPkts);
}

/*********************************************************************
* @purpose  Get the number of invalid routes received on this interface.
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    badRts      @b{(output)} Bad Routes.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfRcvBadRoutesGet(L7_uint32 UnitIndex, 
                                      L7_uint32 intIfNum, L7_ulong32* badRts)
{
  return dvmrpMapIntfRcvBadRoutesGet(intIfNum, badRts);
}

/*********************************************************************
* @purpose  Get the number of routes sent on this interface.
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    sent        @b{(output)} Sent Routes.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfSentRoutesGet(L7_uint32 UnitIndex, 
                                    L7_uint32 intIfNum, L7_ulong32* sent)
{
  return dvmrpMapIntfSentRoutesGet(intIfNum, sent);
}

/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    intIfNum    @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpInterfaceEntryNextGet(L7_uint32 UnitIndex, 
                                                  L7_uint32 *intIfNum)
{
  return dvmrpMapIntfEntryNextGet(intIfNum);
}

/*********************************************************************
* @purpose  Get the DVMRP Uptime for the specified neighbour
*           of the specified interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    nbrIntIfNum @b{(input)} Internal Interface Number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour.
* @param    nbrUpTime   @b{(output)} Neighbor Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborUpTimeGet(L7_uint32 UnitIndex, 
                                    L7_uint32 nbrIntIfNum,
                                    L7_inet_addr_t *nbrIpAddr, 
                                    L7_ulong32* nbrUpTime)
{
  return dvmrpMapNeighborUpTimeGet(nbrIntIfNum, nbrIpAddr, nbrUpTime);
}

/*********************************************************************
* @purpose  Get the DVMRP Expiry time for the specified neighbour
*           of the specified interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    nbrIntIfNum @b{(input)} Internal Interface Number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour.
* @param    nbrExpTime  @b{(output)} Neighbor Expiry Time.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborExpiryTimeGet(L7_uint32 UnitIndex,
                                        L7_uint32 nbrIntIfNum,
                                        L7_inet_addr_t *nbrIpAddr,
                                        L7_ulong32     *nbrExpTime)
{
  return dvmrpMapNeighborExpiryTimeGet(nbrIntIfNum, nbrIpAddr, nbrExpTime);
}

/*********************************************************************
* @purpose  Get the DVMRP Generation ID for the specified neighbour
*           of the specified interface
*
* @param    UnitIndex   @b{(input)} UnitIndex
* @param    nbrIntIfNum @b{(input)} Internal Interface Number
* @param    nbrIpAddr   @b{(input)} IP Address of the neighbour
* @param    nbrGenId    @b{(output)} neighbor Generation Id.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborGenIdGet(L7_uint32 UnitIndex, L7_uint32 nbrIntIfNum, 
                               L7_inet_addr_t *nbrIpAddr, L7_ulong32 *nbrGenId)
{
  return dvmrpMapNeighborGenIdGet(nbrIntIfNum, nbrIpAddr, nbrGenId);
}

/*********************************************************************
* @purpose  Get the DVMRP Major Version for the specified neighbour
*           of the specified interface
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    nbrIntIfNum  @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    majorVersion @b{(output)} Neighbor Major Version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborMajorVersionGet(L7_uint32 UnitIndex, 
             L7_uint32 nbrIntIfNum, L7_inet_addr_t *nbrIpAddr, 
             L7_ushort16* majorVersion)
{
  return dvmrpMapNeighborMajorVersionGet(nbrIntIfNum, nbrIpAddr, majorVersion);
}

/*********************************************************************
* @purpose  Get the DVMRP Minor Version for the specified neighbour
*           of the specified interface
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    nbrIntIfNum  @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    minorVersion @b{(output)} Neighbor Minor Version
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborMinorVersionGet(L7_uint32 UnitIndex, 
             L7_uint32 nbrIntIfNum, L7_inet_addr_t *nbrIpAddr, 
             L7_ushort16* minorVersion)
{
  return dvmrpMapNeighborMinorVersionGet(nbrIntIfNum, nbrIpAddr, minorVersion);
}


/*********************************************************************
* @purpose  Get the DVMRP capabilities for the specified neighbour
*           of the specified interface
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    nbrIntIfNum  @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    capabilities @b{(output)} Neighbor Capabilities.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborCapabilitiesGet(L7_uint32 UnitIndex, 
             L7_uint32 nbrIntIfNum, L7_inet_addr_t *nbrIpAddr, 
             L7_ushort16* capabilities)
{
  return dvmrpMapNeighborCapabilitiesGet(nbrIntIfNum, nbrIpAddr, capabilities);
}

/*********************************************************************
* @purpose  Get the number of routes received for the specified neighbour
*           of the specified interface.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    nbrIntIfNum  @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    rcvRoutes    @b{(output)} Received Routes
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborRcvRoutesGet(L7_uint32 UnitIndex, 
    L7_uint32 nbrIntIfNum, L7_inet_addr_t *nbrIpAddr, L7_ulong32* rcvRoutes)
{
  return dvmrpMapNeighborRcvRoutesGet(nbrIntIfNum, nbrIpAddr, rcvRoutes);
}

/*********************************************************************
* @purpose  Get the number of invalid packets received for the 
*           specified neighbour of the specified interface.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    nbrIntIfNum  @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    badPkts      @b{(output)} Bad Packets.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborRcvBadPktsGet(L7_uint32 UnitIndex, 
    L7_uint32 nbrIntIfNum, L7_inet_addr_t *nbrIpAddr, L7_ulong32* badPkts)
{
  return dvmrpMapNeighborRcvBadPktsGet(nbrIntIfNum, nbrIpAddr, badPkts);
}

/*********************************************************************
* @purpose  Get the number of invalid routes received for the specified 
*           neighbour of the specified interface.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    nbrIntIfNum  @b{(input)} Internal Interface Number
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour
* @param    badRts       @b{(output)} Bad Routes.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborRcvBadRoutesGet(L7_uint32 UnitIndex, 
    L7_uint32 nbrIntIfNum, L7_inet_addr_t *nbrIpAddr, L7_ulong32* badRts)
{
  return dvmrpMapNeighborRcvBadRoutesGet(nbrIntIfNum, nbrIpAddr, badRts);
}

/*********************************************************************
* @purpose  Get the state for the specified neighbour
*           of the specified interface.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    nbrIntIfNum  @b{(input)} Internal Interface Number.
* @param    nbrIpAddr    @b{(input)} IP Address of the neighbour.
* @param    state        @b{(output)} state.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborStateGet(L7_uint32 UnitIndex, 
    L7_uint32 nbrIntIfNum, L7_inet_addr_t *nbrIpAddr, L7_ulong32* state)
{
  return dvmrpMapNeighborStateGet(nbrIntIfNum, nbrIpAddr, state);
}

/*********************************************************************
* @purpose  Get the next entry on this interface.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    intIfNum     @b{(inout)} Internal Interface Number.
* @param    nbrIpAddr    @b{(inout)} IP Address of the neighbour.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborEntryNextGet(L7_uint32 UnitIndex, 
                          L7_uint32 *intIfNum, L7_inet_addr_t *nbrIpAddr)
{
  return dvmrpMapNeighborEntryNextGet(intIfNum, nbrIpAddr);
}

/*********************************************************************
* @purpose  Gets the IP address of neighbor which is the source for
*           the packets for a specified source address.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    addr         @b{(output)} IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteUpstreamNeighborGet(L7_uint32 UnitIndex, 
    L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, L7_inet_addr_t *addr)
{
  return dvmrpMapRouteUpstreamNeighborGet(srcIpAddr, srcMask, addr);
}

/*********************************************************************
* @purpose  Gets the Interface index for a specific route
*           for a specified source address.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    IfIndex      @b{(output)} IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteIfIndexGet(L7_uint32 UnitIndex, 
             L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, 
             L7_uint32 *ifIndex)
{
  return dvmrpMapRouteIfIndexGet(srcIpAddr, srcMask, ifIndex);
}

/*********************************************************************
* @purpose  Gets the distance in hops for a specified source address.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    metric       @b{(output)} metric
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteMetricGet(L7_uint32 UnitIndex, 
    L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, L7_ushort16 *metric)
{
  return dvmrpMapRouteMetricGet(srcIpAddr, srcMask, metric);
}

/*********************************************************************
* @purpose  Gets the route expiry time for a specified source address.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtExpTime    @b{(output)} route expiry time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteExpiryTimeGet(L7_uint32 UnitIndex, 
    L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, L7_ulong32 *rtExpTime)
{
  return dvmrpMapRouteExpiryTimeGet(srcIpAddr, srcMask, rtExpTime);
}

/*********************************************************************
* @purpose  Gets the time when a route was learnt
*           for a specified source address.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    srcIpAddr    @b{(input)} source IP Address
* @param    srcMask      @b{(input)} source Mask
* @param    rtUpTime     @b{(output)} route up time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteUpTimeGet(L7_uint32 UnitIndex, 
    L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask, L7_ulong32 *rtUptime)
{
  return dvmrpMapRouteUptimeGet(srcIpAddr, srcMask, rtUptime);
}

/*********************************************************************
* @purpose  Get the next entry of Routes.
*
* @param    UnitIndex    @b{(input)} UnitIndex
* @param    srcIpAddr    @b{(inout)} source IP Address
* @param    srcMask      @b{(inout)} source Mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteEntryNextGet(L7_uint32 UnitIndex, 
                        L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask)
{
  return dvmrpMapRouteEntryNextGet(srcIpAddr, srcMask);
}

/*********************************************************************
* @purpose  Gets the type of next hop router - leaf or branch,
*           for a specified source address and specified interface.
*
* @param    UnitIndex       @b{(input)} UnitIndex
* @param    nextHopSrc      @b{(input)} Next Hop source IP Address
* @param    nextHopSrcMask  @b{(input)} Next Hop source Mask
* @param    nextHopIfIndex  @b{(input)} Next Hop Interface Index
* @param    nextHopType     @b{(output)} Next Hop Type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNextHopTypeGet(L7_uint32 UnitIndex, 
            L7_inet_addr_t *nextHopSrc, L7_inet_addr_t *nextHopSrcMask, 
            L7_uint32 nextHopIfIndex, L7_uint32 *nextHopType)
{
  return dvmrpMapRouteNextHopTypeGet(nextHopSrc, nextHopSrcMask, 
                                     nextHopIfIndex, nextHopType);
}

/*********************************************************************
* @purpose  Get the next entry of nexthops.
*
* @param    UnitIndex       @b{(input)} UnitIndex
* @param    nextHopSrc      @b{(inout)} Next Hop source IP Address
* @param    nextHopSrcMask  @b{(inout)} Next Hop source Mask
* @param    nextHopIfIndex  @b{(inout)} Next Hop Interface Index
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteNextHopEntryNextGet(L7_uint32 UnitIndex, 
               L7_inet_addr_t *nextHopSrc, L7_inet_addr_t *nextHopSrcMask, 
               L7_uint32 *nextHopIfIndex)
{
  return dvmrpMapRouteNextHopEntryNextGet(nextHopSrc, 
                                          nextHopSrcMask, nextHopIfIndex);
}

/*********************************************************************
* @purpose  Gets the prune expiry time for a group and
*           specified source address.
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    pruneGrp       @b{(input)} prune group
* @param    pruneSrc       @b{(input)} prune source
* @param    pruneSrcMask   @b{(input)} prune source mask
* @param    pruneExpTime   @b{(output)} prune expiry time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR,   if source address does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpPruneExpiryTimeGet(L7_uint32 UnitIndex, 
                   L7_inet_addr_t *pruneGrp, L7_inet_addr_t *pruneSrc, 
                   L7_inet_addr_t *pruneSrcMask, L7_ulong32 *pruneExpTime)
{
  return dvmrpMapPruneExpiryTimeGet(pruneGrp, pruneSrc, 
                                    pruneSrcMask, pruneExpTime);
}

/*********************************************************************
* @purpose  Get the next entry of prunes.
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    pruneGrp       @b{(inout)} prune group
* @param    pruneSrc       @b{(inout)} prune source
* @param    pruneSrcMask   @b{(inout)} prune source mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpPruneEntryNextGet(L7_uint32 UnitIndex, 
                      L7_inet_addr_t *pruneGrp, L7_inet_addr_t *pruneSrc, 
                      L7_inet_addr_t *pruneSrcMask)
{
  return dvmrpMapPruneEntryNextGet(pruneGrp, pruneSrc, pruneSrcMask);
}

/*********************************************************************
* @purpose  Get the entry of Dvmrp Interface.
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    intIfNum       @b{(inout)} Internal Interface Number.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpIntfEntryGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum)
{
  return dvmrpMapIntfEntryGet(intIfNum);
}
/*********************************************************************
* @purpose  Get the entry of Dvmrp Neighbor.
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    nbrIntIfNum    @b{(inout)} Neighbor Interface Number.
* @param    nbrIpAddr      @b{(inout)} Neighbor IP Address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNeighborEntryGet(L7_uint32 UnitIndex, 
                      L7_uint32 *nbrIntIfNum, L7_inet_addr_t *nbrIpAddr)
{
  return dvmrpMapNeighborEntryGet(nbrIntIfNum, nbrIpAddr);
}

/*********************************************************************
* @purpose  Get the entry of Dvmrp Route.
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    srcIpAddr      @b{(inout)} Source Ip Address.
* @param    srcMask        @b{(inout)} source Mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpRouteEntryGet(L7_uint32 UnitIndex, 
                         L7_inet_addr_t *srcIpAddr, L7_inet_addr_t *srcMask)
{
  return dvmrpMapRouteEntryGet(srcIpAddr, srcMask);
}

/*********************************************************************
* @purpose  Get the entry of Dvmrp next hop.
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    nextHopSrc     @b{(inout)} Next Hop Source.
* @param    nextHopSrcMask @b{(inout)} Next Hop source Mask.
* @param    nextHopIfIndex @b{(inout)} Next Hop If Index.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpNextHopEntryGet(L7_uint32 UnitIndex, 
                 L7_inet_addr_t *nextHopSrc, L7_inet_addr_t *nextHopSrcMask, 
                 L7_uint32 *nextHopIfIndex)
{
  return dvmrpMapRouteNextHopEntryGet(nextHopSrc, 
                                      nextHopSrcMask, nextHopIfIndex);
}
/*********************************************************************
* @purpose  Get the entry of Dvmrp prune.
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    pruneGrp       @b{(inout)} prune Group.
* @param    pruneSrc       @b{(inout)} prune Source.
* @param    pruneSrcMask   @b{(inout)} prune source mask.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE  if failure
*
* @comments    needs only intIfNum.
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpPruneEntryGet(L7_uint32 UnitIndex, L7_inet_addr_t *pruneGrp, 
                       L7_inet_addr_t *pruneSrc, L7_inet_addr_t *pruneSrcMask)
{
  return dvmrpMapPruneEntryGet(pruneGrp, pruneSrc, pruneSrcMask);
}


/*********************************************************************
* @purpose  Sets DVMRP Trap Mode
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    mode           @b{(input)}  L7_ENABLE/L7_DISABLE
* @param    trapType       @b{(input)}  trap
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpTrapModeSet(L7_uint32 unitIndex, 
                              L7_uint32 mode, L7_uint32 trapType)
{
  return trapMgrDvmrpTrapModeSet(mode, trapType);
}

/*********************************************************************
* @purpose  Gets DVMRP Trap Mode
*
* @param    UnitIndex      @b{(input)} UnitIndex
* @param    trapType       @b{(input)}  trap
* @param    *mode          @b{(output)}  L7_ENABLE/L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpTrapModeGet(L7_uint32 unitIndex, 
                              L7_uint32 trapType, L7_uint32 *mode)
{
    return trapMgrDvmrpTrapModeGet(mode, trapType);
}

/*********************************************************************
* @purpose  Check if interface is valid for DVMRP
*
* @param    unitIndex      @b{(input)} UnitIndex
* @param    intIfNum       @b{(input)} Internal interface number.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbDvmrpIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
    return dvmrpIntfIsValid(intIfNum);
}
/*********************************************************************
* @purpose  Turns on/off the displaying of dvmrp packet debug info
*            
* @param    rxFlag     @b{(input)} Receive Trace Flag
* @param    txFlag     @b{(input)} Transmit Trace Flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpPacketDebugTraceFlagSet(L7_BOOL rxFlag,
                                          L7_BOOL txFlag)
{
  if (dvmrpDebugTraceFlagSet(DVMRP_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (dvmrpDebugTraceFlagSet(DVMRP_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the dvmrp packet debug info
*            
* @param    rxFlag     @b{(output)} Receive Trace Flag
* @param    txFlag     @b{(output)} Transmit Trace Flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbDvmrpPacketDebugTraceFlagGet(L7_BOOL *rxFlag,
                                          L7_BOOL *txFlag)
{
  if (dvmrpDebugTraceFlagGet(DVMRP_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (dvmrpDebugTraceFlagGet(DVMRP_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

