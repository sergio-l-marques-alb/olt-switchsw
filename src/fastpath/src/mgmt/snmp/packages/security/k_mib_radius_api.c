/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_radius_api.c
*
* Purpose: APIs used for Radius Private MIB
*
* Created by: Prashant Murthy 05/07/2003
*
* Component: SNMP
*
*********************************************************************/ 

#include <k_private_base.h>
#include <k_mib_radius_api.h>
#include "usmdb_radius_api.h"
#include "radius_exports.h"

/* Begin Function Declarations: k_mib_radius_api.h */

/*********************************************************************
*
* @purpose  Clears the statistics for Radius
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val  @b((input)) object value
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
L7_RC_t snmpAgentRadiusStatsClearSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;

  switch (val)
  {
  case D_agentRadiusStatsClear_enable:
    rc = usmDbRadiusStatsClear(UnitIndex);
    break;

  case D_agentRadiusStatsClear_disable:
    rc = L7_SUCCESS;
    break;
  
  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Gets the Radius Accounting Mode.
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val  @b((output)) object value
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
L7_RC_t snmpAgentRadiusAccountingModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbRadiusAccountingModeGet(UnitIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentRadiusAccountingMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentRadiusAccountingMode_disable;
      break;

    default:
      /* unknown value */
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Sets the Radius Accounting Mode.
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val  @b((input)) object value
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
L7_RC_t snmpAgentRadiusAccountingModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentRadiusAccountingMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentRadiusAccountingMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRadiusAccountingModeSet(UnitIndex, temp_val);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the next valid index for the table, or 0 if no more.
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val  @b((input)) object value
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
L7_RC_t snmpAgentRadiusAccountingIndexNextValidGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 tempVal=0;

  rc = usmDbRadiusConfiguredServersCountGet(UnitIndex,RADIUS_SERVER_TYPE_ACCT,&tempVal);
  if (L7_SUCCESS == rc) 
  {
      /* set next valid index accordingly */
      *val = tempVal+1;

      if (*val > FD_RADIUS_MAX_ACCT_SERVERS)
      {
        *val = 0;
      }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Gets the next valid index for the table, or 0 if no more.
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    val  @b((input)) object value
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
L7_RC_t snmpAgentRadiusServerIndexNextValidGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbRadiusServerCountGet(UnitIndex, &temp_val);
       
  if (rc == L7_SUCCESS) 
  {
      /* set next valid index accordingly */
      *val = temp_val+1;

      if (*val > FD_RADIUS_MAX_AUTH_SERVERS)
      {
        *val = 0;
      }
  }

  return rc;

}

/*****************************************************************************************/

/*********************************************************************
*
* @purpose  Check that the index exists
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverIndex  @b((output)) Radius Accounting server index
* @param    serverAddress  @b((output)) server IP Address
* @param    addrType  @b((output)) server IP Address type DNS or IPV4
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

L7_RC_t snmpAgentRadiusAccountingConfigEntryGet(L7_uint32 UnitIndex, L7_uint32 serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrType )
{
  L7_RC_t rc;

  rc = usmDbRadiusAccountingServerIPHostNameByIndexGet(UnitIndex, serverIndex, serverAddress, addrType);

  return rc;
}


/*********************************************************************
*
* @purpose  Find the next valid index
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverIndex  @b((output)) Radius Accounting server index
* @param    serverAddress  @b((output)) server IP Address
* @param    addrType  @b((output)) server IP Address type DNS or IPV4
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

L7_RC_t snmpAgentRadiusAccountingConfigEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrType )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 maxIndex = 1;

  /* look for the next index */
  *serverIndex += 1;

  while (rc == L7_FAILURE &&
         *serverIndex <= maxIndex)
  {
    if ((rc = usmDbRadiusAccountingServerIPHostNameByIndexGet(UnitIndex, *serverIndex, serverAddress, addrType)) == L7_SUCCESS)
    {
      break;
    }
    /* increment the index */
    *serverIndex += 1;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Check that the index exists
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverIndex  @b((output)) Radius server index
* @param    serverAddress  @b((output)) server IP Address
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

L7_RC_t snmpAgentRadiusServerConfigEntryGet(L7_uint32 UnitIndex, 
     L7_uint32 serverIndex, L7_uchar8 *serverAddress, 
     L7_IP_ADDRESS_TYPE_t *addrType)
{
  L7_RC_t rc;

  rc = usmDbRadiusServerIPHostNameByIndexGet(UnitIndex, serverIndex, serverAddress, addrType);

  return rc;
}


/*********************************************************************
*
* @purpose  Find the next valid index
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverIndex  @b((output)) Radius server index
* @param    serverAddress  @b((output)) server IP Address
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
L7_RC_t snmpAgentRadiusServerConfigEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 maxIndex = 0;

  /* look for the next index */
  *serverIndex += 1;

  if (usmDbRadiusServerCountGet(UnitIndex, &maxIndex) != L7_SUCCESS)
    return rc;

  while (rc == L7_FAILURE &&
         *serverIndex <= maxIndex)
  {
    if ((rc = usmDbRadiusServerIPHostNameByIndexGet(UnitIndex, *serverIndex, serverAddress, addrType)) == L7_SUCCESS)
    {
        break;
    }
    /* increment the index */
    *serverIndex += 1;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the primary status of the entry
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverAddress  @b((input)) server IP Address
* @param    val  @b((output)) value of the object
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
snmpAgentRadiusServerPrimaryModeGet(L7_uint32 UnitIndex, L7_uchar8* serverAddress, L7_IP_ADDRESS_TYPE_t type,L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbRadiusHostNameServerEntryTypeGet(UnitIndex, serverAddress,type, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (temp_val == L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY)
    {
      *val = D_agentRadiusServerPrimaryMode_enable;
    }
    else
    {
      *val = D_agentRadiusServerPrimaryMode_disable;
    }
  }

  return rc;
}


/*********************************************************************
*
* @purpose  Set the primary status of the entry
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverAddress  @b((input)) server IP Address
* @param    val  @b((input)) value of the object
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
L7_RC_t snmpAgentRadiusServerPrimaryModeSet(L7_uint32 UnitIndex, 
                       L7_uchar8 *serverAddress,
                       L7_IP_ADDRESS_TYPE_t type, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentRadiusServerPrimaryMode_enable:
    temp_val = L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY;
    break;

  case D_agentRadiusServerPrimaryMode_disable:
    temp_val = L7_RADIUS_SERVER_ENTRY_TYPE_SECONDARY;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRadiusHostNameServerEntryTypeSet(UnitIndex, serverAddress,type, temp_val);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the current mode status of the entry
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverAddress  @b((input)) server IP Address
* @param    val  @b((output)) value of the object
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @comments
*
* @notes This function determines if the specified server is the
*        current server in use by the RADIUS client.
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t
snmpAgentRadiusServerCurrentModeGet(L7_uint32 UnitIndex, L7_uchar8* serverAddress, L7_IP_ADDRESS_TYPE_t addrType, L7_uint32 *val)
{
  L7_uchar8 curAddr[SNMP_BUFFER_LEN];
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_char8 name[L7_RADIUS_SERVER_NAME_LENGTH+1];

  if ((usmDbRadiusServerHostNameGet(UnitIndex, serverAddress, addrType, name) == L7_SUCCESS) && 
	  (usmDbRadiusNamedServerAddrGet(UnitIndex, name, curAddr, &type) == L7_SUCCESS))
  {    
    if (strncmp(serverAddress,curAddr,sizeof(name)) == 0)
    {
      *val = D_agentRadiusServerCurrentMode_yes;
    }
    else
    {
      *val = D_agentRadiusServerCurrentMode_no;
    }

	return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Get the message authenticator attribute setting of the entry
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverAddress  @b((input)) server IP Address
* @param    val  @b((output)) value of the object
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
snmpAgentRadiusServerMsgAuthGet(L7_uint32 UnitIndex, L7_uchar8* serverAddress,L7_IP_ADDRESS_TYPE_t type, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbRadiusHostNameServerIncMsgAuthModeGet(UnitIndex, serverAddress,type, &temp_val);

  if (rc == L7_SUCCESS)
  {
    if (temp_val == L7_ENABLE)
    {
      *val = D_agentRadiusServerMsgAuth_enable;
    }
    else
    {
      *val = D_agentRadiusServerMsgAuth_disable;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Enable or disable the message authenticator attribute for this server
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverAddress  @b((input)) server IP Address
* @param    val  @b((input)) value of the object
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
L7_RC_t snmpAgentRadiusServerMsgAuthSet(L7_uint32 UnitIndex, 
             L7_uchar8 *serverAddress,
             L7_IP_ADDRESS_TYPE_t type, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 temp_val = 0;

  switch (val)
  {
  case D_agentRadiusServerMsgAuth_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentRadiusServerMsgAuth_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmDbRadiusHostNameServerIncMsgAuthModeSet(UnitIndex, serverAddress,type, temp_val);
  }

  return rc;
}


/* End Function Declarations */
