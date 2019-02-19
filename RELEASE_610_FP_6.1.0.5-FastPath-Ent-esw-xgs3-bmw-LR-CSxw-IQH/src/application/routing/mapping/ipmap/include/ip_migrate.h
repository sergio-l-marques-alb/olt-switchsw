
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   ip_migrate.h
*
* @purpose    Ip Mapping layer structures and config data
*
* @component  Ip Mapping Layer
*
* @comments   Stores configuration data structures for previous versions
*             of fastpath.
*
* @create     08/24/2004
*
* @author     Rama Sasthri, Kristipati
* @end
*
**********************************************************************/
#error 1
#ifndef INCLUDE_IP_MIGRATE_H
#define INCLUDE_IP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

#include "l7_common.h"
#include "l3_defaultconfig.h"
#include "ip_config.h"

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
L7_rtrCfgDataV4_t;

typedef struct
{
  L7_uint32 ipAddr;             /* IP Address                   */
  L7_uint32 ipMask;             /* IP Mask                      */
  L7_uint32 flags;              /* Functions enabled            */
  L7_rtrCktData_t circuitData;  /* Additional circuit-related data */
}
L7_rtrCfgCktV4_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_ipCfgData_t ip;
  L7_rtrCfgDataV4_t rtr;
  L7_rtrCfgCktV4_t ckt[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;
}
L7_ipMapCfgV4_t;

typedef L7_ipMapCfg_t L7_ipMapCfgV5_t;

#endif /* INCLUDE_IP_MIGRATE_H */
