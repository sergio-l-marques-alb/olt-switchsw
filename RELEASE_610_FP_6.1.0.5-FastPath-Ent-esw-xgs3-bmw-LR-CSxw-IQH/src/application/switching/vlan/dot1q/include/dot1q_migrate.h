/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename dot1q_migrate.h
*
* @purpose dot1q Configuration Migration
*
* @component dot1q
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef DOT1Q_MIGRATE_H
#define DOT1Q_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "dot1q.h"

#define L7_DOT1Q_MAX_INTF_REL_4_0 (L7_MAX_INTERFACE_COUNT_REL_4_0 + 1)
#define L7_DOT1Q_MAX_INTF_REL_4_1 (L7_MAX_INTERFACE_COUNT_REL_4_1 + 1)
#define L7_DOT1Q_MAX_INTF_REL_4_2 (L7_MAX_INTERFACE_COUNT_REL_4_2 + 1)
#define L7_DOT1Q_MAX_INTF_REL_4_3 (L7_MAX_INTERFACE_COUNT_REL_4_3 + 1)
#define L7_DOT1Q_MAX_INTF_REL_4_4 (L7_MAX_PORT_COUNT_REL_4_4 + L7_MAX_NUM_LAG_INTF_REL_4_4 +1)

/* Number of bytes in mask */
#define DOT1Q_VLAN_INDICES_REL_4_X   ((L7_DOT1Q_MAX_VLAN_ID - 1) / (sizeof(L7_uchar8) * 8) + 1)
#define DOT1Q_VLAN_MAX_MASK_BIT_REL_4_X   L7_DOT1Q_MAX_VLAN_ID

/* Interface storage */
typedef struct
{
  L7_uchar8   value[DOT1Q_VLAN_INDICES_REL_4_X];
} DOT1Q_VLAN_MASK_REL_4_X_t;


/*-------------------------------------------------------------------------*/
/*   DATA STRUCTURES BEGINNING WITH RELEASE 4.0                            */
/*-------------------------------------------------------------------------*/

/* 802.1Q Port Configuration and Operational Structure */
typedef struct
{
  L7_uint32 VID[DOT1Q_MAX_PVID];        /* 1-4094 valid, 4095 reserved */
  L7_uint32 acceptFrameType;    /* Admit All Frames or Admit VLAN_Tagged only */
  L7_uint32 enableIngressFiltering;     /* Enable/Disable */
  dot1p_prio_t priority;        /* 802.1p port priority */
  L7_uint32 gvrp_enabled;
  L7_uint32 gmrp_enabled;
  L7_uint32 portCapabilities;
}
dot1q_Qports_t;

/* bitmasks, used to save memory */

typedef struct
{
  NIM_INTF_MASK_REL_4_0_t forwardAllMask;       /* Forward All Groups           */
  NIM_INTF_MASK_REL_4_0_t forwardUnregMask;     /* Filter Unregistered Groups   */
}
groupFilterSetV2_t;

typedef struct
{
  L7_uint32 inUse;
  L7_uint32 vlanID;             /* 1-4094 valid, 4095 for implementation use */
  L7_uchar8 name[L7_MAX_VLAN_NAME];     /* User-defined name                         */
  L7_uint32 fdbID;              /* Filtering Database ID                     */

  NIM_INTF_MASK_REL_4_0_t staticEgressPorts;
  NIM_INTF_MASK_REL_4_0_t forbiddenEgressPorts;
  NIM_INTF_MASK_REL_4_0_t taggedSet;

  groupFilterSetV2_t groupFilter;
  L7_uint32 flags;
}
QvlanCfg_t;

typedef struct
{
  QvlanCfg_t Qvlan[L7_MAX_VLANS_REL_4_0];
  dot1q_Qports_t Qports[L7_DOT1Q_MAX_INTF_REL_4_0];
}
dot1qCfgDataVer2_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1qCfgDataVer2_t cfg;
  L7_uint32 checkSum;
}
dot1qCfgV2_t;

/*-------------------------------------------------------------------------*/
/*   DATA STRUCTURES CHANGED FROM RELEASE 4.0 TO RELEASE 4.1               */
/*-------------------------------------------------------------------------*/


typedef struct
{
  NIM_INTF_MASK_REL_4_1_t forwardAllMask;       /* Forward All Groups           */
  NIM_INTF_MASK_REL_4_1_t forwardUnregMask;     /* Filter Unregistered Groups   */
}
groupFilterSetV3_t;


/* DOT1Q port configuration */
typedef struct
{
  nimConfigID_t   configId;
  L7_uint32       VID[DOT1Q_MAX_PVID];           /* 1-4094 valid, 4095 reserved */
  L7_uint32       acceptFrameType;               /* Admit All Frames or Admit VLAN_Tagged only*/
  L7_uint32       enableIngressFiltering;        /* Enable/Disable*/
  dot1p_prio_t    priority;                      /* 802.1p port priority */           
  L7_uint32       gvrp_enabled;
  L7_uint32       gmrp_enabled;
  L7_uint32       portCapabilities;              
} dot1qIntfCfgDataV3_t;


typedef struct
{
  L7_uint32 inUse;
  L7_uint32 vlanID;             /* 1-4094 valid, 4095 for implementation use */
  L7_uchar8 name[L7_MAX_VLAN_NAME];     /* User-defined name                         */
  L7_uint32 fdbID;              /* Filtering Database ID                     */

  NIM_INTF_MASK_REL_4_1_t staticEgressPorts;
  NIM_INTF_MASK_REL_4_1_t forbiddenEgressPorts;
  NIM_INTF_MASK_REL_4_1_t taggedSet;

  groupFilterSetV3_t groupFilter;
  L7_uint32 flags;
}
dot1qVlanCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1qIntfCfgDataV3_t Qports[L7_DOT1Q_MAX_INTF_REL_4_1];
  dot1qVlanCfgDataV3_t Qvlan[L7_MAX_VLANS_REL_4_1];
  DOT1Q_VLAN_MASK_REL_4_X_t QvlanIntf;
  L7_uint32 checkSum;
} dot1qCfgV3_t;


/*-------------------------------------------------------------------------*/
/*   DATA STRUCTURES CHANGED FROM RELEASE 4.3 TO RELEASE 4.4               */
/*-------------------------------------------------------------------------*/

typedef struct
{
  NIM_INTF_MASK_REL_4_3_t forwardAllMask;
  NIM_INTF_MASK_REL_4_3_t forwardUnregMask;
}
groupFilterSetV4_t;

typedef dot1qIntfCfgDataV3_t dot1qIntfCfgDataV4_t;      

typedef struct
{
  L7_uint32 inUse;
  L7_uint32 vlanID;
  L7_uchar8 name[L7_MAX_VLAN_NAME];
  L7_uint32 fdbID;

  NIM_INTF_MASK_REL_4_3_t staticEgressPorts;
  NIM_INTF_MASK_REL_4_3_t forbiddenEgressPorts;
  NIM_INTF_MASK_REL_4_3_t taggedSet;

  groupFilterSetV4_t groupFilter;
  L7_uint32 flags;      
} dot1qVlanCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t hdr;
  dot1qGlobCfgData_t Qglobal;
  dot1qIntfCfgDataV4_t Qports[L7_DOT1Q_MAX_INTF_REL_4_3];
  dot1qVlanCfgDataV4_t Qvlan[L7_MAX_VLANS_REL_4_3];
  DOT1Q_VLAN_MASK_REL_4_X_t QvlanIntf;
  L7_uint32 checkSum;
} dot1qCfgV4_t;
typedef struct dot1q_vlan_cfg_V5_s
{
  L7_uchar8         name[L7_MAX_VLAN_NAME];     /* User-defined name     */  
  NIM_INTF_MASK_REL_4_4_t   staticEgressPorts;
  NIM_INTF_MASK_REL_4_4_t   forbiddenEgressPorts;
  NIM_INTF_MASK_REL_4_4_t   taggedSet;
#if DOT1Q_FUTURE_FUNC_GROUP_FILTER
  groupFilterSet_t  groupFilter;
#endif

} dot1q_vlan_cfgV5_t;
typedef struct dot1qVlanCfgDataV5_s
{
  L7_uint32   vlanId;
  dot1q_vlan_cfgV5_t vlanCfg;
      
} dot1qVlanCfgDataV5_t;
typedef struct
{
  L7_fileHdr_t hdr;
  dot1qGlobCfgData_t Qglobal;
  dot1qIntfCfgData_t Qports[L7_DOT1Q_MAX_INTF_REL_4_4 + 1];
  dot1qVlanCfgDataV5_t Qvlan[L7_MAX_VLANS_REL_4_4 + 1];
  DOT1Q_VLAN_MASK_REL_4_X_t QvlanIntf;
  L7_uint32          traceFlags;
  L7_uint32 checkSum;
} dot1qCfgV5_t;


/* Entire DOT1Q (port+vlan) configuration file */


/*-------------------------------------------------------------------------*/
/*   TYPEDEFS  OF  VERSIONS                                                */
/*-------------------------------------------------------------------------*/

typedef dot1qCfgV2_t dot1qCfgDataV2_t;          /* As of release 4.0 */
typedef dot1qCfgV3_t dot1qCfgDataV3_t;          /* As of release 4.1 */
typedef dot1qCfgV4_t dot1qCfgDataV4_t;          /* As of release 4.3 */
typedef dot1qCfgV5_t dot1qCfgDataV5_t;          /* As of release 4.4 */
typedef dot1qCfgData_t dot1qCfgDataV6_t;        /* As of release 5.0 */
void dot1qMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);



#endif /* DOT1Q_MIGRATE_H */
