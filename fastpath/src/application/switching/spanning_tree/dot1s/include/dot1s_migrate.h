
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dot1s_migrate.h
*
* @purpose   Multiple Spanning tree configuration migration
*
* @component dot1s
*
* @comments  none
*
* @create    08/24/2004
*
* @author    Rama Sasthri, Kristipati
*
* @end
*             
**********************************************************************/

#ifndef DOT1S_MIGRATE_H
#define DOT1S_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "dot1s_include.h"

#define L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_0           (L7_MAX_INTERFACE_COUNT_REL_4_0 + 1)
#define L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_1           (L7_MAX_INTERFACE_COUNT_REL_4_1 + 1)
#define L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_2           (L7_MAX_INTERFACE_COUNT_REL_4_2 + 1)
#define L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_3           (L7_MAX_INTERFACE_COUNT_REL_4_3 + 1)
#define L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_4           (L7_MAX_INTERFACE_COUNT_REL_4_4 + 1)


#define L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_0			(4)
#define L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1			(4)
#define	L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_2			(4)
#define L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3			(4)
#define L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_4			L7_MAX_MULTIPLE_STP_INSTANCES

typedef struct
{
  L7_uint32 ExternalPortPathCost;
  L7_uint32 InternalPortPathCost;
  L7_BOOL autoInternalPortPathCost;
  DOT1S_PORTID_t portId;
} DOT1S_PORT_PER_INST_CFG_V1_t;

typedef struct
{
  L7_uint32 portNum;
  L7_uint32 adminEdge;
  L7_uint32 portAdminMode;
  L7_uint32 HelloTime;
  DOT1S_PORT_PER_INST_CFG_V1_t portInstInfo[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_0 + 1];
}
DOT1S_PORT_COMMON_CFG_V1_t;

typedef struct dot1s_instance_v1_s
{
 DOT1S_CIST_t 		cist;

 DOT1S_MSTI_t		msti[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_0 + 1]; /*do not use index 0*/

}DOT1S_INSTANCE_INFO_V1_t;

typedef struct
{
  DOT1S_BRIDGE_t dot1sBridge;
  DOT1S_INSTANCE_INFO_V1_t dot1sInstance;
  DOT1S_PORT_COMMON_CFG_V1_t dot1sCfgPort[L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_0];
  DOT1S_INSTANCE_MAP_t dot1sInstanceMap[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_0 + 1];
  DOT1S_INST_VLAN_MAP_t dot1sInstVlanMap[L7_MAX_VLAN_ID + 2];
}
dot1sCfgDataV1_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1sCfgDataV1_t cfg;
  L7_uint32 checkSum;
}
dot1sCfgV1_t;

typedef struct
{
  L7_uint32 ExternalPortPathCost;
  L7_uint32 InternalPortPathCost;
  L7_BOOL autoInternalPortPathCost;
  DOT1S_PORTID_t portPriority;
} DOT1S_PORT_PER_INST_CFG_V2_t;

typedef struct
{
  nimConfigID_t configId;
  L7_uint32 adminEdge;
  L7_uint32 portAdminMode;
  L7_uint32 HelloTime;
  DOT1S_PORT_PER_INST_CFG_V2_t portInstInfo[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1 + 1];
} DOT1S_PORT_COMMON_CFG_V2_t;

typedef struct dot1s_instance_cfg_v2_s
{
  DOT1S_CIST_CFG_t cist;
  DOT1S_MSTI_CFG_t msti[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1 + 1];     /*do not use index 0 */
} DOT1S_INSTANCE_INFO_CFG_V2_t;

typedef struct 
{
  L7_BOOL Mode;
  DOT1S_FORCE_VERSION_t ForceVersion;
  L7_uint32 FwdDelay;
  L7_uint32 TxHoldCount;
  L7_uint32 MigrateTime;        /*in secs */
  L7_uint32 MaxHops;
  L7_uint32 instanceCount;
  DOT1S_MST_CONFIG_ID_CFG_t MstConfigId;
} DOT1S_BRIDGE_CFG_V1_t;

typedef struct
{
  DOT1S_BRIDGE_CFG_V1_t dot1sBridge;
  DOT1S_INSTANCE_INFO_CFG_V2_t dot1sInstance;
  DOT1S_PORT_COMMON_CFG_V2_t dot1sCfgPort[L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_1];
  DOT1S_INSTANCE_MAP_t dot1sInstanceMap[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1 + 1];
  DOT1S_INST_VLAN_MAP_t dot1sInstVlanMap[L7_MAX_VLAN_ID + 2];
} dot1sCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1sCfgDataV2_t cfg;
  L7_uint32 checkSum;
}
dot1sCfgV2_t;

typedef struct dot1s_instance_cfg_v3_s
{
  DOT1S_CIST_CFG_t cist;
  DOT1S_MSTI_CFG_t msti[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3 + 1];     /*do not use index 0 */
} DOT1S_INSTANCE_INFO_CFG_V3_t;


typedef struct portCommon_v3_s
{
  nimConfigID_t configId;
  L7_uint32 adminEdge;
  L7_uint32 portAdminMode;
  L7_uint32 HelloTime;
  DOT1S_PORT_PER_INST_CFG_t portInstInfo[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3 + 1];
} DOT1S_PORT_COMMON_CFG_V3_t;

typedef struct
{
  DOT1S_BRIDGE_CFG_V1_t dot1sBridge;
  DOT1S_INSTANCE_INFO_CFG_V3_t dot1sInstance;
  DOT1S_PORT_COMMON_CFG_V3_t dot1sCfgPort[L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_3];
  DOT1S_INSTANCE_MAP_t dot1sInstanceMap[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3 + 1];
  DOT1S_INST_VLAN_MAP_t dot1sInstVlanMap[L7_MAX_VLAN_ID + 2];
} dot1sCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1sCfgDataV3_t cfg;
  L7_uint32 checkSum;
} dot1sCfgV3_t;

typedef struct 
{
  nimConfigID_t configId;
  L7_uint32 adminEdge;
  L7_uint32 portAdminMode;
  L7_uint32 HelloTime;
  DOT1S_PORT_PER_INST_CFG_t portInstInfo[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_4 + 1];
} DOT1S_PORT_COMMON_CFG_V4_t;

typedef struct
{
  DOT1S_BRIDGE_CFG_V1_t dot1sBridge;
  DOT1S_INSTANCE_INFO_CFG_t dot1sInstance;
  DOT1S_PORT_COMMON_CFG_V4_t dot1sCfgPort[L7_DOT1S_MAX_INTERFACE_COUNT];
  DOT1S_INSTANCE_MAP_t dot1sInstanceMap[L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_4 + 1];
  DOT1S_INST_VLAN_MAP_t dot1sInstVlanMap[L7_MAX_VLAN_ID + 2];
} dot1sCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1sCfgDataV4_t cfg;
  L7_uint32 checkSum;
} dot1sCfgV4_t;


typedef dot1sCfg_t dot1sCfgV5_t;

#endif /* DOT1S_MIGRATE_H */
