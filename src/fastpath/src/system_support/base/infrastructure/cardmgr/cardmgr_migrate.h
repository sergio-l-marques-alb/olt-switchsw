
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename cardmgr_migrate.h
*
* @purpose cardmgr Configuration Migration
*
* @component cardmgr
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef CARDMGR_MIGRATE_H
#define CARDMGR_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "cardmgr_include.h"

typedef struct
{
  cmgr_card_config_t configCard[L7_MAX_UNITS_PER_STACK_REL_4_1 + 1]
    [L7_MAX_SLOTS_PER_UNIT_REL_4_1 + 1];
} L7_cardMgrCfgParmsV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_cardMgrCfgParmsV1_t cfgParms;      /* raw TC config parms */
  L7_uint32 checkSum;
} L7_cardMgrCfgFileDataV1_t;

typedef struct
{
  cmgr_card_config_t configCard[L7_MAX_UNITS_PER_STACK_REL_4_3 + 1]
    [L7_MAX_SLOTS_PER_UNIT_REL_4_3 + 1];
} L7_cardMgrCfgParmsV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_cardMgrCfgParmsV2_t cfgParms;      /* raw TC config parms */
  L7_uint32 checkSum;
} L7_cardMgrCfgFileDataV2_t;

typedef L7_cardMgrCfgFileData_t L7_cardMgrCfgFileDataV3_t;

#endif /* CARDMGR_MIGRATE_H */
