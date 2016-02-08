/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l3_mcast_commdefs.h
*
* @purpose   Common defines, enums and data structures for packets
*
* @component
*
* @comments  none
*
* @create    03/14/2002
*
* @author    vidhumouli H / M Pavan K Chakravarthi
*
* @end
*
**********************************************************************/


#ifndef L3_MCAST_COMMDEFS_H
#define L3_MCAST_COMMDEFS_H

#include <l7_common.h>
#include <l3_addrdefs.h>
#include <async_event_api.h>
#include <l3_default_cnfgr.h>

#ifndef L7_VOIDPTR
#define L7_VOIDPTR void*
#endif

typedef void*  MCAST_CB_HNDL_t;



/*--------------------------------------*/
/*  PIMDM Constants                       */
/*--------------------------------------*/

#define L7_PIMDM_HELLO_INTERVAL               30

#define L7_PIMDM_DEFAULT_OVERRIDEINTERVAL    2500 /* in milliseconds */
#define L7_PIMDM_DEFAULT_LANPRUNEDELAY       500  /* in milliseconds */
#define L7_PIMDM_DEFAULT_STATEREFRESHINTERVAL 60  /* in seconds */


/*--------------------------------------*/
/*  PIMDM TRAP TYPES                       */
/*--------------------------------------*/

/* The following flags are to be used by user-interface routines to
   enable/disable or get the mode of the specified PIM trap -shareef*/
#define L7_PIM_TRAP_ALL                                 0x01
#define L7_PIM_TRAP_NEIGHBOR_LOSS                       0x01

/* Common PIM  constants for PIMSM and PIMDM */
#define L7_PIM_INTERFACE_HELLO_INTERVAL_MIN      0
#define L7_PIM_INTERFACE_HELLO_INTERVAL_MAX      65535
/*--------------------------------------*/
/*  DVMRP Constants                       */
/*--------------------------------------*/
 
typedef enum
{
    L7_DVMRP_CAPABILITIES_LEAF = 0,
    L7_DVMRP_CAPABILITIES_PRUNE,
    L7_DVMRP_CAPABILITIES_GENID,
    L7_DVMRP_CAPABILITIES_MTRACE
} L7_DVMRP_CAPABILITIES_t;

typedef enum
{
    L7_DVMRP_NEIGHBOR_STATE_ONEWAY  = 1,
    L7_DVMRP_NEIGHBOR_STATE_ACTIVE,
    L7_DVMRP_NEIGHBOR_STATE_IGNORING,
    L7_DVMRP_NEIGHBOR_STATE_DOWN
} L7_DVMRP_NEIGHBOR_STATE_t;

typedef enum
{
    L7_DVMRP_NEXTHOP_TYPE_LEAF = 1,
    L7_DVMRP_NEXTHOP_TYPE_BRANCH
} L7_DVMRP_NEXTHOP_TYPE_t;

/*--------------------------------------*/
/*  DVMRP TRAP TYPES                    */
/*--------------------------------------*/

#define L7_DVMRP_TRAP_NEIGHBOR_LOSS           0x01
#define L7_DVMRP_TRAP_NEIGHBOR_NOT_PRUNING    0x02
#define L7_DVMRP_TRAP_ALL                     (L7_DVMRP_TRAP_NEIGHBOR_LOSS|L7_DVMRP_TRAP_NEIGHBOR_NOT_PRUNING)

  /*------------------*/
/* DVMRP RANGE SIZES */
/*------------------*/

#define L7_DVMRP_INTERFACE_METRIC_MIN 1
#define L7_DVMRP_INTERFACE_METRIC_MAX 31


/*------------------*/
/*   IGMP defines   */
/*------------------*/

/*------------------*/
/* IGMP RANGE SIZES */
/*------------------*/

#define L7_IGMP_VERSION_MIN 1
#define L7_IGMP_VERSION_MAX 3
#define L7_IGMP_RESPONSE_TIME_MIN 0
#define L7_IGMP_RESPONSE_TIME_MAX 255
#define L7_IGMP_QUERY_INTERVAL_MIN 1
#define L7_IGMP_QUERY_INTERVAL_MAX 3600
#define L7_IGMP_ROBUST_MIN 1
#define L7_IGMP_ROBUST_MAX 255
#define L7_IGMP_STARTUP_QUERY_INTERVAL_MIN 1
#define L7_IGMP_STARTUP_QUERY_INTERVAL_MAX 300
#define L7_IGMP_STARTUP_QUERY_COUNT_MIN 1
#define L7_IGMP_STARTUP_QUERY_COUNT_MAX 20
#define L7_IGMP_LAST_MEM_QUERY_COUNT_MIN 1
#define L7_IGMP_LAST_MEM_QUERY_COUNT_MAX 20
#define L7_IGMP_LAST_MEM_QUERY_INTERVAL_MIN 0
#define L7_IGMP_LAST_MEM_QUERY_INTERVAL_MAX 255
#define L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MIN 1
#define L7_IGMP_UNSOLICITED_REPORT_INTERVAL_MAX 260

#define L7_MLD_UNSOLICITED_REPORT_INTERVAL_MIN 1
#define L7_MLD_UNSOLICITED_REPORT_INTERVAL_MAX 260

#define L7_PIMDM_HELLOINTERVAL_MIN  L7_PIM_INTERFACE_HELLO_INTERVAL_MIN
#define L7_PIMDM_HELLOINTERVAL_MAX  L7_PIM_INTERFACE_HELLO_INTERVAL_MAX

#define L7_PIMDM6_HELLOINTERVAL_MIN  L7_PIM_INTERFACE_HELLO_INTERVAL_MIN
#define L7_PIMDM6_HELLOINTERVAL_MAX  L7_PIM_INTERFACE_HELLO_INTERVAL_MAX

#define L7_ENTRY_FREE       1
#define L7_ENTRY_INUSE      2
#define L7_ENTRY_DELETED    3

/*------------------*/
/* MLD RANGE SIZES */
/*------------------*/
#define L7_MLD_VERSION_MIN 1
#define L7_MLD_VERSION_MAX 2
#define L7_MLD_QUERY_INTERVAL_MIN          1
#define L7_MLD_QUERY_INTERVAL_MAX          3600
#define L7_MLD_RESPONSE_TIME_MIN           0
#define L7_MLD_RESPONSE_TIME_MAX           65535
#define L7_MLD_ROBUST_MIN                  1
#define L7_MLD_ROBUST_MAX                  255
#define L7_MLD_STARTUP_QUERY_INTERVAL_MIN 1
#define L7_MLD_STARTUP_QUERY_INTERVAL_MAX 300
#define L7_MLD_STARTUP_QUERY_COUNT_MIN    1
#define L7_MLD_STARTUP_QUERY_COUNT_MAX    20
#define L7_MLD_LAST_MEM_QUERY_INTERVAL_MIN 0
#define L7_MLD_LAST_MEM_QUERY_INTERVAL_MAX 65535
#define L7_MLD_LAST_MEM_QUERY_COUNT_MIN    1
#define L7_MLD_LAST_MEM_QUERY_COUNT_MAX    20

#define MLD_RESP_TIME_FACTOR               1000
/*------------------*/
/*   MCAST defines   */
/*------------------*/
/*------------------*/
/*MCAST RANGE SIZES */
/*------------------*/

#define L7_MCAST_TTL_THRESHOLD_MIN  0
#define L7_MCAST_TTL_THRESHOLD_MAX  255

#define L7_MCAST_STATICROUTE_METRIC_MIN  1 
#define L7_MCAST_STATICROUTE_METRIC_MAX  255

#define L7_MCAST_PIMSM_FLAG_RPT          0
#define L7_MCAST_PIMSM_FLAG_SPT          1

#define L7_MCAST_PIMSM_RP_STATIC          1  /*this values should be same as the values in the rp_type_t enum*/
#define L7_MCAST_PIMSM_RP_DYNAMIC         2  /*in the file "src/application/ip_mcast/vendor/pimsm/pimsmNew.h"*/

#define L7_MCAST_NBR_TUNNEL          0x01    /* neighbors reached via tunnel */
#define L7_MCAST_IP_SRCRT            0x02    /* tunnel uses IP source routing */
#define L7_MCAST_NBR_PIM             0x04    /* neighbor is a PIM neighbor */
#define L7_MCAST_INTF_DOWN           0x10    /* kernel state of interface */
#define L7_MCAST_ADMIN_DISABLED      0x20    /* administratively disabled */
#define L7_MCAST_QUERIER             0x40    /* I am the subnet's querier */
#define L7_MCAST_NBR_LEAF            0x80    /* Neighbor reports that it is a leaf */

#define L7_MCAST_NF_LEAF     0x0001   /* This neighbor is a prune */
#define L7_MCAST_NF_PRUNE    0x0002   /* This neighbor understands prunes */
#define L7_MCAST_NF_GENID    0x0004   /* I supply genId & rtrlist in probe */
#define L7_MCAST_NF_SNMP     0x0010   /* I respond to SNMP Queries */

/* Maximum number of addresses that may be configured on a routing
** interface (including primary and secondary). Note not all interfaces can
** have this many, also limited by secondary address pool.
*/
#define L7_MCAST_MAX_INTF_ADDRS  L7_RTR6_MAX_INTF_ADDRS
typedef enum
{
  L7_MCAST_IANA_MROUTE_UNASSIGNED = 0,    /*   -- none of the following*/
  L7_MCAST_IANA_MROUTE_OTHER      = 1,    /*        -- none of the following*/
  L7_MCAST_IANA_MROUTE_LOCAL      = 2,    /*       -- e.g., manually configured */
  L7_MCAST_IANA_MROUTE_NETMGMT    = 3,    /*    -- set via net.mgmt protocol */
  L7_MCAST_IANA_MROUTE_DVMRP      = 4,
  L7_MCAST_IANA_MROUTE_MOSPF      = 5,
  L7_MCAST_IANA_MROUTE_PIM_SM_DM  = 6, /* -- PIMv1, both DM and SM */
  L7_MCAST_IANA_MROUTE_CBT        = 7,
  L7_MCAST_IANA_MROUTE_PIM_SM     = 8,  /* -- PIM-SM */
  L7_MCAST_IANA_MROUTE_PIM_DM     = 9,  /*  -- PIM-DM */
  L7_MCAST_IANA_MROUTE_IGMP       = 10,
  L7_MCAST_IANA_MROUTE_BMGP       = 11,
  L7_MCAST_IANA_MROUTE_MSDP       = 12,
  L7_MCAST_IANA_MROUTE_IGMP_PROXY = 13
} L7_MCAST_IANA_PROTO_ID_t;



#define L7_MCAST_ADMINSCOPE_ADDR_MIN       0xef000000
#define L7_MCAST_ADMINSCOPE_ADDR_MAX       0xefffffff

/* IP Mcast Zone Table from RFC 5132.
 * Since we do not support multiple Scopes, there will always be a 
 * default Scope Zone.
 * For now, we segregate the zones as IPv4 zone and IPv6 zone.
 * Change the below definitions when RFC 5132 is implemented.
 */
#define L7_MCAST_ZONE_SCOPE_DEFAULT_ZONE_IPV4   L7_AF_INET
#define L7_MCAST_ZONE_SCOPE_DEFAULT_ZONE_IPV6   L7_AF_INET6

#define L7_MCAST_DEFAULT_IP           "0.0.0.0"
#define L7_MCAST_GROUP_ADDR_MIN       0xe0000000
#define L7_MCAST_GROUP_ADDR_MAX       0xefffffff


/*--------------------------------------*/
/*  PIMSM Constants                       */
/*--------------------------------------*/

#define L7_PIMSM_INTERFACE_JOIN_PRUNE_INTERVAL     60
#define L7_PIMSM_DATA_THRESHOLD_RATE               0   /* Kbits/sec */
#define L7_PIMSM_REG_THRESHOLD_RATE                0   /* Kbits/sec */
#define L7_PIMSM_INTERFACE_HELLO_INTERVAL          30
#define L7_PIMSM_CBSR_PRIORITY         0
#define L7_PIMSM_CBSR_HASH_MASK_LENGTH   30
#define L7_PIMSM6_CBSR_HASH_MASK_LENGTH   126
#define L7_PIMSM_REGISTER_INTF                     0
#define L7_PIMSM_INTERFACE_DR_PRIORITY             1
#define L7_PIMSM_RP_GRP_MAPPING_ORIGIN_TYPE_BSR    3

/*------------------*/
/*PIMSM RANGE SIZES */
/*------------------*/
#if 1 /* remove, once web and cli are clean*/
#define L7_PIMSM_JOINPRUNE_INTERVAL_MIN                     0
#define L7_PIMSM_JOINPRUNE_INTERVAL_MAX                 18000
#define L7_PIMSM_INTERFACE_CRP_PREFERENCE_MIN              -1
#define L7_PIMSM_INTERFACE_CRP_PREFERENCE_MAX             255
#define L7_PIMSM6_INTERFACE_CRP_PREFERENCE_MIN              -1
#define L7_PIMSM6_INTERFACE_CRP_PREFERENCE_MAX             255
#endif

#define L7_PIMSM_DATATHRESHOLD_RATE_MIN                     0
#define L7_PIMSM_DATATHRESHOLD_RATE_MAX                  2000
#define L7_PIMSM_REGTHRESHOLD_RATE_MIN                      0
#define L7_PIMSM_REGTHRESHOLD_RATE_MAX                   2000
#define L7_PIMSM_INTERFACE_HELLO_INTERVAL_MIN               L7_PIM_INTERFACE_HELLO_INTERVAL_MIN
#define L7_PIMSM_INTERFACE_HELLO_INTERVAL_MAX               L7_PIM_INTERFACE_HELLO_INTERVAL_MAX
#define L7_PIMSM_INTERFACE_JOINPRUNE_INTERVAL_MIN           0
#define L7_PIMSM_INTERFACE_JOINPRUNE_INTERVAL_MAX       18000
#define L7_PIMSM_INTERFACE_CBSR_PREFERENCE_MIN              0
#define L7_PIMSM_INTERFACE_CBSR_PREFERENCE_MAX            255

#define L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN        0
#define L7_PIMSM_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX       32

#define L7_PIMSM_INTERFACE_DR_PRIORITY_MIN                0
#define L7_PIMSM_INTERFACE_DR_PRIORITY_MAX                2147483647

#define L7_PIMSM6_INTERFACE_DR_PRIORITY_MIN                  0
/* Compiler Dependent. Not accepting such a big number.
#define L7_PIMSM6_INTERFACE_DR_PRIORITY_MAX         4294967294*/
 

#define L7_PIMSM6_REGTHRESHOLD_RATE_MIN                      0
#define L7_PIMSM6_REGTHRESHOLD_RATE_MAX                   2000

#define L7_PIMSM6_DATATHRESHOLD_RATE_MIN                     0
#define L7_PIMSM6_DATATHRESHOLD_RATE_MAX                  2000

#define L7_PIMSM6_JOINPRUNE_INTERVAL_MIN                     0
#define L7_PIMSM6_JOINPRUNE_INTERVAL_MAX                 18000

#define L7_PIMSM6_INTERFACE_HELLO_INTERVAL_MIN           L7_PIM_INTERFACE_HELLO_INTERVAL_MIN
#define L7_PIMSM6_INTERFACE_HELLO_INTERVAL_MAX           L7_PIM_INTERFACE_HELLO_INTERVAL_MAX

#define L7_PIMSM6_INTERFACE_CBSR_SCOPE_MIN                    3
#define L7_PIMSM6_INTERFACE_CBSR_SCOPE_MAX               L7_PIMSM_MAX_PER_SCOPE_BSR_NODES

#define L7_PIMSM6_INTERFACE_CBSR_PREFERENCE_MIN              0
#define L7_PIMSM6_INTERFACE_CBSR_PREFERENCE_MAX            255

#define L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MIN        0
#define L7_PIMSM6_INTERFACE_CBSR_HASH_MASK_LENGTH_MAX       128



#define L7_PIMSM_PRUNEREASON_OTHER                    1
#define L7_PIMSM_PRUNEREASON_PRUNE                    2
#define L7_PIMSM_PRUNEREASON_ASSERT                   3


/*THIS ENTRY ADDED TO SUPPORT SNMP*/
#define L7_MCAST_MROUTE_RTPROTO_DVMRP 17

  /* INTERFACE MODE TYPES */
typedef enum
{
  MGMD_INTERFACE_MODE_NULL=0,
  MGMD_ROUTER_INTERFACE=1,
  MGMD_PROXY_INTERFACE=2
}L7_MGMD_INTF_MODE_t;

/* constansts used by MRPs to return for parameter 'routeType' 
   in mcast mroute table in multicast mib 2932  */
#define L7_UNICAST_ROUTE     1
#define L7_MULTICAST_ROUTE   2

/*------------------*/
/*   Error Codes */
/*------------------*/
typedef enum
{
  L7_MGMD_ERR_NONE = 0,
  L7_MGMD_ERR_INVALID_INPUT,
  L7_MGMD_ERR_NOT_MGMD_INTF,
  L7_MGMD_ERR_NOT_PROXY_INTF,
  L7_MGMD_PROXY_ALREADY_CONFIGURED,
  L7_MGMD_ERR_REQUEST_FAILED,
  L7_MGMD_ERR_MGMD_INTF,
  L7_MGMD_ERR_PROXY_INTF,
  L7_MGMD_ERR_OTHER_MCAST_ROUTING_PROTOCOL_CONFIGURED,
  L7_MGMD_ERR_MCAST_FWD_DISABLED,
  L7_MGMD_ERR_MGMD_DISABLED_ON_SYSTEM,
  L7_MGMD_ERR_ROUTING_NOT_ENABLED,
}L7_MGMD_ERROR_CODE_t;

typedef enum
{
  L7_MRP_UNKNOWN = 0,
  L7_MRP_DVMRP,
  L7_MRP_PIMDM,
  L7_MRP_PIMSM,
  L7_MRP_MGMD_PROXY,
  L7_MRP_MAXIMUM
}L7_MRP_TYPE_t;

typedef enum
{
  L7_IPV6_PKT_RCVR_ID_MFC = 0,
  L7_IPV6_PKT_RCVR_ID_PIMSM,
  L7_IPV6_PKT_RCVR_ID_PIMDM,
  L7_IPV6_PKT_RCVR_ID_MGMD,
  L7_IPV6_PKT_RCVR_ID_MAX /* should be last element */
}L7_IPV6_PKT_RCVR_ID_TYPE_t;

/* PIM Mode Definition */
typedef enum
{
   PIM_MODE_NONE =1,
   PIM_MODE_SSM,
   PIM_MODE_ASM,
   PIM_MODE_BIDIR,
   PIM_MODE_OTHER
}pimMode_t;

/***********  Events across  MCAST COMPONENTS  ***************/
typedef enum
{

  MCAST_EVENT_MIN = 0,
/********************************************************
*                 Events from MFC to MRPs               *
********************************************************/

  /* To Multicast Routing Protocols */
  MCAST_MFC_NOCACHE_EVENT = 1,
  MCAST_MFC_WRONGIF_EVENT,
  MCAST_MFC_ENTRY_EXPIRE_EVENT,
  /* To PIM-SM Vendor code */
  MCAST_MFC_WHOLEPKT_EVENT,

/********************************************************
*                 Events from MCASTMAP to MRPs          *
********************************************************/

  /* To PIM-DM, DVMRP Vendor Code */
  MCAST_EVENT_ADMINSCOPE_BOUNDARY,

  /* To PIM-SM Vendor code */
  MCAST_EVENT_PIMSM_CONTROL_PKT_RECV,

  /* To PIM-DM Vendor code */
  MCAST_EVENT_PIMDM_CONTROL_PKT_RECV,

  /* To IGMP/MLD Vendor code */
  MCAST_EVENT_IGMP_CONTROL_PKT_RECV,
  MCAST_EVENT_MLD_CTRL_PKT_RECV,

  /* To DVMRP Vendor code */
  MCAST_EVENT_DVMRP_CONTROL_PKT_RECV,

  /* To Multicast Routing Protocols */
  MCAST_EVENT_IPv6_CONTROL_PKT_RECV,

  /* To PIM-DM, PIM-SM Vendor Code */
  MCAST_EVENT_STATIC_MROUTE_CHANGE,

/********************************************************
*                 Events from RTO                       *
********************************************************/

  /* To Multicast Routing Protocols */
  MCAST_EVENT_RTO_BEST_ROUTE_CHANGE,

/********************************************************
*                 Events from APPTIMER to PIMSM         *
********************************************************/

  /* To Multicast Routing Protocols */
  MCAST_EVENT_PIMSM_TIMER_EXPIRY,
  MCAST_EVENT_PIMDM_TIMER_EXPIRY,
  MCAST_DVMRP_TIMER_EXPIRY_EVENT,

/********************************************************
*                 Events from IGMP/MLD to MRPs          *
********************************************************/
  /* To Multicast Routing Protocols */
  MCAST_EVENT_MGMD_GROUP_ADD,
  MCAST_EVENT_MGMD_GROUP_DELETE,
  MCAST_EVENT_MGMD_GROUP_UPDATE,
  MCAST_EVENT_MGMD_TIMER_EXPIRY_EVENT,

/********************************************************
*                 Events from MRPs to MGMD              *
********************************************************/
  /* To MGMD */
  MCAST_EVENT_MGMD_GROUP_INFO_GET,

/********************************************************
*                 Events from MRPs to Mcastmap              *
********************************************************/

  MCAST_EVENT_ADMIN_SCOPE_INFO_GET,

  MCAST_EVENT_MAX

} mcastEventTypes_t;


#define MCAST_APPTIMER_QUEUE_MSG_COUNT 3
#define MCAST_STRING_SIZE_MAX 256

#endif /*L3_MCAST_COMMDEFS_H*/
