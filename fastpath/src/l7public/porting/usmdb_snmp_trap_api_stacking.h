/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\usmdb\usmdb_snmp_trap.c
*
* @purpose Provide interface to SNMP Trap API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 01/10/2001
*
* @author cpverne
* @end
*
**********************************************************************/

#ifndef USMDB_SNMP_TRAP_API_STACKING_H
#define USMDB_SNMP_TRAP_API_STACKING_H

/* Begin Function Prototypes */

/*********************************************************************
* @purpose  Send Card Mismatch Trap via SNMP Agent
*
* @param    unitIndex       The unit for this operation
* @param    slotIndex       The slot for this operation
* @param    insertedType    The slot for this operation
* @param    configuredType  The slot for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*       
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpAgentStackingCardMismatchTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType, L7_uint32 configuredType);

/*********************************************************************
* @purpose  Send Card Unsupported Trap via SNMP Agent
*
* @param    unitIndex       The unit for this operation
* @param    slotIndex       The slot for this operation
* @param    insertedType    The slot for this operation
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*       
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpAgentStackingCardUnsupportedTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType);

/*********************************************************************
* @purpose  Send Entity Config Change Trap via SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None.
*       
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpEntConfigChangeTrapSend();

/* End Function Prototypes */

#endif /* USMDB_SNMP_TRAP_API_STACKING_H */

