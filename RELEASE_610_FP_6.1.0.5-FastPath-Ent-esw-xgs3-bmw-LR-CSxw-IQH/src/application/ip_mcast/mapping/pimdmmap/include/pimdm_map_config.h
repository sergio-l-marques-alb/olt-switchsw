/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_map_config.h
*
* @purpose    PIMDM Mapping layer structures and config data
*
* @component  PIMDM Mapping Layer
*
* @comments   none
*
* @create     02/04/2002
*
* @author     gkiran
* @end
*
**********************************************************************/
#ifndef _PIMDM_CONFIG_H
#define _PIMDM_CONFIG_H

#include "l7_common.h"
#include "l3_default_cnfgr.h"
#include "nimapi.h"
#include "l3_addrdefs.h"

#define L7_PIMDM_CFG_FILENAME    "pimdmMap.cfg"
#define L7_PIMDM6_CFG_FILENAME   "pimdmMap6.cfg"
#define L7_PIMDM_CFG_VER_1       0x1
#define L7_PIMDM_CFG_VER_2       0x2
#define L7_PIMDM_CFG_VER_3       0x3
#define L7_PIMDM_CFG_VER_4       0x4
#define L7_PIMDM_CFG_VER_5       0x5
#define L7_PIMDM_CFG_VER_CURRENT L7_PIMDM_CFG_VER_5

/****************************************
*  PIMDM Global Configuration Data
*****************************************/

typedef struct pimdmCfgData_s
{
   L7_uint32           adminMode;
   L7_uint32           traceMode; /* This is just required for Config Migration.
                                   * Not used in Rel_L implementation.
                                   */
} pimdmCfgData_t;

/****************************************
*  PIMDM Circuit Configuraton Data
*****************************************/
typedef struct pimdmCfgCkt_s
{
  nimConfigID_t       configId;     /* interface configuration correlator */
  L7_uint32           mode;         /* Enable PIMDM    per interface */
  L7_uint32           pimInterfaceHelloInterval; /* Hello interval
                                                    per interface*/
} pimdmCfgCkt_t;

/****************************************
*  PIMDM Configuration File
*****************************************/
typedef struct L7_pimdmMapCfg_s
{
  L7_fileHdr_t        cfgHdr;
  pimdmCfgData_t      rtr;
  pimdmCfgCkt_t       ckt[L7_IPMAP_INTF_MAX_COUNT];
  L7_uint32           checkSum;

} L7_pimdmMapCfg_t;

#endif /* _PIMDM_CONFIG_H */
