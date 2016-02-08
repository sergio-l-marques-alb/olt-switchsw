/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   usmdb_mib_pimsm_api.h
*
* @purpose    Api functions for the Protocol Independent Multicast MIB, as
*             specified in RFC 2934
*
* @component  usmdb
*
* @comments   none
*
* @create     04/05/2002
*
* @author     Nitish
* @end
*
**********************************************************************/

#ifndef _USMDB_MIB_PIMSM_API_H_
#define _USMDB_MIB_PIMSM_API_H_


#include "l3_comm_structs.h"
#include "osapi.h"


/*********************************************************************
* @purpose  Gets the IP Address for the specified interface
*
* @param    UnitIndex   @b{(input)}  UnitIndex.
* @param    family      @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  internal Interface number
* @param    ipAddr      @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceIPAddressGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 intIfNum, L7_inet_addr_t *ipAddr);
/*********************************************************************
* @purpose  Gets the Subnet Mask for the specified interface
*
* @param    UnitIndex   @b{(input)}  UnitIndex.
* @param    family      @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  internal Interface number
* @param    netMask     @b{(output)} Subnet Mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR   if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceNetMaskGet(L7_uint32 UnitIndex, L7_uchar8 family,
                               L7_uint32 intIfNum, L7_inet_addr_t *netMask);
/*********************************************************************
* @purpose  Sets the admin mode for the specified interface
*
* @param    UnitIndex   @b{(input)}  UnitIndex.
* @param    family      @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface number
* @param    mode        @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceModeSet(L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 intIfNum, L7_uint32 mode);
/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    UnitIndex   @b{(input)}  UnitIndex.
* @param    family      @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface number
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceModeGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 intIfNum, L7_uint32 *mode);
/*********************************************************************
* @purpose  Get the DR IP address for the specified specified interface
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface number
* @param    dsgRtrIpAddr @b{(output)}  L7_uint32   IP Address of the DR
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceDRGet(L7_uint32 UnitIndex, L7_uchar8 family,
                       L7_uint32 intIfNum, L7_inet_addr_t *dsgRtrIpAddr);
/*********************************************************************
* @purpose  Sets the Hello Interval for the specified interface
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface number
* @param    helloIntvl   @b{(input)}  seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceHelloIntervalSet(L7_uint32 UnitIndex,
                 L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 helloIntvl);
/*********************************************************************
* @purpose  Gets the Hello Interval for the specified interface
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface number
* @param    helloIntvl   @b{(output)} seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceHelloIntervalGet(L7_uint32 UnitIndex,
              L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *helloIntvl);
/*********************************************************************
* @purpose  Sets the Join/Prune Interval for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    joinpruneIntvl @b{(input)}  Join/Prune Interval in seconds
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceJoinPruneIntervalSet(L7_uint32 UnitIndex,
          L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 joinpruneIntvl);
/*********************************************************************
* @purpose  Gets the Join/Prune Interval for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    joinpruneIntvl @b{(output)} Join/Prune Interval in seconds
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceJoinPruneIntervalGet(L7_uint32 UnitIndex, 
             L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 *joinpruneIntvl);
/*********************************************************************
* @purpose  Sets the Candidate BSR Preference for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    candBSRPref    @b{(input)}  Candidate BSR Preference
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceCBSRPreferenceSet(L7_uint32 UnitIndex,
             L7_uchar8 family, L7_uint32 intIfNum, L7_int32 candBSRPref);
/*********************************************************************
* @purpose  Gets the Candidate BSR Preference for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    candBSRPref    @b{(output)} Candidate BSR Preference
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceCBSRPreferenceGet(L7_uint32 UnitIndex,
           L7_uchar8 family,L7_uint32 intIfNum, L7_int32 *candBSRPref);
/*********************************************************************
* @purpose  Sets the Candidate BSR Hash mask Length for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    hashMskLen     @b{(input)}  Candidate BSR Hash Mask Length
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceCBSRHashMaskLengthSet(L7_uint32 UnitIndex,
            L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 hashMskLen);
/*********************************************************************
* @purpose  Gets the Candidate BSR Hash mask Length for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    hashMskLen     @b{(output)} Candidate BSR Hash mask Length
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceCBSRHashMaskLengthGet(L7_uint32 UnitIndex,
            L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *hashMskLen);
/*********************************************************************
* @purpose  Sets the Candidate CRP Preference for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    candRPPref     @b{(input)}  Candidate RP Preference
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceCRPPreferenceSet(L7_uint32 UnitIndex,
             L7_uchar8 family, L7_uint32 intIfNum, L7_int32 candRPPref);
/*********************************************************************
* @purpose  Gets the Candidate RP Preference for the specified interface
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    candRPPref     @b{(output)} Candidate RP Preference
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceCRPPreferenceGet(L7_uint32 UnitIndex,
               L7_uchar8 family, L7_uint32 intIfNum, L7_int32 *candRPPref);

/*********************************************************************
* @purpose  Get the Uptime for the specified neighbour
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    nbrIpAddr      @b{(input)}  IP Address of Neighbor
* @param    nbrUpTime      @b{(output)}   Uptime in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        if neighbor does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborUpTimeGet(L7_uint32 UnitIndex,
                                    L7_uchar8 family,
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *nbrIpAddr,
                                    L7_uint32 *nbrUpTime);
/*********************************************************************
* @purpose  Get the Expiry time for the specified neighbour
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    nbrIpAddr      @b{(input)}  IP Address of Neighbor
* @param    nbrExpiryTime  @b{(output)} Expiry time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        if neighbor does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborExpiryTimeGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family,
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t *nbrIpAddr,
                                        L7_uint32 *nbrExpiryTime);
/*********************************************************************
* @purpose  Gets the Neighbor DR Priority for the specified interface.
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface number
* @param    nbrIpAddr      @b{(input)}  IP Address of Neighbor
* @param    priority       @b{(output)} Neighbor DR Priority.
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborDRPriorityGet(L7_uint32 unitIndex,
                                        L7_uchar8 family,
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t *nbrIpAddr,
                                        L7_uint32 *priority);
/*********************************************************************
* @purpose  Get time remaining before the router changes its upstream
*           neighbor back to its RPF neighbor
*
* @param    UnitIndex          @b{(input)}  Unit Index.
* @param    family             @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  Mroute Group address.
* @param    ipMRouteSource     @b{(input)}  Mroute Source address.
* @param    ipMRouteSourceMask @b{(input)}  Mroute Source Mask.
* @param    assertTimer        @b{(output)} assert Timer in seconds.
*
* @returns  L7_SUCCESS         if success
* @returns  L7_ERROR           if entry does not exist
* @returns  L7_FAILURE         if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteUpstreamAssertTimerGet(L7_uint32 UnitIndex,
                                           L7_uchar8 family,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_inet_addr_t *ipMRouteSource,
                                           L7_inet_addr_t *ipMRouteSourceMask,
                                           L7_uint32 *assertTimer);
/*********************************************************************
* @purpose  Get metric advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is in received
*
* @param    UnitIndex          @b{(input)}  unitIndex
* @param    family             @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  MRoute Group Address
* @param    ipMRouteSource     @b{(input)}  MRoute Source Address
* @param    ipMRouteSourceMask @b{(input)}  MRoute Source Mask
* @param    assertMetric       @b{(output)} assertMetric
*
* @returns  L7_SUCCESS         if success
* @returns  L7_ERROR           if route entry does not exist
* @returns  L7_FAILURE         if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteAssertMetricGet(L7_uint32 UnitIndex,
                                          L7_uchar8 family,
                                          L7_inet_addr_t *ipMRouteGroup,
                                          L7_inet_addr_t *ipMRouteSource,
                                          L7_inet_addr_t *ipMRouteSourceMask,
                                          L7_uint32 *assertMetric);
/*********************************************************************
* @purpose  Get preference advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is in received
*
* @param    UnitIndex          @b{(input)}  UnitIndex.
* @param    family             @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  MRouteGroup Address
* @param    ipMRouteSource     @b{(input)}  MRouteSource Address
* @param    ipMRouteSourceMask @b{(input)}  MRoute Source Mask
* @param    assertMetricPref   @b{(output)} assertMetricPref
*
* @returns  L7_SUCCESS         if success
* @returns  L7_ERROR           if route entry does not exist
* @returns  L7_FAILURE         if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteAssertMetricPrefGet(L7_uint32 UnitIndex,
                                          L7_uchar8 family,
                                          L7_inet_addr_t *ipMRouteGroup,
                                          L7_inet_addr_t *ipMRouteSource,
                                          L7_inet_addr_t *ipMRouteSourceMask,
                                          L7_uint32 *assertMetricPref);
/*********************************************************************
* @purpose  Get the value of the RPT-bit advertised by the assert winner on
*           the upstream interface, or false if no such assert is in effect
*
* @param    UnitIndex          @b{(input)}  UnitIndex.
* @param    family             @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  MRouteGroup Address
* @param    ipMRouteSource     @b{(input)}  MRouteSource Address
* @param    ipMRouteSourceMask @b{(input)}  MRoute Source Mask
* @param    assertRPTBit       @b{(output)} assertRPTBit
*
* @returns  L7_SUCCESS         if success
* @returns  L7_ERROR           if route entry does not exist
* @returns  L7_FAILURE         if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteAssertRPTBitGet(L7_uint32 UnitIndex,
                                          L7_uchar8 family,
                                          L7_inet_addr_t *ipMRouteGroup,
                                          L7_inet_addr_t *ipMRouteSource,
                                          L7_inet_addr_t *ipMRouteSourceMask,
                                          L7_uint32 *assertRPTBit);
/*********************************************************************
* @purpose  Get PIM-specific flags related to a multicast state entry
*
* @param    UnitIndex          @b{(input)}  UnitIndex.
* @param    family             @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  MRouteGroup Address
* @param    ipMRouteSource     @b{(input)}  MRouteSource Address
* @param    ipMRouteSourceMask @b{(input)}  MRoute Source Mask
* @param    ipMRouteFlags      @b{(output)} ipMRouteFlags
*
* @returns  L7_SUCCESS         if success
* @returns  L7_ERROR           if route entry does not exist
* @returns  L7_FAILURE         if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteFlagsGet(L7_uint32 UnitIndex,
                                   L7_uchar8 family,
                                   L7_inet_addr_t *ipMRouteGroup,
                                   L7_inet_addr_t *ipMRouteSource,
                                   L7_inet_addr_t *ipMRouteSourceMask,
                                   L7_uint32 *ipMRouteFlags);
/*********************************************************************
* @purpose  Gets the reason why the downstream interface was pruned
*
* @param    UnitIndex                 @b{(input)}  UnitIndex.
* @param    family                    @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup      @b{(input)}  MRouteNextHopGroup Address
* @param    ipMRouteNextHopSource     @b{(input)}  MRouteNextHopSource Address
* @param    ipMRouteNextHopSourceMask @b{(input)}  MRouteNextHopSource Mask
* @param    ipMRouteNextHopIfIndex    @b{(input)}  MRouteNextHop IfIndex
* @param    ipMRouteNextHopAddres     @b{(input)}  MRouteNextHopAddres
* @param    pruneReason               @b{(output)  pruneReason
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteNextHopPruneReasonGet(L7_uint32 UnitIndex,
                                    L7_uchar8 family,
                                    L7_inet_addr_t *ipMRouteNextHopGroup,
                                    L7_inet_addr_t *ipMRouteNextHopSource,
                                    L7_inet_addr_t *ipMRouteNextHopSourceMask,
                                    L7_uint32 ipMRouteNextHopIfIndex,
                                    L7_inet_addr_t *ipMRouteNextHopAddress,
                                    L7_uint32 *pruneReason);
/************************************************************************
* @purpose  Get the holdtime of a Candidate-RP.  If the local router is not
*           the BSR, this value is 0.
*
* @param    UnitIndex                 @b{(input)}  UnitIndex.
* @param    family                    @b{(input)}  Address Family type
* @param    rpSetGroupAddress         @b{(input)}  rp Group Address
* @param    rpSetGroupMask            @b{(input)}  rp Group Mask
* @param    rpSetAddress              @b{(input)}  rp Address
* @param    rpSetComponent            @b{(input)}  rp Componet
* @param    rpSetHoldTime             @b{(output)} seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmRPSetHoldTimeGet(L7_uint32 UnitIndex,
                                   L7_uchar8 family,
                                   L7_inet_addr_t *rpSetGroupAddress,
                                   L7_inet_addr_t *rpSetGroupMask,
                                   L7_inet_addr_t *rpSetAddress,
                                   L7_uint32 rpSetComponent,
                                   L7_uint32 *rpSetHoldTime);
/************************************************************************
* @purpose  Get the minimum time remaining before the Candidate-RP will be
*           declared down
*
* @param    UnitIndex                 @b{(input)}  UnitIndex.
* @param    family                    @b{(input)}  Address Family type
* @param    rpSetGroupAddress         @b{(input)}  rp Group Address
* @param    rpSetGroupMask            @b{(input)}  rp Group Mask
* @param    rpSetAddress              @b{(input)}  rp Address
* @param    rpSetComponent            @b{(input)}  rp Componet
* @param    rpSetExpiryTime           @b{(output)} seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmRPSetExpiryTimeGet(L7_uint32 UnitIndex,
                                     L7_uchar8 family,
                                     L7_inet_addr_t *rpSetGroupAddress,
                                     L7_inet_addr_t *rpSetGroupMask,
                                     L7_inet_addr_t *rpSetAddress,
                                     L7_uint32 rpSetComponent,
                                     L7_uint32 *rpSetExpiryTime);
/************************************************************************
* @purpose  Get the unicast address of the interface which will
*           be advertised as a Candidate RP
*
* @param    UnitIndex                 @b{(input)}  UnitIndex.
* @param    family                    @b{(input)}  Address Family type
* @param    candidateRPGroupAddress   @b{(input)}  Candidate Group Address
* @param    candidateRPGroupMask      @b{(input)}  Candidate Group Mask
* @param    ipAddress                 @b{(output)} Ip Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmCandRPAddressGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family,
                                        L7_inet_addr_t *candidateRPGroupAddress,
                                        L7_inet_addr_t *candidateRPGroupMask,
                                        L7_inet_addr_t *ipAddress);
/************************************************************************
* @purpose  Get IP address of the bootstrap router (BSR) for the local
*           PIM region
*
* @param    UnitIndex                 @b{(input)}  UnitIndex.
* @param    family                    @b{(input)}  Address Family type
* @param    elecBSRAddress            @b{(output)} bootstrap router Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmElectedBSRAddressGet(L7_uint32 UnitIndex,
                                       L7_uchar8 family,
                                       L7_inet_addr_t *elecBSRAddress);
/************************************************************************
* @purpose  Get minimum time remaining before the bootstrap router in
*           the local domain will be declared down
*
* @param    UnitIndex                 @b{(input)}  UnitIndex.
* @param    family                    @b{(input)}  Address Family type
* @param    bsrExpiryTime             @b{(output)} seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmElectedBSRExpiryTimeGet(L7_uint32 UnitIndex,
                                          L7_uchar8 family,
                                          L7_uint32 *bsrExpiryTime);
/************************************************************************
* @purpose  Get the hold time of the component when it is a
*           candidate RP in the local domain
*
* @param    UnitIndex                 @b{(input)}  UnitIndex.
* @param    family                    @b{(input)}  Address Family type
* @param    elecCRPAdvTime            @b{(output)} seconds
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmElectedCRPHoldTimeGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family,
                                        L7_uint32 *elecCRPAdvTime);
/*********************************************************************
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal interface number
*                                       on which neighbor is lost.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmPimTrapNeighborLoss(L7_uint32 UnitIndex, L7_uchar8 family,
                                      L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Check whether interface entry exists for the specified interface
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal interface number
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceEntryGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                    L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Get the existing interface entry next to the entry of the
*           specified interface
*
* @param    UnitIndex    @b{(input)} UnitIndex.
* @param    family       @b{(input)} Address Family type
* @param    intIfNum     @b{(inout)} internal Interface number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceEntryNextGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family,
                                        L7_uint32 *intIfNum);
/*********************************************************************
* @purpose  Check whether neighbor entry exists for the specified IP address
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  internal Interface number
* @param    ipAddress    @b{(output)}  IP Address
*
* @returns  L7_SUCCESS    if neighbor entry exists
* @returns  L7_FAILURE    if neighbor entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborEntryGet(L7_uint32 UnitIndex, L7_uchar8 family,
                               L7_uint32 intIfNum, L7_inet_addr_t *ipAddress);
/*********************************************************************
* @purpose  Get the existing neighbor entry next to the entry of the
*           specified neighbor
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(inout)}  internal Interface number
* @param    ipAddress    @b{(inout)}  IP Address
*
* @returns  L7_SUCCESS    if neighbor entry exists
* @returns  L7_FAILURE    if neighbor entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborEntryNextGet(L7_uint32 UnitIndex, L7_uchar8 family,
                              L7_uint32 *intIfNum, L7_inet_addr_t *ipAddress);
/*********************************************************************
* @purpose  Check whether IP Multicast route entry exists for the specified
*           group, source and mask
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    ipMRouteGroup  @b{(input)}  IP Multicast route group
* @param    ipMRouteSource @b{(input)}  IP Multicast route source
* @param    ipMRouteMask   @b{(input)}  IP Multicast route mask
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteEntryGet(L7_uint32 UnitIndex,
                                   L7_uchar8 family,
                                   L7_inet_addr_t *ipMRouteGroup,
                                   L7_inet_addr_t *ipMRouteSource,
                                   L7_inet_addr_t *ipMRouteSourceMask);
/*********************************************************************
* @purpose  Get the existing IP Multicast Route entry next to the entry of the
*           specified group, source and mask
*
* @param    UnitIndex      @b{(input)}  UnitIndex.
* @param    family         @b{(input)}  Address Family type
* @param    ipMRouteGroup  @b{(inout)}  IP Multicast route group
* @param    ipMRouteSource @b{(inout)}  IP Multicast route source
* @param    ipMRouteMask   @b{(inout)}  IP Multicast route mask
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteEntryNextGet(L7_uint32 UnitIndex,
                                       L7_uchar8 family,
                                       L7_inet_addr_t *ipMRouteGroup,
                                       L7_inet_addr_t *ipMRouteSource,
                                       L7_inet_addr_t *ipMRouteSourceMask);
/*********************************************************************
* @purpose  Check whether IP Multicast route next hop entry exists for
*           the specified group, source, mask, interface index and address
*
* @param    UnitIndex              @b{(input)}  UnitIndex.
* @param    family                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup   @b{(input)}  IP Multicast route next
*                                               hop group
* @param    ipMRouteNextHopSource  @b{(input)}  IP Multicast route next
*                                               hop source
* @param    ipMRouteNextHopMask    @b{(input)}  IP Multicast route next
*                                               hop mask
* @param    ipMRouteNextHopIfIndex @b{(input)}  IP Multicast route next
*                                               hop internal interface number
* @param    ipMRouteNextHopAddress @b{(input)}  IP Multicast route next
*                                               hop address
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteNextHopEntryGet(L7_uint32 UnitIndex,
                                       L7_uchar8 family,
                                       L7_inet_addr_t *ipMRouteNextHopGroup,
                                       L7_inet_addr_t *ipMRouteNextHopSource,
                                       L7_inet_addr_t *ipMRouteNextHopMask,
                                       L7_uint32 ipMRouteNextHopIfIndex,
                                       L7_inet_addr_t *ipMRouteNextHopAddress);
/*********************************************************************
* @purpose  Get the existing IP Multicast route next hop entry next to the entry
*           of the specified group, source, mask, interface index and address
*
* @param    UnitIndex              @b{(input)}  UnitIndex.
* @param    family                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup   @b{(inout)}  IP Multicast route next
*                                               hop group
* @param    ipMRouteNextHopSource  @b{(inout)}  IP Multicast route next
*                                               hop source
* @param    ipMRouteNextHopMask    @b{(inout)}  IP Multicast route next
*                                               hop mask
* @param    ipMRouteNextHopIfIndex @b{(inout)}  IP Multicast route next
*                                               hop internal interface number.
* @param    ipMRouteNextHopAddress @b{(inout)}  IP Multicast route next
*                                               hop address.
*
* @returns  L7_SUCCESS       if entry exists
* @returns  L7_FAILURE       if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmIpMRouteNextHopEntryNextGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family,
                                        L7_inet_addr_t *ipMRouteNextHopGroup,
                                        L7_inet_addr_t *ipMRouteNextHopSource,
                                        L7_inet_addr_t *ipMRouteNextHopMask,
                                        L7_uint32 *ipMRouteNextHopIfIndex,
                                        L7_inet_addr_t *ipMRouteNextHopAddress);
/*********************************************************************
* @purpose  Check whether RP to Group mapping entry exists for the specified
*           group address, group mask, address.
*
* @param    UnitIndex       @b{(input)}  UnitIndex.
* @param    family          @b{(input)}  Address Family type
* @param    origin          @b{(input)}  Origin
* @param    groupAddress    @b{(input)}  IP Multicast group address
* @param    groupMask       @b{(input)}  Multicast group address mask
* @param    rpAddress       @b{(input)}  IP address of candidate RP
*
* @returns  L7_SUCCESS          if entry exists
* @returns  L7_FAILURE          if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmRpGrpMappingEntryGet(L7_uint32 UnitIndex,
                                       L7_uchar8 family,
                                       L7_uchar8 origin,
                                       L7_inet_addr_t *groupAddress,
                                       L7_inet_addr_t *groupMask,
                                       L7_inet_addr_t *rpAddress);
/*********************************************************************
* @purpose  Get the existing RP to Group mapping entry next to the
*           entry of the specified group address, group mask,
*           address and component number
*
* @param    UnitIndex       @b{(input)}  UnitIndex.
* @param    family          @b{(input)}  Address Family type
* @param    origin          @b{(inout)}  Origin
* @param    groupAddress    @b{(inout)}  IP Multicast group address
* @param    groupMask       @b{(inout)}  Multicast group address mask
* @param    rpAddress       @b{(inout)}  IP address of candidate RP
*
*
* @returns  L7_SUCCESS           if entry exists
* @returns  L7_FAILURE           if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmRpGrpMappingEntryNextGet(L7_uint32 UnitIndex,
                                           L7_uchar8 family,
                                           L7_uchar8 *origin,
                                           L7_inet_addr_t *groupAddress,
                                           L7_inet_addr_t *groupMask,
                                           L7_inet_addr_t *rpAddress);
/*********************************************************************
* @purpose  Check whether Cand-RP entry exists for the specified group address
*           and group mask
*
* @param    UnitIndex       @b{(input)}  UnitIndex
* @param    family          @b{(input)}  Address Family type
* @param    cRPAddress      @b{(input)}  IP Multicast rp address
* @param    cRPGroupAddress @b{(input)}  IP Multicast group address
* @param    cRPprefixLen    @b{(input)}  prefix Length.
*
* @returns  L7_SUCCESS          if entry exists
* @returns  L7_FAILURE          if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandRPEntryGet(L7_uint32 UnitIndex,
                                      L7_uchar8 family,
                                      L7_inet_addr_t *cRPAddress,
                                      L7_inet_addr_t *cRPGroupAddress,
                                      L7_uchar8       cRPprefixLen);
/*********************************************************************
* @purpose  Get the existing Cand-RP entry next to the entry of the specified
*           group address and group mask
*
* @param    UnitIndex       @b{(input)} UnitIndex
* @param    family          @b{(input)} Address Family type
* @param    cRPAddress      @b{(inout)} IP Multicast rp address
* @param    cRPGroupAddress @b{(inout)} IP Multicast group address
* @param    cRPprefixLen    @b{(inout)} prefix Length.
*
* @returns  L7_SUCCESS           if entry exists
* @returns  L7_FAILURE           if entry does not exist
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandRPEntryNextGet(L7_uint32 UnitIndex,
                                          L7_uchar8 family,
                                          L7_inet_addr_t *cRPAddress,
                                          L7_inet_addr_t *cRPGroupAddress,
                                          L7_uchar8      *cRPprefixLen);
/*********************************************************************
* @purpose  Check whether component entry exists for the specified
*           component index
*
* @param    UnitIndex       @b{(input)} UnitIndex
* @param    family          @b{(input)} Address Family type
* @param    compIndex       @b{(input)} Component Index
*
* @returns  L7_SUCCESS   if entry exists
* @returns  L7_FAILURE   if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmComponentEntryGet(L7_uint32 UnitIndex,
                                    L7_uchar8 family, L7_uint32 compIndex);
/*********************************************************************
* @purpose  Get the existing Elected Bsr entry next.
*
* @param    UnitIndex       @b{(input)} UnitIndex
* @param    family          @b{(input)} Address Family type
*
* @returns  L7_SUCCESS   if entry exists
* @returns  L7_FAILURE   if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmElectedBSREntryNextGet(L7_uint32 UnitIndex, L7_uchar8 family);
/*********************************************************************
* @purpose  Check whether interface entry exists for the specified interface
*           for the interface table of SNMP
*
* @param    UnitIndex       @b{(input)} UnitIndex
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSNMPInterfaceEntryGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family, L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Obtain the next sequential routing interface entry
*           after the entry of the specified interface for the
*           Interface Table of SNMP.
*
* @param    UnitIndex       @b{(input)} UnitIndex
* @param    family          @b{(input)} Address Family type
* @param    *intIfNum       @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSNMPInterfaceEntryNextGet(L7_uint32 UnitIndex,
                                       L7_uchar8 family, L7_uint32 *intIfNum);
/*********************************************************************
* @purpose  Obtain the RP that the specified group will be mapped to.
*
* @param    family         @b{(input)} Address Family type
* @param    *grpIpAddr     @b{(inout)} Group ip address
* @param    *rpIpAddr      @b{(inout)} RP ip address
* @param    *origin        @b{(inout)} origin.
*
* @returns  L7_SUCCESS  If group is successfully mapped to RP.
* @returns  L7_FAILURE  If pimsm is not operational or output parm is null.
* @returns  L7_FAILURE  If no RP can be matched to the group.
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmGroupToRPMappingGet(L7_uchar8 family,
                   L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *rpIpAddr,
                   L7_uchar8 *origin);
/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    family             @b{(input)} Address Family type
* @param    bsrCandtRPAddr     @b{(input)} bsr Candidate Addr
* @param    bsrCandtRPGrpAddr  @b{(input)} bsr Candidate RP Grp Addr
* @param    length             @b{(input)} length of the address
* @param    mode               @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrCandRPModeGet(L7_uchar8 family,
                                        L7_inet_addr_t *bsrCandtRPAddr,
                                        L7_inet_addr_t *bsrCandtRPGrpAddr,
                                        L7_uint32 length, L7_uint32 *mode);
/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    family             @b{(input)} Address Family type
* @param    bsrCandtRPAddr     @b{(input)} bsr Candidate RP Addr
* @param    bsrCandtRPGrpAddr  @b{(input)} bsr Candidate RP Grp Addr
* @param    length             @b{(input)} length of the address
* @param    intIfNum           @b{(output)} interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrCandRPInterfaceGet(L7_uchar8 family, 
                                        L7_inet_addr_t *bsrCandtRPAddr,
                                        L7_inet_addr_t *bsrCandtRPGrpAddr,
                                        L7_uint32 length, L7_uint32 *intIfNum);
/*********************************************************************
* @purpose  Check whether ssm range entry exists.
*
* @param    family            @b{(input)} ipv4/v6
* @param    ssmRangeAddr      @b{(input)} SSM Range Addr
* @param    length            @b{(input)} length of the mask
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSsmRangeEntryGet(L7_uchar8 family,
                                   L7_inet_addr_t *ssmRangeAddr,
                                   L7_uchar8 length);
/*********************************************************************
* @purpose  Obtain next  ssm range entry  in the ssmrange TABLE
*
* @param    family            @b{(input)} ipv4/v6
* @param    ssmRangeAddr      @b{(inout)} SSM Range Addr
* @param    length            @b{(inout)} length of the mask
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSsmRangeEntryNextGet(L7_uchar8 family,
                                       L7_inet_addr_t *ssmRangeAddr,
                                       L7_uchar8 *length);
/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    family           @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
* @param    ssmMode          @b{(output)} ssm Mode
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSsmRangeModeGet(L7_uchar8 family,
                                  L7_inet_addr_t *ssmRangeAddr,
                                  L7_uchar8 prefixLen,
                                  L7_uint32 *ssmMode);
/*********************************************************************
* @purpose  Sets the admin mode for the specified interface
*
* @param    family           @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
* @param    ssmMode          @b{(input)} ssm Mode
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSsmRangeModeSet(L7_uchar8 family,
                                  L7_inet_addr_t *ssmRangeAddr,
                                  L7_uchar8 prefixLen, L7_uint32 ssmMode);
/*********************************************************************
* @purpose  To get the static RP Address.
*
* @param    family           @b{(input)} Address Family type
* @param    rpGrpAddr        @b{(input)} static RP Group address
* @param    length           @b{(input)} length of the address
* @param    staticRPAddr     @b{(output)} Static RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPAddrGet(L7_uchar8 family,
                                  L7_inet_addr_t *rpGrpAddr,
                                  L7_uchar8 length,
                                  L7_inet_addr_t *staticRPAddr);
/*********************************************************************
* @purpose  To set the staticRP Address.
*
* @param    family          @b{(input)} Address Family type
* @param    rpGrpAddr       @b{(input)} static RP Group address
* @param    prefixrLength   @b{(input)} length of the address
* @param    staticRPAddr    @b{(input)} Static RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPAddrSet(L7_uchar8 family,
                                  L7_inet_addr_t *rpGrpAddr,
                                  L7_uint32 length,
                                  L7_inet_addr_t *staticRPAddr);
/*********************************************************************
* @purpose  To get the StaticRPOverrideDynamic.
*
* @param    family          @b{(input)}  Address Family type
* @param    rpGrpAddr       @b{(input)}  static RP Group address
* @param    prefixLength    @b{(input)}  length of the address
* @param    overRide        @b{(output)} Get OverrideDynamic
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPOverrideDynamicGet(L7_uchar8 family,
                                       L7_inet_addr_t *rpGrpAddr,
                                       L7_uchar8 prefixLength,
                                       L7_uint32 *overRide);
/*********************************************************************
* @purpose  To set the StaticRPOverrideDynamic.
*
* @param    family          @b{(input)}    whether IPV4/V6
* @param    rpGrpAddr       @b{(input)}    static RP Group address
* @param    length          @b{(input)}    length of the address
* @param    overRide        @b{(input)}    Set OverrideDynamic
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPOverrideDynamicSet(L7_uchar8 family,
                                    L7_inet_addr_t *rpGrpAddr,
                                    L7_uchar8 prefixLength,
                                    L7_uint32 overRide);
/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    family         @b{(input)}    whether IPV4/V6
* @param    rpGrpAddr      @b{(input)}    static RP Group address
* @param    prefixLen      @b{(input)}    prefix Length.
* @param    rowStatus      @b{(output)}   get row status.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPRowStatusGet(L7_uchar8 family,
                                  L7_inet_addr_t *rpGrpAddr,
                                  L7_uchar8 prefixLen,
                                  L7_uint32 *rowStatus);
/*********************************************************************
* @purpose  Sets the admin mode.
*
* @param    family         @b{(input)}    whether IPV4/V6
* @param    rpGrpAddr      @b{(input)}    static RP Group address
* @param    prefixLen      @b{(input)}    prefix Length.
* @param    rowStatus      @b{(input)}    get row status.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRpRowStatusSet(L7_uchar8 family,
                                        L7_inet_addr_t *rpGrpAddr,
                                        L7_uint32 prefixLen,
                                        L7_uint32 rowStatus);
/*********************************************************************
* @purpose  Check whether bsrCandidateBSR entry exists.
*
* @param    family          @b{(input)}  Address Family type
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSREntryGet(L7_uchar8 family);
/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    family          @b{(input)}  Address Family type
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSREntryNextGet(L7_uchar8 family);

/*********************************************************************
* @purpose  To get the bsrCandtBSR Address.
*
* @param    family           @b{(input)}  Address Family type
* @param    interface        @b{(output)} interface
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRInterfaceGet(L7_uchar8 family, 
                                           L7_uint32 *interface);

/*********************************************************************
* @purpose  To get the bsrCandtBSR Address.
*
* @param    family           @b{(input)}  Address Family type
* @param    cbsrAddr         @b{(output)} To get bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRAddrGet(L7_uchar8 family,
                                           L7_inet_addr_t *cbsrAddr);
/*********************************************************************
* @purpose  To set the bsrCandtBSR Address.
*
* @param    family           @b{(input)}  Address Family type
* @param    cbsrAddr         @b{(input)}  To get bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRAddrSet(L7_uchar8 family,
                                           L7_inet_addr_t *cbsrAddr);
/*********************************************************************
* @purpose  To get the bsrCandtBSR priority.
*
* @param    family           @b{(input)}  Address Family type
* @param    cbsrPriority     @b{(output)} bsr candidate BSR priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRPriorityGet(L7_uchar8 family,
                                               L7_uint32 *cbsrPriority);
/*********************************************************************
* @purpose  To set the bsrCandtBSR priority.
*
* @param    family           @b{(input)}  Address Family type
* @param    cbsrPriority     @b{(input)}  bsr candidate BSR priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRPrioritySet(L7_uchar8 family,
                                               L7_uint32 cbsrPriority);
/*********************************************************************
* @purpose  To get the bsrCandtBSR hashMaskLen.
*
* @param    family           @b{(input)}  Address Family type
* @param    hashMaskLen      @b{(output)} get Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRHashMaskLengthGet(L7_uchar8 family,
                                                L7_uint32 *hashMaskLen);
/*********************************************************************
* @purpose  To set the bsrCandtBSR hashMaskLen.
*
* @param    family           @b{(input)} Address Family type
* @param    hashMaskLen      @b{(input)} Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRHashMaskLengthSet(L7_uchar8 family,
                                                     L7_uint32 hashMaskLen);
/*********************************************************************
* @purpose  To get the  electedBSR.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    electedBsr       @b{(output)} BSR is elected/not.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRElectedBSRGet(L7_uchar8 family,
                                                 L7_uint32 *electedBsr);
/*********************************************************************
* @purpose  To get the CandtBSR BootstrapTimer.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    bsTimer          @b{(output)} BootStap Timer.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRBootstrapTimerGet(L7_uchar8 family,
                                                     L7_uint32 *bsTimer);
/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    family           @b{(input)}  Address Family type
* @param    mode             @b{(output)} get bsr mode.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRModeGet(L7_uchar8 family,
                                               L7_uint32 *mode);
/*********************************************************************
* @purpose  Sets the admin mode for the specified interface
*
* @param    family       @b{(input)}  Address Family type
* @param    mode         @b{(input)}  BSR mode.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmCandBSRModeSet(L7_uchar8 family, L7_uint32 mode);
/*********************************************************************
* @purpose  Gets the DR Priority for the specified interface.
*
* @param    UnitIndex   @b{(input)}  UnitIndex.
* @param    family      @b{(input)}  family.
* @param    intIfNum    @b{(input)}  Interface Number
* @param    priority    @b{(output)} Priority
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceDRPriorityGet(L7_uint32 unitIndex,L7_uchar8 family,
                                  L7_uint32 intIfNum, L7_uint32 *priority);
/************************************************************************
* @purpose  Get the Elected Bsr Candidate Priority for the router.
*
* @param    UnitIndex       @b{(input)}   UnitIndex.
* @param    family          @b{(input)}   family.
* @param    priority        @b{(output)}  priority
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmElectedBSRPriorityGet(L7_uint32 UnitIndex,
                                 L7_uchar8 family, L7_uint32 *priority);
/************************************************************************
* @purpose  Get the Elelcted BSR Hash Mask Length for the router.
*
* @param    UnitIndex       @b{(input)}   UnitIndex.
* @param    family          @b{(input)}   family.
* @param    hashMaskLen     @b{(input)}   Hash Mask Length.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t usmDbPimsmElectedBSRHashMaskLengthGet(L7_uint32 UnitIndex,
                                      L7_uchar8 family,L7_uint32 *hashMaskLen);
/*********************************************************************
* @purpose  Sets the BSR Border for the specified interface
*
* @param    UnitIndex     @b{(input)}   UnitIndex.
* @param    family        @b{(input)}   family.
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    mode          @b{(input)}   mode (L7_ENABLE/L7_DISABLE)
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceBsrBorderSet(L7_uint32 unitIndex,
                                        L7_uchar8 family,
                                        L7_uint32 intIfNum, L7_uint32 mode);
/*********************************************************************
* @purpose  Gets the BSR Border for the specified interface.
*
* @param    UnitIndex     @b{(input)}   UnitIndex.
* @param    family        @b{(input)}   family.
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    bsrBorder     @b{(output)}  Bsr Border
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceBsrBorderGet(L7_uint32 unitIndex,L7_uchar8 family,
                                        L7_uint32 intIfNum,
                                        L7_uint32 *bsrBorder);
/*********************************************************************
* @purpose  Sets the DR Priority for the specified interface
*
* @param    UnitIndex     @b{(input)}   UnitIndex.
* @param    family        @b{(input)}   family.
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    priority      @b{(input)}   priority value
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceDRPrioritySet(L7_uint32 unitIndex,
                                      L7_uchar8 family,
                                      L7_uint32 intIfNum,  L7_uint32 priority);
/*********************************************************************
* @purpose  Checks whether an interface is valid for PIM-SM
*
* @param    UnitIndex     @b{(input)}   UnitIndex.
* @param    intIfNum      @b{(input)}   Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbPimsmIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  To get the expiry Time in SG TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pExpiryTime   @b{(output)} expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGExpiryTimeGet(L7_uchar8 family, 
                                 L7_inet_addr_t *pSGGrpAddr, 
                                 L7_inet_addr_t *pSGSrcAddr,
                                 L7_uint32 *pExpiryTime);
/*********************************************************************
* @purpose  To get the RPF Address in SG TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pRpfAddr      @b{(output)} RPF Address.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRpfAddrGet(L7_uchar8 family, 
                               L7_inet_addr_t *pSGGrpAddr, 
                               L7_inet_addr_t *pSGSrcAddr,
                               L7_inet_addr_t *pRpfAddr);

/*********************************************************************
* @purpose  To get the Flags in SG TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pSGFalgs      @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGFlagsGet(L7_uchar8 family, 
                             L7_inet_addr_t *pSGGrpAddr, 
                             L7_inet_addr_t *pSGSrcAddr,
                             L7_uint32      *pSGFlags);
/*********************************************************************
* @purpose  To get the IfIndex in SG TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pSGIfIndex    @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIfIndexGet(L7_uchar8 family, 
                             L7_inet_addr_t *pSGGrpAddr, 
                             L7_inet_addr_t *pSGSrcAddr,
                             L7_uint32      *pSGIfIndex);
/*********************************************************************
* @purpose  To get the expiry Time in Star G TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pExpiryTime   @b{(output)} expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGExpiryTimeGet(L7_uchar8 family, 
                                     L7_inet_addr_t *pGrpAddr, 
                                     L7_uint32 *pExpiryTime);

/*********************************************************************
* @purpose  To get the RPF Address in Star G TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pRpfAddr      @b{(output)} RPF Address.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRpfAddrGet(L7_uchar8 family, 
                               L7_inet_addr_t *pGrpAddr, 
                               L7_inet_addr_t *pRpfAddr);

/*********************************************************************
* @purpose  To get the Flags in Star G TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pFalgs        @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGFlagsGet(L7_uchar8 family, 
                                L7_inet_addr_t *pGrpAddr, 
                                L7_uint32      *pFlags);

/*********************************************************************
* @purpose  To get the IfIndex in Star G TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pIfIndex      @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIfIndexGet(L7_uchar8 family, 
                                  L7_inet_addr_t *pGrpAddr, 
                                  L7_uint32      *pIfIndex);

/*********************************************************************
* @purpose  To get the next entry in Star Star RP TABLE.
*
* @param    family        @b{(input)} family.
* @param    pRpAddr       @b{(inout)} Star Star RP Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarStarRPEntryNextGet(L7_uchar8 family, 
                                         L7_inet_addr_t *pRpAddr);
/*********************************************************************
* @purpose  Obtain the next incoming interface in the Star Star RP TABLE.
*
* @param    family        @b{(input)} family.
* @param    pRpAddr       @b{(inout)} Star Star RP Address
* @param    pIntIfNum     @b{(inout)} Star Star RP Internal Interface Number.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarStarRPIEntryNextGet(L7_uchar8 family, 
                                          L7_inet_addr_t *pRpAddr, 
                                          L7_uint32 *pIntIfNum);
/*********************************************************************
* @purpose  To get the expiry Time in Star Star RP TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pRpAddr       @b{(inout)}  Star Star RP Address
* @param    pExpiryTime   @b{(output)} expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarStarRPExpiryTimeGet(L7_uchar8 family, 
                                          L7_inet_addr_t *pRpAddr, 
                                          L7_uint32 *pExpiryTime);
/*********************************************************************
* @purpose  To get the up Time in Star Star RP TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pRpAddr       @b{(inout)}  Star Star RP Address
* @param    pUpTime       @b{(output)} Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarStarRPUpTimeGet(L7_uchar8 family, 
                                      L7_inet_addr_t *pRpAddr, 
                                      L7_uint32 *pUpTime);
/*********************************************************************
* @purpose  To get the RPF Address in Star Star RP TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pRpAddr       @b{(inout)}  Star Star RP Address
* @param    pRpfAddr      @b{(output)} RPF Address.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarStarRPRpfAddrGet(L7_uchar8 family, 
                                       L7_inet_addr_t *pRpAddr, 
                                       L7_inet_addr_t *pRpfAddr);
/*********************************************************************
* @purpose  To get the Flags in Star Star RP TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pRpAddr       @b{(inout)}  Star Star RP Address
* @param    pFalgs        @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarStarRPFlagsGet(L7_uchar8 family, 
                                     L7_inet_addr_t *pRpAddr, 
                                     L7_uint32      *pFalgs);
/*********************************************************************
* @purpose  To get the IfIndex in Star Star RP TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pRpAddr       @b{(inout)}  Star Star RP Address
* @param    pIfIndex      @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarStarRPIfIndexGet(L7_uchar8 family, 
                                       L7_inet_addr_t *pRpAddr, 
                                       L7_uint32      *pIfIndex);
 /************************************************************************
    New USMDB implementation for the draft-ietf-pim-mib-v2-03.txt follows
 ************************************************************************/




/*********************************************************************
* @purpose  Gets the Generation ID value for the specified interface.
*
* @param    intIfNum @b{(input);}  internal Interface number
* @param    inetIPVersion    IPV4/V6
* @param    *ipAddr   @b{(output);} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceGenerationIDValueGet(L7_uint32 intIfNum,
                                       L7_uint32 inetIPVer, L7_uint32 *genIDValue);

/*********************************************************************
* @purpose  Sets the UseDRPriority option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    inetIPVersion    IPV4/V6
* @param    useDRPriority   Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceUseDRPrioritySet(L7_uint32 intIfNum,
                                                L7_uint32 inetIPVer,
                                            L7_int32 useDRPriority);

/*********************************************************************
* @purpose Gets the UseDRPriority option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    inetIPVersion    IPV4/V6
* @param    *useDRPriority  Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceUseDRPriorityGet(L7_uint32 intIfNum,
                                                L7_uint32 inetIPVer,
                                                L7_int32 *useDRPriority);

/*********************************************************************
* @purpose  Sets the UseDRPriority option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    inetIPVersion    IPV4/V6
* @param    useDRPriority   Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceBSRBorderSet(L7_uint32 intIfNum,
                                         L7_uint32 inetIPVer,
                                        L7_int32 bSRBorder);

/*********************************************************************
* @purpose  Gets the BSRBorder option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    inetIPVersion    IPV4/V6
* @param    *bSRBorder      BSRBorder option (TRUE/FALSE);
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmInterfaceBSRBorderGet(L7_uint32 intIfNum,
                                                L7_uint32 inetIPVer,
                                                L7_int32 *bSRBorder);

/************ END OF PIMSM INTERFACE TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  To get if the neighbor DR Priority is present.
*
* @param    intIfNum    internal Interface number
* @param    inetIPVersion   IP Version
* @param    *inetIPAddr   IP Address
* @param    *bNbrDRPrio   Is Nbr Priority Set.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmNeighborDRPriorityPresentGet(L7_uint32 intIfNum, L7_int32 inetIPVersion,
                                          L7_inet_addr_t *inetIPAddr, L7_uint32 *bNbrDRPrio);

/************ END OF PIMSM NEIGHBOUR TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SG entry exists for the specified index.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SG TABLE
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr);

/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG mode.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG mode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGPimModeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_int32 *sgmode);

/*********************************************************************
* @purpose  To get the SG state.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgstate   SG state
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGUpstreamJoinStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_int32 *sgstate);

/*********************************************************************
* @purpose  To get the SG join timer.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgjoinTimer   SG join timer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGUpstreamJoinTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgjoinTimer);

/*********************************************************************
* @purpose  To get the SG UpstreamNeighbor
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    upstreamNbr  UpstreamNeighbor
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGUpstreamNeighborGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *upstreamNbr);

/*********************************************************************
* @purpose  To get the SG RPFIfIndex.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfIfIndex   SG RPFIfIndex.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPFIfIndexGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *rpfIfIndex);

/*********************************************************************
* @purpose  To get the SG RPFprotocol.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfprotocol   SG RPFprotocol.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPFRouteProtocolGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *rpfprotocol);

/*********************************************************************
* @purpose  To get the SG RPF RouteAddress.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfRtAddr   RPF RouteAddress
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPFRouteAddressGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfRtAddr);

/*********************************************************************
* @purpose  To get the SG RPFRoutePrefixLength.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgrtLen   SG RPFRoutePrefixLength
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPFRoutePrefixLengthGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtLen);

/*********************************************************************
* @purpose  To get the SG RPFRouteMetricPref.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgrtMetricPref SG RPFRouteMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtMetricPref);

/*********************************************************************
* @purpose  To get the SG RPFRouteMetricPref.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgrtMetric SG RPFRouteMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPFRouteMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtMetric);

/*********************************************************************
* @purpose  To get the SG KeepaliveTimer.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgKeepAliveTimer SG KeepaliveTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGKeepaliveTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgKeepAliveTimer);

/*********************************************************************
* @purpose  To get the SG DRRegisterState.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgDRRegState   SG DRRegisterState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGDRRegisterStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgDRRegState);

/*********************************************************************
* @purpose  To get the SG DRRegisterStopTimer.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgDRRegTimer  SG DRRegisterStopTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGDRRegisterStopTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgDRRegTimer);

/*********************************************************************
* @purpose  To get the SG RPRegisterPMBRAddress.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPRegisterPMBRAddress.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPRegisterPMBRAddressGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfNextHop);

/*********************************************************************
* @purpose  To get the SG RPF nextHop.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRPFNextHopGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfNextHop);

/*********************************************************************
* @purpose  To get the SG SPTBit.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGSPTBitGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgSPTBit);

/************ END OF PIMSM SG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRpt entry exists for the specified index.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRpt TABLE
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr);

/*********************************************************************
* @purpose  To get the SGRPT uptime.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgRPTUpTime);

/*********************************************************************
* @purpose  To get the SGRPT upstreamPruneState.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTState   SGRPT upstreamPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptUpstreamPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_int32 *sgRPTState);

/*********************************************************************
* @purpose  To get the SGRPT UpstreamOverrideTimer.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTOverrideTimer   SGRPT UpstreamOverrideTimer.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptUpstreamOverrideTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgRPTOverrideTimer);

/************ END OF PIMSM SGRPT TABLE USMDB APIs ***************/



  
/*********************************************************************
* @purpose  Check whether staticRP entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimStaticRPGrpAddr   static RP Group address
* @param    *length              length of the address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimStaticRPGrpAddr, L7_uchar8 length);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the staticRP TABLE
*
* @param    addrType             whether IPV4/V6
* @param    pimStaticRPGrpAddr   static RP Group address
* @param    *length              length of the address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimStaticRPGrpAddr, L7_uchar8 *length);


  
/*********************************************************************
* @purpose  To get the StaticRPPimMode.
*
* @param    addrType             whether IPV4/V6
* @param    *pimStaticRPGrpAddr   static RP Group address
* @param    length              length of the address
* @param    *pimStaticRPPimMode   Get pimMode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPPimModeGet(L7_uchar8 addrType, L7_inet_addr_t *pimStaticRPGrpAddr, L7_uint32 length,
                                    L7_uint32 *pimStaticRPPimMode);

/*********************************************************************
* @purpose  To get the StaticRPPimMode.
*
* @param    addrType             whether IPV4/V6
* @param    *pimStaticRPGrpAddr   static RP Group address
* @param    length              length of the address
* @param    pimStaticRPPimMode   Set pimMode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStaticRPPimModeSet(L7_uchar8 addrType, L7_inet_addr_t *pimStaticRPGrpAddr, L7_uint32 length,
                                    L7_uint32 pimStaticRPPimMode);


 /*********************************************************************
* @purpose  Check whether Group mapping entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmGroupMappingEntryGet(L7_int32 origin, L7_uchar8 addrType, 
                                             L7_inet_addr_t *pimMappingGrpAddr, L7_uint32 length, 
                                             L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the Group mapping TABLE
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmGroupMappingEntryNextGet(L7_uchar8 *origin, L7_uchar8 addrType, 
                                                 L7_inet_addr_t *pimMappingGrpAddr,L7_uchar8 *length, 
                                                 L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  To get the groupMapping pim mode
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    pimGroupMappingMode  Group mapping mode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmGroupMappingPimModeGet(L7_int32 origin, L7_uchar8 addrType, 
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uint32 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *pimGroupMappingMode);

/************ END OF PIMSM GROUP MAPPING TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether bsrCandidateRP entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    bsrCandtRPAddr       bsr Candidate Addr
* @param    bsrCandtRPGrpAddr    bsr Candidate RP Grp Addr
* @param    *length              length of the address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrCandidateRPEntryGet(L7_uchar8 addrType, 
                                             L7_inet_addr_t *bsrCandtRPAddr, 
                                             L7_inet_addr_t *bsrCandtRPGrpAddr,
                                             L7_uint32 length);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    addrType             whether IPV4/V6
* @param    bsrCandtRPAddr       bsr Candidate Addr
* @param    bsrCandtRPGrpAddr    bsr Candidate RP Grp Addr
* @param    *length              length of the address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrCandRPEntryNextGet(L7_uchar8 addrType, 
                                             L7_inet_addr_t *bsrCandtRPAddr, 
                                             L7_inet_addr_t *bsrCandtRPGrpAddr,
                                             L7_uchar8 *length);

/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    family               whether IPV4/V6
* @param    bsrCandtRPAddr       bsr Candidate Addr
* @param    bsrCandtRPGrpAddr    bsr Candidate RP Grp Addr
* @param    length               length of the address
* @param    *mode                L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrCandRPModeGet(L7_uchar8 family, 
                                        L7_inet_addr_t *bsrCandtRPAddr,
                                        L7_inet_addr_t *bsrCandtRPGrpAddr,
                                        L7_uint32 length, L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the admin mode for the specified interface
*
* @param    addrType             whether IPV4/V6
* @param    bsrCandtRPAddr       bsr Candidate Addr
* @param    bsrCandtRPGrpAddr    bsr Candidate RP Grp Addr
* @param    *length              length of the address
* @param    mode          L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrCandidateRPModeSet(L7_uchar8 addrType, L7_inet_addr_t *bsrCandtRPAddr,
                                              L7_inet_addr_t *bsrCandtRPGrpAddr, L7_uint32 length, L7_uint32 mode);
/************ END OF PIMSM BSR CANDIDATE RP TABLE USMDB APIs ***************/

 /*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    addrType             whether IPV4/V6
* @param    bsrElectedBSR      To get bsr candidate BSR is elected BSR
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrCandtBSRElectedBSRGet(L7_uchar8 addrType, L7_uint32 *bsrElectedBSR);


 /*********************************************************************
* @purpose  Check whether bsrCandidateBSR entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrElectedBSREntryGet(L7_uchar8 addrType); 
/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    addrType             whether IPV4/V6
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrElectedBSREntryNextGet(L7_int32 *addrType); 
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    addrType             whether IPV4/V6
* @param    bsrElectedBSRAddr    To get the bsrElectedBSR Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrElectedBSRAddrGet(L7_uint32 addrType, L7_uint32 *bsrElectedBSRAddr);
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    addrType             whether IPV4/V6
* @param    bsrElectedBSRPriority  To get the bsrElectedBSR Priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrElectedBSRPriorityGet(L7_uchar8 addrType, L7_uint32 *bsrElectedBSRPriority);
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    addrType             whether IPV4/V6
* @param    hashMaskLen  To get the bsrElectedBSR hashMaskLen
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrElectedBSRHashMaskLengthGet(L7_uchar8 addrType, L7_uint32 *hashMaskLen);
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    addrType             whether IPV4/V6
* @param    bsrElectedBSRExpiryTime  To get the bsrElectedBSR ExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrElectedBSRExpiryTimeGet(L7_uchar8 addrType, L7_uint32 *bsrElectedBSRExpiryTime);
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    addrType             whether IPV4/V6
* @param    bsrCRPAdvTimer  To get the bsrElectedBSR AdvTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrElectedBSRCRPAdvTimerGet(L7_uchar8 addrType, L7_uint32 *bsrCRPAdvTimer);

/*********************************************************************
* @purpose  Check whether StarG entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr); 

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the StarG TABLE
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr); 

/*********************************************************************
* @purpose  To get the StarG uptime
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGUpTime    To get the pimStarGUpTime 
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGUpTime);

/*********************************************************************
* @purpose  To get the StarG PimMode
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGPimMode   To get the pimStarGPimMode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGPimModeGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGPimMode);


/*********************************************************************
* @purpose  To get the starG RP address.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPAddr  To get the pimStarGRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *pimStarGRPAddr);

/*********************************************************************
* @purpose  To get the StarGRPOrigin
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPOrigin  To get the StarGRPOrigin
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPOriginGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGRPOrigin);
/*********************************************************************
* @purpose  To get the pimStarGRPIsLocal
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPIsLocal  To get the pimStarGRPIsLocal
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPIsLocalGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGRPIsLocal);
/*********************************************************************
* @purpose  To get the JoinState
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGUpstreamJoinStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *JoinState);

/*********************************************************************
* @purpose  To get the UpstreamJoinTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGUpstreamJoinTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *JoinTimer);

/*********************************************************************
* @purpose  To get the UpstreamNeighbor
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    UpstreamNbr  To get the UpstreamNbr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGUpstreamNbrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *UpstreamNbr);

/*********************************************************************
* @purpose  To get the RPFIfIndex
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFIfIndex  To get the RPFIfIndex
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPFIfIndexGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFIfIndex);

/*********************************************************************
* @purpose  To get the RPFNextHop
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFNextHop  To get the RPFNextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPFNextHopGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *RPFNextHop);

/*********************************************************************
* @purpose  To get the RPFRouteProtocol
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteProtocol  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPFRouteProtocolGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RouteProtocol);

/*********************************************************************
* @purpose  To get the RPFRouteAddress
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteAddress  To get the RPFRouteAddress
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPFRouteAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *RPFRouteAddr);

/*********************************************************************
* @purpose  To get the RPFRoutePrefixLength
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRoutePrefixLength  To get the RPFRoutePrefixLength
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPFRoutePrefixLenGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRoutePrefixLen);

/*********************************************************************
* @purpose  To get the RPFRouteMetricPref
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetricPref  To get the RPFRouteMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRouteMetricPref);

/*********************************************************************
* @purpose  To get the RPFRouteMetric
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetric  To get the RPFRouteMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGRPFRouteMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRouteMetric);
/************ END OF PIMSM STARG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether StarGI entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr      StarGI group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex); 

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the StarGI TABLE
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 *IfIndex); 

/*********************************************************************
* @purpose  To get the StarGI uptime
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    pimStarGIUpTime    To get the pimStarGIUpTime 
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *pimStarGIUpTime);

/*********************************************************************
* @purpose  To get the starG LocalMembership
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    pimStarGIRPAddr  To get the pimStarGIRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *localMemShip);

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinState);

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinTimer);

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinExpiry);

/*********************************************************************
* @purpose  To get the AssertState
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIAssertStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *AssertState);

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIAssertTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *AssertTimer);

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIAssertWinnerAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_inet_addr_t *AssertWinner);

/*********************************************************************
* @purpose  To get the AssertWinnerMetricPref
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    AssertWinnerMetricPref  To get the AssertWinnerMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_int32 ifindex, L7_uint32 *AssertWinnerMetricPref);

/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIAssertWinnerMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_int32 ifindex, L7_uint32 *AssertWinnerMetric);

/************ END OF PIMSM StarGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SG entry exists for the specified index.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SG TABLE
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 *ifindexg);

/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_int32 *LocalMembership);

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *JoinPruneState);

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                    L7_uint32 *JoinTimer);

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,
                                   L7_uint32 *JoinExpiry);

/*********************************************************************
* @purpose  To get the AssertState
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIAssertStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                   L7_uint32 *AssertState);

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIAssertTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                      L7_uint32 *AssertTimer);

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIAssertWinnerAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                    L7_inet_addr_t *AssertWinner);

/*********************************************************************
* @purpose  To get the AssertWinnerMetricPref
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetricPref  To get the AssertWinnerMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                   L7_uint32 *AssertWinnerMetricPref);

/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIAssertWinnerMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                           L7_uint32 *AssertWinnerMetric);

/************ END OF PIMSM SGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRptI entry exists for the specified index.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRptI TABLE
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptIEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 *ifindexg);

/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_int32 *LocalMembership);

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,
                                    L7_uint32 *JoinPruneState);

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, 
                                    L7_uint32 *JoinTimer);

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGRptIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,
                                   L7_uint32 *JoinExpiry);

/*********************************************************************
* @purpose  Check whether bsrRPSet entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrRPSetEntryGet(L7_int32 origin, L7_uchar8 addrType,
                                             L7_inet_addr_t *pimMappingGrpAddr, L7_uint32 length,
                                             L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in bsrRPSet  mapping TABLE
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrRPSetEntryNextGet(L7_uchar8 *origin, L7_uchar8 *addrType,
                                                 L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 *length,
                                                 L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  To get the RPSetPriority
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetPriority  RPSetPriority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrRPSetPriorityGet(L7_int32 origin, L7_uchar8 addrType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uint32 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetPriority);

/*********************************************************************
* @purpose  To get the RPSetHoldtime
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetHoldtime        RPSetHoldtime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrRPSetHoldtimeGet(L7_int32 origin, L7_uchar8 addrType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uint32 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetHoldtime);

/*********************************************************************
* @purpose  To get the RPSetExpiryTime
*
* @param    origin               mapping origin
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetExpiryTime        RPSetExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmbsrRPSetExpiryTimeGet(L7_int32 origin, L7_uchar8 addrType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uint32 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetExpiryTime);




/***************************************************************************
* @purpose  To get the next staticRp entry

* @param family             whether IPV4/V6
* @param rpAddr            Rp address
* @param rpGrpAddr         Rp Group address
* @param  prefixLen        prefix length of rpGrpAddr
* @param  indexstart       pointer to the next entry
*
* @returns  L7_SUCCESS if entry exists
* @returns  L7_FAILURE if entry does not exist
*
* @notes
*
* @end


****************************************************************************/


L7_RC_t usmDbPimsmCliStaticRpNextGet(L7_uchar8 family, L7_inet_addr_t *rpAddr, L7_inet_addr_t *rpGrpAddr, 
                                      L7_uchar8 *prefixLen, L7_uint32*indexstart,L7_BOOL *override);



/****************************************************************************
* @purpose   To get the next candidateRp entry
* @param UnitIndex            L7_uint32   Unit Number
* @param family               whether IPV4/IPV6
* @param intIfNum           internal interface Number
* @param cRPGroupAddress      candidate RP group address
* @param cRPprefixLen         prefix length of candidateRp groupaddress
* @param  indexstart          pointer to the next entry
*
*
* @returns L7_SUCCESS if entry exists
* @returns L7_FAILURE if entry does not exist
*
* @notes
* @end


*****************************************************************************/

L7_RC_t usmDbPimsmCliCandidateRPEntryNextGet(L7_uint32 UnitIndex,
                                          L7_uchar8 family,
                                          L7_uint32 *intIfNum,
                                          L7_inet_addr_t *cRPGroupAddress,
                                          L7_uchar8      *cRPprefixLen, L7_uint32 *indexstart);

/* The below wrappers are written explicitly for XUI to support the new PIM BSR
 * MIB.  Currently do not use these for CLI/SNMP purposes.
 */
/*********************************************************************
* @purpose  Check whether bsrRPSet entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIRPSetEntryGet (L7_uchar8 addrType,
                               L7_inet_addr_t *pimMappingGrpAddr,
                               L7_uint32 length,
                               L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in bsrRPSet  mapping TABLE
*
* @param    addrType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIRPSetEntryNextGet (L7_uchar8 *addrType,
                                   L7_inet_addr_t *pimMappingGrpAddr,
                                   L7_uchar8 *length,
                                   L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  To get the RPSetPriority
*
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetPriority  RPSetPriority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIRPSetPriorityGet (L7_uchar8 addrType,
                                  L7_inet_addr_t *pimMappingGrpAddr,
                                  L7_uint32 length,
                                  L7_inet_addr_t *pimMappingRPAddr,
                                  L7_uint32 *RPSetPriority);

/*********************************************************************
* @purpose  To get the RPSetHoldtime
*
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetHoldtime        RPSetHoldtime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIRPSetHoldtimeGet (L7_uchar8 addrType,
                                  L7_inet_addr_t *pimMappingGrpAddr,
                                  L7_uint32 length,
                                  L7_inet_addr_t *pimMappingRPAddr,
                                  L7_uint32 *RPSetHoldtime);

/*********************************************************************
* @purpose  To get the RPSetExpiryTime
*
* @param    addrType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetExpiryTime        RPSetExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIRPSetExpiryTimeGet (L7_uchar8 addrType,
                                    L7_inet_addr_t *pimMappingGrpAddr,
                                    L7_uint32 length,
                                    L7_inet_addr_t *pimMappingRPAddr,
                                    L7_uint32 *RPSetExpiryTime);

/*********************************************************************
* @purpose  Check whether bsrCandidateBSR entry exists.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSREntryGet (L7_uchar8 zoneIndex);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSREntryNextGet (L7_uchar8 zoneIndex);

/*********************************************************************
* @purpose  To get the bsrCandtBSR Address.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    interface        @b{(output)} interface
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRInterfaceGet (L7_uchar8 zoneIndex, 
                                  L7_uint32 *interface);

/*********************************************************************
* @purpose  To get the bsrCandtBSR Address.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    cbsrAddr         @b{(output)} To get bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRAddrGet (L7_uchar8 zoneIndex, 
                             L7_inet_addr_t *cbsrAddr);

/*********************************************************************
* @purpose  To set the bsrCandtBSR Address.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    cbsrAddr         @b{(input)}  To get bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRAddrSet (L7_uchar8 zoneIndex, 
                             L7_inet_addr_t *cbsrAddr);

/*********************************************************************
* @purpose  To get the bsrCandtBSR priority.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    cbsrPriority     @b{(output)} bsr candidate BSR priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRPriorityGet (L7_uchar8 zoneIndex, 
                                 L7_uint32 *cbsrPriority);

/*********************************************************************
* @purpose  To set the bsrCandtBSR priority.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    cbsrPriority     @b{(input)}  bsr candidate BSR priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRPrioritySet (L7_uchar8 zoneIndex, 
                                 L7_uint32 cbsrPriority);

/*********************************************************************
* @purpose  To get the bsrCandtBSR hashMaskLen.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    hashMaskLen      @b{(output)} get Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRHashMaskLengthGet (L7_uchar8 zoneIndex, 
                                       L7_uint32 *hashMaskLen);

/*********************************************************************
* @purpose  To set the bsrCandtBSR hashMaskLen.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    hashMaskLen      @b{(input)} Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRHashMaskLengthSet (L7_uchar8 zoneIndex, 
                                       L7_uint32 hashMaskLen);

/*********************************************************************
* @purpose  To get the  electedBSR.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    electedBsr       @b{(output)} BSR is elected/not.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRElectedBSRGet (L7_uchar8 zoneIndex, 
                                   L7_uint32 *electedBsr);

/*********************************************************************
* @purpose  To get the CandtBSR BootstrapTimer.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    bsTimer          @b{(output)} BootStap Timer.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRBootstrapTimerGet (L7_uchar8 zoneIndex, 
                                       L7_uint32 *bsTimer);

/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    mode             @b{(output)} get bsr mode.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRModeGet (L7_uchar8 zoneIndex, 
                             L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the admin mode for the specified interface
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    mode         @b{(input)}  BSR mode.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmXUICandBSRModeSet (L7_uchar8 zoneIndex,
                             L7_uint32 mode);

/*********************************************************************
* @purpose  Check whether bsrCandidateBSR entry exists for the specified index.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIElectedBSREntryGet (L7_uchar8 zoneIndex);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIElectedBSREntryNextGet (L7_int32 *zoneIndex);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    bsrElectedBSRAddr    To get the bsrElectedBSR Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIElectedBSRAddrGet (L7_uint32 zoneIndex,
                                   L7_uint32 *bsrElectedBSRAddr);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    bsrElectedBSRPriority  To get the bsrElectedBSR Priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIElectedBSRPriorityGet (L7_uchar8 zoneIndex,
                                       L7_uint32 *bsrElectedBSRPriority);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    hashMaskLen  To get the bsrElectedBSR hashMaskLen
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIElectedBSRHashMaskLengthGet (L7_uchar8 zoneIndex,
                                             L7_uint32 *hashMaskLen);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    bsrElectedBSRExpiryTime  To get the bsrElectedBSR ExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIElectedBSRExpiryTimeGet (L7_uchar8 zoneIndex,
                                         L7_uint32 *bsrElectedBSRExpiryTime);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    zoneIndex        @b{(input)}  Scope Zone index
* @param    bsrCRPAdvTimer  To get the bsrElectedBSR AdvTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
usmDbPimsmbsrXUIElectedBSRCRPAdvTimerGet (L7_uchar8 zoneIndex,
                                          L7_uint32 *bsrCRPAdvTimer);

/************************************************************************
* @purpose  Get IP address of the bootstrap router (BSR) for the local
*           PIM region
*
* @param    zoneIndex                 @b{(input)}  Scope Zone index
* @param    elecBSRAddress            @b{(output)} bootstrap router Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t
usmDbPimsmXUIElectedBSRAddressGet (L7_uchar8 zoneIndex,
                                   L7_inet_addr_t *elecBSRAddress);

/************************************************************************
* @purpose  Get the Elected Bsr Candidate Priority for the router.
*
* @param    zoneIndex       @b{(input)}  Scope Zone index
* @param    priority        @b{(output)}  priority
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t
usmDbPimsmXUIElectedBSRPriorityGet (L7_uchar8 zoneIndex,
                                    L7_uint32 *priority);

/************************************************************************
* @purpose  Get the Elelcted BSR Hash Mask Length for the router.
*
* @param    zoneIndex       @b{(input)}  Scope Zone index
* @param    hashMaskLen     @b{(input)}  Hash Mask Length.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
************************************************************************/
L7_RC_t
usmDbPimsmXUIElectedBSRHashMaskLengthGet (L7_uchar8 zoneIndex,
                                          L7_uint32 *hashMaskLen);

#endif /* _USMDB_MIB_PIMSM_API_H_ */
