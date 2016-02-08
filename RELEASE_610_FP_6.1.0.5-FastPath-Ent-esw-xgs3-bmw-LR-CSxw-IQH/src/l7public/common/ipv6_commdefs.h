/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   ipv6_commdefs.h
*
* @purpose    Ip6 common definitions 
*
* @component  Ip6 Layer
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/



#ifndef INCLUDE_IP6_COMMDEFS_H
#define INCLUDE_IP6_COMMDEFS_H


#include "l3_addrdefs.h"
#include "l7_packet.h"     /* for L7_linklayer_addr_t */
#include "commdefs.h"
#include "datatypes.h"
#include "l3_commdefs.h"



/*--------------------------------------*/
/*  Route Types              */
/*--------------------------------------*/

typedef enum
{
  RTO6_RESERVED = 0,      /* Not a valid protocol */
  RTO6_LOCAL,
  RTO6_STATIC,
  RTO6_MPLS,
  RTO6_OSPFV3_INTRA_AREA,
  RTO6_OSPFV3_INTER_AREA,
  RTO6_OSPFV3_TYPE1_EXT,
  RTO6_OSPFV3_TYPE2_EXT,
  RTO6_OSPFV3_NSSA_TYPE1_EXT,
  RTO6_OSPFV3_NSSA_TYPE2_EXT,
  RTO6_RIP,
  RTO6_DEFAULT,
  RTO6_IBGP,  
  RTO6_EBGP,
  RTO6_6TO4,
  RTO6_RSVD1,       /* Reserved to minimize config file upgrades */
  RTO6_RSVD2,       /* Reserved to minimize config file upgrades */
  RTO6_RSVD3,       /* Reserved to minimize config file upgrades */
  RTO6_LAST_TYPE  /* Always keep this last */

} L7_RTO6_ROUTE_TYPE_INDICES_t;
/* NOTE: if indices change, must modify ip6map_rto_preferences */


#define     L7_RTO6_MAX_ROUTE_TYPES  RTO6_LAST_TYPE


/*-------------------
   ROUTE STRUCTURES
 -------------------*/

typedef struct equalCostPath6_s 
{
  L7_in6_addr_t   ip6Addr; 
  L7_uint32       intIfNum;
} L7_EqualCostPath6_t;


typedef struct L7_ECMPRoute6_s
{
  L7_EqualCostPath6_t   equalCostPath[L7_RT_MAX_EQUAL_COST_ROUTES];  /* list of equal cost paths
                                                                        to the destination network */
  L7_uint32             numOfRoutes;                                 /* number of routes in the
                                                                        equalCostPath array */ 

} L7_ECMPRoute6_t;


/* Implementor note:  
   For each destination network supported, the number of best routes which may be
   stored to that destination can be up to L7_MAX_IP_METRICS.Each metric corresponds 
   to a type of service, The type of service associated with a particular route entry 
   is specified in the entry. */

typedef struct L7_route6Entry_s
{
  L7_in6_addr_t   ip6Addr;
  L7_int32       ip6PrefixLen;   /* need signed int */

  /* Protocol via which the route(s) were learned. 
     A value from L7_RTO6_ROUTE_TYPE_INDICES_t. */
  L7_uint32       protocol;      

  L7_uint32       metric;        /* cost associated with this route */
  L7_ECMPRoute6_t ecmpRoutes;    /* Best routes associated with this TOS */
  L7_uint32       flags;         /* Defined by L7_RT_ENTRY_FLAGS_t */
                                 /* note: dapi presumes same flags as v4 */

  /* Route preference (also called administrative distance). [0 to 255]. */
  L7_uchar8 pref;
  /*Specifies the last time the route was updated (in hours:minutes:seconds)*/
  L7_uint32 updateTime;
} L7_route6Entry_t;


#define RTO6_ROUTE_ADD     1
#define RTO6_ROUTE_MOD     2
#define RTO6_ROUTE_DEL     3

/*
 *  Flags used when registering for route change notifications.
 */
#define RTO6_ALL_ROUTES  1
#define RTO6_BEST_ROUTE  2


/* Used to convery static route configuration information amongst components */

typedef struct L7_rtr6StaticRoute_s
{
	L7_in6_addr_t       ip6Addr;		   /* IP Address                   */
	L7_uint32           ip6PrefixLen;	   /* IP Mask Length               */
	L7_in6_addr_t       nextHopRtr;	   /* Next Hop Router IP           */
        L7_uint32           nhIntIfNum;    /* interface if nh is link local */
	L7_uint32           preference;	   /* route preference             */
        L7_uint32           flags;         /* value based on L7_RT_ENTRY_FLAGS_t */
} L7_rtr6StaticRoute_t;

/* per address prefix flags for router advertisement */
#define L7_IP6_PREF_RADV_ADVERTISE                     0x01
#define L7_IP6_PREF_RADV_ONLINK                        0x02  
#define L7_IP6_PREF_RADV_AUTOCONF                      0x04

/* flags for address add/remove */
#define L7_RTR6_ADDR_LINK_LOCAL_ENA                0x01  /* link local addr even if no global addr */
#define L7_RTR6_ADDR_EUI64                         0x02  /* use eui64 interface ID */
#define L7_RTR6_ADDR_REPLACE                       0x04

/* decrement mode flags for prefixes */
#define L7_RTR6_DECR_VALID_LIFE                    0x01
#define L7_RTR6_DECR_PREFERRED_LIFE                0x02

/* for NbrTblGet */
typedef struct L7_ip6NbrTblEntry_s
{
    L7_in6_addr_t       ip6Addr;
    L7_linkLayerAddr_t  macAddr;
    L7_uint32           intIfNum;
    L7_uint32           nbrState;
    L7_uint32           lastUpdated;
    L7_BOOL             isRouter;

}L7_ip6NbrTblEntry_t;

/* interface address states */
typedef enum ip6IntfAddrStates{
    L7_IPV6_INTFADDR_INACTIVE =1,
    L7_IPV6_INTFADDR_TENTATIVE,
    L7_IPV6_INTFADDR_ACTIVE         /* dad sussessful */
}L7_IPV6_INTFADDR_STATES;

/* an interface address, along with its state */
typedef struct L7_ipv6IntfAddr_s
{
    L7_in6_addr_t             ip6Addr;
    L7_uint32                 ip6PrefixLen;
    L7_IPV6_INTFADDR_STATES   ip6AddrState;
}L7_ipv6IntfAddr_t;

/* Maximum number of IPV6 addresses that may be configured on a routing
** interface (including primary and secondary). Note not all interfaces can
** have this many, also limited by secondary address pool.
*/                               
#define     L7_RTR6_MAX_INTF_ADDRS      4

/* Maximum number of address general prefixes */
#define     L7_RTR6_MAX_GEN_PREFIX      8
#define     L7_RTR6_MAX_GPREF_NAMELEN   31

/* neighbor state definitions per rfc 2465 */
typedef enum
{
   L7_IP6_NBR_STATE_REACHABLE      = 1,
   L7_IP6_NBR_STATE_STALE          = 2,
   L7_IP6_NBR_STATE_DELAY          = 3,
   L7_IP6_NBR_STATE_PROBE          = 4,
   L7_IP6_NBR_STATE_UNKNOWN        = 6
} L7_IP6_NBR_STATE_t;



#define L7_V6_PREFIX_LEN_MAX                 128

#define L7_L3_MIN_IPV6_MTU                   1280
/* V6 MAX MTU is limited by OSPFv3 buffer pool. */
#define L7_L3_MAX_IPV6_MTU                   1500
/* Max IPv6 MTU on a tunnel interface */
#define L7_L3_MAX_IPV6_MTU_TUNNEL            1480
#define L7_IP6_ND_DAD_ATTEMPTS_MAX           600
#define L7_IP6_ND_DAD_ATTEMPTS_MIN           0
#define L7_IP6_NDNS_INTERVAL_MIN             1000
#define L7_IP6_NDNS_INTERVAL_MAX             4294967295U
#define L7_IP6_ND_LIFETIME_MIN               0
#define L7_IP6_ND_LIFETIME_MAX               9000
#define L7_IP6_ND_REACHABLE_MAX              3600000UL 
#define L7_IP6_ND_REACHABLE_MIN              0
#define L7_IP6_ND_PREFERREDLIFE_MAX          4294967295UL 
#define L7_IP6_ND_PREFERREDLIFE_MIN          0
#define L7_IP6_ND_VALIDLIFE_MAX              4294967295UL
#define L7_IP6_ND_VALIDLIFE_MIN              0
#define L7_IP6_ND_RADV_MIN_ADV_MIN           3
#define L7_IP6_ND_RADV_MIN_ADV_MAX           1350
#define L7_IP6_ND_RADV_MAX_ADV_MIN           4
#define L7_IP6_ND_RADV_MAX_ADV_MAX           1800
#define L7_IP6_HOP_LIMIT_MIN                 0
#define L7_IP6_HOP_LIMIT_MAX                 255


#define L7_IP6_LSID_MIN                      0
#define L7_IP6_LSID_MAX                      4294967295UL

/* if mtu unconfigured, link mtu is used */
#define L7_IP6MAP_INTF_MTU_NOT_CONFIGURED              0

/* the multiplier to use to compute min ra-interval from max ra-interval */
#define L7_RTR6_RA_INTERVAL_MULTIPLIER      ((L7_double64)75/100)


/*
 * all the following are defaults values specified by RFCs and cannot be changed.
 */

#define L7_IP6_RADV_PREF_LIFE_DEF    (7*24*60*60)    /* 1 week */
#define L7_IP6_RADV_VALID_LIFE_DEF   (30*24*60*60)
#define L7_IP6_RADV_MIN_ADV_DEF      200
#define L7_IP6_RADV_MAX_ADV_DEF      600 
#define L7_IP6_RADV_ADV_LIFE_DEF     1800
#define L7_IP6_RADV_INTF_MANAGED_DEF  0
#define L7_IP6_RADV_INTF_OTHER_DEF    0
#define L7_IP6_RADV_INTF_SUPPRESS_DEF 0
#define L7_IP6_PREF_RADV_ADVERTISE_DEF  IP6MAP_PREF_RADV_ADVERTISE  
#define L7_IP6_PREF_RADV_ONLINK_DEF     IP6MAP_PREF_RADV_ONLINK
#define L7_IP6_PREF_RADV_AUTOCONFO_DEF  IP6MAP_PREF_RADV_AUTOCONF
#define L7_IP6_ND_DAD_TRANSMITS_DEF   1
/* default is 0, which implies use a non-zero protocol specified value.
   but 0 is also special for other protocol operations. What this means is if
   you want default operation, specify 0, NOT the default value */
#define L7_IP6_RADVD_NS_INTERVAL_DEF        0
#define L7_IP6_RADVD_ND_REACHABLE_DEF       0


/* ping min/max sizes */
#define L7_IP6_PING_MIN          48
#define L7_IP6_PING_MAX          2048

/*------------------------------------------*/
/* DHCP6 Server/Relay definitions           */
/*------------------------------------------*/
#define L7_DHCP6S_POOL_NAME_MINLEN          1
#define L7_DHCP6S_POOL_NAME_MAXLEN          31
#define L7_DHCP6S_DNS_SERVER_MAX            8
#define L7_DHCP6S_DNS_DOMAIN_MAX            5
#define L7_DHCP6S_DOMAIN_NAME_MINLEN        1
#define L7_DHCP6S_DOMAIN_NAME_MAXLEN        256
#define L7_DHCP6S_SIP_SERVER_MAX            8
#define L7_DHCP6S_SIP_DOMAIN_MAX            5
#define L7_DHCP6S_NTP_SERVER_MAX            8
#define L7_DHCP6S_MAX_POOL_NUMBER           16
#define L7_DHCP6S_HOST_NAME_MAXLEN          32
#define L7_DHCP6S_POOL_HOST_MAX             10

#define L7_DHCP6S_PREFIX_MIN               0
#define L7_DHCP6S_PREFIX_MAX               128

/* DHCP6 Server/Relay option flags (32-bit bitmask) */
#define L7_DHCP6S_OPTION_RAPID_COMMIT     0x00000001
#define L7_DHCP6S_OPTION_ALLOW_UNICAST    0x00000002

/* DHCP6 Relay Information Option definitions */
#define L7_DHCP6S_RELAYOPT_REMOTEID_MAXLEN  64
#define L7_DHCP6S_RELAYOPT_REMOTEID_DUID_IFID  "duid-ifid"

/* DHCP6 Remote Id Suboption */
#define L7_DHCP6S_RELAYAGENT_REMOTEOPT_MIN  1
#define L7_DHCP6S_RELAYAGENT_REMOTEOPT_MAX  65535

/* Defines for DHCP6 POOL Type */

#define L7_DHCP6S_INACTIVE_POOL       0
#define L7_DHCP6S_DYNAMIC_POOL        1
#define L7_DHCP6S_MANUAL_POOL         2

/* To support SNMP create operation */
#define L7_DHCP6S_NOT_READY_POOL      3

#define L7_DHCP6_LIFETIME_INFINITE         0xffffffff

typedef enum {
  L7_DHCP6_MODE_NONE = 0, 
  L7_DHCP6_MODE_SERVER, 
  L7_DHCP6_MODE_CLIENT, 
  L7_DHCP6_MODE_RELAY
} L7_DHCP6_MODE_t;

typedef enum {
  L7_DHCP6_SR_MODE_NONE = 0,   /* L7_DHCP6_MODE_NONE */
  L7_DHCP6_SR_MODE_SERVER = 1, /* L7_DHCP6_MODE_SERVER */
  L7_DHCP6_SR_MODE_RELAY = 3,  /* L7_DHCP6_MODE_RELAY */
} L7_DHCP6_SR_MODE_t;

typedef enum {
  L7_DHCP6_PREFIX_IAPD = 0, 
  L7_DHCP6_PREFIX_IANA, 
  L7_DHCP6_PREFIX_IATA 
} L7_DHCP6_PREFIX_TYPE_t;

#define L7_DHCP6_RELAYAGENTINFOOPT_MIN            54
#define L7_DHCP6_RELAYAGENTINFOOPT_MAX            65535

/*------------------------------------------*/
/*  END DHCP6 Server/Relay definitions      */
/*------------------------------------------*/

/*------------------------------------------*/
/*  IPv6 tunneling definitions      */
/*------------------------------------------*/

#define L7_EMPTYSTRING ""


#endif /* INCLUDE_IP6_COMMDEFS_H */

