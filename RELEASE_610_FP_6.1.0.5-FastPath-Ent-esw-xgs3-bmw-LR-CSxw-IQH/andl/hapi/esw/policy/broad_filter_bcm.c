/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_filter_bcm.c
*
* This file implements the custom bcm layer that runs on all units.
*
* @component hapi
*
* @create    4/18/2006
*
* @author    sdoke
*
* @end
*
**********************************************************************/

#include "broad_filter_bcm.h"
#include "broad_filter_xgs2.h"
#include "ibde.h"
#include "cnfgr.h"
#include "bcmx/mirror.h"

/* Policy Table Definitions */
/* This table contains information used to map a policy to an filter entry.
 * It also contains a bitmap of the ports to which this policy applies.
 */

typedef struct
{
    uint32                   flags;
    BROAD_POLICY_TYPE_t      type;
    int                      entryCount;
    BROAD_FFP_RULE_INFO_t    ffp_ruleInfo[BROAD_MAX_RULES_PER_POLICY];
    bcm_pbmp_t               ipbm;
    bcm_pbmp_t               ipbmMask;
    uint32                   mirrorRuleCount;
}
policy_table_t;

static policy_table_t *policy_table[SOC_MAX_NUM_DEVICES] = {0};

#define CHECK_POLICY(p)  {if (p >= BROAD_MAX_POLICIES)  LOG_ERROR(p);}
#define CHECK_UNIT(u)    {if (u >= SOC_MAX_NUM_DEVICES) LOG_ERROR(u);}

/* BCM Functions */

static void _policy_remove_stack_ports(int unit, policy_table_t *policyPtr)
{
    int port;
    pbmp_t  ethernet_ports;

    if (cnfgrIsFeaturePresent (L7_FLEX_STACKING_COMPONENT_ID, 
                           L7_FPS_FEATURE_ID) == L7_FALSE)  
    {
      SOC_PBMP_ASSIGN(policyPtr->ipbm, PBMP_E_ALL(unit));
      return;  
    }

    BCM_PBMP_CLEAR (ethernet_ports);
    PBMP_E_ITER (unit, port)
    {
      if (hpcIsBcmPortStacking (unit, port) == L7_FALSE)
      {
        BCM_PBMP_PORT_ADD (ethernet_ports, port);
      }
    }

    SOC_PBMP_ASSIGN(policyPtr->ipbm, ethernet_ports);
}

static void _policy_set_default_ipbm(int unit, BROAD_POLICY_TYPE_t type, policy_table_t *policyPtr, int policyFlags)
{
    switch (type)
    {
    case BROAD_POLICY_TYPE_SYSTEM:
    case BROAD_POLICY_TYPE_SYS_EXT:     /*  PTin added: Policies */
    case BROAD_POLICY_TYPE_VLAN:
        /* by default applies to all non-stacking ports */
        if (policyFlags & BROAD_POLICY_FPS)
        {
           SOC_PBMP_ASSIGN(policyPtr->ipbm, PBMP_E_ALL(unit));
        }
        else
        {
           _policy_remove_stack_ports(unit, policyPtr);
        }
        break;
    case BROAD_POLICY_TYPE_PORT:
    case BROAD_POLICY_TYPE_COSQ:
        /* by default does not apply to any ports */
        BCM_PBMP_CLEAR(policyPtr->ipbm);
        break;
    default:
        /* indicate that a type was missed */
        LOG_ERROR(type);
        break;
    }        

    SOC_PBMP_ASSIGN(policyPtr->ipbmMask, PBMP_PORT_ALL(unit));
}

/* Policy type determines the filter priority */
static int _policy_filter_get_priority(BROAD_POLICY_TYPE_t type)
{
    int prio=0;

    /* The main purpose of this function is to allot FFP entries in correct order.
     * If priority is not specified, the precedence of filter entries depends on the 
     * order in which they are created (last created filter entry takes precedence).
     * COSQ filters are general filters and have lower priority.
     * Also, tucana/draco have 16 entries in mask table. The prio values below make
     * sure that COSQ entries are in the beginning (0%16), followed by SYSTEM entries
     * (99%16), followed by PORT entries (200%16).
     */ 
    switch (type)
    {
    case BROAD_POLICY_TYPE_VLAN:
        prio = 255;
        break;
    case BROAD_POLICY_TYPE_PORT:
        prio = 208 - L7_ACL_MAX_RULES_PER_LIST;
        break;
    case BROAD_POLICY_TYPE_SYSTEM:
    case BROAD_POLICY_TYPE_SYS_EXT:     /*  PTin added: Policies */
        prio = 99;
        break;
    case BROAD_POLICY_TYPE_COSQ:
        prio = 0;
        break;
    default:
        /* indicate that a type was missed */
        LOG_ERROR(type);
        break;
    }

    return prio;
}

static void _policy_table_init(int unit)
{
    int p;

    if (L7_NULLPTR == policy_table[unit])
    {
        policy_table[unit] = osapiMalloc(L7_DRIVER_COMPONENT_ID, BROAD_MAX_POLICIES * sizeof(policy_table_t));
    }

    if (L7_NULLPTR == policy_table[unit])
    {
        LOG_ERROR(0);
        return;
    }

    /* initialize the policy to filter mapping table */
    for (p = 0; p < BROAD_MAX_POLICIES; p++)
    {
        policy_table[unit][p].flags           = POLICY_TABLE_NONE;
        policy_table[unit][p].entryCount      = 0;
        policy_table[unit][p].mirrorRuleCount = 0;
    }
}

int l7_bcm_policy_xgs2_init()
{
  int unit;
  int rv = BCM_E_NONE;
#ifdef BCM_ROBO_SUPPORT
  for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
#else
  for (unit = 0; unit < bde->num_devices(BDE_ALL_DEVICES); unit++)
#endif
  {
    if (!SOC_IS_XGS_FABRIC(unit))
    {
      /* initialize the policy to filter mapping table */
      _policy_table_init(unit);
    }
  }

  return rv;
}

int l7_bcm_policy_xgs2_create(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData)
{
    int                 i,index;
    int                 prio;
    BROAD_ENTRY_t       entry;
    policy_table_t      *policyPtr;
    int                 saveIpbm = FALSE;
    bcm_pbmp_t          savedIpbm, savedIpbmMask;
    int                 tmprv,rv = BCM_E_NONE;
    BROAD_ACTION_ENTRY_t *actPtr;
    
    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    /* XGS2 only supports the INGRESS stage */
    if (policyData->policyStage != BROAD_POLICY_STAGE_INGRESS)
    {
      return BCM_E_UNAVAIL;
    }

    /* XGS2 only supports certain policy types */
    if ((policyData->policyType != BROAD_POLICY_TYPE_PORT) &&
        (policyData->policyType != BROAD_POLICY_TYPE_VLAN) &&
        (policyData->policyType != BROAD_POLICY_TYPE_SYSTEM) &&
        (policyData->policyType != BROAD_POLICY_TYPE_COSQ) &&
        (policyData->policyType != BROAD_POLICY_TYPE_SYS_EXT))    /*  PTin added: Policies */
    {
      return BCM_E_UNAVAIL;
    }

    policyPtr = &policy_table[unit][policy];

    if (policyPtr->flags & POLICY_TABLE_USED)
    {
        /* This is a refresh existing of an existing policy. To insure the entries are added
         * in precedence order it is necessary to delete and recreate the policy in hardware.
         * Save the IPBM as this is our only indication of which ports are affected.
         */
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            printf("policy changed, delete and re-add\n");

        saveIpbm = TRUE;
        SOC_PBMP_ASSIGN(savedIpbm, policyPtr->ipbm);
        SOC_PBMP_ASSIGN(savedIpbmMask, policyPtr->ipbmMask);

        (void)l7_bcm_policy_xgs2_destroy(unit, policy);
    }
    else if (policyData->policyFlags & BROAD_POLICY_CHANGED)
    {
        /* this is a refresh of a policy that we don't know about, so ignore it */
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            printf("policy not applicable, ignore\n");

        return BCM_E_NONE;
    }
    else
    {
        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            printf("commit policy %d\n", policy);
    }

    /* populate the filter mapping table */
    policyPtr->flags      = POLICY_TABLE_USED;
    policyPtr->type       = policyData->policyType;
    policyPtr->entryCount = 0;

    if (FALSE == saveIpbm)
    {
        _policy_set_default_ipbm(unit, policyData->policyType, policyPtr, policyData->policyFlags);
    }
    else
    {
        SOC_PBMP_ASSIGN(policyPtr->ipbm, savedIpbm);
        SOC_PBMP_ASSIGN(policyPtr->ipbmMask, savedIpbmMask);
    }

    /* calculate the prio of each rule */
    prio = _policy_filter_get_priority(policyData->policyType);
    for (i = policyData->ruleCount-1; i >= 0; i--)
    {
       if (policyData->policyType == BROAD_POLICY_TYPE_PORT)
       {
          /* For PORT policies, assign precedence among rules */
          if (policyData->ruleCount > 1) 
          {
             /* Start with highest precedence and work down to lowest */
             policyPtr->ffp_ruleInfo[i].prio = prio++;
          }
          else
             policyPtr->ffp_ruleInfo[i].prio = prio;
       } 
       else if (policyData->policyType == BROAD_POLICY_TYPE_COSQ)
       {
          if (policyData->ruleCount > 1)
            policyPtr->ffp_ruleInfo[i].prio = prio + 1;
          else
            policyPtr->ffp_ruleInfo[i].prio = prio;
       }
       else 
       {
          policyPtr->ffp_ruleInfo[i].prio = prio;
       }
    }

    /* add new rules to policy */
    for (i = policyPtr->entryCount; i < policyData->ruleCount; i++) 
    {
        int srcRule;

        if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
            printf("- add rule %d\n", i);

        if (policyData->policyType == BROAD_POLICY_TYPE_PORT)
        { 
            /* install other way round */
            index = policyData->ruleCount-i-1;
        }
        else 
        {
            index = i;
        }

        /* convert srcEntry from rule to entry number so lower layer understands */
        srcRule = policyData->ruleInfo[index].meterSrcEntry;
        policyData->ruleInfo[index].meterSrcEntry = policyPtr->ffp_ruleInfo[srcRule].entry; /* ?? */

        rv = policy_filter_add_rule(unit, &policyData->ruleInfo[index], &entry, 
                                    policyPtr->ffp_ruleInfo[index].prio);
        if (BCM_E_NONE != rv)
        {
            if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                printf("- add rule failed\n");

            (void)l7_bcm_policy_xgs2_destroy(unit, policy);

            return rv;
        }

        policyPtr->entryCount++;
        policyPtr->ffp_ruleInfo[index].entry = entry;

        /* Store any meter and counter info, used while deletion */
        policyPtr->ffp_ruleInfo[index].cntr_flags = policyData->ruleInfo[index].ruleFlags;
        policyPtr->ffp_ruleInfo[index].cntr_port = policyData->ruleInfo[index].u.counter.counterInfo.port;
        policyPtr->ffp_ruleInfo[index].ffpcntr_id = policyData->ruleInfo[index].u.counter.counterInfo.ffpcntr_id;
        policyPtr->ffp_ruleInfo[index].meter_flags = policyData->ruleInfo[index].ruleFlags;;
        policyPtr->ffp_ruleInfo[index].mtr_port = policyData->ruleInfo[index].u.meter.meterInfo.port;
        policyPtr->ffp_ruleInfo[index].ffpmtr_id = policyData->ruleInfo[index].u.meter.meterInfo.ffpmtr_id;

        /* restore srcEntry back to original rule number */
        policyData->ruleInfo[index].meterSrcEntry = srcRule;

        actPtr = &policyData->ruleInfo[index].actionInfo;

        if (BROAD_ACTION_IS_SPECIFIED(actPtr, BROAD_ACTION_MIRROR))
        {
          rv = l7_bcm_mirror_enable(actPtr->parms.modid,actPtr->parms.modport);

          /* if 'mirror-to-port' doesnt match with the previously configured value then 
           * delete the rule and again add the rule by disabling the mirroring action
           */	

          if (BCM_E_NONE != rv)
          {
             tmprv = policy_filter_delete_rule(unit, &policyPtr->ffp_ruleInfo[index]);

             if(BCM_E_NONE != tmprv)
               rv = tmprv;

             actPtr->actions[BROAD_POLICY_ACTION_CONFORMING] &= ~(1 << BROAD_ACTION_MIRROR);

             rv = policy_filter_add_rule(unit, &policyData->ruleInfo[index], &entry, prio);

             if (BCM_E_NONE != rv)
             {
                if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
                   printf("- add rule failed\n");

                (void)l7_bcm_policy_xgs2_destroy(unit, policy);

                return rv;
             }
             policyPtr->ffp_ruleInfo[index].entry = entry;
             L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
                    "Failed installing mirror action - rest of the policy applied successfully."
                    " A previously configured probe port is not being used in the policy.  The "
                    "release notes state that only a single probe port can be configured.");
          }
          else
          {
             policyPtr->mirrorRuleCount++;
             policyPtr->ffp_ruleInfo[index].mirror_rule = L7_TRUE;	 	  
          }
        }

        /* must set ipbm/mask for each entry */
        rv = policy_filter_set_ipbm(unit, entry, policyPtr->ipbm, policyPtr->ipbmMask);
        if (BCM_E_NONE != rv)
        {
            (void)l7_bcm_policy_xgs2_destroy(unit, policy);
            return rv;
        }
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW) 
        printf("policy %d created on unit(%d): type %d, ruleCount %d, result %d\n",
               policy, unit, policyData->policyType, policyPtr->entryCount, rv);

    return rv;
}

int l7_bcm_policy_xgs2_destroy(int unit, BROAD_POLICY_t policy)
{
  int tmprv, rv = BCM_E_NONE;
  int i;
  policy_table_t *policyPtr;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  policyPtr = &policy_table[unit][policy];

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
     printf("destroy policy %d: ruleCount %d, ", policy, policyPtr->entryCount);

  if (!(policyPtr->flags & POLICY_TABLE_USED))
      return BCM_E_NONE;

  /* remove each filter entry used by policy */
  for (i = 0; i < policyPtr->entryCount; i++)
  {
      tmprv = policy_filter_delete_rule(unit, &policyPtr->ffp_ruleInfo[i]);
      if (BCM_E_NONE != tmprv)
          rv = tmprv;
      if (policyPtr->ffp_ruleInfo[i].mirror_rule == L7_TRUE)
      {
         if (policyPtr->mirrorRuleCount > 0)
         {
            policyPtr->ffp_ruleInfo[i].mirror_rule = L7_FALSE;
            policyPtr->mirrorRuleCount--;
         }
	 
         if (policyPtr->mirrorRuleCount == 0)
            l7_bcm_mirror_disable();
      }
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
          printf("del-%d ", i);
  }

  policyPtr->flags      = POLICY_TABLE_NONE;
  policyPtr->entryCount = 0;

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      printf("result %d\n", rv);

  return rv;
}

int l7_bcm_policy_xgs2_apply(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
  int                 i, index;
  int                 rv;
  policy_table_t *policyPtr;

  CHECK_UNIT(unit);
  CHECK_POLICY(policy);

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
      printf("policy %d apply port %d\n", policy, port);

  policyPtr = &policy_table[unit][policy];

  if (!(policyPtr->flags & POLICY_TABLE_USED))
      return BCM_E_NONE;

  BCM_PBMP_PORT_ADD(policyPtr->ipbm, port); 

  /* add port to each entry used by policy */
  for (i = 0; i < policyPtr->entryCount; i++)
  {
      if (policyPtr->type == BROAD_POLICY_TYPE_PORT)  
      {
        index = policyPtr->entryCount - i - 1; 
      }
      else 
      {
        index = i;
      }

      rv = policy_filter_set_ipbm(unit, policyPtr->ffp_ruleInfo[index].entry, 
                                  policyPtr->ipbm, policyPtr->ipbmMask);
      if (BCM_E_NONE != rv)
        return rv;
  }

  return BCM_E_NONE;
}

int l7_bcm_policy_xgs2_apply_all(int unit, BROAD_POLICY_t policy)
{
    int             i, index;
    int             rv;
    policy_table_t *policyPtr;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d apply all\n", policy);

    policyPtr = &policy_table[unit][policy];

    if (!(policyPtr->flags & POLICY_TABLE_USED))
        return BCM_E_NONE;

    SOC_PBMP_ASSIGN(policyPtr->ipbm, PBMP_E_ALL(unit));

    /* add port to each entry used by policy */
    for (i = 0; i < policyPtr->entryCount; i++)
    {
        if (policyPtr->type == BROAD_POLICY_TYPE_PORT) 
        {
           index = policyPtr->entryCount - i - 1;
        }
        else
        {
          index = i;
        }

        rv = policy_filter_set_ipbm(unit, policyPtr->ffp_ruleInfo[index].entry, 
                                    policyPtr->ipbm, policyPtr->ipbmMask);
        if (BCM_E_NONE != rv)
            return rv;
    }

    return BCM_E_NONE;
}

int l7_bcm_policy_xgs2_remove(int unit, BROAD_POLICY_t policy, bcm_port_t port)
{
    int             i;
    int             rv = BCM_E_NONE;
    policy_table_t *policyPtr;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d remove port %d\n", policy, port);

    policyPtr = &policy_table[unit][policy];

    if (!(policyPtr->flags & POLICY_TABLE_USED))
        return BCM_E_NONE;

    BCM_PBMP_PORT_REMOVE(policyPtr->ipbm, port);

    /* set new IPBM for each entry used by policy */
    for (i = 0; i < policyPtr->entryCount; i++)
    {
        rv = policy_filter_set_ipbm(unit, policyPtr->ffp_ruleInfo[i].entry, policyPtr->ipbm, policyPtr->ipbmMask);
        if (BCM_E_NONE != rv)
            return rv;
    }

    return rv;
}

int l7_bcm_policy_xgs2_remove_all(int unit, BROAD_POLICY_t policy)
{
    int             i;
    int             rv;
    policy_table_t *policyPtr;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("policy %d remove all\n", policy);

    policyPtr = &policy_table[unit][policy];

    if (!(policyPtr->flags & POLICY_TABLE_USED))
        return BCM_E_NONE;

    BCM_PBMP_CLEAR(policyPtr->ipbm);

    /* set new IPBM for each entry used by policy */
    for (i = 0; i < policyPtr->entryCount; i++)
    {
        rv = policy_filter_set_ipbm(unit, policyPtr->ffp_ruleInfo[i].entry, policyPtr->ipbm, policyPtr->ipbmMask);
        if (BCM_E_NONE != rv)
            return rv;
    }

    return BCM_E_NONE;
}

int l7_bcm_policy_xgs2_stats(int unit, BROAD_POLICY_t policy, uint32 *args)
{
    int             i;
    int             rv = BCM_E_NONE;
    policy_table_t *policyPtr;

    CHECK_UNIT(unit);
    CHECK_POLICY(policy);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_LOW)
        printf("get policy %d stats\n", policy);

    if (SOC_IS_XGS_FABRIC(unit))
    {
      *args++ = 0;
      *args++ = 0;
      *args++ = 0;
      *args++ = 0;
      return BCM_E_NONE;
    } 

    policyPtr = &policy_table[unit][policy];

    if (!(policyPtr->flags & POLICY_TABLE_USED))
        return BCM_E_NOT_FOUND;

    /* retrieve count for each entry in the policy */
    for (i = 0; i < policyPtr->entryCount; i++)
    {
        int    tmprv;
        uint64 val1, val2;

        /* query stats for filter -- in case of error this routine returns 0 */
        tmprv = policy_filter_get_stats(unit, &policyPtr->ffp_ruleInfo[i],
                                        &val1, &val2);
        if (tmprv < rv)
            rv = tmprv;     /* just get the worst error code */

        *args++ = COMPILER_64_HI(val1);
        *args++ = COMPILER_64_LO(val1);
        *args++ = COMPILER_64_HI(val2);
        *args++ = COMPILER_64_LO(val2);
    }

    return rv;
}

int  l7_bcm_mirror_enable(int modid, int mod_port)
{
    bcmx_lport_t mport,old_mport;
    int          rv,omode;

    mport = bcmx_modid_port_to_lport(modid,mod_port);

    rv = bcmx_mirror_mode_get(&omode);

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
       LOG_ERROR (rv);
    }
    
    /*if mirroring is already enabled and probe port is same
     *return success otherwise return BCM_E_CONFIG*/
    if (omode == BCM_MIRROR_L2)
    {
       rv = bcmx_mirror_to_get(&old_mport);
       if (L7_BCMX_OK(rv) != L7_TRUE)
       {
          LOG_ERROR (rv);
       }
       if (old_mport == mport)
          return BCM_E_NONE;
       else
          return BCM_E_CONFIG;
    }

    rv = bcmx_mirror_mode_set(BCM_MIRROR_L2);

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
       LOG_ERROR (rv);
    }

    rv = bcmx_mirror_pfmt_set(1);

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
       LOG_ERROR (rv);
    }

    rv = bcmx_mirror_to_set(mport);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
       LOG_ERROR (rv);
    }
    return rv;
}

void l7_bcm_mirror_disable()
{
    int rv;

    /*Disabling the mirroring and setting mirror to port to default*/
    rv = bcmx_mirror_mode_set(BCM_MIRROR_DISABLE);

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
       LOG_ERROR (rv);
    }
}	   	

/* Debug */
void hapiBroadDebugXGS2TableDump(int unit)
{
    int i, j;

    printf("xgs2 Unit %d\n", unit);
 
    if (!SOC_IS_XGS12_SWITCH(unit))
        return;

    printf("Policy    IPBM     MASK     EntryCount  MirrorCount  Entries\n");
    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        if (policy_table[unit][i].flags & POLICY_TABLE_USED)
        {
            printf("[%2d]      %08x %08x %02d          %02d             [ ",
                   i, policy_table[unit][i].ipbm.pbits[0],
                   (int)policy_table[unit][i].ipbmMask.pbits[0],
                   (int)policy_table[unit][i].entryCount,
		   (int)policy_table[unit][i].mirrorRuleCount);
            for (j = 0; j < policy_table[unit][i].entryCount; j++)
                printf("%d ", policy_table[unit][i].ffp_ruleInfo[j].entry);
            printf("]\n");
        }
    }
}


