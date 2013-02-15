
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    mirror_migrate.h
* @purpose     Port mirroring Configuration Migration
* @component   mirroring
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @end
*             
**********************************************************************/

#ifndef MIRROR_MIGRATE_H
#define MIRROR_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "default_cnfgr.h"
#include "mirror.h"
#include "defaultconfig.h"

typedef struct
{
  L7_fileHdr_t cfgHdr;
  nimUSP_REL_4_0_t mirrorSourceUSP;
  nimUSP_REL_4_0_t mirrorDestUSP;
  L7_uint32 mirrorMode;         /* SIM_PORT_MONITOR_ENABLE or SIM_PORT_MONITOR_DISABLE */
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
}
mirrorCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;

  nimConfigID_t mirrorSourceID;
  nimConfigID_t mirrorDestID;
  L7_uint32 mirrorMode;         /* SIM_PORT_MONITOR_ENABLE or SIM_PORT_MONITOR_DISABLE */

  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
} mirrorCfgDataV2_t;

typedef struct
{
  L7_INTF_MASK_REL_4_3_t srcPorts; 
  nimConfigID_t destID;   
  L7_uint32 mode;    
  L7_MIRROR_DIRECTION_t probeType;
} mirrorSessionV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  mirrorSessionV3_t sessionData[L7_MIRRORING_MAX_SESSIONS];
  L7_uint32 checkSum;     /* check sum of config file NOTE: needs to be last entry */
} mirrorCfgDataV3_t;

typedef mirrorCfgData_t mirrorCfgDataV4_t;

#endif /*MIRROR_MIGRATE_H */
