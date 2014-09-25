/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_policy.c
*
* @purpose   This file implements the Policy Manager.
*
* @component hapi
*
* @create    3/18/2005
*
* @author    robp
*
* @end
*
**********************************************************************/

#include "osapi_support.h"
#include "dtl_exports.h"
#include "broad_policy.h"
#include "broad_common.h"
#include "broad_l2_vlan.h"
#include "acl_exports.h"
#include "l7_usl_bcmx_policy.h"
#include "l7_usl_bcmx_port.h"
#include "l7_usl_policy_db.h"
#include "bcmx/bcmx_int.h"
#include "sysbrds.h"
#include "appl/stktask/topo_brd.h"
#include "bcmx/mirror.h"
#include "bcmx/port.h"
#include "feature.h"
#include "bcmx/switch.h"
#include "bcmx/field.h"


extern DAPI_t *dapi_g;
extern int custom_policy_init();


#define POLICY_NONE       0
#define POLICY_USED       1

/* Policy Table Semaphore
 */
void *policyTableSema = L7_NULLPTR;

static BROAD_POLICY_ENTRY_t *policyInfo = L7_NULL; /* scratch-pad to be used to organize policyInfo before sending to USL. */
static BROAD_POLICY_t        updatePolicyId;       /* Policy Id of the policy being updated. */

/* Macros for error checking. */
#define CHECK_POLICY(p)   {if (p >= BROAD_MAX_POLICIES)         return L7_ERROR;}
#define CHECK_RULE(r)     {if (r >= BROAD_MAX_RULES_PER_POLICY) return L7_ERROR;}
#define CHECK_FIELD(f)    {if (f >= BROAD_FIELD_SPECIAL_LAST)   return L7_ERROR;}
#define CHECK_ACTION(a)   {if (a >= BROAD_ACTION_LAST)          return L7_ERROR;}

/* forward decl */
void hapiBroadPolicyDebug(BROAD_POLICY_t policy);

/*********************************************************************
*
* @purpose Find a used action entry with specified flags.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadPolicyActionFind(BROAD_POLICY_RULE_ENTRY_t  *rulePtr,
                                         BROAD_POLICY_STAGE_t        policyStage,
                                         BROAD_POLICY_ACTION_SCOPE_t action_scope,
                                         BROAD_POLICY_ACTION_t       action,
                                         int                        *param0,
                                         int                        *param1,
                                         int                        *param2)
{
  BROAD_ACTION_ENTRY_t *actionPtr;

  if (action < BROAD_ACTION_LAST)
  {
    actionPtr = &rulePtr->actionInfo;

    if (actionPtr->actions[action_scope] & (1 << action))
    {
      hapiBroadPolicyActionParmsGet(actionPtr, policyStage, action, action_scope, param0, param1, param2);

      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Determines appropriate port for redirect/mirror actions.
*          Handles case where destination is physical port or LAG.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadPolicyPortSelect(DAPI_USP_t *usp, bcmx_lport_t *lport)
{
    BROAD_PORT_t          *hapiPortPtr;
    L7_RC_t                result = L7_SUCCESS;

    *lport = BCMX_LPORT_INVALID;

    hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

    if (L7_TRUE == hapiPortPtr->port_is_lag)
    {
        int          i;
        DAPI_PORT_t *dapiPortPtr;

        dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

        /* no logical handle for LAGs so just pick an arbitrary member port */
        if (hapiPortPtr->hapiModeparm.lag.numMembers > 0)
        {
            for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
            {
                if (dapiPortPtr->modeparm.lag.memberSet[i].inUse == L7_TRUE)
                    break;
            }

            if (i < L7_MAX_MEMBERS_PER_LAG)
                hapiPortPtr = HAPI_PORT_GET(&dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g);
            else
                result = L7_FAILURE;
        }
        else
        {
            /* LAG has no members */
            result = L7_NOT_EXIST;
        }
    }

    if (L7_SUCCESS == result)
        *lport = hapiPortPtr->bcmx_lport;

    return result;
}

/*********************************************************************
*
* @purpose Add an action to an existing rule.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadPolicyActionAdd(BROAD_POLICY_RULE_ENTRY_t  *rulePtr,
                                        BROAD_POLICY_STAGE_t        policyStage,
                                        BROAD_POLICY_ACTION_t       action,
                                        BROAD_POLICY_ACTION_SCOPE_t action_scope,
                                        L7_uint32                   param0,
                                        L7_uint32                   param1,
                                        L7_uint32                   param2)
{
    BROAD_ACTION_ENTRY_t *actionPtr;

    actionPtr = &rulePtr->actionInfo;

    if (action >= 32)
    {
      /* Programming error... need to update BROAD_ACTION_ENTRY_t to widen the 'actions' field. */
      LOG_ERROR(action);
    }
    actionPtr->actions[action_scope] |= (1 << action);

    switch (action)
    {
    case BROAD_ACTION_REDIRECT:
    case BROAD_ACTION_MIRROR:
      if (((action == BROAD_ACTION_REDIRECT) &&
                      (BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_MIRROR) ||
                       BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_TRAP_TO_CPU))) ||
          ((action == BROAD_ACTION_MIRROR) &&
                      BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_REDIRECT)))
      {
        /* These actions are mutually exclusive. */
        actionPtr->actions[action_scope] &= ~(1 << action);
        return L7_ERROR;
      }

      /*supported for ROBO*/
      if(hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
      {
       if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
       {
         LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
       }
      }
      actionPtr->u.ifp_parms.usp.unit = param0;
      actionPtr->u.ifp_parms.usp.slot = param1;
      actionPtr->u.ifp_parms.usp.port = param2;
      break;

    case BROAD_ACTION_TRAP_TO_CPU:
      if (BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_REDIRECT))
      {
        /* These actions are mutually exclusive. */
        actionPtr->actions[action_scope] &= ~(1 << action);
        return L7_ERROR;
      }
      break;

    case BROAD_ACTION_COPY_TO_CPU:
      if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        actionPtr->u.vfp_parms.cpu_cosq = param0;
      }
      break;

    case BROAD_ACTION_TS_TO_CPU:
      if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        actionPtr->u.vfp_parms.cpu_cosq = param0;
      }
      break;

    case BROAD_ACTION_SET_COSQ:
      actionPtr->u.ifp_parms.set_cosq[action_scope] = param0;
      break;

    case BROAD_ACTION_SET_TOS:
    case BROAD_ACTION_SET_DSCP:
      if (((action == BROAD_ACTION_SET_TOS)  && (actionPtr->actions[action_scope] & (1 << BROAD_ACTION_SET_DSCP))) ||
          ((action == BROAD_ACTION_SET_DSCP) && (actionPtr->actions[action_scope] & (1 << BROAD_ACTION_SET_TOS))))
      {
        /* These actions are mutually exclusive. */
        actionPtr->actions[action_scope] &= ~(1 << action);
        return L7_ERROR;
      }
      if (policyStage == BROAD_POLICY_STAGE_EGRESS)
      {
        actionPtr->u.efp_parms.set_dscp[action_scope] = param0;
      }
      else
      {
        actionPtr->u.ifp_parms.set_dscp[action_scope] = param0;
      }
      break;

    case BROAD_ACTION_SET_USERPRIO:
      if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        actionPtr->u.vfp_parms.set_userprio[action_scope] = param0;
      }
      else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
      {
        actionPtr->u.efp_parms.set_userprio[action_scope] = param0;
      }
      else
      {
        actionPtr->u.ifp_parms.set_userprio[action_scope] = param0;
      }
      break;

    /* PTin added */
    #if 1
    case BROAD_ACTION_SET_USERPRIO_INNERTAG:
      if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        actionPtr->u.vfp_parms.set_userprio_innertag[action_scope] = param0;
      }
      else if (policyStage == BROAD_POLICY_STAGE_EGRESS)
      {
        actionPtr->u.efp_parms.set_userprio_innertag[action_scope] = param0;
      }
      else
      {
        actionPtr->u.ifp_parms.set_userprio_innertag[action_scope] = param0;
      }
      break;
    #endif

    case BROAD_ACTION_SET_DROPPREC:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->u.ifp_parms.set_dropprec.conforming    = param0;
      actionPtr->u.ifp_parms.set_dropprec.exceeding     = param1;
      actionPtr->u.ifp_parms.set_dropprec.nonconforming = param2;
      break;

    case BROAD_ACTION_SET_OUTER_VID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        actionPtr->u.vfp_parms.set_ovid = param0;
      }
      else
      {
        actionPtr->u.efp_parms.set_ovid = param0;
      }
      break;

    case BROAD_ACTION_SET_INNER_VID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->u.efp_parms.set_ivid = param0;
      break;

    case BROAD_ACTION_ADD_OUTER_VID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->u.vfp_parms.add_ovid = param0;
      break;

    case BROAD_ACTION_ADD_INNER_VID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->u.vfp_parms.add_ivid = param0;
      break;

    case BROAD_ACTION_SET_CLASS_ID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      if (policyStage == BROAD_POLICY_STAGE_LOOKUP)
      {
        actionPtr->u.vfp_parms.set_class_id = param0;
      }
      else
      {
        actionPtr->u.ifp_parms.set_class_id = param0;
      }
      break;

    case BROAD_ACTION_SET_REASON_CODE:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->u.ifp_parms.set_reason = param0;
      break;

    default:
      /* Check for unexpected parameters */
      if ((param0 != 0) ||
          (param1 != 0) ||
          (param2 != 0))
      {
        LOG_ERROR(action);
      }
      break;
    }

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Update all actions in an existing rule. Some transformation of
*          parameters can only be done in the master so it is performed
*          here prior to being applied to hardware, possibly in a remote
*          unit / stack member.
*
* @end
*
*********************************************************************/
static void hapiBroadPolicyActionUpdate(BROAD_POLICY_ENTRY_t *policyInfo)
{
  int                        actionIdx;
  bcmx_lport_t               lport;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;
  BROAD_ACTION_ENTRY_t      *actionPtr;

  rulePtr = policyInfo->ruleInfo;
  while (rulePtr != L7_NULL)
  {
    actionPtr = &rulePtr->actionInfo;

    for (actionIdx = 0; actionIdx < BROAD_ACTION_LAST; actionIdx++)
    {
      if (BROAD_ACTION_IS_SPECIFIED(actionPtr, actionIdx))
      {
        /* Perform translation of any action params that must be done in the master. */
        switch (actionIdx)
        {
        case BROAD_ACTION_REDIRECT:
         /* RoBo's SDK code does not support using lport as parameter for
            redirect actions. It should by modid, port*/
         if(hapiBroadRoboCheck() != L7_TRUE)
         {
          if (hapiBroadPolicyPortSelect(&actionPtr->u.ifp_parms.usp, &lport) == L7_SUCCESS)
          {
            /* Use gport (i.e. lport) for parameter */
            actionPtr->u.ifp_parms.modid   = 0;
            actionPtr->u.ifp_parms.modport = lport;
          }
          else
          {
            /* dest port is invalid */
            actionPtr->u.ifp_parms.modid   = BCMX_LPORT_INVALID;
            actionPtr->u.ifp_parms.modport = BCMX_LPORT_INVALID;
          }
          break;
         }
        case BROAD_ACTION_MIRROR:
          if (hapiBroadPolicyPortSelect(&actionPtr->u.ifp_parms.usp, &lport) == L7_SUCCESS)
          {
            actionPtr->u.ifp_parms.modid  = BCM_GPORT_MODPORT_MODID_GET(lport);
            actionPtr->u.ifp_parms.modport = BCM_GPORT_MODPORT_PORT_GET(lport);

            if ((actionPtr->u.ifp_parms.modid == HAPI_BROAD_INVALID_MODID) ||
                (actionPtr->u.ifp_parms.modport   == HAPI_BROAD_INVALID_MODPORT))
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
                      "Failed to get modid/port for lport %x\n",
                      lport);
            }

          }
          else
          {
            /* dest port is invalid */
            actionPtr->u.ifp_parms.modid   = BCMX_LPORT_INVALID;
            actionPtr->u.ifp_parms.modport = BCMX_LPORT_INVALID;
          }
          break;

        default:
          /* no translation needed */
          break;
        }
      }
    }

    rulePtr = rulePtr->next;
  }
}

/*********************************************************************
*
* @purpose Combine COS and VID into a single field. This function
*          allows both COS and VID to be specified independently.
*          Also, it works for both INNER and OUTER tags.
*
* @end
*
*********************************************************************/
static void hapiBroadPolicyVidCosCombine(BROAD_FIELD_ENTRY_t       *fieldInfo,
                                         BROAD_POLICY_FIELD_t       destinationField,
                                         BROAD_POLICY_FIELD_t       sourceField,
                                         L7_uchar8                 *value,
                                         L7_uchar8                 *mask)
{
    L7_ushort16 cosVidValue;
    L7_ushort16 cosVidMask;
    L7_uint32   cosVidLen;

    cosVidValue = *(L7_ushort16*)hapiBroadPolicyFieldValuePtr(fieldInfo, destinationField);
    cosVidMask  = *(L7_ushort16*)hapiBroadPolicyFieldMaskPtr(fieldInfo, destinationField);

    if ((BROAD_FIELD_OVID == sourceField) || (BROAD_FIELD_IVID == sourceField))
    {
        cosVidValue &= 0xE000;                           /* keep cos   */
        cosVidMask  &= 0xE000;
        cosVidValue |= (*(L7_ushort16*)value & 0x0FFF);  /* update vid */
        cosVidMask  |= (*(L7_ushort16*)mask  & 0x0FFF);

        cosVidLen = sizeof(L7_ushort16);   /* copy COS + VID */
    }
    else  /* OCOS, ICOS */
    {
        cosVidValue &= 0x0FFF;                           /* keep vid   */
        cosVidMask  &= 0x0FFF;
        cosVidValue |= (*(L7_uchar8*)value << 13);       /* update cos */
        cosVidMask  |= (*(L7_uchar8*)mask  << 13);

        cosVidLen = sizeof(L7_ushort16);   /* copy COS + VID */
    }

    memcpy(hapiBroadPolicyFieldValuePtr(fieldInfo, destinationField), &cosVidValue, cosVidLen);
    memcpy(hapiBroadPolicyFieldMaskPtr(fieldInfo, destinationField),  &cosVidMask,  cosVidLen);
}

/*********************************************************************
*
* @purpose Gets the rule pointer for the given rule index
*
* @end
*
*********************************************************************/
BROAD_POLICY_RULE_ENTRY_t *hapiBroadPolicyRulePtrGet(BROAD_POLICY_ENTRY_t *policyInfo, L7_uint32 ruleIdx)
{
  L7_uint32 i;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;

  rulePtr = policyInfo->ruleInfo;
  i = 0;
  while (rulePtr != L7_NULL)
  {
    if (i == ruleIdx)
    {
      break;
    }

    i++;
    rulePtr = rulePtr->next;
  }

  return rulePtr;
}

/*********************************************************************
*
* @purpose Initialize the Policy Manager data structures upon IPL.
*          After IPL it is expected that each policy is explicitly
*          destroyed after it is no longer needed.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyInit()
{
    policyTableSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (L7_NULLPTR == policyTableSema)
    {
        LOG_ERROR(0);
        return L7_FAILURE;
    }

    policyInfo = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(*policyInfo));
    if (L7_NULLPTR == policyInfo)
    {
      LOG_ERROR(0);
      return L7_FAILURE;
    }

    /* PTin added: Field processor */
    #if 1
    if (bcmx_field_init() != BCM_E_NONE)
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("Error initializing FFP!\n");
    }
    else
    {
      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("FFP initialized!\n");
    }
    #endif

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Reinitialize the policy manager and underlying layers.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyReInit()
{
    int     rv;
    L7_RC_t result = L7_SUCCESS;

    rv = custom_policy_init();
    if (rv != BCM_E_NONE)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "custom_policy_init() returned %d\n", rv);
      result = L7_FAILURE;
    }

    return result;
}


/*********************************************************************
*
* @purpose Create a new policy with no rules.
*          Once a policy is created using this function,
*          the caller must commit or cancel the policy in order to
*          release the semaphore used to protect the policy resources.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyCreate(BROAD_POLICY_TYPE_t type)
{
    /* Take the semaphore here. Give it back on a commit, or cancel. */
    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    hapiBroadPolicyRulesPurge(policyInfo);

    memset(policyInfo, 0, sizeof(*policyInfo));

    policyInfo->policyType = type;
    /* Default stage is INGRESS */
    policyInfo->policyStage = BROAD_POLICY_STAGE_INGRESS;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("Creating policy \n");

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Cancels the creation of a policy. This API can be used
*          for error paths where an error occurs while building
*          up the policy.
*
* @end
*
*********************************************************************/
void hapiBroadPolicyCreateCancel()
{
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
      sysapiPrintf("Canceled policy\n");

  hapiBroadPolicyRulesPurge(policyInfo);

  osapiSemaGive(policyTableSema);
}

/*********************************************************************
*
* @purpose Designates which stage the policy should be applied to.
*          This function should be called before the policy is committed.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_t policyStage)
{
    if (policyStage >= BROAD_POLICY_STAGE_COUNT)
    {
      return L7_ERROR;
    }

    policyInfo->policyStage = policyStage;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("Setting stage for policy to %d\n", policyStage);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Delete an existing policy by removing it from all units.
*          USL is used as it works for both stacking and non-stacking,
*          and is more efficient in the stacking case.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyDelete(BROAD_POLICY_t policy)
{
    L7_RC_t                         result = L7_SUCCESS;
    int                             rv;

    CHECK_POLICY(policy);

    /* remove policy from all units as it is no longer used */
    rv = usl_bcmx_policy_destroy(policy);
    if (BCM_E_NONE != rv)
        result = L7_FAILURE;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("Deleted policy %d\n", policy);

    return result;
}

/*********************************************************************
*
* @purpose Add a new rule to a policy. By default this rule will match
*          all packets and take no actions. The user must add qualifiers,
*          actions, meters, etc to perform as expected.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyPriorityRuleAdd(BROAD_POLICY_RULE_t         *rule,
                                       BROAD_POLICY_RULE_PRIORITY_t priority)
{
    int                             i;
    BROAD_POLICY_RULE_ENTRY_t      *rulePtr, *tmpRulePtr;
    L7_uchar8                      *maskPtr;

    if (policyInfo->ruleCount >= BROAD_MAX_RULES_PER_POLICY)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "Rule count exceeded.\n");
        return L7_FAILURE;
    }

    if(priority > BROAD_POLICY_RULE_PRIORITY_HIGHEST)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
              "Invalid priority.\n");
      return L7_FAILURE;
    }

    /* Allocate a rule. */
    rulePtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(*rulePtr));
    if (rulePtr == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "Insufficient memory.\n");
      return L7_FAILURE;
    }

    *rule = (policyInfo->ruleCount)++;

    memset(rulePtr, 0, sizeof(BROAD_POLICY_RULE_ENTRY_t));

    /* clear field qualification for this rule */
    for (i = 0; i < BROAD_FIELD_LAST; i++)
    {
        hapiBroadPolicyFieldFlagsSet(&rulePtr->fieldInfo, i, BROAD_FIELD_NONE);

        /* default unspecified fields to "mask all" */
        memset(hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, i), FIELD_MASK_ALL, hapiBroadPolicyFieldSize(i));
        maskPtr = hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, i);
        if (maskPtr != L7_NULL)
        {
          memset(maskPtr,  FIELD_MASK_ALL, hapiBroadPolicyFieldSize(i));
        }
    }

    /* clear actions for this rule */
    memset(&rulePtr->actionInfo, 0, sizeof(rulePtr->actionInfo));

    /* Set the ruleFlags to BROAD_RULE_STATUS_ACTIVE on default during rule creation */
    rulePtr->ruleFlags |= BROAD_RULE_STATUS_ACTIVE;
    rulePtr->priority = priority;

    /* PTin added: Policer/Counter */
    rulePtr->meterSrcEntry = *rule;

    /* Add rule to policy */
    if (policyInfo->ruleInfo == L7_NULL)
    {
      policyInfo->ruleInfo = rulePtr;
    }
    else
    {
      tmpRulePtr = policyInfo->ruleInfo;
      while (tmpRulePtr->next != L7_NULL)
      {
        tmpRulePtr = tmpRulePtr->next;
      }
      tmpRulePtr->next = rulePtr;
    }
    rulePtr->next = L7_NULL;

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a new rule to a policy. Uses the default rule priority.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleAdd(BROAD_POLICY_RULE_t *rule)
{
    return hapiBroadPolicyPriorityRuleAdd(rule, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
}

/*********************************************************************
*
* @purpose Create a new rule and copy qualifiers, actions, meters, etc
*          from an existing rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleCopy(BROAD_POLICY_RULE_t  oldRule,
                                BROAD_POLICY_RULE_t *newRule)
{
    BROAD_POLICY_RULE_ENTRY_t      *oldRulePtr;
    BROAD_POLICY_RULE_ENTRY_t      *newRulePtr = L7_NULL;
    BROAD_POLICY_RULE_ENTRY_t      *tmpRulePtr;

    CHECK_RULE(oldRule);

    if (policyInfo->ruleCount >= BROAD_MAX_RULES_PER_POLICY)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "Rule count exceeded.\n");
        return L7_FAILURE;
    }

    if (oldRule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }

    /* Find the old rule. */
    oldRulePtr = hapiBroadPolicyRulePtrGet(policyInfo, oldRule);
    if (oldRulePtr == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "Rule not found.\n");
      return L7_FAILURE;
    }

    /* Allocate a rule. */
    newRulePtr = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(*newRulePtr));
    if (newRulePtr == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "Insufficient memory.\n");
      return L7_FAILURE;
    }

    *newRule = (policyInfo->ruleCount)++;

    /* Copy all the fields/actions from old to new rule. Note that meters use
     * shallow-copies, i.e. they are shared.
     */
    memcpy(newRulePtr, oldRulePtr, sizeof(BROAD_POLICY_RULE_ENTRY_t));

    if (oldRulePtr->ruleFlags & BROAD_METER_SPECIFIED)
    {
        /* indicate meter is shared with existing entry */
        newRulePtr->meterSrcEntry    = oldRule;
        newRulePtr->ruleFlags |= BROAD_METER_SHARED;
    }
    else if (oldRulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
    {
        /* indicate counter is shared with existing entry */
        newRulePtr->meterSrcEntry     = oldRule;
        newRulePtr->ruleFlags |= BROAD_COUNTER_SHARED;
    }

    /* Add rule to policy */
    if (policyInfo->ruleInfo == L7_NULL)
    {
      policyInfo->ruleInfo = newRulePtr;
    }
    else
    {
      tmpRulePtr = policyInfo->ruleInfo;
      while (tmpRulePtr->next != L7_NULL)
      {
        tmpRulePtr = tmpRulePtr->next;
      }
      tmpRulePtr->next = newRulePtr;
    }
    newRulePtr->next = L7_NULL;
    
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Adds qualification criteria to a rule. The user is expected
*          to provide a value and mask of the appropriate size for the
*          field.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleQualifierAdd(BROAD_POLICY_RULE_t  rule,
                                        BROAD_POLICY_FIELD_t field,
                                        L7_uchar8*           value,
                                        L7_uchar8*           mask)
{
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_uchar8                  *maskPtr;

    CHECK_RULE(rule);
    CHECK_FIELD(field);

    if (rule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
    if (rulePtr == L7_NULL)
    {
      return L7_ERROR;
    }

    switch (field)
    {
    case BROAD_FIELD_OVID:
    case BROAD_FIELD_OCOS:
        hapiBroadPolicyVidCosCombine(&rulePtr->fieldInfo, BROAD_FIELD_OVID, field, value, mask);

        hapiBroadPolicyFieldFlagsSet(&rulePtr->fieldInfo, BROAD_FIELD_OVID, BROAD_FIELD_SPECIFIED);   /* update field qualification for this rule */
        break;
    case BROAD_FIELD_IVID:
    case BROAD_FIELD_ICOS:
        hapiBroadPolicyVidCosCombine(&rulePtr->fieldInfo, BROAD_FIELD_IVID, field, value, mask);

        hapiBroadPolicyFieldFlagsSet(&rulePtr->fieldInfo, BROAD_FIELD_IVID, BROAD_FIELD_SPECIFIED);   /* update field qualification for this rule */
        break;
    case BROAD_FIELD_MACDA:
    case BROAD_FIELD_MACSA:
    case BROAD_FIELD_SIP:
    case BROAD_FIELD_DIP:
    case BROAD_FIELD_DSCP:
        /* Disallow adding this field if either IP6_SRC or IP6_DST are already included */
        /* This is because these fields share the same memory space in BROAD_FIELD_ENTRY_t. This
           restriction can be removed if BROAD_FIELD_ENTRY_t is reorganized such that these
           fields do not share memory space. */
        if ((hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_IP6_SRC) == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_IP6_DST) == BROAD_FIELD_SPECIFIED))
        {
          return L7_FAILURE;
        }

        memcpy(hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, field), value, hapiBroadPolicyFieldSize(field));
        maskPtr = hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, field);
        if (maskPtr != L7_NULL)
        {
          memcpy(maskPtr, mask,  hapiBroadPolicyFieldSize(field));
        }

        hapiBroadPolicyFieldFlagsSet(&rulePtr->fieldInfo, field, BROAD_FIELD_SPECIFIED);   /* update field qualification for this rule */
        break;

    case BROAD_FIELD_IP6_SRC:
    case BROAD_FIELD_IP6_DST:
        /* Disallow adding this field if MACDA, MACSA, SIP, or DIP are already included */
        /* This is because these fields share the same memory space in BROAD_FIELD_ENTRY_t. This
           restriction can be removed if BROAD_FIELD_ENTRY_t is reorganized such that these
           fields do not share memory space. */
        if ((hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_MACDA) == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_MACSA) == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_SIP)   == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_DIP)   == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_DSCP)  == BROAD_FIELD_SPECIFIED))
        {
          return L7_FAILURE;
        }
        /* fall through */

    default:
        if (field < BROAD_FIELD_LAST)
        {
            memcpy(hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, field), value, hapiBroadPolicyFieldSize(field));
            maskPtr = hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, field);
            if (maskPtr != L7_NULL)
            {
              memcpy(maskPtr, mask,  hapiBroadPolicyFieldSize(field));
            }

            hapiBroadPolicyFieldFlagsSet(&rulePtr->fieldInfo, field, BROAD_FIELD_SPECIFIED);   /* update field qualification for this rule */
        }
        else
        {
            /* special bit field not handled properly */
            return L7_FAILURE;
        }
        break;
    }

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add action for non-metered or GREEN packets that match the
*          rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleActionAdd(BROAD_POLICY_RULE_t   rule,
                                     BROAD_POLICY_ACTION_t action,
                                     L7_uint32             param0,
                                     L7_uint32             param1,
                                     L7_uint32             param2)
{
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result;

    CHECK_RULE(rule);
    CHECK_ACTION(action);

    if (rule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
    if (rulePtr == L7_NULL)
    {
      return L7_ERROR;
    }

    result = hapiBroadPolicyActionAdd(rulePtr, policyInfo->policyStage, action, BROAD_POLICY_ACTION_CONFORMING, param0, param1, param2);

    return result;
}

/*********************************************************************
*
* @purpose Add action for metered YELLOW packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleExceedActionAdd(BROAD_POLICY_RULE_t     rule,
                                           BROAD_POLICY_ACTION_t   action,
                                           L7_uint32               param0,
                                           L7_uint32               param1,
                                           L7_uint32               param2)
{
  BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
  L7_RC_t                     result;

  CHECK_RULE(rule);
  CHECK_ACTION(action);

  if (rule >= policyInfo->ruleCount)
  {
      return L7_ERROR;
  }

  rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
  if (rulePtr == L7_NULL)
  {
    return L7_ERROR;
  }

  result = hapiBroadPolicyActionAdd(rulePtr, policyInfo->policyStage, action, BROAD_POLICY_ACTION_EXCEEDING, param0, param1, param2);

  return result;
}

/*********************************************************************
*
* @purpose Get action for metered YELLOW packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleExceedActionGet(BROAD_POLICY_RULE_t     rule,
                                           BROAD_POLICY_ACTION_t   action,
                                           L7_uint32              *param0,
                                           L7_uint32              *param1,
                                           L7_uint32              *param2)
{
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result;

    CHECK_RULE(rule);

    if (rule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
    if (rulePtr == L7_NULL)
    {
      return L7_ERROR;
    }

    result = hapiBroadPolicyActionFind(rulePtr, policyInfo->policyStage, BROAD_POLICY_ACTION_EXCEEDING, action, param0, param1, param2);

    return result;
}

/*********************************************************************
*
* @purpose Add action for metered RED packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleNonConfActionAdd(BROAD_POLICY_RULE_t     rule,
                                            BROAD_POLICY_ACTION_t   action,
                                            L7_uint32               param0,
                                            L7_uint32               param1,
                                            L7_uint32               param2)
{
  BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
  L7_RC_t                     result;

  CHECK_RULE(rule);
  CHECK_ACTION(action);

  if (rule >= policyInfo->ruleCount)
  {
      return L7_ERROR;
  }

  rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
  if (rulePtr == L7_NULL)
  {
    return L7_ERROR;
  }

  result = hapiBroadPolicyActionAdd(rulePtr, policyInfo->policyStage, action, BROAD_POLICY_ACTION_NONCONFORMING, param0, param1, param2);

  return result;
}


/*********************************************************************
*
* @purpose Get action for metered RED packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleNonConfActionGet(BROAD_POLICY_RULE_t     rule,
                                            BROAD_POLICY_ACTION_t   action,
                                            L7_uint32              *param0,
                                            L7_uint32              *param1,
                                            L7_uint32              *param2)
{
  BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
  L7_RC_t                     result;

  CHECK_RULE(rule);

  if (rule >= policyInfo->ruleCount)
  {
      return L7_ERROR;
  }

  rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
  if (rulePtr == L7_NULL)
  {
    return L7_ERROR;
  }

  result = hapiBroadPolicyActionFind(rulePtr, policyInfo->policyStage, BROAD_POLICY_ACTION_NONCONFORMING, action, param0, param1, param2);

  return result;
}

/*********************************************************************
*
* @purpose Clear all actions associated with a specific rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleActionClearAll(BROAD_POLICY_RULE_t   rule)
{
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_RULE(rule);

    if (rule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
    if (rulePtr == L7_NULL)
    {
      return L7_ERROR;
    }

    memset(&rulePtr->actionInfo, 0, sizeof(rulePtr->actionInfo));

    return result;
}

/*********************************************************************
*
* @purpose Add a meter to an existing rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleMeterAdd(BROAD_POLICY_RULE_t     rule,
                                    BROAD_METER_ENTRY_t    *meterInfo)
{
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;

    CHECK_RULE(rule);

    if (rule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }

    /* validate meter info */
    if ((0 == meterInfo->pir) && (0 == meterInfo->pbs))
    {
        /* if peak not specified then use committed */
        meterInfo->pir = meterInfo->cir;
        meterInfo->pbs = meterInfo->cbs;
    }
    else
    if ((meterInfo->cir > meterInfo->pir) || (meterInfo->cbs > meterInfo->pbs))
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                "Invalid meter info specified: cir %d, cbs %d, pir %d, pbs %d\n",
                meterInfo->cir, meterInfo->cbs, meterInfo->pir, meterInfo->pbs);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
    if (rulePtr == L7_NULL)
    {
      return L7_ERROR;
    }

    if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
    {
        /* meters and counters are mutually exclusive */
        return L7_ERROR;
    }

    rulePtr->ruleFlags |= BROAD_METER_SPECIFIED;
    memcpy(&rulePtr->policer.policerInfo, meterInfo, sizeof(*meterInfo));   /* PTin modified: SDK 6.3.0 */

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a counter to existing rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleCounterAdd(BROAD_POLICY_RULE_t  rule,
                                      BROAD_COUNTER_MODE_t mode)
{
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;

    CHECK_RULE(rule);

    if (rule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
    if (rulePtr == L7_NULL)
    {
      return L7_ERROR;
    }

    if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
    {
        /* meters and counters are mutually exclusive */
        return L7_ERROR;
    }

    rulePtr->ruleFlags               |= BROAD_COUNTER_SPECIFIED;
    rulePtr->counter.counterInfo.mode = mode;       /* PTin modified: SDK 6.3.0 */

    return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Add a status to rule. info
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleStatusAdd(BROAD_POLICY_RULE_t  rule,
                                     BROAD_RULE_STATUS_t status)
{
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    CHECK_RULE(rule);

    if (rule >= policyInfo->ruleCount)
    {
        return L7_ERROR;
    }
   
    rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, rule);
    if (rulePtr == L7_NULL)
    {
      return L7_ERROR;
    }

    if (status == BROAD_POLICY_RULE_STATUS_ACTIVE)
    {
        rulePtr->ruleFlags |= BROAD_RULE_STATUS_ACTIVE;
        return L7_SUCCESS;
    }
    else if(status == BROAD_POLICY_RULE_STATUS_INACTIVE)
    {
        /* do not set the flag*/
        rulePtr->ruleFlags &= ~(BROAD_RULE_STATUS_ACTIVE);
        return L7_SUCCESS;    
    }
    else
    {
      /* status  can either be ACTIVE/INACTIVATE */
      return L7_FAILURE;
    }
}

/*********************************************************************
*
* @purpose Retrieve stats for a rule in a policy.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStatsGet(BROAD_POLICY_t        policy,
                                L7_uint32             ruleId,
                                BROAD_POLICY_STATS_t *stat)
{
    L7_RC_t                     result = L7_SUCCESS;
    int                         rv;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    /* Free any rules already allocated before calling usl_bcmx_policy_info_get(). */
    hapiBroadPolicyRulesPurge(policyInfo);

    if (usl_bcmx_policy_info_get(policy, policyInfo) == BCM_E_NONE)
    {
      if (ruleId < policyInfo->ruleCount)
      {
        rulePtr = hapiBroadPolicyRulePtrGet(policyInfo, ruleId);
        if (rulePtr == L7_NULL)
        {
          /* Free any rules allocated by usl_bcmx_policy_info_get(). */
          hapiBroadPolicyRulesPurge(policyInfo);
          osapiSemaGive(policyTableSema);
          return L7_ERROR;
        }

        if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
        {
            /* metered mode */
            stat->meter = L7_TRUE;
            stat->statMode.meter.in_prof  = 0;
            stat->statMode.meter.out_prof = 0;
        }
        else
        {
            /* counted mode */
            stat->meter = L7_FALSE;
            stat->statMode.counter.count = 0;     /* PTin modified: SDK 6.3.0 */
        }

        /* Free any rules allocated by usl_bcmx_policy_info_get(). */
        hapiBroadPolicyRulesPurge(policyInfo);
        osapiSemaGive(policyTableSema);

        rv = usl_bcmx_policy_stats_get(policy, ruleId, stat);
        if (BCM_E_NONE != rv)
            result = L7_FAILURE;
      }
      else
      {
        hapiBroadPolicyRulesPurge(policyInfo);
        osapiSemaGive(policyTableSema);
        result = L7_ERROR;
      }
    }
    else
    {
      osapiSemaGive(policyTableSema);
      result = L7_ERROR;
    }

    return result;
}

/*********************************************************************
*
* @purpose Commit a policy to hardware. Any actions that are dynamic
*          in nature are resolved at this time, e.g. redir to USP.
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyCommit(BROAD_POLICY_t *policy)
{
    L7_RC_t result = L7_SUCCESS;
    int     rv = BCM_E_NONE;

    /* update any actions in the rule that require translation */
    hapiBroadPolicyActionUpdate(policyInfo);

    rv = usl_bcmx_policy_create(policy, policyInfo);

    hapiBroadPolicyRulesPurge(policyInfo);

    osapiSemaGive(policyTableSema);

    if (BCM_E_NONE != rv)
    {
      result = L7_FAILURE;
    }

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
    {
      sysapiPrintf("Committed policy %d rv = %d\n", *policy, rv);
      hapiBroadPolicyDebug(*policy);
    }

    return result;
}

/*********************************************************************
*
* @purpose Recommit a policy to hardware. Any actions that are dynamic
*          in nature are resolved at this time, e.g. redir to USP.
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRecommit(BROAD_POLICY_t policy)
{
    L7_RC_t result = L7_SUCCESS;
    int     rv;

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    /* Free any rules already allocated before calling usl_bcmx_policy_info_get(). */
    hapiBroadPolicyRulesPurge(policyInfo);

    if (usl_bcmx_policy_info_get(policy, policyInfo) == BCM_E_NONE)
    {
      /* update any actions in the rule that require translation */
      hapiBroadPolicyActionUpdate(policyInfo);

      rv = usl_bcmx_policy_recommit(policy, policyInfo);
      if (BCM_E_NONE != rv)
      {
        /* clean up */
        usl_bcmx_policy_destroy(policy);
        result = L7_FAILURE;
      }

      if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
      {
        sysapiPrintf("Recommitted policy %d rv = %d\n", policy, rv);
        hapiBroadPolicyDebug(policy);
      }
    }
    else
    {
      result = L7_FAILURE;
    }

    /* Free any rules allocated by usl_bcmx_policy_info_get(). */
    hapiBroadPolicyRulesPurge(policyInfo);

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Starts the process of updating an existing policy. Once
*          an update has successfully started, the caller must
*          call either hapiBroadPolicyUpdateCancel() or
*          hapiBroadPolicyUpdateFinish().
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyUpdateStart(BROAD_POLICY_t policy)
{
  L7_RC_t result = L7_SUCCESS;
  int     rv;

  osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

  /* Free any rules already allocated before calling usl_bcmx_policy_info_get(). */
  hapiBroadPolicyRulesPurge(policyInfo);

  rv = usl_bcmx_policy_info_get(policy, policyInfo);

  if (rv == BCM_E_NONE)
  {
    updatePolicyId = policy;
  }
  else
  {
    result = L7_FAILURE;
    osapiSemaGive(policyTableSema);
  }

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
  {
    sysapiPrintf("Updating policy %d, rv = %d\n", policy, rv);
  }

  return result;
}

/*********************************************************************
*
* @purpose Cancels the process of updating an existing policy.
*          This API must be used to cancel the update process
*          if any errors occur while modifying the policy.
*
*
* @end
*
*********************************************************************/
void hapiBroadPolicyUpdateCancel()
{
  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
      sysapiPrintf("Canceled policy update\n");

  hapiBroadPolicyRulesPurge(policyInfo);

  osapiSemaGive(policyTableSema);
}

/*********************************************************************
*
* @purpose Finishes the process of updating an existing policy.
*          This call will recommit the policy to the hardware with
*          the update policy info.
*
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyUpdateFinish()
{
  L7_RC_t result = L7_SUCCESS;
  int     rv;

  /* update any actions in the rule that require translation */
  hapiBroadPolicyActionUpdate(policyInfo);

  rv = usl_bcmx_policy_recommit(updatePolicyId, policyInfo);
  if (BCM_E_NONE != rv)
  {
    /* clean up */
    usl_bcmx_policy_destroy(updatePolicyId);
    result = L7_FAILURE;
  }

  if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
  {
    sysapiPrintf("Finished updating policy %d rv = %d\n", updatePolicyId, rv);
    hapiBroadPolicyDebug(updatePolicyId);
  }

  hapiBroadPolicyRulesPurge(policyInfo);

  osapiSemaGive(policyTableSema);

  return result;
}

/*********************************************************************
*
*  This function is specific to XGS3 and applies only when undirected/legacy
*  mirroring is used. This may not be the ideal place to do this, but flow
*  based policy requires system wide configuration. Just creating the policy
*  is not sufficient, all units in the system need to be configured.
*
*********************************************************************/
L7_RC_t hapiBroadPolicySetMirroringPath(BROAD_POLICY_t policy, bcmx_lport_t lport, int flags)
{
  int                            rv            = BCM_E_NONE;
  int                            directed      = 0;
  L7_RC_t                        result        = L7_SUCCESS;
  static int                     mirrorCounter = 0;
  static bcmx_lport_t            mtpPort       = BCMX_LPORT_INVALID;
  bcmx_lport_t                   dest_lport;
  BROAD_POLICY_RULE_ENTRY_t     *rulePtr;
  BROAD_ACTION_ENTRY_t          *actionPtr;
  DAPI_USP_t                     usp;
  BROAD_PORT_t                  *hapiMirrorToPortPtr;
  usl_bcm_port_filter_mode_t     mode;
  usl_bcm_port_mirror_config_t   mirrorConfig;


  memset(&mode, 0, sizeof(mode));
  memset(&mirrorConfig, 0, sizeof(mirrorConfig));

  bcmx_switch_control_get(bcmSwitchDirectedMirroring, &directed);

  if (directed != 0)
  {
    return L7_SUCCESS;
  }

  /* Check if the policy has MIRROR action */
  osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

  /* Free any rules already allocated before calling usl_bcmx_policy_info_get(). */
  hapiBroadPolicyRulesPurge(policyInfo);

  rv = usl_bcmx_policy_info_get(policy, policyInfo);
  if (rv == BCM_E_NONE)
  {
    rulePtr = policyInfo->ruleInfo;
    while (rulePtr != L7_NULL)
    {
      actionPtr = &rulePtr->actionInfo;

      if (BROAD_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_MIRROR))
      {
        dest_lport = bcmx_modid_port_to_lport(actionPtr->u.ifp_parms.modid,
                                              actionPtr->u.ifp_parms.modport);

        usp = actionPtr->u.ifp_parms.usp;
        hapiMirrorToPortPtr = HAPI_PORT_GET(&usp, dapi_g);

        if (flags == L7_ENABLE)
        {
          /* Only one MTP port is support with legacy mirroring. All policies
           * must use same MTP port.
           */
          if (mirrorCounter == 0)
          {
            mirrorConfig.flags = BCM_MIRROR_PORT_ENABLE;
            mirrorConfig.probePort = dest_lport;
            mirrorConfig.stackUnit = usp.unit;
            rv = usl_bcmx_port_mirror_set (lport, mirrorConfig);
            if (rv != BCM_E_NONE)
              break;

            mtpPort = dest_lport;

            /* Enable ingress filtering */
            mode.flags =  BCM_PORT_VLAN_MEMBER_INGRESS;
            mode.setFlags = L7_TRUE;
            rv = usl_bcmx_port_vlan_member_set(dest_lport,
                                               mode);
            if (L7_BCMX_OK(rv) != L7_TRUE)
              break;

            /* Disable egress filtering - the mirrored packets may not be on the
            * same vlan as the probe port
            */

            mode.flags =  BCM_PORT_VLAN_MEMBER_EGRESS;
            mode.setFlags = L7_FALSE;
            rv = usl_bcmx_port_vlan_member_set(dest_lport,
                                               mode);


            if (L7_BCMX_OK(rv) != L7_TRUE)
              break;

            rv = usl_bcmx_port_untagged_vlan_set(dest_lport, HPC_STACKING_VLAN_ID);
            if (L7_BCMX_OK(rv) != L7_TRUE)
              break;
          }
          else if ((mtpPort != BCMX_LPORT_INVALID) && (mtpPort != dest_lport))
          {
            /* Do not allow multiple MTP ports at a time */
            rv = BCM_E_PARAM;
            break;
          }
          mirrorCounter++;
        }
        else if (mirrorCounter)
        {
          mirrorCounter--;
          /* If all the mirror policies are removed, Disable mirroring */
          if (mirrorCounter == 0)
          {
            mirrorConfig.flags = BCM_MIRROR_DISABLE;
            mirrorConfig.probePort = dest_lport;
            mirrorConfig.stackUnit = 0;
            rv = usl_bcmx_port_mirror_set(lport, mirrorConfig);
            if (rv != BCM_E_NONE)
              break;

            result = hapiBroadVlanIngressFilterSet(dest_lport, hapiMirrorToPortPtr->ingressFilteringEnabled);
            if (result != L7_SUCCESS)
            {
              rv = BCM_E_FAIL;
              break;
            }

            /* Re-enable egress filtering.
            */

            mode.flags =  BCM_PORT_VLAN_MEMBER_EGRESS;
            mode.setFlags = L7_TRUE;
            rv = usl_bcmx_port_vlan_member_set(dest_lport,
                                               mode);
            if (L7_BCMX_OK(rv) != L7_TRUE)
              break;

            rv = usl_bcmx_port_untagged_vlan_set(dest_lport, hapiMirrorToPortPtr->pvid);
            if (L7_BCMX_OK(rv) != L7_TRUE)
              break;
          }
        }
      }

      rulePtr = rulePtr->next;
    }
  }

  /* Free any rules allocated by usl_bcmx_policy_info_get(). */
  hapiBroadPolicyRulesPurge(policyInfo);

  osapiSemaGive(policyTableSema);

  return((rv == BCM_E_NONE) ? L7_SUCCESS : L7_FAILURE);
}

/*********************************************************************
*
* @purpose Apply a policy to a port. For port or LAG (Trunk) specific
*          policies it is necessary to download the policy to make sure
*          it exists in hardware prior to application. For all other
*          types the policy should already exist in each unit.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyApplyToIface(BROAD_POLICY_t policy, bcmx_lport_t lport)
{
    int                         rv;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);

    /* apply policy to the specified port */
    rv = usl_bcmx_policy_port_apply(policy, lport);
    if (BCM_E_NONE != rv)
    {
      result = L7_FAILURE;
    }
    else
    {
      /* Check to see the policy has MIRROR action. If yes, configure */
      result = hapiBroadPolicySetMirroringPath(policy, lport, L7_ENABLE);
    }

    return result;
}

/*********************************************************************
*
* @purpose Remove a policy from a port. The policy is not actually
*          removed from hardware until it is explicitly deleted.
*          However, once the last port is removed from a policy it
*          has no effect.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRemoveFromIface(BROAD_POLICY_t policy, bcmx_lport_t lport)
{
    int                         rv;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);

    /* remove policy from the specified port */
    rv = usl_bcmx_policy_port_remove(policy, lport);
    if (BCM_E_NONE != rv)
    {
        result = L7_FAILURE;
    }
    else
    {
        /* Check if the policy has mirror action. If yes, unconfigure */
        hapiBroadPolicySetMirroringPath(policy, lport, L7_DISABLE);
    }

    return result;
}

/*********************************************************************
*
* @purpose Apply a policy to all Ethernet ports.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyApplyToAll(BROAD_POLICY_t policy)
{
    int                         rv;
    L7_RC_t                     result = L7_SUCCESS;

    /* apply policy to all ethernet ports */
    rv = usl_bcmx_policy_apply_all(policy);
    if (BCM_E_NONE != rv)
    {
        result = L7_FAILURE;
    }

    return result;
}

/*********************************************************************
*
* @purpose Remove a policy from all Ethernet ports..
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRemoveFromAll(BROAD_POLICY_t policy)
{
    int                         rv;
    L7_RC_t                     result = L7_SUCCESS;

    /* remove policy from all ethernet ports */
    rv = usl_bcmx_policy_remove_all(policy);
    if (BCM_E_NONE != rv)
    {
        result = L7_FAILURE;
    }

    return result;
}

/*********************************************************************
*
* @purpose Specify whether policy applies to FPS ports in addition to
*          all  Ethernet ports.
* @end
*********************************************************************/
L7_RC_t hapiBroadPolicyEnableFPS()
{
    policyInfo->policyFlags |= BROAD_POLICY_FPS;

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Indicate for the policy whether ethertypes should be
*          qualified strictly. If a policy is not configured
*          to strictly qualify ethertypes, then the policy manager
*          may choose to use an IpType qualifier instead for policies
*          that qualify on ethertypes of 0x0800 or 0x86dd. If IpType
*          is used, then the HW may do additional checking beyond
*          the ethertype to ensure the packet is IPv4 or IPv6.
*
* @end
*********************************************************************/
L7_RC_t hapiBroadPolicyStrictEthTypeSet(L7_BOOL strictlyEnforced)
{
    policyInfo->strictEthTypes = strictlyEnforced;

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Returns how many ports exist on each BCM unit.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxPortsPerBcmUnit()
{
  static L7_BOOL   first_time             = L7_TRUE;
  static L7_uint32 max_ports_per_bcm_unit = 0;
  const bcm_sys_board_t *board_info;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    switch (board_info->npd_id)
    {
    case __BROADCOM_56624_ID:
    case __BROADCOM_56634_ID:
      max_ports_per_bcm_unit = 52;
      break;

    case __BROADCOM_56636_ID:
      max_ports_per_bcm_unit = 31;
      break;

    default:
      max_ports_per_bcm_unit = 28;
      break;
    }
  }

  return max_ports_per_bcm_unit;
}

/*********************************************************************
*
* @purpose Returns how many BCM units can exist on each CPU in the stack.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxBcmUnitsPerCpu()
{
  static L7_BOOL   first_time            = L7_TRUE;
  static L7_uint32 max_bcm_units_per_cpu = 0;
  const bcm_sys_board_t *board_info;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    switch (board_info->npd_id)
    {
    /* These boards support back-to-back topologies for a given CPU. */
    case __BROADCOM_56304_ID:
    case __BROADCOM_56314_ID:
    case __BROADCOM_56504_ID:
    case __BROADCOM_56514_ID:
      max_bcm_units_per_cpu = 2;
      break;

    default:
      max_bcm_units_per_cpu = 1;
      break;
    }
  }

  return max_bcm_units_per_cpu;
}

typedef struct
{
  L7_int32   npdId;
  L7_uint32  maxVfpRules;
  L7_uint32  maxIfpSystemRules; /* Includes COS rules */
  L7_uint32  maxIfpUserRules;
  L7_uint32  maxEfpRules;
} npdRulesCapacity_t;

npdRulesCapacity_t npdRulesCapacity[] = {
  {__BROADCOM_56224_ID, 0,       1 * 128, 6 * 128              , 0},
  {__BROADCOM_56304_ID, 0,       3 * 128, 5 * 128              , 0},
  {__BROADCOM_56314_ID, 0,       1 * 128, 6 * 128              , 0},
  {__BROADCOM_56504_ID, 0,       1 * 128, 15 * 128             , 0},
  {__BROADCOM_56514_ID, 4 * 256, 1 * 128, 15 * 256             , 4 * 128},
  {__BROADCOM_56820_ID, 4 * 256, 3 * 128, (5 * 128) + (4 * 256), 4 * 128},
  {__BROADCOM_56624_ID, 4 * 512, 1 * 512, 14 * 512             , 4 * 256},
  {__BROADCOM_56634_ID, 4 * 512, 1 * 512, 14 * 512             , 4 * 256},
  {__BROADCOM_56524_ID, 4 * 512, 1 * 512, 14 * 512             , 4 * 256},
  {__BROADCOM_56636_ID, 4 * 512, 1 * 512, 14 * 512             , 4 * 256},
};

/*********************************************************************
*
* @purpose Returns how many VFP rules can fit into each BCM unit
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxVfpRulesPerUnit()
{
  static L7_BOOL         first_time = L7_TRUE;
  static L7_uint32       max_rules;
  const bcm_sys_board_t *board_info;
  L7_uint32              i, numEntries;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    /* For default value, assume 0 */
    max_rules = 0;
    numEntries = sizeof(npdRulesCapacity) / sizeof(npdRulesCapacity_t);
    for (i = 0; i < numEntries; i++)
    {
      if (npdRulesCapacity[i].npdId == board_info->npd_id)
      {
        max_rules = npdRulesCapacity[i].maxVfpRules;
        break;
      }
    }
  }

  return max_rules;
}

/*********************************************************************
*
* @purpose Returns how many system rules can fit into each BCM unit
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxSystemRulesPerUnit()
{
  static L7_BOOL         first_time = L7_TRUE;
  static L7_uint32       max_rules;
  const bcm_sys_board_t *board_info;
  L7_uint32              i, numEntries;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    /* For default value, assume 128 rules per system slice. */
    max_rules = 128;
    numEntries = sizeof(npdRulesCapacity) / sizeof(npdRulesCapacity_t);
    for (i = 0; i < numEntries; i++)
    {
      if (npdRulesCapacity[i].npdId == board_info->npd_id)
      {
        max_rules = npdRulesCapacity[i].maxIfpSystemRules;
        break;
      }
    }
  }

  return max_rules;
}

/*********************************************************************
*
* @purpose Returns how many user rules can fit into each BCM unit
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxUserRulesPerUnit()
{
  static L7_BOOL         first_time = L7_TRUE;
  static L7_uint32       max_rules;
  const bcm_sys_board_t *board_info;
  L7_uint32              i, numEntries;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    /* For default value, assume 2 groups available w/ 128 rules each. */
    max_rules = 2 * 128;
    numEntries = sizeof(npdRulesCapacity) / sizeof(npdRulesCapacity_t);
    for (i = 0; i < numEntries; i++)
    {
      if (npdRulesCapacity[i].npdId == board_info->npd_id)
      {
        max_rules = npdRulesCapacity[i].maxIfpUserRules;
        break;
      }
    }
  }

  return max_rules;
}

/*********************************************************************
*
* @purpose Returns how many EFP rules can fit into each BCM unit
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxEfpRulesPerUnit()
{
  static L7_BOOL         first_time = L7_TRUE;
  static L7_uint32       max_rules;
  const bcm_sys_board_t *board_info;
  L7_uint32              i, numEntries;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    /* For default value, assume 0 */
    max_rules = 0;
    numEntries = sizeof(npdRulesCapacity) / sizeof(npdRulesCapacity_t);
    for (i = 0; i < numEntries; i++)
    {
      if (npdRulesCapacity[i].npdId == board_info->npd_id)
      {
        max_rules = npdRulesCapacity[i].maxEfpRules;
        break;
      }
    }
  }

  return max_rules;
}


/*********************************************************************
*
* @purpose Returns how many system policies can exist in the system.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxSystemPolicies()
{
  /* PTin modified: FFP rules */
  #if 0
  static L7_BOOL   first_time        = L7_TRUE;
  static L7_uint32 maxSystemPolicies = 0;

  if (first_time == L7_TRUE)
  {
    first_time = L7_FALSE;

    maxSystemPolicies = 16 + L7_MAX_FDB_STATIC_FILTER_ENTRIES + L7_ACL_VLAN_MAX_COUNT;

    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID))
    {
      maxSystemPolicies += L7_ACL_VLAN_MAX_COUNT;
    }
    if (cnfgrIsComponentPresent(L7_FLEX_QOS_ISCSI_COMPONENT_ID))
    {
      maxSystemPolicies += L7_ISCSI_MAX_CONNECTIONS;
      maxSystemPolicies += L7_ISCSI_MAX_TARGET_TCP_PORTS;
    }
  }

  return maxSystemPolicies;
  #else
  return 256;
  #endif
}

/*********************************************************************
*
* @purpose Returns how many policies can exist in the system.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxPolicies()
{
  /* PTin modified: FFP rules */
  #if 0
  static L7_BOOL   first_time   = L7_TRUE;
  static L7_uint32 max_policies = 0;
  const bcm_sys_board_t *board_info;
  L7_int32 maxCosRulesPerUnit;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    /* For SMB and Smartpath, use the old formula for calculating BROAD_MAX_POLICIES. This may result in artificial limitations
       in the number of policies, but will be acceptable in the interest of decreasing memory usage. */
    if (cnfgrIsFeaturePresent(L7_DTL_COMPONENT_ID, L7_DTL_STRICT_POLICY_LIMIT_FEATURE_ID))
    {
      max_policies = L7_MAX_PORT_COUNT + L7_ACL_MAX_LISTS;
      return max_policies;
    }

    max_policies += BROAD_MAX_SYSTEM_POLICIES;
    if (hapiBroadDot1xPolicySupported())
    {
      /* Each dot1x client on each port can have it's own Diffserv policy. */
      /* Note however that the number of dot1x diffserv policies cannot exceed the
         number of user rules available. */
      max_policies += min((L7_MAX_PORT_COUNT * L7_DOT1X_PORT_MAX_MAC_USERS),
                          (hapiBroadMaxUserRulesPerUnit() * L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU));
    }
    else
    {
      max_policies += L7_MAX_PORT_COUNT; /* Assume one policy per physical port for IFP (ACL or Diffserv). */
    }

    /* The number of COS policies is the number of COS rules divided by 8 since each dot1p COS mapping has 8 rules. */
    maxCosRulesPerUnit = hapiBroadMaxSystemRulesPerUnit() - BROAD_MAX_SYSTEM_RULES;
    if (maxCosRulesPerUnit > 0)
    {
      max_policies += (maxCosRulesPerUnit * L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU) / 8;
    }

    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) &&
        (L7_FALSE == cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_FEAT_EGRESS_ACL_ON_IFP_ID)))
    {
      max_policies += L7_MAX_PORT_COUNT; /* Assume one policy per physical port for EFP (ACL or Diffserv). */
    }

    /* Account for VFP policies*/
    /* Assume one policy per rule for VFP (IPSG and dot1x). */
    max_policies += L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU * hapiBroadMaxVfpRulesPerUnit();
  }

  return max_policies;
  #else
  return 2048;
  #endif
}

/*********************************************************************
*
* @purpose Returns how many policies can exist in each BCM unit.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxPoliciesPerBcmUnit()
{
  /* PTin modified: FFP rules */
  #if 0
  static L7_BOOL first_time                  = L7_TRUE;
  static L7_uint32 max_policies_per_bcm_unit = 0;
  const bcm_sys_board_t *board_info;
  L7_int32 maxCosRulesPerUnit;

  /* First get the board info using the bcm call */
  if (first_time == L7_TRUE)
  {
    board_info = hpcBoardGet();
    if (board_info == L7_NULL)
    {
      LOG_ERROR(0);
    }
    first_time = L7_FALSE;

    max_policies_per_bcm_unit += BROAD_MAX_SYSTEM_POLICIES;
    if (hapiBroadDot1xPolicySupported())
    {
      /* Each dot1x client on each port can have it's own Diffserv policy. */
      /* Note however that the number of dot1x diffserv policies cannot exceed the
         number of user rules available. */
      max_policies_per_bcm_unit += min((BROAD_MAX_BCM_PORTS_PER_BCM_UNIT * L7_DOT1X_PORT_MAX_MAC_USERS),
                                       (hapiBroadMaxUserRulesPerUnit()));
    }
    else
    {
      max_policies_per_bcm_unit += BROAD_MAX_BCM_PORTS_PER_BCM_UNIT; /* Assume one policy per physical port for IFP (ACL or Diffserv). */
    }
    /* The number of COS policies is the number of COS rules divided by 8 since each dot1p COS mapping has 8 rules. */
    maxCosRulesPerUnit = hapiBroadMaxSystemRulesPerUnit() - BROAD_MAX_SYSTEM_RULES;
    if (maxCosRulesPerUnit > 0)
    {
      max_policies_per_bcm_unit += (maxCosRulesPerUnit) / 8;
    }

    if (cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) &&
        (L7_FALSE == cnfgrIsFeaturePresent(L7_FLEX_QOS_ACL_COMPONENT_ID, L7_FEAT_EGRESS_ACL_ON_IFP_ID)))
    {
      max_policies_per_bcm_unit += BROAD_MAX_BCM_PORTS_PER_BCM_UNIT; /* Assume one policy per physical port for EFP (ACL or Diffserv). */
    }

    /* Account for VFP policies*/
    /* Assume one policy per rule for VFP (IPSG and dot1x). */
    max_policies_per_bcm_unit += hapiBroadMaxVfpRulesPerUnit();

    /* The max policies per BCM unit cannot exceed the max policies in the system. */
    max_policies_per_bcm_unit = min(max_policies_per_bcm_unit, BROAD_MAX_POLICIES);
  }

  return max_policies_per_bcm_unit;
  #else
  return 2048;
  #endif
}

/* Debug */

void hapiBroadPolicyByteDump(L7_uchar8 *ptr, L7_int32 bytes, BROAD_POLICY_FIELD_t field)
{
    L7_ushort16 data16;
    L7_uint32   data32;
    L7_uchar8  *buf_ptr = ptr;

    switch (field)
    {
    case BROAD_FIELD_ETHTYPE:
    case BROAD_FIELD_OVID:
    case BROAD_FIELD_IVID:
    case BROAD_FIELD_SPORT:
    case BROAD_FIELD_DPORT:
      data16 = osapiHtons(*((L7_ushort16 *)ptr));
      buf_ptr = (L7_uchar8 *)&data16;
      break;

    case BROAD_FIELD_SIP:
    case BROAD_FIELD_DIP:
    case BROAD_FIELD_IP6_FLOWLABEL:
      data32 = osapiHtonl(*((L7_uint32 *)ptr));
      buf_ptr = (L7_uchar8 *)&data32;
      break;

    default:
      break;
    }

    while (bytes-- > 0)
        sysapiPrintf("%02x", *buf_ptr++);
}

void hapiBroadPolicyDebugTable()
{
  int                   rv;
  BROAD_POLICY_ENTRY_t *policyInfo;
  int                   i;

  policyInfo = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_POLICY_ENTRY_t));
  if (policyInfo == L7_NULL)
  {
    return;
  }

  sysapiPrintf("\nPolicy Table\n");
  sysapiPrintf("  Id   Type  Flags  Rules\n");
  for (i = 0; i < BROAD_MAX_POLICIES; i++)
  {
    rv = usl_db_policy_info_get(USL_CURRENT_DB, i, policyInfo);
    if (rv == BCM_E_NONE)
    {
      sysapiPrintf("[%4d] ", i);
      sysapiPrintf("%11s  %4x  %5d  ",
                   hapiBroadPolicyTypeName(policyInfo->policyType),
                   policyInfo->policyFlags,
                   policyInfo->ruleCount);
      sysapiPrintf("\n");

      /* Free any rules allocated by usl_db_policy_info_get(). */
      hapiBroadPolicyRulesPurge(policyInfo);
    }
    else if (rv != BCM_E_NOT_FOUND)
    {
      sysapiPrintf("Couldn't retrieve policy %d info from USL DB: rv = %d\n", i, rv);
    }
  }

  osapiFree(L7_DRIVER_COMPONENT_ID, policyInfo);
}

void hapiBroadPolicyDebugAction(BROAD_ACTION_ENTRY_t       *actionPtr,
                                BROAD_POLICY_STAGE_t        policyStage,
                                BROAD_POLICY_ACTION_t       action,
                                BROAD_POLICY_ACTION_SCOPE_t action_scope)
{
  L7_uint32 param0;
  L7_uint32 param1;
  L7_uint32 param2;

  hapiBroadPolicyActionParmsGet(actionPtr, policyStage, action, action_scope, &param0, &param1, &param2);

  switch (action)
  {
  case BROAD_ACTION_REDIRECT:
  case BROAD_ACTION_MIRROR:
    sysapiPrintf("{%02x, %02x, %02x} ", actionPtr->u.ifp_parms.usp.unit, actionPtr->u.ifp_parms.usp.slot, actionPtr->u.ifp_parms.usp.port);
    sysapiPrintf("{%02x, %02x}\n", param0, param1);
    break;

  case BROAD_ACTION_SET_DROPPREC:
    sysapiPrintf("{%02x, %02x, %02x}\n", param0,
                                         param1,
                                         param2);
    break;

  case BROAD_ACTION_SET_COSQ:
  case BROAD_ACTION_SET_DSCP:
  case BROAD_ACTION_SET_TOS:
  case BROAD_ACTION_SET_USERPRIO:
  case BROAD_ACTION_SET_USERPRIO_INNERTAG:  /* PTin added */
  case BROAD_ACTION_SET_OUTER_VID:
  case BROAD_ACTION_SET_INNER_VID:
  case BROAD_ACTION_ADD_OUTER_VID:
  case BROAD_ACTION_ADD_INNER_VID:
  case BROAD_ACTION_SET_CLASS_ID:
  case BROAD_ACTION_SET_REASON_CODE:
    sysapiPrintf("{%02x}\n", param0);
    break;

  default:
    sysapiPrintf("\n");
    break;
  }
}

void hapiBroadPolicyDebugRule(BROAD_POLICY_RULE_ENTRY_t *rulePtr, BROAD_POLICY_STAGE_t policyStage)
{
    int  i;
    L7_uchar8 *maskPtr;

    for (i = 0; i < BROAD_FIELD_LAST; i++)
    {
        if (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, i) == BROAD_FIELD_SPECIFIED)
        {
            sysapiPrintf("     Field %10s = {", hapiBroadPolicyFieldName(i));
            hapiBroadPolicyByteDump(hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, i), hapiBroadPolicyFieldSize(i), i);
            sysapiPrintf(", ");
            maskPtr = hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, i);
            if (maskPtr != L7_NULL)
            {
              hapiBroadPolicyByteDump(maskPtr,  hapiBroadPolicyFieldSize(i), i);
            }
            else
            {
              sysapiPrintf("n/a");
            }
            sysapiPrintf("}\n");
        }
    }

    for (i = 0; i < BROAD_ACTION_LAST; i++)
    {
        if (BROAD_CONFORMING_ACTION_IS_SPECIFIED((&rulePtr->actionInfo), i))
        {
            sysapiPrintf("     Action %10s: ",
                         hapiBroadPolicyActionName(i));

            sysapiPrintf("/IN-PROF/");

            hapiBroadPolicyDebugAction(&rulePtr->actionInfo, policyStage, i, BROAD_POLICY_ACTION_CONFORMING);
        }
        if (BROAD_EXCEEDING_ACTION_IS_SPECIFIED((&rulePtr->actionInfo), i))
        {
          sysapiPrintf("     Action %10s: ",
                       hapiBroadPolicyActionName(i));

          sysapiPrintf("/EXCEED /");

          hapiBroadPolicyDebugAction(&rulePtr->actionInfo, policyStage, i, BROAD_POLICY_ACTION_EXCEEDING);
        }
        if (BROAD_NONCONFORMING_ACTION_IS_SPECIFIED((&rulePtr->actionInfo), i))
        {
          sysapiPrintf("     Action %10s: ",
                       hapiBroadPolicyActionName(i));

          sysapiPrintf("/NONCONF/");

          hapiBroadPolicyDebugAction(&rulePtr->actionInfo, policyStage, i, BROAD_POLICY_ACTION_NONCONFORMING);
        }
    }

    if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
    {
        /* PTin modified: SDK 6.3.0 */
        sysapiPrintf("     Metered  (%c%c)    : CIR %d CBS %d PIR %d PBS %d\n",
                     rulePtr->policer.policerInfo.colorMode == BROAD_METER_COLOR_BLIND ? 'C' : '-',
                     rulePtr->ruleFlags & BROAD_METER_SHARED ? 'S' : '-',
                     rulePtr->policer.policerInfo.cir, rulePtr->policer.policerInfo.cbs,
                     rulePtr->policer.policerInfo.pir, rulePtr->policer.policerInfo.pbs);
    }

    if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
    {
        /* PTin modified: SDK 6.3.0 */
        sysapiPrintf("     Counted  (%c%c)\n",
                     rulePtr->counter.counterInfo.mode == BROAD_COUNT_PACKETS ? 'P' : 'B',
                     rulePtr->ruleFlags & BROAD_COUNTER_SHARED ? 'S' : '-');
    }
}

void hapiBroadPolicyDebug(BROAD_POLICY_t policy)
{
  int                        rv;
  BROAD_POLICY_ENTRY_t      *policyInfo;
  BROAD_POLICY_RULE_ENTRY_t *rulePtr;
  int                        i;

  if (policy >= BROAD_MAX_POLICIES)
  {
    sysapiPrintf("Invalid policy (%d), valid range is [0 .. %d)\n",
                 policy, BROAD_MAX_POLICIES);
    return;
  }

  policyInfo = osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_POLICY_ENTRY_t));
  if (policyInfo == L7_NULL)
  {
    return;
  }

  rv = usl_db_policy_info_get(USL_CURRENT_DB, policy, policyInfo);
  if (rv == BCM_E_NONE)
  {
    sysapiPrintf("\nPolicy %d\n", policy);
    rulePtr = policyInfo->ruleInfo;
    i = 0;
    while (rulePtr != L7_NULL)
    {
      sysapiPrintf("   Rule %d\n", i);
      hapiBroadPolicyDebugRule(rulePtr, policyInfo->policyStage);

      i++;
      rulePtr = rulePtr->next;
    }

    /* Free any rules allocated by usl_db_policy_info_get(). */
    hapiBroadPolicyRulesPurge(policyInfo);
  }
  else if (rv != BCM_E_NOT_FOUND)
  {
    sysapiPrintf("Couldn't retrieve policy info from USL DB: rv = %d\n", rv);
  }

  osapiFree(L7_DRIVER_COMPONENT_ID, policyInfo);
}
/*********************************************************************
*
* @purpose  Set status for policy Rule ID 
*
* @param    BROAD_POLICY_t    policy
* @param    BROAD_PORT_RULE_t rule
* @param    BROAD_RULE_STATUS_t status
*
* @returns  L7_RC_t
*
* @notes    For internal use only.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleStatusSet(BROAD_POLICY_t policy, BROAD_POLICY_RULE_t rule, BROAD_RULE_STATUS_t status)
{
    L7_RC_t result = L7_SUCCESS;
    int     rv = BCM_E_NONE; 

    rv = usl_bcmx_policy_rule_status_set(policy, rule, status);     
    if (BCM_E_NONE != rv)
    {
        result = L7_FAILURE;
    }

    return result;    
}


