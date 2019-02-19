/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cliutil_card.c
*
* @purpose   Helper routines for slot cli
*
* @component User Interface
*
* @comments
*
* @create    07/31/2003
*
* @author    rjindal
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_card.h"
#include "usmdb_unitmgr_api.h"
#include "cli_web_exports.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_slotmapper.h"

#ifdef L7_STACKING_PACKAGE
#include "unitmgr_api.h"
#include "clicommands_stacking.h"
#endif

/*********************************************************************
* @purpose  CLI helper routine for the slot syntax string
*
* @param  void
*
* @returns  Slot syntax string
*
* @comments  Returned string reflects STACKING component support.
*
* @end
*********************************************************************/
L7_char8 *cliSyntaxSlot(void)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_UnitSlot);
#else
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_Slot_2);
#endif

  osapiSnprintf(intf_string, sizeof(intf_string), tmp_string);
  return intf_string;
}

/*********************************************************************
* @purpose  Determine unit and slot from user-input
*
* @param  *buf       @b{(input)) pointer to user-input string
* @param  *unit      @b{(output)) pointer to the unit index
* @param  *slot      @b{(output)) pointer to the slot index
* @param  *allUnits  @b{(output)) pointer to boolean value,
*                                 L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_NOT_EXIST
* @returns  L7_NOT_SUPPORTED
*
* @comments  If stacking is supported, valid input format is
*            `unit/slot` or `all`.
* @comments  Else, valid input format is `slot` or `all`.
*
* @end
*********************************************************************/
L7_RC_t cliValidUnitSlotCheck(const L7_char8 * buf, L7_uint32 * unit,
                              L7_uint32 * slot, L7_BOOL * allUS)
{
  L7_uint32 u = 0, s = 0, buf_len;
  L7_char8 tmp_buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  buf_len = (L7_uint32)strlen(buf);
  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  memset (tmp_buf, 0, (L7_int32)sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);
  cliConvertToLowerCase(tmp_buf);

  if (strcmp(tmp_buf, pStrInfo_common_EmptyString) == 0)
  {
    return L7_FAILURE;
  }

  /* check user input for `all` slots */
  if (strcmp(tmp_buf, pStrInfo_common_All) == 0)
  {
    *allUS = L7_TRUE;

  #ifdef L7_STACKING_PACKAGE
    /* get id of the first unit in stack */
    if (usmDbUnitMgrStackMemberGetFirst(&u) != L7_SUCCESS)
    {
      return L7_NOT_EXIST;
    }
  #else
    u = cliGetUnitId();
  #endif

    *unit = u;

    /* get the first slot on this unit */
    if (usmDbSlotFirstGet(*unit, &s) != L7_SUCCESS)
    {
      return L7_NOT_SUPPORTED;
    }

    *slot = s;

    return L7_SUCCESS;
  }

  *allUS = L7_FALSE;
  rc = cliValidSpecificUnitSlotCheck(buf, unit, slot);

  return rc;
}

/*********************************************************************
* @purpose  Determine unit and slot from user-input
*
* @param  *buf   @b{(input)) pointer to user-input string
* @param  *unit  @b{(output)) pointer to the unit index
* @param  *slot  @b{(output)) pointer to the slot index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_NOT_EXIST
* @returns  L7_NOT_SUPPORTED
*
* @comments  If stacking is supported, valid input format is `unit/slot`.
* @comments  Else, valid input format is `slot`.
*
* @end
*********************************************************************/
L7_RC_t cliValidSpecificUnitSlotCheck(const L7_char8 * buf,
                                      L7_uint32 * unit, L7_uint32 * slot)
{
  L7_uint32 buf_len;
  L7_char8 tmp_buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * u, * s;

  buf_len = (L7_uint32)strlen(buf);
  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  memset (tmp_buf, 0, (L7_int32)sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);
  cliConvertToLowerCase(tmp_buf);

  if (strcmp(tmp_buf, pStrInfo_common_EmptyString) == 0)
  {
    return L7_FAILURE;
  }

#ifdef L7_STACKING_PACKAGE

  /* NOTE: Input of type <a/b> only are valid.
   *
   * Check for erroneous input, i.e, of type <a>, <a/b/c> etc.
   *
   * Break input string into tokens delimited by the specified delimiters.
   * Pointer to the next token is saved for every subsequent call
   * until no more tokens exist.
   */
  u = strtok(tmp_buf, pStrInfo_base_Slash);
  s = strtok(L7_NULL, pStrInfo_common_EmptyString);

  if ((u == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(u, unit) != L7_SUCCESS) ||
      (s == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(s, slot) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  if ((*unit < L7_UNITMGR_MIN_UNIT_NUMBER) || (*unit > L7_UNITMGR_MAX_UNIT_NUMBER))
  {
    return L7_ERROR;
  }

  /* check if the unit exists */
  if (cliIsUnitPresent(*unit) != L7_TRUE)
  {
    return L7_NOT_EXIST;
  }

#else
  u = s = L7_NULLPTR;

  if (cliConvertTo32BitUnsignedInteger(tmp_buf, slot) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *unit = cliGetUnitId();
#endif

  if (*slot > L7_MAX_SLOTS_PER_UNIT)
  {
    return L7_ERROR;
  }

  /* check if the slot exists */
  if (usmDbSlotGet(*unit, *slot) != L7_SUCCESS)
  {
    return L7_NOT_SUPPORTED;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display information about a slot in the system
*
* @param  ewsContext  @b{(input)) the context structure
* @param  unit        @b{(input)) the unit index
* @param  slot        @b{(input)) the slot index
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void cliUnitSlotDisplay(EwsContext ewsContext, L7_uint32 unit, L7_uint32 slot)
{
  L7_uint32 card_id_ins, cx;
  L7_uint32 val;
  L7_uint32 unit_id, ux, sx;
  L7_uint32 pluggable ;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL slotFull = L7_FALSE;
  #ifdef L7_CHASSIS
  L7_uint32 logicalSlot, phySlot;
  #else
  L7_uint32 card_id_cfg,powerdown;
  #endif

  cliFormatShortText(ewsContext, pStrInfo_common_SlotId);
  #ifndef L7_CHASSIS
  osapiSnprintf(buf, sizeof(buf), cliDisplaySlotHelp(unit, slot));
  #else
  /* Convert internal slot to logicalSlot */
  if (usmDbSlotMapUSIntToLogSlotGet(unit,slot, &logicalSlot) == L7_SUCCESS)
  {
    (void)usmDbSlotMapPLPhysicalSlotGet(logicalSlot, &phySlot);
    sprintf(buf, "%u",logicalSlot);
  }
  #endif
  ewsTelnetWrite(ewsContext, buf);

  cliFormatShortText(ewsContext, pStrInfo_base_SlotStatus);
  if (usmDbSlotIsFullGet(unit, slot, &slotFull) == L7_SUCCESS)
  {
    switch (slotFull)
    {
    case L7_TRUE:
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Full);
      break;
    case L7_FALSE:
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Empty_1);
      break;
    default:
      osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);
      break;
    }
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, buf);

  cliFormatShortText(ewsContext, pStrInfo_base_StateAdmin);
  if (usmDbSlotAdminModeGet(unit, slot, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_ENABLE:
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbl_1);
      break;
    case L7_DISABLE:
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbl_1);
      break;
    default:
      osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);
      break;
    }
  }
  else
#ifdef L7_CHASSIS
  {osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NotApplicable);}
#else
  {osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);}
#endif
  ewsTelnetWrite(ewsContext, buf);

  cliFormatShortText(ewsContext, pStrInfo_base_StatePower);
  if (usmDbSlotPowerModeGet(unit, slot, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_ENABLE:
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbl_1);
      break;
    case L7_DISABLE:
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbl_1);
      break;
    default:
      osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);
      break;
    }
  }
  else
#ifdef L7_CHASSIS
  {osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NotApplicable);}
#else
  {osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);}
#endif
  ewsTelnetWrite(ewsContext, buf);

  if (usmDbCardInsertedCardTypeGet(unit, slot, &card_id_ins) == L7_SUCCESS)
  {
    if (usmDbCardIndexFromIDGet(card_id_ins, &cx) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%s", pStrInfo_base_CardInserted);

      memset (buf, 0, (L7_int32)sizeof(buf));
      if (usmDbCardSupportedCardModelIdGet(cx, (L7_uchar8 *)buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "   %s", pStrInfo_common_ModelId);
        cliFormatShortText(ewsContext, stat);
        ewsTelnetWrite(ewsContext, buf);
      }

      memset (buf, 0, (L7_int32)sizeof(buf));
      if (usmDbCardSupportedCardDescriptionGet(cx, (L7_uchar8 *)buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "   %s", pStrInfo_base_CardDesc);
        cliFormatShortText(ewsContext, stat);
        cliFormatShortStringDisplay(ewsContext, buf);
      }
    }
  }
  #ifndef L7_CHASSIS
  if (usmDbCardConfiguredCardTypeGet(unit, slot, &card_id_cfg) == L7_SUCCESS)
  {
    if (usmDbCardIndexFromIDGet(card_id_cfg, &cx) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%s", pStrInfo_base_CardCfgured);

      memset (buf, 0, (L7_int32)sizeof(buf));
      if (usmDbCardSupportedCardModelIdGet(cx, (L7_uchar8 *)buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "   %s", pStrInfo_common_ModelId);
        cliFormatShortText(ewsContext, stat);
        ewsTelnetWrite(ewsContext, buf);
      }

      memset (buf, 0, (L7_int32)sizeof(buf));
      if (usmDbCardSupportedCardDescriptionGet(cx, (L7_uchar8 *)buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "   %s", pStrInfo_base_CardDesc);
        cliFormatShortText(ewsContext, stat);
        cliFormatShortStringDisplay(ewsContext, buf);
      }
    }

  }
  #endif
  if (usmDbUnitMgrUnitTypeGet(unit, &unit_id) == L7_SUCCESS)
  {
    /* get unit index from unit id */
    if (usmDbUnitIndexFromIDGet(unit_id, &ux) == L7_SUCCESS)
    {
      /* get slot index from slot */
      if (usmDbSlotIndexFromNumGet(ux, slot, &sx) == L7_SUCCESS)
      {
        cliFormatShortText(ewsContext, pStrInfo_base_Pluggable);
#ifdef L7_CHASSIS
        if (usmDbSlotSupportedPluggableGet(ux, phySlot, &pluggable) == L7_SUCCESS)
#else
        if (usmDbSlotSupportedPluggableGet(ux, sx, &pluggable) == L7_SUCCESS)
#endif
        {
          switch (pluggable)
          {
          case L7_TRUE:
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Yes);
            break;
          case L7_FALSE:
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_No);
            break;
          default:
            osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);
            break;
          }
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, buf);
#ifndef L7_CHASSIS
        cliFormatShortText(ewsContext, pStrInfo_base_PowerDown);
        if (usmDbSlotSupportedPowerdownGet(ux, sx, &powerdown) == L7_SUCCESS)
        {
          switch (powerdown)
          {
          case L7_TRUE:
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Yes);
            break;
          case L7_FALSE:
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_No);
            break;
          default:
            osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);
            break;
          }
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, buf);
#endif
      }
    }
  }

  cliSyntaxBottom(ewsContext);
}

/*********************************************************************
* @purpose  CLI helper routine for slot display format
*
* @param  unit  @b{(input)) the unit index
* @param  slot  @b{(input)) the slot index
*
* @returns  Slot display string
*
* @comments  Returned string reflects STACKING component support.
*
* @end
*********************************************************************/
L7_char8 *cliDisplaySlotHelp(L7_uint32 unit, L7_uint32 slot)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(tmp_string, sizeof(tmp_string), "%u/%u", unit, slot);
#else
  osapiSnprintf(tmp_string, sizeof(tmp_string), "%u", slot);
#endif

  osapiSnprintf(intf_string, sizeof(intf_string), tmp_string);
  return intf_string;
}

/*********************************************************************
* @purpose  CLI helper routine for all-slots syntax string
*
* @param  void
*
* @returns  All-slot syntax string
*
* @comments  Returned string reflects STACKING component support.
*
* @end
*********************************************************************/
L7_char8 *cliSyntaxAllSlot(void)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_UnitSlotOrAll);
#else
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_SlotOrAll_1);
#endif

  osapiSnprintf(intf_string, sizeof(intf_string), tmp_string);
  return intf_string;
}

/*********************************************************************
* @purpose  Check presence/absence of the stacking package
*
* @param  void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL cliIsStackingSupported(void)
{
#ifdef L7_STACKING_PACKAGE
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}

/*********************************************************************
* @purpose  CLI helper routine to check if a unit exists in the stack
*
* @param  unit  @b{(input)) the unit index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL cliIsUnitPresent(L7_uint32 unit)
{
  usmDbUnitMgrStatus_t status;

  if (unit == L7_LOGICAL_UNIT)
  {
    return L7_TRUE;
  }

  if (usmDbUnitMgrUnitStatusGet(unit, &status) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

