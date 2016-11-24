/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename src/mgmt/cli/base/cli_trap_inventory.h
*
* @purpose   Inventory Trap API Header file
*
* @component CLI
*
* @comments
*
* @create    06/09/2003
*
* @author    rjindal
* @end
*
*********************************************************************/

#ifndef CLI_TRAP_INVENTORY_H
#define CLI_TRAP_INVENTORY_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "trap_inventory_api.h"

/*********************************************************************
* @purpose  Receive card events from TRAPMGR
*
* @param    unit            the unit index
* @param    slot            the slot index
* @param    ins_cardTypeId  id of inserted card
* @param    cfg_cardTypeId  id of configured card
* @param    event           notification event (see trapMgrNotifyEvents_t)
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cliInventoryCardCallback(L7_uint32 unit, L7_uint32 slot,
                              L7_uint32 ins_cardTypeId,
                              L7_uint32 cfg_cardTypeId,
                              trapMgrNotifyEvents_t event);

/*-- QSCAN_RJ: Uncomment the following code when card traps are made configurable --*/

#if 0  /* since Inventory Traps are not currently configurable */
/*********************************************************************
* @purpose  Signifies a card mismatch state
*
* @param    unit  unit index
* @param    slot  slotindex
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cliInventoryCardMismatchTrap(L7_uint32 unit, L7_uint32 slot);

/*********************************************************************
* @purpose  Signifies that a request to create a card is received
*           for a card type not supported on this unit
*
* @param    unit  unit index
* @param    slot  slotindex
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cliInventoryCardUnsuppportedTrap(L7_uint32 unit, L7_uint32 slot);
#endif  /* since Inventory Traps are not currently configurable */

#endif  /* CLI_TRAP_INVENTORY_H */
