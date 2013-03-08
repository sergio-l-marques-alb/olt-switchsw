/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   ip_config.h
*
* @purpose    Ip Mapping layer structures and config data
*
* @component  Ip Mapping Layer
*
* @comments   none
*
* @create     03/12/2001
*
* @author     asuthan
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#ifndef INCLUDE_IP_CONFIG_H
#define INCLUDE_IP_CONFIG_H

#include "l7_common.h"
#include "l3_defaultconfig.h"
#include "l3_default_cnfgr.h"
#include "nimapi.h"
#include "acl_exports.h"


/****************************************
*
*  IP Configuration Data
*
*****************************************/


typedef struct
{
  L7_uint32           ipSourceAddrCheck;           /* Enable Source IP Address Check       */
  L7_uint32           ipSpoofingCheck;             /* Enable IP Spoofing Check             */
  L7_uint32           ipDefaultTTL;                /* Ip Default time to live */
  L7_uint32           ipArpAgeTime;                /* ARP entry ageout time */
  L7_uint32           ipArpRespTime;               /* ARP request response timeout */
  L7_uint32           ipArpRetries;                /* number of ARP resolution retries */
  L7_uint32           ipArpCacheSize;              /* max number of ARP cache entries */
  L7_uint32           ipArpDynamicRenew;           /* ARP dynamic entry renew mode */
  L7_uint32           route_preferences[L7_ROUTE_MAX_PREFERENCES]; /* Preferences values for protocols */

} L7_ipCfgData_t;



/****************************************
*
*  Router Configuration Data
*
*****************************************/

typedef struct
{
  L7_uint32           rtrAdminMode;                 /* Enable the router entity             */
  L7_uint32           rtrTOSForwarding;             /* Enable Forwarding based on TOS       */
  L7_uint32           rtrICMPRedirectMode;          /* Enable sending of ICMP Redirects     */
  L7_uint32           rtrICMPEchoReplyMode;     /* Enable Ignoring ICMP ECHO Requests   */
  L7_uint32           rtrTraceMode;                 /* Enable tracing of events             */

  /* Configures ICMP Rate limit parameters */
  L7_uint32           rtrICMPRatelimitBurstSize; 
  L7_uint32           rtrICMPRatelimitInterval;

} L7_rtrCfgData_t;


/****************************************
*
*  Router Circuit Configuration Data
*
*****************************************/


typedef struct L7_rtrCfgCkt_s
{
  nimConfigID_t       configId;
  L7_rtrIntfIpAddr_t  addrs[L7_L3_NUM_IP_ADDRS];
  L7_uint32           flags;                    /* Functions enabled            */

  /* IP MTU. 0 means "not configured" */
  L7_uint32           ipMtu;                    

  L7_uint32           icmpUnreachablesMode;     /* Enable sending of ICMP Unreachables    */
  L7_uint32           icmpRedirectsMode;
  L7_uint32           bandwidth;
  #define L7_RTR_INTF_PRESENT                       0x01  /* USP acts as a rtr interface  */
  #define L7_RTR_INTF_ADMIN_MODE_ENABLE             0x02  /* Routing enabled on this intf */
  #define L7_RTR_INTF_MCAST_FWD_MODE_ENABLE         0x04  /* Enable multicast forwarding  */
  #define L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE      0x08  /* Fwd Net Directed Bcasts */

  /* Corresponding bit is set if proxy ARP is enabled on this interface.
   * Note that this bit is only set in the config data structure. It is
   * not set in the "operational" array, rtrIntf.
   */
  #define L7_RTR_INTF_PROXY_ARP                     0x10
  
  /* Corresponding flag bit is set if gratuitous ARP is enabled on this
   * interface. Note that this bit is only set in the config data structure.
   * It is not set in the "operational" array, rtrIntf.
   */
  #define L7_RTR_INTF_GRAT_ARP                      0x20
  
  /* Corresponding bit is set if proxy ARP is enabled on this interface.
   * Note that this bit is only set in the config data structure. It is
   * not set in the "operational" array, rtrIntf.
   */
  #define L7_RTR_INTF_LOCAL_PROXY_ARP                     0x40

  /* Set if interface is unnumbered */
  #define L7_RTR_INTF_UNNUMBERED 0x80

  /* Set if interface IP Address method is configured for DHCP */
  #define L7_RTR_INTF_ADDR_METHOD_DHCP  0x100

  /* Set this flag when the Interface is enabled as a Host.
   * Reset it when the Interface is disabled.
   * The parameters that DTL is interested in are updated to DTL only when
   * this flag is not set.  The Interface remembers this flag and uses to 
   * unset the parameters in DTL when this flag is not set.
   */
  #define L7_RTR_INTF_HOST  0x200

  /* If interface is unnumbered, this points to the interface whose 
   * address is borrowed. It is the interface identified in the 
   * "ip unnumbered" command. Undefined on numbered interfaces. */
  nimConfigID_t numberedIfc;

} L7_rtrCfgCkt_t;


/****************************************
*
*  Static Routes Configuration Data
*
*****************************************/

#define L7_RTR_MAX_STATIC_ROUTES        FD_RTR_MAX_STATIC_ROUTES

typedef struct L7_rtrStaticRouteNhCfgData_s
{
  L7_uint32           nextHopRtr;
  nimConfigID_t       intfConfigId;
} L7_rtrStaticRouteNhCfgData_t;

typedef struct L7_rtrStaticRouteCfgData_s
{
  L7_BOOL             inUse;          /* Keeps track if this entry is in use */
  L7_uint32           ipAddr;         /* Destination prefix                  */
  L7_uint32           ipMask;         /* Destination mask                    */

  /* Array of next hops. A value of 0 in nextHopRtr of nextHops indicates end 
   * of the list. That is, the array has no holes. */
  L7_rtrStaticRouteNhCfgData_t nextHops[L7_RT_MAX_EQUAL_COST_ROUTES];    

  /* Preference (administrative distance) of this route. Used to select
   * among routes to the same destination from different sources. */
  L7_uint32           preference;

  L7_uint32           flags;
#ifndef L7_RT_DEFAULT_ROUTE
#define L7_RT_DEFAULT_ROUTE     0x01
#endif
#ifndef L7_RT_GLOBAL_DEFAULT_GATEWAY
#define L7_RT_GLOBAL_DEFAULT_GATEWAY     0x02 /* Flag to indicate that the default 
                                                 route is the Global Default Gateway */
#endif
} L7_rtrStaticRouteCfgData_t;


typedef struct L7_rtrStaticRouteCfg_s
{
  L7_fileHdr_t                cfgHdr;
  L7_rtrStaticRouteCfgData_t  rtrStaticRouteCfgData[L7_RTR_MAX_STATIC_ROUTES];
  L7_uint32                   checkSum;

}L7_rtrStaticRouteCfg_t;


/****************************************
*
*  Static Arp Configuration Data
*
*****************************************/

/* this is the active definition based on the platform maximum */
#define L7_RTR_MAX_STATIC_ARP_ENTRIES   L7_IP_ARP_CACHE_STATIC_MAX

typedef struct L7_rtrStaticArpCfgData_s
{
  L7_IP_ADDR_t        ipAddr;                   /* IP Address                   */
  L7_linkLayerAddr_t  macAddr;                  /* MAC address                  */
  nimConfigID_t       intfConfigId;             /* Interface configured */
} L7_rtrStaticArpCfgData_t;

typedef struct L7_rtrStaticArpCfg_s
{
  L7_fileHdr_t              cfgHdr;
  /* NOTE: The following array MUST use the factory default max value */
  L7_rtrStaticArpCfgData_t  rtrStaticArpCfgData[FD_RTR_MAX_STATIC_ARP_ENTRIES];
  L7_uint32                 checkSum;

} L7_rtrStaticArpCfg_t;


/****************************************
*
*  Router Configuration File
*
*****************************************/

#define L7_IP_CFG_VER_9      0x9               /* Rel L */
#define L7_IP_CFG_VER_CURRENT L7_IP_CFG_VER_9

#define L7_IP_STATIC_ROUTES_CFG_VER_6           0x6     /* Rel L */
#define L7_IP_STATIC_ROUTES_CFG_VER_CURRENT     L7_IP_STATIC_ROUTES_CFG_VER_6

#define L7_IP_STATIC_ARP_CFG_VER_3              0x3     /* Rel L */
#define L7_IP_STATIC_ARP_CFG_VER_CURRENT        L7_IP_STATIC_ARP_CFG_VER_3

#define L7_IP_MAX_FUNC_NAME                     20

typedef struct L7_ipMapCfg_s
{
  L7_fileHdr_t            cfgHdr;
  L7_ipCfgData_t          ip;
  L7_rtrCfgData_t         rtr;
  L7_rtrCfgCkt_t          ckt[L7_IPMAP_INTF_MAX_COUNT];
  L7_uint32               checkSum;

}L7_ipMapCfg_t;

typedef struct ipMapDebugIcmpCtlStats_s
{
  L7_uint32 totalRateLimitedPkts;
  L7_uint32 totalSuppressedRedirects;
  L7_uint32 rateLimitKicks;
}ipMapDebugIcmpCtlStats_t;

#ifdef L7_QOS_FLEX_PACKAGE_ACL

#define IPMAP_DEBUG_CFG_FILENAME "ipmap_debug.cfg"
#define IPMAP_DEBUG_CFG_VER_1          0x1
#define IPMAP_DEBUG_CFG_VER_CURRENT   IPMAP_DEBUG_CFG_VER_1

typedef struct ipMapDebugCfgData_s
{
  L7_uint32 aclNumTrace[L7_MAX_ACL_ID];
  
} ipMapDebugCfgData_t;

typedef struct ipMapDebugCfg_s
{
  L7_fileHdr_t          hdr;
  ipMapDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} ipMapDebugCfg_t;
#endif


#endif /* INCLUDE_IP_CONFIG_H */

