
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename ipmap_migrate.h
*
* @purpose IPMAP Configuration Migration
*
* @component IPMAP
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef IPMAP_MIGRATE_H
#define IPMAP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l3_defaultconfig.h"
#include "ip_config.h"
#include "ip_util.h"

#define L7_RTR_MAX_STATIC_ROUTES_REL_4_0 FD_RTR_MAX_STATIC_ROUTES_REL_4_0
#define L7_RTR_MAX_STATIC_ROUTES_REL_4_1 FD_RTR_MAX_STATIC_ROUTES_REL_4_1
#define L7_RTR_MAX_STATIC_ROUTES_REL_4_2 FD_RTR_MAX_STATIC_ROUTES_REL_4_2
#define L7_RTR_MAX_STATIC_ROUTES_REL_4_3 FD_RTR_MAX_STATIC_ROUTES_REL_4_3
#define L7_RTR_MAX_STATIC_ROUTES_REL_4_4 FD_RTR_MAX_STATIC_ROUTES_REL_4_4

#define L7_RTR_MAX_STATIC_ARP_ENTRIES_REL_4_4 FD_RTR_MAX_STATIC_ARP_ENTRIES_REL_4_4


typedef struct
{
  L7_uint32 ipSourceAddrCheck;
  L7_uint32 ipSpoofingCheck;
  L7_uint32 ipDefaultTTL;
  L7_uint32 ipArpAgeTime;
  L7_uint32 ipArpRespTime;
  L7_uint32 ipArpRetries;
  L7_uint32 ipArpCacheSize;
  L7_uint32 ipArpDynamicRenew;
  L7_uint32 route_preferences[L7_RTO_MAX_PROTOCOLS_REL_4_0];
} L7_ipCfgDataV4_t;

typedef struct
{
  L7_uint32 routerID;           /* Router ID                  */
  L7_uint32 rtrAdminMode;       /* Enable the router entity             */
  L7_uint32 rtrForwarding;      /* Enable Forwarding                    */
  L7_uint32 rtrTOSForwarding;   /* Enable Forwarding based on TOS       */
  L7_uint32 rtrICMPRedirectMode;        /* Enable sending of ICMP Redirects     */
  L7_uint32 rtrBootpDhcpRelayMode;      /* Enable BOOTP or DHCP Relay           */
  L7_IP_ADDR_t rtrBootpDhcpRelayServer; /* BOOTP or DHCP Relay Server           */
  L7_uint32 rtrProxyArpMode;    /* Enable Proxy ARP                     */
  L7_uint32 rtrGratuitousArpMode;       /* Enable Gratuitous ARP                */
  L7_uint32 rtrTraceMode;       /* Enable tracing of events             */
  L7_uint32 rtrEcmpMode;        /* Enable ECMP routing on the switch */
}
L7_rtrCfgData4_t;

typedef struct
{
  L7_ushort16 vlanID;           /* VLAN ID associated with circuit */
}
L7_rtrCktData4_t;

typedef struct
{
  L7_uint32 ipAddr;             /* IP Address                   */
  L7_uint32 ipMask;             /* IP Mask                      */
  L7_uint32 flags;              /* Functions enabled            */
  L7_rtrCktData4_t circuitData; /* Additional circuit-related data */
}
L7_rtrCfgCkt4_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_ipCfgDataV4_t ip;
  L7_rtrCfgData4_t rtr;
  L7_rtrCfgCkt4_t ckt[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;
}
L7_ipMapCfgV4_t;

typedef struct
{
  L7_uint32 ipSourceAddrCheck;
  L7_uint32 ipSpoofingCheck;
  L7_uint32 ipDefaultTTL;
  L7_uint32 ipArpAgeTime;
  L7_uint32 ipArpRespTime;
  L7_uint32 ipArpRetries;
  L7_uint32 ipArpCacheSize;
  L7_uint32 ipArpDynamicRenew;
  L7_uint32 route_preferences[L7_RTO_MAX_PROTOCOLS_REL_4_1];
} L7_ipCfgDataV5_t;

typedef struct
{
  L7_uint32 rtrAdminMode;       /* Enable the router entity             */
  L7_uint32 rtrForwarding;      /* Enable Forwarding                    */
  L7_uint32 rtrTOSForwarding;   /* Enable Forwarding based on TOS       */
  L7_uint32 rtrICMPRedirectMode;        /* Enable sending of ICMP Redirects     */
  L7_uint32 rtrBootpDhcpRelayMode;      /* Enable BOOTP or DHCP Relay           */
  L7_IP_ADDR_t rtrBootpDhcpRelayServer; /* BOOTP or DHCP Relay Server           */
  L7_uint32 rtrProxyArpMode;    /* Enable Proxy ARP                     */
  L7_uint32 rtrTraceMode;       /* Enable tracing of events             */
  L7_uint32 rtrEcmpMode;        /* Enable ECMP routing on the switch */
} L7_rtrCfgDataV5_t;

typedef struct
{
  nimConfigID_t configId;
  L7_uint32 ipAddr;             /* IP Address                   */
  L7_uint32 ipMask;             /* IP Mask                      */
  L7_uint32 flags;              /* Functions enabled            */
} L7_rtrCfgCktV5_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_ipCfgDataV5_t ip;
  L7_rtrCfgDataV5_t rtr;
  L7_rtrCfgCktV5_t ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;
} L7_ipMapCfgV5_t;

typedef struct
{
  L7_uint32 ipSourceAddrCheck;
  L7_uint32 ipSpoofingCheck;
  L7_uint32 ipDefaultTTL;
  L7_uint32 ipArpAgeTime;
  L7_uint32 ipArpRespTime;
  L7_uint32 ipArpRetries;
  L7_uint32 ipArpCacheSize;
  L7_uint32 ipArpDynamicRenew;
  L7_uint32 route_preferences[L7_RTO_MAX_PROTOCOLS_REL_4_2];
} L7_ipCfgDataV6_t;

typedef struct
{
  L7_uint32 rtrAdminMode;       /* Enable the router entity             */
  L7_uint32 rtrForwarding;      /* Enable Forwarding                    */
  L7_uint32 rtrTOSForwarding;   /* Enable Forwarding based on TOS       */
  L7_uint32 rtrICMPRedirectMode;        /* Enable sending of ICMP Redirects     */
  L7_uint32 rtrBootpDhcpRelayMode;      /* Enable BOOTP or DHCP Relay           */
  L7_IP_ADDR_t rtrBootpDhcpRelayServer; /* BOOTP or DHCP Relay Server           */
  L7_uint32 rtrProxyArpMode;    /* Enable Proxy ARP                     */
  L7_uint32 rtrTraceMode;       /* Enable tracing of events             */
} L7_rtrCfgDataV6_t;

typedef struct
{
  nimConfigID_t configId;
  L7_uint32 ipAddr;             /* IP Address                   */
  L7_uint32 ipMask;             /* IP Mask                      */
  L7_uint32 flags;              /* Functions enabled            */
  L7_uint32 ipMtu;              /* IP MTU value                 */
} L7_rtrCfgCktV6_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_ipCfgDataV6_t ip;
  L7_rtrCfgDataV6_t rtr;
  L7_rtrCfgCktV6_t ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_2];
  L7_uint32 checkSum;
} L7_ipMapCfgV6_t;

typedef struct
{
  L7_uint32 ipSourceAddrCheck;
  L7_uint32 ipSpoofingCheck;
  L7_uint32 ipDefaultTTL;
  L7_uint32 ipArpAgeTime;
  L7_uint32 ipArpRespTime;
  L7_uint32 ipArpRetries;
  L7_uint32 ipArpCacheSize;
  L7_uint32 ipArpDynamicRenew;
  L7_uint32 route_preferences[L7_RTO_MAX_PROTOCOLS_REL_4_3];
} L7_ipCfgDataV7_t;
typedef struct
{
  L7_uint32           rtrAdminMode;                 /* Enable the router entity             */
  L7_uint32           rtrForwarding;                /* Enable Forwarding                    */
  L7_uint32           rtrTOSForwarding;             /* Enable Forwarding based on TOS       */
  L7_uint32           rtrICMPRedirectMode;          /* Enable sending of ICMP Redirects     */
  L7_uint32           rtrBootpDhcpRelayMode;        /* Enable BOOTP or DHCP Relay           */
  L7_IP_ADDR_t        rtrBootpDhcpRelayServer;      /* BOOTP or DHCP Relay Server           */
  L7_uint32           rtrTraceMode;                 /* Enable tracing of events             */

} L7_rtrCfgDataV7_t;

typedef struct 
{
  nimConfigID_t       configId;
  L7_rtrIntfIpAddr_t  addrs[L7_L3_NUM_IP_ADDRS];
  L7_uint32           flags;                    /* Functions enabled            */
  L7_uint32           ipMtu;                    /* IP MTU value                 */
  L7_uint32           icmpUnreachablesMode;     /* Enable sending of ICMP Unreachables    */
} L7_rtrCfgCktV7_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_ipCfgDataV7_t ip;
  L7_rtrCfgDataV7_t rtr;
  L7_rtrCfgCktV7_t ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_3];
  L7_uint32 checkSum;
} L7_ipMapCfgV7_t;

typedef struct 
{
  nimConfigID_t       configId;
  L7_rtrIntfIpAddr_t  addrs[L7_L3_NUM_IP_ADDRS];
  L7_uint32           flags;                    /* Functions enabled            */
  L7_uint32           ipMtu;                    /* IP MTU value                 */
  L7_uint32           icmpUnreachablesMode;     /* Enable sending of ICMP Unreachables    */
} L7_rtrCfgCktV8_t;

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
  L7_uint32           route_preferences[L7_RTO_MAX_PROTOCOLS_REL_4_4];  /* Preferences values for protocols */

} L7_ipCfgDataV8_t;

typedef struct
{
  L7_uint32           rtrAdminMode;                 /* Enable the router entity             */
  L7_uint32           rtrForwarding;                /* Enable Forwarding                    */
  L7_uint32           rtrTOSForwarding;             /* Enable Forwarding based on TOS       */
  L7_uint32           rtrICMPRedirectMode;          /* Enable sending of ICMP Redirects     */
  L7_uint32           rtrBootpDhcpRelayMode;        /* Enable BOOTP or DHCP Relay           */
  L7_IP_ADDR_t        rtrBootpDhcpRelayServer;      /* BOOTP or DHCP Relay Server           */
  L7_uint32           rtrTraceMode;                 /* Enable tracing of events             */

} L7_rtrCfgDataV8_t;


typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_ipCfgDataV8_t ip;
  L7_rtrCfgDataV8_t rtr;
  L7_rtrCfgCktV8_t ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_4];
  L7_uint32 checkSum;
} L7_ipMapCfgV8_t;

typedef L7_ipMapCfg_t L7_ipMapCfgV9_t;

typedef struct
{
  L7_ushort16 vlanID;           /* VLAN ID associated with circuit */
} L7_rtrCktData_t;

typedef struct
{
  L7_BOOL inUse;                /* Keeps track if this entry is in use            */
  L7_uint32 ipAddr;             /* IP Address                                     */
  L7_uint32 ipMask;             /* IP Mask                                        */
  L7_uint32 nextHopRtr;         /* Next Hop Router IP                             */
  L7_uint32 cost;               /* cost of route                                  */
  L7_rtrCktData_t circuitData;  /* additional circuit-related data                */
  L7_uint32 flags;
} L7_rtrStaticRouteCfgDataV1_t;


typedef struct
{
  L7_BOOL inUse;                /* Keeps track if this entry is in use            */
  L7_uint32 ipAddr;             /* IP Address                                     */
  L7_uint32 ipMask;             /* IP Mask                                        */
  L7_uint32 nextHopRtr;         /* Next Hop Router IP                             */
  L7_uint32 cost;               /* cost of route                                  */
  L7_uint32 flags;
} L7_rtrStaticRouteCfgDataV2_t;


typedef struct L7_rtrStaticRouteCfgDataV3_s
{
  L7_BOOL             inUse;          /* Keeps track if this entry is in use */
  L7_uint32           ipAddr;         /* Destination prefix                  */
  L7_uint32           ipMask;         /* Destination mask                    */

  /* IP address of each next hop. A value of 0 indicates the end of the list.
   * That is, the array has no holes. */
  L7_uint32           nextHopRtr[L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_2];    

  /* Preference (administrative distance) of this route. Used to select
   * among routes to the same destination from different sources. */
  L7_uint32           preference;

  L7_uint32           flags;                   
} L7_rtrStaticRouteCfgDataV3_t;




typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_rtrStaticRouteCfgDataV1_t rtrStaticRouteCfgData[L7_RTR_MAX_STATIC_ROUTES_REL_4_0];
  L7_uint32 checkSum;

} L7_rtrStaticRouteCfgV1_t;


typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_rtrStaticRouteCfgDataV2_t rtrStaticRouteCfgData[L7_RTR_MAX_STATIC_ROUTES_REL_4_1];
  L7_uint32 checkSum;

} L7_rtrStaticRouteCfgV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_rtrStaticRouteCfgDataV3_t rtrStaticRouteCfgData[L7_RTR_MAX_STATIC_ROUTES_REL_4_2];
  L7_uint32 checkSum;

} L7_rtrStaticRouteCfgV3_t;

typedef struct
{
  L7_BOOL             inUse;                    /* Keeps track if this entry is in use            */
  L7_uint32           ipAddr;         /* Destination prefix                  */
  L7_uint32           ipMask;         /* Destination mask                    */

  /* IP address of each next hop. A value of 0 indicates the end of the list.
   * That is, the array has no holes. */
  L7_uint32           nextHopRtr[L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_3];

  /* Preference (administrative distance) of this route. Used to select
   * among routes to the same destination from different sources. */
  L7_uint32           preference;

  L7_uint32           flags;
#ifndef L7_RT_DEFAULT_ROUTE
#define L7_RT_DEFAULT_ROUTE     0x01
#endif
} L7_rtrStaticRouteCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_rtrStaticRouteCfgDataV4_t rtrStaticRouteCfgData[L7_RTR_MAX_STATIC_ROUTES_REL_4_3];
  L7_uint32 checkSum;

} L7_rtrStaticRouteCfgV4_t;

typedef struct 
{
  L7_BOOL             inUse;          /* Keeps track if this entry is in use */
  L7_uint32           ipAddr;         /* Destination prefix                  */
  L7_uint32           ipMask;         /* Destination mask                    */

  /* IP address of each next hop. A value of 0 indicates the end of the list.
   * That is, the array has no holes. */
  L7_uint32           nextHopRtr[L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_4];    

  /* Preference (administrative distance) of this route. Used to select
   * among routes to the same destination from different sources. */
  L7_uint32           preference;

  L7_uint32           flags;
#ifndef L7_RT_DEFAULT_ROUTE
#define L7_RT_DEFAULT_ROUTE     0x01
#endif
} L7_rtrStaticRouteCfgDataV5_t;

typedef struct 
{
  L7_fileHdr_t                cfgHdr;
  L7_rtrStaticRouteCfgDataV5_t  rtrStaticRouteCfgData[L7_RTR_MAX_STATIC_ROUTES_REL_4_4];
  L7_uint32                   checkSum;

}L7_rtrStaticRouteCfgV5_t;

typedef L7_rtrStaticRouteCfg_t L7_rtrStaticRouteCfgV6_t;



typedef struct
{
  L7_IP_ADDR_t ipAddr;          /* IP Address                   */
  L7_linkLayerAddr_t macAddr;   /* MAC address                  */
  nimUSP_REL_4_0_t intf;        /* outbound interface           */
} L7_rtrStaticArpCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_rtrStaticArpCfgDataV1_t rtrStaticArpCfgData[FD_RTR_MAX_STATIC_ARP_ENTRIES_REL_4_0];
  L7_uint32 checkSum;

} L7_rtrStaticArpCfgV1_t;

typedef struct 
{
  L7_IP_ADDR_t        ipAddr;                   /* IP Address  */
  L7_linkLayerAddr_t  macAddr;                  /* MAC address */
} L7_rtrStaticArpCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t                cfgHdr;
  L7_rtrStaticArpCfgDataV2_t  rtrStaticArpCfgData[FD_RTR_MAX_STATIC_ARP_ENTRIES_REL_4_4];
  L7_uint32                   checkSum;

} L7_rtrStaticArpCfgV2_t;


typedef L7_rtrStaticArpCfg_t L7_rtrStaticArpCfgV3_t;

#endif /* IPMAP_MIGRATE_H */
