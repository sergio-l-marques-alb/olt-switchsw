
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename mcast_migrate.h
*
* @purpose multicast Configuration Migration
*
* @component multicast
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef MCAST_MIGRATE_H
#define MCAST_MIGRATE_H

#include "mcast_config.h"
#include "platform_config_migrate.h"
           
#define L7_RTR_MAX_STATIC_MROUTES_REL_4_0                      50
#define L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_0                50


#define L7_RTR_MAX_STATIC_MROUTES_REL_4_1                      50
#define L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_1                50

#define L7_RTR_MAX_STATIC_MROUTES_REL_4_3                      50
#define L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_3                50

#define L7_RTR_MAX_STATIC_MROUTES_REL_4_4                      50
#define L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_4                50

typedef struct
{
  L7_uint32 source;
  L7_uint32 mask;
  L7_uint32 intfNum;
  L7_uint32 rpfAddr;            /* RPF address */
  L7_uint32 preference;             /* Metric */
} L7_mcastMapStaticRtsCfgDataV1_t;


typedef struct 
{
  nimConfigID_t ifConfigId;
  L7_uint32 source;
  L7_uint32 mask;
  L7_uint32 rpfAddr;       /* RPF address */
  L7_uint32 preference;        /* Metric */
} L7_mcastMapStaticRtsCfgDataV2_t;


/* structure to support Adminscope boundaries */
typedef struct
{
  L7_ipUSP_REL_4_0_t ipUSP;
  L7_BOOL inUse;                /* entry in use or not */
  L7_uint32 groupIpAddr;        /* boundary IP address  */
  L7_uint32 groupIpMask;        /* boundary IP mask  */
} L7_mcastMapAdminScopeCfgDataV1_t;


typedef struct 
{
    nimConfigID_t  ifConfigId;
    L7_BOOL        inUse;           /* entry in use or not */
    L7_uint32      groupIpAddr;         /* boundary IP address  */
    L7_uint32      groupIpMask;         /* boundary IP mask  */
} L7_mcastMapAdminScopeCfgDataV2_t;


typedef struct
{
  L7_uint32 mcastAdminMode;
  L7_uint32 currentMcastProtocol;
  L7_mcastMapStaticRtsCfgDataV1_t mcastStaticRtsCfgData[L7_RTR_MAX_STATIC_MROUTES_REL_4_0];
  L7_mcastMapAdminScopeCfgDataV1_t mcastAdminScopeCfgData[L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_0];
  L7_uint32 numStaticMRouteEntries;
  L7_uint32 numAdminScopeEntries;
} L7_mcastMapCfgDataV1_t;

typedef struct
{
  L7_uint32 mcastAdminMode;
  L7_uint32 currentMcastProtocol;
  L7_mcastMapStaticRtsCfgDataV2_t mcastStaticRtsCfgData[L7_RTR_MAX_STATIC_MROUTES_REL_4_0];
  L7_mcastMapAdminScopeCfgDataV2_t mcastAdminScopeCfgData[L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_0];
  L7_uint32 numStaticMRouteEntries;
  L7_uint32 numAdminScopeEntries;
} L7_mcastMapCfgDataV2_t;

typedef struct
{
  L7_uint32 ipMRouteIfTtlThresh;
} L7_mcastMapIfCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_mcastMapCfgDataV1_t rtr;
  L7_mcastMapIfCfgDataV1_t intf[L7_MAX_INTERFACE_COUNT_REL_4_0];
  L7_uint32 checkSum;
} L7_mcastMapCfgV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_mcastMapCfgData_t rtr;
  L7_mcastMapIfCfgData_t intf[L7_IPMAP_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;
} L7_mcastMapCfgV2_t;

typedef struct
{
  L7_fileHdr_t            cfgHdr;
  L7_mcastMapCfgData_t    rtr;
  L7_mcastMapIfCfgData_t  intf[L7_IPMAP_INTF_MAX_COUNT_REL_4_3];
  L7_uint32               checkSum;
} L7_mcastMapCfgV3_t;

typedef struct
{
  L7_fileHdr_t            cfgHdr;
  L7_mcastMapCfgDataV2_t  rtr;
  L7_mcastMapIfCfgData_t  intf[L7_IPMAP_INTF_MAX_COUNT_REL_4_4];
  L7_uint32               checkSum;
} L7_mcastMapCfgV4_t;


typedef L7_mcastMapCfg_t L7_mcastMapCfgV5_t;


/********************************************************************
*                     Function Prototypes
*********************************************************************/
extern void mcastMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, 
                                    L7_char8 * pCfgBuffer);

#endif /* MCAST_MIGRATE_H */
