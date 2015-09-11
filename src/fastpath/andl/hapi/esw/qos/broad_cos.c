/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_cos.c
*
* @purpose   This file contains all the routines for Class of Service
*
* @component hapi-broad
*
* @comments
*
* @create    3/16/04
*
* @author    colinw
*
*  COS modifications    jflack
*
*  XGS3 support         robp
*
* @end
*
**********************************************************************/

#include "broad_cos.h"
#include "broad_common.h"
#include "broad_policy.h"
#include "broad_cos_util.h"
#include "l7_usl_bcmx_port.h"
#include "sysbrds.h"

#include "bcmx/cosq.h"
#include "bcmx/port.h"
#include "bcmx/switch.h"
#include "soc/drv.h"
#include "bcmx/bcmx_int.h"
/* Default weights used in non-QoS packages. */
extern L7_uint32 wrr_default_weights[];

extern DAPI_t *dapi_g;
#ifdef L7_ROBO_SUPPORT
extern int bcm_robo_port_dot1p_map_mode_set(int unit, bcm_port_t port, int mode);
#endif

/* When using WDRR, the weights are specified in Kbytes (vs packets for WRR).
   Use the MTU quanta of 16 Kbytes as a multiplier for the weights
   before calling the BCM API. */
#define BROAD_WDRR_MTU_QUANTA 16

/* Unique value used to identify IP Prec mapping values. 
 * This is just a way to make the IP Prec mapping distinct
 * from the dot1p mapping.
 */
#define BROAD_PREC_MAP_ID   'P'
#define BROAD_PREC_DEFAULT_POLICY_ID "PRECDEF"

/*********************************************************************
*
* @purpose  Determines that minimum weight that can be programmed
*           into HW based on the chip type.
*
* @returns  Minimum HW queue weight
*
* @comments For chips that support WDRR mode, we convert the weight
*           to kbytes by multiplying the min HW weight by the
*           MTU quanta. 
*
* @end
*
*********************************************************************/
L7_uint32 hapiBroadCosQueueWeightMin()
{
  static L7_int32 minWeight;
  static L7_BOOL first_time = L7_TRUE;

  /* First get the board info using the bcm call */
  if (first_time)
  {
    first_time = L7_FALSE;
    minWeight = BCM_COSQ_WEIGHT_MIN;

    /* For WDRR, we need to use kbytes instead of packets. Convert weight to kbytes here.*/
    if (hapiBroadCosQueueWDRRSupported())
    {
      minWeight *= BROAD_WDRR_MTU_QUANTA;
    }
  }

  return minWeight;
}

/*********************************************************************
*
* @purpose  Determines that maximum weight that can be programmed
*           into HW based on the chip type.
*
* @returns  Maximum HW queue weight
*
* @comments For chips that support WDRR mode, we convert the weight
*           to kbytes by multiplying the max HW weight by the
*           MTU quanta. 
*
* @end
*
*********************************************************************/
L7_uint32 hapiBroadCosQueueWeightMax()
{
  static L7_int32 maxWeight;
  static L7_BOOL first_time = L7_TRUE;
  const bcm_sys_board_t *board_info ;

  /* First get the board info using the bcm call */
  if (first_time)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      return 0x0F;
    }

    first_time = L7_FALSE;

    switch (board_info->npd_id)
    {
    case __BROADCOM_56304_ID:
    case __BROADCOM_56314_ID:
    case __BROADCOM_56504_ID:
    case __BROADCOM_53115_ID:
      /* These devices only support 4 bits for COS weights. */
      maxWeight = 0x0F;
      break;

    default:
      /* The default is to assume that newer devices have enough bits to support a granularity
         of at least 127 (i.e. at least 7 or 8 bits for the COS weight.) */
      maxWeight = 0x7F;
      break;
    }

    /* For WDRR, we need to use kbytes instead of packets. Convert weight to kbytes here.*/
    if (hapiBroadCosQueueWDRRSupported())
    {
      maxWeight *= BROAD_WDRR_MTU_QUANTA;
    }
  }

  return maxWeight;
}

/*********************************************************************
*
* @purpose  Determines if weighted deficit round robin (WDRR)
*           is supported based on the chip type.
*
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadCosQueueWDRRSupported()
{
  static L7_BOOL wdrrSupported;
  static L7_BOOL first_time = L7_TRUE;
  const bcm_sys_board_t *board_info ;

  /* First get the board info using the bcm call */
  if (first_time)
  {
    board_info = hpcBoardGet() ;
    if (board_info == L7_NULL)
    {
      return L7_FALSE;
    }

    first_time = L7_FALSE;

    switch (board_info->npd_id)
    {
    case __BROADCOM_56304_ID:
    case __BROADCOM_56314_ID:
    case __BROADCOM_56504_ID:
    case __BROADCOM_53115_ID:
      /* We do not support WDRR on these devices. In fact, Helix and FB do support
         WDRR, but because the weights do not translate to data rates in a linear
         manner, they are not suitable for translating from link percentages. */
      wdrrSupported = L7_FALSE;
      break;

    default:
      /* The default is to assume that newer devices support WDRR */
      wdrrSupported = L7_TRUE;
      break;
    }
  }

  return wdrrSupported;
}

/*********************************************************************
*
* @purpose  Determines if egress BW meters
*           are supported based on the chip type.
*
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadQosCosEgressBwSupported()
{
  static L7_BOOL egrBwMetersSupported;
  static L7_BOOL first_time = L7_TRUE;
  const bcm_sys_board_t *board_info ;

  /* First get the board info using the bcm call */
  if (first_time)
  {
    board_info = hpcBoardGet() ;
    if (board_info == L7_NULL)
    {
      return L7_FALSE;
    }

    first_time = L7_FALSE;

    switch (board_info->npd_id)
    {
    case __BROADCOM_56224_ID:
    case __BROADCOM_56304_ID:
    case __BROADCOM_56314_ID:
    case __BROADCOM_56504_ID:
    case __BROADCOM_56514_ID:
    case __BROADCOM_56624_ID:
    case __BROADCOM_56680_ID:
    case __BROADCOM_56685_ID:   /* PTin added: new switch 56685 (Valkyrie2) */
    case __BROADCOM_56843_ID:   /* PTin added: new switch 56843 (Trident) */
    case __BROADCOM_56820_ID:
    case __BROADCOM_56634_ID:
    case __BROADCOM_56524_ID:
    case __BROADCOM_56334_ID:
    case __BROADCOM_56640_ID:   /* PTin added: new switch 5664x (Triumph3) */
    case __BROADCOM_56340_ID:   /* PTin added: new switch 56340 (Helix4) */
      egrBwMetersSupported = L7_TRUE;
      break;

    default:
      egrBwMetersSupported = L7_FALSE;
      break;
    }
  }

  return egrBwMetersSupported;
}

/*********************************************************************
*
* @purpose  Initialize COS component of QOS package
*
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments  
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCosInit(DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Per-port COS init
*
* @param   *dapiPortPtr     @b{(input)} Generic port instance
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments    Invoked once per physical port
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCosPortInit(DAPI_PORT_t *dapiPortPtr)
{
    L7_RC_t result = L7_SUCCESS;

    dapiPortPtr->cmdTable[DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP] = (HAPICTLFUNCPTR_t)hapiBroadQosCosIpPrecedenceToTcMap;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP]       = (HAPICTLFUNCPTR_t)hapiBroadQosCosIpDscpToTcMap;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG]  = (HAPICTLFUNCPTR_t)hapiBroadQosCosIntfTrustModeConfig;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_COS_INTF_CONFIG]             = (HAPICTLFUNCPTR_t)hapiBroadQosCosIntfConfig;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_COS_INTF_STATUS]             = (HAPICTLFUNCPTR_t)hapiBroadQosCosIntfStatus;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG]      = (HAPICTLFUNCPTR_t)hapiBroadQosCosQueueSchedConfig;
    dapiPortPtr->cmdTable[DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG]       = (HAPICTLFUNCPTR_t)hapiBroadQosCosQueueDropConfig;

    return result;
}

/*********************************************************************
*
* @purpose  Reset the DSCP mapping for untrusted or dot1p mappings.
*
* @returns  none
*
* @end
*
*********************************************************************/
static void hapiBroadQosCosResetDscpMapping(bcmx_lport_t port)
{
  int rv;
  int RESET_CP = -1;   /* reset codepoint value */

  /* Remove global mappings */
  rv = bcmx_port_dscp_map_set(BCMX_LPORT_ETHER_ALL, RESET_CP, RESET_CP, 0);

  if (L7_BCMX_OK(rv) == L7_TRUE)
  {
    rv = bcmx_port_dscp_map_mode_set(port, BCM_PORT_DSCP_MAP_NONE);
  }

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "Couldn't reset DSCP mapping for lport 0x%x, rv = %d", port, rv);
  }
}

/* This function is called for untrusted and trust ip-dscp/prec modes. It reuses the dot1p policy as
 * the untrusted and dot1p modes are mutually exclusive. It does not update any port-specific
 * configuration as the port could be acquired and as such the policy is a LAG policy -- we want to
 * maintain port and LAG config separately.
 *
 * Note: This function removes the dot1p policy, if any.
 */
static L7_RC_t hapiBroadQosCosIntfUntrusted(BROAD_PORT_t *hapiPortPtr, L7_uchar8 defTc, L7_BOOL l2Only)
{
    L7_RC_t             result = L7_SUCCESS;
    BROAD_POLICY_t      cosqId;
    L7_BOOL             policyFound = L7_FALSE;
    BROAD_POLICY_RULE_t ruleId;
    L7_uint32           ruleComposite;

    /* Try to find existing policy with same mapping. Generate a single value that
     * is a composite of the default TC and L2 flag for lookup purposes.
     */
     
    if(hapiBroadRoboCheck() == L7_TRUE)
    {
#ifdef L7_ROBO_SUPPORT
        {
          int rv;
          int              bcm_unit;
          bcm_port_t       bcm_port;

          if (BCM_E_NONE != bcmx_lport_to_unit_port(hapiPortPtr->bcmx_lport, &bcm_unit, &bcm_port))
          {
            return L7_FAILURE;
          }

          rv = bcm_robo_port_dot1p_map_mode_set( bcm_unit, bcm_port, 2);
          if (L7_BCMX_OK(rv) == L7_FALSE)
          {
            result = L7_FAILURE;
          }
        }
      return L7_SUCCESS;
#endif
    }

    ruleComposite = defTc << 16 | l2Only;
    if (hapiBroadCosPolicyUtilLookup((L7_uchar8*)&ruleComposite, sizeof(ruleComposite), &cosqId) == L7_TRUE)
    {
        if (hapiPortPtr->dot1pPolicy == cosqId)
            return L7_SUCCESS;   /* policy has not changed */

        policyFound = L7_TRUE;
    }

    if (L7_FALSE == policyFound)   /* create new policy */
    {
        L7_uchar8 nonip[]       = {BROAD_IP_TYPE_NONIP};
        L7_uchar8 exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE};

        /* Create policy to map packets to default traffic class. */
        hapiBroadPolicyCreate(BROAD_POLICY_TYPE_COSQ);

        /* PTin added: allocate CoS rule at VCAP */
        hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);

        /* Add a rule to mark cosq for all other non-ip frames */
        hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
        hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, defTc, 0, 0);
        /* PTin added: FP */
        hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_CLASS_ID, defTc, 0, 0);

        if (L7_TRUE == l2Only) 
        { 
          /* This function is common for Untrusted/DSCP modes. For untrusted, we 
           * match all pkts, l2,l3,tagged,untagged. For DSCP, we need a policy to
           * to match only tagged non-IP packets.
           */  
 
          hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP_TYPE, nonip, exact_match);
        }

        result = hapiBroadPolicyCommit(&cosqId);

        if (L7_SUCCESS == result)
            result = hapiBroadCosPolicyUtilAdd((L7_uchar8*)&ruleComposite, sizeof(ruleComposite), cosqId);

    }

    if (L7_SUCCESS == result)
        result = hapiBroadCosPolicyUtilApply(cosqId, hapiPortPtr->bcmx_lport);

    /* delete the old policy */
    if (BROAD_POLICY_INVALID != hapiPortPtr->dot1pPolicy)
    {
        hapiBroadCosPolicyUtilRemove(hapiPortPtr->dot1pPolicy, hapiPortPtr->bcmx_lport);
        hapiPortPtr->dot1pPolicy = BROAD_POLICY_INVALID;
    }

    if (L7_SUCCESS == result)
        hapiPortPtr->dot1pPolicy = cosqId;

    return result;
}

/* This function is called for trust dot1p mode. It does not update any port-specific
 * configuration as the port could be acquired and as such the policy is a LAG policy --
 * we want to maintain port and LAG config separately.
 */
static L7_RC_t hapiBroadQosCosIntfTrustDot1p(BROAD_PORT_t *hapiPortPtr, L7_uchar8 *dot1pMap)
{
    L7_RC_t result;
    /* Call common code for dot1p mapping as it needed for non-QoS builds also. */

    if (!(hapiBroadRoboCheck() == L7_TRUE))
    {
      result = hapiBroadCosCommitDot1pParams(hapiPortPtr, dot1pMap);
      return result;
    }
#if L7_ROBO_SUPPORT
    {
    L7_int32 i, cosq;
    int       rv;
    int              bcm_unit;
    bcm_port_t       bcm_port;

    if (BCM_E_NONE != bcmx_lport_to_unit_port(hapiPortPtr->bcmx_lport, &bcm_unit, &bcm_port))
    {
      return L7_FAILURE;
    }

    rv = bcm_robo_port_dot1p_map_mode_set( bcm_unit,
                                           bcm_port, 1 );
                            /* last parameter enable/disable */

    if (L7_BCMX_OK(rv) != L7_TRUE)
    { 
        result = L7_FAILURE;
        return result;
    }
    for (i = 0; i < L7_DOT1P_MAX_PRIORITY+1; i++)
    {
      if (dot1pMap[i] <= L7_DOT1P_MAX_PRIORITY)
      {
        cosq = (L7_int32)dot1pMap[i];
        bcmx_cosq_port_mapping_set(hapiPortPtr->bcmx_lport, i, cosq);
      }
    }

    }
#endif
    return L7_SUCCESS;
}

/* This function removes the IP Prec mapping from ports that no longer trust ip-prec. */
static L7_RC_t hapiBroadQosCosRemoveIpPrec(BROAD_PORT_t *hapiPortPtr)
{
    HAPI_BROAD_QOS_PORT_t *qosPortPtr;
    L7_RC_t                result = L7_SUCCESS;

    qosPortPtr = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    /* delete existing policy */
    if (BROAD_POLICY_INVALID != qosPortPtr->cos.precId)
    {
        hapiBroadCosPolicyUtilRemove(qosPortPtr->cos.precId, hapiPortPtr->bcmx_lport);
        qosPortPtr->cos.precId = BROAD_POLICY_INVALID;
    }
    /* delete default 'match all' policy */
    if (BROAD_POLICY_INVALID != qosPortPtr->cos.precDefaultPolicyId)
    {
        hapiBroadCosPolicyUtilRemove(qosPortPtr->cos.precDefaultPolicyId, hapiPortPtr->bcmx_lport);
        qosPortPtr->cos.precDefaultPolicyId = BROAD_POLICY_INVALID;
    }

    return result;
}

/* This function is called for trust ip-prec mode. It does not update any port-specific
 * configuration as the port could be acquired and as such the policy is a LAG policy --
 * we want to maintain port and LAG config separately.
 *
 * Note: Do not use the DSCP table for mapping IP Prec to prioirity as it also updates the
 * 802.1p value in the tagged frames at egress (an undesirable side-effect).
 */
static L7_RC_t hapiBroadQosCosIntfTrustIpPrec(BROAD_PORT_t *hapiPortPtr, L7_uchar8 *precMap, L7_uchar8 defTc)
{
    BROAD_POLICY_t         precId;
    BROAD_POLICY_t         defaultPolicyId;
    L7_uchar8              tmpPrecMap[L7_QOS_COS_MAP_NUM_IPPREC+1];
    L7_uchar8              defaultPolicyKey[] = BROAD_PREC_DEFAULT_POLICY_ID;
    int                    i;
    L7_BOOL                policyFound = L7_FALSE;
    HAPI_BROAD_QOS_PORT_t *qosPortPtr;
    BROAD_POLICY_RULE_t    ruleId;
    L7_RC_t                result = L7_SUCCESS;

    qosPortPtr = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    /* Try to find existing policy with same IP Prec mapping. Make sure the prec mapping
     * looks different from the dot1p mapping by appending a unique value. This is needed
     * as dot1p and ip-prec have the same number of entries and values by default.
     */
    for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
       tmpPrecMap[i] = qosPortPtr->cos.precMap[i];
    tmpPrecMap[L7_QOS_COS_MAP_NUM_IPPREC] = BROAD_PREC_MAP_ID;   /* append a unique value */
    
    if (hapiBroadCosPolicyUtilLookup(tmpPrecMap, L7_QOS_COS_MAP_NUM_IPPREC+1, &precId) == L7_TRUE)
    {
        if (qosPortPtr->cos.precId == precId)
            return L7_SUCCESS;   /* policy has not changed */

        policyFound = L7_TRUE;
    }

    /* delete existing ip precedence policy */
    if (BROAD_POLICY_INVALID != qosPortPtr->cos.precId)
    {
        hapiBroadCosPolicyUtilRemove(qosPortPtr->cos.precId, hapiPortPtr->bcmx_lport);
        qosPortPtr->cos.precId = BROAD_POLICY_INVALID;
    }
    if (BROAD_POLICY_INVALID != qosPortPtr->cos.precDefaultPolicyId)
    {
        hapiBroadCosPolicyUtilRemove(qosPortPtr->cos.precDefaultPolicyId, hapiPortPtr->bcmx_lport);
        qosPortPtr->cos.precDefaultPolicyId = BROAD_POLICY_INVALID;
    }

    /* delete existing dot1p/untrusted policy */
    if (BROAD_POLICY_INVALID != hapiPortPtr->dot1pPolicy)
    {
        hapiBroadCosPolicyUtilRemove(hapiPortPtr->dot1pPolicy, hapiPortPtr->bcmx_lport);
        hapiPortPtr->dot1pPolicy = BROAD_POLICY_INVALID;
    }
    
    if (L7_FALSE == policyFound)   /* create new policy */
    {
        int                 i;
        L7_ushort16         ipv4  = L7_ETYPE_IP;
        L7_uchar8           precMask[]  = {0xE0};   /* 3 msb */
        L7_uchar8           exactMask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE};

        /* create policy with new mappings */
        hapiBroadPolicyCreate(BROAD_POLICY_TYPE_COSQ);

        /* PTin added: allocate CoS rule at VCAP */
        hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);

        for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
        {
            L7_uchar8 ipPrec = BROAD_IPPREC_TO_TOS(i);  /* cast to uchar8 and convert to tos */

            hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
            hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipv4, exactMask);
            hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DSCP, &ipPrec, precMask);
            hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, precMap[i], 0, 0);
            /* PTin added: FP */
            hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_CLASS_ID, precMap[i], 0, 0);
        }

        result = hapiBroadPolicyCommit(&precId);

        if (L7_SUCCESS == result)
            result = hapiBroadCosPolicyUtilAdd(tmpPrecMap, L7_QOS_COS_MAP_NUM_IPPREC+1, precId);
    }

    if (L7_SUCCESS == result)
        result = hapiBroadCosPolicyUtilApply(precId, hapiPortPtr->bcmx_lport);

    if (L7_SUCCESS == result)
    {
        qosPortPtr->cos.precId = precId;

        /* Add 'match all' rule to handle L2-only frames. The Untrusted mode policy is not reused for this
         * as the restriction of sharing a single lookup engine for all COS policies on XGS3 (Issue 42124)
         * makes it impossible to get the rule priorities right such that the default rules do not override the
         * actual mappings. A separate policy is used to add the default rule in this case. */
        if (hapiBroadCosPolicyUtilLookup(defaultPolicyKey, sizeof(defaultPolicyKey), &defaultPolicyId) == FALSE)
        {
          hapiBroadPolicyCreate(BROAD_POLICY_TYPE_COSQ);

          /* PTin added: allocate CoS rule at VCAP */
          hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);

         /* Add a rule to mark cosq for all frames - no qualifier means "match all". 
          * Mark the rule as low priority because we need to ensure that it does not
          * override mappings added at a later time */
          hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
          hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, defTc, 0, 0);
          /* PTin added: FP */
          hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_CLASS_ID, defTc, 0, 0);

          result = hapiBroadPolicyCommit(&defaultPolicyId);

          if (L7_SUCCESS == result)
              result = hapiBroadCosPolicyUtilAdd(defaultPolicyKey, sizeof(defaultPolicyKey), defaultPolicyId);
        }

        if (L7_SUCCESS == result)
            result = hapiBroadCosPolicyUtilApply(defaultPolicyId, hapiPortPtr->bcmx_lport);

        qosPortPtr->cos.precDefaultPolicyId = defaultPolicyId;
    }

    return result;
}

/* This function is called for trust ip-dscp mode. It does not update any port-specific configuration
 * as the port could be acquired and as such the policy is a LAG policy -- we want to maintain port
 * and LAG config separately.
 *
 * Note: The DSCP table updates the 802.1p value in the tagged frames at egress. This is BCM intent
 * but does not match exactly with the FastPath QoS Functional Spec.
 */
static L7_RC_t hapiBroadQosCosIntfTrustIpDscp(BROAD_PORT_t *hapiPortPtr, L7_uchar8 *dscpMap, L7_uchar8 defTc, L7_BOOL remap)
{
  L7_uchar8 dscp;
  int       rv;
  L7_RC_t   result = L7_SUCCESS;

  /* set default for L2-only frames (and remove any dot1p policies that might be in effect) */
  if (hapiBroadRoboCheck()!= L7_TRUE)
  {
    result = hapiBroadQosCosIntfUntrusted(hapiPortPtr, defTc, L7_TRUE);
  }

  if ( result == L7_SUCCESS)
  {
    rv = bcmx_port_dscp_map_mode_set(hapiPortPtr->bcmx_lport, BCM_PORT_DSCP_MAP_ALL);
    if (L7_BCMX_OK(rv) != L7_TRUE)
      result = L7_FAILURE;
  }

  if (result == L7_SUCCESS)
  {
    if ((remap == L7_TRUE) || (hapiBroadRoboCheck() == L7_TRUE) )
    {
      /* set dscp map table for L3 frames */
      for (dscp = 0; dscp < L7_QOS_COS_MAP_NUM_IPDSCP; dscp++)
      {
        /* Change global mapping table */
        rv = bcmx_port_dscp_map_set(hapiPortPtr->bcmx_lport /*BCMX_LPORT_ETHER_ALL*/,
                                    dscp, dscp, dscpMap[dscp]);

        if (L7_BCMX_OK(rv) != L7_TRUE)
          result = L7_FAILURE;
      }
    }
  }
  return result;
}
static L7_RC_t hapiBroadQosCosIntfRateShape(BROAD_PORT_t *dstPortPtr, L7_uint32 egrRate)
{
    int                              rv;
    L7_RC_t                          result = L7_SUCCESS;
    L7_uint32                         portSpeed;
    usl_bcm_port_shaper_config_t     shaperConfig;

    memset(&shaperConfig, 0, sizeof(shaperConfig));
  
    if (L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
    {
      /* parameter provided is in Kbps */
      shaperConfig.rate = egrRate;
    }
    else
    {
      /* result is a percent converted to Kbps */
      hapiBroadQosIntfSpeedGet(dstPortPtr, &portSpeed);
      shaperConfig.rate = ((egrRate * portSpeed) / 100); 

    }

    /* Set kbits_burst equal to 2% of kbit per sec */
    shaperConfig.burst = shaperConfig.rate / 50; 
    
    rv = usl_bcmx_port_rate_egress_set(dstPortPtr->bcmx_lport, shaperConfig);
    if (L7_BCMX_OK(rv) != L7_TRUE)
      result = L7_FAILURE;

    return result;
}

static void hapiBroadQosCosQueueNormalize(int *weights, int base)
{
    int i;
    int total;

    total = 0;

    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
        total += weights[i];

    if (total == 0) return;

    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
        weights[i] = (weights[i] * base) / total;
}

static void hapiBroadQosCosQueueScale(int *weights, int minimum, int maximum)
{
    int i;
    int largest = 0;

    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
        if (weights[i] > largest)
            largest = weights[i];

    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
        int newWeight;

        newWeight = (weights[i] * maximum) / largest;
        if ((weights[i] > 0) && (newWeight > 0))
            weights[i] = newWeight;
        else
        if (weights[i] > 0)
            weights[i] = minimum;
    }
}

/* Use the Egress BW meters to configure guaranteed minimum and maximum BW per port/per COS. */
static L7_RC_t hapiBroadQosCosEgressBwConfig(BROAD_PORT_t *dstPortPtr, HAPI_BROAD_COS_PORT_t *cosData)
{
  int                               rv, i;
  int                               schedulerMode;
  int                               noDelay = 0;
  int                               weights[BCM_COS_COUNT];
  unsigned long                     minKbps[BCM_COS_COUNT];   /* PTin modified: QoS: int to unsigned long */
  unsigned long                     maxKbps[BCM_COS_COUNT];   /* PTin modified: QoS: int to unsigned long */
  L7_uint32                         portSpeed;
  L7_RC_t                           result = L7_SUCCESS;
  usl_bcm_port_cosq_sched_config_t  cosqSchedConfig;

  memset(&cosqSchedConfig, 0, sizeof(cosqSchedConfig));

  schedulerMode = hapiBroadCosQueueWDRRSupported() ? 
                     BCM_COSQ_DEFICIT_ROUND_ROBIN  : 
                     BCM_COSQ_WEIGHTED_ROUND_ROBIN;

  hapiBroadQosIntfSpeedGet(dstPortPtr, &portSpeed);

  /* Start with default weights and remove any SP queues.
   */    
  memcpy(weights, wrr_default_weights, sizeof(weights));
  memset(minKbps, 0, sizeof(minKbps));
  memset(maxKbps, 0, sizeof(maxKbps));

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if (cosData->schedType[i] == DAPI_QOS_COS_QUEUE_SCHED_TYPE_STRICT)
    {
      weights[i] = BCM_COSQ_WEIGHT_STRICT; /* zero */
    }
    else 
    {
      if (schedulerMode == BCM_COSQ_DEFICIT_ROUND_ROBIN)
      {
        /* PTin modified: Use provided weights */
      #if 1
        weights[i] = cosData->wrr_weights[i];
      #else
        /* Need to multiply weight by MTU quantum for BCM APIs when using WDRR mode. */
        weights[i] *= BROAD_WDRR_MTU_QUANTA;
        /* PTin added: correction for trident - Maximum weight for trident is 127 (not 128) */
        weights[i]--;
      #endif
      }
    }

    /* PTin modified: COS */
    /* Kbps units */
    if (L7_QOS_COS_QUEUE_BANDWIDTH_RATE_UNITS == L7_RATE_UNIT_KBPS)
    {
      // Limit speeds
      if (cosData->minBw[i] > portSpeed)  cosData->minBw[i] = portSpeed;
      if (cosData->maxBw[i] > portSpeed)  cosData->maxBw[i] = portSpeed;

      minKbps[i] = cosData->minBw[i];
      maxKbps[i] = cosData->maxBw[i];
    }
    /* Percent units */
    else
    {
      // Limit values
      if (cosData->minBw[i] > 100)  cosData->minBw[i] = 100;
      if (cosData->maxBw[i] > 100)  cosData->maxBw[i] = 100;

      minKbps[i] = (cosData->minBw[i] * portSpeed) / 100;
      maxKbps[i] = (cosData->maxBw[i] * portSpeed) / 100;
    }
  }

  /* apply the scheduling policy to the port */
  cosqSchedConfig.delay = noDelay;
  memcpy(&(cosqSchedConfig.weights), weights, sizeof(cosqSchedConfig.weights));
  memcpy(&(cosqSchedConfig.minKbps), minKbps, sizeof(cosqSchedConfig.minKbps));
  memcpy(&(cosqSchedConfig.maxKbps), maxKbps, sizeof(cosqSchedConfig.maxKbps));
  cosqSchedConfig.mode = schedulerMode;

  rv = usl_bcmx_port_cosq_sched_set(dstPortPtr->bcmx_lport, cosqSchedConfig);
  if (L7_BCMX_OK(rv) == L7_FALSE)
  {
    result = L7_FAILURE;
  }

  return result;
}

/* Use COS weights to configure guaranteed minimum BW per port/ per COS. This approach is generally not
   as accurate as using the egress BW meters. */
static L7_RC_t hapiBroadQosCosQueueWeightsConfig(BROAD_PORT_t *dstPortPtr, HAPI_BROAD_COS_PORT_t *cosData)
{
  int           rv, i;
  int           weights[BCM_COS_MAX+1];
  int           minKbps[BCM_COS_MAX+1];
  int           maxKbps[BCM_COS_MAX+1];
  int           availBw = 100;
  int           NO_DELAY = 0;
  L7_RC_t       result = L7_SUCCESS;
  L7_BOOL       weightedSchd = L7_FALSE; /* Set to L7_TRUE if atleast one queue is of type WEIGHTED */
  L7_BOOL       allWeightedStrict = L7_FALSE; /* Set to L7_TRUE if atleast one queue is of type WEIGHTED */
  L7_uint32     numOfStrictQs = 0;
  usl_bcm_port_cosq_sched_config_t  cosqSchedConfig;
  /* PTin added: COS */
  #if 1
  L7_uint32     portSpeed;

  // Extract port speed
  hapiBroadQosIntfSpeedGet(dstPortPtr, &portSpeed);
  #endif

  memset(&cosqSchedConfig, 0, sizeof(cosqSchedConfig));

  /* Start with default weights and remove any SP queues as they get their
   * share of the bandwidth off the top.
   */    
  memcpy(weights, wrr_default_weights, sizeof(weights));
  memset(minKbps, 0, sizeof(minKbps));
  memset(maxKbps, 0, sizeof(maxKbps));

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    /* PTin added: COS */
    #if 1
    /* Kbps units */
    if (L7_QOS_COS_QUEUE_BANDWIDTH_RATE_UNITS == L7_RATE_UNIT_KBPS)
    {
      // Limit speeds
      if (cosData->minBw[i] > portSpeed)  cosData->minBw[i] = portSpeed;
      if (cosData->maxBw[i] > portSpeed)  cosData->maxBw[i] = portSpeed;

      minKbps[i] = cosData->minBw[i];
      maxKbps[i] = cosData->maxBw[i];

      // Translate min bandwidth to percentage
      cosData->minBw[i] = (cosData->minBw[i] * 100) / portSpeed;
      // Translate max bandwidth to percentage
      cosData->maxBw[i] = (cosData->maxBw[i] * 100) / portSpeed;
    }
    /* Percent units */
    else
    {
      // Limit values
      if (cosData->minBw[i] > 100)  cosData->minBw[i] = 100;
      if (cosData->maxBw[i] > 100)  cosData->maxBw[i] = 100;

      minKbps[i] = (cosData->minBw[i] * portSpeed) / 100;
      maxKbps[i] = (cosData->maxBw[i] * portSpeed) / 100;
    }
    #endif

    if (cosData->schedType[i] == DAPI_QOS_COS_QUEUE_SCHED_TYPE_STRICT)
    {
      weights[i] = BCM_COSQ_WEIGHT_STRICT; /* zero */
    }
    else
    {
      /* Use these weights, instead of default ones */
      weights[i] = cosData->wrr_weights[i];   /* PTin added: QoS */
      weightedSchd = L7_TRUE;
    }
  }

  if (hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
  {
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
      if (weights[i] == BCM_COSQ_WEIGHT_STRICT)
      {
        numOfStrictQs++;
      }

    }
    if (numOfStrictQs == L7_MAX_CFG_QUEUES_PER_PORT)
    {
      allWeightedStrict = L7_TRUE;
    }
    switch (numOfStrictQs)
    {
    case 1:  /* Q3 strict - q2/q1/q0 should be WRR */
      if (weights[L7_MAX_CFG_QUEUES_PER_PORT-1]!= BCM_COSQ_WEIGHT_STRICT)
      {
        return L7_FAILURE;
      }
      break;

    case 2:  /* Q3/q2 strict - q1/q0 should be WRR */
      if ((weights[L7_MAX_CFG_QUEUES_PER_PORT-1]!= BCM_COSQ_WEIGHT_STRICT) ||
          (weights[L7_MAX_CFG_QUEUES_PER_PORT-2]!= BCM_COSQ_WEIGHT_STRICT)
         )
      {
        return L7_FAILURE;
      }
      break;

    case 3: /* This case is not supported in the HW*/
      /* But Kept here to support the q3/q2/q1/q0 case for now */
      if ((weights[L7_MAX_CFG_QUEUES_PER_PORT-1] == BCM_COSQ_WEIGHT_STRICT) && 
          (weights[L7_MAX_CFG_QUEUES_PER_PORT-2] == BCM_COSQ_WEIGHT_STRICT) &&
          (weights[L7_MAX_CFG_QUEUES_PER_PORT-3] == BCM_COSQ_WEIGHT_STRICT)
         )
      {
        return L7_SUCCESS;
      }
      else
      {
        return L7_FAILURE;
      }

      break;

    case L7_MAX_CFG_QUEUES_PER_PORT: /* q3/q2/q1/q0 should be Strict */
      allWeightedStrict = L7_TRUE;
      break;

    default:
      break;

    }
  }
  /* bw guarantees are configured in % so normalize weights accordingly */
  if (weightedSchd == L7_TRUE)
    hapiBroadQosCosQueueNormalize(weights, 100);

  /* Calculate available bw left after meeting the guarantees. */
  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if (DAPI_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED == cosData->schedType[i])
    {
      if (cosData->minBw[i] > 0)
      {
        availBw -= cosData->minBw[i];
      }
    }
  }

  if (availBw < 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "In %s, Failed to configure minimum bandwidth. Available bandwidth %d."
            " Attempting to configure the bandwidth beyond it’s capabilities.",
            __FUNCTION__, availBw);
    return L7_FAILURE;
  }
  else
  {
    /* Normalize all the queues against available bandwidth 
       based on the default weight ratios */
    if (weightedSchd == L7_TRUE)
      hapiBroadQosCosQueueNormalize(weights, availBw);
  }

  for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    if (DAPI_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED == cosData->schedType[i])
    {
      if (cosData->minBw[i] > 0)
      {
        weights[i] += cosData->minBw[i];
      }
      else if (cosData->minBw[i] == 0)
      {
        if (weights[i] < BCM_COSQ_WEIGHT_MIN)
          weights[i] = BCM_COSQ_WEIGHT_MIN;
      }
    }
  }

  for (i = L7_MAX_CFG_QUEUES_PER_PORT; i <= BCM_COS_MAX; i++)
  {
    weights[i] = 0;
  }

/* scale weights so they fit within valid range */
  if (weightedSchd == L7_TRUE)
    hapiBroadQosCosQueueScale(weights, hapiBroadCosQueueWeightMin(), hapiBroadCosQueueWeightMax());

  if (L7_SUCCESS == result)
  {
    cosqSchedConfig.delay = NO_DELAY;
    memcpy(&(cosqSchedConfig.weights), weights, sizeof(cosqSchedConfig.weights));
    memcpy(&(cosqSchedConfig.minKbps), minKbps, sizeof(cosqSchedConfig.minKbps));
    memcpy(&(cosqSchedConfig.maxKbps), maxKbps, sizeof(cosqSchedConfig.maxKbps));

    if (hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
    {
      if (allWeightedStrict == L7_TRUE)
      {
        cosqSchedConfig.mode = BCM_COSQ_STRICT;
      }
      else
      {
        cosqSchedConfig.mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
      } 
    }
    else
    {
      /* apply the scheduling policy to the port */
      cosqSchedConfig.mode = hapiBroadCosQueueWDRRSupported() ? BCM_COSQ_DEFICIT_ROUND_ROBIN : BCM_COSQ_WEIGHTED_ROUND_ROBIN;
    }

    printf("%s(%d) I was here: weights={%u,%u,%u,%u,%u,%u,%u,%u}\r\n",__FUNCTION__,__LINE__,
           cosqSchedConfig.weights[0],cosqSchedConfig.weights[1],cosqSchedConfig.weights[2],cosqSchedConfig.weights[3],cosqSchedConfig.weights[4],cosqSchedConfig.weights[5],cosqSchedConfig.weights[6],cosqSchedConfig.weights[7]);
    rv = usl_bcmx_port_cosq_sched_set(dstPortPtr->bcmx_lport, cosqSchedConfig);
    if (L7_BCMX_OK(rv) == L7_FALSE)
    {
      result = L7_FAILURE;
    }
  }

  return result;
}

static L7_RC_t hapiBroadQosCosQueueConfig(BROAD_PORT_t *dstPortPtr, HAPI_BROAD_COS_PORT_t *cosData)
{
  if (hapiBroadQosCosEgressBwSupported())
  {
    return hapiBroadQosCosEgressBwConfig(dstPortPtr, cosData);
  }
  else
  {
    return hapiBroadQosCosQueueWeightsConfig(dstPortPtr, cosData);
  }
}

/* this function is called whenever a policy is applied to a port. */
/* it copies CoS config from a source port to a dest port. */
static L7_RC_t hapiBroadQosCosApplyPolicy(BROAD_PORT_t *dstPortPtr, BROAD_PORT_t *srcPortPtr)
{
    L7_RC_t                result = L7_SUCCESS;
    BROAD_SYSTEM_t        *hapiSystemPtr;
    HAPI_BROAD_QOS_PORT_t *qosPortPtr;
    HAPI_BROAD_QOS_t      *qos;

    if (BROAD_PORT_IS_LAG(dstPortPtr))
    {
        SYSAPI_PRINTF(SYSAPI_LOGGING_HAPI_ERROR,
                      "%s %d: In %s LAG interface is not valid\n",
                      __FILE__, __LINE__, __FUNCTION__);
        return L7_FAILURE;
    }

    hapiSystemPtr = (BROAD_SYSTEM_t *)(dapi_g->system->hapiSystem);
    qos = (HAPI_BROAD_QOS_t *)(hapiSystemPtr->qos);

    qosPortPtr = (HAPI_BROAD_QOS_PORT_t*)srcPortPtr->qos;

    /* apply trust mode */
    switch (qosPortPtr->cos.trustMode)
    {
    case DAPI_QOS_COS_INTF_MODE_UNTRUSTED:
        hapiBroadQosCosResetDscpMapping(dstPortPtr->bcmx_lport);
        if(hapiBroadRoboCheck() == L7_TRUE)
        {
#ifdef L7_ROBO_SUPPORT
        {
          int rv;
          int              bcm_unit;
          bcm_port_t       bcm_port;

          if (BCM_E_NONE != bcmx_lport_to_unit_port(dstPortPtr->bcmx_lport, &bcm_unit, &bcm_port))
          {
            return L7_FAILURE;
          }

          rv = bcm_robo_port_dot1p_map_mode_set( bcm_unit, bcm_port, 0);
          if (L7_BCMX_OK(rv) == L7_FALSE)
          {
            result = L7_FAILURE;
          }
        }
#endif
        }  
        hapiBroadQosCosRemoveIpPrec(dstPortPtr);
        result = hapiBroadQosCosIntfUntrusted(dstPortPtr, qosPortPtr->cos.defaultCos, L7_FALSE);
        break;

    case DAPI_QOS_COS_INTF_MODE_TRUST_DOT1P:
        hapiBroadQosCosResetDscpMapping(dstPortPtr->bcmx_lport);
        hapiBroadQosCosRemoveIpPrec(dstPortPtr);
        result = hapiBroadQosCosIntfTrustDot1p(dstPortPtr, srcPortPtr->dot1pMap);
        break;

    case DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC:
        hapiBroadQosCosResetDscpMapping(dstPortPtr->bcmx_lport);
        result = hapiBroadQosCosIntfTrustIpPrec(dstPortPtr, qosPortPtr->cos.precMap, qosPortPtr->cos.defaultCos);
        break;

    case DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP:
        hapiBroadQosCosRemoveIpPrec(dstPortPtr);
        
        /* PTin modified: CoS */
        #if 1
        result = hapiBroadQosCosIntfTrustIpDscp(dstPortPtr,
                                                qosPortPtr->cos.dscpMap,
                                                qosPortPtr->cos.defaultCos,
                                                qosPortPtr->cos.dscpMapDirty);
        #else
        result = hapiBroadQosCosIntfTrustIpDscp(dstPortPtr,
                                                qos->dscpMap,
                                                qosPortPtr->cos.defaultCos,
                                                qos->dscpMapDirty);
        #endif

        if (result == L7_SUCCESS)
           qos->dscpMapDirty = L7_FALSE;
           qosPortPtr->cos.dscpMapDirty = L7_FALSE;   /* PTin added: CoS */
        break;

#if defined(FEAT_METRO_CPE_V1_0)
    case DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_IPDSCP:
        hapiBroadQosCosResetDscpMapping(dstPortPtr->bcmx_lport);
        qosPortPtr->cos.trustMode = DAPI_QOS_COS_INTF_MODE_UNTRUSTED;
         break;
    case DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_DOT1P:
        if(hapiBroadRoboCheck() == L7_TRUE)
        {
#ifdef L7_ROBO_SUPPORT
        {
          int rv;
          int              bcm_unit;
          bcm_port_t       bcm_port;

          if (BCM_E_NONE != bcmx_lport_to_unit_port(dstPortPtr->bcmx_lport, &bcm_unit, &bcm_port))
          {
            return L7_FAILURE;
          }

          rv = bcm_robo_port_dot1p_map_mode_set( bcm_unit, bcm_port, 0);
          if (L7_BCMX_OK(rv) == L7_FALSE)
          {
            result = L7_FAILURE;
          }
        }
#endif
        }
        qosPortPtr->cos.trustMode = DAPI_QOS_COS_INTF_MODE_UNTRUSTED;
        break;
#endif
    default:
        result = L7_FAILURE;
        break;
    }

    /* apply interface shaping config */
    if ((L7_SUCCESS == result) && (L7_TRUE == qosPortPtr->cos.intfShapingSpec))
        result = hapiBroadQosCosIntfRateShape(dstPortPtr, qosPortPtr->cos.intfShaping);

    /* apply per-queue config */
    if ((L7_SUCCESS == result) && (L7_TRUE == qosPortPtr->cos.queueConfigSpec))
        result = hapiBroadQosCosQueueConfig(dstPortPtr, &qosPortPtr->cos);

    return result;
}

/* this function is called whenever a policy is applied to an interface. */
/* if the port is a LAG then all member ports are updated */
static L7_RC_t hapiBroadQosCosApplyPolicyToIface(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
    DAPI_PORT_t  *dapiPortPtr;
    BROAD_PORT_t *hapiPortPtr;
    L7_RC_t       result = L7_SUCCESS;

    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

    if (BROAD_PORT_IS_LAG(hapiPortPtr))
    {
        int               i;
        DAPI_LAG_ENTRY_t *lagMemberSet;
        BROAD_PORT_t     *lagMemberPtr;
        L7_RC_t           tmpRc;

        lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

        /* apply CoS config to each LAG member */
        for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
        {
            if (L7_TRUE == lagMemberSet[i].inUse)
            {
                lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);

                tmpRc = hapiBroadQosCosApplyPolicy(lagMemberPtr, hapiPortPtr);
                if (L7_SUCCESS != tmpRc)
                    result = tmpRc;
            }
        }
    }
    else
    {
        /* apply CoS config to individual port */
        if (!BROAD_PORT_IS_ACQUIRED(hapiPortPtr))
        {
            result = hapiBroadQosCosApplyPolicy(hapiPortPtr, hapiPortPtr);
        }
    }

    return result;
}

/*********************************************************************
*
* @purpose  This command maps an individual IP Precedence value to one 
*           of the port's available traffic classes (transmit queues).
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.ipPrecedenceToTcMap
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This COS mapping table is used when port trust mode is set to 
*           DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_IP_PRECEDENCE_TO_TC_MAP */
L7_RC_t hapiBroadQosCosIpPrecedenceToTcMap(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                result = L7_SUCCESS;
    DAPI_QOS_CMD_t        *cmdCos = (DAPI_QOS_CMD_t*)data;
    BROAD_PORT_t          *hapiPortPtr;
    HAPI_BROAD_QOS_PORT_t *qosPortPtr;
    L7_uchar8              prec;
    L7_uchar8              tc;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    if (cmdCos->cmdData.ipPrecedenceToTcMap.getOrSet != DAPI_CMD_SET)
    {
        /* get IP precedence to traffic class mapping from device */
        return L7_FAILURE;
    }

    /* set IP precedence to traffic class mapping in local table */
    prec = cmdCos->cmdData.ipPrecedenceToTcMap.precedence;
    tc   = cmdCos->cmdData.ipPrecedenceToTcMap.traffic_class;

    hapiBroadQosSemTake(dapi_g);

    qosPortPtr->cos.precMap[prec] = tc;

    if (DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC == qosPortPtr->cos.trustMode)
        result = hapiBroadQosCosApplyPolicyToIface(usp, dapi_g);

    hapiBroadQosSemGive(dapi_g);

    return result;
}

/*********************************************************************
*
* @purpose  This command maps an individual IP DSCP value to one of the
*           port's available traffic classes (transmit queues).
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.ipDscpToTcMap
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This table is used when port trust mode is set to 
*           DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_IP_DSCP_TO_TC_MAP */
L7_RC_t hapiBroadQosCosIpDscpToTcMap(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                     result = L7_SUCCESS;
    DAPI_QOS_CMD_t             *cmdCos = (DAPI_QOS_CMD_t*)data;
    BROAD_PORT_t               *hapiPortPtr;
    DAPI_PORT_t                *dapiPortPtr;
    HAPI_BROAD_QOS_PORT_t      *qosPortPtr;
    BROAD_SYSTEM_t             *hapiSystemPtr;
    HAPI_BROAD_QOS_t           *qos;
    L7_uchar8                   dscp;
    L7_uchar8                   tc;
    int                         rv;

    if (cmdCos->cmdData.ipDscpToTcMap.getOrSet != DAPI_CMD_SET)
    {
        /* get IP DSCP to traffic class mapping from device */
        return L7_FAILURE;
    }

    dscp = cmdCos->cmdData.ipDscpToTcMap.dscp;
    tc   = cmdCos->cmdData.ipDscpToTcMap.traffic_class;

    hapiBroadQosSemTake(dapi_g);

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    hapiSystemPtr = (BROAD_SYSTEM_t *)(dapi_g->system->hapiSystem);
    qos = (HAPI_BROAD_QOS_t *)(hapiSystemPtr->qos);

    /* Update the appropriate mapping table and set dirty bit
     * to indicate a change */
    qos->dscpMap[dscp] = tc;
    qos->dscpMapDirty = L7_TRUE;

    /* PTin added: CoS */
    #if 1
    qosPortPtr->cos.dscpMap[dscp] = tc;
    qosPortPtr->cos.dscpMapDirty = L7_TRUE;
    #endif

    /* set dscp map table for L3 frames */
    #if 1
    if (BROAD_PORT_IS_LAG(hapiPortPtr))
    {
        int               i;
        DAPI_LAG_ENTRY_t *lagMemberSet;
        BROAD_PORT_t     *lagMemberPtr;

        lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

        /* apply CoS config to each LAG member */
        for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
        {
            if (L7_TRUE == lagMemberSet[i].inUse)
            {
                lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);

                rv = bcmx_port_dscp_map_set(lagMemberPtr->bcmx_lport,
                                            dscp, dscp, qosPortPtr->cos.dscpMap[dscp]);
                if (BCM_E_NONE != rv)
                    break;
            }
        }
    }
    else
    {
      rv = bcmx_port_dscp_map_set(hapiPortPtr->bcmx_lport,
                                  dscp, dscp, qosPortPtr->cos.dscpMap[dscp]);
    }
    #else
    rv = bcmx_port_dscp_map_set(BCMX_LPORT_ETHER_ALL,
                                dscp, dscp, qos->dscpMap[dscp]);
    #endif
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI, "Error applying bcmx_port_dscp_map_set: rv=%d", rv);
      result = L7_FAILURE;
    }

    hapiBroadQosSemGive(dapi_g);

    return result;
}

/*********************************************************************
*
* @purpose  Configure COS trust mode for the specified interface
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_COS_CMD_t.cmdData.intfTrustModeConfig
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments The intf trust mode setting determines which COS 
*           mapping table is used for handling ingress packets.
*           If the intf mode is untrusted, all ingress packets
*           are assigned based on the port default priority.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_INTF_TRUST_MODE_CONFIG */
L7_RC_t hapiBroadQosCosIntfTrustModeConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    int                         i;
    L7_RC_t                     result = L7_SUCCESS;
    DAPI_QOS_CMD_t             *cmdCos = (DAPI_QOS_CMD_t*)data;
    DAPI_QOS_COS_INTF_MODE_t    intfTrustMode;
    DAPI_QOS_COS_MAP_TABLE_t   *pMapTable;
    BROAD_PORT_t               *hapiPortPtr;
    BROAD_SYSTEM_t             *hapiSystemPtr;
    HAPI_BROAD_QOS_PORT_t      *qosPortPtr;
    HAPI_BROAD_QOS_t           *qos;

    hapiSystemPtr = (BROAD_SYSTEM_t *)(dapi_g->system->hapiSystem);
    qos = (HAPI_BROAD_QOS_t *)(hapiSystemPtr->qos);

    hapiPortPtr = HAPI_PORT_GET(usp,dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    if (cmdCos->cmdData.intfTrustModeConfig.getOrSet != DAPI_CMD_SET)
    {
        /* get trust mode from device */
        return L7_FAILURE;
    }

    intfTrustMode = cmdCos->cmdData.intfTrustModeConfig.mode;
    pMapTable     = &cmdCos->cmdData.intfTrustModeConfig.mapTable;


    hapiBroadQosSemTake(dapi_g);

    /* Store all the CoS configuration for the interface. */
    switch (intfTrustMode)
    {
    case DAPI_QOS_COS_INTF_MODE_UNTRUSTED:
        /* for untrusted mode use the same traffic class for all frames */
        qosPortPtr->cos.trustMode  = DAPI_QOS_COS_INTF_MODE_UNTRUSTED;
        qosPortPtr->cos.defaultCos = pMapTable->defaultPrio; /*pMapTable->dot1p_traffic_class[0];*/     /* PTin modified: + default prio */
        break;

    case DAPI_QOS_COS_INTF_MODE_TRUST_DOT1P:
        /* for trust dot1p mode every frame will fall into a priority, either
         * from the tag or by the port default priority
         */
        qosPortPtr->cos.trustMode  = DAPI_QOS_COS_INTF_MODE_TRUST_DOT1P;
        qosPortPtr->cos.defaultCos = pMapTable->defaultPrio; /*0;*/                                     /* PTin modified: + default prio */
        for (i = 0; i < L7_DOT1P_MAX_PRIORITY+1; i++)
            hapiPortPtr->dot1pMap[i] = pMapTable->dot1p_traffic_class[i];
        hapiPortPtr->dot1pMap[L7_DOT1P_MAX_PRIORITY+1] = pMapTable->defaultPrio;                        /* PTin modified: + default prio */
        break;

    case DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC:
        /* for L3 frames use the IP Prec but for L2 frames use the default */
        qosPortPtr->cos.trustMode  = DAPI_QOS_COS_INTF_MODE_TRUST_IPPREC;
        qosPortPtr->cos.defaultCos = pMapTable->defaultPrio; /*pMapTable->dot1p_traffic_class[0];*/     /* PTin modified: + default prio */
        for (i = 0; i < L7_QOS_COS_MAP_NUM_IPPREC; i++)
            qosPortPtr->cos.precMap[i] = pMapTable->ip_prec_traffic_class[i];
        break;

    case DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP:
        /* for L3 frames use the IP DSCP but for L2 frames use the default */
        qosPortPtr->cos.trustMode  = DAPI_QOS_COS_INTF_MODE_TRUST_IPDSCP;
        qosPortPtr->cos.defaultCos = pMapTable->defaultPrio; /*pMapTable->dot1p_traffic_class[0];*/     /* PTin modified: + default prio */

        /* If the locally stored dscp mapping table is different from the
         * one passed down, indicate a change by setting the dirty bit */ 
        /* Global mapping table */
        if(memcmp(pMapTable->ip_dscp_traffic_class, qos->dscpMap, sizeof(qos->dscpMap)))
        {
            for (i = 0; i < L7_QOS_COS_MAP_NUM_IPDSCP; i++)
            {
                qos->dscpMap[i] = pMapTable->ip_dscp_traffic_class[i];
                qosPortPtr->cos.dscpMap[i] = pMapTable->ip_dscp_traffic_class[i];   /* PTin added: CoS */
            }

            qos->dscpMapDirty = L7_TRUE;
            qosPortPtr->cos.dscpMapDirty = L7_TRUE;   /* PTin added: CoS */
        }
        break;
#if defined(FEAT_METRO_CPE_V1_0)
    case DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_IPDSCP:
        qosPortPtr->cos.trustMode  = DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_IPDSCP;
        break;
    case DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_DOT1P:
        qosPortPtr->cos.trustMode  = DAPI_QOS_COS_INTF_MODE_TRUST_UNSET_DOT1P;
        break;
#endif
    default:
        result = L7_FAILURE;
        break;
    }

    /* Apply the configuration to the interface. */
    if (L7_SUCCESS == result)
        result = hapiBroadQosCosApplyPolicyToIface(usp, dapi_g);

    hapiBroadQosSemGive(dapi_g);

    return result;
}

/*********************************************************************
*
* @purpose  Apply WRED configuration, stored in HAPI, to a port in HW
*
* @param   *usp     @b{(input)} Needs to be a valid usp
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments Sets a port to tail-dropping if WRED has not been 
*           configured for it. 
*           Called with QoS sem held.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadQosCosWredApply(DAPI_USP_t *usp)
{
    BROAD_PORT_t                   *hapiPortPtr, *hapiLagPortPtr;
    DAPI_PORT_t                    *dapiPortPtr;
    HAPI_BROAD_QOS_PORT_t          *qosPortPtr;
    usl_bcm_port_wred_config_t      parms;
    int                             colorIndex, rv;
    L7_uint32                       cosIndex;
    DAPI_USP_t                     *lagUsp;


    /* Just do nothing if this platform doesn't support WRED */
    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_FALSE) 
        return L7_SUCCESS;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    /* For WRED, set parameters as from the application, with CAP_AVERAGE set so 
       that it responds quickly to congestion going away. For taildrop, turn 
       off WRED chip support. */
    if (BROAD_PORT_IS_ACQUIRED(hapiPortPtr))
    {
        /* If this port is a LAG member, use the config values for the LAG. */
        lagUsp = &(hapiPortPtr->hapiModeparm.physical.lagUsp);
        hapiLagPortPtr = HAPI_PORT_GET(lagUsp, dapi_g);
        qosPortPtr = (HAPI_BROAD_QOS_PORT_t*)hapiLagPortPtr->qos;
    }
    parms.bcm_gport = (bcm_gport_t)(hapiPortPtr->bcmx_lport);
    for(cosIndex = 0; cosIndex < L7_MAX_CFG_QUEUES_PER_PORT; cosIndex++) 
    {                
        if (qosPortPtr->cos.dropType[cosIndex] == DAPI_QOS_COS_QUEUE_MGMT_TYPE_WRED) 
        {
            parms.flags[cosIndex] = BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_CAP_AVERAGE;

            /* PTin modified: QoS */
            parms.gain[cosIndex] = qosPortPtr->cos.wredExponent[cosIndex];

            for(colorIndex = 0; colorIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); colorIndex++) 
            {
                parms.minThreshold[cosIndex][colorIndex] = 
                    qosPortPtr->cos.perColorParams[cosIndex].wredMinThresh[colorIndex];
                parms.maxThreshold[cosIndex][colorIndex] = 
                    qosPortPtr->cos.perColorParams[cosIndex].wredMaxThresh[colorIndex];
                parms.dropProb[cosIndex][colorIndex] = 
                    qosPortPtr->cos.perColorParams[cosIndex].wredDropProb[colorIndex];
            }
        }
        else if (qosPortPtr->cos.dropType[cosIndex] == DAPI_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP) 
        {
	  /* For XGS4, we are just disabling WRED in-chip, so the parameters 
	     don't matter. If we want to support configurable tail-drop, 
	     add BCM_COSQ_DISCARD_ENABLE to the flags. */
            parms.flags[cosIndex] = 0; 
            parms.gain[cosIndex] = 0;
            for(colorIndex = 0; colorIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); colorIndex++) 
            {
                parms.minThreshold[cosIndex][colorIndex] = 0;
                parms.maxThreshold[cosIndex][colorIndex] = 
                    qosPortPtr->cos.perColorParams[cosIndex].taildropThresh[colorIndex];
                parms.dropProb[cosIndex][colorIndex] = 0;
            }
        }
        else
        {
            /* Something else, just disable chip WRED entirely */
            parms.flags[cosIndex] = 0;
            parms.gain[cosIndex] = 0;
            for(colorIndex = 0; colorIndex < (L7_MAX_CFG_DROP_PREC_LEVELS+1); colorIndex++) 
            {
                parms.minThreshold[cosIndex][colorIndex] = 0;
                parms.maxThreshold[cosIndex][colorIndex] = 100;
                parms.dropProb[cosIndex][colorIndex] = 0;
            }
        }
    } /* End for each queue */
    rv = usl_bcmx_port_wred_set(parms.bcm_gport, &parms);
    if (!L7_BCMX_OK(rv)) 
    {
        return L7_FAILURE;
    }
    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Configure COS interface-level parameters for the specified port
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_INTF_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.intfConfig
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This command affects the port egress queues.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_INTF_CONFIG */
L7_RC_t hapiBroadQosCosIntfConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                         result = L7_SUCCESS;
    DAPI_QOS_CMD_t                 *cmdCos = (DAPI_QOS_CMD_t*)data;
    BROAD_PORT_t                   *hapiPortPtr;
    DAPI_PORT_t                    *dapiPortPtr;
    HAPI_BROAD_QOS_PORT_t          *qosPortPtr;
    L7_RC_t                         tmpRc;
    L7_BOOL                         wredChanged = L7_FALSE;
    L7_uint8                        cosIndex;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    if (cmdCos->cmdData.intfConfig.getOrSet != DAPI_CMD_SET)
    {
        /* get interface config parameters from device */
        return L7_FAILURE;
    }

    hapiBroadQosSemTake(dapi_g);

    qosPortPtr->cos.intfShapingSpec = L7_TRUE;
    qosPortPtr->cos.intfShaping     = (L7_uint32)cmdCos->cmdData.intfConfig.intfShapingRate;
    /* Ignore queueMgmtTypePerIntf, these devices configure mgmt type per-queue */
    /* Ptin modified: QoS */
    for (cosIndex = 0; cosIndex < L7_MAX_CFG_QUEUES_PER_PORT; cosIndex++)
    {
      if (qosPortPtr->cos.wredExponent[cosIndex] != cmdCos->cmdData.intfConfig.wredDecayExponent) 
      {
          qosPortPtr->cos.wredExponent[cosIndex] = cmdCos->cmdData.intfConfig.wredDecayExponent;
          wredChanged = L7_TRUE;
      }
    }

    /* Apply the configuration to the interface. */

    if (BROAD_PORT_IS_LAG(hapiPortPtr))
    {
      int               i;
      DAPI_LAG_ENTRY_t *lagMemberSet;
      BROAD_PORT_t     *lagMemberPtr;

      lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

      /* apply CoS config to each LAG member */
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (L7_TRUE == lagMemberSet[i].inUse)
        {
          lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);

          tmpRc = hapiBroadQosCosIntfRateShape(lagMemberPtr, qosPortPtr->cos.intfShaping);
          if (L7_SUCCESS != tmpRc)
            result = tmpRc;
          if (wredChanged == L7_TRUE) 
          {
              tmpRc = hapiBroadQosCosWredApply(&lagMemberSet[i].usp);
              if (L7_SUCCESS != tmpRc)
                  result = tmpRc;
          }
        }
      }
    }
    else
    {
      tmpRc = hapiBroadQosCosIntfRateShape(hapiPortPtr, qosPortPtr->cos.intfShaping);
      if (L7_SUCCESS != tmpRc)
        result = tmpRc;
      if (wredChanged == L7_TRUE) 
      {
          tmpRc = hapiBroadQosCosWredApply(usp);
          if (L7_SUCCESS != tmpRc)
              result = tmpRc;
      }
    }

    hapiBroadQosSemGive(dapi_g);

    return result;
}

/*********************************************************************
*
* @purpose  Retrieve status about COS interface-level parameters for the specified port
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_INTF_STATUS
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.intfStatus
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_INTF_STATUS */
L7_RC_t hapiBroadQosCosIntfStatus(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                         result;
    DAPI_QOS_CMD_t                 *cmdCos = (DAPI_QOS_CMD_t*)data;
    BROAD_PORT_t                   *hapiPortPtr;
    DAPI_PORT_t                    *dapiPortPtr;
    int                             rv = BCM_E_FAIL;
    L7_uint32                       kbits_sec;
    L7_uint32                       kbits_burst;

    if (cmdCos->cmdData.intfStatus.getOrSet != DAPI_CMD_GET)
    {
        /* set interface status to device is invalid */
        return L7_FAILURE;
    }

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

    /* Apply the configuration to the interface. */

    if (BROAD_PORT_IS_LAG(hapiPortPtr))
    {
      int               i;
      DAPI_LAG_ENTRY_t *lagMemberSet;

      lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

      /* report CoS status on first found LAG member */
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (L7_TRUE == lagMemberSet[i].inUse)
        {
          hapiPortPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);

          rv = bcmx_port_rate_egress_get(hapiPortPtr->bcmx_lport, &kbits_sec, &kbits_burst);
          if (L7_BCMX_OK(rv) != L7_TRUE)
            result = L7_FAILURE;
          break;
        }
      }
    }
    else
    {
      rv = bcmx_port_rate_egress_get(hapiPortPtr->bcmx_lport, &kbits_sec, &kbits_burst);
    }

    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      result = L7_FAILURE;
    }
    else
    {
      cmdCos->cmdData.intfStatus.intfShapingRate = kbits_sec;
      cmdCos->cmdData.intfStatus.intfShapingBurstSize = kbits_burst;
      result = L7_SUCCESS;
    }

    return result;
}

/*********************************************************************
*
* @purpose  Configure COS queue scheduler parameters for the specified port
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.queueSchedCfg
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments This command affects the port egress queues.
*          if any BWs are 0, and the scheduler type is WFQ/WRR,
*            they will divide up any remaining bandwidth
*            after all the BW's with actual numbers are allocated. e.g.
*            If 3 queues want 10% each and 5 queues have 0, then those 5
*            queues will share the remaining 70% bandwidth evenly.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_QUEUE_SCHED_CONFIG */
L7_RC_t hapiBroadQosCosQueueSchedConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                     result = L7_SUCCESS;
  DAPI_QOS_CMD_t             *cmdCos = (DAPI_QOS_CMD_t*)data;
  BROAD_PORT_t               *hapiPortPtr;
  DAPI_PORT_t                *dapiPortPtr;
  HAPI_BROAD_QOS_PORT_t      *qosPortPtr;
  L7_uint32                   index;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

  if (cmdCos->cmdData.queueSchedConfig.getOrSet != DAPI_CMD_SET)
  {
    /* get queue config parameters from device */
    return L7_FAILURE;
  }

  hapiBroadQosSemTake(dapi_g);

  qosPortPtr->cos.queueConfigSpec = L7_TRUE;

  for (index = 0; index < L7_MAX_CFG_QUEUES_PER_PORT; index++)
  {
    qosPortPtr->cos.minBw[index]     = cmdCos->cmdData.queueSchedConfig.minBandwidth[index];
    qosPortPtr->cos.maxBw[index]     = cmdCos->cmdData.queueSchedConfig.maxBandwidth[index];
    qosPortPtr->cos.schedType[index] = cmdCos->cmdData.queueSchedConfig.schedulerType[index];
    qosPortPtr->cos.wrr_weights[index] = cmdCos->cmdData.queueSchedConfig.wrr_weight[index];    /* PTin added: QoS */
  }

  /* treat remaining queues, if any, as SP */
  for (index = L7_MAX_CFG_QUEUES_PER_PORT; index <= BCM_COS_MAX; index++)
  {
    qosPortPtr->cos.minBw[index]     = 0;
    qosPortPtr->cos.maxBw[index]     = 0;
    qosPortPtr->cos.schedType[index] = DAPI_QOS_COS_QUEUE_SCHED_TYPE_STRICT;
    qosPortPtr->cos.wrr_weights[index] = 0;     /* PTin added: QoS */
  }

  if (BROAD_PORT_IS_LAG(hapiPortPtr))
  {
    int               i;
    DAPI_LAG_ENTRY_t *lagMemberSet;
    BROAD_PORT_t     *lagMemberPtr;
    L7_RC_t           tmpRc;

    lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

    /* apply CoS config to each LAG member */
    for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (L7_TRUE == lagMemberSet[i].inUse)
      {
        lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);

        tmpRc = hapiBroadQosCosQueueConfig(lagMemberPtr, &qosPortPtr->cos);
        if (L7_SUCCESS != tmpRc)
          result = tmpRc;
      }
    }
  }
  else
  {
    if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_FALSE)
    {
      result = hapiBroadQosCosQueueConfig(hapiPortPtr, &qosPortPtr->cos);
      if (result == L7_FAILURE)
      {
        qosPortPtr->cos.queueConfigSpec = L7_FALSE;
      }
    }
  }

  hapiBroadQosSemGive(dapi_g);

  return result;
}

/*********************************************************************
*
* @purpose  Configure COS taildrop or WRED parameters for the specified queue
*
* @param   *usp     @b{(input)} Needs to be a valid usp
* @param    cmd     @b{(input)} DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG
* @param   *data    @b{(input)} DAPI_QOS_CMD_t.cmdData.queueDropCfg
* @param   *dapi_g  @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments    This command affects the egress queues on a physical port.
*
* @end
*
*********************************************************************/
/* DAPI_CMD_QOS_COS_QUEUE_DROP_CONFIG */
L7_RC_t hapiBroadQosCosQueueDropConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    L7_RC_t                     result = L7_SUCCESS, tmpRc = L7_SUCCESS;
    DAPI_QOS_CMD_t             *cmdCos = (DAPI_QOS_CMD_t*)data;
    BROAD_PORT_t               *hapiPortPtr;
    DAPI_PORT_t                *dapiPortPtr;
    HAPI_BROAD_QOS_PORT_t      *qosPortPtr;
    int                         colorIndex;
    L7_ulong32                  queueId;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    if (cmdCos->cmdData.queueDropConfig.getOrSet != DAPI_CMD_SET)
    {
        /* get queue WRED config parameters from device */
        return L7_FAILURE;
    }

    hapiBroadQosSemTake(dapi_g);

    for(queueId=0; queueId < L7_MAX_CFG_QUEUES_PER_PORT; queueId++) 
    {
        if (cmdCos->cmdData.queueDropConfig.parms[queueId].dropType == DAPI_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED) 
        {
            continue;
        }
        qosPortPtr->cos.dropType[queueId] = cmdCos->cmdData.queueDropConfig.parms[queueId].dropType;
        if (qosPortPtr->cos.dropType[queueId] == DAPI_QOS_COS_QUEUE_MGMT_TYPE_WRED) 
        {
            /* PTin added: QoS */
            qosPortPtr->cos.wredExponent[queueId] = cmdCos->cmdData.queueDropConfig.parms[queueId].wred_decayExponent;

            for(colorIndex=0; colorIndex<(L7_MAX_CFG_DROP_PREC_LEVELS+1); colorIndex++) 
            {
                qosPortPtr->cos.perColorParams[queueId].wredDropProb[colorIndex] = cmdCos->cmdData.queueDropConfig.parms[queueId].dropProb[colorIndex];
                qosPortPtr->cos.perColorParams[queueId].wredMinThresh[colorIndex] = cmdCos->cmdData.queueDropConfig.parms[queueId].minThreshold[colorIndex];
                qosPortPtr->cos.perColorParams[queueId].wredMaxThresh[colorIndex] = cmdCos->cmdData.queueDropConfig.parms[queueId].maxThreshold[colorIndex];
            }
        }
        else if (qosPortPtr->cos.dropType[queueId] == DAPI_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP) 
        {
            for(colorIndex=0; colorIndex<(L7_MAX_CFG_DROP_PREC_LEVELS+1); colorIndex++) 
            {
                qosPortPtr->cos.perColorParams[queueId].taildropThresh[colorIndex] = cmdCos->cmdData.queueDropConfig.parms[queueId].maxThreshold[colorIndex];
            }
        }
    }
    if (BROAD_PORT_IS_LAG(hapiPortPtr))
    {
      int               i;
      DAPI_LAG_ENTRY_t *lagMemberSet;

      lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

      /* apply CoS config to each LAG member */
      for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (L7_TRUE == lagMemberSet[i].inUse)
        {
              tmpRc = hapiBroadQosCosWredApply(&lagMemberSet[i].usp);
              if (L7_SUCCESS != tmpRc)
                  result = tmpRc;
        }
      }
    }
    else
    {
      result = hapiBroadQosCosWredApply(usp);
    }
    hapiBroadQosSemGive(dapi_g);

    return result;
}

/*********************************************************************
*
* @purpose  Notifies the COS component that a physical port has been
*           added to a LAG port
*
* @param   *portUsp         @b{(input)} USP of member port
* @param   *lagUsp          @b{(input)} USP of LAG port
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments   
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCosPortLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
    BROAD_PORT_t *hapiPortPtr;
    BROAD_PORT_t *hapiLagPtr;
    L7_RC_t rc;

    hapiPortPtr = HAPI_PORT_GET(portUsp, dapi_g);
    hapiLagPtr  = HAPI_PORT_GET(lagUsp,  dapi_g);

    /* apply LAG policy on the port */
    rc = hapiBroadQosCosApplyPolicy(hapiPortPtr, hapiLagPtr);
    if (rc != L7_SUCCESS) 
    {
        return rc;
    }
    /* Apply WRED config to port. hapiBroadQosCosWredApply() will figure out that it's a 
       LAG member now, and do the right thing. */
    rc = hapiBroadQosCosWredApply(portUsp);
    return rc;
}

/*********************************************************************
*
* @purpose  Notifies the COS component that a physical port has been
*           removed from a LAG port
*
* @param   *portUsp         @b{(input)} USP of member port
* @param   *lagUsp          @b{(input)} USP of LAG port
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCosPortLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
    BROAD_PORT_t *hapiPortPtr;
    L7_RC_t rc;

    hapiPortPtr = HAPI_PORT_GET(portUsp, dapi_g);

    /* restore port policy */
    rc = hapiBroadQosCosApplyPolicy(hapiPortPtr, hapiPortPtr);
    if (rc != L7_SUCCESS) 
    {
        return rc;
    }
    /* Restore taildrop/WRED config to the deleted port. The rest of the LAG will be fine. */
    rc = hapiBroadQosCosWredApply(portUsp);
    return rc;
}

/*********************************************************************
*
* @purpose  Notifies the COS component that a physical port has link.
*
* @param   *portUsp         @b{(input)} USP of member port
* @param   *dapi_g          @b{(input)} The driver object
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadQosCosPortLinkUpNotify(DAPI_USP_t *portUsp, DAPI_t *dapi_g)
{
    BROAD_PORT_t          *hapiPortPtr;
    HAPI_BROAD_QOS_PORT_t *qosPortPtr;
    L7_RC_t                result;

    hapiPortPtr = HAPI_PORT_GET(portUsp, dapi_g);
    qosPortPtr  = (HAPI_BROAD_QOS_PORT_t*)hapiPortPtr->qos;

    /* re-apply port policy to take into account possible link speed change */
    result = hapiBroadQosCosIntfRateShape(hapiPortPtr, qosPortPtr->cos.intfShaping);

    if ((result == L7_SUCCESS) && hapiBroadQosCosEgressBwSupported())
    {
      result = hapiBroadQosCosEgressBwConfig(hapiPortPtr, &qosPortPtr->cos);
    }

    return result;
}

