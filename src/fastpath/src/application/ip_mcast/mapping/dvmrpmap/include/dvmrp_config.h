/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dvmrp_config.h
*
* @purpose    DVMRP Mapping layer structures and config data
*
* @component  DVMRP Mapping Layer
*
* @comments   none
*
* @create     02/04/2002
*
* @author     M Pavan K Chakravarthi
*             gkiran
* @end
*
**********************************************************************/
#ifndef _DVMRP_CONFIG_H
#define _DVMRP_CONFIG_H
#include "l7_common.h"

#define L7_DVMRP_CFG_FILENAME   "dvmrpMap.cfg"
#define L7_DVMRP_CFG_VER_1       0x1
#define L7_DVMRP_CFG_VER_2       0x2
#define L7_DVMRP_CFG_VER_3       0x3
#define L7_DVMRP_CFG_VER_4       0x4
#define L7_DVMRP_CFG_VER_5       0x5
#define L7_DVMRP_CFG_VER_CURRENT   L7_DVMRP_CFG_VER_5

/****************************************
*  DVMRP Global Configuration Data      *
*****************************************/

typedef struct dvmrpCfgData_s
{
  L7_uint32          adminMode;
} dvmrpCfgData_t;

/****************************************
*  DVMRP Circuit Configuraton Data      *
*****************************************/

typedef struct dvmrpCfgCkt_s
{
  nimConfigID_t       configId;
  L7_uint32           intfAdminMode; /* Enable DVMRP per interface */
  L7_ushort16         intfMetric;    /* Metric per interface */

} dvmrpCfgCkt_t;

/****************************************
*  DVMRP Configuration File             *
*****************************************/

typedef struct L7_dvmrpMapCfg_s
{
  L7_fileHdr_t        cfgHdr;
  dvmrpCfgData_t      rtr;
  dvmrpCfgCkt_t       ckt[L7_IPMAP_INTF_MAX_COUNT]; 
  L7_uint32           checkSum;

} L7_dvmrpMapCfg_t;

#endif /* _DVMRP_CONFIG_H*/
