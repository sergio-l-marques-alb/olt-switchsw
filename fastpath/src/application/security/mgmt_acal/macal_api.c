/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename macal_api.c
*
* @purpose Management Access Control and Administration List API functions
*
* @component Management Access Control and Administration List
*
* @comments none
*
* @create 05/05/2005
*
* @author stamboli
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "macal.h"
#include "macal_api.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "l7_ip_api.h"

extern macalCfgData_t  *macalCfgData;
extern osapiRWLock_t    macalRwLock;

/*********************************************************************
*
* @purpose  Create a new management ACAL.
*
* @param    name     @b{(input)} the ACAL name.
*
* @returns  L7_SUCCESS, if the ACAL is created
* @returns  L7_FAILURE, if failed creating MACAL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalListCreate(L7_char8 *name)
{
  const L7_char8 *fnName = "macalListCreate()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  /* Check if the list already exists */
  if (macalCfgData->macalList.creationStatus == L7_CREATED)
  {
    if (strcmp(macalCfgData->macalList.name, name) == L7_NULL)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
              "%s(): List %s already exists.\n", fnName, name);
      (void)osapiWriteLockGive(macalRwLock);
      return L7_SUCCESS;
    }
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: A list already exists. Only single list is allowed to exist in the system.\n", fnName);
    return L7_FAILURE;
  }
  /* List name validation */
  if (strlen(name) == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: Empty list name not allowed\n", fnName);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }
  if (strlen(name) > MACAL_MAX_LISTNAME_LEN)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: Listname %s truncated to %u characters\n", fnName, name, MACAL_MAX_LISTNAME_LEN);
  }

  /* Create list, that is, set the name and mark the creation status */
  strncpy(macalCfgData->macalList.name, name, MACAL_MAX_LISTNAME_LEN);
  macalCfgData->macalList.name[MACAL_MAX_LISTNAME_LEN] = L7_EOS;
  macalCfgData->macalList.creationStatus = L7_CREATED;

  /* Fill all the rules with defaults */
  (void) macalBuildDefaultRules(ALL_RULES);

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete an existing ACAL.
*
* @param    name     @b{(input)} the ACAL name.
*
* @returns  L7_SUCCESS, if the ACAL is deleted
* @returns  L7_FAILURE, if failed deleting the ACAL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalListDelete(L7_char8 *name)
{
  const L7_char8 *fnName = "macalListDelete()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list exists */
  if (strcmp(macalCfgData->macalList.name, name) != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

  /* Effectively delete the list */
  memset(macalCfgData->macalList.name, 0, sizeof(macalCfgData->macalList.name));
  macalCfgData->macalList.activationStatus = L7_DEACTIVATED;
  macalCfgData->macalList.creationStatus = L7_DELETED;

  /* Fill all the rules with defaults */
  (void) macalBuildDefaultRules(ALL_RULES);

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Activate an ACAL
*
* @param    name     @b{(input)} the ACAL name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalActivate(L7_char8 *name)
{
  const L7_char8 *fnName = "macalActivate()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list exists */
  if (strcmp(MACAL_CONSOLE_ONLY, name) == 0)
  {
    macalCfgData->consoleOnly = L7_TRUE;
  }
  else if (strcmp(macalCfgData->macalList.name, name) == 0)
  {
    macalCfgData->consoleOnly = L7_FALSE;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

  /* Register with sysnet for local IP packets */
  if (macalSysnetRegisterDeregister(L7_TRUE) != L7_SUCCESS)
  {
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

  if (macalCfgData->consoleOnly == L7_FALSE)
  {
    macalCfgData->macalList.activationStatus = L7_ACTIVATED;
  }

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deactivate an ACAL
*
* @param    name     @b{(input)} the ACAL name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalDeactivate(L7_char8 *name)
{
  const L7_char8 *fnName = "macalDeactivate()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list exists */
  if (strcmp(MACAL_CONSOLE_ONLY, name) == 0)
  {
    macalCfgData->consoleOnly = L7_FALSE;
  }
  else if (strcmp(macalCfgData->macalList.name, name) != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

  /* Deregister with sysnet */
  macalSysnetRegisterDeregister(L7_FALSE);

  macalCfgData->macalList.activationStatus = L7_DEACTIVATED;

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To add an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name
* @param    priority @b{(input)} rule priority used as the rule index
*
* @returns  L7_SUCCESS, if the rule is added
* @returns  L7_FAILURE, if failed adding the rule
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleAdd(L7_char8* name, L7_uint32 *priority)
{
  const L7_char8 *fnName = "macalRuleAdd()";
  L7_uint32 i;

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list exists */
  if (strcmp(macalCfgData->macalList.name, name) != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }
  /* Check if the priority is a valid number */
  if (*priority == MACAL_REQUEST_PRIORITY)
  {
    *priority = 1;

    /* find first unused rule */
    for (i = MACAL_MAX_NUM_RULES; i >= 1; i--)
    {
      if (macalCfgData->macalList.rule[i].activationStatus == L7_ACTIVATED)
      {
        *priority = i + 1;
        break;
      }
    }
    /* check for no rules left */
    if (i == MACAL_MAX_NUM_RULES)
    {
      *priority = 0;
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
              "%s: List %s has no rules left.\n", fnName, name);
      (void)osapiWriteLockGive(macalRwLock);
      return L7_FAILURE;
    }
  }
  else if ((*priority < MACAL_MIN_PRIORITY) || (*priority > MACAL_MAX_PRIORITY))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: invalid priority value %u.\n", fnName, *priority);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }
  /* Check if the rule exists */
  if (macalCfgData->macalList.rule[*priority].activationStatus == L7_ACTIVATED)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: Rule with priority %u already exists. Overwriting it\n", fnName, *priority);
  }
  /* Fill the rule with defaults */
  (void) macalBuildDefaultRules(*priority);

  /* Mark the rule activated i.e. effectively created */
  macalCfgData->macalList.rule[*priority].activationStatus = L7_ACTIVATED;

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Remove a rule from an existing ACAL.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
*
* @returns  L7_SUCCESS, if the rule is removed
* @returns  L7_FAILURE, if failed removing the rule
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleRemove(L7_char8 *name, L7_uint32 priority)
{
  L7_RC_t rc;

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Verify the list existence, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleRemove()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiWriteLockGive(macalRwLock);
    return rc;
  }

  /* Mark the rule deactivated i.e. effectively deleted */
  macalCfgData->macalList.rule[priority].activationStatus = L7_DEACTIVATED;

  /* Fill the rule with defaults */
  (void) macalBuildDefaultRules(priority);

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add an action to an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name
* @param    priority @b{(input)} rule priority used as the rule index
* @param    action   @b{(input)} action type (permit/deny)
*
* @returns  L7_SUCCESS, if the rule is added
* @returns  L7_FAILURE, if failed adding the rule
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleActionAdd(L7_char8 *name, L7_uint32 priority, macalActionType_t actionType)
{
  L7_RC_t rc;

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Verify the list existence, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleActionAdd()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiWriteLockGive(macalRwLock);
    return rc;
  }

  macalCfgData->macalList.rule[priority].action = actionType;

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add the source ip address and net mask to an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    ipAddr   @b{(input)} the ip address
* @param    mask     @b{(input)} the net mask
*
* @returns  L7_SUCCESS, if source ip address and net mask are added
* @returns  L7_FAILURE, if failed to add ip address and net mask
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleSrcIpMaskAdd(L7_char8 *name, L7_uint32 priority, L7_uint32 ipAddress, L7_uint32 mask)
{
  L7_RC_t rc;
  const L7_char8 *fnName = "macalRuleSrcIpMaskAdd()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Verify the list existence, priority value and the rule existence. */
  rc = macalValidateListPriorityRule(fnName, name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiWriteLockGive(macalRwLock);
    return rc;
  }

  /* Add the source IP addr and mask if the list is not active */
  if (macalCfgData->macalList.activationStatus == L7_ACTIVATED)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s is active. It cannot be modified.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

  /* Assign the IP Addr and mask and mark their presence in the config mask */
  macalCfgData->macalList.rule[priority].srcIp = ipAddress;
  macalCfgData->macalList.rule[priority].srcMask = mask;
  macalCfgData->macalList.rule[priority].configMask |= MACAL_SRCIP;
  macalCfgData->macalList.rule[priority].configMask |= MACAL_SRCIP_MASK;

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add an interface (eth/LAG) to an existing ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    intIfNum @b{(input)} the internal interface number
*
* @returns  L7_SUCCESS, if the interface is added
* @returns  L7_FAILURE, if failed adding the interface
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleInterfaceAdd(L7_char8 *name, L7_uint32  priority, L7_uint32 intIfNum)
{
  L7_RC_t rc;
  const L7_char8 *fnName = "macalRuleInterfaceAdd()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Verify the list existence, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleSrcIpMaskGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiWriteLockGive(macalRwLock);
    return rc;
  }

  /* Add the interface if the list is not active */
  if (macalCfgData->macalList.activationStatus == L7_ACTIVATED)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s is active. It cannot be modified.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

#ifdef L7_ROUTING_PACKAGE
  if (L7_ENABLE == ipMapRtrAdminModeGet())
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_MGMT_ACAL_COMPONENT_ID,
            "MACL: Routing is operational. Therefore, when a management ACL is specified with an Ethernet port, "
            "it may not work as expected. Recommend applying management ACL to VLAN to ensure desired operation.");
  }
#endif

  /* Assign the interface and mark its presence in the config mask */
  macalCfgData->macalList.rule[priority].ifNum = intIfNum;
  macalCfgData->macalList.rule[priority].configMask |= MACAL_IF;

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add a VLAN to a rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    vlanId   @b{(input)} VLAN ID to be added
*
* @returns  L7_SUCCESS, if the VLAN ID is added
* @returns  L7_FAILURE, if failed adding the rule
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleVlanAdd(L7_char8 *name, L7_uint32  priority, L7_uint32 vlanId)
{
  L7_RC_t rc;
  const L7_char8 *fnName = "macalRuleVlanAdd()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Verify the list existence, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleSrcIpMaskGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiWriteLockGive(macalRwLock);
    return rc;
  }

  /* Add the VALN ID if the list is not active */
  if (macalCfgData->macalList.activationStatus == L7_ACTIVATED)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s is active. It cannot be modified.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

  /* Assign the VLAN ID and mark its presence in the config mask */
  macalCfgData->macalList.rule[priority].vlanId = vlanId;
  macalCfgData->macalList.rule[priority].configMask |= MACAL_VLAN;

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add a service to an existing ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    serviceType  @b{(input)} the service to be added
*
* @returns  L7_SUCCESS, if the service is added
* @returns  L7_FAILURE, if failed adding the service
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleServiceAdd(L7_char8 *name, L7_uint32 priority, macalServiceType_t serviceType)
{
  L7_RC_t rc;
  const L7_char8 *fnName = "macalRuleServiceAdd()";

  if (osapiWriteLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Verify the list existence, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleSrcIpMaskGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiWriteLockGive(macalRwLock);
    return rc;
  }

  /* Add the service if the list is not active */
  if (macalCfgData->macalList.activationStatus == L7_ACTIVATED)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s is active. It cannot be modified.\n", fnName, name);
    (void)osapiWriteLockGive(macalRwLock);
    return L7_FAILURE;
  }

  /* Assign the service type and mark its presence in the config mask */
  macalCfgData->macalList.rule[priority].serviceType = serviceType;
  macalCfgData->macalList.rule[priority].configMask |= MACAL_SERVICE;

  macalCfgData->cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the name of the ACAL already existing
*
* @param    name     @b{(output)} the ACAL name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Currently only one MACAL is supported so this API does not need
*           any other argument.
*
* @end
*
*********************************************************************/
L7_RC_t macalNameGet(L7_char8 *name)
{
  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list is created */
  if (macalCfgData->macalList.creationStatus != L7_CREATED)
  {
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }
  strcpy(name, macalCfgData->macalList.name);

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the active list.
*
* @param    name     @b{(output)} the ACAL name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalActiveListGet(L7_char8 *name)
{
  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (macalCfgData->consoleOnly == L7_TRUE)
  {
    strcpy(name, MACAL_CONSOLE_ONLY);
    (void)osapiReadLockGive(macalRwLock);
    return L7_SUCCESS;
  }

  /* Check if the list is created */
  if (macalCfgData->macalList.creationStatus != L7_CREATED)
  {
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }
  /* Check if the list is active */
  if (macalCfgData->macalList.activationStatus != L7_ACTIVATED)
  {
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }
  strcpy(name, macalCfgData->macalList.name);

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the first ACAL rule
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(output)} rule priority used as the rule index
*
* @returns  L7_SUCCESS, if the first rule for this ACAL was found
* @returns  L7_FAILURE, if no rules have been created for this ACL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleFirstGet(L7_char8 *name, L7_uint32 *priority)
{
  const L7_char8 *fnName = "macalRuleFirstGet()";
  L7_uint32 i;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list exists */
  if (strcmp(macalCfgData->macalList.name, name) != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, name);
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }
  /* Find the first active rule */
  for (i = 1; i < (MACAL_MAX_NUM_RULES + 1); i++)
  {
    if (macalCfgData->macalList.rule[i].activationStatus == L7_ACTIVATED)
    {
      *priority = i;
      (void)osapiReadLockGive(macalRwLock);
      return L7_SUCCESS;
    }
  }
  /* No active rule */
  (void)osapiReadLockGive(macalRwLock);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the next ACAL rule
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    nextPriority @b{(output)} the next rule index
*
* @returns  L7_SUCCESS, if the next rule index was found
* @returns  L7_FAILURE, if rule is the last valid rule ID for this ACL
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleNextGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *nextPriority)
{
  const L7_char8 *fnName = "macalRuleNextGet()";
  L7_uint32 i;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list exists */
  if (strcmp(macalCfgData->macalList.name, name) != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, name);
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }
  /* Get the next active rule */
  for (i = priority+1; i < (MACAL_MAX_NUM_RULES + 1); i++)
  {
    if (macalCfgData->macalList.rule[i].activationStatus == L7_ACTIVATED)
    {
      *nextPriority = i;
      (void)osapiReadLockGive(macalRwLock);
      return L7_SUCCESS;
    }
  }
  /* No next active rule */
  (void)osapiReadLockGive(macalRwLock);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Check if a rule with given priority (index) exists
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    bExists  @b{(output)} boolean indicating existence of the rule
*
* @returns  L7_SUCCESS, if the rule was found or not found
* @returns  L7_FAILURE, if the ACAL does not exist or priority is invalid
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleExists(L7_char8* name, L7_uint32 priority, L7_BOOL *bExists)
{
  const L7_char8 *fnName = "macalRuleActionGet()";

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the list exists */
  if (strcmp(macalCfgData->macalList.name, name) != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, name);
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }
  /* Check if the priority is a valid number */
  if (priority < MACAL_MIN_PRIORITY || priority > MACAL_MAX_PRIORITY)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: invalid priority value %u.\n", fnName, priority);
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }
  /* Check for the existence of the rule */
  if (macalCfgData->macalList.rule[priority].activationStatus == L7_ACTIVATED)
  {
    *bExists = L7_TRUE;
  }
  else
  {
    *bExists = L7_FALSE;
  }
  (void)osapiReadLockGive(macalRwLock);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the action for an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    action   @b{(output)} action type (permit/deny) (aclActionIndex_t)
*
* @returns  L7_SUCCESS, if the action is retrieved
* @returns  L7_FAILURE, if there is no action set
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleActionGet(L7_char8 *name, L7_uint32 priority, macalActionType_t *actionType)
{
  L7_RC_t rc;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Validate the List name, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleSrcIpMaskGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiReadLockGive(macalRwLock);
    return rc;
  }

  *actionType = macalCfgData->macalList.rule[priority].action;

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the source ip address and mask for an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    ipAddr   @b{(input)} the ip address
* @param    mask     @b{(input)} the ip address mask
*
* @returns  L7_SUCCESS, if source ip address and mask are retrieved
* @returns  L7_FAILURE,
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleSrcIpMaskGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *ipAddress, L7_uint32 *mask)
{
  L7_RC_t rc;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Verify the list existence, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleSrcIpMaskGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiReadLockGive(macalRwLock);
    return rc;
  }

  if ((macalCfgData->macalList.rule[priority].configMask & MACAL_SRCIP) == 0)
  {
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }

  *ipAddress = macalCfgData->macalList.rule[priority].srcIp;
  *mask = macalCfgData->macalList.rule[priority].srcMask;

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the interface (eth/LAG) for an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    intIfNum @b{(output)} the internal interface number
*
* @returns  L7_SUCCESS, if the interface is retrieved
* @returns  L7_FAILURE, if failed retrieving the interface
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleInterfaceGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *intIfNum)
{
  L7_RC_t rc;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Validate the List name, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleInterfaceGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  if ((macalCfgData->macalList.rule[priority].configMask & MACAL_IF) == 0)
  {
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }

  *intIfNum = macalCfgData->macalList.rule[priority].ifNum;

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the VLAN ID for an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    vlanId   the VLAN ID
*
* @returns  L7_SUCCESS, if the VLAN ID is retrieved
* @returns  L7_FAILURE, if failed to retrieve
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleVlanGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *vlanId)
{
  L7_RC_t rc;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Validate the List name, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleVlanGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiReadLockGive(macalRwLock);
    return rc;
  }

  if ((macalCfgData->macalList.rule[priority].configMask & MACAL_VLAN) == 0)
  {
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }

  *vlanId = macalCfgData->macalList.rule[priority].vlanId;

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the service type for an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    serviceType @b{(input)} the service type
*
* @returns  L7_SUCCESS, if service type is retrieved
* @returns  L7_FAILURE, if failed to retrieve
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleServiceGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *serviceType)
{
  L7_RC_t rc;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Validate the List name, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalRuleServiceGet()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiReadLockGive(macalRwLock);
    return rc;
  }

  if ((macalCfgData->macalList.rule[priority].configMask & MACAL_SERVICE) == 0)
  {
    (void)osapiReadLockGive(macalRwLock);
    return L7_FAILURE;
  }

  *serviceType = macalCfgData->macalList.rule[priority].serviceType;

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Validate the ACAL existence, priority value, and the rule existence.
*
* @param    name     @b{(input)} the function name that called this function.
* @param    name     @b{(input)} the ACAL name.
*
* @returns  L7_SUCCESS, if validation successful
* @returns  L7_FAILURE, if not
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t macalValidateListPriorityRule(const L7_char8 *fnName, L7_char8 *listName, L7_uint32 priority)
{
  /* Check if the list exists */
  if (strcmp(macalCfgData->macalList.name, listName) != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: List %s does not exist.\n", fnName, listName);
    return L7_FAILURE;
  }
  /* Check if the priority is a valid number */
  if (priority < MACAL_MIN_PRIORITY || priority > MACAL_MAX_PRIORITY)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: invalid priority value %u.\n", fnName, priority);
    return L7_FAILURE;
  }
  /* Check if the rule exists */
  if (macalCfgData->macalList.rule[priority].activationStatus == L7_DEACTIVATED)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "%s: Rule with priority %u does not exist.\n", fnName, priority);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Checks to see if a user has configured a field in a rule
*           in an access list
*
* @param    name          @b{(input)} the ACAL name.
* @param    priority      @b{(input)} rule priority used as the rule index
* @param    field         @b{(input)} ruleFields_t enum
* @param    isConfigured  @b{(output)} if configured or not
*
* @returns  L7_SUCCESS, if could find that the field has been configured or not
* @returns  L7_FALSE, if could not
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalIsFieldConfigured(L7_char8* name, L7_uint32 priority,
                               macalRuleFields_t field, L7_BOOL *isConfigured)
{
  L7_RC_t rc;
  L7_uint32 result;

  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Validate the List name, priority value and the rule existence. */
  rc = macalValidateListPriorityRule("macalIsFieldConfigured()", name, priority);
  if (rc != L7_SUCCESS)
  {
    (void)osapiReadLockGive(macalRwLock);
    return rc;
  }

  /* Check if this field is configured */
  result = (macalCfgData->macalList.rule[priority].configMask) & field;
  if (result != 0)
  {
    *isConfigured = L7_TRUE;
  }
  else
  {
    *isConfigured = L7_FALSE;
  }

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Check if the MACAL component is enabled.
*
* @returns  L7_TRUE
*
* @comments Currently it always returns true. It's used from SNMP only.
*
* @end
*
*********************************************************************/
L7_BOOL macalComponentEnabled()
{
  return L7_TRUE;

  /* At this point, there is not enough information available about what you
     can and cannot do when this component is enabled/disabled. So even if
     there is a variable that stores this value, it is not used anywhere in
     the code. */
  /* return  macalCfgData->enabled; */
}


/*********************************************************************
*
* @purpose  Check if a MACAL list is created.
*
* @param    isCreated  @b{(output)} if created or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t macalIsAnyListCreated(L7_BOOL *isCreated)
{
  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (macalCfgData->macalList.creationStatus == L7_CREATED)
  {
    *isCreated = L7_TRUE;
  }
  else
  {
    *isCreated = L7_FALSE;
  }

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Check if a MACAL list is activated.
*
* @param    isActivated  @b{(output)} if activated or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t macalIsAnyListActivated(L7_BOOL *isActivated)
{
  if (osapiReadLockTake(macalRwLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (macalCfgData->macalList.activationStatus == L7_ACTIVATED)
  {
    *isActivated = L7_TRUE;
  }
  else
  {
    *isActivated = L7_FALSE;
  }

  (void)osapiReadLockGive(macalRwLock);

  return L7_SUCCESS;
}
