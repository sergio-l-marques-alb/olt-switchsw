
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    gmrp_migrate.h
* @purpose     GMRP Configuration Migration
* @component   GMRP-GARP
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @end
*             
**********************************************************************/

#ifndef GMRP_MIGRATE_H
#define GMRP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "defaultconfig.h"
#include "garpcfg.h"
#include "gmrapi.h"
#include "gmrp_api.h"

#define L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_0 L7_MFDB_MAX_MAC_ENTRIES_REL_4_0
#define L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_1 L7_MFDB_MAX_MAC_ENTRIES_REL_4_1
#define L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_2 L7_MFDB_MAX_MAC_ENTRIES_REL_4_2
#define L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_3 L7_MFDB_MAX_MAC_ENTRIES_REL_4_3
#define L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_4 L7_MFDB_MAX_MAC_ENTRIES_REL_4_4

#define L7_GARP_MAX_CFG_IDX_REL_4_1 L7_MAX_INTERFACE_COUNT_REL_4_1
#define L7_GARP_MAX_CFG_IDX_REL_4_2 L7_MAX_INTERFACE_COUNT_REL_4_2
#define L7_GARP_MAX_CFG_IDX_REL_4_3 L7_MAX_INTERFACE_COUNT_REL_4_3
#define L7_GARP_MAX_CFG_IDX_REL_4_4 L7_MAX_INTERFACE_COUNT_REL_4_4

typedef struct
{
  L7_uchar8 vlanIdmacAddress[8];
  NIM_INTF_MASK_REL_4_0_t fixedReg;
  NIM_INTF_MASK_REL_4_0_t forbiddenReg;
  L7_BOOL inUse;
}
GMRP_REG_ENTRY_V1_t;

typedef struct
{
  L7_ushort16 vlanId;
  NIM_INTF_MASK_REL_4_0_t fixedReg;
  NIM_INTF_MASK_REL_4_0_t forbiddenReg;
}
GMRP_SERVICE_ENTRY_V1_t;

typedef struct
{
  GMRP_REG_ENTRY_V1_t staticEntry[L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_0];
  GMRP_SERVICE_ENTRY_V1_t staticForwardAll[L7_MAX_VLANS_REL_4_0];
  GMRP_SERVICE_ENTRY_V1_t staticForwardUnreg[L7_MAX_VLANS_REL_4_0];
  L7_BOOL gmrpEnabled[L7_MAX_INTERFACE_COUNT_REL_4_0 + 1];
}
gmrpCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t hdr;
  gmrpCfgDataV1_t cfg;
  L7_uint32 checkSum;
}
gmrpCfgV1_t;

typedef struct
{
  L7_uchar8 vlanIdmacAddress[8];
  NIM_INTF_MASK_REL_4_1_t fixedReg;
  NIM_INTF_MASK_REL_4_1_t forbiddenReg;
  L7_BOOL inUse;
}
GMRP_REG_ENTRY_V2_t;

typedef struct
{
  L7_ushort16 vlanId;
  NIM_INTF_MASK_REL_4_1_t fixedReg;
  NIM_INTF_MASK_REL_4_1_t forbiddenReg;
}
GMRP_SERVICE_ENTRY_V2_t;

typedef struct
{
  GMRP_REG_ENTRY_V2_t staticEntry[L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_1];
  GMRP_SERVICE_ENTRY_V2_t staticForwardAll[L7_MAX_VLANS_REL_4_1];
  GMRP_SERVICE_ENTRY_V2_t staticForwardUnreg[L7_MAX_VLANS_REL_4_1];
  L7_BOOL gmrpEnabled[L7_MAX_INTERFACE_COUNT_REL_4_1 + 1];
}
gmrpCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t hdr;
  gmrpCfgDataV2_t cfg;
  gmrpIntfCfgData_t intf[L7_GARP_MAX_CFG_IDX_REL_4_1];
  L7_uint32 checkSum;
} gmrpCfgV2_t;

typedef struct
{
  L7_uchar8 vlanIdmacAddress[8];
  NIM_INTF_MASK_REL_4_3_t fixedReg;
  NIM_INTF_MASK_REL_4_3_t forbiddenReg;
  L7_BOOL inUse;
} GMRP_REG_ENTRY_V3_t;

typedef struct 
{
  L7_ushort16 vlanId;
  NIM_INTF_MASK_REL_4_3_t fixedReg;
  NIM_INTF_MASK_REL_4_3_t forbiddenReg;
} GMRP_SERVICE_ENTRY_V3_t;

typedef struct
{
  GMRP_REG_ENTRY_V3_t staticEntry[L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_3];
  GMRP_SERVICE_ENTRY_V3_t staticForwardAll[L7_MAX_VLANS_REL_4_3];
  GMRP_SERVICE_ENTRY_V3_t staticForwardUnreg[L7_MAX_VLANS_REL_4_3];
  L7_BOOL gmrpEnabled[L7_MAX_INTERFACE_COUNT_REL_4_3 + 1];
} gmrpCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t hdr;
  gmrpCfgDataV3_t cfg;
  gmrpIntfCfgData_t intf[L7_GARP_MAX_CFG_IDX_REL_4_3];
  L7_uint32 checkSum;
} gmrpCfgV3_t;

typedef gmrpCfg_t gmrpCfgV4_t;

#endif /* GMRP_MIGRATE_H */
