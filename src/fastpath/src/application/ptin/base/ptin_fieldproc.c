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

/**
 * BW POLICERS MANAGEMENT
 */

/**
 * Get BW policer
 * 
 * @param profile : BW profile
 * @param policy : BW policy pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_get(ptin_bw_profile_t *profile, ptin_bw_policy_t *policy)
{
  ptin_bwPolicer_t bwPolicer;

  if (profile==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  bwPolicer.operation  = DAPI_CMD_GET;
  bwPolicer.profile    = *profile;
  bwPolicer.policy_ptr = policy;

  return dtlPtinBWPolicer(&bwPolicer);
}

/**
 * Add a new BW policer
 *  
 * @param intIfNum : Interface to apply profile
 * @param profile : BW profile
 * @param policy : BW policy pointer address
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_set(ptin_bw_profile_t *profile, ptin_bw_policy_t **policy)
{
  ptin_bwPolicer_t bwPolicer;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (profile==L7_NULLPTR || policy==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  bwPolicer.operation  = DAPI_CMD_SET;
  bwPolicer.profile    = *profile;
  bwPolicer.policy_ptr = *policy;

  rc=dtlPtinBWPolicer(&bwPolicer);
  if (rc!=L7_SUCCESS)  return rc;

  /* Policer pointer could been changed */
  *policy = bwPolicer.policy_ptr;

  if (bwPolicer.policy_ptr==L7_NULLPTR)
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned policy pointer is null");
  else
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned policy pointer is valid");

  return rc;
}

/**
 * Remove an existent BW policer
 *  
 * @param policy : BW policy pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_delete(ptin_bw_policy_t *policy)
{
  ptin_bwPolicer_t bwPolicer;

  bwPolicer.operation  = DAPI_CMD_CLEAR;
  bwPolicer.policy_ptr = policy;

  return dtlPtinBWPolicer(&bwPolicer);
}

/**
 * Remove All existent BW policers
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_bwPolicer_deleteAll(void)
{
  ptin_bwPolicer_t bwPolicer;

  bwPolicer.operation  = DAPI_CMD_CLEAR_ALL;
  bwPolicer.policy_ptr = L7_NULLPTR;

  return dtlPtinBWPolicer(&bwPolicer);
}

/**
 * EVC STATISTICS MANAGEMENT
 */

/**
 * Get EVC Statistics
 * 
 * @param stats   : Statistics data
 * @param evcStats : evcStats pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evcStats_get(ptin_evcStats_counters_t *stats, ptin_evcStats_policy_t *policy)
{
  ptin_evcStats_t evcStats;
  L7_RC_t rc;

  memset(&evcStats,0x00,sizeof(ptin_evcStats_t));

  evcStats.operation  = DAPI_CMD_GET;
  evcStats.policy_ptr = policy;

  rc=dtlPtinEvcStats(&evcStats);
  if (rc!=L7_SUCCESS)  return rc;

  /* Copy counters data */
  if (stats!=L7_NULLPTR)  *stats = evcStats.counters;

  return L7_SUCCESS;
}

/**
 * Set a new EVC Statistics rule
 * 
 * @param profile : EVC Stats profile 
 * @param evcStats : evcStats pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evcStats_set(ptin_evcStats_profile_t *profile, ptin_evcStats_policy_t **policy)
{
  ptin_evcStats_t evcStats;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (profile==L7_NULLPTR || policy==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  evcStats.operation  = DAPI_CMD_SET;
  evcStats.profile    = *profile;
  evcStats.policy_ptr = *policy;

  rc=dtlPtinEvcStats(&evcStats);
  if (rc!=L7_SUCCESS)  return rc;

  /* Policer pointer could been changed */
  *policy = evcStats.policy_ptr;

  if (evcStats.policy_ptr==L7_NULLPTR)
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned policy pointer is null");
  else
    LOG_TRACE(LOG_CTX_PTIN_EVC,"Returned policy pointer is valid");

  return rc;
}

/**
 * Delete an EVC Statistics rule
 * 
 * @param profile : EVC Stats profile 
 * @param evcStats : evcStats pointer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evcStats_delete(ptin_evcStats_policy_t *policy)
{
  ptin_evcStats_t evcStats;

  evcStats.operation  = DAPI_CMD_CLEAR;
  evcStats.policy_ptr = policy;

  return dtlPtinEvcStats(&evcStats);
}

/**
 * Remove All existent EVC Statistics rules
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_evcStats_deleteAll(void)
{
  ptin_evcStats_t evcStats;

  evcStats.operation  = DAPI_CMD_CLEAR_ALL;
  evcStats.policy_ptr = L7_NULLPTR;

  return dtlPtinEvcStats(&evcStats);
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

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_IGMP;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying global enable to %u",enable);

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
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid argument");
    return L7_FAILURE;
  }

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_IGMP;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);

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

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_MLD;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying global enable to %u",enable);

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
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid argument");
    return L7_FAILURE;
  }

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_MLD;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);

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

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_DHCP;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying global enable to %u",enable);

  return L7_SUCCESS;
}

/**
 * Create/remove a rule, to allow DHCP packets to go to the CPU
 * 
 * @param vlanId : vlanId to be (dis)allowed
 * @param enable : L7_TRUE/L7_FALSE 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_dhcpPkts_vlan_trap(L7_uint16 vlanId, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid argument");
    return L7_FAILURE;
  }

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_DHCP;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);

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

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_PPPOE;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying global enable to %u",enable);

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
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid argument");
    return L7_FAILURE;
  }

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_PPPOE;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}

/**
 * Storm control profile get
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_stormControl_get(ptin_stormControl_t *stormControl)
{
  return L7_SUCCESS;
}

/**
 * Storm control profile set
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_stormControl_set(ptin_stormControl_t *stormControl)
{
  return L7_SUCCESS;
}

/**
 * Storm control profile clear
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_stormControl_clear(ptin_stormControl_t *stormControl)
{
  return L7_SUCCESS;
}

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
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid Vlan Id %u",vlanId);
    return L7_FAILURE;
  }

  memset(&rateLimit,0x00,sizeof(ptin_pktRateLimit_t));

  rateLimit.vlanId      = vlanId;
  rateLimit.trafficType = PACKET_RATE_LIMIT_BROADCAST;

  rc = dtlPtinRateLimit(L7_ALL_INTERFACES, enable, &rateLimit);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting broadcast rate limit to vlan %u to %u",vlanId,enable);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_API,"Success setting broadcast rate limit to vlan %u to %u",vlanId,enable);
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
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid Vlan Id %u",vlanId);
    return L7_FAILURE;
  }

  memset(&rateLimit,0x00,sizeof(ptin_pktRateLimit_t));

  rateLimit.vlanId      = vlanId;
  rateLimit.trafficType = PACKET_RATE_LIMIT_MULTICAST;

  rc = dtlPtinRateLimit(L7_ALL_INTERFACES, enable, &rateLimit);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting multicast rate limit to vlan %u to %u",vlanId,enable);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_API,"Success setting multicast rate limit to vlan %u to %u",vlanId,enable);
  }

  return rc;
}


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
    LOG_ERR(LOG_CTX_PTIN_API,"Error consulting hardware resources");
  }

  return rc;
}
