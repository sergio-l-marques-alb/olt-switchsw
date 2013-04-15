/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_dot1x_api.c
*
* Purpose: System-specific code for IEEE8021-PAE-MIB
*
* Created by: Colin Verne 04/17/2003
*
* Component: SNMP
*
*********************************************************************/ 

#include <k_private_base.h>
#include <k_mib_dot1x_api.h>
#include "usmdb_dot1x_api.h"
#include "dot1x_exports.h"

/* Begin Function Declarations: k_mib_dot1x_api.h */

/*********************************************************************
*
* @purpose  Get the dot1x administrative mode 
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val @b((output)) ptr to the current value of the administrative mode  
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
snmpDot1xPaeSystemAuthControlGet(L7_uint32 UnitIndex, L7_int32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbDot1xAdminModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_dot1xPaeSystemAuthControl_enabled;
      break;

    case L7_FALSE:
      *val = D_dot1xPaeSystemAuthControl_disabled;
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
* @purpose  Set the Dot1x Administrative Mode 
*          
* @param    unitIndex @b((input)) the unit for this operation
* @param    val @b((input)) the boolean value of the dot1x mode  
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpDot1xPaeSystemAuthControlSet(L7_uint32 UnitIndex, L7_int32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = L7_FALSE;

  switch (val)
  {
  case D_dot1xPaeSystemAuthControl_enabled:
    temp_val = L7_TRUE;
    break;

  case D_dot1xPaeSystemAuthControl_disabled:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1xAdminModeSet(UnitIndex, temp_val);
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpDot1xPaePortCapabilitiesGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_char8 *buf, L7_uint32 *buf_len)
{
  /* set to Auth Capable */
  buf[0] = (1 << D_dot1xPaePortCapabilities_dot1xPaePortAuthCapable);
  *buf_len = 1;

  return L7_SUCCESS;
}

L7_RC_t
snmpDot1xPaePortInitializeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_FAILURE;


  switch(val)
  {
  case D_dot1xPaePortInitialize_false:
    rc = L7_SUCCESS;
    break;

  case D_dot1xPaePortInitialize_true:
    rc = usmDbDot1xPortInitializeSet(UnitIndex, intIfNum, L7_TRUE);
    break;

  default:
    /* unknown nominator */
    break;
  }

  return rc;
}

L7_RC_t
snmpDot1xPaePortReauthenticateSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_FAILURE;


  switch(val)
  {
  case D_dot1xPaePortReauthenticate_false:
    rc = L7_SUCCESS;
    break;

  case D_dot1xPaePortReauthenticate_true:
    rc = usmDbDot1xPortReauthenticateSet(UnitIndex, intIfNum, L7_TRUE);
    break;

  default:
    /* unknown nominator */
    break;
  }

  return rc;
}

/**************************************************************************************************************/

L7_RC_t
snmpDot1xAuthPaeStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_APM_STATES_t temp_val;

  rc = usmDbDot1xPortAuthPaeStateGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_APM_INITIALIZE:
      *val = D_dot1xAuthPaeState_initialize;
      break;
        
    case L7_DOT1X_APM_DISCONNECTED:
      *val = D_dot1xAuthPaeState_disconnected;
      break;

    case L7_DOT1X_APM_CONNECTING:
      *val = D_dot1xAuthPaeState_connecting;
      break;

    case L7_DOT1X_APM_AUTHENTICATING:
      *val = D_dot1xAuthPaeState_authenticating;
      break;

    case L7_DOT1X_APM_AUTHENTICATED:
      *val = D_dot1xAuthPaeState_authenticated;
      break;

    case L7_DOT1X_APM_ABORTING:
      *val = D_dot1xAuthPaeState_aborting;
      break;

    case L7_DOT1X_APM_HELD:
      *val = D_dot1xAuthPaeState_held;
      break;

    case L7_DOT1X_APM_FORCE_AUTH:
      *val = D_dot1xAuthPaeState_forceAuth;
      break;

    case L7_DOT1X_APM_FORCE_UNAUTH:
      *val = D_dot1xAuthPaeState_forceUnauth;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthBackendAuthStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_BAM_STATES_t temp_val;

  rc = usmDbDot1xPortBackendAuthStateGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_BAM_REQUEST:
      *val = D_dot1xAuthBackendAuthState_request;
      break;

    case L7_DOT1X_BAM_RESPONSE:
      *val = D_dot1xAuthBackendAuthState_response;
      break;

    case L7_DOT1X_BAM_SUCCESS:
      *val = D_dot1xAuthBackendAuthState_success;
      break;

    case L7_DOT1X_BAM_FAIL:
      *val = D_dot1xAuthBackendAuthState_fail;
      break;

    case L7_DOT1X_BAM_TIMEOUT:
      *val = D_dot1xAuthBackendAuthState_timeout;
      break;

    case L7_DOT1X_BAM_IDLE:
      *val = D_dot1xAuthBackendAuthState_idle;
      break;

    case L7_DOT1X_BAM_INITIALIZE:
      *val = D_dot1xAuthBackendAuthState_initialize;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthAdminControlledDirectionsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_DIRECTION_t temp_val;

  rc = usmDbDot1xPortAdminControlledDirectionsGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_PORT_DIRECTION_BOTH:
      *val = D_dot1xAuthAdminControlledDirections_both;
      break;
  
    case L7_DOT1X_PORT_DIRECTION_IN:
      *val = D_dot1xAuthAdminControlledDirections_in;
      break;
    
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthAdminControlledDirectionsSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_DOT1X_PORT_DIRECTION_t temp_val = 0;

  switch (val)
  {
  case L7_DOT1X_PORT_DIRECTION_BOTH:
    temp_val = D_dot1xAuthAdminControlledDirections_both;
    break;

  case L7_DOT1X_PORT_DIRECTION_IN:
    temp_val = D_dot1xAuthAdminControlledDirections_in;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1xPortAdminControlledDirectionsSet(UnitIndex, intIfNum, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthOperControlledDirectionsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_DIRECTION_t temp_val;

  rc = usmDbDot1xPortOperControlledDirectionsGet(UnitIndex, intIfNum, &temp_val);


  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_PORT_DIRECTION_BOTH:
      *val = D_dot1xAuthOperControlledDirections_both;
      break;
  
    case L7_DOT1X_PORT_DIRECTION_IN:
      *val = D_dot1xAuthOperControlledDirections_in;
      break;
    
    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthAuthControlledPortStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_STATUS_t temp_val;

  rc = usmDbDot1xPortAuthControlledPortStatusGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_PORT_STATUS_AUTHORIZED:
      *val = D_dot1xAuthAuthControlledPortStatus_authorized;
      break;

    case L7_DOT1X_PORT_STATUS_UNAUTHORIZED:
      *val = D_dot1xAuthAuthControlledPortStatus_unauthorized;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthAuthControlledPortControlGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_DOT1X_PORT_CONTROL_t temp_val;

  rc = usmDbDot1xPortControlModeGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DOT1X_PORT_AUTO:
      *val = D_dot1xAuthAuthControlledPortControl_auto;
      break;

    case L7_DOT1X_PORT_FORCE_AUTHORIZED:
      *val = D_dot1xAuthAuthControlledPortControl_forceAuthorized;
      break;

    case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
      *val = D_dot1xAuthAuthControlledPortControl_forceUnauthorized;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthAuthControlledPortControlSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_DOT1X_PORT_CONTROL_t temp_val = 0;

  switch (val)
  {
  case D_dot1xAuthAuthControlledPortControl_auto:
    temp_val = L7_DOT1X_PORT_AUTO;
    break;

  case D_dot1xAuthAuthControlledPortControl_forceAuthorized:
    temp_val = L7_DOT1X_PORT_FORCE_AUTHORIZED;
    break;

  case D_dot1xAuthAuthControlledPortControl_forceUnauthorized:
    temp_val = L7_DOT1X_PORT_FORCE_UNAUTHORIZED;
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

L7_RC_t
snmpDot1xAuthReAuthEnabledGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbDot1xPortReAuthEnabledGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_dot1xAuthReAuthEnabled_true;
      break;

    case L7_FALSE:
      *val = D_dot1xAuthReAuthEnabled_false;
      break;

    default:
      /* unknown value */
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthReAuthEnabledSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = L7_FALSE;

  switch (val)
  {
  case D_dot1xAuthReAuthEnabled_true:
    temp_val = L7_TRUE;
    break;

  case D_dot1xAuthReAuthEnabled_false:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1xPortReAuthEnabledSet(UnitIndex,intIfNum, temp_val);
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthKeyTxEnabledGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val)
{
  L7_RC_t rc;
  L7_BOOL temp_val;

  rc = usmDbDot1xPortKeyTransmissionEnabledGet(UnitIndex, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE:
      *val = D_dot1xAuthKeyTxEnabled_true;
      break;

    case L7_FALSE:
      *val = D_dot1xAuthKeyTxEnabled_false;
      break;

    default:
      /* unknown value */
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDot1xAuthKeyTxEnabledSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = L7_FALSE;

  switch (val)
  {
  case D_dot1xAuthKeyTxEnabled_true:
    temp_val = L7_TRUE;
    break;

  case D_dot1xAuthKeyTxEnabled_false:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbDot1xPortKeyTransmissionEnabledSet(UnitIndex,intIfNum, temp_val);
  }

  return rc;
}

/* End Function Declarations */
