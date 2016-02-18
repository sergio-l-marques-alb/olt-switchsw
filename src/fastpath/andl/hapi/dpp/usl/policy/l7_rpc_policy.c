/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  l7_rpc_policy.c
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
#include "l7_common.h"
#include "dapi.h"
#include "broad_policy_types.h"
#include "broad_group_bcm.h"
#include "hpc_hw_api.h"
#include "l7_rpc_policy.h"
#include "l7_usl_policy_db.h"
#include "l7_usl_bcmx_port.h"
#include "l7_usl_sm.h"
#include "l7_usl_port_db.h"

#include "bcm/error.h"
#include "bcm/custom.h"
#include "bcmx/custom.h"
#include "bcmx/types.h"
#include "bcmx/bcmx_int.h"
#include "ibde.h"
#include "zlib.h"

#define L7_RPC_POLICY_HANDLER HPC_RPC_FUNCTION_ID(L7_DRIVER_COMPONENT_ID,99)

static BROAD_POLICY_CUSTOM_DATA_t *rpc_policy_data;
static L7_uint32 rpc_policy_rule_count;
static L7_BOOL rpc_debug = L7_FALSE;
static hpcHwRpcData_t rpcStatsResponse[L7_MAX_UNITS_PER_STACK+1];

#define RPC_DEBUG_PRINT if (rpc_debug != L7_FALSE) sysapiPrintf


/* server functions */
static int usl_bcm_policy_handle_cmd(BROAD_POLICY_CUSTOM_DATA_t *policyHdr, hpcHwRpcData_t *rpc_resp)
{
  int rv;

  switch (policyHdr->policyCmd)
  {
  case BROAD_CUSTOM_POLICY_CREATE:
    rv = usl_bcm_policy_create(policyHdr->policyId, &policyHdr->cmdData.policyInfo);
    break;
  case BROAD_CUSTOM_POLICY_DESTROY:
    rv = usl_bcm_policy_destroy(policyHdr->policyId);
    break;
  case BROAD_CUSTOM_POLICY_APPLY_ALL:
    rv = usl_bcm_policy_apply_all(policyHdr->policyId);
    break;
  case BROAD_CUSTOM_POLICY_REMOVE_ALL:
    rv = usl_bcm_policy_remove_all(policyHdr->policyId);
    break;
  case BROAD_CUSTOM_POLICY_STATS:
    rv = usl_bcm_policy_stats_get(policyHdr->policyId, policyHdr->ruleId, rpc_resp);
    break;
  case BROAD_CUSTOM_POLICY_RULE_STATUS_SET:
    rv = usl_bcm_policy_rule_status_set(policyHdr->policyId, policyHdr->ruleId, policyHdr->policyRuleStatus);
    break;
  default:
    rv = BCM_E_PARAM;
    break;
  }

  return rv;
}

L7_RC_t l7_rpc_server_policy_handler(L7_uint32 tid, 
                                     hpcHwRpcData_t *rpc_data,
                                     L7_int32 *status,
                                     hpcHwRpcData_t  *rpc_resp)
{
  int                        rv = BCM_E_NONE;
  BROAD_POLICY_RPC_DATA_t   *data;
  BROAD_POLICY_RULE_ENTRY_t *lastRulePtr, *newRulePtr, *tmpRulePtr;
  L7_uint32                  i;

  if (rpc_data->data_len < sizeof(BROAD_POLICY_RPC_DATA_t))
  {
    *status = BCM_E_FAIL;
    return L7_SUCCESS;
  }

  data = (BROAD_POLICY_RPC_DATA_t *)rpc_data->buf;

  if (data->rpcFlags & BROAD_POLICY_FIRST)
  {
    if ((rpc_policy_data = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_POLICY_CUSTOM_DATA_t))) == L7_NULL)
    {
      *status = BCM_E_FAIL;
      return L7_SUCCESS;
    }

    rpc_policy_rule_count = 0;

    rpc_policy_data->policyFlags = data->rpcFlags;
    
    rpc_policy_data->policyCmd = data->rpcData.policy.policyCmd;
    rpc_policy_data->policyId = data->rpcData.policy.policyId;
    rpc_policy_data->ruleId   = data->rpcData.policy.ruleId;
    rpc_policy_data->cmdData.policyInfo.ruleCount = data->rpcData.policy.ruleCount;
    rpc_policy_data->cmdData.policyInfo.policyFlags = data->rpcData.policy.policyFlags;
    rpc_policy_data->cmdData.policyInfo.strictEthTypes = data->rpcData.policy.strictEthTypes;
    rpc_policy_data->cmdData.policyInfo.policyType = data->rpcData.policy.policyType;
    rpc_policy_data->cmdData.policyInfo.policyStage = data->rpcData.policy.policyStage;
    /* PTin added: policers and counters */
    #if 1
    rpc_policy_data->cmdData.policyInfo.general_policer_id = data->rpcData.policy.policer_id;
    rpc_policy_data->cmdData.policyInfo.general_counter_id = data->rpcData.policy.counter_id;
    #endif
    /* set the policy Rule  and status information to rpc_policy_data only if policy cmd type is 
       BROAD_CUSTOM_POLICY_RULE_STATUS_SET
    */
    if(data->rpcData.policy.policyCmd == BROAD_CUSTOM_POLICY_RULE_STATUS_SET)
    {
       rpc_policy_data->policyRuleStatus = data->rpcData.policy.policyRuleStatus; /* rule status to activate/deactivate the rule */
    }
    RPC_DEBUG_PRINT("l7_rpc_server_policy_handler: Policy-%d received FIRST with command %d\r\n", 
                    rpc_policy_data->policyId, rpc_policy_data->policyCmd);
  }

  if ((data->rpcFlags & BROAD_POLICY_MIDDLE) ||
      ((data->rpcFlags & BROAD_POLICY_LAST) && 
       (data->rpcRulesIncluded > 0)))
  {
    if (rpc_policy_data != L7_NULL)
    {
      RPC_DEBUG_PRINT("l7_rpc_server_policy_handler: Policy-%d received MIDDLE with %d rule(s)\r\n", 
                      rpc_policy_data->policyId, data->rpcRulesIncluded);

      /* Find the last rule in the linked list. */
      lastRulePtr = rpc_policy_data->cmdData.policyInfo.ruleInfo;
      if (lastRulePtr != L7_NULL)
      {
        while (lastRulePtr->next != L7_NULL)
        {
          lastRulePtr = lastRulePtr->next;
        }
      }
      for (i = 0; i < data->rpcRulesIncluded; i++)
      {
        /* Allocate a new rule node. */
        newRulePtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(*newRulePtr));
        if (newRulePtr == L7_NULL)
        {
          RPC_DEBUG_PRINT("l7_rpc_server_policy_handler: Policy-%d Insufficient memory for rule %d\n",
                          rpc_policy_data->policyId, i);
          *status = BCM_E_FAIL; 
          return L7_SUCCESS;
        }

        /* Append the new rule node to the end of the linked list. */
        if (lastRulePtr == L7_NULL)
        {
          rpc_policy_data->cmdData.policyInfo.ruleInfo = newRulePtr;
        }
        else
        {
          lastRulePtr->next = newRulePtr;
        }
        lastRulePtr = newRulePtr;

        /* Copy the rule info from the RPC message. */
        tmpRulePtr = &(((BROAD_POLICY_RULE_ENTRY_t *)(&data->rpcData.rules.firstRule))[i]);
        memcpy(newRulePtr, 
               tmpRulePtr,
               sizeof(BROAD_POLICY_RULE_ENTRY_t));

        newRulePtr->next = L7_NULL;
      }

      rpc_policy_rule_count += data->rpcRulesIncluded;
    }
    else
    {
      *status = BCM_E_FAIL; 
      return L7_SUCCESS;
    }
  }

  if (data->rpcFlags & BROAD_POLICY_LAST)
  {
    RPC_DEBUG_PRINT("l7_rpc_server_policy_handler: Policy-%d received LAST got %d rule(s) total\r\n", 
                    rpc_policy_data->policyId, rpc_policy_rule_count);
    if (rpc_policy_data != L7_NULL)
    {
      rv = usl_bcm_policy_handle_cmd(rpc_policy_data, rpc_resp);

      hapiBroadPolicyRulesPurge(&rpc_policy_data->cmdData.policyInfo);

      osapiFree(L7_DRIVER_COMPONENT_ID, rpc_policy_data);
      rpc_policy_data = L7_NULL;
    }
    else
    {
      *status = BCM_E_FAIL;
      return L7_SUCCESS;
    }
  }

  *status = rv;
  return L7_SUCCESS;
}

int custom_policy_init()
{
  int rv;

  rv = L7_SUCCESS;

  #if 0
  rv = l7_bcm_policy_init();
  #endif

  return rv;
}


int usl_bcm_policy_port_set_handler(int unit, bcm_port_t port, 
                                    int setget, uint32 *args)
{
  int                         rv = BCM_E_NONE;
  BROAD_POLICY_CUSTOM_DATA_t *policyHdr;

  policyHdr = (BROAD_POLICY_CUSTOM_DATA_t *)args;

  switch (policyHdr->policyCmd)
  {
  case BROAD_CUSTOM_POLICY_APPLY:
    rv = usl_bcm_policy_port_apply(unit, policyHdr->policyId, port);
    break;
  case BROAD_CUSTOM_POLICY_REMOVE:
    rv = usl_bcm_policy_port_remove(unit, policyHdr->policyId, port);
    break;
  default:
    rv = BCM_E_PARAM;
    break;
  }

  return rv;
}

L7_BOOL l7_rpc_server_port_policy_handler(int unit, bcm_port_t port, int type,
                                          int setget, uint32 *args, int *rv)
{
  L7_BOOL handled = L7_TRUE;

  switch (type)
  {
    case USL_BCMX_POLICY_SET_HANDLER:
      *rv = usl_bcm_policy_port_set_handler (unit, port, setget, args);
      break;

     default: 
       handled = L7_FALSE;
       break;
    }

  return handled;

}

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
L7_RC_t l7_init_policy_rpc(void)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 i;
  L7_uint32 size;

  /* Allocate buffer for HPC helper stats response */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    size = hpcHardwareRpcMaxMessageLengthGet();
    rpcStatsResponse[i].buf = osapiMalloc(L7_DRIVER_COMPONENT_ID, size);
    if (rpcStatsResponse[i].buf == L7_NULLPTR)
    {
      L7_LOG_ERROR(0);
    }
    rpcStatsResponse[i].buf_size = size;
    rpcStatsResponse[i].data_len = 0; 
  }

  rc = hpcHardwareRpcRegister(L7_RPC_POLICY_HANDLER, l7_rpc_server_policy_handler);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }

  return rc;
}

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
int l7_rpc_client_policy_create(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo)
{
  L7_RC_t                    rc;
  BROAD_POLICY_RPC_DATA_t   *rpcPtr;
  hpcHwRpcStatus_t           rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  int                        app_status[L7_MAX_UNITS_PER_STACK + 1];
  int                        i, rule;
  L7_uint32                  size, maxRules, copiedRules;
  L7_int32                   remainingRules;
  hpcHwRpcData_t             rpc_data;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr, *tmpRulePtr;

  size = hpcHardwareRpcMaxMessageLengthGet();

  if (sizeof(BROAD_POLICY_RPC_DATA_t) > size)
  {
    return BCM_E_FAIL;
  }

  maxRules = (size - sizeof(BROAD_POLICY_RPC_DATA_t)) / sizeof(BROAD_POLICY_RULE_ENTRY_t) + 1;

  if ((rpcPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_POLICY_RPC_DATA_t))) == L7_NULL)
  {
    return BCM_E_UNAVAIL;
  }

  rpcPtr->rpcFlags = BROAD_POLICY_FIRST;

  RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Policy-%d sending FIRST %d rule(s) total, max %d\r\n", 
                  policy, rpcPtr->rpcData.policy.ruleCount, maxRules);

  if (policyInfo->ruleCount == 0)
  {
    RPC_DEBUG_PRINT("l7_rpc_client_policy_create: and setting LAST\r\n");

    rpcPtr->rpcFlags |= BROAD_POLICY_LAST;
    rpcPtr->rpcRulesIncluded = 0;
  }

  /* send rpc policy data */
  rpcPtr->rpcData.policy.policyCmd = BROAD_CUSTOM_POLICY_CREATE;
  rpcPtr->rpcData.policy.policyId = policy;
  rpcPtr->rpcData.policy.policyFlags = policyInfo->policyFlags; 
  rpcPtr->rpcData.policy.strictEthTypes = policyInfo->strictEthTypes; 
  rpcPtr->rpcData.policy.policyType = policyInfo->policyType;
  rpcPtr->rpcData.policy.policyStage = policyInfo->policyStage;
  rpcPtr->rpcData.policy.ruleCount = policyInfo->ruleCount;
  /* PTin added: policers and counters */
  #if 1
  rpcPtr->rpcData.policy.policer_id = policyInfo->general_policer_id;
  rpcPtr->rpcData.policy.counter_id = policyInfo->general_counter_id;
  #endif

  rpc_data.buf = (L7_uchar8 *)rpcPtr;
  rpc_data.buf_size = rpc_data.data_len = sizeof(BROAD_POLICY_RPC_DATA_t);
  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_POLICY_HANDLER,
                      &rpc_data,
                      rpc_status,
                      app_status, L7_NULL);

  osapiFree(L7_DRIVER_COMPONENT_ID, rpcPtr);

  if (L7_SUCCESS == rc)
  {
    for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
    {
      if (HPC_HW_RPC_OK == rpc_status[i])
      {
        if (app_status[i] < BCM_E_NONE)
        {
          RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Error %s on Unit %d\r\n", bcm_errmsg(app_status[i]), i);
          return app_status[i];
        }
      }
    }
  }
  else
  {
    RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Error 1 hpcHardwareRpc() timeout\r\n");
    return BCM_E_TIMEOUT;
  }

  /* send rpc policy rules */
  if (policyInfo->ruleCount > 0)
  {
    if ((rpcPtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, 
                              sizeof(BROAD_POLICY_RPC_DATA_t) + 
                              ((maxRules - 1) * sizeof(BROAD_POLICY_RULE_ENTRY_t)))) == L7_NULL)
    {
      RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Error osapiMalloc()\r\n");
      return BCM_E_UNAVAIL;
    }

    remainingRules = policyInfo->ruleCount;

    /* Iterate through all rules in the policy. */
    rulePtr = policyInfo->ruleInfo;
    for (rule = 0; rule < policyInfo->ruleCount; rule += copiedRules)
    {
      copiedRules = min(maxRules, remainingRules);
      remainingRules -= copiedRules;

      if (remainingRules <= 0)
      {
        rpcPtr->rpcFlags = BROAD_POLICY_LAST;

        RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Policy-%d sending LAST %d rule(s)\r\n", policy, copiedRules);
      }
      else
      {
        rpcPtr->rpcFlags = BROAD_POLICY_MIDDLE;

        RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Policy-%d sending MIDDLE %d rule(s)\r\n", policy, copiedRules);
      }

      rpcPtr->rpcRulesIncluded = copiedRules;

      /* For this RPC, iterate through the rules that will fit in this message. */
      for (i = 0; i < copiedRules; i++)
      {
        if (rulePtr != L7_NULL)
        {
          tmpRulePtr = &(((BROAD_POLICY_RULE_ENTRY_t *)(&rpcPtr->rpcData.rules.firstRule))[i]);
          memcpy(tmpRulePtr,
                 rulePtr,
                 sizeof(BROAD_POLICY_RULE_ENTRY_t));
          rulePtr = rulePtr->next;
        }
      }

      /* send rpc rule data */
      rpc_data.buf = (L7_uchar8 *)rpcPtr;

      rpc_data.buf_size = rpc_data.data_len = 
                          (sizeof(BROAD_POLICY_RPC_DATA_t) + ((copiedRules - 1) * 
                                                              sizeof(BROAD_POLICY_RULE_ENTRY_t)));

      rc = hpcHardwareRpc(L7_ALL_UNITS,
                          L7_RPC_POLICY_HANDLER,
                          &rpc_data,
                          rpc_status,
                          app_status, L7_NULL);

      if (L7_SUCCESS == rc)
      {
        for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
        {
          if (HPC_HW_RPC_OK == rpc_status[i])
          {
            if (app_status[i] < BCM_E_NONE)
            {
              osapiFree(L7_DRIVER_COMPONENT_ID, rpcPtr);
              RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Error %s on Unit %d\r\n", bcm_errmsg(app_status[i]), i);
              return app_status[i];
            }
          }
        }
      }
      else
      {
        RPC_DEBUG_PRINT("l7_rpc_client_policy_create: Error 2 hpcHardwareRpc() timeout\r\n");
        osapiFree(L7_DRIVER_COMPONENT_ID, rpcPtr);
        return BCM_E_TIMEOUT;
      }
    } /* for */

    osapiFree(L7_DRIVER_COMPONENT_ID, rpcPtr);

  } /* if */

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Destroy a Policy
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_destroy(BROAD_POLICY_t policy)
{
  L7_RC_t rc;
  BROAD_POLICY_RPC_DATA_t data;
  int               app_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcStatus_t  rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t    rpc_data;
  L7_uint32         i;

  data.rpcFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
  data.rpcRulesIncluded = 0;

  data.rpcData.policy.policyCmd = BROAD_CUSTOM_POLICY_DESTROY;
  data.rpcData.policy.policyId = policy;

  RPC_DEBUG_PRINT("l7_rpc_client_policy_destroy: Policy-%d sending BROAD_CUSTOM_POLICY_DESTROY\r\n", policy);

  rpc_data.buf = (L7_uchar8*)&data;
  rpc_data.buf_size = rpc_data.data_len = sizeof(BROAD_POLICY_RPC_DATA_t);

  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_POLICY_HANDLER,
                      &rpc_data,
                      rpc_status,
                      app_status, L7_NULL);

  if (L7_SUCCESS == rc)
  {
    for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
    {
      if (HPC_HW_RPC_OK == rpc_status[i])
      {
        if (app_status[i] < BCM_E_NONE) 
        {
          return app_status[i];
        }
      }
    }
  }
  else
  {
    return BCM_E_TIMEOUT;
  }

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_apply_all(BROAD_POLICY_t policy)
{
  L7_RC_t rc;
  BROAD_POLICY_RPC_DATA_t data;
  int               app_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcStatus_t  rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t    rpc_data;
  L7_uint32         i;

  data.rpcFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
  data.rpcRulesIncluded = 0;

  data.rpcData.policy.policyCmd = BROAD_CUSTOM_POLICY_APPLY_ALL;
  data.rpcData.policy.policyId = policy;

  rpc_data.buf = (L7_uchar8*)&data;
  rpc_data.buf_size = rpc_data.data_len = sizeof(BROAD_POLICY_RPC_DATA_t);

  RPC_DEBUG_PRINT("l7_rpc_client_policy_apply_all: Policy-%d sending BROAD_CUSTOM_POLICY_APPLY_ALL\r\n", policy);

  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_POLICY_HANDLER,
                      &rpc_data,
                      rpc_status,
                      app_status, L7_NULL);

  if (L7_SUCCESS == rc)
  {
    for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
    {
      if (HPC_HW_RPC_OK == rpc_status[i])
      {
        if (app_status[i] < BCM_E_NONE) 
        {
          return app_status[i];
        }
      }
    }
  }
  else
  {
    return BCM_E_TIMEOUT;
  }

  return BCM_E_NONE;
}
/*********************************************************************
* @purpose   Set status for policy Rule ID and to activate deactivate the rule installed in hardware
*
* @param    BROAD_POLICY_t        policy
* @param    BROAD_PORT_RULE_t     rule
* @param    BROAD_RULE_STATUS_t   status
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_rule_status_set(BROAD_POLICY_t policy, BROAD_POLICY_RULE_t rule, BROAD_RULE_STATUS_t status)
{
  L7_RC_t rc;
  BROAD_POLICY_RPC_DATA_t data;
  int               app_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcStatus_t  rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t    rpc_data;
  L7_uint32         i;

  data.rpcFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
  data.rpcRulesIncluded = 0;

  data.rpcData.policy.policyCmd = BROAD_CUSTOM_POLICY_RULE_STATUS_SET;
  data.rpcData.policy.policyId = policy;  
  data.rpcData.policy.ruleId = rule;  
  data.rpcData.policy.policyRuleStatus = status;  
  data.rpcData.policy.policySize = 0;

  rpc_data.buf = (L7_uchar8*)&data;
  rpc_data.buf_size = rpc_data.data_len = sizeof(BROAD_POLICY_RPC_DATA_t);

  RPC_DEBUG_PRINT("l7_rpc_client_policy_status_set: Policy-%d  Rule %d sending BROAD_CUSTOM_POLICY_RULE_STATUS_SET\r\n", policy, rule);

  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_POLICY_HANDLER,
                      &rpc_data,
                      rpc_status,
                      app_status, L7_NULL);
  if (L7_SUCCESS == rc)
  {
    for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
    {
      if (HPC_HW_RPC_OK == rpc_status[i])
      {
        if (app_status[i] < BCM_E_NONE)
        {
         RPC_DEBUG_PRINT("l7_rpc_client_policy_rule_status_set: Error %s on Unit %d\r\n", bcm_errmsg(app_status[i]), i);
          return app_status[i];
        }
      }
    }
  }
  else
  {
    RPC_DEBUG_PRINT("l7_rpc_client_policy_rule_status_set: Error 1 hpcHardwareRpc() timeout\r\n");
    return BCM_E_TIMEOUT;
  }

  return BCM_E_NONE;

}

/*********************************************************************
* @purpose  Remove a policy from all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int l7_rpc_client_policy_remove_all(BROAD_POLICY_t policy)
{
  L7_RC_t rc;
  BROAD_POLICY_RPC_DATA_t data;
  int               app_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcStatus_t  rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t    rpc_data;
  L7_uint32         i;

  data.rpcFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
  data.rpcRulesIncluded = 0;

  data.rpcData.policy.policyCmd = BROAD_CUSTOM_POLICY_REMOVE_ALL;
  data.rpcData.policy.policyId = policy;

  RPC_DEBUG_PRINT("l7_rpc_client_policy_remove_all: Policy-%d sending BROAD_CUSTOM_POLICY_REMOVE_ALL\r\n", policy);

  rpc_data.buf = (L7_uchar8*)&data;
  rpc_data.buf_size = rpc_data.data_len = sizeof(BROAD_POLICY_RPC_DATA_t);

  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_POLICY_HANDLER,
                      &rpc_data,
                      rpc_status,
                      app_status, L7_NULL);

  if (L7_SUCCESS == rc)
  {
    for (i = 0; i < L7_MAX_UNITS_PER_STACK+1; i++)
    {
      if (HPC_HW_RPC_OK == rpc_status[i])
      {
        if (app_status[i] < BCM_E_NONE) 
        {
          return app_status[i];
        }
      }
    }
  }
  else
  {
    return BCM_E_TIMEOUT;
  }

  return BCM_E_NONE;
}

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
                                    bcmx_lport_t   port)
{
  int                         rv;
  uint32                      args[BCM_CUSTOM_ARGS_MAX];
  BROAD_POLICY_CUSTOM_DATA_t *pData;

  pData = (BROAD_POLICY_CUSTOM_DATA_t *)args;
  pData->policyCmd   = BROAD_CUSTOM_POLICY_APPLY;
  pData->policyFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
  pData->policyId    = policyId;

  RPC_DEBUG_PRINT("l7_rpc_client_policy_port_apply: Policy-%d sending BROAD_CUSTOM_POLICY_APPLY port 0x%x\r\n", policyId, port);

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_POLICY_SET_HANDLER, sizeof(BROAD_POLICY_CUSTOM_DATA_t)/sizeof(L7_uint32), args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_POLICY_SET_HANDLER, args);
  #endif
  if (L7_BCMX_OK(rv) == L7_TRUE)
    rv = BCM_E_NONE;

  return rv;
}

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
                                     bcmx_lport_t   port)
{
  int                         rv;
  uint32                      args[BCM_CUSTOM_ARGS_MAX];
  BROAD_POLICY_CUSTOM_DATA_t *pData;

  pData = (BROAD_POLICY_CUSTOM_DATA_t *)args;
  pData->policyCmd   = BROAD_CUSTOM_POLICY_REMOVE;
  pData->policyFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
  pData->policyId    = policyId;

  RPC_DEBUG_PRINT("l7_rpc_client_policy_port_remove: Policy-%d sending BROAD_CUSTOM_POLICY_REMOVE port 0x%x\r\n", policyId, port);

  /* PTin modified: SDK 6.3.0 */
  #if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
  rv = bcmx_custom_port_set(port, USL_BCMX_POLICY_SET_HANDLER, sizeof(BROAD_POLICY_CUSTOM_DATA_t)/sizeof(L7_uint32), args);
  #else
  rv = bcmx_custom_port_set(port, USL_BCMX_POLICY_SET_HANDLER, args);
  #endif
  if (L7_BCMX_OK(rv) == L7_TRUE)
    rv = BCM_E_NONE;

  return rv;
}

static void _accumulate_stats(BROAD_POLICY_STATS_t *stats, L7_uchar8 *buffer)
{
  L7_uint64 tmpVal;
  L7_uint32 tmp32_hi, tmp32_lo;

  /* copy returned values (args) into stats structure */
  if (L7_TRUE == stats->meter)
  {
    /* accumulate out-of-profile */
    memcpy(&tmp32_hi, &buffer[0], sizeof(tmp32_hi));
    memcpy(&tmp32_lo, &buffer[4], sizeof(tmp32_lo));
    RPC_DEBUG_PRINT("%s:%d: temp32_hi = 0x%x, temp32_lo = 0x%x\n", __FUNCTION__, __LINE__, tmp32_hi, tmp32_lo);
    COMPILER_64_SET(tmpVal, tmp32_hi, tmp32_lo);
    COMPILER_64_ADD_64(stats->statMode.meter.out_prof, tmpVal);

    /* accumulate in-profile */
    memcpy(&tmp32_hi, &buffer[8], sizeof(tmp32_hi));
    memcpy(&tmp32_lo, &buffer[12], sizeof(tmp32_lo));
    RPC_DEBUG_PRINT("%s:%d: temp32_hi = 0x%x, temp32_lo = 0x%x\n", __FUNCTION__, __LINE__, tmp32_hi, tmp32_lo);
    COMPILER_64_SET(tmpVal, tmp32_hi, tmp32_lo);
    COMPILER_64_ADD_64(stats->statMode.meter.in_prof, tmpVal);
  }
  else
  {
    /* accumulate counter */
    #if (SDK_VERSION_IS >= SDK_VERSION(5,6,0,0))
    memcpy(&tmp32_hi, &buffer[0], sizeof(tmp32_hi));
    memcpy(&tmp32_lo, &buffer[4], sizeof(tmp32_lo));
    #else
    memcpy(&tmp32_hi, &buffer[8], sizeof(tmp32_hi));
    memcpy(&tmp32_lo, &buffer[12], sizeof(tmp32_lo));
    #endif
    RPC_DEBUG_PRINT("%s:%d: temp32_hi = 0x%x, temp32_lo = 0x%x\n", __FUNCTION__, __LINE__, tmp32_hi, tmp32_lo);
    COMPILER_64_SET(tmpVal, tmp32_hi, tmp32_lo);
    COMPILER_64_ADD_64(stats->statMode.counter.count, tmpVal);
  }
}

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
                                   BROAD_POLICY_STATS_t *stats)
{
  L7_RC_t                  rc;
  BROAD_POLICY_RPC_DATA_t  data;
  int                      app_status[L7_MAX_UNITS_PER_STACK + 1];
  static hpcHwRpcStatus_t  rpc_status[L7_MAX_UNITS_PER_STACK + 1];
  hpcHwRpcData_t           rpc_data;
  L7_uint32                i;
  int                      rv = BCM_E_NONE;

  data.rpcFlags = BROAD_POLICY_FIRST | BROAD_POLICY_LAST;
  data.rpcRulesIncluded = 0;

  data.rpcData.policy.policyCmd = BROAD_CUSTOM_POLICY_STATS;
  data.rpcData.policy.policyId = policyId;
  data.rpcData.policy.ruleId   = ruleId;

  RPC_DEBUG_PRINT("l7_rpc_client_policy_stats_get: Policy-%d, ruleId %d sending BROAD_CUSTOM_POLICY_STATS\r\n", policyId, ruleId);

  rpc_data.buf = (L7_uchar8*)&data;
  rpc_data.buf_size = rpc_data.data_len = sizeof(BROAD_POLICY_RPC_DATA_t);

  /* Clear the response buffer */
  for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
  {
    memset(rpcStatsResponse[i].buf, 0, rpcStatsResponse[i].buf_size);
    rpcStatsResponse[i].data_len = 0;
  }

  rc = hpcHardwareRpc(L7_ALL_UNITS,
                      L7_RPC_POLICY_HANDLER,
                      &rpc_data,
                      rpc_status,
                      app_status, 
                      rpcStatsResponse);

  if (rc == L7_SUCCESS)
  {
    /* Loop through units that successfully completed RPC and
    ** return the lowest return code. Since RPC is also executed
    ** on the local unit we are guaranteed at least one successful completion.
    */
    rv = BCM_E_NONE;
    for (i = 0; i <= L7_MAX_UNITS_PER_STACK; i++)
    {
      if (rpc_status[i] == HPC_HW_RPC_OK)
      {
        if (app_status[i] == BCM_E_NONE)
        {
          _accumulate_stats(stats, rpcStatsResponse[i].buf);
        }

        if (app_status[i] < rv)
        {
          rv = app_status[i];
        }
      }
    }
  }
  else
  {
    /* RPC is not ready. This is probably a transient error, so
     * treat it as a timeout.
    */
    rv = BCM_E_TIMEOUT;
  }

  return rv;
}

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
int usl_bcm_policy_create(BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo)
{
  int                         rv = BCM_E_NONE, tmpRv = BCM_E_NONE;
  L7_uint32                   i;
  L7_uint32                   hashVal;

  RPC_DEBUG_PRINT("usl_bcm_policy_create: Policy-%d processing BROAD_CUSTOM_POLICY_CREATE\r\n", policy);

  /* Don't program per-port policies into HW at this point... 
     we'll do that later if/when the policy is applied to a port
     on this unit. */
  if ((BROAD_POLICY_TYPE_PORT        != policyInfo->policyType) &&
      (BROAD_POLICY_TYPE_DOT1AD      != policyInfo->policyType) &&
      (BROAD_POLICY_TYPE_IPSG        != policyInfo->policyType) &&
      (BROAD_POLICY_TYPE_SYSTEM_PORT != policyInfo->policyType))
  {
    if (USL_BCM_CONFIGURE_HW(USL_POLICY_DB_ID) == L7_TRUE)
    {
      for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
      {
        tmpRv = l7_bcm_policy_create(i, policy, policyInfo);
        RPC_DEBUG_PRINT("%s(%d) Policy-%d, return %d\r\n",__FUNCTION__,__LINE__,policy,tmpRv);

        if (tmpRv < rv)
          rv = tmpRv;
      }
    }
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_POLICY_DB_ID) == L7_TRUE))
  {
    hashVal = usl_db_policy_hash_calc(policyInfo);
    rv = usl_db_policy_create_with_id(USL_CURRENT_DB, policy, policyInfo, hashVal);
    RPC_DEBUG_PRINT("%s(%d) Policy-%d, return %d\r\n",__FUNCTION__,__LINE__,policy,rv);
  }
  
  RPC_DEBUG_PRINT("%s(%d) Policy-%d, return %d\r\n",__FUNCTION__,__LINE__,policy,rv);
  return rv;
}

/*********************************************************************
* @purpose  Destroy a Policy
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_destroy(BROAD_POLICY_t policy)
{
  int                         rv = BCM_E_NONE, tmpRv = BCM_E_NONE;
  L7_uint32                   i;

  RPC_DEBUG_PRINT("usl_bcm_policy_destroy: Policy-%d processing BROAD_CUSTOM_POLICY_DESTROY\r\n", policy);

  if (USL_BCM_CONFIGURE_HW(USL_POLICY_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      tmpRv = l7_bcm_policy_destroy(i, policy);

      if (tmpRv < rv)
        rv = tmpRv;
    }
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_POLICY_DB_ID) == L7_TRUE))
  {
    rv = usl_db_policy_destroy(USL_CURRENT_DB, policy);
  }
  
  return rv;
}

/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_apply_all(BROAD_POLICY_t policyId)
{
  int                      rv = BCM_E_NONE;
  int                      unit;
  BROAD_POLICY_ENTRY_t     policyInfo;
  L7_BOOL                  havePolicyInfo = L7_FALSE;
  int (*policyCreateFunc)  (int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData);
  int (*policyApplyAllFunc)(int unit, BROAD_POLICY_t policy);

  RPC_DEBUG_PRINT("usl_bcm_policy_apply_all: Policy-%d processing BROAD_CUSTOM_POLICY_APPLY_ALL\r\n", policyId);

  if (USL_BCM_CONFIGURE_HW(USL_POLICY_DB_ID) == L7_TRUE)
  {
    for (unit = 0; unit < bde->num_devices(BDE_SWITCH_DEVICES); unit++)
    {
      policyCreateFunc   = l7_bcm_policy_create;
      policyApplyAllFunc = l7_bcm_policy_apply_all;

      rv = policyApplyAllFunc(unit, policyId);
  
      /* If there was an error, it may be because we have not yet created this policy on this unit.
         Try to create it now, then retry the apply operation. */
      if (rv != BCM_E_NONE)
      {
        if (havePolicyInfo == L7_FALSE)
        {
          memset(&policyInfo, 0, sizeof(policyInfo));
          rv = usl_db_policy_info_get(USL_CURRENT_DB, policyId, &policyInfo);
          if (rv != BCM_E_NONE)
          {
            RPC_DEBUG_PRINT("usl_bcm_policy_apply_all: Policy-%d not found in USL DB\r\n", policyId);
            return rv;
          }
          havePolicyInfo = L7_TRUE;
        }
  
        if ((BROAD_POLICY_TYPE_PORT        == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_DOT1AD      == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_IPSG        == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_SYSTEM_PORT == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_LLPF        == policyInfo.policyType))
        {
          rv = policyCreateFunc(unit, policyId, &policyInfo);
          if (rv == BCM_E_NONE)
          {
            rv = policyApplyAllFunc(unit, policyId);
          }
          else
          {
            RPC_DEBUG_PRINT("usl_bcm_policy_apply_all: Couldn't create Policy-%d\r\n", policyId);
          }
        }
      }
    }

    if (havePolicyInfo == L7_TRUE)
    {
      /* Free any rules allocated by usl_db_policy_info_get(). */
      hapiBroadPolicyRulesPurge(&policyInfo);
    }
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_POLICY_DB_ID) == L7_TRUE))
  {
    rv = usl_db_policy_apply_all(USL_CURRENT_DB, policyId);
  }
  
  return rv;
}

/*********************************************************************
* @purpose  Remove a policy from all ports
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_remove_all(BROAD_POLICY_t policy)
{
  int                         rv = BCM_E_NONE, tmpRv = BCM_E_NONE;
  L7_uint32                   i;

  RPC_DEBUG_PRINT("usl_bcm_policy_remove_all: Policy-%d processing BROAD_CUSTOM_POLICY_REMOVE_ALL\r\n", policy);

  if (USL_BCM_CONFIGURE_HW(USL_POLICY_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
      tmpRv = l7_bcm_policy_remove_all(i, policy);

      if (tmpRv < rv)
        rv = tmpRv;
    }
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_POLICY_DB_ID) == L7_TRUE))
  {
    rv = usl_db_policy_remove_all(USL_CURRENT_DB, policy);
  }
  
  return rv;
}

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
                              bcm_port_t     port)
{
  int         rv    = BCM_E_NONE;
  bcm_gport_t gport = BCM_GPORT_INVALID;
  BROAD_POLICY_ENTRY_t policyInfo;
  int (*policyCreateFunc)(int unit, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyData);
  int (*policyApplyFunc)(int unit, BROAD_POLICY_t policy, bcm_port_t port);

  RPC_DEBUG_PRINT("usl_bcm_policy_port_apply: Policy-%d processing BROAD_CUSTOM_POLICY_APPLY unit %d port %d\r\n", policyId, unit, port);

  policyCreateFunc = l7_bcm_policy_create;
  policyApplyFunc  = l7_bcm_policy_apply;

  if (USL_BCM_CONFIGURE_HW(USL_POLICY_DB_ID) == L7_TRUE)
  {
    rv = policyApplyFunc(unit, policyId, port);

    /* If there was an error, it may be because we have not yet created this policy on this unit.
       Try to create it now, then retry the apply operation. */
    if (rv != BCM_E_NONE)
    {
      /* Get the policy info. */
      memset(&policyInfo, 0, sizeof(policyInfo));
      rv = usl_db_policy_info_get(USL_CURRENT_DB, policyId, &policyInfo);
      if (rv == BCM_E_NONE)
      {
        if ((BROAD_POLICY_TYPE_PORT        == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_DOT1AD      == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_IPSG        == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_SYSTEM_PORT == policyInfo.policyType) ||
            (BROAD_POLICY_TYPE_LLPF        == policyInfo.policyType))
        {
          rv = policyCreateFunc(unit, policyId, &policyInfo);
          if (rv == BCM_E_NONE)
          {
            rv = policyApplyFunc(unit, policyId, port);
            if (rv != BCM_E_NONE)
            {
              RPC_DEBUG_PRINT("usl_bcm_policy_port_apply: Couldn't apply Policy-%d to port %d, rv %d\r\n", policyId, port, rv);
            }
          }
          else
          {
            RPC_DEBUG_PRINT("usl_bcm_policy_port_apply: Couldn't create Policy-%d, rv %d\r\n", policyId, rv);
          }
        }

        /* Free any rules allocated by usl_db_policy_info_get(). */
        hapiBroadPolicyRulesPurge(&policyInfo);
      }
      else
      {
        RPC_DEBUG_PRINT("usl_bcm_policy_port_apply: Policy-%d not found in USL DB\r\n", policyId);
      }
    }
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_POLICY_DB_ID) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_policy_port_apply(USL_CURRENT_DB, policyId, gport);
    }
  }
  
  return rv;
}

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
                              bcm_port_t     port)
{
  int         rv    = BCM_E_NONE;
  bcm_gport_t gport = BCM_GPORT_INVALID;

  RPC_DEBUG_PRINT("usl_bcm_policy_port_remove: Policy-%d processing BROAD_CUSTOM_POLICY_REMOVE unit %d port %d\r\n", policyId, unit, port);

  if (USL_BCM_CONFIGURE_HW(USL_POLICY_DB_ID) == L7_TRUE)
  {
    rv = l7_bcm_policy_remove(unit, policyId, port);
  }

  if ((L7_BCMX_OK(rv) == L7_TRUE) && 
      (USL_BCM_CONFIGURE_DB(USL_POLICY_DB_ID) == L7_TRUE))
  {
    /* Get the gport for the local bcm unit/port */
    rv = usl_bcm_unit_port_to_gport(unit, port, &gport);
    if (rv == BCM_E_NONE)
    {
      rv = usl_db_policy_port_remove(USL_CURRENT_DB, policyId, gport);
    }
  }
  
  return rv;
}

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
                             hpcHwRpcData_t *rpc_resp)
{
  int       tmpRv = BCM_E_NONE;
  int       rv    = BCM_E_NONE;
  L7_uint32 i;

  RPC_DEBUG_PRINT("usl_bcm_policy_stats_get: Policy-%d, rule %d processing BROAD_CUSTOM_POLICY_STATS\r\n", policyId, ruleId);

  /* Clear the response buffer. */
  memset(rpc_resp->buf, 0, rpc_resp->buf_size);
  rpc_resp->data_len = rpc_resp->buf_size;


  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
    tmpRv = l7_bcm_policy_stats(i, policyId, ruleId, rpc_resp->buf, &rpc_resp->data_len);

    if (tmpRv < rv)
      rv = tmpRv;
  }

  return rv;
}

/*********************************************************************
* @purpose  Clear counters for a rule in a policy
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcm_policy_counter_clear(BROAD_POLICY_t policy)
{
  int                         rv = BCM_E_NONE, tmpRv = BCM_E_NONE;
  L7_uint32                   i;

  RPC_DEBUG_PRINT("usl_bcm_policy_counter_clear: Policy %d \r\n", policy);

  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
    tmpRv = l7_bcm_policy_counter_clear(i, policy);

    if (tmpRv < rv)
      rv = tmpRv;
  }

  return rv;
}

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
void usl_bcm_policy_dataplane_cleanup(L7_uint32 policy, L7_uint32 rule, L7_uint32 cpu_modid, L7_uint32 cpu_modport)
{
  L7_uint32 i;

  RPC_DEBUG_PRINT("usl_bcm_policy_new_mgr_notify: Policy %d, Rule %d, modid %d, modport %d \r\n", 
                  policy,
                  rule,
                  cpu_modid,
                  cpu_modport);

  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
    l7_bcm_policy_dataplane_cleanup(i, policy, rule, cpu_modid, cpu_modport);
  }
}

/* debug */

void rpc_policy_debug(L7_uint32 setting)
{
  if (setting > L7_NULL)
  {
    rpc_debug = L7_TRUE;
  }
  else
  {
    rpc_debug = L7_FALSE;
  }
}
/*********************************************************************
* @purpose  Activate/Deactivate the rule based on rule status
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
                                   BROAD_POLICY_RULE_t rule, BROAD_RULE_STATUS_t status)
{
  int                         rv = BCM_E_NONE;
  L7_uint32                   i;

  RPC_DEBUG_PRINT("\n usl_bcm_policy_rule_status: Policy-%d rule %d processing BROAD_CUSTOM_POLICY_RULE_STATUS_SET\r\n", policyId, rule);

  /* Update the USL database in the current unit */
  if (USL_BCM_CONFIGURE_HW(USL_POLICY_DB_ID) == L7_TRUE)
  {
    for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
    {
        rv = l7_bcm_policy_rule_status_set(i, policyId, rule, status);
    }
    
  }
  if ((L7_BCMX_OK(rv) == L7_TRUE) &&
      (USL_BCM_CONFIGURE_DB(USL_POLICY_DB_ID) == L7_TRUE))
  {
    rv = usl_db_policy_rule_status_set(USL_CURRENT_DB, policyId, rule, status);
  }
  return rv;
}

