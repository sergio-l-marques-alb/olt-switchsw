#include <bcm/error.h>

#include "ptin_hapi_fp_counters.h"
#include "ptin_hapi_fp_utils.h"
#include "broad_policy.h"
#include "broad_group_bcm.h"

#include "logger.h"

#if (PTIN_POLICY_STAGE_COUNT<BROAD_POLICY_STAGE_COUNT)
  #error "PTIN_POLICY_STAGE_COUNT must be >= BROAD_POLICY_STAGE_COUNT"
#endif

/******************************************** 
 * INTERNAL VARS
 ********************************************/

/* Array of counter structures */
ptin_evcStats_policy_t fp_counters_data[PTIN_SYSTEM_MAX_COUNTERS];

/* Database to be managed by ptin_hapi_fp_utils module */
ptin_hapi_database_t fp_counters_database;
ptin_hapi_database_t *cnt_db = &fp_counters_database;

/******************************************** 
 * STATIC FUNCTIONS FOR INTERNAL USAGE
 ********************************************/

static void fpCounters_clear_data(void *policy_ptr);
static L7_BOOL fpCounters_compare(void *profile_ptr, const void *policy_ptr);
static L7_BOOL fpCounters_check_conflicts(void *profile_ptr, const void *policy_ptr, int stage);
static L7_BOOL fpCounters_inUse(void *policy_ptr);


/******************************************** 
 * FUNCTIONS FOR EXTERNAL USAGE
 ********************************************/

/**
 * Initialize all database
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t hapi_ptin_fpCounters_init(void)
{
  ptin_evcStats_policy_t *counter;

  cnt_db->database_base              = (void *) fp_counters_data;
  cnt_db->database_num_elems         = PTIN_SYSTEM_MAX_COUNTERS;
  cnt_db->database_elem_sizeof       = sizeof(ptin_evcStats_policy_t);
  cnt_db->database_index_first_free  = 0;
  cnt_db->policy_inUse               = fpCounters_inUse;
  cnt_db->policy_compare             = fpCounters_compare;
  cnt_db->policy_check_conflicts     = fpCounters_check_conflicts;
  cnt_db->policy_clear_data          = fpCounters_clear_data;

  for (counter=fp_counters_data; FP_POLICY_VALID_PTR(counter,cnt_db); counter++)
  {
    fpCounters_clear_data(counter);
  }

  return L7_SUCCESS;
}

/**
 * Read the list of counters associated to a policy
 * 
 * @param stats : Statistics data
 * @param counter : Pointer to Counter policy
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_fpCounters_get(ptin_evcStats_counters_t *stats, ptin_evcStats_policy_t *counter)
{
  L7_int packet_type;
  BROAD_POLICY_STATS_t stat[PTIN_PACKETS_TYPE_MAX];
  L7_RC_t rc;
  L7_int  stage;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "Going to read counter");

  if (stats==L7_NULLPTR || counter==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Validate counter pointer */
  if (!FP_POLICY_VALID_PTR(counter,cnt_db))
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Not valid counter");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry contents:");
  LOG_TRACE(LOG_CTX_PTIN_HAPI," inUse     = %u",         counter->inUse);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_src = {%d,%d,%d}", counter->ddUsp_src.unit,counter->ddUsp_src.slot,counter->ddUsp_src.port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_dst = {%d,%d,%d}", counter->ddUsp_dst.unit,counter->ddUsp_dst.slot,counter->ddUsp_dst.port);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in   = %u",         counter->outer_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out  = %u",         counter->outer_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in   = %u",         counter->inner_vlan_in);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out  = %u",         counter->inner_vlan_out);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," DID       = %u",         counter->dip);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," policy_id = {%d,%d,%d}", counter->policy_id[0],counter->policy_id[1],counter->policy_id[2]);
  LOG_TRACE(LOG_CTX_PTIN_HAPI," rule_id   = {[%d,%d,%d,%d];[%d,%d,%d,%d];[%d,%d,%d,%d]}",
            counter->rule_id[0][0],counter->rule_id[0][1],counter->rule_id[0][2],counter->rule_id[0][3],
            counter->rule_id[1][0],counter->rule_id[1][1],counter->rule_id[1][2],counter->rule_id[1][3],
            counter->rule_id[2][0],counter->rule_id[2][1],counter->rule_id[2][2],counter->rule_id[2][3]);

  /* Counter not in use */
  if (!counter->inUse)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Counter not in use");
    return L7_NOT_EXIST;
  }

  /* Clear returned stats */
  memset(stats,0x00,sizeof(ptin_evcStats_counters_t));

  for (stage=BROAD_POLICY_STAGE_INGRESS; stage<=BROAD_POLICY_STAGE_EGRESS; stage++)
  {
    /* Validate policy id */
    if (counter->policy_id[stage]<=0)
    {
      LOG_WARNING(LOG_CTX_PTIN_HAPI,"Policy not created for this counter (stage=%d)",stage);
      continue;
    }

    /* Extract number of packets for each type */
    for (packet_type=PTIN_PACKETS_TYPE_FIRST; packet_type<PTIN_PACKETS_TYPE_MAX; packet_type++)
    {
      if ((rc=hapiBroadPolicyStatsGet(counter->policy_id[stage],counter->rule_id[stage][packet_type],&stat[packet_type]))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error reading counters (type=%d)",packet_type);
        return L7_FAILURE;
      }
    }

    if (stage==BROAD_POLICY_STAGE_INGRESS)
    {
      /* Copy extracted data to output structure */
      stats->rx.pktUnicast   = (L7_uint32) stat[PTIN_PACKETS_TYPE_UNICAST  ].statMode.counter.count;
      stats->rx.pktBroadcast = (L7_uint32) stat[PTIN_PACKETS_TYPE_BROADCAST].statMode.counter.count;
      stats->rx.pktMulticast = (L7_uint32) stat[PTIN_PACKETS_TYPE_MULTICAST].statMode.counter.count;
      stats->rx.pktDropped   = (L7_uint32) stat[PTIN_PACKETS_TYPE_DROPPED  ].statMode.counter.count;
      stats->rx.pktTotal     = (L7_uint32) stats->rx.pktUnicast + stats->rx.pktBroadcast + stats->rx.pktMulticast;
      stats->mask |= PTIN_EVCSTATS_COUNTERS_MASK_RX;
    }
    else if (stage==BROAD_POLICY_STAGE_EGRESS)
    {
      /* Copy extracted data to output structure */
      stats->tx.pktUnicast   = (L7_uint32) stat[PTIN_PACKETS_TYPE_UNICAST  ].statMode.counter.count;
      stats->tx.pktBroadcast = (L7_uint32) stat[PTIN_PACKETS_TYPE_BROADCAST].statMode.counter.count;
      stats->tx.pktMulticast = (L7_uint32) stat[PTIN_PACKETS_TYPE_MULTICAST].statMode.counter.count;
      stats->tx.pktDropped   = (L7_uint32) stat[PTIN_PACKETS_TYPE_DROPPED  ].statMode.counter.count;
      stats->tx.pktTotal     = (L7_uint32) stats->tx.pktUnicast + stats->tx.pktBroadcast + stats->tx.pktMulticast;
      stats->mask |= PTIN_EVCSTATS_COUNTERS_MASK_TX;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI, " Stage %s",((stage==BROAD_POLICY_STAGE_INGRESS) ? "INGRESS" : "EGRESS"));
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "  Unicast   = %u", (L7_uint32) stat[PTIN_PACKETS_TYPE_UNICAST  ].statMode.counter.count);
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "  Multicast = %u", (L7_uint32) stat[PTIN_PACKETS_TYPE_MULTICAST].statMode.counter.count);
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "  Broadcast = %u", (L7_uint32) stat[PTIN_PACKETS_TYPE_BROADCAST].statMode.counter.count);
    LOG_TRACE(LOG_CTX_PTIN_HAPI, "  Dropped   = %u", (L7_uint32) stat[PTIN_PACKETS_TYPE_DROPPED  ].statMode.counter.count);
  }

  return L7_SUCCESS;
}

/**
 * Add a new counters policy
 * 
 * @param profile : Policy reference data
 * @param counter : Pointer to Counter policy
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_fpCounters_set(ptin_evcStats_profile_t *profile, ptin_evcStats_policy_t **counter, DAPI_t *dapi_g)
{
  ptin_evcStats_policy_t *counter_ptr;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId  = BROAD_POLICY_RULE_INVALID;
  BROAD_POLICY_TYPE_t policyType = BROAD_POLICY_TYPE_SYSTEM;
  L7_uint8            drop_qualifier = 0;
  bcm_mac_t           dmac     = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  L7_uint8            mask[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_RC_t             result;
  ptin_hapi_intf_t    portDescriptor;
  pbmp_t              pbm, pbm_mask;
  L7_uint8            packets_type;
  BROAD_POLICY_STAGE_t  stage;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Starting processing...");

  /* Check if counter is pointing to a valid database entry, and if it is, use it */
  counter_ptr = L7_NULLPTR;
  if (counter!=L7_NULLPTR && FP_POLICY_VALID_PTR(*counter,cnt_db))
  {
    counter_ptr = *counter;
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Counter has a valid pointer and will be used");
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Counter has a null pointer");
  }

  if (counter_ptr!=L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry contents:");
    LOG_TRACE(LOG_CTX_PTIN_HAPI," inUse     = %u",         counter_ptr->inUse);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_src = {%d,%d,%d}", counter_ptr->ddUsp_src.unit,counter_ptr->ddUsp_src.slot,counter_ptr->ddUsp_src.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_dst = {%d,%d,%d}", counter_ptr->ddUsp_dst.unit,counter_ptr->ddUsp_dst.slot,counter_ptr->ddUsp_dst.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in   = %u",         counter_ptr->outer_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out  = %u",         counter_ptr->outer_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in   = %u",         counter_ptr->inner_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out  = %u",         counter_ptr->inner_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," DIP       = %u",         counter_ptr->dip);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," policy_id = {%d,%d,%d}", counter_ptr->policy_id[0],counter_ptr->policy_id[1],counter_ptr->policy_id[2]);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," rule_id   = {[%d,%d,%d,%d];[%d,%d,%d,%d];[%d,%d,%d,%d]}",
              counter_ptr->rule_id[0][0],counter_ptr->rule_id[0][1],counter_ptr->rule_id[0][2],counter_ptr->rule_id[0][3],
              counter_ptr->rule_id[1][0],counter_ptr->rule_id[1][1],counter_ptr->rule_id[1][2],counter_ptr->rule_id[1][3],
              counter_ptr->rule_id[2][0],counter_ptr->rule_id[2][1],counter_ptr->rule_id[2][2],counter_ptr->rule_id[2][3]);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"No provided database entry!");
  }

  if (profile!=L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Profile contents:");
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_src = {%d,%d,%d}",profile->ddUsp_src.unit,profile->ddUsp_src.slot,profile->ddUsp_src.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," ddUsp_dst = {%d,%d,%d}",profile->ddUsp_dst.unit,profile->ddUsp_dst.slot,profile->ddUsp_dst.port);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_in   = %u",profile->outer_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," OVID_out  = %u",profile->outer_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_in   = %u",profile->inner_vlan_in);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," IVID_out  = %u",profile->inner_vlan_out);
    LOG_TRACE(LOG_CTX_PTIN_HAPI," DIP       = %u",profile->dst_ip);
  }
  else
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"No provided profile!");
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Validating profile inputs...");

  /* If there is not enough input parameters, remove counter and leave */
  if ( (profile==L7_NULLPTR) ||
       ((profile->ddUsp_src.unit<0 && profile->ddUsp_src.slot<0 && profile->ddUsp_src.port<0) &&
        (profile->outer_vlan_in==0 || profile->outer_vlan_in>=4096) && (profile->inner_vlan_in==0 || profile->inner_vlan_in>=4096)) )
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Found conflicting data");
    if (counter_ptr!=L7_NULLPTR)
    {
      hapi_ptin_fpCounters_delete(counter_ptr);
      counter_ptr = L7_NULLPTR;
      if (counter!=L7_NULLPTR)  *counter = L7_NULLPTR;
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry removed");
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Nothing to do");
    }
    return L7_SUCCESS;
  }

  /* AT THIS POINT PROFILE IS A VALID POINTER WITH A VALID CIR */

  /* If counter is not provided, try to find in database an entry with matching inputs of profile */
  if (counter_ptr==L7_NULLPTR && profile!=L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Counter ptr is null: Looking to profile to find a match counter...");
    /* Search in database for an entry with the same profile inputs (Source interface, SVLAN and CVLAN) */
    counter_ptr = ptin_hapi_policy_find(profile, L7_NULLPTR, cnt_db);
    if (counter_ptr!=L7_NULLPTR)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry found!");
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Database entry not found!");
    }
  }

  /* If we are using a valid database entry, compare input parameters */
  if (counter_ptr!=L7_NULLPTR && counter_ptr->inUse)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer_ptr is in use: comparing inputs...");
    /* If some input parameter is different, we have to destroy fp policy */
    if ( ( counter_ptr->ddUsp_src.unit != profile->ddUsp_src.unit ) ||
         ( counter_ptr->ddUsp_src.slot != profile->ddUsp_src.slot ) ||
         ( counter_ptr->ddUsp_src.port != profile->ddUsp_src.port ) ||
         ( counter_ptr->outer_vlan_in  != profile->outer_vlan_in  ) ||
         ( counter_ptr->outer_vlan_out != profile->outer_vlan_out ) ||
         ( counter_ptr->inner_vlan_in  != profile->inner_vlan_in  ) ||
         ( counter_ptr->inner_vlan_out != profile->inner_vlan_out ) ||
         ( counter_ptr->dip            != profile->dst_ip         ) )
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Inputs are different... we have to destroy firstly the counter");
      if (hapi_ptin_fpCounters_delete(counter_ptr)==L7_SUCCESS)
      {
        counter_ptr = L7_NULLPTR;
        if (counter!=L7_NULLPTR)  *counter = L7_NULLPTR;
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Counter destroyed");
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error destroying counter");
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

  /* If counter was not provided, find a new one */
  /* If not found, we have an error */
  if (counter_ptr==L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policer_ptr is null: Trying to find a free database entry...");
    if ((counter_ptr=ptin_hapi_policy_find_free(cnt_db))==L7_NULLPTR)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Free database entry not found... error!");
      return L7_TABLE_IS_FULL;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Free entry found!");
  }

  /* AT THIS POINT POLICER_PTR HAS A VALID ADDRESS */

  /* Interfaces mask (for inports field) */
  hapi_ptin_allportsbmp_get(&pbm_mask);

  BCM_PBMP_CLEAR(pbm);
  portDescriptor.lport      = -1;
  portDescriptor.bcm_port   = -1;
  portDescriptor.trunk_id   = -1;
  portDescriptor.class_port =  0;

  if (ptin_hapi_portDescriptor_get(&(profile->ddUsp_src),dapi_g,&portDescriptor,&pbm)!=L7_SUCCESS ||
      (portDescriptor.bcm_port<0 && portDescriptor.trunk_id<0 && portDescriptor.class_port==0))
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Error acquiring interface descriptor!");
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Configuring counter...");

  /* AT THIS POINT ENTRY IN DATABASE MUST BE CONFIGURED IN HARDWARE (inUse==L7_FALSE) */

  #if 1
  for ( stage=BROAD_POLICY_STAGE_INGRESS; stage<=BROAD_POLICY_STAGE_EGRESS; stage++)
  #else
  stage = BROAD_POLICY_STAGE_INGRESS;
  #endif
  {
    /* Check if policy need to be created.
       At least the outer vlan must be defined. */
    if (stage==BROAD_POLICY_STAGE_INGRESS)
    {
      if (profile->outer_vlan_in<=0 || profile->outer_vlan_in>=4096)
        continue;

      /* Define policy type */
      if (profile->inner_vlan_in<=0 || profile->inner_vlan_in>=4096)
        policyType = BROAD_POLICY_TYPE_STAT_EVC;
      else
        policyType = BROAD_POLICY_TYPE_STAT_CLIENT;
    }
    else if (stage==BROAD_POLICY_STAGE_EGRESS)
    {
      if (profile->outer_vlan_out<=0 || profile->outer_vlan_out>=4096)
        continue;

      /* Check for conflicts */
      if (ptin_hapi_policy_check_conflicts(profile, L7_NULLPTR, cnt_db, BROAD_POLICY_STAGE_EGRESS) != L7_NULLPTR)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Counter already configured in conflict (at egress stage)");
        continue;
      }

      /* Define policy type */
      if (profile->inner_vlan_out<=0 || profile->inner_vlan_out>=4096)
        policyType = BROAD_POLICY_TYPE_STAT_EVC;
      else
        policyType = BROAD_POLICY_TYPE_STAT_CLIENT;
    }
    else
    {
      continue;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI,"policyType=%u",policyType);

    if ((result=hapiBroadPolicyCreate(policyType))!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error creating new policy");
      return result;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"New policy created!");

    /* Set FP stage */
    if ((result=hapiBroadPolicyStageSet(stage))!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error setting stage %u",stage);
      return result;
    }

    result = L7_SUCCESS;

    /* Add 1 rule for each packet type (4) */
    for (packets_type=PTIN_PACKETS_TYPE_FIRST; packets_type<PTIN_PACKETS_TYPE_MAX; packets_type++)
    {
      /* Creatre rule */
      if ((result=hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyPriorityRuleAdd");
        break;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"New rule added!");

      /* Initialize mask */
      memset(mask,0xff,sizeof(mask));

      if (stage==BROAD_POLICY_STAGE_INGRESS)
      {
        if (portDescriptor.bcm_port>=0)
        {
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"value = %08x %08x",pbm.pbits[0],pbm.pbits[1]);
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"mask  = %08x %08x",pbm_mask.pbits[0],pbm_mask.pbits[1]);

          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uint8 *)&pbm, (L7_uint8 *)&pbm_mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(INPORTS)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"InPorts qualifier added");
        }
        /* Trunk id field */
        else if (portDescriptor.trunk_id>=0)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SRCTRUNK, (L7_uint8 *)&(portDescriptor.trunk_id), (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(SRCTRUNK)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Source TrunkId qualifier added");
        }
      }
      else if (stage==BROAD_POLICY_STAGE_EGRESS)
      {
        /* Physical port */
        if (portDescriptor.bcm_port>=0)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uint8 *)&portDescriptor.bcm_port, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OUTPORT)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"OutPort qualifier added");
        }
        /* Class port */
        else if (portDescriptor.class_port>0)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PORTCLASS, (L7_uint8 *)&(portDescriptor.class_port), (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(PORTCLASS)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"Port class qualifier added");
        }
      }

      /* SVLAN */
      if (stage==BROAD_POLICY_STAGE_INGRESS)
      {
        if (profile->outer_vlan_in>0 && profile->outer_vlan_in<4096)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uint8 *)&profile->outer_vlan_in, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OVID_in)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"OVID_in qualifier added");
        }
      }
      else if (stage==BROAD_POLICY_STAGE_EGRESS)
      {
        if (profile->outer_vlan_out>0 && profile->outer_vlan_out<4096)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uint8 *)&profile->outer_vlan_out, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(OVID_out)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"OVID_out qualifier added");
        }
      }

      /* CVLAN */
      if (stage==BROAD_POLICY_STAGE_INGRESS)
      {
        if (profile->inner_vlan_in>0 && profile->inner_vlan_in<4096)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uint8 *)&profile->inner_vlan_in, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(IVID_in)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"CVid_in qualifier added");
        }
      }
      else if (stage==BROAD_POLICY_STAGE_EGRESS)
      {
        if (profile->inner_vlan_out>0 && profile->inner_vlan_out<4096)
        {
          if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uint8 *)&profile->inner_vlan_out, (L7_uint8 *) mask))!=L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(IVID_out)");
            break;
          }
          LOG_TRACE(LOG_CTX_PTIN_HAPI,"CVid_out qualifier added");
        }
      }

      /* DIP */
      if (profile->dst_ip!=0 && profile->dst_ip!=(L7_uint32)-1)
      {
        if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DIP, (L7_uint8 *)&profile->dst_ip, (L7_uint8 *) mask))!=L7_SUCCESS)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(IVID)");
          break;
        }
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"CVid qualifier added");
      }

      drop_qualifier = 0x00;

      switch (packets_type)
      {
      case PTIN_PACKETS_TYPE_UNICAST:
        // Unicast DMAC
        memset(dmac,0x00,sizeof(bcm_mac_t));
        memset(mask,0x00,sizeof(bcm_mac_t));
        dmac[0]=0x00;
        mask[0]=0x01;
        result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, (L7_uint8 *)&dmac, (L7_uint8 *) mask);
        break;

      case PTIN_PACKETS_TYPE_BROADCAST:
        // Broadcast DMAC
        memset(dmac,0xff,sizeof(bcm_mac_t));
        memset(mask,0xff,sizeof(bcm_mac_t));
        result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, (L7_uint8 *)&dmac, (L7_uint8 *) mask);
        break;

      case PTIN_PACKETS_TYPE_MULTICAST:
        // Broadcast DMAC
        memset(dmac,0x00,sizeof(bcm_mac_t));
        memset(mask,0x00,sizeof(bcm_mac_t));
        dmac[0]=0x01;
        mask[0]=0x01;
        result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, (L7_uint8 *)&dmac, (L7_uint8 *) mask);
        break;

      case PTIN_PACKETS_TYPE_DROPPED:
        drop_qualifier = 0x01;
        break;
      }

      /* Evaluate if MACDA rule was successfully added */
      if (result!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(MACDA)... packet_type=%u",packets_type);
        break;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"MACDA %u qualifier added",packets_type);

      /* Drop Qualifier */
      mask[0]=0x01;
      if ((result=hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uint8 *)&drop_qualifier, (L7_uint8 *) mask))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleQualifierAdd(DROP)");
        break;
      }

      /* Add counter */
      if (hapiBroadPolicyRuleCounterAdd(ruleId,BROAD_COUNT_PACKETS)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyRuleCounterAdd");
        break;
      }

      /* Save ruleId */
      counter_ptr->rule_id[stage][packets_type] = ruleId;
    }

    /* Evaluate if any error ocurred */
    if (packets_type<PTIN_PACKETS_TYPE_MAX || result!=L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      LOG_ERR(LOG_CTX_PTIN_HAPI,"An error have ocurred");
      return result;
    }

    /* Commit policer with all 4 rules */
    if ((result=hapiBroadPolicyCommit(&policyId))!=L7_SUCCESS)
    {
      hapiBroadPolicyCreateCancel();
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with hapiBroadPolicyCommit (stage=%u)",stage);
      /* Only return error, for ingress stage */
      if (stage==BROAD_POLICY_STAGE_INGRESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Returning error %u (stage=%u)",result,stage);
        return result;
      }
      /* For others, do nothing */
      else
      {
        policyId = 0;
        LOG_WARNING(LOG_CTX_PTIN_HAPI,"Ignoring error %u (stage=%u)",result,stage);
      }
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policy committed (stage=%u)",stage);
    }

    /* Save policy id */
    counter_ptr->policy_id[stage] = policyId;
  }

  /* AT THIS POINT, THE NEW COUNTER IS APPLIED TO HARDWARE */

  counter_ptr->ddUsp_src      = profile->ddUsp_src;
  counter_ptr->ddUsp_dst      = profile->ddUsp_dst;
  counter_ptr->outer_vlan_in  = profile->outer_vlan_in;
  counter_ptr->outer_vlan_out = profile->outer_vlan_out;
  counter_ptr->inner_vlan_in  = profile->inner_vlan_in;
  counter_ptr->inner_vlan_out = profile->inner_vlan_out;
  counter_ptr->dip            = profile->dst_ip;
  counter_ptr->inUse          = L7_TRUE;

  /* Search for the following empty entry in database */
  ptin_hapi_policy_find_free(cnt_db);

  /* Update counter pointer */
  if (counter!=L7_NULLPTR)  *counter = counter_ptr;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"... Processing finished successfully!");

  /* Success */
  return L7_SUCCESS;
}

/**
 * Remove a counters policy
 *
 * @param counter : Pointer to Counter policy
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_fpCounters_delete(ptin_evcStats_policy_t *counter)
{
  L7_int stage;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Going to destroy counter...");

  /* Validate arguments */
  if (counter==L7_NULLPTR)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"This counter does not exist");
    return L7_SUCCESS;
  }
  if (!FP_POLICY_VALID_PTR(counter,cnt_db))
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI,"Invalid counter element");
    return L7_FAILURE;
  }

  /* Is there need to destroy this counter? */
  if (!counter->inUse)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI,"This counter does not exist");
    return L7_SUCCESS;
  }

  for (stage=BROAD_POLICY_STAGE_INGRESS; stage<=BROAD_POLICY_STAGE_EGRESS; stage++)
  {
    /* Destroy counter */
    if (counter->policy_id[stage]>0)
    {
      if (hapiBroadPolicyDelete(counter->policy_id[stage])!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI,"Error destroying policy (counterId=%u)",counter->policy_id[stage]);
        return L7_FAILURE;
      }
    }
  }

  /* Clear element in database */
  ptin_hapi_policy_clear(counter,cnt_db);

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Counter destroyed!");

  return L7_SUCCESS;
}

/**
 * Remove all counters policies
 *
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_fpCounters_deleteAll(void)
{
  ptin_evcStats_policy_t *counter;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  /* Get first counter */
  counter = ptin_hapi_policy_next(L7_NULLPTR,cnt_db);

  /* Until there is policers, remove them */
  while (counter!=L7_NULLPTR)
  {
    /* Remove counter */
    if ((rc=hapi_ptin_fpCounters_delete(counter))!=L7_SUCCESS)
    {
      rc_global = rc;
    }

    /* Get next counter */
    counter = ptin_hapi_policy_next(counter,cnt_db);
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
static L7_BOOL fpCounters_inUse(void *policy_ptr)
{
  ptin_evcStats_policy_t *ptr = (ptin_evcStats_policy_t *) policy_ptr;

  return ptr->inUse;
}

/**
 * Function used to clear database element
 * 
 * @param policy_ptr : pointer to database element
 */
static void fpCounters_clear_data(void *policy_ptr)
{
  L7_int i, stage;
  ptin_evcStats_policy_t *ptr = (ptin_evcStats_policy_t *) policy_ptr;

  ptr->inUse = L7_FALSE;
  ptr->ddUsp_src.unit = ptr->ddUsp_src.slot = ptr->ddUsp_src.port = -1;
  ptr->ddUsp_dst.unit = ptr->ddUsp_dst.slot = ptr->ddUsp_dst.port = -1;
  ptr->outer_vlan_in  = 0;
  ptr->outer_vlan_out = 0;
  ptr->inner_vlan_in  = 0;
  ptr->inner_vlan_out = 0;
  ptr->dip            = 0;

  for (stage=0; stage<PTIN_POLICY_STAGE_COUNT; stage++)
  {
    ptr->policy_id[stage] = 0;
    for (i=PTIN_PACKETS_TYPE_FIRST; i<PTIN_PACKETS_TYPE_MAX; i++)  ptr->rule_id[stage][i] = 0;
  }
}

/**
 * Function used for database comparison
 * 
 * @param profile_ptr : Profile data
 * @param policy_ptr : Pointer to database
 * 
 * @return L7_BOOL : L7_TRUE if equal / L7_FALSE if not
 */
static L7_BOOL fpCounters_compare(void *profile_ptr, const void *policy_ptr)
{
  ptin_evcStats_profile_t *profile = (ptin_evcStats_profile_t *) profile_ptr;
  const ptin_evcStats_policy_t *ptr = (const ptin_evcStats_policy_t *) policy_ptr;

  /* Skip empty elements */
  if (!ptr->inUse)  return L7_FALSE;

  /* Verify interface */
  if (profile->ddUsp_src.unit!=ptr->ddUsp_src.unit ||
      profile->ddUsp_src.slot!=ptr->ddUsp_src.slot ||
      profile->ddUsp_src.port!=ptr->ddUsp_src.port)  return L7_FALSE;

  /* Verify OVID */
  if (profile->outer_vlan_in !=ptr->outer_vlan_in )  return L7_FALSE;
  if (profile->outer_vlan_out!=ptr->outer_vlan_out)  return L7_FALSE;

  /* Verify CVID */
  if (profile->inner_vlan_in !=ptr->inner_vlan_in )  return L7_FALSE;
  if (profile->inner_vlan_out!=ptr->inner_vlan_out)  return L7_FALSE;

  /* Verify DIP */
  if (profile->dst_ip!=ptr->dip)  return L7_FALSE;

  return L7_TRUE;
}

/**
 * Function used for conflicts detection
 * 
 * @param profile_ptr : Profile data
 * @param policy_ptr : Pointer to database 
 * @param state: ingress or egress 
 * 
 * @return L7_BOOL : L7_TRUE if confictuous / L7_FALSE if not
 */
static L7_BOOL fpCounters_check_conflicts(void *profile_ptr, const void *policy_ptr, int stage)
{
  ptin_evcStats_profile_t *profile = (ptin_evcStats_profile_t *) profile_ptr;
  const ptin_evcStats_policy_t *ptr = (const ptin_evcStats_policy_t *) policy_ptr;

  /* Skip empty elements */
  if (!ptr->inUse)  return L7_FALSE;

  /* Verify interface */
  if (profile->ddUsp_src.unit == ptr->ddUsp_src.unit &&
      profile->ddUsp_src.slot == ptr->ddUsp_src.slot &&
      profile->ddUsp_src.port == ptr->ddUsp_src.port)
  {
    /* Verify OVID */
    if (stage == BROAD_POLICY_STAGE_EGRESS)
    {
      if (profile->outer_vlan_out == ptr->outer_vlan_out)
      {
        if ((profile->inner_vlan_out == ptr->inner_vlan_out) ||             /* Vlans are the same */
            (profile->inner_vlan_out == 0 && ptr->inner_vlan_out != 0) ||   /* one is EVC counter, and the other is client counter */
            (profile->inner_vlan_out != 0 && ptr->inner_vlan_out == 0))
          return L7_TRUE;
      }
    }
    else if (stage == BROAD_POLICY_STAGE_INGRESS)
    {
      if (profile->outer_vlan_in == ptr->outer_vlan_in)
      {
        if ((profile->inner_vlan_in == ptr->inner_vlan_in) ||               /* Vlans are the same */
            (profile->inner_vlan_in == 0 && ptr->inner_vlan_in != 0) ||     /* one is EVC counter, and the other is client counter */
            (profile->inner_vlan_in != 0 && ptr->inner_vlan_in == 0))
          return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/**
 * Dump list of bw policers
 */
void ptin_fpcounters_dump_debug(void)
{
  L7_int index, rule, stage;
  ptin_evcStats_policy_t *ptr;
  BROAD_GROUP_t group_id;
  BROAD_ENTRY_t entry_id;

  printf("Listing bandwidth counter list...\r\n");

  for (index=0; index<PTIN_SYSTEM_MAX_COUNTERS; index++)
  {
    if (!fp_counters_data[index].inUse)  continue;

    ptr = &fp_counters_data[index];

    printf("Index %d:\r\n",index);
    printf("  inUse     = %u\r\n",ptr->inUse);
    printf("  ddUsp_src = {%d,%d,%d}\r\n",ptr->ddUsp_src.unit,ptr->ddUsp_src.slot,ptr->ddUsp_src.port);
    printf("  ddUsp_dst = {%d,%d,%d}\r\n",ptr->ddUsp_dst.unit,ptr->ddUsp_dst.slot,ptr->ddUsp_dst.port);
    printf("  OVID_in   = %u\r\n",ptr->outer_vlan_in);
    printf("  OVID_out  = %u\r\n",ptr->outer_vlan_out);
    printf("  IVID_in   = %u\r\n",ptr->inner_vlan_in);
    printf("  IVID_out  = %u\r\n",ptr->inner_vlan_out);
    printf("  DIP       = %03u.%03u.%03u.%03u\r\n",(ptr->dip>>24) & 0xFF,(ptr->dip>>16) & 0xFF,(ptr->dip>>8) & 0xFF,ptr->dip & 0xFF);
    for (stage=BROAD_POLICY_STAGE_INGRESS; stage<=BROAD_POLICY_STAGE_EGRESS; stage++)
    {
      printf("  Stage=%s\r\n",((stage==BROAD_POLICY_STAGE_INGRESS) ? "INGRESS" : "EGRESS"));
      printf("    policy_id = %d\r\n",ptr->policy_id[stage]);
      for (rule=0; rule<PTIN_PACKETS_TYPE_MAX; rule++)
      {
        /* Also print hw group id and entry id*/
        if (l7_bcm_policy_hwInfo_get(0,ptr->policy_id[stage],ptr->rule_id[stage][rule],&group_id,&entry_id,L7_NULLPTR,L7_NULLPTR)==L7_SUCCESS)
        {
          printf("    Rule %d: group=%d, entry=%d\r\n",ptr->rule_id[stage][rule],group_id,entry_id);
        }
      }
    }
  }
  fflush(stdout);
}

