
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

/*
Lease config data type 3 looks the same as type 2, except that
the clientIdentifier field is one character shorter, and the
numbers of leases has been reduced from 1024 to 256
*/

typedef struct
{
  /* read-only (after lease allocation) */
  L7_char8      poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_uchar8     clientIdLength;
  L7_uchar8     clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_3];
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
} leaseCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  leaseCfgDataV3_t lease[L7_DHCPS_MAX_LEASE_NUMBER_REL_4_3];
  L7_uint32 checkSum;
} L7_dhcpsMapLeaseCfgV3_t;

typedef struct
{
  /* read-only (after lease allocation) */
  L7_char8      poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_uchar8     clientIdLength;
  L7_uchar8     clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_3];
  L7_uchar8     hwAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8     hAddrtype; /* Hardware addr type, ethernet -1 */
  L7_uchar8     hwAddrLength;
  L7_IP_ADDR_t  ipAddr;
  L7_IP_MASK_t  ipMask;

  /* read-write */
  L7_uint32     leaseTime;        /* Lease duration in minutes */
  L7_uint32     leaseExpiration;  /* System uptime at which lease expires */
  L7_uchar8     state;
  L7_BOOL       IsBootp;
  L7_uchar8     conflictDetectionMethod;
} leaseCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t          cfgHdr;
  leaseCfgDataV4_t      lease[L7_DHCPS_MAX_LEASE_NUMBER_REL_4_3];
  L7_uint32             checkSum;

} L7_dhcpsMapLeaseCfgV4_t;

#endif /* DHCPS_MIGRATE_H */
