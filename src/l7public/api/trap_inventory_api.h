/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename trap_inventory_api.h
*
* @purpose Trap Manager Inventory functions
*
* @component Trap Manager
*
* @comments
*
* @created 06/09/2003
*
* @author rjindal
* @end
*
*********************************************************************/

/*********************************************************************
                    
*********************************************************************/

#ifndef TRAP_INVENTORY_API_H
#define TRAP_INVENTORY_API_H

#include "l7_common.h"

#define TRAPMGR_SPECIFIC_CARD_MISMATCH        1
#define TRAPMGR_SPECIFIC_CARD_UNSUPPORTED     2
#define TRAPMGR_SPECIFIC_STACK_PORT_LINK_UP   3
#define TRAPMGR_SPECIFIC_STACK_PORT_LINK_DOWN 4
#define TRAPMGR_SPECIFIC_CARD_PLUGIN          5
#define TRAPMGR_SPECIFIC_CARD_UNPLUG          6
#define TRAPMGR_SPECIFIC_CARD_FAILURE         7


/* TRAPMGR outcall notification events */
typedef enum
{
  TRAPMGR_CARD_MISMATCH_EVENT = 1,      /* card type does not match */
  TRAPMGR_CARD_UNSUPPORTED_EVENT,       /* card type not supported */
  TRAPMGR_CARD_PLUGIN_EVENT,            /* card plugged in */
  TRAPMGR_CARD_UNPLUG_EVENT,            /* card unplugged */
  TRAPMGR_CARD_FAILURE_EVENT,           /* card failed */
  TRAPMGR_CARD_MAX_EVENTS               /* max number of notification events - always last */

} trapMgrNotifyEvents_t;

typedef struct
{
  L7_uint32  registrarID;
  void       (*notify_request)(L7_uint32 unit, 
                               L7_uint32 slot, 
                               L7_uint32 ins_cardTypeId,
                               L7_uint32 cfg_cardTypeId,
                               trapMgrNotifyEvents_t event);
} trapMgrNotifyRequest_t;

extern L7_char8 card_mismatch_str[];
extern L7_char8 card_unsupported_str[];
extern L7_char8 card_plugin_str[];
extern L7_char8 card_unplug_str[];
extern L7_char8 card_failure_str[];

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
                                         L7_uint32 cfg_cardTypeId);

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
                                            L7_uint32 ins_cardTypeId);

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
L7_RC_t trapMgrInventoryStackPortLinkUpTrap(L7_uint32 stackPortIndex, L7_uint32 stackPortUnit, L7_char8 *stackPortTag);

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
L7_RC_t trapMgrInventoryStackPortLinkDownTrap(L7_uint32 stackPortIndex, L7_uint32 stackPortUnit, L7_char8 *stackPortTag);

/*********************************************************************
* @purpose  Signifies that a card is plugged in
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
                                       L7_uint32 ins_cardTypeId);

/*********************************************************************
* @purpose  Signifies that a card is unplugged
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
L7_RC_t trapMgrInventoryCardUnplugTrap(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
* @purpose  Signifies that a card is failed
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
L7_RC_t trapMgrInventoryCardFailureTrap(L7_uint32 unit, L7_uint32 slot);

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
L7_RC_t trapMgrRegisterCardEvent(L7_COMPONENT_IDS_t componentID, 
                                 void (*notify)(L7_uint32 unit, 
                                                L7_uint32 slot, 
                                                L7_uint32 ins_cardTypeId,
                                                L7_uint32 cfg_cardTypeId, 
                                                trapMgrNotifyEvents_t event));

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
                                                  trapMgrNotifyEvents_t event));

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
                                                     trapMgrNotifyEvents_t event));

/*- QSCAN_RJ: Uncomment the following code when card traps are made configurable --*/

#if 0  /* since Stacking Traps are not currently configurable */

/*********************************************************************
* @purpose  Determine configured stacking trap flags
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
L7_RC_t trapMgrStackingTrapFlagsGet(L7_uint32 *trapFlags);

/*********************************************************************
* @purpose  Return Trap Manager's Stacking trap state  
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
L7_RC_t trapMgrStackingTrapModeGet(L7_uint32 unit, 
                                   L7_uint32 trapType, L7_uint32 *mode);

/*********************************************************************
* @purpose  Set Trap Manager's Stacking trap state  
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
L7_RC_t trapMgrStackingTrapModeSet(L7_uint32 trapType, L7_uint32 mode);

#endif  /* since Stacking Traps are not currently configurable */

/*-- QSCAN_RJ: thru here --*/

#endif /* TRAP_INVENTORY_API_H */
