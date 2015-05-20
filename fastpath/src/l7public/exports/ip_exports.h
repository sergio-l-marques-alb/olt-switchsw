/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename ip_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __IP_EXPORTS_H_
#define __IP_EXPORTS_H_


#define L7_OSPFV3 1

#ifndef L7_OSPFV3
#define L7_FLEX_OSPFV3_MAP_COMPONENT_ID       L7_OSPF_MAP_COMPONENT_ID
#define L7_OSPFV3_MAP_COMPONENT_MNEMONIC L7_OSPF_MAP_COMPONENT_MNEMONIC
#define L7_OSPFV3_MAP_COMPONENT_NAME     L7_OSPF_MAP_COMPONENT_NAME
#endif

#define L7_NULL_IP_ADDR 0
#define L7_NULL_IP_MASK 0


/* IP Address type - from RFC 2851 */
typedef enum
{
  L7_IP_ADDRESS_TYPE_UNKNOWN = 0,
  L7_IP_ADDRESS_TYPE_IPV4 = 1,
  L7_IP_ADDRESS_TYPE_IPV6 = 2,
  L7_IP_ADDRESS_TYPE_DNS = 16
} L7_IP_ADDRESS_TYPE_t;


/*--------------------------------------*/
/*  RTR DISC Constants                  */
/*--------------------------------------*/
/* Router Constants */
#define MAX_INITIAL_ADVERT_INTERVAL   16 /* seconds */
#define MAX_INITIAL_ADVERTISEMENTS    3  /* transmissions */
#define MAX_RESPONSE_DELAY        2  /* seconds */


#define L7_DEFAULT_ADV_ADDR     0xE0000001          /* 224.0.0.1 */
#define MAX_ADV_INTERVAL        600
#define ADVERTISEMENT           L7_FALSE
#define PREFERENCE_LEVEL        0


/*--------------------------------------*/
/*  RLIM Constants                      */
/*--------------------------------------*/
/*
 * Tunnel Mode values
 */
typedef enum
{
  L7_TUNNEL_MODE_UNDEFINED = 1,
  L7_TUNNEL_MODE_6OVER4,
  L7_TUNNEL_MODE_6TO4,
  L7_TUNNEL_MODE_ISATAP,
  L7_TUNNEL_MODE_IP6,
  L7_TUNNEL_MODE_4OVER4,
} L7_TUNNEL_MODE_t;

/*--------------------------------------*/
/*  Authentication Constants            */
/*--------------------------------------*/

#define L7_AUTH_MAX_KEY 255

typedef enum
{
  L7_AUTH_TYPE_NONE = 0,
  L7_AUTH_TYPE_SIMPLE_PASSWORD,
  L7_AUTH_TYPE_MD5

} L7_AUTH_TYPES_t;


/* IP MAP Component Feature List */
typedef enum
{
  L7_IPMAP_FEATURE_LAG_RTING_SUPPORT_FEATURE_ID = 0,
  L7_IPMAP_FEATURE_PROXY_ARP,
  L7_IPMAP_FEATURE_MULTINETTING,
  L7_IPMAP_FEATURE_REJECTROUTE,
  L7_IPMAP_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_IPMAP_FEATURE_IDS_t;

/*--------------------------------------*/
/*  IP AT Table Public Constants           */
/*--------------------------------------*/

typedef enum
{
  L7_IP_AT_TYPE_OTHER = 1,
  L7_IP_AT_TYPE_INVALID,
  L7_IP_AT_TYPE_DYNAMIC,
  L7_IP_AT_TYPE_STATIC
} L7_IP_AT_TYPE_t;

/*--------------------------------------*/
/*  ARP Constants                       */
/*--------------------------------------*/
#define L7_IP_ARP_MAX_QUERY             5             /* hard limit needed for DAPI */
/*********************************************************************
*
* L7_RT_BRIDGING_ROUTING_COEXISTANCE
*
* If L7_RT_BRIDGE_FIRST_THEN_ROUTE, all frames received on an interface
* which is enabled for bridging are first bridged, and then routed.
*
* If L7_RT_BRIDGE_OR_ROUTE, all routable frames are routed. All others 
* are bridged.
*
* The aforementioned choices are mutually exclusive.
* VLAN routing is available only when L7_RT_BRIDGE_FIRST_THEN_ROUTE is
* selected.
*
*
*********************************************************************/

typedef enum
{
  L7_RT_BRIDGE_FIRST_THEN_ROUTE = 1,
  L7_RT_BRIDGE_OR_ROUTE

} L7_RT_BRIDGING_ROUTING_COEXISTANCE_t;


#define L7_RT_BRIDGING_ROUTING_COEXISTANCE  L7_RT_BRIDGE_FIRST_THEN_ROUTE

#define L7_CIDR_MAX_SIZE                    4  /* size of L7_uint32 */

/*--------------------------*/
/* DAI specific constants */
/*--------------------------*/
#define L7_ARP_ACL_NAME_LEN_MIN              1
#define L7_ARP_ACL_NAME_LEN_MAX             31
#define L7_ARP_ACL_CMDS_MAX                 50
#define L7_ARP_ACL_RULES_MAX               512
#define L7_DAI_RATE_LIMIT_NONE              -1
#define L7_DAI_RATE_LIMIT_MIN                0
#define L7_DAI_RATE_LIMIT_MAX              300
#define L7_DAI_BURST_INTERVAL_MIN            1
#define L7_DAI_BURST_INTERVAL_MAX           15

/*--------------------------*/
/* Virtual Router constants */
/*--------------------------*/

#define L7_VR_MAX_NAME_LEN          32


/******************** conditional Override *****************************/

#ifdef INCLUDE_IP_EXPORTS_OVERRIDES
#include "ip_exports_overrides.h"
#endif

#endif /* __IP_EXPORTS_H_*/
