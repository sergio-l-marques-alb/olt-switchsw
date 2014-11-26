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
  ptin_bw_policy_t *policer;

  bwp_db->database_base              = (void *) bw_policer_data;
  bwp_db->database_num_elems         = PTIN_SYSTEM_MAX_BW_POLICERS;
  bwp_db->database_elem_sizeof       = sizeof(ptin_bw_policy_t);
  bwp_db->database_index_first_free  = 0;
  bwp_db->policy_inUse               = bwPolicy_inUse;
  bwp_db->policy_compare             = bwPolicy_compare;
  bwp_db->policy_check_conflicts     = bwPolicy_check_conflicts;
  bwp_db->policy_clear_data          = bwPolicy_clear_data;

  for (policer=bw_policer_data; FP_POLICY_VALID_PTR(policer,bwp_db); policer++)
  {
    bwPolicy_clear_data(policer);
  }

  return L7_SUCCESS;
}

/**
 * Read a field processor entry properties
 *  
 * @param usp   
 * @param profile : BW profile (returned data) 
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_get(DAPI_USP_t *usp, ptin_bw_profile_t *profile, DAPI_t *dapi_g)
{
  ptin_bw_policy_t *policer_ptr = L7_NULLPTR;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Going to read policer");

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid arguments");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Looking to profile to find a matched policer...");
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Profile contents:");
  LOG_TRACE(LOG_CTX_PTIN_HAPI," ptin_port = %d",profile->ptin_port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in   = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_int  = %u",profile->outer_vlan_internal);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out  = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in   = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out  = %u",profile->inner_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," COS       = %u",profile->cos);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);

  /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
  policer_ptr = ptin_hapi_policy_find(usp, profile, L7_NULLPTR, bwp_db);

  if (policer_ptr != L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry found!");
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"This policer does not exist");
    return L7_NOT_EXIST;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Going to read policer");

  /* Policer not in use */
  if (!policer_ptr->inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Policer not in use");
    return L7_NOT_EXIST;
  }

  /* Copy meter data */
  profile->meter = policer_ptr->meter;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, " ptin_port = %d", profile->ptin_port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " OVID_in   = %u", profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " OVID_int  = %u", profile->outer_vlan_internal);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " OVID_out  = %u", profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " IVID_in   = %u", profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " IVID_out  = %u", profile->inner_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " COS       = %u", profile->cos);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " MAC       = %02x:%02x:%02x:%02x:%02x:%02x",
            profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " {CIR,CBS}= %u", profile->meter.cir, profile->meter.cbs);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " {EIR,EBS}= %u", profile->meter.eir, profile->meter.ebs);

  return L7_SUCCESS;
}

/**
 * Add a new field processor entry
 *  
 * @param usp    
 * @param profile : BW profile 
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_set(DAPI_USP_t *usp, ptin_bw_profile_t *profile, DAPI_t *dapi_g)
{
  ptin_bw_policy_t     *policer_ptr = L7_NULLPTR;
  BROAD_POLICY_t       policyId;
  BROAD_POLICY_RULE_t  ruleId  = BROAD_POLICY_RULE_INVALID, pcp_ruleId[8];
  BROAD_METER_ENTRY_t  meterInfo;
  BROAD_POLICY_TYPE_t  policyType = BROAD_POLICY_TYPE_PTIN;
  L7_uint8             drop = 0;
  L7_uint8             mask[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_uint64            profile_macAddr;
  L7_RC_t              result;
  ptin_hapi_intf_t     portDescriptor;
  pbmp_t               pbm, pbm_mask;
  BROAD_POLICY_STAGE_t stage = BROAD_POLICY_STAGE_INGRESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Starting processing...");

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"No provided profile!");
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Profile contents:");
  LOG_TRACE(LOG_CTX_PTIN_HAPI," ptin_port = %d",profile->ptin_port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in   = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_int  = %u",profile->outer_vlan_internal);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out  = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in   = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out  = %u",profile->inner_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," COS       = %u",profile->cos);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," meter: cir=%u eir=%u cbs=%u ebs=%u",profile->meter.cir,profile->meter.eir,profile->meter.cbs,profile->meter.ebs);

  /* Store MAC address in a more friendly way */
  profile_macAddr = (L7_uint64) profile->macAddr[0]<<40 ||
                    (L7_uint64) profile->macAddr[1]<<32 ||
                    (L7_uint64) profile->macAddr[2]<<24 ||
                    (L7_uint64) profile->macAddr[3]<<16 ||
                    (L7_uint64) profile->macAddr[4]<<8 ||
                    (L7_uint64) profile->macAddr[5];

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Looking to profile to find a match policer...");

  /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
  policer_ptr = ptin_hapi_policy_find(usp, profile, L7_NULLPTR, bwp_db);

  if (policer_ptr != L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u found!", 
              profile->outer_vlan_in, profile->outer_vlan_internal, profile->inner_vlan_in, profile->outer_vlan_out, profile->inner_vlan_out);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u not found!", 
              profile->outer_vlan_in, profile->outer_vlan_internal, profile->inner_vlan_in, profile->outer_vlan_out, profile->inner_vlan_out);
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Validating profile inputs...");

  /* If there is not enough input parameters, remove policer and leave */
  if ( ((usp->unit<0 && usp->slot<0 && usp->port<0) &&
        /*(profile->outer_vlan_in==0 || profile->outer_vlan_in>=4096) &&*/
        (profile->outer_vlan_internal==0 || profile->outer_vlan_internal>=4096) &&
        (profile->outer_vlan_out==0 || profile->outer_vlan_out>=4096) &&
        (profile->inner_vlan_in==0 || profile->inner_vlan_in>=4096) &&
        (profile_macAddr == 0)) ||
       (profile->meter.cir>=BW_MAX ) )
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Found conflicting data");
    if (policer_ptr != L7_NULLPTR)
    {
      hapi_ptin_bwPolicer_delete(usp, profile, dapi_g);
      policer_ptr = L7_NULLPTR;
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry removed");
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Nothing to do");
    }
    return L7_SUCCESS;
  }

  /* AT THIS POINT POLICER_PTR HAS A VALID ADDRESS */

  /* If we are using a valid database entry, compare input parameters */
  if (policer_ptr != L7_NULLPTR && policer_ptr->inUse)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer_ptr is in use: comparing inputs...");
    /* If some input parameter is different, we have to destroy fp policy */
    if ( ( policer_ptr->ddUsp_src.unit      != usp->unit      ) ||
         ( policer_ptr->ddUsp_src.slot      != usp->slot      ) ||
         ( policer_ptr->ddUsp_src.port      != usp->port      ) ||
         /*( policer_ptr->outer_vlan_in       != profile->outer_vlan_in       ) ||*/
         ( policer_ptr->outer_vlan_internal != profile->outer_vlan_internal ) ||
         ( policer_ptr->outer_vlan_out      != profile->outer_vlan_out      ) ||
         ( policer_ptr->inner_vlan_in       != profile->inner_vlan_in       ) ||
         ( policer_ptr->inner_vlan_out      != profile->inner_vlan_out      ) ||
         ( policer_ptr->cos  != profile->cos && policer_ptr->cos<L7_COS_INTF_QUEUE_MAX_COUNT && profile->cos<L7_COS_INTF_QUEUE_MAX_COUNT) ||
         ( policer_ptr->cos>=L7_COS_INTF_QUEUE_MAX_COUNT && profile->cos<L7_COS_INTF_QUEUE_MAX_COUNT) ||
         ( policer_ptr->cos<L7_COS_INTF_QUEUE_MAX_COUNT && profile->cos>=L7_COS_INTF_QUEUE_MAX_COUNT) ||
         ( memcmp(policer_ptr->macAddr, profile->macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN) != 0 ) ||
         ( policer_ptr->meter.cir != profile->meter.cir ) ||
         ( policer_ptr->meter.eir != profile->meter.eir ) ||
         ( policer_ptr->meter.cbs != profile->meter.cbs ) ||
         ( policer_ptr->meter.ebs != profile->meter.ebs ) )
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Inputs are different... we have to destroy firstly the policer");
      if (hapi_ptin_bwPolicer_delete(usp, profile, dapi_g) == L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer destroyed");
        policer_ptr = L7_NULLPTR;
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error destroying policer");
        return L7_FAILURE;
      }
    }
    /* If inputs are the same, there is no need for update */
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Inputs are the same... there is nothing to do");
      return L7_SUCCESS;
    }
  }

  /* If policer was not provided, find a new one */
  /* If not found, we have an error */
  if (policer_ptr == L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer_ptr is null: Trying to find a free database entry...");

    if ((policer_ptr=ptin_hapi_policy_find_free(bwp_db)) == L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Free database entry not found... error!");
      return L7_TABLE_IS_FULL;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Free entry found!");
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Configuring policer...");

  /* AT THIS POINT ENTRY IN DATABASE MUST BE CONFIGURED IN HARDWARE (inUse==L7_FALSE) */

  // Rate limiter
  meterInfo.cir       = profile->meter.cir;
  meterInfo.cbs       = (profile->meter.cbs * 8) / 1000;
  meterInfo.pir       = profile->meter.cir + profile->meter.eir;
  meterInfo.pbs       = ((profile->meter.cbs + profile->meter.ebs) * 8) / 1000;
  meterInfo.colorMode = BROAD_METER_COLOR_AWARE;

  if ((result=hapiBroadPolicyCreate(policyType))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error creating new policy");
    return result;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"New policy created!");

  /* Validate interface: should be valid */
  if ((usp->slot < 0) || (usp->port < 0))
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"No interface provided!");
    return L7_FAILURE;
  }

  /* Decide if this policer is to be applied at the ingress OR at the egress */
  if (profile->outer_vlan_out > 0 && profile->outer_vlan_out < 4096)
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
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error setting stage %u",stage);
    return result;
  }

  if ((result=hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyPriorityRuleAdd");
    return result;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"New rule added!");

  /* Init interfaces mask (for inports field) */
  hapi_ptin_allportsbmp_get(&pbm_mask);

  BCM_PBMP_CLEAR(pbm);
  portDescriptor.lport    = -1;
  portDescriptor.bcm_port = -1;
  portDescriptor.trunk_id = -1;

  if (stage == BROAD_POLICY_STAGE_INGRESS)
  {
    if (ptin_hapi_portDescriptor_get(usp, dapi_g, &portDescriptor, &pbm) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error acquiring interface descriptor!");
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"trunk_id=%d bcm_port=%d class_port=%d", portDescriptor.trunk_id, portDescriptor.bcm_port, portDescriptor.class_port);

    if (portDescriptor.bcm_port>=0)
    {
      #if 0
      printf("%s(%d) value = %08X %08X %08X\r\n",__FUNCTION__,__LINE__,
             pbm.pbits[0],pbm.pbits[1],pbm.pbits[2]);
      printf("%s(%d) mask  = %08X %08X %08X\r\n",__FUNCTION__,__LINE__,
             pbm_mask.pbits[0],pbm_mask.pbits[1],pbm_mask.pbits[2]);
      #endif
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uint8 *)&pbm, (L7_uint8 *)&pbm_mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(INPORTS)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"InPorts qualifier added");
    }
    /* Trunk id field */
    else if (portDescriptor.trunk_id>=0)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SRCTRUNK, (L7_uint8 *)&portDescriptor.trunk_id, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(SRCTRUNK)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"TrunkId qualifier added");
    }

    /* Internal vlans */
    if (profile->outer_vlan_internal>0 && profile->outer_vlan_internal<4096)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uint8 *)&profile->outer_vlan_internal, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OVID)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"SVid qualifier added");
    }
    if (profile->inner_vlan_in>0 && profile->inner_vlan_in<4096)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uint8 *)&profile->inner_vlan_in, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(IVID)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"IVID qualifier added");
    }
  }
  else if (stage == BROAD_POLICY_STAGE_EGRESS)
  {
    if (ptin_hapi_portDescriptor_get(usp, dapi_g, &portDescriptor, &pbm) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error acquiring interface descriptor!");
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"trunk_id=%d bcm_port=%d class_port=%d", portDescriptor.trunk_id, portDescriptor.bcm_port, portDescriptor.class_port);

    /* Physical port */
    if (portDescriptor.bcm_port >= 0)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uint8 *)&portDescriptor.bcm_port, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OUTPORT)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"OutPort qualifier added");
    }
    /* Class port */
    else if (portDescriptor.class_port > 0)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PORTCLASS, (L7_uint8 *)&(portDescriptor.class_port), (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(PORTCLASS)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Port class qualifier added");
    }

    /* External vlans */
    if (profile->outer_vlan_out>0 && profile->outer_vlan_out<4096)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uint8 *)&profile->outer_vlan_out, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OVID)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"SVid qualifier added");
    }
    if (profile->inner_vlan_out>0 && profile->inner_vlan_out<4096)
    {
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uint8 *)&profile->inner_vlan_out, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        hapiBroadPolicyCreateCancel();
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(IVID)");
        return result;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"IVID qualifier added");
    }
  }

  /* MAC address */
  if (profile_macAddr != 0)
  {
    if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, (L7_uint8 *) profile->macAddr, (L7_uint8 *) mask))!=L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(BROAD_FIELD_MACDA)");
      return result;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"BROAD_FIELD_MACDA qualifier added");
  }

  if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uint8 *)&drop, (L7_uint8 *) mask))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(DROP)");
    return result;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Drop qualifier added");

  //hapiBroadPolicyRuleExceedActionAdd (ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  if ((result=hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleNonConfActionAdd");
    return result;
  }
  if ((result=hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleMeterAdd");
    return result;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Meter added");

  if (profile->cos < L7_COS_INTF_QUEUE_MAX_COUNT)
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
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OCOS)");
            return result;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"OCOS (cos(pcp=%u)=%u) qualifier added", i, cos);

          n_pcps++;
      }
  }//if

  if ((result=hapiBroadPolicyCommit(&policyId))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyCommit");
    return result;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policy committed");

  /* AT THIS POINT, THE NEW POLICER IS APPLIED TO HARDWARE */

  policer_ptr->ddUsp_src            = *usp;
  policer_ptr->outer_vlan_in        = profile->outer_vlan_in;
  policer_ptr->outer_vlan_internal  = profile->outer_vlan_internal;
  policer_ptr->outer_vlan_out       = profile->outer_vlan_out;
  policer_ptr->inner_vlan_in        = profile->inner_vlan_in;
  policer_ptr->inner_vlan_out       = profile->inner_vlan_out;
  policer_ptr->cos                  = profile->cos;
  memcpy(policer_ptr->macAddr, profile->macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  policer_ptr->meter                = profile->meter;
  policer_ptr->policy_id            = policyId;
                              
  policer_ptr->inUse                = L7_TRUE;

  /* Search for the following empty entry in database */
  ptin_hapi_policy_find_free(bwp_db);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"... Processing finished successfully!");

  /* Success */
  return L7_SUCCESS;
}

/**
 * Remove a field processor entry
 *  
 * @param usp     
 * @param profile : BW profile 
 * @param dapi_g 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_delete(DAPI_USP_t *usp, ptin_bw_profile_t *profile, DAPI_t *dapi_g)
{
  ptin_bw_policy_t *policer_ptr = L7_NULLPTR;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Going to destroy policer...");

  /* Validate arguments */
  if (profile == L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"No provided profile!");
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Profile contents:");
  LOG_TRACE(LOG_CTX_PTIN_HAPI," ptin_port = %d",profile->ptin_port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in   = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_int  = %u",profile->outer_vlan_internal);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out  = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in   = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out  = %u",profile->inner_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," COS       = %u",profile->cos);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Looking to profile to find a match policer...");

  /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
  policer_ptr = ptin_hapi_policy_find(usp, profile, L7_NULLPTR, bwp_db);

  if (policer_ptr != L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u found!", 
              profile->outer_vlan_in, profile->outer_vlan_internal, profile->inner_vlan_in, profile->outer_vlan_out, profile->inner_vlan_out);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry with OVID_in=%u, OVID_int=%u, IVID_in=%u, OVID_out=%u and IVID_out=%u not found!", 
              profile->outer_vlan_in, profile->outer_vlan_internal, profile->inner_vlan_in, profile->outer_vlan_out, profile->inner_vlan_out);
  }

  /* Validate arguments */
  if (policer_ptr == L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"This policer does not exist");
    return L7_SUCCESS;
  }

  /* Is there need to destroy this policer? */
  if (!policer_ptr->inUse)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"This policer does not exist");
    return L7_SUCCESS;
  }

  /* Destroy policer */
  if (policer_ptr->policy_id > 0)
  {
    if (hapiBroadPolicyDelete(policer_ptr->policy_id)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error destroying policy");
      return L7_FAILURE;
    }
  }

  /* Clear element in database */
  ptin_hapi_policy_clear(policer_ptr, bwp_db);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer destroyed!");

  return L7_SUCCESS;
}

/**
 * Remove all field processor entries
 *  
 * @param usp      
 * @param profile : BW profile 
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_deleteAll(DAPI_USP_t *usp, ptin_bw_profile_t *profile, DAPI_t *dapi_g)
{
  ptin_bw_policy_t *policer = L7_NULLPTR;
  L7_RC_t rc, rc_policer, rc_global=L7_SUCCESS;

  if (usp != L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "ddUsp     = {%d,%d,%d}", usp->unit, usp->slot, usp->port); 
  }
  if (profile != L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Profile contents:");
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ptin_port = %d",profile->ptin_port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in   = %u",profile->outer_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_int  = %u",profile->outer_vlan_internal);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out  = %u",profile->outer_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in   = %u",profile->inner_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out  = %u",profile->inner_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," COS       = %u",profile->cos);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," MAC       = %02x:%02x:%02x:%02x:%02x:%02x",profile->macAddr[0],profile->macAddr[1],profile->macAddr[2],profile->macAddr[3],profile->macAddr[4],profile->macAddr[5]);
  }

  /* Get first policer */
  policer = L7_NULLPTR;

  /* Until there is policers, remove them */
  while ((policer = ptin_hapi_policy_next(policer, bwp_db)) != L7_NULLPTR)
  {
    /* Validate arguments */
    if (!FP_POLICY_VALID_PTR(policer, bwp_db))
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid policer element");
      rc_global = L7_FAILURE;
      continue;
    }

    /* Is there need to destroy this policer? */
    if (policer->inUse)
    {
      /* Filter parameters: only consider (port) source parameters (not destination) */
      /* Only not null values will be considered */

      if (usp != L7_NULLPTR && (IS_SLOT_TYPE_PHYSICAL(usp, dapi_g) || IS_SLOT_TYPE_LOGICAL_LAG(usp, dapi_g)))
      {
        /* USP matches? */
        if (usp->unit!=policer->ddUsp_src.unit || usp->slot!=policer->ddUsp_src.slot || usp->port!=policer->ddUsp_src.port)
        {
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Different port");
          continue;
        }
      }
      /* Check profile */
      if (profile != L7_NULLPTR)
      {
        #if 0
        /* Input vlan matches? */
        if ((profile->outer_vlan_in >= 1 && profile->outer_vlan_in <= 4095) &&
            (profile->outer_vlan_in != policer->outer_vlan_in))
        {
          continue;
        }
        #endif
        /* Output vlan matches? */
        if ((profile->outer_vlan_out >= 1 && profile->outer_vlan_out <= 4095) &&
            (profile->outer_vlan_out != policer->outer_vlan_out))
        {
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Different outer output vlan");
          continue;
        }
        /* Internal vlan matches? */
        if ((profile->outer_vlan_internal >= 1 && profile->outer_vlan_internal <= 4095) &&
            (profile->outer_vlan_internal != policer->outer_vlan_internal))
        {
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Different internal vlan");
          continue;
        }
        /* Inner VLAN matches? */
        if ((profile->inner_vlan_in >= 1 && profile->inner_vlan_in <= 4095) &&
            (profile->inner_vlan_in != policer->inner_vlan_in))
        {
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Different inner vlan");
          continue;
        }
        /* Output vlan matches? */
        if ((profile->macAddr[0]!=0 || profile->macAddr[1]!=0 || profile->macAddr[2]!=0 || profile->macAddr[3]!=0 || profile->macAddr[4]!=0 || profile->macAddr[5]!=0) &&
            (memcmp(profile->macAddr, policer->macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN) != 0))
        {
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Different MAC address");
          continue;
        }
        /* Destination IP address matches? */
        #if 0
        if ((profile->cos != (L7_uchar8)-1) &&
            (profile->cos != policer->cos))
        {
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Different COS");
          continue;
        }
        #endif
      }

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Proceeding to deletion...");

      rc_policer = L7_SUCCESS;

      /* Destroy policer */
      if (policer->policy_id > 0)
      {
        rc = hapiBroadPolicyDelete(policer->policy_id);

        if (rc != L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI,"Error destroying policy (policerId=%u): usp={%d,%d,%d}, OVLAN_in=%u, OVLAN_int=%u, IVLAN_in=%u, OVLAN_out=%u",
                  policer->policy_id,
                  policer->ddUsp_src.unit, policer->ddUsp_src.slot, policer->ddUsp_src.port,
                  policer->outer_vlan_in, policer->outer_vlan_internal, policer->inner_vlan_in, policer->outer_vlan_out);
          rc_policer  = rc;
          rc_global   = rc;
        }
      }

      /* If success, clear element in database */
      if (rc_policer == L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policy destroyed: usp={%d,%d,%d}, OVLAN_in=%u, OVLAN_int=%u, IVLAN_in=%u, OVLAN_out=%u",
                  policer->ddUsp_src.unit, policer->ddUsp_src.slot, policer->ddUsp_src.port,
                  policer->outer_vlan_in, policer->outer_vlan_internal, policer->inner_vlan_in, policer->outer_vlan_out);
        ptin_hapi_policy_clear(policer, bwp_db);
      }
    }
    else
    {
      LOG_WARNING(LOG_CTX_PTIN_HAPI,"This policer does not exist");
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
  ptr->outer_vlan_in        = 0;
  ptr->outer_vlan_internal  = 0;
  ptr->outer_vlan_out       = 0;
  ptr->inner_vlan_in        = 0;
  ptr->inner_vlan_out       = 0;
  ptr->cos                  = -1;
  memset(ptr->macAddr, 0x00, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  ptr->meter.cir = ptr->meter.cbs = 0;
  ptr->meter.eir = ptr->meter.ebs = 0;
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
      (usp->unit != ptr->ddUsp_src.unit ||
       usp->slot != ptr->ddUsp_src.slot ||
       usp->port != ptr->ddUsp_src.port))  return L7_FALSE;

  /* Verify SVID*/
  //if (profile->outer_vlan_in       != ptr->outer_vlan_in)       return L7_FALSE;
  if (profile->outer_vlan_internal != ptr->outer_vlan_internal) return L7_FALSE;
  if (profile->outer_vlan_out      != ptr->outer_vlan_out)      return L7_FALSE;

  /* Verify IVID */
  if (profile->inner_vlan_in !=ptr->inner_vlan_in )  return L7_FALSE;
  if (profile->inner_vlan_out!=ptr->inner_vlan_out)  return L7_FALSE;

  /* COS */
  if (profile->cos !=ptr->cos)  return L7_FALSE;

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
void ptin_bwpolicer_dump_debug(void)
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

    printf("Index %d:\r\n",index);
    printf("  inUse = %u\r\n",ptr->inUse);
    printf("  ddUsp_src= {%d,%d,%d}\r\n",ptr->ddUsp_src.unit,ptr->ddUsp_src.slot,ptr->ddUsp_src.port);
    //printf("  OVID_in  = %u\r\n",ptr->outer_vlan_in);
    printf("  OVID_int = %u\r\n",ptr->outer_vlan_internal);
    printf("  OVID_out = %u\r\n",ptr->outer_vlan_out);
    printf("  IVID_in  = %u\r\n",ptr->inner_vlan_in);
    printf("  IVID_out = %u\r\n",ptr->inner_vlan_out);
    printf("  COS      = %u\r\n",ptr->cos);
    printf("  MAC      = %02x:%02x:%02x:%02x:%02x:%02x\r\n",ptr->macAddr[0],ptr->macAddr[1],ptr->macAddr[2],ptr->macAddr[3],ptr->macAddr[4],ptr->macAddr[5]);
    printf("  meter: cir=%u eir=%u cbs=%u ebs=%u\r\n",ptr->meter.cir,ptr->meter.eir,ptr->meter.cbs,ptr->meter.ebs);
    printf("  policy_id = %u\r\n",ptr->policy_id);
    /* Also print hw group id and entry id*/
    /* PTin modified: SDK 6.3.0 */
    if (l7_bcm_policy_hwInfo_get(0,ptr->policy_id,0,&group_id,&entry_id,&policer_id,&counter_id)==L7_SUCCESS)
    {
      printf("  group=%d, entry=%d, policer_id, counter_id=%d=%d\r\n",group_id,entry_id,policer_id,counter_id);
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

