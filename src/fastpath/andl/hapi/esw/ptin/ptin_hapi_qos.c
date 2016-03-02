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

#include "broad_policy.h"
#include "broad_group_bcm.h"

#define PTIN_HAPI_QOS_TABLE_SIZE    20
#define PTIN_HAPI_QOS_VLAN_ENTRIES  32
#define PTIN_HAPI_QOS_HW_RULES_MAX  100

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

  bcm_pbmp_t      port_bmp;
  L7_int32        classId;

  L7_uint8 number_of_rules;
  ptin_hapi_qos_rule_t rule[PTIN_HAPI_QOS_VLAN_ENTRIES];

} ptin_hapi_qos_entry_t;

L7_uint16 hw_rules_total = 0;
ptin_hapi_qos_entry_t hapi_qos_table[PTIN_HAPI_QOS_TABLE_SIZE];


#define MAX_CLASS_ID  1024

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
      PT_LOG_ERR(LOG_CTX_HAPI, "Error converting port bitmap to pbmp format");;
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
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }

        /* Add this physical port to bitmap */
        BCM_PBMP_PORT_ADD(*pbm, hapiPortPtr_member->bcm_port);
        PT_LOG_TRACE(LOG_CTX_HAPI,"bcm_port %d added", hapiPortPtr_member->bcm_port);
      }
    }
    /* Extract Physical port */
    else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      BCM_PBMP_PORT_ADD(*pbm, hapiPortPtr->bcm_port);
      PT_LOG_TRACE(LOG_CTX_HAPI,"bcm_port %d considered", hapiPortPtr->bcm_port);
    }
    /* Not valid type */
    else
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Interface has a not valid type: error!");
      return L7_FAILURE;
    }
  }

  /* PBM mask: all ports */
  BCM_PBMP_CLEAR(*pbm_mask);
  if (hapi_ptin_bcmPbmPort_get((L7_uint64)-1, pbm_mask) != L7_SUCCESS) 
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error converting port bitmap to pbmp format");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

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
    PT_LOG_TRACE(LOG_CTX_HAPI, "Class ID %d already exists", classId);
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
    PT_LOG_CRITIC(LOG_CTX_HAPI, "There are no free Class IDs available! rc=%d", rc);
    return L7_TABLE_IS_FULL;
  }

  /* Validate extracted ClassID */
  if (classId_pool_entry->classId < 0 || classId_pool_entry->classId >= MAX_CLASS_ID)
  {
    PT_LOG_CRITIC(LOG_CTX_HAPI, "Invalid extracted Class ID: %d", classId_pool_entry->classId);
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

    PT_LOG_WARN(LOG_CTX_HAPI, "Removed policyId %u belonging to classId %u",
                classid_table[classId].policyId_vcap, classId);
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Extracted Class ID %d", classId);

  /* Configure VCAP rule */
  do
  {
    /* Create rule */
    rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_VLAN);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Cannot create trap policy");
      break;
    }
    /* Ingress stage */
    rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error creating a lookup policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    /* Create rule */
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding rule");
      hapiBroadPolicyCreateCancel();
      break;
    }
    /* Outer VLAN */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&ext_vlan, exact_mask);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding OVID qualifier (%u)", ext_vlan);
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"OVID %u qualifier added", ext_vlan);

    /* Set src class id */
    if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_SRC_CLASS_ID, (L7_uint32) classId, 0, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding SET_SRC_CLASS_ID action");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"SET_SRC_CLASS_ID action added (%d)", classId);

    /* Apply policy */
    if ((rc=hapiBroadPolicyCommit(&policyId_vcap)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error commiting trap policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI, "Trap policy commited successfully (policyId_vcap=%u)",policyId_vcap);
  }
  while (0);

  /* Have occurred any error? */
  if (rc != L7_SUCCESS)
  {
    /* Delete VCAP rule */
    if (policyId_vcap != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(policyId_vcap);
      PT_LOG_TRACE(LOG_CTX_HAPI, "Removing policyId_vcap %u", policyId_vcap);
    }

    /* Restore ClassID */
    dl_queue_add_head(&queue_free_classIds, (dl_queue_elem_t*) &classId_pool[classId]);
    PT_LOG_TRACE(LOG_CTX_HAPI, "Adding again classId %u to free queue", classId);

    PT_LOG_ERR(LOG_CTX_HAPI, "Error while creating VCAP rule");
    return rc;
  }

  /* Save classid information */
  classid_table[classId].in_use     = L7_TRUE;
  classid_table[classId].vlan_value = ext_vlan;
  classid_table[classId].vlan_mask  = 0xfff;
  classid_table[classId].policyId_vcap = policyId_vcap;

  PT_LOG_TRACE(LOG_CTX_HAPI,"VCAP rule configured with classId %d", classId);

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
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid class id %u", classId);
    return L7_FAILURE;
  }

  /* Check if class id is active */
  if (!classid_table[classId].in_use)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Class id %u is not used", classId);
    return L7_SUCCESS;
  }

  #if 0
  /* One less link */
  if (classid_table[classId].number_of_links > 0)
  {
    classid_table[classId].number_of_links--;
    PT_LOG_TRACE(LOG_CTX_HAPI,"Now with %u links", classid_table[classId].number_of_links); 
  }

  /* Remove class id, if it has no links */
  if (classid_table[classId].number_of_links == 0)
  {
  #endif
    /* Delete VCAP rule */
    if (classid_table[classId].policyId_vcap != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(classid_table[classId].policyId_vcap);
      PT_LOG_TRACE(LOG_CTX_HAPI,"Policy %u destroyed", classid_table[classId].policyId_vcap);
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
    PT_LOG_TRACE(LOG_CTX_HAPI,"We still have %u links... nothing to do", classid_table[classId].number_of_links);
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
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid arguments");
    return L7_FAILURE;
  }

  if (!qos_entry->entry_active)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Entry not active... nothing to do");
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
      PT_LOG_TRACE(LOG_CTX_HAPI,"Policy %u destroyed (prio=%u/0x%x)",
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
        PT_LOG_TRACE(LOG_CTX_HAPI,"Now with %u links", classid_table[classId].number_of_links); 
      }

      /* Remove class id, if it has no links */
      if (classid_table[classId].number_of_links == 0)
      {
        if (ptin_hapi_classid_free(classId) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_HAPI,"Error freeing class id %u", classId);
          return L7_FAILURE;
        }
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_HAPI,"We still have %u links... nothing to do", classid_table[classId].number_of_links);
      }
    }

    /* Clear entry */
    memset(qos_entry, 0x00, sizeof(ptin_hapi_qos_entry_t));
    qos_entry->entry_active   = L7_FALSE;
    qos_entry->classId        = -1;
    PT_LOG_TRACE(LOG_CTX_HAPI,"Entry fully cleared!");
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
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid arguments!");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  if (hapiPortPtr == L7_NULLPTR || dapiPortPtr == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface!");
    return L7_FAILURE;
  }

  /* Port is physical? */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    rv = bcm_port_control_set(0, hapiPortPtr->bcm_port, bcmPortControlEgressVlanPriUsesPktPri, !enable);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting bcmPortControlEgressVlanPriUsesPktPri in port {%d,%d,%d} to %u (rv=%d)",
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, !enable, rv);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_HAPI, "bcmPortControlEgressVlanPriUsesPktPri of port {%d,%d,%d} set to %u",
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
        PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
        return L7_FAILURE;
      }
      /* Get enable status for member port */
      rv = bcm_port_control_set(0, hapiPortPtr_member->bcm_port, bcmPortControlEgressVlanPriUsesPktPri, !enable);
      if (rv != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error setting bcmPortControlEgressVlanPriUsesPktPri in port {%d,%d,%d} to %u (rv=%d)",
                dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.port,
                !enable, rv);
        return L7_FAILURE;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_HAPI, "bcmPortControlEgressVlanPriUsesPktPri of port {%d,%d,%d} set to %u",
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

  PT_LOG_TRACE(LOG_CTX_HAPI,"Going to clear all intVLAN %u / extVlan %u rules", int_vlan, ext_vlan);

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
  L7_int      /*entry, free_entry,*/ rule, free_rule, max_rules;
  L7_uint16   vlan_mask = 0xfff;
  bcm_pbmp_t  pbm, pbm_mask;
  L7_int32    classId;
  ptin_hapi_qos_entry_t *qos_entry;
  L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  BROAD_POLICY_t      policyId_icap;
  BROAD_POLICY_RULE_t ruleId;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_HAPI, "intVLAN %u, extVlan %u, leaf:%u, port_pbm=0x%llx, trust_mode=%u, remark=%u, prio=%u/0x%x -> CoS=%u",
            qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side,
            qos_cfg->ptin_port_bmp, qos_cfg->trust_mode, qos_cfg->pbits_remark,
            qos_cfg->priority, qos_cfg->priority_mask, qos_cfg->int_priority);

  /* Get pbm format of ports */
  if (ptin_hapi_port_bitmap_get(dapiPort, qos_cfg->ptin_port_bmp, &pbm, &pbm_mask) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error converting port bitmap to pbmp format");
    return L7_FAILURE;
  }

  /* Validate trust mode */
  if (qos_cfg->trust_mode > L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid trust mode %u",  qos_cfg->trust_mode);
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

  PT_LOG_TRACE(LOG_CTX_HAPI,"intVLAN=%u/0x%x extVlan=%u trust_mode=%u max_rules=%u",
            qos_cfg->int_vlan, vlan_mask, qos_cfg->ext_vlan, qos_cfg->trust_mode, max_rules);

  /* Search for an extry and rule with these configurations */
  qos_entry = ptin_hapi_qos_table_search(qos_cfg, L7_NULLPTR);

  /* If trust mode was not provided, reconfigure all rules with newer port bitmap */
  if (qos_cfg->trust_mode < 0)
  {
    bcm_port_t    bcm_port;
    bcmx_lport_t  bcmx_lport;
    bcm_pbmp_t    pbmp_result;

    /* To reconfigure an entry should be found */
    if (qos_entry == L7_NULLPTR)
    {
      PT_LOG_WARN(LOG_CTX_HAPI,"Entry not found... nothing to do");
      return L7_SUCCESS;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI,"Going to reconfigure ports bitmap of intVLAN %u / extVlan %u / leaf:%u",
              qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side);

    /* Run all VLAN rules */
    for (rule = 0; rule < max_rules; rule++)
    {
      if (qos_entry->rule[rule].in_use && qos_entry->rule[rule].policyId_icap != BROAD_POLICY_INVALID)
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
            if (hapiBroadPolicyApplyToIface(qos_entry->rule[rule].policyId_icap, bcmx_lport) != L7_SUCCESS) 
            {
              PT_LOG_ERR(LOG_CTX_HAPI, "Error adding bcm_port %u to rule %u", bcm_port, rule);
              return L7_FAILURE;
            }
          }
          else if (!BCM_PBMP_MEMBER(pbm, bcm_port) && BCM_PBMP_MEMBER(qos_entry->port_bmp, bcm_port))
          {
            if (hapiBroadPolicyRemoveFromIface(qos_entry->rule[rule].policyId_icap, bcmx_lport) != L7_SUCCESS) 
            {
              PT_LOG_ERR(LOG_CTX_HAPI, "Error removing bcm_port %u from rule %u", bcm_port, rule);
              return L7_FAILURE;
            }
          }
        }
      }
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Ports bitmap of intVLAN %u / extVlan %u / leaf:%u updated",
              qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side);
    return L7_SUCCESS;
  }

  /* If VLAN entry was found, check if trust_mode is conflicting. If it is, clear all rules */
  if (qos_entry != L7_NULLPTR &&
      qos_entry->entry_active &&
      qos_cfg->trust_mode != qos_entry->key.trust_mode)
  {
    /* Trust mode is different -> clear all entry */
    PT_LOG_TRACE(LOG_CTX_HAPI,"Trust mode is different (old value %u). Going to clear VLAN.", qos_entry->key.trust_mode);

    /* Clear all rules */
    rc = ptin_hapi_qos_rule_free(qos_entry, -1);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error clearing VLAN entry!");
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"intVLAN %u / extVlan %u / leaf:%u cleared.", qos_cfg->int_vlan, qos_cfg->ext_vlan, qos_cfg->leaf_side);

    /* Reuse this entry for reconfiguration */
  }

  /* If VLAN entry not found, use the searched free entry */
  if (qos_entry == L7_NULLPTR)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"Entry not found... searching for a free one");

    /* Search for a free entry and rule */
    qos_entry = ptin_hapi_qos_table_free_entry(&rule);

    /* If no free entry was found... return error */
    if (qos_entry == L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "No free entries!");
      return L7_TABLE_IS_FULL;
    }

    memset(qos_entry, 0x00, sizeof(ptin_hapi_qos_entry_t));
    qos_entry->entry_active   = L7_FALSE;
    qos_entry->classId        = -1;
  }

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
      PT_LOG_TRACE(LOG_CTX_HAPI,"Exact matched rule %u found! Nothing to do... exit!", rule);
      return L7_SUCCESS;
    }

    /* Common mask for comparison */
    mask = qos_cfg->priority_mask & qos_entry->rule[rule].priority_mask;

    if ((qos_cfg->priority & mask) == (qos_entry->rule[rule].priority & mask))
    {
      PT_LOG_TRACE(LOG_CTX_HAPI,"Going to remove rule %u: intVlan=%u/extVlan=%u/leaf:%u trust_mode=%u prio=%u/0x%x remark=%u (policyId=%d)",
                rule, qos_entry->key.int_vlan, qos_entry->key.ext_vlan, qos_entry->key.leaf_side, qos_entry->key.trust_mode,
                qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].pbits_remark, qos_entry->rule[rule].policyId_icap);

      /* Free rule (for later reconfiguration) */
      if (ptin_hapi_qos_rule_free(qos_entry, rule) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error clearing rule %u", rule);
        return L7_SUCCESS;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"Rule removed: VLAN rules=%u  Total rules=%u",
                qos_entry->number_of_rules, hw_rules_total);
    }
  }

  /* Redundant configuration... do nothing */
  if ((qos_cfg->trust_mode == 0) ||
      ((qos_cfg->trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P) &&
        (qos_cfg->priority_mask & 0x7) == 0x7 && qos_cfg->priority == qos_cfg->int_priority))
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "Redundant rule for trust_mode %u and prio %u/0x%x! Leaving...",
              qos_cfg->trust_mode, qos_cfg->priority, qos_cfg->priority_mask);
    return L7_SUCCESS;
  }

  /* At this point, we will configure a new rule.
     If no free rule was found, leave with error */
  if ((free_rule < 0 || free_rule >= PTIN_HAPI_QOS_VLAN_ENTRIES) ||
      (hw_rules_total >= PTIN_HAPI_QOS_HW_RULES_MAX))
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "No free rules! (free_rule=%d, hw_rules_total=%u/%u)", free_rule, hw_rules_total, PTIN_HAPI_QOS_HW_RULES_MAX);
    return L7_TABLE_IS_FULL;
  }
  /* Selected rule */
  rule = free_rule;
  PT_LOG_TRACE(LOG_CTX_HAPI,"Inside rule %u will be used.", rule);

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
        PT_LOG_ERR(LOG_CTX_HAPI,"Error allocating or reusing a class id");
        return L7_FAILURE;
      }
      /* If no matched classID was found, create a new one */
      if (classId < 0 || classId >= MAX_CLASS_ID)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Invalid class id %u", classId);
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
    rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM /*BROAD_POLICY_TYPE_COSQ*/);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Cannot create trap policy");
      break;
    }
    /* Ingress stage */
    rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_INGRESS);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error creating an ingress policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    /* Create rule */
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding rule");
      hapiBroadPolicyCreateCancel();
      break;
    }
    /* Inports qualifier */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uchar8 *)&pbm, (L7_uchar8 *)&pbm_mask);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding INPORTS qualifier");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Inports qualifier added");

    /* src class id qualifier */
    if (classId >= 0 && classId < MAX_CLASS_ID)
    {
      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SRC_CLASS_ID, (L7_uchar8 *)&classId, exact_mask);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error adding SRC_CLASS_ID qualifier (%d)", classId);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"SRC_CLASS_ID %d qualifier added", classId);
    }

    /* Outer VLAN qualifier */
    if (qos_cfg->int_vlan > 0 && qos_cfg->int_vlan < 4096)
    {
      rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&qos_cfg->int_vlan, (L7_uint8 *) &vlan_mask);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error adding OVID qualifier (%u)", qos_cfg->int_vlan);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"OVID %u/0x%x qualifier added", qos_cfg->int_vlan, vlan_mask);
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
          PT_LOG_ERR(LOG_CTX_HAPI, "Error adding OCOS qualifier (%u/0x%x)", qos_cfg->priority, qos_cfg->priority_mask);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_HAPI,"OCOS %u/0x%x qualifier added", qos_cfg->priority, qos_cfg->priority_mask);
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
        PT_LOG_ERR(LOG_CTX_HAPI, "Error adding ETHTYPE qualifier (0x%04x)", ethertype_ipv4);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"ETHERTYPE 0x%x qualifier added", ethertype_ipv4);

      if ((qos_cfg->priority_mask & 0x7) != 0)
      {
        /* DSCP  */
        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DSCP, (L7_uchar8 *)&dscp_value, (L7_uchar8 *)&dscp_mask);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error adding DSCP qualifier (%u/0x%x)", dscp_value, dscp_mask);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_HAPI,"DSCP %u/0x%x qualifier added", dscp_value, dscp_mask);
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
        PT_LOG_ERR(LOG_CTX_HAPI, "Error adding ETHTYPE qualifier (0x%04x)", ethertype_ipv4);
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"ETHERTYPE 0x%x qualifier added", ethertype_ipv4);

      if ((qos_cfg->priority_mask & 0x3f) != 0)
      {
        /* DSCP  */
        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DSCP, (L7_uchar8 *)&dscp_value, (L7_uchar8 *)&dscp_mask);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error adding DSCP qualifier (%u/0x%x)", dscp_value, dscp_mask);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_HAPI,"DSCP %u/0x%x qualifier added", dscp_value, dscp_mask);
      }
    }

    /* Set new internal priority */
    if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, qos_cfg->int_priority, 0, 0) != L7_SUCCESS ||
        hapiBroadPolicyRuleExceedActionAdd(ruleId, BROAD_ACTION_SET_COSQ, qos_cfg->int_priority, 0, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding SET_COSQ action");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"COSQ action added (%u)", qos_cfg->int_priority);

    /* Set new PCP */
    if (qos_cfg->pbits_remark)
    {
      if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO, qos_cfg->int_priority & 0x7, 0, 0) != L7_SUCCESS ||
          hapiBroadPolicyRuleExceedActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO, qos_cfg->int_priority & 0x7, 0, 0) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error adding SET_COSQ action");
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"COSQ action added (%u)", qos_cfg->int_priority);
    }

    /* Apply policy */
    if ((rc=hapiBroadPolicyCommit(&policyId_icap)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error commiting trap policy");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI, "Trap policy commited successfully (policyId_icap=%u)", policyId_icap);
  } while (0);

  /* Have occurred any error? */
  if (rc != L7_SUCCESS)
  {
    /* Delete ICAP rule */
    if (policyId_icap != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(policyId_icap);
      PT_LOG_TRACE(LOG_CTX_HAPI, "Removing policyId_icap %u", policyId_icap);
    }
    PT_LOG_ERR(LOG_CTX_HAPI, "Error while creating ICAP rule");
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"ICAP rule configured");

  /* Save entry */
  qos_entry->entry_active       = L7_TRUE;
  qos_entry->key.ext_vlan       = qos_cfg->ext_vlan;
  qos_entry->key.int_vlan       = qos_cfg->int_vlan;
  qos_entry->key.leaf_side      = qos_cfg->leaf_side;
  qos_entry->key.trust_mode     = qos_cfg->trust_mode;
  BCM_PBMP_ASSIGN(qos_entry->port_bmp, pbm);

  qos_entry->rule[rule].priority      = qos_cfg->priority;
  qos_entry->rule[rule].priority_mask = qos_cfg->priority_mask;
  qos_entry->rule[rule].int_priority  = qos_cfg->int_priority;
  qos_entry->rule[rule].pbits_remark  = qos_cfg->pbits_remark;
  qos_entry->rule[rule].policyId_icap = policyId_icap;
  qos_entry->rule[rule].in_use        = L7_TRUE;

  qos_entry->number_of_rules++;
  hw_rules_total++;

  PT_LOG_TRACE(LOG_CTX_HAPI,"Operation finished successfully");

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
        PT_LOG_ERR(LOG_CTX_HAPI,"Entry %u: Error clearing all rules", entry);
        return L7_FAILURE;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"Entry %u: All rules cleared", entry);
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

      PT_LOG_TRACE(LOG_CTX_HAPI,"Going to remove entry %u, rule %u: intVlan=%u/extVlan=%u/leaf:%u trust_mode=%u prio=%u/0x%x remark=%u (policyId=%d)",
                entry, rule, qos_entry->key.int_vlan, qos_entry->key.ext_vlan, qos_entry->key.leaf_side, qos_entry->key.trust_mode,
                qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].pbits_remark, qos_entry->rule[rule].policyId_icap);

      /* Clear rule */
      if (ptin_hapi_qos_rule_free(qos_entry, rule) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error clearing rule %u", rule);
        continue;
      }

      PT_LOG_TRACE(LOG_CTX_HAPI,"Rule removed: VLAN rules=%u  Total rules=%u",
                qos_entry->number_of_rules, hw_rules_total);
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
  L7_uint entry;

  /* Run all active entries */
  for (entry = 0; entry < PTIN_HAPI_QOS_TABLE_SIZE; entry++)
  {
    if (!hapi_qos_table[entry].entry_active)
      continue;

    if (ptin_hapi_qos_rule_free(&hapi_qos_table[entry], -1) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Entry %u: Error clearing all rules", entry);
      return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Entry %u: All entry rules were cleared", entry);
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

      if (l7_bcm_policy_hwInfo_get(0, classid_entry->policyId_vcap, 0, &group_id, &entry_id, L7_NULLPTR, L7_NULLPTR) == L7_SUCCESS) 
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

    printf("Entry %-2u: intVlan=%u extVlan=%u (classId=%d) [%s] TrustMode=%u Pbmp = 0x", entry,
           qos_entry->key.int_vlan, qos_entry->key.ext_vlan, qos_entry->classId,
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

      printf("\t  prio=%2u/0x%02x -> intPrio=%-2u remark=%u",
             qos_entry->rule[rule].priority, qos_entry->rule[rule].priority_mask, qos_entry->rule[rule].int_priority, qos_entry->rule[rule].pbits_remark);

      if (qos_entry->rule[rule].policyId_icap != BROAD_POLICY_INVALID)
      {
        printf("  [PolicyId(ICAP)=%-4d ", qos_entry->rule[rule].policyId_icap);
         
        if (l7_bcm_policy_hwInfo_get(0, qos_entry->rule[rule].policyId_icap, 0, &group_id, &entry_id, L7_NULLPTR, L7_NULLPTR) == L7_SUCCESS) 
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

