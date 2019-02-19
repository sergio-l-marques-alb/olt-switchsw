/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename snooping_migrate.h
*
* @purpose Snooping Configuration Migration
*
* @component Snooping
*
* @comments none
*
* @create 08/23/2004
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef SNOOPING_MIGRATE_H
#define SNOOPING_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "snooping_cfg.h"

#define L7_IGMP_SNOOPING_MAX_INTF_REL_4_0  L7_MAX_INTERFACE_COUNT_REL_4_0
#define L7_IGMP_SNOOPING_MAX_INTF_REL_4_1  (L7_MAX_PORT_COUNT_REL_4_1 + L7_MAX_NUM_LAG_INTF_REL_4_1 + 1)
#define L7_IGMP_SNOOPING_MAX_INTF_REL_4_2  (L7_MAX_PORT_COUNT_REL_4_2 + L7_MAX_NUM_LAG_INTF_REL_4_2 + 1)
#define L7_IGMP_SNOOPING_MAX_INTF_REL_4_3  (L7_MAX_PORT_COUNT_REL_4_3 + L7_MAX_NUM_LAG_INTF_REL_4_3 + 1)
#define L7_IGMP_SNOOPING_MAX_INTF_REL_4_4  (L7_MAX_PORT_COUNT_REL_4_4 + L7_MAX_NUM_LAG_INTF_REL_4_4 + 1)

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 snoopAdminMode;     /* Enable/Disable IGMP Snooping */
  L7_uint32 snoopIntfCfg[L7_IGMP_SNOOPING_MAX_INTF_REL_4_0];    /* Ext intf nums configured for Snooping */
  L7_uint32 groupMembershipInterval;    /* Group Membership Interval */
  L7_uint32 responseTime;       /* Max response time */
  L7_uint32 mcastRtrExpiryTime; /* For aging out multicast router entries */
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
}
snoopCfgDataV1_t;

typedef struct
{
  nimConfigID_t configId;
  L7_uint32 intfMode;
} snoopIntfCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 snoopAdminMode;     /* Enable/Disable IGMP Snooping */
  snoopIntfCfgDataV2_t snoopIntfCfgData[L7_IGMP_SNOOPING_MAX_INTF_REL_4_1];
  L7_uint32 groupMembershipInterval;    /* Group Membership Interval */
  L7_uint32 responseTime;       /* Max response time */
  L7_uint32 mcastRtrExpiryTime; /* For aging out multicast router entries */
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
} snoopCfgDataV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 snoopAdminMode;   /* Enable/Disable IGMP Snooping */
  snoopIntfCfgData_t snoopIntfCfgData[L7_IGMP_SNOOPING_MAX_INTF_REL_4_3];  
  L7_ushort16 vlanGroupMembershipInterval[L7_DOT1Q_MAX_VLAN_ID+1];
  L7_ushort16 vlanResponseTime[L7_DOT1Q_MAX_VLAN_ID+1];
  L7_ushort16 vlanMcastRtrExpiryTime[L7_DOT1Q_MAX_VLAN_ID+1];
  L7_uchar8 snoopVlanFlags[L7_DOT1Q_MAX_VLAN_ID+1];  /* Hardcoded array for direct indexing by the VLANID: l7_igmp_snooping_mode_e */
  L7_uint32 checkSum;
} snoopCfgDataV3_t;

typedef struct
{
  L7_fileHdr_t       cfgHdr;
  L7_uint32          snoopAdminMode;   /* Enable/Disable IGMP Snooping */
  snoopIntfCfgData_t snoopIntfCfgData[L7_IGMP_SNOOPING_MAX_INTF];  
  L7_ushort16        vlanGroupMembershipInterval[L7_DOT1Q_MAX_VLAN_ID+1];
  L7_ushort16        vlanResponseTime[L7_DOT1Q_MAX_VLAN_ID+1];
  L7_ushort16        vlanMcastRtrExpiryTime[L7_DOT1Q_MAX_VLAN_ID+1];
  L7_uchar8          snoopVlanFlags[L7_DOT1Q_MAX_VLAN_ID+1];  /* Hardcoded array for direct indexing by the VLANID: l7_igmp_snooping_mode_e */
  L7_uint32          checkSum;      /* check sum of config file NOTE: needs to be last entry */
} snoopCfgDataV4_t;

typedef snoopCfgData_t snoopCfgDataV5_t;
void
snoopMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);
#endif /* SNOOPING_MIGRATE_H */
