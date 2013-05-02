/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  l7_bgp_api.h
*,
* @purpose   Bgp General Api functions
*
* @component Bgp Mapping Layer
*
* @comments  This file includes getters & setters for general bgp
*            variables, which are global to the bgp process
*
* @create    1/9/2002
*
* @author    Anand SSV
*
* @end
*
**********************************************************************/

#ifndef L7_BGP_API_H
#define L7_BGP_API_H

#include "datatypes.h"
#include "commdefs.h"
#include "l3_commdefs.h"
#include "l3_bgp_comm_structs.h"
#include "osapi.h"
#include "async_event_api.h"

#define L7_BGP_VERSION 4

#define L7_BGP_MAX_COMM_STR_LEN  30

/*
***********************************************************************
*                     API FUNCTIONS  -  BGP UTILITY FUNCTIONS
***********************************************************************
*/

/*********************************************************************
* @purpose  This routine is called if an interface is enabled for
* @purpose  for routing.
*
* @param    intIfnum    internal interface number
* @param    state    @b{(input)} an event listed in L7_RTR_EVENT_CHANGE_t
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t bgpMapRoutingEventChangeCallBack (L7_uint32 intIfNum,
                                          L7_uint32 event,
                                          void *pData,
                                          ASYNC_EVENT_NOTIFY_INFO_t *response);

/*********************************************************************
* @purpose  Set BGP Mapping Layer tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapTraceModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    void
*
* @returns    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 bgpMapTraceModeGet();

/*********************************************************************
* @purpose  Determine if the BGP component has been initialized
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL bgpMapBgpInitialized(void);

/*
***********************************************************************
*                     API FUNCTIONS  -  GLOBAL API
***********************************************************************
*/

/*********************************************************************
*
* @purpose  Reset BGP
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpReset(void);

/*********************************************************************
*
* @purpose  Get BGP Route Table Entry
*
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
L7_RC_t bgpMapBgpRouteTableEntryGetNext(bgpIpAddr *peerId, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                             bgpIpAddr *nextHop, L7_ulong32 *vpnCosId,
                             bgpIpAddr *prevPrefix, L7_ushort16 prevPrefixLen);


/*
**********************************************************************
*                    API FUNCTIONS  -  BGP CONFIG
**********************************************************************
*/


/*********************************************************************
* @purpose  Gets the Bgp Admin Mode
*
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
L7_RC_t bgpMapBgpAdminModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the Bgp Admin mode
*
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
L7_RC_t bgpMapBgpAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the current version number of the BGP protocol.
*
* @param    *version        version number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Currently this is set to 4
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpVersionNumberGet ( L7_ushort16 *version );

/*********************************************************************
* @purpose  Sets the local Autonomous System Number
*
* @param    AS number  @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes
*
*
*
*
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLocalASSet(L7_ushort16 localASnumber);

/*********************************************************************
* @purpose  Gets the Local AS number.
*
* @param    *localASnumber
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLocalASGet(L7_ushort16 *localASnumber);

/*********************************************************************
* @purpose  Delete the local Autonomous System Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLocalASDelete(void);

/*********************************************************************
*
* @purpose  Get the Local Identifier of the Bgp Router
*
* @param    *localId
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLocalIdGet(bgpIpAddr *localId);


/*********************************************************************
* @purpose  Set the Local Identifier of the Bgp Router.
*
* @param    *localId     value
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLocalIdSet ( bgpIpAddr *localId );

/*********************************************************************
* @purpose  Get the Multi-Exit-Descriminator(MED) propagation mode
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if not enabled
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPropagateMEDModeGet (L7_uint32* mode );

/*********************************************************************
* @purpose  Set the Multi-Exit-Descriminator(MED) propagation mode
*
* @param    medMode [L7_ENABLE to to enable and L7_DISABLE to disable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPropagateMEDModeSet (L7_uint32 mode);

/*********************************************************************
* @purpose  Get mode value which decides to use or not use MED value
*
*
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if disabled
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpCalcMEDModeGet(L7_uint32* mode );

/*********************************************************************
* @purpose  Set mode value which decides to use or not use MED value
*
* @param    calcMedMode  [L7_ENABLE to to enable and L7_DISABLE to disable]
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
L7_RC_t bgpMapBgpCalcMEDModeSet(L7_uint32 calcMedMode);

/*********************************************************************
* @purpose  Get the mode for external border router advertisement
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if not enabled
*
* @notes
*
*
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpDisAdExtBdrRtrModeGet (L7_uint32* mode);

/*********************************************************************
* @purpose  Set the mode for external border router advertisement
*
* @param    extBdrRtrAdvtMode [L7_ENABLE to enable and L7_DISABLE to disable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
*
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpDisAdExtBdrRtrModeSet(L7_uint32 extBdrRtrAdvtMode);

/*********************************************************************
*
* @purpose  Retrieve the Min. AS origination time interval
*
* @param    *minAsOrigInterval     time interval in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpMinASOrigIntervalGet(L7_ushort16 *minAsOrigInterval);

/*********************************************************************
*
* @purpose  Set the Min. AS origination time interval
*
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
L7_RC_t bgpMapBgpMinASOrigIntervalSet(L7_ushort16 minAsOrigInterval);

/*********************************************************************
* @purpose  Get the minimum  route advertisement interval
*
* @param    *minRouteAdInterval      (in seconds)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpMinRouteAdIntervalGet (L7_ushort16 *minRouteAdInterval );

/*********************************************************************
* @purpose  Set the minimum  route advertisement interval
*
* @param    minRouteAdInterval        (in seconds)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpMinRouteAdIntervalSet (L7_ushort16 minRouteAdInterval );

/*********************************************************************
* @purpose  Get the optional capability bits in the bit mask
*
* @param    *optCap        (bit mask)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpOptCapSupportGet (L7_ushort16 *optCap );

/*********************************************************************
* @purpose  Set the optional capability bits in the bit mask
*
* @param    optCap        (bit mask)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpOptCapSupportSet (L7_ushort16 optCap );

/*********************************************************************
* @purpose  Retrieve the mode for Route reflection
*
* @returns  L7_TRUE  if enabled for Route Reflection
* @returns  L7_FALSE  if not enabled
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteReflectorModeGet(L7_uint32* mode);

/*********************************************************************
* @purpose  Set the Router as a Route Reflector
*
* @param    rrMode [L7_ENABLE to enable and L7_DISABLE to diable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteReflectorModeSet ( L7_uint32 rrMode );

/*********************************************************************
*
* @purpose  Get the Cluster Id this router belongs to
*
* @param    *clusterId      bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpClusterIdGet(bgpIpAddr *clusterId);

/*********************************************************************
*
* @purpose  Set the Cluster Id this router belongs to
*
* @param    *clusterId      bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpClusterIdSet(bgpIpAddr *clusterId);

/*********************************************************************
*
* @purpose  Get the Confederation Id of this router
*
* @param    *confedId      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    An externally visible autonomous system number that
*       identifies the confederation as a whole
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpConfedIdGet(L7_ushort16 *confedId);

/*********************************************************************
*
* @purpose  Set the Confederation Id of this router
*
* @param    confedId      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    An externally visible autonomous system number that
*       identifies the confederation as a whole
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpConfedIdSet(L7_ushort16 confedId);


/*********************************************************************
*
* @purpose  Get the Next entry of Address Family list
*
* @param    addrEntry
* @param    previousEntry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAddrFamilyEntryGetNext(bgpAddrFamily_t* addrFamily, bgpAddrFamily_t* previousEntry);

/*********************************************************************
*
* @purpose  Add the AFI, SAFI to the list
*
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
L7_RC_t bgpMapBgpAddrFamilyEntryAdd(L7_ushort16  Afi, L7_ushort16  Safi);

/*********************************************************************
*
* @purpose  Delete the AFI, SAFI from the list
*
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
L7_RC_t bgpMapBgpAddrFamilyEntryDelete(L7_ushort16  Afi, L7_ushort16  Safi);

/*********************************************************************
*
* @purpose  Get the number of SNPAs present
*
* @param    *number      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpSnpaNumGet(L7_ushort16 *number);

/*********************************************************************
*
* @purpose  Set the number of SNPAs present
*
* @param    number      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpSnpaNumSet(L7_ushort16 number);


/*********************************************************************
*
* @purpose  Get the First Entry of the SNPA list
*
* @param    snpaEntry
* @param    previousEntry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpSnpaEntryGetNext(bgpSnpa_t* snpaEntry, bgpSnpa_t* previousEntry);

/*********************************************************************
*
* @purpose  Add to the list of SNPAs
*
* @param    *snpaAddr   bgpIPAddr
* @param    snpaLen L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpSnpaEntryAdd(bgpIpAddr *snpaAddr, L7_ushort16 snpaLen);

/*********************************************************************
*
* @purpose  Delete from the list of SNPAs
*
* @param    *snpaAddr   bgpIPAddr
* @param    snpaLen L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpSnpaEntryDelete(bgpIpAddr *snpaAddr, L7_ushort16 snpaLen);

/*********************************************************************
*
* @purpose  Get the mode of auto restart
*
* @returns  L7_ENABLE  if enabled
* @returns  L7_DISABLE if not enabled
*
* @notes    Use internal automatically Start message sending in case of connection failure
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAutoRestartOptModeGet(L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode of auto restart
*
* @param    autoRestartMode L7_uint32  [L7_ENABLE to enable and L7_DISABLE to disable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Use internal automatically Start message sending in case of connection failure
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAutoRestartOptModeSet(L7_uint32 autoRestartMode);

/*********************************************************************
*
* @purpose  Get the value of route origin
*
* @param    *origin L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteOriginGet(L7_ushort16 *origin);

/*********************************************************************
*
* @purpose  Set the value of route origin
*
* @param    origin      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteOriginSet(L7_ushort16 origin);

/*********************************************************************
*
* @purpose  Get the value of MED attribute
*
* @param    *med    L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpMEDGet(L7_long32 *med);

/*********************************************************************
*
* @purpose  Set the value of MED attribute
*
* @param    med     L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpMEDSet(L7_long32 med);

/*********************************************************************
*
* @purpose  Get the value of local preference attribute
*
* @param    *preference L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLocalPrefGet(L7_uint32 *preference);

/*********************************************************************
*
* @purpose  Set the value of local preference attribute
*
* @param    preference      L7_long32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLocalPrefSet(L7_uint32 preference);

/*********************************************************************
*
* @purpose  Get the value of local assosiated community
*
* @param    *community  L7_ulong32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteCommunityGet(L7_uint32 *community);

/*********************************************************************
*
* @purpose  Set the value of local assosiated community
*
* @param    community       L7_ulong32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteCommunitySet(L7_uint32 community);

/*********************************************************************
*
* @purpose  Get the length of next-hop address
*
* @param    *length L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpNextHopAddrLenGet(L7_ushort16 *length);

/*********************************************************************
*
* @purpose  Set the length of next-hop address
*
* @param    length      L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpNextHopAddrLenSet(L7_ushort16 length);

/*********************************************************************
*
* @purpose  Get the status of the BGP object
*
* @returns  L7_TRUE  if active
* @returns  L7_FALSE  if not active
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL bgpMapBgpIsActive();

/*
**********************************************************************
*                API FUNCTIONS  -  BGP ROUTE-FLAP DAMPING API
**********************************************************************
*/

/*********************************************************************
*
* @purpose  Get the mode for route flap damping
*
* @returns  L7_ENABLE  if flap damping is enabled
* @returns  L7_DISABLE if flap damping is disabled
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteFlapModeGet(L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode for route flap damping
*
* @param    flapMode    L7_uint32 [L7_ENABLE to enable, L7_DISABLE to disable]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpRouteFlapModeSet(L7_uint32 flapMode);

/*********************************************************************
*
* @purpose  Get the value of suppress limit
*
* @param    *suppressLimit  L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapSuppressLimitGet(L7_int32 *suppressLimit);

/*********************************************************************
*
* @purpose  Set the value of suppress limit
*
* @param    suppressLimit       L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapSuppressLimitSet(L7_int32 suppressLimit);

/*********************************************************************
*
* @purpose  Get the value of reuse limit
*
* @param    *reuseLimit L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapReuseLimitGet(L7_int32 *reuseLimit);

/*********************************************************************
*
* @purpose  Set the value of reuse limit
*
* @param    reuseLimit      L7_int32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapReuseLimitSet(L7_int32 reuseLimit);

/*********************************************************************
*
* @purpose  Get the value of penalty increment
*
* @param    *penaltyInc L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapPenaltyIncGet(L7_uint32 *penaltyInc);

/*********************************************************************
*
* @purpose  Set the value of penalty increment
*
* @param    penaltyInc      L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapPenaltyIncSet(L7_uint32 penaltyInc);

/*********************************************************************
*
* @purpose  Get the value of delta time
*
* @param    *deltaTime  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapDeltaTimeGet(L7_uint32 *deltaTime);

/*********************************************************************
*
* @purpose  Set the value of delta time
*
* @param    deltaTime       L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapDeltaTimeSet(L7_uint32 deltaTime);

/*********************************************************************
*
* @purpose  Get the value of maximum flap entry life time
*
* @param    *maxFlapTime    L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapMaxTimeGet(L7_uint32 *maxFlapTime);

/*********************************************************************
*
* @purpose  Set the value of maximum flap entry life time
*
* @param    maxFlapTime     L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapMaxTimeSet(L7_uint32 maxFlapTime);

/*********************************************************************
*
* @purpose  Get the value of damping factor
*
* @param    *dampingFactor  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapDampingFactorGet(L7_uint32 *dampingFactor);

/*********************************************************************
*
* @purpose  Set the value of damping factor
*
* @param    dampingFactor       L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapDampingFactorSet(L7_uint32 dampingFactor);


/*********************************************************************
*
* @purpose  Get the value of reuse size
*
* @param    *reuseSize  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapReuseSizeGet(L7_uint32 *reuseSize);

/*********************************************************************
*
* @purpose  Set the value of reuse size
*
* @param    reuseSize       L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpFlapReuseSizeSet(L7_uint32 reuseSize);

/*********************************************************************
*
* @purpose  Get the dampened path
*
* @param *prefix         bgpIpAddr
* @param *prefixLen      L7_ushort16
* @param *state          L7_ushort16
* @param *penaltyValue   L7_int32
* @param *decayDecrement L7_ushort16
* @param *timeCreated    L7_timespec
* @param *timeSuppressed L7_timespec
* @param *eventState     L7_ushort16
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
L7_RC_t bgpMapBgpFlapEntryGetNext(bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                             L7_ushort16 *state, L7_int32 *penaltyValue,
                             L7_ushort16 *decayDecrement, L7_timespec *timeCreated,
                             L7_timespec *timeSuppressed, L7_ushort16 *eventState,
                             bgpIpAddr *prevPrefix, L7_ushort16 prevPrefixLen);


/*
**********************************************************************
*                    API FUNCTIONS  -  BGP AGGREGATION API
**********************************************************************
*/

/*********************************************************************
* @purpose  Get the address aggregation entry based on the index
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    index           L7_uint32
* @param    *prefix         bgpIpAddr
* @param    *prefixLen      L7_ushort16
* @param    *aggrEffect     L7_ushort16
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
L7_RC_t bgpMapBgpAggrEntryGet(L7_uint32 index, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                              L7_ushort16 *aggrEffect, L7_BOOL *advUnfeasible);

/*********************************************************************
* @purpose  Get the next address aggregation entry of the aggr list
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    *prefix         bgpIpAddr
* @param    *prefixLen      L7_ushort16
* @param    *aggrEffect     L7_ushort16
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
L7_RC_t bgpMapBgpAggrEntryGetNext(L7_uint32 *index, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                                  L7_ushort16 *aggrEffect, L7_BOOL *advUnfeasible, bgpIpAddr *previousPrefix);

/*********************************************************************
*
* @purpose  Get the mode for address aggregation
*
* @returns  L7_ENABLE  if address aggregation is enabled
* @returns  L7_DISABLE if address aggregation is disabled
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAddrAggregationModeGet(L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode for address aggregation
*
* @param    addrAggregationMode  L7_uint32 [L7_ENABLE or L7_DISABLE]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAddrAggregationModeSet(L7_uint32 addrAggregationMode);


/*********************************************************************
*
* @purpose  Get the mode for less specific route selection
*
* @returns  L7_ENABLE  if less specific route selection is enabled
* @returns  L7_DISABLE if it is disabled
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLessSpecRouteSelectModeGet(L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode for less specific route selection
*
* @param    lessSpecRouteSelectMode  L7_uint32 [L7_ENABLE or L7_DISABLE]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpLessSpecRouteSelectModeSet(L7_uint32 lessSpecRouteSelectMode);

/*********************************************************************
*
* @purpose  Get the mode for path attribute aggregation
*
* @returns  L7_ENABLE  if path attribute aggregation is enabled
* @returns  L7_DISABLE if path attribute aggregation is disabled
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPathAttrAggregationModeGet(L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the mode for path attribute aggregation
*
* @param    pathAttrAggregationMode  L7_uint32 [L7_ENABLE or L7_DISABLE]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPathAttrAggregationModeSet(L7_uint32 pathAttrAggregationMode);

/*********************************************************************
*
* @purpose  Create an aggregation entry
*
* @param    *prefix         bgpIpAddr
* @param    prefixLen      L7_ushort16
* @param    aggeEffect     L7_uint32
* @param    advUnfeasible  L7_uint32
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAggrEntryAdd(L7_uint32 *index, bgpIpAddr *prefix, L7_ushort16 prefixLen,
                              L7_ushort16 aggrEffect, L7_BOOL advUnfeasible);

/*********************************************************************
*
* @purpose  Delete the aggregation entry
*
* @param    *prefix         bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAggrEntryDelete(bgpIpAddr *prefix);


/*********************************************************************
*
* @purpose  Get the Address Aggregation effect of the specified prefix
*
* @param    *prefix       bgpIpAddr
* @param    *aggrEffect  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAggrEffectGet(bgpIpAddr *prefix, L7_ushort16 prefixLen, L7_ushort16 *aggrEffect);

/*********************************************************************
*
* @purpose  Set the Address Aggregation effect of the specified prefix
*
* @param    *prefix     bgpIpAddr
* @param    aggrEffect L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAggrEffectSet(bgpIpAddr *prefix, L7_ushort16 aggrEffect);

/*********************************************************************
*
* @purpose  Set the Address Aggregation feasibility of the specified prefix
*
* @param    *prefix      bgpIpAddr
* @param    advUnFeasible  L7_BOOL
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    TBA
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAggrFeasibilitySet(bgpIpAddr *prefix, L7_BOOL advUnfeasible);

/*********************************************************************
*
* @purpose  Get the Unfeasible Advt. flag of the specified prefix
*
* @param    *prefix      bgpIpAddr
* @param    *advUnfeasible L7_BOOL
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAggrAdvtUnfeasibleFlagGet(bgpIpAddr *prefix, L7_ushort16 prefixLen, L7_BOOL *advUnfeasible);

/*********************************************************************
*
* @purpose  Set the Unfeasible Advt. flag of the specified prefix
*
* @param    *prefix      bgpIpAddr
* @param    advUnfeasible  L7_BOOL
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpAggrAdvtUnfeasibleFlagSet(bgpIpAddr *prefix, L7_ushort16 prefixLen, L7_BOOL advUnfeasible);

/*
***********************************************************************
*                     API FUNCTIONS  -  PEER API
***********************************************************************
*/

/*---------------------------------------------------------------------*/
/* BGP Peer Table                                                 */
/* The BGP Peer Table describes all peers               */
/*---------------------------------------------------------------------*/

L7_RC_t bgpMapBgpMibPeerRemoteAddrGet ( bgpIpAddr *remoteAddr);

L7_RC_t bgpMapBgpMibPeerRemoteAddrGetNext(bgpIpAddr *remoteAddr, bgpIpAddr *nextRemoteAddr);

/*********************************************************************
* @purpose  Get first peer's remote address
*
* @param    *remoteAddr     bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    gives the identifier of the first peer in the table
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerRemoteAddrGet ( bgpIpAddr *remoteAddr);

/*********************************************************************
* @purpose  Get the next peer's remote address
*
* @param    *remoteAddr     bgpIpAddr
* @param    *prevRemoteAddr     bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerRemoteAddrGetNext ( bgpIpAddr *remoteAddr, bgpIpAddr *prevRemoteAddr );

/*********************************************************************
* @purpose  Configure the peer
*
* @param    *remoteAddr  bgpIpAddr
* @param    remoteAS     L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Terminates the conn. and deletes its config. from the file
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConfigure(bgpIpAddr *remoteAddr, L7_ushort16 remoteAS);

/*********************************************************************
* @purpose  Delete the peer
*
* @param    *IpAddr     bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Terminates the conn. and deletes its config. from the file
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerDelete ( bgpIpAddr *IpAddr);

/*********************************************************************
* @purpose  Reset the peer
*
* @param    *IpAddr     bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    resets the conn with the peer
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerReset ( bgpIpAddr *IpAddr);

/*********************************************************************
* @purpose  Get the state of the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *state      L7_ushort16 state of the Peer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerStateGet(bgpIpAddr *remoteAddr, L7_ushort16 *state);

/*********************************************************************
* @purpose  Get the admin status of the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *adminStatus      L7_ushort16 admin status of the Peer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes        START or STOP
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAdminStatusGet(bgpIpAddr *remoteAddr, L7_ushort16 *adminStatus);

/*********************************************************************
* @purpose  Set the admin status of the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    adminStatus L7_ushort16 admin status of the Peer
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes        START or STOP
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAdminStatusSet(bgpIpAddr *remoteAddr, L7_ushort16 adminStatus);

/*********************************************************************
* @purpose  Get the negotiated version of BGP with the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *negotiatedVersion      L7_ulong32 version negotiated
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerNegotiatedVersionGet(bgpIpAddr *remoteAddr, L7_ulong32 *negotiatedVersion);

/*********************************************************************
* @purpose  Get the local address for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *localAddr      bgpIpAddr   local IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerLocalAddrGet(bgpIpAddr *remoteAddr, bgpIpAddr *localAddr);

/*********************************************************************
* @purpose  Get the local port for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *localPort      L7_ushort16 local port
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerLocalPortGet(bgpIpAddr *remoteAddr, L7_ushort16 *localPort);

/*********************************************************************
* @purpose  Set the local port for the specified Bgp Peer. This
*           API is deprecated. The local port is selected automatically
*           depending on whether this router initiates the TCP
*           connection.
*
* @param    *remoteAddr    bgpIpAddr
* @param    localPort L7_ushort16  local port
*
* @returns  L7_FAILURE
*
* @notes    Deprecated
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerLocalPortSet(bgpIpAddr *remoteAddr, L7_ushort16 localPort);


/*********************************************************************
* @purpose  Get the remote port for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *remotePort      L7_ushort16    remote port
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerRemotePortGet(bgpIpAddr *remoteAddr, L7_ushort16 *remotePort);

/*********************************************************************
* @purpose  Set the remote port for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    remotePort      L7_ushort16 remote port
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerRemotePortSet(bgpIpAddr *remoteAddr, L7_ushort16 remotePort);

/*********************************************************************
* @purpose  Get the peer id for the specified Bgp Peer
*
* @param    *remoteAddr  bgpIpAddr
* @param    *peerId      bgpIpAddr  remote IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 bgpMapBgpPeerIdGet(bgpIpAddr *remoteAddr, bgpIpAddr *peerId);

/*********************************************************************
* @purpose  Set the PeerId for the specified Bgp Peer. This API is
*           deprecated. The BGP peer is learned from the peer's
*           OPEN message.
*
* @param    *remoteAddr  bgpIpAddr  remote IP Address
* @param    *peerId      bgpIpAddr
*
* @returns  L7_FAILURE
*
* @notes    Deprecated.
*
* @end
*********************************************************************/
L7_uint32 bgpMapBgpPeerIdSet(bgpIpAddr *remoteAddr, bgpIpAddr *peerId);


/*********************************************************************
* @purpose  Get the remote AS number of the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *remoteAS      L7_ushort16  remote AS number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerRemoteASGet(bgpIpAddr *remoteAddr, L7_ushort16 *remoteAS);

/*********************************************************************
* @purpose  Set the remote AS number of the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    remoteAS    L7_ushort16 remote AS number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerRemoteASSet(bgpIpAddr *remoteAddr, L7_ushort16 remoteAS);

/*********************************************************************
* @purpose  Get the connection retry interval for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *connRetryInterval      L7_ushort16 connection retry interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConnRetryIntervalGet(bgpIpAddr *remoteAddr, L7_ushort16 *connRetryInterval);

/*********************************************************************
* @purpose  Set the rconnection retry interval for the specified Bgp Peer
*
* @param    *remoteAddr  bgpIpAddr
* @param    connRetryInterval  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConnRetryIntervalSet(bgpIpAddr *remoteAddr, L7_ushort16 connRetryInterval);

/*********************************************************************
* @purpose  Get the negotiated hold time with the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *hold time      L7_ushort16 negotiated hold time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerHoldTimeGet(bgpIpAddr *remoteAddr, L7_ushort16 *holdTime);

/*********************************************************************
* @purpose  Get the negotiated keep-alive time with the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *holdTime   L7_ushort16 negotiated keep-alive
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerKeepAliveTimeGet(bgpIpAddr *remoteAddr, L7_ushort16 *holdTime);

/*********************************************************************
* @purpose  Get the configured hold time for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *configuredHoldTime L7_ushort16 configured hold time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConfiguredHoldTimeGet(bgpIpAddr *remoteAddr, L7_ushort16 *configuredHoldTime);

/*********************************************************************
* @purpose  Set the configured hold time for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    configuredHoldTime      L7_ushort16 configured hold time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConfiguredHoldTimeSet(bgpIpAddr *remoteAddr, L7_uint32 configuredHoldTime);


/*********************************************************************
* @purpose  Get the configured keep alive time for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *configuredKeepAlive    L7_ushort16 configured keep alive time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConfiguredKeepAliveTimeGet(bgpIpAddr *remoteAddr, L7_ushort16 *configuredKeepAlive);

/*********************************************************************
* @purpose  Set the configured keep alive time for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    configuredKeepAlive     L7_ushort16 configured keep alive time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConfiguredKeepAliveTimeSet(bgpIpAddr *remoteAddr, L7_uint32 configuredKeepAlive);

/*********************************************************************
* @purpose  Get the optional capabilities enabled for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *optionalCap      L7_ushort16   capabilities bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerOptionalCapGet(bgpIpAddr *remoteAddr, L7_ushort16 *optionalCap);

/*********************************************************************
* @purpose  Set the optional capabilities for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    optionalCap     L7_ushort16 capabilities bit mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerOptionalCapSet(bgpIpAddr *remoteAddr, L7_ushort16 optionalCap);

/*********************************************************************
*
* @purpose  See if the specified peer is a Route Reflector Client
*
* @param    *remoteAddr bgpIpAddr
*
* @returns  L7_ENABLE  if specified peer is enabled as Route Reflector Client
* @returns  L7_DISABLE if is disabled
*
* @notes
*
* @end
*********************************************************************/
 L7_RC_t bgpMapBgpPeerRrClientModeGet(bgpIpAddr *remoteAddr, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the specified peer as a Route Reflector Client or Non-Client
*
* @param    *remoteAddr bgpIpAddr
* @param    rrClientMode        L7_uint32 [ L7_ENABLE or L7_DISABLE]
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerRrClientModeSet(bgpIpAddr *remoteAddr, L7_uint32 rrClientMode);

/*********************************************************************
*
* @purpose  Get the value of the confederation member flag for the specified peer
*
* @param    *remoteAddr bgpIpAddr
*
* @returns  L7_TRUE  if specified peer is a confederation member
* @returns  L7_FALSE  if not
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConfedMemberModeGet(bgpIpAddr *remoteAddr, L7_uint32* mode);

/*********************************************************************
*
* @purpose  Set the value of the confederation member flag for the specified peer
*
* @param    *remoteAddr bgpIpAddr
* @param    confedMemberMode        L7_BOOL [ L7_TRUE to set as a confed member and
*           L7_FALSE to indicate the peer is not a confed member
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerConfedMemberModeSet(bgpIpAddr *remoteAddr, L7_BOOL confedMemberMode);

/*********************************************************************
*
* @purpose  Get the value of the NotNextHopSelf flag for the specified peer
*
* @param    *remoteAddr bgpIpAddr
* @param    nextHopSelfMode L7_uint32
*
* @returns  L7_TRUE  if notNextHopSelf's value is TRUE
* @returns  L7_FALSE  if not
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerNotNextHopSelfModeGet(bgpIpAddr *remoteAddr, L7_uint32* nextHopSelfMode);

/*********************************************************************
*
* @purpose  Set the value of the NotNextHopSelf flag for the specified peer
*
* @param    *remoteAddr bgpIpAddr
* @param    notNextHopSelfMode     L7_BOOL [ L7_TRUE to set notNextHopSelf value to TRUE
*           L7_FALSE to clear that
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerNotNextHopSelfModeSet(bgpIpAddr *remoteAddr, L7_BOOL notNextHopSelfMode);


/*********************************************************************
*
* @purpose  Get the Next entry of the Address Family list for the specified peer
*
* @param    *remoteAddr bgpIpAddr
* @param    addrEntry
* @param    prevEntry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAddrFamilyEntryGetNext(bgpIpAddr *remoteAddr, bgpAddrFamily_t* addrEntry,
                                            bgpAddrFamily_t* prevEntry);

/*********************************************************************
*
* @purpose  Add the AFI, SAFI to the list for the specified peer
*
* @param    *remoteAddr bgpIpAddr
* @param    Afi L7_ushort16
* @param    Safi L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAddrFamilyEntryAdd(bgpIpAddr *remoteAddr, L7_ushort16  Afi, L7_ushort16  Safi);

/*********************************************************************
*
* @purpose  Delete the AFI, SAFI from the list for the specified peer
*
* @param    *remoteAddr bgpIpAddr
* @param    Afi L7_ushort16
* @param    Safi L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAddrFamilyEntryDelete(bgpIpAddr *remoteAddr, L7_ushort16  Afi, L7_ushort16  Safi);

/*********************************************************************
* @purpose  Get the authentication parms for the specified Bgp Peer
*
* @param    *remoteAddr  bgpIpAddr
* @param    *authCode  L7_ushort16  authentication type code
* @param    authKeyStr  L7_uchar8*  authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAuthenticationGet(bgpIpAddr *remoteAddr, L7_ushort16 *authCode, L7_uchar8 *authKeyStr);

/*********************************************************************
* @purpose  Set the authentication parms for the specified Bgp Peer
*
* @param    *remoteAddr  bgpIpAddr
* @param    authCode  L7_ushort16  authentication type code
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Both the authentication code and key values are set by
*           this function.
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAuthenticationSet(bgpIpAddr *remoteAddr, L7_ushort16 authCode, L7_uchar8 *authKeyStr);

/*********************************************************************
* @purpose  Get the authentication type code for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *authCode      L7_ushort16      authentication type code
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAuthCodeGet(bgpIpAddr *remoteAddr, L7_ushort16 *authCode);

/*********************************************************************
* @purpose  Set the authentication type code for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    authCode        L7_ushort16 authentication type code
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAuthCodeSet(bgpIpAddr *remoteAddr, L7_ushort16 authCode);

/*********************************************************************
* @purpose  Get the authentication key for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    authKeyStr  L7_uchar8* authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAuthKeyGet(bgpIpAddr *remoteAddr, L7_uchar8 *authKeyStr);

/*********************************************************************
* @purpose  Set the authentication key for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    authKeyStr  L7_uchar8   authentication key string
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerAuthKeySet(bgpIpAddr *remoteAddr, L7_uchar8 *authKeyStr);

/*********************************************************************
* @purpose  Get the local interface address of the specified Bgp Peer
*       that will be used as Next hop address
*
* @param    *remoteAddr bgpIpAddr
* @param    localIfAddr bgpIpAddr   peer's local interface IP Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerLocalIfAddrGet(bgpIpAddr *remoteAddr, bgpIpAddr *localIfAddr);

/*********************************************************************
* @purpose  Get the message send limit for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *msgSendLimit      L7_ushort16  message send limit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerMsgSendLimitGet(bgpIpAddr *remoteAddr, L7_ushort16 *msgSendLimit);

/*********************************************************************
* @purpose  Set the message send limit for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    msgSendLimit        L7_ushort16 message send limit
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerMsgSendLimitSet(bgpIpAddr *remoteAddr, L7_ushort16 msgSendLimit);

/*********************************************************************
* @purpose  Get the transmission delay interval for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *txDelayInterval      L7_ushort16   transmission delay interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerTxDelayIntervalGet(bgpIpAddr *remoteAddr, L7_ushort16 *txDelayInterval);

/*********************************************************************
* @purpose  Set the transmission delay interval for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    txDelayInterval     L7_ushort16 transmission delay interval
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerTxDelayIntervalSet(bgpIpAddr *remoteAddr, L7_ushort16 txDelayInterval);

/*********************************************************************
* @purpose  Get whether the router is configured to only log a warning, rather
*           than shutdown the peer, when the number of prefixes learned
*           from the peer exceeds a configured limit.
*
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
L7_RC_t bgpMapPeerPfxWarnOnlyGet(bgpIpAddr *remoteAddr, L7_BOOL *warnOnly);

/*********************************************************************
* @purpose  Set whether the router is configured to only log a warning, rather
*           than shutdown the peer, when the number of prefixes learned
*           from the peer exceeds a configured limit.
*
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
L7_RC_t bgpMapPeerPfxWarnOnlySet(bgpIpAddr *remoteAddr, L7_BOOL warnOnly);

/*********************************************************************
* @purpose  Get the maximum number of prefixes the router is configured
*           to learn from a given peer.
*
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
L7_RC_t bgpMapPeerPfxLimitGet(bgpIpAddr *remoteAddr, L7_uint32 *pfxLimit);

/*********************************************************************
* @purpose  Set the maximum number of prefixes the router is configured
*           to learn from a given peer.
*
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
L7_RC_t bgpMapPeerPfxLimitSet(bgpIpAddr *remoteAddr, L7_uint32 pfxLimit);

 /*********************************************************************
* @purpose  Get the percentage of the prefix limit at which a warning
*           should be logged.
*
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
L7_RC_t bgpMapPeerPfxWarnThreshGet(bgpIpAddr *remoteAddr, L7_uint32 *pfxThresh);

/*********************************************************************
* @purpose  Set the percentage of the prefix limit at which a warning
*           should be logged.
*
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
L7_RC_t bgpMapPeerPfxWarnThreshSet(bgpIpAddr *remoteAddr, L7_uint32 pfxThresh);

/*
***********************************************************************
*                     API FUNCTIONS  -  BGP PER PEER STATISTICS
***********************************************************************
*/

/*********************************************************************
* @purpose  Get the no. of UPDATE messages received from the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *peerInUpdates  L7_ulong32  no. of UPDATE messages
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerInUpdatesGet(bgpIpAddr *remoteAddr, L7_ulong32 *peerInUpdates);

/*********************************************************************
* @purpose  Get the no. of UPDATE messages sent to the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *peerOutUpdates L7_ulong32  no. of UPDATE messages
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerOutUpdatesGet(bgpIpAddr *remoteAddr, L7_ulong32 *peerOutUpdates);

/*********************************************************************
* @purpose  Get the total no. of messages received from the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *peerInTotalMsg L7_ulong32  total no. of messages
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerInTotalMsgGet(bgpIpAddr *remoteAddr, L7_ulong32 *peerInTotalMsg);

/*********************************************************************
* @purpose  Get the total no. of messages sent to the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *peerOutTotalMsg    L7_ulong32  total no. of messages
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerOutTotalMsgGet(bgpIpAddr *remoteAddr, L7_ulong32 *peerOutTotalMsg);

/*********************************************************************
* @purpose  Get the total no. of times the BGP FSM transitioned into established
*        state for the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *totalFsmEstTrans   L7_ulong32  total no.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerFsmEstTransGet(bgpIpAddr *remoteAddr, L7_ulong32 *totalFsmEstTrans);

/*********************************************************************
* @purpose  Get the time the specified Bgp Peer has been in the established state
*
* @param    *remoteAddr bgpIpAddr
* @param    *fsmEstTime L7_timespec total time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerFsmEstTimeGet(bgpIpAddr *remoteAddr, L7_timespec *fsmEstTime);

/*********************************************************************
* @purpose  Get the time in seconds since the last UPDATE message was
*       received from the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    *inUpdateElapsedTime    L7_timespec time elapsed
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerInUpdateElapsedTimeGet(bgpIpAddr *remoteAddr, L7_timespec *inUpdateElapsedTime);

/*********************************************************************
* @purpose  Get the last error code/sub code seen by this connection
*       to the specified Bgp Peer
*
* @param    *remoteAddr bgpIpAddr
* @param    lastError[] L7_uchar8   error code/sub code
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t bgpMapBgpPeerLastErrorGet(bgpIpAddr *remoteAddr, L7_uchar8 *lastError);

/**************************************************************************
*
* @purpose  Gets next policy. If prevIndex is zero it gets first policy in the list.
*
* @param    *ptoto          @b{{input}} protocol
* @param    *index          @b{{input}} indicates the Index in the Access List
* @param    *matchType      @b{{input}} policy match type
* @param    *permitFlasg    @b{{input}} permisstion flags
* @param    *numRanges      @b{{input}} number of ranges
* @param    *numActions     @b{{input}} number of actions
* @param    *numRangeArgs   @b{{input}} number of arguments for a range of policy
* @param    *numActionArgs  @b{{input}} number of arguments for a action of policy
* @param    **rangeArgs     @b{{input}} range arguments
* @param    **actionArgs    @b{{input}} actoin arguments
* @param    prevIndex       @{{input}}  prevIndex which will tell whether first index
*                            or next index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if no entry or invalid index
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapPolicyEntryGetNext(L7_ushort16 *proto, L7_uint32 *index,
                                 L7_ushort16 *matchType, L7_BOOL *permitFlag,
                                 L7_uint32 *numRanges, L7_uint32 *numActions,
                                 bgpPolicyRange_t *ranges, bgpPolicyAction_t *actions, L7_uint32 prevIndex);

/*********************************************************************
*
* @purpose  Set the Unfeasible Advt. flag of the specified prefix
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    advUnfeasible       @b{{input}} L7_BOOL
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpRcvdPathAttrEntryMibGet(bgpIpAddr *prefix, L7_int32 *prefixLen, bgpIpAddr *peerId);

/**************************************************************************
* @purpose  Get the next peer's RemoteAddr
*
* @param    *prefix         @b{{input}} BGP IP address
* @param    *prefixLen      @b{{input}} prefix leght
* @param    *peerId         @b{{input}} Next BGP peer IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
***************************************************************************/

L7_RC_t bgpMapBgpRcvdPathAttrEntryMibGetNext(bgpIpAddr *prefix, L7_int32 *prefixLen, bgpIpAddr *peerId);

/***************************************************************************
*
* @purpose  Set the Unfeasible Advt. flag of the specified prefix
*
* @param    *prefix             @b{{input}} prefix
* @param    *prefixLen          @b{{input}} prefixLen
* @param    *remoteAddr         @b{{input}} remote address
* @param    *preRemoteAddr      @b{{input}} previous remore address
* @param    *entry              @b{{input}} BGP path attributes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
****************************************************************************/

L7_RC_t bgpMapBgpPathAttrEntryGetNext(bgpIpAddr *prefix, L7_ushort16 *prefixLen, bgpIpAddr *remoteAddr,
                                      bgpIpAddr *prevRemoteAddr, bgpMapPathAttribute_t *entry);

/****************************************************************************
* @purpose  Gets entry of origin path attributes
*
* @param    *prefix         @b{{input}} BGP IP address
* @param    *prefixLen      @b{{input}} prefix lenght
* @param    *remoteAddr     @b{{input}} Next BGP IP address
* @param    *origin         @b{{input}} path attributes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrOriginGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                   L7_ushort16* origin);

/****************************************************************************
* @purpose  Get an AS path segment attributes.
*
* @param    *prefix         @b{{input}} BGP IP address
* @param    *prefixLen      @b{{input}} prefix lenght
* @param    *remoteAddr     @b{{input}} Next BGP IP address
* @param    *asPathSegnent  @b{{input}} AS Path segment len
* @param    *asPathSegLen   @b{{input}} AS path segmentation Len
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrAsPathSegmentGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                          L7_uchar8*asPathSegment, L7_uint32* asPathSegLen);

/****************************************************************************
* @purpose  Get next hop path attributes
*
* @param    *prefix         @b{{input}} BGP IP address
* @param    *prefixLen      @b{{input}} prefix lenght
* @param    *remoteAddr     @b{{input}} remote BGP IP address
* @param    *nextHop        @b{{input}} next BGP IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrNextHopGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                    bgpIpAddr *nextHop);

/****************************************************************************
* @purpose  Get MED path attributes
*
* @param    *prefix         @b{{input}} BGP IP address
* @param    *prefixLen      @b{{input}} prefix lenght
* @param    *remoteAddr     @b{{input}} remote BGP IP address
* @param    *med          @b{{input}} next BGP IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrMEDGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                L7_long32 *med);

/****************************************************************************
* @purpose  Get local path attributes
*
* @param    *prefix         @b{{input}} BGP IP address
* @param    *prefixLen      @b{{input}} prefix lenght
* @param    *remoteAddr     @b{{input}} remote BGP IP address
* @param    *localPref      @b{{input}} local preference value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrLocalPrefGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                      L7_long32 *localPref);

/****************************************************************************
* @purpose  Get Atomic path attributes
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} remote BGP IP address
* @param    *atomicAggregate    @b{{input}} automic aggregate value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrAtomicAggregateGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                            L7_ushort16 *atomicAggregate);

/****************************************************************************
* @purpose  Get aggregator AS path attributes
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} remote BGP IP address
* @param    *aggregatorAS       @b{{input}} aggregator AS value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrAggregatorAsGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                         L7_ushort16 *aggregatorAs);

/****************************************************************************
* @purpose  Get aggregator address attributes
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} remote BGP IP address
* @param    *aggregatorAddr     @b{{input}} aggregator address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrAggregatorAddrGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                           bgpIpAddr *aggregatorAddr);

/********************************************************************************
* @purpose  Get local preference path attributes
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} remote BGP IP address
* @param    *calcLocalPref      @b{{input}} local Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
********************************************************************************/

L7_RC_t bgpMapBgpPathAttrCalcLocalPrefGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                          L7_long32 *calcLocalPref);

/****************************************************************************
* @purpose  Get the set of communities associated with a path.
*
* @param    prefix          @b{{input}} a destination prefix
* @param    prefixLen       @b{{input}} length of destination prefix
* @param    remoteAddr      @b{{input}} peer that advertised this path
* @param    communities     @b{{output}} array of communities. On input, point
*                                        to an array of at least 255 integers
*                                        where communities can be stored.
* @param    numCommunities  @b{{output}} the number of communities written to
*                                        the communities array on output
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t bgpMapBgpPathAttrCommunitiesGet(bgpIpAddr *prefix, L7_int32 prefixLen,
                                        bgpIpAddr *remoteAddr,
                                        L7_uint32 *communities,
                                        L7_uint32 *numCommunities);

/****************************************************************************
* @purpose  Get best path attributes
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} remote BGP IP address
* @param    *best               @b{{input}} best path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrBestGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr, L7_ushort16 *best);

/****************************************************************************
* @purpose  Get unknow path attributes
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} remote BGP IP address
* @param    *unknown            @b{{input}} unknown
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpPathAttrUnknownGet(bgpIpAddr *prefix, L7_int32 prefixLen, bgpIpAddr *remoteAddr,
                                    L7_uchar8 *unknown, L7_uint32* unknownLen);

/*********************************************************************
*
* @purpose  Get an NLRI entry based on the index at which it is stored
*
* @param    index           index of the entry
* @param    *prefix         bgpIpAddr
* @param    *prefixLen      L7_ushort16
* @param    *vpnCos         L7_ulong32
* @param    *nextHop        bgpIpAddr
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no entry at index
*
* @notes    TBA
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpNlriEntryGet(L7_uint32 index, bgpIpAddr *prefix, L7_ushort16 *prefixLen, L7_ulong32 *vpnCos,
                               L7_BOOL *sendNow, bgpIpAddr *nextHop);

/*********************************************************************
*
* @purpose  Get the next address aggregation entry of the aggr list
*
* @param    *prefix         bgpIpAddr
* @param    *prefixLen      L7_ushort16
* @param    *vpnCos         L7_ulong32
* @param    *nextHop        bgpIpAddr
* @param    *prevPrefix     bgpIpAddr
* @param    *prevPrefixLen  L7_ushort16
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if end of list
*
* @notes    TBA
*
* @end
*********************************************************************/


L7_RC_t bgpMapBgpNlriEntryGetNext( L7_uint32 *index, bgpIpAddr *prefix, L7_ushort16 *prefixLen,
                                   L7_ulong32 *vpnCos, L7_BOOL *sendNow, bgpIpAddr *nextHop, bgpIpAddr *prevPrefix, L7_ushort16 prevPrefixLen);

/****************************************************************************
* @purpose  Add Network Layer Reachability Information (NRLI)
*
* @param    *index              @b{{input}} BGP IP address
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *vpnCos             @b{{input}} VPN
* @param    *nexthop            @b{{input}} next hop BGP IP addrss
* @param    *sendnow            @b{{input}} send now
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpNlriAdd( L7_uint32 *index, bgpIpAddr *prefix, L7_ushort16 prefixLen, L7_ulong32 vpnCos,
                          bgpIpAddr *nextHop, L7_BOOL sendNow);

/****************************************************************************
* @purpose  Delete Network Layer Reachability Information (NRLI)
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpNlriDelete(bgpIpAddr *prefix, L7_ushort16 prefixLen);

/****************************************************************************
* @purpose  Get BGP trap Mode
*
* @param    *prefix             @b{{input}} mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpTrapModeGet(L7_uint32 *trapMode);

/****************************************************************************
* @purpose  Set BGP trap Mode
*
* @param    *prefix             @b{{input}} mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpTrapModeSet(L7_uint32 trapMode);

/****************************************************************************
* @purpose  Add MPLS lable
*
* @param    *prefix            @b{{input}} BGP IP address
* @param    prefixLen          @b{{input}} prefix length
* @param    *remoteAddr        @b{{input}} BGP IP address
* @param    vpnCos             @b{{input}} VPN/COS
* @param    *p_Lable           @b{{input}} lable to be added
* @param    numOfLables        @b{{input}} number of lables
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpMplsLabelAdd(bgpIpAddr *prefix, L7_ushort16 prefixLen, bgpIpAddr *remoteAddr,
                              L7_ulong32 vpnCos, L7_ulong32 *p_Label, L7_ushort16 numOfLabels);

/****************************************************************************
* @purpose  Delete MPLS lable
*
* @param    *prefix            @b{{input}} BGP IP address
* @param    prefixLen          @b{{input}} prefix length
* @param    *remoteAddr        @b{{input}} BGP IP address
* @param    vpnCos             @b{{input}} VPN/COS
* @param    *p_Lable           @b{{input}} lable to be added
* @param    numOfLables        @b{{input}} number of lables
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpMplsLabelDelete(bgpIpAddr *prefix, L7_ushort16 prefixLen, bgpIpAddr *remoteAddr,
                              L7_ulong32 vpnCos, L7_ulong32 *p_Label, L7_ushort16 numOfLabels);

/****************************************************************************
* @purpose  Update MPLS lable
*
* @param    *prefix            @b{{input}} BGP IP address
* @param    prefixLen          @b{{input}} prefix length
* @param    *remoteAddr        @b{{input}} BGP IP address
* @param    vpnCos             @b{{input}} VPN/COS
* @param    *p_Lable           @b{{input}} lable to be added
* @param    numOfLables        @b{{input}} number of lables
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpMplsLabelUpdate(bgpIpAddr *prefix, L7_ushort16 prefixLen, bgpIpAddr *remoteAddr,
                              L7_ulong32 vpnCos, L7_ulong32 *p_Label, L7_ushort16 numOfLabels);

/****************************************************************************
* @purpose  Get MPLS lable
*
* @param    *prefix            @b{{input}} BGP IP address
* @param    prefixLen          @b{{input}} prefix length
* @param    *remoteAddr        @b{{input}} BGP IP address
* @param    vpnCos             @b{{input}} VPN/COS
* @param    *p_Lable           @b{{input}} lable to be added
* @param    numOfLables        @b{{input}} number of lables
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpMplsLabelGet(bgpIpAddr *prefix, L7_ushort16 prefixLen, bgpIpAddr *remoteAddr,
                              L7_ulong32 vpnCos, L7_ulong32 *p_Label, L7_ushort16 *numOfLabels);

/****************************************************************************
* @purpose  Get VPN/COS ID
*
* @param    *prefix             @b{{input}} BGP IP address
* @param    *prefixLen          @b{{input}} prefix length
* @param    *remoteAddr         @b{{input}} BGP IP address
* @param    vpnCos              @b{{input}} VPN/COS
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Peer
*
* @end
*****************************************************************************/

L7_RC_t bgpMapBgpVpnCosIdGet(bgpIpAddr *prefix, L7_ushort16 prefixLen, bgpIpAddr *remoteAddr,
                              L7_ulong32 *vpnCos);

/****************************************************************************
* @purpose  Add VPN/COS ID
*
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

L7_RC_t bgpMapBgpVpnCosIdAdd(bgpIpAddr *prefix, L7_ushort16 prefixLen, bgpIpAddr *remoteAddr,
                              L7_ulong32 vpnCos, L7_BOOL sendUpdate);

/****************************************************************************
* @purpose  Delete VPN/COS ID
*
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

L7_RC_t bgpMapBgpVpnCosIdDelete(bgpIpAddr *prefix, L7_ushort16 prefixLen, bgpIpAddr *remoteAddr,
                              L7_ulong32 vpnCos, L7_BOOL sendUpdate);

/**************************************************************************
*
* @purpose  Creates the policy Access List
*
* @param    policyIndex     @b{{input}} Indicates the Index in the Access List
* @param    policyAccess    @b{{input}} Indicates whether it is Permit/Deny
* @param    protocol        @b{{input}} Specifies the protocol intended to add
*                                       the Access List in.
* @param    matchType       @b{{input}} Specifies the Attribute the type the
*                                       range addresses.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyCreate(L7_uint32 policyIndex, L7_uint32 policyAccess, L7_ushort16 protocol,
                              L7_ushort16 matchType);


/**************************************************************************
*
* @purpose  Gets exact policy.
*
* @param    *ptoto          @b{{input}} protocol
* @param    *index          @b{{input}} indicates the Index in the Access List
* @param    *matchType      @b{{input}} policy match type
* @param    *permitFlasg    @b{{input}} permisstion flags
* @param    *numRanges      @b{{input}} number of ranges
* @param    *numActions     @b{{input}} number of actions
* @param    *numRangeArgs   @b{{input}} number of arguments for a range of policy
* @param    *numActionArgs  @b{{input}} number of arguments for a action of policy
* @param    **rangeArgs     @b{{input}} range arguments
* @param    **actionArgs    @b{{input}} actoin arguments
* @param    curIndex       @{{input}}  prevIndex which will tell whether first index
*                            or next index
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if invalid index
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapPolicyEntryGetExact(L7_ushort16 *proto, L7_uint32 index,
                                 L7_ushort16 *matchType, L7_BOOL *permitFlag,
                                 L7_uint32 *numRanges, L7_uint32 *numActions,
                                 bgpPolicyRange_t *ranges, bgpPolicyAction_t *actions);

/**************************************************************************
*
* @purpose  Gets next policy index.
*
* @param    *index          @b{{input}} indicates the Index in the Access List
* @param    prevIndex       @b{{input}} previous index into list
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no entry or invalid index
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapPolicyIndexEntryGetNext(L7_uint32 *policyIndex, L7_uint32 prevIndex);

/**************************************************************************
*
* @purpose  Gets exact policy index.
*
* @param    *index          @b{{input}} indicates the Index in the Access List
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapPolicyIndexEntryGetExact(L7_uint32 *index);

/**************************************************************************
*
* @purpose  add/Modify a Action (type of action specified by matchType)
*           to the Policy Access List
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    matchType       @b{{input}} attribute type the action to be applied to.
* @param    modifier        @b{{input}} whether it is Add/Modify/Remove action.
* @param    val             @b{{input}} the value for the action.
* @param    val2            @b{{input}} optional value for the action.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space is left of invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyActionSet(L7_uint32 policyIndex, L7_uint32 matchType, L7_uint32 modifier, L7_uint32 val,
                                 L7_int32 val2);

/**************************************************************************
*
* @purpose  Sets address range for a specified policy
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    ipAddr          @b{{input}} IP address for the range
* @param    mask            @b{{input}} mask to be assocaited to the range.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space if available or invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeAddressSet(L7_uint32 policyIndex, L7_uint32 ipAddr, L7_uint32 mask);

/**************************************************************************
*
* @purpose  Adds a Range policy of "between type" to the Policy Access List
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    Minval          @b((input}} minimal value for the range
* @param    Maxval          @b{{input}} maximum value for the range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space is available or invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeBetweenSet(L7_uint32 policyIndex, L7_uint32 minVal, L7_uint32 maxVal);

/**************************************************************************
*
* @purpose  Adds a Range policy of "greaterthan type" to the Policy Access List
*
* @param    policyIndex     @b{{input}} Indicates the Index in the Access List
* @param    val             @b{{input}} Indicates the Val value for the range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space or invalid index is passed
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeGreaterthanSet(L7_uint32 policyIndex, L7_uint32 val);

/**************************************************************************
*
* @purpose  Adds a Range policy of "lessthan type" to the Policy Access List
*
* @param    policyIndex     @b{{input}} Indicates the Index in the Access List
* @param    val             @b{{input}} Indicates the Minimal value for the range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space or invalid index is passed
*
* @notes
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeLessthanSet(L7_uint32 policyIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose  Adds a Range to the Policy Access List
*
* @param    policyIndex @b{{input}}  Indicates the Index in the Access List
* @param    val         @b{{input}}  Indicates the value for the range
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no space or invalid Index
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpPolicyRangeEqualSet(L7_uint32 policyIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose  Adds a Range to the Policy Access List
*
* @param    policyIndex @b{{input}}  Indicates the Index in the Access List
* @param    valstr      @b{{input}}  Indicates the value for the range
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if no space or invalid Index
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpPolicyRangeMatchSet(L7_uint32 policyIndex, L7_char8 *valstr);

/**************************************************************************
*
* @purpose  Gets address range for a specified policy
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    *ipAddr         @b{{input}} IP address for the range
* @param    *mask           @b{{input}} mask to be assocaited to the range.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space if available or invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeAddressGet(L7_uint32 policyIndex, L7_uint32 *ipAddr, L7_uint32 *mask);

/**************************************************************************
*
* @purpose  Removes a Range from the Policy Access List
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    matchIndex      @b{{input}} attribute type the range to be applied to.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeRemove(L7_uint32 policyIndex, L7_uint32 matchType);

/**************************************************************************
*
* @purpose  Gets a Range policy of "between type" to the Policy Access List
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    *minVal         @b((input}} minimal value for the range
* @param    *maxVal         @b{{input}} maximum value for the range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space is available or invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeBetweenGet(L7_uint32 policyIndex, L7_uint32 *minVal, L7_uint32 *maxVal);

/**************************************************************************
*
* @purpose  Gets a Range policy of "greaterthan type" to the Policy Access List
*
* @param    policyIndex     @b{{input}} Indicates the Index in the Access List
* @param    *val            @b{{input}} Indicates the Val value for the range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index is passed
*
* @notes    BGP policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeGreaterthanGet(L7_uint32 policyIndex, L7_uint32 *val);

/**************************************************************************
*
* @purpose  Gets a Range policy of "lessthan type" to the Policy Access List
*
* @param    policyIndex     @b{{input}} Indicates the Index in the Access List
* @param    *val             @b{{input}} Indicates the Minimal value for the range
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index is passed
*
* @notes
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyRangeLessthanGet(L7_uint32 policyIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets a Range to the Policy Access List
*
* @param    policyIndex @b{{input}}  Indicates the Index in the Access List
* @param    *val         @b{{input}}  Indicates the value for the range
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid index
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpPolicyRangeEqualGet(L7_uint32 policyIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets the next Range of "equal" type after the given value
*           from the Policy Access List
*
* @param    policyIndex @b{{input}}  Indicates the Index in the Access List
* @param    val         @b{{input}}  IN variable for "previous" value
*                                    (-1 means get first value found).
*                                    OUT variable to return found value.
*
* @returns  L7_SUCCESS  if "next" value is found.
* @returns  L7_FAILURE  if invalid index, or "next" not found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpPolicyRangeEqualGetNext(L7_uint32 policyIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets a Range to the Policy Access List
*
* @param    policyIndex @b{{input}}  Indicates the Index in the Access List
* @param    valstr      @b{{input}}  Indicates the value for the range
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid index
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpPolicyRangeMatchGet(L7_uint32 policyIndex, L7_char8 *valstr);

/*********************************************************************
*
* @purpose  Gets the next Range of "match" type after the given value
*           from the Policy Access List
*
* @param    policyIndex @b{{input}}  Indicates the Index in the Access List
* @param    val         @b{{input}}  IN variable for "previous" value
*                                    (empty string means get first value found).
*                                    OUT variable to return found value.
*
* @returns  L7_SUCCESS  if "next" value is found.
* @returns  L7_FAILURE  if invalid index, or "next" not found.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t bgpMapBgpPolicyRangeMatchGetNext(L7_uint32 policyIndex, L7_char8 *val);

/**************************************************************************
*
* @purpose  add/Modify a Action to the Policy Access List
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    matchType       @b{{input}} attribute type the action to be applied to.
* @param    modifier        @b{{input}} whether it is Add/Modify/Remove action.
* @param    val             @b{{input}} the value for the action.
* @param    val2            @b{{input}} optional value for the action.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no space is left of invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyActionAdd(L7_uint32 policyIndex, L7_uint32 matchType, L7_uint32 modifier,
                                 L7_uint32 val, L7_int32 val2);

/**************************************************************************
*
* @purpose  Removes a Action to the Policy Access List
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    matchIndex      @b{{input}} attribute type the action to be applied to.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyActionRemove(L7_uint32 policyIndex, L7_uint32 matchType);

/**************************************************************************
*
* @purpose  Deletes the Policy Access Object
*
* @param    policyIndex     @b{{input}} index in the Access List
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyDeleteIndex(L7_uint32 policyIndex);

/**************************************************************************
*
* @purpose  Gets a Action info (type of action specified by matchType)
*           of a specified policy.
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    matchType       @b{{input}} attribute type the action to be applied to.
* @param    *modifier       @b{{input}} whether it is Add/Modify/Remove action.
* @param    *val            @b{{input}} the value for the action.
* @param    *val2           @b{{input}} optional value for the action.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyActionGet(L7_uint32 policyIndex, L7_uint32 matchType, L7_uint32 *modifier,
                                 L7_uint32 *val, L7_int32 *val2);
/**************************************************************************
*
* @purpose  Gets First Action for the specified poilicy
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    *prevIndex      @b{{input}} prevous index
* @param    *matchType      @b{{input}} attribute type the action to be applied to.
* @param    *modifier       @b{{input}} whether it is Add/Modify/Remove action.
* @param    *val            @b{{input}} value for the action.
* @param    *val2           @b{{input}} optional value for the action.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no actoins were added or invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyActionGetFirst(L7_uint32 policyIndex, L7_uint32 *prevIndex, L7_uint32 *matchType,
                                      L7_uint32 *modifier, L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Gets Next Action for the specified policy
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    *prevIndex      @b{{input}} prevous index
* @param    *matchType      @b{{input}} attribute type the action to be applied to.
* @param    *modifier       @b{{input}} whether it is Add/Modify/Remove action.
* @param    *val            @b{{input}} value for the action.
* @param    *val2           @b{{input}} optional value for the action.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if no policy were added or invalid inedx is added
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyActionGetNext(L7_uint32 policyIndex, L7_uint32 *prevIndex, L7_uint32 *matchType,
                                     L7_uint32 *modifier, L7_uint32 *val, L7_int32 *val2);

/**************************************************************************
*
* @purpose  Get Exact Action for the specified policy
*
* @param    policyIndex     Indicates the Index in the Access List
* @param    *exactIndex     Ptr to the prevous index
* @param    *matchType      Indicates the attribute type the action to be applied to.
* @param    *modifier       Indicates whether it is Add/Modify/Remove action.
* @param    *val            Indicates the value for the action.
* @param    *val2           Indicates the optional value for the action.
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE      if no action were added or invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyActionGetExact(L7_uint32 policyIndex, L7_uint32 *exactIndex, L7_uint32 *matchType,
                                      L7_uint32 *modifier, L7_uint32 *val, L7_int32 *val2);


/**************************************************************************
*
* @purpose  Gets matchType for a pecified policy
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    *matchType      @b{{input}} match type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyMatchTypeGet(L7_uint32 policyIndex, L7_ushort16 *matchType);


/**************************************************************************
*
* @purpose  Gets access for a pecified policy
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    *access         @b{{input}} access
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyAccessGet(L7_uint32 policyIndex, L7_BOOL *access);

/**************************************************************************
*
* @purpose  Gets protocol for a pecified policy
*
* @param    policyIndex     @b{{input}} index in the Access List
* @param    *protocol       @b{{input}} match type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE      if invalid index is passed
*
* @notes    BGP Policy
*
* @end
*************************************************************************/

L7_RC_t bgpMapBgpPolicyProtocolGet(L7_uint32 policyIndex, L7_ushort16 *protocol);



/**************************************************************************
* @purpose  Specify whether BGP is allowed to redistribute a default route.
*
* @param    val - TRUE if BGP can redistribute the default route.
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistDefRouteSet(L7_BOOL val);

/**************************************************************************
* @purpose  Specify whether BGP is allowed to redistribute a default route.
*
* @param    val - TRUE if BGP can redistribute the default route.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if val is NULL
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistDefRouteGet(L7_BOOL *val);

/**************************************************************************
* @purpose  Set a default metric for BGP to use as the MED when it
*           redistributes routes from another protocol.
*
* @param    defMetric - The default metric.
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpDefaultMetricSet(L7_uint32 defMetric);

/**************************************************************************
* @purpose  Clear the default metric that BGP uses as the MED when it
*           redistributes routes from another protocol.
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpDefaultMetricClear();

/**************************************************************************
* @purpose  Get the default metric that BGP uses as the MED when it
*           redistributes routes from another protocol.
*
* @param    defMetric - return value. Only valid if L7_SUCCESS returned.
*
* @returns  L7_SUCCESS if successful.
* @returns  L7_FAILURE if defMetric is NULL
* @returns  L7_NOT_EXIST if no default metric is configured.
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpDefaultMetricGet(L7_uint32 *defMetric);

/**************************************************************************
* @purpose  Set route redistribution options for routes learned from
*           a specific source.
*
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
L7_RC_t bgpMapBgpRedistributionSet(L7_REDIST_RT_INDICES_t sourceProto,
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
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistribute - true if BGP should redistribute from sourceProto.
*
* @returns  L7_SUCCESS if configuration successfully saved
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistributeSet(L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL redistribute);

/**************************************************************************
* @purpose  Get whether BGP redistributes routes from a given source.
*
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistribute - set to true if BGP redistributes from sourceProto.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid or redistribute is NULL
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistributeGet(L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_BOOL *redistribute);

/**************************************************************************
* @purpose  For a given source of redistributed routes, revert the
*           redistribution configuration to the defaults.
*
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
L7_RC_t bgpMapBgpRedistributeRevert(L7_REDIST_RT_INDICES_t sourceProto);

/**************************************************************************
* @purpose  Specify an access used to filter routes when BGP redistributes
*           routes from a given source.
*
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    distList - Number of access list to use to filter routes from
*                      sourceProto
*
* @returns  L7_SUCCESS if configuration successfully saved
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistributeDistListSet(L7_REDIST_RT_INDICES_t sourceProto,
                                         L7_uint32 distList);

/**************************************************************************
* @purpose  No longer use an access list to filter routes when BGP redistributes
*           routes from a given source.
*
* @param    sourceProto - Source protocol providing routes for redistribution
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistributeDistListClear(L7_REDIST_RT_INDICES_t sourceProto);

/**************************************************************************
* @purpose  Get number of the access list used to filter routes when BGP
*           redistributes routes from a given source.
*
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
L7_RC_t bgpMapBgpRedistributeDistListGet(L7_REDIST_RT_INDICES_t sourceProto,
                                         L7_uint32 *distList);

/**************************************************************************
* @purpose  Set redistribution metric.
*
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistMetric - BGP will set the MED to this value for routes
*                          it redistributes from sourceProto
*
* @returns  L7_SUCCESS if configuration successfully saved
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistMetricSet(L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_uint32 redistMetric);

/**************************************************************************
* @purpose  Clear the redistribution metric.
*
* @param    sourceProto - Source protocol providing routes for redistribution
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistMetricClear(L7_REDIST_RT_INDICES_t sourceProto);

/**************************************************************************
* @purpose  Get redistribution metric.
*
* @param    sourceProto - Source protocol providing routes for redistribution
* @param    redistMetric - return value.
*
* @returns  L7_SUCCESS if successful
* @returns  L7_FAILURE if sourceProto is invalid or redistMetric is NULL
* @returns  L7_NOT_EXIST if no redistribution metric is set
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpRedistMetricGet(L7_REDIST_RT_INDICES_t sourceProto,
                                 L7_uint32 *redistMetric);

/**************************************************************************
* @purpose  Specify the types of OSPF routes BGP will redistribute.
*
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
L7_RC_t bgpMapBgpRedistOspfTypesSet(L7_BOOL matchInternal,
                                    L7_BOOL matchExtType1,
                                    L7_BOOL matchExtType2,
                                    L7_BOOL matchNssaExtType1,
                                    L7_BOOL matchNssaExtType2);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF internal routes.
*
* @param    matchInternal - true if BGP may redistribute OSPF internal routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpMatchOspfInternalSet(L7_BOOL matchInternal);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF external type 1 routes.
*
* @param    matchExtType1 - true if BGP may redistribute OSPF ext type 1 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpMatchOspfExt1Set(L7_BOOL matchExtType1);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF external type 2 routes.
*
* @param    matchExtType2 - true if BGP may redistribute OSPF ext type 2 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpMatchOspfExt2Set(L7_BOOL matchExtType2);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF NSSA external
*           type 1 routes.
*
* @param    matchNssaExt1 - true if BGP may redistribute OSPF NSSA
*                           external type 1 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpMatchOspfNssaExt1Set(L7_BOOL matchNssaExt1);

/**************************************************************************
* @purpose  Specify whether BGP may redistribute OSPF NSSA external
*           type 2 routes.
*
* @param    matchNssaExt2 - true if BGP may redistribute OSPF NSSA
*                           external type 2 routes
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t bgpMapBgpMatchOspfNssaExt2Set(L7_BOOL matchNssaExt2);

/**************************************************************************
* @purpose  Get the types of OSPF routes BGP will redistribute.
*
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
L7_RC_t bgpMapBgpRedistOspfTypesGet(L7_BOOL *matchInternal,
                                    L7_BOOL *matchExtType1,
                                    L7_BOOL *matchExtType2,
                                    L7_BOOL *matchNssaExtType1,
                                    L7_BOOL *matchNssaExtType2);

/*********************************************************************
* @purpose  Indicates whether a sourceIndex is a valid
*           source from which BGP may redistribute.
*
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
L7_RC_t bgpMapRouteRedistributeGet(L7_REDIST_RT_INDICES_t sourceIndex);

/*********************************************************************
* @purpose  Given a route redistribution source, set nextSourceIndex
*           to the next valid source of redistributed routes.
*
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
L7_RC_t bgpMapRouteRedistributeGetNext(L7_REDIST_RT_INDICES_t sourceIndex,
                                       L7_REDIST_RT_INDICES_t *nextSourceIndex);

/*********************************************************************
* @purpose  Convert a BGP community number to a string.
*
* @param    community - A BGP community value
* @param    commStr - The community value converted to a string. commStr
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
L7_RC_t bgpMapCommunityString(L7_uint32 community, L7_uchar8 *commStr);

#endif /* L7_BGP_API_H */

