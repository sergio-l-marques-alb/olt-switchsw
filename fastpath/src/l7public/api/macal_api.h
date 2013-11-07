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
* @filename macal_api.h
*
* @purpose Management Access Control and Administration List (MACAL) APIs
*
* @component Management Access Control and Administration List (MACAL)
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

#ifndef MACAL_API_H
#define MACAL_API_H

#define MACAL_CONSOLE_ONLY "console-only"
#define MACAL_REQUEST_PRIORITY 0xFFFF
#include "macal_exports.h"

typedef enum
{
  MACAL_PROTOCOL_NONE,
  MACAL_PROTOCOL_TELNET,
  MACAL_PROTOCOL_HTTP,
  MACAL_PROTOCOL_HTTPS,
  MACAL_PROTOCOL_SNMP,
  MACAL_PROTOCOL_SSH,
  MACAL_PROTOCOL_TFTP,
  MACAL_PROTOCOL_SNTP
} macalServiceType_t;

typedef enum
{
  MACAL_ACTION_DENY,
  MACAL_ACTION_PERMIT
} macalActionType_t;

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
L7_RC_t macalListCreate(L7_char8 *name);

/*********************************************************************
*
* @purpose  Delete an existing ACAL.
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
L7_RC_t macalListDelete(L7_char8 *name);

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
L7_RC_t macalActivate(L7_char8 *name);

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
L7_RC_t macalDeactivate(L7_char8 *name);

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
L7_RC_t macalRuleAdd(L7_char8* name, L7_uint32 *priority);

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
L7_RC_t macalRuleRemove(L7_char8 *name, L7_uint32 priority);

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
L7_RC_t macalRuleActionAdd(L7_char8 *name, L7_uint32 priority, macalActionType_t actionType);

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
L7_RC_t macalRuleSrcIpMaskAdd(L7_char8 *name, L7_uint32 priority, L7_uint32 ipAddress, L7_uint32 mask);

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
L7_RC_t macalRuleInterfaceAdd(L7_char8 *name, L7_uint32  priority, L7_uint32 intIfNum);

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
L7_RC_t macalRuleVlanAdd(L7_char8 *name, L7_uint32  priority, L7_uint32 vlanId);

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
L7_RC_t macalRuleServiceAdd(L7_char8 *name, L7_uint32 priority, L7_uint32 serviceType);

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
L7_RC_t macalNameGet(L7_char8 *name);

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
L7_RC_t macalActiveListGet(L7_char8 *name);

/*********************************************************************
*
* @purpose  Get the first ACAL rule
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(output)} rule priority used as the rule index
*
* @returns  L7_SUCCESS, if the first rule for this ACAL was found
* @returns  L7_FAILURE, if no rule exists
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleFirstGet(L7_char8 *name, L7_uint32 *priority);

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
L7_RC_t macalRuleNextGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *nextPriority);

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
L7_RC_t macalRuleExists(L7_char8* name, L7_uint32 priority, L7_BOOL *bExists);

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
L7_RC_t macalRuleActionGet(L7_char8 *name, L7_uint32 priority, macalActionType_t *actionType);

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
L7_RC_t macalRuleSrcIpMaskGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *ipAddress, L7_uint32 *mask);

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
L7_RC_t macalRuleInterfaceGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose  Get the VLAN ID for an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    vlanId   the VLAN ID
*
* @returns  L7_SUCCESS, if the VLAN ID is retrieved
* @returns  L7_FAILURE, if failed retrieving
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleVlanGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *vlanId);

/*********************************************************************
*
* @purpose  Get the service type for an ACAL rule.
*
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
* @param    serviceType @b{(input)} the service type
*
* @returns  L7_SUCCESS, if service type is retrieved
* @returns  L7_FAILURE, if failed retrieving
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t macalRuleServiceGet(L7_char8 *name, L7_uint32 priority, L7_uint32 *serviceType);

/*********************************************************************
*
* @purpose  Validate the ACAL existence, priority value, and the rule existence.
*
* @param    name     @b{(input)} the function name that called this function.
* @param    name     @b{(input)} the ACAL name.
* @param    priority @b{(input)} rule priority used as the rule index
*
* @returns  L7_SUCCESS, if validation successful
* @returns  L7_FAILURE, if not
*
* @comments None
*
* @end
*
*********************************************************************/
L7_RC_t macalValidateListPriorityRule(const L7_char8 *fnName, L7_char8 *listName, L7_uint32 priority);

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
                               macalRuleFields_t field, L7_BOOL *isConfigured);

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
L7_BOOL macalComponentEnabled();

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
L7_RC_t macalIsAnyListCreated(L7_BOOL *isCreated);

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
L7_RC_t macalIsAnyListActivated(L7_BOOL *isActivated);
#endif /* MACAL_API_H */
