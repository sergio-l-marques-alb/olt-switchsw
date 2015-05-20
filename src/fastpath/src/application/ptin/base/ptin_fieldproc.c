/**
 * ptin_fieldproc.c
 *  
 * Implements bandwidth profiles and counters at request, using 
 * field processor rules 
 *
 * Created on: 2011/0913 
 * Author: Milton Ruas (milton-r-silva@ptinovacao.pt) 
 *  
 * Notes:
 */

#include "ptin_fieldproc.h"
#include "dtl_ptin.h"
#include "ptin_intf.h"

/**
 * BW POLICERS MANAGEMENT
 */

/**
 * Create a new BW policer
 *  
 * @param profile : BW profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_create(ptin_bw_meter_t *meter, L7_int *policer_id)
{
  ptin_bwPolicer_t bwPolicer;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (meter==L7_NULLPTR || policer_id==L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid arguments");
    return L7_FAILURE;
  }

  memset(&bwPolicer,0x00,sizeof(ptin_bwPolicer_t));

  bwPolicer.operation  = PTIN_CMD_CREATE;
  bwPolicer.meter      = *meter;
  bwPolicer.policer_id = *policer_id;

  rc = dtlPtinBWPolicer(L7_ALL_INTERFACES, &bwPolicer);

  /* Return result */
  if (rc == L7_SUCCESS)
  {
    *policer_id = bwPolicer.policer_id;
  }

  return rc;
}

/**
 * Destroy a BW policer
 *  
 * @param profile : BW profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_destroy(L7_int policer_id)
{
  ptin_bwPolicer_t bwPolicer;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (policer_id <= 0)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid arguments (policer_id=%d)", policer_id);
    return L7_FAILURE;
  }

  memset(&bwPolicer,0x00,sizeof(ptin_bwPolicer_t));

  bwPolicer.operation  = PTIN_CMD_DESTROY;
  bwPolicer.policer_id = policer_id;

  rc = dtlPtinBWPolicer(L7_ALL_INTERFACES, &bwPolicer);

  return rc;
}

/**
 * Get BW policer
 * 
 * @param profile : BE policer profile 
 * @param meter   : Policer meter
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_get(ptin_bw_profile_t *profile, ptin_bw_meter_t *meter)
{
  L7_uint32 intIfNum;
  ptin_bwPolicer_t bwPolicer;
  L7_RC_t rc;

  /* Validate profile */
  if (profile == L7_NULLPTR || meter == L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
    {
      LOG_PT_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
      return L7_FAILURE;
    }
  }
  else
  {
    intIfNum = L7_ALL_INTERFACES;
  }

  memset(&bwPolicer,0x00,sizeof(ptin_bwPolicer_t));

  bwPolicer.operation  = DAPI_CMD_GET;
  bwPolicer.profile    = *profile;
  bwPolicer.policer_id = -1;
  bwPolicer.policy_ptr = L7_NULLPTR;

  rc = dtlPtinBWPolicer(intIfNum, &bwPolicer);

  /* Copy meter data */
  if (rc == L7_SUCCESS)
  {
    memcpy(meter, &bwPolicer.meter, sizeof(ptin_bw_meter_t));
  }

  return rc;
}

/**
 * Add a new BW policer
 *  
 * @param profile : BW profile 
 * @param meter   : Policer meter 
 * @param policer_id : Policer id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_set(ptin_bw_profile_t *profile, ptin_bw_meter_t *meter, L7_int policer_id)
{
  L7_uint32 intIfNum;
  ptin_bwPolicer_t bwPolicer;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (profile==L7_NULLPTR || meter==L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS) 
    {
      LOG_PT_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
      return L7_FAILURE;
    }
  }
  else
  {
    intIfNum = L7_ALL_INTERFACES;
  }

  memset(&bwPolicer,0x00,sizeof(ptin_bwPolicer_t));

  bwPolicer.operation  = DAPI_CMD_SET;
  bwPolicer.profile    = *profile;
  bwPolicer.meter      = *meter;
  bwPolicer.policer_id = policer_id;
  bwPolicer.policy_ptr = L7_NULLPTR;

  rc = dtlPtinBWPolicer(intIfNum, &bwPolicer);

  return rc;
}

/**
 * Remove an existent BW policer
 *  
 * @param profile : BW profile 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_delete(ptin_bw_profile_t *profile)
{
  L7_uint32 intIfNum;
  ptin_bwPolicer_t bwPolicer;
  L7_RC_t rc;

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
    {
      LOG_PT_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
      return L7_FAILURE;
    }
  }
  else
  {
    intIfNum = L7_ALL_INTERFACES;
  }

  memset(&bwPolicer,0x00,sizeof(ptin_bwPolicer_t));

  bwPolicer.operation  = DAPI_CMD_CLEAR;
  bwPolicer.profile    = *profile;
  bwPolicer.policer_id = -1;
  bwPolicer.policy_ptr = L7_NULLPTR;

  rc = dtlPtinBWPolicer(intIfNum, &bwPolicer);

  return rc;
}

/**
 * Remove All existent BW policers
 *  
 * @param profile : BW profile 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_deleteAll(ptin_bw_profile_t *profile)
{
  L7_uint32 intIfNum;
  ptin_bwPolicer_t bwPolicer;
  L7_RC_t rc;

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS) 
    {
      LOG_PT_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
      return L7_FAILURE;
    }
  }
  else
  {
    intIfNum = L7_ALL_INTERFACES;
  }

  memset(&bwPolicer,0x00,sizeof(ptin_bwPolicer_t));

  bwPolicer.operation  = DAPI_CMD_CLEAR_ALL;
  bwPolicer.profile    = *profile;
  bwPolicer.policy_ptr = L7_NULLPTR;

  rc = dtlPtinBWPolicer(intIfNum, &bwPolicer);

  return rc;
}

/**
 * EVC STATISTICS MANAGEMENT
 */

/**
 * Get EVC Statistics
 * 
 * @param stats   : Statistics data
 * @param profile : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evcStats_get(ptin_evcStats_counters_t *stats, ptin_evcStats_profile_t *profile)
{
  L7_uint32 intIfNum;
  ptin_evcStats_t evcStats;
  L7_RC_t rc;

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
    return L7_FAILURE;
  }

  memset(&evcStats,0x00,sizeof(ptin_evcStats_t));

  evcStats.operation  = DAPI_CMD_GET;
  evcStats.profile    = *profile;
  evcStats.policy_ptr = L7_NULLPTR;

  rc = dtlPtinEvcStats(intIfNum, &evcStats);
  if (rc!=L7_SUCCESS)  return rc;

  /* Copy counters data */
  if (stats!=L7_NULLPTR)  *stats = evcStats.counters;

  return L7_SUCCESS;
}

/**
 * Set a new EVC Statistics rule
 * 
 * @param profile : EVC Stats profile 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evcStats_set(ptin_evcStats_profile_t *profile)
{
  L7_uint32 intIfNum;
  ptin_evcStats_t evcStats;
  L7_RC_t rc;

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
    return L7_FAILURE;
  }

  memset(&evcStats,0x00,sizeof(ptin_evcStats_t));

  evcStats.operation  = DAPI_CMD_SET;
  evcStats.profile    = *profile;
  evcStats.policy_ptr = L7_NULLPTR;

  rc = dtlPtinEvcStats(intIfNum, &evcStats);
  if (rc!=L7_SUCCESS)  return rc;

  return rc;
}

/**
 * Delete an EVC Statistics rule
 * 
 * @param profile : EVC Stats profile
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_evcStats_delete(ptin_evcStats_profile_t *profile)
{
  L7_uint32 intIfNum;
  ptin_evcStats_t evcStats;

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
    return L7_FAILURE;
  }

  memset(&evcStats,0x00,sizeof(ptin_evcStats_t));

  /* Clear stats */
  evcStats.operation  = DAPI_CMD_CLEAR;  
  evcStats.profile    = *profile;
  evcStats.policy_ptr = L7_NULLPTR;

  return dtlPtinEvcStats(intIfNum, &evcStats);
}

/**
 * Remove All existent EVC Statistics rules 
 *  
 * @param profile : EVC Stats profile 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evcStats_deleteAll(ptin_evcStats_profile_t *profile)
{
  L7_uint32 intIfNum;
  ptin_evcStats_t evcStats;

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_PT_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
    /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
    {
      LOG_PT_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
      return L7_FAILURE;
    }
  }
  else
  {
    intIfNum = L7_ALL_INTERFACES;
  }

  memset(&evcStats,0x00,sizeof(ptin_evcStats_t));

  evcStats.operation  = DAPI_CMD_CLEAR_ALL;
  evcStats.profile    = *profile;
  evcStats.policy_ptr = L7_NULLPTR;

  return dtlPtinEvcStats(intIfNum, &evcStats);
}

/**
 * Set global enable for IGMP packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmpPkts_global_trap(L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = 0;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_IGMP;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

  return L7_SUCCESS;
}

/**
 * Create/remove a rule, to allow IGMP packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_igmpPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_PT_ERR(LOG_CTX_API,"Invalid argument");
    return L7_FAILURE;
  }

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_IGMP;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}

/**
 * Set global enable for MLD packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_mldPkts_global_trap(L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_MLD;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

  return L7_SUCCESS;
}

/**
 * Create/remove a rule, to allow MLD packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_mldPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_PT_ERR(LOG_CTX_API,"Invalid argument");
    return L7_FAILURE;
  }

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_MLD;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}

/**
 * Set global enable for DHCP packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_dhcpPkts_global_trap(L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_DHCP;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

  return L7_SUCCESS;
}

/**
 * Create/remove a rule, to allow DHCP packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 * @param family : ip version 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_dhcpPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable, L7_uint8 family)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_PT_ERR(LOG_CTX_API,"Invalid argument");
    return L7_FAILURE;
  }

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = family;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_DHCP;

  rc = dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc != L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}

/**
 * Set global enable for PPPoE packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_pppoePkts_global_trap(L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_PPPOE;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

  return L7_SUCCESS;
}

/**
 * Create/remove a rule, to allow PPPoE packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_pppoePkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_PT_ERR(LOG_CTX_API,"Invalid argument");
    return L7_FAILURE;
  }

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_PPPOE;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_PT_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}

/**
 * Storm control profile get
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_stormControl_get(L7_BOOL enable, L7_uint32 intIfNum, L7_uint16 vlanId, L7_uint16 vlanId_mask, ptin_stormControl_t *stormControl)
{
  /* NOT IMPLEMENTED */
  memset(stormControl, 0x00, sizeof(ptin_stormControl_t));

  return L7_SUCCESS;
}

/**
 * Storm control profile set
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_stormControl_config(L7_BOOL enable, ptin_stormControl_t *stormControl)
{
  L7_RC_t rc;

  LOG_PT_TRACE(LOG_CTX_API,"Going to apply storm control (enable=%u)", enable);

  rc = dtlPtinStormControl(L7_ALL_INTERFACES, enable, stormControl);
  if (rc!=L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting storm control enable=%u", enable);
    return rc;
  }
  else
  {
    LOG_PT_TRACE(LOG_CTX_API,"Success setting storm control enable=%u", enable);
  }

  LOG_PT_TRACE(LOG_CTX_API,"Storm control applied successfully (enable=%u)", enable);

  return L7_SUCCESS;
}

#if 0
/**
 * Apply Rate limit to broadcast traffic
 * 
 * @param enable : enable status
 * @param vlanId : vlan id
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_broadcast_rateLimit(L7_BOOL enable, L7_uint16 vlanId)
{
  ptin_pktRateLimit_t rateLimit;
  L7_RC_t rc;

  if (vlanId==0 || vlanId>4095)
  {
    LOG_PT_ERR(LOG_CTX_API,"Invalid Vlan Id %u",vlanId);
    return L7_FAILURE;
  }

  memset(&rateLimit,0x00,sizeof(ptin_pktRateLimit_t));

  rateLimit.vlanId      = vlanId;
  rateLimit.vlanId_mask = PTIN_VLAN_MASK(vlanId);
  rateLimit.trafficType = PTIN_PKT_RATELIMIT_MASK_BCAST;
  rateLimit.rate        = RATE_LIMIT_BCAST;

  rc = dtlPtinRateLimit(L7_ALL_INTERFACES, enable, &rateLimit);

  if (rc!=L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting broadcast rate limit to vlan %u to %u",vlanId,enable);
  }
  else
  {
    LOG_PT_TRACE(LOG_CTX_API,"Success setting broadcast rate limit to vlan %u to %u",vlanId,enable);
  }

  return rc;
}

/**
 * Apply Rate limit to multicast traffic
 * 
 * @param enable : enable status
 * @param vlanId : vlan id
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_multicast_rateLimit(L7_BOOL enable, L7_uint16 vlanId)
{
  ptin_pktRateLimit_t rateLimit;
  L7_RC_t rc;

  if (vlanId==0 || vlanId>4095)
  {
    LOG_PT_ERR(LOG_CTX_API,"Invalid Vlan Id %u",vlanId);
    return L7_FAILURE;
  }

  memset(&rateLimit,0x00,sizeof(ptin_pktRateLimit_t));

  rateLimit.vlanId      = vlanId;
  rateLimit.vlanId_mask = PTIN_VLAN_MASK(vlanId);
  rateLimit.trafficType = PTIN_PKT_RATELIMIT_MASK_MCAST;
  rateLimit.rate        = RATE_LIMIT_MCAST;

  rc = dtlPtinRateLimit(L7_ALL_INTERFACES, enable, &rateLimit);

  if (rc!=L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_API,"Error setting multicast rate limit to vlan %u to %u",vlanId,enable);
  }
  else
  {
    LOG_PT_TRACE(LOG_CTX_API,"Success setting multicast rate limit to vlan %u to %u",vlanId,enable);
  }

  return rc;
}
#endif

/**
 * Consult hardware resources
 * 
 * @param resources : available resources
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hw_resources_get(st_ptin_policy_resources *resources)
{
  L7_RC_t rc;

  memset(resources,0x00,sizeof(st_ptin_policy_resources));

  if ((rc=dtlPtinHwResources(resources))!=L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_API,"Error consulting hardware resources");
  }

  return rc;
}
