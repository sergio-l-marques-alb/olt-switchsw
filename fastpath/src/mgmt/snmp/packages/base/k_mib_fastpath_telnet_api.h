/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_telnet_private_api.h
*
* @purpose  Provide interface to Outbound Telnet Private MIB
*
* @component SNMP
*
* @comments
*
* @create 29/03/2004
*
* @author Anindya Sarkar
*
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include <usmdb_telnet_api.h>
#include "usmdb_common.h"
#include "usmdb_telnet_api.h"


/*********************************************************************
* @purpose  Get the Outbound Telnet Admin Mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} Value of Admin Mode
*
*
* @returns  L7_SUCCESS  Admin Mode is get
* @returns  L7_FAILURE  Failed to get Admin Mode
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t snmpAgentOutboundTelnetAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t   rc;
  L7_uint32 temp_val;


  rc = usmDbTelnetAdminModeGet(USMDB_UNIT_CURRENT, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE :
      *val = D_agentOutboundTelnetAdminMode_enable;
      break;

    case L7_DISABLE :
      *val = D_agentOutboundTelnetAdminMode_disable;
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
* @purpose  Set the Outbound Telnet Admin Mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} Value of Admin Mode to be set
*
*
* @returns  L7_SUCCESS  Admin Mode is set
* @returns  L7_FAILURE  Failed to set Admin Mode
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t snmpAgentOutboundTelnetAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;


  switch (val)
  {
  case D_agentOutboundTelnetAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentOutboundTelnetAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  rc = usmDbTelnetAdminModeSet(USMDB_UNIT_CURRENT, temp_val);

  return rc;
}



/*********************************************************************
* @purpose  Set the maximum number of Outbound Telnet sessions allowed 
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    intMaxSessions  @b{(input)} Max no. of sessions allowed to be set
*
*
* @returns  L7_SUCCESS  Max no. of sessions allowed is set
* @returns  L7_FAILURE  Failed to set the max no. of sessions allowed
*
* @comments If the number of max sessions allowed is 0, it calls 
*           function usmDbTelnetMaxSessionsReset().
*           If the number of max sessions allowed is within allowable-range it calls
*           function usmDbTelnetMaxSessionsSet().
*
* @end
*********************************************************************/

L7_RC_t snmpAgentOutboundTelnetMaxNoOfSessionsSet(L7_uint32 UnitIndex, L7_uint32 intMaxSessions)
{
  L7_RC_t rc;


  if (intMaxSessions > L7_TELNET_MAX_SESSIONS || intMaxSessions < L7_TELNET_MIN_SESSIONS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    rc = usmDbTelnetMaxSessionsSet(UnitIndex, intMaxSessions);
  }

  return rc;
}


/*********************************************************************
* @purpose  Set the Outbound Telnet login inactivity timeout
*
* @param    UnitIndex     @b{(input)} Unit for this operation
* @param    intTimeout    @b{(input)} Login inactivity timeout to be set
*
*
* @returns  L7_SUCCESS  Login inactivity timeout is set
* @returns  L7_FAILURE  Failed to set login inactivity timeout
*
* @comments If the login inactivity timeout is 0, it calls
*           function usmDbTelnetTimeoutReset().
*           If the login inactivity timeout is within allowable-range it calls
*           function usmDbTelnetTimeoutSet().
*
* @end
*********************************************************************/

L7_RC_t snmpAgentOutboundTelnetTimeoutSet(L7_uint32 UnitIndex, L7_uint32 intTimeout)
{
  L7_RC_t rc;


  if (intTimeout > L7_TELNET_MAX_SESSION_TIMEOUT || intTimeout < L7_TELNET_MIN_SESSION_TIMEOUT)
  {
    rc = L7_FAILURE;
  }
  else
  {
    rc = usmDbTelnetTimeoutSet(UnitIndex, intTimeout);
  }

  return rc;
}
