/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_dot1xAdvanced_api.h
*
* Purpose: System-specific code for IEEE8021-PAE-MIB
*
* Created by: Colin Verne 04/17/2003
*
* Component: SNMP
*
*********************************************************************/ 

#ifndef K_MIB_DOT1X_ADVANCED_API_H
#define K_MIB_DOT1X_ADVANCED_API_H

#include <k_private_base.h>

/* Begin Function Prototypes */
/*********************************************************************
*
* @purpose  Get the dot1x port control mode 
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the port control mode  
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortControlModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Set the dot1x port control mode 
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the port control mode  
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortControlModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

/*********************************************************************
*
* @purpose  Get the dot1x vlan assigned for the port  
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the vlan assigned 
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortVlanAssignedGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x vlan assigned reason for the port  
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the vlan assigned reason
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortVlanAssignedReasonGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x vlan assigned for the port  
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the vlan assigned 
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortSessionTimeoutGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x session termination action for the port    
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the termination action
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortTerminationActionGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x MAB mode  for the port    
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the MAB mode
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortMABenabledGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Set the dot1x MAB mode 
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the MAB mode  
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortMABenabledSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

/*********************************************************************
*
* @purpose  Get the operational dot1x MAB mode  for the port    
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the MAB mode
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xPortMABenabledOperationalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x authentication PAE state for the client  
*          
* @param    lIntIfNum @b((input)) the logical interface for this client
* @param    val @b((output)) ptr to the current value of authenticator PAE state
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientAuthPAEstateGet(L7_uint32 lIntIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x Backend machine state for the client  
*          
* @param    lIntIfNum @b((input)) the logical interface for this client
* @param    val @b((output)) ptr to the current value of authenticator PAE state
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientBackendStateGet(L7_uint32 lIntIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x user name for the client  
*          
* @param    lIntIfNum @b((input)) the logical interface for this client
* @param    val @b((output)) ptr to the current value of user name
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments returns success only if client is in authenticated state
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientUserNameGet(L7_uint32 unitIndex,L7_uint32 lIntIfNum,
                                L7_uchar8 *userName);

/*********************************************************************
*
* @purpose  Get the dot1x session time for the client  
*          
* @param    lIntIfNum @b((input)) the logical interface for this client
* @param    val @b((output)) ptr to the current value of session time
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments returns success only if client is in authenticated state
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientSessionTimeGet(L7_uint32 unitIndex,L7_uint32 lIntIfNum,
                                     L7_uint32 *value);

/*********************************************************************
*
* @purpose  Get the dot1x filter Id for the client  
*          
* @param    lIntIfNum @b((input)) the logical interface for this client
* @param    val @b((output)) ptr to the current value of filter Id
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments returns success only if client is in authenticated state
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientFilterIDGet(L7_uint32 unitIndex,L7_uint32 lIntIfNum,
                                L7_uchar8 *filter);

/*********************************************************************
*
* @purpose  Get the dot1x assigned vlan for the client  
*          
* @param    lIntIfNum @b((input)) the logical interface for this client
* @param    val @b((output)) ptr to the current value of assigned vlan
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments returns success only if client is in authenticated state
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientVlanAssignedGet(L7_uint32 lIntIfNum,
                                     L7_uint32 *value);


/*********************************************************************
*
* @purpose  Get the dot1x vlan assigned reason for the client  
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the vlan assigned reason
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientVlanAssignedReasonGet(L7_uint32 lIntIfNum, L7_int32 *val);

/*********************************************************************
*
* @purpose  Get the dot1x session timeout for the client  
*          
* @param    lIntIfNum @b((input)) the logical interface for this client
* @param    val @b((output)) ptr to the current value of session timeout
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments returns success only if client is in authenticated state
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientSessionTimeoutGet(L7_uint32 unitIndex,L7_uint32 lIntIfNum,
                                               L7_uint32 *session_timeout);

/*********************************************************************
*
* @purpose  Get the dot1x session termination action for the client    
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the termination action
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientTerminationActionGet(L7_uint32 UnitIndex, L7_uint32 lIntIfNum, L7_int32 *val);

/*********************************************************************
* @purpose  Get the dot1x vlan assignment mode 
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val       @b((output)) ptr to current vlan assignment mode
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpagentDot1xVlanAssignmentModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the dot1x dynamic vlan creation mode.
*
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val       @b((output)) ptr to current vlan assignment mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpagentDot1xDynamicVlanCreationModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/* End Function Prototypes */

#endif /* K_MIB_DOT1X_API_H */
