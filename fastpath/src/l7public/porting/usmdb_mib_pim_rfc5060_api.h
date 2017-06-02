/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   usmdb_mib_pim_rfc5060.c
*
* @purpose    Api functions for the Protocol Independent Multicast MIB, as
*             specified in RFC 5060
*
* @component  PIM usmdb Layer
*
* @comments   none
*
* @create     05/19/2008
*
* @author     Kamlesh Agrawal
* @end
*
**********************************************************************/
#ifndef USMDB_MIB_PIM_RFC5060_H
#define USMDB_MIB_PIM_RFC5060_H

#include "l7_common.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#endif

#include "osapi.h"
#include "nimapi.h"

#include "trap_layer3_api.h"
#include "l7_pimdm_api.h"

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
L7_RC_t usmDbPimSGIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg);

/*********************************************************************
* @purpose  Get the next SG entry.
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
L7_RC_t usmDbPimSGIEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 *ifindexg);

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
L7_RC_t usmDbPimStarGIEntryNextGet(L7_uchar8 family, 
                                     L7_inet_addr_t *pGrpAddr, 
                                     L7_uint32 *pIntIfNum);

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
L7_RC_t usmDbPimSGIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *sgUpTime);

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
L7_RC_t usmDbPimSGILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                             L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *LocalMembership);

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
L7_RC_t usmDbPimSGIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                 L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_uint32 *JoinPruneState);

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
L7_RC_t usmDbPimSGIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinTimer);

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
L7_RC_t usmDbPimSGIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinExpiry);

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
L7_RC_t usmDbPimSGIAssertStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                               L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertState);

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
L7_RC_t usmDbPimSGIAssertTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                        L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertTimer);

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
L7_RC_t usmDbPimSGIAssertWinnerAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_inet_addr_t *AssertWinner);

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
L7_RC_t usmDbPimSGIRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetricPref);

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
L7_RC_t usmDbPimSGIAssertWinnerMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                         L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetric);

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
L7_RC_t usmDbPimSGEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr);

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
L7_RC_t usmDbPimSGEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr);

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
L7_RC_t usmDbPimSGUpTimeGet(L7_uchar8 family, 
                              L7_inet_addr_t *pSGGrpAddr, 
                              L7_inet_addr_t *pSGSrcAddr,
                              L7_uint32 *pUpTime);

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
L7_RC_t usmDbPimSGPimModeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
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
L7_RC_t usmDbPimSGUpstreamJoinStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgstate);

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
L7_RC_t usmDbPimSGUpstreamJoinTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
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
L7_RC_t usmDbPimSGUpstreamNeighborGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
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
L7_RC_t usmDbPimSGRPFIfIndexGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *rpfIfIndex);

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
L7_RC_t usmDbPimSGRPFNextHopGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfNextHop);

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
L7_RC_t usmDbPimSGRPFRouteProtocolGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
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
L7_RC_t usmDbPimSGRPFRouteAddressGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
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
L7_RC_t usmDbPimSGRPFRoutePrefixLengthGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
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
L7_RC_t usmDbPimSGRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtMetricPref);


/*********************************************************************
* @purpose  To get the SG pimSGSPTBit.
*
* @param    addrType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit SG pimSGSPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGSPTBitGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgSPTBit);


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
L7_RC_t usmDbPimSGRPFRouteMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtMetric);

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
L7_RC_t usmDbPimSGRPRegisterPMBRAddressGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfNextHop);

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
L7_RC_t usmDbPimNeighborEntryGet(L7_uint32 UnitIndex, L7_uchar8 family,
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
L7_RC_t usmDbPimNeighborEntryNextGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                              L7_uint32 *intIfNum, L7_inet_addr_t *ipAddress);

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
L7_RC_t usmDbPimNeighborUpTimeGet(L7_uint32 UnitIndex,
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
L7_RC_t usmDbPimNeighborExpiryTimeGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family,
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t *nbrIpAddr, 
                                        L7_uint32 *nbrExpiryTime);

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
L7_RC_t usmDbPimInterfaceEntryGet(L7_uint32 UnitIndex, L7_uchar8 family, 
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
L7_RC_t usmDbPimInterfaceEntryNextGet(L7_uint32 UnitIndex, 
                                        L7_uchar8 family,
                                        L7_uint32 *intIfNum);

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
L7_RC_t usmDbPimInterfaceIPAddressGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 intIfNum, L7_inet_addr_t *ipAddr);

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
L7_RC_t usmDbPimInterfaceJoinPruneIntervalGet(L7_uint32 UnitIndex, 
             L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 *joinpruneIntvl);

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
L7_RC_t usmDbPimInterfaceGenerationIDValueGet(L7_uint32 intIfNum,
                                       L7_uint32 inetIPVer, L7_uint32 *genIDValue);

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
L7_RC_t usmDbPimInterfaceUseDRPriorityGet(L7_uint32 intIfNum,
                                                L7_uint32 inetIPVer,
                                                L7_int32 *useDRPriority);

/*********************************************************************
* @purpose  Gets the PIM Interface Mode
*
* @param    UnitIndex   @b{(input)}  Unit
* @param    family      @b{(input)}  family Type.
* @param    ifIndex     @b{(input)}  PIM interface index
* @param    mode        @b{(output)} PIM Interface Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceModeGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 ifIndex, L7_uint32* mode);

/*********************************************************************
* @purpose  Sets the PIM Interface Mode
*
* @param    UnitIndex   @b{(input)}  Unit
* @param    family      @b{(input)}  family Type.
* @param    ifIndex     @b{(input)}  PIM interface index
* @param    mode        @b{(input)} PIM Interface Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceModeSet(L7_uint32 UnitIndex, L7_uchar8 family,
                                 L7_uint32 ifIndex, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the Interface Operational State of PIMSM in the router.
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} internal Interface number
*
* @returns  L7_TRUE   if success
* @returns  L7_FALSE  if failure
*
* @comments    none
*
* @end
*********************************************************************/
L7_BOOL usmDbPimInterfaceOperationalStateGet(L7_uint32 UnitIndex, 
                                  L7_uchar8 family, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the PIM Interface Hello Interval
*
* @param    UnitIndex    @b{(input)}  Unit Index.
* @param    family       @b{(input)}  family Type. 
* @param    ifIndex      @b{(input)}  PIM interface index
* @param    helloIntvl   @b{(output)} PIM Interface Hello Interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIntfHelloIntervalGet(L7_uint32 UnitIndex, 
               L7_uchar8 family, L7_uint32 intIfNum,L7_uint32* helloIntvl);


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
L7_RC_t usmDbPimIntfHelloIntervalSet(L7_uint32 UnitIndex,
                 L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 helloIntvl);


/*********************************************************************
* @purpose  Get the Neighbor count for specified interface
*
* @param    UnitIndex       @b{(input)} unit Index.
* @param    family          @b{(input)} Address Family type
* @param    intIfNum        @b{(input)} Internal Interface number
* @param    nbrCount        @b{(output)} Neighbor Count
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborCountGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                                       L7_uint32 intIfNum, L7_uint32 *nbrCount);

#endif
