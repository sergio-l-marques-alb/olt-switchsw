/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_diffserv.c
*
* Purpose: This file contains all the routines for DiffServ
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/

#include "broad_diffserv.h"
#include "l7_diffserv_api.h"
#include "osapi_support.h"

/* This flag is used to enable summing stats for all rules that make-up
 * a policy instance. It is currently undefined as it causes copied
 * rules to be double-counted.
 */
#undef BROAD_ACCUMULATE_RULE_STATS

/*********************************************************************
*
* @purpose Initialize DiffServ component of QOS package
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
L7_RC_t hapiBroadQosDiffServInit(DAPI_t *dapi_g)
{
    L7_RC_t                result = L7_SUCCESS;

    /* init software required for DiffServ */

    return result;

}

/*********************************************************************
*
* @function hapiBroadQosDiffservPortInit
*
* @purpose  Per-port Diffserv init
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
L7_RC_t hapiBroadQosDiffservPortInit(DAPI_PORT_t *dapiPortPtr)
{
    L7_RC_t result = L7_SUCCESS;

    dapiPortPtr->cmdTable[DAPI_CMD_QOS_DIFFSERV_INST_ADD]           = (HAPICTLFUNCPTR_t)hapiBroadQosDiffServInstAdd;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_DIFFSERV_INST_DELETE]        = (HAPICTLFUNCPTR_t)hapiBroadQosDiffServInstDelete;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET]   = (HAPICTLFUNCPTR_t)hapiBroadQosDiffServIntfStatInGet;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET]  = (HAPICTLFUNCPTR_t)hapiBroadQosDiffServIntfStatOutGet;

    return result;
}

/*********************************************************************
*
* @purpose Add color-aware entries to policy instance
*
* @notes   This function has only been tested for simple-policing.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosDiffServColorAwarePolicing(HAPI_QOS_INTF_DIR_t direction,
                                                      BROAD_POLICY_RULE_t confRule,
                                                      L7_uint32           policingStyle,
                                                      L7_uint32           colorType,
                                                      L7_uint32           colorValue,
                                                      L7_ushort16         ethType)
{
    L7_RC_t             result = L7_SUCCESS;
    BROAD_POLICY_RULE_t nonConfRule;
    L7_uchar8           value;
    L7_uchar8           defaultMask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE};

    /* Specified rule contains all match criteria, except the conforming type.
     * Agument this rule to match the conforming type and start the packet
     * marking at Green, based upon the meter. Other packets matching this rule
     * will start with non-conforming actions and drop precedence of Yellow,
     * based upon the meter.
     *
     * NOTES:
     * 1) Rule copy has the desired side effect of sharing the meter.
     * 2) The non-conforming rule should have only the qualification criteria
     *    of the conforming rule, not the actions.
     */
    result = hapiBroadPolicyRuleCopy(confRule, &nonConfRule);

    if (L7_SUCCESS == result)
    {
        /* clear all actions associated with non-conf rule */
        result = hapiBroadPolicyRuleActionClearAll(nonConfRule);
    }

    if (L7_SUCCESS == result)
    {
        /* Augment conforming rule with definition of conforming (Green) packets. */
        switch (colorType)
        {
        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS:
            value  = (L7_uchar8)colorValue;
            result = hapiBroadPolicyRuleQualifierAdd(confRule, BROAD_FIELD_OCOS, &value, defaultMask);
            break;
        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPDSCP:
            value  = (L7_uchar8)colorValue << 2;
            defaultMask[0] <<= 2; /* Mask out the lowest two bits */
            defaultMask[1] = FIELD_MASK_ALL;
            if (ethType == 0x86dd)
            {
              result = hapiBroadPolicyRuleQualifierAdd(confRule, BROAD_FIELD_IP6_TRAFFIC_CLASS, &value, defaultMask);
            }
            else
            {
              result = hapiBroadPolicyRuleQualifierAdd(confRule, BROAD_FIELD_DSCP, &value, defaultMask);
            }
            break;
        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPPREC:
            value  = (L7_uchar8)colorValue << 5;
            defaultMask[0] <<= 5; /* Mask out the lowest 5 bits */
            defaultMask[1] = FIELD_MASK_ALL;
            if (ethType == 0x86dd)
            {
              result = hapiBroadPolicyRuleQualifierAdd(confRule, BROAD_FIELD_IP6_TRAFFIC_CLASS, &value, defaultMask);
            }
            else
            {
              result = hapiBroadPolicyRuleQualifierAdd(confRule, BROAD_FIELD_DSCP, &value, defaultMask);
            }
            break;
        default:
            result = L7_ERROR;
            break;
        }
    }

    /* Update Drop Precedence for conforming rule. */
    if ((L7_SUCCESS == result) && (direction == HAPI_QOS_INTF_DIR_IN))
    {
        /* Set drop precedence to Green/Yellow/Red. */
        result = hapiBroadPolicyRuleActionAdd(confRule,
                                              BROAD_ACTION_SET_DROPPREC,
                                              BROAD_COLOR_GREEN,
                                              BROAD_COLOR_YELLOW,
                                              BROAD_COLOR_RED);
    }

    /* Update Drop Precedence and Actions for non-conforming rule. */
    if (L7_SUCCESS == result)
    {
        BROAD_POLICY_ACTION_t nonConfAction;
        L7_uint32             nonConfValue0, nonConfValue1, nonConfValue2;

        /* Set drop precedence rule to Yellow/Yellow/Red. */
        if ((L7_SUCCESS == result) && (direction == HAPI_QOS_INTF_DIR_IN))
        {
            result = hapiBroadPolicyRuleActionAdd(nonConfRule,
                                                  BROAD_ACTION_SET_DROPPREC,
                                                  BROAD_COLOR_YELLOW,
                                                  BROAD_COLOR_YELLOW,
                                                  BROAD_COLOR_RED);
        }

        /* Set conforming action and non-conforming actions to be the same. */
        if (L7_SUCCESS == result)
        {
          for (nonConfAction = 0; nonConfAction < BROAD_ACTION_LAST; nonConfAction++)
          {  
            if (hapiBroadPolicyRuleNonConfActionGet(confRule,
                                                    nonConfAction,
                                                    &nonConfValue0,
                                                    &nonConfValue1,
                                                    &nonConfValue2) == L7_SUCCESS)
            {
              result = hapiBroadPolicyRuleActionAdd(nonConfRule,
                                                    nonConfAction,
                                                    nonConfValue0,
                                                    nonConfValue1,
                                                    nonConfValue2);

              if (L7_SUCCESS != result)
              {
                break;
              }

              result = hapiBroadPolicyRuleNonConfActionAdd(nonConfRule,
                                                           nonConfAction,
                                                           nonConfValue0,
                                                           nonConfValue1,
                                                           nonConfValue2);
              if (L7_SUCCESS != result)
              {
                break;
              }
            }
          }
        }
    }

    return result;
}

/*********************************************************************
*
* @purpose Add a policy-class instance to an interface
*
* @param   DAPI_USP_t               *usp
* @param   L7_tlv_t                 *pClassDefTLV
* @param   L7_tlv_t                 *pPolicyDefTLV
* @param   DAPI_t                   *dapi_g
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosDiffServClassAdd(DAPI_USP_t         *usp,
                                            HAPI_QOS_INTF_DIR_t direction,
                                            L7_BOOL             keepStats,
                                            L7_uint32           instanceId,
                                            L7_uint32           instanceIndex,
                                            L7_tlv_t           *pClassDefTLV,
                                            L7_tlv_t           *pPolicyDefTLV,
                                            L7_uint32           groupId,
                                            DAPI_USP_t         *dependentLag,
                                            L7_uint32          *dependentLagCount,
                                            DAPI_t             *dapi_g)
{
    L7_RC_t                  result = L7_SUCCESS;
    L7_tlv_t                *pRuleTLV;
    L7_tlv_t                *pMatchTLV;
    L7_tlv_t                *pPolicyTLV;
    L7_tlv_t                *pPoliceActionTLV;
    L7_int32                 classDefLen, classRuleLen, policyLen;
    L7_uint32                policeLen;
    BROAD_PORT_t            *hapiPortPtr;
    HAPI_BROAD_QOS_PORT_t   *qosPort;
    BROAD_POLICY_RULE_t      ruleId;
    L7_ushort16              ethType2 = 0;
    L7_ushort16              ethType;
    L7_uchar8                defaultMask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                              FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
    L7_BOOL                  policing = L7_FALSE;
    L7_BOOL                  dropAction = L7_FALSE;
    L7_uint32                data32, mask32;
    L7_ushort16              data16, mask16;
    L7_uint32                i;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    qosPort     = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

    classDefLen = osapiNtohl(pClassDefTLV->length);

    /* point to first rule */
    pRuleTLV = GET_SUB_TLV(pClassDefTLV,0);

    /* start a new rule */
    result = hapiBroadPolicyRuleAdd(&ruleId);
    if (L7_SUCCESS != result)
    {
        return result;
    }

    if (keepStats == L7_TRUE)
    {
      /* associate instance with first rule, assume rules are consecutive */
      qosPort->aclds.ruleList[direction][instanceIndex].inUse = L7_TRUE;
      qosPort->aclds.ruleList[direction][instanceIndex].rule = ruleId;
      qosPort->aclds.ruleList[direction][instanceIndex].instance = instanceId;
    }

    /* iterate through class rules */
    while (classDefLen > 0)
    {
        L7_BOOL     matchEvery;
        L7_BOOL     tosSpecified;
        L7_uchar8   tosValue, tosMask;
        L7_BOOL     protoSpecified;
        L7_uchar8   protoValue, protoMask;

        if (osapiNtohl(pRuleTLV->type) != L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_TYPE)
        {
            return L7_FAILURE;
        }

        /* deny flag is only used for 'match not' class types, which we don't support */
        classRuleLen = osapiNtohl(pRuleTLV->length) - 4;    /* subtract 4 to account for denyFlag */
        pMatchTLV    = GET_SUB_TLV(pRuleTLV,4); /* point to first match criterion */

        ethType = 0;

        /* iterate through match criteria for this rule */
        matchEvery     = L7_FALSE;
        tosSpecified   = L7_FALSE;
        protoSpecified = L7_FALSE;
        while ((classRuleLen > 0) && (matchEvery == L7_FALSE))
        {
            switch (osapiNtohl(pMatchTLV->type))
            {
            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIP_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_DIP,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_TYPE:
                /* TODO: Add L4 port range support */
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_DPORT,
                                                         (L7_uchar8 *)&data16,
                                                         (L7_uchar8 *)&mask16);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTMAC_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_MACDA,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,6));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_TYPE:
                tosSpecified = L7_TRUE;
                tosValue     = *(GET_VALUE_PTR(pMatchTLV,0));
                tosMask      = *(GET_VALUE_PTR(pMatchTLV,1));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_TYPE:
                protoSpecified = L7_TRUE;
                protoValue     = *(GET_VALUE_PTR(pMatchTLV,0));
                protoMask      = *(GET_VALUE_PTR(pMatchTLV,1));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIP_TYPE:
                data32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask32 = osapiNtohl(*((L7_uint32 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_SIP,
                                                         (L7_uchar8 *)&data32,
                                                         (L7_uchar8 *)&mask32);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_TYPE:
                /* TODO: Add L4 port range support */
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                mask16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,4)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_SPORT,
                                                         (L7_uchar8 *)&data16,
                                                         (L7_uchar8 *)&mask16);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCMAC_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_MACSA,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,6));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_OCOS,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         defaultMask);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID_TYPE:
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_OVID,
                                                         (L7_uchar8 *)&data16,
                                                         defaultMask);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS2_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_ICOS,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         defaultMask);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID2_TYPE:
                data16 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IVID,
                                                         (L7_uchar8 *)&data16,
                                                         defaultMask);
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_TYPE:
                ethType = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,0)));
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_ETHTYPE,
                                                         (L7_uchar8 *)&ethType,
                                                         defaultMask);
                ethType2 = osapiNtohs(*((L7_ushort16 *)GET_VALUE_PTR(pMatchTLV,2)));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIPV6_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IP6_DST,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,16));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIPV6_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IP6_SRC,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,16));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPV6_FLOWLBL_TYPE:
                result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                         BROAD_FIELD_IP6_FLOWLABEL,
                                                         GET_VALUE_PTR(pMatchTLV,0),
                                                         GET_VALUE_PTR(pMatchTLV,4));
                break;

            case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_TYPE:
                /* once we hit a match every, the rest are don't cares */
                matchEvery = L7_TRUE;
                break;

            default:
                /* unsupported match criteria */
                result = L7_ERROR;
                break;
            }

            /* check for errors */
            if (result != L7_SUCCESS)
            {
                return result;
            }

            classRuleLen -= (osapiNtohl(pMatchTLV->length) + sizeof(pMatchTLV->type) + sizeof(pMatchTLV->length));
            /* get next match criteria */
            pMatchTLV = GET_NEXT_TLV(pMatchTLV);
        }

        if (tosSpecified == L7_TRUE)
        {
          if (ethType == 0x0800)
          {
            result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                     BROAD_FIELD_DSCP,
                                                     &tosValue,
                                                     &tosMask);
          }
          else if (ethType == 0x86dd)
          {
            result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                     BROAD_FIELD_IP6_TRAFFIC_CLASS,
                                                     &tosValue,
                                                     &tosMask);
          }
          else
          {
            return L7_FAILURE;
          }
        }

        if (protoSpecified == L7_TRUE)
        {
          if (ethType == 0x0800)
          {
            result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                     BROAD_FIELD_PROTO,
                                                     &protoValue,
                                                     &protoMask);
          }
          else if (ethType == 0x86dd)
          {
            result = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                                     BROAD_FIELD_IP6_NEXTHEADER,
                                                     &protoValue,
                                                     &protoMask);
          }
          else
          {
            return L7_FAILURE;
          }
        }

        classDefLen -= (osapiNtohl(pRuleTLV->length) + sizeof(pRuleTLV->type) + sizeof(pRuleTLV->length));
        /* get next rule */
        pRuleTLV = GET_NEXT_TLV(pRuleTLV);
    }

    /* iterate through policies */
    policyLen = osapiNtohl(pPolicyDefTLV->length);

    /* point to policy attribute */
    pPolicyTLV = GET_SUB_TLV(pPolicyDefTLV,0);

    while (policyLen > 0)
    {
        L7_uchar8           policingStyle     = 0;
        L7_uchar8           colorAwareMode    = 0;
        L7_uchar8           colorConformType  = 0;
        L7_uchar8           colorConformValue = 0;
        BROAD_METER_ENTRY_t meterDef = {0, 0, 0, 0, BROAD_METER_COLOR_BLIND};

        memset(&meterDef, 0, sizeof(meterDef));

        switch (osapiNtohl(pPolicyTLV->type))
        {
        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BW_ALLOC_TYPE:
            /* default is "no action" */
            result = L7_SUCCESS;
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_ASSIGN_QUEUE_TYPE:
            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                  BROAD_ACTION_SET_COSQ,
                                                  *GET_VALUE_PTR(pPolicyTLV,0),
                                                  0,
                                                  0);
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPDSCPVAL_TYPE:
            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                  BROAD_ACTION_SET_DSCP,
                                                  *GET_VALUE_PTR(pPolicyTLV,0),
                                                  0,
                                                  0);
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_TYPE:
            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                  BROAD_ACTION_SET_TOS,
                                                  *GET_VALUE_PTR(pPolicyTLV,0),
                                                  0,
                                                  0);
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_TYPE:
            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                  BROAD_ACTION_SET_USERPRIO,
                                                  *GET_VALUE_PTR(pPolicyTLV,0),
                                                  0,
                                                  0);
           break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_AS_COS2_TYPE:
            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                  BROAD_ACTION_SET_USERPRIO_AS_COS2,
                                                  0,
                                                  0,
                                                  0);
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_DROP_TYPE:
            dropAction = L7_TRUE;
            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                  BROAD_ACTION_HARD_DROP,
                                                  0,
                                                  0,
                                                  0);
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_REDIRECT_TYPE:
            {
                DAPI_USP_t redirUsp;

                redirUsp.unit = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pPolicyTLV,0));
                redirUsp.slot = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pPolicyTLV,4));
                redirUsp.port = osapiNtohl(*(L7_int32*)GET_VALUE_PTR(pPolicyTLV,8));

                result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                      BROAD_ACTION_REDIRECT,
                                                      redirUsp.unit,
                                                      redirUsp.slot,
                                                      redirUsp.port);

                if (L7_LAG_SLOT_NUM == redirUsp.slot)
                {
                  /* This policy now depends upon the LAG as it's destination. */
                  /* Make sure we don't add the same LAG USP twice. */
                  for (i = 0; i < (*dependentLagCount); i++)
                  {
                    if (dependentLag[(*dependentLagCount)].port == redirUsp.port)
                      break;
                  }
                  if ((i == (*dependentLagCount)) && ((*dependentLagCount) < L7_MAX_NUM_LAG_INTF))
                  {
                    dependentLag[(*dependentLagCount)] = redirUsp;
                    (*dependentLagCount)++;
                  }
                }
            }
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MIRROR_TYPE:
            {   /* Mirror Policy */
                DAPI_USP_t    mirrorUsp;

                mirrorUsp.unit = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR( pPolicyTLV, 0 ));
                mirrorUsp.slot = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR( pPolicyTLV, 4 ));
                mirrorUsp.port = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR( pPolicyTLV, 8 ));

                result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                      BROAD_ACTION_MIRROR,
                                                      mirrorUsp.unit,
                                                      mirrorUsp.slot,
                                                      mirrorUsp.port);

                if (L7_LAG_SLOT_NUM == mirrorUsp.slot)
                {
                  /* This policy now depends upon the LAG as it's destination. */
                  /* Make sure we don't add the same LAG USP twice. */
                  for (i = 0; i < (*dependentLagCount); i++)
                  {
                    if (dependentLag[(*dependentLagCount)].port == mirrorUsp.port)
                      break;
                  }
                  if ((i == (*dependentLagCount)) && ((*dependentLagCount) < L7_MAX_NUM_LAG_INTF))
                  {
                    dependentLag[(*dependentLagCount)] = mirrorUsp;
                    (*dependentLagCount)++;
                  }
                }
            }
            break;

        case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE:
            /* indicate this is a policing policy */
            policing = L7_TRUE;

            /* iterate through policing actions */
            policingStyle    = *GET_VALUE_PTR(pPolicyTLV,0);
            colorAwareMode   = *GET_VALUE_PTR(pPolicyTLV,1);
            colorConformType = *GET_VALUE_PTR(pPolicyTLV,2);
            colorConformValue= *GET_VALUE_PTR(pPolicyTLV,3);

            policeLen = osapiNtohl(pPolicyTLV->length) - 8; /* subtract 8 to account for style/color */

            /* point to sub TLV */
            pPoliceActionTLV = GET_SUB_TLV(pPolicyTLV,8);

            /* Use multiple classifier entries to implement flexible color-aware mode,
             * instead of based upon DSCP only.
             */
            meterDef.colorMode = BROAD_METER_COLOR_BLIND;

            while (policeLen > 0)
            {
                L7_uchar8 action;
                L7_uchar8 markValue;

                switch (osapiNtohl(pPoliceActionTLV->type))
                {
                case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_CONFORM_TYPE:
                    /* used for all policing types: simple, single and two-rate  */
                    meterDef.cir = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pPoliceActionTLV,0));
                    meterDef.cbs = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pPoliceActionTLV,4));

                    action    = *GET_VALUE_PTR(pPoliceActionTLV,8);
                    markValue = *GET_VALUE_PTR(pPoliceActionTLV,9);

                    switch (action)
                    {
                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_DROP:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_HARD_DROP,
                                                                  0,
                                                                  0,
                                                                  0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_SEND:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_PERMIT,
                                                                  0,
                                                                  0,
                                                                  0);
                        }
                        break;
                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKDSCP:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_SET_DSCP,
                                                                  markValue,
                                                                  0,
                                                                  0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_PERMIT,
                                                                  0,
                                                                  0,
                                                                  0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKPREC:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_SET_TOS,
                                                                  markValue,
                                                                  0,
                                                                  0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_PERMIT,
                                                                  0,
                                                                  0,
                                                                  0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_SET_USERPRIO,
                                                                  markValue,
                                                                  0,
                                                                  0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_PERMIT,
                                                                  0,
                                                                  0,
                                                                  0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS_AS_COS2:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_SET_USERPRIO_AS_COS2,
                                                                  0,
                                                                  0,
                                                                  0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleActionAdd(ruleId,
                                                                  BROAD_ACTION_PERMIT,
                                                                  0,
                                                                  0,
                                                                  0);
                        }
                        break;

                    default:
                        result = L7_FAILURE;
                        break;
                    }
                    break;

                case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_EXCEED_TYPE:
                    /* Used for both single rate (pir = cir, pbs > cbs) and
                     * two-rate policing (pir > cir, pbs > cbs).
                     */
                    meterDef.pir = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pPoliceActionTLV,0));
                    meterDef.pbs = osapiNtohl(*(L7_uint32 *)GET_VALUE_PTR(pPoliceActionTLV,4));

                    action    = *GET_VALUE_PTR(pPoliceActionTLV,8);
                    markValue = *GET_VALUE_PTR(pPoliceActionTLV,9);

                    switch (action)
                    {
                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_DROP:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_HARD_DROP,
                                                                        0,
                                                                        0,
                                                                        0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_SEND:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_PERMIT,
                                                                        0,
                                                                        0,
                                                                        0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKDSCP:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_SET_DSCP,
                                                                        markValue,
                                                                        0,
                                                                        0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_PERMIT,
                                                                        0,
                                                                        0,
                                                                        0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKPREC:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_SET_TOS,
                                                                        markValue,
                                                                        0,
                                                                        0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_PERMIT,
                                                                        0,
                                                                        0,
                                                                        0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_SET_USERPRIO,
                                                                        markValue,
                                                                        0,
                                                                        0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_PERMIT,
                                                                        0,
                                                                        0,
                                                                        0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS_AS_COS2:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_SET_USERPRIO_AS_COS2,
                                                                        0,
                                                                        0,
                                                                        0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleExceedActionAdd(ruleId,
                                                                        BROAD_ACTION_PERMIT,
                                                                        0,
                                                                        0,
                                                                        0);
                        }
                        break;



                    default:
                        result = L7_FAILURE;
                        break;
                    }
                    break;

                case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_NONCONFORM_TYPE:
                    action    = *GET_VALUE_PTR(pPoliceActionTLV,0);
                    markValue = *GET_VALUE_PTR(pPoliceActionTLV,1);

                    switch (action)
                    {
                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_DROP:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_HARD_DROP,
                                                                         0,
                                                                         0,
                                                                         0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_SEND:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_PERMIT,
                                                                         0,
                                                                         0,
                                                                         0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKDSCP:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_SET_DSCP,
                                                                         markValue,
                                                                         0,
                                                                         0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_PERMIT,
                                                                         0,
                                                                         0,
                                                                         0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKPREC:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_SET_TOS,
                                                                         markValue,
                                                                         0,
                                                                         0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_PERMIT,
                                                                         0,
                                                                         0,
                                                                         0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_SET_USERPRIO,
                                                                         markValue,
                                                                         0,
                                                                         0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_PERMIT,
                                                                         0,
                                                                         0,
                                                                         0);
                        }
                        break;

                    case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS_AS_COS2:
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_SET_USERPRIO_AS_COS2,
                                                                         0,
                                                                         0,
                                                                         0);
                        }
                        if (L7_SUCCESS == result)
                        {
                            result = hapiBroadPolicyRuleNonConfActionAdd(ruleId,
                                                                         BROAD_ACTION_PERMIT,
                                                                         0,
                                                                         0,
                                                                         0);
                        }
                        break;


                    default:
                        result = L7_FAILURE;
                        break;
                    }
                    break;

                default:
                    result = L7_FAILURE;
                    break;
                }

                /* check for errors */
                if (L7_SUCCESS != result)
                {
                    return result;
                }

                policeLen -= (osapiNtohl(pPoliceActionTLV->length) + sizeof(pPoliceActionTLV->type) + sizeof(pPoliceActionTLV->length));
                /* get next police action */
                pPoliceActionTLV = GET_NEXT_TLV(pPoliceActionTLV);
            }

            if (L7_SUCCESS == result)
            {
                result = hapiBroadPolicyRuleMeterAdd(ruleId, &meterDef);
            }
            break;

        default:
            /* unsupported policy */
            result = L7_FAILURE;
            break;
        }


        /* Handle case of color-aware policing */
        if ((L7_TRUE == policing) && (L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_AWARE == colorAwareMode))
        {
            result = hapiBroadQosDiffServColorAwarePolicing(direction,
                                                            ruleId,
                                                            policingStyle,
                                                            colorConformType,
                                                            colorConformValue,
                                                            ethType);
        }

        /* check for errors */
        if (L7_SUCCESS != result)
        {
            return result;
        }

        policyLen -= (osapiNtohl(pPolicyTLV->length) + sizeof(pPolicyTLV->type) + sizeof(pPolicyTLV->length));
        /* get next policy */
        pPolicyTLV = GET_NEXT_TLV(pPolicyTLV);
    }

    if (groupId != HAPI_DOT1X_GROUP_ID_INVALID)
    {
      L7_uchar8 tempVal;
      L7_uchar8 tempMask = 0xff;

      tempVal = groupId;

      /* Include the Group ID as a qualifier. */
      if (hapiBroadPolicyRuleQualifierAdd(ruleId,
                                          BROAD_FIELD_L2_CLASS_ID,
                                          &tempVal,
                                          &tempMask) != L7_SUCCESS)
      {
          return result;
      }
    }

    /* If we are not intending to drop this packet, then we need to explicitly permit it.
       This is essential on XGS, XGS2, and Tucana because if any rule on the port indicates 'drop', then
       it will take effect regardless of rule priority. */
    if ((dropAction == L7_FALSE) && (policing == L7_FALSE))
    {
      result = hapiBroadPolicyRuleActionAdd(ruleId,
                                            BROAD_ACTION_PERMIT,
                                            0,
                                            0,
                                            0);
    }

    /* If there were attributes, but not policing ones, create a counter */
    if ((osapiNtohl(pPolicyDefTLV->length)) && (L7_FALSE == policing))
    {
        /* add a counter for non-policing instances */
         result = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    }

    /* Support for multiple ETHTYPEs requires copying the existing rule(s), including
     * all actions and meters.
     */
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

    return result;
}

/*********************************************************************
*
* @functions hapiBroadDiffServPolicyCreate
*
* @purpose Parses a TLV and applies a DiffServ policy to a port.
*
* @param DAPI_USP_t *usp
* @param L7_tlv_t   *pTLV
* @param L7_uint32   groupId
* @param L7_uint32  *policyId
* @param DAPI_t     *dapi_g
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDiffServPolicyCreate(DAPI_USP_t          *usp,
                                      L7_tlv_t            *pTLV,
                                      HAPI_QOS_INTF_DIR_t  direction,
                                      L7_uint32            groupId,
                                      L7_BOOL              keepStats,
                                      L7_uint32           *policyId,
                                      DAPI_t              *dapi_g)
{
  L7_RC_t                   result = L7_SUCCESS;
  L7_uint32                 instanceCount, instanceIndex = 0;
  BROAD_POLICY_STAGE_t      stage;
  DAPI_USP_t                dependentLag[L7_MAX_NUM_LAG_INTF];
  L7_uint32                 dependentLagCount = 0;
  L7_uint32                 i;

  *policyId = BROAD_POLICY_INVALID;

  if (direction == DAPI_QOS_INTF_DIR_IN)
  {
    stage = BROAD_POLICY_STAGE_INGRESS;
  }
  else
  {
    stage = BROAD_POLICY_STAGE_EGRESS;
  }

  /* all instances come down at the same time for real-time efficiency */
  if (osapiNtohl(pTLV->type) != L7_QOS_DIFFSERV_TLV_POLICY_ADD_TYPE)
  {
      return L7_FAILURE;
  }

  instanceCount = *(L7_uint32 *)(GET_VALUE_PTR(pTLV,0));

  if (instanceCount <= 0)
  {
    return L7_FAILURE;
  }


  hapiBroadQosSemTake(dapi_g);

  do
  {
    if ((result = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT)) != L7_SUCCESS)
      break;

    if ((result = hapiBroadPolicyStageSet(stage)) != L7_SUCCESS)
      break;

  } while (0);

  if (L7_SUCCESS != result)
  {
    hapiBroadPolicyCreateCancel();
    hapiBroadQosSemGive(dapi_g);
    return result;
  }

  /* point to first classifier instance */
  pTLV = GET_SUB_TLV(pTLV, 4);

  while (instanceCount-- > 0)
  {
    L7_uint32  instanceId;
    L7_tlv_t  *pClassDefTLV;
    L7_tlv_t  *pPolicyDefTLV;

    if (osapiNtohl(pTLV->type) != L7_QOS_DIFFSERV_TLV_CLSF_INST_TYPE)
    {
      result = L7_FAILURE;
      break;
    }

    instanceId = *(L7_uint32*)GET_VALUE_PTR(pTLV,0);

    pClassDefTLV = GET_SUB_TLV(pTLV,sizeof(instanceId));  /* skip instance id */

    if (osapiNtohl(pClassDefTLV->type) != L7_QOS_DIFFSERV_TLV_CLASS_DEF_TYPE)
    {
      result = L7_FAILURE;
      break;
    }

    pPolicyDefTLV = GET_NEXT_TLV(pClassDefTLV);

    if (osapiNtohl(pPolicyDefTLV->type) != L7_QOS_DIFFSERV_TLV_POLICY_DEF_TYPE)
    {
      result = L7_FAILURE;
      break;
    }
    result = hapiBroadQosDiffServClassAdd(usp,
                                          direction,
                                          keepStats,
                                          instanceId,
                                          instanceIndex,
                                          pClassDefTLV,
                                          pPolicyDefTLV,
                                          groupId,
                                          dependentLag,
                                          &dependentLagCount,
                                          dapi_g);

    if (result != L7_SUCCESS)
    {
      break;
    }

    instanceIndex++;
    /* point to next classifier instance */
    pTLV = GET_NEXT_TLV(pTLV);
  }

  if (L7_SUCCESS == result)
  {
    result = hapiBroadPolicyCommit(policyId);

    if (L7_SUCCESS == result)
    {
      /* Update the LAG dependency lists */
      for (i = 0; i < dependentLagCount; i++)
      {
        if (hapiBroadQosAddDepends(*policyId, &dependentLag[i], dapi_g) != L7_SUCCESS)
        {
          (void)hapiBroadPolicyDelete(*policyId);
          *policyId = BROAD_POLICY_INVALID;
          break;
        }
      }
    }
  }
  else
  {
    hapiBroadPolicyCreateCancel();
  }

  hapiBroadQosSemGive(dapi_g);
  return result;
}

/*********************************************************************
*
* @functions hapiBroadQosDiffServInstAdd
*
* @purpose Adds a policy-class instance to an interface
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
/* DAPI_CMD_QOS_DIFFSERV_INST_ADD */
L7_RC_t hapiBroadQosDiffServInstAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                   result = L7_SUCCESS;
    DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
    L7_tlv_t                 *pTLV;
    BROAD_POLICY_t            diffId;
    DAPI_PORT_t              *dapiPortPtr;
    BROAD_PORT_t             *hapiPortPtr;
    HAPI_BROAD_QOS_PORT_t    *qosPort;
    HAPI_QOS_INTF_DIR_t       direction;

    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) &&
        (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE))
    {
        /* only allow physical ports and LAGs */
        return L7_FAILURE;
    }

    if (qosCmd->cmdData.diffServInstAdd.ifDirection == DAPI_QOS_INTF_DIR_IN)
    {
      direction = HAPI_QOS_INTF_DIR_IN;
    }
    else
    {
      direction = HAPI_QOS_INTF_DIR_OUT;
    }

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    qosPort     = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

    pTLV = qosCmd->cmdData.diffServInstAdd.pTLV;

    hapiBroadQosSemTake(dapi_g);

    if (BROAD_POLICY_INVALID != qosPort->aclds.policyId[direction])
    {
        /* policy already on this port */
        hapiBroadQosSemGive(dapi_g);
        return L7_FAILURE;
    }

    /* Check to see if there are any dot1x policies on this port. */
    if (direction == HAPI_QOS_INTF_DIR_IN)
    {
      if (hapiBroadDot1xPoliciesApplied(usp, dapi_g) == L7_TRUE)
      {
        hapiBroadQosSemGive(dapi_g);
        return L7_FAILURE;
      }
    }

    result = hapiBroadDiffServPolicyCreate(usp, pTLV, direction, HAPI_DOT1X_GROUP_ID_INVALID, L7_TRUE, &diffId, dapi_g);

    if (L7_SUCCESS == result)
    {
      result = hapiBroadQosApplyPolicy(diffId, usp, dapi_g);

      qosPort->aclds.policyId[direction]   = diffId;
    }

    if (L7_SUCCESS != result)
    {
      /* Delete policy and remove any dependencies it may have on other ports. */
      if (diffId != BROAD_POLICY_INVALID)
      {
        (void)hapiBroadPolicyDelete(diffId);
        (void)hapiBroadQosDelDependsAll(diffId, dapi_g);
      }

      qosPort->aclds.policyId[direction] = BROAD_POLICY_INVALID;
    }

    hapiBroadQosSemGive(dapi_g);
    return result;
}

/*********************************************************************
*
* @functions hapiBroadQosDiffServInstDelete
*
* @purpose Deletes an policy-class instance from an interface
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
/* DAPI_CMD_QOS_DIFFSERV_INST_DELETE */
L7_RC_t hapiBroadQosDiffServInstDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                   result = L7_SUCCESS;
    DAPI_QOS_CMD_t           *qosCmd = (DAPI_QOS_CMD_t *)data;
    L7_tlv_t                 *pTLV;
    DAPI_PORT_t              *dapiPortPtr;
    BROAD_PORT_t             *hapiPortPtr;
    HAPI_BROAD_QOS_PORT_t    *qosPort;
    int                       i;
    HAPI_QOS_INTF_DIR_t       direction;

    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE) &&
        (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) == L7_FALSE))
    {
        /* only allow physical ports and LAGs */
        return L7_FAILURE;
    }

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    qosPort     = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

    pTLV = qosCmd->cmdData.diffServInstDelete.pTLV;

    if (osapiNtohl(pTLV->type) != L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_TYPE)
    {
        return L7_FAILURE;
    }

    if (qosCmd->cmdData.diffServInstDelete.ifDirection == DAPI_QOS_INTF_DIR_IN)
    {
      direction = HAPI_QOS_INTF_DIR_IN;
    }
    else
    {
      direction = HAPI_QOS_INTF_DIR_OUT;
    }

    hapiBroadQosSemTake(dapi_g);

    if (BROAD_POLICY_INVALID == qosPort->aclds.policyId[direction])
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "No DiffServ policy applied on port %d\n", hapiPortPtr->bcmx_lport);
        hapiBroadQosSemGive(dapi_g);
        return L7_ERROR;
    }

    if (hapiBroadPolicyDelete(qosPort->aclds.policyId[direction]) != L7_SUCCESS)
        result = L7_FAILURE;

    if (hapiBroadQosDelDependsAll(qosPort->aclds.policyId[direction], dapi_g) != L7_SUCCESS)
        result = L7_FAILURE;

    qosPort->aclds.policyId[direction]   = BROAD_POLICY_INVALID;

    /* mark all rules as invalid */
    for (i = 0; i < L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    {
        qosPort->aclds.ruleList[direction][i].inUse    = L7_FALSE;
        qosPort->aclds.ruleList[direction][i].instance = BROAD_POLICY_RULE_INVALID;
        qosPort->aclds.ruleList[direction][i].rule     = BROAD_POLICY_RULE_INVALID;
    }

    hapiBroadQosSemGive(dapi_g);
    return result;
}

/*********************************************************************
*
* @functions hapiBroadQosDiffServIntfInboundStatAccum
*
* @purpose Accumulate stats for all rules that constitute a policy
*          instance.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosDiffServIntfInboundStatAccum(DAPI_QOS_DIFFSERV_INTF_STAT_IN_t type,
                                                        BROAD_POLICY_STATS_t            *stats,
                                                        L7_uint64                       *value)
{
  L7_RC_t result = L7_SUCCESS;

  switch (type)
  {
  case DAPI_QOS_DIFFSERV_STAT_IN_OFFERED_PACKETS:
    if (L7_TRUE == stats->meter)
    {
      /* offered = in_prof + out_prof */
      *value += stats->statMode.meter.in_prof;
      *value += stats->statMode.meter.out_prof;
    }
    else
    {
      *value += stats->statMode.counter.count;
    }
    break;
  case DAPI_QOS_DIFFSERV_STAT_IN_DISCARDED_PACKETS:
    if (L7_TRUE == stats->meter)
    {
      /* discarded really means out-of-profile */
      *value += stats->statMode.meter.out_prof;
    }
    else
    {
      *value += 0;    /* no-op */
    }
    break;
  default:
    result = L7_NOT_SUPPORTED;
    break;
  }

  return result;
}

/*********************************************************************
*
* @functions hapiBroadQosDiffServIntfOutboundStatAccum
*
* @purpose Accumulate stats for all rules that constitute a policy
*          instance.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosDiffServIntfOutboundStatAccum(DAPI_QOS_DIFFSERV_INTF_STAT_IN_t type,
                                                         BROAD_POLICY_STATS_t            *stats,
                                                         L7_uint64                       *value)
{
  L7_RC_t result = L7_SUCCESS;

  switch (type)
  {
  case DAPI_QOS_DIFFSERV_STAT_OUT_OFFERED_PACKETS:
    if (L7_TRUE == stats->meter)
    {
      /* offered = in_prof + out_prof */
      *value += stats->statMode.meter.in_prof;
      *value += stats->statMode.meter.out_prof;
    }
    else
    {
      *value += stats->statMode.counter.count;
    }
    break;
  case DAPI_QOS_DIFFSERV_STAT_OUT_DISCARDED_PACKETS:
    if (L7_TRUE == stats->meter)
    {
      /* discarded really means out-of-profile */
      *value += stats->statMode.meter.out_prof;
    }
    else
    {
      *value += 0;    /* no-op */
    }
    break;
  default:
    result = L7_NOT_SUPPORTED;
    break;
  }

  return result;
}

/*********************************************************************
*
* @functions hapiBroadQosDiffServIntfStatInGet
*
* @purpose Gets a DiffServ statistic from an inbound interface
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
/* DAPI_CMD_QOS_DIFFSERV_INTF_STAT_IN_GET */
L7_RC_t hapiBroadQosDiffServIntfStatInGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result = L7_SUCCESS;
  DAPI_QOS_CMD_t        *qosCmd= (DAPI_QOS_CMD_t *)data;
  BROAD_PORT_t          *hapiPortPtr;
  HAPI_BROAD_QOS_PORT_t *qosPortPtr;
  L7_ulong64            *value;
  L7_uint64              tmpVal = 0;
  L7_uint32              instance;
  BROAD_POLICY_t         policy;
  BROAD_POLICY_STATS_t   stats;
  HAPI_QOS_INTF_DIR_t    direction = HAPI_QOS_INTF_DIR_IN;
  BROAD_POLICY_RULE_t    ruleId = BROAD_POLICY_RULE_INVALID;
  L7_uint32              i;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  qosPortPtr  = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

  value    = qosCmd->cmdData.diffServIntfStatInGet.value;
  instance = qosCmd->cmdData.diffServIntfStatInGet.instanceKey;

  hapiBroadQosSemTake(dapi_g);

  policy = qosPortPtr->aclds.policyId[direction];
  if (BROAD_POLICY_INVALID == policy)
  {
    result = L7_NOT_EXIST;
  }
  else
  {
    /* map instance to rule(s) in order to query stats - assumes contiguous rules */
    for (i = 0; i < L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    {
      if ((qosPortPtr->aclds.ruleList[HAPI_QOS_INTF_DIR_IN][i].inUse == L7_TRUE) && 
          (qosPortPtr->aclds.ruleList[HAPI_QOS_INTF_DIR_IN][i].instance == instance))
      {
        ruleId = qosPortPtr->aclds.ruleList[HAPI_QOS_INTF_DIR_IN][i].rule;
        break;
      }
    }

    if (ruleId != BROAD_POLICY_RULE_INVALID)
    {
      result = hapiBroadPolicyStatsGet(policy, ruleId, &stats);

      if (L7_SUCCESS == result)
      {
        result = hapiBroadQosDiffServIntfInboundStatAccum(qosCmd->cmdData.diffServIntfStatInGet.statistic,
                                                          &stats,
                                                          &tmpVal);
        hapiBroadStatsConvert(value, tmpVal);
      }
    }
    else
    {
      result = L7_ERROR;
    }
  }

  hapiBroadQosSemGive(dapi_g);
  return result;
}

/*********************************************************************
*
* @functions hapiBroadQosDiffServIntfStatOutGet
*
* @purpose Gets a DiffServ statistic from an outbound interface
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
/* DAPI_CMD_QOS_DIFFSERV_INTF_STAT_OUT_GET */
L7_RC_t hapiBroadQosDiffServIntfStatOutGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t               result = L7_SUCCESS;
  DAPI_QOS_CMD_t        *qosCmd= (DAPI_QOS_CMD_t *)data;
  BROAD_PORT_t          *hapiPortPtr;
  HAPI_BROAD_QOS_PORT_t *qosPortPtr;
  L7_ulong64            *value;
  L7_uint64              tmpVal = 0;
  L7_uint32              instance;
  BROAD_POLICY_t         policy;
  BROAD_POLICY_STATS_t   stats;
  HAPI_QOS_INTF_DIR_t    direction = HAPI_QOS_INTF_DIR_OUT;
  BROAD_POLICY_RULE_t    rule = BROAD_POLICY_RULE_INVALID;
  L7_uint32              i;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  qosPortPtr  = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

  value    = qosCmd->cmdData.diffServIntfStatOutGet.value;
  instance = qosCmd->cmdData.diffServIntfStatOutGet.instanceKey;

  hapiBroadQosSemTake(dapi_g);

  policy = qosPortPtr->aclds.policyId[direction];
  if (BROAD_POLICY_INVALID == policy)
  {
    result = L7_NOT_EXIST;
  }
  else
  {
    /* map instance to rule(s) in order to query stats - assumes contiguous rules */
    for (i = 0; i < L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    {
      if ((qosPortPtr->aclds.ruleList[HAPI_QOS_INTF_DIR_OUT][i].inUse == L7_TRUE) && 
          (qosPortPtr->aclds.ruleList[HAPI_QOS_INTF_DIR_OUT][i].instance == instance))
      {
        rule = qosPortPtr->aclds.ruleList[HAPI_QOS_INTF_DIR_OUT][i].rule;
        break;
      }
    }

    if (rule != BROAD_POLICY_RULE_INVALID)
    {
      result = hapiBroadPolicyStatsGet(policy, rule, &stats);
      if (L7_SUCCESS == result)
      {
        result = hapiBroadQosDiffServIntfOutboundStatAccum(qosCmd->cmdData.diffServIntfStatOutGet.statistic,
                                                           &stats,
                                                           &tmpVal);
        hapiBroadStatsConvert(value, tmpVal);
      }
    }
    else
    {
      result = L7_ERROR;
    }
  }

  hapiBroadQosSemGive(dapi_g);
  return result;
}


