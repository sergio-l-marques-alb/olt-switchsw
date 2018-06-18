/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l3_comm_strcusts.h
*
* @purpose The purpose of this file is to have a central location for
*          layer 3 common structures to be used by the entire system.
*
* @component sysapi
*
* @comments none
*
* @create 3/20/2000
*
* @author wjacobs
* @end
*
**********************************************************************/

/**************************@null{*************************************
                     
 *******************************}*************************************}
**********************************************************************/

#ifndef INCLUDE_L3_COMM_STRUCTS
#define INCLUDE_L3_COMM_STRUCTS

#include <commdefs.h>
#include <datatypes.h>
#include <l3_commdefs.h>
#include <l3_addrdefs.h>
#include <l7_packet.h>
#include <nimapi.h>

/*************************************************
*
*  GENERIC IP MAPPING STRUCTURES
*
**************************************************/

/* Router Interface/IP Circuit Definition */                           
typedef struct
{
  L7_ushort16         vlanID;
  L7_uint32           intIfNum;

} L7_ipCircuit_t;

/* To have correspondence between VLanId and USP */
typedef struct 
{
  L7_ushort16 vlanID;
  nimUSP_t usp;
} L7_ipUSP_t;

typedef struct L7_rtrIntfIpAddr_s
{
  L7_IP_ADDR_t ipAddr;   /* IP Address  */
  L7_IP_MASK_t ipMask;   /* Subnet Mask */

} L7_rtrIntfIpAddr_t;

/*********************************************************************
 * @purpose   This structure contains the result of a dtlIpv6IntfStatsGet() call
 *
 * @notes     The fields in this structure match MIB fields from RFC 2465
 * 
 *********************************************************************/
typedef struct L7_ipv6InterfaceStats_s
{
  L7_uint32 ipv6IfStatsInTooBigErrors;
  L7_uint32 ipv6IfStatsInNoRoutes;
  L7_uint32 ipv6IfStatsInAddrErrors;
  L7_uint32 ipv6IfStatsInUnknownProtos;
  L7_uint32 ipv6IfStatsInTruncatedPkts;
  L7_uint32 ipv6IfStatsOutRequests;
  L7_uint32 ipv6IfStatsOutFragOKs;
  L7_uint32 ipv6IfStatsOutFragFails;
  L7_uint32 ipv6IfStatsOutFragCreates;
  L7_uint32 ipv6IfStatsReasmReqds;
  L7_uint32 ipv6IfStatsReasmOKs;
  L7_uint32 ipv6IfStatsReasmFails;
  L7_uint32 ipv6IfStatsInReceives;
  L7_uint32 ipv6IfStatsInHdrErrors;
  L7_uint32 ipv6IfStatsInDiscards;
  L7_uint32 ipv6IfStatsInDelivers;
  L7_uint32 ipv6IfStatsOutForwDatagrams;
  L7_uint32 ipv6IfStatsOutDiscards;
  L7_uint32 ipv6IfStatsInMcastPkts;
  L7_uint32 ipv6IfStatsOutMcastPkts;
} L7_ipv6InterfaceStats_t;

/*********************************************************************
*
* @enums L7_RT_FRAGMENTATION_MODE_t
*
* @purpose
*
* @notes none
*
*********************************************************************/
typedef enum
{

  L7_RT_FRAGMENT_DISCARD,
  L7_RT_FRAGMENT_SEND_TO_CPU,
  L7_RT_FRAGMENT_SEND_TO_2ND_RTR

} L7_RT_FRAGMENTATION_MODE_t;

/****************************************
*
*  IP ROUTER STRUCTURES AND DEFINES
*
*****************************************/


/*----------------
   ARP STRUCTURES
 ----------------*/

/* ARP entry information */
typedef struct L7_arpEntry_s
{
  L7_IP_ADDR_t        ipAddr;     /* Ip Address of the gateway */
  L7_linkLayerAddr_t  macAddr;    /* Mac address of the gateway if possible */
  L7_ushort16         vlanId;     /* vlan id */
  L7_uint32           intIfNum;   /* outgoing internal interface number */
  L7_uint32           hits;       /*  non-zero, count if possible */
  L7_uint32           flags;      /* Local */
/* Flags for L7_arpEntry_t */
#define 	L7_ARP_LOCAL	           0x01   /* IP address is one of switch's MAC addresses */
#define		L7_ARP_GATEWAY             0x02   /* Destination is a router */
#define		L7_ARP_RESOLVED            0x04   /* Resolved ARP entry, dest MAC address known */
#define 	L7_ARP_NET_DIR_BCAST       0x08   /* Net directed broadcast addr */
#define 	L7_ARP_DISCARD_WHEN_SOURCE 0x10   /* Discard when seen as source */
#define 	L7_ARP_STATIC              0x20   /* Static ARP entry */
#define 	L7_ARP_UNNUMBERED          0x40   /* Unnumbered intf ARP entry */
  L7_uint32           ageSecs;    /* ARP entry age time, in seconds */ 
} L7_arpEntry_t;

/* ARP entry status information */
typedef struct L7_arpQuery_s
{
  L7_IP_ADDR_t        ipAddr;     /* Ip Address of the gateway */
  L7_ushort16         vlanId;     /* vlan id - ignored for now */
  L7_uint32           intIfNum;   /* outgoing internal interface number */
  L7_uint32           arpQueryFlags;
/* Flags for L7_arpQuery_t */
#define   L7_ARP_HIT_SOURCE  (1 << 4)  /* hardware indicates src hit */
#define   L7_ARP_HIT_DEST    (1 << 5)  /* hardware indicates dst hit */
#define   L7_ARP_HIT_ANY (L7_ARP_HIT_SOURCE | L7_ARP_HIT_DEST)
  L7_uint32           lastHitTime;    /* Last time the entry was hit in hardware (in secs) */
} L7_arpQuery_t;

/* various ARP cache statistics (useful for 'show' functions, etc.) */
typedef struct L7_arpCacheStats_s
{
  L7_uint32     cacheCurrent;           /* current overall count */
  L7_uint32     cachePeak;              /* peak overall count */
  L7_uint32     cacheMax;               /* maximum (configured) overall count */
  L7_uint32     staticCurrent;          /* current static entry count */
  L7_uint32     staticMax;              /* maximum allowed static entry count */
} L7_arpCacheStats_t;


/* IPv6 Neighbor entry information */
typedef struct L7_in6NeighborEntry_s
{
  L7_in6_addr_t       ipAddr;     /* In6 Address of the neighbor */
  L7_linkLayerAddr_t  macAddr;    /* Mac address of the neighbor */
  L7_ushort16         vlanId;     /* vlan id */
  L7_uint32           intIfNum;   /* outgoing internal interface number */
  L7_uint32           hits;       /*  non-zero, count if possible */
  L7_uint32           flags;      /* Local */
  L7_uint32           ageSecs;    /* Neighbor entry age time, in seconds */ 
} L7_in6NeighborEntry_t;

/* Used to query hardware for hit status of IPv6 host entry */
typedef struct L7_ndpQuery_s
{
  L7_in6_addr_t  ip6Addr;    /* neighbor's IPv6 address. could be global or link local. */
  L7_ushort16  vlanId;     /* vlan id - ignored for now */
  L7_uint32  intIfNum;   /* internal interface number to neighbor */

  /* A source hit means we received a packet with source MAC set to that of our host entry. */
  /* Dest hit means we sent a packet to this host and used this host entry to resolve 
   * the next hop. */
  L7_uint32  ndpQueryFlags;
#define   L7_NDP_HIT_SOURCE  (1 << 4)  
#define   L7_NDP_HIT_DEST    (1 << 5)  
#define   L7_NDP_HIT_ANY (L7_NDP_HIT_SOURCE | L7_NDP_HIT_DEST)

  /* Last time the entry was hit in hardware (seconds since boot) */
  L7_uint32  lastHitTime;    

} L7_ndpQuery_t;

/*-------------------
   ROUTE ENTRY FLAGS
 -------------------*/

/* Some of these flags are analagous to the flags defined in netbsd/route.h.  
   An enum is used rather than a #define to simplify searching for these values.

   These values are returned for the SNMP ipRouteProto variable.
*/

typedef enum
{
  L7_RTF_DEFAULT_ROUTE  = 1,        /* default route                                    */
  L7_RTF_STATIC         = 2,        /* statically added route                           */
                                    /*    - analogus to RTF_STATIC                      */   
  L7_RTF_DIRECT_ATTACH  = 4,        /* directly attached network                        */
  L7_RTF_DISCARD        = 8,        /* discard packets without error                    */
                                    /*    - analogus to RTF_BLACKHOLE                   */   
  L7_RTF_REJECT         = 16,       /* discard packets with error (i.e. unreachable)    */
                                    /*    - analogus to RTF_BLACKHOLE                   */   
  L7_RTF_SEND_TO_CPU    = 32,       /* send to CPU to handle                            */
  L7_RTF_SEND_TO_2ND_RTR= 64,       /* Forwarder must send to 2ndary rtg engine         */
  L7_RTF_CHECKPOINT = 128,
  L7_RTF_SRC_ONLY = 256             /* route used only for originating traffic, ie no fwding */

} L7_RT_ENTRY_FLAGS_t;


/* The following values are returned for the SNMP ipRouteType variable 
*/
typedef enum
{
  L7_RTF_TYPE_OTHER  = 1,        /* default route                                    */
  L7_RTF_TYPE_INVALID  = 2,      /* Invalid route 									 */
  L7_RTF_TYPE_DIRECT  = 3,       /* directly attached network                        */
  L7_RTF_TYPE_INDIRECT  = 4      /* route to a remote network 					     */
} L7_RT_ENTRY_ROUTE_TYPE_t;

/* The following values are returned for the SNMP inetCidrRouteType variable
*/
typedef enum
{
  L7_IP_CIDR_ROUTE_TYPE_OTHER     = 1,     /* not specified by this mib                        */
  L7_IP_CIDR_ROUTE_TYPE_REJECT    = 2,     /* discard packets with error                       */
  L7_IP_CIDR_ROUTE_TYPE_LOCAL     = 3,     /* local interface                                  */
  L7_IP_CIDR_ROUTE_TYPE_REMOTE    = 4,     /* remote destination                               */
  L7_IP_CIDR_ROUTE_TYPE_BLACKHOLE = 5      /* discard packets without error                    */
} L7_IP_CIDR_ENTRY_ROUTE_TYPE_t;


/*-------------------
   ROUTE STRUCTURES
 -------------------*/

typedef struct equalCostPath_s 
{
  L7_arpEntry_t   arpEntry;         /* Arp information for the specified ECMP route */
  L7_uint32       flags;            /* L7_RT_ENTRY_FLAGS_t */
#define	L7_RT_EQ_PATH_VALID	   1    /* valid route exists for this metric/equal cost path */
} L7_EqualCostPath_t;

typedef struct in6equalCostPath_s 
{
  L7_in6NeighborEntry_t neighEntry;  /* Neighbor information for the
                                      * specified ECMP route */
  L7_uint32       flags;             /* L7_RT_ENTRY_FLAGS_t */
} L7_in6EqualCostPath_t;


typedef struct L7_ECMPRoute_s
{
  L7_EqualCostPath_t    equalCostPath[L7_RT_MAX_EQUAL_COST_ROUTES];  /* list of equal cost paths
                                                                        to the destination network */
  L7_uint32             numOfRoutes;                                 /* number of routes in the
                                                                        equalCostPath array */ 

} L7_ECMPRoute_t;

typedef struct L7_ECMP6Route_s
{
  L7_in6EqualCostPath_t equalCostPath[L7_RT_MAX_EQUAL_COST_ROUTES];  /* list of equal cost paths
                                                                        to the destination network */
  L7_uint32             numOfRoutes;                                 /* number of routes in the
                                                                        equalCostPath array */ 

} L7_ECMP6Route_t;


/* Implementor note:  
   For each destination network supported, the number of best routes which may be
   stored to that destination can be up to L7_MAX_IP_METRICS.Each metric corresponds 
   to a type of service, The type of service associated with a particular route entry 
   is specified in the entry. */

typedef struct L7_routeEntry_s
{
  L7_uint32       ipAddr;
  L7_uint32       subnetMask;

  /* Protocol via which the route(s) were learned. 
     A value from L7_RTO_PROTOCOL_INDICES_t. */
  L7_uint32       protocol;      

  L7_uint32       metric;        /* cost associated with this route */
  L7_ECMPRoute_t  ecmpRoutes;    /* Best routes associated with this TOS */
  L7_uint32       flags;         /* Defined by L7_RT_ENTRY_FLAGS_t */

  /* Route preference (also called administrative distance). [0 to 255]. */
  L7_uchar8 pref;
  /*Specifies the last time the route was updated (in hours:minutes:seconds)*/
  L7_uint32 updateTime;
} L7_routeEntry_t;

typedef struct L7_in6routeEntry_s
{
  L7_in6_addr_t   addr;
  L7_uint32       prefixLen;

  /* Protocol via which the route(s) were learned. 
     A value from L7_RTO_PROTOCOL_INDICES_t. */
  L7_uint32       protocol;      

  L7_uint32       metric;        /* cost associated with this route */
  L7_ECMP6Route_t ecmpRoutes;    /* Best routes associated with this Service Class */
  L7_uint32       flags;         /* Defined by L7_RT_ENTRY_FLAGS_t */

  /* Route preference (also called administrative distance). [0 to 255]. */
  L7_uchar8 pref;

} L7_in6routeEntry_t;

/*-------------------
   OSPF  STRUCTURES
 -------------------*/

/* enums */
typedef enum
{
   L7_ROUTER             = 0x01,
   L7_AS_BOUNDARY_ROUTER = 0x02,
   L7_BORDER_ROUTER      = 0x04,
   L7_NETWORK            = 0x08,
   L7_STUB_NETWORK       = 0x10
}  L7_OSPF_DESTINATION_TYPE_t;

typedef enum
{
   L7_OSPF_INTRA_AREA,
   L7_OSPF_INTER_AREA,
   L7_OSPF_TYPE_1_EXT,
   L7_OSPF_TYPE_2_EXT,
   L7_OSPF_NSSA_TYPE_1_EXT,  /* NSSA Change */
   L7_OSPF_NSSA_TYPE_2_EXT,  /* NSSA Change */
   L7_OSPF_PATH_TYPE_ILG
}  L7_OSPF_PATH_TYPE_t;

/* The LSA entry structure */
typedef struct L7_ospfLsdbEntry_s
{
  L7_uint32             AreaId;

  /* if link opaque LSA, the internal interface number of the associated interface */
  L7_uint32             intIfNum;

  /* The ordering here is in the same lines as the LSA Header */
  L7_ushort16           LsdbAge;
  L7_uchar8             LsdbOptions;
  L7_uchar8             LsdbType;
  L7_uint32             LsdbLsid;
  L7_uint32             LsdbRouterId;
  L7_uint32             LsdbSequence;
  L7_ushort16           LsdbChecksum;
  L7_ushort16           LsdbAdvertLength;

  L7_uchar8             rtrLsaFlags;

} L7_ospfLsdbEntry_t;

/* The Opaque LSA (Type - 9,10,11) entry structure */
typedef struct L7_ospfOpaqueLsdbEntry_s
{
  L7_uint32             AreaId;

  /* The ordering here is in the same lines as the LSA Header */
  L7_ushort16           LsdbAge;
  L7_uchar8             LsdbOptions;
  L7_uchar8             LsdbType;
  L7_uint32             LsdbLsid;
  L7_uint32             LsdbRouterId;
  L7_uint32             LsdbSequence;
  L7_ushort16           LsdbChecksum;
  L7_ushort16           LsdbAdvertLength;

  L7_uchar8             rtrLsaFlags;

  /* Next two attributes identify an interface. If the interface is numbered, 
   * the interface is identified by its primary IP address. If the interface
   * is unnumbered, the interface is identified by its interface index. 
   * (While we could use interface index all the time, we are stuck with this 
   * given the definitions in the standard OSPFv2 MIB (RFC 4750 at the moment). */
  L7_uint32             IpAdr;
  
  /* Identifies interface for link opaque LSAs. Same as internal interface number. */
  L7_uint32             IfIndex;  

} L7_ospfOpaqueLsdbEntry_t;

/* The LSA entry structure for OSPFv3 */
typedef struct L7_ospfv3LsdbEntry_s
{
  L7_uint32             AreaId;

  /* The ordering here is in the same lines as the LSA Header */
  L7_ushort16           LsdbAge;
  L7_ushort16           LsdbType;
  L7_uint32             LsdbLsid;
  L7_uint32             LsdbRouterId;
  L7_uint32             LsdbSequence;
  L7_ushort16           LsdbChecksum;
  L7_ushort16           LsdbAdvertLength;

  L7_uchar8             rtrLsaFlags;

  /* Options field is 24-bits long.
   * And as per rfc 2740, lower 6 bits of Options field are assigned */
  L7_uint32             LsdbOptions;

} L7_ospfv3LsdbEntry_t;

/* Identification of area aggregation */
typedef struct L7_ospfAreaRangeDescr_s
{
  L7_IP_ADDR_t                  areaId;
  L7_uint32                     lsdbType;   /* L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t */
  L7_IP_ADDR_t                  ipAddr;
  L7_IP_MASK_t                  netMask;
} L7_ospfAreaRangeDescr_t;


/* Configuration of area aggregation */
typedef struct L7_ospfAreaAddrRangeInfo_s     
{
    L7_ospfAreaRangeDescr_t descr;         /* Description of area range */
    L7_uint32               advertiseMode; /* L7_BOOL  */
} L7_ospfAreaAddrRangeInfo_t;

/* The Path structure corresponding to each next hop
 * in the routing entry */
typedef struct L7_RtbPathInfo_s
{
   L7_uint32        nextHopIpAdr;        /* Next Hop Ip Address */
   L7_uint32        nextHopIfIndex;      /* internal interface number */
} L7_RtbPathInfo_t;

/* The routing table entry information that is shown to
 * the external world */
typedef struct L7_RtbEntryInfo_s
{
   L7_uint32      destinationIp;       /* Router Id or network IP address */
   L7_uint32      destinationIpMask;   /* network IP mask */
   L7_uint32      areaId;              /* entry originator's Area */
   L7_ushort16    destinationType;     /* Router or Network 
                                       (defined by L7_OSPF_DESTINATION_TYPE_t)*/
   L7_ushort16    pathType;            /* defined by L7_OSPF_PATH_TYPE_t */
   L7_uint32      cost;                /* link cost */

   L7_uint32      no_of_paths;         /* number of equal cost multipaths */

   L7_RtbPathInfo_t    path[L7_RT_MAX_EQUAL_COST_ROUTES]; /* equal cost multiple next hops */
} L7_RtbEntryInfo_t;

/* Maximum number of characters in an OSPF SPF reason string */
#define L7_SPF_REASON_LEN 50

/* Data for "show ip ospf statistics" */
typedef struct L7_OspfSpfStats_s
{
  /* time of an SPF run. Seconds since boot. */
  L7_uint32 spfTime;

  /* how long the SPF took. Milliseconds. */
  L7_uint32 spfDuration; 

  L7_uchar8 reason[L7_SPF_REASON_LEN];

} L7_OspfSpfStats_t;

/* Data for "show ip ospf database database-summary" */
typedef struct L7_OspfAreaDbStats_s
{
  L7_uint32 RouterLsaCount;
  L7_uint32 NetworkLsaCount;
  L7_uint32 NetSumLsaCount;
  L7_uint32 InterPrefixLsaCount;
  L7_uint32 RtrSumLsaCount;
  L7_uint32 InterRouterLsaCount;
  L7_uint32 NSSALsaCount;
  L7_uint32 LinkLsaCount;
  L7_uint32 IntraPrefixLsaCount;
  L7_uint32 GraceLsaCount;
  L7_uint32 LinkUnknownLsaCount;
  L7_uint32 AreaUnknownLsaCount;
  L7_uint32 AsUnknownLsaCount;
  L7_uint32 SelfOrigNSSALsaCount;
  L7_uint32 OpaqueLinkLsaCount;
  L7_uint32 OpaqueAreaLsaCount;

} L7_OspfAreaDbStats_t;

/* matches t_PCK_Stats */
typedef struct L7_OspfIntfStats_s
{
   L7_uint32  RxPackets;
   L7_uint32  DiscardPackets;
   L7_uint32  BadVersion;
   L7_uint32  BadNetwork;
   L7_uint32  BadVirtualLink;
   L7_uint32  BadArea;
   L7_uint32  BadDstAdr;
   L7_uint32  BadAuType;
   L7_uint32  BadAuthentication;
   L7_uint32  BadNeighbor;
   L7_uint32  BadPckType;
   L7_uint32  TxPackets;
   L7_uint32  RxHellos;
   L7_uint32  RxDbDescr;
   L7_uint32  RxLsReq;
   L7_uint32  RxLsUpdate;
   L7_uint32  RxLsAck;
   L7_uint32  HellosIgnored;
   L7_uint32  TxHellos;
   L7_uint32  TxDbDescr;
   L7_uint32  TxLsReq;
   L7_uint32  TxLsUpdate;
   L7_uint32  TxLsAck;
} L7_OspfIntfStats_t;

/* Structure for status global information from OSPFv2 or OSPFv3. 
 * This is status information, not configuration.  */
typedef struct L7_ospfStatus_s
{
  /* Number of normal OSPF areas currently active. */
  L7_uint32 normalAreas;

  /* Number of stub areas currently active. */
  L7_uint32 stubAreas;

  /* Number of NSSAs currently active. */
  L7_uint32 nssaAreas;

  /* L7_TRUE if router is an ABR */
  L7_BOOL abr;     

  /* L7_TRUE if router is an ASBR */
  L7_BOOL asbr;    

  /* L7_TRUE if router is in stub mode. Could be LSDB overload. For OSPFv3
   * corresponds to the R-bit in the router LSA. If R-bit is clear (router not 
   * active), stubRouter is L7_TRUE. */
  L7_BOOL stubRouter;   

  /* If OSPF is a stub router, the reason */
  OSPF_STUB_ROUTER_REASON_t stubReason;

  /* If the stub reason is startup, the number of seconds until the 
   * configured startup time expires. */
  L7_uint32 stubRemainingTime;

  /* L7_TRUE if router is in external LSDB overflow, as defined by RFC 1765. */
  L7_BOOL extLsdbOverflow;

  /* The number of external LSAs currently in the LSDB. */
  L7_uint32 extLsaCount;

  /* Sum of the checksums of all external LSAs. */
  L7_uint32 extLsaChecksum;

  /* Number of opaque LSAs with AS flooding scope */
  L7_uint32 asOpaqueLsaCount;

  /* Sum of checksums of opaque LSAs with AS flooding scope. */
  L7_uint32 asOpaqueLsaChecksum;

  /* The number of LSAs originated by this router. */
  L7_uint32 lsasOrig;

  /* The number of LSAs received by this router. To be counted, the LSA must
   * pass validation and must either be new (not previously in the LSDB) or
   * more recent than the existing db entry. */
  L7_uint32 lsasReceived;

  /* Current number of LSAs in LSDB. */
  L7_uint32 lsaCount;

  /* The maximum number of LSAs the LSDB can store. */
  L7_uint32 lsaCapacity; 

  /* High water mark for the number of LSAs in the LSDB. */
  L7_uint32 lsaHighWater;

  /* Current number of retransmission list entries */
  L7_uint32 retxEntries;

  /* The maximum number of retransmission list entries. */
  L7_uint32 retxCapacity;

  /* High water mark for the number of retransmission list entries allocated */
  L7_uint32 retxHighWater;

  /* If doing graceful restart */
  OSPF_GR_RESTART_STATUS_t restartStatus;

  /* How much longer until graceful restart grace period expires (seconds) */
  L7_uint32 restartAge;

  OSPF_GR_EXIT_REASON_t grExitReason;

} L7_ospfStatus_t;

/* Structure to get OSPFv2 and OSPFv3 helpful neighbor status */
#define OSPF_RESTART_REASON_LEN 64    /* max string len of restart reason */
#define OSPF_GR_EXIT_REASON_LEN 64    /* max string len of exit reason */
typedef struct L7_ospfHelpfulNbrStatus_s
{
  /* How last helper mode ended */
  OSPF_HELPER_EXIT_REASON_t exitReason;

  L7_uchar8 exitReasonStr[OSPF_GR_EXIT_REASON_LEN];

  /* Reason code from neighbor's last grace LSA */
  L7_uint32 restartCode;    /* actual code value from grace LSA */
  L7_uchar8 restartReason[OSPF_RESTART_REASON_LEN];

  /* the number of seconds remaining in the current graceful restart interval */
  L7_uint32 restartAge;
  
} L7_ospfHelpfulNbrStatus_t;


/*-------------------
   OSPFV3  STRUCTURES
   todo:  when we implement ospfv3 area range change ipaddr to be ipv6addr
 -------------------*/

/* Identification of area aggregation */
typedef struct L7_ospfv3AreaRangeDescr_s
{
  L7_IP_ADDR_t                  areaId;
  L7_uint32                     lsdbType;   /* L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t */
  L7_in6_prefix_t               prefix;
} L7_ospfv3AreaRangeDescr_t;


/* Configuration of area aggregation */
typedef struct L7_ospfv3AreaAddrRangeInfo_s     
{
    L7_ospfv3AreaRangeDescr_t descr;         /* Description of area range */
    L7_uint32                 advertiseMode; /* L7_BOOL  */
} L7_ospfv3AreaAddrRangeInfo_t;

/* The Path structure corresponding to each next hop
 * in the routing entry */

typedef struct L7_in6_RtbPathInfo_s
{
   L7_in6_addr_t    nextHopIpAdr;        /* Next Hop Ip Address */
   L7_uint32        nextHopIfIndex;      /* internal interface number */
} L7_in6_RtbPathInfo_t;

/* The routing table entry information that is shown to
 * the external world */
typedef struct L7_in6_RtbEntryInfo_s
{
   L7_uint32      destinationIp;       /* Router Id or network IP address */
   L7_uint32      destinationIpMask;   /* network IP mask */
   L7_uint32      areaId;              /* entry originator's Area */
   L7_ushort16    destinationType;     /* Router or Network 
                                       (defined by L7_OSPF_DESTINATION_TYPE_t)*/
   L7_ushort16    pathType;            /* defined by L7_OSPF_PATH_TYPE_t */
   L7_uint32      cost;                /* link cost */

   L7_uint32      no_of_paths;         /* number of equal cost multipaths */

   L7_in6_RtbPathInfo_t    path[L7_RT_MAX_EQUAL_COST_ROUTES]; /* equal cost multiple next hops */
} L7_in6_RtbEntryInfo_t;

typedef struct ipHelperStats_s
{
  /* Number of valid DHCP client messages received */
  L7_uint32 dhcpClientMsgsReceived;

  /* Number of DHCP client packets relayed to a server. If a packet is relayed to
   * multiple servers, stat is incremented for each server. */
  L7_uint32 dhcpClientMsgsRelayed;

  /* Number of messages received from a DHCP server */
  L7_uint32 dhcpServerMsgsReceived;

  /* Number of DHCP server messages relayed to client */
  L7_uint32 dhcpServerMsgsRelayed;

  /* Number of UDP client messages received. Includes DHCP. */
  L7_uint32 udpClientMsgsReceived;

  /* Number of UDP client messages relayed. Includes DHCP. */
  L7_uint32 udpClientMsgsRelayed;

  /* DHCP client message arrived with hops > max allowed */
  L7_uint32 tooManyHops;

  /* DHCP client message arrived with secs field lower than min allowed */
  L7_uint32 tooEarly;

  /* Received DHCP client message with giaddr already set to our own address */
  L7_uint32 spoofedGiaddr;

  /* Number of packets intercepted whose TTL is <= 1 */
  L7_uint32 ttlExpired;

  /* Number of times server lookup matched a discard entry */
  L7_uint32 matchDiscardEntry;

} ipHelperStats_t;

#endif
