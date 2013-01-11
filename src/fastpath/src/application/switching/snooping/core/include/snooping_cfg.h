/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  snooping_cfg.h
*
* @purpose   Snooping configuration structure definitions
*
* @component Snooping
*
* @comments
*
* @create    10/27/2006
*
* @author    drajendra
*
* @end
*
**********************************************************************/
#ifndef SNOOPING_CFG_H
#define SNOOPING_CFG_H

#include "nimapi.h"
#include "default_cnfgr.h"

#include "dot1q_api.h"

/* Configuration File IGMP */
#define SNOOP_IGMP_CFG_FILENAME         "snoopCfgData.cfg"
#define SNOOP_IGMP_CFG_VER_1            0x1
#define SNOOP_IGMP_CFG_VER_2            0x2
#define SNOOP_IGMP_CFG_VER_3            0x3
#define SNOOP_IGMP_CFG_VER_4            0x4
#define SNOOP_IGMP_CFG_VER_5            0x5
#define SNOOP_IGMP_CFG_VER_CURRENT      SNOOP_IGMP_CFG_VER_5

/* Configuration File MLD */
#define SNOOP_MLD_CFG_FILENAME     "snoopMLDCfgData.cfg"
#define SNOOP_MLD_CFG_VER_1        0x1
#define SNOOP_MLD_CFG_VER_CURRENT  SNOOP_MLD_CFG_VER_1

/* Debug Files IGMP */
#define SNOOP_IGMP_DEBUG_CFG_FILENAME          "snoop_debug.cfg"
#define SNOOP_IGMP_DEBUG_CFG_VER_1              0x1
#define SNOOP_IGMP_DEBUG_CFG_VER_CURRENT        SNOOP_IGMP_DEBUG_CFG_VER_1

/* Debug Files MLD */
#define SNOOP_MLD_DEBUG_CFG_FILENAME       "snoop_mld_debug.cfg"
#define SNOOP_MLD_DEBUG_CFG_VER_1          0x1
#define SNOOP_MLD_DEBUG_CFG_VER_CURRENT    SNOOP_MLD_DEBUG_CFG_VER_1

/* Debug Configuration Data */
typedef struct snoopDebugCfgData_s
{
  L7_BOOL snoopDebugPacketTraceTxFlag;
  L7_BOOL snoopDebugPacketTraceRxFlag;
} snoopDebugCfgData_t;

/* Debug Configuration File Structure */
typedef struct snoopDebugCfg_s
{
  L7_fileHdr_t          hdr;
  snoopDebugCfgData_t   cfg;
  L7_uint32             checkSum;
} snoopDebugCfg_t;

/* Interface Configuration structure */
typedef struct
{
  nimConfigID_t      configId;

  L7_ushort16        groupMembershipInterval; /* Group Membership Interval */
  L7_ushort16        responseTime; /* Max response time */
  L7_uint32          mcastRtrExpiryTime; /* For aging out multicast router entries */
  /* Save space by making these uchar8 */
  L7_uchar8          intfMode;  /* Some combination of l7_igmp_snooping_mode_e */

  L7_uchar8          intfMcastRtrAttached;
  L7_VLAN_MASK_t     vlanStaticMcastRtr;
} snoopIntfCfgData_t;

/* VLAN Configuration structure */
typedef struct snoopVlanQuerierCfgData_s
{
#define SNOOP_QUERIER_MODE                 0x01    /* Querier admin mode */
#define SNOOP_QUERIER_ELECTION_PARTICIPATE 0x10    /* Participate in querier
                                                      election flag */
  L7_uint32      snoopQuerierVlanAdminMode;
  L7_inet_addr_t snoopQuerierVlanAddress;
} snoopVlanQuerierCfgData_t;

typedef struct snoopVlanCfgData_s
{
  L7_ushort16               groupMembershipInterval;
  L7_ushort16               maxResponseTime;
  L7_ushort16               mcastRtrExpiryTime;
  snoopVlanQuerierCfgData_t snoopVlanQuerierCfgData;
} snoopVlanCfgData_t;

/* Querier Configuration Data Structure */
typedef struct snoopQuerierCfgData_s
{
  L7_uint32      snoopQuerierAdminMode;
  L7_inet_addr_t snoopQuerierAddress;
  L7_uint32      snoopQuerierQueryInterval;
  L7_uint32      snoopQuerierExpiryInterval;
  L7_uint32      snoopQuerierVersion;
} snoopQuerierCfgData_t;

/* Persistent Storage Configuration Structure Format */
typedef struct snoopCfgData_s
{
  L7_fileHdr_t       cfgHdr;

  L7_uint32          snoopAdminMode;        /* Enable/Disable IGMP Snooping */
  L7_uint32          snoopAdminIGMPPrio;    /* PTin added: IGMP */
  snoopIntfCfgData_t snoopIntfCfgData[L7_IGMP_SNOOPING_MAX_INTF];
  snoopVlanCfgData_t snoopVlanCfgData[L7_DOT1Q_MAX_VLAN_ID+1];
  snoopQuerierCfgData_t snoopQuerierCfgData;
/* Save space by masking these values with a uchar8 value */
#define SNOOP_VLAN_MODE            0x01   /* Snooping Mode for Vlan */
#define SNOOP_VLAN_FAST_LEAVE_MODE 0x10   /* VLAN fast-leave config */
  L7_uchar8          snoopVlanMode[L7_DOT1Q_MAX_VLAN_ID+1];  /* Hardcoded array for direct indexing by the VLANID:
                  l7_igmp_snooping_mode_e */
  L7_BOOL        igmpv3_tos_rtr_alert_check;

  L7_uint32          checkSum;     /* check sum of config file NOTE: needs to be last entry */
} snoopCfgData_t;


struct snoop_cb_s;
struct snoop_eb_s;

/* Begin Function Prototypes */

L7_RC_t snoopSave(void);
L7_RC_t snoopRestore(void);
L7_BOOL snoopHasDataChanged(void);
void snoopResetDataChanged(void);
L7_RC_t snoopApplyConfigData(struct snoop_cb_s *pSnoopCB);
void    snoopBuildDefaultIntfConfigData(nimConfigID_t *configId,
                                        snoopIntfCfgData_t *pCfg,
                                        struct snoop_cb_s *pSnoopCB);
void    snoopIGMPBuildDefaultIntfConfigData(nimConfigID_t *configId,
                                            snoopIntfCfgData_t *pCfg);
void    snoopIGMPBuildDefaultConfigData(L7_uint32 ver);
void    snoopMLDBuildDefaultIntfConfigData(nimConfigID_t *configId,
                                           snoopIntfCfgData_t *pCfg);
void    snoopMLDBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t snoopDebugSave(void);
L7_RC_t snoopDebugRestore(void);
L7_BOOL snoopDebugHasDataChanged(void);
void    snoopIGMPDebugBuildDefaultConfigData(L7_uint32 ver);
void    snoopMLDDebugBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t snoopApplyDebugConfigData(struct snoop_cb_s *pSnoopCB);

/* End of function Prototypes */
#endif /* SNOOPING_CFG_H */
