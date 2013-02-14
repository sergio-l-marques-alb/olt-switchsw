/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
* @filename usmdb_macal.c
*
* @purpose USMDB API's for Management Access Control and Administration List (MACAL)
*
* @component Management Access Control and Administration List (MACAL)
*
* @create 05/05/2005
*
* @author stamboli
*
* @end
**********************************************************************/   

#include <stdio.h>
#include "l7_common.h"
#include "usmdb_macal_api.h"

/*********************************************************************
*
* @purpose  To create a new management ACAL.
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
L7_RC_t usmDbMacalListCreate(L7_char8* name)
{
  return macalListCreate(name);
}

/*********************************************************************
*
* @purpose  To delete an existing ACAL.
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
L7_RC_t usmDbMacalListDelete(L7_char8* name) 
{
  return macalListDelete(name);
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
L7_RC_t usmDbMacalActivate(L7_char8* name)
{
  return macalActivate(name);
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
L7_RC_t usmDbMacalDeactivate(L7_char8* name)
{
  return macalDeactivate(name);
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
L7_RC_t usmDbMacalRuleEntryAdd(L7_char8* name, L7_uint32 *priority)
{
  return macalRuleAdd(name, priority);
}

/*********************************************************************
*
* @purpose  To remove a rule from an existing ACAL.
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
L7_RC_t usmDbMacalRuleRemove(L7_char8* name, L7_uint32 priority)
{
  return macalRuleRemove(name, priority);
}

/*********************************************************************
*
* @purpose  To add an action to an ACAL rule.
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
L7_RC_t usmDbMacalRuleActionAdd(L7_char8* name, L7_uint32 priority, macalActionType_t actionType)
{
  return macalRuleActionAdd(name, priority, actionType);
}

/*********************************************************************
*
* @purpose  To add the source ip address and net mask to an ACAL rule.
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
L7_RC_t usmDbMacalRuleSrcIpMaskAdd(L7_char8* name, L7_uint32 priority, L7_uint32 ipAddress, L7_uint32 mask)
{
  return macalRuleSrcIpMaskAdd(name, priority, ipAddress, mask);
}

/*********************************************************************
*
* @purpose  To add an interface (eth/LAG) to an existing ACAL rule.
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
L7_RC_t usmDbMacalRuleInterfaceAdd(L7_char8* name, L7_uint32  priority, L7_uint32 intIfNum)
{
  return macalRuleInterfaceAdd(name, priority, intIfNum);
}

/*********************************************************************
*
* @purpose  To add a VLAN to a rule.
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
L7_RC_t usmDbMacalRuleVlanAdd(L7_char8* name, L7_uint32  priority, L7_uint32 vlanId)
{
  return macalRuleVlanAdd(name, priority, vlanId);
}

/*********************************************************************
*
* @purpose  To add a service to an existing ACAL rule.
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
L7_RC_t usmDbMacalRuleServiceAdd(L7_char8* name, L7_uint32 priority, macalServiceType_t serviceType)
{
  return macalRuleServiceAdd(name, priority, serviceType);
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
L7_RC_t usmDbMacalNameGet(L7_char8* name)
{
  return macalNameGet(name);
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
L7_RC_t usmDbMacalActiveListGet(L7_char8* name)
{
  return macalActiveListGet(name);
}

/*********************************************************************
*
* @purpose  Get the first ACAL rule
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(output)} rule priority used as the rule index
*
* @returns  L7_SUCCESS, if the first rule for this ACAL was found
* @returns  L7_FAILURE, 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMacalRuleFirstGet(L7_char8 *name, L7_uint32 *priority)
{
  return macalRuleFirstGet(name, priority);
}

/*********************************************************************
*
* @purpose  Get the next rule
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
L7_RC_t usmDbMacalRuleNextGet(L7_char8 *name, L7_uint32 priority, L7_uint32 * nextPriority)
{
  return macalRuleNextGet(name, priority, nextPriority);
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
L7_RC_t usmDbMacalRuleExists(L7_char8 *name, L7_uint32 priority, L7_BOOL *bExists)
{
  return macalRuleExists(name, priority, bExists);
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
L7_RC_t usmDbMacalRuleActionGet(L7_char8* name, L7_uint32 priority, L7_uint32 *actionType) 
{
  return macalRuleActionGet(name, priority, actionType);
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
L7_RC_t usmDbMacalRuleSrcIpMaskGet(L7_char8* name, L7_uint32 priority, L7_uint32 *ipAddress, L7_uint32 *mask)
{
  return macalRuleSrcIpMaskGet(name, priority, ipAddress, mask);
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
L7_RC_t usmDbMacalRuleInterfaceGet(L7_char8* name, L7_uint32 priority, L7_uint32 *intIfNum)
{
  return macalRuleInterfaceGet(name, priority, intIfNum);
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
* @returns  L7_FAILURE, 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMacalRuleVlanGet(L7_char8* name, L7_uint32 priority, L7_uint32 *vlanId)
{
  return macalRuleVlanGet(name, priority, vlanId);
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
* @returns  L7_FAILURE, 
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbMacalRuleServiceGet(L7_char8* name, L7_uint32 priority, L7_uint32 *serviceType)
{
  return macalRuleServiceGet(name, priority, serviceType);
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
L7_RC_t usmDbMacalIsFieldConfigured(L7_char8* name, L7_uint32 priority, 
                                    macalRuleFields_t field, L7_BOOL *isConfigured)
{
  return macalIsFieldConfigured(name, priority, field, isConfigured);
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
L7_BOOL usmDbIsMacalComponentEnabled()
{
  return macalComponentEnabled();
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
L7_RC_t usmDbMacalIsAnyListCreated(L7_BOOL *isCreated)
{
  return macalIsAnyListCreated(isCreated);
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
L7_RC_t usmDbMacalIsAnyListActivated(L7_BOOL *isActivated)
{
  return macalIsAnyListActivated(isActivated);
}


/*** FOR TESTING PURPOSES ONLY****/
void createRules(L7_uint32 rule0, L7_uint32 rule1, L7_uint32 rule2, L7_uint32 rule3, L7_uint32 rule4, L7_uint32 rule5)
{
  L7_char8 *name = "List1";
  L7_RC_t rc;

  rc = usmDbMacalListCreate(name);
  printf("\nList List1 created with retval %u \n", rc);

  rc = usmDbMacalRuleEntryAdd(name, &rule0);
  printf("Rule %u added with retval %u \n", rule1, rc);
  rc = usmDbMacalRuleActionAdd(name, rule0, MACAL_ACTION_PERMIT);
  printf("Action PERMIT added to rule %u  with retval %u \n", rule0, rc);

  rc = usmDbMacalRuleEntryAdd(name, &rule1);
  printf("Rule %u added with retval %u \n", rule1, rc);
  rc = usmDbMacalRuleActionAdd(name, rule1, MACAL_ACTION_DENY);
  printf("Action DENY added to rule %u  with retval %u \n", rule1, rc);
  rc = usmDbMacalRuleSrcIpMaskAdd(name, rule1, 0xC0A84DDF, 0xffffff00); 
  printf("IP addr 0xC0A84DDF and mask 0xffffff00 added with retval %u \n", rc);

  rc = usmDbMacalRuleEntryAdd(name, &rule2);
  printf("Rule %u added with retval %u \n", rule2, rc);
  rc = usmDbMacalRuleActionAdd(name, rule2, MACAL_ACTION_PERMIT);
  printf("Action PERMIT added to rule %u  with retval %u \n", rule2, rc);
  rc = usmDbMacalRuleInterfaceAdd(name, rule2, 2);
  printf("Interface 2 added to rule %u  with retval %u \n", rule1, rc);

  rc = usmDbMacalRuleEntryAdd(name, &rule3);
  printf("Rule %u added with retval %u \n", rule3, rc);
  rc = usmDbMacalRuleActionAdd(name, rule3, MACAL_ACTION_DENY);
  printf("Action DENY added to rule %u  with retval %u \n", rule3, rc);
  rc = usmDbMacalRuleServiceAdd(name, rule3, MACAL_PROTOCOL_TELNET);
  printf("TELNET service added to rule %u  with retval %u \n", rule3, rc);

  rc = usmDbMacalRuleEntryAdd(name, &rule4);
  printf("Rule %u added with retval %u \n", rule4, rc);
  rc = usmDbMacalRuleActionAdd(name, rule4, MACAL_ACTION_PERMIT);
  printf("Action DENY added to rule %u  with retval %u \n", rule4, rc);
  rc = usmDbMacalRuleVlanAdd(name, rule4, 10);
  printf("VLAN ID 10 added to rule %u  with retval %u \n", rule4, rc);

  rc = usmDbMacalRuleEntryAdd(name, &rule5);
  printf("Rule %u added with retval %u \n", rule5, rc);
  rc = usmDbMacalRuleActionAdd(name, rule5, MACAL_ACTION_DENY);
  printf("Action DENY added to rule %u  with retval %u \n", rule5, rc);
  rc = usmDbMacalRuleInterfaceAdd(name, rule5, 87);
  printf("LAG interface 87 added to rule %u  with retval %u \n", rule5, rc);

} 

L7_RC_t deleteRule(L7_uint32 rule)
{
  L7_RC_t rc = usmDbMacalRuleRemove("List1", rule);
  printf("\nRemoved rule %u with retval %u \n", rule, rc);
  return rc;
} 

void showRule(L7_uint32 rule)
{
  L7_RC_t rc;
  L7_uint32 action, intf, addr, mask, service;

  rc = usmDbMacalRuleActionGet("List1", rule, &action);
  printf("\nGet Action for rule %u  with retval %u \n", rule, rc);
  rc = usmDbMacalRuleInterfaceGet("List1", rule, &intf);
  printf("Get INterface for rule %u  with retval %u \n", rule, rc);
  rc = usmDbMacalRuleSrcIpMaskGet("List1", rule, &addr, &mask);
  printf("Get AddrMask for rule %u  with retval %u \n", rule, rc);
  rc = usmDbMacalRuleServiceGet("List1", rule, &service);
  printf("Get Serive for rule %u  with retval %u \n", rule, rc);

  printf("\nRule Action: %u\n", action);
  printf("\nRule Interafce: %u\n", intf);
  /*printf("\nRule vlan id: %u\n", vid);*/
  printf("\nRule IP addr: %X\n", addr);
  printf("\nRule Mask: %X\n", mask);
  printf("\nRule service: %u\n", service);
/*  printf("Mask for fields: %u\n", macalCfgData->macalList.rule[rule].configMask);*/
}
