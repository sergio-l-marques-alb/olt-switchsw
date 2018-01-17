/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  l7_usl_bcmx_policy.h
*
* @purpose   This file implements the Policy Manager USL BCMX routines.
*
* @component hapi
*
* @create    12/12/08
*
* @author    colinw
*
* @end
*
**********************************************************************/
#ifndef BROAD_L7_USL_BCMX_POLICY_H
#define BROAD_L7_USL_BCMX_POLICY_H

#include "broad_policy_common.h"


/*********************************************************************
* @purpose  Get information about a Policy
*
* @param     policy      @{(input)}  policy ID
* @param    *policyInfo  @{(output)} the policy info
*
* @notes    It is up to the caller to free all nodes allocated for the
*           rule info. hapiBroadPolicyRulesPurge() can be used for this.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_info_get(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Create a Policy in the HW's table
*
* @param    *policy      @{(output)} the policy ID that is created
* @param    *policyInfo  @{(output)} the policy info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_create(BROAD_POLICY_t *policy, BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Recommit a Policy in the HW's table
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_recommit(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Destroy a Policy in the HW's table
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_destroy(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_apply_all(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Remove a policy from all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_remove_all(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Apply a policy to a port
*
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_port_apply(BROAD_POLICY_t policy,
                               bcmx_lport_t   port);

/*********************************************************************
* @purpose  Remove a policy from a port
*
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_port_remove(BROAD_POLICY_t policy,
                                bcmx_lport_t   port);

/*********************************************************************
* @purpose  Retrieve statistics for a policy
*
* @param    policy      @{(input)} policy ID
* @param    ruleId      @{(input)} rule ID to get stats for
* @param    stats       @{(output)} statistics
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_stats_get(BROAD_POLICY_t        policy,
                              L7_uint32             ruleId,
                              BROAD_POLICY_STATS_t *stats);

/*********************************************************************
* @purpose  Set status for policy Rule ID
* @param    BROAD_POLICY_t       policy
* @param    BROAD_PORT_RULE_t    rule
* @param    BROAD_RULE_STATUS_t  status
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_rule_status_set(BROAD_POLICY_t policy,
                               BROAD_POLICY_RULE_t rule, BROAD_RULE_STATUS_t status);

#endif /* BROAD_L7_USL_BCMX_POLICY_H */
