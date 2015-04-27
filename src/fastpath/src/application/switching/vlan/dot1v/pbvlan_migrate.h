
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename pbvlan_migrate.h
*
* @purpose pbvlan Configuration Migration
*
* @component pbvlan
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef PBVLAN_MIGRATE_H
#define PBVLAN_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

#include "defaultconfig.h"
#include "pbvlan.h"

typedef struct
{
  L7_char8 name[L7_PBVLAN_MAX_GROUP_NAME];
  L7_BOOL inUse;
  L7_uint32 groupID;
  L7_uint32 vlanid;
  L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  NIM_INTF_MASK_REL_4_0_t ports;
}
pbVlanGroupInfoV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  pbVlanGroupInfoV1_t group[L7_PBVLAN_MAX_NUM_GROUPS_REL_4_0 + 1];
  L7_uint32 checkSum;
}
pbVlanCfgDataV1_t;

typedef struct
{
  L7_char8 name[L7_PBVLAN_MAX_GROUP_NAME];
  L7_BOOL inUse;
  L7_uint32 groupID;
  L7_uint32 vlanid;
  L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  NIM_INTF_MASK_REL_4_1_t ports;
}
pbVlanGroupInfoV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  pbVlanGroupInfoV2_t group[L7_PBVLAN_MAX_NUM_GROUPS_REL_4_1 + 1];
  L7_uint32 checkSum;
} pbVlanCfgDataV2_t;

typedef struct
{
  L7_char8 name[L7_PBVLAN_MAX_GROUP_NAME];
  L7_BOOL inUse;
  L7_uint32 groupID;
  L7_uint32 vlanid;
  L7_uint32 protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  NIM_INTF_MASK_REL_4_3_t ports;
} pbVlanGroupInfoV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  pbVlanGroupInfoV3_t group[L7_PBVLAN_MAX_NUM_GROUPS_REL_4_3 + 1];
  L7_uint32 checkSum;
} pbVlanCfgDataV3_t;

typedef struct
{
  L7_char8        name[L7_PBVLAN_MAX_GROUP_NAME];
  L7_BOOL         inUse;
  L7_uint32       groupID;
  L7_uint32       vlanid;
  L7_uint32       protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  NIM_INTF_MASK_t ports;
} pbVlanGroupInfoV4_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  L7_uint32           etherProtocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  pbVlanGroupInfoV4_t group[L7_PBVLAN_MAX_NUM_GROUPS + 1];
  L7_uint32           checkSum;
} pbVlanCfgDataV4_t;

typedef pbVlanCfgData_t pbVlanCfgDataV5_t;

#endif /* PBVLAN_MIGRATE_H */
