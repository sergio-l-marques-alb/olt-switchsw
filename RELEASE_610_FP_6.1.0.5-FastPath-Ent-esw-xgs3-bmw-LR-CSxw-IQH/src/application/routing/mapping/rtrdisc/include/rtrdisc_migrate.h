
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename rtrdisc_migrate.h
*
* @purpose rtrdisc Configuration Migration
*
* @component rtrdisc
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef RTRDISC_MIGRATE_H
#define RTRDISC_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"
#include "rtrdisc.h"
#include "rtrdisc_config.h"
#include "rtrdisc_cnfgr.h"
#include "rtrdisc_util.h"

#define L7_RTR_DISC_INTF_MAX_COUNT_REL_4_1  L7_IPMAP_INTF_MAX_COUNT_REL_4_1
#define L7_RTR_DISC_INTF_MAX_COUNT_REL_4_2  L7_IPMAP_INTF_MAX_COUNT_REL_4_2 
#define L7_RTR_DISC_INTF_MAX_COUNT_REL_4_3  L7_IPMAP_INTF_MAX_COUNT_REL_4_3 
#define L7_RTR_DISC_INTF_MAX_COUNT_REL_4_4  L7_IPMAP_INTF_MAX_COUNT_REL_4_4 



typedef struct
{
  L7_IP_ADDR_t advAddress;
  L7_uint32 maxAdvInt;
  L7_uint32 minAdvInt;
  L7_uint32 advLifetime;
  L7_BOOL advertise;            /* flag indicating whether address is to be advertised */
  L7_int32 preferenceLevel;     /* the prefarability of each router address */
}
rtrDiscIntfCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  rtrDiscIntfCfgDataV1_t rtrDiscData[L7_MAX_SLOTS_PER_BOX_REL_4_0]
    [L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;

}
rtrDiscCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  rtrDiscIntfCfgData_t rtrDiscIntfCfgData[L7_RTR_DISC_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;

} rtrDiscCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  rtrDiscIntfCfgData_t rtrDiscIntfCfgData[L7_RTR_DISC_INTF_MAX_COUNT_REL_4_3];
  L7_uint32 checkSum;

} rtrDiscCfgDataV3_t;

typedef struct
{
        L7_fileHdr_t            cfgHdr;
        rtrDiscIntfCfgData_t    rtrDiscIntfCfgData[L7_RTR_DISC_INTF_MAX_COUNT_REL_4_4];
        L7_uint32               checkSum;

} rtrDiscCfgDataV4_t;

typedef rtrDiscCfgData_t rtrDiscCfgDataV5_t;

#endif /* RTRDISC_MIGRATE_H */
