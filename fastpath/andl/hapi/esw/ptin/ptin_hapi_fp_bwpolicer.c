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
static L7_BOOL bwPolicy_compare(void *profile_ptr, const void *policy_ptr);
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
 * @param profile : BW profile (returned data)
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_get(ptin_bw_profile_t *profile, ptin_bw_policy_t *policer)
{
  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Going to read policer");

  if (profile==L7_NULLPTR || policer==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate policer pointer */
  if (!FP_POLICY_VALID_PTR(policer,bwp_db))
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Not valid policer");
    return L7_FAILURE;
  }

  /* Policer not in use */
  if (!policer->inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Policer not in use");
    return L7_NOT_EXIST;
  }

  /* Copy data */
  profile->ddUsp_src      = policer->ddUsp_src;
  profile->ddUsp_dst      = policer->ddUsp_dst;
  profile->outer_vlan_in  = policer->outer_vlan_in;
  profile->outer_vlan_out = policer->outer_vlan_out;
  profile->inner_vlan_in  = policer->inner_vlan_in;
  profile->inner_vlan_out = policer->inner_vlan_out;
  profile->meter          = policer->meter;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, " ddUsp_src= {%d,%d,%d}",profile->ddUsp_src.unit,profile->ddUsp_src.slot,profile->ddUsp_src.port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " ddUsp_dst= {%d,%d,%d}",profile->ddUsp_dst.unit,profile->ddUsp_dst.slot,profile->ddUsp_dst.port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " OVID_in     = %u",profile->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " OVID_out    = %u",profile->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " IVID_in     = %u",profile->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " IVID_out    = %u",profile->inner_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " {CIR,CBS}= %u",profile->meter.cir,profile->meter.cbs);
  LOG_TRACE(LOG_CTX_PTIN_HAPI, " {EIR,EBS}= %u",profile->meter.eir,profile->meter.ebs);

  return L7_SUCCESS;
}

/**
 * Add a new field processor entry
 * 
 * @param profile : BW profile
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_set(ptin_bw_profile_t *profile, ptin_bw_policy_t **policer, DAPI_t *dapi_g)
{
  ptin_bw_policy_t *policer_ptr;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId  = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  BROAD_POLICY_TYPE_t policyType = BROAD_POLICY_TYPE_PTIN;
  L7_uint8            drop = 0;
  L7_uint8            mask[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_RC_t             result;
  ptin_hapi_intf_t    portDescriptor;
  pbmp_t        pbm, pbm_mask;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Starting processing...");

  /* Check if policer is pointing to a valid database entry, and if it is, use it */
  policer_ptr = L7_NULLPTR;
  if (policer!=L7_NULLPTR && FP_POLICY_VALID_PTR(*policer,bwp_db))
  {
    policer_ptr = *policer;
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer has a valid pointer and will be used");
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer has a null pointer");
  }

  if (policer_ptr!=L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry contents:");
    LOG_TRACE(LOG_CTX_PTIN_HAPI," inUse=%u",policer_ptr->inUse);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_src={%d,%d,%d}",policer_ptr->ddUsp_src.unit,policer_ptr->ddUsp_src.slot,policer_ptr->ddUsp_src.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_dst={%d,%d,%d}",policer_ptr->ddUsp_dst.unit,policer_ptr->ddUsp_dst.slot,policer_ptr->ddUsp_dst.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in  =%u",policer_ptr->outer_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out =%u",policer_ptr->outer_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in  =%u",policer_ptr->inner_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out =%u",policer_ptr->inner_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," meter: cir=%u eir=%u cbs=%u ebs=%u",policer_ptr->meter.cir,policer_ptr->meter.eir,policer_ptr->meter.cbs,policer_ptr->meter.ebs);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," policy_id    =%d",policer_ptr->policy_id);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"No provided database entry!");
  }

  if (profile!=L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Profile contents:");
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_src={%d,%d,%d}",profile->ddUsp_src.unit,profile->ddUsp_src.slot,profile->ddUsp_src.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_dst={%d,%d,%d}",profile->ddUsp_dst.unit,profile->ddUsp_dst.slot,profile->ddUsp_dst.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in  =%u",profile->outer_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out =%u",profile->outer_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in  =%u",profile->inner_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out =%u",profile->inner_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," meter: cir=%u eir=%u cbs=%u ebs=%u",profile->meter.cir,profile->meter.eir,profile->meter.cbs,profile->meter.ebs);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"No provided profile!");
  }

  /* If policer is not provided, try to find in database an entry with matching inputs of profile */
  if (policer_ptr==L7_NULLPTR && profile!=L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer ptr is null: Looking to profile to find a match policer...");
    /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
    policer_ptr = ptin_hapi_policy_find(profile, L7_NULLPTR, bwp_db);
    if (policer_ptr!=L7_NULLPTR)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry with OVID_in=%u and IVID_in=%u found!",profile->outer_vlan_in,profile->inner_vlan_in);
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry with OVID_in=%u and IVID_in=%u not found!",profile->outer_vlan_in,profile->inner_vlan_in);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Validating profile inputs...");

  /* If there is not enough input parameters, remove policer and leave */
  if ( (profile==L7_NULLPTR) ||
       ((profile->ddUsp_src.unit<0 && profile->ddUsp_src.slot<0 && profile->ddUsp_src.port<0) &&
        (profile->outer_vlan_in==0 || profile->outer_vlan_in>=4096) && (profile->inner_vlan_in==0 || profile->inner_vlan_in>=4096)) ||
       (profile->meter.cir>=BW_MAX ) )
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Found conflicting data");
    if (policer_ptr!=L7_NULLPTR)
    {
      hapi_ptin_bwPolicer_delete(policer_ptr);
      policer_ptr = L7_NULLPTR;
      if (policer!=L7_NULLPTR)  *policer = L7_NULLPTR;
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry removed");
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Nothing to do");
    }
    return L7_SUCCESS;
  }

  /* Interfaces mask (for inports field) */
  hapi_ptin_allportsbmp_get(&pbm_mask);

  BCM_PBMP_CLEAR(pbm);
  portDescriptor.lport    = -1;
  portDescriptor.bcm_port = -1;
  portDescriptor.trunk_id = -1;

  if (ptin_hapi_portDescriptor_get(&(profile->ddUsp_src),dapi_g,&portDescriptor,&pbm)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error acquiring interface descriptor!");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"trunk_id=%d bcm_port=%d class_port=%d",portDescriptor.trunk_id,portDescriptor.bcm_port,portDescriptor.class_port);

  /* AT THIS POINT POLICER_PTR HAS A VALID ADDRESS */

  /* If we are using a valid database entry, compare input parameters */
  if (policer_ptr!=L7_NULLPTR && policer_ptr->inUse)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer_ptr is in use: comparing inputs...");
    /* If some input parameter is different, we have to destroy fp policy */
    if ( ( policer_ptr->ddUsp_src.unit !=profile->ddUsp_src.unit ) ||
         ( policer_ptr->ddUsp_src.slot !=profile->ddUsp_src.slot ) ||
         ( policer_ptr->ddUsp_src.port !=profile->ddUsp_src.port ) ||
         ( policer_ptr->outer_vlan_in  != profile->outer_vlan_in ) ||
         /*( policer_ptr->outer_vlan_out != profile->outer_vlan_out) ||*/
         ( policer_ptr->inner_vlan_in  != profile->inner_vlan_in ) ||
         /*( policer_ptr->inner_vlan_out != profile->inner_vlan_out) ||*/
         ( policer_ptr->meter.cir      != profile->meter.cir     ) ||
         ( policer_ptr->meter.eir      != profile->meter.eir     ) ||
         ( policer_ptr->meter.cbs      != profile->meter.cbs     ) ||
         ( policer_ptr->meter.ebs      != profile->meter.ebs     ) )
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Inputs are different... we have to destroy firstly the policer");
      if (hapi_ptin_bwPolicer_delete(policer_ptr)==L7_SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer destroyed");
        policer_ptr = L7_NULLPTR;
        if (policer!=L7_NULLPTR)  *policer = L7_NULLPTR;
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
  if (policer_ptr==L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer_ptr is null: Trying to find a free database entry...");
    if ((policer_ptr=ptin_hapi_policy_find_free(bwp_db))==L7_NULLPTR)
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

  if ((result=hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyPriorityRuleAdd");
    return result;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"New rule added!");

  if (portDescriptor.trunk_id>=0)
  {
    if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SRCTRUNK, (L7_uint8 *)&portDescriptor.trunk_id, (L7_uint8 *) mask))!=L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(SRCTRUNK)");
      return result;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"TrunkId qualifier added");
  }
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
  if (profile->outer_vlan_in>0 && profile->outer_vlan_in<4096)
  {
    if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uint8 *)&profile->outer_vlan_in, (L7_uint8 *) mask))!=L7_SUCCESS)
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

  if ((result=hapiBroadPolicyCommit(&policyId))!=L7_SUCCESS)
  {
    hapiBroadPolicyCreateCancel();
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyCommit");
    return result;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policy committed");

  /* AT THIS POINT, THE NEW POLICER IS APPLIED TO HARDWARE */

  policer_ptr->ddUsp_src      = profile->ddUsp_src;
  policer_ptr->ddUsp_dst      = profile->ddUsp_dst;
  policer_ptr->outer_vlan_in  = profile->outer_vlan_in;
  policer_ptr->outer_vlan_out = profile->outer_vlan_out;
  policer_ptr->inner_vlan_in  = profile->inner_vlan_in;
  policer_ptr->inner_vlan_out = profile->inner_vlan_out;
  policer_ptr->meter          = profile->meter;
  policer_ptr->policy_id      = policyId;
                              
  policer_ptr->inUse          = L7_TRUE;

  /* Search for the following empty entry in database */
  ptin_hapi_policy_find_free(bwp_db);

  /* Update policer pointer */
  if (policer!=L7_NULLPTR)  *policer = policer_ptr;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"... Processing finished successfully!");

  /* Success */
  return L7_SUCCESS;
}

/**
 * Remove a field processor entry
 * 
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_delete(ptin_bw_policy_t *policer)
{
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Going to destroy policer...");

  /* Validate arguments */
  if (policer==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"This policer does not exist");
    return L7_SUCCESS;
  }
  if (!FP_POLICY_VALID_PTR(policer,bwp_db))
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid policer element");
    return L7_FAILURE;
  }

  /* Is there need to destroy this policer? */
  if (!policer->inUse)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"This policer does not exist");
    return L7_SUCCESS;
  }

  /* Destroy policer */
  if (policer->policy_id>0)
  {
    if (hapiBroadPolicyDelete(policer->policy_id)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error destroying policy");
      return L7_FAILURE;
    }
  }

  /* Clear element in database */
  ptin_hapi_policy_clear(policer,bwp_db);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer destroyed!");

  return L7_SUCCESS;
}

/**
 * Remove all field processor entries
 * 
 * @param policer : BW policer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bwPolicer_deleteAll(void)
{
  ptin_bw_policy_t *policer;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  /* Get first policer */
  policer = ptin_hapi_policy_next(L7_NULLPTR,bwp_db);
  
  /* Until there is policers, remove them */
  while (policer!=L7_NULLPTR)
  {
    /* Remove policer */
    if ((rc=hapi_ptin_bwPolicer_delete(policer))!=L7_SUCCESS)
    {
      rc_global = rc;
    }

    /* Get next policer */
    policer = ptin_hapi_policy_next(policer,bwp_db);
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

  ptr->inUse          = L7_FALSE;
  ptr->policy_id      = 0;
  ptr->ddUsp_src.unit = ptr->ddUsp_src.slot = ptr->ddUsp_src.port = -1;
  ptr->ddUsp_dst.unit = ptr->ddUsp_dst.slot = ptr->ddUsp_dst.port = -1;
  ptr->outer_vlan_in  = 0;
  ptr->outer_vlan_out = 0;
  ptr->inner_vlan_in  = 0;
  ptr->inner_vlan_out = 0;
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
static L7_BOOL bwPolicy_compare(void *profile_ptr, const void *policy_ptr)
{
  ptin_bw_profile_t *profile = (ptin_bw_profile_t *) profile_ptr;
  const ptin_bw_policy_t *ptr = (const ptin_bw_policy_t *) policy_ptr;

  /* Skip empty elements */
  if (!ptr->inUse)  return L7_FALSE;

  /* Verify src interface */
  if (profile->ddUsp_src.unit!=ptr->ddUsp_src.unit ||
      profile->ddUsp_src.slot!=ptr->ddUsp_src.slot ||
      profile->ddUsp_src.port!=ptr->ddUsp_src.port)  return L7_FALSE;

  /* Verify SVID*/
  if (profile->outer_vlan_in !=ptr->outer_vlan_in )  return L7_FALSE;
  //if (profile->outer_vlan_out!=ptr->outer_vlan_out)  return L7_FALSE;

  /* Verify IVID */
  if (profile->inner_vlan_in !=ptr->inner_vlan_in )  return L7_FALSE;
  //if (profile->inner_vlan_out!=ptr->inner_vlan_out)  return L7_FALSE;

  return L7_TRUE;
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
    printf("  ddUsp_dst= {%d,%d,%d}\r\n",ptr->ddUsp_dst.unit,ptr->ddUsp_dst.slot,ptr->ddUsp_dst.port);
    printf("  OVID_in  = %u\r\n",ptr->outer_vlan_in);
    printf("  OVID_out = %u\r\n",ptr->outer_vlan_out);
    printf("  IVID_in  = %u\r\n",ptr->inner_vlan_in);
    printf("  IVID_out = %u\r\n",ptr->inner_vlan_out);
    printf("  meter: cir=%u eir=%u cbs=%u ebs=%u\r\n",ptr->meter.cir,ptr->meter.eir,ptr->meter.cbs,ptr->meter.ebs);
    printf("  policy_id = %u\r\n",ptr->policy_id);
    /* Also print hw group id and entry id*/
    /* PTin modified: SDK 6.3.0 */
    if (l7_bcm_policy_hwInfo_get(0,ptr->policy_id,0,&group_id,&entry_id,&policer_id,&counter_id)==L7_SUCCESS)
    {
      printf("  group=%d, entry=%d, policer_id, counter_id=%d=%d\r\n",group_id,entry_id,policer_id,counter_id);
    }
  }
}

