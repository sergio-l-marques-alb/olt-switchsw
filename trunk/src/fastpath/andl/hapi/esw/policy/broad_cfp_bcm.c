/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_cfp_bcm.c
*
* This file implements the custom bcm layer that runs on all units. 
*
* @component hapi
*
* @create    
*
* @author    
*
* @end
*
**********************************************************************/


#include "broad_cfp_bcm.h"
#include "broad_cfp_robo.h"
#include "ibde.h"
#include "bcm/custom.h"

/* Policy Group Map Table Definitions */
/* This table contains information used to map a policy to multiple entries
 * within a group. It also contains a bitmap of the ports to which this
 * policy applies.
 */

#define GROUP_MAP_NONE     0
#define GROUP_MAP_USED     1

typedef struct
{
    BROAD_GROUP_t        group;
    BROAD_ENTRY_t        entry[BROAD_MAX_RULES_PER_POLICY];
    L7_int               policer_id[BROAD_MAX_RULES_PER_POLICY];    /* PTin added: policer */
    bcm_pbmp_t           pbm;
    bcm_pbmp_t           pbmMask;
    unsigned char        flags;
    BROAD_POLICY_STAGE_t policyStage;
    unsigned char        portClass; /* Used for LOOKUP and EGRESS port classes */
#if (BROAD_MAX_RULES_PER_POLICY < 256)
    unsigned char        entryCount;
#else
    unsigned short       entryCount;
#endif
}
policy_map_table_t;

static policy_map_table_t *policy_map_table[SOC_MAX_NUM_DEVICES] = {0};     /* This structure only holds data for the policies local to a BCM unit. */
static L7_short16         *policy_map_index_map[SOC_MAX_NUM_DEVICES] = {0}; /* This structure maps global policy IDs to an index into policy_map_table. */

void *_cfp_policy_sem = L7_NULL;

#define CHECK_POLICY(p)  {if (p >= BROAD_MAX_POLICIES)  LOG_ERROR(p);}
#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) LOG_ERROR(u);}
#define CHECK_RULE(r)    {if (r >= BROAD_MAX_RULES_PER_POLICY) LOG_ERROR(r);}

typedef struct
{
  bcm_pbmp_t pbm;
} port_class_t;

static port_class_t port_class_table[SOC_MAX_NUM_DEVICES][BROAD_POLICY_STAGE_COUNT][BROAD_MAX_PORT_CLASS_ID];

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

    /* Don't allocate a portClass for NULL bitmaps */
    if (BCM_PBMP_IS_NULL(policyPtr->pbm))
    {
      policyPtr->portClass = BROAD_INVALID_PORT_CLASS;
      return BCM_E_NONE;
    }

    /* Don't allocate a portClass if there is only one port in the bitmap. */
    BCM_PBMP_COUNT(policyPtr->pbm, numPorts);
    if (numPorts == 1)
    {
      policyPtr->portClass = BROAD_INVALID_PORT_CLASS;
      return BCM_E_NONE;
    }

    /* Try to find a port class that has the same bitmap as the policy. */
    for (i = 1; i < BROAD_MAX_PORT_CLASS_ID; i++)
    {
      if (BCM_PBMP_EQ(policyPtr->pbm, port_class_table[unit][policyPtr->policyStage][i].pbm))
      {
        policyPtr->portClass = i;
        return BCM_E_NONE;
      }
    }

    /* At this point, no portclass bitmaps equalled the policy's bitmap. 
       Next, try to find an available portclass that can be used by this
       policy and assign the policy's bitmap to that portclass. */
    for (i = 1; i < BROAD_MAX_PORT_CLASS_ID; i++)
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
        return BCM_E_NONE;
      }
    }

    return BCM_E_RESOURCE;
}

static void _cfp_policy_sem_take()
{
  if (_cfp_policy_sem == L7_NULL)
  {
    LOG_ERROR(0);
  }

  osapiSemaTake(_cfp_policy_sem, L7_WAIT_FOREVER);
}

static void _cfp_policy_sem_give()
{
  if (_cfp_policy_sem == L7_NULL)
  {
    LOG_ERROR(0);
  }

  osapiSemaGive(_cfp_policy_sem);
}

/* BCM Functions */

static void _policy_group_set_default_pbm(int unit, BROAD_POLICY_TYPE_t type, policy_map_table_t *policyPtr)
{
    switch (type)
    {
    case BROAD_POLICY_TYPE_SYSTEM:
    case BROAD_POLICY_TYPE_PTIN:        /* Ptin added: policer */
    case BROAD_POLICY_TYPE_STAT_EVC:    /* Ptin added: stats */
    case BROAD_POLICY_TYPE_STAT_CLIENT: /* Ptin added: stats */
    case BROAD_POLICY_TYPE_VLAN:
    case BROAD_POLICY_TYPE_ISCSI:
    case BROAD_POLICY_TYPE_DOT1AD_SNOOP:
        /* by default applies to all non-stacking ports */
        SOC_PBMP_ASSIGN(policyPtr->pbm, PBMP_E_ALL(unit));
        break;
    case BROAD_POLICY_TYPE_PORT:
    case BROAD_POLICY_TYPE_IPSG:
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_COSQ:
    case BROAD_POLICY_TYPE_DVLAN:
        /* by default does not apply to any ports */
        BCM_PBMP_CLEAR(policyPtr->pbm);
        break;
    default:
        /* indicate that a type was missed */
        LOG_ERROR(type);
        break;
    }        

    SOC_PBMP_ASSIGN(policyPtr->pbmMask, PBMP_PORT_ALL(unit));
}

static void _policy_table_init(int unit)
{
    int p;

    if (L7_NULLPTR == policy_map_table[unit])
    {
        policy_map_table[unit] = osapiMalloc(L7_DRIVER_COMPONENT_ID, BROAD_MAX_POLICIES_PER_BCM_UNIT * sizeof(policy_map_table_t));
    }

    if (L7_NULLPTR == policy_map_table[unit])
    {
        LOG_ERROR(0);
        return;
    }

    if (L7_NULLPTR == policy_map_index_map[unit])
    {
        policy_map_index_map[unit] = osapiMalloc(L7_DRIVER_COMPONENT_ID, BROAD_MAX_POLICIES * sizeof(L7_short16));
    }

    if (L7_NULLPTR == policy_map_index_map[unit])
    {
        LOG_ERROR(0);
        return;
    }

    if (L7_NULLPTR == _cfp_policy_sem)
    {
      _cfp_policy_sem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    }

    if (L7_NULLPTR == _cfp_policy_sem)
    {
        LOG_ERROR(0);
        return;
    }

    _cfp_policy_sem_take();
    /* initialize the policy to group mapping table */
    for (p = 0; p < BROAD_MAX_POLICIES_PER_BCM_UNIT; p++)
    {
        policy_map_table[unit][p].flags      = GROUP_MAP_NONE;
        policy_map_table[unit][p].group      = BROAD_GROUP_INVALID;
        policy_map_table[unit][p].entryCount = 0;
    }

    memset(policy_map_index_map[unit], BROAD_POLICY_MAP_INVALID, BROAD_MAX_POLICIES * sizeof(L7_short16));

    _cfp_policy_sem_give();
}

int l7_bcm_cfp_policy_init()
{
    int unit;
    int tmprv, rv = BCM_E_NONE;
    extern int soc_ndev;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            sysapiPrintf("l7_bcm_cfp_policy_init soc_ndev=%d\n", soc_ndev);

    for (unit = 0; unit < BDE_SWITCH_DEVICES; unit++)
    {
        if (!SOC_IS_XGS_FABRIC(unit))
        {
            /* initialize the policy to group mapping table */
            _policy_table_init(unit);

            /* initialize hardware device tables */
            tmprv = policy_cfp_group_init(unit);
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
  unsigned int         oldPortClass = BROAD_INVALID_PORT_CLASS;
  L7_short16           policyIdx;

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
        return rv;
  }

  /* add port to each entry used by policy */
  for (i = 0; i < policyPtr->entryCount; i++)
  {
    if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
    {
      rv = policy_cfp_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[i],
                                policyPtr->pbm, policyPtr->pbmMask);
      if (BCM_E_NONE != rv)
          return rv;
    }
    else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
             (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      if ((oldPortClass != policyPtr->portClass) || (policyPtr->portClass == BROAD_INVALID_PORT_CLASS))
      {
        rv = policy_cfp_group_set_portclass(unit, 
                                        policyPtr->policyStage, 
                                        policyPtr->group, 
                                        policyPtr->entry[i],
                                        policyPtr->pbm,
                                        policyPtr->portClass);
        if (BCM_E_NONE != rv)
            return rv;
      }
    }
  }

  return rv;
}

int l7_bcm_cfp_policy_create(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData, L7_BOOL shuffleAllowed)
{
    int                 i;
    BROAD_GROUP_t       group;
    BROAD_ENTRY_t       entry;
    policy_map_table_t *policyPtr = L7_NULL;
    int                 savePbm = FALSE;
    bcm_pbmp_t          savedPbm, savedPbmMask;
    int                 rv = BCM_E_NONE;
    L7_short16           policyIdx;
    BROAD_POLICY_RULE_ENTRY_t *rulePtr;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    _cfp_policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    memset(&savedPbmMask, 0x00, sizeof(bcm_pbmp_t));
    memset(&savedPbm, 0x00, sizeof(bcm_pbmp_t));
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
        SOC_PBMP_ASSIGN(savedPbmMask, policyPtr->pbmMask);

        (void)l7_bcm_cfp_policy_destroy(unit, policy);
    }
    else if (policyData->policyFlags & BROAD_POLICY_CHANGED)
    {
        _cfp_policy_sem_give();
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
      _cfp_policy_sem_give();
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          sysapiPrintf("couldn't allocate local BCM policy\n");

      return BCM_E_RESOURCE;
    }
    policy_map_index_map[unit][policy] = policyIdx;

    policyPtr->flags       = GROUP_MAP_USED;
    policyPtr->policyStage = policyData->policyStage;

    if (FALSE == savePbm)
    {
        _policy_group_set_default_pbm(unit, policyData->policyType, policyPtr);
    }
    else
    {
        SOC_PBMP_ASSIGN(policyPtr->pbm, savedPbm);
        SOC_PBMP_ASSIGN(policyPtr->pbmMask, savedPbmMask);
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      if ((policyData->policyType == BROAD_POLICY_TYPE_PORT) ||
          (policyData->policyType == BROAD_POLICY_TYPE_IPSG) ||
          (policyData->policyType == BROAD_POLICY_TYPE_DVLAN))
      {
        rv = _policy_alloc_portclass(unit, policy);
        if (BCM_E_NONE != rv)
        {
          _cfp_policy_sem_give();
          (void)l7_bcm_cfp_policy_destroy(unit, policy);
          return rv;
        }
      }
    }

    if (policyData->ruleCount > 0)
    {
      rv = policy_cfp_group_create(unit, policyPtr->policyStage, policyData, &group);
      if (BCM_E_NONE != rv)
      {
        _cfp_policy_sem_give();
        (void)l7_bcm_cfp_policy_destroy(unit, policy);
        return rv;
      }

      /* populate the group mapping table */
      policyPtr->group      = group;
      policyPtr->entryCount = 0;

      /* add new rules to policy */
      rulePtr = policyData->ruleInfo;
      i = 0;
      while (rulePtr != L7_NULL)
      {
          int srcRule;
          entry = BROAD_ENTRY_INVALID;

          if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
              sysapiPrintf("- add rule %d\n", i);

          /* convert srcEntry from rule to entry number so lower layer understands */
          srcRule = rulePtr->meterSrcEntry;
          rulePtr->meterSrcEntry = policyPtr->entry[srcRule];

          rv = policy_cfp_group_add_rule(unit, policyPtr->policyStage, group, rulePtr, policyPtr->pbm, &entry);
          if (BCM_E_NONE != rv)
          {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                  sysapiPrintf("- add rule failed\n");

              /* If entry is not invalid, then the rule has been created
               * but the add failed during a subsequent phase. The entry 
               * must be saved in order to ensure proper removal. */
              if(entry != BROAD_ENTRY_INVALID)
              {
                policyPtr->entry[i] = entry;
                policyPtr->entryCount++;
              }

              _cfp_policy_sem_give();
              (void)l7_bcm_cfp_policy_destroy(unit, policy);

              return rv;
          }
          /* restore srcEntry back to original rule number */
          rulePtr->meterSrcEntry = srcRule;

          /* Set the priority of the rule */
          rv = policy_cfp_group_rule_priority_set(unit, policyPtr->policyStage, group, entry, rulePtr->priority);
          if (BCM_E_NONE != rv)
          {
              _cfp_policy_sem_give();
              (void)l7_bcm_cfp_policy_destroy(unit, policy);
              return rv;
          }

          if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
          {
            /* must set pbm/mask for each entry */
            rv = policy_cfp_group_set_pbm(unit, policyPtr->policyStage, group, entry, policyPtr->pbm, policyPtr->pbmMask);
            if (BCM_E_NONE != rv)
            {
                _cfp_policy_sem_give();
                (void)l7_bcm_cfp_policy_destroy(unit, policy);
                return rv;
            }
          }
          else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
                   (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
          {
            if ((policyData->policyType == BROAD_POLICY_TYPE_PORT) ||
                (policyData->policyType == BROAD_POLICY_TYPE_IPSG) ||
                (policyData->policyType == BROAD_POLICY_TYPE_DVLAN))
            {
              rv = policy_cfp_group_set_portclass(unit, 
                                              policyPtr->policyStage,
                                              group,
                                              entry,
                                              policyPtr->pbm,
                                              policyPtr->portClass);
              if (BCM_E_NONE != rv)
              {
                _cfp_policy_sem_give();
                (void)l7_bcm_cfp_policy_destroy(unit, policy);
                return rv;
              }
            }
          }

          policyPtr->entry[i] = entry;
          policyPtr->entryCount++;

          rulePtr = rulePtr->next;
          i++;
      }
    }
    _cfp_policy_sem_give();

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d created: type %d, ruleCount %d, result %d\n",
               policy, policyData->policyType, 
               policyPtr->entryCount, rv);

    return rv;
}

int l7_bcm_cfp_policy_destroy(int unit, BROAD_POLICY_t policy)
{
    int                  tmprv, rv = BCM_E_NONE;
    int                  i;
    L7_short16           policyIdx;
    L7_short16           tempPolicyIdx;
    policy_map_table_t  *policyPtr;
    policy_map_table_t  *tempPolicyPtr;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    _cfp_policy_sem_take();

    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _cfp_policy_sem_give();
      return BCM_E_NONE;  
      /* 43084 - Do not return an error if policy not found. */
      /* return BCM_E_NOT_FOUND; */
    }

    policyPtr = &policy_map_table[unit][policyIdx];

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("destroy policy %d: group %d, ruleCount %d, ",
               policy, policyPtr->group, policyPtr->entryCount);

    policyPtr->flags      = GROUP_MAP_NONE;
    policy_map_index_map[unit][policy] = BROAD_POLICY_MAP_INVALID;

    /* remove each group entry used by policy */
    if (policyPtr->group != BROAD_GROUP_INVALID)
    {
      for (i = policyPtr->entryCount - 1; i >= 0 ; --i)
      {
          tmprv = policy_cfp_group_delete_rule(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[i]);
          if (BCM_E_NONE != tmprv)
              rv = tmprv;
      }

      /* Determine if any other policy is using this group. If not, then destroy it. */
      for (tempPolicyIdx = 0; tempPolicyIdx < BROAD_MAX_POLICIES_PER_BCM_UNIT; tempPolicyIdx++)
      {
        tempPolicyPtr = &policy_map_table[unit][tempPolicyIdx];
        if ((policyPtr != tempPolicyPtr) && (tempPolicyPtr->flags & GROUP_MAP_USED))
        {
          if ((tempPolicyPtr->group == policyPtr->group) &&
              (tempPolicyPtr->policyStage      == policyPtr->policyStage))
          {
            break;
          }
        }
      }
      if (tempPolicyIdx == BROAD_MAX_POLICIES_PER_BCM_UNIT)
      {
        /* indicate the group is no longer used by this policy */
        tmprv = policy_cfp_group_destroy(unit, policyPtr->policyStage, policyPtr->group);
        if (BCM_E_NONE != tmprv)
            rv = tmprv;
      }

      policyPtr->group = BROAD_GROUP_INVALID;
    }

    policyPtr->entryCount = 0;

    _cfp_policy_sem_give();

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("result %d\n", rv);

    return rv;
}

int l7_bcm_cfp_policy_apply(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
    int                  rv;
    policy_map_table_t  *policyPtr;
    bcm_pbmp_t           tempPbm;
    L7_short16           policyIdx;
    unsigned int         oldPortClass;
    bcm_port_t           tempBcmPort;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d apply port %d\n", policy, port);

    _cfp_policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _cfp_policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    BCM_PBMP_PORT_ADD(policyPtr->pbm, port); 

    oldPortClass = policyPtr->portClass; /* save the original portClass */
    rv = _policy_apply_to_ports(unit, policy);
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _cfp_policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      rv = policy_cfp_port_class_set(unit, port, policyPtr->policyStage, policyPtr->portClass);
      if (BCM_E_NONE != rv)
      {
        _cfp_policy_sem_give();
        return rv;
      }
      /* If the portClass has changed for this policy, then we need to update all of the
         other ports in this group. */
      if (oldPortClass != policyPtr->portClass)
      {
        BCM_PBMP_ITER(tempPbm, tempBcmPort)
        {
          rv = policy_cfp_port_class_set(unit, tempBcmPort, policyPtr->policyStage, policyPtr->portClass);
          if (BCM_E_NONE != rv)
          {
            _cfp_policy_sem_give();
            return rv;
          }
        }
      }
    }

    _cfp_policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_cfp_policy_apply_all(int unit, BROAD_POLICY_t policy)
{
    int                 rv;
    policy_map_table_t *policyPtr;
    bcm_port_t          port;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d apply all\n", policy);

    _cfp_policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _cfp_policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    BCM_PBMP_ASSIGN(policyPtr->pbm, PBMP_E_ALL(unit));

    rv = _policy_apply_to_ports(unit, policy);
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _cfp_policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      BCM_PBMP_ITER(policyPtr->pbm, port)
      {
        rv = policy_cfp_port_class_set(unit, port, policyPtr->policyStage, policyPtr->portClass);
        if (BCM_E_NONE != rv)
        {
          _cfp_policy_sem_give();
          return rv;
        }
      }
    }

    _cfp_policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_cfp_policy_remove(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
    int                 rv = BCM_E_NONE;
    policy_map_table_t *policyPtr;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;
    unsigned int        oldPortClass;
    bcm_port_t          tempBcmPort;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d remove port %d\n", policy, port);

    _cfp_policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _cfp_policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    BCM_PBMP_PORT_REMOVE(policyPtr->pbm, port);

    oldPortClass = policyPtr->portClass; /* save the original portClass */
    rv = _policy_apply_to_ports(unit, policy);
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _cfp_policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      rv = policy_cfp_port_class_set(unit, port, policyPtr->policyStage, BROAD_INVALID_PORT_CLASS);
      if (BCM_E_NONE != rv)
      {
        _cfp_policy_sem_give();
        return rv;
      }
      /* If the portClass has changed for this policy, then we need to update all of the
         remaining ports in this group. */
      if (oldPortClass != policyPtr->portClass)
      {
        BCM_PBMP_ITER(policyPtr->pbm, tempBcmPort)
        {
          rv = policy_cfp_port_class_set(unit, tempBcmPort, policyPtr->policyStage, policyPtr->portClass);
          if (BCM_E_NONE != rv)
          {
            _cfp_policy_sem_give();
            return rv;
          }
        }
      }
    }

    _cfp_policy_sem_give();
    return rv;
}

int l7_bcm_cfp_policy_remove_all(int unit, BROAD_POLICY_t policy)
{
    int                 rv;
    policy_map_table_t *policyPtr;
    bcm_port_t          port;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("policy %d remove all\n", policy);

    _cfp_policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _cfp_policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
    BCM_PBMP_CLEAR(policyPtr->pbm);

    rv = _policy_apply_to_ports(unit, policy);
    if (BCM_E_NONE != rv)
    {
      /* Restore the PBMP */
      BCM_PBMP_ASSIGN(policyPtr->pbm, tempPbm);
      _cfp_policy_sem_give();
      return rv;
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      BCM_PBMP_ITER(PBMP_E_ALL(unit), port)
      {
        rv = policy_cfp_port_class_set(unit, port, policyPtr->policyStage, BROAD_INVALID_PORT_CLASS);
        if (BCM_E_NONE != rv)
        {
          _cfp_policy_sem_give();
          return rv;
        }
      }
    }

    _cfp_policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_cfp_policy_stats(int unit, BROAD_POLICY_t policy, L7_uint32 ruleId, L7_uchar8 *buffer, L7_uint32 *buffer_size)
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
  CHECK_RULE(ruleId);

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
    sysapiPrintf("get policy %d rule %d stats\n", policy, ruleId);

  _cfp_policy_sem_take();
  policyIdx = policy_map_index_map[unit][policy];
  if (policyIdx == BROAD_POLICY_MAP_INVALID)
  {
    _cfp_policy_sem_give();
    return BCM_E_NONE;
  }
  policyPtr = &policy_map_table[unit][policyIdx];

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
    tmprv = policy_cfp_group_get_stats(unit, policyPtr->policyStage, policyPtr->group, policyPtr->entry[ruleId],
                                   &val1, &val2);

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

  _cfp_policy_sem_give();
  return rv;
}

int l7_bcm_cfp_policy_counter_clear(int unit, BROAD_POLICY_t policy)
{
    int                  i;
    int                  rv = BCM_E_NONE;
    policy_map_table_t  *policyPtr;
    L7_short16           policyIdx;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        sysapiPrintf("clear counters policy %d\n", policy);

    _cfp_policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _cfp_policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    /* retrieve count for each entry in the policy */
    for (i = 0; i < policyPtr->entryCount; i++)
    {
        int    tmprv;

        /* query stats for entry -- in case of error this routine returns 0 */
        tmprv = policy_cfp_group_stats_clear(unit, policyPtr->policyStage, policyPtr->entry[i]);
        if (tmprv < rv)
            rv = tmprv;     /* just get the worst error code */
    }

    _cfp_policy_sem_give();
    return rv;
}

void l7_bcm_cfp_policy_dataplane_cleanup(L7_uint32 unit, L7_uint32 policy, L7_uint32 rule, L7_uint32 cpu_modid, L7_uint32 cpu_modport)
{
  policy_map_table_t  *policyPtr;
  L7_short16           policyIdx;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      sysapiPrintf("new mgr policy %d, rule %d, modid %d, modport %d\n", policy, rule, cpu_modid, cpu_modport);

  _cfp_policy_sem_take();

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
    policy_cfp_group_dataplane_cleanup(unit, policyPtr->entry[rule], cpu_modid, cpu_modport);

  } while (0);

  _cfp_policy_sem_give();
}

/* Debug */

void debug_cfp_policy_table(int unit)
{
  int i, j;
  char policyStageString[BROAD_POLICY_STAGE_COUNT][10] = {"Lookup","Ingress","Egress"};
  L7_short16 policyIdx;
  policy_map_table_t *policyPtr;

  sysapiPrintf("Unit %d\n", unit);

  sysapiPrintf("Policy    Group         PBM     MASK     EntryCount  Entries\n");
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    policyIdx = policy_map_index_map[unit][i];
    if (policyIdx != BROAD_POLICY_MAP_INVALID)
    {
      policyPtr = &policy_map_table[unit][policyIdx];
      if (policyPtr->group != BROAD_GROUP_INVALID)
      {
        sysapiPrintf("[%2d]       %2d %7s   %08x %08x %02d         [",
               i, policyPtr->group, policyStageString[policyPtr->policyStage],
               policyPtr->pbm.pbits[0],
               (int)policyPtr->pbmMask.pbits[0],
               (int)policyPtr->entryCount);
        for (j = 0; j < policyPtr->entryCount; j++)
          sysapiPrintf("%d ", policyPtr->entry[j]);
        sysapiPrintf("]\n");
      }
    }
  }

  if (policy_cfp_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP))
  {
    sysapiPrintf("Lookup Stage:\n");
    sysapiPrintf("Port Class    PBM         Policies\n");
    for (i = 1; i < BROAD_MAX_PORT_CLASS_ID; i++)
    {
      sysapiPrintf("[%2d]          %08x    ",i, port_class_table[unit][BROAD_POLICY_STAGE_LOOKUP][i].pbm.pbits[0]);
      if (BCM_PBMP_NOT_NULL(port_class_table[unit][BROAD_POLICY_STAGE_LOOKUP][i].pbm))
      {
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
      }
      sysapiPrintf("\n");
    }
  }
  if (policy_cfp_stage_supported(unit, BROAD_POLICY_STAGE_EGRESS))
  {
    sysapiPrintf("Egress Stage:\n");
    sysapiPrintf("Port Class    PBM\n");
    for (i = 1; i < BROAD_MAX_PORT_CLASS_ID; i++)
    {
      sysapiPrintf("[%2d]          %08x    ",i, port_class_table[unit][BROAD_POLICY_STAGE_EGRESS][i].pbm.pbits[0]);
      if (BCM_PBMP_NOT_NULL(port_class_table[unit][BROAD_POLICY_STAGE_EGRESS][i].pbm))
      {
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
      }
      sysapiPrintf("\n");
    }
  }
}

int cfp_policy_map_table_t_size()
{
  return sizeof(policy_map_table_t);
}
