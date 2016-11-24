
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename mgmd_migrate.h
*
* @purpose MGMD Configuration Migration
*
* @component MGMD
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef MGMD_MIGRATE_H
#define MGMD_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

typedef struct
{
  L7_uint32 adminMode;
  L7_uint32 queryInterval;
  L7_uint32 version;
  L7_uint32 responseInterval;
  L7_uint32 robustness;
  L7_uint32 startupQueryInterval;
  L7_uint32 startupQueryCount;
  L7_uint32 lastMemQueryInterval;
  L7_uint32 lastMemQueryCount;
  L7_uchar8 entryStatus;        /* in-use, deleted, free */
}
mgmdIntfCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 adminMode;
  L7_uint32 traceMode;
  mgmdIntfCfgDataV1_t mgmdIntf[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;
}
L7_mgmdMapCfgV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 adminMode;
  L7_uint32 traceMode;
  mgmdIntfCfgData_t mgmdIntf[L7_IPMAP_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;
} L7_mgmdMapCfgV2_t;

typedef struct mgmdIntfCfgDataV3_s
{
  nimConfigID_t configId;
  L7_uint32 adminMode;
  L7_uint32 queryInterval;
  L7_uint32 version;
  L7_uint32 responseInterval;
  L7_uint32 robustness;
  L7_uint32 startupQueryInterval;
  L7_uint32 startupQueryCount;
  L7_uint32 lastMemQueryInterval;
  L7_uint32 lastMemQueryCount;
  L7_uchar8  entryStatus; /* in-use, deleted, free*/
}mgmdIntfCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 adminMode;
  L7_uint32 traceMode;
  mgmdIntfCfgDataV3_t mgmdIntf[L7_IPMAP_INTF_MAX_COUNT_REL_4_3];
  L7_uint32 checkSum;
}L7_mgmdMapCfgV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 adminMode;
  L7_uint32 traceMode;
  mgmdIntfCfgData_t mgmdIntf[L7_IPMAP_INTF_MAX_COUNT_REL_4_4];
  L7_BOOL           mgmd_proxy_UpStreamDisable; /* MGMD-Proxy: Keeps track of 
                                         multicast data stream forward */

  L7_uint32 checkSum;
}L7_mgmdMapCfgV4_t;


typedef L7_mgmdMapCfg_t L7_mgmdMapCfgV5_t;


/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @comments This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @end
*********************************************************************/
void
mgmdMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* MGMD_MIGRATE_H */
