/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trap_inventory.c
*
* @purpose Trap Manager Inventory functions
*
* @component Trap Manager
*
* @comments
*
* @created 06/06/2003
*
* @author rjindal
* @end
*
*********************************************************************/

/*********************************************************************

*********************************************************************/

#include <stdio.h>
#include "inventory_commdefs.h"
#include "trap_inventory_api.h"
#include "trap_inventory.h"
#include "trapapi.h"
#include "trap.h"
#include "usmdb_snmp_api.h"
#include "usmdb_common.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_snmp_trap_api.h"
#include "cardmgr_api.h"

extern trapMgrTrapData_t  trapMgrTrapData;
extern trapMgrCfgData_t   trapMgrCfgData;

#ifndef L7_CHASSIS
L7_char8 card_mismatch_str[] = "Card Mismatch: Unit:%d Slot:%d Inserted-Card:0x%x Config-Card:0x%x";
L7_char8 card_unsupported_str[] = "Card Unsupported: Unit:%d Slot:%d Inserted-Card:0x%x";
L7_char8 stack_port_link_up_str[] = "Stack Port Link Up: Index: %x Unit: %d Tag: %s";
L7_char8 stack_port_link_down_str[] = "Stack Port Link Down: Index: %x Unit: %d Tag: %s";
L7_char8 card_plugin_str[] = "Card Plugin: Unit:%d Slot:%d Inserted-Card:0x%x";
L7_char8 card_unplug_str[] = "Card Unplug: Unit:%d Slot:%d";
L7_char8 card_failure_str[] = "Card Failure: Unit:%d Slot:%d";
#else
L7_char8 card_mismatch_str[] = "Card Mismatch: Module:%d Slot:%d Inserted-Card:0x%x Config-Card:0x%x";
L7_char8 card_unsupported_str[] = "Card Unsupported: Module:%d Slot:%d Inserted-Card:0x%x";
L7_char8 stack_port_link_up_str[] = "Backplane Port Link Up: Index: %x Module: %d Tag: %s";
L7_char8 stack_port_link_down_str[] = "Backplane Port Link Down: Index: %x Module: %d Tag: %s";
L7_char8 card_plugin_str[] = "Card Plugin: Module:%d Slot:%d Inserted-Card:0x%x";
L7_char8 card_unplug_str[] = "Card Unplug: Module:%d Slot:%d";
L7_char8 card_failure_str[] = "Card Failure: Module:%d Slot:%d";
#endif

extern trapMgrNotifyRequest_t *pTrapMgrNotifyRequest;

/*********************************************************************
* @purpose  Receive call back event from Card Manager
*
* @param    unit            the unit index
* @param    slot            the slot index
* @param    ins_cardTypeId  id of inserted card
* @param    cfg_cardTypeId  id of configured card
* @param    event           notification event (see trapMgrNotifyEvents_t)
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
void trapMgrCardEventCallBack(L7_uint32 unit, L7_uint32 slot,
                              L7_uint32 ins_cardTypeId, L7_uint32 cfg_cardTypeId,
                              trapMgrNotifyEvents_t event)
{
  switch (event)
  {
  case TRAPMGR_CARD_MISMATCH_EVENT:
    (void)trapMgrInventoryCardMismatchTrap(unit, slot, ins_cardTypeId, cfg_cardTypeId);
    break;

  case TRAPMGR_CARD_UNSUPPORTED_EVENT:
    (void)trapMgrInventoryCardUnsupportedTrap(unit, slot, ins_cardTypeId);
    break;

  case TRAPMGR_CARD_PLUGIN_EVENT:
    (void)trapMgrInventoryCardPluginTrap(unit, slot, ins_cardTypeId);
    break;

  case TRAPMGR_CARD_UNPLUG_EVENT:
    (void)trapMgrInventoryCardUnplugTrap(unit, slot);
    break;

  case TRAPMGR_CARD_FAILURE_EVENT:
    (void)trapMgrInventoryCardFailureTrap(unit, slot);
    break;

  default:
    break;
  }

  return;
}

/*********************************************************************
* @purpose  Signifies a card mismatch state
*
* @param    unit            the unit index
* @param    slot            the slot index
* @param    ins_cardTypeId  id of inserted card
* @param    cfg_cardTypeId  id of configured card
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryCardMismatchTrap(L7_uint32 unit, L7_uint32 slot,
                                         L7_uint32 ins_cardTypeId,
                                         L7_uint32 cfg_cardTypeId)
{
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t   rc = L7_SUCCESS;

  /* notify registered routines */
  trapMgrNotifyRegisteredUsers(unit, slot, ins_cardTypeId,
                               cfg_cardTypeId, TRAPMGR_CARD_MISMATCH_EVENT);

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  sprintf(trapStringBuf, card_mismatch_str, unit, slot, ins_cardTypeId, cfg_cardTypeId);

  /* add trap to the trap log */
  trapMgrLogTrapToLocalLog(trapStringBuf);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CARD_MISMATCH;

  /* send trap to snmp, if enabled */
  if (usmDbSnmpStatusGet(USMDB_UNIT_CURRENT) == L7_ENABLE)
    rc = usmDbSnmpAgentInventoryCardMismatchTrapSend(unit, slot,
                                                     ins_cardTypeId, cfg_cardTypeId);
  return rc;
}

/*********************************************************************
* @purpose  Signifies that a request to create a card is received
*           for a card type not supported on this unit
*
* @param    unit            the unit index
* @param    slot            the slot index
* @param    ins_cardTypeId  id of inserted card
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryCardUnsupportedTrap(L7_uint32 unit, L7_uint32 slot,
                                            L7_uint32 ins_cardTypeId)
{
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t   rc = L7_SUCCESS;

  /* notify registered routines */
  trapMgrNotifyRegisteredUsers(unit, slot, ins_cardTypeId,
                               0, TRAPMGR_CARD_UNSUPPORTED_EVENT);

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  sprintf(trapStringBuf, card_unsupported_str, unit, slot, ins_cardTypeId);

  /* add trap to the trap log */
  trapMgrLogTrapToLocalLog(trapStringBuf);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CARD_UNSUPPORTED;

  /* send trap to snmp, if enabled */
  if (usmDbSnmpStatusGet(USMDB_UNIT_CURRENT) == L7_ENABLE)
    rc = usmDbSnmpAgentInventoryCardUnsupportedTrapSend(unit, slot,
                                                        ins_cardTypeId);
  return rc;
}

/*********************************************************************
* @purpose  Signifies that a Stack Port has transitioned to Link Up
*
* @param    stackPortIndex  the Stack Port index
* @param    stackPortUnit   the Unit Index
* @param    stackPortTag    the tag associated with the Stack Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryStackPortLinkUpTrap(L7_uint32 stackPortIndex, L7_uint32 stackPortUnit, L7_char8 *stackPortTag)
{
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t   rc = L7_SUCCESS;

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  sprintf(trapStringBuf, stack_port_link_up_str, stackPortIndex, stackPortUnit, stackPortTag);

  /* add trap to the trap log */
  trapMgrLogTrapToLocalLog(trapStringBuf);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_STACK_PORT_LINK_UP;

  /* send trap to snmp, if enabled */
  if (usmDbSnmpStatusGet(USMDB_UNIT_CURRENT) == L7_ENABLE)
    rc = usmDbSnmpAgentInventoryStackPortLinkUpTrapSend(stackPortIndex, stackPortUnit, stackPortTag);
  return rc;
}

/*********************************************************************
* @purpose  Signifies that a Stack Port has transitioned to Link Up
*
* @param    stackPortIndex  the Stack Port index
* @param    stackPortUnit   the Unit Index
* @param    stackPortTag    the tag associated with the Stack Port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryStackPortLinkDownTrap(L7_uint32 stackPortIndex, L7_uint32 stackPortUnit, L7_char8 *stackPortTag)
{
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t   rc = L7_SUCCESS;

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  sprintf(trapStringBuf, stack_port_link_down_str, stackPortIndex, stackPortUnit, stackPortTag);

  /* add trap to the trap log */
  trapMgrLogTrapToLocalLog(trapStringBuf);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_STACK_PORT_LINK_DOWN;

  /* send trap to snmp, if enabled */
  if (usmDbSnmpStatusGet(USMDB_UNIT_CURRENT) == L7_ENABLE)
  {
    rc = usmDbSnmpAgentInventoryStackPortLinkDownTrapSend(stackPortIndex, stackPortUnit, stackPortTag);
    usmDbTrapMgrStackLinkFailedTrapSend(stackPortUnit, stackPortTag);
  }
  return rc;
}

/*********************************************************************
* @purpose  Signifies a card plugin state
*
* @param    unit            the unit index
* @param    slot            the slot index
* @param    ins_cardTypeId  id of inserted card
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryCardPluginTrap(L7_uint32 unit, L7_uint32 slot,
                                         L7_uint32 ins_cardTypeId)
{
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t   rc = L7_SUCCESS;

  /* notify registered routines */
  trapMgrNotifyRegisteredUsers(unit, slot, ins_cardTypeId, 0, TRAPMGR_CARD_PLUGIN_EVENT);

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  sprintf(trapStringBuf, card_plugin_str, unit, slot, ins_cardTypeId);

  /* add trap to the trap log */
  trapMgrLogTrapToLocalLog(trapStringBuf);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CARD_PLUGIN;

  /* send trap to snmp, if enabled */
  if (usmDbSnmpStatusGet(USMDB_UNIT_CURRENT) == L7_ENABLE)
    rc = usmDbSnmpAgentInventoryCardPluginTrapSend(unit, slot, ins_cardTypeId);
  return rc;
}

/*********************************************************************
* @purpose  Signifies a card unplug state
*
* @param    unit            the unit index
* @param    slot            the slot index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryCardUnplugTrap(L7_uint32 unit, L7_uint32 slot)
{
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t   rc = L7_SUCCESS;

  /* notify registered routines */
  trapMgrNotifyRegisteredUsers(unit, slot, 0, 0, TRAPMGR_CARD_UNPLUG_EVENT);

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  sprintf(trapStringBuf, card_unplug_str, unit, slot);

  /* add trap to the trap log */
  trapMgrLogTrapToLocalLog(trapStringBuf);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CARD_UNPLUG;

  /* send trap to snmp, if enabled */
  if (usmDbSnmpStatusGet(USMDB_UNIT_CURRENT) == L7_ENABLE)
    rc = usmDbSnmpAgentInventoryCardUnplugTrapSend(unit, slot);
  return rc;
}

/*********************************************************************
* @purpose  Signifies a card failure state
*
* @param    unit            the unit index
* @param    slot            the slot index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryCardFailureTrap(L7_uint32 unit, L7_uint32 slot)
{
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t   rc = L7_SUCCESS;

  /* notify registered routines */
  trapMgrNotifyRegisteredUsers(unit, slot, 0, 0, TRAPMGR_CARD_FAILURE_EVENT);

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  sprintf(trapStringBuf, card_failure_str, unit, slot);

  /* add trap to the trap log */
  trapMgrLogTrapToLocalLog(trapStringBuf);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CARD_FAILURE;

  /* send trap to snmp, if enabled */
  if (usmDbSnmpStatusGet(USMDB_UNIT_CURRENT) == L7_ENABLE)
    rc = usmDbSnmpAgentInventoryCardFailureTrapSend(unit, slot);
  return rc;
}


/*********************************************************************
* @purpose  Register a routine to be called when a card mismatch
*           event occurs
*
* @param    componentID  user component's ID (see L7_COMPONENT_IDS_t)
* @param    *notify      pointer to notification routine to be invoked
*                        for event change with the following parms:
*                        unit            the unit id
*                        slot            the slot id
*                        ins_cardTypeId  id of inserted card
*                        cfg_cardTypeId  id of configured card
*                        event           TRAPMGR_CARD_MISMATCH_EVENT
*
* @returns  L7_SUCCESS, if notification is successful
* @returns  L7_FAILURE, if invalid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrRegisterCardMismatchEvent(L7_COMPONENT_IDS_t componentID,
                                   void (*notify)(L7_uint32 unit,
                                                  L7_uint32 slot,
                                                  L7_uint32 ins_cardTypeId,
                                                  L7_uint32 cfg_cardTypeId,
                                                  trapMgrNotifyEvents_t event))
{
  if (componentID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "TRAPMGR: component ID %d greater than or equal to L7_LAST_COMPONENT_ID\n",
            (L7_int32)componentID);
    return L7_FAILURE;
  }

  pTrapMgrNotifyRequest[componentID].registrarID = componentID;
  pTrapMgrNotifyRequest[componentID].notify_request = notify;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register a routine to be called when an unsupported card
*           event occurs
*
* @param    componentID  user component's ID (see L7_COMPONENT_IDS_t)
* @param    *notify      pointer to notification routine to be invoked
*                        for event change with the following parms:
*                        unit            the unit id
*                        slot            the slot id
*                        ins_cardTypeId  id of inserted card
*                        cfg_cardTypeId  id of configured card
*                        event           TRAPMGR_CARD_UNSUPPORTED_EVENT
*
* @returns  L7_SUCCESS, if notification is successful
* @returns  L7_FAILURE, if invalid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrRegisterCardUnsupportedEvent(L7_COMPONENT_IDS_t componentID,
                                      void (*notify)(L7_uint32 unit,
                                                     L7_uint32 slot,
                                                     L7_uint32 ins_cardTypeId,
                                                     L7_uint32 cfg_cardTypeId,
                                                     trapMgrNotifyEvents_t event))
{
  if (componentID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "TRAPMGR: component ID %d greater than or equal to L7_LAST_COMPONENT_ID\n",
            (L7_int32)componentID);
    return L7_FAILURE;
  }

  pTrapMgrNotifyRequest[componentID].registrarID = componentID;
  pTrapMgrNotifyRequest[componentID].notify_request = notify;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register a routine to be called when a card event occurs
*
* @param    componentID  user component's ID (see L7_COMPONENT_IDS_t)
* @param    *notify      pointer to notification routine to be invoked
*                        for event change with the following parms:
*                        unit            the unit id
*                        slot            the slot id
*                        ins_cardTypeId  id of inserted card
*                        cfg_cardTypeId  id of configured card
*                        event           TRAPMGR_CARD_MISMATCH_EVENT or
*                                        TRAPMGR_CARD_UNSUPPORTED_EVENT
*
* @returns  L7_SUCCESS, if notification is successful
* @returns  L7_FAILURE, if invalid component ID
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrRegisterCardEvent( L7_COMPONENT_IDS_t componentID,
                                 void (*notify)(L7_uint32 unit,
                                                L7_uint32 slot,
                                                L7_uint32 ins_cardTypeId,
                                                L7_uint32 cfg_cardTypeId,
                                                trapMgrNotifyEvents_t event))
{
  if (componentID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TRAPMGR_COMPONENT_ID,
            "TRAPMGR: component ID %d greater than or equal to L7_LAST_COMPONENT_ID\n",
            (L7_int32)componentID);
    return L7_FAILURE;
  }

  pTrapMgrNotifyRequest[componentID].registrarID = componentID;
  pTrapMgrNotifyRequest[componentID].notify_request = notify;

  return L7_SUCCESS;
}




/*********************************************************************
* @purpose  Notify registered users of registered event request
*
* @param    unit            the unit index
* @param    slot            the slot index
* @param    ins_cardTypeId  id of inserted card
* @param    cfg_cardTypeId  id of configured card
* @param    event           notification event (see trapMgrNotifyEvents_t)
*
* @returns  Void
*
* @comments
*
* @end
*********************************************************************/
void trapMgrNotifyRegisteredUsers(L7_uint32 unit, L7_uint32 slot,
                                  L7_uint32 ins_cardTypeId, L7_uint32 cfg_cardTypeId,
                                  trapMgrNotifyEvents_t event)
{
  L7_uint32 i;

  if ((event >= TRAPMGR_CARD_MISMATCH_EVENT) &&
      (event < TRAPMGR_CARD_MAX_EVENTS))
  {
    for (i = 1; i < L7_LAST_COMPONENT_ID; i++)
    {
      if (pTrapMgrNotifyRequest[i].registrarID != L7_NULL)
      {
        pTrapMgrNotifyRequest[i].notify_request(unit, slot, ins_cardTypeId,
                                                cfg_cardTypeId, event);
      }
    }
  }

  return;
}

/*- QSCAN_RJ: Uncomment the following code when card traps are made configurable --*/

#if 0  /* since Inventory Traps are not currently configurable */

/*********************************************************************
* @purpose  Determine configured Inventory trap flags
*
* @param    trapFlags  bit mask indicating enabled stacking traps
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryTrapFlagsGet(L7_uint32 *trapFlags)
{
  *trapFlags = trapMgrCfgData.trapInventory;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return Trap Manager's Inventory trap state
*
* @param    unit      unit index
* @param    trapType  stacking trap type (see L7_STACKING_TRAP_t)
* @param    *mode     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryTrapModeGet(L7_uint32 unit, L7_uint32 trapType, L7_uint32 *mode)
{
  if(trapMgrCfgData.trapInventory & trapType)
    *mode = L7_ENABLE;
  else
    *mode = L7_DISABLE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set Trap Manager's Inventory trap state
*
* @param    trapType  stacking trap type (see L7_STACKING_TRAP_t)
* @param    mode      L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t trapMgrInventoryTrapModeSet(L7_uint32 trapType, L7_uint32 mode)
{
  if(mode == L7_ENABLE)
    trapMgrCfgData.trapInventory |= trapType;
  else if(mode == L7_DISABLE)
    trapMgrCfgData.trapInventory &= ~trapType;
  else
    return L7_FAILURE;

  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

#endif  /* since Inventory Traps are not currently configurable */

/*-- QSCAN_RJ: thru here --*/

