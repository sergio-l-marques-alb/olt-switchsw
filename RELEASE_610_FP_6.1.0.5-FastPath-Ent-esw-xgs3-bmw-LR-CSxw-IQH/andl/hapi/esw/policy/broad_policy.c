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
#include "broad_policy_bcmx.h"
#include "broad_common.h"
#include "broad_l2_vlan.h"
#include "acl_exports.h"
#include "l7_usl_bcm_policy.h"
#include "l7_bcmx_port.h"
#include "bcmx/bcmx_int.h"
#include "sysbrds.h"
#include "appl/stktask/topo_brd.h"
#include "bcmx/mirror.h"
#include "bcmx/port.h"
#include "feature.h"
#include "bcmx/switch.h"



extern DAPI_t *dapi_g;


#define POLICY_NONE       0
#define POLICY_USED       1

/* Policy Table Semaphore
 */
void *policyTableSema = L7_NULLPTR;

/* Policy Table
 * This table is used to manage all policies in the system. Policy identifers
 * are global and can be downloaded to zero or more units. A policy must be
 * downloaded to a unit before it can be applied to a port.
 */
typedef struct BROAD_POLICY_RULE_POOL_ENTRY_s
{
  BROAD_POLICY_RULE_ENTRY_t              ruleInfo;
  struct BROAD_POLICY_RULE_POOL_ENTRY_s *next;
}
BROAD_POLICY_RULE_POOL_ENTRY_t;

typedef struct
{
    L7_uchar8                       flags;
    BROAD_POLICY_TYPE_t             policyType;
    BROAD_POLICY_STAGE_t            policyStage;
    unsigned char                   policyFlags;
#if (BROAD_MAX_RULES_PER_POLICY < 256)
    unsigned char                   ruleCount;
#else
    unsigned short                  ruleCount;
#endif
    BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;
}
BROAD_POLICY_TABLE_ENTRY_t;

static BROAD_POLICY_TABLE_ENTRY_t     *broadPolicyTable                = L7_NULLPTR;
static BROAD_POLICY_RULE_POOL_ENTRY_t *broadPolicyTableRulePool        = L7_NULLPTR;
static BROAD_POLICY_RULE_POOL_ENTRY_t *broadPolicyTableRulePoolFreePtr = L7_NULLPTR;
static BROAD_POLICY_ENTRY_t            policyInfo; /* scratch-pad to be used to organize policyInfo before sending to USL/customx. */

/* Macros for error checking. */
#define CHECK_POLICY(p)   {if (p >= BROAD_MAX_POLICIES)         return L7_ERROR;}
#define CHECK_RULE(r)     {if (r >= BROAD_MAX_RULES_PER_POLICY) return L7_ERROR;}
#define CHECK_FIELD(f)    {if (f >= BROAD_FIELD_SPECIAL_LAST)   return L7_ERROR;}
#define CHECK_ACTION(a)   {if (a >= BROAD_ACTION_LAST)          return L7_ERROR;}

extern L7_RC_t hapiBroadPolicyUtilInit();

/* forward decl */
void hapiBroadPolicyDebug(BROAD_POLICY_t policy);

static BROAD_POLICY_RULE_ENTRY_t *hapiBroadPolicyGetRulePtr(BROAD_POLICY_TABLE_ENTRY_t *policyPtr, L7_uint32 ruleNumberToFind)
{
  BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;
  L7_uint32                       ruleNumber;
  BROAD_POLICY_RULE_ENTRY_t      *rulePtr = L7_NULL;

  ruleInfoPtr = policyPtr->ruleInfoPtr;
  ruleNumber  = 0;
  while (ruleInfoPtr != L7_NULL)
  {
    if (ruleNumber == ruleNumberToFind)
    {
      rulePtr = &ruleInfoPtr->ruleInfo;
      break;
    }
    ruleNumber++;
    ruleInfoPtr = ruleInfoPtr->next;
  }

  return rulePtr;
}

static void hapiBroadPolicyInfoConvert(BROAD_POLICY_TABLE_ENTRY_t *policyPtr, BROAD_POLICY_ENTRY_t *policyInfo)
{
  BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;
  BROAD_POLICY_RULE_ENTRY_t      *rulePtr = L7_NULL;
  L7_uint32                       rule;

  policyInfo->policyType  = policyPtr->policyType;
  policyInfo->policyStage = policyPtr->policyStage;
  policyInfo->policyFlags = policyPtr->policyFlags;
  policyInfo->ruleCount   = policyPtr->ruleCount;

  ruleInfoPtr = policyPtr->ruleInfoPtr;
  rulePtr     = &ruleInfoPtr->ruleInfo;
  for (rule = 0; rule < policyInfo->ruleCount; rule++)
  {
    memcpy(&policyInfo->ruleInfo[rule], rulePtr, sizeof(BROAD_POLICY_RULE_ENTRY_t));

    ruleInfoPtr = ruleInfoPtr->next;
    rulePtr     = &ruleInfoPtr->ruleInfo;
  }
}

/*********************************************************************
*
* @purpose Initialize a policy entry in the table.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadPolicyInitEntry(BROAD_POLICY_t policy)
{
    CHECK_POLICY(policy);

    memset(&broadPolicyTable[policy], 0, sizeof(broadPolicyTable[policy]));

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Find a used action entry with specified flags.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadPolicyActionFind(BROAD_POLICY_RULE_ENTRY_t  *rulePtr,
                                         BROAD_POLICY_ACTION_SCOPE_t action_scope,
                                         BROAD_POLICY_ACTION_t      *action,
                                         int                        *param0,
                                         int                        *param1,
                                         int                        *param2)
{
  BROAD_ACTION_ENTRY_t *actionPtr;
  int i;

  actionPtr = &rulePtr->actionInfo;

  for (i = 0; i < BROAD_ACTION_LAST; i++)
  {
    if (actionPtr->actions[action_scope] & (1 << i))
    {
      *action = i;

      switch (*action)
      {
      case BROAD_ACTION_REDIRECT:
      case BROAD_ACTION_REDIRECT_TRUNK:   /* PTin added */
      case BROAD_ACTION_MIRROR:
        *param0 = actionPtr->parms.usp.unit;
        *param1 = actionPtr->parms.usp.slot;
        *param2 = actionPtr->parms.usp.port;
        break;

      case BROAD_ACTION_SET_COSQ:
        *param0 = actionPtr->parms.set_cosq[action_scope];
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_DSCP:
        *param0 = actionPtr->parms.set_dscp[action_scope];
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_TOS:
        *param0 = actionPtr->parms.set_tos[action_scope];
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_USERPRIO:
        *param0 = actionPtr->parms.set_userprio[action_scope];
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_DROPPREC:
        *param0 = actionPtr->parms.set_dropprec.conforming;
        *param1 = actionPtr->parms.set_dropprec.exceeding;
        *param2 = actionPtr->parms.set_dropprec.nonconforming;
        break;

      case BROAD_ACTION_SET_OUTER_VID:
        *param0 = actionPtr->parms.set_ovid;
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_INNER_VID:
        *param0 = actionPtr->parms.set_ivid;
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_ADD_OUTER_VID:
        *param0 = actionPtr->parms.add_ovid;
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_CLASS_ID:
        *param0 = actionPtr->parms.set_class_id;
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_REASON_CODE:
        *param0 = actionPtr->parms.set_reason;
        *param1 = 0;
        *param2 = 0;
        break;

      case BROAD_ACTION_SET_USERPRIO_AS_COS2:
      case BROAD_ACTION_SET_COSQ_AS_PKTPRIO:    /* PTin added */
      default:
        *param0 = 0;
        *param1 = 0;
        *param2 = 0;
        break;
      }

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
        // PTin flag: That is why REDIRECT action doesn't work with LAGs... we have to use REDIRECT_TRUNK action
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
    case BROAD_ACTION_REDIRECT_TRUNK:   /* PTin added */
    case BROAD_ACTION_MIRROR:
      if (((action == BROAD_ACTION_REDIRECT) && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_REDIRECT_TRUNK)) ||  /* PTin added */
          ((action == BROAD_ACTION_REDIRECT_TRUNK) && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_REDIRECT)) ||  /* PTin added */
          ((action == BROAD_ACTION_REDIRECT) && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_MIRROR)) ||
          ((action == BROAD_ACTION_REDIRECT_TRUNK) && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_MIRROR)) ||    /* PTin added */
          ((action == BROAD_ACTION_MIRROR)   && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_REDIRECT)) ||
          ((action == BROAD_ACTION_MIRROR)   && BROAD_CONFORMING_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_REDIRECT_TRUNK)))    /* PTin added */
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
      actionPtr->parms.usp.unit = param0;
      actionPtr->parms.usp.slot = param1;
      actionPtr->parms.usp.port = param2;
      break;

    case BROAD_ACTION_SET_COSQ:
      actionPtr->parms.set_cosq[action_scope] = param0;
      break;

    case BROAD_ACTION_SET_DSCP:
      actionPtr->parms.set_dscp[action_scope] = param0;
      break;

    case BROAD_ACTION_SET_TOS:
      actionPtr->parms.set_tos[action_scope] = param0;
      break;

    case BROAD_ACTION_SET_USERPRIO:
      actionPtr->parms.set_userprio[action_scope] = param0;
      break;

    case BROAD_ACTION_SET_DROPPREC:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->parms.set_dropprec.conforming    = param0;
      actionPtr->parms.set_dropprec.exceeding     = param1;
      actionPtr->parms.set_dropprec.nonconforming = param2;
      break;

    case BROAD_ACTION_SET_OUTER_VID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->parms.set_ovid = param0;
      break;

    case BROAD_ACTION_SET_INNER_VID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->parms.set_ivid = param0;
      break;

    case BROAD_ACTION_ADD_OUTER_VID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->parms.add_ovid = param0;
      break;

    case BROAD_ACTION_SET_CLASS_ID:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->parms.set_class_id = param0;
      break;

    case BROAD_ACTION_SET_REASON_CODE:
      if (action_scope != BROAD_POLICY_ACTION_CONFORMING)
      {
        LOG_ERROR(action_scope); /* Catch programming errors... BROAD_ACTION_ENTRY_t may need to be updated to support multiple action scopes. */
      }
      actionPtr->parms.set_reason = param0;
      break;

    case BROAD_ACTION_SET_COSQ_AS_PKTPRIO:  /* PTin added */
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
static L7_RC_t hapiBroadPolicyActionUpdate(BROAD_POLICY_TABLE_ENTRY_t *policyPtr)
{
  int i;
  bcmx_lport_t lport;
  BROAD_PORT_t *hapiPortPtr;    // PTin added
  BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;

  ruleInfoPtr = policyPtr->ruleInfoPtr;

  while (ruleInfoPtr != L7_NULL)
  {
    BROAD_POLICY_RULE_ENTRY_t *rulePtr;
    BROAD_ACTION_ENTRY_t      *actionPtr;

    rulePtr   = &ruleInfoPtr->ruleInfo;
    actionPtr = &rulePtr->actionInfo;

    for (i = 0; i < BROAD_ACTION_LAST; i++)
    {
      if (BROAD_ACTION_IS_SPECIFIED(actionPtr, i))
      {
        /* Perform translation of any action params that must be done in the master. */
        switch (i)
        {
        case BROAD_ACTION_REDIRECT:
        case BROAD_ACTION_REDIRECT_TRUNK:     /* PTin added */
          // PTin added
          hapiPortPtr = HAPI_PORT_GET(&(actionPtr->parms.usp), dapi_g);
          if (hapiPortPtr->port_is_lag)
          {
            // Clear REDIRECT (port) action
            actionPtr->actions[BROAD_POLICY_ACTION_CONFORMING] &= ~((L7_uint32) 1<<BROAD_ACTION_REDIRECT);
            // Add REDIRECT_TRUNK action
            actionPtr->actions[BROAD_POLICY_ACTION_CONFORMING] |=  ((L7_uint32) 1<<BROAD_ACTION_REDIRECT_TRUNK);
            // Specify trunkId
            actionPtr->parms.trunkid = hapiPortPtr->hapiModeparm.lag.tgid;
          }
          // PTin end
          else
          {
            // PTin added
            // Clear REDIRECT_TRUNK action
            actionPtr->actions[BROAD_POLICY_ACTION_CONFORMING] &= ~((L7_uint32) 1<<BROAD_ACTION_REDIRECT_TRUNK);
            // Add REDIRECT (port) action
            actionPtr->actions[BROAD_POLICY_ACTION_CONFORMING] |=  ((L7_uint32) 1<<BROAD_ACTION_REDIRECT);
            // PTin end

            /* RoBo's SDK code does not support using lport as parameter for
              redirect actions. It should by modid, port*/
            if (hapiBroadRoboCheck() != L7_TRUE)
            {
              if (hapiBroadPolicyPortSelect(&actionPtr->parms.usp, &lport) == L7_SUCCESS)
              {
                /* Use gport (i.e. lport) for parameter */
                actionPtr->parms.modid   = 0;
                actionPtr->parms.modport = lport;
              }
              else
              {
                /* dest port is invalid */
                actionPtr->parms.modid   = BCMX_LPORT_INVALID;
                actionPtr->parms.modport = BCMX_LPORT_INVALID;
              }
            }
          }
          break;

        case BROAD_ACTION_MIRROR:
          if (hapiBroadPolicyPortSelect(&actionPtr->parms.usp, &lport) == L7_SUCCESS)
          {
            /* convert USP to modId/modPort for custom bcmx layer */
            const bcm_sys_board_t *board_info;
            board_info = hpcBoardGet();
            bcmx_lport_to_modid_port(lport, &actionPtr->parms.modid,
                                            &actionPtr->parms.modport);

          }
          else
          {
            /* dest port is invalid */
            actionPtr->parms.modid   = BCMX_LPORT_INVALID;
            actionPtr->parms.modport = BCMX_LPORT_INVALID;
          }
          break;

        case BROAD_ACTION_TRAP_TO_CPU:
        case BROAD_ACTION_COPY_TO_CPU:
          /* populate CPU modid/modPort for custom bcmx layer */
          {
            const bcm_sys_board_t *board_info;
            board_info = hpcBoardGet() ;

            bcmx_lport_to_modid_port(BCMX_LPORT_LOCAL_CPU_GET(0),
                                     &actionPtr->parms.cpu_modid,
                                     &actionPtr->parms.cpu_modport);

          }
          break;
        default:
          /* no translation needed */
          break;
        }
      }
    }

    ruleInfoPtr = ruleInfoPtr->next;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Find a free policy table entry.
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadPolicyFindFree(BROAD_POLICY_t *policy)
{
    BROAD_POLICY_t i;

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        if (!(broadPolicyTable[i].flags & POLICY_USED))
        {
            *policy = i;
            return L7_SUCCESS;
        }
    }

    return L7_FAILURE;
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
* @purpose Initialize the Policy Manager data structures upon IPL.
*          After IPL it is expected that each policy is explicitly
*          destroyed after it is no longer needed.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyInit()
{
    BROAD_POLICY_t i;

    broadPolicyTable = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                   BROAD_MAX_POLICIES * sizeof(BROAD_POLICY_TABLE_ENTRY_t));
    if (L7_NULLPTR == broadPolicyTable)
	{
        LOG_ERROR(0);
        return L7_FAILURE;
    }

    broadPolicyTableRulePool = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                   BROAD_MAX_RULES * sizeof(BROAD_POLICY_RULE_POOL_ENTRY_t));
    if (L7_NULLPTR == broadPolicyTableRulePool)
    {
        LOG_ERROR(0);
        return L7_FAILURE;
    }

    hapiBroadPolicyUtilInit();

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
        (void)hapiBroadPolicyInitEntry(i);

    for (i = 0; i < (BROAD_MAX_RULES - 1); i++)
    {
      broadPolicyTableRulePool[i].next = &broadPolicyTableRulePool[i+1];
    }
    broadPolicyTableRulePool[BROAD_MAX_RULES - 1].next = L7_NULL;
    broadPolicyTableRulePoolFreePtr = &broadPolicyTableRulePool[0];

    policyTableSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    if (L7_NULLPTR == policyTableSema)
	{
        LOG_ERROR(0);
        return L7_FAILURE;
    }

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
    BROAD_POLICY_t i;

    hapiBroadPolicyUtilInit();

    for (i = 0; i < BROAD_MAX_POLICIES; i++)
        (void)hapiBroadPolicyInitEntry(i);

    for (i = 0; i < (BROAD_MAX_RULES - 1); i++)
    {
      broadPolicyTableRulePool[i].next = &broadPolicyTableRulePool[i+1];
    }
    broadPolicyTableRulePool[BROAD_MAX_RULES - 1].next = L7_NULL;
    broadPolicyTableRulePoolFreePtr = &broadPolicyTableRulePool[0];

    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Create a new policy with no rules.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyCreate(BROAD_POLICY_TYPE_t type, BROAD_POLICY_t *policy)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;

    *policy = BROAD_POLICY_INVALID;

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    if (hapiBroadPolicyFindFree(policy) != L7_SUCCESS)
    {
        LOG_MSG("Policy table overflow.\n");
        osapiSemaGive(policyTableSema);
        return L7_FAILURE;
    }

    (void)hapiBroadPolicyInitEntry(*policy);

    policyPtr = &broadPolicyTable[*policy];

    policyPtr->flags = POLICY_USED;

    policyPtr->policyType = type;
    /* Default stage is INGRESS */
    policyPtr->policyStage = BROAD_POLICY_STAGE_INGRESS;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("Created policy %d\n", *policy);

    osapiSemaGive(policyTableSema);
    
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Designates which stage the policy should be applied to.
*          This function should be called before the policy is committed.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStageSet(BROAD_POLICY_t policy, BROAD_POLICY_STAGE_t policyStage)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);
    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
      osapiSemaGive(policyTableSema);
      return L7_ERROR;
    }
    if (policyStage >= BROAD_POLICY_STAGE_COUNT)
    {
      osapiSemaGive(policyTableSema);
      return L7_ERROR;
    }
    if (policyPtr->policyFlags & BROAD_POLICY_COMMITTED)
    {
      osapiSemaGive(policyTableSema);
      return L7_ERROR;
    }

    policyPtr->policyStage = policyStage;

    osapiSemaGive(policyTableSema);

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        sysapiPrintf("Setting stage for policy %d to %d\n", policy, policyStage);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Returns which stage the policy is applied to.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStageGet(BROAD_POLICY_t policy, BROAD_POLICY_STAGE_t *policyStage)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);
    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
      osapiSemaGive(policyTableSema);
      return L7_ERROR;
    }
    if (policyStage == L7_NULL)
    {
      osapiSemaGive(policyTableSema);
      return L7_ERROR;
    }
    *policyStage = policyPtr->policyStage;

    osapiSemaGive(policyTableSema);

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
    BROAD_POLICY_TABLE_ENTRY_t     *policyPtr;
    L7_RC_t                         result = L7_SUCCESS;
    int                             rv;
    BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;
    BROAD_POLICY_RULE_POOL_ENTRY_t *tempRuleInfoPtr;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
      osapiSemaGive(policyTableSema);
      return L7_SUCCESS;
    }

    /* remove policy from all units as it is no longer used */
    rv = usl_policy_destroy(policy);
    if (BCM_E_NONE != rv)
        result = L7_FAILURE;

    /* clean-up as much as possible even if an error occurs */
    ruleInfoPtr = policyPtr->ruleInfoPtr;
    while (ruleInfoPtr != L7_NULL)
    {
      tempRuleInfoPtr = ruleInfoPtr->next;

      ruleInfoPtr->next = broadPolicyTableRulePoolFreePtr;
      broadPolicyTableRulePoolFreePtr = ruleInfoPtr;

      ruleInfoPtr = tempRuleInfoPtr;
    }
    (void)hapiBroadPolicyInitEntry(policy);

    osapiSemaGive(policyTableSema);

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
L7_RC_t hapiBroadPolicyPriorityRuleAdd(BROAD_POLICY_t policy,
                                       BROAD_POLICY_RULE_t *rule,
                                       BROAD_POLICY_RULE_PRIORITY_t priority)
{
    int                             i;
    BROAD_POLICY_TABLE_ENTRY_t     *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t      *rulePtr;
    L7_uchar8                      *maskPtr;
    BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (policyPtr->ruleCount >= BROAD_MAX_RULES_PER_POLICY)
    {
        osapiSemaGive(policyTableSema);
        LOG_MSG("Rule count exceeded.\n");
        return L7_FAILURE;
    }

    if(priority > BROAD_POLICY_RULE_PRIORITY_HIGHEST)
    {
      osapiSemaGive(policyTableSema);
      LOG_MSG("Invalid priority.\n");
      return L7_FAILURE;
    }

    if (broadPolicyTableRulePoolFreePtr == L7_NULL)
    {
      osapiSemaGive(policyTableSema);
      LOG_MSG("Max Rule count exceeded.\n");
      return L7_FAILURE;
    }

    *rule   = policyPtr->ruleCount++;
    /* Allocate a rule. */
    ruleInfoPtr = policyPtr->ruleInfoPtr;
    if (ruleInfoPtr == L7_NULL)
    {
      /* First rule... */
      policyPtr->ruleInfoPtr          = broadPolicyTableRulePoolFreePtr;
      broadPolicyTableRulePoolFreePtr = broadPolicyTableRulePoolFreePtr->next;
      policyPtr->ruleInfoPtr->next    = L7_NULL;
      rulePtr                         = &policyPtr->ruleInfoPtr->ruleInfo;
    }
    else
    {
      /* Append new rule to end of the list. */
      while (ruleInfoPtr->next != L7_NULL)
      {
        ruleInfoPtr = ruleInfoPtr->next;
      }
      ruleInfoPtr->next               = broadPolicyTableRulePoolFreePtr;
      broadPolicyTableRulePoolFreePtr = broadPolicyTableRulePoolFreePtr->next;
      ruleInfoPtr->next->next         = L7_NULL;
      rulePtr                         = &ruleInfoPtr->next->ruleInfo;
    }

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

    rulePtr->ruleFlags = 0;
    rulePtr->priority = priority;
    
    osapiSemaGive(policyTableSema);

    //printf("%s(%d) policy=%u, *rule=%u broadPolicyTableRulePoolFreePtr=%u\r\n",__FUNCTION__,__LINE__,policy,*rule,((L7_uint32) broadPolicyTableRulePoolFreePtr - (L7_uint32) broadPolicyTableRulePool)/sizeof(BROAD_POLICY_RULE_POOL_ENTRY_t));

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a new rule to a policy. Uses the default rule priority.
* 
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleAdd(BROAD_POLICY_t policy, BROAD_POLICY_RULE_t *rule)
{
    return hapiBroadPolicyPriorityRuleAdd(policy, rule, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
}

/*********************************************************************
*
* @purpose Create a new rule and copy qualifiers, actions, meters, etc
*          from an existing rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleCopy(BROAD_POLICY_t       policy,
                                BROAD_POLICY_RULE_t  oldRule,
                                BROAD_POLICY_RULE_t *newRule)
{
    BROAD_POLICY_TABLE_ENTRY_t     *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t      *oldRulePtr;
    BROAD_POLICY_RULE_ENTRY_t      *newRulePtr = L7_NULL;
    BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;

    CHECK_POLICY(policy);
    CHECK_RULE(oldRule);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (policyPtr->ruleCount >= BROAD_MAX_RULES_PER_POLICY)
    {
        osapiSemaGive(policyTableSema);
        LOG_MSG("Rule count exceeded.\n");
        return L7_FAILURE;
    }

    if (oldRule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, oldRule);
        return L7_ERROR;
    }

    if (broadPolicyTableRulePoolFreePtr == L7_NULL)
    {
      osapiSemaGive(policyTableSema);
      LOG_MSG("Max Rule count exceeded.\n");
      return L7_FAILURE;
    }

    /* Allocate a rule. */
    ruleInfoPtr = policyPtr->ruleInfoPtr;
    if (ruleInfoPtr == L7_NULL)
    {
      /* First rule... we shouldn't get here since there should be an old rule to copy from. */
      LOG_ERROR(oldRule);
    }
    else
    {
      /* Append new rule to end of the list. */
      while (ruleInfoPtr->next != L7_NULL)
      {
        ruleInfoPtr = ruleInfoPtr->next;
      }
      ruleInfoPtr->next               = broadPolicyTableRulePoolFreePtr;
      broadPolicyTableRulePoolFreePtr = broadPolicyTableRulePoolFreePtr->next;
      ruleInfoPtr->next->next         = L7_NULL;
      newRulePtr                      = &ruleInfoPtr->next->ruleInfo;
    }

    *newRule = policyPtr->ruleCount++;

    oldRulePtr = hapiBroadPolicyGetRulePtr(policyPtr, oldRule);
    if (oldRulePtr == L7_NULL)
    {
      LOG_ERROR(oldRule);
    }

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

    osapiSemaGive(policyTableSema);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Deletes an existing rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleDelete(BROAD_POLICY_t policy, BROAD_POLICY_RULE_t rule)
{
    CHECK_POLICY(policy);
    CHECK_RULE(rule);

    /* TODO: delete a rule */

    return L7_FAILURE;
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
L7_RC_t hapiBroadPolicyRuleQualifierAdd(BROAD_POLICY_t       policy,
                                        BROAD_POLICY_RULE_t  rule,
                                        BROAD_POLICY_FIELD_t field,
                                        L7_uchar8*           value,
                                        L7_uchar8*           mask)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_uchar8                  *maskPtr;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);
    CHECK_FIELD(field);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
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
        // Disallow adding this field if either IP6_SRC or IP6_DST are already included
        // This is because these fields share the same memory space in BROAD_FIELD_ENTRY_t. This
        //   restriction can be removed if BROAD_FIELD_ENTRY_t is reorganized such that these
        //   fields do not share memory space.
      /* PTin commented */
//      if ((hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_IP6_SRC) == BROAD_FIELD_SPECIFIED) ||
//          (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_IP6_DST) == BROAD_FIELD_SPECIFIED))
//      {
//        osapiSemaGive(policyTableSema);
//        return L7_FAILURE;
//      }

        memcpy(hapiBroadPolicyFieldValuePtr(&rulePtr->fieldInfo, field), value, hapiBroadPolicyFieldSize(field));
        maskPtr = hapiBroadPolicyFieldMaskPtr(&rulePtr->fieldInfo, field);
        if (maskPtr != L7_NULL)
        {
          memcpy(maskPtr, mask,  hapiBroadPolicyFieldSize(field));
        }
        hapiBroadPolicyFieldFlagsSet(&rulePtr->fieldInfo, field, BROAD_FIELD_SPECIFIED);   // update field qualification for this rule
        break;

        /* PTin commented */
        /*
    case BROAD_FIELD_IP6_SRC:
    case BROAD_FIELD_IP6_DST:
        // Disallow adding this field if MACDA, MACSA, SIP, or DIP are already included
        // This is because these fields share the same memory space in BROAD_FIELD_ENTRY_t. This
        // restriction can be removed if BROAD_FIELD_ENTRY_t is reorganized such that these
        // fields do not share memory space.
        if ((hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_MACDA) == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_MACSA) == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_SIP)   == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_DIP)   == BROAD_FIELD_SPECIFIED) ||
            (hapiBroadPolicyFieldFlagsGet(&rulePtr->fieldInfo, BROAD_FIELD_DSCP)  == BROAD_FIELD_SPECIFIED))
        {
          osapiSemaGive(policyTableSema);
          return L7_FAILURE;
        }
        // fall through
        */

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
            osapiSemaGive(policyTableSema);
            return L7_FAILURE;
        }
        break;
    }

    osapiSemaGive(policyTableSema);

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
L7_RC_t hapiBroadPolicyRuleActionAdd(BROAD_POLICY_t        policy,
                                     BROAD_POLICY_RULE_t   rule,
                                     BROAD_POLICY_ACTION_t action,
                                     L7_uint32             param0,
                                     L7_uint32             param1,
                                     L7_uint32             param2)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);
    CHECK_ACTION(action);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }

    result = hapiBroadPolicyActionAdd(rulePtr, action, BROAD_POLICY_ACTION_CONFORMING, param0, param1, param2);

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Add action for metered YELLOW packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleExceedActionAdd(BROAD_POLICY_t          policy,
                                           BROAD_POLICY_RULE_t     rule,
                                           BROAD_POLICY_ACTION_t   action,
                                           L7_uint32               param0,
                                           L7_uint32               param1,
                                           L7_uint32               param2)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);
    CHECK_ACTION(action);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }

    result = hapiBroadPolicyActionAdd(rulePtr, action, BROAD_POLICY_ACTION_EXCEEDING, param0, param1, param2);

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Get action for metered YELLOW packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleExceedActionGet(BROAD_POLICY_t          policy,
                                           BROAD_POLICY_RULE_t     rule,
                                           BROAD_POLICY_ACTION_t  *action,
                                           L7_uint32              *param0,
                                           L7_uint32              *param1,
                                           L7_uint32              *param2)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }

    result = hapiBroadPolicyActionFind(rulePtr, BROAD_POLICY_ACTION_EXCEEDING, action, param0, param1, param2);

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Add action for metered RED packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleNonConfActionAdd(BROAD_POLICY_t          policy,
                                            BROAD_POLICY_RULE_t     rule,
                                            BROAD_POLICY_ACTION_t   action,
                                            L7_uint32               param0,
                                            L7_uint32               param1,
                                            L7_uint32               param2)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);
    CHECK_ACTION(action);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }

    result = hapiBroadPolicyActionAdd(rulePtr, action, BROAD_POLICY_ACTION_NONCONFORMING, param0, param1, param2);

    osapiSemaGive(policyTableSema);

    return result;
}


/*********************************************************************
*
* @purpose Get action for metered RED packets that match the rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleNonConfActionGet(BROAD_POLICY_t          policy,
                                            BROAD_POLICY_RULE_t     rule,
                                            BROAD_POLICY_ACTION_t  *action,
                                            L7_uint32              *param0,
                                            L7_uint32              *param1,
                                            L7_uint32              *param2)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }

    result = hapiBroadPolicyActionFind(rulePtr, BROAD_POLICY_ACTION_NONCONFORMING, action, param0, param1, param2);

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Clear all actions associated with a specific rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleActionClearAll(BROAD_POLICY_t        policy,
                                          BROAD_POLICY_RULE_t   rule)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }

    memset(&rulePtr->actionInfo, 0, sizeof(rulePtr->actionInfo));

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Add a meter to an existing rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleMeterAdd(BROAD_POLICY_t          policy,
                                    BROAD_POLICY_RULE_t     rule,
                                    BROAD_METER_ENTRY_t    *meterInfo)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
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
        osapiSemaGive(policyTableSema);
        LOG_MSG("Invalid meter info specified: cir %d, cbs %d, pir %d, pbs %d\n",
                meterInfo->cir, meterInfo->cbs, meterInfo->pir, meterInfo->pbs);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }

    if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
    {
        /* meters and counters are mutually exclusive */
        osapiSemaGive(policyTableSema);
        return L7_ERROR;
    }

    rulePtr->ruleFlags = BROAD_METER_SPECIFIED;
    memcpy(&rulePtr->u.meter.meterInfo, meterInfo, sizeof(*meterInfo));

    osapiSemaGive(policyTableSema);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a counter to existing rule.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyRuleCounterAdd(BROAD_POLICY_t       policy,
                                      BROAD_POLICY_RULE_t  rule,
                                      BROAD_COUNTER_MODE_t mode,
                                      bcm_port_t           port)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;

    CHECK_POLICY(policy);
    CHECK_RULE(rule);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (rule >= policyPtr->ruleCount)
    {
        osapiSemaGive(policyTableSema);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, 
            "Policy %d does not contain rule %d."
            " The rule was not added to the policy due to a discrepancy in "
            "the rule count for this specific policy .  Additionally, the "
            "message can be displayed when an old rule is being modified, "
            "but the old rule is not in the policy.", policy, rule);
        return L7_ERROR;
    }

    rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, rule);
    if (rulePtr == L7_NULL)
    {
      LOG_ERROR(rule);
    }
    
    if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
    {
        /* meters and counters are mutually exclusive */
        osapiSemaGive(policyTableSema);
        return L7_ERROR;
    }
    
    rulePtr->ruleFlags                  = BROAD_COUNTER_SPECIFIED;
    rulePtr->u.counter.counterInfo.port = port;
    rulePtr->u.counter.counterInfo.mode = mode;

    osapiSemaGive(policyTableSema);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Retrieve stats for all rules in a policy.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyStatsGet(BROAD_POLICY_t        policy,
                                L7_uint32            *numStats,
                                BROAD_POLICY_STATS_t *stats)
{
    int                         i, rv;
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if (*numStats > policyPtr->ruleCount)
        *numStats = policyPtr->ruleCount;

    /* init stat struct prior to collecting policy stats */
    for (i = 0; i < *numStats; i++)
    {
        rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, i);
        if (rulePtr == L7_NULL)
        {
          LOG_ERROR(i);
        }

        if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
        {
            /* metered mode */
            stats[i].meter = L7_TRUE;
            stats[i].statMode.meter.in_prof  = 0;
            stats[i].statMode.meter.out_prof = 0;
        }
        else
        {
            /* counted mode */
            stats[i].meter = L7_FALSE;
            stats[i].statMode.counter.count = 0;
        }
    }

    rv = customx_policy_stats_get(policy, *numStats, stats);
    if (BCM_E_NONE != rv)
        result = L7_FAILURE;

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Commit a policy to hardware. Any actions that are dynamic
*          in nature are resolved at this time, e.g. redir to USP.
*
*          For performance reasons, all global policies are loaded
*          at this time.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyCommit(BROAD_POLICY_t policy)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    L7_BOOL                     downloadPolicy = L7_FALSE;
    L7_RC_t                     result = L7_SUCCESS;
    int                         rv;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    /* update any actions in the rule that require translation */
    result = hapiBroadPolicyActionUpdate(policyPtr);
    if (L7_SUCCESS != result)
    {
          osapiSemaGive(policyTableSema);
          return result;
    }

    if ((BROAD_POLICY_TYPE_PORT        != policyPtr->policyType) &&
        (BROAD_POLICY_TYPE_IPSG        != policyPtr->policyType) &&
        (BROAD_POLICY_TYPE_SYSTEM_PORT != policyPtr->policyType))
    {
        /* Send system/cosq/vlan policies to all units at this
         * time since they likely will apply to multiple ports.
         */
        downloadPolicy = L7_TRUE;
    }

    if (policyPtr->policyFlags & BROAD_POLICY_COMMITTED)
    {
        /* Update all units with the policy if it has changed. */
        downloadPolicy = L7_TRUE;
    
        policyPtr->policyFlags |= BROAD_POLICY_CHANGED;
    }

    if (L7_TRUE == downloadPolicy)
    {
        hapiBroadPolicyInfoConvert(policyPtr, &policyInfo);
        rv = usl_policy_create(policy, &policyInfo);
        if (BCM_E_NONE != rv)
        {
          /* clean up */
          usl_policy_destroy(policy);
          result = L7_FAILURE;
        }
    }

    if (L7_SUCCESS == result)
        policyPtr->policyFlags |= BROAD_POLICY_COMMITTED;

    if (hapiBroadPolicyDebugLevel() > POLICY_DEBUG_NONE)
        hapiBroadPolicyDebug(policy);

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Check if policy is already configured on the given interface
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPolicyIfaceCheck(BROAD_POLICY_t policy, bcmx_lport_t lport)
{
    int                         rv;
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    L7_RC_t                     result = L7_NOT_EXIST;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    /* check policy against the specified port */
    rv = customx_port_policy_check(policy, lport);
    if (BCM_E_EXISTS == rv)
        result = L7_ALREADY_CONFIGURED;

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
   L7_RC_t result = L7_SUCCESS;
   int rv = BCM_E_NONE, directed = 0;
   static int mirrorCounter=0;
   static bcmx_lport_t mtpPort=BCMX_LPORT_INVALID;
   bcmx_lport_t dest_lport;
   BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
   BROAD_POLICY_RULE_ENTRY_t *rulePtr;
   BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;
   BROAD_ACTION_ENTRY_t      *actionPtr;
   DAPI_USP_t usp;
   BROAD_PORT_t *hapiMirrorToPortPtr;

   bcmx_switch_control_get(bcmSwitchDirectedMirroring, &directed);

   if ((hapiBroadXGS2Check() == L7_TRUE) || (directed != 0))
   {
      return L7_SUCCESS;
   }

   policyPtr = &broadPolicyTable[policy];
   ruleInfoPtr = policyPtr->ruleInfoPtr;

   /* Check if the policy has MIRROR action */
   while (ruleInfoPtr != L7_NULL)
   {
       rulePtr   = &ruleInfoPtr->ruleInfo;
       actionPtr = &rulePtr->actionInfo;

       if (BROAD_ACTION_IS_SPECIFIED(actionPtr, BROAD_ACTION_MIRROR))
       {
         dest_lport = bcmx_modid_port_to_lport(actionPtr->parms.modid,
                                               actionPtr->parms.modport);

         usp = actionPtr->parms.usp;
         hapiMirrorToPortPtr = HAPI_PORT_GET(&usp, dapi_g);

         if (flags == L7_ENABLE) 
         {
             /* Only one MTP port is support with legacy mirroring. All policies 
              * must use same MTP port. 
              */
             if (mirrorCounter == 0) 
             {
                rv = bcmx_mirror_port_set (lport, dest_lport, BCM_MIRROR_PORT_ENABLE);
                if (rv == BCM_E_NONE) 
                {
                    mtpPort = dest_lport;

                    /* Do not use hapiBroadVlanIngressFilterSet() here as we need egress
                     * filtering to be disabled - the mirrored packets may not be on the
                     * same vlan as the probe port */
                    rv = l7_bcmx_port_vlan_member_set(dest_lport, BCM_PORT_VLAN_MEMBER_INGRESS);
                    if (L7_BCMX_OK(rv) != L7_TRUE)
                    {
                      return L7_FAILURE;
                    }

                    rv = l7_bcmx_port_untagged_vlan_set(dest_lport, HPC_STACKING_VLAN_ID);
                    if (L7_BCMX_OK(rv) != L7_TRUE)
                    {
                      return L7_FAILURE;
                    }
                }
                else  
                {
                   break;
                }
             }
             else if ((mtpPort != BCMX_LPORT_INVALID) && (mtpPort != dest_lport))
             {
                /* Donot allow multiple MTP ports at a time */
                return L7_FAILURE;
             }
             mirrorCounter++; 
         }
         else if (mirrorCounter)
         {
             mirrorCounter--;
             /* If all the mirror policies are removed, Disable mirroring */
             if (mirrorCounter == 0)
             {
                rv = bcmx_mirror_port_set(lport, dest_lport, BCM_MIRROR_DISABLE);

                if (rv == BCM_E_NONE)
                {
                  result = hapiBroadVlanIngressFilterSet(dest_lport, hapiMirrorToPortPtr->ingressFilteringEnabled);

                  if (result != L7_SUCCESS)
                  {
                    return L7_FAILURE;
                  }

                  rv = l7_bcmx_port_untagged_vlan_set(dest_lport, hapiMirrorToPortPtr->pvid);
                  if (L7_BCMX_OK(rv) != L7_TRUE)
                  {
                    return L7_FAILURE;
                  }
                }
             }
         }
       }

       ruleInfoPtr = ruleInfoPtr->next;
   }
   return ((rv == BCM_E_NONE) ? L7_SUCCESS : L7_FAILURE);
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
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
      osapiSemaGive(policyTableSema);
      return L7_NOT_EXIST;
    }

    if ((policyPtr->policyType == BROAD_POLICY_TYPE_VLAN) || (policyPtr->policyType == BROAD_POLICY_TYPE_ISCSI))
    {
      osapiSemaGive(policyTableSema);
      return L7_SUCCESS;
    }

    /* apply policy to the specified port */
    rv = customx_port_policy_apply(policy, lport);
    if (BCM_E_NOT_FOUND == rv)
    {
      /* The policy may not exist on the port's unit. Create it and try again. */
      hapiBroadPolicyInfoConvert(policyPtr, &policyInfo);
      rv = customx_port_policy_create(policy, &policyInfo, lport);
      if (BCM_E_NONE == rv)
      {
        rv = customx_port_policy_apply(policy, lport);
      }
    }

    if (BCM_E_NONE != rv)
    {
      result = L7_FAILURE;
    }
    else
    {
      /* Check to see the policy has MIRROR action. If yes, configure */
      result = hapiBroadPolicySetMirroringPath(policy, lport, L7_ENABLE);
    }

    osapiSemaGive(policyTableSema);

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
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
      osapiSemaGive(policyTableSema);
      return L7_SUCCESS;
    }

    if ((policyPtr->policyType == BROAD_POLICY_TYPE_VLAN) || (policyPtr->policyType == BROAD_POLICY_TYPE_ISCSI))
    {
      osapiSemaGive(policyTableSema);
      return L7_SUCCESS;
    }

    /* remove policy from the specified port */
    rv = customx_port_policy_remove(policy, lport);
    if (BCM_E_NONE != rv)
    {
        result = L7_FAILURE;
    }
    else 
    {
        /* Check if the policy has mirror action. If yes, unconfigure */
        hapiBroadPolicySetMirroringPath(policy, lport, L7_DISABLE);
    }

    osapiSemaGive(policyTableSema);

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
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
          osapiSemaGive(policyTableSema);
          return L7_NOT_EXIST;
    }

    if ((policyPtr->policyType == BROAD_POLICY_TYPE_VLAN) || (policyPtr->policyType == BROAD_POLICY_TYPE_ISCSI))
    {
      osapiSemaGive(policyTableSema);
      return L7_SUCCESS;
    }

    /* apply policy to all ethernet ports */
    rv = usl_policy_apply_all(policy);
    if (BCM_E_NONE != rv)
    {
        result = L7_FAILURE;
    }

    osapiSemaGive(policyTableSema);

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
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    L7_RC_t                     result = L7_SUCCESS;

    CHECK_POLICY(policy);

    osapiSemaTake(policyTableSema, L7_WAIT_FOREVER);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
    {
      osapiSemaGive(policyTableSema);
      return L7_SUCCESS;
    }

    if ((policyPtr->policyType == BROAD_POLICY_TYPE_VLAN) || (policyPtr->policyType == BROAD_POLICY_TYPE_ISCSI))
    {
      osapiSemaGive(policyTableSema);
      return L7_SUCCESS;
    }

    /* remove policy from all ethernet ports */
    rv = usl_policy_remove_all(policy);
    if (BCM_E_NONE != rv)
    {
        result = L7_FAILURE;
    }

    osapiSemaGive(policyTableSema);

    return result;
}

/*********************************************************************
*
* @purpose Specify whether policy applies to FPS ports in addition to
*          all  Ethernet ports.
* @end
*********************************************************************/
L7_RC_t hapiBroadPolicyEnableFPS(BROAD_POLICY_t policy)
{
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;

    CHECK_POLICY(policy);

    policyPtr = &broadPolicyTable[policy];
    if (!(policyPtr->flags & POLICY_USED))
        return L7_NOT_EXIST;
 
    policyPtr->policyFlags |= BROAD_POLICY_FPS;
   
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
  L7_uint32  maxIfpCosRules;
  L7_uint32  maxIfpSystemRules;
  L7_uint32  maxIfpUserRules;
  L7_uint32  maxEfpRules;
} npdRulesCapacity_t;

/* The following table is based on how we actually use the various devices. The sqsets
   that we generally use may require different resources on various chips. For example, 
   our standard L3/L4 sqset may use double-wide mode on one chip and may use intraslice 
   double-wide mode on another.  */
#ifdef L7_IPV6_PACKAGE
#define MAX_COS_SLICES    1
#define MAX_SYSTEM_SLICES 3
#else
#define MAX_COS_SLICES    2
#define MAX_SYSTEM_SLICES 2
#endif

npdRulesCapacity_t npdRulesCapacity[] = {
  {__BROADCOM_56304_ID, 0,       MAX_COS_SLICES * 128, MAX_SYSTEM_SLICES * 128, 2 * 128,  0},
  {__BROADCOM_56314_ID, 0,       MAX_COS_SLICES * 128, MAX_SYSTEM_SLICES * 128, 2 * 128,  0},
  {__BROADCOM_56504_ID, 0,       MAX_COS_SLICES * 128, MAX_SYSTEM_SLICES * 128, 6 * 128,  0},
  {__BROADCOM_56514_ID, 4 * 128, MAX_COS_SLICES * 256, MAX_SYSTEM_SLICES * 256, 6 * 256,  4 * 128},
  {__BROADCOM_56680_ID, 4 * 128, MAX_COS_SLICES * 256, MAX_SYSTEM_SLICES * 256, 12 * 128, 4 * 128},
  {__BROADCOM_56624_ID, 4 * 256, MAX_COS_SLICES * 512, MAX_SYSTEM_SLICES * 512, 12 * 256, 4 * 256},
  {__BROADCOM_56820_ID, 4 * 256, MAX_COS_SLICES * 128, MAX_SYSTEM_SLICES * 128, 12 * 128, 4 * 128},
  {__BROADCOM_56634_ID, 4 * 256, MAX_COS_SLICES * 512, MAX_SYSTEM_SLICES * 512, 12 * 256, 4 * 256},
  {__BROADCOM_56524_ID, 4 * 256, MAX_COS_SLICES * 512, MAX_SYSTEM_SLICES * 512, 12 * 256, 4 * 256},
  {__BROADCOM_56636_ID, 4 * 256, MAX_COS_SLICES * 512, MAX_SYSTEM_SLICES * 512, 12 * 256, 4 * 256},
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
* @purpose Returns how many COS rules can fit into each BCM unit
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxCosRulesPerUnit()
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

    /* For default value, assume 128 rules per COS slice. */
    max_rules = MAX_COS_SLICES * 128;
    numEntries = sizeof(npdRulesCapacity) / sizeof(npdRulesCapacity_t);
    for (i = 0; i < numEntries; i++)
    {
      if (npdRulesCapacity[i].npdId == board_info->npd_id)
      {
        max_rules = npdRulesCapacity[i].maxIfpCosRules;
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
    max_rules = MAX_SYSTEM_SLICES * 128;
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
}

/*********************************************************************
*
* @purpose Returns how many policies can exist in the system.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxPolicies()
{
  static L7_BOOL   first_time   = L7_TRUE;
  static L7_uint32 max_policies = 0;
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
    max_policies += hapiBroadMaxCosRulesPerUnit() * L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU / 8;

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
}

/*********************************************************************
*
* @purpose Returns how many policies can exist in each BCM unit.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxPoliciesPerBcmUnit()
{
  static L7_BOOL first_time                  = L7_TRUE;
  static L7_uint32 max_policies_per_bcm_unit = 0;
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
    max_policies_per_bcm_unit += hapiBroadMaxCosRulesPerUnit() / 8;

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
}

/*********************************************************************
*
* @purpose Returns how many rules can exist in the system.
* @end
*********************************************************************/
L7_uint32 hapiBroadMaxRules()
{
  static L7_BOOL first_time       = L7_TRUE;
  static L7_uint32 max_rules      = 0;
  L7_uint32 max_ifp_rules         = 0;
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

    max_rules += BROAD_MAX_SYSTEM_RULES;    /* This is an approximation of the total rules in the system policies (not including VLAN ACLs). */

    /* Allow up to 8 rules per COS policy. Note however that the COS rules cannot exceed the HW capabilities. */
    max_rules += min((L7_MAX_PORT_COUNT * 8),
                     (hapiBroadMaxCosRulesPerUnit() * L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU));

    max_ifp_rules = min((L7_ACL_VLAN_MAX_COUNT+L7_MAX_PORT_COUNT) * BROAD_MAX_RULES_PER_POLICY,
                        (hapiBroadMaxUserRulesPerUnit() * L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU));
    max_rules += max_ifp_rules;
    max_rules += hapiBroadMaxVfpRulesPerUnit() * L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU;
    max_rules += hapiBroadMaxEfpRulesPerUnit() * L7_MAX_UNITS_PER_STACK * BROAD_MAX_BCM_UNITS_PER_CPU;
  }

  return max_rules;
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
    int                         i;
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;

    sysapiPrintf("\nPolicy Table (%d bytes)\n", sizeof(broadPolicyTable));
    sysapiPrintf("  Id   Type  Flags  Rules\n");
    for (i = 0; i < BROAD_MAX_POLICIES; i++)
    {
        policyPtr = &broadPolicyTable[i];

        if (policyPtr->flags & POLICY_USED)
        {
            sysapiPrintf("[%4d] ", i);
            sysapiPrintf("%11s  %4x  %5d  ",
                         hapiBroadPolicyTypeName(policyPtr->policyType),
                         policyPtr->policyFlags,
                         policyPtr->ruleCount);
            sysapiPrintf("\n");
        }
    }
}

void hapiBroadPolicyDebugAction(BROAD_ACTION_ENTRY_t *actionPtr, BROAD_POLICY_ACTION_t action, BROAD_POLICY_ACTION_SCOPE_t action_scope)
{
  switch (action)
  {
  case BROAD_ACTION_REDIRECT:
  case BROAD_ACTION_REDIRECT_TRUNK:   /* PTin added */
  case BROAD_ACTION_MIRROR:
    sysapiPrintf("{%02x, %02x, %02x}\n", actionPtr->parms.usp.unit, actionPtr->parms.usp.slot, actionPtr->parms.usp.port);
    break;

  case BROAD_ACTION_SET_COSQ:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_cosq[action_scope]);
    break;

  case BROAD_ACTION_SET_DSCP:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_dscp[action_scope]);
    break;

  case BROAD_ACTION_SET_TOS:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_tos[action_scope]);
    break;

  case BROAD_ACTION_SET_USERPRIO:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_userprio[action_scope]);
    break;

  case BROAD_ACTION_SET_DROPPREC:
    sysapiPrintf("{%02x, %02x, %02x}\n", actionPtr->parms.set_dropprec.conforming, 
                                         actionPtr->parms.set_dropprec.exceeding, 
                                         actionPtr->parms.set_dropprec.nonconforming);
    break;

  case BROAD_ACTION_SET_OUTER_VID:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_ovid);
    break;

  case BROAD_ACTION_SET_INNER_VID:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_ivid);
    break;

  case BROAD_ACTION_ADD_OUTER_VID:
    sysapiPrintf("{%02x}\n", actionPtr->parms.add_ovid);
    break;

  case BROAD_ACTION_SET_CLASS_ID:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_class_id);
    break;

  case BROAD_ACTION_SET_REASON_CODE:
    sysapiPrintf("{%02x}\n", actionPtr->parms.set_reason);
    break;

  case BROAD_ACTION_SET_COSQ_AS_PKTPRIO:  /* PTin added */
  default:
    sysapiPrintf("\n");
    break;
  }
}

void hapiBroadPolicyDebugRule(BROAD_POLICY_RULE_ENTRY_t *rulePtr)
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

            hapiBroadPolicyDebugAction(&rulePtr->actionInfo, i, BROAD_POLICY_ACTION_CONFORMING);
        }
        if (BROAD_EXCEEDING_ACTION_IS_SPECIFIED((&rulePtr->actionInfo), i))
        {
          sysapiPrintf("     Action %10s: ",
                       hapiBroadPolicyActionName(i));

          sysapiPrintf("/EXCEED /");

          hapiBroadPolicyDebugAction(&rulePtr->actionInfo, i, BROAD_POLICY_ACTION_EXCEEDING);
        }
        if (BROAD_NONCONFORMING_ACTION_IS_SPECIFIED((&rulePtr->actionInfo), i))
        {
          sysapiPrintf("     Action %10s: ",
                       hapiBroadPolicyActionName(i));

          sysapiPrintf("/NONCONF/");

          hapiBroadPolicyDebugAction(&rulePtr->actionInfo, i, BROAD_POLICY_ACTION_NONCONFORMING);
        }
    }

    if (rulePtr->ruleFlags & BROAD_METER_SPECIFIED)
    {
        sysapiPrintf("     Metered  (%c%c)    : CIR %d CBS %d PIR %d PBS %d\n",
                     rulePtr->u.meter.meterInfo.colorMode == BROAD_METER_COLOR_BLIND ? 'C' : '-',
                     rulePtr->ruleFlags & BROAD_METER_SHARED ? 'S' : '-',
                     rulePtr->u.meter.meterInfo.cir, rulePtr->u.meter.meterInfo.cbs,
                     rulePtr->u.meter.meterInfo.pir, rulePtr->u.meter.meterInfo.pbs);
    }

    if (rulePtr->ruleFlags & BROAD_COUNTER_SPECIFIED)
    {
        sysapiPrintf("     Counted  (%c%c)\n",
                     rulePtr->u.counter.counterInfo.mode == BROAD_COUNT_PACKETS ? 'P' : 'B',
                     rulePtr->ruleFlags & BROAD_COUNTER_SHARED ? 'S' : '-');
    }
}

void hapiBroadPolicyDebug(BROAD_POLICY_t policy)
{
    int                         i;
    BROAD_POLICY_TABLE_ENTRY_t *policyPtr;
    BROAD_POLICY_RULE_ENTRY_t  *rulePtr;

    if (policy >= BROAD_MAX_POLICIES)
    {
        sysapiPrintf("Invalid policy (%d), valid range is [0 .. %d)\n",
                     policy, BROAD_MAX_POLICIES);
        return;
    }

    policyPtr = &broadPolicyTable[policy];

    if (policyPtr->flags & POLICY_USED)
    {
        sysapiPrintf("\nPolicy %d\n", policy);
        for (i = 0; i < policyPtr->ruleCount; i++)
        {
            sysapiPrintf("   Rule %d\n", i);
            rulePtr = hapiBroadPolicyGetRulePtr(policyPtr, i);
            if (rulePtr != L7_NULL)
            {
                hapiBroadPolicyDebugRule(rulePtr);
            }
        }
    }
}

BROAD_POLICY_STAGE_t defaultPolicyStage = BROAD_POLICY_STAGE_EGRESS;

void hapiBroadPolicyDebugDefaultStageSet(BROAD_POLICY_STAGE_t policyStage)
{
  defaultPolicyStage = policyStage;
}

/*********************************************************************
*
* @purpose Create a policy in HW w/ number of rules specified by user.
* @end
*********************************************************************/
L7_RC_t hapiBroadPolicyDebugRulesAdd(L7_uint32 ruleCount, L7_BOOL ipv6, L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  L7_RC_t             result;
  L7_uint32           i;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_uint32           dstIpValue = 0x01010101;
  L7_uint32           dstIpMask  = 0xFFFFFFFF;
  L7_uchar8           dstIp6Value[16];
  L7_uchar8           dstIp6Mask[16];
  DAPI_USP_t          usp;
  BROAD_PORT_t       *hapiPortPtr;

  memset(dstIp6Value, 0x00, sizeof(dstIp6Value));
  memset(dstIp6Mask,  0xff, sizeof(dstIp6Mask));

  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT, &policyId);
  hapiBroadPolicyStageSet(policyId, defaultPolicyStage);

  for (i = 0; i < ruleCount; i++)
  {
    hapiBroadPolicyRuleAdd(policyId, &ruleId);
    if (ipv6)
    {
      memcpy(dstIp6Value, &dstIpValue, sizeof(dstIpValue));
      hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_IP6_DST, (L7_uchar8 *)&dstIp6Value, (L7_uchar8 *)&dstIp6Mask);
    }
    else
    {
      hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_DIP, (L7_uchar8 *)&dstIpValue, (L7_uchar8 *)&dstIpMask);
    }
    hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);

    dstIpValue++;
  }

  result = hapiBroadPolicyCommit(policyId);

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;
  hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
  result = hapiBroadPolicyApplyToIface(policyId, hapiPortPtr->bcmx_lport);

  return result;
}

/*********************************************************************
*
* @purpose Create a policy in HW w/ number of rules specified by user.
* @end
*********************************************************************/
L7_RC_t hapiBroadPolicyDebugVlanRulesAdd(L7_uint32 ruleCount, L7_BOOL ipv6, L7_ushort16 vlanId)
{
  L7_RC_t             result;
  L7_uint32           i;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_uint32           dstIpValue = 0x01010101;
  L7_uint32           dstIpMask  = 0xFFFFFFFF;
  L7_uchar8           dstIp6Value[16];
  L7_uchar8           dstIp6Mask[16];

  memset(dstIp6Value, 0x00, sizeof(dstIp6Value));
  memset(dstIp6Mask,  0xff, sizeof(dstIp6Mask));

  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_VLAN, &policyId);
  hapiBroadPolicyStageSet(policyId, defaultPolicyStage);

  for (i = 0; i < ruleCount; i++)
  {
    hapiBroadPolicyRuleAdd(policyId, &ruleId);
    if (ipv6)
    {
      memcpy(dstIp6Value, &dstIpValue, sizeof(dstIpValue));
      hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_IP6_DST, (L7_uchar8 *)&dstIp6Value, (L7_uchar8 *)&dstIp6Mask);
    }
    else
    {
      hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_DIP, (L7_uchar8 *)&dstIpValue, (L7_uchar8 *)&dstIpMask);
    }
    hapiBroadPolicyRuleQualifierAdd(policyId, ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&vlanId, (L7_uchar8 *)&dstIpMask);
    hapiBroadPolicyRuleActionAdd(policyId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);

    dstIpValue++;
  }

  result = hapiBroadPolicyCommit(policyId);

  return result;
}

extern int policy_map_table_t_size();
extern int usl_small_policy_table_t_size();
extern int usl_policy_table_t_size();
extern int usl_num_large_system_policies();
extern int usl_num_small_system_policies();

int hapiBroadDebugPolicyFreeRuleCount()
{
  int                             freeRuleCount = 0;
  BROAD_POLICY_RULE_POOL_ENTRY_t *ruleInfoPtr;

  ruleInfoPtr = broadPolicyTableRulePoolFreePtr;

  while (ruleInfoPtr)
  {
    freeRuleCount++;
    ruleInfoPtr = ruleInfoPtr->next;
  }

  return freeRuleCount;
}

void hapiBroadDebugPolicyMemory()
{
  printf("BROAD_MAX_POLICIES                 == %d\n", BROAD_MAX_POLICIES);
  printf("BROAD_MAX_POLICIES_PER_BCM_UNIT    == %d\n", BROAD_MAX_POLICIES_PER_BCM_UNIT);
  printf("BROAD_MAX_RULES_PER_POLICY         == %d\n", BROAD_MAX_RULES_PER_POLICY);
  printf("BROAD_MAX_RULES                    == %d (%d free)\n", BROAD_MAX_RULES, hapiBroadDebugPolicyFreeRuleCount());
  printf("BROAD_MAX_SYSTEM_POLICIES          == %d\n", BROAD_MAX_SYSTEM_POLICIES);
  printf("BROAD_MAX_RULES_PER_SMALL_POLICY   == %d\n", BROAD_MAX_RULES_PER_SMALL_POLICY);
  printf("sizeof(policy_map_table_t)         == %d\n", policy_map_table_t_size());
  printf("sizeof(BROAD_POLICY_ENTRY_t)       == %d\n", sizeof(BROAD_POLICY_ENTRY_t));
  printf("sizeof(BROAD_POLICY_SMALL_ENTRY_t) == %d\n", sizeof(BROAD_POLICY_SMALL_ENTRY_t));
  printf("sizeof(BROAD_POLICY_RULE_ENTRY_t)  == %d\n", sizeof(BROAD_POLICY_RULE_ENTRY_t));
  printf("sizeof(BROAD_FIELD_ENTRY_t)        == %d\n", sizeof(BROAD_FIELD_ENTRY_t));
  printf("sizeof(BROAD_ACTION_ENTRY_t)       == %d\n", sizeof(BROAD_ACTION_ENTRY_t));
  printf("sizeof(BROAD_POLICY_TABLE_ENTRY_t) == %d\n", sizeof(BROAD_POLICY_TABLE_ENTRY_t));
#ifdef L7_STACKING_PACKAGE
  printf("sizeof(usl_policy_table_t)         == %d\n", usl_policy_table_t_size());
  printf("sizeof(usl_small_policy_table_t)   == %d\n", usl_small_policy_table_t_size());
#endif
  printf("\n\n");

  printf("policy_map_table          == %d\n", BROAD_MAX_BCM_UNITS_PER_CPU * policy_map_table_t_size() * BROAD_MAX_POLICIES_PER_BCM_UNIT);
  printf("policyPartialData         == %d\n", 2 * sizeof(BROAD_POLICY_ENTRY_t));
  printf("broadUtilTable            == %d\n", 16 * BROAD_MAX_POLICIES);
  printf("broadPolicyTable          == %d\n", sizeof(BROAD_POLICY_TABLE_ENTRY_t) * BROAD_MAX_POLICIES);
  printf("broadPolicyTableRulePool  == %d\n", BROAD_MAX_RULES * sizeof(BROAD_POLICY_RULE_POOL_ENTRY_t));
#ifdef L7_STACKING_PACKAGE
  printf("policy_table              == %d\n", usl_policy_table_t_size() * usl_num_large_system_policies());
  printf("small_policy_table        == %d\n", usl_small_policy_table_t_size() * usl_num_small_system_policies());
#endif
}
