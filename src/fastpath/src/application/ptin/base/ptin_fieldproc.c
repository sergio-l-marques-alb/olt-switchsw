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

#include "usmdb_dot3ad_api.h"

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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments");
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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid arguments (policer_id=%d)", policer_id);
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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
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
  L7_uint64 ptin_port_bmp = 0;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (profile==L7_NULLPTR || meter==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }

  if(profile->ptin_port_bmp == 0)/*For port bmp ports skip */
  {
    /* Get intIfNum */
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
      return L7_FAILURE;
    }
  }
  /* Get intIfNum */
  if (profile->ptin_port >= 0 && profile->ptin_port < PTIN_SYSTEM_N_INTERF)
  {
    /* Is it a LAG? */
    if (profile->ptin_port >= PTIN_SYSTEM_N_PORTS)
    {
      ptin_LACPLagConfig_t lagInfo;

      /* Get LAG id */
      if (ptin_intf_port2lag(profile->ptin_port, &lagInfo.lagId) == L7_SUCCESS)
      {
        /* Get LAG members */
        if (ptin_intf_LagConfig_get(&lagInfo) == L7_SUCCESS && lagInfo.admin && lagInfo.members_pbmp64 != 0)
        {
          ptin_port_bmp = lagInfo.members_pbmp64;
        }
        else
        {
          PT_LOG_ERR(LOG_CTX_API, "ptin_port %u (LAG) does not have members", intIfNum, profile->ptin_port);
          return L7_FAILURE;
        }
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_API, "Invalid ptin_port %u", profile->ptin_port);
        return L7_FAILURE;
      }
    }
  }
  else
  {
    intIfNum = L7_ALL_INTERFACES;
    bwPolicer.ptin_port_bmp = profile->ptin_port_bmp;
  }

  memset(&bwPolicer,0x00,sizeof(ptin_bwPolicer_t));

  bwPolicer.operation     = DAPI_CMD_SET;
  bwPolicer.ptin_port_bmp = ptin_port_bmp;
  bwPolicer.profile       = *profile;
  bwPolicer.meter         = *meter;
  bwPolicer.policer_id    = policer_id;
  bwPolicer.policy_ptr    = L7_NULLPTR;

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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }

  if(profile->ptin_port_bmp != 0)/*For port bmp ports skip */
  {
    intIfNum = L7_ALL_INTERFACES;
    bwPolicer.ptin_port_bmp = profile->ptin_port_bmp;
  }
  else
  {
    /* Get intIfNum */
    if (profile->ptin_port >= 0)
    {
      if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
        return L7_FAILURE;
      }
    }
    else
    {
      intIfNum = L7_ALL_INTERFACES;
    }
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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_port %u", profile->ptin_port);
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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
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
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid profile");
    return L7_FAILURE;
  }
  /* Get intIfNum */
    /* Get intIfNum */
  if (profile->ptin_port >= 0)
  {
    if (ptin_intf_port2intIfNum(profile->ptin_port, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"ptin_port %u not valid", profile->ptin_port);
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
    PT_LOG_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

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
    PT_LOG_ERR(LOG_CTX_API,"Invalid argument");
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
    PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

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
    PT_LOG_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

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
    PT_LOG_ERR(LOG_CTX_API,"Invalid argument");
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
    PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

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
    PT_LOG_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

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
    PT_LOG_ERR(LOG_CTX_API,"Invalid argument");
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
    PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

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
    PT_LOG_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

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
    PT_LOG_ERR(LOG_CTX_API,"Invalid argument");
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
    PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

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

  PT_LOG_TRACE(LOG_CTX_API,"Going to apply storm control (enable=%u)", enable);

  rc = dtlPtinStormControl(L7_ALL_INTERFACES, enable, stormControl);
  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API,"Error setting storm control enable=%u", enable);
    return rc;
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_API,"Success setting storm control enable=%u", enable);
  }

  PT_LOG_TRACE(LOG_CTX_API,"Storm control applied successfully (enable=%u)", enable);

  return L7_SUCCESS;
}

#if 0
typedef struct
{
  L7_uint8 cos_bitmap;    /* CoS occurrences in terms of bitmap (8 possible values) */
  L7_uint8 mask;          /* Mask applied to this entry */
  L7_uint8 value;         /* Value applied to this entry */
} cos_bitmap_comb_t;

/* Lookup table to optimize rules configuration:
   It is important the be sorted in terms of considered bits (higher to lower) */
cos_bitmap_comb_t cos_lookup[]={
  {0xff, 0x0, 0x0},   /* 8 values: 1111 1111 - 0,1,2,3,4,5,6,7 */
  {0x0f, 0x4, 0x0},   /* 4 values: 0000 1111 - 0,1,2,3 */
  {0xf0, 0x4, 0x4},   /* 4 values: 1111 0000 - 4,5,6,7 */
  {0x33, 0x2, 0x0},   /* 4 values: 0011 0011 - 0,1,4,5 */
  {0xcc, 0x2, 0x2},   /* 4 values: 1100 1100 - 2,3,6,7 */
  {0x55, 0x1, 0x0},   /* 4 values: 0101 0101 - 0,2,4,6 */
  {0xaa, 0x1, 0x1},   /* 4 values: 1010 1010 - 1,3,5,7 */
  {0x03, 0x6, 0x0},   /* 2 values: 0000 0011 - 0,1 */
  {0xc0, 0x6, 0x2},   /* 2 values: 0000 1100 - 2,3 */
  {0x30, 0x6, 0x4},   /* 2 values: 0011 0000 - 4,5 */
  {0xc0, 0x6, 0x5},   /* 2 values: 1100 0000 - 6,7 */
  {0x05, 0x5, 0x0},   /* 2 values: 0000 0101 - 0,2 */
  {0x0a, 0x5, 0x1},   /* 2 values: 0000 1010 - 1,3 */
  {0x50, 0x5, 0x4},   /* 2 values: 0101 0000 - 4,6 */
  {0xa0, 0x5, 0x5},   /* 2 values: 1010 0000 - 5,7 */
  {0x11, 0x3, 0x0},   /* 2 values: 0001 0001 - 0,4 */
  {0x22, 0x3, 0x1},   /* 2 values: 0010 0010 - 1,5 */
  {0x44, 0x3, 0x2},   /* 2 values: 0100 0100 - 2,6 */
  {0x88, 0x3, 0x3},   /* 2 values: 1000 1000 - 3,7 */
  {0x01, 0xf, 0x0},   /* 1 value : 0000 0001 - 0 */
  {0x02, 0xf, 0x1},   /* 1 value : 0000 0010 - 1 */
  {0x04, 0xf, 0x2},   /* 1 value : 0000 0100 - 2 */
  {0x08, 0xf, 0x3},   /* 1 value : 0000 1000 - 3 */
  {0x10, 0xf, 0x4},   /* 1 value : 0001 0000 - 4 */
  {0x20, 0xf, 0x5},   /* 1 value : 0010 0000 - 5 */
  {0x40, 0xf, 0x6},   /* 1 value : 0100 0000 - 6 */
  {0x80, 0xf, 0x7},   /* 1 value : 1000 0000 - 7 */
}
#endif

/* This is the Mask's list to be considered (already sorted from the widest to the most specific value) */
L7_uint8 cos_mask_lookup_1bit[] = {
  0x00, 0x01
};

L7_uint8 cos_mask_lookup_2bits[] = {
  0x00, 0x02, 0x01, 0x03
};

L7_uint8 cos_mask_lookup_3bits[] = {
  0x00, 0x04, 0x02, 0x01, 0x06, 0x05, 0x03, 0x07
};

L7_uint8 cos_mask_lookup_4bits[] = {
  0x00, 0x08, 0x04, 0x02, 0x01, 0x0c, 0x0a, 0x09,
  0x06, 0x05, 0x03, 0x0e, 0x0d, 0x0b, 0x07, 0x0f
};

L7_uint8 cos_mask_lookup_5bits[] = {
  0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x18, 0x14,
  0x12, 0x11, 0x0c, 0x0a, 0x09, 0x06, 0x05, 0x03,
  0x1c, 0x1a, 0x19, 0x16, 0x15, 0x13, 0x0e, 0x0d,
  0x0b, 0x07, 0x1e, 0x1d, 0x1b, 0x17, 0x0f, 0x1f
};

L7_uint8 cos_mask_lookup_6bits[] = {
  0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x30,
  0x28, 0x24, 0x22, 0x21, 0x18, 0x14, 0x12, 0x11,
  0x0c, 0x0a, 0x09, 0x06, 0x05, 0x03, 0x38, 0x34,
  0x32, 0x31, 0x2c, 0x2a, 0x29, 0x26, 0x25, 0x23,
  0x1c, 0x1a, 0x19, 0x16, 0x15, 0x13, 0x0e, 0x0d,
  0x0b, 0x07, 0x3c, 0x3a, 0x39, 0x36, 0x35, 0x33,
  0x2e, 0x2d, 0x2b, 0x27, 0x1e, 0x1d, 0x1b, 0x17,
  0x0f, 0x3e, 0x3d, 0x3b, 0x37, 0x2f, 0x1f, 0x3f
};

/**
 * Clear all configuration for one particular VLAN
 * 
 * @param qos : QoS info
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_vlan_clear(ptin_qos_vlan_t *qos)
{
  ptin_dtl_qos_t qos_cfg;
  L7_RC_t rc;

  memset(&qos_cfg, 0x00, sizeof(ptin_dtl_qos_t));
  qos_cfg.ext_vlan   = qos->nni_vlan;
  qos_cfg.int_vlan   = qos->int_vlan;
  qos_cfg.leaf_side  = qos->leaf_side;
  qos_cfg.trust_mode = 0;   /* None */
  
  rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_QOS_CLASSIFY, DAPI_CMD_CLEAR, sizeof(ptin_dtl_qos_t), &qos_cfg);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error removing all rules of intVLAN %u / NNIVlan %u / leaf:%d",
            qos->int_vlan, qos->nni_vlan, qos->leaf_side);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_INTF, "All rules removed from intVLAN %u / NNIVlan %u / leaf:%u",
            qos->int_vlan, qos->nni_vlan, qos->leaf_side);

  return L7_SUCCESS;
}

/**
 * VLAN CoS configuration into the HW
 * 
 * @param qos_cfg : Struct to be used for HW configuration
 * @param cos_map : CoS(pbit)={a,b,c,d,e,f,g,h} 
 * @param cos_map_size : cos_map's number of elements 
 * @param n_prios : number of distinct priorities (-1 to 
 *                reconfigure ports, 0 to delete rules, ...)
 * @param n_cos : number of CoS 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t cos_vlan_configure(ptin_dtl_qos_t *qos_cfg, L7_uint8 *cos_map, L7_uint8 cos_map_size,
                                  L7_int8 n_prios, L7_int8 n_cos)
{
  L7_uint8 cos;
  L7_RC_t rc;
  L7_uint8 *cos_mask_lookup;
  
  /* Validate arguments */
  if (qos_cfg == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Only update port information */
  if (n_prios < 0)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "Going to port data of intVLAN %u / NNIVlan %u",
              qos_cfg->int_vlan, qos_cfg->ext_vlan);

    qos_cfg->trust_mode = -1;
    rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_QOS_CLASSIFY, DAPI_CMD_SET, sizeof(ptin_dtl_qos_t), qos_cfg);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error reconfiguring rules of intVLAN %u / NNIVlan %u",
              qos_cfg->int_vlan, qos_cfg->ext_vlan);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "All rules were reconfigured");
    return L7_SUCCESS;
  }

  /* Remove configurations? */
  if (n_prios == 0)
  {
    ptin_qos_vlan_t qos_apply;

    memset(&qos_apply, 0x00, sizeof(ptin_qos_vlan_t));
    qos_apply.nni_vlan   = qos_cfg->ext_vlan;
    qos_apply.int_vlan   = qos_cfg->int_vlan;
    qos_apply.leaf_side  = qos_cfg->leaf_side;
    qos_apply.trust_mode = 0;

    PT_LOG_TRACE(LOG_CTX_INTF, "Going to remove all configurations of intVLAN %u / NNIVlan %u / leaf:%u",
              qos_apply.int_vlan, qos_apply.nni_vlan, qos_apply.leaf_side);
    /* Remove all configurations */
    rc = ptin_qos_vlan_clear(&qos_apply);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error removing all rules of intVLAN %u / NNIVlan %u / leaf:%u",
              qos_apply.int_vlan, qos_apply.nni_vlan, qos_apply.leaf_side);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "All rules removed of intVLAN %u / NNIVlan %u / leaf:%u",
              qos_apply.int_vlan, qos_apply.nni_vlan, qos_apply.leaf_side);

    return L7_SUCCESS;
  }

  /* A CoS map should be provided */
  if (cos_map == L7_NULLPTR || cos_map_size == 0)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid arguments");
    return L7_FAILURE;
  }

  if (n_prios < 2)
  {
    /* All priorities will be mapped into one single priority */
    PT_LOG_TRACE(LOG_CTX_INTF, "All priorities will be mapped in a single int_prio");
    qos_cfg->priority = 0;
    qos_cfg->priority_mask = 0;
    qos_cfg->int_priority = cos_map[0];
    rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_QOS_CLASSIFY, DAPI_CMD_SET, sizeof(ptin_dtl_qos_t), qos_cfg);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error configuring rule");
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "All priorities were mapped to int_prio %u", qos_cfg->int_priority);
    return L7_SUCCESS;
  }
  else if (n_prios < 4)
  {
    /* 2 CoS values (1 bit) */
    n_prios = 2;
    cos_mask_lookup = cos_mask_lookup_1bit;
    PT_LOG_TRACE(LOG_CTX_API, "Assuming a 1 bit mask (2 distinct priorities)");
  }
  else if (n_prios < 8)
  {
    /* 4 CoS values (2 bits) */
    n_prios = 4;
    cos_mask_lookup = cos_mask_lookup_2bits;
    PT_LOG_TRACE(LOG_CTX_API, "Assuming a 2 bit mask (4 distinct priorities)");
  }
  else if (n_prios < 16)
  {
    /* 8 CoS values (3 bits) */
    n_prios = 8;
    cos_mask_lookup = cos_mask_lookup_3bits;
    PT_LOG_TRACE(LOG_CTX_API, "Assuming a 3 bit mask (8 distinct priorities)");
  }
  else if (n_prios < 32)
  {
    /* 16 CoS values (4 bits) */
    n_prios = 16;
    cos_mask_lookup = cos_mask_lookup_4bits;
    PT_LOG_TRACE(LOG_CTX_API, "Assuming a 4 bit mask (16 distinct priorities)");
  }
  else if (n_prios < 64)
  {
    /* 32 CoS values (5 bits) */
    n_prios = 32;
    cos_mask_lookup = cos_mask_lookup_5bits;
    PT_LOG_TRACE(LOG_CTX_API, "Assuming a 5 bit mask (32 distinct priorities)");
  }
  else
  {
    /* 64 CoS values (6 bits) */
    n_prios = 64;
    cos_mask_lookup = cos_mask_lookup_6bits;
    PT_LOG_TRACE(LOG_CTX_API, "Assuming a 6 bit mask (64 distinct priorities)");
  }

  /* Limit size of cos_map to be considered */
  if (cos_map_size > n_prios)
  {
    cos_map_size = n_prios;
    PT_LOG_WARN(LOG_CTX_API, "CoS map size limited to %u elements", cos_map_size);
  }

  /* Run all possible CoS values */
  for (cos = 0; cos < n_cos; cos++)
  {
    L7_uint8 i, look;
    L7_uint64 cos_bitmap, cos_bitmap_mask;

    PT_LOG_TRACE(LOG_CTX_API,"Searching for CoS %u...", cos);

    /* Calculate bitmap of 1s where this CoS occurs (input) */
    cos_bitmap = 0;
    cos_bitmap_mask = 0;
    for (i = 0; i < cos_map_size; i++)
    {
      if (cos_map[i] == cos) 
      {
        cos_bitmap |= 1ULL<<i;
      }
      /* Start by looking to all bits os cos_bitmap (input) */
      cos_bitmap_mask |= 1ULL<<i;
    }

    PT_LOG_TRACE(LOG_CTX_API,"Entries with CoS %u: 0x%llx", cos, cos_bitmap);

    /* If this CoS is not used, go to next cos */
    if (cos_bitmap == 0)
    {
      PT_LOG_TRACE(LOG_CTX_API,"No entries have CoS %u. Skipt it!", cos);
      continue;
    }

    /* Run all given masks (sorted from the most wider to the most specific) */
    for (look = 0; look < n_prios && cos_bitmap_mask != 0x00; look++)
    {
      L7_uint8 mask, value;
      L7_uint8 value_and_mask[8];
      L7_uint64 cos_bitmap_reference, values_excluded;

      /* Mask to be considered for this lookup entry */
      mask = cos_mask_lookup[look];

      PT_LOG_TRACE(LOG_CTX_API,"Lookup entry %u: mask=0x%x", look, mask);

      /* List of possible results from the AND operation between all values and this mask (some may be repeated) */
      for (value = 0; value < n_prios; value++)
      {
        value_and_mask[value] = value & mask;
      }

      /* Bitmap of values to be excluded for processing (starting with none) */
      values_excluded = 0x00;

      /* Run all possible values for this mask */
      for (value = 0; value < n_prios; value++)
      {
        /** The values_excluded variable is important to only use the
         *  necessary values related to this mask */

        /* Skip value if it's excluded */
        if ((values_excluded >> value) & 1)
        {
          //PT_LOG_TRACE(LOG_CTX_API,"value %u skipped for being excluded in previous iterations.", value);
          continue;
        }

        //PT_LOG_TRACE(LOG_CTX_API,"Processing value %u / mask 0x%x", value, mask);

        /* CoS bitmap to be used as reference */
        cos_bitmap_reference = 0x00;

        /** The following Algorithm is destined to determine the
         *  reference bitmap to be compared with the input bitmap */

        /* Obtain CoS bitmap to be compared with */
        for (i = value; i < n_prios; i++)
        {
          /* Skip value if excluded in previos iterations */
          if ((values_excluded >> i) & 1)
            continue;

          /* Only proceed if AND result is the same: this is where the common result group is made */
          if (value_and_mask[i] != value_and_mask[value])
            continue;

          /* Add this bit to CoS pattern to be matched */
          cos_bitmap_reference |= 1ULL<<i;

          /* Exclude this value to not be used in the following iterations for this particular mask */
          values_excluded |= 1ULL << i;
        }

        /* If reference CoS bitmap is empty, skip to next value */
        if (cos_bitmap_reference == 0x00)
        {
          //PT_LOG_TRACE(LOG_CTX_API,"CoS bitmap reference empty. Going to next Lookup entry");
          continue;
        }

        PT_LOG_TRACE(LOG_CTX_API,"CoS bitmap reference: 0x%llx (Excluded values: 0x%llx)",
                  cos_bitmap_reference, values_excluded);

        /** The reference bitmap was already determined.
         *  Now, is important to exclude older matched patterns to avoid
         *  rules superposition */

        /* Clear hidden bits (hidden by previous iterations, to avoid conflicting rules) */
        cos_bitmap &= cos_bitmap_mask; 

        PT_LOG_TRACE(LOG_CTX_API,"CoS bitmap to be used: 0x%llx (cos_bitmap_mask: 0x%llx)", cos_bitmap, cos_bitmap_mask);

        /* If a matched pattern occurs, apply rule */
        if ((cos_bitmap & cos_bitmap_reference) == cos_bitmap_reference)
        {
          /* Hide these bits, in order to not be considered next time */
          cos_bitmap_mask &= ~cos_bitmap_reference;

          PT_LOG_TRACE(LOG_CTX_API,"Pattern 0x%llx is present (0x%llx -> 0x%llx). Removing this to cos_bitmap_mask (0xllx)",
                    cos_bitmap, cos_bitmap_reference, cos_bitmap_mask);

          /* Rule configuration */
          qos_cfg->priority = value;
          qos_cfg->priority_mask = mask;
          qos_cfg->int_priority = cos;
          rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_QOS_CLASSIFY, DAPI_CMD_SET, sizeof(ptin_dtl_qos_t), qos_cfg);
          if (rc != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_INTF, "Error configuring priorities %u/0x%x", qos_cfg->priority, qos_cfg->priority_mask);
            return L7_FAILURE;
          }
          PT_LOG_TRACE(LOG_CTX_INTF, "Priorities %u/0x%x is mapped to int_prio %u", qos_cfg->priority, qos_cfg->priority_mask, qos_cfg->int_priority);
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_INTF, "Nothing done... next iteration");
        }
      }
    }
  }

  return L7_SUCCESS;
}

//static L7_RC_t ptin_qos_vlan_add_recursive(ptin_dtl_qos_t *qos_cfg, L7_uint8 *cos_map, L7_uint8 prio_start, L7_uint8 prio_end);

/**
 * Get port bitmap from list of ports
 * 
 * @param ptin_port 
 * @param number_of_ports 
 * @param ptin_port_bmp 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_qos_port_bitmap_get(L7_uint32 *ptin_port, L7_uint8 number_of_ports, L7_uint64 *ptin_port_bmp)
{
  L7_int    i, j;
  L7_uint32 intIfNum, port;
  L7_INTF_TYPES_t intfType;
  L7_uint64 pbmp;
  L7_uint32 lag_members_list[PTIN_SYSTEM_N_PORTS], number_of_lag_members;
  L7_RC_t rc = L7_SUCCESS;

    /* Prepare list of ports */
  pbmp = 0;
  for (i = 0; i < number_of_ports; i++)
  {
    /* Validate ptin_port */
    if (ptin_port[i] >= PTIN_SYSTEM_N_INTERF)
    {
      PT_LOG_ERR(LOG_CTX_API, "Invalid ptin port %u", ptin_port[i]);
      rc = L7_FAILURE;
      continue;
    }
    /* Convert to intIfNum */
    if (ptin_intf_port2intIfNum(ptin_port[i], &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_API, "port %u does not exist", ptin_port[i]);
      rc = L7_FAILURE;
      continue;
    }
    /* Get interface type */
    if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_L2,"Error getting interface type of intIfNum=%u", intIfNum);
      rc = L7_FAILURE;
      continue;
    }
    /* Is it a LAG? If so, get its members */
    if (intfType == L7_LAG_INTF)
    {
      /* Get list of active ports */
      number_of_lag_members = PTIN_SYSTEM_N_PORTS;
      if (usmDbDot3adMemberListGet(1, intIfNum, &number_of_lag_members, lag_members_list) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Error reading Members List of intIfNum %u", intIfNum);
        rc = L7_FAILURE;
        continue;
      }
      /* Run all members, and add them to the bitmap list */
      for (j = 0; j < number_of_lag_members; j++)
      {
        if (ptin_intf_intIfNum2port(lag_members_list[j], &port) == L7_SUCCESS)
        {
          pbmp |= 1ULL << port; 
        }
        else
        {
          PT_LOG_ERR(LOG_CTX_INTF, "Error adding LAG member intIfNum %u", lag_members_list[j]);
          rc = L7_FAILURE;
        }
      }
    }
    /* Physical interface */
    else if (intfType == L7_PHYSICAL_INTF)
    {
      pbmp |= 1ULL << ptin_port[i];
    }
    /* Not recognized type */
    else
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Invalid intIfNum %u", intIfNum);
      rc = L7_FAILURE;
      continue;
    }
  }
  PT_LOG_TRACE(LOG_CTX_API, "Bitmap ports: 0x%llx", pbmp);

  if (ptin_port_bmp != L7_NULLPTR)
  {
    *ptin_port_bmp = pbmp;
  }

  return rc;
}


/**
 * Update list of ports of QoS configuration
 * 
 * @param qos : QoS info 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_vlan_ports_update(ptin_qos_vlan_t *qos)
{
  ptin_qos_vlan_t qos_apply = *qos;

  qos_apply.trust_mode   = -1;
  qos_apply.cos_map_size = 0;

  return ptin_qos_vlan_add(&qos_apply);
}

/**
 * Control QoS egress remarking
 * 
 * @param intIfNum 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_egress_remark(L7_uint32 intIfNum, L7_BOOL enable)
{
  L7_uint32 remark_enable;
  L7_RC_t rc;

  remark_enable = enable;

  rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_QOS_REMARK, DAPI_CMD_SET, sizeof(L7_uint32), (void *) &remark_enable);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error setting remarking for intIfNum %u: rc=%d", intIfNum, rc);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_INTF, "QoS remark set to %u at intIfNum %u", remark_enable, intIfNum);

  return L7_SUCCESS;
}

/**
 * Configure QoS mapping rules for a particular VLAN
 * 
 * @param qos : QoS info
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_vlan_add(ptin_qos_vlan_t *qos)
{
  L7_int    n_prios;
  L7_uint64 ptin_port_bmp;
  ptin_dtl_qos_t qos_cfg;
  L7_RC_t   rc;

  /* Validate arguments */
  if (qos->int_vlan > PTIN_VLAN_MAX || qos->nni_vlan > PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid vlan id: int_vlan=%u nni_vlan=%u", qos->int_vlan, qos->nni_vlan);
    return L7_FAILURE;
  }

  /* Validate trust mode */
  if (qos->trust_mode > L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid trust mode: %u", qos->trust_mode);
    return L7_FAILURE;
  }

  /* Maximum number of CoS */
  switch (qos->trust_mode)
  {
    case 0:
      n_prios = 0;  /* This value will remove any configuration */
      break;
    case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
      n_prios = 1;  /* This value will configure one single rule */
      break;
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
      n_prios = 8;
      break;
    case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
      n_prios = 64;
      break;
    default:
      n_prios = -1;
      break;
  }

  /* Validate ports */
  if (qos->number_of_ports == 0)
  {
    PT_LOG_WARN(LOG_CTX_API, "No ports provided (ports=%u)... ignoring", qos->number_of_ports);
    return L7_SUCCESS;
  }

  /* Get bitmap of ports */
  if (ptin_qos_port_bitmap_get(qos->ptin_port, qos->number_of_ports, &ptin_port_bmp) != L7_SUCCESS)
  {
    PT_LOG_WARN(LOG_CTX_API, "Error getting bitmap of ports");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "VLAN %u, Bitmap ports: 0x%llx", qos->int_vlan, ptin_port_bmp);

  memset(&qos_cfg, 0x00, sizeof(qos_cfg));
  qos_cfg.ext_vlan      = qos->nni_vlan;
  qos_cfg.int_vlan      = qos->int_vlan;
  qos_cfg.leaf_side     = qos->leaf_side;
  qos_cfg.trust_mode    = qos->trust_mode;
  qos_cfg.pbits_remark  = qos->pbits_remark;
  qos_cfg.ptin_port_bmp = ptin_port_bmp;

  /* Configure QoS */
  rc = cos_vlan_configure(&qos_cfg, qos->cos_map, qos->cos_map_size, n_prios, 8);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "QoS mapping failed");
    return rc;
  }

  return L7_SUCCESS;
}

#if 0
/**
 * Recursive routive for ptin_qos_vlan_add
 * 
 * @param qos_cfg 
 * @param cos_map 
 * @param prio_start 
 * @param prio_end 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_qos_vlan_add_recursive(ptin_dtl_qos_t *qos_cfg, L7_uint8 *cos_map, L7_uint8 prio_start, L7_uint8 prio_end)
{
  L7_uint8 i, mask;  
  L7_RC_t rc;

  if (prio_start > 7 || prio_end > 7 || prio_start > prio_end)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid prio range: %u - %u", prio_start, prio_end);
    return L7_FAILURE;
  }

  /* Final configuration */
  if (prio_start == prio_end)
  {
    qos_cfg->priority = prio_start;
    qos_cfg->priority_mask = 7;
    qos_cfg->int_priority = cos_map[prio_start];
    rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_QOS, DAPI_CMD_SET, sizeof(ptin_dtl_qos_t), qos_cfg);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error configuring priority %u", prio_start);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "Priority %u will be mapped to int_prio %u", prio_start, cos_map[prio_start]);

    return L7_SUCCESS;
  }

  /* Check if all entries are equal */
  mask = prio_start;
  for (i = prio_start+1; i <= prio_end; i++)
  {
    if (cos_map[i] != cos_map[i-1])  break;
    mask = ~(mask ^ i) & 0x7;
  }
  /* If all entries are equal, make configuration and leave */
  if (i > prio_end)
  {
    qos_cfg->priority = prio_start;
    qos_cfg->priority_mask = mask;
    qos_cfg->int_priority = cos_map[prio_start];
    rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_QOS, DAPI_CMD_SET, sizeof(ptin_dtl_qos_t), qos_cfg);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error configuring priority %u-%u", prio_start, prio_end);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_INTF, "Priorities %u-%u will be mapped to int_prio %u", prio_start, prio_end, cos_map[prio_start]);

    return L7_SUCCESS;
  }

  /* No configuration done... split priority range in 2 halfs, and try to configure each one */
  /* First half */
  (void) ptin_qos_vlan_add_recursive(qos_cfg, cos_map, prio_start, (prio_end+1-prio_start)/2+prio_start-1);

  /* Second half */
  (void) ptin_qos_vlan_add_recursive(qos_cfg, cos_map, prio_start+(prio_end+1-prio_start)/2, prio_end);

  return L7_SUCCESS;
}
#endif

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
    PT_LOG_ERR(LOG_CTX_API,"Invalid Vlan Id %u",vlanId);
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
    PT_LOG_ERR(LOG_CTX_API,"Error setting broadcast rate limit to vlan %u to %u",vlanId,enable);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_API,"Success setting broadcast rate limit to vlan %u to %u",vlanId,enable);
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
    PT_LOG_ERR(LOG_CTX_API,"Invalid Vlan Id %u",vlanId);
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
    PT_LOG_ERR(LOG_CTX_API,"Error setting multicast rate limit to vlan %u to %u",vlanId,enable);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_API,"Success setting multicast rate limit to vlan %u to %u",vlanId,enable);
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
    PT_LOG_ERR(LOG_CTX_API,"Error consulting hardware resources");
  }

  return rc;
}
