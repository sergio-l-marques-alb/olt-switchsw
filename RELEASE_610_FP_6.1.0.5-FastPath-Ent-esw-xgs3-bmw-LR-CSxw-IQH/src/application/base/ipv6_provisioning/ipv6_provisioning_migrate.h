
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_migrate.h
*
* @purpose Contains IPv6 Provisioning Configuration Migration
*
* @component ipv6_provisioning
*
* @comments  none
*
* @create 08/24/2004
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef IPV6_PROVISIONING_MIGRATE_H
#define IPV6_PROVISIONING_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "sysnet_api.h"
#include "l7_cnfgr_api.h"
#include "ipv6_provisioning_config.h"

#define L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_0 (L7_MAX_INTERFACE_COUNT_REL_4_0 + 0)
#define L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_1 (L7_MAX_INTERFACE_COUNT_REL_4_1 + 1)
#define L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_2 (L7_MAX_INTERFACE_COUNT_REL_4_2 + 1)
#define L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_3 (L7_MAX_INTERFACE_COUNT_REL_4_3 + 1)

typedef struct
{
  L7_uint32 ipv6Mode;
  L7_uint32 dstIfNum;
  L7_uint32 ipv6copyToCpu;      /* To indicate whether packets should be copied
                                   to cpu when bridging between ports */
}
ipv6PortInfoV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  ipv6PortInfoV1_t ipv6PortMappings[L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_0];
  L7_uint32 checkSum;
}
ipv6ProvCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  ipv6PortInfo_t ipv6PortMappings[L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_1];
  L7_uint32 checkSum;
} ipv6ProvCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  ipv6PortInfo_t ipv6PortMappings[L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_3];
  L7_uint32 checkSum;
} ipv6ProvCfgDataV3_t;

typedef ipv6ProvCfgData_t ipv6ProvCfgDataV4_t;

#endif /* IPV6_PROVISIONING_MIGRATE_H */
