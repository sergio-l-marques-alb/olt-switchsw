/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: privatetrap_entity.h
*
* Purpose: Entity-related trap functions
*
* Created by: cpverne 06/26/2003
*
* Component: SNMP
*
*********************************************************************/

#ifndef PRIVATETRAP_ENTITY_H
#define PRIVATETRAP_ENTITY_H

/* Begin Function Prototypes */

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
* @end
*********************************************************************/
L7_RC_t snmp_entConfigChangeTrapSend();

/* End Function Prototypes */

#endif /* PRIVATETRAP_ENTITY_H */
