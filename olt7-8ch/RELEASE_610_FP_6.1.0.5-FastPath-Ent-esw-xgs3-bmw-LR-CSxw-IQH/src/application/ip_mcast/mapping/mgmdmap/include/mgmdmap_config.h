/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   mgmd_config.h
*
* @purpose    MGMD Mapping layer structures and config data
*
* @component  MGMD Mapping Layer
*
* @comments   none
*
* @create     02/04/2002
*
* @author     ramakrishna
*             gkiran
* @end
*
**********************************************************************/
#ifndef _INCLUDE_MGMD_MAP_CONFIG_H
#define _INCLUDE_MGMD_MAP_CONFIG_H

#include "l7_common.h"

#define L7_IGMP_CFG_FILENAME  "igmpMap.cfg"
#define L7_MLD_CFG_FILENAME   "mldMap.cfg"
#define L7_MGMD_CFG_VER_1      0x1
#define L7_MGMD_CFG_VER_2      0x2
#define L7_MGMD_CFG_VER_3      0x3
#define L7_MGMD_CFG_VER_4      0x4
#define L7_MGMD_CFG_VER_5      0x5
#define L7_MGMD_CFG_VER_CURRENT  L7_MGMD_CFG_VER_5

/****************************************
*
*  MGMD Global Configuration Data
*
*****************************************/
typedef struct mgmdIntfCfgData_s
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
  L7_uchar8 entryStatus; /* in-use, deleted, free*/
  L7_uint32 interfaceMode;
  L7_uint32 unsolicitedReportInterval;
}mgmdIntfCfgData_t;

/*****************************************
*
*  MGMD configuration file
*******************************************/

typedef struct L7_mgmdMapCfg_s
{
  L7_fileHdr_t      cfgHdr;
  L7_uint32         adminMode;
  L7_BOOL           checkRtrAlert;      /* this flag to be used only on recption of 
                                           IGMP control packets as this validation 
                                           is mandatory for MLD packets  */
  mgmdIntfCfgData_t mgmdIntf[L7_IPMAP_INTF_MAX_COUNT];
  L7_uint32         checkSum;
}L7_mgmdMapCfg_t;

#endif /* _INCLUDE_MGMD_MAP_CONFIG_H */
