/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_clear.c
 *
 * @purpose clear commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  08/09/2000
 *
 * @author  srikrishnas
 *
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_card.h"
#include "cli_web_mgr_api.h"
#include "usmdb_counters_api.h"
#include "usmdb_common.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_radius_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snooping_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"
#include "ews.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_user_mgmt_api.h"


/*********************************************************************
*
* @purpose  clear igmp snooping entries
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
* @notes none
*
* @cmdsyntax [Privileged EXEC]> clear igmpsnooping
*
* @cmdhelp Clear all IGMP snooping entries.
*
* @cmddescript
*   This command causes the all igmp entries to be cleared.
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIgmpsnooping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, numArgs;
  L7_uint32 unit;
  L7_uchar8 family = L7_AF_INET;
  cliSyntaxTop(ewsContext);

  /* Below code checks for the command-type . The variable 'commType'
     is a newly introduced 'enum' of type 'COMMAND_TYPE_t'  defined in ews_clix.h.
     The enum is added as a new variable to the structure 'ewsContext'.*/

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if (strcmp(argv[1], pStrInfo_common_Igmpsnooping) == 0)
  {
    family = L7_AF_INET;
  }
  else
  {
    family = L7_AF_INET6;
  }

  if ( numArgs != 0 )
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CLEARIGMPSNOOPING(family));
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
  {
    cliSetCharInputID(U_IDX, ewsContext, argv);

    cliAlternateCommandSet(CLISYNTAX_CLEARIGMPSNOOPING_COMMAND(family));       /*  clear igmpsnooping */
    return CLIVERIFY_CLEAR_ALL_IGMP_SNOOPING_ENTRIES_YN(family);  /* Are you sure you want to clear all IGMP snooping entries? (y/n) */
  }
  else if ( cliGetCharInputID() == 1 || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbSnoopEntriesFlush(unit, family);
        ewsTelnetWrite( ewsContext, CLIIGMP_SNOOPING_ENTRIES_CLEARED(family));  /*  IGMP Snooping Entries Cleared. */
        cliSyntaxBottom(ewsContext);
      }
    }
    else
    {
      ewsTelnetWrite( ewsContext, CLIIGMP_SNOOPING_ENTRIES_NOT_CLEARED(family));  /*  IGMP Snooping Entries Not Cleared. */
      cliSyntaxBottom(ewsContext);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  clear the traplog
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
* @notes none
*
* @cmdsyntax [Privileged EXEC]> clear traplog
*
* @cmdhelp Clear the trap log.
*
* @cmddescript
*   This command causes the traplog to be erased.
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearTraplog(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, numArgs, unit;

  cliSyntaxTop(ewsContext);

  /* Below code checks for the command-type . The variable 'commType'
     is a newly introduced 'enum' of type 'COMMAND_TYPE_t'  defined in ews_clix.h.
     The enum is added as a new variable to the structure 'ewsContext'.*/

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ClrTrapLog_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
  {
    cliSetCharInputID(U_IDX, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_ClrTrapLogCmd);   /*  clear traplog */
    return pStrInfo_base_VerifyClrTrapLogYesNo;    /*  Are you sure you want to clear the traplog? (y/n)  */
  }
  else if ( cliGetCharInputID() == 1 || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbCtrlClearTrapLogSwSet(unit);
        ewsTelnetWrite( ewsContext, pStrInfo_base_TrapLogClred);   /* Trap Log Cleared. */
        cliSyntaxBottom(ewsContext);
      }
    }
    else
    {
      ewsTelnetWrite( ewsContext, pStrErr_base_TrapLogNotClred);   /* Trap Log Not Cleared. */
      cliSyntaxBottom(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  reset a port-channel (lag) to the default (clears port-channel)
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
* @notes none
*
* @cmdsyntax  [Privileged EXEC]> clear port-channel
*
* @cmdhelp Reset the port-channel (lag) to default values.
*
* @cmddescript Restore the port-channel (lag) information to the default values.
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearPortChannel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intIfNum, unit;
  L7_uint32 extIfNum, numArgs;
  L7_RC_t lrc;

  cliSyntaxTop(ewsContext);

  /* Below code checks for the command-type . The variable 'commType'
     is a newly introduced 'enum' of type 'COMMAND_TYPE_t'  defined in ews_clix.h.
     The enum is added as a new variable to the structure 'ewsContext'.*/

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ClrLag_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
  {
    cliSetCharInputID(U_IDX, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_ClrLagCmd);    /* clear port-chanel */
    return pStrInfo_base_VerifyClrAllLagsYesNo;      /* Are you sure you want to clear all port-channels? (y/n)  */
  }

  else if ( cliGetCharInputID() == 1 || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* get and check validity of ifIndex then get and display information
         * for the first port-channel (LAG) member */
        lrc = usmDbDot3adIfIndexGetFirst(unit, &extIfNum);
        while ( lrc == L7_SUCCESS )
        {
          if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
          {
            usmDbDot3adRemoveSet(unit, intIfNum);   /* don't care if it's configured just delete it*/
            lrc = usmDbDot3adIfIndexGetNext(unit, extIfNum, &extIfNum);
          }
        }
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_LagClred);    /*   Port-channel (LAG) Cleared. */
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_LagNotReset);   /*  Port-channel (LAG) Not Reset! */
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  reset VLAN information to factory defaults
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
* @notes none
*
* @cmdsyntax [VLAN]>clear vlan
*
* @cmdhelp Reset all VLAN Configurations to Default.
*
* @cmddescript
*   This function allows you to reset VLAN configuration parameters
*   to those default parameters provided by the factory.
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, numArgs, unit, val;

  cliSyntaxTop(ewsContext);

  /* Below code checks for the command-type . The variable 'commType'
     is a newly introduced 'enum' of type 'COMMAND_TYPE_t'  defined in ews_clix.h.
     The enum is added as a new variable to the structure 'ewsContext'.*/

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {         /*check to for invalid # of arguments */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ClrVlan_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbVlanResetToDefaultSet(unit, 2);
        /* apply_resetVlancfg(1,0,1); */
        cliWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, pStrInfo_base_VlansRestored);    /*    VLAN's Restored!  */

        while((usmDbDot1qNumVlansGet(unit, &val) == L7_SUCCESS) && (val != 1))
        {
          osapiSleep(2);   /*Sleep and check until all vlans are restored */
          cliWrite(".");
        }

      }
    }
    else
    {                                                                     /* no */
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_VlansNotRestored);    /*   VLAN's Not Restored! */
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(1, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_ClrVlanCmd);   /* clear vlan */                            /*hard coded in command name for now, #define later  */
    return pStrInfo_base_VerifyRestoreAllVlansToDeflYesNo;    /* Are you sure you want to restore all VLANs to default? (y/n) */
  }
}

/*********************************************************************
*
* @purpose  clear the switch configuration to factory defaults
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
* @notes none
*
* @cmdsyntax  [Privileged EXEC]> clear config
*
* @cmdhelp Reset configuration data to factory defaults.
*
* @cmddescript
*   You can reset the configuration to factory default values without
*   powering off the switch. The factory defaults are not restored until
*   the switch is reset. The switch is automatically reset when this
*   command is processed. You are prompted to confirm that you want the
*   reset to proceed.
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearConfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, numArgs, unit;

  /* Below code checks for the command-type . The variable 'commType'
     is a newly introduced 'enum' of type 'COMMAND_TYPE_t'  defined in ews_clix.h.
     The enum is added as a new variable to the structure 'ewsContext'.*/

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {           /*check to for invalid # of arguments */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ClrCfg_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        cliWebCmdLoggerEntryAdd(pStrErr_base_ClrCfgCmd, FD_WEB_DEFAULT_MAX_CONNECTIONS+1);
        rc = usmDbResetConfigActionSet(unit, 3);
        /*   LOG_ERROR(USER_RESET);  You don't want to reset the Hardware */
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CfgClred);     /*   Configuration Cleared!  */
      }
    }
    else
    {                                                                     /* no */
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CfgNotClred);    /*Configuration Not Cleared!" */
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPrompt(ewsContext);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /*  bzero(cliCommon[cliUtil.handleNum].prompt,sizeof(cliCommon[cliUtil.handleNum].prompt));
         osapiSnprintf(cliCommon[cliUtil.handleNum].prompt, sizeof(cliCommon[cliUtil.handleNum].prompt, "%s%s", cliUtil.systemPrompt, CLIPRIVILEDGEUSEREXECMODE_PROMPT); */
      return " ";
    }

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }
  else
  {
    cliSyntaxTop(ewsContext);
    cliSetCharInputID(1, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_ClrCfgCmd);            /*  clear config */
    return pStrInfo_base_VerifyClrCfgYesNo;             /* Are you sure you want to clear the configuration? (y/n) */
  }
}

/*********************************************************************
*
* @purpose  clear superuser password
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
* @notes not tested
*
* @cmdsyntax [Privileged EXEC]> clear pass
*
* @cmdhelp Reset read/write password to factory default.
*
* @cmddescript
*   You can reset user passwords to factory default values without powering
*   off the switch. The factory defaults are not restored until the switch
*   is reset. The switch is automatically reset when this command is processed.
*   You are prompted to confirm that you want the reset to proceed.
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearPass(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_int32 tempInt = 0, numArgs;
  L7_char8 buf[1];

  cliSyntaxTop(ewsContext);

  /* Below code checks for the command-type . The variable 'commType'
     is a newly introduced 'enum' of type 'COMMAND_TYPE_t'  defined in ews_clix.h.
     The enum is added as a new variable to the structure 'ewsContext'.*/

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {                /*check to for invalid # of arguments */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_ClrPass_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if ( tolower(cliGetCharInput()) == 'y')
    {           /* yes */
      /* set_default_passwords(); */
      buf[0] = L7_NULL;
      tempInt = 0;
      for ( ; tempInt < L7_MAX_LOGINS; tempInt++ )
      {
        usmDbPasswordSet(unit, tempInt, buf, L7_FALSE);
      }

      ewsTelnetWrite(ewsContext, pStrInfo_base_PasswdsReset);  /*   Passwords Reset!  */
    }
    else
    {                                                                     /* no */
      ewsTelnetWrite(ewsContext, pStrErr_base_PasswdsNotReset);  /*   Passwords Not Reset! */
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(1, ewsContext, argv);
    cliAlternateCommandSet(pStrErr_base_ClrPassCmd);                     /* clear pass */
    return pStrInfo_base_VerifyResetAllPasswdsYesNo;  /* Are you sure you want to reset all passwords? (y/n) */
  }
}

/*********************************************************************
* @purpose  clear statistic counters for a specified port or all the ports
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
* @cmdsyntax in stacking env:  [User Exec]>clear counters [<unit/slot/port> | all]
* @cmdsyntax in non-stacking env:  [User Exec]>clear counters [<slot/port> | all]
*
* @cmdhelp Reset certain stats to 0.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandClearCounters(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_int32 intIfNum, slot, port;
  L7_uint32 interface, nextInterface;
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 argSlotPort = 1;
  L7_int32 retVal;
  L7_uint32 unit, numArgs;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  if (( numArgs != 1 ) && (numArgs != 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ClrCounters_2, cliSyntaxInterfaceHelp());
  }

  if(numArgs == 1)
  {
    if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)        /* all */
    {
      /* get switch ID based on presence/absence of STACKING package */
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox_1);
        }

        if (cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
        {
          osapiSnprintf(stat, sizeof(stat), "%s %s", pStrErr_base_ClrCountersCmd, argv[index+argSlotPort]);    /*  clear stats port */
          cliAlternateCommandSet(stat);
          cliSetCharInputID(U_IDX, ewsContext, argv);
          return pStrInfo_base_VerifyClrAllPortStatsYesNo;       /* Are you sure you want to clear ALL port stats? (y/n) */
        }

        else if (tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
        {

          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            while (interface)
            {
              usmDbIntfStatReset(unit, interface);
              if (L7_SUCCESS != usmDbValidIntIfNumNext(interface, &nextInterface))
              {
                interface = 0;
              }
              else
              {
                interface = nextInterface;
              }
            }
            ewsTelnetWrite(ewsContext, pStrInfo_base_AllPortStatsClred);     /*  ALL Port Stats Cleared. */
          }
        }

        else
        {
          ewsTelnetWrite( ewsContext, pStrErr_base_PortStatsNotClred);   /*  Port Stats Not Cleared. */
        }
        cliSyntaxBottom(ewsContext);
      }
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
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
          if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
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
          if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &intIfNum) != L7_SUCCESS)
          {
            return cliPrompt(ewsContext);
          }
        }

        if (usmDbVisibleInterfaceCheck(unit, intIfNum, &retVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
        else
        {
          if (cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
          {
            osapiSnprintf(stat, sizeof(stat), "%s %s", pStrErr_base_ClrCountersCmd, argv[index+argSlotPort]);    /* clear stats port */
            cliAlternateCommandSet(stat);
            cliSetCharInputID(U_IDX, ewsContext, argv);
            return pStrInfo_base_VerifyClrPortStatsYesNo;   /* Are you sure you want to clear the port stats? (y/n) */
          }
          else if (tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
          {
            usmDbIntfStatReset(unit, intIfNum);
            ewsTelnetWrite(ewsContext, pStrInfo_base_PortStatsClred);     /* Port Stats Cleared. */
          }
          else
          {
            ewsTelnetWrite( ewsContext, pStrErr_base_PortStatsNotClred);
          }                                                            /*  Port Stats Not Cleared. */

          cliSyntaxBottom(ewsContext);
        }
      }
    }
  }
  else if(numArgs == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      cliSetCharInputID(U_IDX, ewsContext, argv) ;

      cliAlternateCommandSet(pStrErr_base_ClrCountersCmd);      /*    clear stats switch  */
      return pStrInfo_base_VerifyClrSwitchStatsYesNo;   /* Are you sure you want to clear the switch stats? (y/n) */
    }
    else if ( cliGetCharInputID() == 1 || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          usmDbMgtSwitchintIfNumGet(unit, &interface);
          usmDbIntfStatReset(unit, interface);
          (void)usmDbRadiusStatsClear(unit);
          ewsTelnetWrite(ewsContext, pStrInfo_base_SwitchStatsClred);   /* Switch Stats Cleared. */
        }
      }
      else
      {
        ewsTelnetWrite( ewsContext, pStrErr_base_SwitchStatsNotClred);  /* Switch Stats Not Cleared. */
      }
      cliSyntaxBottom(ewsContext);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To clear the host name from host name-to-address cache
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax for normal command:  clear host {name| all}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearHostname(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_char8  hostName[DNS_DISPLAY_DOMAIN_NAME_SIZE_MAX];
  L7_uint32 ipAddr = 0;
  L7_RC_t   rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if (usmDbHostNameValidate((L7_uchar8 *)argv[index + 1]) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_InvalidHostName);
  }
  bzero(hostName, sizeof(hostName));
  osapiStrncpySafe(hostName, argv[index+1], sizeof(hostName));
  rc = usmDbDnsIpAddressValidate(hostName, &ipAddr);
  if (rc != L7_FAILURE)
  {
    /* Invalid HostName */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_InvalidHostName);
  }

  /*********check for * to remove all hosts in the table  *************/
  if (strcmp(hostName, "all") == 0)
  {
    if (usmDbDNSClientCacheFlush() != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
             ewsContext, pStrErr_base_UnableToRemoveAll);
    }
  }
  else
  {
    /*********check for host name *************/
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbDNSClientCacheHostFlush(hostName) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
               ewsContext, pStrErr_base_UnableToRemoveEntry);
      }
    }
  }
  /****************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

