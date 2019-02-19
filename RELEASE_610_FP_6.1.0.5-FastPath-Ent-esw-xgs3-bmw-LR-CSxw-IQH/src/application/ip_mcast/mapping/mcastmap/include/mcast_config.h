/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  mcast_config.h
*
* @purpose   MCAST vendor-specific function prototypes
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    05/17/2002
*
* @author    Vemula & Srusti
*
* @end
*
**********************************************************************/

#ifndef _MCAST_CONFIG_H_
#define _MCAST_CONFIG_H_

#include "l7_common.h"
#include "nimapi.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_default_cnfgr.h"

#define L7_MCAST_CFG_FILENAME  "mcastMap.cfg"
#define L7_MCAST_CFG_VER_0        0x0
#define L7_MCAST_CFG_VER_1        0x1
#define L7_MCAST_CFG_VER_2        0x2
#define L7_MCAST_CFG_VER_3        0x3
#define L7_MCAST_CFG_VER_4        0x4
#define L7_MCAST_CFG_VER_5        0x5
#define L7_MCAST_CFG_VER_CURRENT   L7_MCAST_CFG_VER_5


typedef struct L7_mcastMapStaticRtsCfgData_s
{
  nimConfigID_t  ifConfigId;    /* RPF Interface */
  L7_inet_addr_t source;        /* Source Address */
  L7_inet_addr_t mask;          /* Source Address Mask */
  L7_inet_addr_t rpfAddr;       /* RPF address */
  L7_uint32      preference;    /* Metric Preference*/
} L7_mcastMapStaticRtsCfgData_t;

   /* structure to support Adminscope boundaries */
typedef struct L7_mcastMapAdminScopeCfgData_s
{
    nimConfigID_t  ifConfigId; 
    L7_BOOL        inUse;           /* entry in use or not */
    L7_inet_addr_t groupIpAddr;         /* boundary IP address  */
    L7_inet_addr_t groupIpMask;         /* boundary IP mask  */
} L7_mcastMapAdminScopeCfgData_t;


typedef struct L7_mcastMapCfgData_s
{
  L7_uint32 mcastAdminMode;
  L7_uint32 currentMcastProtocol;
  L7_uint32 currentMcastV6Protocol;
  L7_mcastMapStaticRtsCfgData_t mcastStaticRtsCfgData[L7_RTR_MAX_STATIC_MROUTES];
  L7_mcastMapAdminScopeCfgData_t mcastAdminScopeCfgData[L7_MCAST_MAX_ADMINSCOPE_ENTRIES];
  L7_uint32 numStaticMRouteEntries;
  L7_uint32 numAdminScopeEntries;
} L7_mcastMapCfgData_t;

typedef struct L7_mcastMapIfCfgData_s
{
  nimConfigID_t configId;
  L7_uint32  ipMRouteIfTtlThresh;
} L7_mcastMapIfCfgData_t;

typedef struct L7_mcastMapCfg_s
{
  L7_fileHdr_t            cfgHdr;
  L7_mcastMapCfgData_t    rtr;
  L7_mcastMapIfCfgData_t  intf[L7_IPMAP_INTF_MAX_COUNT];
  L7_uint32               checkSum;
} L7_mcastMapCfg_t;


#endif /* _MCAST_CONFIG_H_ */
