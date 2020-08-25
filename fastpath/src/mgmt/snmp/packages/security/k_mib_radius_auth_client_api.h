/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_radius_auth_client_api.h
*
* Purpose: APIs used for Radius Auth MIB
*
* Created by: Colin Verne 05/13/2003
*
* Component: SNMP
*
*********************************************************************/ 

#ifndef K_MIB_RADIUS_AUTH_CLIENT_API_H
#define K_MIB_RADIUS_AUTH_CLIENT_API_H

#include <k_private_base.h>

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Check that the index exists
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverIndex  @b((output)) Radius Auth server index
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

L7_RC_t snmpRadiusAuthServerEntryGet(L7_uint32 UnitIndex, L7_uint32 serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrtype);


/*********************************************************************
*
* @purpose  Find the next valid index
*          
* @param    UnitIndex @b((input)) the unit for this operation
* @param    serverIndex  @b((output)) Radius Auth server index
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

L7_RC_t snmpRadiusAuthServerEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *serverIndex, L7_uchar8 *serverAddress, L7_IP_ADDRESS_TYPE_t *addrtype);

/* End Function Prototypes */

#endif /* K_MIB_RADIUS_AUTH_CLIENT_API_H */
