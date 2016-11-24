/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_show_card.c
*
* @purpose Slot show commands for the cli
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

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "datatypes.h"
#include "cli_web_user_mgmt.h"
#include "clicommands_card.h"
#include "cardmgr_api.h"
#include "sysapi_hpc.h"
#include "usmdb_cardmgr_api.h"
#include "cli_web_exports.h"
#include "usmdb_unitmgr_api.h"

#ifndef _L7_OS_LINUX_
  #include <vxworks_config.h>   /* for DEFAULT_MAX_CONNECTIONS */
#endif /* _L7_OS_LINUX_ */

/*********************************************************************
* @purpose  Display information about the slot(s) in the system
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype  const L7_char8  *
* @returns  cliPrompt(ewsContext)
*
* @comments
*
* @mode  User EXEC
*
* @cmdsyntax in stacking env:  show slot [unit/slot]
* @cmdsyntax in non-stacking env:  show slot [slot]
*
* @cmdhelp  Display information about slot(s).
*
* @cmddescript  Shows Read-Only information about slot(s) in system.
*
* @end
*********************************************************************/
const L7_char8 *commandShowSlot(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSlot = 1;
  L7_uint32 unit, slot, unit_id;
  L7_uint32 card_id_cfg, cx;
  L7_uint32 val, i, numSlots;
  L7_uint32 ptrSlots[L7_MAX_PHYSICAL_SLOTS_PER_UNIT];
  L7_uint32 ux, sx, pluggable, powerdown;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL slotFull = L7_FALSE;
  L7_BOOL foundSlotNum = L7_FALSE;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_common_ShowSlot_1, cliSyntaxSlot());
  }

  /* check validity of input slot */
  else if (cliNumFunctionArgsGet() == 1)
  {
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

    /* call helper routine to display slot details */
    cliUnitSlotDisplay(ewsContext, unit, slot);
  }

  else
  {
    /* display info about physical slots */
    ewsTelnetWriteAddBlanks (1, 0, 14, 0, L7_NULLPTR, ewsContext,pStrInfo_base_AdminPowerCfguredCardPower);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_base_SlotStatusStateStateModelIdPluggableDown);
    ewsTelnetWrite(ewsContext,"\r\n----- ------ ------- ------- -------------------------------- --------- -----\r\n");

    rc = usmDbUnitMgrStackMemberGetFirst(&unit);
    while (rc == L7_SUCCESS)
    {
      if (usmDbUnitMgrUnitTypeGet(unit, &unit_id) == L7_SUCCESS)
      {
        if (usmDbUnitDbEntrySlotsGet(unit_id, &numSlots, ptrSlots) == L7_SUCCESS)
        {
          for (i = 0; i < numSlots; i++)
          {
            ewsTelnetPrintf (ewsContext, "%-6s", cliDisplaySlotHelp(unit, ptrSlots[i]));

            memset (buf, 0, (L7_int32)sizeof(buf));
            if (usmDbSlotIsFullGet(unit, ptrSlots[i], &slotFull) == L7_SUCCESS)
            {
              switch (slotFull)
              {
              case L7_TRUE:
                osapiSnprintf(buf, sizeof(buf), "%-7s", pStrInfo_common_Full);
                break;
              case L7_FALSE:
                osapiSnprintf(buf, sizeof(buf), "%-7s", pStrInfo_common_Empty_1);
                break;
              default:
                osapiSnprintf (buf, sizeof(buf), "%-7s", pStrInfo_common_EmptyString);
                break;
              }
            }
            else
            {
              osapiSnprintf(buf, sizeof(buf), "%-7s", pStrErr_common_Err);
            }
            ewsTelnetWrite(ewsContext, buf);

            memset (buf, 0, (L7_int32)sizeof(buf));
            if (usmDbSlotAdminModeGet(unit, ptrSlots[i], &val) == L7_SUCCESS)
            {
              osapiSnprintf(buf,sizeof(buf),"%-8s",strUtilEnableDisableGet(val,pStrInfo_common_Space));
            }
            else
            {
              osapiSnprintf(buf, sizeof(buf), "%-8s", pStrErr_common_Err);
            }
            ewsTelnetWrite(ewsContext, buf);

            memset (buf, 0, (L7_int32)sizeof(buf));
            if (usmDbSlotPowerModeGet(unit, ptrSlots[i], &val) == L7_SUCCESS)
            {
              osapiSnprintf(buf,sizeof(buf),"%-8s",strUtilEnableDisableGet(val,pStrInfo_common_Space));
            }
            else
            {
              osapiSnprintf(buf, sizeof(buf), "%-8s", pStrErr_common_Err);
            }
            ewsTelnetWrite(ewsContext, buf);

            if (usmDbCardConfiguredCardTypeGet(unit, ptrSlots[i], &card_id_cfg) == L7_SUCCESS)
            {
              memset (stat, 0, (L7_int32)sizeof(stat));
              if (usmDbCardIndexFromIDGet(card_id_cfg, &cx) == L7_SUCCESS)
              {
                memset (buf, 0, (L7_int32)sizeof(buf));
                if (usmDbCardSupportedCardModelIdGet(cx, (L7_uchar8 *)buf) == L7_SUCCESS)
                {
                  osapiSnprintf(stat, sizeof(stat), "%-33s", buf);
                }
                else
                {
                  osapiSnprintf (stat, sizeof(stat), "%-33s", pStrInfo_common_EmptyString);
                }
              }
              else
              {
                osapiSnprintf (stat, sizeof(stat), "%-33s", pStrInfo_common_EmptyString);
              }
              ewsTelnetWrite(ewsContext, stat);
            }

            /* get unit index from unit id */
            if (usmDbUnitIndexFromIDGet(unit_id, &ux) == L7_SUCCESS)
            {
              /* get slot index from slot */
              if (usmDbSlotIndexFromNumGet(ux, ptrSlots[i], &sx) == L7_SUCCESS)
              {
                memset (buf, 0, (L7_int32)sizeof(buf));
                if (usmDbSlotSupportedPluggableGet(ux, sx, &pluggable) == L7_SUCCESS)
                {
                  switch (pluggable)
                  {
                  case L7_TRUE:
                    osapiSnprintf(buf, sizeof(buf), "   %-7s", pStrInfo_common_Yes);
                    break;
                  case L7_FALSE:
                    osapiSnprintf(buf, sizeof(buf), "   %-7s", pStrInfo_common_No);
                    break;
                  default:
                    osapiSnprintf (buf, sizeof(buf), "   %-7s", pStrInfo_common_EmptyString);
                    break;
                  }
                }
                else
                {
                  osapiSnprintf(buf, sizeof(buf), "   %-7s", pStrErr_common_Err);
                }
                ewsTelnetWrite(ewsContext, buf);

                memset (buf, 0, (L7_int32)sizeof(buf));
                if (usmDbSlotSupportedPowerdownGet(ux, sx, &powerdown) == L7_SUCCESS)
                {
                  switch (powerdown)
                  {
                  case L7_TRUE:
                    osapiSnprintf(buf, sizeof(buf), " %-4s", pStrInfo_common_Yes);
                    break;
                  case L7_FALSE:
                    osapiSnprintf(buf, sizeof(buf), " %-4s", pStrInfo_common_No);
                    break;
                  default:
                    osapiSnprintf (buf, sizeof(buf), " %-4s", pStrInfo_common_EmptyString);
                    break;
                  }
                }
                else
                {
                  osapiSnprintf(buf, sizeof(buf), " %-4s", pStrErr_common_Err);
                }
                ewsTelnetWrite(ewsContext, buf);
              }
            }

            cliSyntaxBottom(ewsContext);
          }
        }
      }

      rc = usmDbUnitMgrStackMemberGetNext(unit, &unit);
    }
  } /* end else */

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose Display information about supported card types
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype  const L7_char8  *
* @returns  cliPrompt(ewsContext)
*
* @comments
*
* @mode  User EXEC
*
* @cmdsyntax  show supported cardtype [cardindex]
*
* @cmdhelp  Display supported card types.
*
* @cmddescript  Shows Read-Only information about supported card types.
*
* @end
*********************************************************************/
const L7_char8 *commandShowSupportedCardType(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 cx;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowCardSupportedType);
  }

  else if (cliNumFunctionArgsGet() == 1)
  {
    (void)commandShowSupportedCard(ewsContext, argc, argv, index);
  }

  else
  {
    cx = 0;
    rc = usmDbCardSupportedPhysicalIndexNextGet(&cx);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_CardNoneSupported);
    }

    /* display info about all physical supported cards */

    ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_base_CidCardModelId);
    ewsTelnetWrite(ewsContext,"\r\n--- --------------------------------\r\n");

    while (rc == L7_SUCCESS)
    {
      memset (stat, 0, (L7_int32)sizeof(stat));
      memset (buf, 0, (L7_int32)sizeof(buf));
      osapiSnprintf(stat, sizeof(stat), "%u", cx);
      ewsTelnetPrintf (ewsContext, "%-4s", stat);

      memset (buf, 0, (L7_int32)sizeof(buf));
      memset (stat, 0, (L7_int32)sizeof(buf));
      if (usmDbCardSupportedCardModelIdGet(cx, (L7_uchar8 *)stat) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%-32s", stat);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "%-32s", pStrInfo_common_EmptyString);
      }
      ewsTelnetWrite(ewsContext, buf);

      /* get the next physical supported card */
      rc = usmDbCardSupportedPhysicalIndexNextGet(&cx);
      cliSyntaxBottom(ewsContext);
    }
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Display detailed information about a supported card type
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype  const L7_char8  *
* @returns  cliPrompt(ewsContext)
*
* @comments  This command gets called from commandShowSupportedCardType().
*
* @mode  User EXEC
*
* @cmdsyntax  show supported cardtype <cardindex>
*
* @cmdhelp  Display information about a supported card type.
*
* @cmddescript  Shows Read-Only information for a supported card type.
*
* @end
*********************************************************************/
const L7_char8 *commandShowSupportedCard(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argCX = 1;
  L7_uint32 cx, cx_tmp, card_id;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL phyCard = L7_FALSE;
  L7_RC_t rc;

  /* check validity of input cardindex */
  if (cliConvertTo32BitUnsignedInteger(argv[index+argCX], &cx) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
  }

  cx_tmp = 0;
  rc = usmDbCardSupportedPhysicalIndexNextGet(&cx_tmp);
  while (rc == L7_SUCCESS)
  {
    if (cx_tmp == cx)
    {
      phyCard = L7_TRUE;
      break;
    }
    rc = usmDbCardSupportedPhysicalIndexNextGet(&cx_tmp);
  }

  if ((phyCard != L7_TRUE) || (usmDbCardSupportedTypeIdGet(cx, &card_id) != L7_SUCCESS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, buf, sizeof(buf), pStrErr_base_InvalidCardIdx, (L7_int32)cx);
    ewsTelnetWrite(ewsContext, buf);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_ShowCardIdx);
  }

  /* display information about supported card identified by cardindex */

  cliFormatShortText(ewsContext, pStrInfo_base_CardType);
  ewsTelnetPrintf (ewsContext, "0x%x", card_id);

  memset (buf, 0, (L7_int32)sizeof(buf));
  if (usmDbCardSupportedCardModelIdGet(cx, (L7_uchar8 *)buf) == L7_SUCCESS)
  {
    cliFormatShortText(ewsContext, pStrInfo_common_ModelId);
    ewsTelnetWrite(ewsContext, buf);
  }

  memset (buf, 0, (L7_int32)sizeof(buf));
  if (usmDbCardSupportedCardDescriptionGet(cx, (L7_uchar8 *)buf) == L7_SUCCESS)
  {
    cliFormatShortText(ewsContext, pStrInfo_base_CardDesc);
    cliFormatShortStringDisplay(ewsContext, buf);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}
