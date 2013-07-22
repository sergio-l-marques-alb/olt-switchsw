/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_fastpathinventory.h
*
* Purpose: Inventory-related trap functions
*
* Created by: cpverne 06/26/2003
*
* Component: SNMP
*
*********************************************************************/

#ifndef PRIVATETRAP_FASTPATHINVENTORY_H
#define PRIVATETRAP_FASTPATHINVENTORY_H

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Sends a trap associated with inserting a card into a slot
*           which is configured for a card of a different type.
*
* @param    unitIndex       Unit Index
* @param    slotIndex       Slot Index
* @param    insertedType    Card type inserted
* @param    configuredType  Card the slot is configured for
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryCardMismatchTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType, L7_uint32 configuredType);


/*********************************************************************
*
* @purpose  Sends a trap associated with inserting a card into a slot
*           which is not supported by the slot.
*
* @param    unitIndex       Unit Index
* @param    slotIndex       Slot Index
* @param    insertedType    Card type inserted into the
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryCardUnsupportedTrapSend(L7_uint32 unitIndex, L7_uint32 slotIndex, L7_uint32 insertedType);

/*********************************************************************
*
* @purpose  Sends a trap associated with the link status of a Front Panel Stack Port going up.
*
* @param    agentInventoryStackPortIndex  Stack Port Index
* @param    agentInventoryStackPortUnit   Unit Index of the Unit the Stack Port is on
* @param    agentInventoryStackPortTag    Tag associated with that Stack Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryStackPortLinkUpTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag);

/*********************************************************************
*
* @purpose  Sends a trap associated with the link status of a Front Panel Stack Port going down.
*
* @param    agentInventoryStackPortIndex  Stack Port Index
* @param    agentInventoryStackPortUnit   Unit Index of the Unit the Stack Port is on
* @param    agentInventoryStackPortTag    Tag associated with that Stack Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates an SNMP trap OID and a VarBind structure.
*
* @end
*********************************************************************/
L7_RC_t snmp_agentInventoryStackPortLinkDownTrapSend(L7_uint32 agentInventoryStackPortIndex, L7_uint32 agentInventoryStackPortUnit, L7_char8 *agentInventoryStackPortTag);

/* End Function Prototypes */

#endif /* PRIVATETRAP_FASTPATHINVENTORY_H */
