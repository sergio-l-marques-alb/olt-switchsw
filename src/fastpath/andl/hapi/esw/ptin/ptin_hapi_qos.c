/*
* QoS
*
* Description: QoS management
*
*
* Author: Milton Ruas
*
* Copyright: See COPYING file that comes with this distribution
*
*/
#include "ptin_globaldefs.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "ptin_hapi_qos.h"
#include "l7_common.h"

#include "broad_policy.h"
#include "broad_group_bcm.h"

#define PTIN_HAPI_QOS_TABLE_SIZE    20
#define PTIN_HAPI_QOS_VLAN_ENTRIES  32

typedef struct
{
  L7_uint16   vlan_id;
  L7_BOOL     leaf_side;
  L7_uint8    trust_mode;
} ptin_hapi_qos_entry_key_t;

typedef struct
{
  L7_BOOL   entry_active;

  ptin_hapi_qos_entry_key_t key;

  bcm_pbmp_t  port_bmp;

  L7_uint8 number_of_rules;
  struct
  {
    L7_BOOL     in_use;
    L7_uint8    priority;
    L7_uint8    priority_mask;
    L7_uint8    int_priority;
    BROAD_POLICY_t  policyId;
  } rule[PTIN_HAPI_QOS_VLAN_ENTRIES];

} ptin_hapi_qos_entry_t;

L7_uint16 hw_rules_total = 0;
ptin_hapi_qos_entry_t hapi_qos_table[PTIN_HAPI_QOS_TABLE_SIZE];


/**
 * Get pbm format por ports
 * 
 * @param dapiPort 
 * @param ptin_port_bmp 
 * @param pbm 
 * @param pbm_mask 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_hapi_port_bitmap_get(ptin_dapi_port_t *dapiPort, L7_uint64 ptin_port_bmp,
                                         bcm_pbmp_t *pbm, bcm_pbmp_t *pbm_mask)
{
  L7_int i;
  DAPI_PORT_t  *dapiPortPtr = L7_NULLPTR;
  BROAD_PORT_t *hapiPortPtr = L7_NULLPTR;

  /* TODO: configure rule */
  BCM_PBMP_CLEAR(*pbm);

  if (ptin_port_bmp != 0)
  {
    if (hapi_ptin_bcmPbmPort_get(ptin_port_bmp, pbm) != L7_SUCCESS) 
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error converting port bitmap to pbmp format");;
      return L7_FAILURE;
    }
  }
  else if (dapiPort->usp->port >= 0 && dapiPort->usp->slot >= 0 && dapiPort->usp->port >= 0)
  {
    BROAD_PORT_t *hapiPortPtr_member;

    dapiPortPtr = DAPI_PORT_GET( dapiPort->usp, dapiPort->dapi_g );
    hapiPortPtr = HAPI_PORT_GET( dapiPort->usp, dapiPort->dapi_g );

    /* Extract Trunk id */
    if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
    {
      /* Apply to all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g);
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }

        /* Add this physical port to bitmap */
        BCM_PBMP_PORT_ADD(*pbm, hapiPortPtr_member->bcm_port);
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"bcm_port %d added", hapiPortPtr_member->bcm_port);
      }
    }
    /* Extract Physical port */
    else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      BCM_PBMP_PORT_ADD(*pbm, hapiPortPtr->bcm_port);
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"bcm_port %d considered", hapiPortPtr->bcm_port);
    }
    /* Not valid type */
    else
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Interface has a not valid type: error!");
      return L7_FAILURE;
    }
  }

  /* PBM mask: all ports */
  BCM_PBMP_CLEAR(*pbm_mask);
  if (hapi_ptin_bcmPbmPort_get((L7_uint64)-1, pbm_mask) != L7_SUCCESS) 
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error converting port bitmap to pbmp format");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Init QoS
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_init(void)
{
  return L7_SUCCESS;
}

/**
 * Removes all entries belonging to a VLAN
 * 
 * @author mruas (10/27/2015)
 * 
 * @param vlan_id 
 * @param leaf_side : Root or leaf interfaces? 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_vlan_remove(L7_uint16 vlan_id, L7_BOOL leaf_side)
{
  ptin_dtl_qos_t qos_cfg;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Going to clear all VLAN %u rules", vlan_id);

  memset(&qos_cfg, 0x00, sizeof(ptin_dtl_qos_t));

  qos_cfg.vlan_id    = vlan_id;
  qos_cfg.leaf_side  = leaf_side;
  qos_cfg.trust_mode = 0;

  return ptin_hapi_qos_entry_remove(&qos_cfg);
}

/**
 * Add a QoS entry
 * 
 * @param dapiPort 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_qos_t *qos_cfg)
{
  L7_int      entry, free_entry, rule, max_rules;
  L7_uint16   vlan_mask = 0xfff;
  bcm_pbmp_t  pbm, pbm_mask;
  ptin_hapi_qos_entry_t *qos_entry;
  L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_HAPI, "VLAN %u, leaf:%u, trust_mode, port_pbm=0x%llx, prio=%u/0x%x -> CoS=%u",
            qos_cfg->vlan_id, qos_cfg->leaf_side, qos_cfg->trust_mode, qos_cfg->ptin_port_bmp, qos_cfg->priority, qos_cfg->priority_mask, qos_cfg->int_priority);

  /* Get pbm format of ports */
  if (ptin_hapi_port_bitmap_get(dapiPort, qos_cfg->ptin_port_bmp, &pbm, &pbm_mask) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error converting port bitmap to pbmp format");
    return L7_FAILURE;
  }

  /* Validate trust mode */
  if (qos_cfg->trust_mode > L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    LOG_WARNING(LOG_CTX_STARTUP, "Invalid trust mode %u",  qos_cfg->trust_mode);
    return L7_FAILURE;
  }

  /* Validate trust_mode, and determine maximum number of rules */
  switch (qos_cfg->trust_mode)
  {
  case 0:
    max_rules = 0;
    break;
  case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
    max_rules = 1;
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
    max_rules = 8;
    break;
  case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
    max_rules = PTIN_HAPI_QOS_VLAN_ENTRIES;
    break;
  default:
    max_rules = PTIN_HAPI_QOS_VLAN_ENTRIES;
    break;
  }

  /* Get VLAN mask */
  vlan_mask = PTIN_VLAN_MASK(qos_cfg->vlan_id);
  qos_cfg->vlan_id &= vlan_mask;

  LOG_TRACE(LOG_CTX_PTIN_HAPI," VLAN=%u/0x%x trust_mode=%u max_rules=%u",
            qos_cfg->vlan_id, vlan_mask, qos_cfg->trust_mode, max_rules);

  /* If trust mode is provided, remove all related entries */
  if (qos_cfg->trust_mode >= 0)
  {
    rc = ptin_hapi_qos_entry_remove(qos_cfg); 
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error removing entries!");
      return L7_FAILURE;
    }
  }

  /* Search for this VLAN */
  free_entry = -1;
  qos_entry = L7_NULLPTR;
  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    if (!hapi_qos_table[entry].entry_active)
    {
      if (free_entry < 0)
        free_entry = entry;
      continue; 
    }

    if (qos_cfg->vlan_id    == hapi_qos_table[entry].key.vlan_id &&
        qos_cfg->leaf_side  == hapi_qos_table[entry].key.leaf_side)
    {
      qos_entry = &hapi_qos_table[entry];
      break;
    }
  }

  /* Reconfigure all rules with newer port bitmap */
  if (qos_cfg->trust_mode < 0)
  {
    bcm_port_t    bcm_port;
    bcmx_lport_t  bcmx_lport;
    bcm_pbmp_t    pbmp_result;

    /* To reconfigure an entry should be found */
    if (qos_entry == L7_NULLPTR)
    {
      LOG_WARNING(LOG_CTX_PTIN_HAPI,"Entry not found... nothing to do");
      return L7_SUCCESS;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Going to reconfigure ports bitmap of VLAN %u / leaf:%u", qos_cfg->vlan_id, qos_cfg->leaf_side);

    /* Run all VLAN rules */
    for (rule = 0; rule < max_rules; rule++)
    {
      if (qos_entry->rule[rule].in_use && qos_entry->rule[rule].policyId != BROAD_POLICY_INVALID)
      {
        /* Merge bitmaps */
        BCM_PBMP_ASSIGN(pbmp_result, pbm);
        BCM_PBMP_OR(pbmp_result, qos_entry->port_bmp);
        /* Add new bitmap */
        BCM_PBMP_ITER(pbmp_result, bcm_port)
        {
          bcmx_lport = bcmx_unit_port_to_lport(0, bcm_port);
          if (BCM_PBMP_MEMBER(pbm, bcm_port) && !BCM_PBMP_MEMBER(qos_entry->port_bmp, bcm_port))
          {
            if (hapiBroadPolicyApplyToIface(qos_entry->rule[rule].policyId, bcmx_lport) != L7_SUCCESS) 
            {
              LOG_ERR(LOG_CTX_PTIN_HAPI, "Error adding bcm_port %u to entry %u, rule %u", bcm_port, entry, rule);
              return L7_FAILURE;
            }
          }
          else if (!BCM_PBMP_MEMBER(pbm, bcm_port) && BCM_PBMP_MEMBER(qos_entry->port_bmp, bcm_port))
          {
            if (hapiBroadPolicyRemoveFromIface(qos_entry->rule[rule].policyId, bcmx_lport) != L7_SUCCESS) 
            {
              LOG_ERR(LOG_CTX_PTIN_HAPI, "Error removing bcm_port %u from entry %u, rule %u", bcm_port, entry, rule);
              return L7_FAILURE;
            }
          }
        }
      }
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Ports bitmap of VLAN %u / leaf:%u updated", qos_cfg->vlan_id, qos_cfg->leaf_side);
    return L7_SUCCESS;
  }

  /* If VLAN not found, use a free entry */
  if (qos_entry == L7_NULLPTR)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Entry not found... searching for a free one");

    /* If no free entry was found... return error */
    if (free_entry < 0)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "No free entries!");
      return L7_TABLE_IS_FULL;
    }

    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Free entry index = %u", free_entry);

    entry = free_entry;
    qos_entry = &hapi_qos_table[entry];
    memset(qos_entry, 0x00, sizeof(ptin_hapi_qos_entry_t));
  }

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Assuming entry index = %u", entry);

  if (qos_entry->entry_active)
  {
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"Entry is in use");

    /* If trust mode is different, clear all entry */
    if (qos_cfg->trust_mode != qos_entry->key.trust_mode)
    {
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Trust mode is different (old value %u). Going to clear VLAN.", qos_entry->key.trust_mode);

      rc = ptin_hapi_qos_vlan_remove(qos_cfg->vlan_id, qos_cfg->leaf_side);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_HAPI, "Error clearing VLAN entry!");
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"VLAN %u / leaf:%u cleared.", qos_cfg->vlan_id, qos_cfg->leaf_side);
    }
  }

  /* Redundant configuration... do nothing */
  if ((qos_cfg->trust_mode == 0) ||
      ((qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P) &&
        (qos_cfg->priority_mask & 0x7) == 0x7 && qos_cfg->priority == qos_cfg->int_priority))
  {
    LOG_WARNING(LOG_CTX_STARTUP, "Redundant rule for trust_mode %u and prio %u/0x%x! Leaving...",
                qos_cfg->trust_mode, qos_cfg->priority, qos_cfg->priority_mask);
    return L7_SUCCESS;
  }

  /* Search for an empty rule (to be created) */
  for (rule = 0; rule < max_rules; rule++)
  {
    if (!qos_entry->rule[rule].in_use)
      break;
  }
  /* Not found... return error */
  if (rule >= max_rules)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "No free entries!");
    return L7_TABLE_IS_FULL;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Inside entry %u, rule %u will be used.", entry, rule);

  /* Create rule */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_VLAN);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Cannot create trap policy");
    return L7_FAILURE;
  }
  /* Ingress stage */
  rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_INGRESS);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Error creating a egress policy");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  /* Create rule */
  rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Error adding rule");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  /* Inports qualifier */
  rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uchar8 *)&pbm, (L7_uchar8 *)&pbm_mask);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Error adding INPORTS qualifier");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Inports qualifier added");

  /* Outer VLAN */
  rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&qos_cfg->vlan_id, (L7_uint8 *) &vlan_mask);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Error adding OVID qualifier (%u)", qos_cfg->vlan_id);
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"OVID %u/0x%x qualifier added", qos_cfg->vlan_id, vlan_mask);

  /* Check trust mode and apply appropriated configuration */
  if (qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
  {
    if ((qos_cfg->priority_mask & 0x7) != 0)
    {
      /* Pbit */
      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OCOS, (L7_uchar8 *)&qos_cfg->priority, (L7_uchar8 *)&qos_cfg->priority_mask);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_STARTUP, "Error adding OCOS qualifier (%u/0x%x)", qos_cfg->priority, qos_cfg->priority_mask);
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"OCOS %u/0x%x qualifier added", qos_cfg->priority, qos_cfg->priority_mask);
    }
  }
  else if (qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
  {
    L7_ushort16   ethertype_ipv4  = L7_ETYPE_IP;
    L7_uchar8     dscp_value, dscp_mask;

    dscp_value = (qos_cfg->priority << 5) & 0xff;
    dscp_mask  = (qos_cfg->priority_mask << 5) & 0xff;

    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ethertype_ipv4, exact_mask);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding ETHTYPE qualifier (0x%04x)", ethertype_ipv4);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"ETHERTYPE 0x%x qualifier added", ethertype_ipv4);

    if ((qos_cfg->priority_mask & 0x7) != 0)
    {
      /* DSCP  */
      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DSCP, (L7_uchar8 *)&dscp_value, (L7_uchar8 *)&dscp_mask);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_STARTUP, "Error adding DSCP qualifier (%u/0x%x)", dscp_value, dscp_mask);
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"DSCP %u/0x%x qualifier added", dscp_value, dscp_mask);
    }
  }
  else if (qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    L7_ushort16   ethertype_ipv4  = L7_ETYPE_IP;
    L7_uchar8     dscp_value, dscp_mask;

    dscp_value = (qos_cfg->priority) & 0xff;
    dscp_mask  = (qos_cfg->priority_mask) & 0xff;

    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ethertype_ipv4, exact_mask);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding ETHTYPE qualifier (0x%04x)", ethertype_ipv4);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_HAPI,"ETHERTYPE 0x%x qualifier added", ethertype_ipv4);

    if ((qos_cfg->priority_mask & 0x3f) != 0)
    {
      /* DSCP  */
      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DSCP, (L7_uchar8 *)&dscp_value, (L7_uchar8 *)&dscp_mask);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_STARTUP, "Error adding DSCP qualifier (%u/0x%x)", dscp_value, dscp_mask);
        hapiBroadPolicyCreateCancel();
        return L7_FAILURE;
      }
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"DSCP %u/0x%x qualifier added", dscp_value, dscp_mask);
    }
  }

  /* Set new internal priority */
  if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, qos_cfg->int_priority, 0, 0) != L7_SUCCESS ||
      hapiBroadPolicyRuleExceedActionAdd(ruleId, BROAD_ACTION_SET_COSQ, qos_cfg->int_priority, 0, 0) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Error adding SET_COSQ action");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_HAPI,"COSQ action added (%u)", qos_cfg->int_priority);

  /* Apply policy */
  if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_STARTUP, "Error commiting trap policy");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_STARTUP, "Trap policy commited successfully (policyId=%u)",policyId);

  /* Save entry */
  qos_entry->entry_active       = L7_TRUE;
  qos_entry->key.vlan_id        = qos_cfg->vlan_id;
  qos_entry->key.leaf_side      = qos_cfg->leaf_side;
  qos_entry->key.trust_mode     = qos_cfg->trust_mode;
  BCM_PBMP_ASSIGN(qos_entry->port_bmp, pbm);

  qos_entry->rule[rule].priority      = qos_cfg->priority;
  qos_entry->rule[rule].priority_mask = qos_cfg->priority_mask;
  qos_entry->rule[rule].int_priority  = qos_cfg->int_priority;
  qos_entry->rule[rule].policyId      = policyId;
  qos_entry->rule[rule].in_use        = L7_TRUE;

  qos_entry->number_of_rules++;
  hw_rules_total++;

  LOG_TRACE(LOG_CTX_PTIN_HAPI,"Operation finished successfully");

  return L7_SUCCESS;
}

/**
 * Remove a QoS entry
 * 
 * @param dapiPort 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_entry_remove(ptin_dtl_qos_t *qos_cfg)
{
  L7_uint8  mask;
  L7_uint   entry, rule;
  ptin_hapi_qos_entry_t *qos_entry;

  /* Search all related entries */
  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    qos_entry = &hapi_qos_table[entry];

    /* Skip not active entries */
    if (!qos_entry->entry_active)
      continue;

    /* Skip non included entries */
    if ((qos_cfg->vlan_id != qos_entry->key.vlan_id) ||
        (qos_cfg->leaf_side >= 0 && qos_cfg->leaf_side != qos_entry->key.leaf_side))
      continue;

    /* Run all rules belonging to this VLAN */
    for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
    {
      if (!qos_entry->rule[rule].in_use)
        continue;

      /* Common mask for comparison */
      mask = qos_cfg->priority_mask & qos_entry->rule[rule].priority_mask;

      /* Only look to trustmode, priority and mask, if trust_mode is not null */
      if (qos_cfg->trust_mode != 0)
      {
        if (qos_cfg->trust_mode != qos_entry->key.trust_mode) 
          continue;

        if ((qos_cfg->priority & mask) != (qos_entry->rule[rule].priority & mask))
          continue;
      }

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Going to remove entry %u, rule %u: vlan=%u/leaf:%u trust_mode=%u prio=%u/0x%x (policyId=%d)",
                entry, rule, qos_entry->key.vlan_id, qos_entry->key.leaf_side, qos_entry->key.trust_mode,
                qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].policyId);

      /* Delete rule */
      if (qos_entry->rule[rule].policyId != BROAD_POLICY_INVALID)
      {
        hapiBroadPolicyDelete(qos_entry->rule[rule].policyId);
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policy %u destroyed (prio=%u/0x%x)",
                  qos_entry->rule[rule].policyId, qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask);
        qos_entry->rule[rule].policyId = BROAD_POLICY_INVALID;
      }

      /* Deactivate rule */
      qos_entry->rule[rule].in_use = L7_FALSE;

      if (qos_entry->number_of_rules > 0)
        qos_entry->number_of_rules--;

      if (hw_rules_total > 0)
        hw_rules_total--;

      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Rule removed: VLAN rules=%u  Total rules=%u",
                qos_entry->number_of_rules, hw_rules_total);
    }

    /* If all rules were removed, clear this VLAN entry */
    if (qos_cfg->trust_mode == 0 || qos_entry->number_of_rules == 0)
    {
      /* Clear entry */
      memset(qos_entry, 0x00, sizeof(ptin_hapi_qos_entry_t));
      qos_entry->entry_active = L7_FALSE;
      LOG_TRACE(LOG_CTX_PTIN_HAPI,"Entry belonging to VLAN %u / leaf:%u was cleared!", qos_cfg->vlan_id, qos_cfg->leaf_side);
    }
  }

  return L7_SUCCESS;
}

/**
 * Flush several entries of the QoS table
 * 
 * @param dapiPort 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_table_flush(ptin_dapi_port_t *dapiPort, ptin_dtl_qos_t *qos_cfg)
{
  L7_uint entry, rule;

  /* Run all active entries */
  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    if (!hapi_qos_table[entry].entry_active)
      continue;

    /* Delete rules */
    for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
    {
      if (!hapi_qos_table[entry].rule[rule].in_use)
        continue;

      if (hapi_qos_table[entry].rule[rule].policyId != BROAD_POLICY_INVALID) 
      {
        hapiBroadPolicyDelete(hapi_qos_table[entry].rule[rule].policyId);
        LOG_TRACE(LOG_CTX_PTIN_HAPI,"Policy %u destroyed destroyed", hapi_qos_table[entry].rule[rule].policyId);
        hapi_qos_table[entry].rule[rule].policyId = BROAD_POLICY_INVALID;
      }

      hapi_qos_table[entry].rule[rule].in_use = L7_FALSE;

      if (hapi_qos_table[entry].number_of_rules > 0)
        hapi_qos_table[entry].number_of_rules--;

      if (hw_rules_total > 0)
        hw_rules_total--;
    }

    /* Clear entry */
    memset(&hapi_qos_table[entry], 0x00, sizeof(ptin_hapi_qos_entry_t));
    hapi_qos_table[entry].entry_active = L7_FALSE;
  }

  return L7_SUCCESS;
}

/**
 * Update a QoS entry
 * 
 * @param dapiPort 
 * @param qos_cfg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_dump(void)
{
  L7_uint entry, j, rule;
  ptin_hapi_qos_entry_t *qos_entry;
  BROAD_GROUP_t group_id;
  BROAD_ENTRY_t entry_id;

  printf("Dumping VLAN-QoS table...\r\n");

  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    qos_entry = &hapi_qos_table[entry];

    if (!qos_entry->entry_active)
      continue;

    printf("Entry %-2u:  VlanID=%-4u (%s)  TrustMode=%u  Pbmp = 0x", entry,
           qos_entry->key.vlan_id,
           ((qos_entry->key.leaf_side) ? "LEAF" : "ROOT"),
           qos_entry->key.trust_mode);
    for (j = 0; j < _SHR_PBMP_WORD_MAX; j++)
    {
      printf("%08x ", qos_entry->port_bmp.pbits[j]);
    }
    printf("\r\n");

    for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
    {
      if (!qos_entry->rule[rule].in_use)
        continue;

      printf("\t    prio=%2u/0x%02x -> intPrio=%-2u (",
             qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].int_priority);

      if (qos_entry->rule[rule].policyId != BROAD_POLICY_INVALID)
      {
        printf("PolicyId=%-4d, ", qos_entry->rule[rule].policyId);
         
        if (l7_bcm_policy_hwInfo_get(0, qos_entry->rule[rule].policyId, 0, &group_id, &entry_id, L7_NULLPTR, L7_NULLPTR) == L7_SUCCESS) 
        {
          printf("group=%-2d, entry=%-4d", group_id, entry_id);
        }
        else
        {
          printf("error obtaining group/entry information");
        }
      }
      else
      {
        printf("No policy defined");
      }
      printf(")\r\n");
    }
  }
  printf("Total number of hw rules: %u\r\n", hw_rules_total);

  return L7_SUCCESS;
}

