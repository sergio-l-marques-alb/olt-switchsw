/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_pml.c
 *
 * @purpose vlan show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 *
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "clicommands.h"
#include "clicommands_card.h"
#include "usmdb_pml_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
#include "ews.h"

/*********************************************************************
*
* @purpose  Display Port MAC Locking (port-security) information for global configuration
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  show port-security
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPortSecurity(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 numArg;         /* New variable Added */
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort=1;
  static L7_uint32 interface = 0;
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL all = L7_FALSE;
  L7_uint32 lineCount = 0;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {                                                                     /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if ( (numArg<0) || (numArg>1)    )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_ShowPortSecurity_1);
    }
    if (numArg == 0)
    {
      rc = usmDbPmlAdminModeGet(unit, &val);
      if (rc == L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_PortSecurityAdministrationMode);
        return cliSyntaxReturnPrompt(ewsContext,strUtilEnabledDisabledGet(val,pStrInfo_common_Dsbld));
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_switching_GetPortSecurityInfo);
      }

    }
    else if(numArg == 1)
    {
      if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
      {
        all = L7_TRUE;

        if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }

        /* get switch ID based on presence/absence of STACKING package */
        unit = cliGetUnitId();
        if (unit == 0)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
        }
      }
      else
      {
        if (cliIsStackingSupported() == L7_TRUE)
        {
          if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
          {
            if (rc == L7_ERROR)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
              ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
            }
            else if (rc == L7_NOT_EXIST)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
            }
            else if (rc == L7_NOT_SUPPORTED)
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
            }
            else
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
            }

            return cliSyntaxReturnPrompt (ewsContext, "");
          }

          /* Get interface and check its validity */
          if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
          }
        }
        else
        {
          /* NOTE: No need to check the value of `unit` as
           *       ID of a standalone switch is always `U_IDX` (=> 1).
           */
          unit = cliGetUnitId();
          if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS)
          {
            return cliPrompt(ewsContext);
          }
        }

        if (usmDbPmlIsValidIntf(unit, interface) != L7_TRUE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
    }

    ewsTelnetWriteAddBlanks (1, 0, 9, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_AdminDynStaticViolation);
    ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_IntfModeLimitLimitTrapMode);
    ewsTelnetWrite(ewsContext,"\r\n------  -------  ----------  ---------  ----------");
  }

  while ((interface > 0) && (lineCount < CLI_MAX_SCROLL_LINES-6))
  {
    if (usmDbPmlIsValidIntf(unit, interface) == L7_TRUE)
    {
      rc = usmDbUnitSlotPortGet(interface, &unit, &slot, &port);
      if (rc == L7_SUCCESS)
      {
        /* Interface */
        ewsTelnetPrintf (ewsContext, "\r\n%-8s", cliDisplayInterfaceHelp(unit, slot, port));
        lineCount++;
      }
      /* Admin Mode */
      rc = usmDbPmlIntfModeGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),"%-9s",strUtilEnabledDisabledGet(val,pStrInfo_common_Dsbld));

      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_Dsbld);
      }
      ewsTelnetWrite(ewsContext,stat);

      /* Dynamic Limit */
      rc = usmDbPmlIntfDynamicLimitGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-12u", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-12s", "----");
      }
      ewsTelnetWrite(ewsContext,stat);

      /* Static Limit */
      rc = usmDbPmlIntfStaticLimitGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-11u", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-11s", "----");
      }
      ewsTelnetWrite(ewsContext,stat);

      /* Violation Mode */
      rc = usmDbPmlIntfViolationTrapModeGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),"%-9s",strUtilEnabledDisabledGet(val,pStrInfo_common_Dsbld));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_Dsbld);
      }
      ewsTelnetWrite(ewsContext,stat);

    }

    if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) != 0)
    {
      interface = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {
      if (usmDbValidIntIfNumNext(interface, &interface) != L7_SUCCESS)
      {
        interface = 0;
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
  }                                                                       /* end of while  */

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  osapiSnprintf(cmdBuf, sizeof(cmdBuf), pStrInfo_switching_ShowPortSecurity_2, argv[index+argSlotPort]);
  cliAlternateCommandSet(cmdBuf);

  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose  Display Port MAC Locking (port-security) information for an interface / all interfaces
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  show port-security <all | inteterface>
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowPortSecurityInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 numArg;         /* New variable Added */
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort=1;
  L7_BOOL all = L7_FALSE;
  L7_uint32 interface = 0;
  L7_uint32 nextInterface = 0;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowPortSecurityIntf, cliSyntaxInterfaceHelp());
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
  {
    all = L7_TRUE;

    if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }

    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }
  }

  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
        }
        else if (rc == L7_NOT_EXIST)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
        }
        else if (rc == L7_NOT_SUPPORTED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
        }

        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    if (usmDbPmlIsValidIntf(unit, interface) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 9, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_AdminDynStaticViolation);
  ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_IntfModeLimitLimitTrapMode);
  ewsTelnetWrite(ewsContext,"\r\n------  -------  ----------  ---------  ----------");

  while (interface)
  {
    if (usmDbPmlIsValidIntf(unit, interface) == L7_TRUE)
    {
      rc = usmDbUnitSlotPortGet(interface, &unit, &slot, &port);
      if (rc == L7_SUCCESS)
      {

        /* Interface */
        ewsTelnetPrintf (ewsContext, "\r\n%-8s", cliDisplayInterfaceHelp(unit, slot, port));
      }
      /* Admin Mode */
      rc = usmDbPmlIntfModeGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        if (val == L7_ENABLE)
        {
          osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_Enbld);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_Dsbld);
      }
      ewsTelnetWrite(ewsContext,stat);

      /* Dynamic Limit */
      rc = usmDbPmlIntfDynamicLimitGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-12u", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-12s", "----");
      }
      ewsTelnetWrite(ewsContext,stat);

      /* Static Limit */
      rc = usmDbPmlIntfStaticLimitGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-11u", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-11s", "----");
      }
      ewsTelnetWrite(ewsContext,stat);

      /* Violation Mode */
      rc = usmDbPmlIntfViolationTrapModeGet(unit, interface, &val);
      if (rc == L7_SUCCESS)
      {
        if (val == L7_ENABLE)
        {
          osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_Enbld);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_Dsbld);
      }
      ewsTelnetWrite(ewsContext,stat);

      if (!all)
      {
        interface = 0;
      }
      else
      {
        if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
        {
          interface = nextInterface;
        }
        else
        {
          interface = 0;
        }
      }
    }
    else
    {
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
      }
      else
      {
        interface = 0;
      }
    }
  }                                                                        /* end of while  */

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Display the dynamically locked entries on an interface
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  show port-security dynamic <interface>
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowPortSecurityDynamic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 numArg;          /* New variable Added */
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort=1;
  L7_uint32 i;
  L7_uint32 lineCount;
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  static L7_BOOL entryFound;
  static L7_uint32 interface;
  static L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  static L7_ushort16 vlanId;
  L7_uint32 learned;
  static L7_BOOL firstTime = L7_TRUE;
  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowPortSecurityDyn_1, cliSyntaxInterfaceHelp());
  }
  else
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {                                                                        /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        firstTime = L7_TRUE;
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      entryFound = L7_FALSE;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
        {
          if (rc == L7_ERROR)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
          }
          else if (rc == L7_NOT_EXIST)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
          }
          else if (rc == L7_NOT_SUPPORTED)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
          }

          return cliSyntaxReturnPrompt (ewsContext, "");
        }

        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
        }
      }
      else
      {
        /* NOTE: No need to check the value of `unit` as
         *       ID of a standalone switch is always `U_IDX` (=> 1).
         */
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }

      if (usmDbPmlIsValidIntf(unit, interface) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      memset (stat, 0,sizeof(stat));
      rc = usmDbUnitSlotPortGet(interface, &unit, &slot, &port);
      learned = 0;
      if (usmDbPmlIntfDynamicEntryGetFirst(0, interface, (L7_enetMacAddr_t *)mac, &vlanId) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_NoDynMacAddrPresent);
        firstTime = L7_TRUE;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      else
      {
        entryFound = L7_TRUE;
      }
    }
  }
  if ((entryFound == L7_TRUE) &&(usmdbPmlGetDynamicLearnedAddrCount(unit,interface,&learned)==L7_SUCCESS )
      &&(firstTime == L7_TRUE) )
  {
    osapiSnprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_NumOfDynMacAddresLearned, learned);
    ewsTelnetWrite(ewsContext,stat);
  }

  if(firstTime == L7_TRUE)
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf) ;
    ewsTelnetWriteAddBlanks (1, 0, 1, 3, L7_NULLPTR, ewsContext,pStrInfo_switching_DynallyLearnedMacAddrVlanId);
    ewsTelnetWrite(ewsContext,"\r\n ------------------------------- \t    -------\r\n");
    firstTime = L7_FALSE;
  }

  lineCount = 0;
  while ((lineCount < (CLI_MAX_SCROLL_LINES-5)) && (entryFound == L7_TRUE))
  {
    learned++;
    lineCount++;
    ewsTelnetPrintf (ewsContext, " %02X:%02X:%02X:%02X:%02X:%02X\t\t\t%11u",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], vlanId);
    cliSyntaxBottom(ewsContext);

    if (usmDbPmlIntfDynamicEntryGetNext(unit, interface, (L7_enetMacAddr_t *)mac, &vlanId) == L7_SUCCESS)
    {
      entryFound = L7_TRUE;
    }
    else
    {
      entryFound = L7_FALSE;
    }
  }
  if( entryFound == L7_FALSE )
  {
    firstTime = L7_TRUE;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
  for (i=1; i<argc; i++)
  {
    OSAPI_STRNCAT(cmdBuf, " ");
    OSAPI_STRNCAT(cmdBuf, argv[i]);
  }
  cliAlternateCommandSet(cmdBuf);
  return pStrInfo_common_Name_2;     /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose  Display the statically locked addresses on an interface
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  show port-security static <interface>
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPortSecurityStatic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 numArg;          /* New variable Added */
  L7_uint32 interface = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort=1;
  L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 vlanId;
  L7_uint32 staticCount = 0;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowPortSecurityStatic_1, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if (usmDbPmlIsValidIntf(unit, interface) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  rc = usmDbUnitSlotPortGet(interface, &unit, &slot, &port);
  if( (usmdbPmlGetStaticLearnedAddrCount(unit,interface,&staticCount)==L7_SUCCESS )
     && ( staticCount != 0 ) )
  {
    osapiSnprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_NumOfStaticMacAddresCfgured, staticCount);
    ewsTelnetWrite(ewsContext,stat);
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_NoStaticMacAddrPresent);
  }
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf) ;
  ewsTelnetWriteAddBlanks (1, 0, 1, 3, L7_NULLPTR, ewsContext,pStrInfo_switching_StaticallyCfguredMacAddrVlanId);
  ewsTelnetWrite(ewsContext,"\r\n --------------------------------- \t\t-------\r\n");

  rc = usmDbPmlIntfStaticEntryGetFirst(unit, interface, (L7_enetMacAddr_t *)mac, &vlanId);
  while(rc == L7_SUCCESS)
  {

    ewsTelnetPrintf (ewsContext, " %02X:%02X:%02X:%02X:%02X:%02X \t\t\t    %11u",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], vlanId);
    cliSyntaxBottom(ewsContext);
    rc = usmDbPmlIntfStaticEntryGetNext(unit, interface, (L7_enetMacAddr_t *)mac, &vlanId);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");

}
/*********************************************************************
*
* @purpose  Display the last violating mac address on a locked interface
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  show port-security violation <interface>
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowPortSecurityViolation(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 numArg;          /* New variable Added */
  L7_uint32 interface = 0;

  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort=1;
  L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 vlanId;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowPortSecurityViolation_1, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
    }
  }
  else
  {
    /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).
     */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  if (usmDbPmlIsValidIntf(unit, interface) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  memset (stat, 0,sizeof(stat));
  rc = usmDbUnitSlotPortGet(interface, &unit, &slot, &port);

  rc = usmDbPmlIntfLastViolationAddrGet(unit, interface, (L7_enetMacAddr_t *)mac, &vlanId);
  if (rc == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf) ;
    ewsTelnetWriteAddBlanks (1, 0, 1, 3, L7_NULLPTR, ewsContext,pStrInfo_switching_LastViolationMacAddrVlanId);
    ewsTelnetWrite(ewsContext,"\r\n -------------------------- \t -------\r\n");

    return cliSyntaxReturnPrompt (ewsContext," %02X:%02X:%02X:%02X:%02X:%02X \t     %11u",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], vlanId);
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_NoDynMacAddrPresent);
  }
}
