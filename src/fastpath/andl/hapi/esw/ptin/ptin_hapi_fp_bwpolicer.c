#include <bcm/error.h>

#include "ptin_hapi_fp_bwpolicer.h"
#include "ptin_hapi_fp_utils.h"
#include "broad_policy.h"
#include "broad_group_bcm.h"

#include "logger.h"

/******************************************** 
 * INTERNAL VARS
 ********************************************/

#define BW_MAX  100000000UL    /* Maximum bandwidth is 100 Gbps (for LAGs) */

/* Array of bw policer structures */
ptin_bw_policy_t bw_policer_data[PTIN_SYSTEM_MAX_BW_POLICERS];

/* Database to be managed by ptin_hapi_fp_utils module */
ptin_hapi_database_t bw_policer_database;
ptin_hapi_database_t *bwp_db = &bw_policer_database;

/******************************************** 
 * STATIC FUNCTIONS FOR INTERNAL USAGE
 ********************************************/

static void bwPolicy_clear_data(void *policy_ptr);
static L7_BOOL bwPolicy_compare(DAPI_USP_t *usp, void *profile_ptr, const void *policy_ptr);
static L7_BOOL bwPolicy_check_conflicts(DAPI_USP_t *usp, void *profile_ptr, const void *policy_ptr, int stage);
static L7_BOOL bwPolicy_inUse(void *policy_ptr);


/******************************************** 
 * FUNCTIONS FOR EXTERNAL USAGE
 ********************************************/

/**
 * Initialize all database
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t hapi_ptin_bwPolicer_init(void)
{
  ptin_bw_policy_t *policer_ptr;

  bwp_db->database_base              = (void *) bw_policer_data;
  bwp_db->database_num_elems         = PTIN_SYSTEM_MAX_BW_POLICERS;
  bwp_db->database_elem_sizeof       = sizeof(ptin_bw_policy_t);
  bwp_db->database_index_first_free  = 0;
  bwp_db->policy_inUse               = bwPolicy_inUse;
  bwp_db->policy_compare             = bwPolicy_compare;
  bwp_db->policy_check_conflicts     = bwPolicy_check_conflicts;
  bwp_db->policy_clear_data          = bwPolicy_clear_data;

  for (policer_ptr=bw_policer_data; FP_POLICY_VALID_PTR(policer_ptr,bwp_db); policer_ptr++)
  {
    bwPolicy_clear_data(policer_ptr);
  }

  return L7_SUCCESS;
}

/**
 * Create a bw bwPolicer
 *  
 * @param usp    
 * @param bwPolicer : BW bwPolicer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_create(DAPI_USP_t *usp, ptin_bwPolicer_t *bwPolicer, DAPI_t *dapi_g)
{
  bcm_policer_t        policer_id;
  bcm_policer_config_t policer_cfg;
  int                  rv = BCM_E_NONE;
  int unit;

  /* Validate arguments */
  if (bwPolicer == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Profile not provided!");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Meter info:");
  PT_LOG_TRACE(LOG_CTX_HAPI, " {CIR,CBS}= {%u,%u}", bwPolicer->meter.cir, bwPolicer->meter.cbs);
  PT_LOG_TRACE(LOG_CTX_HAPI, " {EIR,EBS}= {%u,%u}", bwPolicer->meter.eir, bwPolicer->meter.ebs);

  bcm_policer_config_t_init(&policer_cfg);

  policer_cfg.flags = 0;
  policer_cfg.mode  = bcmPolicerModeTrTcm;  /* RFC 2698 */
  policer_cfg.ckbits_sec    = bwPolicer->meter.cir;
  policer_cfg.ckbits_burst  = (bwPolicer->meter.cbs * 8) / 1000;
  policer_cfg.pkbits_sec    = bwPolicer->meter.cir + bwPolicer->meter.eir;
  policer_cfg.pkbits_burst  = ((bwPolicer->meter.cbs + bwPolicer->meter.ebs) * 8) / 1000;
  policer_cfg.action_id     = bcmPolicerActionRpDrop;
  policer_cfg.sharing_mode  = 0;

  policer_id = bwPolicer->policer_id;

  PT_LOG_TRACE(LOG_CTX_HAPI,"Gived policer id %d", policer_id);

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Modify already existent policer? */
    if (policer_id > 0)
    {
      PT_LOG_TRACE(LOG_CTX_HAPI,"Reconfiguring policer id %d", policer_id);

      /* Create bwPolicer */
      rv = bcm_policer_set(unit, policer_id, &policer_cfg);
      if (rv != BCM_E_NONE)
      {
        break;
      }
      
      PT_LOG_TRACE(LOG_CTX_HAPI,"Policer id %d reconfigured: rv=%d", policer_id, rv);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_HAPI,"Creating new policer id");

      /* Create bwPolicer */
      rv = bcm_policer_create(unit, &policer_cfg, &policer_id);
      if (rv != BCM_E_NONE)
      {
        break;
      }

      PT_LOG_TRACE(LOG_CTX_HAPI,"New Policer id %d created: rv=%d", policer_id, rv);
    }
  }

  if (BCM_E_NONE != rv)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"We have an error! rv=%d", rv);
    bwPolicer->policer_id = -1;
  }
  else
  {
    bwPolicer->policer_id = policer_id;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"result: rv=%d", rv);

  return rv;
}

/**
 * Destroy a bw bwPolicer
 *  
 * @param usp    
 * @param bwPolicer : BW bwPolicer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_destroy(DAPI_USP_t *usp, ptin_bwPolicer_t *bwPolicer, DAPI_t *dapi_g)
{
  int unit, rv, rv_ret = BCM_E_NONE;

  /* Validate arguments */
  if (bwPolicer == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Profile not provided!");
    return L7_FAILURE;
  }
  /* Check bwPolicer id */
  if (bwPolicer->policer_id <= 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"No bwPolicer configured!");
    return L7_SUCCESS;
  }

  /* Run all units */
  BCM_UNIT_ITER(unit)
  {
    /* Create bwPolicer */
    rv = bcm_policer_destroy(unit, bwPolicer->policer_id);

    if (BCM_E_NONE != rv)
    {
      rv_ret = rv;
      PT_LOG_ERR(LOG_CTX_HAPI,"We have an error! rv=%d", rv);
    }
  }

  return rv_ret;
}


/**
 * Read a field processor entry properties
 *  
 * @param usp   
 * @param bwPolicer : BW bwPolicer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_get(DAPI_USP_t *usp, ptin_bwPolicer_t *bwPolicer, DAPI_t *dapi_g)
{
  ptin_bw_policy_t  *policer_ptr = L7_NULLPTR;

  PT_LOG_TRACE(LOG_CTX_HAPI, "Going to read bwPolicer");

  /* Validate arguments */
  if (bwPolicer == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid arguments");
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Looking to profile to find a matched bwPolicer...");
  PT_LOG_TRACE(LOG_CTX_HAPI,"Profile contents:");
  PT_LOG_TRACE(LOG_CTX_HAPI," ptin_ports= 0x%llx",bwPolicer->profile.ptin_port_bmp);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_in   = %u",bwPolicer->profile.outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_int  = %u",bwPolicer->profile.outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_out  = %u",bwPolicer->profile.outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI," IVID_in   = %u",bwPolicer->profile.inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI," IVID_out  = %u",bwPolicer->profile.inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI," COS       = %u",bwPolicer->profile.cos);
  PT_LOG_TRACE(LOG_CTX_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",
            bwPolicer->profile.macAddr[0], bwPolicer->profile.macAddr[1], bwPolicer->profile.macAddr[2], bwPolicer->profile.macAddr[3], bwPolicer->profile.macAddr[4], bwPolicer->profile.macAddr[5]);

  /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
  policer_ptr = ptin_hapi_policy_find(usp, &bwPolicer->profile, L7_NULLPTR, bwp_db);

  if (policer_ptr != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Database entry found!");
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_HAPI,"This bwPolicer does not exist");
    return L7_NOT_EXIST;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Going to read bwPolicer");

  /* Policer not in use */
  if (!policer_ptr->inUse)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Policer not in use");
    return L7_NOT_EXIST;
  }

  /* Copy meter data */
  bwPolicer->meter = policer_ptr->meter;

  PT_LOG_TRACE(LOG_CTX_HAPI, " ptin_ports= 0x%llx", bwPolicer->profile.ptin_port_bmp);
  PT_LOG_TRACE(LOG_CTX_HAPI, " OVID_in   = %u", bwPolicer->profile.outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_HAPI, " OVID_int  = %u", bwPolicer->profile.outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI, " OVID_out  = %u", bwPolicer->profile.outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI, " IVID_in   = %u", bwPolicer->profile.inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI, " IVID_out  = %u", bwPolicer->profile.inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI, " COS       = %u", bwPolicer->profile.cos);
  PT_LOG_TRACE(LOG_CTX_HAPI, " MAC       = %02x:%02x:%02x:%02x:%02x:%02x",
            bwPolicer->profile.macAddr[0],bwPolicer->profile.macAddr[1],bwPolicer->profile.macAddr[2],bwPolicer->profile.macAddr[3],bwPolicer->profile.macAddr[4],bwPolicer->profile.macAddr[5]);

  PT_LOG_TRACE(LOG_CTX_HAPI, " {CIR,CBS}= {%u,%u}", bwPolicer->meter.cir, bwPolicer->meter.cbs);
  PT_LOG_TRACE(LOG_CTX_HAPI, " {EIR,EBS}= {%u,%u}", bwPolicer->meter.eir, bwPolicer->meter.ebs);

  return L7_SUCCESS;
}

/**
 * Add a new field processor entry
 *  
 * @param usp    
 * @param bwPolicer : BW bwPolicer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_set(DAPI_USP_t *usp, ptin_bwPolicer_t *bwPolicer, DAPI_t *dapi_g)
{
  ptin_bw_profile_t   *profile;           // Profile data
  ptin_bw_meter_t     *meter;             // Meter info
  ptin_bw_policy_t    *policer_ptr = L7_NULLPTR;
  BROAD_POLICY_t       policyId;
  BROAD_POLICY_RULE_t  ruleId  = BROAD_POLICY_RULE_INVALID/*, pcp_ruleId[8]*/;
  BROAD_METER_ENTRY_t  meterInfo;
  BROAD_POLICY_TYPE_t  policyType = BROAD_POLICY_TYPE_PTIN;
  L7_uint8             drop = 0;
  L7_uint8             mask[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_uint64            profile_macAddr;
  L7_RC_t              result;

  DAPI_PORT_t         *dapiPortPtr;
  BROAD_PORT_t        *hapiPortPtr;
  ptin_hapi_intf_t     portDescriptor;
  L7_uint64            usp_bmp;
  bcm_pbmp_t           pbm, pbm_mask;
  BROAD_POLICY_STAGE_t stage = BROAD_POLICY_STAGE_INGRESS;
  /* Define if interfaces are set after commiting rule */
  L7_BOOL              post_commit_intf_set = L7_FALSE;

  /* Trident switch only has 10 FP groups... excluding BW policers for this board */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
  PT_LOG_WARN(LOG_CTX_HAPI, "Not possible to configure BW policers for CXO640G");
  return L7_SUCCESS;
#endif

  PT_LOG_TRACE(LOG_CTX_HAPI,"Starting processing...");

  /* Validate arguments */
  if (bwPolicer == L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"No provided profile!");
  }

  profile = &bwPolicer->profile;
  meter   = &bwPolicer->meter;

  /* Force vport to 0 for board that don't support bw profiles on ingress looking to dst vport */
#if (PTIN_BOARD != PTIN_BOARD_TC16SXG)
  profile->vport = 0;
#endif

  PT_LOG_TRACE(LOG_CTX_HAPI,"Profile contents:");
  PT_LOG_TRACE(LOG_CTX_HAPI, "usp       = {%d,%d,%d}", usp->unit, usp->slot, usp->port);
  PT_LOG_TRACE(LOG_CTX_HAPI," ptin_ports= 0x%llx",profile->ptin_port_bmp);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_in   = %u",profile->outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_int  = %u",profile->outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_out  = %u",profile->outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI," IVID_in   = %u",profile->inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI," IVID_out  = %u",profile->inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI," COS       = %u",profile->cos);
  PT_LOG_TRACE(LOG_CTX_HAPI," Vport     = %u", profile->vport);
  PT_LOG_TRACE(LOG_CTX_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",
            profile->macAddr[0], profile->macAddr[1], profile->macAddr[2], profile->macAddr[3], profile->macAddr[4], profile->macAddr[5]);
  PT_LOG_TRACE(LOG_CTX_HAPI," meter: cir=%u eir=%u cbs=%u ebs=%u", meter->cir, meter->eir, meter->cbs, meter->ebs);
  PT_LOG_TRACE(LOG_CTX_HAPI," Policer ID= %d", bwPolicer->policer_id);

  /* Store MAC address in a more friendly way */
  profile_macAddr = (L7_uint64) profile->macAddr[0]<<40 ||
                    (L7_uint64) profile->macAddr[1]<<32 ||
                    (L7_uint64) profile->macAddr[2]<<24 ||
                    (L7_uint64) profile->macAddr[3]<<16 ||
                    (L7_uint64) profile->macAddr[4]<<8 ||
                    (L7_uint64) profile->macAddr[5];

  PT_LOG_TRACE(LOG_CTX_HAPI,"Looking to profile to find a match bwPolicer...");

  /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
  policer_ptr = ptin_hapi_policy_find(usp, &bwPolicer->profile, L7_NULLPTR, bwp_db);

  if (policer_ptr != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u found!", 
              profile->outer_vlan_lookup, profile->outer_vlan_ingress, profile->inner_vlan_ingress, profile->outer_vlan_egress, profile->inner_vlan_egress);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u not found!", 
              profile->outer_vlan_lookup, profile->outer_vlan_ingress, profile->inner_vlan_ingress, profile->outer_vlan_egress, profile->inner_vlan_egress);
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Validating profile inputs...");

  /* If there is not enough input parameters, remove bwPolicer and leave */
  if ( (/*(usp->unit<0 && usp->slot<0 && usp->port<0) && (profile->ptin_ports_bmp==0) &&
        (profile->outer_vlan_lookup==0 || profile->outer_vlan_lookup>=4096) &&*/
        (profile->outer_vlan_ingress==0 || profile->outer_vlan_ingress>=4096) &&
        (profile->outer_vlan_egress==0 || profile->outer_vlan_egress>=4096) &&
        (profile->inner_vlan_ingress==0 || profile->inner_vlan_ingress>=4096) &&
        (profile->cos > 7) &&
        (profile_macAddr == 0)) ||
       (meter->cir>=BW_MAX ) )
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Found conflicting data");
    if (policer_ptr != L7_NULLPTR)
    {
      hapi_ptin_bwPolicer_delete(usp, bwPolicer, dapi_g);
      policer_ptr = L7_NULLPTR;
      PT_LOG_TRACE(LOG_CTX_HAPI,"Database entry removed");
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_HAPI,"Nothing to do");
    }
    return L7_SUCCESS;
  }

  /* AT THIS POINT POLICER_PTR HAS A VALID ADDRESS */

  /* If we are using a valid database entry, compare input parameters */
  if (policer_ptr != L7_NULLPTR && policer_ptr->inUse)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Policer_ptr is in use: comparing inputs...");
    /* If some input parameter is different, we have to destroy fp policy */
    if ( ( policer_ptr->ddUsp_src.unit      != usp->unit      ) ||
         ( policer_ptr->ddUsp_src.slot      != usp->slot      ) ||
         ( policer_ptr->ddUsp_src.port      != usp->port      ) ||
         ( policer_ptr->ptin_port_bmp       != profile->ptin_port_bmp ) ||
         /*( policer_ptr->outer_vlan_lookup   != profile->outer_vlan_lookup ) ||*/
         ( policer_ptr->outer_vlan_ingress  != profile->outer_vlan_ingress ) ||
         ( policer_ptr->outer_vlan_egress   != profile->outer_vlan_egress  ) ||
         ( policer_ptr->inner_vlan_ingress  != profile->inner_vlan_ingress ) ||
         ( policer_ptr->inner_vlan_egress   != profile->inner_vlan_egress  ) ||
         ( policer_ptr->vport               != profile->vport  ) ||
         ( policer_ptr->cos  != profile->cos && policer_ptr->cos<L7_COS_INTF_QUEUE_MAX_COUNT && profile->cos<L7_COS_INTF_QUEUE_MAX_COUNT) ||
         ( policer_ptr->cos>=L7_COS_INTF_QUEUE_MAX_COUNT && profile->cos<L7_COS_INTF_QUEUE_MAX_COUNT) ||
         ( policer_ptr->cos<L7_COS_INTF_QUEUE_MAX_COUNT && profile->cos>=L7_COS_INTF_QUEUE_MAX_COUNT) ||
         ( memcmp(policer_ptr->macAddr, profile->macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN) != 0 ) ||
         ( policer_ptr->meter.cir  != meter->cir ) ||
         ( policer_ptr->meter.eir  != meter->eir ) ||
         ( policer_ptr->meter.cbs  != meter->cbs ) ||
         ( policer_ptr->meter.ebs  != meter->ebs ) ||
         ( policer_ptr->policer_id != bwPolicer->policer_id ))
    {
      PT_LOG_TRACE(LOG_CTX_HAPI,"Inputs are different... we have to destroy firstly the bwPolicer");
      if (hapi_ptin_bwPolicer_delete(usp, bwPolicer, dapi_g) == L7_SUCCESS)
      {
        PT_LOG_TRACE(LOG_CTX_HAPI,"Policer destroyed");
        policer_ptr = L7_NULLPTR;
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error destroying bwPolicer");
        return L7_FAILURE;
      }
    }
    /* If inputs are the same, there is no need for update */
    else
    {
      PT_LOG_TRACE(LOG_CTX_HAPI,"Inputs are the same... there is nothing to do");
      return L7_SUCCESS;
    }
  }

  /* If bwPolicer was not provided, find a new one */
  /* If not found, we have an error */
  if (policer_ptr == L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Policer_ptr is null: Trying to find a free database entry...");

    if ((policer_ptr=ptin_hapi_policy_find_free(bwp_db)) == L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Free database entry not found... error!");
      return L7_TABLE_IS_FULL;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Free entry found!");
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"Configuring bwPolicer...");

  /* AT THIS POINT ENTRY IN DATABASE MUST BE CONFIGURED IN HARDWARE (inUse==L7_FALSE) */

  // Rate limiter
  meterInfo.cir       = meter->cir;
  meterInfo.cbs       = (meter->cbs * 8) / 1000;
  meterInfo.pir       = meter->cir + meter->eir;
  meterInfo.pbs       = ((meter->cbs + meter->ebs) * 8) / 1000;
  meterInfo.colorMode = BROAD_METER_COLOR_AWARE;

  if ((result=hapiBroadPolicyCreate(policyType))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error creating new policy");
    return result;
  }
  PT_LOG_TRACE(LOG_CTX_HAPI,"New policy created!");

  /* Decide if this policer is to be applied at the ingress OR at the egress */
  if (profile->outer_vlan_egress > 0 && profile->outer_vlan_egress < 4096 && (profile->vport == 0))
  {
    stage = BROAD_POLICY_STAGE_EGRESS;
  }
  else
  {
    stage = BROAD_POLICY_STAGE_INGRESS;
  }

  result = L7_SUCCESS;

  /* Set FP stage */
  if ((result=hapiBroadPolicyStageSet(stage))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    PT_LOG_ERR(LOG_CTX_HAPI,"Error setting stage %u",stage);
    return result;
  }

  /* Set global policer? */
  if (bwPolicer->policer_id > 0)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"policer id %d set", bwPolicer->policer_id);

    if ((result=hapiBroadPolicyPolicerSet(bwPolicer->policer_id)) != L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      PT_LOG_ERR(LOG_CTX_HAPI,"Error setting policer id %u", bwPolicer->policer_id);
      return result;
    }
  }

  if ((result=hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyPriorityRuleAdd");
    return result;
  }
  PT_LOG_TRACE(LOG_CTX_HAPI,"New rule added!");

  usp_bmp = 0;
  portDescriptor.gport    = -1;
  portDescriptor.bcm_port = -1;
  portDescriptor.trunk_id = -1;

  /* Check ports to be used */
  /* For valid usp values */
  if (usp->unit >= 0 && usp->slot >= 0 && usp->port >= 0)
  {
    if (ptin_hapi_portDescriptor_get(usp, dapi_g, &portDescriptor, &dapiPortPtr, &hapiPortPtr) != L7_SUCCESS) 
    {
      hapiBroadPolicyCreateCancel();
      PT_LOG_ERR(LOG_CTX_HAPI,"Error acquiring interface descriptor!");
      return L7_FAILURE;
    }
    usp_bmp = portDescriptor.usp_bmp;

    PT_LOG_TRACE(LOG_CTX_HAPI,"trunk_id=%d bcm_port=%d xlate_class_port=%d",
              portDescriptor.trunk_id, portDescriptor.bcm_port, portDescriptor.xlate_class_port);

    /* For TG16G board, if a trunk was provided, use the uplink ports */
  #if (PTIN_BOARD == PTIN_BOARD_TG16G || PTIN_BOARD == PTIN_BOARD_TG16GF)
    if (portDescriptor.trunk_id >= 0)
    {
      usp_bmp = PTIN_SYSTEM_10G_PORTS_MASK; /* All uplink ports */
    }
  #endif
  }
  /* Use port bitmap, if provided (only at ingress stage) */
  else if (bwPolicer->port_bmp != 0)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Going to use usp_port bitmap 0x%llx", bwPolicer->port_bmp);

    usp_bmp = bwPolicer->port_bmp;
  }

  /* Get pbm format of ports */
  if (hapi_ptin_port_bitmap_get(usp, dapi_g, usp_bmp, &pbm, &pbm_mask) != L7_SUCCESS)
  {
    hapiBroadPolicyDelete(policyId);
    PT_LOG_ERR(LOG_CTX_HAPI, "Error converting port bitmap to pbmp format");
    return L7_FAILURE;
  }

  if (stage == BROAD_POLICY_STAGE_INGRESS)
  {
      if ((profile->vport != 0)) 
      {
        if ((result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_L2INTF_ID, (L7_uint8 *)&profile->vport, (L7_uint8 *)mask)) != L7_SUCCESS) 
        {
          hapiBroadPolicyCreateCancel();
          PT_LOG_ERR(LOG_CTX_HAPI, "Error with hapiBroadPolicyRuleQualifierAdd(OVID)");
          return result;
        }
      }
      else
      {
          /* Trunk qualifier is not supported for TG16G boards (to allow using single-wide rules) */
    #if (PTIN_BOARD != PTIN_BOARD_TG16G && PTIN_BOARD != PTIN_BOARD_TG16GF)
        /* Trunk id field */
        if (portDescriptor.trunk_id >= 0)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SRCTRUNK, (L7_uint8 *)&portDescriptor.trunk_id, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            hapiBroadPolicyCreateCancel();
            PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(SRCTRUNK)");
            return result;
          }
          PT_LOG_TRACE(LOG_CTX_HAPI,"TrunkId qualifier added");
        }
        else
    #endif
        if (usp_bmp != 0)
        {
    #ifdef ICAP_INTERFACES_SELECTION_BY_CLASSPORT
          /* Interfaces will be set after committing rule */
          post_commit_intf_set = L7_TRUE;
    #else
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uint8 *)&pbm, (L7_uint8 *)&pbm_mask))!=L7_SUCCESS)
          {
            hapiBroadPolicyCreateCancel();
            PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(INPORTS)");
            return result;
          }
          PT_LOG_TRACE(LOG_CTX_HAPI,"InPorts qualifier added");
    #endif
        }

        /* Internal vlans */
        if (profile->outer_vlan_ingress>0 && profile->outer_vlan_ingress<4096)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uint8 *)&profile->outer_vlan_ingress, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            hapiBroadPolicyCreateCancel();
            PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OVID)");
            return result;
          }
          PT_LOG_TRACE(LOG_CTX_HAPI,"SVid qualifier added");
        }
        if (profile->inner_vlan_ingress>0 && profile->inner_vlan_ingress<4096)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uint8 *)&profile->inner_vlan_ingress, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            hapiBroadPolicyCreateCancel();
            PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(IVID)");
            return result;
          }
          PT_LOG_TRACE(LOG_CTX_HAPI,"IVID qualifier added");
        }
      }
  }
  else if (stage == BROAD_POLICY_STAGE_EGRESS)
  {
    /* Interfaces will be set after committing rule */
    post_commit_intf_set = L7_TRUE;

    /* External vlans */
    if (profile->outer_vlan_egress>0 && profile->outer_vlan_egress<4096)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uint8 *)&profile->outer_vlan_egress, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OVID)");
        return result;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"SVid qualifier added");
    }
    if (profile->inner_vlan_egress>0 && profile->inner_vlan_egress<4096)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uint8 *)&profile->inner_vlan_egress, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(IVID)");
        return result;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"IVID qualifier added");
    }
  }

  /* MAC address */
  if (profile_macAddr != 0)
  {
    if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, (L7_uint8 *) profile->macAddr, (L7_uint8 *) mask))!=L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(BROAD_FIELD_MACDA)");
      return result;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"BROAD_FIELD_MACDA qualifier added");
  }

  if (stage != BROAD_POLICY_STAGE_EGRESS)
  {
    if ((result = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uint8 * ) & drop, (L7_uint8 * )mask)) != L7_SUCCESS) 
    {
      hapiBroadPolicyCreateCancel();
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(DROP)");
      return result;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Drop qualifier added");
  }

  /* Drop red packets */
  result = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  if (result != L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleNonConfActionAdd");
    return result;
  }
  /* Only apply drop precedence to ingress stage */
  if (stage == BROAD_POLICY_STAGE_INGRESS)
  {
    result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_DROPPREC, BROAD_COLOR_GREEN, BROAD_COLOR_YELLOW, BROAD_COLOR_RED);
    if (result != L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleNonConfActionAdd");
      return result;
    }
  }

  /* Only configure local policer, if policer id was given */
  if (bwPolicer->policer_id <= 0)
  {
    if ((result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo)) != L7_SUCCESS) 
    {
      hapiBroadPolicyCreateCancel();
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleMeterAdd");
      return result;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Meter added");
  }

  #if 1
  if (profile->cos < L7_COS_INTF_QUEUE_MAX_COUNT)
  {
    BROAD_POLICY_FIELD_t fp_field;

#if ((PTIN_BOARD == PTIN_BOARD_CXO160G) || (PTIN_BOARD == PTIN_BOARD_TA48GE) || \
     (PTIN_BOARD == PTIN_BOARD_OLT1T0) || (PTIN_BOARD == PTIN_BOARD_OLT1T0F))
    L7_uint8  priority = profile->cos;
    fp_field = BROAD_FIELD_INT_PRIO;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, fp_field, (L7_uchar8 *) &priority, mask);
#else
    L7_uint32 class_id = profile->cos;
    fp_field = BROAD_FIELD_CLASS_ID;
    result = hapiBroadPolicyRuleQualifierAdd(ruleId, fp_field, (L7_uchar8 *) &class_id, mask);
#endif
    if (result != L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(INT_PRIO)");
      return result;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"OCOS (priority=%u) qualifier added", profile->cos);
  }
  #else
  if (profile->cos < L7_COS_INTF_QUEUE_MAX_COUNT &&
      usp != L7_NULLPTR && usp->unit >= 0 && usp->slot >= 0 && usp->port >= 0)
  {
      L7_uint8 i, cos, n_pcps;
      L7_uchar8 v;
      //ptin_intf_t ptin_intf;
      //ptin_Qos_intf_t intfQos;//msg_QoSConfiguration_t qos;
      BROAD_PORT_t *hapiPortPtr;

      //ptin_QoS_intf_config_get(&ptin_intf, &intfQos)//ptin_msg_CoS_get(&qos);
      hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

      for (i=0, n_pcps=0; i<=L7_DOT1P_MAX_PRIORITY; i++) {
          cos=  hapiPortPtr->dot1pMap[i];//intfQos.pktprio.cos[i];
          if (cos!=profile->cos) continue;//L7_COS_INTF_QUEUE_MAX_COUNT

          if (0==n_pcps) {
              pcp_ruleId[0]=ruleId;
          }
          else {
              pcp_ruleId[n_pcps]=BROAD_POLICY_RULE_INVALID;
              hapiBroadPolicyRuleCopy(pcp_ruleId[n_pcps-1], &pcp_ruleId[n_pcps]);
          }

          v=i;
          if ((result=hapiBroadPolicyRuleQualifierAdd(pcp_ruleId[n_pcps], BROAD_FIELD_OCOS, (L7_uint8 *)&v, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            hapiBroadPolicyCreateCancel();
            PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OCOS)");
            return result;
          }
          PT_LOG_TRACE(LOG_CTX_HAPI,"OCOS (cos(pcp=%u)=%u) qualifier added", i, cos);

          n_pcps++;
      }
  }//if
  #endif

  if ((result=hapiBroadPolicyCommit(&policyId))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with hapiBroadPolicyCommit");
    return result;
  }
  PT_LOG_TRACE(LOG_CTX_HAPI,"Policy committed");

  /* If interfaces are to be set after commiting rule, do it now */
  if (post_commit_intf_set)
  {
    /* Add bitmap of ports to policy */
    if (hapiBroadPolicyApplyToMultiIface(policyId, pbm) != L7_SUCCESS)
    {
      hapiBroadPolicyDelete(policyId);
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding port bitmap to policyId %u", policyId);
      return L7_FAILURE;
    }
  }

  /* AT THIS POINT, THE NEW POLICER IS APPLIED TO HARDWARE */

  policer_ptr->ddUsp_src            = *usp;
  policer_ptr->ptin_port_bmp        = profile->ptin_port_bmp;
  policer_ptr->outer_vlan_lookup    = profile->outer_vlan_lookup;
  policer_ptr->outer_vlan_ingress   = profile->outer_vlan_ingress;
  policer_ptr->outer_vlan_egress    = profile->outer_vlan_egress;
  policer_ptr->inner_vlan_ingress   = profile->inner_vlan_ingress;
  policer_ptr->inner_vlan_egress    = profile->inner_vlan_egress;
  policer_ptr->cos                  = profile->cos;
  policer_ptr->vport                = profile->vport;
  memcpy(policer_ptr->macAddr, profile->macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  policer_ptr->meter                = bwPolicer->meter;
  policer_ptr->policer_id           = bwPolicer->policer_id;
  policer_ptr->policy_id            = policyId;
                              
  policer_ptr->inUse                = L7_TRUE;

  /* Search for the following empty entry in database */
  ptin_hapi_policy_find_free(bwp_db);

  PT_LOG_TRACE(LOG_CTX_HAPI,"... Processing finished successfully!");

  /* Success */
  return L7_SUCCESS;
}

/**
 * Remove a field processor entry
 *  
 * @param usp     
 * @param bwPolicer : BW bwPolicer
 * @param dapi_g 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_delete(DAPI_USP_t *usp, ptin_bwPolicer_t *bwPolicer, DAPI_t *dapi_g)
{
  ptin_bw_profile_t *profile;           // Profile data
  ptin_bw_meter_t   *meter;             // Meter info
  ptin_bw_policy_t  *policer_ptr = L7_NULLPTR;

  PT_LOG_TRACE(LOG_CTX_HAPI,"Going to destroy bwPolicer...");

  /* Validate arguments */
  if (bwPolicer == L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"No provided profile!");
  }

  profile = &bwPolicer->profile;
  meter   = &bwPolicer->meter;

  PT_LOG_TRACE(LOG_CTX_HAPI,"Profile contents:");
  PT_LOG_TRACE(LOG_CTX_HAPI," ptin_ports= 0x%llx",profile->ptin_port_bmp);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_in   = %u",profile->outer_vlan_lookup);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_int  = %u",profile->outer_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI," OVID_out  = %u",profile->outer_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI," IVID_in   = %u",profile->inner_vlan_ingress);
  PT_LOG_TRACE(LOG_CTX_HAPI," IVID_out  = %u",profile->inner_vlan_egress);
  PT_LOG_TRACE(LOG_CTX_HAPI," COS       = %u",profile->cos);
  PT_LOG_TRACE(LOG_CTX_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);

  PT_LOG_TRACE(LOG_CTX_HAPI,"Looking to profile to find a match bwPolicer...");

  /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
  policer_ptr = ptin_hapi_policy_find(usp, &bwPolicer->profile, L7_NULLPTR, bwp_db);

  if (policer_ptr != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u found!", 
              profile->outer_vlan_lookup, profile->outer_vlan_ingress, profile->inner_vlan_ingress, profile->outer_vlan_egress, profile->inner_vlan_egress);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u not found!", 
              profile->outer_vlan_lookup, profile->outer_vlan_ingress, profile->inner_vlan_ingress, profile->outer_vlan_egress, profile->inner_vlan_egress);
  }

  /* Validate arguments */
  if (policer_ptr == L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"This bwPolicer does not exist");
    return L7_SUCCESS;
  }

  /* Is there need to destroy this bwPolicer? */
  if (!policer_ptr->inUse)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"This bwPolicer does not exist");
    return L7_SUCCESS;
  }

  /* Destroy bwPolicer */
  if (policer_ptr->policy_id > 0)
  {
    if (hapiBroadPolicyDelete(policer_ptr->policy_id)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error destroying policy");
      return L7_FAILURE;
    }
  }

  /* Clear element in database */
  ptin_hapi_policy_clear(policer_ptr, bwp_db);

  PT_LOG_TRACE(LOG_CTX_HAPI,"Policer destroyed!");

  return L7_SUCCESS;
}

/**
 * Remove all field processor entries
 *  
 * @param usp      
 * @param bwPolicer : BW bwPolicer
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_deleteAll(DAPI_USP_t *usp, ptin_bwPolicer_t *bwPolicer, DAPI_t *dapi_g)
{
  ptin_bw_policy_t  *policer_ptr = L7_NULLPTR;
  L7_RC_t rc, rc_policer, rc_global=L7_SUCCESS;

  if (usp != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "ddUsp     = {%d,%d,%d}", usp->unit, usp->slot, usp->port); 
  }
  if (bwPolicer != L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Profile contents:");
    PT_LOG_TRACE(LOG_CTX_HAPI," ptin_ports= 0x%llx",bwPolicer->profile.ptin_port_bmp);
    PT_LOG_TRACE(LOG_CTX_HAPI," OVID_in   = %u",bwPolicer->profile.outer_vlan_lookup);
    PT_LOG_TRACE(LOG_CTX_HAPI," OVID_int  = %u",bwPolicer->profile.outer_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_HAPI," OVID_out  = %u",bwPolicer->profile.outer_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_HAPI," IVID_in   = %u",bwPolicer->profile.inner_vlan_ingress);
    PT_LOG_TRACE(LOG_CTX_HAPI," IVID_out  = %u",bwPolicer->profile.inner_vlan_egress);
    PT_LOG_TRACE(LOG_CTX_HAPI," COS       = %u",bwPolicer->profile.cos);
    PT_LOG_TRACE(LOG_CTX_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",bwPolicer->profile.macAddr[0],bwPolicer->profile.macAddr[1],bwPolicer->profile.macAddr[2],bwPolicer->profile.macAddr[3],bwPolicer->profile.macAddr[4],bwPolicer->profile.macAddr[5]);
  }

  /* Get first bwPolicer */
  policer_ptr = L7_NULLPTR;

  /* Until there is policers, remove them */
  while ((policer_ptr = ptin_hapi_policy_next(policer_ptr, bwp_db)) != L7_NULLPTR)
  {
    /* Validate arguments */
    if (!FP_POLICY_VALID_PTR(policer_ptr, bwp_db))
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Invalid bwPolicer element");
      rc_global = L7_FAILURE;
      continue;
    }

    /* Is there need to destroy this bwPolicer? */
    if (policer_ptr->inUse)
    {
      /* Filter parameters: only consider (port) source parameters (not destination) */
      /* Only not null values will be considered */

      if ((usp != L7_NULLPTR) &&
          (usp->unit >= 0 && usp->slot >= 0 && usp->port >= 0) &&
          (IS_SLOT_TYPE_PHYSICAL(usp, dapi_g) || IS_SLOT_TYPE_LOGICAL_LAG(usp, dapi_g)))
      {
        /* USP matches? */
        if (usp->unit!=policer_ptr->ddUsp_src.unit || usp->slot!=policer_ptr->ddUsp_src.slot || usp->port!=policer_ptr->ddUsp_src.port)
        {
          PT_LOG_TRACE(LOG_CTX_HAPI,"Different port");
          continue;
        }
      }
      /* Check profile */
      if (bwPolicer != L7_NULLPTR)
      {
        #if 0
        /* ptin_ports match? */
        if (bwPolicer->profile.ptin_port_bmp != policer_ptr->ptin_port_bmp)
        {
          PT_LOG_TRACE(LOG_CTX_HAPI,"Different ptin_ports list");
          continue;
        }
        #endif

        #if 0
        /* Input vlan matches? */
        if ((bwPolicer->profile.outer_vlan_lookup >= 1 && bwPolicer->profile.outer_vlan_lookup <= 4095) &&
            (bwPolicer->profile.outer_vlan_lookup != policer_ptr->outer_vlan_lookup))
        {
          continue;
        }
        #endif
        /* Output vlan matches? */
        if ((bwPolicer->profile.outer_vlan_egress >= 1 && bwPolicer->profile.outer_vlan_egress <= 4095) &&
            (bwPolicer->profile.outer_vlan_egress != policer_ptr->outer_vlan_egress))
        {
          PT_LOG_TRACE(LOG_CTX_HAPI,"Different outer output vlan");
          continue;
        }
        /* Internal vlan matches? */
        if ((bwPolicer->profile.outer_vlan_ingress >= 1 && bwPolicer->profile.outer_vlan_ingress <= 4095) &&
            (bwPolicer->profile.outer_vlan_ingress != policer_ptr->outer_vlan_ingress))
        {
          PT_LOG_TRACE(LOG_CTX_HAPI,"Different internal vlan");
          continue;
        }
        /* Inner VLAN matches? */
        if ((bwPolicer->profile.inner_vlan_ingress >= 1 && bwPolicer->profile.inner_vlan_ingress <= 4095) &&
            (bwPolicer->profile.inner_vlan_ingress != policer_ptr->inner_vlan_ingress))
        {
          PT_LOG_TRACE(LOG_CTX_HAPI,"Different inner vlan");
          continue;
        }
        /* Output vlan matches? */
        if ((bwPolicer->profile.macAddr[0]!=0 || bwPolicer->profile.macAddr[1]!=0 || bwPolicer->profile.macAddr[2]!=0 || bwPolicer->profile.macAddr[3]!=0 || bwPolicer->profile.macAddr[4]!=0 || bwPolicer->profile.macAddr[5]!=0) &&
            (memcmp(bwPolicer->profile.macAddr, policer_ptr->macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN) != 0))
        {
          PT_LOG_TRACE(LOG_CTX_HAPI,"Different MAC address");
          continue;
        }
        /* Destination IP address matches? */
        #if 0
        if ((bwPolicer->profile.cos != (L7_uchar8)-1) &&
            (bwPolicer->profile.cos != policer_ptr->cos))
        {
          PT_LOG_TRACE(LOG_CTX_HAPI,"Different COS");
          continue;
        }
        #endif
      }

      PT_LOG_TRACE(LOG_CTX_HAPI,"Proceeding to deletion...");

      rc_policer = L7_SUCCESS;

      /* Destroy bwPolicer */
      if (policer_ptr->policy_id > 0)
      {
        rc = hapiBroadPolicyDelete(policer_ptr->policy_id);

        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_HAPI,"Error destroying policy (policerId=%u): usp={%d,%d,%d}/ptin_ports=0x%llx, OVLAN_in=%u, OVLAN_int=%u, IVLAN_in=%u, OVLAN_out=%u",
                  policer_ptr->policy_id,
                  policer_ptr->ddUsp_src.unit, policer_ptr->ddUsp_src.slot, policer_ptr->ddUsp_src.port, policer_ptr->ptin_port_bmp,
                  policer_ptr->outer_vlan_lookup, policer_ptr->outer_vlan_ingress, policer_ptr->inner_vlan_ingress, policer_ptr->outer_vlan_egress);
          rc_policer  = rc;
          rc_global   = rc;
        }
      }

      /* If success, clear element in database */
      if (rc_policer == L7_SUCCESS)
      {
        PT_LOG_TRACE(LOG_CTX_HAPI,"Policy destroyed: usp={%d,%d,%d}/ptin_ports=0x%llx, OVLAN_in=%u, OVLAN_int=%u, IVLAN_in=%u, OVLAN_out=%u",
                  policer_ptr->ddUsp_src.unit, policer_ptr->ddUsp_src.slot, policer_ptr->ddUsp_src.port, policer_ptr->ptin_port_bmp,
                  policer_ptr->outer_vlan_lookup, policer_ptr->outer_vlan_ingress, policer_ptr->inner_vlan_ingress, policer_ptr->outer_vlan_egress);
        ptin_hapi_policy_clear(policer_ptr, bwp_db);
      }
    }
    else
    {
      PT_LOG_WARN(LOG_CTX_HAPI,"This bwPolicer does not exist");
    }
  }

  /* Return global status */
  return rc_global;
}


/******************************************** 
 * STATIC FUNCTIONS FOR INTERNAL USAGE
 ********************************************/

/**
 * Function to check if database element is in use
 * 
 * @param policy_ptr : pointer to database element
 * 
 * @return L7_BOOL : L7_TRUE if in use / L7_FALSE if not
 */
static L7_BOOL bwPolicy_inUse(void *policy_ptr)
{
  ptin_bw_policy_t *ptr = (ptin_bw_policy_t *) policy_ptr;

  return ptr->inUse;
}

/**
 * Function used to clear database element
 * 
 * @param policy_ptr : pointer to database element
 */
static void bwPolicy_clear_data(void *policy_ptr)
{
  ptin_bw_policy_t *ptr = (ptin_bw_policy_t *) policy_ptr;

  ptr->inUse                = L7_FALSE;
  ptr->policy_id            = 0;
  ptr->ddUsp_src.unit       = ptr->ddUsp_src.slot = ptr->ddUsp_src.port = -1;
  ptr->ptin_port_bmp        = 0;
  ptr->outer_vlan_lookup    = 0;
  ptr->outer_vlan_ingress   = 0;
  ptr->outer_vlan_egress    = 0;
  ptr->inner_vlan_ingress   = 0;
  ptr->inner_vlan_egress    = 0;
  ptr->cos                  = -1;
  memset(ptr->macAddr, 0x00, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  ptr->meter.cir = ptr->meter.cbs = 0;
  ptr->meter.eir = ptr->meter.ebs = 0;
  ptr->policer_id = -1;
}

/**
 * Function used for database comparison
 * 
 * @param profile_ptr : Profile data
 * @param policy_ptr : Pointer to database
 * 
 * @return L7_BOOL : L7_TRUE if equal / L7_FALSE if not
 */
static L7_BOOL bwPolicy_compare(DAPI_USP_t *usp, void *profile_ptr, const void *policy_ptr)
{
  ptin_bw_profile_t *profile = (ptin_bw_profile_t *) profile_ptr;
  const ptin_bw_policy_t *ptr = (const ptin_bw_policy_t *) policy_ptr;

  /* Skip empty elements */
  if (!ptr->inUse)  return L7_FALSE;

  /* Verify src interface */
  if ((usp != L7_NULLPTR) &&
      /*(usp->unit >= 0 && usp->slot >= 0 && usp->port >= 0) &&*/
      (usp->unit != ptr->ddUsp_src.unit ||
       usp->slot != ptr->ddUsp_src.slot ||
       usp->port != ptr->ddUsp_src.port /*||
       profile->ptin_port_bmp != ptr->ptin_port_bmp*/))  return L7_FALSE;

  /* Verify SVID*/
  //if (profile->outer_vlan_lookup       != ptr->outer_vlan_lookup)       return L7_FALSE;
  if (profile->outer_vlan_ingress != ptr->outer_vlan_ingress) return L7_FALSE;
  if (profile->outer_vlan_egress  != ptr->outer_vlan_egress)  return L7_FALSE;

  /* Verify IVID */
  if (profile->inner_vlan_ingress !=ptr->inner_vlan_ingress ) return L7_FALSE;
  if (profile->inner_vlan_egress  !=ptr->inner_vlan_egress)   return L7_FALSE;

  /* COS */
  if (profile->cos != ptr->cos)  return L7_FALSE;

  /* MAC addr */
  if (memcmp(profile->macAddr, ptr->macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN) != 0)  return L7_FALSE;

  return L7_TRUE;
}

/**
 * Function used for conflicts detection between different 
 * policies 
 * 
 * @param profile_ptr : Profile data
 * @param policy_ptr : Pointer to database 
 * @param stage: ingress or egress 
 * 
 * @return L7_BOOL : L7_TRUE if there is conflict / L7_FALSE if 
 *         not
 */
static L7_BOOL bwPolicy_check_conflicts(DAPI_USP_t *usp, void *profile_ptr, const void *policy_ptr, int stage)
{
  return L7_FALSE;
}

/**
 * Dump list of bw policers
 */
void ptin_bwpolicer_dump_debug(L7_uint32 vid_internal)
{
  L7_int index;
  ptin_bw_policy_t *ptr;
  BROAD_GROUP_t group_id;
  BROAD_ENTRY_t entry_id;
  L7_int policer_id, counter_id;  /* PTin added: SDK 6.3.0 */

  printf("Listing bandwidth policer list...\r\n");

  for (index=0; index<PTIN_SYSTEM_MAX_BW_POLICERS; index++)
  {
    if (!bw_policer_data[index].inUse)  continue;

    ptr = &bw_policer_data[index];

    /* Filter entries */
    if (vid_internal > 0 && vid_internal < 4096 &&
        vid_internal != ptr->outer_vlan_ingress)
    {
      continue;
    }

    printf("Index %d:\r\n",index);
    printf("  inUse     = %u\r\n",ptr->inUse);
    printf("  ddUsp_src = {%d,%d,%d} ptin_ports=0x%llx\r\n",ptr->ddUsp_src.unit,ptr->ddUsp_src.slot,ptr->ddUsp_src.port,ptr->ptin_port_bmp);
    printf("  ptin_ports= 0x%llx", ptr->ptin_port_bmp);
    //printf("  OVID_in  = %u\r\n",ptr->outer_vlan_lookup);
    printf("  OVID_int  = %u\r\n",ptr->outer_vlan_ingress);
    printf("  OVID_out  = %u\r\n",ptr->outer_vlan_egress);
    printf("  IVID_in   = %u\r\n",ptr->inner_vlan_ingress);
    printf("  IVID_out  = %u\r\n",ptr->inner_vlan_egress);
    printf("  COS       = %u\r\n",ptr->cos);
    printf("  MAC       = %02x:%02x:%02x:%02x:%02x:%02x\r\n",ptr->macAddr[0],ptr->macAddr[1],ptr->macAddr[2],ptr->macAddr[3],ptr->macAddr[4],ptr->macAddr[5]);
    printf("  meter: cir=%u eir=%u cbs=%u ebs=%u\r\n",ptr->meter.cir,ptr->meter.eir,ptr->meter.cbs,ptr->meter.ebs);
    printf("  policer_id= %u\r\n",ptr->policer_id);
    printf("  policy_id = %u\r\n",ptr->policy_id);
    /* Also print hw group id and entry id*/
    /* PTin modified: SDK 6.3.0 */
    /* FIXME: Only applied to unit 0 */
    if (l7_bcm_policy_hwInfo_get(0 /*unit*/,ptr->policy_id,0,&group_id,&entry_id,&policer_id,&counter_id)==L7_SUCCESS)
    {
      printf("  group=%d, entry=%d, policer_id=%d, counter_id=%d\r\n",group_id,entry_id,policer_id,counter_id);
    }
  }
  fflush(stdout);
}

/**
 * Flush all Policers
 */
void ptin_bwpolicer_flush_debug(void)
{
  printf("Flushing all counters...\r\n");

  hapi_ptin_bwPolicer_deleteAll(L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);

  printf("Counters flushed!\r\n");
  fflush(stdout);
}

