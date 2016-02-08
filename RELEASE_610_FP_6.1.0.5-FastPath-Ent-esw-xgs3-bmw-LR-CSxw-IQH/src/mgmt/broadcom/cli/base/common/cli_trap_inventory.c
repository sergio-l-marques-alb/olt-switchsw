/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename src/mgmt/cli/base/cli_trap_inventory.c
*
* @purpose   Inventory Trap API Implementation file
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

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include <stdio.h>
#include "l7_common.h"
#include "cli_web_exports.h"
#include "cli_trap_inventory.h"
#include "trap_inventory_api.h"
#include "cardmgr_api.h"
#include "cliapi.h"

extern L7_char8 card_mismatch_str[];
extern L7_char8 card_unsupported_str[];

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
                              trapMgrNotifyEvents_t event)
{
  L7_char8 trapStringBuf[L7_CLI_MAX_STRING_LENGTH];

  memset (trapStringBuf, 0, (L7_int32)sizeof(trapStringBuf));
  switch (event)
  {
  case TRAPMGR_CARD_MISMATCH_EVENT:
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), card_mismatch_str, unit, slot, ins_cardTypeId, cfg_cardTypeId);
    break;

  case TRAPMGR_CARD_UNSUPPORTED_EVENT:
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), card_unsupported_str, unit, slot, ins_cardTypeId);
    break;

  default:
    break;
  }

  cliTrapMsgStringSet(trapStringBuf);

  return;
}

/*-- QSCAN_RJ: Uncomment the following code when card traps are made configurable --*/

#if 0  /* since Inventory Traps are not currently configurable */
/*********************************************************************
* @purpose  Signifies a card mismatch state
*
* @param    unit  unit index
* @param    slot  slot index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cliInventoryCardMismatchTrap(L7_uint32 unit, L7_uint32 slot)
{
  return trapMgrInventoryCardMismatchTrap(unit, slot);
}

/*********************************************************************
* @purpose  Signifies that a request to create a card is received
*           for a card type not supported on this unit
*
* @param    unit  unit index
* @param    slot  slot index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cliInventoryCardUnsuppportedTrap(L7_uint32 unit, L7_uint32 slot)
{
  return trapMgrInventoryCardUnsupportedTrap(unit, slot);
}
#endif  /* since Inventory Traps are not currently configurable */
