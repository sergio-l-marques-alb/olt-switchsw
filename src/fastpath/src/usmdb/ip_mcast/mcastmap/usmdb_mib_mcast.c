/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_mib_mcast.c
*
* @purpose  Provide interface to  mcast components
*
* @component unitmgr
*
* @comments
*
* @create 15/05/2002
*
* @author Nitish
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "usmdb_mib_mcast_api.h"

#include "l3_mcast_commdefs.h"
#include "mcast_debug_api.h"
#include "l7_mcast_api.h"

/***************************************************************
*
* @purpose  Set the MCAST administrative mode
*
* @param    UnitIndex       @b{(input)}    unit
* @param    mode            @b{(input)}    Administrative Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastAdminModeSet(L7_uint32 UnitIndex, L7_uint32 mode)
{
    return mcastMapMcastAdminModeSet(mode);
}

/***************************************************************
*
* @purpose  Gets the MCAST administrative mode
*
* @param    UnitIndex       @b{(input)}    unit
* @param    mode            @b{(output)}   Administrative Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *mode)
{
    return mcastMapMcastAdminModeGet(mode);
}

/***************************************************************
*
* @purpose  Gets the multicast protocal running on the router.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    family          @b{(input)}    IP address family
* @param    proto           @b{(output)}   Multicast Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  protocol
*
* @comments    none
*
* @end
***************************************************************/
L7_RC_t usmDbMcastIpProtocolGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                L7_MCAST_IANA_PROTO_ID_t *proto)
{
    return mcastMapIpCurrentMcastProtocolGet(family, proto);
}

/**********************************************************************
*
* @purpose  Get the IP Multicast route table  entry count
*
* @param    UnitIndex       @b{(input)}    unit
* @param    count           @b{(output)}   Number of Entires in Mroute Table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
**********************************************************************/
L7_RC_t usmDbMcastIpMRouteEntryCountGet(L7_uint32 UnitIndex, L7_uint32 *count)
{
    return mcastMapIpMRouteEntryCountGet(L7_AF_INET, count);
}

/*********************************************************************
*
* @purpose  Get the IP Multicast route table  Highest entry count
*
* @param    UnitIndex       @b{(input)}    unit
* @param    count           @b{(output)}   Highest Entry Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
**********************************************************************/
L7_RC_t usmDbMcastIpMRouteHighestEntryCountGet(L7_uint32 UnitIndex,
                                               L7_uint32 *count)
{
    return mcastMapIpMRouteHighestEntryCountGet(count);
}

/**********************************************************************
*
* @purpose  Get the IP Multicast route table  Maximum Size
*
* @param    UnitIndex       @b{(input)}    unit
* @param    size            @b{(output)}   Maximum Size of Mroute Table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
**********************************************************************/
L7_RC_t usmDbMcastIpMRouteTableMaxSizeGet(L7_uint32 UnitIndex, L7_uint32 *size)
{
    return mcastMapIpMRouteTableMaxSizeGet(size);
}

/***************************************************************
*
* @purpose  Gets the address of the upstream neighbor from
*           which IP datagrams from these sources to this
*           multicast address are received.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    upstmNbr        @b{(output)}   upstream Neighbor.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  upstmNbr
*
* @comments    returns 0 if upstream neighbor is unknown.
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteUpstreamNbrGet(L7_uchar8 family,
                                         L7_uint32 UnitIndex,
                                         L7_inet_addr_t *ipMRtGrp,
                                         L7_inet_addr_t *ipMRtSrc,
                                         L7_inet_addr_t *ipMRtSrcMask,
                                         L7_inet_addr_t *upstmNbr)
{
    return mcastMapIpMRouteUpstreamNbrGet(family, ipMRtGrp, ipMRtSrc, 
                                          ipMRtSrcMask, upstmNbr);
}

/*****************************************************************
*
* @purpose  Gets the interface index on which IP datagrams sent
*           by these sources to this multicast address are
*           received.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    inIfIndex       @b{(output)}   Incoming Interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  inIfIndex
*
* @comments    A value of 0 indicates that datagrams are not
*           subject to an incoming interface check, but may
*           be accepted on multiple interfaces.
*
* @end
*
*****************************************************************/
L7_RC_t usmDbMcastIpMRouteIfIndexGet(L7_uchar8 family,
                                     L7_uint32 UnitIndex,
                                     L7_inet_addr_t *ipMRtGrp,
                                     L7_inet_addr_t *ipMRtSrc,
                                     L7_inet_addr_t *ipMRtSrcMask,
                                     L7_uint32 *inIfIndex)
{
    return mcastMapIpMRouteIfIndexGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, inIfIndex);
}

/*****************************************************************
*
* @purpose  Gets the time since the multicast routing information
*           represented by this entry was learned by the router.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    upTime          @b{(output)}   Up Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteUpTimeGet(L7_uchar8 family,
                                    L7_uint32 UnitIndex,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_uint32 *upTime)
{
 
    return mcastMapIpMRouteUpTimeGet(family, ipMRtGrp, ipMRtSrc, 
                                     ipMRtSrcMask, upTime);
}

/***************************************************************
*
* @purpose  Gets the minimum amount of time remaining before
*           this entry will be aged out.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    expire          @b{(output)}   Expiry Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    The value of 0 indicates that the entry is not
*           subject to aging.
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteExpiryGet(L7_uchar8 family, 
                                    L7_uint32 UnitIndex,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_uint32 *expTime)
{
    return mcastMapIpMRouteExpiryTimeGet(family, ipMRtGrp, ipMRtSrc,
                                         ipMRtSrcMask, expTime);
}

/***************************************************************
*
* @purpose  Gets multicast routing protocol
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    protocol        @b{(output)}   Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    The multicast routing protocol via which this
*           multicast forwarding entry was learned.
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteProtocolGet(L7_uchar8 family,
                                      L7_uint32 UnitIndex,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_uint32 *protocol)
{
    return mcastMapIpMRouteProtocolGet(family, ipMRtGrp, ipMRtSrc,
                                       ipMRtSrcMask, protocol);
}

/***************************************************************
*
* @purpose  Gets the routing protocol used to find the
*           upstream or parent interface.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    protocol        @b{(output)}   Protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteRtProtoGet(L7_uint32 UnitIndex,
                                     L7_inet_addr_t *ipMRtGrp,
                                     L7_inet_addr_t *ipMRtSrc,
                                     L7_inet_addr_t *ipMRtSrcMask,
                                     L7_uint32 *protocol)
{
    return mcastMapIpMRouteRtProtoGet(ipMRtGrp, ipMRtSrc,
                                      ipMRtSrcMask, protocol);
}

/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(inout)}    Multicast Group Address
* @param    ipMRtSrc        @b{(inout)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(inout)}    Multicast Source Address Mask
* @param    intIfNum        @b{(inout)}   Outgoing Interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
*****************************************************************/
L7_RC_t usmDbMcastIpMRouteOutIntfEntryNextGet(L7_uchar8 family,
                                              L7_uint32 UnitIndex,
                                              L7_inet_addr_t *ipMRtGrp,
                                              L7_inet_addr_t *ipMRtSrc,
                                              L7_inet_addr_t *ipMRtSrcMask,
                                              L7_uint32 *intIfNum)
{
    return mcastMapIpMRouteOutIntfEntryNextGet(family, ipMRtGrp, ipMRtSrc,
                                               ipMRtSrcMask, intIfNum);
}

/***************************************************************
*
* @purpose  Gets the address field of the route used to find
*           the upstream or parent interface
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    rtAddr          @b{(output)}   Route IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteRtAddressGet(L7_uint32 UnitIndex,
                                       L7_inet_addr_t *ipMRtGrp,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *ipMRtSrcMask,
                                       L7_inet_addr_t *rtAddr)
{
    return mcastMapIpMRouteRtAddressGet(ipMRtGrp, ipMRtSrc,
                                        ipMRtSrcMask, rtAddr);
}

/***************************************************************
*
* @purpose  Gets the mask associated with the route used to
*           find the upstream or parent interface.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    rtMask          @b{(output)}   Route Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteRtMaskGet(L7_uint32 UnitIndex,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_inet_addr_t *rtMask)
{
    return mcastMapIpMRouteRtMaskGet(ipMRtGrp, ipMRtSrc, 
                                     ipMRtSrcMask, rtMask);
}

/***************************************************************
*
* @purpose  Gets the type of route used (unicast or multicast)
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    rttype          @b{(output)}   Route Type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  rtType
*
* @comments    The reason the given route was placed in the MRIB.
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteRtTypeGet(L7_uint32 UnitIndex,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRtSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_uint32 *rtType)
{
    return mcastMapIpMRouteRtTypeGet(ipMRtGrp, ipMRtSrc, 
                                     ipMRtSrcMask, rtType);
}

/***************************************************************
*
* @purpose  Gets the Rpf address for the given index
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    rpfAddress      @b{(output)}   RPF Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMrouteRpfAddressGet(L7_uchar8 family,
                                        L7_uint32 UnitIndex,
                                        L7_inet_addr_t *ipMRtGrp,
                                        L7_inet_addr_t *ipMRtSrc,
                                        L7_inet_addr_t *ipMRtSrcMask,
                                        L7_inet_addr_t *rpfAddress)
{
    return mcastMapIpMRouteRpfAddressGet(family, ipMRtGrp, ipMRtSrc,
                                         ipMRtSrcMask, rpfAddress);
}

/***************************************************************
*
* @purpose  Gets PIM specific flags related to a multicast
*           state entry
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
* @param    flag            @b{(output)}   Flag 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    Flag is '0' for RPT (Shared tree) and '1' for
*           SPT(Source Tree)
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteFlagsGet(L7_uchar8 family,
                                   L7_uint32 UnitIndex,
                                   L7_inet_addr_t *ipMRtGrp,
                                   L7_inet_addr_t *ipMRtSrc,
                                   L7_inet_addr_t *ipMRtSrcMask,
                                   L7_uint32 *flag)
{
    return mcastMapIpMRouteFlagsGet(family, ipMRtGrp, ipMRtSrc, 
                                    ipMRtSrcMask, flag);
}

/***************************************************************
*
* @purpose  Get the IP Multicast route table  entry
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteEntryGet(L7_uchar8 family,
                                   L7_uint32 UnitIndex,
                                   L7_inet_addr_t *ipMRtGrp,
                                   L7_inet_addr_t *ipMRtSrc,
                                   L7_inet_addr_t *ipMRtSrcMask)
{
    return mcastMapIpMRouteEntryGet(family, ipMRtGrp, ipMRtSrc, ipMRtSrcMask);
}

/***************************************************************
*
* @purpose  Get the IP Multicast route table Next entry
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(inout)}    Multicast Group Address
* @param    ipMRtSrc        @b{(inout)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(inout)}    Multicast Source Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteEntryNextGet(L7_uchar8 family,
                                       L7_uint32 UnitIndex,
                                       L7_inet_addr_t *ipMRtGrp,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *ipMRtSrcMask)
{
    return mcastMapIpMRouteEntryNextGet(family, ipMRtGrp, ipMRtSrc, ipMRtSrcMask);
}


/***************************************************************
*
* @purpose  Gets the indication of whether the outgoing interface
*           and next hop represented by the given entry is currently
*           being used to forward IP datagrams.
*
* @param    UnitIndex            @b{(input)} Unit
* @param    ipMRtNextHopGrp      @b{(input)} Next Hop Group Address
* @param    ipMRtNextHopSrc      @b{(input)} Next Hop Source Address
* @param    ipMRtNextHopSrcMask  @b{(input)} Next Hop Source Mask
* @param    ipMRtNextHopIfIndex  @b{(input)} Next Hop If Index
* @param    ipMRtNextHopAddr     @b{(input)} Next Hop Router Address
* @param    state                @b{(output)} Next Hop state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    If state is forwarding indicates it is currently being
*           used and if the value 'pruned' indicates it is not
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteNextHopStateGet(L7_uint32 UnitIndex,
                                  L7_inet_addr_t *ipMRtNextHopGrp,
                                  L7_inet_addr_t *ipMRtNextHopSrc,
                                  L7_inet_addr_t *ipMRtNextHopSrcMask,
                                  L7_uint32 ipMRtNextHopIfIndex,
                                  L7_inet_addr_t *ipMRtNextHopAddr,
                                  L7_uint32 *state)
{
    return mcastMapNextHopMRouteStateGet(ipMRtNextHopGrp,
                                   ipMRtNextHopSrc,
                                   ipMRtNextHopSrcMask,
                                   ipMRtNextHopIfIndex,
                                   ipMRtNextHopAddr,
                                   state);
}

/***************************************************************
*
* @purpose  Checks whether the entry corresponding to the given
*           source group, and source mask is there in the next
*           hop routing table
*
* @param    UnitIndex            @b{(input)} Unit
* @param    ipMRtNextHopGrp      @b{(input)} Next Hop Group Address
* @param    ipMRtNextHopSrc      @b{(input)} Next Hop Source Address
* @param    ipMRtNextHopSrcMask  @b{(input)} Next Hop Source Mask
* @param    ipMRtNextHopIfIndex  @b{(input)} Next Hop If Index
* @param    ipMRtNextHopAddr     @b{(input)} Next Hop Router Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
****************************************************************/
L7_RC_t usmDbMcastIpMRouteNextHopEntryGet(L7_uint32 UnitIndex,
                                          L7_inet_addr_t *ipMRtNextHopGrp,
                                          L7_inet_addr_t *ipMRtNextHopSrc,
                                          L7_inet_addr_t *ipMRtNextHopSrcMask,
                                          L7_uint32 ipMRtNextHopIfIndex,
                                          L7_inet_addr_t *ipMRtNextHopAddr)
{
    return mcastMapNextHopMRouteEntryGet(ipMRtNextHopGrp,
                                           ipMRtNextHopSrc,
                                           ipMRtNextHopSrcMask,
                                           ipMRtNextHopIfIndex,
                                           ipMRtNextHopAddr);
}

/***************************************************************
*
* @purpose  Gets the next entry in the next hop routing table
*           for the given source, group and source mask.
*
* @param    UnitIndex            @b{(input)}  Unit
* @param    ipMRtNextHopGrp      @b{(inout)} Next Hop Group Address
* @param    ipMRtNextHopSrc      @b{(inout)} Next Hop Source Address
* @param    ipMRtNextHopSrcMask  @b{(inout)} Next Hop Source Mask
* @param    ipMRtNextHopIfIndex  @b{(inout)} Next Hop If Index
* @param    ipMRtNextHopAddr     @b{(inout)} Next Hop Router Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteNextHopEntryNextGet(L7_uint32 UnitIndex,
                                      L7_inet_addr_t *ipMRtNextHopGrp,
                                      L7_inet_addr_t *ipMRtNextHopSrc,
                                      L7_inet_addr_t *ipMRtNextHopSrcMask,
                                      L7_uint32 ipMRtNextHopIfIndex,
                                      L7_inet_addr_t *ipMRtNextHopAddr)
{
    return mcastMapNextHopMRouteEntryNextGet(ipMRtNextHopIfIndex,
                                               ipMRtNextHopGrp,
                                               ipMRtNextHopSrc,
                                               ipMRtNextHopSrcMask,
                                               ipMRtNextHopAddr);
}

/***************************************************************
*
* @purpose  Set the Ttl threshold of the specified interface
*
* @param    UnitIndex       @b{(input)}    unit
* @param    intIfNum        @b{(input)}    Internal Interface Number
* @param    ifTtl           @b{(input)}    TTL threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
****************************************************************/
L7_RC_t usmDbMcastIpMRouteInterfaceTtlSet(L7_uint32 UnitIndex,
                                          L7_uint32 intIfNum,
                                          L7_uint32 ifTtl)
{
    return mcastMapIntfTtlThresholdSet(intIfNum, ifTtl);
}

/***************************************************************
*
* @purpose  Get the Ttl threshold of the specified interface
*
* @param    UnitIndex       @b{(input)}    unit
* @param    intIfNum        @b{(input)}    Internal Interface Number
* @param    ifTtl           @b{(output)}   ttl value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
****************************************************************/
L7_RC_t usmDbMcastIpMRouteInterfaceTtlGet(L7_uint32 UnitIndex,
                                          L7_uint32 intIfNum,
                                          L7_uint32 *ifTtl)
{
    return mcastMapIntfTtlThresholdGet(intIfNum, ifTtl);
}

/***************************************************************
*
* @purpose  Gets the multicast protocol running on the interface
*
* @param    UnitIndex      @b{(input)}    unit
* @param    intfNum        @b{(input)}    Interface
* @param    protocol       @b{(output)}   Protocol Running On the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  protocol
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteInterfaceProtocolGet(L7_uint32 UnitIndex,
                                               L7_uint32 intIfNum,
                                               L7_uint32 *protocol)
{
    return mcastMapIpMRouteInterfaceProtocolGet(intIfNum, protocol);
}

/***************************************************************
*
* @purpose  Checks the entry corresponding to intfNum is in
*           route table or not.
*
* @param    UnitIndex      @b{(input)}    unit
* @param    intfNum        @b{(input)}    Interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteInterfaceEntryGet(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum)
{
    return mcastMapIpMRouteIntfEntryGet(intIfNum);
}

/***************************************************************
*
* @purpose  Gets the next interface entry in the routing table.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    intfNum         @b{(inout)}    Next Interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
*****************************************************************/
L7_RC_t usmDbMcastIpMRouteInterfaceEntryNextGet(L7_uint32 UnitIndex,
                                                L7_uint32 *intIfNum)
{
    return mcastMapIpMRouteIntfEntryNextGet(intIfNum);
}

/***************************************************************
*
* @purpose  Deletes given entry from multicast routing table
*
* @param    UnitIndex       @b{(input)}    unit
* @param    ipMRtGrp        @b{(input)}    Multicast Group Address
* @param    ipMRtSrc        @b{(input)}    Multicast Source Address
* @param    ipMRtSrcMask    @b{(input)}    Multicast Source Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastIpMRouteEntryDelete(L7_uint32 UnitIndex,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask)
{
    return L7_NOT_SUPPORTED;
}

/***************************************************************
*
* @purpose  Flush the Multicast Mroute Table
*
* @param    UnitIndex       @b{(input)}    unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMRouteTableFlush(L7_uint32 UnitIndex)
{
    return L7_NOT_SUPPORTED;
}

/***************************************************************
*
* @purpose  Adds an entry to the static route table, if does
*           not exists already in the static route table.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    addrFamily      @b{(input)}    Address Family Identifier
* @param    ipSrc           @b{(input)}    Source IP Address
* @param    ipMask          @b{(input)}    Source IP Address  Mask
* @param    rpfAddr         @b{(input)}    Next Hop  IP Address
* @param    intIfNum        @b{(input)}    Internal Interface Num
* @param    preference      @b{(input)}    Preference for this route
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
**************************************************************/
L7_RC_t usmDbMcastStaticMRouteAdd(L7_uint32 UnitIndex,
                                  L7_uchar8 addrFamily,
                                  L7_inet_addr_t *ipSrc,
                                  L7_inet_addr_t *ipMask,
                                  L7_inet_addr_t *rpfAddr,
                                  L7_uint32 intIfNum,
                                  L7_uint32 preference)
{
  return mcastMapStaticMRouteSet (addrFamily, ipSrc, ipMask, rpfAddr, intIfNum,
                                  preference);
}

/***************************************************************
*
* @purpose  Deletes the entry corresponding to given source
*           address from static route table
*
* @param    UnitIndex       @b{(input)}    unit
* @param    addrFamily      @b{(input)}    Address Family Identifier
* @param    ipSrc           @b{(input)}    Source IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastStaticMRouteDelete(L7_uint32 UnitIndex,
                                     L7_uchar8 addrFamily,
                                     L7_inet_addr_t *ipSrc,
                                     L7_inet_addr_t *ipMask)
{
  return mcastMapStaticMRouteReset (addrFamily, ipSrc, ipMask);
}

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
usmDbMcastMapStaticMRouteMatchingEntryGet (L7_uint32 UnitIndex, 
                                           L7_uchar8 addrFamily,
                                      L7_inet_addr_t *srcAddr,
                                      L7_inet_addr_t *srcMask)
{
  return mcastMapStaticMRouteMatchingEntryGet(addrFamily, srcAddr, srcMask);
}
/***************************************************************
*
* @purpose  Verifies the entry corresponding to given source
*           address in the static route table.
*
* @param    UnitIndex       @b{(input)}    unit
* @param    addrFamily      @b{(input)}    Address Family Identifier
* @param    ipSrc           @b{(input)}    Source IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastStaticMRouteEntryGet(L7_uint32 UnitIndex, 
                                       L7_uchar8 addrFamily,
                                       L7_inet_addr_t *ipSrc,
                                       L7_inet_addr_t *ipMask)
{
  return mcastMapStaticMRouteEntryGet (addrFamily, ipSrc, ipMask);
}

/***************************************************************
*
* @purpose  Returns next entry's source address in the static
*           route table for the given source address.
*
* @param    UnitIndex       @b{(input)}     unit
* @param    addrFamily      @b{(input)}     Address Family Identifier
* @param    ipSrc           @b{(inout)}     Source IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
****************************************************************/
L7_RC_t usmDbMcastStaticMRouteEntryNextGet(L7_uint32 UnitIndex,
                                           L7_uchar8 addrFamily,
                                           L7_inet_addr_t *ipSrc,
                                           L7_inet_addr_t *ipMask)
{
  return mcastMapStaticMRouteEntryNextGet (addrFamily, ipSrc, ipMask);
}

/***************************************************************
*
* @purpose  Gets the rpf address corresponding to given source
*           address in the static route table
*
* @param    UnitIndex       @b{(input)}     unit
* @param    addrFamily      @b{(input)}     Address Family Identifier
* @param    ipSrc           @b{(input)}     Source IP Address
* @param    rpfAddr         @b{(output)}    Next Hop Router Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastStaticMRouteRpfAddressGet(L7_uint32 UnitIndex,
                                            L7_uchar8 addrFamily,
                                            L7_inet_addr_t *ipSrc,
                                            L7_inet_addr_t *ipMask,
                                            L7_inet_addr_t *rpfIpAddr)
{
  return mcastMapStaticMRouteRpfAddrGet (addrFamily, ipSrc, ipMask, rpfIpAddr);
}

/***************************************************************
*
* @purpose  Gets the internal interface number corresponding to
*           given source address in the static route table
*
* @param    UnitIndex       @b{(input)}     unit
* @param    addrFamily      @b{(input)}     Address Family Identifier
* @param    ipSrc           @b{(input)}     Source IP Address
* @param    intIfNum        @b{(output)}    Internal Interface Num
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastStaticMRouteInterfaceGet(L7_uint32 UnitIndex,
                                           L7_uchar8 addrFamily,
                                           L7_inet_addr_t *ipSrc,
                                           L7_inet_addr_t *ipMask,
                                           L7_uint32 *intIfNum)
{
  return mcastMapStaticMRouteRpfInterfaceGet (addrFamily, ipSrc, ipMask, intIfNum);
}

/***************************************************************
*
* @purpose  Gets the metric value corresponding to given source
*           address in the static route table
*
* @param    UnitIndex       @b{(input)}     unit
* @param    addrFamily      @b{(input)}     Address Family Identifier
* @param    ipSrc           @b{(input)}     Source IP Address
* @param    preference      @b{(output)}    Preference for this route
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_RC_t usmDbMcastStaticMRoutePreferenceGet(L7_uint32 UnitIndex,
                                            L7_uchar8 addrFamily,
                                            L7_inet_addr_t *ipSrc,
                                            L7_inet_addr_t *ipMask,
                                            L7_uint32 *preference)
{
  return mcastMapStaticMRoutePreferenceGet (addrFamily, ipSrc, ipMask, preference);
}


/***************************************************************
*
* @purpose  Gets whether MCAST component is operational
*
* @param    UnitIndex       @b{(input)}     unit
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
*
***************************************************************/
L7_BOOL usmDbMcastOperationalStateGet(L7_uint32 UnitIndex)
{
    /* The family parameter has to be passed from CLI/Web interface
       once the command for IPv6 is ready  */
    return mcastMapMcastIsOperational(L7_AF_INET);
}

/**********************************************************************
* @purpose  Adds an entry to the Admin Scope Boundary List for the
*           given index
*
* @param    UnitIndex       @b{(input)}     unit
* @param    intIfNum        @b{(input)}     internal interface number
* @param    ipGrp           @b{(input)}     Group IP Address
* @param    ipMask          @b{(input)}     Group IP Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
**********************************************************************/
L7_RC_t usmDbMcastMrouteBoundaryAdd(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                    L7_inet_addr_t *ipGrp,
                                    L7_inet_addr_t *ipMask)
{
    return mcastMapAdminScopeBoundarySet(intIfNum, ipGrp, ipMask);
}

/**********************************************************************
* @purpose  Deletes an entry from the Admin Scope Boundary List for
*           the given index
*
* @param    UnitIndex       @b{(input)}     unit
* @param    intIfNum        @b{(input)}     internal interface Number
* @param    ipGrp           @b{(input)}     Group IP Address
* @param    ipMask          @b{(input)}     Group IP Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
**********************************************************************/
L7_RC_t usmDbMcastMrouteBoundaryDelete(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                 L7_inet_addr_t *ipGrp, L7_inet_addr_t *ipMask)
{
    return mcastMapAdminScopeBoundaryReset(intIfNum, ipGrp, ipMask);
}

/*********************************************************************
* @purpose  Checks whether the entry corresponding to the given index
*           exists
*
* @param    UnitIndex       @b{(input)}     unit
* @param    intIfNum        @b{(input)}     Internal interface Number
* @param    ipGrp           @b{(input)}     Group IP Address
* @param    ipMask          @b{(input)}     Group IP Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
**********************************************************************/
L7_RC_t usmDbMcastMrouteBoundaryEntryGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfNum,
                                         L7_inet_addr_t *ipGrp,
                                         L7_inet_addr_t *ipMask)
{ 
    return mcastMapAdminScopeBoundaryEntryGet(intIfNum, ipGrp, ipMask);
}

/*********************************************************************
* @purpose  Gets the next entry in the Admin Scope Boundary table
*           for the given index
*
* @param    UnitIndex       @b{(input)}     unit
* @param    intIfNum        @b{(inout)}     interface Number
* @param    ipGrp           @b{(inout)}     Group IP Address
* @param    ipMask          @b{(inout)}     Group IP Address Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
**********************************************************************/
L7_RC_t usmDbMcastMrouteBoundaryEntryNextGet(L7_uint32 UnitIndex,
                                             L7_uint32 *intIfNum,
                                             L7_inet_addr_t *ipGrp,
                                             L7_inet_addr_t *ipMask)
{
    return mcastMapAdminScopeBoundaryEntryNextGet(intIfNum, ipGrp, ipMask);
}

/*********************************************************************
* @purpose  Check if an interface is valid for mcast
*
* @param    UnitIndex       @b{(input)}     unit
* @param    intIfNum        @b{(input)}     Internal interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments    none
*
* @end
**********************************************************************/
L7_BOOL usmDbMcastIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
    return mcastIntfIsValid(intIfNum);
}
/*********************************************************************
* @purpose  Turns on/off the displaying of mcache(data) packet debug info
*            
* @param    family     @b{(input)} Address Family type
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
L7_RC_t usmDbMcachePacketDebugTraceFlagSet(L7_uchar8 family, L7_BOOL rxFlag,
                                          L7_BOOL txFlag)
{
  if (mcastDebugTraceFlagSet(family, MCAST_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (mcastDebugTraceFlagSet(family, MCAST_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the mcache(data) packet debug info
*            
* @param    family     @b{(input)} Address Family type
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
L7_RC_t usmDbMcachePacketDebugTraceFlagGet(L7_uchar8 family, L7_BOOL *rxFlag,
                                          L7_BOOL *txFlag)
{
  if (mcastDebugTraceFlagGet(family, MCAST_DEBUG_PACKET_RX_TRACE, rxFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  if (mcastDebugTraceFlagGet(family, MCAST_DEBUG_PACKET_TX_TRACE, txFlag) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
