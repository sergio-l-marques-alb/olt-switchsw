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

#include "l7_common.h"
#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"
#endif

#include "osapi.h"
#include "l3_mcast_commdefs.h"

#include "l7_pimsm_api.h"
#include "l7_mcast_api.h"

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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIEntryGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg);        
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
}

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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 *ifindexg)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIEntryNextGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg);        
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimStarGIEntryNextGet(L7_uchar8 family, 
                                     L7_inet_addr_t *pGrpAddr, 
                                     L7_uint32 *pIntIfNum)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapStarGIEntryNextGet(family, pGrpAddr, pIntIfNum);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIUpTimeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *sgUpTime)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIUpTimeGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,sgUpTime);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGILocalMembershipGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                             L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *LocalMembership)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGILocalMembershipGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,LocalMembership);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIJoinPruneStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                 L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_uint32 *JoinPruneState)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIJoinPruneStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,ifindexg,JoinPruneState);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIPrunePendingTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinTimer)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIPrunePendingTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,JoinTimer);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIJoinExpiryTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinExpiry)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return  pimsmMapSGIJoinExpiryTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,JoinExpiry);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIAssertStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                               L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertState)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIAssertStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertState);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIAssertTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                        L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertTimer)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIAssertTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertTimer);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIAssertWinnerAddrGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_inet_addr_t *AssertWinner)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIAssertWinnerAddrGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertWinner);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetricPref)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIRPFRouteMetricPrefGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertWinnerMetricPref);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGIAssertWinnerMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr,
                         L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetric)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGIAssertWinnerMetricGet(addrType,pimSGGrpAddr,pimSGSrcAddr,IfIndex,AssertWinnerMetric);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
}


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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGEntryGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGEntryGet(addrType,pimSGGrpAddr,pimSGSrcAddr);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGEntryNextGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return  pimsmMapSGEntryNextGet(addrType,pimSGGrpAddr,pimSGSrcAddr);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGUpTimeGet(L7_uchar8 family, 
                              L7_inet_addr_t *pSGGrpAddr, 
                              L7_inet_addr_t *pSGSrcAddr,
                              L7_uint32 *pUpTime)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGUpTimeGet(family, pSGGrpAddr, pSGSrcAddr, pUpTime);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
}


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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGPimModeGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_int32 *sgmode)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGUpstreamJoinStateGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgstate)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGUpstreamJoinStateGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgstate);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGUpstreamJoinTimerGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgjoinTimer)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGUpstreamJoinTimerGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgjoinTimer);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGUpstreamNeighborGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *upstreamNbr)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGUpstreamNeighborGet(addrType,pimSGGrpAddr,pimSGSrcAddr,upstreamNbr);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPFIfIndexGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *rpfIfIndex)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGRPFIfIndexGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfIfIndex);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPFNextHopGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfNextHop)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return  pimsmMapSGRPFNextHopGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfNextHop);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPFRouteProtocolGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *rpfprotocol)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGRPFRouteProtocolGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfprotocol);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPFRouteAddressGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfRtAddr)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGRPFRouteAddressGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfRtAddr);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPFRoutePrefixLengthGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtLen)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGRPFRoutePrefixLengthGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgrtLen);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPFRouteMetricPrefGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtMetricPref)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGRPFRouteMetricPrefGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgrtMetricPref);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
}

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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGSPTBitGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgSPTBit)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGSPTBitGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgSPTBit);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPFRouteMetricGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgrtMetric)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGRPFRouteMetricGet(addrType,pimSGGrpAddr,pimSGSrcAddr,sgrtMetric);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimSGRPRegisterPMBRAddressGet(L7_uchar8 addrType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_inet_addr_t *rpfNextHop)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(addrType, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapSGRPRegisterPMBRAddressGet(addrType,pimSGGrpAddr,pimSGSrcAddr,rpfNextHop);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
}


/*********************************************************************
* @purpose  Check whether neighbor entry exists for the specified IP address
*
* @param    UnitIndex    @b{(input)}  UnitIndex.
* @param    family       @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  internal Interface number
* @param    ipAddress    @b{(input)}  IP Address
*
* @returns  L7_SUCCESS    if neighbor entry exists
* @returns  L7_FAILURE    if neighbor entry does not exist
*
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborEntryGet(L7_uint32 UnitIndex, L7_uchar8 family,
                               L7_uint32 intIfNum, L7_inet_addr_t *ipAddress)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapNeighborEntryGet(family, intIfNum, ipAddress);
            break;

        case L7_MCAST_IANA_MROUTE_PIM_DM:
            return pimdmMapNbrEntryByIfIndexGet (family, intIfNum, ipAddress);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborEntryNextGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                              L7_uint32 *intIfNum, L7_inet_addr_t *ipAddress)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapNeighborEntryNextGet(family, intIfNum, ipAddress);
            break;
            
        case L7_MCAST_IANA_MROUTE_PIM_DM:
            return pimdmMapNbrEntryByIfIndexNextGet (family, intIfNum, ipAddress);
            break;
            
        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborUpTimeGet(L7_uint32 UnitIndex,
                                    L7_uchar8 family,
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *nbrIpAddr, 
                                    L7_uint32 *nbrUpTime)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapNeighborUpTimeGet(family, intIfNum, nbrIpAddr, nbrUpTime);
            break;

        case L7_MCAST_IANA_MROUTE_PIM_DM:
            return pimdmMapNbrUpTimeByIfIndexGet (family, nbrIpAddr, intIfNum, nbrUpTime);
            break;
            
        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborExpiryTimeGet(L7_uint32 UnitIndex,
                                        L7_uchar8 family,
                                        L7_uint32 intIfNum,
                                        L7_inet_addr_t *nbrIpAddr, 
                                        L7_uint32 *nbrExpiryTime)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapNeighborExpiryTimeGet(family, intIfNum, nbrIpAddr, nbrExpiryTime);
            break;

        case L7_MCAST_IANA_MROUTE_PIM_DM:
            return pimdmMapNbrExpiryTimeByIfIndexGet (family, nbrIpAddr, intIfNum, nbrExpiryTime);
            break;

        default:
            return L7_NOT_SUPPORTED;
    }
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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceEntryGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                    L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  if (pimdmMapIntfEntryGet (family, intIfNum) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }
  if (pimsmMapInterfaceEntryGet (family, intIfNum) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceEntryNextGet(L7_uint32 UnitIndex, 
                                        L7_uchar8 family,
                                        L7_uint32 *intIfNum)
{
  L7_RC_t rc = L7_FAILURE;

  if (pimdmMapIntfEntryNextGet (family, intIfNum) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }
  if (pimsmMapInterfaceEntryNextGet (family, intIfNum) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceIPAddressGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                   L7_uint32 intIfNum, L7_inet_addr_t *ipAddr)
{
  if (pimdmMapIntfIPAddrGet (family, intIfNum, ipAddr) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  if (pimsmMapInterfaceIPAddressGet (family, intIfNum, ipAddr) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceJoinPruneIntervalGet(L7_uint32 UnitIndex, 
             L7_uchar8 family,L7_uint32 intIfNum, L7_uint32 *joinpruneIntvl)
{
  return pimsmMapInterfaceJoinPruneIntervalGet(family, intIfNum, joinpruneIntvl);
}



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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceGenerationIDValueGet(L7_uint32 intIfNum,
                                       L7_uint32 inetIPVer, L7_uint32 *genIDValue)
{
  return pimsmMapInterfaceGenerationIDValueGet(intIfNum,inetIPVer,genIDValue);
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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceUseDRPriorityGet(L7_uint32 intIfNum,
                                                L7_uint32 inetIPVer,
                                                L7_int32 *useDRPriority)
{
  return L7_SUCCESS; /* PIMDM doesn't support this item.
                      * THIS OBJECT ALWAYS RETURNS TRUE for PIMSM.
                      */
}

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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceModeGet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                 L7_uint32 intIfNum, L7_uint32* mode)
{
  L7_RC_t rc = L7_FAILURE;

  if (pimdmMapIntfAdminModeGet (family, intIfNum, mode) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }
  if (pimsmMapInterfaceModeGet (family, intIfNum, mode) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

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
* @notes    This is a configuration item. Try and first set the 
*           item for PIMDM and PIMSM and if both are successful then
            return Success else Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimInterfaceModeSet(L7_uint32 UnitIndex, L7_uchar8 family, 
                                 L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  if (pimdmMapIntfAdminModeSet (family, intIfNum, mode) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  if (pimsmMapInterfaceModeSet (family, intIfNum, mode) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }

  return rc;
}

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
* @notes    This is a status/dynamic item. Check the enabled protocol
*           and get its operational status.
*
* @end
*********************************************************************/
L7_BOOL usmDbPimInterfaceOperationalStateGet(L7_uint32 UnitIndex, 
                                  L7_uchar8 family, L7_uint32 intIfNum)
{
    L7_uint32 proto;
    L7_BOOL rc = L7_FALSE;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            rc = pimsmMapIntfIsOperational (family, intIfNum);
            break;

        case L7_MCAST_IANA_MROUTE_PIM_DM:
            if (pimdmMapIntfOperationalStateGet (family, intIfNum) == L7_SUCCESS)
            {
              rc = L7_TRUE;
            }
            break;

        default:
            rc = L7_FALSE;
    }

    return rc;
}

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
* @notes    This is a configuration item. Try and first get the 
*           item from PIMDM and if fails, then try and get
*           the item from PIMSM else return Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIntfHelloIntervalGet(L7_uint32 UnitIndex, 
               L7_uchar8 family, L7_uint32 intIfNum,L7_uint32* helloIntvl)
{
  if (pimdmMapIntfHelloIntervalGet (family, intIfNum,helloIntvl) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  if (pimsmMapInterfaceHelloIntervalGet (family, intIfNum,helloIntvl) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
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
* @notes    This is a configuration item. Try and first set the 
*           item for PIMDM and PIMSM and if both are successful then
            return Success else Failure.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimIntfHelloIntervalSet(L7_uint32 UnitIndex,
                 L7_uchar8 family, L7_uint32 intIfNum, L7_uint32 helloIntvl)
{
  L7_RC_t rc = L7_SUCCESS;

  if (pimdmMapIntfHelloIntervalSet (family, intIfNum, helloIntvl) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  if (pimsmMapInterfaceHelloIntervalSet (family, intIfNum, helloIntvl) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }

  return rc;
}

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
* @notes    This is a dynamic item.  Get the current running multicast
*           protocol and return the values.
*
* @end
*********************************************************************/
L7_RC_t usmDbPimNeighborCountGet(L7_uint32 UnitIndex, L7_uchar8 family,
                                                       L7_uint32 intIfNum, L7_uint32 *nbrCount)
{
    L7_uint32 proto;

    mcastMapIpCurrentMcastProtocolGet(family, &proto);
    switch(proto)
    {
        case L7_MCAST_IANA_MROUTE_PIM_SM:
            return pimsmMapNeighborCountGet(family, intIfNum, nbrCount);
            break;

        case L7_MCAST_IANA_MROUTE_PIM_DM:
            return pimdmMapNbrCountGet (family, intIfNum, nbrCount);
            break;

        default:
            return L7_FAILURE;
    }
}

