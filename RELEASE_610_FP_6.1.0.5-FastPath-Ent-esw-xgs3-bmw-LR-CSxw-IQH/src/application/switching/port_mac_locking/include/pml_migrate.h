
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename pml_migrate.h
*
* @purpose pml Configuration Migration
*
* @component pml
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef PML_MIGRATE_H
#define PML_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "pml.h"

#define L7_PML_MAX_INTF_REL_4_3 (L7_MAX_PORT_COUNT_REL_4_3 + \
                                 L7_MAX_NUM_LAG_INTF_REL_4_3 + 1)
#define L7_PML_MAX_INTF_REL_4_4 (L7_MAX_PORT_COUNT_REL_4_4 + \
                                 L7_MAX_NUM_LAG_INTF_REL_4_4 + 1)

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_BOOL globalLockEnabled;
  pmlIntfCfgData_t pmlIntfCfgData[L7_PML_MAX_INTF_REL_4_3];
  L7_uint32 checkSum;
} pmlCfgDataV1_t;

typedef pmlCfgData_t pmlCfgDataV2_t;

#endif /* PML_MIGRATE_H */
