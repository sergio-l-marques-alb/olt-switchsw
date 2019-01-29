
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename std_policy_migrate.h
*
* @purpose Configuration Migration
*
* @component policy
*
* @comments none
*
* @create 8/23/2004
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef STD_POLICY_MIGRATE_H
#define STD_POLICY_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "std_policy.h"

#define L7_POLICY_INTF_MAX_COUNT_REL_4_1 (L7_MAX_PORT_COUNT_REL_4_1 + 1)
#define L7_POLICY_INTF_MAX_COUNT_REL_4_2 (L7_MAX_PORT_COUNT_REL_4_3 + 1)
#define L7_POLICY_INTF_MAX_COUNT_REL_4_3 (L7_MAX_PORT_COUNT_REL_4_3 + 1)
#define L7_POLICY_INTF_MAX_COUNT_REL_4_4 (L7_MAX_PORT_COUNT_REL_4_4 + 1)

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 systemFlowControlMode;
  L7_uint32 systemBcastStormMode;
  L7_uint32 systemBcastStormModeHigh;
  L7_uint32 systemBcastStormModeLow;

  L7_uint32 flowControlMode[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 bcastStormMode[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 bcastStormHighLimit[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 bcastStormLowLimit[L7_MAX_SLOTS_PER_BOX_REL_4_0][L7_MAX_PORTS_PER_SLOT_REL_4_0 + 1];
  L7_uint32 checkSum;
}
policyCfgDataV1_t;

typedef struct
{
  nimConfigID_t  configId;
  L7_uint32      flowControlMode;
  L7_uint32      bcastStormMode;
  L7_uint32      bcastStormHighLimit;
  L7_uint32      bcastStormLowLimit;
} policyIntfCfgDataV2V3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 systemFlowControlMode;
  L7_uint32 systemBcastStormMode;
  L7_uint32 systemBcastStormModeHigh;
  L7_uint32 systemBcastStormModeLow;
  policyIntfCfgDataV2V3_t policyIntfCfgData[L7_POLICY_INTF_MAX_COUNT_REL_4_1];
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
} policyCfgDataV2_t;


typedef struct
{
  L7_fileHdr_t   cfgHdr; 
  L7_uint32      systemFlowControlMode;
  L7_uint32      systemBcastStormMode;
  L7_uint32      systemBcastStormModeHigh;
  L7_uint32      systemBcastStormModeLow;
  policyIntfCfgDataV2V3_t policyIntfCfgData[L7_POLICY_INTF_MAX_COUNT_REL_4_3];
  L7_uint32      checkSum;    /* check sum of config file NOTE: needs to be last entry */
} policyCfgDataV3_t;

typedef struct
{
  nimConfigID_t  configId;
  L7_uint32      flowControlMode;
  L7_uint32      bcastStormMode;
  L7_uint32      bcastStormThreshold;
  L7_uint32      bcastStormBurstSize;           /* PTin added: stormcontrol */
  L7_uint32      mcastStormMode;
  L7_uint32      mcastStormThreshold;
  L7_uint32      mcastStormBurstSize;           /* PTin added: stormcontrol */
  L7_uint32      ucastStormMode;
  L7_uint32      ucastStormThreshold;
  L7_uint32      ucastStormBurstSize;           /* PTin added: stormcontrol */
} policyIntfCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr; 
  L7_uint32      systemFlowControlMode;
  L7_uint32      systemBcastStormMode;
  L7_uint32      systemBcastStormThreshold;
  L7_uint32      systemBcastStormBurstSize;     /* PTin added: stormcontrol */
  L7_uint32      systemMcastStormMode;
  L7_uint32      systemMcastStormThreshold;
  L7_uint32      systemMcastStormBurstSize;     /* PTin added: stormcontrol */
  L7_uint32      systemUcastStormMode;
  L7_uint32      systemUcastStormThreshold;
  L7_uint32      systemUcastStormBurstSize;     /* PTin added: stormcontrol */
  policyIntfCfgDataV4_t policyIntfCfgData[L7_POLICY_INTF_MAX_COUNT];
  L7_uint32      checkSum;    /* check sum of config file NOTE: needs to be last entry */
} policyCfgDataV4_t;

typedef policyCfgData_t policyCfgDataV5_t;

#endif /* STD_POLICY_MIGRATE_H */
