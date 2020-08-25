/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename trap_inventory.h
*
* @purpose Trap Manager Inventory internal routines
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

#ifndef TRAP_INVENTORY_H
#define TRAP_INVENTORY_H

#include "trap_inventory_api.h"

/*********************************************************************
* @purpose  Receive call back event from Card Manager/Power Unit
*           Manager/Fan Unit Manager
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
void trapMgrCardEventCallBack(L7_uint32 unit, L7_uint32 slot, 
                              L7_uint32 ins_cardTypeId, 
                              L7_uint32 cfg_cardTypeId, 
                              trapMgrNotifyEvents_t event);

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
                                  L7_uint32 ins_cardTypeId, 
                                  L7_uint32 cfg_cardTypeId, 
                                  trapMgrNotifyEvents_t event);

#endif /* TRAP_INVENTORY_H */
