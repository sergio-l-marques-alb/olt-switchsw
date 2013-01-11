
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l3_commdefs.h
*
* @purpose   Common defines, enums and data structures for packets
*
* @component 
*
* @comments  none
*
* @create    03/15/2001
*
* @author    wjacobs
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef L3_COMMDEFS_H
#define L3_COMMDEFS_H

#include <datatypes.h>
#include <l7_packet.h>


/****************************************
*
*  IP ROUTER DEFINES
*
*****************************************/

/* IP MTU defines */
#define L7_L3_MIN_IP_MTU             68
/* Interfaces now allow the IPv4 MTU to be > 1500 bytes. The IPv4 MTU is limited 
 * by the link MTU. The maximum IPv6 MTU is still 1500. */
#define L7_L3_MAX_IP_MTU             1500

#define L7_L3_MAX_POSSIBLE_IP_MTU    (L7_MAX_FRAME_SIZE - (L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + L7_ETHERNET_CHECKSUM_LEN))

/* Invalid Interface number. */
#define L7_INVALID_INTF             0

#define L7_RTO_PREFERENCE_MIN                       1   
#define L7_RTO_PREFERENCE_MAX                       255 
#define L7_RTO_RIP_PREFERENCE_DEF                   FD_RTR_RTO_PREF_RIP 


#define L7_RTO_STATIC_PREFERENCE_DEF                FD_RTR_RTO_PREF_STATIC 

#define L7_RTO_PREFERENCE_INTRA                     110 
#define L7_RTO_PREFERENCE_INTER                     110 
#define L7_RTO_PREFERENCE_EXTERNAL                  110 

/* registration for next hop notification */
#define L7_IP_NHRES_MAX                             64

#define MCAST_UNUSED_PARAM(arg) ((void) arg)

/*--------------------------------------*/
/*  IP Metrics                          */
/*--------------------------------------*/
typedef enum TosTypes
{
  L7_TOS_NORMAL_SERVICE        = 0,
  L7_TOS_MINIMIZE_COST         = 2,
  L7_TOS_MAXIMIZE_RELIABILITY  = 4,
  L7_TOS_MAXIMIZE_THROUGHPUT   = 8,
  L7_TOS_MINIMIZE_DELAY        = 16
}  L7_TOSTYPES;


typedef enum TosTypeIndex
{
  L7_TOS_NORMAL_SERVICE_INDEX  = 0,
  L7_TOS_MINIMIZE_COST_INDEX,
  L7_TOS_MAXIMIZE_RELIABILITY_INDEX,
  L7_TOS_MAXIMIZE_THROUGHPUT_INDEX,
  L7_TOS_MINIMIZE_DELAY_INDEX        
}  L7_TOS_TYPES_INDICES;

#define     L7_RTR_MAX_TOS_INDICES         5
#define     L7_RTR_MAX_TOS                32

/*---------------------------------------*/
/*   Bandwidth                           */
/*---------------------------------------*/
#define     L7_OSPF_MIN_REF_BW                1
#define     L7_OSPF_MAX_REF_BW                4294967

#define     L7_L3_MIN_BW                 1
#define     L7_L3_MAX_BW                 10000000

/*--------------------------------------*/
/*   ICMP Rate Limit                    */
/*---------------------------------------*/
#define L7_L3_ICMP_RATE_LIMIT_MAX_BURST_SIZE 200
#define L7_L3_ICMP_RATE_LIMIT_MIN_BURST_SIZE 1
#define L7_L3_ICMP_RATE_LIMIT_MAX_INTERVAL 2147483647 /* Milli seconds */
#define L7_L3_ICMP_RATE_LIMIT_MIN_INTERVAL 0

/*--------------------------------------*/
/*  IP Routing Protocols                */
/*--------------------------------------*/

/*This is used for IP Mapping Registrants */

/* NOTE: Make sure to also update ipMapEventClientNames array with
 * appropriate protocol names in the same order.
 */
typedef enum
{
  L7_RESERVED_PROTOCOL_ID = 0,
  L7_IPRT_RIP,
  L7_IPRT_VRRP,
  L7_IPRT_OSPF,
  L7_IPRT_MGMD,  /*This sould be before multicast routing protocols */
  L7_IPRT_PIMDM,
  L7_IPRT_DVMRP,
  L7_IPRT_STATIC,
  L7_IPRT_LOCAL,
  L7_IPRT_BGP,
  L7_IPRT_DEFAULT,  
  L7_IPRT_RTR_DISC,
  L7_IPRT_MPLS,
  L7_IPRT_MCAST,
  L7_IPRT_PIMSM,
  L7_IPRT_OSPFV3,
  L7_IPRT_RLIM,
  L7_IPRT_DHCP6S,
  L7_IPRT_WIRELESS,
  L7_IPRT_RADIUS,
  L7_IPRT_SNOOPING,
  L7_IPRT_SNTP,
  L7_IPRT_DHCP_CLIENT,
  L7_LAST_ROUTER_PROTOCOL

} L7_IPMAP_REGISTRANTS_t;

/* The following enum will be used for the ipMapIntfStateChangeCallback
   function used to alert protocols on configuration/ state changes
   on routing interfaces. If you change this enum, also change 
   ipMapRouterEventNames[]. */
typedef enum
{
  L7_RESERVED_STATE_CHANGE = 0,
  L7_RTR_ENABLE,                            /* Routing  enabled */
  L7_RTR_DISABLE_PENDING,                   /* Routing to be disabled */
  L7_RTR_DISABLE,                           /* Routing  disable */   
  L7_RTR_INTF_CREATE,                       /* routing interface create */
  L7_RTR_INTF_ENABLE,                       /* Routing interface enable complete */
  L7_RTR_INTF_DISABLE_PENDING,              /* Routing interface disable pendng */
  L7_RTR_INTF_DISABLE,                      /* Routing interface disable complete */
  L7_RTR_INTF_DELETE,                       /* Delete routing interface */
  L7_RTR_INTF_IPADDR_ADD,                   /* IP MAP no longer sends */
  L7_RTR_INTF_IPADDR_DELETE_PENDING,        /* IP MAP no longer sends */
  L7_RTR_INTF_IPADDR_DELETE,                /* IP MAP no longer sends */
  L7_RTR_INTF_SECONDARY_IP_ADDR_ADD,        /* Routing interface secondary ip address add   */
  L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE,     /* Routing interface secondary ip address delete */
  L7_RTR_INTF_MTU,                          /* IP MTU has changed on this interface */
  L7_RTR_INTF_BW_CHANGE,                    /* Interface bandwidth changed */
  L7_RTR_INTF_SPEED_CHANGE,                 /* Actual interface speed has changed */
  L7_RTR_STARTUP_DONE,                      /* All router events have been sent at startup */
  L7_RTR_INTF_HOST_MODE,                    /* Host Interface is Enabled */
  L7_LAST_RTR_STATE_CHANGE

} L7_RTR_EVENT_CHANGE_t;

/*--------------------------------------*/
/*  Routing Protocol Types              */
/*  (lower number means higher priority)*/
/*--------------------------------------*/
/* If you add or remove an enum, you must also update protocolMapping[]
 * and rtoProtoNames[] in rto.c. */
typedef enum
{
  RTO_RESERVED = 0,      /* Not a valid protocol */
  RTO_LOCAL,
  RTO_STATIC,
  RTO_MPLS,
  RTO_OSPF_INTRA_AREA,
  RTO_OSPF_INTER_AREA,
  RTO_OSPF_TYPE1_EXT,
  RTO_OSPF_TYPE2_EXT,
  RTO_OSPF_NSSA_TYPE1_EXT,
  RTO_OSPF_NSSA_TYPE2_EXT,
  RTO_OSPFV3_INTRA_AREA,
  RTO_OSPFV3_INTER_AREA,
  RTO_OSPFV3_TYPE1_EXT,
  RTO_OSPFV3_TYPE2_EXT,
  RTO_OSPFV3_NSSA_TYPE1_EXT,
  RTO_OSPFV3_NSSA_TYPE2_EXT,
  RTO_RIP,
  RTO_DEFAULT,
  RTO_IBGP,  
  RTO_EBGP,
  RTO_RSVD1,       /* Reserved to minimize config file upgrades */
  RTO_RSVD2,       /* Reserved to minimize config file upgrades */
  RTO_RSVD3,       /* Reserved to minimize config file upgrades */
  RTO_LAST_PROTOCOL  /* Always keep this last */

} L7_RTO_PROTOCOL_INDICES_t;

/*----------------------------------------------------------*/
/*  Route Preference Types                                  */
/*  Route types for which preferences can be set            */
/*  We are making independent enums for the preferences     */
/*  and decoupling from the L7_RTO_PROTOCOL_INDICES_t enum  */
/*----------------------------------------------------------*/
typedef enum
{
  ROUTE_PREF_RESERVED = 0,      /* Not a valid protocol */
  ROUTE_PREF_LOCAL,
  ROUTE_PREF_STATIC,
  ROUTE_PREF_MPLS,
  ROUTE_PREF_OSPF_INTRA_AREA,
  ROUTE_PREF_OSPF_INTER_AREA,
  ROUTE_PREF_OSPF_EXTERNAL,
  ROUTE_PREF_RIP,
  ROUTE_PREF_IBGP,  
  ROUTE_PREF_EBGP,
  ROUTE_PREF_6TO4,
  ROUTE_PREF_GLOBAL_DEFAULT_GATEWAY,
  ROUTE_PREF_DEFAULT_ROUTE_DHCP,
  ROUTE_PREF_LAST               /* Always keep this last */
 
} L7_ROUTE_PREF_INDICES_t;

#define     L7_RTO_MAX_PROTOCOLS        RTO_LAST_PROTOCOL
#define     L7_ROUTE_MAX_PREFERENCES    ROUTE_PREF_LAST

/* Redistribution route types*/
typedef enum
{
  REDIST_RT_FIRST = 0,
  REDIST_RT_LOCAL,
  REDIST_RT_STATIC,
  REDIST_RT_RIP,
  REDIST_RT_OSPF,
  REDIST_RT_BGP,
  REDIST_RT_OSPFV3,
  REDIST_RT_LAST
} L7_REDIST_RT_INDICES_t;

/* NSF applications that may send IPv4 routes to RTO. Values are powers
 * of two used to index nsfRoutesPending. RIP and BGP don't support NSF 
 * now but adding values for them anyway. */
typedef enum
{
  NSF_PROTOCOL_OSPF = 0x1,
  NSF_PROTOCOL_RIP = 0x2,
  NSF_PROTOCOL_BGP = 0x4,
  NSF_PROTOCOL_LAST = 0x8

} L7_NSF_PROTOCOL_t;

/* These values should match those in aclActionIndex_t, except 
 * we do not need the CPU values here.
 */
typedef enum
{
    FILTER_ACTION_PERMIT = 0,
    FILTER_ACTION_DENY = 1
} FILTER_ACTION_t;

/*------------------------------*/
/* CIDR Routing Protocol Types  */
/*------------------------------*/
/* The following Enum is used by the ipMapIpCidrRouteProtoGet
   Routing protocols have been defined as per rfc 2096 */

typedef enum
{
  L7_IP_CIDR_ROUTE_PROTO_OTHER = 1,                           /* 1: Not specified                 */
  L7_IP_CIDR_ROUTE_PROTO_LOCAL,                               /* 2: Local interface               */
  L7_IP_CIDR_ROUTE_PROTO_NETMGMT,                             /* 3: Static Route                  */
  L7_IP_CIDR_ROUTE_PROTO_ICMP,                                /* 4: Result of ICMP Redirect       */

  /* The following are dynamic routing protocols */
  L7_IP_CIDR_ROUTE_PROTO_EGP,                                 /* 5:   Exterior Gateway Protocol   */
  L7_IP_CIDR_ROUTE_PROTO_GGP,                                 /* 6:   Gateway-Gateway Protocol    */
  L7_IP_CIDR_ROUTE_PROTO_HELLO,                               /* 7:   FuzzBall HelloSpeak         */
  L7_IP_CIDR_ROUTE_PROTO_RIP,                                 /* 8:   Berkeley RIP or RIP-II      */
  L7_IP_CIDR_ROUTE_PROTO_ISIS,                                /* 9:   Dual IS-IS                  */
  L7_IP_CIDR_ROUTE_PROTO_ESIS,                                /* 10:  ISO 9542                    */
  L7_IP_CIDR_ROUTE_PROTO_CISCOIGRP,                           /* 11:  Cisco IGRP                  */
  L7_IP_CIDR_ROUTE_PROTO_BBNSPFIGP,                           /* 12:  BBN SPF IGP                 */
  L7_IP_CIDR_ROUTE_PROTO_OSPF,                                /* 13:  Open Shortest Path First    */
  L7_IP_CIDR_ROUTE_PROTO_BGP,                                 /* 14:  Border Gateway Protocol     */
  L7_IP_CIDR_ROUTE_PROTO_IDPR,                                /* 15:  Interdomain Policy Routing  */
  L7_IP_CIDR_ROUTE_PROTO_CISCOEIGRP,                          /* 16:  Cisco EIGRP                 */
  L7_IP_CIDR_ROUTE_PROTO_LAST

} L7_IP_CIDR_ROUTE_PROTO_t;

/*---------------------------------*/
/* IP Address Configuration Method */
/*---------------------------------*/
/* The following enum indicates the method used to
 * configure an IP Address on a Router Interface.
 */
typedef enum
{
  L7_RTR_INTF_IP_ADDR_METHOD_NONE = 0,
  L7_RTR_INTF_IP_ADDR_METHOD_CONFIG,     /* Manual Configuration */
  L7_RTR_INTF_IP_ADDR_METHOD_DHCP        /* Leased through DHCP */

} L7_RTR_INTF_IP_ADDR_METHOD_t;

/****************************************
*
*  ARP DEFINES
*
*****************************************/

/*--------------------------------------*/
/*  ARP Constants                       */
/*--------------------------------------*/

/* config parameter min/max/default values */
#define L7_IP_ARP_AGE_TIME_MIN          15            /* seconds */
#define L7_IP_ARP_AGE_TIME_MAX          21600         /* seconds (=6 hr) */
#define L7_IP_ARP_AGE_TIME_DEF          1200          /* seconds (=20 min)*/

#define L7_IP_ARP_RESP_TIME_MIN         1             /* seconds */
#define L7_IP_ARP_RESP_TIME_MAX         10            /* seconds */
#define L7_IP_ARP_RESP_TIME_DEF         1             /* seconds */

#define L7_IP_ARP_RETRIES_MIN           0
#define L7_IP_ARP_RETRIES_MAX           10
#define L7_IP_ARP_RETRIES_DEF           4

#define L7_IP_ARP_AGE_TIME_CHECK        360          /* 6 minutes */

#define L7_IP_ARP_CACHE_SIZE_MIN        (L7_RTR_MAX_RTR_INTERFACES + (2 * L7_IP_ARP_CACHE_STATIC_MAX))

#define L7_IP_ARP_DYNAMIC_RENEW_DEF     L7_DISABLE


/**************************************
*
*     OSPF DEFINES
*
***************************************/

#define L7_OSPF_NOT_CONFIGURED  0

typedef enum
{
  L7_OSPF_METRIC_EXT_TYPE1 = 1,
  L7_OSPF_METRIC_EXT_TYPE2

} L7_OSPF_EXT_METRIC_TYPES_t;

typedef enum
{
  L7_OSPF_METRIC_TYPE_INTERNAL = 1,
  L7_OSPF_METRIC_TYPE_EXT1 = 2,
  L7_OSPF_METRIC_TYPE_EXT2 = 4,
  L7_OSPF_METRIC_TYPE_NSSA_EXT1 = 8,
  L7_OSPF_METRIC_TYPE_NSSA_EXT2 = 16

} L7_OSPF_METRIC_TYPES_t;

typedef enum
{
  L7_OSPF_AREA_IMPORT_EXT = 1,            /* Import and propagate T5 LSAs */
  L7_OSPF_AREA_IMPORT_NO_EXT,             /* Do not import and propagate ext LSAs- make stub area*/
  L7_OSPF_AREA_IMPORT_NSSA                /* Import and propagate T7 LSAs */

} L7_OSPF_AREA_EXT_ROUTING_t;

typedef enum
{
  L7_OSPF_AREA_NO_SUMMARY = 1,            /* Do not originate and propagate summary LSAs */
  L7_OSPF_AREA_SEND_SUMMARY               /* Originate and propagate summary LSAs */

} L7_OSPF_AREA_SUMMARY_t;

typedef enum
{
  L7_OSPF_ROW_ACTIVE = 1,       /* - in call:   activate the object  */
                                /*   in return: the object is active */
  L7_OSPF_ROW_NOT_IN_SERVICE,   /* - in call:   deactivate the object */
                                /* - in return: the object is not active */
  L7_OSPF_ROW_NOT_READY,        /* - in return only: the object not ready */
  L7_OSPF_ROW_CREATE_AND_GO,    /* - in call only: init and run the object */
  L7_OSPF_ROW_CREATE_AND_WAIT,  /* - in call only: init the object */
  L7_OSPF_ROW_DESTROY,          /* - in call only: delete the object */
  L7_OSPF_ROW_CHANGE

} L7_OSPF_ROW_STATUS_t;

typedef enum
{
  L7_OSPF_AREA_STUB_METRIC = 1,
  L7_OSPF_AREA_STUB_COMPARABLE_COST,
  L7_OSPF_AREA_STUB_NON_COMPARABLE_COST

} L7_OSPF_STUB_METRIC_TYPE_t;

typedef enum
{
  L7_OSPF_AREA_RANGE_EFFECT_ADVERTISE_MATCHING = 1,
  L7_OSPF_AREA_RANGE_EFFECT_DONOT_ADVERTISE_MATCHING

}L7_OSPF_AREA_RANGE_EFFECT_t;

typedef enum
{
  L7_OSPF_AREA_AGGREGATE_EFFECT_ADVERTISEMATCHING = 1,
  L7_OSPF_AREA_AGGREGATE_EFFECT_DONOT_ADVERTISEMATCHING

}L7_OSPF_AREA_AGGREGATE_STATUS_t;

typedef enum
{
  L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK = 3,
  L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK = 7

}L7_OSPF_AREA_AGGREGATE_LSDBTYPE_t;

typedef enum
{
  L7_OSPF_MULTICAST_FORWARDING_BLOCKED = 0,
  L7_OSPF_MULTICAST_FORWARDING_MULTICAST,
  L7_OSPF_MULTICAST_FORWARDING_UNICAST

}L7_OSPF_MULTICAST_FORWARDING_t;

typedef enum
{
  L7_OSPF_NBMA_NBR_PERMANENCE_DOWN = 1,
  L7_OSPF_NBMA_NBR_PERMANENCE_PERMANENT

}L7_OSPF_NBMA_NBR_PERMANENCE__t;

typedef enum
{
  L7_OSPF_VIRT_NBR_STATE_DOWN = 1,
  L7_OSPF_VIRT_NBR_STATE_ATTEMPT,
  L7_OSPF_VIRT_NBR_STATE_INIT,
  L7_OSPF_VIRT_NBR_STATE_TWOWAY,
  L7_OSPF_VIRT_NBR_STATE_EXCHANGE_START,
  L7_OSPF_VIRT_NBR_STATE_EXCHANGE,
  L7_OSPF_VIRT_NBR_STATE_LOADING,
  L7_OSPF_VIRT_NBR_STATE_FULL

}L7_OSPF_VIRT_NBR_STATE_t;

typedef enum
{
  L7_OSPF_VIRT_IFSTATE_DOWN = 1,
  L7_OSPF_VIRT_IFSTATE_POINTTOPOINT = 4

}L7_OSPF_VIRT_IFSTATE__t;


/* NSSATranslatorRole
** Specifies whether or not an NSSA border router will
** unconditionally translate Type-7 LSAs into Type-5 LSAs.  When
** it is set to Always, an NSSA border router always translates
** Type-7 LSAs into Type-5 LSAs regardless of the translator state
** of other NSSA border routers.  When it is set to Candidate, an
** NSSA border router participates in the translator election
** process described in Section 3.1.  The default setting is
** Candidate.
*/
typedef enum
{
   L7_OSPF_NSSA_TRANSLATOR_ALWAYS    = 1,
   L7_OSPF_NSSA_TRANSLATOR_CANDIDATE = 2

}  L7_OSPF_NSSA_TRANSLATOR_ROLE_t;

typedef enum
{
   L7_OSPF_NSSA_TRANS_STATE_ENABLED  = 1,
   L7_OSPF_NSSA_TRANS_STATE_ELECTED  = 2,
   L7_OSPF_NSSA_TRANS_STATE_DISABLED = 3

}  L7_OSPF_NSSA_TRANSLATOR_STATE_t;

/* TranslatorStabilityInterval
** Defines the length of time an elected Type-7 translator will
** continue to perform its translator duties once it has
** determined that its translator status has been deposed by
** another NSSA border router translator as described in Section
** 3.1 and 3.3.  The default setting is 40 seconds.
*/
#define L7_OSPF_NSSA_TRANS_STABILITY_INT_MAX   3600
#define L7_OSPF_NSSA_TRANS_STABILITY_INT_MIN   0
#define L7_OSPF_NSSA_TRANS_STABILITY_INT_DEF   40

typedef enum
{
  L7_OSPF_OPT_Q_BIT = 0x01,
  L7_OSPF_OPT_E_BIT = 0x02,
  L7_OSPF_OPT_MC_BIT = 0x04,
  L7_OSPF_OPT_NP_BIT = 0x08,
  L7_OSPF_OPT_O_BIT = 0x40,
  L7_VPN_OPTION_BIT = 0x80

} L7_OSPF_OPTIONS_t;

/* Bits defined for the OSPF router LSA flags field */
typedef enum
{
    L7_OSPFRTRLSAFLAGS_B = 0x1,
    L7_OSPFRTRLSAFLAGS_E = 0x2,
    L7_OSPFRTRLSAFLAGS_V = 0x4,
    L7_OSPFRTRLSAFLAGS_W = 0x8,
    L7_OSPFRTRLSAFLAGS_Nt = 0x10
} L7_OSPFRTRLSAFLAGS_t;

/* OSPF stub router configuration options */
typedef enum
{
  /* OSPF is not configured as a stub router. */
  OSPF_STUB_RTR_CFG_DISABLED = 0,

  /* OSPF is configured as a stub router. */
  OSPF_STUB_RTR_CFG_ALWAYS = 1,

  /* OSPF is a stub router for a limited time after reboot. */
  OSPF_STUB_RTR_CFG_STARTUP = 2

} OSPF_STUB_ROUTER_CFG_t;

/* Reason why OSPF is in stub router advertisement mode */
typedef enum
{
  /* OSPF is configured as a stub router */
  OSPF_STUB_RTR_CONFIGURED = 0,

  /* OSPF configured as a stub router at startup and OSPF is
   * in the startup period. */
  OSPF_STUB_RTR_STARTUP = 1,

  /* OSPF automaically entered stub router mode because of a resource limitation. */
  OSPF_STUB_ROUTER_RESOURCE = 2

} OSPF_STUB_ROUTER_REASON_t;

typedef enum
{
  /* Does not act as a helpful neighbor */
  OSPF_HELPER_NONE,

  /* Acts as a helpful neighbor for planned restarts only */
  OSPF_HELPER_PLANNED_ONLY,

  /* Acts as a helpful neighbor for planned and unplanned restarts */
  OSPF_HELPER_ALWAYS

} OSPF_HELPER_SUPPORT_t;

typedef enum
{
  /* Never do graceful restart */
  OSPF_NSF_NONE,

  /* Do graceful restart only for planned restarts */
  OSPF_NSF_PLANNED_ONLY,

  /* Do graceful restart for both planned and unplanned restarts */
  OSPF_NSF_ALWAYS

} OSPF_NSF_SUPPORT_t;

/* reasons why OSPF could exit helpful neighbor mode for a given neighbor. Compare to 
 * ospfNbrRestartHelperExitReason in RFC 4750. */
typedef enum
{
  OSPF_HELPER_EXIT_NONE = 1,    /* never acted as helpful neighbor for this neighbor */
  OSPF_HELPER_IN_PROGRESS = 2,  /* restart is in progress */
  OSPF_HELPER_COMPLETED = 3,    /* restart successfully completed */
  OSPF_HELPER_TIMED_OUT = 4,    /* grace period timed out */
  OSPF_HELPER_TOPO_CHANGE = 5,  /* topology change interrupted the graceful restart */
  OSPF_HELPER_MANUAL_CLEAR = 6, /* clear ip ospf neighbor */
  OSPF_HELPER_LINK_DOWN = 7     /* link to neighbor went down */

} OSPF_HELPER_EXIT_REASON_t;

/* reasons why OSPF could exit graceful restart. Compare to ospfRestartExitReason
 * in RFC 4750. */
typedef enum
{
  OSPF_GR_EXIT_NONE = 1,    /* never attempted graceful restart */
  OSPF_GR_IN_PROGRESS = 2,  /* restart is in progress */
  OSPF_GR_COMPLETED = 3,    /* restart successfully completed */
  OSPF_GR_TIMED_OUT = 4,    /* grace period timed out */
  OSPF_GR_TOPO_CHANGE = 5,  /* topology change interrupted the graceful restart */
  OSPF_GR_MANUAL_CLEAR = 6  /* clear ip ospf */

} OSPF_GR_EXIT_REASON_t;

/* Current status of OSPF graceful restart. Compare to RFC 4750 ospfRestartStatus. */
typedef enum
{
  OSPF_GR_NOT_RESTARTING = 1,
  OSPF_GR_PLANNED_RESTART = 2,
  OSPF_GR_UNPLANNED_RESTART = 3

} OSPF_GR_RESTART_STATUS_t;


#define L7_OSPF_INTF_DOWN                             1
#define L7_OSPF_INTF_LOOPBACK                         2
#define L7_OSPF_INTF_WAITING                          3
#define L7_OSPF_INTF_POINTTOPOINT                     4
#define L7_OSPF_INTF_DESIGNATEDROUTER                 5
#define L7_OSPF_INTF_BACKUPDESIGNATEDROUTER           6
#define L7_OSPF_INTF_OTHERDESIGNATEDROUTER            7

#define L7_OSPF_INTF_DEFAULT                          0
#define L7_OSPF_INTF_BROADCAST                        1
#define L7_OSPF_INTF_NBMA                             2
#define L7_OSPF_INTF_PTP                              3
#define L7_OSPF_INTF_VIRTUAL                          4
#define L7_OSPF_INTF_POINTTOMULTIPOINT                5
#define L7_OSPF_INTF_TYPE_LOOPBACK                    6

#define L7_OSPF_INTF_PASSIVE                          L7_TRUE
#define L7_OSPF_INTF_NOT_PASSIVE                      L7_FALSE

#define L7_OSPF_INTF_SECONDARIES_NONE                 0
#define L7_OSPF_INTF_SECONDARIES_ALL                  0xffffffff

/* if L7_OSPF_INTF_METRIC_NOT_CONFIGURED is zero, the metric will be 
 *  computed from port speed otherwise only the value configured by user will be used
 */
#define L7_OSPF_INTF_METRIC_NOT_CONFIGURED            0 

typedef enum
{
  L7_S_ILLEGAL_LSA          = 0,
  L7_S_ROUTER_LSA           = 1,
  L7_S_NETWORK_LSA          = 2,
  L7_S_IPNET_SUMMARY_LSA    = 3,
  L7_S_ASBR_SUMMARY_LSA     = 4,
  L7_S_AS_EXTERNAL_LSA      = 5,
  L7_S_GROUP_MEMBER_LSA     = 6,
  L7_S_NSSA_LSA             = 7,
  L7_S_TMP2_LSA             = 8,
  L7_S_LINK_OPAQUE_LSA      = 9,
  L7_S_AREA_OPAQUE_LSA      = 10,
  L7_S_AS_OPAQUE_LSA        = 11,
  L7_S_LAST_LSA_TYPE        = 12
} L7_OSPF_LSDB_TYPE_t;

#define L7_OSPF_DYNAMIC            1
#define L7_OSPF_PERMANENT          2

#define L7_OSPF_DOWN               1
#define L7_OSPF_ATTEMPT            2
#define L7_OSPF_INIT               3
#define L7_OSPF_TWOWAY             4
#define L7_OSPF_EXCHANGESTART      5
#define L7_OSPF_EXCHANGE           6
#define L7_OSPF_LOADING            7
#define L7_OSPF_FULL               8

/*------------------*/
/* OSPF RANGE SIZES */
/*------------------*/

#define L7_OSPF_INTF_PRIORITY_MAX     255
#define L7_OSPF_INTF_PRIORITY_MIN     0
#define L7_OSPF_INTF_PRIORITY_DEF     1

#define L7_VRRP_INTF_PRIORITY_MAX     255
#define L7_VRRP_INTF_PRIORITY_MIN     0
#define L7_VRRP_INTF_PRIORITY_DEF     1

#define L7_OSPF_INTF_SECONDS_MAX      65535
#define L7_OSPF_INTF_SECONDS_MIN      1
#define L7_OSPF_INTF_SECONDS_DEF      10

#define L7_OSPF_INTF_DEAD_MAX         65535
#define L7_OSPF_INTF_DEAD_MIN         1
#define L7_OSPF_INTF_DEAD_DEF         40

#define L7_OSPF_INTF_RETRANSMIT_MAX   3600
#define L7_OSPF_INTF_RETRANSMIT_MIN   0
#define L7_OSPF_INTF_RETRANSMIT_DEF   5

#define L7_OSPF_INTF_NBMAPOLL_MAX     0x7FFFFFFF   /* 2147483647    */
#define L7_OSPF_INTF_NBMAPOLL_MIN     0

#define L7_OSPF_INTF_TRANSMITDELAY_MAX 3600
#define L7_OSPF_INTF_TRANSMITDELAY_MIN 1
#define L7_OSPF_INTF_TRANSMITDELAY_DEF 5

/* Cisco allows the user to set this to 16777215. Presumably, this allows
 * an operator to make destinations outside the OSPF domain or even the
 * stub area unreachable. */
#define L7_OSPF_AREA_STUBMETRIC_MAX   0xFFFFFF /*16777215*/
#define L7_OSPF_AREA_STUBMETRIC_MIN   1

#define L7_OSPF_AREA_NSSA_DEFAULT_METRIC_MAX   0xFFFFFE /*16777214*/
#define L7_OSPF_AREA_NSSA_DEFAULT_METRIC_MIN   1

#define L7_OSPF_INTF_COST_MAX   65535
#define L7_OSPF_INTF_COST_MIN   1
#define L7_OSPF_INTF_COST_DEF   10

#define L7_OSPF_VIRTIF_RETRANSMIT_MAX  3600
#define L7_OSPF_VIRTIF_RETRANSMIT_MIN  0
#define L7_OSPF_VIRTIF_RETRANSMIT_DEF  5

#define L7_OSPF_VIRTIF_DELAY_MAX   3600
#define L7_OSPF_VIRTIF_DELAY_MIN   0
#define L7_OSPF_VIRTIF_DELAY_DEF   1
                                     
#define L7_OSPF_EXT_LSDB_LIMIT_MAX   0x7FFFFFFF   /* 2147483647 */
#define L7_OSPF_EXT_LSDB_LIMIT_MIN   -1
#define L7_OSPF_EXT_LSDB_LIMIT_DEF   -1

#define L7_OSPF_EXIT_OVERFLOW_INT_MAX   0x7FFFFFFF   /* 2147483647  */
#define L7_OSPF_EXIT_OVERFLOW_INT_MIN   0
#define L7_OSPF_EXIT_OVERFLOW_INT_DEF   0

#define L7_OSPF_SPF_DELAY_TIME_MAX      65535
#define L7_OSPF_SPF_DELAY_TIME_MIN      0
#define L7_OSPF_SPF_DELAY_TIME_DEF      5

#define L7_OSPF_SPF_HOLD_TIME_MAX      65535
#define L7_OSPF_SPF_HOLD_TIME_MIN      0
#define L7_OSPF_SPF_HOLD_TIME_DEF      10


#define L7_OSPF_AUTH_KEY_ID_MIN         0
#define L7_OSPF_AUTH_KEY_ID_MAX         255

#define L7_VRID_RANGE_MAX     255
#define L7_VRID_RANGE_MIN     1
#define L7_OSPF_REDIST_METRIC_MIN       0
#define L7_OSPF_REDIST_METRIC_MAX       16777214

#define L7_OSPF_DEFAULT_METRIC_MIN      1
#define L7_OSPF_DEFAULT_METRIC_MAX      16777214

/* How long to stay in stub router mode at startup. 5 seconds to 1 day */
#define L7_OSPF_STUB_ROUTER_DURATION_MIN   5
#define L7_OSPF_STUB_ROUTER_DURATION_MAX   86400  

/* Override metric for summary LSAs when in stub router mode. */
#define L7_OSPF_STUB_ROUTER_SUMLSA_MET_MIN  1
#define L7_OSPF_STUB_ROUTER_SUMLSA_MET_MAX  16777215

#define L7_OSPF_RESTART_INTERVAL_MIN    0
#define L7_OSPF_RESTART_INTERVAL_MAX    1800

#define L7_OSPF_LSA_PACKET_LENGTH_MAX   65535

/* NOTE: Must match OSPF_MAP_EXTEN_AUTH_MAX_KEY_SIMPLE definition */
#define L7_AUTH_MAX_KEY_OSPF_SIMPLE   8
/* NOTE: Must match OSPF_MAP_EXTEN_AUTH_MAX_KEY_MD5 definition */
#define L7_AUTH_MAX_KEY_OSPF_MD5      16
/* define this as the larger of the two */
#define L7_AUTH_MAX_KEY_OSPF          L7_AUTH_MAX_KEY_OSPF_MD5

#define L7_OSPF_AREA_BACKBONE         0


/*------------------*/
/* OSPF TRAP TYPES  */
/*------------------*/

/* The following flags are to be used by user-interface routines to
   enable/disable or get the mode of the specified ospf trap */
#define L7_OSPF_TRAP_NONE                             0x0
#define L7_OSPF_TRAP_ALL                  (L7_OSPF_TRAP_ERRORS_ALL | \
                                          L7_OSPF_TRAP_LSA_ALL  | \
                                          L7_OSPF_TRAP_RETRANSMIT_ALL | \
                                          L7_OSPF_TRAP_STATE_CHANGE_ALL | \
                                          L7_OSPF_TRAP_OVERFLOW_ALL)

#define L7_OSPF_TRAP_ERRORS_ALL          (L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR | \
                                          L7_OSPF_TRAP_IF_CONFIG_ERROR | \
                                          L7_OSPF_TRAP_IF_AUTH_FAILURE | \
                                          L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE | \
                                          L7_OSPF_TRAP_RX_BAD_PACKET | \
                                          L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)
#define L7_OSPF_TRAP_LSA_ALL             (L7_OSPF_TRAP_MAX_AGE_LSA | \
                                          L7_OSPF_TRAP_ORIGINATE_LSA)                                          
#define L7_OSPF_TRAP_RETRANSMIT_ALL      (L7_OSPF_TRAP_TX_RETRANSMIT | \
                                          L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT)
#define L7_OSPF_TRAP_STATE_CHANGE_ALL    (L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE | \
                                          L7_OSPF_TRAP_NBR_STATE_CHANGE | \
                                          L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE | \
                                          L7_OSPF_TRAP_IF_STATE_CHANGE)
#define L7_OSPF_TRAP_OVERFLOW_ALL        (L7_OSPF_TRAP_LS_DB_OVERFLOW | \
                                          L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW)
#define L7_OSPF_TRAP_RTB_ALL              L7_OSPF_TRAP_RTB_ENTRY_INFO                                          
#define L7_OSPF_TRAP_IF_RX_ALL            L7_OSPF_TRAP_IF_RX_PACKET

#define L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE             0x1
#define L7_OSPF_TRAP_NBR_STATE_CHANGE                 0x2
#define L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE            0x4
#define L7_OSPF_TRAP_IF_CONFIG_ERROR                  0x8
#define L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR             0x10
#define L7_OSPF_TRAP_IF_AUTH_FAILURE                  0x20
#define L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE             0x40
#define L7_OSPF_TRAP_RX_BAD_PACKET                    0x80
#define L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET            0x100
#define L7_OSPF_TRAP_TX_RETRANSMIT                    0x200
#define L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT            0x400
#define L7_OSPF_TRAP_ORIGINATE_LSA                    0x800
#define L7_OSPF_TRAP_MAX_AGE_LSA                      0x1000
#define L7_OSPF_TRAP_LS_DB_OVERFLOW                   0x2000
#define L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW       0x4000
#define L7_OSPF_TRAP_IF_STATE_CHANGE                  0x8000
#define L7_OSPF_TRAP_IF_RX_PACKET                     0x10000
#define L7_OSPF_TRAP_RTB_ENTRY_INFO                   0x20000

/*enumerated values for ospfConfigErrorType */
typedef enum
{
  L7_CONFIG_OK = 0,
  L7_BAD_VERSION,
  L7_AREA_MISMATCH,
  L7_UNKNOWN_NBMA_NBR,
  L7_UNKNOWN_VIRTUAL_NBR,
  L7_AUTH_TYPE_MISMATCH,
  L7_AUTH_FAILURE,
  L7_NET_MASK_MISMATCH,
  L7_HELLO_INTERVAL_MISMATCH,
  L7_DEAD_INTERVAL_MISMATCH,
  L7_OPTION_MISMATCH,
  L7_BAD_PACKET,
  L7_UKNOWN_ERROR

}L7_OSPF_TRAP_CONFIG_ERROR_TYPE_t;

/* OSPF Packet Types */
typedef enum 
{
    L7_S_ILLEGAL = 0,
    L7_S_HELLO,
    L7_S_DB_DESCR,
    L7_S_LS_REQUEST,
    L7_S_LS_UPDATE,
    L7_S_LS_ACK
}L7_OSPF_PCKTYPE_t;

/**************************************
*
*     OSPFV3 DEFINES
*
*  ospfv3 uses the ospf defines. 
*  This section contains definitions that
*  Must be different for ospfv3.
*
***************************************/

#define L7_OSPFV3_NOT_CONFIGURED  0

/* draft OSPFv3 MIB says dead interval max is 65535. That's different from
 * the dead max for OSPFv2. */
#define L7_OSPFV3_INTF_DEAD_MAX         0xFFFF   /* 65535 */
#define L7_OSPFV3_INTF_DEAD_MIN         1
#define L7_OSPFV3_INTF_DEAD_DEF         40

typedef enum
{
  L7_V3_ILLEGAL_LSA          = 0,
  L7_V3_ROUTER_LSA           = 1,
  L7_V3_NETWORK_LSA          = 2,
  L7_V3_INTER_PREFIX_LSA     = 3,
  L7_V3_INTER_ROUTER_LSA     = 4,
  L7_V3_AS_EXTERNAL_LSA      = 5,
  L7_V3_GROUP_MEMBER_LSA     = 6,
  L7_V3_NSSA_LSA             = 7,
  L7_V3_LINK_LSA             = 8,
  L7_V3_INTRA_PREFIX_LSA     = 9,
  L7_V3_TMP3_LSA             = 10,
  L7_V3_GRACE_LSA            = 11,
  L7_V3_LINK_UNKNOWN_LSA     = 12,
  L7_V3_AREA_UNKNOWN_LSA     = 13,
  L7_V3_AS_UNKNOWN_LSA       = 14,
  L7_V3_LAST_LSA_TYPE        = 15
} L7_OSPFV3_LSDB_TYPE_t;

typedef enum
{
  L7_OSPFV3_OPT_V6_BIT = 0x01,
  L7_OSPFV3_OPT_E_BIT =  0x02,
  L7_OSPFV3_OPT_MC_BIT = 0x04,
  L7_OSPFV3_OPT_NP_BIT = 0x08,
  L7_OSPFV3_OPT_R_BIT =  0x10,
  L7_OSPFV3_OPT_DC_BIT = 0x20

} L7_OSPFV3_OPTIONS_t;
/* The Following are the Exceptions in Ospfv3 Traps because of the current release doesn't support IPSec */
#define L7_OSPFV3_TRAP_ALL              (L7_OSPFV3_TRAP_ERRORS_ALL | \
                                         L7_OSPF_TRAP_LSA_ALL | \
                                         L7_OSPF_TRAP_RETRANSMIT_ALL | \
                                         L7_OSPF_TRAP_STATE_CHANGE_ALL | \
                                         L7_OSPF_TRAP_OVERFLOW_ALL)

#define L7_OSPFV3_TRAP_ERRORS_ALL       (L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR | \
                                         L7_OSPF_TRAP_IF_CONFIG_ERROR | \
                                         L7_OSPF_TRAP_RX_BAD_PACKET | \
                                         L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)

/**************************************
*
*     ROUTERDISCOVER DEFINES
*
***************************************/

#define L7_RTR_DISC_MAXINTERVAL_MIN                    4 
#define L7_RTR_DISC_MAXINTERVAL_MAX                    1800
#define L7_RTR_DISC_MAXINTERVAL_DEF                    600

#define L7_RTR_DISC_MININTERVAL_MIN                    3 
#define L7_RTR_DISC_MININTERVAL_MAX                    1800
#define L7_RTR_DISC_MININTERVAL_DEF(time)              (0.75*(time))

#define L7_RTR_DISC_LIFETIME_MIN                       L7_RTR_DISC_MAXINTERVAL_MIN
#define L7_RTR_DISC_LIFETIME_MAX                       9000
#define L7_RTR_DISC_LIFETIME_DEF(time)                 (3*(time))

#define L7_RTR_DISC_PREFERENCE_MIN                     0x80000000 /* -2147483648 */  
#define L7_RTR_DISC_PREFERENCE_MAX                     0x7FFFFFFF /* 2147483647  */
#define L7_RTR_DISC_PREFERENCE_DEF                     0

/******************************************
*
*     ROUTER VRRP DEFINES
*
*******************************************/

#define L7_ROUTING_VRRP_MIN_VRID                      L7_PLATFORM_ROUTING_VRRP_MIN_VRID

#define L7_ROUTING_VRRP_MAX_VRID                      L7_PLATFORM_ROUTING_VRRP_MAX_VRID 

/**************************************
*
*     RIP DEFINES
*
***************************************/

/*--------------------------------------*/
/*  Authentication Constants            */
/*--------------------------------------*/

#define L7_AUTH_MAX_KEY_RIP        16

#define L7_RIP_CONF_AUTH_KEY_ID_MIN     0
#define L7_RIP_CONF_AUTH_KEY_ID_MAX     255

/*--------------------------------------*/
/*  RIP Constants                       */
/*--------------------------------------*/

typedef enum
{
  L7_RIP_CONF_SEND_DO_NOT_SEND = 1,
  L7_RIP_CONF_SEND_RIPV1,
  L7_RIP_CONF_SEND_RIPV1_COMPAT,                    
  L7_RIP_CONF_SEND_RIPV2,
  L7_RIP_CONF_SEND_RIPV1_DEMAND,                    /* not used */
  L7_RIP_CONF_SEND_RIPV2_DEMAND                     /* not used */

} L7_RIP_CONF_SEND_t;

typedef enum
{
  L7_RIP_CONF_RECV_RIPV1 = 1,
  L7_RIP_CONF_RECV_RIPV2,
  L7_RIP_CONF_RECV_RIPV1_RIPV2,
  L7_RIP_CONF_RECV_DO_NOT_RECEIVE

} L7_RIP_CONF_RECV_t;

typedef enum
{
  L7_RIP_SPLITHORIZON_NONE = 1,
  L7_RIP_SPLITHORIZON_SIMPLE,
  L7_RIP_SPLITHORIZON_POISONREVERSE,
  L7_RIP_SPLITHORIZON_LAST                /* Should be the last entry */
} L7_RIP_SPLITHORIZON_t;

/* config parameter min/max/default values */
#define L7_RIP_CONF_METRIC_MIN                  1
#define L7_RIP_CONF_METRIC_MAX                  15


 /*--------------------------------------*/
/* OSPF Constants                       */
/*--------------------------------------*/

#define L7_OSPF_REDIST_TAG_MIN      0
#define L7_OSPF_REDIST_TAG_MAX      4294967295UL

#define L7_OSPF_DEFAULT_RT_ADVERTISE_METRIC_MIN      0
#define L7_OSPF_DEFAULT_RT_ADVERTISE_METRIC_MAX      16777214

/*--------------------------------------*/
/* BGP Constants                        */
/*--------------------------------------*/

#define L7_BGP4_DEFAULT_METRIC_MIN  0
#define L7_BGP4_DEFAULT_METRIC_MAX  4294967295UL

#define L7_BGP4_REDIST_METRIC_MIN   0
#define L7_BGP4_REDIST_METRIC_MAX   4294967295UL

/**************************************
*
*     VRRP DEFINES
*
***************************************/

/*--------------------------------------*/
/*  Authentication Constants            */
/*--------------------------------------*/

#define L7_VRRP_MAX_AUTH_DATA        8

/*--------------------------------------*/
/*  VRRP Constants                       */
/*--------------------------------------*/

/*------------------*/
/* VRRP TRAP TYPES  */
/*------------------*/

/* The following flags are to be used by user-interface routines to
   enable/disable or get the mode of the specified VRRP trap */
#define L7_VRRP_TRAP_ALL                              0x3
#define L7_VRRP_TRAP_NEW_MASTER                       0x1
#define L7_VRRP_TRAP_AUTH_FAILURE                     0x2


/**************************************
*
*     BootP/DHCP Relay DEFINES
*
***************************************/

/*------------------------------------*/
/* BootP/DHCP Relay Parameter Limits  */
/*------------------------------------*/

#define L7_DHCP_MAX_HOP_COUNT       16
#define L7_DHCP_MIN_HOP_COUNT       1
#define L7_DHCP_MAX_WAIT_TIME       100
#define L7_DHCP_MIN_WAIT_TIME       0

#define L7_RTR_DEFAULT_BOOTP_DHCP_RELAY_SERVER "0.0.0.0"

/* Max number of IP helper entries */
#define L7_IP_HELPER_ENTRIES_MAX 512

#define L7_IP_HELPER_MIN_PORT       1
#define L7_IP_HELPER_MAX_PORT   65535


/* wildcard for IP helper ingress interface */
#define IH_INTF_ANY 0

/* wildcard for IP helper destination UDP port number */
#define IH_UDP_PORT_DEFAULT 0

/* Max length of a UDP port name */
#define IH_UDP_PORT_NAME_LEN 64

/* UDP Relay Constants */

#define L7_UDP_PORT_DHCP              67
#define L7_UDP_PORT_DOMAIN            53
#define L7_UDP_PORT_ISAKMP           500
#define L7_UDP_PORT_MOBILEIP         434
#define L7_UDP_PORT_NAMESERVER        42
#define L7_UDP_PORT_NETBIOS_DGM      138
#define L7_UDP_PORT_NETBIOS_NS       137
#define L7_UDP_PORT_NTP              123
#define L7_UDP_PORT_PIM_AUTO_RP      496
#define L7_UDP_PORT_RIP              520
#define L7_UDP_PORT_TACACS            49
#define L7_UDP_PORT_TFTP              69
#define L7_UDP_PORT_TIME              37



/*------------------------------*/
/* BootP/DHCP Relay TRAP TYPES  */
/*------------------------------*/

/* The following flags are to be used by user-interface routines to
   enable/disable or get the mode of the specified BootP/DHCP Relay trap */
#define L7_RELAY_TRAP_ALL                               0x07
#define L7_RELAY_TRAP_FORWARD_MODE                      0x01
#define L7_RELAY_TRAP_CIRCUITID_OPTION_MODE             0x02
#define L7_RELAY_TRAP_REMOTEID_OPTION_MODE              0x04


/* Virtual router. This version of FASTPATH has no virtual router support. These
 * defines are retained because they are referenced by ping and traceroute code. */
#define L7_VR_MAX_VIRTUAL_ROUTERS 1
#define L7_VALID_VRID 0
 
/* A way to indicate vrid is out-of-range */
#define L7_VR_NO_VRID 0xFFFFFFFF

/**************************************
*
*     IANA standard port DEFINES
*     http://www.iana.org/assignments/port-numbers
*
***************************************/
#define L7_IANA_IP_PORT_MIN                0
#define L7_IANA_IP_PORT_MAX                65535
#define L7_IANA_IP_PORT_WELL_KNOWN_MIN     0
#define L7_IANA_IP_PORT_WELL_KNOWN_MAX     1023
#define L7_IANA_IP_PORT_REGISTERED_MIN     1024
#define L7_IANA_IP_PORT_REGISTERED_MAX     49151
#define L7_IANA_IP_PORT_PRIVATE_MIN        49152
#define L7_IANA_IP_PORT_PRIVATE_MAX        65535


#endif /* L3_COMMDEFS_H */
