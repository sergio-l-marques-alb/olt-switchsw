
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename dvmrp_migrate.h
*
* @purpose dvmrp Configuration Migration
*
* @component dvmrp
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef DVMRP_MIGRATE_H
#define DVMRP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

typedef struct
{
  L7_uint32 intfAdminMode;      /* Enable DVMRP    per interface        */
  L7_ushort16 intfMetric;       /* Metric per interface */
}
dvmrpCfgCktV1_t;

typedef struct dvmrpCfgDataV1_s
{
  L7_uint32          adminMode;
  L7_uint32           traceMode; 
} dvmrpCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  dvmrpCfgDataV1_t rtr;
  dvmrpCfgCktV1_t ckt[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;
}
L7_dvmrpMapCfgV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  dvmrpCfgDataV1_t rtr;
  dvmrpCfgCkt_t ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;
} L7_dvmrpMapCfgV2_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  dvmrpCfgDataV1_t    rtr;
  dvmrpCfgCkt_t       ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_3]; 
  L7_uint32           checkSum;

} L7_dvmrpMapCfgV3_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  dvmrpCfgDataV1_t    rtr;
  dvmrpCfgCkt_t       ckt[L7_IPMAP_INTF_MAX_COUNT_REL_4_4]; 
  L7_uint32           checkSum;

} L7_dvmrpMapCfgV4_t;

typedef L7_dvmrpMapCfg_t L7_dvmrpMapCfgV5_t;

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void
dvmrpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);
#endif /* DVMRP_MIGRATE_H */
