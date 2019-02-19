/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename pimdm_map_migrate.h
*
* @purpose pimdm Configuration Migration
*
* @component pimdm
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef PIMDM_MIGRATE_H
#define PIMDM_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "pimdm_map_config.h"

typedef struct
{
  L7_uint32 mode;               /* Enable PIMDM    per interface */
  L7_uint32 pimInterfaceHelloInterval;  /* Hello interval per interface */
  L7_uchar8 entryStatus;        /* in-use, deleted, free */
}
pimdmCfgCktV1_t;

typedef struct pimdmCfgDataV1_s
{
   L7_uint32           adminMode;
   L7_uint32           traceMode; 
} pimdmCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  pimdmCfgDataV1_t rtr;
  pimdmCfgCktV1_t ckt[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;
}
L7_pimdmMapCfgV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  pimdmCfgDataV1_t rtr;
  pimdmCfgCkt_t ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;

} L7_pimdmMapCfgV2_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  pimdmCfgDataV1_t      rtr;
  pimdmCfgCkt_t       ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_3];
  L7_uint32           checkSum;

} L7_pimdmMapCfgV3_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  pimdmCfgDataV1_t    rtr;
  pimdmCfgCkt_t       ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_4];
  L7_uint32           checkSum;

} L7_pimdmMapCfgV4_t;


typedef L7_pimdmMapCfg_t L7_pimdmMapCfgV5_t;

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @comments This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
*
* @end
*********************************************************************/
void
pimdmMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);
#endif /* PIMDM_MIGRATE_H */
