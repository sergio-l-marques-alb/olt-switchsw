/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    garp_migrate.h
* @purpose     GARP Configuration Migration
* @component   GARP
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @author      
* @end
*             
**********************************************************************/

#ifndef GARP_MIGRATE_H
#define GARP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "garpcfg.h"
#include "garpapi.h"
#include "garp_dot1q_api.h"

#define L7_GARP_MAX_INTF_REL_4_1 L7_MAX_INTERFACE_COUNT_REL_4_1
#define L7_GARP_MAX_INTF_REL_4_3 L7_MAX_INTERFACE_COUNT_REL_4_3
#define L7_GARP_MAX_INTF_REL_4_4 L7_MAX_INTERFACE_COUNT_REL_4_4

/*-------------------------------------------------------------------------*/
/*   DATA STRUCTURES BEGINNING WITH RELEASE 4.0                            */
/*-------------------------------------------------------------------------*/


typedef struct
{
  L7_uint32 join_time[L7_MAX_INTERFACE_COUNT_REL_4_0];
  L7_uint32 leave_time[L7_MAX_INTERFACE_COUNT_REL_4_0];
  L7_uint32 leaveall_time[L7_MAX_INTERFACE_COUNT_REL_4_0];
  L7_BOOL gvrp_enabled;
  L7_BOOL gmrp_enabled;
}
garpCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t hdr;
  garpCfgDataV1_t cfg;
  L7_uint32 checkSum;
}
garpCfgV1_t;


/*-------------------------------------------------------------------------*/
/*   DATA STRUCTURES CHANGED FROM RELEASE 4.0 TO RELEASE 4.1               */
/*-------------------------------------------------------------------------*/

typedef struct
{
  nimConfigID_t configId;
  L7_uint32     join_time;
  L7_uint32     leave_time;
  L7_uint32     leaveall_time;
} garpIntfCfgDataV2_t;


typedef struct
{
  L7_fileHdr_t hdr;
  L7_BOOL gvrp_enabled;
  L7_BOOL gmrp_enabled;
  garpIntfCfgDataV2_t intf[L7_GARP_MAX_INTF_REL_4_1];
  L7_uint32 checkSum;
} garpCfgV2_t;


/*-------------------------------------------------------------------------*/
/*   DATA STRUCTURES CHANGED FROM RELEASE 4.3 TO RELEASE 4.4               */
/*-------------------------------------------------------------------------*/


typedef garpIntfCfgDataV2_t garpIntfCfgDataV3_t;      

typedef struct
{
  L7_fileHdr_t hdr;
  L7_BOOL gvrp_enabled;
  L7_BOOL gmrp_enabled;
  garpIntfCfgDataV3_t intf[L7_GARP_MAX_INTF_REL_4_3];
  L7_uint32 checkSum;
} garpCfgV3_t;

typedef garpCfg_t garpCfgV4_t;


#endif /* GARP_MIGRATE_H */
