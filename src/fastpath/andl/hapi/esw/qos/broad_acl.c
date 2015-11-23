/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_acl.c
*
* Purpose: This file contains all the routines for Access Control Lists
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/

#include "broad_acl.h"
#include "broad_policy_types.h"
#include "acl_api.h"
#include "dapi.h"
#include "unitmgr_api.h"
#include "zlib.h"
#include "osapi_support.h"
#include "ptin_globaldefs.h"

/* ACL Correlator Table Definition (tracks rule correlators received from TLV) */

typedef struct
{
    L7_uint32             correlator;
    L7_ushort16           corrFlag;
    BROAD_POLICY_t        aclId;
    BROAD_POLICY_RULE_t   ruleId;
    L7_BOOL               addedToPolicy;
    L7_uint64             lastCount;
} BROAD_ACL_CORR_TABLE_t;

static BROAD_ACL_CORR_TABLE_t  *broadAclCorrTable = L7_NULL;
/* Time based ACLs- Variable to maintain number of log entries in the correlator table*/
static L7_uint32 logCount = 0;
/* ACL Instance Table Definition */

typedef struct
{
    L7_BOOL             used;
    L7_uint32           hashVal;
    HAPI_QOS_INTF_DIR_t direction;
    BROAD_POLICY_t      l2v4PolicyId; /* This policy holds the MAC and IPv4 ACLs.*/
    BROAD_POLICY_t      v6PolicyId;   /* This policy holds the IPv6 ACLs.        */
    int                 ifCount;
    L7_uint32           instanceId;
}
BROAD_ACL_TABLE_t;

static BROAD_ACL_TABLE_t   *broadAclTable = L7_NULL;
static BROAD_POLICY_RULE_t *timeBasedRuleId = L7_NULL; 
static BROAD_POLICY_RULE_t *timeBasedRuleCorrelator = L7_NULL;

/*********************************************************************
*
* @function hapiBroadMaxAclPolicies
*
* @purpose  Calculates the maximum number of ACL policies in the system.
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_uint32 hapiBroadMaxAclPolicies()
{
  static L7_BOOL   first_time     = L7_TRUE;
  static L7_uint32 maxAclPolicies = 0;

  if (first_time)
  {
    first_time = L7_FALSE;

    /* Assume that each port and VLAN can accomodate one ACL policy. */
    maxAclPolicies = L7_MAX_PORT_COUNT + L7_ACL_VLAN_MAX_COUNT;

    /* Check for egress ACL support. */
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID))
    {
      maxAclPolicies += L7_MAX_PORT_COUNT;
    }

    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID))
    {
      maxAclPolicies += L7_ACL_VLAN_MAX_COUNT;
    }
  }

  return maxAclPolicies;
}
/*********************************************************************
*
* @function hapiBroadMaxAclCorrEntries
*
* @purpose  Calculates the maximum number of ACL entries that correlator table can hold
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_uint32 hapiBroadMaxAclCorrTableEntries()
{
  static L7_uint32 maxAclEntries = 0;
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID))
  {
      maxAclEntries = L7_ACL_MAX_RULES;
  }
  else
  {
      maxAclEntries = L7_ACL_LOG_RULE_LIMIT;
  }
  return maxAclEntries;
}

/*********************************************************************
*
* @function hapiBroadQosAclCorrelatorEntryReset
*
* @purpose  Resets contents of specified ACL correlator table entry
*
* @param    L7_uint32       idx
*
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
static void hapiBroadQosAclCorrelatorEntryReset(L7_uint32 idx)
{
    broadAclCorrTable[idx].correlator    = L7_QOS_ACL_TLV_RULE_CORR_NULL;
    broadAclCorrTable[idx].aclId         = BROAD_POLICY_INVALID;
    broadAclCorrTable[idx].ruleId        = BROAD_POLICY_RULE_INVALID;
    broadAclCorrTable[idx].addedToPolicy = L7_FALSE;
    broadAclCorrTable[idx].lastCount     = 0;
    /* Reset the corrFlag */
    broadAclCorrTable[idx].corrFlag     = ACL_RULE_NONE;
}

/*********************************************************************
*
* @purpose Initialize ACL component of QOS package
*
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosAclInit(DAPI_t *dapi_g)
{
    int i;
    L7_uint32 maxAclPolicies;
    L7_uint32 maxAclEntries;

    maxAclPolicies = hapiBroadMaxAclPolicies();

    if (broadAclTable == L7_NULL)
    {
      broadAclTable = osapiMalloc(L7_DRIVER_COMPONENT_ID, maxAclPolicies * sizeof(BROAD_ACL_TABLE_t));
    }
    if (broadAclTable == L7_NULL)
    {
      LOG_ERROR(0);
    }
    /* Init the ACL instance lookup table. */
    for (i = 0; i < maxAclPolicies; i++)
    {
        broadAclTable[i].used = L7_FALSE;
    }

    /* Allocate memory for correaltor table */
    maxAclEntries = hapiBroadMaxAclCorrTableEntries();
    
    if (broadAclCorrTable == L7_NULL)
    {
      broadAclCorrTable = osapiMalloc(L7_DRIVER_COMPONENT_ID, maxAclEntries * sizeof(BROAD_ACL_CORR_TABLE_t));
    }
    if (broadAclCorrTable == L7_NULL)
    {
      LOG_ERROR(0);
    }
    /* init the ACL correlator table */
    for (i = 0; i < maxAclEntries; i++)
    {
        hapiBroadQosAclCorrelatorEntryReset(i);
    }

    if (timeBasedRuleId == L7_NULL)
    {
      timeBasedRuleId = osapiMalloc(L7_DRIVER_COMPONENT_ID, L7_ACL_MAX_RULES_PER_MULTILIST * sizeof(BROAD_POLICY_RULE_t));
      if (timeBasedRuleId == L7_NULL)
      {
        LOG_ERROR(0);
      }
    }

    if (timeBasedRuleCorrelator == L7_NULL)
    {
      timeBasedRuleCorrelator = osapiMalloc(L7_DRIVER_COMPONENT_ID, L7_ACL_MAX_RULES_PER_MULTILIST * sizeof(BROAD_POLICY_RULE_t));
      if (timeBasedRuleCorrelator == L7_NULL)
      {
        LOG_ERROR(0);
      }
    }

    return L7_SUCCESS;
}

/*********************************************************************
*
* @function hapiBroadQosAclPortInit
*
* @purpose  Per-port ACL init
*
* @param    DAPI_PORT_t* dapiPortPtr - generic port instance
*
* @returns  L7_RC_t
*
* @notes    Invoked once per physical port
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosAclPortInit(DAPI_PORT_t *dapiPortPtr)
{
    L7_RC_t result = L7_SUCCESS;

    dapiPortPtr->cmdTable[DAPI_CMD_QOS_ACL_ADD]    = (HAPICTLFUNCPTR_t)hapiBroadQosAclAdd;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_ACL_DELETE] = (HAPICTLFUNCPTR_t)hapiBroadQosAclDelete;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_ACL_RULE_COUNT_GET] = (HAPICTLFUNCPTR_t)hapiBroadQosAclRuleCountGet;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_ACL_RULE_STATUS_SET] = (HAPICTLFUNCPTR_t)hapiBroadQosAclRuleStatusSet;
    /*
     * The same functions are used to process these commands since the only
     * difference will be the USP is the CPU port, not a physical or logical
     * interface.
     */
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_SYS_ACL_ADD]    = (HAPICTLFUNCPTR_t)hapiBroadQosAclAdd;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_SYS_ACL_DELETE] = (HAPICTLFUNCPTR_t)hapiBroadQosAclDelete;

    return result;
}

/*********************************************************************
*
* @function hapiBroadQosAclPolicyTLVHash
*
* @purpose  Hash TLV to generate pseudo unique identifier
*
* @param    L7_tlv_t *pTLV
*
* @returns  L7_BOOL
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_uint32 hapiBroadQosAclPolicyTLVHash(L7_tlv_t *pTLV)
{
    /* Skip instanceId as it can vary and jump right to the rules. */
    return crc32(0L, (L7_uchar8*)GET_SUB_TLV(pTLV,4), osapiNtohl(pTLV->length)-4);
}

/*********************************************************************
*
* @function hapiBroadQosAclPolicyTLVFind
*
* @purpose  Search ACL table for matching policy definition (TLV)
*
* @param    L7_tlv_t *pTLV
* @param    int      *idx
*
* @returns  L7_BOOL
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_BOOL hapiBroadQosAclPolicyTLVFind(L7_tlv_t *pTLV, HAPI_QOS_INTF_DIR_t direction, int *idx)
{
    int                  i;
    L7_uint32            hash;
    L7_uint32            maxAclPolicies;

    /* compute rudimentary hash over TLV */
    hash = hapiBroadQosAclPolicyTLVHash(pTLV);

    maxAclPolicies = hapiBroadMaxAclPolicies();

    for (i = 0; i < maxAclPolicies; i++)
    {
        if ((L7_TRUE   == broadAclTable[i].used) &&
            (hash      == broadAclTable[i].hashVal) &&
            (direction == broadAclTable[i].direction))
        {
          *idx = i;       /* found match */
          return L7_TRUE;
        }
    }

    return L7_FALSE;
}

/*********************************************************************
*
* @function hapiBroadQosAclPolicyTLVAdd
*
* @purpose  Add ACL instance to table
*
* @param    L7_tlv_t       *pTLV
* @param    BROAD_POLICY_t  policy
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosAclPolicyTLVAdd(L7_tlv_t           *pTLV,
                                           BROAD_POLICY_t      l2v4PolicyId,
                                           BROAD_POLICY_t      v6PolicyId,
                                           HAPI_QOS_INTF_DIR_t direction,
                                           L7_uint32           instanceId)
{
    int i;
    L7_uint32 maxAclPolicies;

    maxAclPolicies = hapiBroadMaxAclPolicies();

    for (i = 0; i < maxAclPolicies; i++)
    {
        if (L7_FALSE == broadAclTable[i].used)
        {
            broadAclTable[i].used         = L7_TRUE;
            broadAclTable[i].hashVal      = hapiBroadQosAclPolicyTLVHash(pTLV);
            broadAclTable[i].l2v4PolicyId = l2v4PolicyId;
            broadAclTable[i].v6PolicyId   = v6PolicyId;
            broadAclTable[i].direction    = direction;
            broadAclTable[i].ifCount      = 1;
            broadAclTable[i].instanceId   = instanceId;

            return L7_SUCCESS;
        }
    }

    return L7_FAILURE;
}

/*********************************************************************
*
* @function hapiBroadQosAclCorrelatorFind
*
* @purpose  Search ACL correlator table for matching entry
*
* @param    L7_uint32       correlator
* @param    int            *idx
*
* @returns  L7_BOOL
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_BOOL hapiBroadQosAclCorrelatorFind(L7_uint32 correlator, int *idx)
{
    int i;
    L7_uint32 maxAclEntries = 0;
    maxAclEntries = hapiBroadMaxAclCorrTableEntries();
    for (i = 0; i < maxAclEntries; i++)
    {
        if (correlator == broadAclCorrTable[i].correlator)
        {
            *idx = i;       /* found match */
            return L7_TRUE;
        }
    }

    return L7_FALSE;
}

/*********************************************************************
*
* @function hapiBroadQosAclCorrelatorFindNext
*
* @purpose  Search ACL correlator table for next matching entry
*
* @param    L7_uint32       correlator
* @param    int            *idx
*
* @returns  L7_BOOL
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_BOOL hapiBroadQosAclCorrelatorFindNext(L7_uint32 correlator, int *idx)
{
    int i;
    L7_uint32 maxAclEntries = 0;
    maxAclEntries = hapiBroadMaxAclCorrTableEntries();
    for (i = (*idx)+1; i < maxAclEntries; i++)
    {
        if (correlator == broadAclCorrTable[i].correlator)
        {
            *idx = i;       /* found match */
            return L7_TRUE;
        }
    }

    return L7_FALSE;
}

/*********************************************************************
*
* @function hapiBroadQosAclCorrelatorAdd
*
* @purpose  Add entry to ACL correlator table
*
* @param    BROAD_POLICY_t      aclId
* @param    BROAD_POLICY_RULE_t ruleId
* @param    L7_uint32           correlator
* @param    L7_BOOL             addedToPolicy
* @param    L7_BOOL             isLog
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosAclCorrelatorAdd(BROAD_POLICY_t      aclId,
                                            BROAD_POLICY_RULE_t ruleId,
                                            L7_uint32           correlator,
                                            L7_BOOL             addedToPolicy,
                                            L7_BOOL             isLog)
{
    int i;
    L7_uint32 maxAclEntries = 0;
    maxAclEntries = hapiBroadMaxAclCorrTableEntries();
 
    /* Log entries */
    if(isLog == L7_TRUE)
    {
     if(logCount < L7_ACL_LOG_RULE_LIMIT)
     {
      for (i = 0; i < maxAclEntries; i++)
      {
        /*  check to verify if there is already an entry in table with same correlator, aclID
         *  and rule ID because of time based ACLs, update the existing entry in the
         *  correlator table
         */
        if ((broadAclCorrTable[i].correlator == correlator) && (broadAclCorrTable[i].aclId == aclId) && (broadAclCorrTable[i].ruleId == ruleId))
        {
           broadAclCorrTable[i].lastCount     = 0;
           broadAclCorrTable[i].corrFlag      |= ACL_RULE_LOG;
           logCount++;
           return L7_SUCCESS;
        }
      }
      for (i = 0; i < maxAclEntries; i++)
      {
        if (L7_QOS_ACL_TLV_RULE_CORR_NULL == broadAclCorrTable[i].correlator)
        {
            broadAclCorrTable[i].correlator    = correlator;
            broadAclCorrTable[i].aclId         = aclId;
            broadAclCorrTable[i].ruleId        = ruleId;
            broadAclCorrTable[i].addedToPolicy = addedToPolicy;
            broadAclCorrTable[i].lastCount     = 0;
            broadAclCorrTable[i].corrFlag      |= ACL_RULE_LOG;
            logCount++;
            return L7_SUCCESS;
        }
      }
     }
     else
     {
       /* maximum entries reached for logging */    
       L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                    "maximum entries reached for logging, logCount = %d\n", logCount);
       return L7_FAILURE;
     }
    }
    /* Time based Entries*/
    else
    { 
      for (i = 0; i < maxAclEntries; i++)
      {
        /* check to verify if there already an entry in table with same correlator  aclID and rule ID because
         * of Logged ACLs Rules, update the existing entry in the correlator table
         */
        if ((broadAclCorrTable[i].correlator == correlator) && (broadAclCorrTable[i].aclId == aclId) && (broadAclCorrTable[i].ruleId == ruleId))
        {
           broadAclCorrTable[i].corrFlag      |= ACL_RULE_TIME_RANGE;
           return L7_SUCCESS;
        }
      }
      for (i = 0; i < maxAclEntries; i++)
      {
        if (L7_QOS_ACL_TLV_RULE_CORR_NULL == broadAclCorrTable[i].correlator)
        {
            broadAclCorrTable[i].correlator    = correlator;
            broadAclCorrTable[i].aclId         = aclId;
            broadAclCorrTable[i].ruleId        = ruleId;
            broadAclCorrTable[i].addedToPolicy = addedToPolicy;
            broadAclCorrTable[i].lastCount     = 0;
            broadAclCorrTable[i].corrFlag      |= ACL_RULE_TIME_RANGE;
            return L7_SUCCESS;
        }
      }
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @function hapiBroadQosAclCorrelatorDelete
*
* @purpose  Remove all entries from the ACL correlator table for this ACL ID
*
* @param    BROAD_POLICY_t  aclId
*
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
static void hapiBroadQosAclCorrelatorDelete(BROAD_POLICY_t aclId)
{
    int i;
    L7_uint32 maxAclEntries = 0;
    maxAclEntries = hapiBroadMaxAclCorrTableEntries();
    for (i = 0; i < maxAclEntries; i++)
    {
        if ((L7_QOS_ACL_TLV_RULE_CORR_NULL != broadAclCorrTable[i].correlator) &&
            (aclId == broadAclCorrTable[i].aclId))
        {
            if(broadAclCorrTable[i].corrFlag & ACL_RULE_LOG)
            {
               /* decrement the logCount if log entry in table is being reset */
               logCount--;
            }
            hapiBroadQosAclCorrelatorEntryReset(i);
        }
    }
}

/*********************************************************************
*
* @function hapiBroadQosAclCorrelatorCountClear
*
* @purpose  Clear all counters in the ACL correlator table for this ACL ID
*
* @param    BROAD_POLICY_t  aclId
*
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
static void hapiBroadQosAclCorrelatorCountClear(BROAD_POLICY_t aclId)
{
    int i;
    L7_uint32 maxAclEntries = 0;
    maxAclEntries = hapiBroadMaxAclCorrTableEntries();
    for (i = 0; i < maxAclEntries; i++)
    {
        if ((L7_QOS_ACL_TLV_RULE_CORR_NULL != broadAclCorrTable[i].correlator) &&
            (aclId == broadAclCorrTable[i].aclId))
        {
            broadAclCorrTable[i].lastCount = 0;
        }
    }
}

/*********************************************************************
*
* @function hapiBroadQosAclPolicyIdFindInstance
*
* @purpose  Search ACL table for matching instance ID.
*
* @param    L7_uint32       instanceId
* @param    int            *idx
*
* @returns  L7_BOOL
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_BOOL hapiBroadQosAclPolicyIdFindInstance( L7_uint32 instanceId, HAPI_QOS_INTF_DIR_t direction, int *idx )
{
  int i;
  L7_uint32 maxAclPolicies;

  maxAclPolicies = hapiBroadMaxAclPolicies();

  for ( i = 0; i < maxAclPolicies; i++ )
  {
    if ( ( L7_TRUE    == broadAclTable[i].used ) &&
         ( instanceId == broadAclTable[i].instanceId ) &&
         ( direction  == broadAclTable[i].direction ))
    {
      *idx = i;       /* found match */
      return( L7_TRUE );
    }
  }

  return( L7_FALSE );
}

/*********************************************************************
*
* @functions hapiBroadQosAclRuleIsIpv6
*
* @purpose Determines if a rule is considered to be an IPv6 rule, based
*          on the ethertype being equal to 0x86dd. However, if the ethertype
*          is 0x86dd, but no IPv6 rules had been found, then this rule is
*          considered to NOT be an IPv6 rule (i.e. it will be treated as
*          an L2 rule).
*
* @param L7_tlv_t  *pRuleTLV
* @param L7_BOOL    ipv6RuleFound
* @param L7_BOOL   *ruleIsIpv6
*
* @notes none
*
* @end
*
*********************************************************************/
static void hapiBroadQosAclRuleIsIpv6(L7_tlv_t *pRuleTLV, L7_BOOL ipv6RuleFound, L7_BOOL *ruleIsIpv6)
{
  L7_tlv_t                 *pMatchTLV;
  L7_ushort16               ethType;
  L7_BOOL                   matchEvery;
  L7_int32                  classRuleLen;


  classRuleLen = osapiNtohl(pRuleTLV->length) - L7_QOS_ACL_TLV_RULE_DEF_LEN;    /* account for TLV header fields */
  pMatchTLV    = GET_SUB_TLV(pRuleTLV,L7_QOS_ACL_TLV_RULE_DEF_LEN); /* point to first match criterion */

  ethType = 0;
  *ruleIsIpv6 = L7_FALSE;

  /* iterate through match criteria for this rule */
  matchEvery = L7_FALSE;
  while ((classRuleLen > 0) && (L7_FALSE == matchEvery))
  {
    switch (osapiNtohl(pMatchTLV->type))
    {
    case L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE:
      ethType = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
      if ((ethType == 0x86dd) && (ipv6RuleFound == L7_TRUE))
      {
        *ruleIsIpv6 = L7_TRUE;
      }
      break;

    default:
      break;
    }

    if (*ruleIsIpv6 == L7_TRUE)
      break;

    classRuleLen -= (osapiNtohl(pMatchTLV->length) + sizeof(pMatchTLV->type) + sizeof(pMatchTLV->length));
    /* get next match criteria */
    pMatchTLV = GET_NEXT_TLV(pMatchTLV);
  }
}

/*********************************************************************
*
* @functions hapiBroadQosAclRuleCount
*
* @purpose Counts the number of L2/IPv4 rules and IPv6 rules.
*          If the ethertype is 0x86dd, the rule is counted as IPv6,
*          otherwise it is counted as L2/IPv4. Care must be taken however,
*          if no rules include any IPv6 qualifiers, then a rule whose
*          ethertype is considered to be L2 (i.e. MAC ACL w/ ethtype == 0x86dd).
*
* @param L7_tlv_t  *pListTLV
* @param L7_uint32 *l2v4RuleCount
* @param L7_uint32 *v6RuleCount
*
* @notes none
*
* @end
*
*********************************************************************/
static void hapiBroadQosAclRuleCount(L7_tlv_t *pListTLV, L7_uint32 *l2v4RuleCount, L7_uint32 *v6RuleCount)
{
  L7_tlv_t                 *pRuleTLV;
  L7_tlv_t                 *pMatchTLV;
  L7_int32                  classDefLen, classRuleLen;
  L7_ushort16               ethType;
  L7_BOOL                   matchEvery;
  L7_uint32                 l2v4Count = 0;
  L7_uint32                 v6Count   = 0;
  L7_BOOL                   ipv6QualifierSpecified = L7_FALSE;
  L7_BOOL                   isIpv6Rule;

  *l2v4RuleCount = l2v4Count;
  *v6RuleCount   = v6Count;

  /* First, determine if any rules in the list have IPv6 qualifiers. */

  classDefLen = osapiNtohl(pListTLV->length) - 4; /* subtract 4 to account for instanceId */

  /* point to first rule */
  pRuleTLV = GET_SUB_TLV(pListTLV,4);

  /* iterate through class rules */
  while (classDefLen > 0)
  {
    if (osapiNtohl(pRuleTLV->type) != L7_QOS_ACL_TLV_RULE_DEF)
    {
      return;
    }

    classRuleLen = osapiNtohl(pRuleTLV->length) - L7_QOS_ACL_TLV_RULE_DEF_LEN;    /* account for TLV header fields */
    pMatchTLV    = GET_SUB_TLV(pRuleTLV,L7_QOS_ACL_TLV_RULE_DEF_LEN); /* point to first match criterion */

    ethType = 0;

    /* iterate through match criteria for this rule */
    matchEvery = L7_FALSE;
    while ((classRuleLen > 0) && (L7_FALSE == matchEvery))
    {
      switch (osapiNtohl(pMatchTLV->type))
      {
      case L7_QOS_ACL_TLV_MATCH_EVERY_TYPE:
        matchEvery = L7_TRUE;
        /* Fall through */
      case L7_QOS_ACL_TLV_MATCH_DSTL4PORT_TYPE:
      case L7_QOS_ACL_TLV_MATCH_SRCL4PORT_TYPE:
      case L7_QOS_ACL_TLV_MATCH_IPTOS_TYPE:
      case L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE:
      case L7_QOS_ACL_TLV_MATCH_ICMPMSG_TYPE:
        if (ethType == 0x86dd)
        {
          ipv6QualifierSpecified = L7_TRUE;
        }
        break;

      case L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE:
        ethType = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
        break;

      case L7_QOS_ACL_TLV_MATCH_DSTIPV6_TYPE:
      case L7_QOS_ACL_TLV_MATCH_SRCIPV6_TYPE:
      case L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_TYPE:
        ipv6QualifierSpecified = L7_TRUE;
        break;

      default:
        break;
      }

      if (ipv6QualifierSpecified == L7_TRUE)
        break;

      classRuleLen -= (osapiNtohl(pMatchTLV->length) + sizeof(pMatchTLV->type) + sizeof(pMatchTLV->length));
      /* get next match criteria */
      pMatchTLV = GET_NEXT_TLV(pMatchTLV);
    }

    if (ipv6QualifierSpecified == L7_TRUE)
      break;

    classDefLen -= (osapiNtohl(pRuleTLV->length) + sizeof(pRuleTLV->type) + sizeof(pRuleTLV->length));
    /* get next rule */
    pRuleTLV = GET_NEXT_TLV(pRuleTLV);
  }

  /* Next, get a count of rules. */

  classDefLen = osapiNtohl(pListTLV->length) - 4; /* subtract 4 to account for instanceId */

  /* point to first rule */
  pRuleTLV = GET_SUB_TLV(pListTLV,4);

  /* iterate through class rules */
  while (classDefLen > 0)
  {
    if (osapiNtohl(pRuleTLV->type) != L7_QOS_ACL_TLV_RULE_DEF)
    {
      return;
    }

    hapiBroadQosAclRuleIsIpv6(pRuleTLV, ipv6QualifierSpecified, &isIpv6Rule);

    if (isIpv6Rule == L7_TRUE)
    {
      v6Count++;
    }
    else
    {
      l2v4Count++;
    }

    classDefLen -= (osapiNtohl(pRuleTLV->length) + sizeof(pRuleTLV->type) + sizeof(pRuleTLV->length));

    /* get next rule */
    pRuleTLV = GET_NEXT_TLV(pRuleTLV);
  }

  *l2v4RuleCount = l2v4Count;
  *v6RuleCount   = v6Count;
}

/*********************************************************************
*
* @purpose Add a ACL instance to an inbound interface
*
* @param   DAPI_USP_t         *usp
* @param   L7_uint32           instanceId
* @param   L7_tlv_t            pListTLV
* @param   DAPI_t             *dapi_g
* @param   BROAD_POLICY_t     *policy
* @param   HAPI_QOS_INTF_DIR_t direction,
* @param   L7_BOOL             processV6RulesOnly
* @param   L7_BOOL             ipv6RuleFound
*
* @returns L7_RC_t
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosAclInstAdd(DAPI_USP_t               *usp,
                                      L7_uint32                 instanceId,
                                      L7_tlv_t                 *pListTLV,
                                      DAPI_t                   *dapi_g,
                                      BROAD_POLICY_t           *policyId,
                                      HAPI_QOS_INTF_DIR_t       direction,
                                      L7_BOOL                   processV6RulesOnly,
                                      L7_BOOL                   ipv6RuleFound)
{
    L7_RC_t                   result = L7_SUCCESS;
    L7_tlv_t                 *pRuleTLV;
    L7_tlv_t                 *pMatchTLV;
    L7_int32                  classDefLen, classRuleLen;
    BROAD_PORT_t             *hapiPortPtr;
    BROAD_POLICY_t            aclId;
    BROAD_POLICY_RULE_t       ruleId;
    DAPI_PORT_t              *dapiPortPtr;
    L7_uint32                 data32, mask32;
    L7_ushort16               data16, mask16;
    L7_BOOL                   ruleIsIpv6;
    BROAD_POLICY_RULE_t       loggingRuleId[L7_ACL_LOG_RULE_LIMIT];
    BROAD_POLICY_RULE_t       loggingRuleCorrelator[L7_ACL_LOG_RULE_LIMIT];
    L7_uint32                 loggingRuleCount = 0;
    L7_uint32                 timeBasedRuleCount = 0;
    DAPI_USP_t                dependentLag[L7_MAX_NUM_LAG_INTF];
    L7_uint32                 dependentLagCount = 0;
    L7_uint32                 i;
    L7_BOOL                   l3OrL4RuleFound = L7_FALSE;

    *policyId = BROAD_POLICY_INVALID;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    if ( L7_FALSE == IS_PORT_TYPE_CPU( dapiPortPtr ) )
    {
      result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);
    }
    else
    {
      result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_VLAN);
    }
    if (L7_SUCCESS != result)
    {
      hapiBroadPolicyCreateCancel();
      return result;
    }

    if (direction == HAPI_QOS_INTF_DIR_IN)
    {
      result = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_INGRESS);
    }
    else
    {
      result = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_EGRESS);
    }
    if (L7_SUCCESS != result)
    {
      hapiBroadPolicyCreateCancel();
      return result;
    }

    classDefLen = osapiNtohl(pListTLV->length) - 4; /* subtract 4 to account for instanceId */

    /* point to first rule */
    pRuleTLV = GET_SUB_TLV(pListTLV,4);

    /* iterate through class rules */
    while (classDefLen > 0)
    {
        L7_uchar8   denyFlag;
        L7_uint32   correlator;
        L7_BOOL     matchEvery;
        L7_uchar8   defaultMask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
            FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
        L7_ushort16 ethType;
        L7_ushort16 ethType2 = 0;
        /* Time Based ACLs variables*/
        L7_uint32   trCorrelator;   /* Time based Correlator */
        L7_ushort16 ruleStatus;     /* Rule status */ 
        L7_ushort16 status;     /* Rule status */ 

        if (osapiNtohl(pRuleTLV->type) != L7_QOS_ACL_TLV_RULE_DEF)
        {
          hapiBroadPolicyCreateCancel();
          return L7_FAILURE;
        }

        /* Determine if we should process this rule at this time. */
        hapiBroadQosAclRuleIsIpv6(pRuleTLV, ipv6RuleFound, &ruleIsIpv6);

        /* If the rule is an IPv6 rule and we're not processing IPv6 rules,
           or if the rule is not an IPv6 rule and we are processing IPv6 rules,
           skip to the next rule in the TLV. */
        if ((ruleIsIpv6 && !processV6RulesOnly) ||
            (!ruleIsIpv6 && processV6RulesOnly))
        {
          classDefLen -= (osapiNtohl(pRuleTLV->length) + sizeof(pRuleTLV->type) + sizeof(pRuleTLV->length));
          /* get next rule */
          pRuleTLV = GET_NEXT_TLV(pRuleTLV);

          continue;
        }

        result = hapiBroadPolicyRuleAdd(&ruleId);
        if (L7_SUCCESS != result)
        {
            (void)hapiBroadPolicyCreateCancel();
            return result;
        }

        denyFlag     = *GET_VALUE_PTR(pRuleTLV,0);
        correlator   = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pRuleTLV,4));
        classRuleLen = osapiNtohl(pRuleTLV->length) - L7_QOS_ACL_TLV_RULE_DEF_LEN;    /* account for TLV header fields */
        pMatchTLV    = GET_SUB_TLV(pRuleTLV,L7_QOS_ACL_TLV_RULE_DEF_LEN); /* point to first match criterion */

        ethType = 0;

        /* iterate through match criteria for this rule */
        matchEvery = L7_FALSE;
        while ((classRuleLen > 0) && (L7_FALSE == matchEvery))
        {

            switch (osapiNtohl(pMatchTLV->type))
            {
            case L7_QOS_ACL_TLV_MATCH_DSTIP_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_DIP,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_DSTL4PORT_TYPE:
                /* TODO: Add L4 port range support */
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_DPORT,
                                                         (L7_uchar8 *)&data16,
                                                         (L7_uchar8 *)&mask16);
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_IPTOS_TYPE:
                if (ethType == 0x0800)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                           BROAD_FIELD_DSCP,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else if (ethType == 0x86dd)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                           BROAD_FIELD_IP6_TRAFFIC_CLASS,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else
                {
                  result = L7_FAILURE;
                }
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE:
                if (ethType == 0x0800)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                           BROAD_FIELD_PROTO,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else if (ethType == 0x86dd)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                           BROAD_FIELD_IP6_NEXTHEADER,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else
                {
                  result = L7_FAILURE;
                }
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCIP_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_SIP,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCL4PORT_TYPE:
                /* TODO: Add L4 port range support */
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_SPORT,
                                                         (L7_uchar8 *)&data16,
                                                         (L7_uchar8 *)&mask16);
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE:

                ethType = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_ETHTYPE,
                                                         (L7_uchar8 *)&ethType,
                                                         defaultMask);
                ethType2 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,2)));
                break;

            case L7_QOS_ACL_TLV_MATCH_VLANID_TYPE:
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_OVID,
                                                         (L7_uchar8 *)&data16,
                                                         defaultMask);
                break;

            case L7_QOS_ACL_TLV_MATCH_COS_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_OCOS,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         defaultMask);
                break;

            case L7_QOS_ACL_TLV_MATCH_VLANID2_TYPE:
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IVID,
                                                         (L7_uchar8 *)&data16,
                                                         defaultMask);
                break;

            case L7_QOS_ACL_TLV_MATCH_COS2_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_ICOS,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         defaultMask);
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCMAC_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_MACSA,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,6));
                break;

            case L7_QOS_ACL_TLV_MATCH_DSTMAC_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_MACDA,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,6));
                break;

            case L7_QOS_ACL_TLV_MATCH_DSTIPV6_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IP6_DST,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,16));
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCIPV6_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IP6_SRC,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,16));
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IP6_FLOWLABEL,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_MATCH_ICMPMSG_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_ICMP_MSG_TYPE,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,1));
                l3OrL4RuleFound = L7_TRUE;
                break;

            case L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_TYPE:
                if (0 == denyFlag)    /* deny action takes precedence */
                {
                    result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                          BROAD_ACTION_SET_COSQ,
                                                          *GET_VALUE_PTR(pMatchTLV,0),
                                                          0,
                                                          0);
                }
                break;

            case L7_QOS_ACL_TLV_ATTR_REDIRECT_TYPE:
                if (0 == denyFlag)    /* deny action takes precedence */
                {
                    DAPI_USP_t redirUsp;

                    redirUsp.unit = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,0));
                    redirUsp.slot = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,4));
                    redirUsp.port = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,8));

                    result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                          BROAD_ACTION_REDIRECT,
                                                          redirUsp.unit,
                                                          redirUsp.slot,
                                                          redirUsp.port);

                    if (L7_LAG_SLOT_NUM == redirUsp.slot)
                    {
                      /* This policy now depends upon the LAG as it's destination. */
                      /* Make sure we don't add the same LAG USP twice. */
                      for (i = 0; i < dependentLagCount; i++)
                      {
                        if (dependentLag[dependentLagCount].port == redirUsp.port)
                          break;
                      }
                      if ((i == dependentLagCount) && (dependentLagCount < L7_MAX_NUM_LAG_INTF))
                      {
                        dependentLag[dependentLagCount] = redirUsp;
                        dependentLagCount++;
                      }
                    }
                }
                break;

            case L7_QOS_ACL_TLV_ATTR_MIRROR_TYPE:
                if (0 == denyFlag)    /* deny action takes precedence */
                {
                    DAPI_USP_t mirrorUsp;
                    BROAD_METER_ENTRY_t meterInfo;

//                  mirrorUsp.unit = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,0));
//                  mirrorUsp.slot = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,4));
//                  mirrorUsp.port = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,8));
                    mirrorUsp.unit = 0;
                    mirrorUsp.slot = L7_CPU_SLOT_NUM;
                    mirrorUsp.port = 0;

                    result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                          BROAD_ACTION_MIRROR,
                                                          mirrorUsp.unit,
                                                          mirrorUsp.slot,
                                                          mirrorUsp.port);

                    if (L7_LAG_SLOT_NUM == mirrorUsp.slot)
                    {
                      /* This policy now depends upon the LAG as it's destination. */
                      /* Make sure we don't add the same LAG USP twice. */
                      for (i = 0; i < dependentLagCount; i++)
                      {
                        if (dependentLag[dependentLagCount].port == mirrorUsp.port)
                          break;
                      }
                      if ((i == dependentLagCount) && (dependentLagCount < L7_MAX_NUM_LAG_INTF))
                      {
                        dependentLag[dependentLagCount] = mirrorUsp;
                        dependentLagCount++;
                      }
                    }
                    else if (L7_CPU_SLOT_NUM == mirrorUsp.slot)
                    {
                        meterInfo.cir       = RATE_LIMIT_PCAP;
                        meterInfo.cbs       = 128;
                        meterInfo.pir       = RATE_LIMIT_PCAP;
                        meterInfo.pbs       = 128;
                        meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

                        result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
                        if (result != L7_SUCCESS)  break;
                    }
                }
                break;
            /* Timed Based ACLs - Reading the correaltor and rule status information from the TLV */
            case L7_QOS_ACL_TLV_RULE_STATUS:
                 ruleStatus = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));             
                 trCorrelator = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,4));
                 if(ruleStatus == L7_ACL_RULE_STATUS_INACTIVE)
                 {
                    status = BROAD_POLICY_RULE_STATUS_INACTIVE;
                 }
                 else                  {
                    status = BROAD_POLICY_RULE_STATUS_ACTIVE;
                 }                 

                 if(L7_QOS_ACL_TLV_RULE_CORR_NULL != trCorrelator)
                 {
                      result = hapiBroadPolicyRuleStatusAdd(ruleId, status);
                     if(result == L7_SUCCESS)
                     {
                        /* save a non-null correlator so that it may be used later when the
                         * time range notification is received by the application
                         */
                         timeBasedRuleId[timeBasedRuleCount]         = ruleId;
                         timeBasedRuleCorrelator[timeBasedRuleCount] = trCorrelator;
                         timeBasedRuleCount++;
                      }
                 }                             
            break;
            case L7_QOS_ACL_TLV_MATCH_EVERY_TYPE:
                /* Once we hit a match every, the rest are don't cares */
                matchEvery = L7_TRUE;
                break;

            default:
                /* unsupported match criteria */
                result = L7_FAILURE;
            }

            if (L7_SUCCESS != result)
            {
                (void)hapiBroadPolicyCreateCancel();
                return result;
            }

            classRuleLen -= (osapiNtohl(pMatchTLV->length) + sizeof(pMatchTLV->type) + sizeof(pMatchTLV->length));
            /* get next match criteria */
            pMatchTLV = GET_NEXT_TLV(pMatchTLV);
        }

        if (denyFlag)
        {
            result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
            if (L7_SUCCESS == result)
            {
                if (L7_QOS_ACL_TLV_RULE_CORR_NULL != correlator)
                {
                    result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
                    if (L7_SUCCESS == result)
                    {
                        /* save a non-null correlator so that it may be used later when the
                         * counter is requested by the application
                         *
                         * NOTE:  There is no guarantee that the counter will be successfully
                         *        committed for the policy, but need to store the correlator
                         *        while it is still available from the TLV.
                         */
                        loggingRuleId[loggingRuleCount]         = ruleId;
                        loggingRuleCorrelator[loggingRuleCount] = correlator;
                        loggingRuleCount++;
                    }
                }
            }
        }
        else
        {
            result = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_PERMIT, 0, 0, 0);
        }

        /* Support for multiple ETHTYPEs requires copying the existing rule. */
        if ((L7_SUCCESS == result) && (ethType2))
        {
            BROAD_POLICY_RULE_t newRule;

            result = hapiBroadPolicyRuleCopy(ruleId, &newRule);
            if (L7_SUCCESS == result)
            {
                /* update new rule with ETHTYPE2 value */
                result = hapiBroadPolicyRuleQualifierAdd(newRule,
                                                         BROAD_FIELD_ETHTYPE,
                                                         (L7_uchar8*)&ethType2,
                                                         defaultMask);
            }
        }

        if (L7_SUCCESS != result)
        {
            (void)hapiBroadPolicyCreateCancel();
            return result;
        }

        classDefLen -= (osapiNtohl(pRuleTLV->length) + sizeof(pRuleTLV->type) + sizeof(pRuleTLV->length));

        /* get next rule */
        pRuleTLV = GET_NEXT_TLV(pRuleTLV);
    }

    if (l3OrL4RuleFound == L7_FALSE)
    {
      /* If there are no L3 or L4 rules, then that implies this is a MAC ACL. 
         In the case of MAC ACLs, we need to strictly qualify on the ethertype qualifiers. 
         Otherwise, the policy manager may elect to replace ethertypes of 0x0800 or 0x86dd 
         w/ IpType qualifiers, which require correct IP headers to get a rule match. */
      result = hapiBroadPolicyStrictEthTypeSet(L7_TRUE);
      if (L7_SUCCESS != result)
      {
        hapiBroadPolicyCreateCancel();
        return result;
      }
    }

    result = hapiBroadPolicyCommit(&aclId);
    if (L7_SUCCESS != result)
    {
        return result;
    }

    /* Update the correlator table for log entries*/
    for (i = 0; i < loggingRuleCount; i++)
    {
      result = hapiBroadQosAclCorrelatorAdd(aclId, loggingRuleId[i], loggingRuleCorrelator[i], L7_TRUE, L7_TRUE);
      if (L7_SUCCESS != result)
      {
       hapiBroadQosAclCorrelatorDelete(aclId);
       (void)hapiBroadPolicyDelete(aclId);
        return result;
      }
    }
    /* Time Based ACLs - Update the correlator table for time based entries */
    for (i = 0; i < timeBasedRuleCount; i++)
    {
      result = hapiBroadQosAclCorrelatorAdd(aclId, timeBasedRuleId[i], timeBasedRuleCorrelator[i], L7_TRUE, L7_FALSE);
      if (L7_SUCCESS != result)
      {
         hapiBroadQosAclCorrelatorDelete(aclId);
        (void)hapiBroadPolicyDelete(aclId);
        return result;
      }
    }

    /* Update the LAG dependency lists */
    for (i = 0; i < dependentLagCount; i++)
    {
      if (hapiBroadQosAddDepends(aclId, &dependentLag[i], dapi_g) != L7_SUCCESS)
      {
        hapiBroadQosAclCorrelatorDelete(aclId);
        (void)hapiBroadPolicyDelete(aclId);
        return result;
      }
    }

    *policyId = aclId;

    return result;
}

/*********************************************************************
*
* @functions hapiBroadQosAclAdd
*
* @purpose Adds an Access Control List to an interface
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t  cmd
* @param void       *data
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_ACL_ADD */
L7_RC_t hapiBroadQosAclAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                   l2V4Result = L7_SUCCESS;
  L7_RC_t                   v6Result   = L7_SUCCESS;
  DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
  L7_uint32                 instanceId;
  L7_tlv_t                 *pTLV;
  DAPI_PORT_t              *dapiPortPtr;
  BROAD_PORT_t             *hapiPortPtr;
  HAPI_BROAD_QOS_PORT_t    *qosPort;
  BROAD_POLICY_t            l2v4PolicyId; /* This policy holds the MAC and IPv4 ACLs.*/
  BROAD_POLICY_t            v6PolicyId;   /* This policy holds the IPv6 ACLs.        */
  HAPI_QOS_INTF_DIR_t       direction;
  int                       idx;
  BROAD_POLICY_t           *portPolicyIdPtr;
  L7_uint32                 l2v4RuleCount, v6RuleCount;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) &&
      (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE) &&
      (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_FALSE))
  {
    /* only allow physical ports, LAGs, and CPU ports (for VLAN ACLs) */
    return L7_FAILURE;
  }

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  qosPort = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

  pTLV = qosCmd->cmdData.aclInstAdd.pTLV;

  if (osapiNtohl(pTLV->type) != L7_QOS_ACL_TLV_LIST_TYPE)
  {
    return L7_FAILURE;
  }

  if (qosCmd->cmdData.aclInstAdd.ifDirection == DAPI_QOS_INTF_DIR_IN)
  {
    direction = HAPI_QOS_INTF_DIR_IN;
  }
  else
  {
    direction = HAPI_QOS_INTF_DIR_OUT;
  }

  hapiBroadQosSemTake(dapi_g);

  /* Check to see if there are any dot1x policies on this port. */
  if (direction == HAPI_QOS_INTF_DIR_IN)
  {
    if (hapiBroadDot1xPoliciesApplied(usp, dapi_g) == L7_TRUE)
    {
      hapiBroadQosSemGive(dapi_g);
      return L7_FAILURE;
    }
  }

  if ((L7_TRUE              == IS_PORT_TYPE_CPU( dapiPortPtr )) ||
      (BROAD_POLICY_INVALID == qosPort->aclds.policyId[direction]))
  {
    instanceId = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pTLV,0));

    portPolicyIdPtr = &qosPort->aclds.policyId[direction];

    if (hapiBroadQosAclPolicyTLVFind(pTLV, direction, &idx) == L7_TRUE)
    {
      /* reuse existing policy */
      l2v4PolicyId = broadAclTable[idx].l2v4PolicyId;
      if (l2v4PolicyId != BROAD_POLICY_INVALID)
      {
        /* clear any counters in the ACL correlator table, since the
         * HW counts will be reset when the policy is applied
         */
        hapiBroadQosAclCorrelatorCountClear(l2v4PolicyId);

        l2V4Result = hapiBroadQosApplyPolicy(l2v4PolicyId, usp, dapi_g);
        if (L7_SUCCESS == l2V4Result)
        {
          *portPolicyIdPtr = l2v4PolicyId;  /* update port QoS table */
          portPolicyIdPtr = &qosPort->aclds.policyId2[direction];
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                  "ACL failed to apply on port %d\n", hapiPortPtr->bcmx_lport);
        }
      }
      v6PolicyId = broadAclTable[idx].v6PolicyId;
      if (v6PolicyId != BROAD_POLICY_INVALID)
      {
        /* clear any counters in the ACL correlator table, since the
         * HW counts will be reset when the policy is applied
         */
        hapiBroadQosAclCorrelatorCountClear(v6PolicyId);

        v6Result = hapiBroadQosApplyPolicy(v6PolicyId, usp, dapi_g);
        if (L7_SUCCESS == v6Result)
        {
          *portPolicyIdPtr = v6PolicyId;  /* update port QoS table */
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                  "ACL failed to apply on port %d\n", hapiPortPtr->bcmx_lport);
          /* Clean up l2V4 ACL if it was applied. */
          if (qosPort->aclds.policyId[direction] != BROAD_POLICY_INVALID)
          {
            hapiBroadQosRemovePolicy(qosPort->aclds.policyId[direction], usp, dapi_g);
            qosPort->aclds.policyId[direction] = BROAD_POLICY_INVALID;
          }
        }
      }
      if ((L7_SUCCESS == v6Result) && (L7_SUCCESS == l2V4Result))
      {
        broadAclTable[idx].ifCount++;   /* update ACL table */
      }
    }
    else
    {
      /* create a new policy */
      /* Determine if the ACL has IPv6 qualifiers in any of the rules. */
      hapiBroadQosAclRuleCount(pTLV, &l2v4RuleCount, &v6RuleCount);
      l2v4PolicyId = BROAD_POLICY_INVALID;
      v6PolicyId   = BROAD_POLICY_INVALID;

      if (l2v4RuleCount)
      {
        l2V4Result = hapiBroadQosAclInstAdd(usp,
                                            instanceId,
                                            pTLV,
                                            dapi_g,
                                            &l2v4PolicyId,
                                            direction,
                                            L7_FALSE,
                                            (v6RuleCount ? L7_TRUE : L7_FALSE));
      }
      if (v6RuleCount)
      {
        v6Result = hapiBroadQosAclInstAdd(usp,
                                          instanceId,
                                          pTLV,
                                          dapi_g,
                                          &v6PolicyId,
                                          direction,
                                          L7_TRUE,
                                          (v6RuleCount ? L7_TRUE : L7_FALSE));
      }
      if ((L7_SUCCESS == l2V4Result) && (L7_SUCCESS == v6Result))
      {
        /* save the policy info */
        l2V4Result = hapiBroadQosAclPolicyTLVAdd(pTLV, l2v4PolicyId, v6PolicyId, direction, instanceId);
        if (L7_SUCCESS == l2V4Result)
        {
          if (l2v4PolicyId != BROAD_POLICY_INVALID)
          {
            l2V4Result = hapiBroadQosApplyPolicy(l2v4PolicyId, usp, dapi_g);
            if (L7_SUCCESS == l2V4Result)
            {
              *portPolicyIdPtr = l2v4PolicyId;  /* update port QoS table */
              portPolicyIdPtr = &qosPort->aclds.policyId2[direction];
            }
          }
          if (v6PolicyId != BROAD_POLICY_INVALID)
          {
            v6Result = hapiBroadQosApplyPolicy(v6PolicyId, usp, dapi_g);
            if (L7_SUCCESS == v6Result)
            {
              *portPolicyIdPtr = v6PolicyId;  /* update port QoS table */
            }
          }
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "ACL internal table overflow."
            " Attempting to add an ACL to a full table.");
        }
      }

      if ((L7_SUCCESS != l2V4Result) || (L7_SUCCESS != v6Result))
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "ACL not applied to port %d\n", hapiPortPtr->bcmx_lport);

        if (hapiBroadQosAclPolicyTLVFind(pTLV, direction, &idx) == L7_TRUE)
        {
          broadAclTable[idx].ifCount      = 0;
          broadAclTable[idx].used         = L7_FALSE;        /* update ACL table */
          broadAclTable[idx].l2v4PolicyId = BROAD_POLICY_INVALID;
          broadAclTable[idx].v6PolicyId   = BROAD_POLICY_INVALID;
        }

        if (l2v4PolicyId != BROAD_POLICY_INVALID)
        {
          /* Delete all correlators for this ACL */
          hapiBroadQosAclCorrelatorDelete(l2v4PolicyId);

          /* Delete policy and remove any dependencies it may have on other ports. */
          (void)hapiBroadPolicyDelete(l2v4PolicyId);
          (void)hapiBroadQosDelDependsAll(l2v4PolicyId, dapi_g);
        }
        if (v6PolicyId != BROAD_POLICY_INVALID)
        {
          /* Delete all correlators for this ACL */
          hapiBroadQosAclCorrelatorDelete(v6PolicyId);

          /* Delete policy and remove any dependencies it may have on other ports. */
          (void)hapiBroadPolicyDelete(v6PolicyId);
          (void)hapiBroadQosDelDependsAll(v6PolicyId, dapi_g);
        }

        qosPort->aclds.policyId[direction]  = BROAD_POLICY_INVALID;
        qosPort->aclds.policyId2[direction] = BROAD_POLICY_INVALID;
      }
    }
  }
  else
  {
    /* policy already on this port */
    l2V4Result = L7_FAILURE;
  }

  hapiBroadQosSemGive(dapi_g);

  if ((L7_SUCCESS == l2V4Result) && (L7_SUCCESS == v6Result))
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*********************************************************************
*
* @functions hapiBroadQosAclDelete
*
* @purpose Deletes an Access Control List from an interface
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t  cmd
* @param void       *data
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_ACL_DELETE */
L7_RC_t hapiBroadQosAclDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                   l2V4Result = L7_SUCCESS;
  L7_RC_t                   v6Result   = L7_SUCCESS;
  DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
  L7_uint32                 instanceId;
  L7_uint32                 numInstanceIds;
  L7_tlv_t                 *pTLV;
  DAPI_PORT_t              *dapiPortPtr;
  BROAD_PORT_t             *hapiPortPtr;
  HAPI_BROAD_QOS_PORT_t    *qosPort;
  int                       idx;
  BROAD_POLICY_t            l2v4PolicyId;
  BROAD_POLICY_t            v6PolicyId;
  HAPI_QOS_INTF_DIR_t       direction;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) &&
      (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE) &&
      (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_FALSE))
  {
    /* only allow physical ports, LAGs, and CPU ports (for VLAN ACLs) */
    return L7_FAILURE;
  }

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  qosPort     = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

  pTLV = qosCmd->cmdData.aclInstDelete.pTLV;

  if (osapiNtohl(pTLV->type) != L7_QOS_ACL_TLV_INST_DEL_LIST_TYPE)
  {
    return L7_ERROR;
  }

  if (qosCmd->cmdData.aclInstDelete.ifDirection == DAPI_QOS_INTF_DIR_IN)
  {
    direction = HAPI_QOS_INTF_DIR_IN;
  }
  else
  {
    direction = HAPI_QOS_INTF_DIR_OUT;
  }

  numInstanceIds = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pTLV,0));

  if (numInstanceIds != 1)
  {
    /* there should be only one ACL instance per interface */
    return L7_FAILURE;
  }

  instanceId = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pTLV,4));

  hapiBroadQosSemTake(dapi_g);

  if (( L7_FALSE             == IS_PORT_TYPE_CPU( dapiPortPtr )) &&
      ( BROAD_POLICY_INVALID == qosPort->aclds.policyId[direction] ))
  {
    /* no policy on this port */
    hapiBroadQosSemGive(dapi_g);
    return L7_ERROR;
  }

  if ( hapiBroadQosAclPolicyIdFindInstance( instanceId, direction, &idx ) != L7_TRUE )
  {
    hapiBroadQosSemGive( dapi_g );
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "ACL instance 0x%x not found in internal table\n", instanceId );
    return( L7_FAILURE );
  }
  l2v4PolicyId = broadAclTable[idx].l2v4PolicyId;
  v6PolicyId   = broadAclTable[idx].v6PolicyId;

  /* Remove the policy from this interface. */
  if (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_FALSE)
  {
    if (l2v4PolicyId != BROAD_POLICY_INVALID)
    {
      l2V4Result = hapiBroadQosRemovePolicy( l2v4PolicyId, usp, dapi_g );
    }
    if (v6PolicyId != BROAD_POLICY_INVALID)
    {
      v6Result   = hapiBroadQosRemovePolicy( v6PolicyId, usp, dapi_g );
    }
  }

  if ((L7_SUCCESS != l2V4Result) || (L7_SUCCESS != v6Result))
  {
    /* continue to clean-up even if an error occurs */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "ACL not removed from port %d\n", hapiPortPtr->bcmx_lport);
  }

  if (--broadAclTable[idx].ifCount <= 0)
  {
    if (l2v4PolicyId != BROAD_POLICY_INVALID)
    {
      /* Delete all correlators for this ACL */
      hapiBroadQosAclCorrelatorDelete(l2v4PolicyId);

      /* Delete policy and remove any dependencies it may have on other ports. */
      if (hapiBroadPolicyDelete(l2v4PolicyId) != L7_SUCCESS)
        l2V4Result = L7_FAILURE;

      if (hapiBroadQosDelDependsAll(l2v4PolicyId, dapi_g) != L7_SUCCESS)
        l2V4Result = L7_FAILURE;
    }
    if (v6PolicyId != BROAD_POLICY_INVALID)
    {
      /* Delete all correlators for this ACL */
      hapiBroadQosAclCorrelatorDelete(v6PolicyId);

      /* Delete policy and remove any dependencies it may have on other ports. */
      if (hapiBroadPolicyDelete(v6PolicyId) != L7_SUCCESS)
        v6Result = L7_FAILURE;

      if (hapiBroadQosDelDependsAll(v6PolicyId, dapi_g) != L7_SUCCESS)
        v6Result = L7_FAILURE;
    }

    broadAclTable[idx].used         = L7_FALSE;        /* update ACL table */
    broadAclTable[idx].l2v4PolicyId = BROAD_POLICY_INVALID;
    broadAclTable[idx].v6PolicyId   = BROAD_POLICY_INVALID;
  }

  qosPort->aclds.policyId[direction]    = BROAD_POLICY_INVALID;
  qosPort->aclds.policyId2[direction]   = BROAD_POLICY_INVALID;

  hapiBroadQosSemGive(dapi_g);

  if ((L7_SUCCESS == l2V4Result) && (L7_SUCCESS == v6Result))
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*********************************************************************
*
* @functions hapiBroadQosAclRuleCountGet
*
* @purpose Get the ACL rule counter for the specified correlator value
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t  cmd
* @param void       *data
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_ACL_RULE_COUNT_GET */
L7_RC_t hapiBroadQosAclRuleCountGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                   result = L7_NOT_EXIST;
  DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
  DAPI_PORT_t              *dapiPortPtr;
  L7_uint32                 idx;
  L7_uint32                 correlator;
  L7_BOOL                   found;
  BROAD_POLICY_t            aclId;
  BROAD_POLICY_RULE_t       ruleId;
  BROAD_POLICY_STATS_t      stat;
  L7_uint64                 prev_val, delta_val;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) &&
      (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE))
  {
    /* only allow physical ports and LAGs */
    return L7_FAILURE;
  }

  hapiBroadQosSemTake(dapi_g);

  /* search the ACL correlator table for the matching entry */
  correlator = qosCmd->cmdData.aclRuleCountGet.correlator;

  delta_val = 0;
  found = hapiBroadQosAclCorrelatorFind(correlator, &idx);
  while (L7_TRUE == found)
  {
    aclId = broadAclCorrTable[idx].aclId;
    ruleId = broadAclCorrTable[idx].ruleId;

    result = hapiBroadPolicyStatsGet(aclId, ruleId, &stat);

    if (result == L7_SUCCESS)
    {
      if (L7_FALSE == stat.meter)
      {
        /* save latest hit count and calculate delta from the previous count
         *
         * NOTE:  Always subtract prev count from latest.  The unsigned delta 
         *        value will be correct even if the HW counter wrapped.
         */
        prev_val = broadAclCorrTable[idx].lastCount;
        broadAclCorrTable[idx].lastCount = stat.statMode.counter.count;

        delta_val += (broadAclCorrTable[idx].lastCount - prev_val);
        result = L7_SUCCESS;

      } /* endif HW counter exists */
    }

    found = hapiBroadQosAclCorrelatorFindNext(correlator, &idx);
  }

  if (L7_SUCCESS == result)
  {
    if (L7_NULLPTR != qosCmd->cmdData.aclRuleCountGet.hitCount)
    {
      qosCmd->cmdData.aclRuleCountGet.hitCount->high = (L7_uint32)(delta_val >> 32) & 0xffffffff;
      qosCmd->cmdData.aclRuleCountGet.hitCount->low  = (L7_uint32)delta_val & 0xffffffff;
    }
  }

  hapiBroadQosSemGive(dapi_g);

  return result;
}
/*********************************************************************
*
* @purpose  Set status for policy Rule ID
*
* @param    BROAD_POLICY_t    policy
* @param    DAPI_USP_t       *usp
* @param    DAPI_t           *dapi_g
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_ACL_RULE_STATUS_SET*/
L7_RC_t hapiBroadQosAclRuleStatusSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                   result = L7_NOT_EXIST;
    DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
    DAPI_PORT_t              *dapiPortPtr;
    L7_uint32                 idx;
    L7_uint32                 correlator;
    L7_ushort16               ruleStatus;
    L7_ushort16               status;
    L7_BOOL                   found;
    BROAD_POLICY_t            aclId;
    BROAD_POLICY_RULE_t       ruleId;

    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) &&
        (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE))
    {
        /* only allow physical ports and LAGs */
        return L7_FAILURE;
    }


    hapiBroadQosSemTake(dapi_g);

    correlator = qosCmd->cmdData.aclRuleStatusSet.correlator;
    ruleStatus = qosCmd->cmdData.aclRuleStatusSet.status;
     
    if(ruleStatus == L7_ACL_RULE_STATUS_INACTIVE)
    {
       status = BROAD_POLICY_RULE_STATUS_INACTIVE;
    }
    else if(ruleStatus == L7_ACL_RULE_STATUS_ACTIVE)
    {
       status = BROAD_POLICY_RULE_STATUS_ACTIVE;
    }
    else
    {
       return L7_FAILURE;
    }
 
    found = hapiBroadQosAclCorrelatorFind(correlator, &idx);
    while (L7_TRUE == found)
    {
        aclId = broadAclCorrTable[idx].aclId;
        ruleId = broadAclCorrTable[idx].ruleId;
        /* reinstall rule in hardware activating/deactivating */
        result = hapiBroadPolicyRuleStatusSet(aclId, ruleId, status);                
        found = hapiBroadQosAclCorrelatorFindNext(correlator, &idx);
    }


    hapiBroadQosSemGive(dapi_g);

    return result;

}

/* Debug */

void hapiBroadQosAclDebugTable()
{
    int i;
    L7_uint32 maxAclPolicies;

    maxAclPolicies = hapiBroadMaxAclPolicies();

    sysapiPrintf("ACL Table\n");
    for (i = 0; i < maxAclPolicies; i++)
    {
        if (L7_TRUE == broadAclTable[i].used)
        {
            sysapiPrintf("[%2d] ", i);
            sysapiPrintf("instanceId %08x, l2v4PolicyId %2d, v6PolicyId %2d, direction %s, hash %08x, count %d\n",
                         broadAclTable[i].instanceId,
                         broadAclTable[i].l2v4PolicyId,
                         broadAclTable[i].v6PolicyId,
                         (broadAclTable[i].direction == HAPI_QOS_INTF_DIR_IN) ? "In" : "Out",
                         broadAclTable[i].hashVal,
                         broadAclTable[i].ifCount);
        }
    }
}

void hapiBroadQosAclDebugCorrelatorTable()
{
    int         i;
    L7_uint32   h_val, l_val;
    L7_uint32 maxAclEntries = 0;
    maxAclEntries = hapiBroadMaxAclCorrTableEntries();

    sysapiPrintf("ACL Correlator Table\n");
    for (i = 0; i < maxAclEntries; i++)
    {
        if (L7_QOS_ACL_TLV_RULE_CORR_NULL != broadAclCorrTable[i].correlator)
        {
            h_val = (L7_uint32)(broadAclCorrTable[i].lastCount >> 32) & 0xffffffff;
            l_val = (L7_uint32)broadAclCorrTable[i].lastCount & 0xffffffff;

            sysapiPrintf("[%3d] correlator=0x%8.8x, CorrFlag=%d,  aclId=%3d, ruleId=%3d, "
                         "addedToPolicy=%d lastCount=0x%8.8x-%8.8x\n, logCount=%d",
                         i,
                         broadAclCorrTable[i].correlator,
                         broadAclCorrTable[i].corrFlag, 
                         broadAclCorrTable[i].aclId,
                         broadAclCorrTable[i].ruleId,
                         broadAclCorrTable[i].addedToPolicy,
                         h_val, l_val,logCount);
        }
    }
    sysapiPrintf("\nlogCount=%d \n",logCount);
    sysapiPrintf("\nA total of %d entries scanned (empty entries not displayed)\n",
                 i);
}
