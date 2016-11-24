/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy.h
*
* @purpose   This file contains the Policy Manager interface used by HAPI
*            to access Classifier (FFP/CAP) resources. The API is defined
*            to be hardware independent so it can work on a variety of
*            platforms.
*
* @component hapi
*
* @comments  Policies can have zero or more rules and be applied to multiple
*            ports. By default SYSTEM policies apply to all Ethernet (non-
*            stacking) ports. All other policies do not apply to any ports
*            by default; instead, these policies must be selectively applied
*            and removed from ports. LAG support requires that the caller
*            individually apply/remove the policy from each physical port as
*            it is acquired/released.
*
*            Rules can have zero or more fields. Unspecified fields default
*            to "don't care". Specifying no fields means "match all". Rules
*            can have zero or more actions. No action implies "no-op" and
*            prevents subsequent rules from being evaluated.
*
*            General usage:
*            1) create a policy using hapiBroadPolicyCreate(...)
*            2) add a rule to the policy using hapiBroadPolicyRuleAdd(...)
*            3) add zero or more fields to the rule using hapiBroadPolicyRuleQualifierAdd(...)
*            4) add zero or more actions to the rule using hapiBroadPolicyRuleActionAdd(...)
*            5) add zero or one meters/counters to the rule using either
                hapiBroadPolicyRuleMeterAdd(...) or hapiBroadPolicyRuleCounterAdd(...)
*            6) repeat steps 2-5 for each rule
*            7) complete the policy using hapiBroadPolicyCommit(...)
*            8) selectively add or remove ports using hapiBroadPolicyApplyToIface(...)
*
*            Notes:
*            1) New rules can be added to policies after they have been committed. However,
*               the changes will not take place until the policy is re-committed and
*               re-applied.
*            2) DO NOT CHANGE EXISTING RULES without explicitly deleting and recreating the
*               policy. Otherwise, the changed rules will not take effect.
*            3) If multiple policies apply to an interface then they are evaluated in order
*               of precedence. This is not recommended for PORT-specific policies as the
*               order of rules in the same policy are deterministic, but between policies
*               of the same type is non-deterministic.
*
* @create    3/18/2005
*
* @author    robp
*
* @end
*
**********************************************************************/

#ifndef BROAD_POLICY_H
#define BROAD_POLICY_H

#include "broad_policy_common.h"

/*********************************************************************
*
* @purpose Initialize the Policy manager.
*
* @param   none
*
* @returns L7_RC_t
*
* @notes   Called once at IPL.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyInit();

/*********************************************************************
*
* @purpose Reinitialize the policy manager and underlying layers.
*
* @param   none
*
* @returns L7_RC_t
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyReInit();

/*********************************************************************
*
* @purpose Creates a new policy intended to be managed by a single user.
*
* @param   BROAD_POLICY_TYPE_t type - determines characteristics of policy
* @param   BROAD_POLICY_t *policy   - returned policy identifier
*
* @returns L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyCreate(BROAD_POLICY_TYPE_t  type,
                              BROAD_POLICY_t      *policy);

/*********************************************************************
*
* @purpose Designates which stage the policy should be applied to.
*          This function should be called before the policy is committed.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStageSet(BROAD_POLICY_t policy, BROAD_POLICY_STAGE_t policyStage);

/*********************************************************************
*
* @purpose Returns which stage the policy is applied to.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStageGet(BROAD_POLICY_t policy, BROAD_POLICY_STAGE_t *policyStage);

/*********************************************************************
*
* @purpose Deletes an existing policy which removes it completely from
*          the system, i.e. the policy no longer applies to any ports.
*
* @param   BROAD_POLICY_t policy   - policy to delete
*
* @returns L7_RC_t
*
* @notes   All policies must be explicitly deleted when they are no
*          longer needed. No policies are automatically cleaned-up
*          on "clear config" or other related mgt. activity.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyDelete(BROAD_POLICY_t policy);

/*********************************************************************
*
* @purpose Adds a new rule to an existing policy. Rules are added in
*          order of precedence, highest first.
*
* @param   BROAD_POLICY_t policy     - policy to change
* @param   BROAD_POLICY_RULE_t *rule - returned rule identifier
*
* @returns L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleAdd(BROAD_POLICY_t       policy,
                               BROAD_POLICY_RULE_t *rule);

/*********************************************************************
*
* @purpose Adds a new rule to an existing policy with the specified
*          priority value.
*
* @param   BROAD_POLICY_t policy                  - policy to change
* @param   BROAD_POLICY_RULE_t *rule              - returned rule identifier
* @param   BROAD_POLICY_RULE_PRIORITY_t priority  - rule priority (within group)
*
* @returns L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyPriorityRuleAdd(BROAD_POLICY_t       policy,
                                       BROAD_POLICY_RULE_t *rule,
                                       BROAD_POLICY_RULE_PRIORITY_t priority);

/*********************************************************************
*
* @purpose Copies an existing rule, adding it to the end of the policy.
*          All rule criteria (fields, actions, meters, ...) are copied.
*
* @param   BROAD_POLICY_t policy        - policy to change
* @param   BROAD_POLICY_RULE_t *OldRule - rule to copy
* @param   BROAD_POLICY_RULE_t *newRule - returned rule identifier
*
* @returns L7_RC_t
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleCopy(BROAD_POLICY_t       policy,
                                BROAD_POLICY_RULE_t  oldRule,
                                BROAD_POLICY_RULE_t *newRule);

/*********************************************************************
*
* @purpose Deletes an existing rule.
*
* @param   BROAD_POLICY_t policy    - policy to change
* @param   BROAD_POLICY_RULE_t rule - rule to delete
*
* @returns L7_RC_t
*
* @notes   This function is currently limited to rolling-back the
*          last rule in case of an error.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleDelete(BROAD_POLICY_t      policy,
                                  BROAD_POLICY_RULE_t rule);

/*********************************************************************
*
* @purpose Add qualification criteria to an existing rule. A rule can
*          qualify on multiple fields by calling this function multiple
*          times. A particular field should be specified only once.
*
* @param   BROAD_POLICY_t policy      - policy to change
* @param   BROAD_POLICY_RULE_t rule   - rule to change
* @param   BROAD_POLICY_FIELD_t field - field to qualify on
* @param   L7_uchar8 *value           - value array
* @param   L7_uchar8 *mask            - mask array
*
* @returns L7_RC_t
*
* @notes   The value and mask array must be the same size as the field
*          width, in bytes.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleQualifierAdd(BROAD_POLICY_t       policy,
                                        BROAD_POLICY_RULE_t  rule,
                                        BROAD_POLICY_FIELD_t field,
                                        L7_uchar8*           value,
                                        L7_uchar8*           mask);

/*********************************************************************
*
* @purpose Add actions to be taken when a rule matches a packet. Multiple
*          actions can be specified by calling this function multiple
*          times. A particular action should be specified only once.
*
* @param   BROAD_POLICY_t policy        - policy to change
* @param   BROAD_POLICY_RULE_t rule     - rule to change
* @param   BROAD_POLICY_ACTION_t action - action to take
* @param   L7_uint32 param0             - param0 (optional for some actions)
* @param   L7_uint32 param1             - param1 (optional for some actions)
* @param   L7_uint32 param2             - param2 (optional for some actions)
*
* @returns L7_RC_t
*
* @notes   Each action has associated params that must be specified. Refer
*          to the list of actions for the params. For example, the DROP
*          action does not require any params, whereas, REDIRECT requires
*          the egress port.
*
*          This API applies to non-metered or GREEN traffic. To specify
*          actions for metered traffic use the Exceed and NonConf APIs.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleActionAdd(BROAD_POLICY_t          policy,
                                     BROAD_POLICY_RULE_t     rule,
                                     BROAD_POLICY_ACTION_t   action,
                                     L7_uint32               param0,
                                     L7_uint32               param1,
                                     L7_uint32               param2);

/*********************************************************************
*
* @purpose Add a meter to police ingress traffic matching the given
*          rule. Only one meter can be associated with a rule. Use
*          the Exceed and NonConf APIs to define actions for each
*          color packet. Rules with meters automatically get a
*          counter.
*
* @param   BROAD_POLICY_t policy          - policy to change
* @param   BROAD_POLICY_RULE_t rule       - rule to change
* @param   BROAD_METER_ENTRY_t *meterInfo - metering rates (cir,cbs,pir,pbs)
*
* @returns L7_RC_t
*
* @notes   Meters and counters are mutually exclusive.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleMeterAdd(BROAD_POLICY_t          policy,
                                    BROAD_POLICY_RULE_t     rule,
                                    BROAD_METER_ENTRY_t    *meterInfo);

/*********************************************************************
*
* @purpose Add a counter to the rule to count the number of packets
*          that match.
*
* @param   BROAD_POLICY_t policy          - policy to change
* @param   BROAD_POLICY_RULE_t rule       - rule to change
* @param   BROAD_COUNTER_MODE_t           - packet or byte
* @param   bcm_port_t port                - port for ffp Counters
*
* @returns L7_RC_t
*
* @notes   Meters and counters are mutually exclusive.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleCounterAdd(BROAD_POLICY_t       policy,
                                      BROAD_POLICY_RULE_t  rule,
                                      BROAD_COUNTER_MODE_t mode,
                                      bcm_port_t           port);

/*********************************************************************
*
* @purpose Retrieve stats for all rules in a policy, regardless of
*          whether they are based upon meter or counter.
*
* @param   BROAD_POLICY_t policy           - policy to query
* @param   L7_uint32 *                     - number of stats (in/out)
* @param   BROAD_POLICY_STATS_t *          - stats array
*
* @returns L7_RC_t
*
* @notes   Returns counter stats for all ports to which this policy
*          applies.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStatsGet(BROAD_POLICY_t        policy,
                                L7_uint32            *numStats,
                                BROAD_POLICY_STATS_t *stats);

/*********************************************************************
*
* @purpose Add actions to be taken for a rule for exceeding traffic,
*          as determined by the meter.
*
* @param   BROAD_POLICY_t policy        - policy to change
* @param   BROAD_POLICY_RULE_t rule     - rule to change
* @param   BROAD_POLICY_ACTION_t action - action to take
* @param   L7_uint32 param0             - param0 (optional for some actions)
* @param   L7_uint32 param1             - param1 (optional for some actions)
* @param   L7_uint32 param2             - param2 (optional for some actions)
*
* @returns L7_RC_t
*
* @notes   This API applies only to exceeding, or YELLOW, traffic.
*
*          Not all actions are valid for exceeding traffic on all platforms.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleExceedActionAdd(BROAD_POLICY_t          policy,
                                           BROAD_POLICY_RULE_t     rule,
                                           BROAD_POLICY_ACTION_t   action,
                                           L7_uint32               param0,
                                           L7_uint32               param1,
                                           L7_uint32               param2);

/*********************************************************************
*
* @purpose Get actions to be taken for a rule for exceeding traffic,
*          as determined by the meter.
*
* @param   BROAD_POLICY_t policy        - policy to change
* @param   BROAD_POLICY_RULE_t rule     - rule to change
* @param   BROAD_POLICY_ACTION_t action - action to take
* @param   L7_uint32 param0             - param0 (optional for some actions)
* @param   L7_uint32 param1             - param1 (optional for some actions)
* @param   L7_uint32 param2             - param2 (optional for some actions)
*
* @returns L7_RC_t
*
* @notes   This API applies only to exceeding, or YELLOW, traffic.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleExceedActionGet(BROAD_POLICY_t          policy,
                                           BROAD_POLICY_RULE_t     rule,
                                           BROAD_POLICY_ACTION_t  *action,
                                           L7_uint32              *param0,
                                           L7_uint32              *param1,
                                           L7_uint32              *param2);

/*********************************************************************
*
* @purpose Add actions to be taken for a rule for non-conforming traffic,
*          as determined by the meter.
*
* @param   BROAD_POLICY_t policy        - policy to change
* @param   BROAD_POLICY_RULE_t rule     - rule to change
* @param   BROAD_POLICY_ACTION_t action - action to take
* @param   L7_uint32 param0             - param0 (optional for some actions)
* @param   L7_uint32 param1             - param1 (optional for some actions)
* @param   L7_uint32 param2             - param2 (optional for some actions)
*
* @returns L7_RC_t
*
* @notes   This API applies only to non-conf, or RED, traffic.
*
*          Not all actions are valid for non-conf traffic on all platforms.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleNonConfActionAdd(BROAD_POLICY_t          policy,
                                            BROAD_POLICY_RULE_t     rule,
                                            BROAD_POLICY_ACTION_t   action,
                                            L7_uint32               param0,
                                            L7_uint32               param1,
                                            L7_uint32               param2);

/*********************************************************************
*
* @purpose Get actions to be taken for a rule for non-conforming traffic,
*          as determined by the meter.
*
* @param   BROAD_POLICY_t policy        - policy to change
* @param   BROAD_POLICY_RULE_t rule     - rule to change
* @param   BROAD_POLICY_ACTION_t action - action to take
* @param   L7_uint32 param0             - param0 (optional for some actions)
* @param   L7_uint32 param1             - param1 (optional for some actions)
* @param   L7_uint32 param2             - param2 (optional for some actions)
*
* @returns L7_RC_t
*
* @notes   This API applies only to non-conf, or RED, traffic.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleNonConfActionGet(BROAD_POLICY_t          policy,
                                            BROAD_POLICY_RULE_t     rule,
                                            BROAD_POLICY_ACTION_t  *action,
                                            L7_uint32              *param0,
                                            L7_uint32              *param1,
                                            L7_uint32              *param2);

/*********************************************************************
*
* @purpose Clear all actions (conf, exceed, non-conf) associated with
*          the specified rule. It does not affect meters or counters.
*
* @param   BROAD_POLICY_t policy        - policy to change
* @param   BROAD_POLICY_RULE_t rule     - rule to change
*
* @returns L7_RC_t
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleActionClearAll(BROAD_POLICY_t        policy,
                                          BROAD_POLICY_RULE_t   rule);

/*********************************************************************
*
* @purpose Commit policy to hardware after all rules have been defined.
*          This step is mandatory for all policies as it updates any
*          info that is dynamic in nature and downloads the policies
*          to affected units.       
*
* @param   BROAD_POLICY_t policy        - policy to commit
*
* @returns L7_RC_t
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyCommit(BROAD_POLICY_t policy);

/*********************************************************************
*
* @purpose Apply a policy to a port after it has been committed.
*
* @param   BROAD_POLICY_t policy   - policy to apply
* @param   bcmx_lport_t lport      - affected port
*
* @returns L7_RC_t
*
* @notes   A policy can be applied to multiple ports individually.
*          This API is not intended for SYSTEM policies.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyApplyToIface(BROAD_POLICY_t policy,
                                    bcmx_lport_t   lport);

/*********************************************************************
*
* @purpose Check the status of a policy wrt the given interface.
*
* @param   BROAD_POLICY_t policy   - policy to apply
* @param   bcmx_lport_t lport      - affected port
*
* @returns L7_ALREADY_CONFIGURED - if policy is already assigned to port
*          L7_NOT_EXIST          - if policy is not configured on port
*
* @notes   A policy can be applied to multiple ports individually.
*          This API is not intended for SYSTEM policies.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyIfaceCheck(BROAD_POLICY_t policy,
                                  bcmx_lport_t   lport);

/*********************************************************************
*
* @purpose Apply a policy to all Ethernet ports.
*
* @param   BROAD_POLICY_t policy   - policy to apply
*
* @returns L7_RC_t
*
* @notes   Same as individually adding all ports, only faster.
*          This API can be used for SYSTEM policies.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyApplyToAll(BROAD_POLICY_t policy);

/*********************************************************************
*
* @purpose Remove a policy from a port.
*
* @param   BROAD_POLICY_t policy   - policy to remove
* @param   bcmx_lport_t lport      - affected port
*
* @returns L7_RC_t
*
* @notes   A policy can be removed from multiple ports individually.
*          This API is not intended for SYSTEM policies.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRemoveFromIface(BROAD_POLICY_t policy,
                                       bcmx_lport_t   lport);

/*********************************************************************
*
* @purpose Remove a policy from all Ethernet ports..
*
* @param   BROAD_POLICY_t policy   - policy to remove
* @param   bcmx_lport_t lport      - affected port
*
* @returns L7_RC_t
*
* @notes   Same as individually removing all ports, only faster.
*          This API can be used for SYSTEM policies.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRemoveFromAll(BROAD_POLICY_t policy);


/*********************************************************************
*
* @purpose Specify whether policy applies to FPS ports in addition to
*          all  Ethernet ports.
* @end
*********************************************************************/
L7_RC_t hapiBroadPolicyEnableFPS(BROAD_POLICY_t policy);

#endif /* BROAD_POLICY_H */
