/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  l7_usl_bcmx_policy.c
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
#include "broad_common.h"
#include "broad_policy_types.h"
#include "l7_usl_bcmx_policy.h"
#include "l7_usl_policy_db.h"
#include "l7_rpc_policy.h"
#include "l7_usl_sm.h"
#include "l7_usl_trace.h"
#include "zlib.h"

static void    *pUslPolicyBcmxSema       = L7_NULLPTR;

#define USL_POLICY_BCMX_LOCK_TAKE()\
{ \
  usl_trace_sema(USL_POLICY_DB_ID,"POL",osapiTaskIdSelf(),__LINE__,L7_TRUE); \
  if (osapiSemaTake(pUslPolicyBcmxSema,L7_WAIT_FOREVER) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Take failed, semId %x\n", pUslPolicyBcmxSema); \
  }\
}

#define USL_POLICY_BCMX_LOCK_GIVE()\
{ \
  usl_trace_sema(USL_POLICY_DB_ID,"POL",osapiTaskIdSelf(),__LINE__,L7_FALSE); \
  if (osapiSemaGive(pUslPolicyBcmxSema) != L7_SUCCESS) \
  { \
    USL_LOG_MSG(USL_BCM_E_LOG,"Sema Give failed, semId %x\n", pUslPolicyBcmxSema); \
  }\
}

/*********************************************************************
* @purpose  Initialize Policy bcmx
*
* @param    none
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_policy_bcmx_init(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    pUslPolicyBcmxSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslPolicyBcmxSema == L7_NULLPTR ) break;


    if (l7_init_policy_rpc() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "USL policy RPC init failed\n");
      break;
    }
  

    rc = L7_SUCCESS;
  }
  while(0);

  return rc;
}

/*********************************************************************
* @purpose  Suspend Policy Bcmx calls
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void usl_policy_bcmx_suspend()
{
  USL_POLICY_BCMX_LOCK_TAKE();
}

/*********************************************************************
* @purpose  Suspend Policy Bcmx calls
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void usl_policy_bcmx_resume()
{
  USL_POLICY_BCMX_LOCK_GIVE();
}

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
int usl_bcmx_policy_info_get(BROAD_POLICY_t        policy, 
                             BROAD_POLICY_ENTRY_t *policyInfo)
{
  int     rv;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  rv = usl_db_policy_info_get(USL_CURRENT_DB, policy, policyInfo);

  USL_POLICY_BCMX_LOCK_GIVE();

  return rv;
}

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
int usl_bcmx_policy_create(BROAD_POLICY_t       *policy, 
                           BROAD_POLICY_ENTRY_t *policyInfo)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Create the policy first. If the DB already has this policy, 
    then the same policy ID will be returned to the caller. */
    dbRv = usl_db_policy_create(USL_CURRENT_DB, policy, policyInfo);

    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_create(*policy, policyInfo);
    }

    /* Clean up DB if HW add failed. */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      (void)l7_rpc_client_policy_destroy(*policy);
      (void)usl_db_policy_destroy(USL_CURRENT_DB, *policy);
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to create policy, hwRv %d dbRv %d\n",
                hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Recommit a Policy in the HW's table
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_recommit(BROAD_POLICY_t        policy, 
                             BROAD_POLICY_ENTRY_t *policyInfo)
{
  int       rv;
  int       hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  L7_uint32 hashVal;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_create(policy, policyInfo);
    }

    /* Clean up DB if HW add failed. */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      (void)usl_db_policy_destroy(USL_CURRENT_DB, policy);
      break;
    }

    /* Update the USL Db */
    hashVal = usl_db_policy_hash_calc(policyInfo);
    dbRv = usl_db_policy_create_with_id(USL_CURRENT_DB, policy, policyInfo, hashVal);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to recommit policy %d, hwRv %d dbRv %d\n",
                policy, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Destroy a Policy in the HW's table
*
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_bcmx_policy_destroy(BROAD_POLICY_t policy)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_destroy(policy);
    }

    /* Update the USL Db even if hw delete failed */
    dbRv = usl_db_policy_destroy(USL_CURRENT_DB, policy);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Error while destroying policy %d, hwRv %d dbRv %d\n",
                policy, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

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
int usl_bcmx_policy_apply_all(BROAD_POLICY_t policy)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_apply_all(policy);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_policy_apply_all(USL_CURRENT_DB, policy);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to apply policy %d to all ports, hwRv %d dbRv %d\n",
                policy, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

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
int usl_bcmx_policy_remove_all(BROAD_POLICY_t policy)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_remove_all(policy);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_policy_remove_all(USL_CURRENT_DB, policy);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to remove policy %d from all ports, hwRv %d dbRv %d\n",
                policy, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

  return rv;
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
int usl_bcmx_policy_port_apply(BROAD_POLICY_t policy,
                               bcmx_lport_t   port)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Only allow this function on physical ports. */
    if (!BCM_GPORT_IS_MODPORT(port))
    {
      hwRv = BCM_E_PARAM;
      break;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_port_apply(policy, port);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_policy_port_apply(USL_CURRENT_DB, policy, port);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to apply policy %d to port %d, hwRv %d dbRv %d\n",
                policy, port, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

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
int usl_bcmx_policy_port_remove(BROAD_POLICY_t policy,
                                bcmx_lport_t   port)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Only allow this function on physical ports. */
    if (!BCM_GPORT_IS_MODPORT(port))
    {
      hwRv = BCM_E_PARAM;
      break;
    }

    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_port_remove(policy, port);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_policy_port_remove(USL_CURRENT_DB, policy, port);
    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to remove policy %d from port %d, hwRv %d dbRv %d\n",
                policy, port, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

  return rv;
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
int usl_bcmx_policy_stats_get(BROAD_POLICY_t        policy,
                              L7_uint32             ruleId,
                              BROAD_POLICY_STATS_t *stats)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;
  
  USL_POLICY_BCMX_LOCK_TAKE();

  do
  {
    /* Check if the hardware is valid. */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_stats_get(policy, ruleId, stats);
    }
    else
    {
      /* We can't refer to the HW yet. */
      hwRv = BCM_E_UNAVAIL;
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    {
      break;
    }
  } while (0);

  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG, 
                "USL: Failed to get stats for policy %d, ruleId %d, hwRv %d dbRv %d\n",
                policy, ruleId, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

  return rv;
}
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
                               BROAD_POLICY_RULE_t rule, BROAD_RULE_STATUS_t status)
{
  int     rv;
  int     hwRv = BCM_E_NONE, dbRv = BCM_E_NONE;

  /* Validate the policy */
  if (policy >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  /* Validate the rule */
  if(rule >= BROAD_MAX_RULES_PER_POLICY)
  {
    return BCM_E_PARAM;
  }
  
  /* validate the status */
  if((status != BROAD_POLICY_RULE_STATUS_INACTIVE) && (status != BROAD_POLICY_RULE_STATUS_ACTIVE))
  {
    return BCM_E_PARAM;
  }
  USL_POLICY_BCMX_LOCK_TAKE();
  do
  {
    /* Check if the hardware should be configured */
    if (USL_BCMX_CONFIGURE_HW(USL_POLICY_DB_ID))
    {
      hwRv = l7_rpc_client_policy_rule_status_set(policy, rule, status);
    }

    /* Do not update the USL Db if hw add failed */
    if (L7_BCMX_OK(hwRv) != L7_TRUE)
    { 
      break;
    }

    /* Update the USL Db */
    dbRv = usl_db_policy_rule_status_set(USL_CURRENT_DB, policy, rule, status);

    if (L7_BCMX_OK(dbRv) != L7_TRUE)
    {
      break;
    }

  }while(0);
  /* Return the worst error code */
  rv = min(hwRv, dbRv);

  /* Log the message on error */
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    USL_LOG_MSG(USL_INFO_LOG,
                "USL: Failed to set rule status for policy %d rule ID %d, hwRv %d dbRv %d\n",
                policy, rule, hwRv, dbRv);
  }

  USL_POLICY_BCMX_LOCK_GIVE();

  return rv;
}


