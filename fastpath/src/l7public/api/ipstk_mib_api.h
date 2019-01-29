/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   ipstk_mib_api.h
*
* @purpose    ipstack Mapping layer external function prototypes
*
* @component  ipstack Mapping Layer
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/




#ifndef INCLUDE_IPSTK_MIB_API
#define INCLUDE_IPSTK_MIB_API


#include "l7_common.h"
#include "l3_addrdefs.h"


/*************************************************************
*
*    SNMP MIB access
*
**************************************************************/


typedef struct ipstkTcpInfo_s
{
    L7_uint32 tcpRtoAlgorithm;
    L7_uint32 tcpRtoMin;
    L7_uint32 tcpRtoMax;
    L7_uint32 tcpMaxConn;
    L7_uint32 tcpActiveOpens;
    L7_uint32 tcpPassiveOpens;
    L7_uint32 tcpAttemptFails;
    L7_uint32 tcpEstabResets;
    L7_uint32 tcpCurrEstab;
    L7_uint32 tcpInSegs;
    L7_uint32 tcpOutSegs;
    L7_uint32 tcpRetransSegs;
    L7_uint32 tcpOutRsts;
    L7_uint32 tcpInErrs;

}ipstkTcpInfo_t;


typedef struct ipstkTcpConnInfo_s
{
    L7_uint32     tcpConnState;
    L7_uint32     tcpConnLocalAddress;
    L7_ushort16   tcpConnLocalPort;
    L7_uint32     tcpConnRemAddress;
    L7_ushort16   tcpConnRemPort;

}ipstkTcpConnInfo_t;

#define IPSTK_SNMP_TCPCONN_DELETE_TCB 12

typedef struct ipstkUdpInfo_s{
    L7_uint32 udpInDatagrams;
    L7_uint32 udpNoPorts;
    L7_uint32 udpInErrors;
    L7_uint32 udpOutDatagrams;

}ipstkUdpInfo_t;

typedef struct ipstkUdpListInfo_s
{
    L7_uint32     udpLocalAddress;
    L7_ushort16   udpLocalPort;

}ipstkUdpListInfo_t;


#define IPSTK_SNMP_MAX_PHYADDR 6
typedef struct ipstkArpEntry_s
{
    L7_uint32     ipNetToMediaIfIndex;             /* stack's ifindex */
    L7_uint32     ipNetToMediaNetAddress;
    L7_uint32     ipNetToMediaPhyAddressLen;
    L7_uchar8     ipNetToMediaPhyAddress[IPSTK_SNMP_MAX_PHYADDR];
    L7_uint32     ipNetToMediaType;

}ipstkArpEntry_t;



typedef struct ipstkIpGroupInfo_s
{
    L7_uint32 ipForwarding;
    L7_uint32 ipDefaultTTL;
    L7_uint32 ipInReceives;
    L7_uint32 ipInHdrErrors;
    L7_uint32 ipInAddrErrors;
    L7_uint32 ipForwDatagrams;
    L7_uint32 ipInUnknownProtos;
    L7_uint32 ipInDiscards;
    L7_uint32 ipInDelivers;
    L7_uint32 ipOutRequests;
    L7_uint32 ipOutDiscards;
    L7_uint32 ipOutNoRoutes;
    L7_uint32 ipReasmTimeout;
    L7_uint32 ipReasmReqds;
    L7_uint32 ipReasmOKs;
    L7_uint32 ipReasmFails;
    L7_uint32 ipFragOKs;
    L7_uint32 ipFragFails;
    L7_uint32 ipFragCreates;
    L7_uint32 ipRoutingDiscards;

}ipstkIpGroupInfo_t;

typedef struct ipstkIpAddressInfo_s
{
   L7_uint32  ipAdEntAddr;
   L7_uint32  ipAdEntIfIndex;         /* stack's ifindex */
   L7_uint32  ipAdEntNetMask;
   L7_uint32  ipAdEntBcastAddr;
   L7_uint32  ipAdEntReasmMaxSize;

}ipstkIpAddressInfo_t;

typedef struct ipstkIcmpInfo_s
{
    L7_uint32 icmpInMsgs;
    L7_uint32 icmpInErrors;
    L7_uint32 icmpInDestUnreachs;
    L7_uint32 icmpInTimeExcds;
    L7_uint32 icmpInParmProbs;
    L7_uint32 icmpInSrcQuenchs;
    L7_uint32 icmpInRedirects;
    L7_uint32 icmpInEchos;
    L7_uint32 icmpInEchoReps;
    L7_uint32 icmpInTimestamps;
    L7_uint32 icmpInTimestampReps;
    L7_uint32 icmpInAddrMasks;
    L7_uint32 icmpInAddrMaskReps;

    L7_uint32 icmpOutMsgs;
    L7_uint32 icmpOutErrors;
    L7_uint32 icmpOutDestUnreachs;
    L7_uint32 icmpOutTimeExcds;
    L7_uint32 icmpOutParmProbs;
    L7_uint32 icmpOutSrcQuenchs;
    L7_uint32 icmpOutRedirects;
    L7_uint32 icmpOutEchos;
    L7_uint32 icmpOutEchoReps;
    L7_uint32 icmpOutTimestamps;
    L7_uint32 icmpOutTimestampReps;
    L7_uint32 icmpOutAddrMasks;
    L7_uint32 icmpOutAddrMaskReps;

}ipstkIcmpInfo_t;

typedef struct ipstkIpRouteEntryInfo_s
{
    L7_uint32 ipRouteDest;
    L7_uint32 ipRouteIfIndex;       /* stack's ifindex */
    L7_uint32 ipRouteMetric1;
    L7_uint32 ipRouteMetric2;
    L7_uint32 ipRouteMetric3;
    L7_uint32 ipRouteMetric4;
    L7_uint32 ipRouteNextHop;
    L7_uint32 ipRouteType;
    L7_uint32 ipRouteProto;
    L7_uint32 ipRouteAge;
    L7_uint32 ipRouteMask;
    L7_uint32 ipRouteMetric5;
    L7_uint32 ipRouteInfoLength;
    L7_uint32 ipRouteInfoArray[2];

}ipstkIpRouteEntryInfo_t;



typedef struct ipstkPhysAddr_s
{
    L7_uint32   len;
    L7_uchar8   addr[IPSTK_SNMP_MAX_PHYADDR];
}ipstkPhysAddr_t;

typedef struct ipstkNullOid_s
{
    L7_uint32   len;
    L7_uchar8   oid[2];
}ipstkNullOid_t;

typedef struct ipstkIpv6GeneralGroup_s
{
    L7_uint32  ipv6Forwarding;
    L7_uint32  ipv6DefaultHopLimit;
    L7_uint32  ipv6Interfaces;
    L7_uint32  ipv6IfTableLastChange;
    L7_uint32  ipv6RouteNumber;
    L7_uint32  ipv6DiscardedRoutes;

}ipstkIpv6GeneralGroup_t;

typedef struct ipstkIpv6IfEntry_s
{
    L7_uint32       ipv6IfIndex;              /* intIfNum */
    L7_uchar8       ipv6IfDescr[20];
    ipstkNullOid_t  ipv6IfLowerLayer;
    L7_uint32       ipv6IfEffectiveMtu;
    L7_uint32       ipv6ReasmMaxSize; 
    L7_uchar8       ipv6IfIdentifier[8];
    L7_uint32       ipv6IfIdentifierLength;
    ipstkPhysAddr_t ipv6IfPhysicalAddress;
    L7_uint32       ipv6IfAdminStatus;
    L7_uint32       ipv6IfOperStatus;
    L7_uint32       ipv6IfLastChange;

}ipstkIpv6IfEntry_t;

/* interface mib private extensions */
typedef struct ipstkIpv6IfEntryExt_s{
    L7_uint32       ipv6IfIndex;              /* intIfNum */
    L7_uint32       ipv6IfMtuValue;
    L7_uint32       ipv6IfDadTransmits;
    L7_uint32       ipv6IfRadvSuppressMode;
    L7_uint32       ipv6IfRadvMaxAdvertisementInterval;
    L7_uint32       ipv6IfRadvMinAdvertisementInterval;
    L7_uint32       ipv6IfRadvAdvertisementLifetime;
    L7_uint32       ipv6IfRadvNbrSolicitInterval;
    L7_uint32       ipv6IfRadvReachableTime;
    L7_uint32       ipv6IfRadvManagedFlag;
    L7_uint32       ipv6IfRadvOtherFlag;
}ipstkIpv6IfEntryExt_t;


typedef struct ipstkIpv6AddrPrefixEntry_s
{
    L7_uint32       ipv6IfIndex;             /* intIfNum */
    L7_in6_addr_t   ipv6AddrPrefix;
    L7_uint32       ipv6AddrPrefixLength;
    L7_uint32       ipv6AddrPrefixOnLinkFlag;
    L7_uint32       ipv6AddrPrefixAutonomousFlag;
    L7_uint32       ipv6AddrPrefixAdvPreferredLifetime;
    L7_uint32       ipv6AddrPrefixAdvValidLifetime;

}ipstkIpv6AddrPrefixEntry_t;


typedef struct ipstkIpv6AddrEntry_s
{
    L7_uint32        ipv6IfIndex;           /* intIfNum */
    L7_in6_addr_t    ipv6AddrAddress;
    L7_uint32        ipv6AddrPfxLength;
    L7_uint32        ipv6AddrType;
    L7_uint32        ipv6AddrAnycastFlag;
    L7_uint32        ipv6AddrEui64Flag;
    L7_uint32        ipv6AddrStatus;
}ipstkIpv6AddrEntry_t;


typedef struct ipstkIpv6RouteEntry_s
{
    L7_in6_addr_t    ipv6RouteDest;
    L7_uint32        ipv6RoutePfxLength;
    L7_uint32        ipv6RouteIndex;
    L7_uint32        ipv6RouteIfIndex;      /* intIfNum */
    L7_in6_addr_t    ipv6RouteNextHop;
    L7_uint32        ipv6RouteType;
    L7_uint32        ipv6RouteProtocol;
    L7_uint32        ipv6RoutePolicy;
    L7_uint32        ipv6RouteAge;
    L7_uint32        ipv6RouteNextHopRDI;
    L7_uint32        ipv6Metric;
    L7_uint32        ipv6RouteWeight;
    ipstkNullOid_t   ipv6RouteInfo;   /* tbd: protocol oid */
    L7_uint32        ipv6RouteValid;
    L7_uint32        ipv6RouteProtocolType; /* Sub-type field for more general type ipv6RouteProtocol */
}ipstkIpv6RouteEntry_t;


typedef struct ipstkIpv6NetToMediaEntry_s
{
    L7_uint32        ipv6IfIndex;  /* intIfNum: index not part of table */
    L7_in6_addr_t    ipv6NetToMediaNetAddress;
    ipstkPhysAddr_t  ipv6NetToMediaPhysAddress;
    L7_uint32        ipv6IfNetToMediaType;
    L7_uint32        ipv6IfNetToMediaState;
    L7_uint32        ipv6IfNetToMediaLastUpdated;
    L7_uint32        ipv6IfNetToMediaValid;
    L7_BOOL          ipv6IfNetToMediaIsRouter;     /* not in mib */

}ipstkIpv6NetToMediaEntry_t;

/*
traps
ipv6IfStateChange
*/



typedef struct ipstkIpv6IfIcmpStats_s
{
    L7_uint32  ipv6IfIndex;       /* intIfNum */
    L7_uint32  ipv6IfIcmpInMsgs;
    L7_uint32  ipv6IfIcmpInErrors;
    L7_uint32  ipv6IfIcmpInDestUnreachs;
    L7_uint32  ipv6IfIcmpInAdminProhibs;
    L7_uint32  ipv6IfIcmpInTimeExcds;
    L7_uint32  ipv6IfIcmpInParmProblems;
    L7_uint32  ipv6IfIcmpInPktTooBigs;
    L7_uint32  ipv6IfIcmpInEchos;
    L7_uint32  ipv6IfIcmpInEchoReplies;
    L7_uint32  ipv6IfIcmpInRouterSolicits;
    L7_uint32  ipv6IfIcmpInRouterAdvertisements;
    L7_uint32  ipv6IfIcmpInNeighborSolicits;
    L7_uint32  ipv6IfIcmpInNeighborAdvertisements;
    L7_uint32  ipv6IfIcmpInRedirects;
    L7_uint32  ipv6IfIcmpInGroupMembQueries;
    L7_uint32  ipv6IfIcmpInGroupMembResponses;
    L7_uint32  ipv6IfIcmpInGroupMembReductions;

    L7_uint32  ipv6IfIcmpOutMsgs;
    L7_uint32  ipv6IfIcmpOutErrors;
    L7_uint32  ipv6IfIcmpOutDestUnreachs;
    L7_uint32  ipv6IfIcmpOutAdminProhibs;
    L7_uint32  ipv6IfIcmpOutTimeExcds;
    L7_uint32  ipv6IfIcmpOutParmProblems;
    L7_uint32  ipv6IfIcmpOutPktTooBigs;
    L7_uint32  ipv6IfIcmpOutEchos;
    L7_uint32  ipv6IfIcmpOutEchoReplies;
    L7_uint32  ipv6IfIcmpOutRouterSolicits;
    L7_uint32  ipv6IfIcmpOutRouterAdvertisements;
    L7_uint32  ipv6IfIcmpOutNeighborSolicits;
    L7_uint32  ipv6IfIcmpOutNeighborAdvertisements;
    L7_uint32  ipv6IfIcmpOutRedirects;
    L7_uint32  ipv6IfIcmpOutGroupMembQueries;
    L7_uint32  ipv6IfIcmpOutGroupMembResponses;
    L7_uint32  ipv6IfIcmpOutGroupMembReductions;

    L7_uint32  ipv6IfIcmpDupAddrDetects;

}ipstkIpv6IfIcmpStats_t;

typedef struct ipstkIpv6IfStats_s
{
    L7_uint32  ipv6IfIndex;             /* intIfNum */
    L7_uint32  ipv6IfStatsInReceives;
    L7_uint32  ipv6IfStatsInMcastPkts;
    L7_uint32  ipv6IfStatsInDelivers;
    L7_uint32  ipv6IfStatsInUnknownProtos;
    L7_uint32  ipv6IfStatsInAddrErrors;
    L7_uint32  ipv6IfStatsInHdrErrors;
    L7_uint32  ipv6IfStatsInNoRoutes;
    L7_uint32  ipv6IfStatsInTooBigErrors;
    L7_uint32  ipv6IfStatsInTruncatedPkts;
    L7_uint32  ipv6IfStatsReasmReqds;
    L7_uint32  ipv6IfStatsReasmOKs;
    L7_uint32  ipv6IfStatsReasmFails;
    L7_uint32  ipv6IfStatsInDiscards;

    L7_uint32  ipv6IfStatsOutRequests;
    L7_uint32  ipv6IfStatsOutMcastPkts;
    L7_uint32  ipv6IfStatsOutForwDatagrams;
    L7_uint32  ipv6IfStatsOutFragCreates;
    L7_uint32  ipv6IfStatsOutFragOKs;
    L7_uint32  ipv6IfStatsOutFragFails;
    L7_uint32  ipv6IfStatsOutDiscards;
}ipstkIpv6IfStats_t;



/*********************************************************************
* @purpose  get icmp group info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    called directly by switching build
*
* @end
*********************************************************************/
L7_RC_t osapiIcmpGroupInfoGet(ipstkIcmpInfo_t *vars);

/*********************************************************************
* @purpose  get tcp group info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiTcpGroupInfoGet(ipstkTcpInfo_t *vars);

/*********************************************************************
* @purpose  get tcp connection info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiTcpConnInfoGet( L7_uint32 searchType, ipstkTcpConnInfo_t *vars);


/*********************************************************************
* @purpose  Set tcp connection info in stack
*
* @param    vars        pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    allows only deleteTCB
*
* @end
*********************************************************************/
L7_RC_t osapiTcpConnInfoSet( ipstkTcpConnInfo_t *vars);

/*********************************************************************
* @purpose  get udp group info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiUdpGroupInfoGet(ipstkUdpInfo_t *vars);

/*********************************************************************
* @purpose  get udp listener info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiUdpListInfoGet( L7_uint32 searchType, ipstkUdpListInfo_t *vars);



/*********************************************************************
* @purpose  get NDisc info from stack
*
* @param    get_all     L7_TRUE to include all ports, L7_FALSE for only router interfaces
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6NetToMediaEntryGet( L7_uint32 searchType, L7_BOOL get_all, ipstkIpv6NetToMediaEntry_t *vars);

/*********************************************************************
* @purpose  get interface stats from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6IfStatsGet( L7_uint32 searchType, ipstkIpv6IfStats_t *vars);


/*********************************************************************
* @purpose  get interface icmp stats from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6IfIcmpStatsGet( L7_uint32 searchType, ipstkIpv6IfIcmpStats_t *vars);



/*********************************************************************
* @purpose  get v6 general group info from stack
*
* @param    vars        return pointer for data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6GeneralGroupGet( ipstkIpv6GeneralGroup_t *vars);


#endif /*INCLUDE_IPSTK_MIB_API*/

