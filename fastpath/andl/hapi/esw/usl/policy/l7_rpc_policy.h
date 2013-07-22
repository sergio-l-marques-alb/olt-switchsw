/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  l7_rpc_policy.h
*
* @purpose   This file implements the Policy Manager RPC routines.
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
#ifndef BROAD_L7_RPC_POLICY_H
#define BROAD_L7_RPC_POLICY_H

#include "bcmx/types.h"
#include "broad_policy_types.h"
#include "hpc_hw_api.h"

/* Policy Custom Types */

typedef enum
{
    BROAD_CUSTOM_POLICY_CREATE,     /* SET */
    BROAD_CUSTOM_POLICY_DESTROY,    /* SET */
    BROAD_CUSTOM_POLICY_APPLY,      /* SET */
    BROAD_CUSTOM_POLICY_APPLY_ALL,  /* SET */
    BROAD_CUSTOM_POLICY_REMOVE,     /* SET */
    BROAD_CUSTOM_POLICY_REMOVE_ALL, /* SET */
    BROAD_CUSTOM_POLICY_STATS,      /* GET */
    BROAD_CUSTOM_POLICY_RULE_STATUS_SET, /* SET */
}
BROAD_POLICY_CUSTOM_CMD_t;

#define BROAD_POLICY_NONE       0
#define BROAD_POLICY_FIRST      1    /* for fragmenting policy messages */
#define BROAD_POLICY_MIDDLE     2    /* for fragmenting policy messages */
#define BROAD_POLICY_LAST       4    /* for fragmenting policy messages */
#define BROAD_POLICY_COMPRESSED 8    /* for compressing policy data */

typedef struct
{
    BROAD_POLICY_CUSTOM_CMD_t    policyCmd;
    int                          policyFlags;
    BROAD_POLICY_t               policyId;
    L7_ushort16                  policyRuleStatus; /* rule status to activate/deactivate ACL rule*/
    int                          ruleId;
    union
    {
        BROAD_POLICY_ENTRY_t     policyInfo; /* BROAD_CUSTOM_POLICY_CREATE */
    }
    cmdData;
}
BROAD_POLICY_CUSTOM_DATA_t;

typedef struct
{
  int rpcFlags;
  int rpcRulesIncluded;
  union
  {
    struct
    {
      BROAD_POLICY_CUSTOM_CMD_t policyCmd;
      BROAD_POLICY_t            policyId;
      BROAD_POLICY_t            ruleId;
      L7_ushort16               policyRuleStatus; /* rule status to activate/deactivate ACL rule*/
      int                       policySize;
      BROAD_POLICY_TYPE_t       policyType;
      int                       policyFlags;
      L7_BOOL                   strictEthTypes;
      BROAD_POLICY_STAGE_t      policyStage;
      int                       ruleCount;
    } policy;
    struct
    {
      BROAD_POLICY_RULE_ENTRY_t firstRule;
    } rules;
  } rpcData;
}
BROAD_POLICY_RPC_DATA_t;

int custom_policy_init();

L7_BOOL l7_rpc_server_port_policy_handler(int unit, bcm_port_t port, int type,
                                          int setget, uint32 *args, int *rv);

/*********************************************************************
* @purpose Initialize Policy RPC 
*
* @params  none
*
* @returns none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t l7_init_policy_rpc(void);

/*********************************************************************
* @purpose  Create a Policy
*
* @param     policy      @{(input)} the policy ID that is created
* @param    *policyInfo  @{(input)} the policy info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_create(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Destroy a Policy
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_destroy(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_apply_all(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Remove a policy from all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_remove_all(BROAD_POLICY_t policy);

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
int l7_rpc_client_policy_port_apply(BROAD_POLICY_t policyId,
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
int l7_rpc_client_policy_port_remove(BROAD_POLICY_t policyId,
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
int l7_rpc_client_policy_stats_get(BROAD_POLICY_t        policyId,
                                   L7_uint32             ruleId,
                                   BROAD_POLICY_STATS_t *stats);

/*********************************************************************
* @purpose  Create a Policy
*
* @param     policy      @{(input)} the policy ID that is created
* @param    *policyInfo  @{(input)} the policy info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_create(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo);

/*********************************************************************
* @purpose  Destroy a Policy
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_destroy(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_apply_all(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Remove a policy from all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_remove_all(BROAD_POLICY_t policy);


/*********************************************************************
* @purpose  Apply a policy to a port
*
* @param    unit        @{(input)} bcm unit
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_port_apply(L7_uint32      unit, 
                             BROAD_POLICY_t policyId,
                             bcm_port_t     port);

/*********************************************************************
* @purpose  Remove a policy from a port
*
* @param    unit        @{(input)} bcm unit
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_port_remove(L7_uint32      unit, 
                              BROAD_POLICY_t policyId,
                              bcm_port_t     port);

/*********************************************************************
* @purpose  Retrieve statistics for a policy
*
* @param    policy      @{(input)} policy ID
* @param    ruleId      @{(input)} rule ID
* @param    rpc_resp    @{(output)} response buffer
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_stats_get(BROAD_POLICY_t  policyId,
                             L7_uint32       ruleId,
                             hpcHwRpcData_t *rpc_resp);

/*********************************************************************
* @purpose  Clear counters for a rule in a policy
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_counter_clear(BROAD_POLICY_t policy);

/*********************************************************************
* @purpose  Updates a specific rule in a specific policy
*           w/ the new modid and modport of the new manager's 
*           CPU port.
*
* @params   policy      {(input)} policy
*           rule        {(input)} rule
*           cpu_modid   {(input)} modid of new CPU port. 
*           cpu_modport {(input)} modport of new CPU. 
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void usl_bcm_policy_dataplane_cleanup(L7_uint32 policy, L7_uint32 rule, L7_uint32 cpu_modid, L7_uint32 cpu_modport);
/*********************************************************************
* @purpose   Set rule status for policy Rule ID. 
*
* @param    BROAD_POLICY_t        policy
* @param    BROAD_PORT_RULE_t     rule
* @param    BROAD_RULE_STATUS_t   status
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_rule_status_set(BROAD_POLICY_t policy, 
                                         BROAD_POLICY_RULE_t rule,
                                         BROAD_RULE_STATUS_t status);
/*********************************************************************
* @purpose  Activate deactivate the rule based on rule status
*
* @param    policy      @{(input)} policy ID
* @param    rule        @{(input)} rule
* @param    status      @{(input)} rule status
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_rule_status_set(BROAD_POLICY_t      policyId,
                                   BROAD_POLICY_RULE_t rule,
                                   BROAD_RULE_STATUS_t status);

void rpc_policy_debug(L7_uint32 setting);

#endif /* BROAD_L7_RPC_POLICY_H */
