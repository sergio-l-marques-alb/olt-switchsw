
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    filter_migrate.h
* @purpose     Port filter Configuration Migration
* @component   filter
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @end
*             
**********************************************************************/

#ifndef FILTER_MIGRATE_H
#define FILTER_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

#include "nimapi.h"
#include "defaultconfig.h"
#include "filter_api.h"
#include "filter.h"

#define L7_FILTER_MAX_INTF_REL_4_0      (L7_MAX_PORT_COUNT_REL_4_0+L7_MAX_NUM_LAG_INTF_REL_4_0)
#define L7_FILTER_MAX_INTF_REL_4_1      (L7_MAX_PORT_COUNT_REL_4_1+L7_MAX_NUM_LAG_INTF_REL_4_1)
#define L7_FILTER_MAX_INTF_REL_4_2      (L7_MAX_PORT_COUNT_REL_4_2+L7_MAX_NUM_LAG_INTF_REL_4_2)
#define L7_FILTER_MAX_INTF_REL_4_3      (L7_MAX_PORT_COUNT_REL_4_3+L7_MAX_NUM_LAG_INTF_REL_4_3)
#define L7_FILTER_MAX_INTF_REL_4_4      (L7_MAX_PORT_COUNT_REL_4_4+L7_MAX_NUM_LAG_INTF_REL_4_4)
#define L7_FILTER_MAX_INTF_REL_5_0      (L7_MAX_PORT_COUNT_REL_5_0+L7_MAX_NUM_LAG_INTF_REL_5_0)

#define L7_FILTER_INTF_MAX_COUNT_REL_4_1 (L7_FILTER_MAX_INTF_REL_4_1 + 1)
#define L7_FILTER_INTF_MAX_COUNT_REL_4_2 (L7_FILTER_MAX_INTF_REL_4_2 + 1)
#define L7_FILTER_INTF_MAX_COUNT_REL_4_3 (L7_FILTER_MAX_INTF_REL_4_3 + 1)
#define L7_FILTER_INTF_MAX_COUNT_REL_4_4 (L7_FILTER_MAX_INTF_REL_4_4 + 1)
#define L7_FILTER_INTF_MAX_COUNT_REL_5_0 (L7_FILTER_MAX_INTF_REL_5_0 + 1)

typedef struct
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 srcifIndex[L7_FILTER_MAX_INTF_REL_4_0];     /*uses embedded unit slot port info in ifIndex */
  L7_uint32 numSrc;
  L7_uint32 dstifIndex[L7_FILTER_MAX_INTF_REL_4_0];
  L7_uint32 numDst;
  L7_BOOL inUse;
}
filterDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  filterDataV1_t filterData[L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_0];
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
}
filterCfgDataV1_t;

typedef struct
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 srcifIndex[L7_FILTER_MAX_INTF_REL_4_1];     /*uses embedded unit slot port info in ifIndex */
  L7_uint32 numSrc;
  L7_uint32 dstifIndex[L7_FILTER_MAX_INTF_REL_4_1];
  L7_uint32 numDst;
  L7_BOOL inUse;
} filterDataV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  filterDataV2_t filterData[L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_1];
  filterIntfCfgData_t filterIntfCfgData[L7_FILTER_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
} filterCfgDataV2_t;

typedef struct 
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 srcifIndex[L7_FILTER_MAX_INTF_REL_4_3]; /*uses intIfNum */
  L7_uint32 numSrc;
  L7_uint32 dstifIndex[L7_FILTER_MAX_INTF_REL_4_3]; /*uses intIfNum */
  L7_uint32 numDst;
  L7_BOOL inUse;
} filterDataV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  filterDataV3_t filterData[L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_3];
  filterIntfCfgData_t filterIntfCfgData[L7_FILTER_INTF_MAX_COUNT_REL_4_3];
  L7_uint32 checkSum;
} filterCfgDataV3_t;
typedef struct 
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 srcifIndex[L7_FILTER_MAX_INTF_REL_4_4]; /*uses intIfNum */
  L7_uint32 numSrc;
  L7_uint32 dstifIndex[L7_FILTER_MAX_INTF_REL_4_4]; /*uses intIfNum */
  L7_uint32 numDst;
  L7_BOOL inUse;
} filterDataV4_t;
typedef struct
{
  L7_fileHdr_t        cfgHdr;
  filterDataV4_t        filterData[L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_4];
  filterIntfCfgData_t filterIntfCfgData[L7_FILTER_INTF_MAX_COUNT_REL_4_4];
  L7_uint32           checkSum;                 /* check sum of config file NOTE: needs to be last entry */
} filterCfgDataV4_t;

typedef struct 
{
  L7_uchar8      macAddr[L7_MAC_ADDR_LEN];
  L7_uint32      vlanId;
  L7_uint32      srcifIndex[L7_FILTER_MAX_INTF_REL_5_0]; /*uses intIfNum */
  L7_uint32      numSrc;
  L7_uint32      dstifIndex[L7_FILTER_MAX_INTF_REL_5_0]; /*uses intIfNum */
  L7_uint32      numDst;
  L7_BOOL        inUse;
} filterDataV5_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  filterDataV5_t        filterData[L7_MAX_STATIC_FILTER_ENTRIES_REL_5_0];
  filterIntfCfgData_t filterIntfCfgData[L7_FILTER_INTF_MAX_COUNT_REL_5_0];
  L7_uint32           checkSum;                 /* check sum of config file NOTE: needs to be last entry */
} filterCfgDataV5_t;


#endif /*FILTER_MIGRATE_H */
