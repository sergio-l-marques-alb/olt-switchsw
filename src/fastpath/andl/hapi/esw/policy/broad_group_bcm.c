/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_group_bcm.c
*
* This file implements the custom bcm layer that runs on all units. 
*
* @component hapi
*
* @create    3/18/2005
*
* @author    robp
*
* @end
*
**********************************************************************/
#include "flex.h"
#include "broad_group_bcm.h"
#include "broad_group_xgs3.h"
#include "broad_group_shuffle.h"
#include "ibde.h"
#include "bcm/custom.h"

#include "l7_usl_policy_db.h"
#include "l7_usl_common.h"

/* PTin added: includes */
#include "logger.h"

/* Policy Group Map Table Definitions */


static policy_map_table_t *policy_map_table[SOC_MAX_NUM_DEVICES] = {0};     /* This structure only holds data for the policies local to a BCM unit. */
static L7_short16         *policy_map_index_map[SOC_MAX_NUM_DEVICES] = {0}; /* This structure maps global policy IDs to an index into policy_map_table. */

void *_policy_sem = L7_NULL;

#define CHECK_POLICY(p)  {if (p >= BROAD_MAX_POLICIES)  L7_LOG_ERROR(p);}
#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) L7_LOG_ERROR(u);}

/* For outbound policies only.*/
typedef struct
{
    bcm_pbmp_t            unitEpbm; /* Egress mask for the entire unit. */
    BROAD_ENTRY_t         entry;    /* Entry for the unit default rule */
} policy_efp_on_ifp_table_t;

static BROAD_POLICY_RULE_ENTRY_t *policy_rule_ptr_get(BROAD_POLICY_ENTRY_t *policyInfo, L7_uint32 ruleIdx)
{
  L7_uint32 i;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;

  rulePtr = policyInfo->ruleInfo;
  i = 0;
  while (rulePtr != L7_NULL)
  {
    if (i == ruleIdx)
    {
      break;
    }

    i++;
    rulePtr = rulePtr->next;
  }

  return rulePtr;
}

static policy_efp_on_ifp_table_t policy_efp_on_ifp_table[SOC_MAX_NUM_DEVICES];

typedef struct
{
  bcm_pbmp_t pbm;
} port_class_t;

static port_class_t *port_class_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT] = {{0}};

static L7_uint32 _policy_max_port_classes(L7_uint32 unit)
{
  L7_uint32 max_port_classes = EFP_STD_CLASS_ID_MAX;

  /* Return the maximum number of port classes supported for this unit. 
     The max number is the number of bits in the port class field in HW. 
     Most devices support 8 bits, FB2 supports 4. */
  if (SOC_IS_FIREBOLT2(unit))
  {
    max_port_classes = 4;
  }

  return max_port_classes;
}

static int _policy_alloc_portclass(int unit, BROAD_POLICY_t policy)
{
    int                  i;
    policy_map_table_t  *policyPtr;
    L7_short16           policyIdx;
    L7_short16           tempPolicyIdx;
    policy_map_table_t  *tempPolicyPtr;
    int                  numPorts;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    policyIdx = policy_map_index_map[unit][policy];

    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      return BCM_E_NOT_FOUND;
    }

    policyPtr = &policy_map_table[unit][policyIdx];

    /* Don't allocate a portClass if there aren't at least two ports in the bitmap. */
    BCM_PBMP_COUNT(policyPtr->pbm, numPorts);
    if (numPorts < 2)
    {
      policyPtr->portClass = BROAD_INVALID_PORT_CLASS;
      return BCM_E_NONE;
    }

    /* Try to find a port class that has the same bitmap as the policy. */
    for (i = 0; i < _policy_max_port_classes(unit); i++)
    {
      if (BCM_PBMP_EQ(policyPtr->pbm, port_class_table[unit][policyPtr->policyStage][i].pbm))
      {
        policyPtr->portClass = i;

        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Reusing portClass %u\n",__FUNCTION__,__LINE__,policyPtr->portClass);

        return BCM_E_NONE;
      }
    }

    /* At this point, no portclass bitmaps equaled the policy's bitmap. 
       Next, try to find an available portclass that can be used by this
       policy and assign the policy's bitmap to that portclass. */
    for (i = 0; i < _policy_max_port_classes(unit); i++)
    {
      for (tempPolicyIdx = 0; tempPolicyIdx < BROAD_MAX_POLICIES_PER_BCM_UNIT; tempPolicyIdx++)
      {
        tempPolicyPtr = &policy_map_table[unit][tempPolicyIdx];
        if ((tempPolicyPtr != policyPtr) && 
            (tempPolicyPtr->flags & GROUP_MAP_USED) && 
            (tempPolicyPtr->policyStage == policyPtr->policyStage) &&
            (tempPolicyPtr->portClass == i))
        {
          /* This policy is using the same portClass, so try the next portClass */
          break;
        }
      }
      /* If no other policies are using this portClass, then choose this portClass */
      if (tempPolicyIdx == BROAD_MAX_POLICIES_PER_BCM_UNIT)
      {
        policyPtr->portClass = i;
        BCM_PBMP_ASSIGN(port_class_table[unit][policyPtr->policyStage][i].pbm, policyPtr->pbm);

        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("%s(%d) Using new portClass %u\n",__FUNCTION__,__LINE__,policyPtr->portClass);

        return BCM_E_NONE;
      }
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("%s(%d) Portclasses not available!\n",__FUNCTION__,__LINE__);

    return BCM_E_RESOURCE;
}

static void _policy_sem_take()
{
  if (_policy_sem == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  osapiSemaTake(_policy_sem, L7_WAIT_FOREVER);
}

static void _policy_sem_give()
{
  if (_policy_sem == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  osapiSemaGive(_policy_sem);
}

/* BCM Functions */

static void _policy_group_set_default_pbm(int unit, BROAD_POLICY_TYPE_t type, BROAD_POLICY_STAGE_t stage, policy_map_table_t *policyPtr)
{
    soc_pbmp_t tempPbm;

    SOC_PBMP_CLEAR(tempPbm);

    switch (type)
    {
    case BROAD_POLICY_TYPE_VLAN:
        if (stage == BROAD_POLICY_STAGE_INGRESS)
        {
          SOC_PBMP_ASSIGN(policyPtr->pbm, PBMP_E_ALL(unit));
        }
        else
        {
          BCM_PBMP_CLEAR(policyPtr->pbm);
        }
        break;
    case BROAD_POLICY_TYPE_SYSTEM:
    case BROAD_POLICY_TYPE_PTIN:        /* PTin added: policer */
    case BROAD_POLICY_TYPE_STAT_EVC:    /* Ptin added: stats */
    case BROAD_POLICY_TYPE_STAT_CLIENT: /* Ptin added: stats */
    case BROAD_POLICY_TYPE_ISCSI:
        /* by default applies to all non-stacking ports */
        SOC_PBMP_OR(tempPbm, PBMP_E_ALL(unit));
        /* TODO: SDK 6.3.0 */
        #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
        LOG_WARNING(LOG_CTX_PTIN_HAPI, "TODO: LB ports not considered!");
        #else
        SOC_PBMP_OR(tempPbm, PBMP_LB(unit));
        #endif
        SOC_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
        break;
    case BROAD_POLICY_TYPE_PORT:
    case BROAD_POLICY_TYPE_DOT1AD:
    case BROAD_POLICY_TYPE_IPSG:
    case BROAD_POLICY_TYPE_LLPF:
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_COSQ:
        /* by default does not apply to any ports */
        BCM_PBMP_CLEAR(policyPtr->pbm);
        break;
    default:
        /* indicate that a type was missed */
        L7_LOG_ERROR(type);
        break;
    }        
}

static void _policy_table_init(int unit)
{
    int p, i;

    if (L7_NULLPTR == policy_map_table[unit])
    {
        policy_map_table[unit] = osapiMalloc(L7_DRIVER_COMPONENT_ID, BROAD_MAX_POLICIES_PER_BCM_UNIT * sizeof(policy_map_table_t));
    }

    if (L7_NULLPTR == policy_map_table[unit])
    {
        L7_LOG_ERROR(0);
        return;
    }

    if (L7_NULLPTR == policy_map_index_map[unit])
    {
        policy_map_index_map[unit] = osapiMalloc(L7_DRIVER_COMPONENT_ID, BROAD_MAX_POLICIES * sizeof(L7_short16));
    }

    if (L7_NULLPTR == policy_map_index_map[unit])
    {
        L7_LOG_ERROR(0);
        return;
    }

    for (i = 0; i < BROAD_POLICY_STAGE_COUNT; i++)
    {
      if (L7_NULLPTR == port_class_table[unit][i])
      {
          port_class_table[unit][i] = osapiMalloc(L7_DRIVER_COMPONENT_ID, _policy_max_port_classes(unit) * sizeof(port_class_t));
      }

      if (L7_NULLPTR == port_class_table[unit][i])
      {
          L7_LOG_ERROR(0);
          return;
      }

    }

    if (L7_NULLPTR == _policy_sem)
    {
      _policy_sem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    }

    if (L7_NULLPTR == _policy_sem)
    {
        L7_LOG_ERROR(0);
        return;
    }

    _policy_sem_take();
    /* initialize the policy to group mapping table */
    for (p = 0; p < BROAD_MAX_POLICIES_PER_BCM_UNIT; p++)
    {
        policy_map_table[unit][p].flags      = GROUP_MAP_NONE;
        policy_map_table[unit][p].group      = BROAD_GROUP_INVALID;
        policy_map_table[unit][p].entry      = L7_NULL;
        policy_map_table[unit][p].entryCount = 0;
    }

    memset(policy_map_index_map[unit], BROAD_POLICY_MAP_INVALID, BROAD_MAX_POLICIES * sizeof(L7_short16));

    policy_efp_on_ifp_table[unit].entry = BROAD_ENTRY_INVALID;
    BCM_PBMP_CLEAR(policy_efp_on_ifp_table[unit].unitEpbm);

    for (i = 0; i < BROAD_POLICY_STAGE_COUNT; i++)
    {
      for (p = 0; p < _policy_max_port_classes(unit); p++)
      {
        BCM_PBMP_CLEAR(port_class_table[unit][i][p].pbm);
      }
    }

    _policy_sem_give();
}

/* PTin added: policer */
/**
 * Get hardware information about fp rules
 * 
 * @param unit : unit
 * @param policy_id 
 * @param rule_id 
 * @param group_id 
 * @param entry_id 
 * @param policer_id 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t l7_bcm_policy_hwInfo_get(int unit, BROAD_POLICY_t policy_id, L7_uint rule_id,
                                 BROAD_GROUP_t *group_id, BROAD_ENTRY_t *entry_id,
                                 int *policer_id, int *counter_id)    /* PTin added: SDK 6.3.0 */
{
  L7_short16 policyIdx;
  policy_map_table_t  *policyPtr;

  /* Validate arguments */
  CHECK_UNIT(unit);
  CHECK_POLICY(policy_id);

  policyIdx = policy_map_index_map[unit][policy_id];

  /* Validate policy index */
  if (policyIdx == BROAD_POLICY_MAP_INVALID)
  {
    return BCM_E_NOT_FOUND;
  }

  policyPtr = &policy_map_table[unit][policyIdx];

  /* Validate rule_id */
  if (rule_id >= policyPtr->entryCount)
  {
    return BCM_E_NOT_FOUND;
  }

  if (group_id!=L7_NULLPTR)   *group_id = policyPtr->group;
  if (entry_id!=L7_NULLPTR)   *entry_id = policyPtr->entry[rule_id];
  if (policer_id!=L7_NULLPTR) *policer_id = (policyPtr->policer_id[rule_id] > 0) ? policyPtr->policer_id[rule_id] : policyPtr->general_policer_id;
  if (counter_id!=L7_NULLPTR) *counter_id = policyPtr->counter_id[rule_id];

  return L7_SUCCESS;
}
/* PTin end */

int l7_bcm_policy_init()
{
    int unit;
    int tmprv, rv = BCM_E_NONE;

    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
    {
        if (!SOC_IS_XGS_FABRIC(unit))
        {
            /* initialize the policy to group mapping table */
            _policy_table_init(unit);

            /* initialize hardware device tables */
            tmprv = policy_group_init(unit);
            if (tmprv < rv)
            {
                rv = tmprv;
            }
        }
    }

    return rv;
}

static int _policy_apply_to_ports(int unit, BROAD_POLICY_t policy)
{
  int                  i;
  int                  rv = BCM_E_NONE;
  policy_map_table_t  *policyPtr;
  unsigned char        oldPortClass = BROAD_INVALID_PORT_CLASS;
  L7_short16           policyIdx;
  BROAD_POLICY_ENTRY_t policyInfo;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;

  bcm_pbmp_t  tempPbm;
  L7_ushort16 vlanId;
  L7_ushort16 mask;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  policyIdx = policy_map_index_map[unit][policy];

  if (policyIdx == BROAD_POLICY_MAP_INVALID)
  {
    return BCM_E_NOT_FOUND;
  }

  policyPtr = &policy_map_table[unit][policyIdx];

  if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
      (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
  {
    oldPortClass = policyPtr->portClass;
    rv = _policy_alloc_portclass(unit, policy);
    if (BCM_E_NONE != rv)
    {
      return rv;
    }
  }

  /* Get the policy information for policy ID from USL database */
  rv = usl_db_policy_info_get(USL_CURRENT_DB, policy, &policyInfo);
  if (rv != BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("Couldn't retrieve policy %d info from USL DB: rv = %d\n\n", policy, rv);
    return rv;
  }

  /* clear temppbm and use it to set for rule whose status is inactive 
   */
  BCM_PBMP_CLEAR(tempPbm);

  /* add port to each entry used by policy */
  for (i = 0; i < policyPtr->entryCount; i++)
  {
    rulePtr = policy_rule_ptr_get(&policyInfo, i);

    if (rulePtr == L7_NULLPTR)
    {
      /* Free any rules allocated by usl_db_policy_info_get(). */
      hapiBroadPolicyRulesPurge(&policyInfo);
      return BCM_E_FAIL;
    }

    if (rulePtr->ruleFlags & BROAD_RULE_STATUS_ACTIVE)
    {
      if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
      {
        rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group,
                                  policyPtr->entry[i],
                                  policyPtr->pbm);      
        if (BCM_E_NONE != rv)
        {
          /* Free any rules allocated by usl_db_policy_info_get(). */
          hapiBroadPolicyRulesPurge(&policyInfo);
          return rv;
        }
      }
      else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
               (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
      {
        if ((oldPortClass != policyPtr->portClass) || (policyPtr->portClass == BROAD_INVALID_PORT_CLASS))
        {
          rv = policy_group_set_portclass(unit, 
                                          policyPtr->policyStage, 
                                          policyPtr->group, 
                                          policyPtr->entry[i],
                                          policyPtr->pbm,
                                          policyPtr->portClass);      
          if (BCM_E_NONE != rv)
          {
            /* Free any rules allocated by usl_db_policy_info_get(). */
            hapiBroadPolicyRulesPurge(&policyInfo);
            return rv;
          }
        }

        /* set the outerVlan for the corresponding entry for rule from policy_map_table_t
         *
         */
        if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_OVID) == BROAD_FIELD_SPECIFIED)
        {

          rv = policy_group_set_outervlan(unit, policyPtr, i,
                                          (char*)hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo,BROAD_FIELD_OVID),
                                          (char*)hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, BROAD_FIELD_OVID));                   
          if (BCM_E_NONE != rv)
          {
            /* Free any rules allocated by usl_db_policy_info_get(). */
            hapiBroadPolicyRulesPurge(&policyInfo);
            return rv;
          }
        }
        else
        {
          /* if the rule is not qualified for BROAD_FIELD_OVID then set the outerVlan field as dont care*/
          vlanId = 0;
          mask = FIELD_MASK_ALL;
          rv = policy_group_set_outervlan(unit, policyPtr, i, (char*) &vlanId,(char*)&mask);
          if (BCM_E_NONE != rv)
          {
            /* Free any rules allocated by usl_db_policy_info_get(). */
            hapiBroadPolicyRulesPurge(&policyInfo);
            return rv;
          }
        }       
      }
    }
    else
    {

      if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
      {
        /* set the pbm as 0 to deactivate the rule for the corresponding entry for rule from policy_map_table_t */
        rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[i],
                                  tempPbm);           
      }
      else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
               (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
      {
        /* set the portclass */
        if ((oldPortClass != policyPtr->portClass) || (policyPtr->portClass == BROAD_INVALID_PORT_CLASS))
        {
          rv = policy_group_set_portclass(unit,
                                          policyPtr->policyStage,
                                          policyPtr->group,
                                          policyPtr->entry[i],
                                          policyPtr->pbm,
                                          policyPtr->portClass);    
          if (BCM_E_NONE != rv)
          {
            /* Free any rules allocated by usl_db_policy_info_get(). */
            hapiBroadPolicyRulesPurge(&policyInfo);
            return rv;
          }
        }

        /* set the outerVlan for the corresponding entry for rule from policy_map_table_t as 4095 to deactivate the rule
        *
        */
        vlanId = 4095;
        mask = FIELD_MASK_NONE;
        rv = policy_group_set_outervlan(unit, policyPtr, i,(char*)&vlanId,(char*)&mask);          
        if (BCM_E_NONE != rv)
        {
          /* Free any rules allocated by usl_db_policy_info_get(). */
          hapiBroadPolicyRulesPurge(&policyInfo);
          return rv;
        }
      }
    }
  }
  /* Free any rules allocated by usl_db_policy_info_get(). */
  hapiBroadPolicyRulesPurge(&policyInfo);
  return rv;
}

static int _policy_apply_egress_mask(int unit, BROAD_POLICY_t policy, bcm_pbmp_t affectedPortsPbm, BROAD_POLICY_EPBM_CMD_t action)
{
  policy_map_table_t  *policyPtr;
  L7_short16           policyIdx;
  policy_map_table_t         *tmpPolicyPtr;
  policy_efp_on_ifp_table_t  *globalPolicyPtr;
  bcm_pbmp_t                  permitPbm, denyPbm, tempPbm;
  int                         i, p;
  int                         rv = BCM_E_NONE;
  BROAD_POLICY_ENTRY_t policyInfo;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;
  
  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  policyIdx = policy_map_index_map[unit][policy];

  if (policyIdx == BROAD_POLICY_MAP_INVALID)
  {
    return BCM_E_NOT_FOUND;
  }
  policyPtr = &policy_map_table[unit][policyIdx];   

  globalPolicyPtr = &policy_efp_on_ifp_table[unit];

  BCM_PBMP_CLEAR(permitPbm);
  BCM_PBMP_CLEAR(denyPbm);

  /* Following changes are needed when a policy is applied on the port:
     1) Update all the permit rules of the policy with ports in unit epbm
        except ports on which this policy is applied.
     2) Update all the deny rules of the policy with ports on which this
        policy is applied.
     3) If the policy has an implicit deny-all rule, then update all the 
        outbound policy rules in the unit to mask out this port. Also 
        update the unit egress mask to include this port.
     4) Create the global default rule.
  */
  if (action == BROAD_POLICY_EPBM_CMD_ADD)
  {
    BCM_PBMP_OR(policyPtr->pbm, affectedPortsPbm);
  }
  else if (action == BROAD_POLICY_EPBM_CMD_REMOVE)
  {
    BCM_PBMP_REMOVE(policyPtr->pbm, affectedPortsPbm);
    BCM_PBMP_REMOVE(globalPolicyPtr->unitEpbm, affectedPortsPbm);
  }
  else 
  {
    return BCM_E_FAIL;
  }

  BCM_PBMP_ASSIGN(permitPbm,  globalPolicyPtr->unitEpbm);
  BCM_PBMP_REMOVE(permitPbm, policyPtr->pbm);
  BCM_PBMP_ASSIGN(denyPbm,  globalPolicyPtr->unitEpbm);
  BCM_PBMP_OR(denyPbm, policyPtr->pbm);

  /* Get the policy information for policy ID from USL database */
  rv = usl_db_policy_info_get(USL_CURRENT_DB, policy, &policyInfo);
  if (rv != BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("couldn't retrieve policy %d info from USL DB: rv = %d\n\n", policy, rv);
     return rv;
  }
  
  for (i = 0; i < policyPtr->entryCount; i++)
  {
    rulePtr = policy_rule_ptr_get(&policyInfo, i);
    if (rulePtr == L7_NULLPTR)
    {
      /* Free any rules allocated by usl_db_policy_info_get(). */
      hapiBroadPolicyRulesPurge(&policyInfo);
      return BCM_E_FAIL;
    }

    /* Set the inports explicitly to all ports to activate the rule 
       Set the inports explicitly to zero to deactivate the rule */
    if(rulePtr->ruleFlags & BROAD_RULE_STATUS_ACTIVE)
    {
       BCM_PBMP_ASSIGN(tempPbm, PBMP_E_ALL(unit));
       rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[i],
                                tempPbm);
       if (BCM_E_NONE != rv)
       {
         /* Free any rules allocated by usl_db_policy_info_get(). */
         hapiBroadPolicyRulesPurge(&policyInfo);
         return rv;
       }
    }
    else
    {  
       BCM_PBMP_CLEAR(tempPbm);
       rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[i],
                                tempPbm);
       if (BCM_E_NONE != rv)
       {
          /* Free any rules allocated by usl_db_policy_info_get(). */
          hapiBroadPolicyRulesPurge(&policyInfo);
          return rv;
       }
    }
 
      rv = policy_group_set_epbm(unit, policyPtr->group, policyPtr->entry[i],
                                 (ENTRY_DENY_ISMASKBITSET(policyPtr, i)) ? denyPbm : permitPbm, 
                                 BROAD_POLICY_EPBM_CMD_ASSIGN);
      if (BCM_E_NONE != rv)
      {
         /* Free any rules allocated by usl_db_policy_info_get(). */
         hapiBroadPolicyRulesPurge(&policyInfo); 
         return rv;
      }
  }

  /* Update all the outbound policy rules on this unit masking out this port */
  if (policyPtr->flags & GROUP_MAP_REQUIRES_IMPLICIT_DENY) 
  {
      for (p = 0; p < BROAD_MAX_POLICIES_PER_BCM_UNIT; p++) 
      {
          tmpPolicyPtr = &policy_map_table[unit][p];
          if (((tmpPolicyPtr->flags & (GROUP_MAP_USED | GROUP_MAP_EFP_ON_IFP)) == (GROUP_MAP_USED | GROUP_MAP_EFP_ON_IFP)) && 
              (tmpPolicyPtr->group == policyPtr->group) &&
              (tmpPolicyPtr != policyPtr))
          {
              for (i = 0; i < tmpPolicyPtr->entryCount; i++)
              {
                  rv = policy_group_set_epbm(unit, tmpPolicyPtr->group, 
                                             tmpPolicyPtr->entry[i],
                                             affectedPortsPbm, 
                                             action);
                  if (BCM_E_NONE != rv)
                  {
                      /* Free any rules allocated by usl_db_policy_info_get(). */
                      hapiBroadPolicyRulesPurge(&policyInfo);
                      return rv;
                  }
              }
          }
      }

    /* Set the port in unitEpbm */
    if (action == BROAD_POLICY_EPBM_CMD_ADD)
    {
      BCM_PBMP_OR(globalPolicyPtr->unitEpbm, affectedPortsPbm);
    }
  }

  /* Add the global default rule */
  if (globalPolicyPtr->entry != BROAD_ENTRY_INVALID)
  {
      /* PTin modified: policer */
      policy_group_delete_rule(unit, BROAD_POLICY_STAGE_INGRESS, policyPtr->group,
                               globalPolicyPtr->entry, globalPolicyPtr->entry,  /* PTin modified: Policer/Counter */
                               0, 0); /* PTin modified: SDK 6.3.0 */
      globalPolicyPtr->entry = BROAD_ENTRY_INVALID;
  }

  if (BCM_PBMP_NOT_NULL(globalPolicyPtr->unitEpbm))
  {
    rv = policy_group_create_default_rule(unit, policyPtr->group, 
                                          globalPolicyPtr->unitEpbm,      
                                         &(globalPolicyPtr->entry));
  }

  /* Free any rules allocated by usl_db_policy_info_get(). */
  hapiBroadPolicyRulesPurge(&policyInfo);
  return rv;

}

int l7_bcm_policy_create(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData, L7_BOOL shuffleAllowed)
{
  int                          i;
  BROAD_GROUP_t                group;
  BROAD_ENTRY_t                entry;
  policy_map_table_t          *policyPtr = L7_NULL;
  int                          savePbm = FALSE;
  bcm_pbmp_t                   savedPbm;
  int                          rv = BCM_E_NONE;
  L7_short16                   policyIdx;
  BROAD_POLICY_RULE_ENTRY_t   *rulePtr;
  L7_uint32                    actions, actionMask;
  L7_ushort16                  ethType;
  policy_efp_on_ifp_table_t   *globalPolicyPtr;
  BROAD_ACTION_ENTRY_t        *actionPtr;
  L7_int                       policer_id = 0, counter_id = 0;  /* PTin added: SDK 6.3.0 */

  L7_ushort16 vlanId;
  L7_ushort16 mask;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  memset(&savedPbm, 0, sizeof(savedPbm));

  _policy_sem_take();
  policyIdx = policy_map_index_map[unit][policy];

  if (policyIdx != BROAD_POLICY_MAP_INVALID)
  {
    /* This is a refresh existing of an existing policy. To insure the entries are added
     * in precedence order it is necessary to delete and recreate the policy in hardware.
     * Save the PBM as this is our only indication of which ports are affected.
     */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("policy changed, delete and re-add\n");

    policyPtr = &policy_map_table[unit][policyIdx];

    savePbm = TRUE;
    SOC_PBMP_ASSIGN(savedPbm, policyPtr->pbm);

    (void)l7_bcm_policy_destroy(unit, policy);
  }
  else if (policyData->policyFlags & BROAD_POLICY_CHANGED)
  {
    _policy_sem_give();
    /* this is a refresh of a policy that we don't know about, so ignore it */
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("policy not applicable, ignore\n");

    return BCM_E_NONE;
  }
  else
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("create policy %d\n", policy);
  }

  /* Create a new mapping from global policy to local BCM policy. */
  for (policyIdx = 0; policyIdx < BROAD_MAX_POLICIES_PER_BCM_UNIT; policyIdx++)
  {
    policyPtr = &policy_map_table[unit][policyIdx];

    if (policyPtr->flags == GROUP_MAP_NONE)
    {
      break;
    }
  }
  if (policyIdx == BROAD_MAX_POLICIES_PER_BCM_UNIT)
  {
    _policy_sem_give();
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("couldn't allocate local BCM policy\n");
    return BCM_E_RESOURCE;
  }

  if (policyData->ruleCount > 0)
  {
    policyPtr->entry    = osapiMalloc(L7_DRIVER_COMPONENT_ID, policyData->ruleCount * sizeof(policyPtr->entry[0]));
    /* PTin added: Policer/Counter */
    policyPtr->srcEntry = osapiMalloc(L7_DRIVER_COMPONENT_ID, policyData->ruleCount * sizeof(policyPtr->entry[0]));
    /* PTin added: SDK 6.3.0 */
    #if 1
    policyPtr->policer_id = osapiMalloc(L7_DRIVER_COMPONENT_ID, policyData->ruleCount * sizeof(policyPtr->policer_id[0]));
    policyPtr->counter_id = osapiMalloc(L7_DRIVER_COMPONENT_ID, policyData->ruleCount * sizeof(policyPtr->counter_id[0]));
    memset(policyPtr->policer_id, 0x00, policyData->ruleCount * sizeof(policyPtr->policer_id[0]));
    memset(policyPtr->counter_id, 0x00, policyData->ruleCount * sizeof(policyPtr->counter_id[0]));
    #endif
    if (policyPtr->entry == L7_NULL)
    {
      _policy_sem_give();
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("couldn't allocate memory for policy entries\n");

      return BCM_E_MEMORY;
    }
  }

  policy_map_index_map[unit][policy] = policyIdx;

  globalPolicyPtr = &policy_efp_on_ifp_table[unit];

  policyPtr->flags       = GROUP_MAP_USED;
  policyPtr->policyType  = policyData->policyType;
  policyPtr->policyStage = policyData->policyStage;
  policyPtr->entryCount  = 0;
  policyPtr->group       = BROAD_GROUP_INVALID;

  /* PTin added: global policer */
  #if 1
  policyPtr->general_policer_id = policyData->general_policer_id;
  #endif

  /* If we're supporting Egress ACLs using the IFP,
     process the stage as IFP instead of EFP. */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_FEAT_EGRESS_ACL_ON_IFP_ID))
  {
    if ((policyData->policyStage == BROAD_POLICY_STAGE_EGRESS) &&
        (policyData->policyType  == BROAD_POLICY_TYPE_PORT))
    {
      policyPtr->policyStage   = BROAD_POLICY_STAGE_INGRESS;
      policyPtr->flags        |= GROUP_MAP_EFP_ON_IFP;
      policyData->policyFlags |= BROAD_POLICY_EGRESS_ON_INGRESS;
    }
  }

  if (FALSE == savePbm)
  {
    _policy_group_set_default_pbm(unit, policyData->policyType, policyData->policyStage, policyPtr);
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("Default Port bitmap: 0x%08x %08x %08x\n",
                   policyPtr->pbm.pbits[2], policyPtr->pbm.pbits[1], policyPtr->pbm.pbits[0]);
  }
  else
  {
    SOC_PBMP_ASSIGN(policyPtr->pbm, savedPbm);
  }

  if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
      (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
  {
    if ((policyData->policyType == BROAD_POLICY_TYPE_PORT)   ||
        (policyData->policyType == BROAD_POLICY_TYPE_DOT1AD) ||
        (policyData->policyType == BROAD_POLICY_TYPE_IPSG)   ||
        (policyData->policyType == BROAD_POLICY_TYPE_LLPF))
    {
      rv = _policy_alloc_portclass(unit, policy);
      if (BCM_E_NONE != rv)
      {
        _policy_sem_give();
        (void)l7_bcm_policy_destroy(unit, policy);
        return rv;
      }
    }
    else
    {
      policyPtr->portClass = BROAD_INVALID_PORT_CLASS;
    }
  }

  if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
  {
    ENTRY_DENY_CLRMASK(policyPtr);
  }

  if (policyData->ruleCount > 0)
  {
    if (policy_stage_supported(unit, policyData->policyStage) == L7_FALSE)
    {
      _policy_sem_give();
      (void)l7_bcm_policy_destroy(unit, policy);
      return BCM_E_UNAVAIL;
    }

    rv = _policy_group_calc_qset(unit, policyData, &policyPtr->resourceReq);
    if (BCM_E_NONE != rv)
    {
      _policy_sem_give();
      (void)l7_bcm_policy_destroy(unit, policy);
      return rv;
    }

    rv = policy_group_create(unit, 
                             policyData, 
                             &group, 
                             &policyPtr->resourceReq);

    if (BCM_E_NONE != rv)
    {
      if (shuffleAllowed)
      {
        /* Couldn't use an existing group or allocate a new group.
           Run policy shuffle routine to try and fit it. */
        rv = policy_group_shuffle(unit, policy, policyData);
        if (rv == BCM_E_NONE)
        {
          _policy_sem_give();
          return rv;
        }
      }

      /* Either a shuffle is not allowed or the shuffle failed. */
      _policy_sem_give();
      (void)l7_bcm_policy_destroy(unit, policy);
      return rv;
    }

    /* populate the group mapping table */
    policyPtr->group = group;

    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      /* Delete the default deny rule now. It will be added back later after this policy's rules
         are put in HW. */
      if (globalPolicyPtr->entry != BROAD_ENTRY_INVALID)
      {
        policy_group_delete_rule(unit, policyPtr->policyStage, policyPtr->group,
                                 globalPolicyPtr->entry, globalPolicyPtr->entry, /* PTin modified: Policer/Counter */
                                 0, 0); /* PTin modified: SDK 6.3.0 */
        globalPolicyPtr->entry = BROAD_ENTRY_INVALID;
      }
    }

    /* add new rules to policy */
    rulePtr = policyData->ruleInfo;
    i = 0;
    while (rulePtr != L7_NULL)
    {
      int srcRule;
      entry = BROAD_ENTRY_INVALID;

      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("- add rule %d\n", i);

      /* For EFP rules on IFP, ensure that only valid actions of permit/deny are allowed */
      if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
      {
        actions = policyData->ruleInfo[i].actionInfo.actions[BROAD_POLICY_ACTION_CONFORMING];
        actionMask = ~((1 << BROAD_ACTION_HARD_DROP) | (1 << BROAD_ACTION_PERMIT));
        if (actions & actionMask)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("- invalid action in rule %d\n", i);

          _policy_sem_give();
          (void)l7_bcm_policy_destroy(unit, policy);

          return BCM_E_FAIL;
        }

        /* Check that only valid qualifiers of IPv4/Layer 4 are allowed and that
           at least one qualifier is present. The only time there would be no 
           qualifiers is for the implicit deny rule of a MAC ACL, which we want to
           forbid. */
        if ((policyData->ruleInfo[i].fieldInfo.flags & 
             ~((1 << BROAD_FIELD_ETHTYPE) | 
               (1 << BROAD_FIELD_DSCP)    | 
               (1 << BROAD_FIELD_PROTO)   | 
               (1 << BROAD_FIELD_SIP)     | 
               (1 << BROAD_FIELD_DIP)     | 
               (1 << BROAD_FIELD_SPORT)   | 
               (1 << BROAD_FIELD_DPORT))) ||
            (policyData->ruleInfo[i].fieldInfo.flags == 0))
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("- invalid qualifier in rule %d\n", i);

          _policy_sem_give();
          (void)l7_bcm_policy_destroy(unit, policy);

          return BCM_E_FAIL;
        }

        /* Check for an implicit deny rule. If present, skip adding this rule since there 
           will be a single deny rule used for the group performing EFP on IFP. */
        actionPtr = &(policyData->ruleInfo[i].actionInfo);
        if (policyData->ruleInfo[i].fieldInfo.flags == (1 << BROAD_FIELD_ETHTYPE))
        {
          memcpy(&ethType, policyData->ruleInfo[i].fieldInfo.fieldEthtype.value, sizeof(ethType));
          if (ethType == 0x0800)
          {
            if (BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_HARD_DROP))
            {
              policyPtr->flags |= GROUP_MAP_REQUIRES_IMPLICIT_DENY;
            }

            /* Don't bother adding implicit rule regardless of action. */
            continue;
          }
        }

        /* Keep track of which rules have a deny action. */
        if (BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_HARD_DROP))
        {
          ENTRY_DENY_SETMASKBIT(policyPtr, i);
        }
      }

      /* convert srcEntry from rule to entry number so lower layer understands */
      srcRule = rulePtr->meterSrcEntry;
      rulePtr->meterSrcEntry = policyPtr->entry[srcRule];       /* PTin modified: entry */
      rulePtr->src_policerId = policyPtr->policer_id[srcRule];  /* PTin added: policer */
      rulePtr->src_counterId = policyPtr->counter_id[srcRule];  /* PTin added: counter */

      /* PTin modified: SDK 6.3.0 */
      policer_id = policyData->general_policer_id;
      counter_id = 0;
      rv = policy_group_add_rule(unit, policyPtr->policyStage, policyData->policyType, group, rulePtr, policyPtr->pbm, policyPtr->flags & GROUP_MAP_EFP_ON_IFP,
                                 &entry, &policer_id, &counter_id);

      if (entry != BROAD_ENTRY_INVALID)
      {
        /* PTin added: SDK 6.3.0 */
        #if 1
        policyPtr->policer_id[i] = (rulePtr->ruleFlags & BROAD_METER_SPECIFIED  ) ? policer_id : -1;
        policyPtr->counter_id[i] = (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED) ? counter_id : -1;
        #endif
        policyPtr->entry[i] = entry;
        policyPtr->srcEntry[i] = policyPtr->entry[srcRule]; /* PTin added: Policer/Counter */
        policyPtr->entryCount++;
      }
      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("- add rule failed\n");

        _policy_sem_give();
        (void)l7_bcm_policy_destroy(unit, policy);

        return rv;
      }

      /* restore srcEntry back to original rule number */
      rulePtr->meterSrcEntry = srcRule;

      /* Set the priority of the rule */
      rv = policy_group_rule_priority_set(unit, policyPtr->policyStage, group, entry, rulePtr->priority);
      //printf("%s(%d) Group %d, entry=%d, Policy-%d, rv=%d\r\n",__FUNCTION__,__LINE__,group, entry, policy,rv);
      if (BCM_E_NONE != rv)
      {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("- priority set failed\n");
        _policy_sem_give();
        (void)l7_bcm_policy_destroy(unit, policy);
        return rv;
      }

      /* For VLAN based policies activate/deactivate the rule based rule status*/
      if (policyData->policyType == BROAD_POLICY_TYPE_VLAN)
      {
        /* Rule status is active- Intsall the rule as activated */
        if (rulePtr->ruleFlags & BROAD_RULE_STATUS_ACTIVE)
        {
          /* Only add specified field if configured for BROAD_FIELD_OVID */
          if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_OVID) == BROAD_FIELD_SPECIFIED)
          {
            rv = policy_group_set_outervlan(unit, policyPtr, i,
                                            (char*)hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo,BROAD_FIELD_OVID),
                                            (char*)hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, BROAD_FIELD_OVID));
            if (BCM_E_NONE != rv)
            {
              return rv;
            }
          }
          else
          {
            /* set the field to match any, as rule is not configured for BROAD_FIELD_OVID*/
            vlanId = 0;
            mask = FIELD_MASK_ALL;
            rv = policy_group_set_outervlan(unit, policyPtr, i, (char*)&vlanId, (char*)&mask);
            if (BCM_E_NONE != rv)
            {
              return rv;
            }
          }
        }
        else
        {
          /* for VLAN Based policies deactivate the rule setting outervlan qualifier to 4095 */
          vlanId = 4095;
          mask = FIELD_MASK_NONE;
          rv = policy_group_set_outervlan(unit, policyPtr, i, (char*)&vlanId, (char*)&mask);
          if (BCM_E_NONE != rv)
          {
            return rv;
          }
        }
      }

      if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
      {
        /* must set pbm for each entry */
        if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
        {
          bcm_pbmp_t tmpPbm;

          BCM_PBMP_CLEAR(tmpPbm);
          /* Set the epbm based on whether the rule action is deny/permit. */
          rv = policy_group_set_epbm(unit, group, entry,
                                     (ENTRY_DENY_ISMASKBITSET(policyPtr, i)) ? policyPtr->pbm : tmpPbm, 
                                     BROAD_POLICY_EPBM_CMD_ASSIGN);
        }
        else if ( !( ( rulePtr->fieldInfo.flags >> BROAD_FIELD_INPORTS) & 1) &&
                  !( ( rulePtr->fieldInfo.flags >> BROAD_FIELD_INPORT) & 1) &&      /* PTin added: ICAP */
                  !( ( rulePtr->fieldInfo.flags >> BROAD_FIELD_SRCTRUNK) & 1) )     /* PTin added: ICAP */
        {
          rv = policy_group_set_pbm(unit, policyPtr->policyStage, group, entry, policyPtr->pbm);
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("Port bitmap to be applied: 0x%08x %08x %08x\n",
                         policyPtr->pbm.pbits[2], policyPtr->pbm.pbits[1], policyPtr->pbm.pbits[0]);
        }
        else
        {
          /* PTin added: save inport data */
          SOC_PBMP_ASSIGN(policyPtr->pbm, *((bcm_pbmp_t *) rulePtr->fieldInfo.fieldInports.value));
        }

        if (BCM_E_NONE != rv)
        {
          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("- pbm set failed\n");

          _policy_sem_give();
          (void)l7_bcm_policy_destroy(unit, policy);
          return rv;
        }
      }
      else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
               (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
      {
        if ((policyData->policyType == BROAD_POLICY_TYPE_PORT)   ||
            (policyData->policyType == BROAD_POLICY_TYPE_DOT1AD) ||
            (policyData->policyType == BROAD_POLICY_TYPE_IPSG)   ||
            (policyData->policyType == BROAD_POLICY_TYPE_LLPF))
        {
          rv = policy_group_set_portclass(unit, 
                                          policyPtr->policyStage,
                                          group,
                                          entry,
                                          policyPtr->pbm,
                                          policyPtr->portClass);
          if (BCM_E_NONE != rv)
          {
            _policy_sem_give();
            (void)l7_bcm_policy_destroy(unit, policy);
            return rv;
          }
        }
      }

      rulePtr = rulePtr->next;
      i++;
    }
  }
  _policy_sem_give();

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("policy %d created: type %d, ruleCount %d, result %d\n",
                 policy, policyData->policyType, 
                 policyPtr->entryCount, rv);

  return rv;
}

int l7_bcm_policy_destroy(int unit, BROAD_POLICY_t policy)
{
    int                        tmprv, rv = BCM_E_NONE;
    int                        i;
    L7_short16                 policyIdx;
    L7_short16                 tempPolicyIdx;
    policy_map_table_t        *policyPtr;
    policy_map_table_t        *tempPolicyPtr;
    L7_BOOL                    isEfpOnIfpPolicy;
    policy_efp_on_ifp_table_t *globalPolicyPtr;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    _policy_sem_take();

    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NONE;  
      /* 43084 - Do not return an error if policy not found. */
      /* return BCM_E_NOT_FOUND; */
    }

    policyPtr = &policy_map_table[unit][policyIdx];

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("destroy policy %d: group %d, ruleCount %d",
               policy, 
               policyPtr->group, policyPtr->entryCount);

    isEfpOnIfpPolicy = (policyPtr->flags & GROUP_MAP_EFP_ON_IFP) ? L7_TRUE : L7_FALSE;
    policyPtr->flags      = GROUP_MAP_NONE;
    policy_map_index_map[unit][policy] = BROAD_POLICY_MAP_INVALID;

    /* remove each group entry used by policy */
    if (policyPtr->group != BROAD_GROUP_INVALID)
    {
      for (i = policyPtr->entryCount - 1; i >= 0 ; --i)
      {
          tmprv = policy_group_delete_rule(unit, policyPtr->policyStage, policyPtr->group,
                                           policyPtr->entry[i], policyPtr->srcEntry[i],   /* PTin modified: Policer/Counter */
                                           policyPtr->policer_id[i], policyPtr->counter_id[i]);     /* PTin added: SDK 6.3.0 */
          /* PTin added: SDK 6.3.0 */
          #if 1
          policyPtr->policer_id[i] = 0;
          policyPtr->counter_id[i] = 0;
          #endif
          policyPtr->srcEntry[i] = 0; /* PTin added: Policer/Counter */
          if (BCM_E_NONE != tmprv)
              rv = tmprv;
      }

      /* Determine if any other policy is using this group. If not, then destroy it. */
      for (tempPolicyIdx = 0; tempPolicyIdx < BROAD_MAX_POLICIES_PER_BCM_UNIT; tempPolicyIdx++)
      {
        tempPolicyPtr = &policy_map_table[unit][tempPolicyIdx];
        if ((policyPtr != tempPolicyPtr) && (tempPolicyPtr->flags & GROUP_MAP_USED))
        {
          if ((tempPolicyPtr->group       == policyPtr->group) &&
              (tempPolicyPtr->policyStage == policyPtr->policyStage))
          {
            break;
          }
        }
      }
      if (tempPolicyIdx == BROAD_MAX_POLICIES_PER_BCM_UNIT)
      {
        /* indicate the group is no longer used by this policy */
        if (isEfpOnIfpPolicy)
        {
          /* If this policy was EFP on IFP, then we need to destroy the global rule. */
          globalPolicyPtr = &policy_efp_on_ifp_table[unit];
          if (globalPolicyPtr->entry != BROAD_ENTRY_INVALID)
          {
              policy_group_delete_rule(unit, BROAD_POLICY_STAGE_INGRESS, policyPtr->group,
                                       globalPolicyPtr->entry, globalPolicyPtr->entry,  /* PTin modified: Policer/Counter */
                                       0, 0); /* PTin modified: SDK 6.3.0 */
              globalPolicyPtr->entry = BROAD_ENTRY_INVALID;
          }
        }
        tmprv = policy_group_destroy(unit, policyPtr->policyStage, policyPtr->group);
        if (BCM_E_NONE != tmprv)
            rv = tmprv;
      }

      policyPtr->group = BROAD_GROUP_INVALID;
    }

    if (policyPtr->entry != L7_NULL)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, policyPtr->entry);
    }
    policyPtr->entry = L7_NULL;
    /* PTin added: Policer */
    if (policyPtr->srcEntry != L7_NULL)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, policyPtr->srcEntry);
    }
    policyPtr->srcEntry = L7_NULL;
    /* PTin added: SDK 6.3.0 */
    #if 1
    if (policyPtr->policer_id != L7_NULL)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, policyPtr->policer_id);
    }
    policyPtr->policer_id = L7_NULL;
    if (policyPtr->counter_id != L7_NULL)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, policyPtr->counter_id);
    }
    policyPtr->counter_id = L7_NULL;

    /* No global policer */
    policyPtr->general_policer_id = 0;
    #endif

    policyPtr->entryCount = 0;
    _policy_sem_give();

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("result %d\n", rv);

    return rv;
}

int l7_bcm_policy_apply(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
    int                  rv;
    policy_map_table_t  *policyPtr;
    bcm_pbmp_t           tempPbm;
    L7_short16           policyIdx;
    unsigned int         oldPortClass = BROAD_INVALID_PORT_CLASS;
    bcm_pbmp_t           affectedPortsPbm;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d apply port %d\n", policy, port);

    _policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      BCM_PBMP_PORT_SET(affectedPortsPbm, port); 
      rv = _policy_apply_egress_mask(unit, policy, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_ADD);
    }
    else
    {
      BCM_PBMP_PORT_ADD(policyPtr->pbm, port); 

      oldPortClass = policyPtr->portClass; /* save the original portClass */
      rv = _policy_apply_to_ports(unit, policy);
    }
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      /* Update port class for this port. */
      BCM_PBMP_PORT_SET(tempPbm, port);

      /* If the portClass has changed for this policy, then we need to update all of the
         ports in this group. */
      if (oldPortClass != policyPtr->portClass)
      {
        BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
      }

      rv = policy_port_class_pbmp_update(unit, tempPbm, policyPtr->policyStage, oldPortClass, policyPtr->portClass);
      if (BCM_E_NONE != rv)
      {
        _policy_sem_give();
        return rv;
      }
    }

    _policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_policy_apply_all(int unit, BROAD_POLICY_t policy)
{
    int                 rv;
    policy_map_table_t *policyPtr;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;
    unsigned int        oldPortClass = BROAD_INVALID_PORT_CLASS;
    bcm_pbmp_t          affectedPortsPbm;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d apply all\n", policy);

    _policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      BCM_PBMP_ASSIGN(affectedPortsPbm, PBMP_E_ALL(unit));
      rv = _policy_apply_egress_mask(unit, policy, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_ADD);
    }
    else
    {
      BCM_PBMP_ASSIGN(policyPtr->pbm, PBMP_E_ALL(unit));

      oldPortClass = policyPtr->portClass; /* save the original portClass */
      rv = _policy_apply_to_ports(unit, policy);
    }
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      rv = policy_port_class_pbmp_update(unit, policyPtr->pbm, policyPtr->policyStage, oldPortClass, policyPtr->portClass);
      if (BCM_E_NONE != rv)
      {
        _policy_sem_give();
        return rv;
      }
    }

    _policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_policy_remove(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
    int                 rv = BCM_E_NONE;
    policy_map_table_t *policyPtr;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;
    unsigned int        oldPortClass = BROAD_INVALID_PORT_CLASS;
    bcm_pbmp_t          affectedPortsPbm;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d remove port %d\n", policy, port);

    _policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NONE;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      BCM_PBMP_PORT_SET(affectedPortsPbm, port); 
      rv = _policy_apply_egress_mask(unit, policy, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_REMOVE);
    }
    else
    {
      BCM_PBMP_PORT_REMOVE(policyPtr->pbm, port);

      oldPortClass = policyPtr->portClass; /* save the original portClass */
      rv = _policy_apply_to_ports(unit, policy);
    }
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      /* Update port class for this port. */
      rv = policy_port_class_add_remove(unit, port, policyPtr->policyStage, oldPortClass, L7_FALSE);
      if (BCM_E_NONE != rv)
      {
        _policy_sem_give();
        return rv;
      }

      /* If the portClass has changed for this policy, then we need to update all of the
         remaining ports in this group. */
      if (oldPortClass != policyPtr->portClass)
      {
        rv = policy_port_class_pbmp_update(unit, policyPtr->pbm, policyPtr->policyStage, oldPortClass, policyPtr->portClass);
        if (BCM_E_NONE != rv)
        {
          _policy_sem_give();
          return rv;
        }
      }
    }

    /* If this policy is a port policy but there are no ports assigned to the policy,
       go ahead and remove the policy from HW to free up resources. */
    if (BCM_PBMP_IS_NULL(policyPtr->pbm) && (policyPtr->policyType == BROAD_POLICY_TYPE_PORT))
    {
      l7_bcm_policy_destroy(unit, policy);
    }

    _policy_sem_give();
    return rv;
}
/*********************************************************************
* @purpose   Set status for policy Rule ID Activate/Deactivate the rule installed in hardware
*
* @param    BROAD_POLICY_t       policy
* @param    BROAD_PORT_RULE_t    rule
* @param    BROAD_RULE_STATUS_t  status
* 
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_bcm_policy_rule_status_set(int unit, BROAD_POLICY_t policy, BROAD_POLICY_RULE_t rule,BROAD_RULE_STATUS_t status)
{
  int                        rv = BCM_E_NONE;
  policy_map_table_t        *policyPtr;
  L7_short16                 policyIdx;
  BROAD_POLICY_ENTRY_t       policyData;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;
  L7_ushort16                vlanId;
  L7_ushort16                mask;
  bcm_pbmp_t                 tempPbm;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  _policy_sem_take();
  policyIdx = policy_map_index_map[unit][policy];
  if (policyIdx == BROAD_POLICY_MAP_INVALID)
  {
    /*return BCM_E_NONE code if policy not found on the unit*/
    _policy_sem_give();
    return BCM_E_NONE;
  }
  policyPtr = &policy_map_table[unit][policyIdx];

  /* Get the policy information for policy ID from USL database */
  rv = usl_db_policy_info_get(USL_CURRENT_DB, policy, &policyData);
  if (rv != BCM_E_NONE)
  {
    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("Couldn't retrieve policy %d info from USL DB: rv = %d\n\n", policy, rv);
    _policy_sem_give();
    return rv;
  }

  rulePtr = policy_rule_ptr_get(&policyData, rule); 
  if (rulePtr == L7_NULLPTR)
  {
    /* Free any rules allocated by usl_db_policy_info_get(). */
    hapiBroadPolicyRulesPurge(&policyData);
    _policy_sem_give();   
    return BCM_E_FAIL;
  }

  /* Read the  status of rule from policy information
   * Rule status active
   */
  if (status == BROAD_POLICY_RULE_STATUS_ACTIVE)
  {
    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      /* set the Inports Qualifier to all ports to activate outbound rule on EFP_ON_IFP*/
      BCM_PBMP_ASSIGN(tempPbm, PBMP_E_ALL(unit));
      rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[rule], tempPbm);
      if (BCM_E_NONE != rv)
      {
        /* Free any rules allocated by usl_db_policy_info_get(). */
        hapiBroadPolicyRulesPurge(&policyData);
        _policy_sem_give();
        return rv;
      }
    }
    else
    {
      /* if policy is port based */
      if (policyData.policyType == BROAD_POLICY_TYPE_PORT)
      {
        if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
        {
          /* set the pbm for the corresponding entry for rule from policy_map_table_t */
          rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[rule], policyPtr->pbm);
          if (BCM_E_NONE != rv)
          {
            /* Free any rules allocated by usl_db_policy_info_get(). */
            hapiBroadPolicyRulesPurge(&policyData);
            _policy_sem_give();
            return rv;
          }
        }
        else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
                 (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
        {
          /* set the outerVlan for the corresponding entry for rule from policy_map_table_t
           * 
           */
          if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_OVID) == BROAD_FIELD_SPECIFIED)
          {
            rv = policy_group_set_outervlan(unit, policyPtr, rule,
                                            (char*)hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo,BROAD_FIELD_OVID),
                                            (char*)hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, BROAD_FIELD_OVID));           
            if (BCM_E_NONE != rv)
            {
              /* Free any rules allocated by usl_db_policy_info_get(). */
              hapiBroadPolicyRulesPurge(&policyData);
              _policy_sem_give();
              return rv;
            }
          }
          else
          {
            /* if the rule is not qualified for BROAD_FIELD_OVID
             * then set the outerVlan field as dont care
             */
            vlanId = 0;
            mask = FIELD_MASK_ALL;
            rv = policy_group_set_outervlan(unit, policyPtr, rule, (char*) &vlanId,(char*)&mask);
            if (BCM_E_NONE != rv)
            {
              /* Free any rules allocated by usl_db_policy_info_get(). */
              hapiBroadPolicyRulesPurge(&policyData);
              _policy_sem_give();
              return rv;
            }
          } 
        }
      }
      else if (policyData.policyType == BROAD_POLICY_TYPE_VLAN)
      {
        /* for VLAN Based policies TYPE VLAN should be configured */
        rv = policy_group_set_outervlan(unit, policyPtr, rule,
                                        (char*)hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo,BROAD_FIELD_OVID),
                                        (char*)hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, BROAD_FIELD_OVID));
        if (BCM_E_NONE != rv)
        {
          /* Free any rules allocated by usl_db_policy_info_get(). */
          hapiBroadPolicyRulesPurge(&policyData);
          _policy_sem_give();
          return rv;
        }
      }
    }   /* end of check for policy EFP_ON_IFP*/ 
  }
  /* Rule status inactive */
  else
  {
    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      /* set the Inports Qualifier to all ports to activate outbound rule on EFP_ON_IFP*/
      BCM_PBMP_CLEAR(tempPbm);
      rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group,
                                policyPtr->entry[rule], tempPbm);
      if (BCM_E_NONE != rv)
      {
        /* Free any rules allocated by usl_db_policy_info_get(). */
        hapiBroadPolicyRulesPurge(&policyData);
        _policy_sem_give();
        return rv;
      }
    }
    else
    {
      if (policyData.policyType == BROAD_POLICY_TYPE_PORT)
      {
        if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
        {
          /* set the pbm as 0 to deactivate the rule for the corresponding entry */
          BCM_PBMP_CLEAR(tempPbm);
          rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group,
                                    policyPtr->entry[rule],
                                    tempPbm);
          if (BCM_E_NONE != rv)
          {
            /* Free any rules allocated by usl_db_policy_info_get(). */
            hapiBroadPolicyRulesPurge(&policyData);
            _policy_sem_give();
            return rv;
          }
        }
        else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
                 (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
        {
          /* set the outerVlan for the corresponding entry to 4095 to deactivate the rule 
           *
           */
          vlanId = 4095;
          mask = FIELD_MASK_NONE;
          rv = policy_group_set_outervlan(unit, policyPtr, rule, (char*)&vlanId,(char*)&mask);
          if (BCM_E_NONE != rv)
          {
            /* Free any rules allocated by usl_db_policy_info_get(). */
            hapiBroadPolicyRulesPurge(&policyData);
            _policy_sem_give();
            return rv;
          }
        }
      }
      else if (policyData.policyType == BROAD_POLICY_TYPE_VLAN)
      {
        /* set the outerVlan for the corresponding entry to 4095 to deactivate the rule 
        *
        */
        vlanId = 4095;
        mask = FIELD_MASK_NONE;
        rv = policy_group_set_outervlan(unit, policyPtr, rule, (char*)&vlanId,(char*)&mask);
        if (BCM_E_NONE != rv)
        {
          /* Free any rules allocated by usl_db_policy_info_get(). */
          hapiBroadPolicyRulesPurge(&policyData);
          _policy_sem_give();
          return rv;
        }
      }
    } /* end of check for policy EFP_ON_IFP*/
  }

  /* Free any rules allocated by usl_db_policy_info_get(). */
  hapiBroadPolicyRulesPurge(&policyData);
  _policy_sem_give();
  return BCM_E_NONE;
}


int l7_bcm_policy_remove_all(int unit, BROAD_POLICY_t policy)
{
    int                 rv;
    policy_map_table_t *policyPtr;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;
    bcm_pbmp_t          affectedPortsPbm;
    unsigned int        oldPortClass = BROAD_INVALID_PORT_CLASS;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d remove all\n", policy);

    _policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      BCM_PBMP_ASSIGN(affectedPortsPbm, PBMP_E_ALL(unit));
      rv = _policy_apply_egress_mask(unit, policy, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_REMOVE);
    }
    else
    {
      BCM_PBMP_CLEAR(policyPtr->pbm);

      oldPortClass = policyPtr->portClass; /* save the original portClass */
      rv = _policy_apply_to_ports(unit, policy);
    }
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      rv = policy_port_class_pbmp_update(unit, tempPbm, policyPtr->policyStage, oldPortClass, policyPtr->portClass);
      if (BCM_E_NONE != rv)
      {
        _policy_sem_give();
        return rv;
      }
    }

    _policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_policy_stats(int unit, BROAD_POLICY_t policy, L7_uint32 ruleId, L7_uchar8 *buffer, L7_uint32 *buffer_size)
{
  int                 rv = BCM_E_NONE;
  policy_map_table_t *policyPtr;
  L7_short16          policyIdx;
  L7_uint32           val1_hi, val1_lo, val2_hi, val2_lo;
  int                 tmprv;
  uint64              val1, val2;
  uint64              tmpVal1, tmpVal2;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("get policy %d rule %d stats\n", policy, ruleId);

  _policy_sem_take();
  policyIdx = policy_map_index_map[unit][policy];
  if (policyIdx == BROAD_POLICY_MAP_INVALID)
  {
    _policy_sem_give();
    return BCM_E_NONE;
  }
  policyPtr = &policy_map_table[unit][policyIdx];

  if (ruleId > policyPtr->entryCount)
  {
    _policy_sem_give();
    return BCM_E_PARAM;
  }

  /* Each entry requires 16 bytes. Make sure we don't overflow the response buffer. */
  if (*buffer_size >= 16)
  {
    /* Read the stat values from the buffer. */
    memcpy(&val1_hi, &buffer[0],  sizeof(val1_hi));
    memcpy(&val1_lo, &buffer[4],  sizeof(val1_lo));
    memcpy(&val2_hi, &buffer[8],  sizeof(val2_hi));
    memcpy(&val2_lo, &buffer[12], sizeof(val2_lo));
    COMPILER_64_SET(tmpVal1, val1_hi, val1_lo);
    COMPILER_64_SET(tmpVal2, val2_hi, val2_lo);

    /* query stats for entry -- in case of error this routine returns 0 */
    tmprv = policy_group_get_stats(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[ruleId], &val1, &val2);

    if (tmprv < rv)
      rv = tmprv;     /* just get the worst error code */

    /* Accumulate the stats. */
    COMPILER_64_ADD_64(val1, tmpVal1);
    COMPILER_64_ADD_64(val2, tmpVal2);
    val1_hi = COMPILER_64_HI(val1);
    val1_lo = COMPILER_64_LO(val1);
    val2_hi = COMPILER_64_HI(val2);
    val2_lo = COMPILER_64_LO(val2);

    if (hapiBroadPolicyDebugLevel() == POLICY_DEBUG_HIGH)
        sysapiPrintf("%s:%d: rv = %d, val1_hi = 0x%x, val1_lo = 0x%x, val2_hi = 0x%x, val2_lo = 0x%x\n", __FUNCTION__, __LINE__, tmprv, 
               val1_hi, val1_lo, val2_hi, val2_lo);

    /* Write the stat values to the buffer. */
    memcpy(&buffer[0],  &val1_hi, sizeof(val1_hi));
    memcpy(&buffer[4],  &val1_lo, sizeof(val1_lo));
    memcpy(&buffer[8],  &val2_hi, sizeof(val2_hi));
    memcpy(&buffer[12], &val2_lo, sizeof(val2_lo));

    *buffer_size = 16; /* 16 bytes per rule. */
  }

  _policy_sem_give();
  return rv;
}

int l7_bcm_policy_counter_clear(int unit, BROAD_POLICY_t policy)
{
    int                 i;
    int                 rv = BCM_E_NONE;
    policy_map_table_t *policyPtr;
    L7_short16          policyIdx;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("clear counters policy %d\n", policy);

    _policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    /* retrieve count for each entry in the policy */
    for (i = 0; i < policyPtr->entryCount; i++)
    {
        int    tmprv;

        /* query stats for entry -- in case of error this routine returns 0 */
        tmprv = policy_group_stats_clear(unit, policyPtr->policyStage, policyPtr->entry[i]);
        if (tmprv < rv)
            rv = tmprv;     /* just get the worst error code */
    }

    _policy_sem_give();
    return rv;
}

void l7_bcm_policy_dataplane_cleanup(L7_uint32 unit, L7_uint32 policy, L7_uint32 rule, L7_uint32 cpu_modid, L7_uint32 cpu_modport)
{
  policy_map_table_t *policyPtr;
  L7_short16          policyIdx;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("new mgr policy %d, rule %d, modid %d, modport %d\n", policy, rule, cpu_modid, cpu_modport);

  _policy_sem_take();

  do
  {
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      break;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    if (rule > policyPtr->entryCount)
    {
      break;
    }
    policy_group_dataplane_cleanup(unit, policyPtr->entry[rule], cpu_modid, cpu_modport);

  } while (0);

  _policy_sem_give();
}

int _policy_map_get_info(int unit, int policy, policy_map_table_t *policyPtr)
{
  int rv = BCM_E_NOT_FOUND;
  unsigned int policyIdx;

  memset(policyPtr, 0, sizeof(*policyPtr));

  if (policy < BROAD_MAX_POLICIES)
  {
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx != BROAD_POLICY_MAP_INVALID)
    {
      memcpy(policyPtr, &policy_map_table[unit][policyIdx], sizeof(*policyPtr));
      rv = BCM_E_NONE;
    }
  }

  return rv;
}

/* Debug */

void debug_policy_table(int unit)
{
  int i, j;
  char policyStageString[BROAD_POLICY_STAGE_COUNT][10] = {"VFP","IFP","EFP"};
  L7_short16 policyIdx;
  policy_map_table_t *policyPtr;
  char pfmt1[SOC_PBMP_FMT_LEN];
  char pfmt2[SOC_PBMP_FMT_LEN];

  sysapiPrintf("Unit %d\n", unit);

  sysapiPrintf("Policy    Group       PBM                MASK             EntryCount  Entries\n");
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    policyIdx = policy_map_index_map[unit][i];
    if (policyIdx != BROAD_POLICY_MAP_INVALID)
    {
      policyPtr = &policy_map_table[unit][policyIdx];
      if (policyPtr->group != BROAD_GROUP_INVALID)
      {
        sysapiPrintf("[%2d]       %2d %3s   %1s%s %s %02d         [",
               i, policyPtr->group, policyStageString[policyPtr->policyStage],
               (policyPtr->flags & GROUP_MAP_EFP_ON_IFP) ? "E" : " ",
               SOC_PBMP_FMT(policyPtr->pbm, pfmt1),
               SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmt2),
               (int)policyPtr->entryCount);
        for (j = 0; j < policyPtr->entryCount; j++)
          sysapiPrintf("%d ", policyPtr->entry[j]);
        sysapiPrintf("]\n");
      }
    }
  }

  if (policy_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP))
  {
    sysapiPrintf("Lookup Stage:\n");
    sysapiPrintf("Port Class      PBM                 Policies\n");
    for (i = 0; i < _policy_max_port_classes(unit); i++)
    {
      if (BCM_PBMP_NOT_NULL(port_class_table[unit][BROAD_POLICY_STAGE_LOOKUP][i].pbm))
      {
        sysapiPrintf("[%2d]          %s    ",i, SOC_PBMP_FMT(port_class_table[unit][BROAD_POLICY_STAGE_LOOKUP][i].pbm, pfmt1));
        for (j = 0; j < BROAD_MAX_POLICIES; j++)
        {
          policyIdx = policy_map_index_map[unit][j];
          if (policyIdx != BROAD_POLICY_MAP_INVALID)
          {
            policyPtr = &policy_map_table[unit][policyIdx];
            if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) && 
                (BCM_PBMP_EQ(policyPtr->pbm, port_class_table[unit][BROAD_POLICY_STAGE_LOOKUP][i].pbm)))
            {
              sysapiPrintf("[%d] ", j);
            }
          }
        }
        sysapiPrintf("\n");
      }
    }
  }
  if (policy_stage_supported(unit, BROAD_POLICY_STAGE_EGRESS))
  {
    sysapiPrintf("Egress Stage:\n");
    sysapiPrintf("Port Class      PBM                 Policies\n");
    for (i = 0; i < _policy_max_port_classes(unit); i++)
    {
      if (BCM_PBMP_NOT_NULL(port_class_table[unit][BROAD_POLICY_STAGE_EGRESS][i].pbm))
      {
        sysapiPrintf("[%2d]          %s    ",i, SOC_PBMP_FMT(port_class_table[unit][BROAD_POLICY_STAGE_EGRESS][i].pbm, pfmt1));
        for (j = 0; j < BROAD_MAX_POLICIES; j++)
        {
          policyIdx = policy_map_index_map[unit][j];
          if (policyIdx != BROAD_POLICY_MAP_INVALID)
          {
            policyPtr = &policy_map_table[unit][policyIdx];
            if ((policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS) && 
                (BCM_PBMP_EQ(policyPtr->pbm, port_class_table[unit][BROAD_POLICY_STAGE_EGRESS][i].pbm)))
            {
              sysapiPrintf("[%d] ", j);
            }
          }
        }
        sysapiPrintf("\n");
      }
    }
  }
}

int policy_map_table_t_size()
{
  return sizeof(policy_map_table_t);
}

void debug_policy_efp_on_ifp_table(int unit)
{
    policy_efp_on_ifp_table_t  *globalPolicyPtr;

    globalPolicyPtr = &policy_efp_on_ifp_table[unit];
    sysapiPrintf("Unit %d\n", unit);
    sysapiPrintf("Default entry eid = %d\n",globalPolicyPtr->entry);
    sysapiPrintf("Unit Epbm = %08x\n",SOC_PBMP_WORD_GET(globalPolicyPtr->unitEpbm, 0));
}
