/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_dot1xAdvanced_api.c
*
* Purpose: System-specific code for IEEE8021-PAE-MIB
*
* Created by: Subhashini Koundinya 11/20/2007
*
* Component: SNMP
*
*********************************************************************/ 

#include <k_private_base.h>
#include <k_mib_dot1xAdvanced_api.h>
#include "usmdb_common.h"
#include "dot1x_exports.h"
#include "usmdb_dot1x_api.h"

/* Begin Function Declarations: k_mib_dot1xAdvanced_api.h */

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
snmpagentDot1xPortControlModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t temp_val;

  rc = usmDbDot1xPortControlModeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
      *val = D_agentDot1xPortControlMode_forceUnauthorized;
      break;

    case L7_DOT1X_PORT_AUTO:
      *val = D_agentDot1xPortControlMode_auto;
      break;

    case L7_DOT1X_PORT_FORCE_AUTHORIZED:
        *val = D_agentDot1xPortControlMode_forceAuthorized;
        break;

    case L7_DOT1X_PORT_AUTO_MAC_BASED:
        *val = D_agentDot1xPortControlMode_macBased;
        break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

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
snmpagentDot1xPortControlModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_DOT1X_PORT_CONTROL_t temp_val = 0;

  switch (val)
  {
  case D_agentDot1xPortControlMode_auto:
    temp_val = L7_DOT1X_PORT_AUTO;
    break;

  case D_agentDot1xPortControlMode_forceAuthorized:
    temp_val = L7_DOT1X_PORT_FORCE_AUTHORIZED;
    break;

  case D_agentDot1xPortControlMode_forceUnauthorized:
    temp_val = L7_DOT1X_PORT_FORCE_UNAUTHORIZED;
    break;

  case D_agentDot1xPortControlMode_macBased:
      if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)==L7_TRUE) 
      {
          temp_val = L7_DOT1X_PORT_AUTO_MAC_BASED;
      }
      else
          rc = L7_FAILURE;

    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1xPortControlModeSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}

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
snmpagentDot1xPortVlanAssignedGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t temp_val;
  L7_uint32 temp_val2;

  
  if ((usmDbDot1xPortControlModeGet(UnitIndex,intIfNum,&temp_val)== L7_SUCCESS )&&
      (temp_val != L7_DOT1X_PORT_AUTO)) 
  {
      *val = 0;
      return L7_SUCCESS;
  }

  rc = usmDbDot1xPortVlanAssignedGet(UnitIndex, intIfNum, &temp_val2);

  if (rc == L7_SUCCESS)
  {
    *val = temp_val2;
  }

  return rc;
}

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
snmpagentDot1xPortVlanAssignedReasonGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t port_control_val;
  L7_DOT1X_VLAN_ASSIGNED_MODE_t temp_val;

  if ((usmDbDot1xPortControlModeGet(UnitIndex,intIfNum,&port_control_val)==L7_SUCCESS) &&
      (port_control_val != L7_DOT1X_PORT_AUTO)) 
  {
      *val = D_agentDot1xPortVlanAssignedReason_notAssigned;
      return L7_SUCCESS;
  }

  rc = usmDbDot1xPortVlanAssignedReasonGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_DEFAULT_ASSIGNED_VLAN:
      *val = D_agentDot1xPortVlanAssignedReason_default;
      break;

    case L7_DOT1X_RADIUS_ASSIGNED_VLAN:
      *val = D_agentDot1xPortVlanAssignedReason_radius;
      break;

    case L7_DOT1X_UNAUTHENTICATED_VLAN:
        *val = D_agentDot1xPortVlanAssignedReason_unauthenticatedVlan;
        break;

    case L7_DOT1X_GUEST_VLAN:
        *val = D_agentDot1xPortVlanAssignedReason_guestVlan;
        break;

    case L7_DOT1X_NOT_ASSIGNED:
        *val = D_agentDot1xPortVlanAssignedReason_notAssigned;
    break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

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
snmpagentDot1xPortSessionTimeoutGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t temp_val;
  L7_uint32 temp_val2;
  
  if ((usmDbDot1xPortControlModeGet(UnitIndex,intIfNum,&temp_val)== L7_SUCCESS) &&
      (temp_val != L7_DOT1X_PORT_AUTO)) 
  {
      *val = 0;
      return L7_SUCCESS;
  }

  rc = usmDbDot1xPortSessionTimeoutGet(UnitIndex, intIfNum, &temp_val2);

  if (rc == L7_SUCCESS)
  {
    *val = temp_val2;
  }

  return rc;
}

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
snmpagentDot1xPortTerminationActionGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t port_control_val;
  L7_DOT1X_TERMINATION_ACTION_t temp_val;

  if ((usmDbDot1xPortControlModeGet(UnitIndex,intIfNum,&port_control_val)==L7_SUCCESS) &&
      (port_control_val != L7_DOT1X_PORT_AUTO)) 
  {
      *val = D_agentDot1xPortTerminationAction_default;
      return L7_SUCCESS;
  }

  rc = usmDbDot1xPortTerminationActionGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_TERMINATION_ACTION_DEFAULT:
      *val = D_agentDot1xPortTerminationAction_default;
      break;

    case L7_DOT1X_TERMINATION_ACTION_RADIUS:
      *val = D_agentDot1xPortTerminationAction_reauthenticate;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}


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
snmpagentDot1xPortMABenabledGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t port_control_val;
  L7_uint32 temp_val;

  if ((usmDbDot1xPortControlModeGet(UnitIndex,intIfNum,&port_control_val)==L7_SUCCESS) &&
      (port_control_val != L7_DOT1X_PORT_AUTO_MAC_BASED)) 
  {
      *val = D_agentDot1xPortMABenabled_disable;
      return L7_SUCCESS;
  }

  rc = usmDbDot1xPortMABEnabledGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DISABLE:
      *val = D_agentDot1xPortMABenabled_disable;
      break;

    case L7_ENABLE:
      *val = D_agentDot1xPortMABenabled_enable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

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
snmpagentDot1xPortMABenabledSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentDot1xPortMABenabled_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentDot1xPortMABenabled_disable:
    temp_val = L7_DISABLE;
    break;

   default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1xPortMABEnabledSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}

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
snmpagentDot1xPortMABenabledOperationalGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t port_control_val;
  L7_uint32 temp_val;

  if ((usmDbDot1xPortControlModeGet(UnitIndex,intIfNum,&port_control_val)==L7_SUCCESS) &&
      (port_control_val != L7_DOT1X_PORT_AUTO_MAC_BASED)) 
  {
      *val = D_agentDot1xPortMABenabled_disable;
      return L7_SUCCESS;
  }

  rc = usmDbDot1xPortOperMABEnabledGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DISABLE:
      *val = D_agentDot1xPortMABenabled_disable;
      break;

    case L7_ENABLE:
      *val = D_agentDot1xPortMABenabled_enable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}
/*********************************************************************
*
* @purpose  Get the logical port number for specified client MAC address     
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    macAddress @b((input)) the client MAC address
* @param    lIntIfNum  @b((output)) the logical port number of that client
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments shows clients that are in the authenticated mode only.
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientMacAddressGet(L7_uint32 unitIndex,L7_uchar8 *macAddr,
                                      L7_uint32 *lIntIfNum )
{
    L7_uint32 lportno;
    L7_DOT1X_APM_STATES_t paestate;

    if (usmDbDot1xClientMacAddressGet(unitIndex,macAddr,&lportno)==L7_SUCCESS) 
    {
        if ((usmDbDot1xLogicalPortPaeStateGet(lportno,&paestate)== L7_SUCCESS)&&
            (paestate == L7_DOT1X_APM_AUTHENTICATED))
        {
            *lIntIfNum = lportno;
            return L7_SUCCESS;
        }
        else
            return L7_FAILURE;
    }
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the next client MAC address     
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    macAddress @b((input/output)) the client MAC address
* @param    lIntIfNum  @b((output)) the logical port number of that client
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments shows clients that are in the authenticated mode only.
*
* @notes 
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpagentDot1xClientMacAddressGetNext(L7_uint32 unitIndex,L7_uchar8 *macAddr,
                                      L7_uint32 *lIntIfNum )
{
    L7_uint32 lportno;
    L7_DOT1X_APM_STATES_t paestate;
    L7_RC_t rc;

    rc = usmDbDot1xClientMacAddressNextGet(unitIndex,macAddr,&lportno);
    while (rc==L7_SUCCESS) 
    {
        if ((usmDbDot1xLogicalPortPaeStateGet(lportno,&paestate)== L7_SUCCESS)&&
            (paestate == L7_DOT1X_APM_AUTHENTICATED))
        {
            *lIntIfNum = lportno;
            return L7_SUCCESS;
        }
        else
            lportno =0;
       rc = usmDbDot1xClientMacAddressNextGet(unitIndex,macAddr,&lportno);
    }
    return rc;
}

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
snmpagentDot1xClientAuthPAEstateGet(L7_uint32 lIntIfNum, L7_int32 *val)
{
    L7_RC_t rc;
    L7_DOT1X_APM_STATES_t temp_val;

    rc = usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&temp_val);

    if (rc == L7_SUCCESS)
    {
        switch (temp_val)
        {
        case L7_DOT1X_APM_INITIALIZE:
            *val = D_agentDot1xClientAuthPAEstate_initialize;
         break;

        case L7_DOT1X_APM_DISCONNECTED:
            *val = D_agentDot1xClientAuthPAEstate_disconnected;
        break;

        case L7_DOT1X_APM_CONNECTING:
            *val = D_agentDot1xClientAuthPAEstate_connecting;
        break;

        case L7_DOT1X_APM_AUTHENTICATING:
            *val = D_agentDot1xClientAuthPAEstate_authenticating;
        break;

        case L7_DOT1X_APM_AUTHENTICATED:
            *val = D_agentDot1xClientAuthPAEstate_authenticated;
        break;

        case L7_DOT1X_APM_ABORTING:
            *val = D_agentDot1xClientAuthPAEstate_aborting;
        break;

        case L7_DOT1X_APM_HELD:
            *val = D_agentDot1xClientAuthPAEstate_held;
        break;

        case L7_DOT1X_APM_FORCE_AUTH:
            *val = D_agentDot1xClientAuthPAEstate_forceAuth;
        break;

        case L7_DOT1X_APM_FORCE_UNAUTH:
            *val = D_agentDot1xClientAuthPAEstate_forceUnauth;
        break;

        default:
            /* unknown value */
            rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

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
snmpagentDot1xClientBackendStateGet(L7_uint32 lIntIfNum, L7_int32 *val)
{
    L7_RC_t rc;
   L7_DOT1X_BAM_STATES_t temp_val;

    rc = usmDbDot1xLogicalPortBackendAuthStateGet(lIntIfNum,&temp_val);

    if (rc == L7_SUCCESS)
    {
        switch (temp_val)
        {
        case L7_DOT1X_BAM_REQUEST:
            *val = D_agentDot1xClientBackendState_request;
         break;

        case L7_DOT1X_BAM_RESPONSE:
            *val = D_agentDot1xClientBackendState_response;
        break;

        case L7_DOT1X_BAM_SUCCESS:
            *val = D_agentDot1xClientBackendState_success;
        break;

        case L7_DOT1X_BAM_FAIL:
            *val = D_agentDot1xClientBackendState_fail;
        break;

        case  L7_DOT1X_BAM_TIMEOUT:
            *val = D_agentDot1xClientBackendState_timeout;
        break;

        case L7_DOT1X_BAM_IDLE:
            *val = D_agentDot1xClientBackendState_idle;
        break;

        case L7_DOT1X_BAM_INITIALIZE:
            *val = D_agentDot1xClientBackendState_initialize;
        break;

        default:
            /* unknown value */
            rc = L7_FAILURE;
        break;
    }
  }

  return rc;
}

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
                                L7_uchar8 *userName)
{
    L7_DOT1X_APM_STATES_t paestate;
    L7_RC_t rc;

    if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
        return L7_SUCCESS;
    }


    rc = usmdbDot1xLogicalPortUserNameGet(USMDB_UNIT_CURRENT,lIntIfNum,userName);
    return rc;

}

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
                                     L7_uint32 *value)
{
    L7_DOT1X_APM_STATES_t paestate;
    L7_RC_t rc;

    if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
        *value = 0;
        return L7_SUCCESS;
    }

    rc = usmdbDot1xPortSessionTimeGet(USMDB_UNIT_CURRENT,lIntIfNum,value);
    return rc;

}

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
                                L7_uchar8 *filter)
{
    L7_DOT1X_APM_STATES_t paestate;
    L7_DOT1X_PORT_CONTROL_t port_control_val;
    L7_uint32 intIfNum;
    L7_RC_t rc;

    if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&paestate)== L7_SUCCESS)&&
     (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
        return L7_SUCCESS;
    }

    /* Filter ID assignment only supported for Mac based*/
    if ((usmDbDot1xPhysicalPortGet(unitIndex,lIntIfNum,&intIfNum)==L7_SUCCESS) &&
        (usmDbDot1xPortControlModeGet(unitIndex,intIfNum,&port_control_val)==L7_SUCCESS) &&
        (port_control_val != L7_DOT1X_PORT_AUTO_MAC_BASED)) 
    {
       return L7_SUCCESS;
    }

    rc = usmdbDot1xLogicalPortFilterIdGet(USMDB_UNIT_CURRENT,lIntIfNum,filter);
    return rc;

}

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
                                     L7_uint32 *value)
{
    L7_DOT1X_APM_STATES_t paestate;
    L7_RC_t rc;
    L7_uint32 mode;

    if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
        *value = 0;
        return L7_SUCCESS;
    }

    rc = usmDbDot1xLogicalPortVlanAssignmentGet(lIntIfNum,value,&mode);
    return rc;

}
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
snmpagentDot1xClientVlanAssignedReasonGet(L7_uint32 lIntIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_uint32 vlanId;
  L7_DOT1X_VLAN_ASSIGNED_MODE_t temp_val;
  L7_DOT1X_APM_STATES_t paestate;

  if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&paestate)== L7_SUCCESS)&&
       (paestate != L7_DOT1X_APM_AUTHENTICATED))
  {
      *val = D_agentDot1xClientVlanAssignedReason_invalid;
      return L7_SUCCESS;
  }

  
  rc = usmDbDot1xLogicalPortVlanAssignmentGet(lIntIfNum, &vlanId,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_DEFAULT_ASSIGNED_VLAN:
      *val = D_agentDot1xClientVlanAssignedReason_default;
      break;

    case L7_DOT1X_RADIUS_ASSIGNED_VLAN:
      *val = D_agentDot1xClientVlanAssignedReason_radius;
      break;

    case L7_DOT1X_UNAUTHENTICATED_VLAN:
        *val = D_agentDot1xClientVlanAssignedReason_unauthenticatedVlan;
        break;

    case L7_DOT1X_MONITOR_MODE_VLAN:
        *val = D_agentDot1xClientVlanAssignedReason_monitorVlan;
        break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

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
                                               L7_uint32 *session_timeout)
{
    L7_DOT1X_APM_STATES_t paestate;
    L7_RC_t rc;

    if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
        *session_timeout = 0;
        return L7_SUCCESS;
    }

    rc = usmDbDot1xLogicalPortSessionTimeoutGet(unitIndex,lIntIfNum,session_timeout);
    return rc;

}
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
snmpagentDot1xClientTerminationActionGet(L7_uint32 UnitIndex, L7_uint32 lIntIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_TERMINATION_ACTION_t temp_val;
  L7_DOT1X_APM_STATES_t paestate;

  if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&paestate)== L7_SUCCESS)&&
       (paestate != L7_DOT1X_APM_AUTHENTICATED))
  {
      *val = D_agentDot1xClientTerminationAction_default;
      return L7_SUCCESS;
  }
  
  rc = usmDbDot1xLogicalPortTerminationActionGet(UnitIndex, lIntIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_TERMINATION_ACTION_DEFAULT:
      *val = D_agentDot1xClientTerminationAction_default;
      break;

    case L7_DOT1X_TERMINATION_ACTION_RADIUS:
      *val = D_agentDot1xClientTerminationAction_reauthenticate;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

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
snmpagentDot1xVlanAssignmentModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbDot1xVlanAssignmentModeGet(UnitIndex, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentDot1xRadiusVlanAssignment_enable;
      break;

    case L7_DISABLE:
      *val = D_agentDot1xRadiusVlanAssignment_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

#ifdef I_agentDot1xDynamicVlanCreationMode
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
snmpagentDot1xDynamicVlanCreationModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbDot1xDynamicVlanCreationModeGet(UnitIndex, &temp_val);
  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentDot1xDynamicVlanCreationMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentDot1xDynamicVlanCreationMode_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}
#endif

/* End Function Declarations */
