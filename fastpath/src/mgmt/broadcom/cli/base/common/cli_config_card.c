/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_config_card.c
*
* @purpose Slot config commands for the cli
*
* @component User Interface
*
* @comments
*
* @create  07/31/2003
*
* @author  rjindal
* @end
*
*********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "datatypes.h"
#include "cli_web_user_mgmt.h"
#include "clicommands_card.h"
#include "cardmgr_api.h"
#include "unitmgr_api.h"
#include "ews.h"
#include "usmdb_cardmgr_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_unitmgr_api.h"

#ifndef _L7_OS_LINUX_
  #include <vxworks_config.h>   /* for DEFAULT_MAX_CONNECTIONS */
#endif /* _L7_OS_LINUX_ */

/*********************************************************************
* @purpose  Configure or remove all configured information of a slot
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype  const L7_char8  *
* @returns  cliPrompt(ewsContext)
*
* @comments  If configuring, this command will result in pre-configuration
*            of the card and all ports associated with the card type.
* @comments  If removing card, this command will remove all configured
*            information for cards or ports located in the slot.
* @comments  Also when removing card, if slot is full, all configured
*            information will be replaced with default information
*            and card will be administratively disabled.
*
* @mode  Global Config
*
* @cmdsyntax in stacking env:
*         for normal command:  slot <unit/slot> <cardindex>
*         for   no   command:  no slot <unit/slot>
* @cmdsyntax in non-stacking env:
*             for normal command:  slot <slot> <cardindex>
*             for   no   command:  no slot <slot>
*
* @cmdhelp  Configure slots.
*
* @cmddescript  Configures or removes configured information of slot(s).
*
* @end
*********************************************************************/
const L7_char8 *commandSlot(EwsContext ewsContext, L7_uint32 argc,
                            const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSlot = 1;
  L7_uint32 argType = 2;
  L7_uint32 unit, slot;
  L7_uint32 ux, sx, slot_number;
  L7_uint32 unit_id, card_id, user_cx;
  L7_uint32 i, numSlots;
  L7_uint32 ptrSlots[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL foundSlotNum = L7_FALSE, foundSlotIndex = L7_FALSE;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag!=L7_EXECUTE_SCRIPT)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  cliSyntaxTop(ewsContext);

  /* Check if the specified unit/slot exists */
  if ((rc = cliValidSpecificUnitSlotCheck(argv[index+argSlot], &unit, &slot)) != L7_SUCCESS)
  {
    if (rc == L7_ERROR)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_Invalid_2);
    }
    else if (rc == L7_NOT_EXIST)
    {
      ewsTelnetPrintf (ewsContext, CLIUNIT_X_DOES_NOT_EXIST, (L7_int32)unit);
    }
    else if (rc == L7_NOT_SUPPORTED)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SlotXDoesntExist, (L7_int32)slot);
      ewsTelnetWrite(ewsContext, buf);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* check if specified slot is supported on the specified unit */

  if (usmDbUnitMgrUnitTypeGet(unit, &unit_id) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext,CLIUNIT_X_DOES_NOT_EXIST, (L7_int32)unit);
  }

  if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SlotNoneExist);
  }

  for (i = 0; i < numSlots; i++)
  {
    if (ptrSlots[i] == slot)
    {
      foundSlotNum = L7_TRUE;
      break;
    }
  }

  if (foundSlotNum != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_SlotXDoesntExist, (L7_int32)slot);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgSlot, cliSyntaxSlot());
    }

    if (usmDbUnitIndexFromIDGet(unit_id, &ux) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_SwitchTypeNotSupported, unit_id);
    }

    sx = 1;
    while (usmDbSlotNumFromIndexGet(ux, sx, &slot_number) == L7_SUCCESS)
    {
      if (slot_number == slot)
      {
        foundSlotIndex = L7_TRUE;
        break;
      }
      sx++;
    }

    if (foundSlotIndex != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_SlotXDoesntExist, (L7_int32)slot);
    }

    /* check user input for cardType */
    if (cliConvertTo32BitUnsignedInteger(argv[index+argType], &user_cx) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_UserInput, ewsContext, pStrErr_base_UsrInputCardType);
    }

    if (usmDbCardSupportedTypeIdGet(user_cx, &card_id) != L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, sizeof(buf), pStrErr_base_CardTypeUnSupported, (L7_int32)user_cx);
      ewsTelnetWrite(ewsContext, buf);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_ShowCardIdx);
    }

    rc = usmDbCardInSlotSupportedGet(ux, sx, user_cx);
    if (rc != L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, sizeof(buf), pStrErr_base_CardType_1, (L7_int32)user_cx, cliDisplaySlotHelp(unit, slot));
      ewsTelnetWrite(ewsContext, buf);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_ShowCardIdx);
    }

    /* configure specified slot */
    if ((rc = usmDbSlotCardConfigSet(unit, slot, card_id)) != L7_SUCCESS)
    {
      memset (buf, 0, (L7_int32)sizeof(buf));
      /* cardType is same as the one already configured for the slot */
      if (rc == L7_ALREADY_CONFIGURED)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_CardCreate, cliDisplaySlotHelp(unit, slot));
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, buf, sizeof(buf), pStrErr_base_CardCreate2, cliDisplaySlotHelp(unit, slot));
      }
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgNoSlot, cliSyntaxSlot());
    }

    /* clear specified slot */
    if ((rc = usmDbSlotCardConfigClear(unit, slot)) != L7_SUCCESS)
    {
      /* If cards are not pluggable, clearing the slot configuration
       * and deletion of ports for the slot supported card should be
       * done when unit is deleted.
       *
       * This is done to prevent ports from being deleted when slot
       * configuration is cleared. Return an error message if an attempt
       * is made to clear slot configuration if cards are not pluggable.
       */
      memset (buf, 0, (L7_int32)sizeof(buf));
      if (rc == L7_NOT_SUPPORTED)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_CardDel, cliDisplaySlotHelp(unit, slot));
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, buf, sizeof(buf), pStrErr_base_CardDel2, cliDisplaySlotHelp(unit, slot));
      }
      return cliSyntaxReturnPrompt (ewsContext, buf);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure the administrative mode of slot(s)
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype  const L7_char8  *
* @returns  cliPrompt(ewsContext)
*
* @comments  If slot is full, mode is applied to currently inserted card.
* @comments  If slot is empty, mode will be applied to next inserted card.
* @comments  If a card is disabled, all ports on the device are
*            operationally disabled and behave as "unplugged".
* @comments  If cards are not pluggable, slot adminmode is always enabled
*            and cannot be disabled.
* @comments  Default slot adminmode is "enable".
*
* @mode  Global Config
*
* @cmdsyntax in stacking env:  [no] set slot disable <unit/slot | all>
* @cmdsyntax in non-stacking env:  [no] set slot disable <slot | all>
*
* @cmdhelp  Enable or disable slot administrative mode.
*
* @cmddescript  Indicates whether the slot is enabled or disabled.
*
* @end
*********************************************************************/
const L7_char8 *commandSetSlotDisable(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSlot = 1;
  L7_uint32 unit, slot;
  L7_uint32 unit_id, mode = 0;
  L7_uint32 i, numSlots;
  L7_uint32 ptrSlots[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmp_1[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 tmp_2[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_BOOL first = L7_TRUE, foundSlotNum = L7_FALSE;
  L7_BOOL first_notSupported = L7_TRUE;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgSlotDsbl, cliSyntaxAllSlot());
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag!=L7_EXECUTE_SCRIPT)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /* NOTE: This command is used for both, enabling and disabling slot
   *       adminmode. Default adminmode is `enable`.
   *
   * the `normal` form of command disables adminmode, and
   * the `no` form of command (re)enables adminmode
   */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_DISABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_ENABLE;
  }

  if (strcmp(argv[index+argSlot], pStrInfo_common_All) != 0)
  {
    /* check validity of user input slot */
    if ((rc = cliValidSpecificUnitSlotCheck(argv[index+argSlot], &unit, &slot)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_Invalid_2);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetPrintf (ewsContext, CLIUNIT_X_DOES_NOT_EXIST, (L7_int32)unit);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SlotXDoesntExist, (L7_int32)slot);
        ewsTelnetWrite(ewsContext, buf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* check if specified slot is supported on the specified unit */

    if (usmDbUnitMgrUnitTypeGet(unit, &unit_id) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLIUNIT_X_DOES_NOT_EXIST, (L7_int32)unit);
    }

    if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SlotNoneExist);
    }

    for (i = 0; i < numSlots; i++)
    {
      if (ptrSlots[i] == slot)
      {
        foundSlotNum = L7_TRUE;
        break;
      }
    }

    if (foundSlotNum != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_SlotXDoesntExist, (L7_int32)slot);
    }

    if ((rc = usmDbSlotAdminModeSet(unit, slot, mode)) != L7_SUCCESS)
    {
      memset (buf, 0, (L7_int32)sizeof(buf));
      /* slot adminmode already configured */
      if (rc == L7_ALREADY_CONFIGURED)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_CardAdminAlreadyEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_CardAdminAlreadyDsbld);
        }
      }
      /* slot adminmode cannot be disabled if cards are not pluggable in the slot */
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CardDsblNonPluggable);
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CardDsbl2, cliDisplaySlotHelp(unit, slot));
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_AdminModeSlot);
        ewsTelnetPrintf (ewsContext, cliDisplaySlotHelp(unit, slot));
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_Period);
      }
    }
  }

  /* else, if user input is `all` */
  else
  {
    memset (tmp_1, 0, (L7_int32)sizeof(tmp_1));
    memset (tmp_2, 0, (L7_int32)sizeof(tmp_2));
    rc = usmDbUnitMgrStackMemberGetFirst(&unit);
    while (rc == L7_SUCCESS)
    {
      if (usmDbUnitMgrUnitTypeGet(unit, &unit_id) == L7_SUCCESS)
      {
        if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) == L7_SUCCESS)
        {
          for (i = 0; i < numSlots; i++)
          {
            if ((rc = usmDbSlotAdminModeSet(unit, ptrSlots[i], mode)) != L7_SUCCESS)
            {
              memset (buf, 0, (L7_int32)sizeof(buf));
              /* slot adminmode already configured */
              if (rc == L7_ALREADY_CONFIGURED)
              {
                continue;
              }
              /* slot adminmode cannot be disabled if cards are not pluggable in the slot */
              else if (rc == L7_NOT_SUPPORTED)
              {
                if (first_notSupported == L7_TRUE)
                {
                  osapiSnprintf(buf, sizeof(buf), cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                else
                {
                  osapiSnprintf(buf, sizeof(buf), ", %s", cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                OSAPI_STRNCAT(tmp_1, buf);
                first_notSupported = L7_FALSE;
              }
              else
              {
                if (first == L7_TRUE)
                {
                  osapiSnprintf(buf, sizeof(buf), cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                else
                {
                  osapiSnprintf(buf, sizeof(buf), ", %s", cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                OSAPI_STRNCAT(tmp_2, buf);
                first = L7_FALSE;
              }
            }
          }
        }
      }

      rc = usmDbUnitMgrStackMemberGetNext(unit, &unit);
    }

    /* adminmode already configured on all slots */
    if ((first_notSupported == L7_TRUE) && (first == L7_TRUE))
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CardAdminAlreadyEnbldAll);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CardAdminAlreadyDsbldAll);
      }
    }

    if (first_notSupported != L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrErr_base_CardDsbl);
      ewsTelnetWrite(ewsContext, tmp_1);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_Period);

    }

    if (first != L7_TRUE)
    {
      if (first_notSupported != L7_TRUE)
      {
        cliSyntaxNewLine(ewsContext);
      }
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_AdminModeSlot);
      ewsTelnetWrite(ewsContext, tmp_2);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_Period);

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
* @purpose  Configure the power mode of slot(s)
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype  const L7_char8  *
* @returns  cliPrompt(ewsContext)
*
* @comments  This command determines whether or not power will be
*            supplied to a card in the slot.
* @comments  If slot is full, power is supplied to currently inserted card.
* @comments  If slot is empty, power will be supplied to next inserted card.
* @comments  If slot cannot be powered down, slot powermode is always enabled
*            and cannot be disabled.
* @comments  Default slot powermode is "enable".
*
* @mode  Global Config
*
* @cmdsyntax in stacking env:  [no] set slot power <unit/slot | all>
* @cmdsyntax in non-stacking env:  [no] set slot power <slot | all>
*
* @cmdhelp  Enable or disable slot power mode.
*
* @cmddescript  Indicates whether slot power mode is enabled or disabled.
*
* @end
*********************************************************************/
const L7_char8 *commandSetSlotPower(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSlot = 1;
  L7_uint32 unit, slot;
  L7_uint32 unit_id, mode = 0;
  L7_uint32 i, numSlots;
  L7_uint32 ptrSlots[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmp_1[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_char8 tmp_2[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_BOOL first = L7_TRUE, foundSlotNum = L7_FALSE;
  L7_BOOL first_notSupported = L7_TRUE;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgSlotPowerOff, cliSyntaxAllSlot());
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag!=L7_EXECUTE_SCRIPT)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /* NOTE: This command is used for both, enabling and disabling slot
   *       powermode. Default powermode is `enable`.
   *
   * the `normal` form of command (re)enables powermode, and
   * the `no` form of command disables powermode
   */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  if (strcmp(argv[index+argSlot], pStrInfo_common_All) != 0)
  {
    /* check validity of user input slot */
    if ((rc = cliValidSpecificUnitSlotCheck(argv[index+argSlot], &unit, &slot)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_Invalid_2);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetPrintf (ewsContext, CLIUNIT_X_DOES_NOT_EXIST, (L7_int32)unit);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_SlotXDoesntExist, (L7_int32)slot);
        ewsTelnetWrite(ewsContext, buf);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* check if specified slot is supported on the specified unit */

    if (usmDbUnitMgrUnitTypeGet(unit, &unit_id) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext,CLIUNIT_X_DOES_NOT_EXIST, (L7_int32)unit);
    }

    if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SlotNoneExist);
    }

    for (i = 0; i < numSlots; i++)
    {
      if (ptrSlots[i] == slot)
      {
        foundSlotNum = L7_TRUE;
        break;
      }
    }

    if (foundSlotNum != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_SlotXDoesntExist, (L7_int32)slot);
    }

    if ((rc = usmDbSlotPowerModeSet(unit, slot, mode)) != L7_SUCCESS)
    {
      memset (buf, 0, (L7_int32)sizeof(buf));
      /* slot powermode already configured */
      if (rc == L7_ALREADY_CONFIGURED)
      {
        if (mode == L7_ENABLE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_CardPowerAlreadyEnbld);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_CardPowerAlreadyDsbld);
        }
      }
      /* slot powermode cannot be disabled if slot cannot be powered down */
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_CardPowerDown);
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_CardPowerDownCheck, cliDisplaySlotHelp(unit, slot));

      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_PowerMode);
        ewsTelnetPrintf (ewsContext, cliDisplaySlotHelp(unit, slot));
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_Period);

      }
    }
  }

  /* else, if user input is `all` */
  else
  {
    memset (tmp_1, 0, (L7_int32)sizeof(tmp_1));
    memset (tmp_2, 0, (L7_int32)sizeof(tmp_2));
    rc = usmDbUnitMgrStackMemberGetFirst(&unit);
    while (rc == L7_SUCCESS)
    {
      if (usmDbUnitMgrUnitTypeGet(unit, &unit_id) == L7_SUCCESS)
      {
        if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) == L7_SUCCESS)
        {
          for (i = 0; i < numSlots; i++)
          {
            if ((rc = usmDbSlotPowerModeSet(unit, ptrSlots[i], mode)) != L7_SUCCESS)
            {
              memset (buf, 0, (L7_int32)sizeof(buf));
              /* slot powermode already configured */
              if (rc == L7_ALREADY_CONFIGURED)
              {
                continue;
              }
              /* slot powermode cannot be disabled if slot cannot be powered down */
              else if (rc == L7_NOT_SUPPORTED)
              {
                if (first_notSupported == L7_TRUE)
                {
                  osapiSnprintf(buf, sizeof(buf), cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                else
                {
                  osapiSnprintf(buf, sizeof(buf), ", %s", cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                OSAPI_STRNCAT(tmp_1, buf);
                first_notSupported = L7_FALSE;
              }
              else
              {
                if (first == L7_TRUE)
                {
                  osapiSnprintf(buf, sizeof(buf), cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                else
                {
                  osapiSnprintf(buf, sizeof(buf), ", %s", cliDisplaySlotHelp(unit, ptrSlots[i]));
                }
                OSAPI_STRNCAT(tmp_2, buf);
                first = L7_FALSE;
              }
            }
          }
        }
      }

      rc = usmDbUnitMgrStackMemberGetNext(unit, &unit);
    }

    /* powermode already configured on all slots */
    if ((first_notSupported == L7_TRUE) && (first == L7_TRUE))
    {
      if (mode == L7_ENABLE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CardPowerAlreadyEnbldAll);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CardPowerAlreadyDsbldAll);
      }
    }

    if (first_notSupported != L7_TRUE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrErr_base_CardNoPower);
      ewsTelnetWrite(ewsContext, tmp_1);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_Period);

    }

    if (first != L7_TRUE)
    {
      if (first_notSupported != L7_TRUE)
      {
        cliSyntaxNewLine(ewsContext);
      }
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_PowerMode);
      ewsTelnetWrite(ewsContext, tmp_2);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_Period);

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}
