/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   usmdb_mib_pimsm.c
*
* @purpose    Api functions for the Protocol Independent Multicast MIB, as
*             specified in RFC 2934
*
* @component  PIM-SM usmdb Layer
*
* @comments   none
*
* @create     04/05/2002
*
* @author     Nitish
* @end
*
**********************************************************************/

#include "l7_common.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pimsm_api.h"
#endif

#include "osapi.h"
#include "l3_mcast_commdefs.h"

#include "l7_pimsm_api.h"

/*********************************************************************
* @purpose  Gets the IP Address for the specified interface
*
*@param    UnitIndex   @b{(input)}  UnitIndex.
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
                                   L7_uint32 intIfNum, L7_inet_addr_t *ipAddr)
{
    return pimsmMapInterfaceIPAddressGet(family, intIfNum, ipAddr);
}

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
                               L7_uint32 intIfNum, L7_inet_addr_t *netMask)
{
   return pimsmMapInterfaceNetMaskGet(family, intIfNum, netMask);
}

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
                                   L7_uint32 intIfNum, L7_uint32 mode)
{
     return pimsmMapInterfaceModeSet(family, intIfNum, mode);
}

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
                                   L7_uint32 intIfNum, L7_uint32 *mode)
{
    return pimsmMapInterfaceModeGet(family, intIfNum, mode);
}

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
                       L7_uint32 intIfNum, L7_inet_addr_t *dsgRtrIpAddr)
{
    return pimsmMapInterfaceDRGet(family, intIfNum, dsgRtrIpAddr);
}

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
                 L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 helloIntvl)
{
    return pimsmMapInterfaceHelloIntervalSet(family, intIfNum, helloIntvl);
}

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
              L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *helloIntvl)
{
   return pimsmMapInterfaceHelloIntervalGet(family, intIfNum, helloIntvl);
}

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
          L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 joinpruneIntvl)
{
    return pimsmMapInterfaceJoinPruneIntervalSet(family, intIfNum, 
                                                 joinpruneIntvl);
}

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
             L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 *joinpruneIntvl)
{
    return pimsmMapInterfaceJoinPruneIntervalGet(family, intIfNum, 
                                                 joinpruneIntvl);
}

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
             L7_uchar8 family, L7_uint32 intIfNum, L7_int32 candBSRPref)
{
    return L7_NOT_SUPPORTED;
}

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
           L7_uchar8 family,L7_uint32 intIfNum, L7_int32 *candBSRPref)
{
   return L7_NOT_SUPPORTED;
}

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
            L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 hashMskLen)
{
   return L7_NOT_SUPPORTED;
}

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
            L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 *hashMskLen)
{
   return L7_NOT_SUPPORTED;
}
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
             L7_uchar8 family, L7_uint32 intIfNum, L7_int32 candRPPref)
{
    return L7_NOT_SUPPORTED;
}

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
               L7_uchar8 family, L7_uint32 intIfNum, L7_int32 *candRPPref)
{
    return L7_NOT_SUPPORTED;
}


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
                                    L7_uint32 *nbrUpTime)
{
    return pimsmMapNeighborUpTimeGet(family, intIfNum, nbrIpAddr, nbrUpTime);
}

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
                                        L7_uint32 *nbrExpiryTime)
{
    return pimsmMapNeighborExpiryTimeGet(family, intIfNum, 
                                         nbrIpAddr, nbrExpiryTime);
}

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
                                        L7_uint32 *priority)
{
    return pimsmMapNeighborDrPriorityGet(family, intIfNum, 
                                         nbrIpAddr, priority);
}

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
                                           L7_uint32 *assertTimer)
{
    return L7_NOT_SUPPORTED;
#ifdef PIMSM_MAP_TBD    
    return pimsmMapIpMRouteUpstreamAssertTimerGet(family,ipMRouteGroup,
                                                  ipMRouteSource, 
                                                  ipMRouteSourceMask, 
                                                  assertTimer);
#endif
}

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
                                          L7_uint32 *assertMetric)
{
    return L7_NOT_SUPPORTED;
#ifdef PIMSM_MAP_TBD     
    return pimsmMapIpMRouteAssertMetricGet(family, ipMRouteGroup, 
                        ipMRouteSource, ipMRouteSourceMask, assertMetric);

#endif
}

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
                                          L7_uint32 *assertMetricPref)
{
    return L7_NOT_SUPPORTED;
#ifdef PIMSM_MAP_TBD     
  return pimsmMapIpMRouteAssertMetricPrefGet(family,ipMRouteGroup, 
                                               ipMRouteSource, 
                                               ipMRouteSourceMask, 
                                               assertMetricPref);
#endif
}
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
                                          L7_uint32 *assertRPTBit)
{
    return L7_NOT_SUPPORTED;
#ifdef PIMSM_MAP_TBD     
    return pimsmMapIpMRouteAssertRPTBitGet(family, ipMRouteGroup, 
                 ipMRouteSource, ipMRouteSourceMask, assertRPTBit);
#endif
}
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
                                   L7_uint32 *ipMRouteFlags)
{
    return L7_NOT_SUPPORTED;
#ifdef PIMSM_MAP_TBD     
    return pimsmMapIpMRouteFlagsGet(family, ipMRouteGroup, ipMRouteSource, 
                                    ipMRouteSourceMask, ipMRouteFlags);
#endif
}

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
                                    L7_uint32 *pruneReason)
{
    return L7_NOT_SUPPORTED;
}

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
                                   L7_uint32 *rpSetHoldTime)
{
    return pimsmMapRPSetHoldTimeGet(family, rpSetGroupAddress, 
                                    rpSetGroupMask, rpSetAddress, 
                                    rpSetComponent, rpSetHoldTime);
}

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
                                     L7_uint32 *rpSetExpiryTime)
{
    return pimsmMapRPSetExpiryTimeGet(family, rpSetGroupAddress, 
                                      rpSetGroupMask, rpSetAddress, 
                                      rpSetComponent, rpSetExpiryTime);
}

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
                                        L7_inet_addr_t *ipAddress)
{
    return pimsmMapCandRPAddressGet(family, candidateRPGroupAddress, 
                                          candidateRPGroupMask, ipAddress);
}

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
                                       L7_inet_addr_t *elecBSRAddress)
{
    return pimsmMapElectedBSRAddressGet(family, elecBSRAddress);
}

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
                                          L7_uint32 *bsrExpiryTime)
{
    return pimsmMapElectedBSRExpiryTimeGet(family, bsrExpiryTime);
}

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
                                        L7_uint32 *elecCRPAdvTime)
{
     return pimsmMapElectedCRPAdvTimeGet(family, elecCRPAdvTime);
}

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
                                    L7_uint32 intIfNum)
{
   return pimsmMapInterfaceEntryGet(family, intIfNum);
}

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
                                        L7_uint32 *intIfNum)
{
    return pimsmMapInterfaceEntryNextGet(family, intIfNum);
}

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
                               L7_uint32 intIfNum, L7_inet_addr_t *ipAddress)
{
   return pimsmMapNeighborEntryGet(family, intIfNum, ipAddress);
}

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
                              L7_uint32 *intIfNum, L7_inet_addr_t *ipAddress)
{
    return pimsmMapNeighborEntryNextGet(family, intIfNum, ipAddress);
}

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
                                   L7_inet_addr_t *ipMRouteSourceMask)
{
    return L7_NOT_SUPPORTED;
#ifdef PIMSM_MAP_TBD      
    return pimsmMapIpMRouteEntryGet(family, ipMRouteGroup, ipMRouteSource, 
                                    ipMRouteSourceMask);
#endif
}

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
                                       L7_inet_addr_t *ipMRouteSourceMask)
{
    return L7_NOT_SUPPORTED;
#ifdef PIMSM_MAP_TBD      
    return pimsmMapIpMRouteEntryNextGet(family,ipMRouteGroup, ipMRouteSource, 
                                        ipMRouteSourceMask);
#endif
}

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
                                       L7_inet_addr_t *ipMRouteNextHopAddress)
{
    return L7_NOT_SUPPORTED;
}

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
                                        L7_inet_addr_t *ipMRouteNextHopAddress)
{
    return L7_NOT_SUPPORTED;   
}

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
                                       L7_inet_addr_t *rpAddress)
{
    return pimsmMapRPGroupEntryGet(family, origin, groupAddress, 
                                   groupMask, rpAddress);
}

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
                                           L7_inet_addr_t *rpAddress)
{
    return pimsmMapRpGroupEntryNextGet(family, origin, groupAddress, 
                                       groupMask, rpAddress);
}

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
L7_RC_t usmDbPimsmCandRPEntryGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                      L7_inet_addr_t *cRPAddress, 
                                      L7_inet_addr_t *cRPGroupAddress, 
                                      L7_uchar8       cRPprefixLen)
{
    return pimsmMapCandRPEntryGet(UnitIndex,family, cRPAddress, cRPGroupAddress,
                                       cRPprefixLen);
}


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
                                          L7_uchar8      *cRPprefixLen)
{
   return  pimsmMapCandRPEntryNextGet(family, cRPAddress, 
                                           cRPGroupAddress, cRPprefixLen);
}

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
                                    L7_uchar8 family, L7_uint32 compIndex)
{
    return L7_NOT_SUPPORTED;
}

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
L7_RC_t usmDbPimsmElectedBSREntryNextGet(L7_uint32 UnitIndex, L7_uchar8 family)
{
    return pimsmMapElectedBSREntryNextGet(family);
}

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
                                        L7_uchar8 family, L7_uint32 intIfNum)
{
   return L7_NOT_SUPPORTED;
}

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
                                       L7_uchar8 family, L7_uint32 *intIfNum)
{
   return L7_NOT_SUPPORTED;   
}

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
                   L7_uchar8 *origin)
{
    return pimsmMapGroupToRPMappingGet(family, grpIpAddr, rpIpAddr, origin);
}

/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    family             @b{(input)} Address Family type
* @param    bsrCandtRPAddr     @b{(input)} bsr Candidate RP Addr
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
                                        L7_uint32 length, L7_uint32 *mode)
{
    return pimsmMapCandRPModeGet(family, bsrCandtRPAddr, 
                                      bsrCandtRPGrpAddr, length, mode);
}

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
                                        L7_uint32 length, L7_uint32 *intIfNum)
{
    return pimsmMapCandRPInterfaceGet(family, bsrCandtRPAddr, 
                                      bsrCandtRPGrpAddr, length, intIfNum);
}

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
                                   L7_uchar8 length)
{
    return pimsmMapSsmRangeEntryGet(family, ssmRangeAddr, length);
}

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
                                       L7_uchar8 *length)
{
    return pimsmMapSsmRangeEntryNextGet(family, ssmRangeAddr, length);
}

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
                                  L7_uint32 *ssmMode)
{
    return pimsmMapSsmRangeModeGet(family, ssmRangeAddr, prefixLen, ssmMode);
}
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
                                  L7_uchar8 prefixLen, L7_uint32 ssmMode)
{
    return pimsmMapSsmRangeModeSet(family, ssmRangeAddr, prefixLen, ssmMode);
}

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
                                  L7_inet_addr_t *staticRPAddr)
{
    return pimsmMapStaticRPAddrGet(family, rpGrpAddr, 
                                           length, staticRPAddr);
}

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
                                  L7_inet_addr_t *staticRPAddr)
{
    return pimsmMapStaticRPAddrSet(family, rpGrpAddr, 
                                           length, staticRPAddr);
}

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
                                       L7_uint32 *overRide)
{

   return pimsmMapStaticRPOverrideDynamicGet(family, rpGrpAddr, 
                                             prefixLength, overRide);
}

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
                                    L7_uint32 overRide)
{
    return pimsmMapStaticRPOverrideDynamicSet(family, rpGrpAddr, 
                                              prefixLength, overRide);
}

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
                                  L7_uint32 *rowStatus)
{
    return pimsmMapStaticRPRowStatusGet(family, rpGrpAddr, 
                                                prefixLen, rowStatus);
}

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
L7_RC_t usmDbPimsmStaticRPRowStatusSet(L7_uchar8 family,
                                        L7_inet_addr_t *rpGrpAddr, 
                                        L7_uint32 prefixLen, 
                                        L7_uint32 rowStatus)
{
    return pimsmMapStaticRPRowStatusSet(family, rpGrpAddr, 
                                                prefixLen, rowStatus);
}

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
L7_RC_t usmDbPimsmCandBSREntryGet(L7_uchar8 family) 
{
    return pimsmMapCandBSREntryGet(family);
}

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
L7_RC_t usmDbPimsmCandBSREntryNextGet(L7_uchar8 family) 
{
    return pimsmMapCandBSREntryNextGet(family);
}

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
                                           L7_uint32 *interface)
{
    return pimsmMapCandBSRInterfaceGet(family, interface);
}
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
                                           L7_inet_addr_t *cbsrAddr)
{
    return pimsmMapCandBSRAddrGet(family, cbsrAddr);
}

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
                                           L7_inet_addr_t *cbsrAddr)
{
    return pimsmMapCandBSRAddrSet(family, cbsrAddr);
}

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
                                               L7_uint32 *cbsrPriority)
{
    return pimsmMapCandBSRPriorityGet(family, cbsrPriority);
}

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
                                               L7_uint32 cbsrPriority)
{
    return pimsmMapCandBSRPrioritySet(family, cbsrPriority);
}

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
                                                L7_uint32 *hashMaskLen)
{
    return pimsmMapCandBSRHashMaskLengthGet(family, hashMaskLen);
}

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
                                                     L7_uint32 hashMaskLen)
{
    return pimsmMapCandBSRHashMaskLengthSet(family, hashMaskLen);
}

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
                                                 L7_uint32 *electedBsr)
{
    return pimsmMapCandBSRElectedBSRGet(family, electedBsr);
}

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
                                                     L7_uint32 *bsTimer)
{
    return pimsmMapCandBSRBootstrapTimerGet(family, bsTimer);
}

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
                                               L7_uint32 *mode)
{
    return pimsmMapCandBSRModeGet(family, mode);
}

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
L7_RC_t usmDbPimsmCandBSRModeSet(L7_uchar8 family, L7_uint32 mode)
{
    return pimsmMapCandBSRModeSet(family, mode);
}
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
                                  L7_uint32 intIfNum, L7_uint32 *priority)
{
   return pimsmMapInterfaceDRPriorityGet(family, intIfNum, priority);
}

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
                                 L7_uchar8 family, L7_uint32 *priority)
{
    return pimsmMapElectedBSRPriorityGet(family, priority);
}

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
                                      L7_uchar8 family,L7_uint32 *hashMaskLen)
{
     return pimsmMapElectedBSRHashMaskLengthGet(family, hashMaskLen);
}

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
                                        L7_uint32 intIfNum, L7_uint32 mode)
{
  return pimsmMapInterfaceBsrBorderSet(family, intIfNum, mode);
}

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
                                        L7_uint32 *bsrBorder)
{
  return pimsmMapInterfaceBsrBorderGet(family, intIfNum, bsrBorder);
}

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
                                         L7_uint32 intIfNum,  L7_uint32 priority)
{
  return pimsmMapInterfaceDRPrioritySet(family, intIfNum, priority);
}

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
L7_BOOL usmDbPimsmIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
    return pimsmIntfIsValid(intIfNum);
}

/* (S,G), *,G Tables  functions */


/*********************************************************************
* @purpose  Obtain the next incoming interface in the SG TABLE
*
* @param    family        @b{(input)} family.
* @param    pSGGrpAddr    @b{(inout)} Group Address
* @param    pSGSrcAddr    @b{(inout)} Source Address
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGIEntryNextGet(L7_uchar8 family, 
                                 L7_inet_addr_t *pSGGrpAddr, 
                                 L7_inet_addr_t *pSGSrcAddr,
                                 L7_int32 *pIntIfNum)
{
    return pimsmMapSGIEntryNextGet(family, pSGGrpAddr, 
                                   pSGSrcAddr, pIntIfNum);
}

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
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGExpiryTimeGet(L7_uchar8 family, 
                                 L7_inet_addr_t *pSGGrpAddr, 
                                 L7_inet_addr_t *pSGSrcAddr,
                                 L7_uint32 *pExpiryTime)
{
    return pimsmMapSGExpiryTimeGet(family, pSGGrpAddr, 
                                   pSGSrcAddr, pExpiryTime);
}

/*********************************************************************
* @purpose  To get the up Time in SG TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pSGGrpAddr    @b{(input)}  Group Address
* @param    pSGSrcAddr    @b{(input)}  Source Address
* @param    pUpTime       @b{(output)} expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGUpTimeGet(L7_uchar8 family, 
                              L7_inet_addr_t *pSGGrpAddr, 
                              L7_inet_addr_t *pSGSrcAddr,
                              L7_uint32 *pUpTime)
{
    return pimsmMapSGUpTimeGet(family, pSGGrpAddr, 
                               pSGSrcAddr, pUpTime);
}

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
                               L7_inet_addr_t *pRpfAddr)
{
    return pimsmMapSGRpfAddrGet(family, pSGGrpAddr, 
                                pSGSrcAddr, pRpfAddr);
}

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
                             L7_uint32      *pSGFlags)
{
    return pimsmMapSGFlagsGet(family, pSGGrpAddr,
                              pSGSrcAddr, pSGFlags);
}

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
                             L7_uint32      *pSGIfIndex)
{
    return pimsmMapSGIfIndexGet(family, pSGGrpAddr,
                                pSGSrcAddr, pSGIfIndex);
}
/* Star G Table API's */
/*********************************************************************
* @purpose  To get the next entry in Star G TABLE.
*
* @param    family        @b{(input)} family.
* @param    pGrpAddr      @b{(inout)} Star G Group Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGEntryNextGet(L7_uchar8 family, 
                                    L7_inet_addr_t *pGrpAddr)
{
    return pimsmMapStarGEntryNextGet(family, pGrpAddr);
}

/*********************************************************************
* @purpose  Obtain the next incoming interface in the Star G TABLE
*
* @param    family        @b{(input)} family.
* @param    pGrpAddr      @b{(inout)} Star G Group Address
* @param    pIntIfNum     @b{(inout)} Star G Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGIEntryNextGet(L7_uchar8 family, 
                                     L7_inet_addr_t *pGrpAddr, 
                                     L7_int32 *pIntIfNum)
{
    return pimsmMapStarGIEntryNextGet(family, pGrpAddr, pIntIfNum);
}

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
                                     L7_uint32 *pExpiryTime)
{
    return pimsmMapStarGExpiryTimeGet(family, pGrpAddr, pExpiryTime);
}

/*********************************************************************
* @purpose  To get the up Time in Star G TABLE.
*
* @param    family        @b{(input)}  family.
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pUpTime       @b{(output)} Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmStarGUpTimeGet(L7_uchar8 family, 
                                 L7_inet_addr_t *pGrpAddr, 
                                 L7_uint32 *pUpTime)
{
    return pimsmMapStarGUpTimeGet(family, pGrpAddr, pUpTime);
}

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
                               L7_inet_addr_t *pRpfAddr)
{
    return pimsmMapStarGRpfAddrGet(family, pGrpAddr, pRpfAddr);
}

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
                                L7_uint32      *pFlags)
{
    return pimsmMapStarGFlagsGet(family, pGrpAddr, pFlags);
}

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
                                  L7_uint32      *pIfIndex)
{
    return pimsmMapStarGIfIndexGet(family, pGrpAddr, pIfIndex);
}
#ifdef PIMSM_TBD
/* Star Star RP Table API's */
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
                                         L7_inet_addr_t *pRpAddr)
{
    return pimsmMapStarStarRPEntryNextGet(family, pRpAddr);
}

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
                                          L7_uint32 *pIntIfNum)
{
    return pimsmMapStarStarRPIEntryNextGet(family, pRpAddr, pIntIfNum);
}

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
                                          L7_uint32 *pExpiryTime)
{
    return pimsmMapStarStarRPExpiryTimeGet(family, pRpAddr, pExpiryTime);
}

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
                                      L7_uint32 *pUpTime)
{
    return pimsmMapStarStarRPUpTimeGet(family, pRpAddr, pUpTime);
}

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
                                       L7_inet_addr_t *pRpfAddr)
{
    return pimsmMapStarStarRPRpfAddrGet(family, pRpAddr, pRpfAddr);
}

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
                                     L7_uint32      *pFalgs)
{
    return pimsmMapStarStarRPFlagsGet(family, pRpAddr, pFalgs);
}

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
                                       L7_uint32      *pIfIndex)
{
    return pimsmMapStarStarRPIfIndexGet(family, pRpAddr, pIfIndex);
}
#endif


/*********************************************************************
* @purpose  Gets the Generation ID value for the specified interface.
*
* @param    intIfNum @b{(input)}  internal Interface number
* @param    inetIPVersion    IPV4/V6
* @param    *ipAddr   @b{(output)} IP Address
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
                                       L7_uint32 inetIPVer, L7_uint32 *genIDValue)
{
     return pimsmMapInterfaceGenerationIDValueGet(intIfNum,inetIPVer,genIDValue);
}




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
                                            L7_int32 useDRPriority)
{
    return L7_FAILURE;/* Internal API ALAWAYS RETURNS SUUCESS*/

}

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
                                                L7_int32 *useDRPriority)
{
   return  L7_SUCCESS; /*THIS OBJECT ALAWAYS RETURNS TRUE*/

}


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
                                        L7_int32 bSRBorder)
{

     return pimsmMapInterfaceBSRBorderSet(intIfNum,inetIPVer,bSRBorder);

}

/*********************************************************************
* @purpose  Gets the BSRBorder option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    inetIPVersion    IPV4/V6
* @param    *bSRBorder      BSRBorder option (TRUE/FALSE)
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
                                                L7_int32 *bSRBorder)
{
     return pimsmMapInterfaceBSRBorderGet(intIfNum,inetIPVer,bSRBorder);
     
}

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
                                          L7_inet_addr_t *inetIPAddr, L7_uint32 *bNbrDRPrio)
{
     return pimsmMapNeighborDRPriorityPresentGet(intIfNum,inetIPVersion,inetIPAddr,bNbrDRPrio);

}

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
L7_RC_t usmDbPimsmSGEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
     return pimsmMapSGEntryGet(addrType,pimSGGrpAddr,pimSGSrcAddr);

}

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
L7_RC_t usmDbPimsmSGEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{

    return  pimsmMapSGEntryNextGet(addrType,pimSGGrpAddr,pimSGSrcAddr);

}


/*********************************************************************
* @purpose  To get the SG mode.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgmode         SG PIM mode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGPimModeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                               pimMode_t *sgmode)
{
     return pimsmMapSGPimModeGet (addrType, pimSGGrpAddr, pimSGSrcAddr, sgmode);
}

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
                                    L7_int32 *sgstate)
{
     return pimsmMapSGUpstreamJoinStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgstate);

}

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
                                    L7_uint32 *sgjoinTimer)
{

     return pimsmMapSGUpstreamJoinTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgjoinTimer);
}

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
                                    L7_inet_addr_t *upstreamNbr)
{
     return pimsmMapSGUpstreamNeighborGet(addrType,pimSGGrpAddr,pimSGSrcAddr,upstreamNbr);
}

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
                                    L7_uint32 *rpfIfIndex)
{
      return pimsmMapSGRPFIfIndexGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfIfIndex);

}
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
                                    L7_inet_addr_t *rpfNextHop)
{
    return  pimsmMapSGRPFNextHopGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfNextHop);

}

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
                                    L7_uint32 *rpfprotocol)
{
     return pimsmMapSGRPFRouteProtocolGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfprotocol);

}

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
                                    L7_inet_addr_t *rpfRtAddr)
{
    return pimsmMapSGRPFRouteAddressGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfRtAddr);

}
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
                                    L7_uint32 *sgrtLen)
{
     return pimsmMapSGRPFRoutePrefixLengthGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgrtLen);

}

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
                                    L7_uint32 *sgrtMetricPref)
{

   return pimsmMapSGRPFRouteMetricPrefGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgrtMetricPref);

}

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
                                    L7_uint32 *sgrtMetric)
{
    return pimsmMapSGRPFRouteMetricGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgrtMetric);

}

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
                                    L7_uint32 *sgSPTBit)
{
   return pimsmMapSGSPTBitGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgSPTBit);

}

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
                                    L7_uint32 *sgKeepAliveTimer)
{
    return pimsmMapSGKeepaliveTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgKeepAliveTimer);
 
}

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
                                    L7_uint32 *sgDRRegState)
{
   return pimsmMapSGDRRegisterStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgDRRegState);
 
}

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
                                    L7_uint32 *sgDRRegTimer)
{
    return pimsmMapSGDRRegisterStopTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgDRRegTimer);

}

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
                                    L7_inet_addr_t *rpfNextHop)
{
    return pimsmMapSGRPRegisterPMBRAddressGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfNextHop);
  
}
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
L7_RC_t usmDbPimsmSGRptEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
     return  pimsmMapSGRptEntryGet(addrType,pimSGGrpAddr,pimSGSrcAddr);
}

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
L7_RC_t usmDbPimsmSGRptEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
      return  pimsmMapSGRptEntryNextGet(addrType,pimSGGrpAddr,pimSGSrcAddr);
 
}

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
                                    L7_uint32 *sgRPTUpTime)
{
     return pimsmMapSGRptUpTimeGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgRPTUpTime);
 
}

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
                                    L7_int32 *sgRPTState)
{
      return pimsmMapSGRPTUpstreamPruneStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgRPTState);

}

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
                                    L7_uint32 *sgRPTOverrideTimer)
{
    return  pimsmMapSGRPTUpstreamOverrideTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgRPTOverrideTimer);

}
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
L7_RC_t usmDbPimsmStaticRPEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimStaticRPGrpAddr, L7_uchar8 length)
{

    return pimsmMapStaticRPEntryGet(addrType,pimStaticRPGrpAddr,length);
               

}

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
L7_RC_t usmDbPimsmStaticRPEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimStaticRPGrpAddr, L7_uchar8 *length)
{

    return pimsmMapStaticRPNextGet(addrType,pimStaticRPGrpAddr,length);

}


  
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
                                    L7_uint32 *pimStaticRPPimMode)
{
    *pimStaticRPPimMode = PIM_MODE_ASM;
    return L7_SUCCESS;

}

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
                                    L7_uint32 pimStaticRPPimMode)
{
    /* Default mode is PIM_MODE_ASM, other modes are not supported */
    return L7_SUCCESS;
    
}

 
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
                                             L7_inet_addr_t *pimMappingRPAddr)
{
    L7_inet_addr_t groupMask;
    if( inetMaskLenToMask(addrType,length, &groupMask) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }    
    return pimsmMapRPGroupEntryGet(addrType, origin, pimMappingGrpAddr,
                                   &groupMask,pimMappingRPAddr);  

}

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
L7_RC_t usmDbPimsmGroupMappingEntryNextGet(L7_uchar8 *origin,L7_uchar8 addrType, 
                                                 L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 *length, 
                                                 L7_inet_addr_t *pimMappingRPAddr)
{

    L7_inet_addr_t groupMask;
    if( inetMaskLenToMask(addrType,*length, &groupMask) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if ( pimsmMapRpGroupEntryNextGet(addrType ,origin, pimMappingGrpAddr,
                                       &groupMask, pimMappingRPAddr) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (inetMaskToMaskLen(&groupMask, length) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    return L7_SUCCESS;

}

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
                                               L7_uint32 *pimGroupMappingMode)
{
     return  pimsmMapGroupMappingPimModeGet(origin,addrType,pimMappingGrpAddr,length
                                            ,pimMappingRPAddr,pimGroupMappingMode);
}

/************ END OF PIMSM GROUP MAPPING TABLE USMDB APIs ***************/

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
                                             L7_uchar8 *length)
{
 
    return pimsmMapCandRPEntryNextGet(addrType, bsrCandtRPAddr,
                                           bsrCandtRPGrpAddr,length);
    
}


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
                                              L7_inet_addr_t *bsrCandtRPGrpAddr, L7_uint32 length, L7_uint32 mode)
{
   return L7_FAILURE;

    /*return mcastMapCmd(L7_MCAST_IANA_MROUTE_PIM_SM,
                       MCAST_PIMSM_NEW_MIB_BSRCANDTRP_MODE_SET_CMD,
                       addrType, (L7_uint32)bsrCandtRPAddr,(L7_uint32)bsrCandtRPGrpAddr, length, mode,0,0,0,0,0);*/
}

/************ END OF PIMSM BSR CANDIDATE RP TABLE USMDB APIs ***************/


 
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
L7_RC_t usmDbPimsmbsrElectedBSREntryGet(L7_uchar8 addrType) 
{
    return pimsmMapbsrElectedBSREntryGet(addrType);

}

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
L7_RC_t usmDbPimsmbsrElectedBSREntryNextGet(L7_int32 *addrType) 
{
     return pimsmMapbsrElectedBSREntryNextGet(addrType);

}

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
L7_RC_t usmDbPimsmbsrElectedBSRAddrGet(L7_uint32 addrType,L7_uint32 *bsrElectedBSRAddr)
{
     return pimsmMapbsrElectedBSRAddrGet(addrType,bsrElectedBSRAddr);

}

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
L7_RC_t usmDbPimsmbsrElectedBSRPriorityGet(L7_uchar8 addrType, L7_uint32 *bsrElectedBSRPriority)
{
     return pimsmMapbsrElectedBSRPriorityGet(addrType,bsrElectedBSRPriority);
}


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
L7_RC_t usmDbPimsmbsrElectedBSRHashMaskLengthGet(L7_uchar8 addrType, L7_uint32 *hashMaskLen)
{
    return  pimsmMapbsrElectedBSRHashMaskLengthGet(addrType,hashMaskLen);

}



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
L7_RC_t usmDbPimsmbsrElectedBSRExpiryTimeGet(L7_uchar8 addrType, L7_uint32 *bsrElectedBSRExpiryTime)
{
    return  pimsmMapbsrElectedBSRExpiryTimeGet(addrType,bsrElectedBSRExpiryTime);
}
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
L7_RC_t usmDbPimsmbsrElectedBSRCRPAdvTimerGet(L7_uchar8 addrType, L7_uint32 *bsrCRPAdvTimer)
{
     return pimsmMapbsrElectedBSRCRPAdvTimerGet(addrType,bsrCRPAdvTimer);

}

/************ END OF PIMSM BSR Elected BSR TABLE USMDB APIs ***************/

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
L7_RC_t usmDbPimsmStarGEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr) 
{
     return  pimsmMapStarGEntryGet(addrType,pimStarGGrpAddr);
}


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
L7_RC_t usmDbPimsmStarGPimModeGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGPimMode)
{
     return L7_FAILURE;/* WE CAN REMOVE THIS */

}


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
L7_RC_t usmDbPimsmStarGRPAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr,
                                        L7_inet_addr_t *pimStarGRPAddr)
{
   return pimsmMapStarGRPAddrGet(addrType,pimStarGGrpAddr,pimStarGRPAddr);
}

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
L7_RC_t usmDbPimsmStarGRPOriginGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr,
                                         L7_uint32 *pimStarGRPOrigin)
{
      return pimsmMapStarGRPOriginGet(addrType,pimStarGGrpAddr,pimStarGRPOrigin);
 
}
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
L7_RC_t usmDbPimsmStarGRPIsLocalGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *pimStarGRPIsLocal)
{
    return pimsmMapStarGRPIsLocalGet(addrType,pimStarGGrpAddr,pimStarGRPIsLocal);

}


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
L7_RC_t usmDbPimsmStarGUpstreamJoinStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, 
                                                 L7_uint32 *JoinState)
{
  return pimsmMapStarGUpstreamJoinStateGet(addrType,pimStarGGrpAddr,JoinState);
}

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
L7_RC_t usmDbPimsmStarGUpstreamJoinTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, 
                                                   L7_uint32 *JoinTimer)
{
  return  pimsmMapStarGUpstreamJoinTimerGet(addrType,pimStarGGrpAddr,JoinTimer);
}



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
L7_RC_t usmDbPimsmStarGUpstreamNbrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, 
                                       L7_inet_addr_t *UpstreamNbr)
{
    return pimsmMapStarGUpstreamNbrGet(addrType,pimStarGGrpAddr,UpstreamNbr);

}

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
L7_RC_t usmDbPimsmStarGRPFIfIndexGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFIfIndex)
{
     return pimsmMapStarGRPFIfIndexGet(addrType,pimStarGGrpAddr,RPFIfIndex);
}

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
L7_RC_t usmDbPimsmStarGRPFNextHopGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *RPFNextHop)
{
   return pimsmMapStarGRPFNextHopGet(addrType,pimStarGGrpAddr,RPFNextHop);
   
}

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
L7_RC_t usmDbPimsmStarGRPFRouteProtocolGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr,
                                           L7_uint32 *RouteProtocol)
{
     return pimsmMapStarGRPFRouteProtocolGet(addrType,pimStarGGrpAddr,RouteProtocol);

}

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
L7_RC_t usmDbPimsmStarGRPFRouteAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_inet_addr_t *RPFRouteAddr)
{
    return pimsmMapStarGRPFRouteAddrGet(addrType,pimStarGGrpAddr,RPFRouteAddr);

}

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
L7_RC_t usmDbPimsmStarGRPFRoutePrefixLenGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRoutePrefixLen)
{
     return pimsmMapStarGRPFRoutePrefixLenGet(addrType,pimStarGGrpAddr,RPFRoutePrefixLen);
    
}

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
L7_RC_t usmDbPimsmStarGRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRouteMetricPref)
{
    return  pimsmMapStarGRPFRouteMetricPrefGet(addrType,pimStarGGrpAddr,RPFRouteMetricPref);

}

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
L7_RC_t usmDbPimsmStarGRPFRouteMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 *RPFRouteMetric)
{
   return pimsmMapStarGRPFRouteMetricGet(addrType,pimStarGGrpAddr,RPFRouteMetric);

}
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
L7_RC_t usmDbPimsmStarGIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex) 
{
   return  pimsmMapStarGIEntryGet(addrType,pimStarGIGrpAddr,IfIndex);


}


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
L7_RC_t usmDbPimsmStarGIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *pimStarGIUpTime)
{
     return pimsmMapStarGIUpTimeGet(addrType,pimStarGIGrpAddr,IfIndex,pimStarGIUpTime);

}

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
L7_RC_t usmDbPimsmStarGILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *localMemShip)
{
    return pimsmMapStarGILocalMembershipGet(addrType,pimStarGIGrpAddr,IfIndex,localMemShip);

}

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
L7_RC_t usmDbPimsmStarGIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinState)
{
   return  pimsmMapStarGIJoinPruneStateGet(addrType,pimStarGIGrpAddr,IfIndex,JoinState);
}

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
L7_RC_t usmDbPimsmStarGIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinTimer)
{
    return pimsmMapStarGIPrunePendingTimerGet(addrType,pimStarGIGrpAddr,IfIndex,JoinTimer);

}

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
L7_RC_t usmDbPimsmStarGIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *JoinExpiry)
{
    return pimsmMapStarGIJoinExpiryTimerGet(addrType,pimStarGIGrpAddr,IfIndex,JoinExpiry);

}

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
L7_RC_t usmDbPimsmStarGIAssertStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *AssertState)
{
    return pimsmMapStarGIAssertStateGet(addrType,pimStarGIGrpAddr,IfIndex,AssertState);

}

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
L7_RC_t usmDbPimsmStarGIAssertTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_uint32 *AssertTimer)
{
     return pimsmMapStarGIAssertTimerGet(addrType,pimStarGIGrpAddr,IfIndex,AssertTimer);

}

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
L7_RC_t usmDbPimsmStarGIAssertWinnerAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGIGrpAddr, L7_int32 IfIndex, L7_inet_addr_t *AssertWinner)
{
     return pimsmMapStarGIAssertWinnerAddrGet(addrType,pimStarGIGrpAddr,IfIndex,AssertWinner);

}

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
L7_RC_t usmDbPimsmStarGIRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_int32 ifindex, L7_uint32 *AssertWinnerMetricPref)
{
     return L7_SUCCESS;

}

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
L7_RC_t usmDbPimsmStarGIAssertWinnerMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimStarGGrpAddr, L7_int32 ifindex, L7_uint32 *AssertWinnerMetric)
{
     return pimsmMapStarGIAssertWinnerMetricGet(addrType,pimStarGGrpAddr,ifindex,AssertWinnerMetric);

}

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
L7_RC_t usmDbPimsmSGIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg)
{
    return pimsmMapSGIEntryGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg);

}


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
L7_RC_t usmDbPimsmSGIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *sgUpTime)
{
    return pimsmMapSGIUpTimeGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,sgUpTime);

}

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAd
dr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimsmSGILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                             L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_int32 *LocalMembership)
{
    return pimsmMapSGILocalMembershipGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,LocalMembership);

}

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
L7_RC_t usmDbPimsmSGIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                 L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_uint32 *JoinPruneState)
{
   return pimsmMapSGIJoinPruneStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,JoinPruneState);
 
}

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
L7_RC_t usmDbPimsmSGIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinTimer)
{
    return pimsmMapSGIPrunePendingTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,JoinTimer);

}

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
L7_RC_t usmDbPimsmSGIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinExpiry)
{
   return  pimsmMapSGIJoinExpiryTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,JoinExpiry);

}

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
L7_RC_t usmDbPimsmSGIAssertStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                               L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertState)
{
    return pimsmMapSGIAssertStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertState);
 
}

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
L7_RC_t usmDbPimsmSGIAssertTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                        L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertTimer)
{
    return pimsmMapSGIAssertTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertTimer);

}

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
L7_RC_t usmDbPimsmSGIAssertWinnerAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_inet_addr_t *AssertWinner)
{
    return pimsmMapSGIAssertWinnerAddrGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertWinner);
}

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
L7_RC_t usmDbPimsmSGIRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetricPref)
{
    return pimsmMapSGIRPFRouteMetricPrefGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertWinnerMetricPref);
}

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
L7_RC_t usmDbPimsmSGIAssertWinnerMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                         L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetric)
{
     return pimsmMapSGIAssertWinnerMetricGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertWinnerMetric);
    
}

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
L7_RC_t usmDbPimsmSGRptIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg)
{
     return  pimsmMapSGRptIEntryGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg);
}

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
L7_RC_t usmDbPimsmSGRptIEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 *ifindexg)
{
    return pimsmMapSGRptIEntryNextGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg);

}

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
L7_RC_t usmDbPimsmSGRptIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                           L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_uint32 *sgUpTime)
{

   return pimsmMapSGRptIUpTimeGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,sgUpTime);
      
}

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
L7_RC_t usmDbPimsmSGRptILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_int32 *LocalMembership)
{
     return pimsmMapSGRptILocalMembershipGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,LocalMembership);
   
}

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
L7_RC_t usmDbPimsmSGRptIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                    L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_uint32 *JoinPruneState)
{

    return pimsmMapSGRptIJoinPruneStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,JoinPruneState);
}

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
L7_RC_t usmDbPimsmSGRptIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                      L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinTimer)
{
    return pimsmMapSGRptIPrunePendingTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,JoinTimer);

}

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
L7_RC_t usmDbPimsmSGRptIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinExpiry)
{
   return pimsmMapSGRptIJoinExpiryTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,JoinExpiry);

}

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
                                             L7_inet_addr_t *pimMappingRPAddr)
{

   return pimsmMapbsrRPSetEntryGet(origin,addrType,pimMappingGrpAddr,length,pimMappingRPAddr);
}

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
                                                 L7_inet_addr_t *pimMappingRPAddr)
{
       return pimsmMapbsrRPSetEntryNextGet(origin,addrType,pimMappingGrpAddr,length,pimMappingRPAddr);

}

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
                                               L7_uint32 *RPSetPriority)
{

     return pimsmMapbsrRPSetPriorityGet(origin,addrType,pimMappingGrpAddr,length,
                                        pimMappingRPAddr,RPSetPriority);
}

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
                                               L7_uint32 *RPSetHoldtime)
{
     return pimsmMapbsrRPSetHoldtimeGet(origin,addrType,pimMappingGrpAddr,length,
                                                         pimMappingRPAddr,RPSetHoldtime);

}


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
                                               L7_uint32 *RPSetExpiryTime)
{
 
  return pimsmMapbsrRPSetExpiryTimeGet(origin,addrType,pimMappingGrpAddr,length,
                                        pimMappingRPAddr,RPSetExpiryTime); 
   
}



/***********************************************************************************
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
***********************************************************************************/
L7_RC_t usmDbPimsmCliStaticRpNextGet(L7_uchar8 family, L7_inet_addr_t *rpAddr, L7_inet_addr_t *rpGrpAddr, L7_uchar8 *prefixLen,
                                      L7_uint32*indexstart,L7_BOOL *override)
  
 {
  

    return pimsmMapCliStaticRpNextGet(family,rpAddr,rpGrpAddr,prefixLen,indexstart,override);
 

 }
/**********************************************************************************
* @purpose   To get the next candidateRp entry
* @param UnitIndex            L7_uint32   Unit Number       
* @param family               whether IPV4/IPV6
* @param intIfNum             intIfNum
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
**********************************************************************************/

 L7_RC_t usmDbPimsmCliCandidateRPEntryNextGet(L7_uint32 UnitIndex,
                                          L7_uchar8 family,
                                          L7_uint32 *intIfNum,
                                          L7_inet_addr_t *cRPGroupAddress,
                                          L7_uchar8      *cRPprefixLen, L7_uint32 *indexstart)
{

   
    return pimsmMapCliCandidateRPEntryNextGet(family,intIfNum,cRPGroupAddress,cRPprefixLen,indexstart);


}

/* The below wrappers are written explicitly for XUI to support the new PIM BSR
 * MIB.  Currently do not use these for CLI/SNMP purposes.
 */
/*********************************************************************
 ************* The BSR Elected BSR RP-Set Table for XUI **************
 ********************************************************************/

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
                               L7_inet_addr_t *pimMappingRPAddr)
{
  return pimsmMapbsrRPSetEntryGet (L7_PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                   addrType,
                                   pimMappingGrpAddr,
                                   length,
                                   pimMappingRPAddr);
}

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
                                   L7_inet_addr_t *pimMappingRPAddr)
{
  L7_uchar8 origin = L7_NULL;
  return pimsmMapbsrRPSetEntryNextGet (&origin,
                                       addrType,
                                       pimMappingGrpAddr,
                                       length,
                                       pimMappingRPAddr);
}

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
                                  L7_uint32 *RPSetPriority)
{
  return pimsmMapbsrRPSetPriorityGet (L7_PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                      addrType,
                                      pimMappingGrpAddr,
                                      length,
                                      pimMappingRPAddr,
                                      RPSetPriority);
}

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
                                  L7_uint32 *RPSetHoldtime)
{
  return pimsmMapbsrRPSetHoldtimeGet (L7_PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                      addrType,
                                      pimMappingGrpAddr,
                                      length,
                                      pimMappingRPAddr,
                                      RPSetHoldtime);
}

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
                                    L7_uint32 *RPSetExpiryTime)
{
  return pimsmMapbsrRPSetExpiryTimeGet (L7_PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR,
                                        addrType,
                                        pimMappingGrpAddr,
                                        length,
                                        pimMappingRPAddr,
                                        RPSetExpiryTime);
}

/*********************************************************************
 ************* The BSR Candidate-BSR Table for XUI *******************
 ********************************************************************/

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
usmDbPimsmXUICandBSREntryGet (L7_uchar8 zoneIndex) 
{
  return pimsmMapCandBSREntryGet (zoneIndex);
}

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
usmDbPimsmXUICandBSREntryNextGet (L7_uchar8 zoneIndex) 
{
  return pimsmMapCandBSREntryNextGet (zoneIndex);
}

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
                                  L7_uint32 *interface)
{
  return pimsmMapCandBSRInterfaceGet (zoneIndex, interface);
}
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
                             L7_inet_addr_t *cbsrAddr)
{
  return pimsmMapCandBSRAddrGet (zoneIndex, cbsrAddr);
}

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
                             L7_inet_addr_t *cbsrAddr)
{
  return pimsmMapCandBSRAddrSet (zoneIndex, cbsrAddr);
}

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
                                 L7_uint32 *cbsrPriority)
{
  return pimsmMapCandBSRPriorityGet (zoneIndex, cbsrPriority);
}

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
                                 L7_uint32 cbsrPriority)
{
  return pimsmMapCandBSRPrioritySet (zoneIndex, cbsrPriority);
}

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
                                       L7_uint32 *hashMaskLen)
{
  return pimsmMapCandBSRHashMaskLengthGet (zoneIndex, hashMaskLen);
}

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
                                       L7_uint32 hashMaskLen)
{
  return pimsmMapCandBSRHashMaskLengthSet (zoneIndex, hashMaskLen);
}

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
                                   L7_uint32 *electedBsr)
{
  return pimsmMapCandBSRElectedBSRGet (zoneIndex, electedBsr);
}

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
                                       L7_uint32 *bsTimer)
{
  return pimsmMapCandBSRBootstrapTimerGet (zoneIndex, bsTimer);
}

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
                             L7_uint32 *mode)
{
  return pimsmMapCandBSRModeGet(zoneIndex, mode);
}

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
                             L7_uint32 mode)
{
  return pimsmMapCandBSRModeSet (zoneIndex, mode);
}

/*********************************************************************
 *************** The BSR Elected BSR Table for XUI *******************
 ********************************************************************/

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
usmDbPimsmbsrXUIElectedBSREntryGet (L7_uchar8 zoneIndex) 
{
  return pimsmMapbsrElectedBSREntryGet (zoneIndex);
}

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
usmDbPimsmbsrXUIElectedBSREntryNextGet (L7_int32 *zoneIndex) 
{
  return pimsmMapbsrElectedBSREntryNextGet (zoneIndex);
}

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
                                   L7_uint32 *bsrElectedBSRAddr)
{
  return pimsmMapbsrElectedBSRAddrGet (zoneIndex,
                                       bsrElectedBSRAddr);
}

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
                                       L7_uint32 *bsrElectedBSRPriority)
{
  return pimsmMapbsrElectedBSRPriorityGet (zoneIndex,
                                           bsrElectedBSRPriority);
}

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
                                             L7_uint32 *hashMaskLen)
{
  return pimsmMapbsrElectedBSRHashMaskLengthGet (zoneIndex,
                                                 hashMaskLen);
}

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
                                         L7_uint32 *bsrElectedBSRExpiryTime)
{
  return pimsmMapbsrElectedBSRExpiryTimeGet (zoneIndex,
                                             bsrElectedBSRExpiryTime);
}

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
                                          L7_uint32 *bsrCRPAdvTimer)
{
  return pimsmMapbsrElectedBSRCRPAdvTimerGet (zoneIndex,
                                              bsrCRPAdvTimer);
}

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
                                   L7_inet_addr_t *elecBSRAddress)
{
  return pimsmMapElectedBSRAddressGet(zoneIndex, elecBSRAddress);
}

/************************************************************************
* @purpose  Get the Elected Bsr Candidate Priority for the router.
*
* @param    zoneIndex       @b{(input)}  Scope Zone index
* @param    priority        @b{(output)} Priority
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
                                    L7_uint32 *priority)
{
  return pimsmMapElectedBSRPriorityGet (zoneIndex, priority);
}

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
                                          L7_uint32 *hashMaskLen)
{
  return pimsmMapElectedBSRHashMaskLengthGet (zoneIndex, hashMaskLen);
}

