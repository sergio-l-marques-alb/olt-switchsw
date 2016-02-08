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

/* ACL Correlator Table Definition (tracks rule correlators received from TLV) */

typedef struct
{
    L7_uint32             correlator;
    BROAD_POLICY_t        aclId;
    BROAD_POLICY_RULE_t   ruleId;
    L7_BOOL               addedToPolicy;
    L7_uint64             lastCount;
} BROAD_ACL_CORR_TABLE_t;

static BROAD_ACL_CORR_TABLE_t  broadAclCorrTable[L7_ACL_LOG_RULE_LIMIT];

/* ACL Instance Table Definition */

typedef struct
{
    L7_BOOL        used;
    L7_uint32      hashVal;
    BROAD_POLICY_t policyId;
    int            ifCount;
    L7_uint32      instanceId;
    HAPI_QOS_INTF_DIR_t direction;
}
BROAD_ACL_TABLE_t;

static BROAD_ACL_TABLE_t *broadAclTable = L7_NULL;

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

    /* init the ACL correlator table */
    for (i = 0; i < L7_ACL_LOG_RULE_LIMIT; i++)
    {
        hapiBroadQosAclCorrelatorEntryReset(i);
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
    return crc32(0L, (L7_uchar8*)GET_SUB_TLV(pTLV,4), pTLV->length-4);
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
    BROAD_POLICY_STAGE_t policyStage;
    L7_uint32            maxAclPolicies;

    /* compute rudimentary hash over TLV */
    hash = hapiBroadQosAclPolicyTLVHash(pTLV);

    maxAclPolicies = hapiBroadMaxAclPolicies();

    for (i = 0; i < maxAclPolicies; i++)
    {
        if ((L7_TRUE == broadAclTable[i].used) && (hash == broadAclTable[i].hashVal))
        {
          if (hapiBroadPolicyStageGet(broadAclTable[i].policyId, &policyStage) == L7_SUCCESS)
          {
            if (((policyStage == BROAD_POLICY_STAGE_INGRESS) && (direction == HAPI_QOS_INTF_DIR_IN)) ||
                ((policyStage == BROAD_POLICY_STAGE_EGRESS) && (direction == HAPI_QOS_INTF_DIR_OUT)))
            {
              *idx = i;       /* found match */
              return L7_TRUE;
            }
          }
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
                                           BROAD_POLICY_t      policy,
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
            broadAclTable[i].used     = L7_TRUE;
            broadAclTable[i].hashVal  = hapiBroadQosAclPolicyTLVHash(pTLV);
            broadAclTable[i].policyId = policy;
            broadAclTable[i].ifCount  = 1;
            broadAclTable[i].instanceId = instanceId; 
            broadAclTable[i].direction  = direction; 

            return L7_SUCCESS;
        }
    }

    return L7_FAILURE;
}

/*********************************************************************
*
* @function hapiBroadQosAclPolicyIdFind
*
* @purpose  Search ACL table for matching policy id
*
* @param    BROAD_POLICY_t  policy
* @param    int            *idx
*
* @returns  L7_BOOL
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_BOOL hapiBroadQosAclPolicyIdFind(BROAD_POLICY_t policy, int *idx)
{
    int i;
    L7_uint32 maxAclPolicies;

    maxAclPolicies = hapiBroadMaxAclPolicies();

    for (i = 0; i < maxAclPolicies; i++)
    {
        if ((L7_TRUE == broadAclTable[i].used) && (policy == broadAclTable[i].policyId))
        {
            *idx = i;       /* found match */
            return L7_TRUE;
        }
    }

    return L7_FALSE;
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

    for (i = 0; i < L7_ACL_LOG_RULE_LIMIT; i++)
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

    for (i = (*idx)+1; i < L7_ACL_LOG_RULE_LIMIT; i++)
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
                                            L7_BOOL             addedToPolicy)
{
    int i;

    for (i = 0; i < L7_ACL_LOG_RULE_LIMIT; i++)
    {
        if (L7_QOS_ACL_TLV_RULE_CORR_NULL == broadAclCorrTable[i].correlator)
        {
            broadAclCorrTable[i].correlator    = correlator;
            broadAclCorrTable[i].aclId         = aclId;
            broadAclCorrTable[i].ruleId        = ruleId;
            broadAclCorrTable[i].addedToPolicy = addedToPolicy;
            broadAclCorrTable[i].lastCount     = 0;

            return L7_SUCCESS;
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

    for (i = 0; i < L7_ACL_LOG_RULE_LIMIT; i++)
    {
        if ((L7_QOS_ACL_TLV_RULE_CORR_NULL != broadAclCorrTable[i].correlator) &&
            (aclId == broadAclCorrTable[i].aclId))
        {
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

    for (i = 0; i < L7_ACL_LOG_RULE_LIMIT; i++)
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
* @purpose Add a ACL instance to an inbound interface
*
* @param   DAPI_USP_t         *usp
* @param   L7_uint32           instanceId
* @param   L7_tlv_t            pListTLV
* @param   DAPI_t             *dapi_g
* @param   BROAD_POLICY_t     *policy
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
                                      HAPI_QOS_INTF_DIR_t       direction)
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

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    if ( L7_FALSE == IS_PORT_TYPE_CPU( dapiPortPtr ) )
    {
      result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT, &aclId);
    }
    else
    {
      result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_VLAN, &aclId);
    }
    if (L7_SUCCESS != result)
        return result;

    if (direction == HAPI_QOS_INTF_DIR_IN)
    {
      result = hapiBroadPolicyStageSet(aclId, BROAD_POLICY_STAGE_INGRESS);
    }
    else
    {
      result = hapiBroadPolicyStageSet(aclId, BROAD_POLICY_STAGE_EGRESS);
    }
    if (L7_SUCCESS != result)
        return result;

    classDefLen = osapiNtohs(pListTLV->length) - 4; /* subtract 4 to account for instanceId */

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

        if (osapiNtohs(pRuleTLV->type) != L7_QOS_ACL_TLV_RULE_DEF)
        {
            return L7_FAILURE;
        }

        result = hapiBroadPolicyRuleAdd(aclId, &ruleId);
        if (L7_SUCCESS != result)
        {
            hapiBroadQosAclCorrelatorDelete(aclId);
            (void)hapiBroadPolicyDelete(aclId);
            return result;
        }

        denyFlag     = *GET_VALUE_PTR(pRuleTLV,0);
        correlator   = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pRuleTLV,4));
        classRuleLen = osapiNtohs(pRuleTLV->length) - L7_QOS_ACL_TLV_RULE_DEF_LEN;    /* account for TLV header fields */
        pMatchTLV    = GET_SUB_TLV(pRuleTLV,L7_QOS_ACL_TLV_RULE_DEF_LEN); /* point to first match criterion */

        ethType = 0;

        /* iterate through match criteria for this rule */
        matchEvery = L7_FALSE;
        while ((classRuleLen > 0) && (L7_FALSE == matchEvery))
        {

            switch (osapiNtohs(pMatchTLV->type))
            {
            case L7_QOS_ACL_TLV_MATCH_DSTIP_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_DIP,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                break;

            case L7_QOS_ACL_TLV_MATCH_DSTL4PORT_TYPE:
                /* TODO: Add L4 port range support */
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_DPORT,
                                                         (L7_uchar8 *)&data16,
                                                         (L7_uchar8 *)&mask16);
                break;

            case L7_QOS_ACL_TLV_MATCH_IPTOS_TYPE:
                if (ethType == 0x0800)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                           BROAD_FIELD_DSCP,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else if (ethType == 0x86dd)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                           BROAD_FIELD_IP6_TRAFFIC_CLASS,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else 
                {
                  result = L7_FAILURE;
                }
                break;

            case L7_QOS_ACL_TLV_MATCH_PROTOCOL_TYPE:
                if (ethType == 0x0800)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                           BROAD_FIELD_PROTO,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else if (ethType == 0x86dd)
                {
                  result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                           BROAD_FIELD_IP6_NEXTHEADER,
                                                           GET_VALUE_PTR(pMatchTLV,0),
                                                           GET_VALUE_PTR(pMatchTLV,1));
                }
                else
                {
                  result = L7_FAILURE;
                }
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCIP_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_SIP,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCL4PORT_TYPE:
                /* TODO: Add L4 port range support */
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_SPORT,
                                                         (L7_uchar8 *)&data16,
                                                         (L7_uchar8 *)&mask16);
                break;

            case L7_QOS_ACL_TLV_MATCH_ETYPE_TYPE:
                
		ethType = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_ETHTYPE,
                                                         (L7_uchar8 *)&ethType,
                                                         defaultMask);
                ethType2 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,2)));
                break;

            case L7_QOS_ACL_TLV_MATCH_VLANID_TYPE:
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_OVID,
                                                         (L7_uchar8 *)&data16,
                                                         defaultMask);
                break;

            case L7_QOS_ACL_TLV_MATCH_COS_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_OCOS,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         defaultMask);
                break;   

            case L7_QOS_ACL_TLV_MATCH_VLANID2_TYPE:
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_IVID,
                                                         (L7_uchar8 *)&data16,
                                                         defaultMask);
                break;

            case L7_QOS_ACL_TLV_MATCH_COS2_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_ICOS,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         defaultMask);
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCMAC_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_MACSA,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,6));
                break;

            case L7_QOS_ACL_TLV_MATCH_DSTMAC_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_MACDA,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,6));
                break;

            case L7_QOS_ACL_TLV_MATCH_DSTIPV6_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_IP6_DST,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,16));
                break;

            case L7_QOS_ACL_TLV_MATCH_SRCIPV6_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_IP6_SRC,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,16));
                break;

            case L7_QOS_ACL_TLV_MATCH_FLOWLBLV6_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_IP6_FLOWLABEL,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                break;

            case L7_QOS_ACL_TLV_MATCH_ICMPMSG_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(aclId, ruleId,
                                                         BROAD_FIELD_ICMP_MSG_TYPE,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,1));
                break;

            case L7_QOS_ACL_TLV_ATTR_ASSIGN_QUEUE_TYPE:
                if (0 == denyFlag)    /* deny action takes precedence */
                {
                    result = hapiBroadPolicyRuleActionAdd(aclId, ruleId,
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

                    result = hapiBroadPolicyRuleActionAdd(aclId, ruleId,
                                                          BROAD_ACTION_REDIRECT,
                                                          redirUsp.unit,
                                                          redirUsp.slot,
                                                          redirUsp.port);

                    if (L7_LAG_SLOT_NUM == redirUsp.slot)
                    {
                        /* This policy now depends upon the LAG as it's destination. */
                        if (hapiBroadQosAddDepends(aclId, &redirUsp, dapi_g) != L7_SUCCESS)
                            result = L7_FAILURE;
                    }
                }
                break;

            case L7_QOS_ACL_TLV_ATTR_MIRROR_TYPE:
                if (0 == denyFlag)    /* deny action takes precedence */
                {
                    DAPI_USP_t mirrorUsp;

                    mirrorUsp.unit = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,0));
                    mirrorUsp.slot = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,4));
                    mirrorUsp.port = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pMatchTLV,8));

                    result = hapiBroadPolicyRuleActionAdd(aclId, ruleId,
                                                          BROAD_ACTION_MIRROR,
                                                          mirrorUsp.unit,
                                                          mirrorUsp.slot,
                                                          mirrorUsp.port);

                    if (L7_LAG_SLOT_NUM == mirrorUsp.slot)
                    {
                        /* This policy now depends upon the LAG as it's destination. */
                        if (hapiBroadQosAddDepends(aclId, &mirrorUsp, dapi_g) != L7_SUCCESS)
                            result = L7_FAILURE;
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
                hapiBroadQosAclCorrelatorDelete(aclId);
                (void)hapiBroadPolicyDelete(aclId);
                return result;
            }

            classRuleLen -= (osapiNtohs(pMatchTLV->length) + sizeof(pMatchTLV->type) + sizeof(pMatchTLV->length));
            /* get next match criteria */
            pMatchTLV = GET_NEXT_TLV(pMatchTLV);
        }

        if (denyFlag)
        {
            result = hapiBroadPolicyRuleActionAdd(aclId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
            if (L7_SUCCESS == result)
            {
                if (L7_QOS_ACL_TLV_RULE_CORR_NULL != correlator)
                {
                    result = hapiBroadPolicyRuleCounterAdd(aclId, ruleId, BROAD_COUNT_PACKETS, hapiPortPtr->bcm_port);
                    if (L7_SUCCESS == result)
                    {
                        /* save a non-null correlator so that it may be used later when the
                         * counter is requested by the application
                         *
                         * NOTE:  There is no guarantee that the counter will be successfully
                         *        committed for the policy, but need to store the correlator 
                         *        while it is still available from the TLV.
                         */
                        result = hapiBroadQosAclCorrelatorAdd(aclId, ruleId, correlator, L7_TRUE);
                    }
                }
            }
        }
        else
        {
            result = hapiBroadPolicyRuleActionAdd(aclId, ruleId, BROAD_ACTION_PERMIT, 0, 0, 0);
        }

        /* Support for multiple ETHTYPEs requires copying the existing rule. */
        if ((L7_SUCCESS == result) && (ethType2))
        {
            BROAD_POLICY_RULE_t newRule;

            result = hapiBroadPolicyRuleCopy(aclId, ruleId, &newRule);
            if (L7_SUCCESS == result)
            {
                /* update new rule with ETHTYPE2 value */
                result = hapiBroadPolicyRuleQualifierAdd(aclId, newRule,
                                                         BROAD_FIELD_ETHTYPE,
                                                         (L7_uchar8*)&ethType2,
                                                         defaultMask);
            }
        }

        if (L7_SUCCESS != result)
        {
            hapiBroadQosAclCorrelatorDelete(aclId);
            (void)hapiBroadPolicyDelete(aclId);
            return result;
        }

        classDefLen -= (osapiNtohs(pRuleTLV->length) + sizeof(pRuleTLV->type) + sizeof(pRuleTLV->length));
        /* get next rule */
        pRuleTLV = GET_NEXT_TLV(pRuleTLV);
    }

    result = hapiBroadPolicyCommit(aclId);    
    if (L7_SUCCESS != result)
    {
        hapiBroadQosAclCorrelatorDelete(aclId);
        (void)hapiBroadPolicyDelete(aclId);
        return result;
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
  L7_RC_t                   result = L7_SUCCESS;
  DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
  L7_uint32                 instanceId;
  L7_tlv_t                 *pTLV;
  DAPI_PORT_t              *dapiPortPtr;
  BROAD_PORT_t             *hapiPortPtr;
  HAPI_BROAD_QOS_PORT_t    *qosPort;
  BROAD_POLICY_t            policy;
  HAPI_QOS_INTF_DIR_t       direction;
  int                       idx;

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

  if (osapiNtohs(pTLV->type) != L7_QOS_ACL_TLV_LIST_TYPE)
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

  if ((L7_TRUE == IS_PORT_TYPE_CPU( dapiPortPtr )) ||
      (L7_FALSE == qosPort->aclds.policyInit[direction]))
  {
    instanceId = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pTLV,0));

    if (hapiBroadQosAclPolicyTLVFind(pTLV, direction, &idx) == L7_TRUE)
    {
      /* reuse existing policy */
      policy = broadAclTable[idx].policyId;

      /* clear any counters in the ACL correlator table, since the
       * HW counts will be reset when the policy is applied
       */
      hapiBroadQosAclCorrelatorCountClear(policy);

      result = hapiBroadQosApplyPolicy(policy, usp, dapi_g);
      if (L7_SUCCESS == result)
      {
        broadAclTable[idx].ifCount++;   /* update ACL table */

        qosPort->aclds.policyId[direction]   = policy;   /* update port QoS table */
        qosPort->aclds.policyInit[direction] = L7_TRUE;
      }
      else
      {
        LOG_MSG("ACL failed to apply on port %d\n", hapiPortPtr->bcmx_lport);
      }
    }
    else
    {
      /* create a new policy */
      result = hapiBroadQosAclInstAdd(usp, instanceId, pTLV, dapi_g, &policy, direction);
      if (L7_SUCCESS == result)
      {
        /* save the policy info */
        result = hapiBroadQosAclPolicyTLVAdd(pTLV, policy, direction, instanceId);
        if (L7_SUCCESS != result)
        {
          L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID,
            "ACL internal table overflow."
            " Attempting to add an ACL to a full table.");
        }

        if (L7_SUCCESS == result)
        {
          result = hapiBroadQosApplyPolicy(policy, usp, dapi_g);
        }

        if (L7_SUCCESS == result)
        {
          qosPort->aclds.policyId[direction]   = policy;
          qosPort->aclds.policyInit[direction] = L7_TRUE;
        }
        else
        {
          LOG_MSG("ACL not applied to port %d\n", hapiPortPtr->bcmx_lport);

          if (hapiBroadQosAclPolicyTLVFind(pTLV, direction, &idx) == L7_TRUE)
          {
            broadAclTable[idx].ifCount  = 0;
            broadAclTable[idx].used     = L7_FALSE;        /* update ACL table */
            broadAclTable[idx].policyId = BROAD_POLICY_INVALID;
          }

          /* Delete all correlators for this ACL */
          hapiBroadQosAclCorrelatorDelete(policy);

          /* Delete policy and remove any dependencies it may have on other ports. */
          (void)hapiBroadPolicyDelete(policy);
          (void)hapiBroadQosDelDependsAll(policy, dapi_g);

        }
      }
    }
  }
  else
  {
    /* policy already on this port */
    result = L7_FAILURE;
  }


  hapiBroadQosSemGive(dapi_g);
  return result;
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
  L7_RC_t                   result = L7_SUCCESS;
  DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
  L7_uint32                 instanceId;
  L7_uint32                 numInstanceIds;
  L7_tlv_t                 *pTLV;
  DAPI_PORT_t              *dapiPortPtr;
  BROAD_PORT_t             *hapiPortPtr;
  HAPI_BROAD_QOS_PORT_t    *qosPort;
  int                       idx;
  BROAD_POLICY_t            policyId;
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

  if (osapiNtohs(pTLV->type) != L7_QOS_ACL_TLV_INST_DEL_LIST_TYPE)
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

  if (( L7_FALSE == IS_PORT_TYPE_CPU( dapiPortPtr )) &&
      ( L7_FALSE == qosPort->aclds.policyInit[direction] ))
  {
    /* no policy on this port */
    hapiBroadQosSemGive(dapi_g);
    return L7_ERROR;
  }

  if ( L7_TRUE == IS_PORT_TYPE_CPU( dapiPortPtr ) )
  {
    if ( hapiBroadQosAclPolicyIdFindInstance( instanceId, direction, &idx ) != L7_TRUE )
    {
      hapiBroadQosSemGive( dapi_g );
      LOG_MSG( "ACL instance 0x%x not found in internal table\n", instanceId );
      return( L7_FAILURE );
    }
    policyId = broadAclTable[idx].policyId;
  }
  else
  {
    policyId = qosPort->aclds.policyId[direction];
    /* Check if ACL policy is still being used on any interface. */
    if ( hapiBroadQosAclPolicyIdFind( policyId, &idx ) != L7_TRUE )
    {
      hapiBroadQosSemGive(dapi_g);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "ACL %d not found in internal table."
            " Attempting to delete a non-existent ACL.", policyId);
      return L7_FAILURE;
    }
  }
  /* Remove the policy from this interface. */
  if (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_FALSE)
  {
    result = hapiBroadQosRemovePolicy( policyId, usp, dapi_g );
  }

  if (L7_SUCCESS != result)
  {
    /* continue to clean-up even if an error occurs */
    LOG_MSG("ACL not removed from port %d\n", hapiPortPtr->bcmx_lport);
  }

  if (--broadAclTable[idx].ifCount <= 0)
  {
    /* Delete all correlators for this ACL */
    hapiBroadQosAclCorrelatorDelete(policyId);

    /* Delete policy and remove any dependencies it may have on other ports. */
    if (hapiBroadPolicyDelete(policyId) != L7_SUCCESS)
      result = L7_FAILURE;

    if (hapiBroadQosDelDependsAll(policyId, dapi_g) != L7_SUCCESS)
      result = L7_FAILURE;

    broadAclTable[idx].used     = L7_FALSE;        /* update ACL table */
    broadAclTable[idx].policyId = BROAD_POLICY_INVALID;
  }

  qosPort->aclds.policyInit[direction] = L7_FALSE;          /* update per-port QoS data */
  qosPort->aclds.policyId[direction]   = BROAD_POLICY_INVALID;

  hapiBroadQosSemGive(dapi_g);
  return result;
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
    BROAD_POLICY_STATS_t      stats[L7_ACL_MAX_RULES_PER_MULTILIST];
    L7_uint32                 numStats;
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
        numStats = L7_ACL_MAX_RULES_PER_MULTILIST;

        hapiBroadPolicyStatsGet(aclId, &numStats, stats);

        if (L7_FALSE == stats[ruleId].meter)
        {
            /* save latest hit count and calculate delta from the previous count
             *
             * NOTE:  Always subtract prev count from latest.  The unsigned delta 
             *        value will be correct even if the HW counter wrapped.
             */
            prev_val = broadAclCorrTable[idx].lastCount;
            broadAclCorrTable[idx].lastCount = stats[ruleId].statMode.counter.count;

            delta_val += (broadAclCorrTable[idx].lastCount - prev_val);
            result = L7_SUCCESS;

        } /* endif HW counter exists */

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
            sysapiPrintf("policy %2d, hash %08x, count %d, direction %s\n",
                         broadAclTable[i].policyId,
                         broadAclTable[i].hashVal,
                         broadAclTable[i].ifCount,
                         broadAclTable[i].direction == HAPI_QOS_INTF_DIR_IN ? "IN":"OUT");
        }
    }
}

void hapiBroadQosAclDebugCorrelatorTable()
{
    int         i;
    L7_uint32   h_val, l_val;

    sysapiPrintf("ACL Correlator Table\n");
    for (i = 0; i < L7_ACL_LOG_RULE_LIMIT; i++)
    {
        if (L7_QOS_ACL_TLV_RULE_CORR_NULL != broadAclCorrTable[i].correlator)
        {
            h_val = (L7_uint32)(broadAclCorrTable[i].lastCount >> 32) & 0xffffffff;
            l_val = (L7_uint32)broadAclCorrTable[i].lastCount & 0xffffffff;

            sysapiPrintf("[%3d] correlator=0x%8.8x, aclId=%3d, ruleId=%3d, "
                         "addedToPolicy=%d lastCount=0x%8.8x-%8.8x\n",
                         i,
                         broadAclCorrTable[i].correlator, 
                         broadAclCorrTable[i].aclId, 
                         broadAclCorrTable[i].ruleId, 
                         broadAclCorrTable[i].addedToPolicy,
                         h_val, l_val);
        }
    }
    sysapiPrintf("\nA total of %d entries scanned (empty entries not displayed)\n",
                 i);
}
