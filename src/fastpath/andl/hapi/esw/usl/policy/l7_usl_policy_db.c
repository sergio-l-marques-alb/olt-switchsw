/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  l7_usl_policy_db.c
*
* @purpose   This file implements the Policy Manager USL DB routines.
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
#include "broad_policy.h"
#include "l7_rpc_policy.h"
#include "l7_usl_policy_db.h"
#include  "l7_usl_policy_db_int.h"
#include "l7_usl_port_db.h"
#include "l7_usl_bcm.h"
#include "l7_usl_trace.h"
#include "zlib.h"
#include "buff_api.h"
#include "unitmgr_api.h"
#include "ibde.h"
#include "l7_usl_sm.h"

L7_BOOL usl_debug = L7_FALSE;



void                              *pUslPolicyDbSema         = L7_NULLPTR;

BROAD_USL_POLICY_t                *pUslOperPolicyDB         = L7_NULLPTR;

BROAD_USL_POLICY_t                *pUslShadowPolicyDB       = L7_NULLPTR;

BROAD_USL_POLICY_t                *pUslPolicyDBHandle       = L7_NULLPTR;

L7_BOOL                            uslPolicyDbActive        = L7_FALSE;

#ifdef L7_STACKING_PACKAGE
L7_uint32 uslPolicyDataplaneCleanupTime  = 0; /* Total time spent doing dataplane cleanups */
L7_uint32 uslPolicyDataplaneCleanupCount = 0; /* Total dataplane cleanups */
#endif


static char *policyPortModeStr[PORT_MODE_LAST] = 
{
  "Default",
  "All",
  "None"
};

static char *policyTypeStr[BROAD_POLICY_TYPE_LAST] = 
{
  "Port",
  "Dot1ad",
  "IPSG",
  "VLAN",
  "ISCSI",
  "System Port",
  "System",
  "COSQ",
  "DVLAN",
  "DOT1AD Snoop",
  "LLPF",
  "PTIN",
  "STAT_EVC",
  "STAT_CLIENT"
};

static char *policyStageStr[BROAD_POLICY_STAGE_COUNT] = 
{
  "Lookup",
  "Ingress",
  "Egress"
};


static L7_RC_t usl_policy_db_memory_init(BROAD_USL_POLICY_t **pUslPolicyDB)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 max;

  do
  {
    max = BROAD_MAX_POLICIES;
    *pUslPolicyDB = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_USL_POLICY_t) * max);
    if (*pUslPolicyDB == L7_NULLPTR)
      break;
    else
      memset((void*)*pUslPolicyDB, 0, max * sizeof(BROAD_USL_POLICY_t));

    rc = L7_SUCCESS;

  } while (0);

  return rc;
}
/*********************************************************************
* @purpose  Create the USL Policy db
*
* @params   none
*
* @returns  none
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usl_policy_db_init(void)
{
  L7_RC_t          rc = L7_FAILURE;
  uslDbSyncFuncs_t policyDbFuncs;

  do
  {
    if (custom_policy_init() != BCM_E_NONE)  
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
              "Custom policy init failed\n");
      break;
    }


    pUslPolicyDbSema = osapiSemaMCreate (OSAPI_SEM_Q_PRIORITY);
    if ( pUslPolicyDbSema == L7_NULLPTR ) break;

    if (usl_policy_db_memory_init(&pUslOperPolicyDB) != L7_SUCCESS)
      break;

    /* Allocate shadow tables for NSF feature */
    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_TRUE)
    {
      if (usl_policy_db_memory_init(&pUslShadowPolicyDB) != L7_SUCCESS)
        break;

      /* Register the reconciliation routines */
      memset(&policyDbFuncs, 0, sizeof(policyDbFuncs));
      policyDbFuncs.get_size_of_db_elem = usl_get_size_of_policy_db_elem;
      policyDbFuncs.get_db_elem         = usl_get_policy_db_elem;
      policyDbFuncs.delete_elem_from_db = usl_delete_policy_db_elem;
      policyDbFuncs.print_db_elem       = usl_print_policy_db_elem;
      policyDbFuncs.create_usl_bcm      = usl_create_policy_db_elem_bcm;
      policyDbFuncs.delete_usl_bcm      = usl_delete_policy_db_elem_bcm;
      policyDbFuncs.update_usl_bcm      = usl_update_policy_db_elem_bcm;

      if (usl_db_sync_func_table_register(USL_POLICY_DB_ID, 
                                          &policyDbFuncs) != L7_SUCCESS)
      {
        LOG_ERROR(0);   
      }
    }
    
    pUslPolicyDBHandle       = pUslOperPolicyDB;
    uslPolicyDbActive        = L7_TRUE;

    rc = L7_SUCCESS; 

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  De-allocate the USL Policy db
*
* @params   none
*
* @returns  none
*
* @end
*********************************************************************/
L7_RC_t usl_policy_db_fini(void)
{
  L7_RC_t rc = L7_FAILURE;

  do
  {
    uslPolicyDbActive = L7_FALSE;

    if (pUslPolicyDbSema  != L7_NULLPTR)
    {
      osapiSemaDelete(pUslPolicyDbSema);
      pUslPolicyDbSema  = L7_NULLPTR;
    }

    if (pUslOperPolicyDB != L7_NULLPTR)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslOperPolicyDB);
      pUslOperPolicyDB = L7_NULLPTR;
    }

    if (pUslShadowPolicyDB != L7_NULLPTR)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID, pUslShadowPolicyDB);
      pUslShadowPolicyDB = L7_NULLPTR;
    }
  
    pUslPolicyDBHandle = L7_NULLPTR;

    rc = L7_SUCCESS; 

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Invalidate the content of the USL Policy db
*
* @params   flags @{(input)} Type of database to be invalidated
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t usl_policy_db_invalidate(USL_DB_TYPE_t flags)
{
  L7_RC_t                            rc = L7_FAILURE;
  L7_uint32                          max;
  L7_uint32                          i;

  if (uslPolicyDbActive == L7_FALSE)
  {
    rc = L7_SUCCESS;
    return rc;
  }

  USL_POLICY_DB_LOCK_TAKE();

  do
  {
    /* Clear the operational table */
    if (flags & USL_OPERATIONAL_DB)
    {
      if (custom_policy_init() != BCM_E_NONE)  /* init custom bcmx layer */
      {
        L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, 
                "Custom policy init failed\n");
      }

      max = BROAD_MAX_POLICIES;
      for (i = 0; i < max; i++)
      {
        if (pUslOperPolicyDB[i].used)
        {
          hapiBroadPolicyRulesPurge(&pUslOperPolicyDB[i].policyInfo);
        }
      }
      memset(pUslOperPolicyDB, 0, sizeof(BROAD_USL_POLICY_t) * max);
    }

    /* Clear the shadow table */
    if (flags & USL_SHADOW_DB)
    {
      max = BROAD_MAX_POLICIES;
      for (i = 0; i < max; i++)
      {
        if (pUslShadowPolicyDB[i].used)
        {
          hapiBroadPolicyRulesPurge(&pUslShadowPolicyDB[i].policyInfo);
        }
      }
      memset(pUslShadowPolicyDB, 0, sizeof(BROAD_USL_POLICY_t) * max);
    }
    
    rc = L7_SUCCESS;

  } while (0);

  USL_POLICY_DB_LOCK_GIVE();

  return rc;
}

/*********************************************************************
* @purpose  Set the Policy db handle to Operational or Shadow table
*
* @param    dbType  {(input)} Type of db to set
*
* @returns  L7_SUCCESS - if all the ports were deleted
* @returns  L7_FAILURE - An error was encountered during deletion
*
*       
* @end
*********************************************************************/
L7_RC_t usl_policy_db_handle_set(USL_DB_TYPE_t dbType)
{
  L7_RC_t rc = L7_SUCCESS;

  /* If trying to set the dbHandle to Shadow tables when they are not allocated */
  if ((dbType == USL_SHADOW_DB) &&
      (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID) == L7_FALSE))
  {
    rc = L7_FAILURE;
    return rc;    
  }
 
  USL_POLICY_DB_LOCK_TAKE();

  if (dbType == USL_OPERATIONAL_DB)
  {
    pUslPolicyDBHandle = pUslOperPolicyDB;
  }
  else if (dbType == USL_SHADOW_DB)
  {
    pUslPolicyDBHandle = pUslShadowPolicyDB;
  }

  USL_POLICY_DB_LOCK_GIVE();

  return rc;

}

/*********************************************************************
* @purpose  Get the policy db handle based on db type
*
* @param    dbType           {(input)}   Type of db to get
*           *dbHandle        {{output}}  DB handle
*
* @returns  
*
* @notes    none
*
* @end
*********************************************************************/
void usl_policy_db_handle_get(USL_DB_TYPE_t dbType, BROAD_USL_POLICY_t **dbHandle)
{
  *dbHandle       = L7_NULLPTR;

  switch (dbType)
  {
    case USL_OPERATIONAL_DB:
      *dbHandle = pUslOperPolicyDB;
      break;

    case USL_SHADOW_DB:
      *dbHandle = pUslShadowPolicyDB;
      break;

    case USL_CURRENT_DB:
      *dbHandle = pUslPolicyDBHandle;
      break;

    default:
      break;
  }

  return;
}

void usl_dump_policy(L7_uint32 policyId, L7_BOOL filteredOutput, USL_DB_TYPE_t dbType)
{
  L7_uint32                     ruleId;
  BROAD_USL_POLICY_t           *pUslPolicyDBPtr;
  BROAD_POLICY_RULE_ENTRY_t    *rulePtr;

  if (uslPolicyDbActive == L7_FALSE)
  {
    sysapiPrintf("Policy database is not active\n");
    return;
  }

  if (policyId >= BROAD_MAX_POLICIES)
  {
    sysapiPrintf("Invalid policy ID\n");
    return;
  }

  USL_POLICY_DB_LOCK_TAKE();

  /* Default to displaying the operational DB, so this can be called from devshell w/o specifying the DB explicitly. */
  if (dbType == 0)
  {
    dbType = USL_OPERATIONAL_DB;
  }
  usl_policy_db_handle_get(dbType, &pUslPolicyDBPtr);
  if (pUslPolicyDBPtr == L7_NULL)
  {
    USL_POLICY_DB_LOCK_GIVE();
    return;
  }

  sysapiPrintf("Dumping Policy %s\n", (dbType == USL_OPERATIONAL_DB) ? USL_OPERATIONAL_DB_NAME : USL_SHADOW_DB_NAME);

  if (pUslPolicyDBPtr[policyId].used)
  {
    sysapiPrintf("\n");
    sysapiPrintf("   policyId:                    %d\n",   policyId);
    sysapiPrintf("   policyMode:                  %s\n",   policyPortModeStr[pUslPolicyDBPtr[policyId].policyMode]);
    sysapiPrintf("   policyType:                  %s\n",   policyTypeStr[pUslPolicyDBPtr[policyId].policyInfo.policyType]);
    sysapiPrintf("   policyStage:                 %s\n",   policyStageStr[pUslPolicyDBPtr[policyId].policyInfo.policyStage]);
    sysapiPrintf("   policyFlags:                 %d\n",   pUslPolicyDBPtr[policyId].policyInfo.policyFlags);
    sysapiPrintf("   strictEthTypes:              %d\n",   pUslPolicyDBPtr[policyId].policyInfo.strictEthTypes);
    sysapiPrintf("   ruleCount:                   %d\n",   pUslPolicyDBPtr[policyId].policyInfo.ruleCount);
    sysapiPrintf("   hashVal:                     0x%x\n", pUslPolicyDBPtr[policyId].hashVal);
    sysapiPrintf("\n");

    if (!filteredOutput)
    {
      rulePtr = pUslPolicyDBPtr[policyId].policyInfo.ruleInfo;
      ruleId = 0;
      while (rulePtr != L7_NULL)
      {
        sysapiPrintf("   Rule %d\n", ruleId);
        hapiBroadPolicyDebugRule(rulePtr, pUslPolicyDBPtr[policyId].policyInfo.policyStage);

        ruleId++;
        rulePtr = rulePtr->next;
      }
    }
  }

  USL_POLICY_DB_LOCK_GIVE();
}

void usl_dump_policy_db(USL_DB_TYPE_t dbType, L7_BOOL detailed)
{
  L7_uint32                     policyId;
  BROAD_USL_POLICY_t           *pUslPolicyDBPtr;
  L7_uint32                     policiesUsed;
  L7_uint32                     rulesUsed;

  if (uslPolicyDbActive == L7_FALSE)
  {
    sysapiPrintf("Policy database is not active\n");
    return;
  }
  else
  {
    sysapiPrintf("Policy database is active\n");
  }

  USL_POLICY_DB_LOCK_TAKE();

  usl_policy_db_handle_get(dbType, &pUslPolicyDBPtr);
  if (pUslPolicyDBPtr == L7_NULL)
  {
    USL_POLICY_DB_LOCK_GIVE();
    return;
  }

  sysapiPrintf("Dumping Policy %s\n", (dbType == USL_OPERATIONAL_DB) ? USL_OPERATIONAL_DB_NAME : USL_SHADOW_DB_NAME);

  policiesUsed      = 0;
  rulesUsed         = 0;

  for (policyId = 0; policyId < BROAD_MAX_POLICIES; policyId++)
  {
    if (pUslPolicyDBPtr[policyId].used)
    {
      policiesUsed      += 1;
      rulesUsed         += pUslPolicyDBPtr[policyId].policyInfo.ruleCount;

      usl_dump_policy(policyId, !detailed, dbType);
    }
  }

  sysapiPrintf("\n");
  sysapiPrintf("Number of policies used:                %d\n",       policiesUsed);
  sysapiPrintf("Number of rules used:                   %d\n",       rulesUsed);
  sysapiPrintf("\n");

  USL_POLICY_DB_LOCK_GIVE();
}

extern int policy_map_table_t_size();

void usl_policy_db_memory_info()
{
  sysapiPrintf("BROAD_MAX_POLICIES                 == %d\n", BROAD_MAX_POLICIES);
  sysapiPrintf("BROAD_MAX_POLICIES_PER_BCM_UNIT    == %d\n", BROAD_MAX_POLICIES_PER_BCM_UNIT);
  sysapiPrintf("BROAD_MAX_RULES_PER_POLICY         == %d\n", BROAD_MAX_RULES_PER_POLICY);
  sysapiPrintf("BROAD_MAX_SYSTEM_POLICIES          == %d\n", BROAD_MAX_SYSTEM_POLICIES);
  sysapiPrintf("sizeof(policy_map_table_t)         == %d\n", policy_map_table_t_size());
  sysapiPrintf("sizeof(BROAD_POLICY_ENTRY_t)       == %d\n", sizeof(BROAD_POLICY_ENTRY_t));
  sysapiPrintf("sizeof(BROAD_POLICY_RULE_ENTRY_t)  == %d\n", sizeof(BROAD_POLICY_RULE_ENTRY_t));
  sysapiPrintf("sizeof(BROAD_FIELD_ENTRY_t)        == %d\n", sizeof(BROAD_FIELD_ENTRY_t));
  sysapiPrintf("sizeof(BROAD_ACTION_ENTRY_t)       == %d\n", sizeof(BROAD_ACTION_ENTRY_t));
  sysapiPrintf("\n\n");

  sysapiPrintf("policy_map_table[]          == %d\n", BROAD_MAX_BCM_UNITS_PER_CPU * policy_map_table_t_size() * BROAD_MAX_POLICIES_PER_BCM_UNIT);
  sysapiPrintf("pUslOperPolicyDB[]          == %d\n", sizeof(BROAD_USL_POLICY_t) * BROAD_MAX_POLICIES);
}

/*********************************************************************
* @purpose  Get policy info from the USL policy DB
*
* @param     dbType      @{(input)}  Type of db
* @param     policy      @{(input)} the policy ID that is created
* @param    *policyInfo  @{(output)} the policy info
*
* @notes    It is up to the caller to free all nodes allocated for the
*           rule info. hapiBroadPolicyRulesPurge() can be used for this.
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_info_get(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo)
{
  int                           rv = BCM_E_NONE;
  BROAD_USL_POLICY_t           *dbHandle;

  if (policy >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  memset(policyInfo, 0, sizeof(*policyInfo));

  USL_POLICY_DB_LOCK_TAKE();
  usl_policy_db_handle_get(dbType, &dbHandle);
  if (dbHandle != L7_NULL)
  {
    if (dbHandle[policy].used)
    {
      rv = hapiBroadPolicyCopy(&dbHandle[policy].policyInfo, policyInfo);
      if (rv != BCM_E_NONE)
      {
        hapiBroadPolicyRulesPurge(policyInfo);
      }
    }
    else
    {
      rv = BCM_E_NOT_FOUND;
    }
  }
  else
  {
    rv = BCM_E_FAIL;
  }
  USL_POLICY_DB_LOCK_GIVE();
  return rv;
}

/*********************************************************************
* @purpose  Create a Policy in the DB using the specified policyId
*
* @param     dbType      @{(input)}  Type of db
* @param     policy      @{(input)} the policy ID that is created
* @param    *policyInfo  @{(input)} the policy info
* @param     hashVal      @{(input)} crc32 value from policyInfo
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_create_with_id(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy, BROAD_POLICY_ENTRY_t *policyInfo, L7_uint32 hashVal)
{
  int                                rv = BCM_E_NONE;
  BROAD_USL_POLICY_t                *dbHandle;

  USL_DEBUG_PRINT("usl_db_policy_create_with_id: Policy-%d\r\n", policy);

  USL_POLICY_DB_LOCK_TAKE();

  usl_policy_db_handle_get(dbType, &dbHandle);
  if (dbHandle != L7_NULL)
  {
    dbHandle[policy].used                   = L7_TRUE;
    dbHandle[policy].policyId               = policy;
    dbHandle[policy].policyMode             = PORT_MODE_DEFAULT;
    dbHandle[policy].hashVal                = hashVal;
  
    /* Free any rule buffers that may have already been allocated for this policy. */
    hapiBroadPolicyRulesPurge(&dbHandle[policy].policyInfo);
  
    rv = hapiBroadPolicyCopy(policyInfo, &dbHandle[policy].policyInfo);
  }
  else
  {
    rv = BCM_E_FAIL;
  }

  USL_POLICY_DB_LOCK_GIVE();
  return rv;
}

/*********************************************************************
* @purpose  Calculate the hash signature of a policy.
*
* @param    *policyInfo  @{(input)} the policy info
*
* @returns  32 bit CRC
*
* @end
*********************************************************************/
L7_uint32 usl_db_policy_hash_calc(BROAD_POLICY_ENTRY_t *policyInfo)
{
  L7_uint32                  crc = 0;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;

  /* Start off w/ base policy info. */
  /* Exclude the pointer to the first rule. */
  crc = crc32(crc, (void *)policyInfo, sizeof(BROAD_POLICY_ENTRY_t) - sizeof(BROAD_POLICY_RULE_ENTRY_t *));

  /* Next, iterate through each rule in the policy, updating the CRC. */
  rulePtr = policyInfo->ruleInfo;
  while (rulePtr != L7_NULL)
  {
    /* Exclude the pointer to the next rule. */
    crc = crc32(crc, (void *)rulePtr, sizeof(BROAD_POLICY_RULE_ENTRY_t) - sizeof(BROAD_POLICY_RULE_ENTRY_t *));

    rulePtr = rulePtr->next;
  }

  return crc;
}

/*********************************************************************
* @purpose  Create a Policy in the DB
*
* @param     dbType      @{(input)}  Type of db
* @param    *policy      @{(output)} the policy ID that is created
* @param    *policyInfo  @{(input)} the policy info
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_create(USL_DB_TYPE_t dbType, BROAD_POLICY_t *policy, BROAD_POLICY_ENTRY_t *policyInfo)
{
  L7_uint32                          i;
  L7_uint32                          freeEntry = BROAD_MAX_POLICIES;
  L7_uint32                          hashVal;
  int                                rv = BCM_E_NONE;

  hashVal = usl_db_policy_hash_calc(policyInfo);

  USL_DEBUG_PRINT("usl_db_policy_create: ");

  USL_POLICY_DB_LOCK_TAKE();
  /* First, check to see if we already know about this policy. 
     If so, just return the policy ID to the caller. 
     Note that the logic here always searches the operational DB.
     If a failover occurs, the shadow table will be empty as applications
     add new policies, so searching the 'current' table will always result
     in a 'not found' condition. Therefore we need to search the 
     operational DB in order to see if the policy was known before. If so,
     return the same policy ID to the caller. */
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    /* Check both the operational DB and the current DB (which may be the operational DB)
       to determine the first free entry. */
    if (!pUslPolicyDBHandle[i].used && 
        !pUslOperPolicyDB[i].used && 
        (freeEntry == BROAD_MAX_POLICIES))
    {
      freeEntry = i;
    }

    /* Only need to do the policy comparison during warm start, otherwise
       just find the first free entry. */
    if (usl_state_get() == USL_WARM_START_STATE)
    {
      /* Use the operational DB to determine if this policy is known. */
      if (pUslOperPolicyDB[i].used && (pUslOperPolicyDB[i].hashVal == hashVal))
      {
        freeEntry = i;
        USL_DEBUG_PRINT("Policy %d exists\r\n", i);
        break;
      }
    }
    else if (freeEntry < BROAD_MAX_POLICIES)
    {
      break;
    }
  }

  *policy = BROAD_POLICY_INVALID;

  if (freeEntry == BROAD_MAX_POLICIES)
  {
    USL_POLICY_DB_LOCK_GIVE();
    USL_DEBUG_PRINT("No available DB entries\r\n");
    return BCM_E_FULL;
  }

  rv = usl_db_policy_create_with_id(dbType, freeEntry, policyInfo, hashVal);
  if (rv == BCM_E_NONE)
  {
    *policy = freeEntry;
  }

  USL_POLICY_DB_LOCK_GIVE();
  return rv;
}

/*********************************************************************
* @purpose  Destroy a Policy in the DB
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_destroy(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy)
{
  uslDbElemInfo_t               elemInfo;
  BROAD_USL_POLICY_t           *dbHandle;
  int                           rv;

  if (policy >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  USL_DEBUG_PRINT("usl_db_policy_destroy: Policy-%d\r\n", policy);

  usl_policy_db_handle_get(dbType, &dbHandle);
  if (dbHandle != L7_NULL)
  {
    elemInfo.dbElem    = &dbHandle[policy];
    elemInfo.elemIndex = policy;
    rv = usl_delete_policy_db_elem(dbType, elemInfo);
  }
  else
  {
    rv = BCM_E_FAIL;
  }

  return rv;
}

/*********************************************************************
* @purpose  Apply a policy to all ports
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_apply_all(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy)
{
  int                           rv = BCM_E_NONE;
  BROAD_USL_POLICY_t           *dbHandle;

  if (policy >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  USL_DEBUG_PRINT("usl_db_policy_apply_all: Policy-%d\r\n", policy);

  USL_POLICY_DB_LOCK_TAKE();
  usl_policy_db_handle_get(dbType, &dbHandle);
  if (dbHandle != L7_NULL)
  {
    if (dbHandle[policy].used)
    {
      dbHandle[policy].policyMode = PORT_MODE_ALL;
    }
    else
    {
      rv = BCM_E_NOT_FOUND;
    }
  }
  else
  {
    rv = BCM_E_FAIL;
  }
  USL_POLICY_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Remove a policy from all ports
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_remove_all(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy)
{
  int                           rv = BCM_E_NONE;
  BROAD_USL_POLICY_t           *dbHandle;

  if (policy >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  USL_DEBUG_PRINT("usl_db_policy_remove_all: Policy-%d\r\n", policy);

  USL_POLICY_DB_LOCK_TAKE();
  usl_policy_db_handle_get(dbType, &dbHandle);
  if (dbHandle != L7_NULL)
  {
    if (dbHandle[policy].used)
    {
      dbHandle[policy].policyMode = PORT_MODE_NONE;
    }
    else
    {
      rv = BCM_E_NOT_FOUND;
    }
  }
  else
  {
    rv = BCM_E_FAIL;
  }
  USL_POLICY_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Set the status for the rule in policy
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
* @param    rule        @{(input)} rule ID
* @param    status      @{(input)} status information of the rule
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_rule_status_set(USL_DB_TYPE_t dbType, BROAD_POLICY_t policy, BROAD_POLICY_RULE_t rule, BROAD_RULE_STATUS_t status)
{
  int                           rv = BCM_E_NONE;
  BROAD_USL_POLICY_t           *dbHandle;
  BROAD_POLICY_RULE_ENTRY_t     *rulePtr;

  USL_DEBUG_PRINT("usl_db_policy_rule_status_set: Policy-%d Rule ID-%d\r\n", policy, rule);

  if (policy >= BROAD_MAX_POLICIES)
  {
    USL_DEBUG_PRINT("usl_db_policy_rule_status_set: Invalid Policy-%d\r\n", policy);
    return BCM_E_PARAM;
  }

  if(rule >= BROAD_MAX_RULES_PER_POLICY)
  {
    USL_DEBUG_PRINT("usl_db_policy_rule_status_set: Invalid rule-%d\r\n", rule);
    return BCM_E_PARAM;
  }

  USL_POLICY_DB_LOCK_TAKE();

 /* get the policyInfo from dbHandle and
  * set the ruleFlags 
  */
  usl_policy_db_handle_get(dbType, &dbHandle);
  if (dbHandle != L7_NULL)
  {
    if (dbHandle[policy].used)
    {
      if (rule >= dbHandle[policy].policyInfo.ruleCount)
      {
        USL_DEBUG_PRINT("usl_db_policy_rule_status_set: rule-%d not found in USL DB\r\n", rule);
        USL_POLICY_DB_LOCK_GIVE();
        return BCM_E_NOT_FOUND;
      }
      rulePtr = hapiBroadPolicyRulePtrGet(&dbHandle[policy].policyInfo, rule);
    }
    else
    {
      USL_DEBUG_PRINT("usl_db_policy_rule_status_set: Policy-%d not found in USL iDB\r\n", policy);
      USL_POLICY_DB_LOCK_GIVE();
      return BCM_E_NOT_FOUND;
    }
  }
  else
  {

    USL_DEBUG_PRINT("usl_db_policy_rule_status_set: dbHandle is not valid\r\n");
    USL_POLICY_DB_LOCK_GIVE();
    return BCM_E_FAIL;
  }

  if (rulePtr == L7_NULLPTR)
  {
    USL_DEBUG_PRINT("usl_db_policy_rule_status_set: Unable to get rulePtr "
                    "for rule-%d in USL DB\r\n", rule);
    USL_POLICY_DB_LOCK_GIVE();
    return BCM_E_FAIL;
  }

  /* if status is active set the ruleFlags otherwise clear the flag */
  if(status == BROAD_POLICY_RULE_STATUS_ACTIVE)
  {
    /* set the flag  to active*/
     rulePtr->ruleFlags |= BROAD_RULE_STATUS_ACTIVE;
  }
  else if(status == BROAD_POLICY_RULE_STATUS_INACTIVE)
  {
    /* clear the flag */
    rulePtr->ruleFlags &= ~(BROAD_RULE_STATUS_ACTIVE);
  }
  else
  {
    rv = BCM_E_PARAM;
  }

  if(rv != BCM_E_NONE)
  {
    USL_DEBUG_PRINT("usl_db_policy_rule_status_set: Unable to set "
                   "status information for Policy %d rule %d",policy,rule);
  }
  USL_POLICY_DB_LOCK_GIVE();
  return rv;
}

/*********************************************************************
* @purpose  Apply a policy to a port
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_port_apply(USL_DB_TYPE_t  dbType, 
                             BROAD_POLICY_t policyId,
                             bcmx_lport_t   port)
{
  return usl_portdb_policy_add(dbType, port, policyId);
}

/*********************************************************************
* @purpose  Remove a policy from a port
*
* @param    dbType      @{(input)} Type of db
* @param    policy      @{(input)} policy ID
* @param    port        @{(input)} port
*
* @returns  Defined by the Broadcom driver
*
* @end
*********************************************************************/
int usl_db_policy_port_remove(USL_DB_TYPE_t  dbType, 
                              BROAD_POLICY_t policyId,
                              bcmx_lport_t   port)
{
  return usl_portdb_policy_remove(dbType, port, policyId);
}

/*********************************************************************
* @purpose  Get the size of Policy Db Elem
*
* @params   none
*
* @returns  Size of db Elem
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usl_get_size_of_policy_db_elem(void)
{
  return (sizeof(BROAD_USL_POLICY_t));
}

/*********************************************************************
* @purpose  Print the contents of a Policy Db element in specified buffer
*
* @params   elem     {(input)}  Element to be printed
*           buffer   {(input)}  buffer pointer
*           size     {(input)}  size of buffer
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
void usl_print_policy_db_elem(void *item, L7_uchar8 *buffer, L7_uint32 size)
{
  BROAD_USL_POLICY_t  *pUslPolicyEntry = item;

  osapiSnprintf(buffer, size, "policyId: %d\n",          pUslPolicyEntry->policyId);
  osapiSnprintfcat(buffer, size, "policyMode: %d\n",     pUslPolicyEntry->policyMode);
  osapiSnprintfcat(buffer, size, "policyType: %d\n",     pUslPolicyEntry->policyInfo.policyType);
  osapiSnprintfcat(buffer, size, "policyStage: %d\n",    pUslPolicyEntry->policyInfo.policyStage);
  osapiSnprintfcat(buffer, size, "policyFlags: %d\n",    pUslPolicyEntry->policyInfo.policyFlags);
  osapiSnprintfcat(buffer, size, "strictEthTypes: %d\n", pUslPolicyEntry->policyInfo.strictEthTypes);
  osapiSnprintfcat(buffer, size, "ruleCount: %d\n",      pUslPolicyEntry->policyInfo.ruleCount);
  osapiSnprintfcat(buffer, size, "hashVal: 0x%x\n",      pUslPolicyEntry->hashVal);

  return;
}  

/*********************************************************************
* @purpose  Get info about an element from the Db
*
* @params   dbType        {(input)}  dbType to be searched
*           flags         {(input)}  EXACT or GET_NEXT
*           searchInfo    {(input)}  Information about element to be searched
*           elemInfo      {(output)} Storage for element & index
*
* @returns  BCM error code
*
* @notes    Caller must allocate memory to copy the db elem in
*           elemInfo.dbElem
*
* @end
*********************************************************************/
L7_int32 usl_get_policy_db_elem(USL_DB_TYPE_t     dbType, 
                                L7_uint32         flags,
                                uslDbElemInfo_t   searchElem,
                                uslDbElemInfo_t  *elemInfo)
{
  L7_int32                      rv              = BCM_E_NONE;
  BROAD_USL_POLICY_t           *dbHandle        = L7_NULLPTR;
  L7_BOOL                       foundEntry      = L7_FALSE;
  L7_uint32                     i;

  if (!uslPolicyDbActive)
  {
    return rv;    
  }

  /* Make sure caller has allocated memory in elemInfo.dbElem */
  if (elemInfo->dbElem == L7_NULLPTR)
  {
    rv = BCM_E_FAIL;
    return rv;    
  }

  USL_POLICY_DB_LOCK_TAKE();

  do
  {
    usl_policy_db_handle_get(dbType, &dbHandle);
    if (dbHandle == L7_NULLPTR)
    {
      rv = BCM_E_FAIL;
      break;    
    }

    if (flags == USL_DB_NEXT_ELEM)
    {
      for (i = searchElem.elemIndex + 1; i < BROAD_MAX_POLICIES; i++)
      {
        if (dbHandle[i].used)
        {
          /* Element found, copy in the user memory */
          memcpy(elemInfo->dbElem, &dbHandle[i], sizeof(BROAD_USL_POLICY_t));
          elemInfo->elemIndex = i;
          foundEntry          = L7_TRUE;
          break;
        }
      }
    }
    else
    {
      /* Search for the elem */
      if (searchElem.elemIndex < BROAD_MAX_POLICIES)
      {
        if (dbHandle[searchElem.elemIndex].used)
        {
          /* Element found, copy in the user memory */
          memcpy(elemInfo->dbElem, &dbHandle[searchElem.elemIndex], sizeof(BROAD_USL_POLICY_t));
          elemInfo->elemIndex = searchElem.elemIndex;
          foundEntry          = L7_TRUE;
        }
      }
    }

    if (foundEntry != L7_TRUE)
    {
      /* Element not found */
      rv = BCM_E_NOT_FOUND;
    }

  } while (0);
  
  USL_POLICY_DB_LOCK_GIVE();

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Policy Db
*
* @params   dbType   {(input)} 
*           elemInfo {(input)}
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_delete_policy_db_elem(USL_DB_TYPE_t   dbType, 
                                   uslDbElemInfo_t elemInfo)
{
  BROAD_USL_POLICY_t                *dbHandle;
  L7_uint32                          policy;

  if (!uslPolicyDbActive)
  {
    return BCM_E_NONE;    
  }

  policy = elemInfo.elemIndex;
  if (policy >= BROAD_MAX_POLICIES)
  {
    return BCM_E_PARAM;
  }

  usl_policy_db_handle_get(dbType, &dbHandle);
  if (dbHandle == L7_NULLPTR)
  {
    return BCM_E_FAIL;
  }

  USL_POLICY_DB_LOCK_TAKE();
  if (dbHandle[policy].used)
  {
    hapiBroadPolicyRulesPurge(&dbHandle[policy].policyInfo);

    dbHandle[policy].used = L7_FALSE;
  }
  USL_POLICY_DB_LOCK_GIVE();

  return BCM_E_NONE;
}

/*********************************************************************
* @purpose  Create a given element in Policy Db
*
* @params   dbItem {(input)}  Pointer to db item to be created
*
* @returns  BCM error code
*
* @notes    
*
* @end
*********************************************************************/
L7_int32 usl_create_policy_db_elem_bcm(void *dbItem)
{
  BROAD_POLICY_ENTRY_t        *policyInfo;
  BROAD_USL_POLICY_t          *uslPolicyInfo;
  int                          rv = BCM_E_NONE;
  int                          tmprv;

  if (!uslPolicyDbActive)
  {
    return BCM_E_NONE;    
  }

  uslPolicyInfo = dbItem;
  policyInfo    = &uslPolicyInfo->policyInfo;

  tmprv = usl_bcm_policy_create(uslPolicyInfo->policyId, policyInfo);
  if (tmprv < rv)
  {
    USL_DEBUG_PRINT("usl_create_policy_db_elem_bcm: usl_bcm_policy_create() returned %d\n", tmprv);
    rv = tmprv;
  }

  /* update port application mode, if specified */
  if (uslPolicyInfo->policyMode == PORT_MODE_ALL)
  {
    tmprv = usl_bcm_policy_apply_all(uslPolicyInfo->policyId);
    if (tmprv < rv)
    {
      USL_DEBUG_PRINT("usl_create_policy_db_elem_bcm: usl_bcm_policy_apply_all() returned %d\n", tmprv);
      rv = tmprv;
    }
  }
  else if (uslPolicyInfo->policyMode == PORT_MODE_NONE)
  {
    tmprv = usl_bcm_policy_remove_all(uslPolicyInfo->policyId);
    if (tmprv < rv)
    {
      USL_DEBUG_PRINT("usl_create_policy_db_elem_bcm: usl_bcm_policy_remove_all() returned %d\n", tmprv);
      rv = tmprv;
    }
  }

  return rv;
}

/*********************************************************************
* @purpose  Delete a given element from Policy Db
*
* @params   dbItem {(input)}  Pointer to db item to be deleted
*
* @returns  BCM error code
*
* @end
*********************************************************************/
L7_int32 usl_delete_policy_db_elem_bcm(void *dbItem)
{
  int                 rv, tmprv;
  BROAD_USL_POLICY_t *uslPolicyInfo;
  uslDbSyncFuncs_t    dbFuncTable;
  uslDbElemInfo_t     searchElem, operElem;
  L7_uint32           dbElemSize;
  usl_port_db_elem_t *operPortDbEntry;

  if (!uslPolicyDbActive)
  {
    return BCM_E_NONE;    
  }

  uslPolicyInfo = dbItem;

  rv = usl_bcm_policy_destroy(uslPolicyInfo->policyId);
  if (rv != BCM_E_NONE)
  {
    USL_DEBUG_PRINT("usl_delete_policy_db_elem_bcm: usl_bcm_policy_destroy() returned %d\n", rv);
  }

  /* Iterate through all ports in the port DB and ensure that
     none have the policy applied. */
  if (usl_db_sync_func_table_get(USL_PORT_DB_ID, &dbFuncTable) != L7_SUCCESS)
  {
    USL_LOG_MSG(USL_BCM_E_LOG,
                "usl_db_sync_func_table_get failed for dbId %s\n",
                usl_db_name_get(USL_PORT_DB_ID));
    rv = BCM_E_FAIL;    
    return rv;
  }

  searchElem.dbElem    = L7_NULLPTR;
  searchElem.elemIndex = USL_INVALID_DB_INDEX;

  operElem.dbElem      = L7_NULLPTR;
  operElem.elemIndex   = USL_INVALID_DB_INDEX;

  /* Get the size of the db element */
  dbElemSize = dbFuncTable.get_size_of_db_elem();
  
  if (dbFuncTable.alloc_db_elem)
  {
    searchElem.dbElem = dbFuncTable.alloc_db_elem();
    if (searchElem.dbElem == L7_NULLPTR)
    {
      LOG_ERROR(0);    
    }

    operElem.dbElem = dbFuncTable.alloc_db_elem();
    if (operElem.dbElem == L7_NULLPTR)
    {
      LOG_ERROR(0);    
    }
  }
  else
  {
    searchElem.dbElem = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                    dbElemSize);
    if (searchElem.dbElem == L7_NULLPTR)
    {
      LOG_ERROR(0);    
    }

    memset(searchElem.dbElem, 0, dbElemSize);

    operElem.dbElem = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                  dbElemSize);
    if (operElem.dbElem == L7_NULLPTR)
    {
      LOG_ERROR(0);    
    }

    memset(operElem.dbElem, 0, dbElemSize);
  }

  /* Walk through all the entries in the Operational Db */
  while ((dbFuncTable.get_db_elem(USL_OPERATIONAL_DB, USL_DB_NEXT_ELEM, 
                                  searchElem, &operElem)) == BCM_E_NONE)
  {
    /* Copy the returned element for next iteration */
    if (dbFuncTable.copy_db_elem)
    {
      dbFuncTable.copy_db_elem(searchElem.dbElem, operElem.dbElem);
    }
    else
    {
      memcpy(searchElem.dbElem, operElem.dbElem, dbElemSize);
    }

    memcpy(&(searchElem.elemIndex), &(operElem.elemIndex), 
             sizeof(operElem.elemIndex));

    operPortDbEntry = operElem.dbElem;

    tmprv = usl_db_policy_port_remove(USL_CURRENT_DB, uslPolicyInfo->policyId, operPortDbEntry->data.key.gport);
    if (tmprv < rv)
    {
      rv = tmprv;
      USL_DEBUG_PRINT("usl_delete_policy_db_elem_bcm: usl_db_policy_port_remove() returned %d\n", tmprv);
    }
  }

  if (dbFuncTable.free_db_elem)
  {
    dbFuncTable.free_db_elem(searchElem.dbElem);
    dbFuncTable.free_db_elem(operElem.dbElem);
  }
  else
  {
    osapiFree(L7_DRIVER_COMPONENT_ID,
              searchElem.dbElem);

    osapiFree(L7_DRIVER_COMPONENT_ID,
              operElem.dbElem);
  }

  return rv;
}

/*********************************************************************
* @purpose  Compare the shadow and oper db elem. If they differ then
*           program the USL BCM with shadow db info.

* @params   shadowDbElem  {(input)} Pointer to Shadow Db item
*           operDbElem    {(input)} Pointer to Oper Db item
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 usl_update_policy_db_elem_bcm(void *shadowDbItem, void *operDbItem)
{
  int                          rv = BCM_E_NONE;
  BROAD_USL_POLICY_t          *uslPolicyInfoShadow;
  BROAD_USL_POLICY_t          *uslPolicyInfoOper;

  if (!uslPolicyDbActive)
  {
    return BCM_E_NONE;    
  }

  uslPolicyInfoShadow = shadowDbItem;
  uslPolicyInfoOper   = operDbItem;

  /* If the two policies are not the same, delete the oper policy and install the
     shadow policy. */
  if ((uslPolicyInfoShadow->policyId                  != uslPolicyInfoOper->policyId)    ||
      (uslPolicyInfoShadow->policyMode                != uslPolicyInfoOper->policyMode)  ||
      (uslPolicyInfoShadow->policyInfo.policyType     != uslPolicyInfoOper->policyInfo.policyType)  ||
      (uslPolicyInfoShadow->policyInfo.policyStage    != uslPolicyInfoOper->policyInfo.policyStage) ||
      (uslPolicyInfoShadow->policyInfo.policyFlags    != uslPolicyInfoOper->policyInfo.policyFlags) ||
      (uslPolicyInfoShadow->policyInfo.strictEthTypes != uslPolicyInfoOper->policyInfo.strictEthTypes) ||
      (uslPolicyInfoShadow->policyInfo.ruleCount      != uslPolicyInfoOper->policyInfo.ruleCount)   ||
      (uslPolicyInfoShadow->hashVal                   != uslPolicyInfoOper->hashVal))
  {
    rv = usl_delete_policy_db_elem_bcm(operDbItem);
    if (rv != BCM_E_NONE)
    {
      USL_DEBUG_PRINT("usl_update_policy_db_elem_bcm: usl_delete_policy_db_elem_bcm() returned %d\n", rv);
    }
    rv = usl_create_policy_db_elem_bcm(shadowDbItem);
    if (rv != BCM_E_NONE)
    {
      USL_DEBUG_PRINT("usl_update_policy_db_elem_bcm: usl_create_policy_db_elem_bcm() returned %d\n", rv);
    }
  }
  else
  {
    /* Else, the policies are the same. In this case, we need to clear the counters
       associated with the policy. */
    rv = usl_bcm_policy_counter_clear(uslPolicyInfoOper->policyId);
  }

  return rv;
}

#ifdef L7_STACKING_PACKAGE
/*********************************************************************
* @purpose  Used to update all policies that have a 'trap to CPU' action
*           with the modid and modport of the new manager's CPU.
*           This function is called by each stack unit after
*           a failover occurs.
*
* @params   cpu_modid   {(input)} modid of new CPU port.
*           cpu_modport {(input)} modport of new CPU. 
*
* @returns  BCM error code
*
* @notes    none
*
* @end
*********************************************************************/
int usl_policy_db_dataplane_cleanup(L7_uint32 cpu_modid, L7_uint32 cpu_modport)
{
  int                           rv = BCM_E_NONE;
  L7_uint32                     policyId;
  L7_uint32                     maxPolicies;
  BROAD_USL_POLICY_t           *dbHandle;
  BROAD_POLICY_ENTRY_t         *policyInfo = L7_NULL;
  BROAD_POLICY_RULE_ENTRY_t    *rulePtr;
  BROAD_ACTION_ENTRY_t         *actionPtr;
  L7_uint32                     startTime, endTime;
  L7_uint32                     ruleId;

  USL_POLICY_DB_LOCK_TAKE();

  startTime = osapiTimeMillisecondsGet();

  do
  {
    usl_policy_db_handle_get(USL_OPERATIONAL_DB, &dbHandle);
    if (dbHandle == L7_NULL)
    {
      rv = BCM_E_FAIL;
      break;
    }

    maxPolicies = BROAD_MAX_POLICIES;
    for (policyId = 0; policyId < maxPolicies; policyId++)
    {
      if (dbHandle[policyId].used)
      {
        policyInfo = &dbHandle[policyId].policyInfo;
        rulePtr    = policyInfo->ruleInfo;
        ruleId     = 0;

        while (rulePtr != L7_NULL)
        {
          actionPtr = &rulePtr->actionInfo;
          if (BROAD_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_TRAP_TO_CPU))
          {
            usl_bcm_policy_dataplane_cleanup(policyId, ruleId, cpu_modid, cpu_modport);
          }

          ruleId++;
          rulePtr = rulePtr->next;
        }
      }
    }
  } while (0);

  endTime = osapiTimeMillisecondsGet();
  uslPolicyDataplaneCleanupTime += (endTime - startTime);
  uslPolicyDataplaneCleanupCount++;

  USL_POLICY_DB_LOCK_GIVE();

  return rv;
}

void usl_policy_db_dataplane_cleanup_debug_show()
{
  sysapiPrintf("uslPolicyDataplaneCleanupTime (ms) == %d\n", uslPolicyDataplaneCleanupTime);
  sysapiPrintf("uslPolicyDataplaneCleanupCount     == %d\n", uslPolicyDataplaneCleanupCount);
  sysapiPrintf("average                       (ms) == %d\n", uslPolicyDataplaneCleanupTime / uslPolicyDataplaneCleanupCount);
}
void usl_policy_db_dataplane_cleanup_debug_clear()
{
  uslPolicyDataplaneCleanupTime  = 0;
  uslPolicyDataplaneCleanupCount = 0;
}
#endif

/* debug */

void usl_policy_debug(L7_uint32 setting)
{
  if (setting > L7_NULL)
  {
    usl_debug = L7_TRUE;
  }
  else
  {
    usl_debug = L7_FALSE;
  }
}
