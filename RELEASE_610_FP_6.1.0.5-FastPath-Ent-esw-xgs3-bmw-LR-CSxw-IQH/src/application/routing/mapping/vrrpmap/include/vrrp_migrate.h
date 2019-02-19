
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename vrrp_migrate.h
*
* @purpose VRRP Configuration Migration
*
* @component VRRP
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef VRRP_MIGRATE_H
#define VRRP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_vrrpinclude.h"


     
#define L7_VRRP_MAX_AUTH_DATA_REL_4_0        8
#define L7_VRRP_MAX_VIRT_ROUTERS_REL_4_0    20   /* L7_VRRP_MAX_VRRP_CONFIGURATION */

#define L7_VRRP_MAX_AUTH_DATA_REL_4_1        8
#define L7_VRRP_MAX_VIRT_ROUTERS_REL_4_1    20

#define L7_VRRP_MAX_AUTH_DATA_REL_4_2        8
#define L7_VRRP_MAX_VIRT_ROUTERS_REL_4_2    20

#define L7_VRRP_MAX_AUTH_DATA_REL_4_3        8
#define L7_VRRP_MAX_VIRT_ROUTERS_REL_4_3    20


typedef struct
{
  L7_uchar8 vrid;               /* Virtual Router ID           */
  L7_uchar8 priority;           /* Priority                    */
  L7_BOOL preempt_mode;         /* Pre-Empt mode               */
  nimUSP_REL_4_0_t usp;         /* unit-slot-port */
  L7_uchar8 adver_int;          /* Advertisement Interval      */
  L7_AUTH_TYPES_t auth_types;
  L7_uchar8 auth_data[L7_VRRP_MAX_AUTH_DATA_REL_4_0];
  L7_IP_ADDR_t ipaddress;
  L7_vrrpStartState_t vr_start_state;
}
vrrpCfgCktV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  vrrpCfgData_t rtr;
  vrrpCfgCktV1_t ckt[L7_VRRP_MAX_VIRT_ROUTERS_REL_4_0];
  L7_uint32 checkSum;
}
L7_vrrpMapCfgV1_t;

typedef struct
{
  nimConfigID_t configId;
  L7_uchar8 vrid;               /* Virtual Router ID           */
  L7_uchar8 priority;           /* Priority                    */
  L7_BOOL preempt_mode;         /* Pre-Empt mode               */
  L7_uchar8 adver_int;          /* Advertisement Interval      */
  L7_AUTH_TYPES_t auth_types;
  L7_uchar8 auth_data[L7_VRRP_MAX_AUTH_DATA_REL_4_3];
  L7_IP_ADDR_t ipaddress;       /* Virtual router's IP address */
  L7_vrrpStartState_t vr_start_state;
} vrrpVirtRouterCfgV2_t;
typedef vrrpVirtRouterCfg_t vrrpVirtRouterCfgV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  vrrpCfgData_t rtr;
  vrrpVirtRouterCfgV2_t virtRouter[L7_VRRP_MAX_VIRT_ROUTERS_REL_4_3];
  L7_uint32 checkSum;
} L7_vrrpMapCfgV2_t;

typedef L7_vrrpMapCfg_t L7_vrrpMapCfgV3_t;

#endif /* VRRP_MIGRATE_H */
