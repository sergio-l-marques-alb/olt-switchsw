/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_dot1x_api.h
*
* Purpose: System-specific code for IEEE8021-PAE-MIB
*
* Created by: Colin Verne 04/17/2003
*
* Component: SNMP
*
*********************************************************************/ 

#ifndef K_MIB_DOT1X_API_H
#define K_MIB_DOT1X_API_H

#include <k_private_base.h>

/* Begin Function Prototypes */

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
snmpDot1xPaeSystemAuthControlGet(L7_uint32 UnitIndex, L7_int32 *val);


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
snmpDot1xPaeSystemAuthControlSet(L7_uint32 UnitIndex, L7_int32 val);

/**************************************************************************************************************/

L7_RC_t
snmpDot1xPaePortCapabilitiesGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_char8 *buf, L7_uint32 *buf_len);

L7_RC_t
snmpDot1xPaePortInitializeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

L7_RC_t
snmpDot1xPaePortReauthenticateSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

/**************************************************************************************************************/

L7_RC_t
snmpDot1xAuthPaeStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthBackendAuthStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthAdminControlledDirectionsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthAdminControlledDirectionsSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

L7_RC_t
snmpDot1xAuthOperControlledDirectionsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthAuthControlledPortStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthAuthControlledPortControlGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthAuthControlledPortControlSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

L7_RC_t
snmpDot1xAuthReAuthEnabledGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthReAuthEnabledSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

L7_RC_t
snmpDot1xAuthKeyTxEnabledGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val);

L7_RC_t
snmpDot1xAuthKeyTxEnabledSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 val);

/* End Function Prototypes */

#endif /* K_MIB_DOT1X_API_H */
