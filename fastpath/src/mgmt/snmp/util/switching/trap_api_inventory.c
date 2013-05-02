/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: trap_api_stacking.c
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by: cpverne 06/26/2003
*
* Component: SNMP
*
*********************************************************************/

#include "l7_common.h"
#include "snmp_api.h"
#include "privatetrap_stacking.h"
#include "snmp_trap_api_stacking.h"

/* Begin Function Definitions: snmp_trap_api_stacking.h */

/*********************************************************************
*
* @purpose  Send agentStackingCardMismatch Trap
*
* @param    unitIndex       the unit identifier
* @param    slotIndex       the slot identifier
* @param    insertedType    the card type inserted into this slot
* @param    configuredType  the card type configured for this slot
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpAgentStackingCardMismatchTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType, L7_int32 configuredType)
{
  if ( SnmpStatusGet() == L7_ENABLE &&
       snmp_agentStackingCardMismatchTrapSend(unitIndex, slotIndex, insertedType, configuredType) == 0 )
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Send agentStackingCardUnsupported Trap
*
* @param    unitIndex       the unit identifier
* @param    slotIndex       the slot identifier
* @param    insertedType    the card type inserted into this slot
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpAgentStackingCardUnsupportedTrapSend(L7_int32 unitIndex, L7_int32 slotIndex, L7_int32 insertedType)
{
  if ( SnmpStatusGet() == L7_ENABLE &&
       snmp_agentStackingCardUnsupportedTrapSend(unitIndex, slotIndex, insertedType) == 0 )
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Send agentStackingCardMismatch Trap
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpEntConfigChangeTrapSend()
{
  if ( SnmpStatusGet() == L7_ENABLE &&
       snmp_entConfigChangeTrapSend() == 0 )
    return L7_SUCCESS;

  return L7_FAILURE;
}

/* End Function Definitions */
