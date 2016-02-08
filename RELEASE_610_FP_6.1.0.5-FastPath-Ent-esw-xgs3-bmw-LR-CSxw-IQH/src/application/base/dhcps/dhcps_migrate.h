
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   dhcps_migrate.h
*
* @purpose    DHCP Server Configuration Migration
*
* @component  DHCP Server Component
*
* @comments   none
*
* @create     08/24/2004
*
* @author     Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef DHCPS_MIGRATE_H
#define DHCPS_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_dhcpsinclude.h"

typedef struct
{
  L7_uint32 dhcpsAdminMode;     /* DHCP Server Mode           */
  L7_uint32 dhcpsPingPktNo;     /* No. of ping pkts           */
}
dhcpsCfgDataV1_t;

typedef struct
{
  /* read-only (after pool allocation) */
  L7_char8 poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t ipMask;
  L7_int32 poolType;

  /* read-write */
  L7_IP_ADDR_t defaultrouterAddrs[L7_DHCPS_DEFAULT_ROUTER_MAX];
  L7_IP_ADDR_t DNSServerAddrs[L7_DHCPS_DNS_SERVER_MAX];
  L7_uint32 leaseTime;          /* Lease time in minutes  */

  /* Extra fields for a Manual pool */
  L7_uchar8 hostIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_0];
  L7_uchar8 hostIdLength;
  L7_uchar8 hostAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8 hostAddrtype;       /* Hardware addr type, ethernet - 1 */
  L7_uchar8 hostAddrLength;
  L7_uchar8 hostName[L7_DHCPS_HOST_NAME_MAXLEN_REL_4_0];
} poolCfgDataV1_t;

typedef struct
{
  /* read-only (after lease allocation) */
  L7_char8 poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_uchar8 clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_0];
  L7_uchar8 hwAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8 hAddrtype;          /* Hardware addr type, ethernet -1 */
  L7_uchar8 hwAddrLength;
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t ipMask;
  L7_uint32 intIfNum;

  /* read-write */
  L7_uint32 leaseStartTime;     /* Lease start time   */
  L7_uint32 leaseEndTime;       /* Lease end time   */
  L7_uint32 leaseTime;          /* Lease duration in minutes */
  L7_int32 state;
  L7_BOOL IsBootp;
} leaseCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  dhcpsCfgDataV1_t dhcps;
  poolCfgDataV1_t pool[L7_DHCPS_MAX_POOL_NUMBER_REL_4_0];       /* Max automatic pool + No of manual pools */
  excludedAddrCfgData_t excludedAddrs[L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_0];
  leaseCfgDataV1_t lease[L7_DHCPS_MAX_LEASE_NUMBER_REL_4_0];
  L7_uint32 checkSum;
} L7_dhcpsMapCfgV1_t;

typedef struct
{
  /* read-only (after pool allocation) */
  L7_char8 poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t ipMask;
  L7_uchar8 poolType;

  /* read-write */
  L7_IP_ADDR_t defaultrouterAddrs[L7_DHCPS_DEFAULT_ROUTER_MAX];
  L7_IP_ADDR_t DNSServerAddrs[L7_DHCPS_DNS_SERVER_MAX];
  L7_uint32 leaseTime;          /* Lease time in minutes  */

  /* Extra fields for a Manual pool */
  L7_uchar8 hostIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_0];
  L7_uchar8 hostIdLength;
  L7_uchar8 hostAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8 hostAddrtype;       /* Hardware addr type, ethernet - 1 */
  L7_uchar8 hostAddrLength;
  L7_uchar8 hostName[L7_DHCPS_HOST_NAME_MAXLEN_REL_4_0];
}
poolCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  dhcpsCfgDataV1_t dhcps;
  poolCfgDataV2_t pool[L7_DHCPS_MAX_POOL_NUMBER_REL_4_0];
  excludedAddrCfgData_t excludedAddrs[L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_0];
  L7_uint32 checkSum;
}
L7_dhcpsMapCfgV2_t;

typedef struct
{
  /* read-only (after pool allocation) */
  L7_char8    poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_IP_ADDR_t  ipAddr;
  L7_IP_MASK_t  ipMask;
  L7_uchar8     poolType;

  /* read-write */
  L7_IP_ADDR_t  defaultrouterAddrs[L7_DHCPS_DEFAULT_ROUTER_MAX];
  L7_IP_ADDR_t  DNSServerAddrs[L7_DHCPS_DNS_SERVER_MAX];
  L7_uint32     leaseTime;                /* Lease time in minutes  */

  /* Extra fields for a Manual pool */
  L7_uchar8     hostIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_2];
  L7_uchar8     hostIdLength;
  L7_uchar8     hostAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8     hostAddrtype; /* Hardware addr type, ethernet - 1 */
  L7_uchar8     hostAddrLength;
  L7_uchar8     hostName[L7_DHCPS_HOST_NAME_MAXLEN_REL_4_2];

  /* Phase 2 fields*/
  L7_IP_ADDR_t  netBiosNameServer[L7_DHCPS_NETBIOS_NAME_SERVER_MAX]; /* netbios name server */
  L7_uchar8     netBiosNodeType;  /* Netbios node type */
  L7_IP_ADDR_t  nextServer; /* Next server address */
  L7_uchar8  domainName[L7_DHCPS_DOMAIN_NAME_MAXLEN];  /* Client domain name */
  L7_uchar8  clientBootFile[L7_DHCPS_BOOT_FILE_NAME_MAXLEN] ; /* Boot file name */
  L7_uchar8  options[L7_DHCPS_POOL_OPTION_BUFFER];   /* DHCP option, compact form*/
} 
poolCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  dhcpsCfgData_t dhcps;
  poolCfgDataV3_t pool[L7_DHCPS_MAX_POOL_NUMBER_REL_4_2];
  excludedAddrCfgData_t excludedAddrs[L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_2];
  L7_uint32 checkSum;
}
L7_dhcpsMapCfgV3_t;

typedef L7_dhcpsMapCfg_t L7_dhcpsMapCfgV4_t;


/*********************************************************************
*
**********************************************************************/

typedef struct
{
  L7_fileHdr_t cfgHdr;
  leaseCfgDataV1_t lease[L7_DHCPS_MAX_LEASE_NUMBER_REL_4_0];
  L7_uint32 checkSum;
} L7_dhcpsMapLeaseCfgV1_t;


typedef struct
{
  /* read-only (after lease allocation) */
  L7_char8      poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_uchar8     clientIdLength;
  L7_uchar8     clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_1];
  L7_uchar8     hwAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8     hAddrtype; /* Hardware addr type, ethernet -1 */
  L7_uchar8     hwAddrLength;
  L7_IP_ADDR_t  ipAddr;
  L7_IP_MASK_t  ipMask;

  /* read-write */
  L7_uint32     leaseTime;        /* Lease duration in minutes */
  L7_uchar8     state;
  L7_BOOL       IsBootp;
  L7_uchar8     conflictDetectionMethod;
} leaseCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  leaseCfgDataV2_t lease[L7_DHCPS_MAX_LEASE_NUMBER_REL_4_1];
  L7_uint32 checkSum;
} L7_dhcpsMapLeaseCfgV2_t;

typedef L7_dhcpsMapLeaseCfgV2_t L7_dhcpsMapLeaseCfgV3_t;

#endif /* DHCPS_MIGRATE_H */
