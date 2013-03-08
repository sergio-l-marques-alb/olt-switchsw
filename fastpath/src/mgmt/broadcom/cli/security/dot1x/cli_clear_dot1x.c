/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/dot1x/cli_clear_dot1x.c
 *
 * @purpose dot1x clear commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  3/05/2003
 *
 * @author  jflanagan
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "cliutil_dot1x.h"
#include "clicommands_dot1x.h"
#include "clicommands_card.h"
#include "usmdb_counters_api.h"
#include "cli_web_exports.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  clear dot1x entries
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax clear dot1x statistics <slot/port | all>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearDot1xStatistics(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index)
{
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 interface, nextInterface;
  L7_uint32 slot, port, numArgs, u, s, p;
  L7_int32 retVal;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_ClrDot1xPortStats, cliSyntaxInterfaceHelp());
  }

  osapiStrncpySafe(strSlotPort, argv[index+argSlotPort], sizeof(strSlotPort));
  cliConvertToLowerCase(strSlotPort);

  if ( strcmp(strSlotPort, pStrInfo_common_All /* "all" */) == 0 )
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
    }

    if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      cliSetCharInputID(1, ewsContext, argv);
      osapiSnprintf(buf, sizeof(buf),
                    pStrInfo_security_ClrDot1xStats_1, /*"clear dot1x statistics %s" */
                    argv[index+argSlotPort] );

      cliAlternateCommandSet(buf);
      return pStrInfo_security_VerifyClrDot1xPortStats; /* "Are you sure you want to clear the dot1x stats?"*/
    }
    else if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      rc = L7_SUCCESS;
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        while (interface != 0)
        {
          if ( usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS &&
              usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS &&
              cliDot1xInterfaceValidateWithMessage(ewsContext, unit, interface, L7_FALSE ) == L7_SUCCESS )
          {
            rc = usmDbDot1xPortStatsClear(unit, interface);
          }

          if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
          {
            interface = nextInterface;
          }
          else
          {
            interface = 0;
          }
        }
        if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_UnableToClrAllDot1xPortStats);
        }                                                                       /* "Unable to clear all dot1x port stats."*/
      }
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
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
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
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    if ( cliGetCharInputID() == CLI_INPUT_EMPTY  && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      cliSetCharInputID(1, ewsContext, argv);
      osapiSnprintf(buf, sizeof(buf),
                    pStrInfo_security_ClrDot1xStats_1, /*"clear dot1x statistics %s" */
                    argv[index+argSlotPort] );

      cliAlternateCommandSet(buf);
      return pStrInfo_security_VerifyClrDot1xPortStats; /* "Are you sure you want to clear the dot1x stats?"*/
    }
    else if (tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbDot1xPortStatsClear(unit, interface) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_UnableToClrAllDot1xPortStats);
        }
      }
    }
    else
    {
      ewsTelnetWrite( ewsContext, pStrInfo_security_Dot1xPortStatsNotClred); /* "Dot1x port stats not cleared."*/
      cliSyntaxBottom(ewsContext);
    }
  }

  /**************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Purge dot1x authentication history for a specific port
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  clear authentication-history [<u/s/p>] 
*
*
* @cmdhelp
*
*
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandDot1xAuthenticationHistoryClear(EwsContext ewsContext,
                                                       uintf argc,
                                                       const L7_char8 **argv,
                                                       uintf index)
{

  L7_uint32 unit;
  L7_uint32 numArgs;
  L7_uint32 slot, port, iface;
  L7_uint32 argSlotPort = 1;
  L7_RC_t   rc;

  cliSyntaxTop(ewsContext);


  numArgs = cliNumFunctionArgsGet();

  if ((numArgs != 0) && (numArgs != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_Dot1xAuthHistoryLogPurge);
  }

  if(numArgs == 0)
  {
    unit = cliGetUnitId();
    usmDbDot1xAuthHistoryLogPurgeAll(unit); 
    return cliPrompt(ewsContext);
  }

  if(numArgs == 1)
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
      if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS ||
          cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
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
       if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &slot, &port, &iface) != L7_SUCCESS ||
           cliDot1xInterfaceValidate(ewsContext, unit, iface) != L7_SUCCESS)
       {
         return cliPrompt(ewsContext);
       }
    }
  }

  usmDbDot1xAuthHistoryLogInterfacePurge(unit, iface); 

  return cliPrompt(ewsContext);
}




