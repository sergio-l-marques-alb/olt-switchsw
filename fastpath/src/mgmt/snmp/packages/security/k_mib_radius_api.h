/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_radius_api.h
*
* Purpose: APIs used for Radius Private MIB
*
* Created by: Prashant Murthy 05/07/2003
*
* Component: SNMP
*
*********************************************************************/ 

#ifndef K_MIB_RADIUS_API_H
#define K_MIB_RADIUS_API_H

#include <k_private_base.h>

/* Begin Function Prototypes */

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
L7_RC_t snmpAgentRadiusStatsClearSet(L7_uint32 UnitIndex, L7_uint32 val);

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
L7_RC_t snmpAgentRadiusAccountingModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

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
L7_RC_t snmpAgentRadiusAccountingModeSet(L7_uint32 UnitIndex, L7_uint32 val);

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
L7_RC_t snmpAgentRadiusAccountingIndexNextValidGet(L7_uint32 UnitIndex, L7_uint32 *val);

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
L7_RC_t snmpAgentRadiusServerIndexNextValidGet(L7_uint32 UnitIndex, L7_uint32 *val);

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

L7_RC_t snmpAgentRadiusAccountingConfigEntryGet(L7_uint32 UnitIndex, L7_uint32 serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrType);


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

L7_RC_t snmpAgentRadiusAccountingConfigEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrType);


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

L7_RC_t snmpAgentRadiusServerConfigEntryGet(L7_uint32 UnitIndex, L7_uint32 serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrType);


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

L7_RC_t snmpAgentRadiusServerConfigEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrType);


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
snmpAgentRadiusServerPrimaryModeGet(L7_uint32 UnitIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t type,L7_uint32 *val);


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
L7_RC_t snmpAgentRadiusServerPrimaryModeSet(L7_uint32 UnitIndex, L7_uchar8 *serverAddress,  L7_IP_ADDRESS_TYPE_t type,L7_uint32 val);

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
snmpAgentRadiusServerCurrentModeGet(L7_uint32 UnitIndex, L7_uchar8* serverAddress, L7_IP_ADDRESS_TYPE_t addrType, L7_uint32 *val);

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
snmpAgentRadiusServerMsgAuthGet(L7_uint32 UnitIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t type, L7_uint32 *val);

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
snmpAgentRadiusServerPrimaryModeGet(L7_uint32 UnitIndex, L7_uchar8* serverAddress,L7_IP_ADDRESS_TYPE_t type, L7_uint32 *val);

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
L7_RC_t snmpAgentRadiusServerPrimaryModeSet(L7_uint32 UnitIndex, L7_uchar8 *serverAddress,L7_IP_ADDRESS_TYPE_t type, L7_uint32 val);

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
L7_RC_t snmpAgentRadiusServerMsgAuthSet(L7_uint32 UnitIndex, L7_uchar8 *serverAddress,L7_IP_ADDRESS_TYPE_t type,  L7_uint32 val);

/* End Function Prototypes */

#endif /* K_MIB_RADIUS_API_H */
