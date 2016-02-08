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


#include "broad_group_bcm.h"
#include "broad_group_xgs3.h"
#include "ibde.h"
#include "bcm/custom.h"
#include "flex.h"
#include "acl_exports.h"


#if L7_FEAT_EGRESS_ACL_ON_IFP
/* Number of bytes in mask */
#define ENTRY_DENY_INDICES   ((BROAD_MAX_RULES_PER_POLICY - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Interface storage */
typedef struct
{
  L7_uchar8 value[ENTRY_DENY_INDICES];
} ENTRY_DENY_MASK_t;

/* SETMASKBIT turns on bit index # k in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_SETMASKBIT(j, k)                                    \
            ((j->denyMask).value[((k)/(8*sizeof(L7_uchar8)))] \
                         |= 1 << ((k) % (8*sizeof(L7_uchar8))))   

/* CLRMASK turns off all bits in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_CLRMASK(j)                                    \
           memset((j->denyMask).value, 0, ENTRY_DENY_INDICES)

/* ISMASKBITSET returns 0 if the bit k is not set in denyMask of policy_map_table_t entry 'j' */
#define ENTRY_DENY_ISMASKBITSET(j, k)                               \
        ((j->denyMask).value[((k)/(8*sizeof(L7_uchar8)))]  \
                         & ( 1 << ((k) % (8*sizeof(L7_char8)))) )
#else
/* SETMASKBIT turns on bit index # k in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_SETMASKBIT(j, k)

/* CLRMASK turns off all bits in denyMask of policy_map_table_t entry 'j'. */
#define ENTRY_DENY_CLRMASK(j)

/* ISMASKBITSET returns 0 if the bit k is not set in denyMask of policy_map_table_t entry 'j' */
#define ENTRY_DENY_ISMASKBITSET(j, k) 0
#endif

/* Policy Group Map Table Definitions */
/* This table contains information used to map a policy to multiple entries
 * within a group. It also contains a bitmap of the ports to which this
 * policy applies.
 */

#define GROUP_MAP_NONE                   0
#define GROUP_MAP_USED                   1
#define GROUP_MAP_EFP_ON_IFP             2 /* Used to indicate that the policy is configured for egress filtering
                                              even though it is applied on ingress. */
#define GROUP_MAP_REQUIRES_IMPLICIT_DENY 4 /* This policy is configured for EFP on IFP and requires an implicit deny rule. */

/* Allow policies at this level to include multiple groups (slices)
   to allow the user to combine L2, IPv4, and IPv6 classifications 
   into a single policy. */ 
typedef enum
{
  BROAD_POLICY_GROUP_L2_IPV4 = 0, /* These groups can classify on either L2 or IPv4 fields */
  BROAD_POLICY_GROUP_IPV6,        /* These groups can classify on IPv6 fields */
  BROAD_POLICY_GROUP_COUNT
} BROAD_POLICY_GROUP_t;

typedef struct
{
#if (BROAD_MAX_RULES_PER_POLICY < 256)
  unsigned char userEntryIndex[BROAD_POLICY_GROUP_COUNT][BROAD_MAX_RULES_PER_POLICY];
#else
  short userEntryIndex[BROAD_POLICY_GROUP_COUNT][BROAD_MAX_RULES_PER_POLICY]; /* This field maps back to the 
                                                                                 original rule index when the 
                                                                                 policy was applied. This is 
                                                                                 necessary because the order
                                                                                 of stats needs to match
                                                                                 the order of the rules when
                                                                                 they were added. */
#endif
} rule_order_t;

typedef struct
{
    BROAD_GROUP_t        group[BROAD_POLICY_GROUP_COUNT];
    BROAD_ENTRY_t        entry[BROAD_POLICY_GROUP_COUNT][BROAD_MAX_RULES_PER_POLICY];
    bcm_pbmp_t           pbm;
    unsigned char        flags;
    BROAD_POLICY_STAGE_t policyStage;
    unsigned char        portClass; /* Used for LOOKUP and EGRESS port classes */
#if (BROAD_MAX_RULES_PER_POLICY < 256)
    unsigned char        entryCount[BROAD_POLICY_GROUP_COUNT];
#else
    unsigned short       entryCount[BROAD_POLICY_GROUP_COUNT];
#endif
    rule_order_t         ruleOrder;
#if L7_FEAT_EGRESS_ACL_ON_IFP
    ENTRY_DENY_MASK_t    denyMask;  /* Each bit indicates whether the corresponding policy rule has a 
                                       deny action. Only used for EFP on IFP. */
#endif
}
policy_map_table_t;

static policy_map_table_t *policy_map_table[SOC_MAX_NUM_DEVICES] = {0};     /* This structure only holds data for the policies local to a BCM unit. */
static L7_short16         *policy_map_index_map[SOC_MAX_NUM_DEVICES] = {0}; /* This structure maps global policy IDs to an index into policy_map_table. */

BROAD_POLICY_ENTRY_t policyPartialData[BROAD_POLICY_GROUP_COUNT];
void *_policy_sem = L7_NULL;

#define CHECK_POLICY(p)  {if (p >= BROAD_MAX_POLICIES)  LOG_ERROR(p);}
#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) LOG_ERROR(u);}

/* For outbound policies only.*/
typedef struct
{
    bcm_pbmp_t            unitEpbm; /* Egress mask for the entire unit. */
    BROAD_ENTRY_t         entry;    /* Entry for the unit default rule */
} policy_efp_on_ifp_table_t;

static policy_efp_on_ifp_table_t policy_efp_on_ifp_table[SOC_MAX_NUM_DEVICES];

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
    bcm_pbmp_t           tempPbm;

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

    /* Before allocating a new portClass, check for conflicts. A port can 
       belong to only a single portClass.  A port can belong to multiple
       policies as long as the policy is applied only to one port (i.e. 
       the InPort/OutPort will be used as a qualifier instead of the 
       portClass. */
    for (tempPolicyIdx = 0; tempPolicyIdx < BROAD_MAX_POLICIES_PER_BCM_UNIT; tempPolicyIdx++)
    {
      tempPolicyPtr = &policy_map_table[unit][tempPolicyIdx];
      if ((tempPolicyPtr != policyPtr) && 
          (tempPolicyPtr->flags & GROUP_MAP_USED) && 
          (tempPolicyPtr->policyStage == policyPtr->policyStage))
      {
        BCM_PBMP_COUNT(tempPolicyPtr->pbm, numPorts);
        /* If the port count is greater than one that means the policy uses a portClass. */
        if (numPorts > 1)
        {
          BCM_PBMP_ASSIGN(tempPbm, policyPtr->pbm);
          BCM_PBMP_AND(tempPbm, tempPolicyPtr->pbm);

          /* Return an error if a port exists in both bitmaps. */
          if (BCM_PBMP_NOT_NULL(tempPbm))
          {
            return BCM_E_CONFIG;
          }
        }
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

static void _policy_sem_take()
{
  if (_policy_sem == L7_NULL)
  {
    LOG_ERROR(0);
  }

  osapiSemaTake(_policy_sem, L7_WAIT_FOREVER);
}

static void _policy_sem_give()
{
  if (_policy_sem == L7_NULL)
  {
    LOG_ERROR(0);
  }

  osapiSemaGive(_policy_sem);
}

/* BCM Functions */

static void _policy_group_set_default_pbm(int unit, BROAD_POLICY_TYPE_t type, BROAD_POLICY_STAGE_t stage, policy_map_table_t *policyPtr)
{
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
    case BROAD_POLICY_TYPE_SYS_EXT:   /*  PTin added: Policies */
    case BROAD_POLICY_TYPE_ISCSI:
        /* by default applies to all non-stacking ports */
        SOC_PBMP_ASSIGN(policyPtr->pbm, PBMP_E_ALL(unit));
        break;
    case BROAD_POLICY_TYPE_PORT:
    case BROAD_POLICY_TYPE_IPSG:
    case BROAD_POLICY_TYPE_SYSTEM_PORT:
    case BROAD_POLICY_TYPE_COSQ:
        /* by default does not apply to any ports */
        BCM_PBMP_CLEAR(policyPtr->pbm);
        break;
    default:
        /* indicate that a type was missed */
        LOG_ERROR(type);
        break;
    }        
}

static void _policy_table_init(int unit)
{
    int p;
    BROAD_POLICY_GROUP_t groupType;

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

    if (L7_NULLPTR == _policy_sem)
    {
      _policy_sem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    }

    if (L7_NULLPTR == _policy_sem)
    {
        LOG_ERROR(0);
        return;
    }

    _policy_sem_take();
    /* initialize the policy to group mapping table */
    for (p = 0; p < BROAD_MAX_POLICIES_PER_BCM_UNIT; p++)
    {
        policy_map_table[unit][p].flags = GROUP_MAP_NONE;
        for (groupType = 0; groupType < BROAD_POLICY_GROUP_COUNT; groupType++)
        {
          policy_map_table[unit][p].group[groupType]      = BROAD_GROUP_INVALID;
          policy_map_table[unit][p].entryCount[groupType] = 0;
        }
    }

    memset(policy_map_index_map[unit], BROAD_POLICY_MAP_INVALID, BROAD_MAX_POLICIES * sizeof(L7_short16));

    policy_efp_on_ifp_table[unit].entry = BROAD_ENTRY_INVALID;
    SOC_PBMP_CLEAR(policy_efp_on_ifp_table[unit].unitEpbm);

    _policy_sem_give();
}

int l7_bcm_policy_init()
{
    int unit;
    int tmprv, rv = BCM_E_NONE;

#ifdef BCM_ROBO_SUPPORT
    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
#else
    for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++)
#endif
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

void _policy_user_policy_split(BROAD_POLICY_ENTRY_t *policyData,
                               BROAD_POLICY_ENTRY_t *policyPartialData,
                               rule_order_t         *ruleOrder)
{
  int i;
  int idx;
  BROAD_POLICY_GROUP_t groupType;
  L7_BOOL ipv6QualifierSpecified = L7_FALSE;

  for (groupType = 0; groupType < BROAD_POLICY_GROUP_COUNT; groupType++)
  {
    memset(&policyPartialData[groupType], 0, sizeof(BROAD_POLICY_ENTRY_t));
    policyPartialData[groupType].policyType  = policyData->policyType;
    policyPartialData[groupType].policyFlags = policyData->policyFlags;
  }

  /* Find all the IPv6 rules and put them into one policy structure and put 
     everything else in the other policy structure. For rules that qualify 
     on Ethertype == 0x86dd, we need to determine if this should
     qualify as a 'IPv6' rule or a 'L2_IPv4' rule. If any rule in the 
     policy has an IPv6 qualifer (SIP, DIP, nextheader, etc), then
     treat rules that qualify on Ethertype == 0x86dd as 'IPv6',
     otherwise treat them as 'L2_IPv4'. */
  /* For system policies, don't split the policy into V4/V6 policies. */
  if (policyData->policyType != BROAD_POLICY_TYPE_SYSTEM &&
      policyData->policyType != BROAD_POLICY_TYPE_SYS_EXT )     /*  PTin added: Policies */
  {
    for (i = 0; i < policyData->ruleCount; i++)
    {
      if ((hapiBroadPolicyFieldFlagsGet(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_IP6_HOPLIMIT) == BROAD_FIELD_SPECIFIED) ||
          (hapiBroadPolicyFieldFlagsGet(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_IP6_NEXTHEADER) == BROAD_FIELD_SPECIFIED) ||
          (hapiBroadPolicyFieldFlagsGet(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_IP6_SRC) == BROAD_FIELD_SPECIFIED) ||
          (hapiBroadPolicyFieldFlagsGet(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_IP6_DST) == BROAD_FIELD_SPECIFIED) ||
          (hapiBroadPolicyFieldFlagsGet(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_IP6_FLOWLABEL) == BROAD_FIELD_SPECIFIED) ||
          (hapiBroadPolicyFieldFlagsGet(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_IP6_TRAFFIC_CLASS) == BROAD_FIELD_SPECIFIED))
      {
        ipv6QualifierSpecified = L7_TRUE;
        break;
      }
    }
  }

  for (i = 0; i < policyData->ruleCount; i++)
  {
    if (hapiBroadPolicyFieldFlagsGet(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_ETHTYPE) == BROAD_FIELD_SPECIFIED)
    {
      L7_uchar8 *value;

      value = hapiBroadPolicyFieldValuePtr(&policyData->ruleInfo[i].fieldInfo, BROAD_FIELD_ETHTYPE);

      if ((value[0] == 0x86) &&
          (value[1] == 0xdd))
      {
        if (ipv6QualifierSpecified == L7_TRUE)
        {
          idx = policyPartialData[BROAD_POLICY_GROUP_IPV6].ruleCount;

          memcpy(&policyPartialData[BROAD_POLICY_GROUP_IPV6].ruleInfo[idx], 
                 &policyData->ruleInfo[i], 
                 sizeof(BROAD_POLICY_RULE_ENTRY_t));

          ruleOrder->userEntryIndex[BROAD_POLICY_GROUP_IPV6][idx] = i;
          policyPartialData[BROAD_POLICY_GROUP_IPV6].ruleCount++;
          continue;
        }
      }
    }

    idx = policyPartialData[BROAD_POLICY_GROUP_L2_IPV4].ruleCount;

    memcpy(&policyPartialData[BROAD_POLICY_GROUP_L2_IPV4].ruleInfo[idx], 
           &policyData->ruleInfo[i], 
           sizeof(BROAD_POLICY_RULE_ENTRY_t));

    ruleOrder->userEntryIndex[BROAD_POLICY_GROUP_L2_IPV4][idx] = i;
    policyPartialData[BROAD_POLICY_GROUP_L2_IPV4].ruleCount++;
  }

  return;
}

static int _policy_apply_to_ports(int unit, BROAD_POLICY_t policy)
{
  int                  i;
  int                  rv = BCM_E_NONE;
  policy_map_table_t  *policyPtr;
  BROAD_POLICY_GROUP_t groupType;
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

  for (groupType = 0; groupType < BROAD_POLICY_GROUP_COUNT; groupType++)
  {
    /* add port to each entry used by policy */
    for (i = 0; i < policyPtr->entryCount[groupType]; i++)
    {
      if (policyPtr->policyStage == BROAD_POLICY_STAGE_INGRESS)
      {
        rv = policy_group_set_pbm(unit, policyPtr->policyStage, policyPtr->group[groupType], policyPtr->entry[groupType][i],
                                  policyPtr->pbm);
        if (BCM_E_NONE != rv)
            return rv;
      }
      else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
               (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
      {
        if ((oldPortClass != policyPtr->portClass) || (policyPtr->portClass == BROAD_INVALID_PORT_CLASS))
        {
          rv = policy_group_set_portclass(unit, 
                                          policyPtr->policyStage, 
                                          policyPtr->group[groupType], 
                                          policyPtr->entry[groupType][i],
                                          policyPtr->pbm,
                                          policyPtr->portClass);
          if (BCM_E_NONE != rv)
              return rv;
        }
      }
    }
  }

  return rv;
}

static int _policy_apply_egress_mask(int unit, policy_map_table_t *policyPtr, bcm_pbmp_t affectedPortsPbm, BROAD_POLICY_EPBM_CMD_t action)
{
  policy_map_table_t         *tmpPolicyPtr;
  policy_efp_on_ifp_table_t  *globalPolicyPtr;
  bcm_pbmp_t                  permitPbm, denyPbm;
  int                         i, p;
  int                         rv = BCM_E_NONE;

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

  for (i = 0; i < policyPtr->entryCount[BROAD_POLICY_GROUP_L2_IPV4]; i++)
  {
      rv = policy_group_set_epbm(unit, policyPtr->group[BROAD_POLICY_GROUP_L2_IPV4], policyPtr->entry[BROAD_POLICY_GROUP_L2_IPV4][i],
                                 (ENTRY_DENY_ISMASKBITSET(policyPtr, i)) ? denyPbm : permitPbm, 
                                 BROAD_POLICY_EPBM_CMD_ASSIGN);
      if (BCM_E_NONE != rv)
          return rv;
  }

  /* Update all the outbound policy rules on this unit masking out this port */
  if (policyPtr->flags & GROUP_MAP_REQUIRES_IMPLICIT_DENY) 
  {
      for (p = 0; p < BROAD_MAX_POLICIES_PER_BCM_UNIT; p++) 
      {
          tmpPolicyPtr = &policy_map_table[unit][p];
          if (((tmpPolicyPtr->flags & (GROUP_MAP_USED | GROUP_MAP_EFP_ON_IFP)) == (GROUP_MAP_USED | GROUP_MAP_EFP_ON_IFP)) && 
              (tmpPolicyPtr->group[BROAD_POLICY_GROUP_L2_IPV4] == policyPtr->group[BROAD_POLICY_GROUP_L2_IPV4]) &&
              (tmpPolicyPtr != policyPtr))
          {
              for (i = 0; i < tmpPolicyPtr->entryCount[BROAD_POLICY_GROUP_L2_IPV4]; i++)
              {
                  rv = policy_group_set_epbm(unit, tmpPolicyPtr->group[BROAD_POLICY_GROUP_L2_IPV4], 
                                             tmpPolicyPtr->entry[BROAD_POLICY_GROUP_L2_IPV4][i],
                                             affectedPortsPbm, 
                                             action);
                  if (BCM_E_NONE != rv)
                      return rv;
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
      policy_group_delete_rule(unit, BROAD_POLICY_STAGE_INGRESS, policyPtr->group[BROAD_POLICY_GROUP_L2_IPV4], globalPolicyPtr->entry);
      globalPolicyPtr->entry = BROAD_ENTRY_INVALID;
  }

  if (BCM_PBMP_NOT_NULL(globalPolicyPtr->unitEpbm))
  {
    rv = policy_group_create_default_rule(unit, policyPtr->group[BROAD_POLICY_GROUP_L2_IPV4], 
                                          globalPolicyPtr->unitEpbm,      
                                         &(globalPolicyPtr->entry));
  }

  return rv;

}

int l7_bcm_policy_create(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData)
{
    int                 i;
    BROAD_GROUP_t       group;
    BROAD_ENTRY_t       entry;
    policy_map_table_t *policyPtr = L7_NULL;
    int                 savePbm = FALSE;
    bcm_pbmp_t          savedPbm;
    int                 rv = BCM_E_NONE;
    BROAD_POLICY_GROUP_t groupType;
    rule_order_t         ruleOrder;
    L7_short16           policyIdx;
    L7_uint32            actions, actionMask;
    L7_ushort16          ethType;
    policy_efp_on_ifp_table_t  *globalPolicyPtr;
    BROAD_ACTION_ENTRY_t        *actionPtr;

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
            printf("policy changed, delete and re-add\n");

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
            printf("policy not applicable, ignore\n");
        return BCM_E_NONE;
    }
    else
    {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            printf("create policy %d\n", policy);
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
          printf("couldn't allocate local BCM policy\n");
      return BCM_E_RESOURCE;
    }
    policy_map_index_map[unit][policy] = policyIdx;

    globalPolicyPtr = &policy_efp_on_ifp_table[unit];

    policyPtr->flags       = GROUP_MAP_USED;
    policyPtr->policyStage = policyData->policyStage;
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

    /* split the policyData into two sets of policyData... one for L2/IPv4 and one for
       IPv6 */
    _policy_user_policy_split(policyData, &policyPartialData[0], &ruleOrder);


    if (FALSE == savePbm)
    {
        _policy_group_set_default_pbm(unit, policyData->policyType, policyData->policyStage, policyPtr);
    }
    else
    {
        SOC_PBMP_ASSIGN(policyPtr->pbm, savedPbm);
    }

    if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
        (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
    {
      if ((policyData->policyType == BROAD_POLICY_TYPE_PORT) ||
          (policyData->policyType == BROAD_POLICY_TYPE_IPSG))
      {
        rv = _policy_alloc_portclass(unit, policy);
        if (BCM_E_NONE != rv)
        {
          _policy_sem_give();
          (void)l7_bcm_policy_destroy(unit, policy);
          return rv;
        }
      }
    }

    if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
    {
      ENTRY_DENY_CLRMASK(policyPtr);
    }

    for (groupType = 0; groupType < BROAD_POLICY_GROUP_COUNT; groupType++)
    {
      if (policyPartialData[groupType].ruleCount > 0)
      {
        rv = policy_group_create(unit, policyPtr->policyStage, &policyPartialData[groupType], &group);
        if (BCM_E_NONE != rv)
        {
          _policy_sem_give();
          (void)l7_bcm_policy_destroy(unit, policy);
          return rv;
        }

        /* populate the group mapping table */
        policyPtr->group[groupType]      = group;
        policyPtr->entryCount[groupType] = 0;

        if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
        {
          /* Delete the default deny rule now. It will be added back later after this policy's rules
             are put in HW. */
          if (globalPolicyPtr->entry != BROAD_ENTRY_INVALID) 
          {
              policy_group_delete_rule(unit, policyPtr->policyStage, policyPtr->group[groupType], globalPolicyPtr->entry);
              globalPolicyPtr->entry = BROAD_ENTRY_INVALID;
          }
        }

        /* add new rules to policy */
        for (i = policyPtr->entryCount[groupType]; i < policyPartialData[groupType].ruleCount; i++)
        {
            int srcRule;
            entry = BROAD_ENTRY_INVALID;

            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                printf("- add rule %d\n", i);

            /* For EFP rules on IFP, ensure that only valid actions of permit/deny are allowed */
            if (policyPtr->flags & GROUP_MAP_EFP_ON_IFP)
            {
              actions = policyPartialData[groupType].ruleInfo[i].actionInfo.actions[BROAD_POLICY_ACTION_CONFORMING];
              actionMask = ~((1 << BROAD_ACTION_HARD_DROP) | (1 << BROAD_ACTION_PERMIT));
              if (actions & actionMask)
              {
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                    printf("- invalid action in rule %d\n", i);

                _policy_sem_give();
                (void)l7_bcm_policy_destroy(unit, policy);

                return BCM_E_FAIL;
              }

              /* Check that only valid qualifiers of IPv4/Layer 4 are allowed and that
                 at least one qualifier is present. The only time there would be no 
                 qualifiers is for the implicit deny rule of a MAC ACL, which we want to
                 forbid. */
              if ((policyPartialData[groupType].ruleInfo[i].fieldInfo.flags & 
                  ~((1 << BROAD_FIELD_ETHTYPE)  | 
                    (1 << BROAD_FIELD_DSCP)     | 
                    (1 << BROAD_FIELD_PROTO)    | 
                    (1 << BROAD_FIELD_SIP)      | 
                    (1 << BROAD_FIELD_DIP)      | 
                    (1 << BROAD_FIELD_SPORT)    | 
                    (1 << BROAD_FIELD_DPORT)    |
                    (1 << BROAD_FIELD_IP_TYPE)  |
                    (1 << BROAD_FIELD_IP6_DST)  |
                    (1 << BROAD_FIELD_INPORTS)  |     /* PTin added */
                    (1 << BROAD_FIELD_DROP)))  ||     /* PTin added */
                   (policyPartialData[groupType].ruleInfo[i].fieldInfo.flags == 0))
              {
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                    printf("- invalid qualifier in rule %d\n", i);

                _policy_sem_give();
                (void)l7_bcm_policy_destroy(unit, policy);

                return BCM_E_FAIL;
              }

              /* Check for an implicit deny rule. If present, skip adding this rule since there 
                 will be a single deny rule used for the group performing EFP on IFP. */
              actionPtr = &(policyPartialData[groupType].ruleInfo[i].actionInfo);
              if (policyPartialData[groupType].ruleInfo[i].fieldInfo.flags == (1 << BROAD_FIELD_ETHTYPE))
              {
                memcpy(&ethType, policyPartialData[groupType].ruleInfo[i].fieldInfo.fieldEthtype.value, sizeof(ethType));
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
            srcRule = policyPartialData[groupType].ruleInfo[i].meterSrcEntry;
            policyPartialData[groupType].ruleInfo[i].meterSrcEntry = policyPtr->entry[groupType][srcRule];

            rv = policy_group_add_rule(unit, 
                                       policyPtr->policyStage, 
                                       policyData->policyType, 
                                       group, 
                                       &policyPartialData[groupType].ruleInfo[i], 
                                       policyPtr->pbm, 
                                       policyPtr->flags & GROUP_MAP_EFP_ON_IFP, 
                                       &entry);
            if(entry != BROAD_ENTRY_INVALID)
            {
              policyPtr->entry[groupType][i] = entry;
              policyPtr->entryCount[groupType]++;
              policyPtr->ruleOrder.userEntryIndex[groupType][i] = ruleOrder.userEntryIndex[groupType][i];
            }
            if (BCM_E_NONE != rv)
            {
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                    printf("- add rule failed\n");

                _policy_sem_give();
                (void)l7_bcm_policy_destroy(unit, policy);
                return rv;
            }

            /* restore srcEntry back to original rule number */
            policyPartialData[groupType].ruleInfo[i].meterSrcEntry = srcRule;

            /* Set the priority of the rule */
            rv = policy_group_rule_priority_set(unit, policyPtr->policyStage, group, entry, policyPartialData[groupType].ruleInfo[i].priority);
            if (BCM_E_NONE != rv)
            {
              if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                  printf("- priority set failed\n");

                _policy_sem_give();
                (void)l7_bcm_policy_destroy(unit, policy);
                return rv;
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
              // PTin modified (else ...)
              else if ( !( (policyPartialData[groupType].ruleInfo[i].fieldInfo.flags >> BROAD_FIELD_INPORTS) & 1) )
              {
                
                rv = policy_group_set_pbm(unit, policyPtr->policyStage, group, entry, policyPtr->pbm);
              }
              if (BCM_E_NONE != rv)
              {
                  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                      printf("- pbm set failed\n");

                  _policy_sem_give();
                  (void)l7_bcm_policy_destroy(unit, policy);
                  return rv;
              }
            }
            else if ((policyPtr->policyStage == BROAD_POLICY_STAGE_LOOKUP) ||
                     (policyPtr->policyStage == BROAD_POLICY_STAGE_EGRESS))
            {
              if ((policyData->policyType == BROAD_POLICY_TYPE_PORT) ||
                  (policyData->policyType == BROAD_POLICY_TYPE_IPSG))
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
        }
      }
    }
    _policy_sem_give();

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d created: type %d, ruleCount{L2_IPV4} %d, ruleCount{IPV6} %d, result %d\n",
               policy, policyData->policyType, 
               policyPtr->entryCount[BROAD_POLICY_GROUP_L2_IPV4], 
               policyPtr->entryCount[BROAD_POLICY_GROUP_IPV6], rv);

    return rv;
}

int l7_bcm_policy_destroy(int unit, BROAD_POLICY_t policy)
{
    int                  tmprv, rv = BCM_E_NONE;
    int                  i;
    L7_short16           policyIdx;
    L7_short16           tempPolicyIdx;
    policy_map_table_t  *policyPtr;
    policy_map_table_t  *tempPolicyPtr;
    BROAD_POLICY_GROUP_t groupType;
    L7_BOOL              isEfpOnIfpPolicy;
    policy_efp_on_ifp_table_t  *globalPolicyPtr;

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
        printf("destroy policy %d: {L2_IPV4} group %d, ruleCount %d, {IPV6} group %d, ruleCount %d, ",
               policy, 
               policyPtr->group[BROAD_POLICY_GROUP_L2_IPV4], policyPtr->entryCount[BROAD_POLICY_GROUP_L2_IPV4], 
               policyPtr->group[BROAD_POLICY_GROUP_IPV6], policyPtr->entryCount[BROAD_POLICY_GROUP_IPV6]);

    isEfpOnIfpPolicy = (policyPtr->flags & GROUP_MAP_EFP_ON_IFP) ? L7_TRUE : L7_FALSE;
    policyPtr->flags      = GROUP_MAP_NONE;
    policy_map_index_map[unit][policy] = BROAD_POLICY_MAP_INVALID;

    for (groupType = 0; groupType < BROAD_POLICY_GROUP_COUNT; groupType++)
    {
      /* remove each group entry used by policy */
      if (policyPtr->group[groupType] != BROAD_GROUP_INVALID)
      {
        for (i = policyPtr->entryCount[groupType] - 1; i >= 0 ; --i)
        {
            tmprv = policy_group_delete_rule(unit, policyPtr->policyStage, policyPtr->group[groupType], policyPtr->entry[groupType][i]);
            if (BCM_E_NONE != tmprv)
                rv = tmprv;
        }

        /* Determine if any other policy is using this group. If not, then destroy it. */
        for (tempPolicyIdx = 0; tempPolicyIdx < BROAD_MAX_POLICIES_PER_BCM_UNIT; tempPolicyIdx++)
        {
          tempPolicyPtr = &policy_map_table[unit][tempPolicyIdx];
          if ((policyPtr != tempPolicyPtr) && (tempPolicyPtr->flags & GROUP_MAP_USED))
          {
            if ((tempPolicyPtr->group[groupType] == policyPtr->group[groupType]) &&
                (tempPolicyPtr->policyStage      == policyPtr->policyStage))
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
                policy_group_delete_rule(unit, BROAD_POLICY_STAGE_INGRESS, policyPtr->group[groupType], globalPolicyPtr->entry);
                globalPolicyPtr->entry = BROAD_ENTRY_INVALID;
            }
          }
          tmprv = policy_group_destroy(unit, policyPtr->policyStage, policyPtr->group[groupType]);
          if (BCM_E_NONE != tmprv)
              rv = tmprv;
        }

        policyPtr->group[groupType] = BROAD_GROUP_INVALID;
      }

      policyPtr->entryCount[groupType] = 0;
    }
    _policy_sem_give();

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("result %d\n", rv);

    return rv;
}

int l7_bcm_policy_check(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
    policy_map_table_t *policyPtr;
    L7_short16          policyIdx;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    _policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    if(BCM_PBMP_MEMBER(policyPtr->pbm, port))
    {
      _policy_sem_give();
      return BCM_E_EXISTS;
    }

    _policy_sem_give();
    return BCM_E_NOT_FOUND;
}

int l7_bcm_policy_apply(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
    int                  rv;
    policy_map_table_t  *policyPtr;
    bcm_pbmp_t           tempPbm;
    L7_short16           policyIdx;
    unsigned int         oldPortClass = BROAD_INVALID_PORT_CLASS;
    bcm_port_t           tempBcmPort;
    bcm_pbmp_t           affectedPortsPbm;
    L7_BOOL              portClassConflict;
    unsigned int         i;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d apply port %d\n", policy, port);

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
      rv = _policy_apply_egress_mask(unit, policyPtr, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_ADD);
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
      /* Check to see if any other portClasses include this port */
      portClassConflict = L7_FALSE;
      for (i = 1; i < BROAD_MAX_PORT_CLASS_ID; i++)
      {
        if (BCM_PBMP_MEMBER(port_class_table[unit][policyPtr->policyStage][i].pbm, port) && (i != policyPtr->portClass))
        {
          portClassConflict = L7_TRUE;
        }
      }

      /* Only set the portClass if this port is not a member of another portClass. */
      if (portClassConflict == L7_FALSE)
      {
        rv = policy_port_class_set(unit, port, policyPtr->policyStage, policyPtr->portClass);
        if (BCM_E_NONE != rv)
        {
          _policy_sem_give();
          return rv;
        }
      }
      /* If the portClass has changed for this policy, then we need to update all of the
         other ports in this group. */
      if (oldPortClass != policyPtr->portClass)
      {
        BCM_PBMP_ITER(tempPbm, tempBcmPort)
        {
          rv = policy_port_class_set(unit, tempBcmPort, policyPtr->policyStage, policyPtr->portClass);
          if (BCM_E_NONE != rv)
          {
            _policy_sem_give();
            return rv;
          }
        }
      }
    }

    _policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_policy_apply_all(int unit, BROAD_POLICY_t policy)
{
    int                 rv;
    policy_map_table_t *policyPtr;
    bcm_port_t          port;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;
    bcm_pbmp_t          affectedPortsPbm;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d apply all\n", policy);

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
      rv = _policy_apply_egress_mask(unit, policyPtr, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_ADD);
    }
    else
    {
      BCM_PBMP_ASSIGN(policyPtr->pbm, PBMP_E_ALL(unit));

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
      BCM_PBMP_ITER(policyPtr->pbm, port)
      {
        rv = policy_port_class_set(unit, port, policyPtr->policyStage, policyPtr->portClass);
        if (BCM_E_NONE != rv)
        {
          _policy_sem_give();
          return rv;
        }
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
    bcm_port_t          tempBcmPort; 
    bcm_pbmp_t          affectedPortsPbm;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d remove port %d\n", policy, port);

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
      rv = _policy_apply_egress_mask(unit, policyPtr, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_REMOVE);
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
      rv = policy_port_class_set(unit, port, policyPtr->policyStage, BROAD_INVALID_PORT_CLASS);
      if (BCM_E_NONE != rv)
      {
        _policy_sem_give();
        return rv;
      }
      /* If the portClass has changed for this policy, then we need to update all of the
         remaining ports in this group. */
      if (oldPortClass != policyPtr->portClass)
      {
        BCM_PBMP_ITER(policyPtr->pbm, tempBcmPort)
        {
          rv = policy_port_class_set(unit, tempBcmPort, policyPtr->policyStage, policyPtr->portClass);
          if (BCM_E_NONE != rv)
          {
            _policy_sem_give();
            return rv;
          }
        }
      }
    }

    _policy_sem_give();
    return rv;
}

int l7_bcm_policy_remove_all(int unit, BROAD_POLICY_t policy)
{
    int                 rv;
    policy_map_table_t *policyPtr;
    bcm_port_t          port;
    bcm_pbmp_t          tempPbm;
    L7_short16          policyIdx;
    bcm_pbmp_t          affectedPortsPbm;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d remove all\n", policy);

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
      rv = _policy_apply_egress_mask(unit, policyPtr, affectedPortsPbm, BROAD_POLICY_EPBM_CMD_REMOVE);
    }
    else
    {
      BCM_PBMP_CLEAR(policyPtr->pbm);

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
      BCM_PBMP_ITER(PBMP_E_ALL(unit), port)
      {
        rv = policy_port_class_set(unit, port, policyPtr->policyStage, BROAD_INVALID_PORT_CLASS);
        if (BCM_E_NONE != rv)
        {
          _policy_sem_give();
          return rv;
        }
      }
    }

    _policy_sem_give();
    return BCM_E_NONE;
}

int l7_bcm_policy_stats(int unit, BROAD_POLICY_t policy, uint32 *args)
{
    int                 i;
    int                 rv = BCM_E_NONE;
    policy_map_table_t *policyPtr;
    BROAD_POLICY_GROUP_t groupType;
    int                 index;
    L7_short16          policyIdx;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("get policy %d stats\n", policy);

    _policy_sem_take();
    policyIdx = policy_map_index_map[unit][policy];
    if (policyIdx == BROAD_POLICY_MAP_INVALID)
    {
      _policy_sem_give();
      return BCM_E_NOT_FOUND;
    }
    policyPtr = &policy_map_table[unit][policyIdx];

    /* retrieve the L2/IPv4 stats first, then the IPv6 stats */
    for (groupType = 0; groupType < BROAD_POLICY_GROUP_COUNT; groupType++)
    {
      /* retrieve count for each entry in the policy */
      for (i = 0; i < policyPtr->entryCount[groupType]; i++)
      {
          int    tmprv;
          uint64 val1, val2;

          /* query stats for entry -- in case of error this routine returns 0 */
          tmprv = policy_group_get_stats(unit, policyPtr->policyStage, policyPtr->group[groupType], policyPtr->entry[groupType][i],
                                         &val1, &val2);
          if (tmprv < rv)
              rv = tmprv;     /* just get the worst error code */

          index = policyPtr->ruleOrder.userEntryIndex[groupType][i] * 4;

          if (index < BCM_CUSTOM_ARGS_MAX)
          {
            args[index + 0] = COMPILER_64_HI(val1);
            args[index + 1] = COMPILER_64_LO(val1);
            args[index + 2] = COMPILER_64_HI(val2);
            args[index + 3] = COMPILER_64_LO(val2);
          }
      }
    }

    _policy_sem_give();
    return rv;
}

/* Debug */

void debug_policy_table(int unit)
{
  int i, j;
  BROAD_POLICY_GROUP_t groupType;
  char policyStageString[BROAD_POLICY_STAGE_COUNT][10] = {"VFP","IFP","EFP"};
  L7_short16 policyIdx;
  policy_map_table_t *policyPtr;
  char pfmt1[SOC_PBMP_FMT_LEN];
  char pfmt2[SOC_PBMP_FMT_LEN];

  printf("Unit %d\n", unit);

  printf("Policy    Group       PBM                MASK             EntryCount  Entries\n");
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    policyIdx = policy_map_index_map[unit][i];
    if (policyIdx != BROAD_POLICY_MAP_INVALID)
    {
      policyPtr = &policy_map_table[unit][policyIdx];
      for (groupType = 0; groupType < BROAD_POLICY_GROUP_COUNT; groupType++)
      {
        if (policyPtr->group[groupType] != BROAD_GROUP_INVALID)
        {
          printf("[%2d]       %2d %3s   %1s%s %s %02d         [",
                 i, policyPtr->group[groupType], policyStageString[policyPtr->policyStage],
                 (policyPtr->flags & GROUP_MAP_EFP_ON_IFP) ? "E" : " ",
                 SOC_PBMP_FMT(policyPtr->pbm, pfmt1),
                 SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmt2),
                 (int)policyPtr->entryCount[groupType]);
          for (j = 0; j < policyPtr->entryCount[groupType]; j++)
            printf("%d ", policyPtr->entry[groupType][j]);
          printf("]\n");
        }
      }
    }
  }

  if (policy_stage_supported(unit, BROAD_POLICY_STAGE_LOOKUP))
  {
    printf("Lookup Stage:\n");
    printf("Port Class      PBM                 Policies\n");
    for (i = 1; i < BROAD_MAX_PORT_CLASS_ID; i++)
    {
      printf("[%2d]          %s    ",i, SOC_PBMP_FMT(port_class_table[unit][BROAD_POLICY_STAGE_LOOKUP][i].pbm, pfmt1));
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
              printf("[%d] ", j);
            }
          }
        }
      }
      printf("\n");
    }
  }
  if (policy_stage_supported(unit, BROAD_POLICY_STAGE_EGRESS))
  {
    printf("Egress Stage:\n");
    printf("Port Class      PBM                 Policies\n");
    for (i = 1; i < BROAD_MAX_PORT_CLASS_ID; i++)
    {
      printf("[%2d]          %s    ",i, SOC_PBMP_FMT(port_class_table[unit][BROAD_POLICY_STAGE_EGRESS][i].pbm, pfmt1));
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
              printf("[%d] ", j);
            }
          }
        }
      }
      printf("\n");
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
    printf("Unit %d\n", unit);
    printf("Default entry eid = %d\n",globalPolicyPtr->entry);
    printf("Unit Epbm = %08x\n",SOC_PBMP_WORD_GET(globalPolicyPtr->unitEpbm, 0));
}
