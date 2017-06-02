
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename cos_migrate.h
*
* @purpose COS Configuration Migration
*
* @component COS
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef COS_MIGRATE_H
#define COS_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "cos_config.h"
#include "cos_util.h"


typedef struct
{
  L7_fileHdr_t            cfgHdr;
  L7_uint32               msgLvl;       /* debug message level (saved config) */
  L7_cosCfgGlobalParms_t  cosGlobal;
  L7_cosCfgIntfParms_t    cosIntf[L7_COS_INTF_MAX_COUNT_REL_4_3];
  L7_uint32               checkSum;
  
} L7_cosCfgDataV1_t;

typedef L7_cosCfgData_t L7_cosCfgDataV2_t;

#endif /* COS_MIGRATE_H */
