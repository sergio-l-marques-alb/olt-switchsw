
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dvlantag_migrate.h
*
* @purpose   Double Vlan Tagging configuration Migration
*
* @component DvlanTag
*
* @comments  none
*
* @create    8/23/2004
*
* @author    Rama Sasthri, Kristipati
*
* @end
*             
**********************************************************************/

#ifndef DVLANTAG_MIGRATE_H
#define DVLANTAG_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "dvlantag_include.h"

#define L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_0 (L7_MAX_INTERFACE_COUNT_REL_4_0)
#define L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_1 (L7_MAX_INTERFACE_COUNT_REL_4_1 + 1)
#define L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_2 (L7_MAX_INTERFACE_COUNT_REL_4_2 + 1)
#define L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_3 (L7_MAX_INTERFACE_COUNT_REL_4_3 + 1)
#define L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_4 (L7_MAX_INTERFACE_COUNT_REL_4_4 + 1)

typedef struct
{
  nimUSP_REL_4_0_t usp;         /*nim config id for stacking builds */
  L7_uint32 mode;               /*default - L7_DISABLE */
  L7_ushort16 etherType;        /*default - 0x8100    */
  L7_ushort16 custId;           /*default - 0x00      */
}
DVLANTAG_INTF_CFG_V1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  DVLANTAG_INTF_CFG_V1_t intfCfg[L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_0];
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
}
DVLANTAG_CFG_V1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  DVLANTAG_INTF_CFG_t intfCfg[L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_1];
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
} DVLANTAG_CFG_V2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  DVLANTAG_INTF_CFG_t intfCfg[L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_3];
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
} DVLANTAG_CFG_V3_t;

typedef DVLANTAG_CFG_t DVLANTAG_CFG_V4_t;

#endif /* DVLANTAG_MIGRATE_H */
