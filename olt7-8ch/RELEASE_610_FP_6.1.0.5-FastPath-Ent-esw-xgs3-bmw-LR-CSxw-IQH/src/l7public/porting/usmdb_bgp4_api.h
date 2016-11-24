
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   usmdb_bgp4_api.h
*
* @purpose    externs for L7_RC_t usmDb layer
*
* @component  BGP4(L7_uint32 UnitIndex, Layer 3);
*
* @comments   none
*
* @create     01/28/2002
*
* @author     mahe.korukonda
*
* @end
*
**********************************************************************/

#ifndef USMDB_BGP4_API_H
#define USMDB_BGP4_API_H

#include "l3_commdefs.h"
#include "l3_bgp_comm_structs.h"
#include "usmdb_common.h"

/*********************************************************************
* @purpose  Create an aggregation entry
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *prefix     bgpIpAddr
* @param    *prefixLen    L7_ushort16
* @param    *aggrEffect   L7_ushort16
* @param    *advUnfeasible  L7_BOOL
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AddrAggregationCreate(L7_uint32 UnitIndex, L7_uint32 *index, L7_uint32 prefix,
                                       L7_ushort16 prefixLen, L7_ushort16 effect, L7_BOOL advUnfeasible);

/*********************************************************************
*
* @purpose  Delete the aggregation entry
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *prefix    bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AddrAggregationDelete(L7_uint32 UnitIndex, L7_uint32 prefix);

/*********************************************************************
*
* @purpose  Update the aggregation entry advertise mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *prefix    bgpIpAddr
* @param    effect  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AddrAggregationAdvMode(L7_uint32 UnitIndex, L7_uint32 prefix,
                                       L7_ushort16 effect);


/*********************************************************************
*
* @purpose  Update the aggregation entry feasibility mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *prefix    bgpIpAddr
* @param    advUnfeasible  L7_BOOL
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AddrAggregationFeasMode(L7_uint32 UnitIndex, L7_uint32 prefix,
                                       L7_BOOL advUnfeasible);

/*********************************************************************
*
* @purpose  Get the Next entry of Address Family list
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *addrEntry  bgpAddrFamily_t
* @param    *previousEntry  bgpAddrFamily_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AddrFamilyEntryGetNext(L7_uint32 UnitIndex, bgpAddrFamily_t* addrEntry,
                                        bgpAddrFamily_t* prevAddrEntry);
/*********************************************************************
* @purpose  Gets the Bgp Admin Mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " The administrative status of BGP in the router. The value
*           'enabled' denotes that the BGP process is active on at
*           least one interface; 'disabled' disables it on all
*           interfaces."
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AdminModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Sets the Bgp Admin mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode  @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    " The administrative status of BGP in the router. The value
*           'enabled' denotes that the BGP process is active on at
*           least one interface; 'disabled' disables it on all
*           interfaces."
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AdminModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the next address aggregation entry of the aggr list
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *prefix     bgpIpAddr
* @param    *prefixLen    L7_ushort16
* @param    *aggrEffect   L7_ushort16
* @param    *advUnfeasible  L7_BOOL
* @param    *previousPrefix bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AggrEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *index, bgpIpAddr* prefix, L7_ushort16 *prefixLen,
                                  L7_ushort16 *aggrEffect, L7_BOOL *advUnfeasible, bgpIpAddr *previousPrefix);

/*********************************************************************
* @purpose  Get the address aggregation entry based on the index
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    index     L7_uint32
* @param    *prefix     bgpIpAddr
* @param    *prefixLen    L7_ushort16
* @param    *aggrEffect   L7_ushort16
* @param    *advUnfeasible  L7_BOOL
* @param    *previousPrefix bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AggrEntryGet(L7_uint32 UnitIndex, L7_uint32 index, bgpIpAddr* prefix, L7_ushort16 *prefixLen,
                              L7_ushort16 *aggrEffect, L7_BOOL *advUnfeasible);

/*********************************************************************
* @purpose  Get the mode of aggregation.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode        @b{(output)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AggregationModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Set the mode of aggregation.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode        @b{(input)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AggregationModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the local Autonomous System Number
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    asNumber  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4LocalAsSet(L7_uint32 UnitIndex, L7_uint32 asNumber);

/*********************************************************************
*
* @purpose  Get the mode of auto restart
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE

* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if not enabled
*
* @notes    Use internal automatically Start message sending in case of connection failure
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AutoRestartModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode of auto restart
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode  L7_uint32  [L7_ENABLE to enable and L7_DISABLE to disable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Use internal automatically Start message sending in case of connection failure
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4AutoRestartModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
* @purpose  Get mode value which decides to use or not use MED value
*
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if disabled
*
* @notes
*
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4CalcMEDGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Set mode value which decides to use or not use MED value
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode  L7_uint32  [L7_ENABLE to enable and L7_DISABLE to disable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The number of times that the intra-area route table
*             has been calculated using this area's link-state
*             database. This is typically done using Djikstra's
*             algorithm."
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4CalcMEDSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the Confederation Id of this router
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *confedId      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    An externally visible autonomous system number that
*     identifies the confederation as a whole
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ConfedIdGet(L7_uint32 UnitIndex, L7_ushort16 *confedId);

/*********************************************************************
*
* @purpose  Set the Confederation Id of this router
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    confedId      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    An externally visible autonomous system number that
*     identifies the confederation as a whole
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ConfedIdSet(L7_uint32 UnitIndex, L7_uint32 confedId);

/*********************************************************************
*
* @purpose  Add the AFI, SAFI to the list
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    Afi  L7_ushort16
* @param    Safi  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ConfigAddrFamily(L7_uint32 UnitIndex, L7_ushort16 afi, L7_ushort16 safi);

/*********************************************************************
*
* @purpose  Get the Cluster Id this router belongs to
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *clusterId      bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ClusterIDGet(L7_uint32 UnitIndex, L7_uint32* clusterId);

/*********************************************************************
*
* @purpose  Set the Cluster Id this router belongs to
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *clusterId      bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ClusterIDSet(L7_uint32 UnitIndex, L7_uint32 clusterId);

/*********************************************************************
*
* @purpose  Set the Cluster Id this router belongs to
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *clusterId      bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4DampenedPathsGetNext(L7_uint32 UnitIndex, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                                      L7_ushort16 *state, L7_uint32 *penaltyValue, L7_ushort16 *decayDecrement,
                                      usmDbTimeSpec_t *timeCreated, usmDbTimeSpec_t *timeSuppressed, L7_ushort16 *eventState,
                                      bgpIpAddr *prevPrefix, L7_ushort16 prevPrefixLen);

/*********************************************************************
*
* @purpose  Delete the AFI, SAFI from the list
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    Afi  L7_ushort16
* @param    Safi  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4DeleteAddrFamily(L7_uint32 UnitIndex, L7_ushort16 afi, L7_ushort16 safi);

/*********************************************************************
* @purpose  Get the mode for external border router advertisement
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if not enabled
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ExtBrdRouterAdvModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Get the mode for external border router advertisement
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if not enabled
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ExtBrdRouterAdvModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
* @purpose  Set the mode for external border router advertisement
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode  L7_uint32  [L7_ENABLE to enable and L7_DISABLE to disable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4FlapDampingFactorGet(L7_uint32 UnitIndex, L7_uint32* dampFactor);

/*********************************************************************
*
* @purpose  Get the value of damping factor
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *dampFactor L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4FlapDampingFactorSet(L7_uint32 UnitIndex, L7_uint32 dampFactor);

/*********************************************************************
*
* @purpose  Set the value of damping factor
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    dampFactor    L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4FlapDampingModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode for route flap damping
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode    L7_uint32 [L7_ENABLE to enable, L7_DISABLE to disable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4FlapDampingModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the value of maximum flap entry life time
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *flapMaxTime  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4FlapMaxTimeGet(L7_uint32 UnitIndex, L7_uint32* flapMaxTime);

/*********************************************************************
*
* @purpose  Set the value of maximum flap entry life time
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    flapMaxTime   L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4FlapMaxTimeSet(L7_uint32 UnitIndex, L7_uint32 flapMaxTime);

/*********************************************************************
*
* @purpose  Get the value of local route origin
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *localOrigin  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4LocalOriginGet(L7_uint32 UnitIndex, L7_ushort16* localOrigin);

/*********************************************************************
*
* @purpose  Set the value of local Origin
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    localOrigin   L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4LocalOriginSet(L7_uint32 UnitIndex, L7_uint32 localOrigin);

/*********************************************************************
*
* @purpose  Get the value of MED attribute
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *localMed L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4LocalMedGet(L7_uint32 UnitIndex, L7_long32* localMed);

/*********************************************************************
*
* @purpose  Set the value of MED attribute
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    localMed    L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4LocalMedSet(L7_uint32 UnitIndex, L7_long32 localMed);

/*********************************************************************
*
* @purpose  Get the value of local preference attribute
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *localPref  L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4LocalPrefGet(L7_uint32 UnitIndex, L7_uint32* localPref);
/*********************************************************************
*
* @purpose  Set the value of local preference attribute
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    localPref   L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4LocalPrefSet(L7_uint32 UnitIndex, L7_uint32 localPref);
/*********************************************************************
*
* @purpose  Retrieve the Min. AS origination time interval
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *minAsOrigInterval     time interval in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4MinASOrigIntervalGet(L7_uint32 UnitIndex, L7_ushort16* minAsOrigInterval);

/*********************************************************************
*
* @purpose  Set the Min. AS origination time interval
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    minAsOrigInterval      time interval in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4MinASOrigIntervalSet(L7_uint32 UnitIndex, L7_uint32 minAsOrigInterval);

/*********************************************************************
* @purpose  Get the minimum  route advertisement interval
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *routeAdvInterval      (in seconds)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4MinRouteAdvIntervalGet(L7_uint32 UnitIndex, L7_ushort16* routeAdvInterval);

/*********************************************************************
* @purpose  Set the minimum  route advertisement interval
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    routeAdvInterval        (in seconds)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4MinRouteAdvIntervalSet(L7_uint32 UnitIndex, L7_uint32 routeAdvInterval);


/*********************************************************************
*
* @purpose  Get the length of next-hop address
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *nextHopAdrLen  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4NextHopAdrLenGet(L7_uint32 UnitIndex, L7_ushort16* nextHopAdrLen);

/*********************************************************************
*
* @purpose  Set the length of next-hop address
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    nextHopAdrLen   L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4NextHopAdrLenSet(L7_uint32 UnitIndex, L7_uint32 nextHopAdrLen);

/*********************************************************************
*
* @purpose  Add NLRI
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    prefix    L7_long32
* @param    prefixLen
* @param    remotePeerAddr
* @param    vpnCos
* @param    nextHop
* @param    sendNow
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4NlriAdd(L7_uint32 UnitIndex,L7_uint32 *index, L7_uint32 prefix, L7_ushort16 prefixLen,
                         L7_ulong32 vpnCos, L7_uint32 nextHop, L7_BOOL sendNow);

/*********************************************************************
*
* @purpose  Delete NLRI
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    prefix    L7_long32
* @param    prefixLen
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4NlriDelete(L7_uint32 UnitIndex, L7_uint32 prefix, L7_ushort16 prefixLen);

/*********************************************************************
*
* @purpose  Get NLRI entry based on its index.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    prefix    L7_long32
* @param    prefixLen
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4NlriGet(L7_uint32 UnitIndex, L7_uint32 index, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                         bgpIpAddr *nextHop, L7_ulong32 *vpnCosId, L7_BOOL *sendNow);

/*********************************************************************
*
* @purpose  Get NLRI Entries
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    prefix    L7_long32
* @param    prefixLen
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4NlriGetNext(L7_uint32 UnitIndex, L7_uint32 *index, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                             bgpIpAddr *nextHop, L7_ulong32 *vpnCosId, L7_BOOL *sendNow, bgpIpAddr *prevPrefix,
                             L7_ushort16 prevPrefixLen);

/*********************************************************************
*
* @purpose  Get the number of SNPAs present
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *numOfSnpa      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4NumOfSnpaGet(L7_uint32 UnitIndex, L7_ushort16* numOfSnpa);

/*********************************************************************
* @purpose  Get the optional capability bits in the bit mask
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *optionalCapSupp        (bit mask)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4OptionalCapSuppGet(L7_uint32 UnitIndex, L7_ushort16* optionalCapSupp);

/*********************************************************************
* @purpose  Set the optional capability bits in the bit mask
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    optionalCapSupp        (bit mask)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4OptionalCapSuppSet(L7_uint32 UnitIndex, L7_uint32 optionalCapSupp);

/*********************************************************************
*
* @purpose  Get the mode for path attribute aggregation
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_ENABLE  if path attribute aggregation is enabled
* @returns  L7_DISABLE if path attribute aggregation is disabled
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PathAttrAggregationModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode for path attribute aggregation
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode  L7_uint32 [L7_ENABLE or L7_DISABLE]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PathAttrAggregationModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the value of penalty increment
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *penaltyInc L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PenaltyIncGet(L7_uint32 UnitIndex, L7_uint32* penaltyInc);

/*********************************************************************
*
* @purpose  Set the value of penalty increment
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    penaltyInc    L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PenaltyIncSet(L7_uint32 UnitIndex, L7_uint32 penaltyInc);

/*********************************************************************
* @purpose  Get the Multi-Exit-Descriminator(MED) propagation mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *mode  @b{(output)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if not enabled
*
* @notes
*
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PropagateMedModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Set the Multi-Exit-Descriminator(MED) propagation mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode  L7_uint32 [L7_ENABLE or L7_DISABLE]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PropagateMedModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the value of reuse limit
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *reuseLimit L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ReuseLimitGet(L7_uint32 UnitIndex, L7_uint32* reuseLimit);

/*********************************************************************
*
* @purpose  Set the value of reuse limit
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    reuseLimit    L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ReuseLimitSet(L7_uint32 UnitIndex, L7_uint32 reuseLimit);

/*********************************************************************
*
* @purpose  Get the value of reuse size
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *reuseMaxSize L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ReuseMaxSizeGet(L7_uint32 UnitIndex, L7_uint32* reuseMaxSize);

/*********************************************************************
*
* @purpose  Set the value of reuse size
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    reuseMaxSize    L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ReuseMaxSizeSet(L7_uint32 UnitIndex, L7_uint32 reuseMaxSize);

/*********************************************************************
*
* @purpose  Get the value of local assosiated community
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *routeCommunity L7_ulong32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4RouteCommunityGet(L7_uint32 UnitIndex, L7_uint32* routeCommunity);

/*********************************************************************
*
* @purpose  Set the value of local assosiated community
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    routeCommunity    L7_ulong32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4RouteCommunitySet(L7_uint32 UnitIndex, L7_uint32 routeCommunity);

/*********************************************************************
* @purpose  Retrieve the mode for Route reflection
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode        @b{(output)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_TRUE  if enabled for Route Reflection
* @returns  L7_FALSE  if not enabled
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4RouteReflectModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Set the Router as a Route Reflector
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode [L7_ENABLE to enable and L7_DISABLE to diable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4RouteReflectModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get BGP Route Table Entry
*
* @param UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param *prefix         bgpIpAddr
* @param *prefixLen      L7_ushort16
* @param *nextHop        bgpIpAddr
* @param *vpnCosId       L7_ulong32
* @param *prevPrefix     bgpIpAddr
* @param prevPrefixLen   L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4RouteTableGetNext(L7_uint32 UnitIndex, bgpIpAddr *remotePeerAddr, bgpIpAddr *prefix,
                                   L7_ushort16 *prefixLen, bgpIpAddr *nextHop, L7_ulong32 *vpnCosId,
                                   bgpIpAddr *prevPrefix, L7_ushort16 prevPrefixLen);

/*********************************************************************
*
* @purpose  Add to the list of SNPAs
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *snpaAddr bgpIPAddr
* @param    snpaLen L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4SnpaAdd(L7_uint32 UnitIndex, L7_uint32 snpaAddr, L7_uint32 snpaLen);

/*********************************************************************
*
* @purpose  Delete from the list of SNPAs
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *snpaAddr bgpIPAddr
* @param    snpaLen L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4SnpaDelete(L7_uint32 UnitIndex, L7_uint32 snpaAddr, L7_uint32 snpaLen);

/*********************************************************************
*
* @purpose  Get the First Entry of the SNPA list
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    snpaEntry
* @param    prevSnpaEntry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4SnpaEntryGetNext(L7_uint32 UnitIndex, bgpSnpa_t* snpaEntry, bgpSnpa_t* prevSnpaEntry);

/*********************************************************************
*
* @purpose  Get the value of suppress limit
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *suppressLimit  L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4SuppressLimitGet(L7_uint32 UnitIndex, L7_uint32* suppressLimit);

/*********************************************************************
*
* @purpose  Set the value of suppress limit
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    suppressLimit   L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4SuppressLimitSet(L7_uint32 UnitIndex, L7_uint32 suppressLimit);

/*********************************************************************
*
* @purpose  Get the mode for less specific route selection
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode        @b{(output)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_ENABLE  if less specific route selection is enabled
* @returns  L7_DISABLE if it is disabled
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4SuppressModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode for less specific route selection
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode        @b{(input)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4SuppressModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Reset BGP
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ResetBgp(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose  Get the value of timerResolution
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *timerResolution  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4TimerResolutionGet(L7_uint32 UnitIndex, L7_uint32* timerResolution);

/*********************************************************************
*
* @purpose  Set the value of timerResolution
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    timerResolution   L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4TimerResolutionSet(L7_uint32 UnitIndex, L7_uint32 timerResolution);

/*********************************************************************
* @purpose  Get BGP Mapping Layer tracing mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode        @b{(output)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4TraceModeGet(L7_uint32 UnitIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Set BGP Mapping Layer tracing mode
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    mode        @b{(input)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4TraceModeSet(L7_uint32 UnitIndex, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Allows a user to know whether the specified trap
*           is enabled or disabled
*
* @param    unitIndex          Unit Index
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  BGP trap type specified in L7_BGP_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgpTrapModeGet(L7_uint32 unitIndex, L7_uint32 *mode, L7_uint32 trapType);

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    unitIndex          Unit Index
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  BGP trap type specified in L7_BGP_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgpTrapModeSet(L7_uint32 unitIndex, L7_uint32 mode, L7_uint32 trapType);

/****************************************************************************
* @purpose  Add VPN/COS ID
*
* @param    *unitIndex          @b{{input}} BGP IP address
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} BGP IP address
* @param    vpnCos              @b{{input}} VPN/COS
* @param    sendUpdate          @b{{input}} send update to BGP peers
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t usmDbBgp4VpncosIdAdd(L7_uint32 UnitIndex, L7_uint32 prefix, L7_ushort16 prefixLen,
                             L7_uint32 remotePeerAddr, L7_ulong32 vpnCos, L7_BOOL sendUpdate);

/****************************************************************************
* @purpose  Delete VPN/COS ID
*
* @param    UnitIndex           @b{{input}} unit index into policy list
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} BGP IP address
* @param    vpnCos              @b{{input}} VPN/COS
* @param    sendUpdate          @b{{input}} send update to BGP peers
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t usmDbBgp4VpncosIdDelete(L7_uint32 UnitIndex, L7_uint32 prefix, L7_ushort16 prefixLen,
                                L7_uint32 remotePeerAddr, L7_ulong32 vpnCos, L7_BOOL sendUpdate);

/*********************************************************************
*
* @purpose  Get the Next entry of the Address Family list for the specified peer
*
* @param    *remotePeerAddr     @b{{input}} BGP IP address
* @param    *addrEnrty          @b{{input}} BGP address entry
* @param    *prevAddrEntry      @b{{input}} previous BGP address entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerAddrFamilyEntryGetNext(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr,
                                            bgpAddrFamily_t* addrEntry, bgpAddrFamily_t* prevAddrEntry);

/*********************************************************************
* @purpose  Get the authentication params for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    *authCode
* @param    *authKeyStr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerAuthenticationGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* authCode,
                                       L7_uchar8* authKeyStr);

/*********************************************************************
* @purpose  Set the authentication params for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    *authCode
* @param    *authKeyStr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerAuthenticationSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 authCode,
                                       L7_uchar8* authKeyStr);

/*********************************************************************
*
* @purpose  Get the value of the confederation member flag for the specified peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    mode
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerConfedMemberFlagGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the value of the confederation member flag for the specified peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    mode    L7_BOOL [ L7_TRUE to set as a confed member and
*     L7_FALSE to indicate the peer is not a confed member
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerConfedMemberFlagSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Add the AFI, SAFI to the list for the specified peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    afi L7_ushort16
* @param    safi L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerConfigAddrFamily(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 afi,
                                      L7_ushort16 safi);
/*********************************************************************
* @purpose  Configure the peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    remoteAS     L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Terminates the conn. and deletes its config. from the file
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerConfigurePeer(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr,
                                   L7_ushort16 remoteAs);

/*********************************************************************
*
* @purpose  Delete the AFI, SAFI from the list for the specified peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    afi L7_ushort16
* @param    safi L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerDeleteAddrFamily(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 afi,
                                      L7_ushort16 safi);
/*********************************************************************
* @purpose  Delete the peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *IpAddr   bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Terminates the conn. and deletes its config. from the file
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerDeletePeer(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr);

/*********************************************************************
* @purpose  Get the next peer identifier
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *IpAddr   bgpIpAddr
* @param    *nextIpAddr   bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4RemotePeerIdGetNext(L7_uint32 UnitIndex, bgpIpAddr* presentPeerId, bgpIpAddr* previousPeerId);

/*********************************************************************
* @purpose  Get the local interface address of the specified Bgp Peer
*   that will be used as Next hop address
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    localIfAddr bgpIpAddr peer's local interface IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerLocalIfAddressGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32* localIfAddr);


/*********************************************************************
* @purpose  Set the local interface address of the specified Bgp Peer
*   that will be used as Next hop address. THIS FUNCTION IS
*       DEPRECATED. THE LOCAL ADDRESS IS NOW INFERRED FROM THE REMOTE
*       ADDRESS.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    *localIfAddr    bgpIpAddr peer's local interface IP Address
*
* @returns  L7_FAILURE
*
* @notes    DEPRECATED
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PeerLocalIfAddressSet(L7_uint32 UnitIndex,
                                       L7_uint32 remotePeerAddr,
                                       L7_uint32 localIfAddr);

/*********************************************************************
* @purpose  Get the message send limit for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    *msgSendLimit      L7_ushort16  message send limit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerMsgSendLimitGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* msgSendLimit);

/*********************************************************************
* @purpose  Set the message send limit for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    msgSendLimit    L7_ushort16 message send limit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerMsgSendLimitSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 msgSendLimit);

/*********************************************************************
*
* @purpose  Get the value of the NotNextHopSelf flag for the specified peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    mode  L7_uint32
*
* @returns  L7_TRUE  if notNextHopSelf's value is TRUE
* @returns  L7_FALSE  if not
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerNotNextHopSelfModeGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the value of the NotNextHopSelf flag for the specified peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    mode     L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerNotNextHopSelfModeSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the optional capabilities enabled for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    *option      L7_ushort16  capabilities bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerOptionalCapGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* option);

/*********************************************************************
* @purpose  Set the optional capabilities for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    option    L7_ushort16 capabilities bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerOptionalCapModeSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 option);

/*********************************************************************
* @purpose  Set the remote AS number of the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    remoteAS    L7_ushort16 remote AS number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerRemoteAsSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 remoteAs);

/*********************************************************************
*
* @purpose  See if the specified peer is a Route Reflector Client
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    mode        @b{(output)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_ENABLE  if specified peer is enabled as Route Reflector Client
* @returns  L7_DISABLE if is disabled
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerRouteReflectorModeGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the specified peer as a Route Reflector Client or Non-Client
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    mode        @b{(input)} value is either L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerRouteReflectorModeSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16 mode);

/*********************************************************************
* @purpose  Get the transmission delay interval for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    *txDelayInt      L7_ushort16  transmission delay interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerTxDelayIntGet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_ushort16* txDelayInt);

/*********************************************************************
* @purpose  Set the transmission delay interval for the specified Bgp Peer
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *remotePeerAddr
* @param    txDelayInt    L7_ushort16 transmission delay interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PeerTxDelayIntSet(L7_uint32 UnitIndex, L7_uint32 remotePeerAddr, L7_uint32 txDelayInt);

/*********************************************************************
* @purpose  Get whether the router is configured to only log a warning, rather
*           than shutdown the peer, when the number of prefixes learned
*           from the peer exceeds a configured limit.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    remoteAddr @b{(input)}  IP address of peer
* @param    warnOnly   @b{(output)}  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no peer at this address
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PeerPfxWarnOnlyGet(L7_uint32 UnitIndex, 
                                    L7_uint32 remotePeerAddr, 
                                    L7_BOOL *warnOnly);

/*********************************************************************
* @purpose  Set whether the router is configured to only log a warning, rather
*           than shutdown the peer, when the number of prefixes learned
*           from the peer exceeds a configured limit.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    remoteAddr @b{(input)}  IP address of peer
* @param    warnOnly   @b{(input)}  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no peer at this address
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PeerPfxWarnOnlySet(L7_uint32 UnitIndex, 
                                    L7_uint32 remotePeerAddr, 
                                    L7_BOOL warnOnly);

/*********************************************************************
* @purpose  Get the maximum number of prefixes the router is configured 
*           to learn from a given peer.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    remoteAddr @b{(input)}  IP address of peer
* @param    pfxLimit   @b{(output)} max number of prefixes allowed
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no peer at this address
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PeerPfxLimitGet(L7_uint32 UnitIndex, 
                                 L7_uint32 remotePeerAddr, 
                                 L7_uint32 *pfxLimit);

/*********************************************************************
* @purpose  Set the maximum number of prefixes the router is configured 
*           to learn from a given peer.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    remoteAddr @b{(input)}  IP address of peer
* @param    pfxLimit   @b{(input)}  max prefixes allowed
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no peer at this address
*
* @notes    If the router learns more than this number of prefixes from
*           this peer, the peer will be administratively disabled, unless
*           the router is configured to only warn of this condition.
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PeerPfxLimitSet(L7_uint32 UnitIndex, 
                                 L7_uint32 remotePeerAddr, 
                                 L7_uint32 pfxLimit);

 /*********************************************************************
* @purpose  Get the percentage of the prefix limit at which a warning
*           should be logged.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    remoteAddr @b{(input)}  IP address of peer
* @param    pfxThresh  @b{(output)} percentage at which to warn
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no peer at this address
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PeerPfxWarnThreshGet(L7_uint32 UnitIndex, 
                                      L7_uint32 remotePeerAddr, 
                                      L7_uint32 *pfxThresh);

/*********************************************************************
* @purpose  Set the percentage of the prefix limit at which a warning
*           should be logged.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    remoteAddr @b{(input)}  IP address of peer
* @param    pfxLimit   @b{(input)}  max prefixes allowed
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no peer at this address
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PeerPfxWarnThreshSet(L7_uint32 UnitIndex, 
                                      L7_uint32 remotePeerAddr, 
                                      L7_uint32 pfxThresh);

/*********************************************************************
*
* @purpose  Set the Unfeasible Advt. flag of the specified prefix
*
* @param    *UnitIndex             @b{{input}} BGP IP address
* @param    *prefix                @b{{input}} prefix
* @param    *prefixLen             @b{{input}} prfix lenght
* @param    *remotePeerAddr        @b{{input}} remote Peer IP address
* @param    *prevRemoteAddr        @b{{input}} previous remote addess
* @param    *pathAttrEntry         @b{{input}} BGP path attribute entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4PathAttrGetNext(L7_uint32 UnitIndex, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                                 bgpIpAddr *remoteAddr, bgpIpAddr *prevRemoteAddr, bgpMapPathAttribute_t
                                 *pathAttrEntry);

/*********************************************************************
*
* @purpose  Set the Unfeasible Advt. flag of the specified prefix
*
* @param    *UnitIndex             @b{{input}} BGP IP address
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t usmDbBgp4ClearFlapStats(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose  Create policies
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    protocol
* @param    matchType
* @param    numOfRanges
* @param    numOfActions
* @param    poilicies
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4PolicyCreate(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 policyAccess,
                              L7_ushort16 protocol, L7_ushort16 matchType);

/**************************************************************************
*
* @purpose  Sets address range for a specified policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    ipAddr          Indicates the IP address for the range
* @param    mask            Specifies the Mask to be assocaited to the range.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeAddressSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 ipAddr,
                                       L7_uint32 mask);

/**************************************************************************
*
* @purpose  gets address range for a specified policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    ipAddr          Ptr to the IP address for the range
* @param    mask            Ptr to the Mask to be assocaited to the range.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeAddressGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *ipAddr,
                                       L7_uint32 *mask);


/**************************************************************************
*
* @purpose  Adds a Range policy of "between type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    Minval          Indicates the Minimal value for the range
* @param    Maxval          Indicates the Maximum value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4PolicyRangeBetweenSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 minVal,
                                       L7_uint32 maxVal);

/**************************************************************************
*
* @purpose  Gets a Range policy of "between type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    Minval          Ptr to the Minimal value for the range
* @param    Maxval          Ptr to the Maximum value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4PolicyRangeBetweenGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *minVal,
                                       L7_uint32 *maxVal);

/**************************************************************************
*
* @purpose  Adds a Range policy of "greaterthan type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    val             Indicates the Val value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeGreaterthanSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 val);

/**************************************************************************
*
* @purpose  Gets a Range policy of "greaterthan type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    val             Ptr to the Val value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeGreaterthanGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *val);

/**************************************************************************
*
* @purpose  Adds a Range policy of "lessthan type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    val             Indicates the Minimal value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeLessthanSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 val);

/**************************************************************************
*
* @purpose  Gets a Range policy of "lessthan type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    val             Ptr to the Minimal value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeLessthanGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *val);

/**************************************************************************
*
* @purpose  Adds a Range policy of "equal type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    val             Indicates the value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeEqualSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 val);

/**************************************************************************
*
* @purpose  Gets a Range policy of "equal type" to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    val             Ptr to the value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeEqualGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *val);

/**************************************************************************
 *
 * @purpose      Gets the next Range policy of "equal type" from the Policy Access List
 *
 * @param    UnitInidex      Specifies unit index
 * @param    policyIndex     Indicates the Index in the Access List
 * @param    val             Ptr to the value for the "previous" value and
 *                           also used for return value of "next".
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @end
 *************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeEqualGetNext(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *val);

/**************************************************************************
*
* @purpose  Adds a Range policy of "match" type to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    valstr          Indicates the value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeMatchSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_char8 *valstr);

/**************************************************************************
*
* @purpose  Gets a Range policy of "match" type to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    valstr          Ptr to the value for the range
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeMatchGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_char8 *valstr);

/**************************************************************************
 *
 * @purpose      Gets the next Range policy of "match type" from the Policy Access List
 *
 * @param    UnitInidex      Specifies unit index
 * @param    policyIndex     Indicates the Index in the Access List
 * @param    val             Ptr to the value for the "previous" value and
 *                           also used for return value of "next".
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @end
 *************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeMatchGetNext(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_char8 *val);

/**************************************************************************
*
* @purpose  Removes a Range from the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    matchIndex      Indicates the attribute type the range to be applied to.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRangeRemove(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 matchType);

/**************************************************************************
*
* @purpose  add/Modify a Action to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    matchType       Indicates the attribute type the action to be applied to.
* @param    modifier        Indicates whether it is Add/Modify/Remove action.
* @param    val             Indicates the value for the action.
* @param    val2            Indicates the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyActionAdd(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 matchType,
                                 L7_uint32 modifier, L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  Gets First Action for the specified poilicy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    prevIndex       Ptr to the prevous index
* @param    matchType       Ptr to the attribute type the action to be applied to.
* @param    modifier        Ptr to whether it is Add/Modify/Remove action.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyActionGetFirst(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *prevIndex,
                                      L7_uint32 *matchType, L7_uint32 *modifier, L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Get Next Action for the specified policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    prevIndex       Ptr to the prevous index
* @param    matchType       Ptr to attribute type the action to be applied to.
* @param    modifier        Ptr to whether it is Add/Modify/Remove action.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyActionGetNext(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *prevIndex,
                                     L7_uint32 *matchType, L7_uint32 *modifier, L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Get Exact Action for the specified policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    index           Ptr to the prevous index
* @param    matchType       Indicates the attribute type the action to be applied to.
* @param    modifier        Indicates whether it is Add/Modify/Remove action.
* @param    val             Indicates the value for the action.
* @param    val2            Indicates the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyActionGetExact(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *index,
                                      L7_uint32 *matchType, L7_uint32 *modifier, L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Removes a Action to the Policy Access List
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    matchIndex      Indicates the attribute type the action to be applied to.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyActionRemove(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 matchType);

/**************************************************************************
*
* @purpose  Deletes the Policy Access Object
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4PolicyDeleteIndex(L7_uint32 UnitIndex, L7_uint32 policyIndex);

/**************************************************************************
*
* @purpose  Gets next policy. If prevIndex is zero it gets first policy in the list.
*
* @param    UnitInidex      Specifies unit index
* @param    ptoto           Ptr to protocol
* @param    index           Ptr to indicates the Index in the Access List
* @param    matchType       Ptr to policy match type
* @param    permitFlasg     Ptr to permisstion flags
* @param    numRanges       Ptr to number of ranges
* @param    numActions      Ptr to number of actions
* @param    numRangeArgs    Ptr to number of arguments for a range of policy
* @param    numActionArgs   Ptr to number of arguments for a action of policy
* @param    rangeArgs       Ptr to range arguments
* @param    actionArgs      Ptr to range arguments
* @param    prevIndex       Ptr to prevIndex which will tell whether first index or next index
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4PolicyGetNext(L7_uint32 UnitIndex, L7_ushort16 *proto, L7_uint32 *index,
                                 L7_ushort16 *matchType, L7_BOOL *permitFlag,
                                 L7_uint32 *numRanges, L7_uint32 *numActions,
                                 bgpPolicyRange_t *ranges, bgpPolicyAction_t *actions, L7_uint32 prevIndex);

/**************************************************************************
*
* @purpose  Gets Exact policy.
*
* @param    UnitInidex      Specifies unit index
* @param    ptoto           Ptr to protocol
* @param    index           Ptr to indicates the Index in the Access List
* @param    matchType       Ptr to policy match type
* @param    permitFlasg     Ptr to permisstion flags
* @param    numRanges       Ptr to number of ranges
* @param    numActions      Ptr to number of actions
* @param    numRangeArgs    Ptr to number of arguments for a range of policy
* @param    numActionArgs   Ptr to number of arguments for a action of policy
* @param    rangeArgs       Ptr to range arguments
* @param    actionArgs      Ptr to range arguments
* @param    curIndex       Ptr to prevIndex which will tell whether first index or next index
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4PolicyGetExact(L7_uint32 UnitIndex, L7_ushort16 *proto, L7_uint32 policyIndex,
                                L7_ushort16 *matchType, L7_BOOL *permitFlag,
                                L7_uint32 *numRanges, L7_uint32 *numActions,
                                bgpPolicyRange_t *ranges, bgpPolicyAction_t *actions);

/**************************************************************************
*
* @purpose  Gets next policy index. If prevIndex is zero it gets first policy in the list.
*
* @param    UnitInidex      Specifies unit index
* @param    index           Ptr to indicates the Index in the Access List
* @param    prevIndex       previous index in the list
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyIndexGetNext(L7_uint32 UnitIndex, L7_uint32 *index, L7_uint32 prevIndex);

/**************************************************************************
*
* @purpose  Gets Exact poilicy Index.
*
* @param    UnitInidex      Specifies unit index
* @param    index           Ptr to indicates the Index in the Access List
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4PolicyIndexGetExact(L7_uint32 UnitIndex, L7_uint32 *index);

/**************************************************************************
*
* @purpose  gets AS path action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAsPathActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                       L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  stes AS path action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAsPathActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                       L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets origin action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyOriginActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                       L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets origin action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyOriginActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                       L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Local Preference Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyLocalPreferenceActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                                L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Local Preference Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyLocalPreferenceActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                                L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Multi Exit Disc Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyMultiExitDiscActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                              L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Multi Exit Disc Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyMultiExitDiscActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                              L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Communinity Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyCommunityActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                          L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  sets Communinity Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyCommunityActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                          L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets ConfederationID Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyConfederationIDActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                                L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets ConfederationID Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyConfederationIDActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                                L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Cluster Number Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyClusterNumberActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                              L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Cluster Number Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyClusterNumberActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                              L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets flowspec Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyFlowspecActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                         L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets flowspec Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyFlowspecActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                         L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Dest IP Pref Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyDestIPPrefActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                           L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Dest IP Pref Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyDestIPPrefActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                           L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Next Hop Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyNextHopActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                        L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Next Hop Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyNextHopActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                        L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Max Setup Priority Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyMaxSetupPrioActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                             L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Max Setup Priority Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyMaxSetupPrioActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                             L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Policy Max Hold Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyMaxHoldPrioActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                            L7_uint32 *val, L7_int32 *val2);


/**************************************************************************
*
* @purpose  Sets Policy Max Hold Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyMaxHoldPrioActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                            L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Policy String Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyPolicyStringActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                             L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Policy String Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyPolicyStringActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                             L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Policy user Define Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyUserDefineActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                           L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Policy user Define Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyUserDefineActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                           L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets AS Path Len Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAsPathLenActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                          L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets AS Path Len Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAsPathLenActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                          L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Protocol ID Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyProtocolIDActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                           L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Protocol ID Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyProtocolIDActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                           L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets OSPF Destination Type Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyOspfDestTypeActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                             L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  sets OSPF Destination Type Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyOspfDestTypeActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                             L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Peer Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyPeerActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                     L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Sets Peer Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyPeerActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                     L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Atomic Aggrigate Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAtomicAggregateActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex,
                                                L7_uint32 *modifier, L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Set Atomic Aggrigate Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAtomicAggregateActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                                L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Aggrigator AS Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAggregatorAsActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                             L7_uint32 *val, L7_int32 *val2);


/**************************************************************************
*
* @purpose  Sets Aggrigator AS Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAggregatorAsActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                             L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Aggrigator ID Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAggregatorIDActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                             L7_uint32 *val, L7_int32 *val2);


/**************************************************************************
*
* @purpose  Sets Aggrigator ID Action's attributes for a specifid policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyAggregatorIDActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                             L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  gets Route ACL Action's attributes for a specified policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRouteAclActionGet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 *modifier,
                                             L7_uint32 *val, L7_int32 *val2);


/**************************************************************************
*
* @purpose  Sets Route ACL Action's attributes for a specified policy
*
* @param    UnitInidex      Specifies unit index
* @param    policyIndex     Indicates the Index in the Access List
* @param    modifier        Ptr to modifier.
* @param    val             Ptr to the value for the action.
* @param    val2            Ptr to the optional value for the action.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/

L7_RC_t usmDbBgp4PolicyRouteAclActionSet(L7_uint32 UnitIndex, L7_uint32 policyIndex, L7_uint32 modifier,
                                             L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  Gets Exact poilicy Index.
*
* @param    UnitInidex      Specifies unit index
* @param    index           Ptr to indicates the Index in the Access List
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4PolicyIndexGetExact(L7_uint32 UnitIndex, L7_uint32 *index);

/**************************************************************************
* @purpose  Specify whether BGP is allowed to redistribute a default route.
*
* @param    unitIndex - unit to which this command applies
* @param    val - TRUE if BGP can redistribute the default route.
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistDefRouteSet(L7_uint32 unitIndex, L7_BOOL val);

/**************************************************************************
* @purpose  Get whether BGP is allowed to redistribute a default route.
*
* @param    unitIndex - unit to which this command applies
* @param    val - TRUE if BGP can redistribute the default route.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if val is NULL
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistDefRouteGet(L7_uint32 unitIndex, L7_BOOL *val);

/**************************************************************************
* @purpose  Set a default metric for BGP to use as the MED when it
*           redistributes routes from another protocol.
*
* @param    unitIndex - unit to which this command applies
* @param    defMetric - The default metric.
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4DefaultMetricSet(L7_uint32 unitIndex, L7_uint32 defMetric);

/**************************************************************************
* @purpose  Clear the default metric that BGP uses as the MED when it
*           redistributes routes from another protocol.
*
* @param    unitIndex - unit to which this command applies
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4DefaultMetricClear(L7_uint32 unitIndex);

/**************************************************************************
* @purpose  Get the default metric that BGP uses as the MED when it
*           redistributes routes from another protocol.
*
* @param    unitIndex - unit to which this command applies
* @param    defMetric - return value. Only valid if L7_SUCCESS returned.
*
* @returns  L7_SUCCESS if successful.
* @returns  L7_FAILURE if defMetric is NULL
* @returns  L7_NOT_EXIST if no default metric is configured.
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4DefaultMetricGet(L7_uint32 unitIndex, L7_uint32 *defMetric);

/**************************************************************************
* @purpose  Set route redistribution options for routes learned from
*           a specific source.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistribute - Set to true to allow BGP to redistribute routes
*                          from sourceProto
* @param    distList - Number of access list to use to filter routes from
*                      sourceProto
* @param    redistMetric - BGP will set the MED to this value for routes
*                          it redistributes from sourceProto
* @param    redistMetConfigured - Set to false if user has not configured
*                                 a redistribution metric. redistMetric is
*                                 ignored in this case.
* @param    matchInternal - true if BGP may redistribute OSPF internal routes
* @param    matchExtType1 - true if BGP may redistribute OSPF external type 1
*                           routes
* @param    matchExtType2 - true if BGP may redistribute OSPF external type 2
*                           routes
* @param    matchNssaExtType1 - true if BGP may redistribute OSPF NSSA external
*                               type 1 routes
* @param    matchNssaExtType2 - true if BGP may redistribute OSPF NSSA external
*                               type 2 routes
*
* @returns  L7_SUCCESS if configuration successfully saved
* @returns  L7_FAILURE if sourceProto is invalid
*
* @notes    The match arguments are ignored if sourceProto is not L7_IPRT_OSPF.
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistributionSet(L7_uint32 unitIndex,
                                   L7_REDIST_RT_INDICES_t sourceProto,
                                   L7_BOOL redistribute,
                                   L7_uint32 distList,
                                   L7_uint32 redistMetric,
                                   L7_BOOL redistMetConfigured,
                                   L7_BOOL matchInternal,
                                   L7_BOOL matchExtType1,
                                   L7_BOOL matchExtType2,
                                   L7_BOOL matchNssaExtType1,
                                   L7_BOOL matchNssaExtType2);

/**************************************************************************
* @purpose  Specify whether BGP redistributes routes from a given source.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistribute - true if BGP should redistribute from sourceProto.
*
* @returns  L7_SUCCESS if configuration successfully saved
* @returns  L7_FAILURE if sourceProto is invalid
*
* @notes    Setting redistribute to L7_FALSE does not affect the value of
*           any other redistribution parameters.
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistributeSet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL redistribute);

/**************************************************************************
* @purpose  Get whether BGP redistributes routes from a given source.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistribute - set to true if BGP redistributes from sourceProto.
*
* @returns  L7_SUCCESS if successful if successful
* @returns  L7_FAILURE if sourceProto is invalid or redistribute is NULL
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistributeGet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL *redistribute);

/**************************************************************************
* @purpose  For a given source of redistributed routes, revert the
*           redistribution configuration to the defaults.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistribute - set to true if BGP redistributes from sourceProto.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid
*
* @notes    Defaults as follows:
*              redistribute: FALSE
*              distribute list: 0
*              metric: not configured
*              matchInternal: TRUE
*              matchExtType1: FALSE
*              matchExtType2: FALSE
*              matchNssaExtType1: FALSE
*              matchNssaExtType2: FALSE
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistributeRevert(L7_uint32 unitIndex,
                                    L7_REDIST_RT_INDICES_t sourceProto);

/**************************************************************************
* @purpose  Specify an access used to filter routes when BGP redistributes
*           routes from a given source.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    distList - Number of access list to use to filter routes from
*                      sourceProto
*
* @returns  L7_SUCCESS if configuration successfully saved
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistributeDistListSet(L7_uint32 unitIndex,
                                         L7_REDIST_RT_INDICES_t sourceProto,
                                         L7_uint32 distList);

/**************************************************************************
* @purpose  No longer use an access list to filter routes when BGP redistributes
*           routes from a given source.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistributeDistListClear(L7_uint32 unitIndex,
                                           L7_REDIST_RT_INDICES_t sourceProto);

/**************************************************************************
* @purpose  Get number of the access list used to filter routes when BGP
*           redistributes routes from a given source.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    distList - Set to the number of the access list to use to
*                      filter routes from sourceProto
*
* @returns  L7_SUCCESS if distList is successfully set.
* @returns  L7_FAILURE if sourceProto is invalid or distList is NULL
* @returns  L7_NOT_EXIST if no distribute list is configured.
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistributeDistListGet(L7_uint32 unitIndex,
                                         L7_REDIST_RT_INDICES_t sourceProto,
                                         L7_uint32 *distList);

/**************************************************************************
* @purpose  Set redistribution metric.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistMetric - BGP will set the MED to this value for routes
*                          it redistributes from sourceProto
*
* @returns  L7_SUCCESS if configuration successfully saved
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistMetricSet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_uint32 redistMetric);

/**************************************************************************
* @purpose  Clear the redistribution metric.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistMetricClear(L7_uint32 unitIndex,
                                   L7_REDIST_RT_INDICES_t sourceProto);

/**************************************************************************
* @purpose  Get redistribution metric.
*
* @param    unitIndex - unit to which this command applies
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistMetric - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid
* @returns  L7_NOT_EXIST if no redistribution metric is set
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistMetricGet(L7_uint32 unitIndex,
                                 L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_uint32 *redistMetric);

/**************************************************************************
* @purpose  Specify the types of OSPF routes BGP will redistribute.
*
* @param    unitIndex - unit to which this command applies
* @param    matchInternal - true if BGP may redistribute OSPF internal routes
* @param    matchExtType1 - true if BGP may redistribute OSPF external type 1
*                           routes
* @param    matchExtType2 - true if BGP may redistribute OSPF external type 2
*                           routes
* @param    matchNssaExtType1 - true if BGP may redistribute OSPF NSSA external
*                               type 1 routes
* @param    matchNssaExtType2 - true if BGP may redistribute OSPF NSSA external
*                               type 2 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistOspfTypesSet(L7_uint32 unitIndex,
                                    L7_BOOL matchInternal,
                                    L7_BOOL matchExtType1,
                                    L7_BOOL matchExtType2,
                                    L7_BOOL matchNssaExtType1,
                                    L7_BOOL matchNssaExtType2);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF internal routes.
*
* @param    unitIndex - unit to which this command applies
* @param    matchInternal - true if BGP may redistribute OSPF internal routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4MatchOspfInternalSet(L7_uint32 unitIndex,
                                      L7_BOOL matchInternal);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF external type 1 routes.
*
* @param    unitIndex - unit to which this command applies
* @param    matchExtType1 - true if BGP may redistribute OSPF ext type 1 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4MatchOspfExt1Set(L7_uint32 unitIndex, L7_BOOL matchExtType1);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF external type 2 routes.
*
* @param    unitIndex - unit to which this command applies
* @param    matchExtType2 - true if BGP may redistribute OSPF ext type 2 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4MatchOspfExt2Set(L7_uint32 unitIndex, L7_BOOL matchExtType2);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF NSSA external
*           type 1 routes.
*
* @param    unitIndex - unit to which this command applies
* @param    matchNssaExt1 - true if BGP may redistribute OSPF NSSA
*                           external type 1 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4MatchOspfNssaExt1Set(L7_uint32 unitIndex,
                                      L7_BOOL matchNssaExt1);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF NSSA external
*           type 2 routes.
*
* @param    unitIndex - unit to which this command applies
* @param    matchNssaExt2 - true if BGP may redistribute OSPF NSSA
*                           external type 2 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4MatchOspfNssaExt2Set(L7_uint32 unitIndex,
                                      L7_BOOL matchNssaExt2);

/**************************************************************************
* @purpose  Get the types of OSPF routes BGP will redistribute.
*
* @param    unitIndex - unit to which this command applies
* @param    matchInternal - true if BGP redistributes OSPF internal routes
* @param    matchExtType1 - true if BGP redistributes OSPF external type 1
*                           routes
* @param    matchExtType2 - true if BGP redistributes OSPF external type 2
*                           routes
* @param    matchNssaExtType1 - true if BGP redistributes OSPF NSSA external
*                               type 1 routes
* @param    matchNssaExtType2 - true if BGP redistributes OSPF NSSA external
*                               type 2 routes
*
* @returns  L7_SUCCESS
*
* @comments If an argument is NULL, that argument is skipped.
*
* @end
*************************************************************************/
L7_RC_t usmDbBgp4RedistOspfTypesGet(L7_uint32 unitIndex,
                                    L7_BOOL *matchInternal,
                                    L7_BOOL *matchExtType1,
                                    L7_BOOL *matchExtType2,
                                    L7_BOOL *matchNssaExtType1,
                                    L7_BOOL *matchNssaExtType2);

/*********************************************************************
* @purpose  Indicates whether a sourceIndex is a valid
*           source from which BGP may redistribute.
*
* @param    unitIndex - the unit for this operation
* @param    sourceIndex - A source for redistributed routes.
*
* @returns  L7_SUCCESS if sourceIndex corresponds to a valid
*                      source of routes for BGP to redistribute.
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4RouteRedistributeGet(L7_uint32 unitIndex,
                                      L7_REDIST_RT_INDICES_t sourceIndex);

/*********************************************************************
* @purpose  Given a route redistribution source, set nextSourceIndex
*           to the next valid source of redistributed routes.
*
* @param    unitIndex - the unit for this operation
* @param    sourceIndex - Corresponds to a source for redistributed routes.
* @param    nextSourceIndex - The source index following sourceIndex.
*
* @returns  L7_SUCCESS if there is a valid nextSourceIndex
* @returns  L7_FAILURE otherwise
*
* @notes    Intended for use with SNMP walk.
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4RouteRedistributeGetNext(L7_uint32 unitIndex,
                                          L7_REDIST_RT_INDICES_t sourceIndex,
                                          L7_REDIST_RT_INDICES_t *nextSourceIndex);

/*********************************************************************
* @purpose  Convert a BGP community number to a string.
*
* @param    unit - the unit for this operation
* @param    community - A BGP community value
* @param    commStr - The community value converted to a string. This 
*                     must be a buffer of at least L7_BGP_MAX_COMM_STR_LEN
*                     characters.
*
* @returns  L7_SUCCESS 
*
* @notes    For well-known communities, returns the name of the community.
*           Generates at most L7_BGP_MAX_COMM_STR_LEN characters.
*
* @end
*********************************************************************/
L7_RC_t usmDbBgp4CommunityString(L7_uint32 unit, L7_uint32 community, 
                                 L7_uchar8 *commStr);

/*********************************************************************
* @purpose  Turns on/off the displaying of Bgp packet debug info
*            
* @param    flag         new value of the Packet Debug flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbBgpPacketDebugTraceFlagSet(L7_BOOL flag);

/*********************************************************************
* @purpose  Returns the BGP packet debug info
*            
* @param    
*
* @returns  Trace Flag
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL usmDbBgpPacketDebugTraceFlagGet();


#endif
