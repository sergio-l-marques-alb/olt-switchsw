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
#include "ptin_utils.h"

#include "bcm/port.h"

#include "broad_policy.h"
#include "broad_group_bcm.h"

#define PTIN_HAPI_QOS_TABLE_SIZE    128   /* Maximum number of services */
#define PTIN_HAPI_QOS_VLAN_ENTRIES  32    /* Maximum number of rules per service */
#define PTIN_HAPI_QOS_HW_RULES_MAX  512   /* Maximum number of available hardware rules (for all services) */

typedef struct
{
  L7_uint16   ext_vlan;
  L7_uint16   int_vlan;
  L7_BOOL     leaf_side;
  L7_uint8    trust_mode;
} ptin_hapi_qos_entry_key_t;

typedef struct
{
  L7_BOOL     in_use;
  L7_uint8    priority;
  L7_uint8    priority_mask;
  L7_uint8    int_priority;
  L7_BOOL     pbits_remark;
  BROAD_POLICY_t  policyId_icap;
} ptin_hapi_qos_rule_t;

typedef struct
{
  L7_BOOL   entry_active;

  ptin_hapi_qos_entry_key_t key;

  L7_uint64       usp_port_bmp;
  bcm_pbmp_t      bcm_port_bmp;
  L7_int32        classId;

  L7_uint8 number_of_rules;
  ptin_hapi_qos_rule_t rule[PTIN_HAPI_QOS_VLAN_ENTRIES];

} ptin_hapi_qos_entry_t;

L7_uint16 hw_rules_total = 0;
ptin_hapi_qos_entry_t hapi_qos_table[PTIN_HAPI_QOS_TABLE_SIZE];


#define MAX_CLASS_ID  256

typedef struct
{
  L7_BOOL in_use;

  L7_uint16 vlan_value;
  L7_uint16 vlan_mask;

  L7_uint16 number_of_links;
  BROAD_POLICY_t policyId_vcap;
} ptin_hapi_classid_entry_t;

ptin_hapi_classid_entry_t classid_table[MAX_CLASS_ID];


/* EVC entry (for pool queue) */
struct classId_entry_s {
  /* Pointers used in queues manipulation (MUST be placed at the top of the struct) */
  struct classId_entry_s *next;
  struct classId_entry_s *prev;

  L7_int32 classId;  /* One index of class IDs array */
};

/* Array with all the indexes of Class IDs to be used */
static struct classId_entry_s  classId_pool[MAX_CLASS_ID];

/* Queues */
static dl_queue_t queue_free_classIds;    /* Queue of free Class ID entries */


/**
 * Search for an entry in ClassID table
 * 
 * @param extVlan 
 * 
 * @return class ID 
 */
static L7_int ptin_hapi_classid_table_search(L7_uint16 extVlan, L7_uint16 mask)
{
  L7_uint classId;

  for (classId = 0; classId < MAX_CLASS_ID; classId++)
  {
    /* Skip non used entries */
    if (!classid_table[classId].in_use)
      continue;

    /* Search for an entry with matched vlans */
    if (classid_table[classId].vlan_value == extVlan &&
        classid_table[classId].vlan_value == mask)
    {
      return classId;
    }
  }

  return -1;
}

/**
 * Search for an entry in QoS table
 * 
 * @param qos_cfg 
 * @param ruleId 
 * 
 * @return ptin_hapi_qos_entry_t* 
 */
static ptin_hapi_qos_entry_t *ptin_hapi_qos_table_search(ptin_dtl_qos_t *qos_cfg, L7_int *ruleId)
{
  L7_uint entry, rule;

  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    /* Skip non used entries */
    if (!hapi_qos_table[entry].entry_active)
    {
      continue;
    }

    /* Search for an entry with matched vlans */
    if ((qos_cfg->int_vlan == hapi_qos_table[entry].key.int_vlan && 
           qos_cfg->ext_vlan == hapi_qos_table[entry].key.ext_vlan) &&
        (qos_cfg->leaf_side == hapi_qos_table[entry].key.leaf_side))
    {
      if (ruleId != L7_NULLPTR)
      {
        /* Search rule */
        for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
        {
          if (!hapi_qos_table[entry].rule[rule].in_use)
            continue;

          if (qos_cfg->priority == hapi_qos_table[entry].rule[rule].priority &&
              qos_cfg->priority_mask == hapi_qos_table[entry].rule[rule].priority_mask)
          {
            *ruleId = rule;
            break;
          }
        }
        /* Not found */
        if (rule >= PTIN_HAPI_QOS_VLAN_ENTRIES)
        {
          *ruleId = -1;
        }
      }
      return &hapi_qos_table[entry];
    }
  }

  return L7_NULLPTR;
}

/**
 * Search for free entry and rule
 * 
 * @param ruleId 
 * 
 * @return ptin_hapi_qos_entry_t* 
 */
static ptin_hapi_qos_entry_t *ptin_hapi_qos_table_free_entry(L7_int *ruleId)
{
  L7_uint entry, rule;

  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    /* Skip non used entries */
    if (!hapi_qos_table[entry].entry_active)
    {
      /* Search free ruleId */
      if (ruleId != L7_NULLPTR)
      {
        /* Search rule */
        for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
        {
          if (!hapi_qos_table[entry].rule[rule].in_use)
          {
            *ruleId = rule;
            break;
          }
        }
        /* Not found */
        if (rule >= PTIN_HAPI_QOS_VLAN_ENTRIES)
        {
          *ruleId = -1;
        }
      }
      return &hapi_qos_table[entry];
    }
  }

  return L7_NULLPTR;
}

/**
 * Allocate a new Class ID for a particular Ext Vlan
 * 
 * @author mruas (11/4/2015)
 * 
 * @param ext_vlan : External VLAN
 * @param mask : VLAN mask
 * @param classId_ret : new class id 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_hapi_classid_allocate(L7_uint16 ext_vlan, L7_uint16 mask, L7_int32 *classId_ret)
{
  L7_int32 classId;
  struct classId_entry_s *classId_pool_entry;
  BROAD_POLICY_t      policyId_vcap;
  BROAD_POLICY_RULE_t ruleId;
  L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_RC_t rc = L7_SUCCESS;

  /* Search for an existent entry */
  classId = ptin_hapi_classid_table_search(ext_vlan, mask);

  /* If a class id was found, there is nothing to do */
  if (classId >= 0 && classId < MAX_CLASS_ID)
  {
    PT_LOG_TRACE(LOG_CTX_QOS, "Class ID %d already exists", classId);
    /* Return class id */
    if (classId_ret != L7_NULLPTR)
    {
      *classId_ret = classId;
    }
    return L7_SUCCESS;
  }

  /* Get new Class ID */
  rc = dl_queue_remove_head(&queue_free_classIds, (dl_queue_elem_t **) &classId_pool_entry);
  if (rc != NOERR) {
    PT_LOG_CRITIC(LOG_CTX_QOS, "There are no free Class IDs available! rc=%d", rc);
    return L7_TABLE_IS_FULL;
  }

  /* Validate extracted ClassID */
  if (classId_pool_entry->classId < 0 || classId_pool_entry->classId >= MAX_CLASS_ID)
  {
    PT_LOG_CRITIC(LOG_CTX_QOS, "Invalid extracted Class ID: %d", classId_pool_entry->classId);
    dl_queue_add_head(&queue_free_classIds, (dl_queue_elem_t *) classId_pool_entry);
    return L7_FAILURE;
  }

  /* Save new class id */
  classId = classId_pool_entry->classId;

  /* Check if there is a rule configured: Remove it, if exists */
  if (classid_table[classId].in_use &&
      classid_table[classId].policyId_vcap != BROAD_POLICY_INVALID)
  {
    hapiBroadPolicyDelete(classid_table[classId].policyId_vcap);
    classid_table[classId].policyId_vcap = BROAD_POLICY_INVALID;
    classid_table[classId].in_use = L7_FALSE;

    PT_LOG_WARN(LOG_CTX_QOS, "Removed policyId %u belonging to classId %u",
                classid_table[classId].policyId_vcap, classId);
  }

  PT_LOG_TRACE(LOG_CTX_QOS, "Extracted Class ID %d", classId);

  /* Configure VCAP rule */
  do
  {
    /* Create rule */
    rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_VLAN);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Cannot create trap policy");
      break;
    }
    /* Ingress stage */
    rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error creating a lookup policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    /* Create rule */
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error adding rule");
      hapiBroadPolicyCreateCancel();
      break;
    }
    /* Outer VLAN */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&ext_vlan, exact_mask);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error adding OVID qualifier (%u)", ext_vlan);
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_QOS,"OVID %u qualifier added", ext_vlan);

    /* Set src class id */
    if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_SRC_CLASS_ID, (L7_uint32) classId, 0, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error adding SET_SRC_CLASS_ID action");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_QOS,"SET_SRC_CLASS_ID action added (%d)", classId);

    /* Apply policy */
    if ((rc=hapiBroadPolicyCommit(&policyId_vcap)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error commiting trap policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_QOS, "Trap policy commited successfully (policyId_vcap=%u)",policyId_vcap);
  }
  while (0);

  /* Have occurred any error? */
  if (rc != L7_SUCCESS)
  {
    /* Delete VCAP rule */
    if (policyId_vcap != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(policyId_vcap);
      PT_LOG_TRACE(LOG_CTX_QOS, "Removing policyId_vcap %u", policyId_vcap);
    }

    /* Restore ClassID */
    dl_queue_add_head(&queue_free_classIds, (dl_queue_elem_t*) &classId_pool[classId]);
    PT_LOG_TRACE(LOG_CTX_QOS, "Adding again classId %u to free queue", classId);

    PT_LOG_ERR(LOG_CTX_QOS, "Error while creating VCAP rule");
    return rc;
  }

  /* Save classid information */
  classid_table[classId].in_use     = L7_TRUE;
  classid_table[classId].vlan_value = ext_vlan;
  classid_table[classId].vlan_mask  = 0xfff;
  classid_table[classId].policyId_vcap = policyId_vcap;

  PT_LOG_TRACE(LOG_CTX_QOS,"VCAP rule configured with classId %d", classId);

  /* Return class id */
  if (classId_ret != L7_NULLPTR)
  {
    *classId_ret = classId;
  }

  return L7_SUCCESS;
}

/**
 * Free a class id
 * 
 * @param classId 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_hapi_classid_free(L7_int32 classId)
{
  /* Validate class id */
  if (classId < 0 && classId >= MAX_CLASS_ID)
  {
    PT_LOG_ERR(LOG_CTX_QOS,"Invalid class id %u", classId);
    return L7_FAILURE;
  }

  /* Check if class id is active */
  if (!classid_table[classId].in_use)
  {
    PT_LOG_TRACE(LOG_CTX_QOS,"Class id %u is not used", classId);
    return L7_SUCCESS;
  }

  #if 0
  /* One less link */
  if (classid_table[classId].number_of_links > 0)
  {
    classid_table[classId].number_of_links--;
    PT_LOG_TRACE(LOG_CTX_QOS,"Now with %u links", classid_table[classId].number_of_links); 
  }

  /* Remove class id, if it has no links */
  if (classid_table[classId].number_of_links == 0)
  {
  #endif
    /* Delete VCAP rule */
    if (classid_table[classId].policyId_vcap != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(classid_table[classId].policyId_vcap);
      PT_LOG_TRACE(LOG_CTX_QOS,"Policy %u destroyed", classid_table[classId].policyId_vcap);
      classid_table[classId].policyId_vcap = BROAD_POLICY_INVALID;
    }
    /* Restore class id to free queue */
    dl_queue_add_head(&queue_free_classIds, (dl_queue_elem_t*) &classId_pool[classId]);

    classid_table[classId].in_use = L7_FALSE;
    classid_table[classId].number_of_links = 0;
  #if 0
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_QOS,"We still have %u links... nothing to do", classid_table[classId].number_of_links);
  }
  #endif

  return L7_SUCCESS;
}

/**
 * Clear rule of specific entry
 * 
 * @param qos_entry : Pointer to QoS table entry
 * @param ruleId : rule identifier (-1 to clear all)
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_hapi_qos_rule_free(ptin_hapi_qos_entry_t *qos_entry, L7_int ruleId)
{
  L7_uint rule;
  L7_int32 classId;

  if (qos_entry == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_QOS, "Invalid arguments");
    return L7_FAILURE;
  }

  if (!qos_entry->entry_active)
  {
    PT_LOG_WARN(LOG_CTX_QOS, "Entry not active... nothing to do");
    return L7_SUCCESS;
  }

  /* Run all rules */
  for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
  {
    /* Clear rule, if provided ruleId is invalid, or, if being valid, matches with referenced rule */
    if ((ruleId >= 0 && ruleId < PTIN_HAPI_QOS_VLAN_ENTRIES) &&
        rule != ruleId) 
      continue;

    if (!qos_entry->rule[rule].in_use)
    {
      continue;
    }

    /* Delete ICAP rule */
    if (qos_entry->rule[rule].policyId_icap != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(qos_entry->rule[rule].policyId_icap);
      PT_LOG_TRACE(LOG_CTX_QOS,"Policy %u destroyed (prio=%u/0x%x)",
                qos_entry->rule[rule].policyId_icap, qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask);
      qos_entry->rule[rule].policyId_icap = BROAD_POLICY_INVALID;
    }

    /* Deactivate rule */
    qos_entry->rule[rule].in_use = L7_FALSE;

    if (qos_entry->number_of_rules > 0)
      qos_entry->number_of_rules--;

    if (hw_rules_total > 0)
      hw_rules_total--;
  }

  /* If no rules present, clear entry */
  if ((ruleId < 0 || ruleId >= PTIN_HAPI_QOS_VLAN_ENTRIES) ||
      (qos_entry->number_of_rules == 0))
  {
    classId = qos_entry->classId;

    /* If class id is used, update number of links to classid_table */
    if (classId >= 0 && classId < MAX_CLASS_ID)
    {
      /* One less link */
      if (classid_table[classId].number_of_links > 0)
      {
        classid_table[classId].number_of_links--;
        PT_LOG_TRACE(LOG_CTX_QOS,"Now with %u links", classid_table[classId].number_of_links); 
      }

      /* Remove class id, if it has no links */
      if (classid_table[classId].number_of_links == 0)
      {
        if (ptin_hapi_classid_free(classId) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_QOS,"Error freeing class id %u", classId);
          return L7_FAILURE;
        }
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_QOS,"We still have %u links... nothing to do", classid_table[classId].number_of_links);
      }
    }

    /* Clear entry */
    memset(qos_entry, 0x00, sizeof(ptin_hapi_qos_entry_t));
    qos_entry->entry_active   = L7_FALSE;
    qos_entry->classId        = -1;
    PT_LOG_TRACE(LOG_CTX_QOS,"Entry fully cleared!");
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
  L7_uint i;

  /* Initialize Class IDs free queue and update Class ID indexes of the pool elements */
  dl_queue_init(&queue_free_classIds);
  for (i=1; i<MAX_CLASS_ID; i++)
  {
    classId_pool[i].classId = i;
    dl_queue_add_tail(&queue_free_classIds, (dl_queue_elem_t*) &classId_pool[i]);
  }

  return L7_SUCCESS;
}

/**
 * Enable or disable Pbits remarking at egress port
 * 
 * @param dapiPort 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_egress_pbits_remark(ptin_dapi_port_t *dapiPort, L7_BOOL enable)
{
  L7_uint i;
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr, *hapiPortPtr_member;
  bcm_error_t rv;

  if (dapiPort == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_QOS, "Invalid arguments!");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  if (hapiPortPtr == L7_NULLPTR || dapiPortPtr == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_QOS, "Invalid interface!");
    return L7_FAILURE;
  }

  /* Port is physical? */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    rv = bcm_port_control_set(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, bcmPortControlEgressVlanPriUsesPktPri, !enable);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error setting bcmPortControlEgressVlanPriUsesPktPri in port {%d,%d,%d} to %u (rv=%d)",
                 dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, !enable, rv);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_QOS, "bcmPortControlEgressVlanPriUsesPktPri of port {%d,%d,%d} set to %u",
                   dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, !enable);
    }
  }
  else
  {
    /* Apply to all member ports */
    for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

      hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
      if (hapiPortPtr_member==L7_NULLPTR)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                   dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                   dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                   dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
        return L7_FAILURE;
      }
      /* Get enable status for member port */
      rv = bcm_port_control_set(hapiPortPtr_member->bcm_unit, hapiPortPtr_member->bcm_port, bcmPortControlEgressVlanPriUsesPktPri, !enable);
      if (rv != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error setting bcmPortControlEgressVlanPriUsesPktPri in port {%d,%d,%d} to %u (rv=%d)",
                   dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                   dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                   dapiPortPtr->modeparm.lag.memberSet[i].usp.port,
                   !enable, rv);
        return L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_QOS, "bcmPortControlEgressVlanPriUsesPktPri of port {%d,%d,%d} set to %u",
                     dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                     dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                     dapiPortPtr->modeparm.lag.memberSet[i].usp.port,
                     !enable);
      }
    }
  }

  return L7_SUCCESS;
}


/**
 * Removes all entries belonging to a VLAN
 * 
 * @author mruas (10/27/2015)
 *  
 * @param ext_vlan 
 * @param int_vlan 
 * @param leaf_side : Root or leaf interfaces? 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_vlan_remove(L7_uint16 ext_vlan, L7_uint16 int_vlan, L7_BOOL leaf_side)
{
  ptin_dtl_qos_t qos_cfg;

  PT_LOG_TRACE(LOG_CTX_QOS,"Going to clear all intVLAN %u / extVlan %u rules", int_vlan, ext_vlan);

  memset(&qos_cfg, 0x00, sizeof(ptin_dtl_qos_t));

  qos_cfg.ext_vlan   = ext_vlan;
  qos_cfg.int_vlan   = int_vlan;
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
  L7_int      rule, max_rules, free_rule=0;
  L7_uint16   vlan_mask = 0xfff;
  bcm_pbmp_t  pbm, pbm_mask;
  L7_int32    classId;
  ptin_hapi_qos_entry_t *qos_entry;
  L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  BROAD_POLICY_t      policyId_icap;
  BROAD_POLICY_RULE_t ruleId;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_QOS, "intVLAN %u, extVlan %u, leaf:%u, port_pbm=0x%llx, trust_mode=%u, remark=%u, prio=%u/0x%x -> CoS=%u",
            qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side,
            qos_cfg->port_bmp, qos_cfg->trust_mode, qos_cfg->pbits_remark,
            qos_cfg->priority, qos_cfg->priority_mask, qos_cfg->int_priority);

  /* Get pbm format of ports */
  if (hapi_ptin_port_bitmap_get(dapiPort->usp, dapiPort->dapi_g,
                                qos_cfg->port_bmp, &pbm, &pbm_mask) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_QOS, "Error converting port bitmap to pbmp format");
    return L7_FAILURE;
  }

  /* Validate trust mode */
  if (qos_cfg->trust_mode > L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    PT_LOG_ERR(LOG_CTX_QOS, "Invalid trust mode %u",  qos_cfg->trust_mode);
    return L7_FAILURE;
  }

  /* Validate trust_mode, and determine maximum number of rules for each trust_mode */
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

  /* Get Internal VLAN mask */
  vlan_mask = PTIN_VLAN_MASK(qos_cfg->int_vlan);
  qos_cfg->int_vlan &= vlan_mask;

  PT_LOG_TRACE(LOG_CTX_QOS,"intVLAN=%u/0x%x extVlan=%u trust_mode=%u max_rules=%u",
            qos_cfg->int_vlan, vlan_mask, qos_cfg->ext_vlan, qos_cfg->trust_mode, max_rules);

  /* Search for an extry and rule with these configurations */
  qos_entry = ptin_hapi_qos_table_search(qos_cfg, L7_NULLPTR);

  /* If trust mode was not provided, reconfigure all rules with newer port bitmap */
  if (qos_cfg->trust_mode < 0)
  {
    int           usp_port;
    L7_uint64     _usp_bmp;
    DAPI_USP_t    _usp;
    BROAD_PORT_t *_hapiPortPtr;

    /* To reconfigure an entry should be found */
    if (qos_entry == L7_NULLPTR)
    {
      PT_LOG_WARN(LOG_CTX_QOS,"Entry not found... nothing to do");
      return L7_SUCCESS;
    }

    PT_LOG_TRACE(LOG_CTX_QOS,"Going to reconfigure ports bitmap of intVLAN %u / extVlan %u / leaf:%u",
              qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side);

    /* Run all VLAN rules */
    for (rule = 0; rule < max_rules; rule++)
    {
      if (qos_entry->rule[rule].in_use && qos_entry->rule[rule].policyId_icap != BROAD_POLICY_INVALID)
      {
        hapi_ptin_usp_init(&_usp, 0, 0);

        /* Merge USP_port bitmaps */
        _usp_bmp = qos_cfg->port_bmp | qos_entry->usp_port_bmp;

        /* Iterate USP ports */
        for (usp_port = 0;
             _usp_bmp != 0;
             _usp_bmp >>= 1, usp_port++)
        {
          _usp.port = usp_port;

          /* Get port descriptor */
          _hapiPortPtr = HAPI_PORT_GET(&_usp, dapiPort->dapi_g);
          if (_hapiPortPtr == L7_NULLPTR)
          {
            PT_LOG_ERR(LOG_CTX_QOS, "usp_port %u: invalid hapiPortPtr", usp_port);
            return L7_FAILURE;
          }

          /* Port is new (add it) */
          if ((qos_cfg->port_bmp & usp_port) && !(qos_entry->usp_port_bmp & usp_port))
          {
            if (hapiBroadPolicyApplyToIface(qos_entry->rule[rule].policyId_icap, _hapiPortPtr->bcm_gport) != L7_SUCCESS)
            {
              PT_LOG_ERR(LOG_CTX_QOS, "Error adding bcm_gport 0x%x to rule %u", _hapiPortPtr->bcm_gport, rule);
              return L7_FAILURE;
            }
          }
          /* Port is not present (remove it) */
          else if (!(qos_cfg->port_bmp & usp_port) && (qos_entry->usp_port_bmp & usp_port))
          {
            if (hapiBroadPolicyRemoveFromIface(qos_entry->rule[rule].policyId_icap, _hapiPortPtr->bcm_gport) != L7_SUCCESS) 
            {
              PT_LOG_ERR(LOG_CTX_QOS, "Error removing bcm_gport 0x%x from rule %u", _hapiPortPtr->bcm_gport, rule);
              return L7_FAILURE;
            }
          }
        }
      }
    }

    /* Update bitmap of ports */
    qos_entry->usp_port_bmp = qos_cfg->port_bmp;
    BCM_PBMP_ASSIGN(qos_entry->bcm_port_bmp, pbm);

    PT_LOG_TRACE(LOG_CTX_QOS,"Ports bitmap of intVLAN %u / extVlan %u / leaf:%u updated",
                 qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side);
    return L7_SUCCESS;
  }

  /* If VLAN entry was found, check if trust_mode is conflicting. If it is, clear all rules */
  if (qos_entry != L7_NULLPTR &&
      qos_entry->entry_active &&
      qos_cfg->trust_mode != qos_entry->key.trust_mode)
  {
    /* Trust mode is different -> clear all entry */
    PT_LOG_TRACE(LOG_CTX_QOS,"Trust mode is different (old value %u). Going to clear VLAN.", qos_entry->key.trust_mode);

    /* Clear all rules */
    rc = ptin_hapi_qos_rule_free(qos_entry, -1);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error clearing VLAN entry!");
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_QOS,"intVLAN %u / extVlan %u / leaf:%u cleared.", qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side);

    /* Reuse this entry for reconfiguration */
  }

  /* Remove rules for reprocessing (only if QoS entry already exists) */
  if (qos_entry != L7_NULLPTR)
  {
    /* Search for a rule matching our configuration (and a free rule) */
    free_rule = -1;
    for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
    {
      L7_uint8 mask;

      if (!qos_entry->rule[rule].in_use)
      {
        if (free_rule < 0)  free_rule = rule;
        continue;
      }

      /* Found exact match rule: nothing to do if found */
      if (qos_cfg->priority      == qos_entry->rule[rule].priority &&
          qos_cfg->priority_mask == qos_entry->rule[rule].priority_mask &&
          qos_cfg->pbits_remark  == qos_entry->rule[rule].pbits_remark &&
          qos_cfg->int_priority  == qos_entry->rule[rule].int_priority)
      {
        PT_LOG_TRACE(LOG_CTX_QOS,"Exact matched rule %u found! Nothing to do... exit!", rule);
        return L7_SUCCESS;
      }

      /* Common mask for comparison */
      mask = qos_cfg->priority_mask & qos_entry->rule[rule].priority_mask;

      if ((qos_cfg->priority & mask) == (qos_entry->rule[rule].priority & mask))
      {
        PT_LOG_TRACE(LOG_CTX_QOS,"Going to remove rule %u: intVlan=%u/extVlan=%u/leaf:%u trust_mode=%u prio=%u/0x%x remark=%u (policyId=%d)",
                  rule, qos_entry->key.int_vlan, qos_entry->key.ext_vlan, qos_entry->key.leaf_side, qos_entry->key.trust_mode,
                  qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].pbits_remark, qos_entry->rule[rule].policyId_icap);

        /* Free rule (for later reconfiguration) */
        if (ptin_hapi_qos_rule_free(qos_entry, rule) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_QOS,"Error clearing rule %u", rule);
          return L7_SUCCESS;
        }
        PT_LOG_TRACE(LOG_CTX_QOS,"Rule removed: VLAN rules=%u  Total rules=%u",
                  qos_entry->number_of_rules, hw_rules_total);
      }
    }
  }

  /* Redundant configuration... do nothing */
  if ((qos_cfg->trust_mode == 0) ||
      ((qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P) &&
        (qos_cfg->priority_mask & 0x7) == 0x7 && qos_cfg->priority == qos_cfg->int_priority))
  {
    PT_LOG_TRACE(LOG_CTX_QOS, "Redundant rule for trust_mode %u and prio %u/0x%x! Leaving...",
              qos_cfg->trust_mode, qos_cfg->priority, qos_cfg->priority_mask);
    return L7_SUCCESS;
  }

  /* If VLAN entry don't exist, search for a free entry */
  if (qos_entry == L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_QOS,"Entry not found... searching for a free one");

    /* Search for a free entry and rule */
    qos_entry = ptin_hapi_qos_table_free_entry(&rule);

    /* If no free entry was found... return error */
    if (qos_entry == L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "No free entries!");
      return L7_TABLE_IS_FULL;
    }

    memset(qos_entry, 0x00, sizeof(ptin_hapi_qos_entry_t));
    qos_entry->entry_active   = L7_FALSE;
    qos_entry->classId        = -1;

    /* By default, free rule is the first one */
    free_rule = 0;
  }

  /* At this point, we will configure a new rule.
     If no free rule was found, leave with error */
  if ((free_rule < 0 || free_rule >= PTIN_HAPI_QOS_VLAN_ENTRIES) ||
      (hw_rules_total >= PTIN_HAPI_QOS_HW_RULES_MAX))
  {
    PT_LOG_ERR(LOG_CTX_QOS, "No free rules! (free_rule=%d, hw_rules_total=%u/%u)", free_rule, hw_rules_total, PTIN_HAPI_QOS_HW_RULES_MAX);
    return L7_TABLE_IS_FULL;
  }
  /* Selected rule */
  rule = free_rule;
  PT_LOG_TRACE(LOG_CTX_QOS,"Inside rule %u will be used.", rule);

  policyId_icap = BROAD_POLICY_INVALID;
  classId       = qos_entry->classId;

  /* If is provided an external VLAN use a class ID */
  if (qos_cfg->ext_vlan > 0 && qos_cfg->ext_vlan < 4096)
  {
    /* If there is no class id association, search for an existent one who matches */
    if (classId < 0 || classId >= MAX_CLASS_ID)
    {
      rc = ptin_hapi_classid_allocate(qos_cfg->ext_vlan, 0xfff, &classId);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_QOS,"Error allocating or reusing a class id");
        return L7_FAILURE;
      }
      /* If no matched classID was found, create a new one */
      if (classId < 0 || classId >= MAX_CLASS_ID)
      {
        PT_LOG_ERR(LOG_CTX_QOS,"Invalid class id %u", classId);
        return L7_FAILURE;
      }
      
      /* Saving data in VLAN-QoS table */
      qos_entry->classId = classId;
      classid_table[classId].number_of_links++;
    }
  }

  /* Configure ICAP rule */
  do
  {
    /* Create ICAP rule */
    rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_COSQ);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Cannot create trap policy");
      break;
    }
    /* Ingress stage */
    rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_INGRESS);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error creating an ingress policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    /* Create rule */
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error adding rule");
      hapiBroadPolicyCreateCancel();
      break;
    }

#ifndef ICAP_INTERFACES_SELECTION_BY_CLASSPORT
    /* Inports qualifier */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uchar8 *)&pbm, (L7_uchar8 *)&pbm_mask);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error adding INPORTS qualifier");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_QOS,"Inports qualifier added");
#endif

    /* src class id qualifier */
    if (classId >= 0 && classId < MAX_CLASS_ID)
    {
      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SRC_CLASS_ID, (L7_uchar8 *)&classId, exact_mask);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error adding SRC_CLASS_ID qualifier (%d)", classId);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_QOS,"SRC_CLASS_ID %d qualifier added", classId);
    }

    /* Outer VLAN qualifier */
    if (qos_cfg->int_vlan > 0 && qos_cfg->int_vlan < 4096)
    {
      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&qos_cfg->int_vlan, (L7_uint8 *) &vlan_mask);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error adding OVID qualifier (%u)", qos_cfg->int_vlan);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_QOS,"OVID %u/0x%x qualifier added", qos_cfg->int_vlan, vlan_mask);
    }

    /* Check trust mode and apply appropriated configuration */
    if (qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
    {
      if ((qos_cfg->priority_mask & 0x7) != 0)
      {
        /* Pbit */
        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OCOS, (L7_uchar8 *)&qos_cfg->priority, (L7_uchar8 *)&qos_cfg->priority_mask);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_QOS, "Error adding OCOS qualifier (%u/0x%x)", qos_cfg->priority, qos_cfg->priority_mask);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_QOS,"OCOS %u/0x%x qualifier added", qos_cfg->priority, qos_cfg->priority_mask);
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
        PT_LOG_ERR(LOG_CTX_QOS, "Error adding ETHTYPE qualifier (0x%04x)", ethertype_ipv4);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_QOS,"ETHERTYPE 0x%x qualifier added", ethertype_ipv4);

      if ((qos_cfg->priority_mask & 0x7) != 0)
      {
        /* DSCP  */
        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DSCP, (L7_uchar8 *)&dscp_value, (L7_uchar8 *)&dscp_mask);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_QOS, "Error adding DSCP qualifier (%u/0x%x)", dscp_value, dscp_mask);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_QOS,"DSCP %u/0x%x qualifier added", dscp_value, dscp_mask);
      }
    }
    else if (qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
    {
      L7_ushort16   ethertype_ipv4  = L7_ETYPE_IP;
      L7_uchar8     dscp_value, dscp_mask;

      dscp_value = (qos_cfg->priority << 2) & 0xff;
      dscp_mask  = (qos_cfg->priority_mask << 2) & 0xff;

      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ethertype_ipv4, exact_mask);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error adding ETHTYPE qualifier (0x%04x)", ethertype_ipv4);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_QOS,"ETHERTYPE 0x%x qualifier added", ethertype_ipv4);

      if ((qos_cfg->priority_mask & 0x3f) != 0)
      {
        /* DSCP  */
        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DSCP, (L7_uchar8 *)&dscp_value, (L7_uchar8 *)&dscp_mask);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_QOS, "Error adding DSCP qualifier (%u/0x%x)", dscp_value, dscp_mask);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_QOS,"DSCP %u/0x%x qualifier added", dscp_value, dscp_mask);
      }
    }

    /* Set new internal priority */
    if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, qos_cfg->int_priority, 0, 0) != L7_SUCCESS ||
        hapiBroadPolicyRuleExceedActionAdd(ruleId, BROAD_ACTION_SET_COSQ, qos_cfg->int_priority, 0, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error adding SET_COSQ action");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_QOS,"COSQ action added (%u)", qos_cfg->int_priority);

    /* Set new PCP */
    if (qos_cfg->pbits_remark)
    {
      PT_LOG_TRACE(LOG_CTX_QOS,"Remark rule added (%u)", qos_cfg->int_priority);
      if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO, qos_cfg->int_priority & 0x7, 0, 0) != L7_SUCCESS ||
          hapiBroadPolicyRuleExceedActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO, qos_cfg->int_priority & 0x7, 0, 0) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error adding SET_COSQ action");
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_QOS,"COSQ action added (%u)", qos_cfg->int_priority);
    }

    /* Apply policy */
    if ((rc=hapiBroadPolicyCommit(&policyId_icap)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error commiting trap policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_QOS, "Trap policy commited successfully (policyId_icap=%u)", policyId_icap);

#ifdef ICAP_INTERFACES_SELECTION_BY_CLASSPORT
    if (hapiBroadPolicyApplyToMultiIface(policyId_icap, pbm) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error adding port bitmap to rule %u ", rule);
      rc = L7_FAILURE;
      break;
    }
#endif
  } while (0);

  /* Have occurred any error? */
  if (rc != L7_SUCCESS)
  {
    /* Delete ICAP rule */
    if (policyId_icap != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(policyId_icap);
      PT_LOG_TRACE(LOG_CTX_QOS, "Removing policyId_icap %u", policyId_icap);
    }
    PT_LOG_ERR(LOG_CTX_QOS, "Error while creating ICAP rule");
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_QOS,"ICAP rule configured");

  /* Save entry */
  qos_entry->entry_active       = L7_TRUE;
  qos_entry->key.ext_vlan       = qos_cfg->ext_vlan;
  qos_entry->key.int_vlan       = qos_cfg->int_vlan;
  qos_entry->key.leaf_side      = qos_cfg->leaf_side;
  qos_entry->key.trust_mode     = qos_cfg->trust_mode;

  qos_entry->usp_port_bmp       = qos_cfg->port_bmp;
  BCM_PBMP_ASSIGN(qos_entry->bcm_port_bmp, pbm);

  qos_entry->rule[rule].priority      = qos_cfg->priority;
  qos_entry->rule[rule].priority_mask = qos_cfg->priority_mask;
  qos_entry->rule[rule].int_priority  = qos_cfg->int_priority;
  qos_entry->rule[rule].pbits_remark  = qos_cfg->pbits_remark;
  qos_entry->rule[rule].policyId_icap = policyId_icap;
  qos_entry->rule[rule].in_use        = L7_TRUE;

  qos_entry->number_of_rules++;
  hw_rules_total++;

  PT_LOG_TRACE(LOG_CTX_QOS,"Operation finished successfully");

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
    if ((qos_cfg->ext_vlan != qos_entry->key.ext_vlan || qos_cfg->int_vlan != qos_entry->key.int_vlan) ||
        (qos_cfg->leaf_side >= 0 && qos_cfg->leaf_side != qos_entry->key.leaf_side))
      continue;

    /* if trust_mode is not provided, clear all rules */
    if (qos_cfg->trust_mode <= 0)
    {
      if (ptin_hapi_qos_rule_free(qos_entry, -1) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_QOS,"Entry %u: Error clearing all rules", entry);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_QOS,"Entry %u: All rules cleared", entry);
      continue;
    }

    /* Run all rules belonging to this VLAN */
    for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
    {
      if (!qos_entry->rule[rule].in_use)
        continue;

      /* Common mask for comparison */
      mask = qos_cfg->priority_mask & qos_entry->rule[rule].priority_mask;

      /* Only look to trustmode, priority and mask, if trust_mode is not null */
      if (qos_cfg->trust_mode != qos_entry->key.trust_mode) 
        continue;

      /* If not similar entry, continue */
      if ((qos_cfg->priority & mask) != (qos_entry->rule[rule].priority & mask))
        continue;

      PT_LOG_TRACE(LOG_CTX_QOS,"Going to remove entry %u, rule %u: intVlan=%u/extVlan=%u/leaf:%u trust_mode=%u prio=%u/0x%x remark=%u (policyId=%d)",
                entry, rule, qos_entry->key.int_vlan, qos_entry->key.ext_vlan, qos_entry->key.leaf_side, qos_entry->key.trust_mode,
                qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].pbits_remark, qos_entry->rule[rule].policyId_icap);

      /* Clear rule */
      if (ptin_hapi_qos_rule_free(qos_entry, rule) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_QOS,"Error clearing rule %u", rule);
        continue;
      }

      PT_LOG_TRACE(LOG_CTX_QOS,"Rule removed: VLAN rules=%u  Total rules=%u",
                qos_entry->number_of_rules, hw_rules_total);
    }
  }

  return L7_SUCCESS;
}

/**
 * Shaper max rate and burst configuration
 * 
 * @author mruas (06/01/21)
 * 
 * @param dapiPort 
 * @param queueSet : l7_cosq_set_t
 * @param tc : Traffic class
 * @param rate_min 
 * @param rate_max 
 * @param burst_size 
 * 
 * @return L7_RC_t 
 */
L7_RC_t 
ptin_hapi_qos_shaper_set(ptin_dapi_port_t *dapiPort, l7_cosq_set_t queueSet, L7_int tc,
                         L7_uint32 rate_min, L7_uint32 rate_max, L7_uint32 burst_size)
{
  L7_uint32    portSpeed;
  BROAD_PORT_t *hapiPortPtr;
  bcm_error_t  rv = BCM_E_NONE;

  hapiPortPtr = HAPI_PORT_GET( dapiPort->usp, dapiPort->dapi_g );

  /* Get port speed */
  hapiBroadIntfSpeedGet(hapiPortPtr, &portSpeed);

  PT_LOG_TRACE(LOG_CTX_QOS, "usp {%d,%d,%d}, bcm_unit %u bcm_port %u, queueSet %u, tc %d: rate_min=%u kbps, rate_max=%u kbps, burst_size=%u bytes",
               dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, 
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port,
               queueSet, tc, rate_min, rate_max, burst_size);

  /* Port Level */
  if (queueSet == L7_QOS_QSET_DEFAULT ||
      queueSet == L7_QOS_QSET_PORT)
  {
    /* All traffic classes (port configuration) */
    if (tc < 0 /*All TCs*/)
    {
      rv = bcm_port_rate_egress_set(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, rate_max, burst_size);

      if (rv != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error with bcm_port_rate_egress_set(bcm_unit=%u, bcm_port=%u, rate_max=%u, burst_size=%u)=> rv=%d (%s)",
                   hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, rate_max, burst_size, rv, bcm_errmsg(rv));
        return L7_FAILURE;
      }
    }
    else
    {
      rv = bcm_cosq_port_bandwidth_set(hapiPortPtr->bcm_unit,
                                       hapiPortPtr->bcm_port,
                                       tc, 
                                       rate_min, 
                                       rate_max, 
                                       0);
      if (rv != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error with bcm_cosq_port_bandwidth_set(bcm_unit=%u, bcm_port=%u, tc=%d, rate_min=%u rate_max=%u, 0)=> rv=%d (%s)",
                   hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, tc, rate_min, rate_max, rv, bcm_errmsg(rv));
        return L7_FAILURE;
      }
    }

    PT_LOG_TRACE(LOG_CTX_QOS, "bcm_unit %u, bcm_port %u, tc=%d: Success applying shaper with rate_min=%u, rate_max=%u, burst_size=%u.",
                 hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, tc, rate_min, rate_max, burst_size);
  }
#if (PLAT_BCM_CHIP == L7_BCM_TRIDENT3_X3)
  else /* Extra queues */
  {
    bcm_gport_t qos_gport = BCM_GPORT_INVALID;

    /* Get QoS gport */
    if (ptin_hapi_qos_gport_get(dapiPort, queueSet, tc, &qos_gport) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "usp {%d,%d,%d}, queueSet %u, tc %d: Error obtaining QoS gport",
                 dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, queueSet);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_QOS, "usp {%d,%d,%d}, queueSet %u, tc %d -> Using QoS gport 0x%x",
                   dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, queueSet, qos_gport);
    }

    /* Apply shaper configuration */
    rv = bcm_cosq_gport_bandwidth_set(hapiPortPtr->bcm_unit,
                                      qos_gport,
                                      0 /*Don't care*/, 
                                      rate_min, 
                                      rate_max, 
                                      0);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error with bcm_cosq_port_bandwidth_set(bcm_unit=%u, qos_gport=0x%x, 0, rate_min=%u, rate_max=%u, 0)=> rv=%d (%s)",
                 hapiPortPtr->bcm_unit, qos_gport, rate_min, rate_max, rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }

    /* For now, burst size will only be applied to the port level */
#if 1
    if (tc < 0 /*All TCs*/)
    {
      /* Configure burst size at the psysical port side */
      rv = bcm_port_rate_egress_set(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, portSpeed, burst_size);

      if (rv != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_QOS, "Error with bcm_port_rate_egress_set(bcm_unit %u, bcm_port %u, portSpeed=%u, burst_size=%u)=> rv=%d (%s)",
                   hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, portSpeed, burst_size, rv, bcm_errmsg(rv));
        return L7_FAILURE;
      }
    }
#else
    if (rate_min != 0)
    {
      rv = bcm_cosq_control_set(hapiPortPtr->bcm_unit,
                                qos_gport,
                                0 /*Don't care*/,
                                bcmCosqControlBandwidthBurstMin,
                                burst_size);
      PT_LOG_ERR(LOG_CTX_QOS, "bcm_unit %u, qos_gport 0x%x: Error with bcm_cosq_control_set(bcmCosqControlBandwidthBurstMin)=> rv=%d (%s)",
                 hapiPortPtr->bcm_unit, qos_gport, rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }
    if (rate_max != 0)
    {
      rv = bcm_cosq_control_set(hapiPortPtr->bcm_unit,
                                qos_gport,
                                0 /*Don't care*/,
                                bcmCosqControlBandwidthBurstMax,
                                burst_size);
      PT_LOG_ERR(LOG_CTX_QOS, "bcm_unit %u, qos_gport 0x%x: Error with bcm_cosq_control_set(bcmCosqControlBandwidthBurstMax)=> rv=%d (%s)",
                 hapiPortPtr->bcm_unit, qos_gport, rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }
#endif
    PT_LOG_TRACE(LOG_CTX_QOS, "bcm_unit %u, bcm_port %u, qos_gport 0x%x: Success applying shaper with rate_min=%u, rate_max=%u, burst_size=%u.",
                 hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, qos_gport, rate_min, rate_max, burst_size);
  }
#else /*(PLAT_BCM_CHIP == L7_BCM_TRIDENT3_X3)*/
  else
  {
    PT_LOG_ERR(LOG_CTX_QOS, "Invalid queueSet %u for this Switch!", queueSet);
    return L7_FAILURE;
  }
#endif /*(PLAT_BCM_CHIP == L7_BCM_TRIDENT3_X3)*/
  
  return L7_SUCCESS;
}

/**
 * Shaper max rate and burst configuration
 * 
 * @author mruas (06/01/21)
 * 
 * @param dapiPort 
 * @param queueSet : l7_cosq_set_t
 * @param tc : Traffic class
 * @param rate_min 
 * @param rate_max 
 * @param burst_size 
 * 
 * @return L7_RC_t 
 */
L7_RC_t 
ptin_hapi_qos_shaper_get(ptin_dapi_port_t *dapiPort, l7_cosq_set_t queueSet, L7_int tc,
                         L7_uint32 *rate_min, L7_uint32 *rate_max, L7_uint32 *burst_size)
{
  int _rate_min=0, _rate_max=0, _burst_size=0, _flags=0;
  BROAD_PORT_t *hapiPortPtr;
  bcm_error_t  rv = BCM_E_NONE;

  hapiPortPtr = HAPI_PORT_GET( dapiPort->usp, dapiPort->dapi_g );

  /* All traffic classes (port configuration) */
  if (tc < 0)
  {
    rv = bcm_port_rate_egress_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, &_rate_max, &_burst_size);

    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error with bcm_port_rate_egress_get: rv=%d (%s)",
                 rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }
  }
  else
  {
    rv = bcm_cosq_port_bandwidth_get(hapiPortPtr->bcm_unit,
                                     hapiPortPtr->bcm_port,
                                     tc,
                                     &_rate_min,
                                     &_rate_max,
                                     &_flags);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_QOS, "Error with bcm_cosq_port_bandwidth_get: rv=%d (%s)",
                 rv, bcm_errmsg(rv));
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_QOS, "usp={%d,%d,%d}, bcm_unit=%u bcm_port=%u, queueSet=%u, tc=%d: rate_min=%d %rate_max=%d burst_size=%d",
               dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, 
               hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port,
               queueSet, tc, _rate_min, _rate_max, _burst_size);

  /* Return values */
  if (rate_min   != L7_NULLPTR)  *rate_min   = _rate_min;
  if (rate_max   != L7_NULLPTR)  *rate_max   = _rate_max;
  if (burst_size != L7_NULLPTR)  *burst_size = _burst_size;

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
  L7_uint entry;

  /* Run all active entries */
  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    if (!hapi_qos_table[entry].entry_active)
      continue;

    if (ptin_hapi_qos_rule_free(&hapi_qos_table[entry], -1) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_QOS,"Entry %u: Error clearing all rules", entry);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_QOS,"Entry %u: All entry rules were cleared", entry);
  }

  return L7_SUCCESS;
}

/**
 * Dump QoS VLAN information tables
 * 
 * @param void
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_vlan_dump(void)
{
  L7_uint entry, j, rule;
  ptin_hapi_qos_entry_t *qos_entry;
  ptin_hapi_classid_entry_t *classid_entry;
  BROAD_GROUP_t group_id;
  BROAD_ENTRY_t entry_id;

  printf("Dumping ClassID table...\r\n");
  for (entry = 0; entry < MAX_CLASS_ID; entry++)
  {
    classid_entry = &classid_table[entry];

    if (!classid_entry->in_use)
      continue;

    printf("ClassID %-3u: extVlan=%u mask=0x%03x   #links=%-2u",
           entry, classid_entry->vlan_value, classid_entry->vlan_mask, classid_entry->number_of_links);

    if (classid_entry->policyId_vcap != BROAD_POLICY_INVALID)
    {
      printf("  [PolicyId(VCAP)=%d ", classid_entry->policyId_vcap);

      /* FIXME: Only applied to unit 0 */
      if (l7_bcm_policy_hwInfo_get(0 /*unit*/, classid_entry->policyId_vcap, 0, &group_id, &entry_id, L7_NULLPTR, L7_NULLPTR) == L7_SUCCESS) 
      {
        printf("gid=%-2d eid=%-4d", group_id, entry_id);
      }
      else
      {
        printf("error");
      }
      printf("]");
    }
    printf("\r\n");
  }

  printf("\r\nDumping VLAN-QoS table...\r\n");

  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    qos_entry = &hapi_qos_table[entry];

    if (!qos_entry->entry_active)
      continue;

    printf("Entry %-2u: intVlan=%u extVlan=%u (classId=%d) [%s] TrustMode=%u USPbmp=0x%llx Pbmp = 0x", entry,
           qos_entry->key.int_vlan, qos_entry->key.ext_vlan, qos_entry->classId,
           ((qos_entry->key.leaf_side) ? "LEAF" : "ROOT"),
           qos_entry->key.trust_mode,
           qos_entry->usp_port_bmp);
    for (j = 0; j < _SHR_PBMP_WORD_MAX; j++)
    {
      printf("%08x ", qos_entry->bcm_port_bmp.pbits[j]);
    }
    printf("\r\n");

    for (rule = 0; rule < PTIN_HAPI_QOS_VLAN_ENTRIES; rule++)
    {
      if (!qos_entry->rule[rule].in_use)
        continue;

      printf("\t  prio=%2u/0x%02x -> intPrio=%-2u remark=%u",
             qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].int_priority, qos_entry->rule[rule].pbits_remark);

      if (qos_entry->rule[rule].policyId_icap != BROAD_POLICY_INVALID)
      {
        printf("  [PolicyId(ICAP)=%-4d ", qos_entry->rule[rule].policyId_icap);
        
        /* FIXME: Only applied to unit 0 */
        if (l7_bcm_policy_hwInfo_get(0 /*unit*/, qos_entry->rule[rule].policyId_icap, 0, &group_id, &entry_id, L7_NULLPTR, L7_NULLPTR) == L7_SUCCESS) 
        {
          printf("gid=%-2d eid=%-4d", group_id, entry_id);
        }
        else
        {
          printf("error");
        }
        printf("]");
      }
      printf("\r\n");
    }
  }
  printf("Total number of hw rules: %u\r\n", hw_rules_total);

  return L7_SUCCESS;
}















#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
/**
 * EGRESS PORT SCHEDULING HIERARCHY's GPORT TABLE (Originally 
 * for TC16SXG's BCM56370) 
 *  
 *  
 * https://jira.ptin.corppt.com/browse/OLTSWITCH-1386?focusedCommentId=1582817&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-1582817 
 * https://jira.ptin.corppt.com/browse/OLTSWITCH-1386?focusedCommentId=1583548&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-1583548 
 * https://jira.ptin.corppt.com/secure/attachment/630858/QoS_TD3X3.png 
 * https://jira.ptin.corppt.com/browse/OLTSWITCH-1386?focusedCommentId=1580853&page=com.atlassian.jira.plugin.system.issuetabpanels:comment-tabpanel#comment-1580853 
 *  
 *  
 *  
 *  
 * SOME NOTES: 
 *  
 * Relate HAPI's usp port (ranging 
 * 0-L7_MAX_PHYSICAL_PORTS_PER_UNIT) with bcm_port via
 * hapi_ptin_get_uspport_from_bcmdata() 
 *  
 *  
 * find vendor/broadcom_TC16SXG/ -name '*.[h]'|xargs grep -C 5 -n "_NODE_LVL_L0," 
 *  
 * leads to 
 * typedef enum { 
 *  SOC_HX5_NODE_LVL_ROOT = 0, 
 *  SOC_HX5_NODE_LVL_L0, 
 * [...] 
 * } soc_hx5_node_lvl_e;
 *  
 *  
 *  Table HQoS built traversing gports
 *  (bcm_cosq_gport_traverse()) with function
 *  bcm_cosq_gport_info_get()
 *  
 */




#include "soc/helix5.h"

#define N_L0s   1
#define N_L1s   2
#define N_iL2s  8
typedef struct {
    bcm_gport_t L0; //"L0.0" (SE)
    struct {
        bcm_gport_t SE;  /*FIXME: nomes p/ constantes 2 e 8 aqui presentes*/
        struct {
            bcm_gport_t SE; 
            bcm_gport_t UCq;
            bcm_gport_t MCq;
        } L2[N_iL2s];
    } L1[N_L1s];
} eg_prt_sched_hrchy_t;


static eg_prt_sched_hrchy_t HQoS[L7_MAX_PHYSICAL_PORTS_PER_UNIT];
/*usp port indexed*/


/**
 * Initialize an "eg_prt_sched_hrchy_t" struct
 * 
 * @param eg_prt_sched_hrchy_t pointer 
 * 
 * @return void 
 */
static 
void eg_prt_sched_hrchy_t_init(eg_prt_sched_hrchy_t *p) {
    int i, j;

    p->L0 = BCM_GPORT_INVALID;
    for (i=0; i<N_L1s; i++) {
        p->L1[i].SE = BCM_GPORT_INVALID;
        for (j=0; j<N_iL2s; j++) {
            p->L1[i].L2[j].SE = BCM_GPORT_INVALID;
            p->L1[i].L2[j].UCq = BCM_GPORT_INVALID;
            p->L1[i].L2[j].MCq = BCM_GPORT_INVALID;
        }
    }
}


/* 
To understand the following SE/UCQ/MCQ functions please check: 
_bcm_hx5_cosq_gport_add() 
 
hx5_gport_traverse.c CINT, available in KB0029160 "Helix5 Gport Traverse" (https://broadcomcsm.service-now.com/csm?id=kb_article&sys_id=246d1f9a1b73c850849ba82fbd4bcb74 )

AND 

(NOTE: Port 1 is "Downlnk", Port 17 "HG/Stacking". Different egress scheduling.) 
[root@TC16SXG~]# fp.shell hsp pbm=xe
Broadcom shell > hsp pbm=xe

SW Information COSQ - Unit 0
=== PORT 1
L0.0: GPORT=0x37800001 HW_INDEX=57 MODE=WRR WT=1
    L1.0: GPORT=0x37810001 HW_INDEX=82 MODE=WRR WT=1
       L2.0: GPORT=0x37830001 HW_INDEX=560 MODE=SP WT=0
          UC.0: GPORT=0x24004000 HW_INDEX=560 MODE=WRR WT=1
          MC.0: GPORT=0x30004000 HW_INDEX=560 MODE=WRR WT=1
       L2.1: GPORT=0x37840001 HW_INDEX=561 MODE=SP WT=0
          UC.1: GPORT=0x24004001 HW_INDEX=561 MODE=WRR WT=1
          MC.1: GPORT=0x30004001 HW_INDEX=561 MODE=WRR WT=1
       L2.2: GPORT=0x37850001 HW_INDEX=562 MODE=SP WT=0
          UC.2: GPORT=0x24004002 HW_INDEX=562 MODE=WRR WT=1
          MC.2: GPORT=0x30004002 HW_INDEX=562 MODE=WRR WT=1
       L2.3: GPORT=0x37860001 HW_INDEX=563 MODE=SP WT=0
          UC.3: GPORT=0x24004003 HW_INDEX=563 MODE=WRR WT=1
          MC.3: GPORT=0x30004003 HW_INDEX=563 MODE=WRR WT=1
       L2.4: GPORT=0x37870001 HW_INDEX=564 MODE=SP WT=0
          UC.4: GPORT=0x24004004 HW_INDEX=564 MODE=WRR WT=1
          MC.4: GPORT=0x30004004 HW_INDEX=564 MODE=WRR WT=1
       L2.5: GPORT=0x37880001 HW_INDEX=565 MODE=SP WT=0
          UC.5: GPORT=0x24004005 HW_INDEX=565 MODE=WRR WT=1
          MC.5: GPORT=0x30004005 HW_INDEX=565 MODE=WRR WT=1
       L2.6: GPORT=0x37890001 HW_INDEX=566 MODE=SP WT=0
          UC.6: GPORT=0x24004006 HW_INDEX=566 MODE=WRR WT=1
          MC.6: GPORT=0x30004006 HW_INDEX=566 MODE=WRR WT=1
       L2.7: GPORT=0x378a0001 HW_INDEX=567 MODE=SP WT=0
          UC.7: GPORT=0x24004007 HW_INDEX=567 MODE=WRR WT=1
          MC.7: GPORT=0x30004007 HW_INDEX=567 MODE=WRR WT=1
    L1.1: GPORT=0x37820001 HW_INDEX=83 MODE=WRR WT=1
       L2.8: GPORT=0x378b0001 HW_INDEX=568 MODE=SP WT=0
          UC.8: GPORT=0x24004008 HW_INDEX=568 MODE=WRR WT=1
          MC.8: GPORT=0x30004008 HW_INDEX=568 MODE=WRR WT=1
       L2.9: GPORT=0x378c0001 HW_INDEX=569 MODE=SP WT=0
          UC.9: GPORT=0x24004009 HW_INDEX=569 MODE=WRR WT=1
          MC.9: GPORT=0x30004009 HW_INDEX=569 MODE=WRR WT=1
       L2.10: GPORT=0x378d0001 HW_INDEX=570 MODE=SP WT=0
          UC.10: GPORT=0x2400400a HW_INDEX=570 MODE=WRR WT=1
          MC.10: GPORT=0x3000400a HW_INDEX=570 MODE=WRR WT=1
       L2.11: GPORT=0x378e0001 HW_INDEX=571 MODE=SP WT=0
          UC.11: GPORT=0x2400400b HW_INDEX=571 MODE=WRR WT=1
          MC.11: GPORT=0x3000400b HW_INDEX=571 MODE=WRR WT=1
       L2.12: GPORT=0x378f0001 HW_INDEX=572 MODE=SP WT=0
          UC.12: GPORT=0x2400400c HW_INDEX=572 MODE=WRR WT=1
          MC.12: GPORT=0x3000400c HW_INDEX=572 MODE=WRR WT=1
       L2.13: GPORT=0x37900001 HW_INDEX=573 MODE=SP WT=0
          UC.13: GPORT=0x2400400d HW_INDEX=573 MODE=WRR WT=1
          MC.13: GPORT=0x3000400d HW_INDEX=573 MODE=WRR WT=1
       L2.14: GPORT=0x37910001 HW_INDEX=574 MODE=SP WT=0
          UC.14: GPORT=0x2400400e HW_INDEX=574 MODE=WRR WT=1
          MC.14: GPORT=0x3000400e HW_INDEX=574 MODE=WRR WT=1
       L2.15: GPORT=0x37920001 HW_INDEX=575 MODE=SP WT=0
          UC.15: GPORT=0x2400400f HW_INDEX=575 MODE=WRR WT=1
          MC.15: GPORT=0x3000400f HW_INDEX=575 MODE=WRR WT=1
=========== 
[...] 


SW Information COSQ - Unit 0
=== PORT 17
L0.0: GPORT=0x37800011 HW_INDEX=36 MODE=WRR WT=1
    L1.0: GPORT=0x37830011 HW_INDEX=48 MODE=WRR WT=1
       L2.0: GPORT=0x37870011 HW_INDEX=312 MODE=SP WT=0
           UC.0: GPORT=0x24044000 HW_INDEX=312 MODE=WRR WT=1
           MC.0: GPORT=0x30044000 HW_INDEX=312 MODE=WRR WT=1
       L2.1: GPORT=0x37880011 HW_INDEX=313 MODE=SP WT=0
           UC.1: GPORT=0x24044001 HW_INDEX=313 MODE=WRR WT=1
           MC.1: GPORT=0x30044001 HW_INDEX=313 MODE=WRR WT=1
       L2.2: GPORT=0x37890011 HW_INDEX=314 MODE=SP WT=0
           UC.2: GPORT=0x24044002 HW_INDEX=314 MODE=WRR WT=1
           MC.2: GPORT=0x30044002 HW_INDEX=314 MODE=WRR WT=1
       L2.3: GPORT=0x378a0011 HW_INDEX=315 MODE=SP WT=0
           UC.3: GPORT=0x24044003 HW_INDEX=315 MODE=WRR WT=1
           MC.3: GPORT=0x30044003 HW_INDEX=315 MODE=WRR WT=1
       L2.4: GPORT=0x378b0011 HW_INDEX=316 MODE=SP WT=0
           UC.4: GPORT=0x24044004 HW_INDEX=316 MODE=WRR WT=1
           MC.4: GPORT=0x30044004 HW_INDEX=316 MODE=WRR WT=1
       L2.5: GPORT=0x378c0011 HW_INDEX=317 MODE=SP WT=0
           UC.5: GPORT=0x24044005 HW_INDEX=317 MODE=WRR WT=1
           MC.5: GPORT=0x30044005 HW_INDEX=317 MODE=WRR WT=1
       L2.6: GPORT=0x378d0011 HW_INDEX=318 MODE=SP WT=0
           UC.6: GPORT=0x24044006 HW_INDEX=318 MODE=WRR WT=1
           MC.6: GPORT=0x30044006 HW_INDEX=318 MODE=WRR WT=1
       L2.7: GPORT=0x378e0011 HW_INDEX=319 MODE=SP WT=0
           UC.7: GPORT=0x24044007 HW_INDEX=319 MODE=WRR WT=1
           MC.7: GPORT=0x30044007 HW_INDEX=319 MODE=WRR WT=1
    L1.1: GPORT=0x37840011 HW_INDEX=49 MODE=WRR WT=1
       L2.8: GPORT=0x378f0011 HW_INDEX=320 MODE=SP WT=0
           UC.8: GPORT=0x24044008 HW_INDEX=320 MODE=WRR WT=1
           MC.8: GPORT=0x30044008 HW_INDEX=320 MODE=WRR WT=1
       L2.9: GPORT=0x37900011 HW_INDEX=321 MODE=SP WT=0
           UC.9: GPORT=0x24044009 HW_INDEX=321 MODE=WRR WT=1
           MC.9: GPORT=0x30044009 HW_INDEX=321 MODE=WRR WT=1
       L2.10: GPORT=0x37910011 HW_INDEX=322 MODE=SP WT=0
           UC.10: GPORT=0x2404400a HW_INDEX=322 MODE=WRR WT=1
           MC.10: GPORT=0x3004400a HW_INDEX=322 MODE=WRR WT=1
       L2.11: GPORT=0x37920011 HW_INDEX=323 MODE=SP WT=0
           UC.11: GPORT=0x2404400b HW_INDEX=323 MODE=WRR WT=1
           MC.11: GPORT=0x3004400b HW_INDEX=323 MODE=WRR WT=1
       L2.12: GPORT=0x37930011 HW_INDEX=324 MODE=SP WT=0
           UC.12: GPORT=0x2404400c HW_INDEX=324 MODE=WRR WT=1
           MC.12: GPORT=0x3004400c HW_INDEX=324 MODE=WRR WT=1
       L2.13: GPORT=0x37940011 HW_INDEX=325 MODE=SP WT=0
           UC.13: GPORT=0x2404400d HW_INDEX=325 MODE=WRR WT=1
           MC.13: GPORT=0x3004400d HW_INDEX=325 MODE=WRR WT=1
       L2.14: GPORT=0x37950011 HW_INDEX=326 MODE=SP WT=0
           UC.14: GPORT=0x2404400e HW_INDEX=326 MODE=WRR WT=1
           MC.14: GPORT=0x3004400e HW_INDEX=326 MODE=WRR WT=1
       L2.15: GPORT=0x37960011 HW_INDEX=327 MODE=SP WT=0
           UC.15: GPORT=0x2404400f HW_INDEX=327 MODE=WRR WT=1
           MC.15: GPORT=0x3004400f HW_INDEX=327 MODE=WRR WT=1
L0.1: GPORT=0x37810011 HW_INDEX=37 MODE=WRR WT=1
    L1.2: GPORT=0x37850011 HW_INDEX=50 MODE=WRR WT=1
       L2.16: GPORT=0x37970011 HW_INDEX=328 MODE=SP WT=0
           UC.16: GPORT=0x24044010 HW_INDEX=328 MODE=WRR WT=1
           MC.16: GPORT=0x30044010 HW_INDEX=328 MODE=WRR WT=1
       L2.17: GPORT=0x37980011 HW_INDEX=329 MODE=SP WT=0
           UC.17: GPORT=0x24044011 HW_INDEX=329 MODE=WRR WT=1
           MC.17: GPORT=0x30044011 HW_INDEX=329 MODE=WRR WT=1
       L2.18: GPORT=0x37990011 HW_INDEX=330 MODE=SP WT=0
           UC.18: GPORT=0x24044012 HW_INDEX=330 MODE=WRR WT=1
           MC.18: GPORT=0x30044012 HW_INDEX=330 MODE=WRR WT=1
       L2.19: GPORT=0x379a0011 HW_INDEX=331 MODE=SP WT=0
           UC.19: GPORT=0x24044013 HW_INDEX=331 MODE=WRR WT=1
           MC.19: GPORT=0x30044013 HW_INDEX=331 MODE=WRR WT=1
       L2.20: GPORT=0x379b0011 HW_INDEX=332 MODE=SP WT=0
           UC.20: GPORT=0x24044014 HW_INDEX=332 MODE=WRR WT=1
           MC.20: GPORT=0x30044014 HW_INDEX=332 MODE=WRR WT=1
       L2.21: GPORT=0x379c0011 HW_INDEX=333 MODE=SP WT=0
           UC.21: GPORT=0x24044015 HW_INDEX=333 MODE=WRR WT=1
           MC.21: GPORT=0x30044015 HW_INDEX=333 MODE=WRR WT=1
       L2.22: GPORT=0x379d0011 HW_INDEX=334 MODE=SP WT=0
           UC.22: GPORT=0x24044016 HW_INDEX=334 MODE=WRR WT=1
           MC.22: GPORT=0x30044016 HW_INDEX=334 MODE=WRR WT=1
       L2.23: GPORT=0x379e0011 HW_INDEX=335 MODE=SP WT=0
           UC.23: GPORT=0x24044017 HW_INDEX=335 MODE=WRR WT=1
           MC.23: GPORT=0x30044017 HW_INDEX=335 MODE=WRR WT=1
L0.2: GPORT=0x37820011 HW_INDEX=38 MODE=WRR WT=1
    L1.3: GPORT=0x37860011 HW_INDEX=51 MODE=WRR WT=1
       L2.24: GPORT=0x379f0011 HW_INDEX=336 MODE=WRR WT=1
           UC.24: GPORT=0x24044018 HW_INDEX=336 MODE=WRR WT=1
           MC.24: GPORT=0x30044018 HW_INDEX=336 MODE=WRR WT=1
       L2.25: GPORT=0x37a00011 HW_INDEX=337 MODE=WRR WT=1
           UC.25: GPORT=0x24044019 HW_INDEX=337 MODE=WRR WT=1
           MC.25: GPORT=0x30044019 HW_INDEX=337 MODE=WRR WT=1
=========== 
[...] 
*/


/** 
 *  
 * To understand the following SE/UCQ/MCQ functions please check:
 * _bcm_hx5_cosq_gport_add()
 * 
 * hx5_gport_traverse.c CINT, available in KB0029160 "Helix5 Gport Traverse" (https://broadcomcsm.service-now.com/csm?id=kb_article&sys_id=246d1f9a1b73c850849ba82fbd4bcb74 )
 *
 * AND
 *
 * Broadcom shell > hsp pbm=xe
 * (Check partial output of this command above.)
 * (NOTE: Port 1 "Downlnk", Port 17 "HG/Stacking". Different egress scheduling.) 
 * 
 * 
 * Get scheduler IDs (levels 0,1,2) for the input gport and sched type
 * 
 * @param gport
 * @param dwn0_up1_st2
 * According to ToO BCM56370/56370-PG103.pdf ~pg 558, there are
 * 3 types of egress schedulers: downlink(0), uplink(1), HG/stacking(2).
 * Empirism showed just downlink and HG/stacking.
 * @param *id0, *id1, *id2
 * Output scheduling coordinates on the 3 levels 0-2.
 * Refer to "hsp pbm=xe" and above URLs.
 * 
 * @return void 
 */
static
void BCM_GPORT_SCHEDULER_2_SCHED_id(int gport,
                                    int dwn0_up1_st2,
                                    unsigned int *id0,
                                    unsigned int *id1, unsigned int *id2)
{
    unsigned int
        sched_encap,
        id,
        //"Downlink Egress port scheduling"
        rd0[SOC_HX5_NUM_SCHEDULER_PER_PORT] = {
            0,                  //L0
            0,0,                //L1.0-1
            0,0,0,0,0,0,0,0,    //L2.0-7
            0,0,0,0,0,0,0,0,    //L2.8-15
        },
        rd1[SOC_HX5_NUM_SCHEDULER_PER_PORT] = {
            -1,                 //L0
            0,1,                //L1.0-1
            0,0,0,0,0,0,0,0,    //L2.0-7
            1,1,1,1,1,1,1,1,    //L2.8-15
        },
        rd2[SOC_HX5_NUM_SCHEDULER_PER_PORT] = {
            -1,                 //L0
            -1,-1,              //L1.0-1
            0,1,2,3,4,5,6,7,    //L2.0-7
            0,1,2,3,4,5,6,7,    //L2.8-15
        },

        //"Stacking Egress port scheduling"
#define N_STCKG_L0s   3
#define N_STCKG_L1s   4
#define N_STCKG_iL2s  8
        rs0[SOC_HX5_NUM_SCHEDULER_PER_ST_PORT] = {
            0,1,2,              //L0.0-2
            0,0,1,2,            //L1.0-3
            0,0,0,0,0,0,0,0,    //L2.0-7
            0,0,0,0,0,0,0,0,    //L2.8-15
            1,1,1,1,1,1,1,1,    //L2.16-23
            2,2,                //L2.24-25
        },
        rs1[SOC_HX5_NUM_SCHEDULER_PER_ST_PORT] = {
            -1,-1,-1,           //L0.0-2
            0,1,2,3,            //L1.0-3
            0,0,0,0,0,0,0,0,    //L2.0-7
            1,1,1,1,1,1,1,1,    //L2.8-15
            2,2,2,2,2,2,2,2,    //L2.16-23
            3,3,                //L2.24-25
        },
        rs2[SOC_HX5_NUM_SCHEDULER_PER_ST_PORT] = {
            -1,-1,-1,           //L0.0-2
            -1,-1,-1,-1,        //L1.0-3
            0,1,2,3,4,5,6,7,    //L2.0-7
            0,1,2,3,4,5,6,7,    //L2.8-15
            0,1,2,3,4,5,6,7,    //L2.16-23
            0,1,                //L2.24-25
        };


    //please check _bcm_hx5_cosq_gport_add()
    sched_encap = BCM_GPORT_SCHEDULER_GET(gport);
    id = sched_encap >>16;

    if (0==dwn0_up1_st2 && id < SOC_HX5_NUM_SCHEDULER_PER_PORT) {
        if (NULL != id0) *id0 = rd0[id];
        if (NULL != id1) *id1 = rd1[id];
        if (NULL != id2) *id2 = rd2[id];
    }
    else
    if (0!=dwn0_up1_st2 && id < SOC_HX5_NUM_SCHEDULER_PER_ST_PORT) {
        if (NULL != id0) *id0 = rs0[id];
        if (NULL != id1) *id1 = rs1[id];
        if (NULL != id2) *id2 = rs2[id];
    }
    else {
        PT_LOG_CRITIC(LOG_CTX_QOS, "Wrong SE id %d gport 0x%x", id, gport);
        if (NULL != id0) *id0 = -1;
        if (NULL != id1) *id1 = -1;
        if (NULL != id2) *id2 = -1;
    }
}//BCM_GPORT_SCHEDULER_2_SCHED_id


/** 
 *  
 * To understand the following SE/UCQ/MCQ functions please check:
 * _bcm_hx5_cosq_gport_add()
 * 
 * hx5_gport_traverse.c CINT, available in KB0029160 "Helix5 Gport Traverse" (https://broadcomcsm.service-now.com/csm?id=kb_article&sys_id=246d1f9a1b73c850849ba82fbd4bcb74 )
 *
 * AND
 *
 * Broadcom shell > hsp pbm=xe
 * (Check partial output of this command above.)
 * 
 * 
 * Get input gport Unicast Queue Group scheduling coordinates (levels 0,1,2)
 * 
 * @param gport
 * @param dwn0_up1_st2
 * According to ToO BCM56370/56370-PG103.pdf ~pg 558, there are
 * 3 types of egress schedulers: downlink(0), uplink(1), HG/stacking(2).
 * Empirism showed just downlink and HG/stacking.
 * @param *id0, *id1, *id2
 * Output scheduling coordinates on the 3 levels 0-2.
 * Refer to "hsp pbm=xe" and above URLs.
 * 
 * @return void 
 */
static
void BCM_GPORT_UCAST_QUEUE_GROUP_2_Qid(int gport,
                                       int dwn0_up1_st2,
                                       unsigned int *id0,
                                       unsigned int *id1, unsigned int *id2)
{
    unsigned int id;
    //please check _bcm_hx5_cosq_gport_add()
    id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
    if ((0==dwn0_up1_st2 && id < SOC_HX5_NUM_UCAST_QUEUE_PER_PORT)
        ||
        (0!=dwn0_up1_st2 && id < SOC_HX5_NUM_UCAST_QUEUE_PER_ST_PORT))
    {
        if (NULL != id0) *id0 = id<0x10?  0: id<0x18?  1: id<0x1a?  2: -1;
        if (NULL != id1) *id1 = id/N_iL2s;
        if (NULL != id2) *id2 = id%N_iL2s;
    }
    else {
        PT_LOG_CRITIC(LOG_CTX_QOS,"Wrong UCQG id %d gport 0x%x", id, gport);
        if (NULL != id0) *id0 = -1;
        if (NULL != id1) *id1 = -1;
        if (NULL != id2) *id2 = -1;
    }
}


/** 
 *  
 * To understand the following SE/UCQ/MCQ functions please check:
 * _bcm_hx5_cosq_gport_add() 
 * 
 * hx5_gport_traverse.c CINT, available in KB0029160 "Helix5
 * Gport Traverse"
 * (https://broadcomcsm.service-now.com/csm?id=kb_article&sys_id=246d1f9a1b73c850849ba82fbd4bcb74
 * )
 *
 * AND
 *
 * Broadcom shell > hsp pbm=xe
 * (Check partial output of this command above.)
 * 
 * 
 * Get input gport Multicast Queue Group scheduling coordinates (levels 0,1,2)
 * 
 * @param gport
 * @param dwn0_up1_st2
 * According to ToO BCM56370/56370-PG103.pdf ~pg 558, there are
 * 3 types of egress schedulers: downlink(0), uplink(1), HG/stacking(2).
 * Empirism showed just downlink and HG/stacking.
 * @param *id0, *id1, *id2
 * Output scheduling coordinates on the 3 levels 0-2.
 * Refer to "hsp pbm=xe" and above URLs.
 * 
 * @return void 
 */
static
void BCM_GPORT_MCAST_QUEUE_GROUP_2_Qid(int gport,
                                       int dwn0_up1_st2,
                                       unsigned int *id0,
                                       unsigned int *id1, unsigned int *id2)
{
    unsigned int id;
    //please check _bcm_hx5_cosq_gport_add()
    id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
    if ((0==dwn0_up1_st2 && id < SOC_HX5_NUM_MCAST_QUEUE_PER_PORT)
        ||
        (0!=dwn0_up1_st2 && id < SOC_HX5_NUM_MCAST_QUEUE_PER_ST_PORT)) {
        if (NULL != id0) *id0 = id<0x10?  0: id<0x18?  1: id<0x1a?  2: -1;
        if (NULL != id1) *id1 = id/N_iL2s;
        if (NULL != id2) *id2 = id%N_iL2s;
    }
    else {
        PT_LOG_CRITIC(LOG_CTX_QOS,"Wrong MCQG id %d gport 0x%x", id, gport);
        if (NULL != id0) *id0 = -1;
        if (NULL != id1) *id1 = -1;
        if (NULL != id2) *id2 = -1;
    }
}


/*
Adapting hx5_gport_traverse_port.c to our struct 
This callback follows "bcm_cosq_gport_traverse_cb" typedef prototyping
*/
static 
int gport_callback(int unit, bcm_gport_t port, int numq, uint32 flags,
                   bcm_gport_t gport, void *user_data)
{
    int bcm_port;
    unsigned int id0, id1, id2;
    L7_uint32 usp_port;
    L7_RC_t r;
    int rv;
    bcm_cosq_gport_level_info_t info;

    bcm_port = BCM_GPORT_MODPORT_PORT_GET(port);

    r = hapi_ptin_get_uspport_from_bcmdata(bcm_unit, bcm_port, (L7_uint)-1, &usp_port);
    if (L7_SUCCESS != r) {
        //PT_LOG_ERR(LOG_CTX_QOS,
        //           "hapi_ptin_get_uspport_from_bcmdata(bcm_unit=%d, bcm_port=%d,"
        //           " ...) = %d", bcm_unit, bcm_port, r);
        switch (r)
        {
            case L7_NOT_EXIST:
                return BCM_E_NOT_FOUND;
            case L7_FAILURE:
            default:
                return BCM_E_FAIL;
        }
    }

    bcm_cosq_gport_level_info_t_init(&info);
    rv = bcm_cosq_gport_info_get(bcm_unit, gport,  &info);
    if( rv != BCM_E_NONE) {
        PT_LOG_CRITIC(LOG_CTX_QOS,
                      "bcm_cosq_gport_info_get(bcm_unit=%d, gport=0x%x,"
                      " ...) = %d", bcm_unit, gport, rv);
        return rv;
    }

    PT_LOG_TRACE(LOG_CTX_QOS, "=== USP_PORT %d  (bcm_unit %u, bcm_port %u)",
                 usp_port, bcm_unit, bcm_port);

    if (flags & BCM_COSQ_GPORT_SCHEDULER)
    {
        //if (info.parent_port_type) {
        //    //"UP-LNK"
        // 
        // EMPIRISM SHOWED HOWEVER (TC16SXG - BCM56370):
        // -just downlink+stacking ports
        // -this field (parent_port_type) identified stacking (!0) / dwnlnk (0)
        //  (instead of uplnk/dwnlnk)
        //}
        {//else {
            if(info.level == SOC_HX5_NODE_LVL_L0) {
                BCM_GPORT_SCHEDULER_2_SCHED_id(gport, info.parent_port_type?2:0,
                                               &id0, NULL, NULL);
                if (id0 >= N_L0s) {
                    PT_LOG_TRACE(LOG_CTX_QOS,
                                 "Unused L0 SE id0 %d gport 0x%x", id0, gport);
                    return BCM_E_FAIL;
                }
                HQoS[usp_port].L0 = gport;
                PT_LOG_TRACE(LOG_CTX_QOS, "L0.0: GPORT=0x%x", gport);
            }
            else
            if (info.level == SOC_HX5_NODE_LVL_L1) {
                BCM_GPORT_SCHEDULER_2_SCHED_id(gport, info.parent_port_type?2:0,
                                               &id0, &id1, NULL);
                if (id0 >= N_L0s) {
                    PT_LOG_TRACE(LOG_CTX_QOS,
                                 "Unused L0 SE id0 %d gport 0x%x", id0, gport);
                    return BCM_E_FAIL;
                }
                if (id1 >= N_L1s) {
                    PT_LOG_TRACE(LOG_CTX_QOS,
                                 "Unused L1 SE id1 %d gport 0x%x", id1, gport);
                    return BCM_E_FAIL;
                }
                HQoS[usp_port].L1[id1].SE = gport;
                PT_LOG_TRACE(LOG_CTX_QOS, "\tL1.%1.1u: GPORT=0x%x",
                             id1, gport);
            }
            else
            if (info.level == SOC_HX5_NODE_LVL_L2) {
                BCM_GPORT_SCHEDULER_2_SCHED_id(gport, info.parent_port_type?2:0,
                                               &id0, &id1, &id2);
                if (id0 >= N_L0s) {
                    PT_LOG_TRACE(LOG_CTX_QOS,
                                 "Unused L0 SE id0 %d gport 0x%x", id0, gport);
                    return BCM_E_FAIL;
                }
                if (id1 >= N_L1s) {
                    PT_LOG_TRACE(LOG_CTX_QOS,
                                 "Unused L1 SE id1 %d gport 0x%x", id1, gport);
                    return BCM_E_FAIL;
                }
                if (id2 >= N_iL2s) {
                    PT_LOG_TRACE(LOG_CTX_QOS,
                                 "Unused L2 SE id2 %d gport 0x%x", id2, gport);
                    return BCM_E_FAIL;
                }
                HQoS[usp_port].L1[id1].L2[id2].SE = gport;
                PT_LOG_TRACE(LOG_CTX_QOS, "\t\tL2.%1.1u: GPORT=0x%x",
                             id2, gport);
            }
        }
    }
    else
    if (flags & BCM_COSQ_GPORT_UCAST_QUEUE_GROUP) {
        //if (info.parent_port_type) {
        //    //"UP-LNK"
        //}
        {//else {
            BCM_GPORT_UCAST_QUEUE_GROUP_2_Qid(gport, info.parent_port_type?2:0,
                                              &id0, &id1, &id2);
            if (id0 >= N_L0s) {
                PT_LOG_TRACE(LOG_CTX_QOS,
                             "Unused UCQG id0 %d gport 0x%x", id0, gport);
                return BCM_E_FAIL;
            }
            if (id1 >= N_L1s) {
                PT_LOG_TRACE(LOG_CTX_QOS,
                             "Unused UCQG id1 %d gport 0x%x", id1, gport);
                return BCM_E_FAIL;
            }
            if (id2 >= N_iL2s) {
                PT_LOG_TRACE(LOG_CTX_QOS,
                             "Unused UCQG id2 %d gport 0x%x", id2, gport);
                return BCM_E_FAIL;
            }
            HQoS[usp_port].L1[id1].L2[id2].UCq = gport;
            PT_LOG_TRACE(LOG_CTX_QOS, "\t\t\tUC.%1.1u (%1.1u): GPORT=0x%x",
                         id2+id1*N_iL2s, id2, gport);
        }
    }
    else
    if (flags & BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
        //if (info.parent_port_type) {
        //    //"UP-LNK"
        //}
        {//else {
            BCM_GPORT_MCAST_QUEUE_GROUP_2_Qid(gport, info.parent_port_type?2:0,
                                              &id0, &id1, &id2);
            if (id0 >= N_L0s) {
                PT_LOG_TRACE(LOG_CTX_QOS,
                             "Unused MCQG id0 %d gport 0x%x", id0, gport);
                return BCM_E_FAIL;
            }
            if (id1 >= N_L1s) {
                PT_LOG_TRACE(LOG_CTX_QOS,
                             "Unused MCQG id1 %d gport 0x%x", id1, gport);
                return BCM_E_FAIL;
            }
            if (id2 >= N_iL2s) {
                PT_LOG_TRACE(LOG_CTX_QOS,
                             "Unused MCQG id2 %d gport 0x%x", id2, gport);
                return BCM_E_FAIL;
            }
            HQoS[usp_port].L1[id1].L2[id2].MCq = gport;
            PT_LOG_TRACE(LOG_CTX_QOS, "\t\t\tMC.%1.1u (%1.1u): GPORT=0x%x",
                         id2+id1*N_iL2s, id2, gport);
        }
    }

    return 0;
}//gport_callback


/**
 * Initializes and fills the Egress Port Scheduling Hierarchy 
 * table's relevant gports
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_hierarchy_table_build(void)
{
    int i, rv;
    int user_data;

    for (i=0; i<L7_MAX_PHYSICAL_PORTS_PER_UNIT; i++) {
        eg_prt_sched_hrchy_t_init(&HQoS[i]);
    }

    rv = bcm_cosq_gport_traverse(0, gport_callback, &user_data);
    if (rv != BCM_E_NONE)
    {
        PT_LOG_CRITIC(LOG_CTX_QOS, "bcm_cosq_gport_traverse()=%d", rv);
        return L7_FAILURE;
    }
    
    return L7_SUCCESS;
}
#endif //#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)


/**
 * 
 * @param usp_port 
 *  Pointer to ptin_dapi_port_t (that gives us "usp_port" and L0
 *  SE for boards other than TC16SXG
 * @param queueSet
 *  Parameter integrating scheduling coordinates L0, L1.x
 * @param tc 
 *  Traffic Class
 * @param gport (out)
 *  Pointer to function output gport we're getting
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_qos_gport_get(ptin_dapi_port_t *dapiPort,
                                int queueSet, L7_int tc,
                                bcm_gport_t *gport)
{
  L7_uint32 usp_port;
  BROAD_PORT_t *hapiPortPtr;

  hapiPortPtr = HAPI_PORT_GET( dapiPort->usp, dapiPort->dapi_g );
  usp_port = dapiPort->usp->port;

#if (PTIN_BOARD == PTIN_BOARD_TC16SXG)
    switch (queueSet)
    {
        case L7_QOS_QSET_PORT:
            *gport = HQoS[usp_port].L0;
            break;
        case L7_QOS_QSET_WIRED:
        case L7_QOS_QSET_WIRELESS:
            {
                unsigned int id1;

                id1 = (L7_QOS_QSET_WIRELESS == queueSet) ? 0: 1;
                if (tc < 0 || tc >= N_iL2s)
                {
                    *gport = HQoS[usp_port].L1[id1].SE;
                }
                else
                {
                    *gport = HQoS[usp_port].L1[id1].L2[tc].SE;
                }
            }
            break;
        default:
            *gport = BCM_GPORT_INVALID;
            PT_LOG_ERR(LOG_CTX_QOS, "Not supported queueSet %u", queueSet);
            return L7_FAILURE;
    }
#else /* Legacy boards */
    /* For Port/Default queue set, use port's gport */
    if (queueSet == L7_QOS_QSET_PORT ||
        queueSet == L7_QOS_QSET_DEFAULT)
    {
      *gport = hapiPortPtr->bcm_gport;
    }
    else
    {
      *gport = BCM_GPORT_INVALID;
      PT_LOG_ERR(LOG_CTX_QOS, "Not supported queueSet %u", queueSet);
      return L7_FAILURE;
    }
#endif

    return L7_SUCCESS;
}


/** 
 *  
 * Dumps EGRESS PORT SCHEDULING HIERARCHY's GPORT TABLE 
 * (Originally for TC16SXG's BCM56370) 
 * 
 * @param usp_port (if invalid, prints all)
 * 
 * @return void 
 */
void ptin_hapi_qos_gport_dump(L7_uint32 usp_port) {
#if (PTIN_BOARD != PTIN_BOARD_TC16SXG)
    printf("Sorry. Feature unavailable.\r\n");
    return;
#else
    extern DAPI_t *dapi_g;
    DAPI_USP_t     ddUsp;
    BROAD_PORT_t  *hapiPortPtr;
    L7_uint32      usp_port_i, usp_port_f;

    /* Check if dapi_g is valid */
    if (L7_NULLPTR == dapi_g)
    {
      printf("dapi_g pointer is NULL\r\n");
      return;
    }

    if (usp_port < L7_MAX_PHYSICAL_PORTS_PER_SLOT) {
        usp_port_i=usp_port;
        usp_port_f=usp_port_i+1;
    }
    else {
        usp_port_i=0;
        usp_port_f=L7_MAX_PHYSICAL_PORTS_PER_SLOT;
    }
    
    for (usp_port=usp_port_i; usp_port<usp_port_f; usp_port++) {
        unsigned int id2, id1;
        /* Initialize USP */
        hapi_ptin_usp_init(&ddUsp, 0, usp_port);
        /* Get hapiPortPtr */
        hapiPortPtr = HAPI_PORT_GET(&ddUsp, dapi_g);
        /* Check if hapiPortPtr pointer is valid */
        if (hapiPortPtr == L7_NULLPTR)
        {
          printf("usp_port %u: hapiPortPtr pointer is NULL\r\n", usp_port);
          continue;
        }

        printf("=== USP_PORT %u (modid %u, unit %u, port %u, gport 0x%x)\r\n",
               usp_port, hapiPortPtr->bcm_modid, hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, hapiPortPtr->bcm_gport);
        printf("L0.0: GPORT=0x%x\r\n", HQoS[usp_port].L0);
        for (id1=0; id1<N_L1s; id1++) {
            printf("\tL1.%u: GPORT=0x%x\r\n", id1, HQoS[usp_port].L1[id1].SE);
            for (id2=0; id2<N_iL2s; id2++) {
                printf("\t\tL2.%u: GPORT=0x%x\r\n",
                       id2, HQoS[usp_port].L1[id1].L2[id2].SE);
                printf("\t\t\tUC.%u: GPORT=0x%x\r\n",
                       id2, HQoS[usp_port].L1[id1].L2[id2].UCq);
                printf("\t\t\tMC.%u: GPORT=0x%x\r\n",
                       id2, HQoS[usp_port].L1[id1].L2[id2].MCq);
            }
        }
    }

#endif
}//ptin_hapi_qos_gport_dump

