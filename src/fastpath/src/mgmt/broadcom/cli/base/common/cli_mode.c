/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_mode.c
 *
 * @purpose Command mode functions
 *
 * @component user interface
 *
 * @comments
 *
 * @create   18/06/2003
 *
 * @author  Jagdish
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "ews.h"
#include "cli_mode.h"
#include "clicommands.h"
#include "comm_mask.h"
#ifdef L7_BGP_PACKAGE
#include "usmdb_bgp4_api.h"
#include "dhcps_exports.h"
#include "user_manager_exports.h"
#include "usmdb_util_api.h"

#ifdef L7_WIRELESS_PACKAGE
#include "usmdb_wdm_ap_api.h"
#include "usmdb_wdm_ap_profile_api.h"
#include "usmdb_wdm_api.h"
#endif

#endif
#include "cli_web_user_mgmt.h"
#include "usmdb_dhcps_api.h"
#include "clicommands_card.h"
#include "clicommands_tacacs.h"
#include "usmdb_tacacs_api.h"
#include "usmdb_rlim_api.h"

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE
#include "rlim_api.h"
#include "clicommands_loopback.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#include "clicommands_ipv6.h"
#endif /* L7_IPV6_PACKAGE */
#endif /* L7_RLIM_PACKAGE */
#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
#include "usmdb_dhcp6s_api.h"
#endif /* L7_IPV6_PACKAGE */
#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_WIRELESS_PACKAGE
#include "clicommands_wireless.h"
#include "cli_macro_wireless.h"
#include "wireless_defaultconfig.h"
#include "strlib_wireless_cli.h"
#include "usmdb_wdm_network_api.h"
#include "usmdb_wdm_wds_api.h"
#include "usmdb_wdm_dev_loc_api.h"
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "strlib_security_cli.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_web_api.h"
#include "captive_portal_defaultconfig.h"
#endif
#include "usmdb_user_mgmt_api.h"
#include "clicommands_usermgr.h"
#include "usmdb_dot1x_auth_serv_api.h"
#include "log_exports.h"
#include "usmdb_common.h"
#include "usmdb_log_api.h"

/*********************************************************************
*
* @purpose Function to set the mode prompt
*
* @param L7_char8 *tmpPrompt
* @param const L7_char8 **argv
* @param EwsContext ewsContext
* @param EwsCliCommandP menuTree
*
* @returntype const L7_char8*
*
* @returns  cliPrompt(ewsContext)
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliPromptSet(L7_char8 * tmpPrompt, const L7_char8 * * argv,
                             EwsContext ewsContext, EwsCliCommandP menuTree)
{
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  osapiSnprintf(cliCommon[cliUtil.handleNum].prompt, sizeof(cliCommon[cliUtil.handleNum].prompt),
                "%s%s", cliUtil.systemPrompt, tmpPrompt);
  cliSetTree(menuTree);
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose Function to check the common conditions
*
* @param L7_uint32 depth
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param EwsContext ewsContext
* @param EwsCliCommandP menuTree
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext);
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCheckCommonConditions(L7_uint32 depth, L7_char8 * tmpUpPrompt,
                                         L7_uint32 argc,const L7_char8 * * argv, EwsContext ewsContext, EwsCliCommandP menuTree)
{
  if ( argc == 1 && cliStrCaseCmp(pStrInfo_common_Exit, (char *)argv[argc-1], strlen(argv[argc-1])) == L7_SUCCESS)
  {
    return cliPromptSet(tmpUpPrompt, argv, ewsContext, menuTree);
  }
  return NULL;
}

/*********************************************************************
*
* @purpose Action function to check the no form
*
* @param EwsContext ewsContext
*
* @returntype const L7_char8*
*
* @returns NULL - for error case
* @returns prompt
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCheckNoForm(EwsContext ewsContext)
{
  if ( ewsContext->commType == CLI_NO_CMD)
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    return NULL;
  }
  return cliCommon[cliUtil.handleNum].prompt;
}

/*********************************************************************
*
* @purpose Common function to change the mode, when no command specific
*          call is present.
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error case
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCommonPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                EwsContext ewsContext, EwsCliCommandP upMode,
                                EwsCliCommandP currMode)
{
  L7_char8 * prompt;
  if( cliCheckNoForm(ewsContext) == NULL)
  {
    return NULL;
  }
  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,upMode);
  if( prompt != NULL )
  {
    return prompt;
  }
  else if ((strcmp(tmpPrompt, pStrInfo_common_GlobalCfgModePrompt)==0) && mainMenu[cliCurrentHandleGet()] != currMode && argc == 1)
  {
    return cliPromptSet(tmpPrompt,argv, ewsContext,currMode );
  }
  else if ( argc == depth && mainMenu[cliCurrentHandleGet()] != currMode )
  {
    return cliPromptSet(tmpPrompt,argv, ewsContext,currMode );
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to set Line Config Console Prompt.
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error case
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliConsolePrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                EwsContext ewsContext, EwsCliCommandP upMode,
                                EwsCliCommandP currMode)
{
  L7_char8 *prompt;
  if (cliCheckNoForm(ewsContext) == NULL)
  {
    return NULL;
  }

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,upMode);

  if (prompt != NULL)
  {
    return prompt;
  }
  else if ((argc == depth) &&
           (mainMenu[cliCurrentHandleGet()] != currMode))
  {
    EWSLINECONFIGTYPE(ewsContext)= ACCESS_LINE_CONSOLE;
    return cliPromptSet(tmpPrompt, argv, ewsContext, currMode);
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to set Line Config SSH Prompt.
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error case
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliSshPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                EwsContext ewsContext, EwsCliCommandP upMode,
                                EwsCliCommandP currMode)
{
  L7_char8 *prompt;
  if (cliCheckNoForm(ewsContext) == NULL)
  {
    return NULL;
  }

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,upMode);

  if (prompt != NULL)
  {
    return prompt;
  }
  else if ((argc == depth) &&
           (mainMenu[cliCurrentHandleGet()] != currMode))
  {
    EWSLINECONFIGTYPE(ewsContext)= ACCESS_LINE_SSH;
    return cliPromptSet(tmpPrompt, argv, ewsContext, currMode);
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to set Line Config Telnet Prompt.
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error case
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliTelnetPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                EwsContext ewsContext, EwsCliCommandP upMode,
                                EwsCliCommandP currMode)
{
  L7_char8 *prompt;
  if (cliCheckNoForm(ewsContext) == NULL)
  {
    return NULL;
  }

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,upMode);

  if (prompt != NULL)
  {
    return prompt;
  }
  else if ((argc == depth) &&
           (mainMenu[cliCurrentHandleGet()] != currMode))
  {
    EWSLINECONFIGTYPE(ewsContext)= ACCESS_LINE_TELNET;
    return cliPromptSet(tmpPrompt, argv, ewsContext, currMode);
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Function to set the interface mode
*
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @end
*
*********************************************************************/
const L7_char8 *cliInterfacePrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                   L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                   EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 result;
  L7_uint32 interface;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv,
                                                ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));

  if( prompt != NULL )
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }
  else if (argc > depth)
  {
#ifdef L7_ROUTING_PACKAGE

#ifdef L7_RLIM_PACKAGE
#ifdef L7_IPV6_PACKAGE
    if(strcmp(argv[depth], RLIM_INTF_TUNNEL_NAME_PREFIX) == 0)
    {
      return cliTunnelConfigPrompt(argc, pStrInfo_base_TunnelIntfCfgPrompt,
                                   pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
    }
#endif /* L7_IPV6_PACKAGE */
    if (strcmp(argv[depth], RLIM_INTF_LOOPBACK_NAME_PREFIX) == 0)
    {
      return cliLoopbackConfigPrompt(argc, pStrInfo_base_LoopBackIntfCfgPrompt,
                                     pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
    }
#endif /* L7_RLIM_PACKAGE */
#endif /* L7_IPV6_PACKAGE */

    if (argc == depth + 1 && mainMenu[cliCurrentHandleGet()] !=
        cliGetMode(L7_INTERFACE_CONFIG_MODE))
    {
      if( cliCheckNoForm(ewsContext) == NULL)
      {
        return NULL;
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (cliParseEthernetRangeInput(argv[argc-1], &(EWSINTFMASK(ewsContext)),&(EWSINTFTYPEMASK(ewsContext))) != L7_SUCCESS)
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInterfaceRange);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        else 
        {
          L7_INTF_NONZEROMASK(EWSINTFMASK(ewsContext), result);
          if (!result)
          {
            cliSyntaxTop(ewsContext);
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInterfaceRange);
            cliSyntaxBottom(ewsContext);
            return NULL;
          }

          /* Provide first U/S/P for commands not changed to support interface ranges */
          L7_INTF_FLMASKBIT(EWSINTFMASK(ewsContext), interface);
          if ((0 == interface) || (usmDbUnitSlotPortGet(interface, &EWSUNIT(ewsContext), &EWSSLOT(ewsContext), &EWSPORT(ewsContext)) != L7_SUCCESS))
          {
            EWSUNIT(ewsContext) = EWSSLOT(ewsContext) = EWSPORT(ewsContext) = 0;
          }
        }
      }

      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

      osapiSnprintf(buf, sizeof(buf), "%s %s)#", tmpPrompt, argv[argc-1]);
      return cliPromptSet(buf, argv, ewsContext, cliGetMode(L7_INTERFACE_CONFIG_MODE));
    }
    else
    {
      ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
      ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
      return cliPrompt(ewsContext);
    }
  }
  ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
  ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  /* then just return previous prompt */
  return cliPrompt(ewsContext);
}

static const L7_char8 *cliEnablePrompt(const L7_char8 *prompt, L7_uint32 argc,
                                       const L7_char8 **argv, EwsContext ewsContext)
{
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;

  osapiStrncpy(buf, argv[0], sizeof(buf));

  for (i = 1; i < argc; i++)
  {
    osapiStrncat(buf, " ", sizeof(buf));
    osapiStrncat(buf, argv[i], sizeof(buf));
  }

  cliAlternateCommandSet(buf);
  cliSetStringInputID(1, ewsContext, argv);
  cliSetStringPassword();
  return prompt;
}

/* Enable Authentication List capable */

#if 1
const L7_char8 *cliPriviledgeUserPrompt(L7_uint32 depth, L7_char8 *tmpPrompt,
                                        L7_char8 *tmpUpPrompt, L7_uint32 argc, const L7_char8 **argv,
                                        EwsContext ewsContext)
{
  L7_ACCESS_LINE_t     accessLine;
  L7_BOOL              challengeFlag = L7_FALSE;
  L7_uint32            challengeFlags = 0;
  L7_RC_t              rc;
  L7_uchar8            buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uchar8     challengeState[L7_USER_MGR_STATE_FIELD_SIZE + 1];
  /* This needs to be static since it is returned as a prompt */
  static L7_uchar8     challengePhrase[L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH + 1];
  L7_uchar8            commandLog[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8            password[L7_PASSWORD_SIZE];
  L7_uchar8           *stringInput;
  L7_uchar8            userName[L7_LOGIN_SIZE];
  L7_uint32            accessLevel = L7_LOGIN_ACCESS_READ_WRITE, tmpAccessLevel = L7_LOGIN_ACCESS_READ_WRITE;
  L7_uint32            handle        = cliCurrentHandleGet();
  L7_uint32            unit;
  const L7_char8      *prompt;
  lvl7SecurityLevels_t userLevel;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliExamine(ewsContext, argc, argv, 0);


  /*
   * Retrieve the user access level associated with the logged in user name.
   * This is different than the access level for the user name, if any,
   * provided to enter privileged user mode.
   */
  cliWebLoginSessionUserGet(cliLoginSessionIndexGet(), (L7_char8 *)&userName);
#if 0
  if (0 == strcmp(CLI_WEB_LOGIN_SERIAL_USERNAME, userName))
  {
    userLevel = L7_LOGIN_ACCESS_READ_WRITE;
  }
  else
  {
#endif
   
    userLevel = cliUserAccessGet(); /* Get Access Level from the current CLI Handle */
   
#if 0
  }
#endif

#if 0 /* User Level is the current access level of the User,
       * Access Level is what we are requesting, which should always be Level 15 */
  if (userLevel <= L7_LOGIN_ACCESS_READ_ONLY)
  {
    accessLevel = L7_LOGIN_ACCESS_READ_ONLY;
  }
  else
  {
    accessLevel = L7_LOGIN_ACCESS_READ_WRITE;
  }
#endif

  /*
   * Check to see if the "exit" command has been entered.
   */
  prompt = cliCheckCommonConditions(depth, tmpUpPrompt, argc, argv,
                                    ewsContext, cliGetMode(L7_USER_EXEC_MODE));
  if (prompt != NULL)
  {
    /* exiting from privilege mode, reset access level */
    cliCommon[handle].userAccess = userLevel;
    return prompt;
  }

  if (CLI_SERIAL_HANDLE_NUM == handle)
  {
    accessLine = ACCESS_LINE_CONSOLE;
  }
  else if (L7_TRUE == ewsContext->telnet->ssh)
  {
    accessLine = ACCESS_LINE_SSH;
  }
  else
  {
    accessLine = ACCESS_LINE_TELNET;
  }

  /*
   * If in password recovery mode, do not check for credentials if on serial console.
   */
#if 0
  if ((globalStartType == START_OPR_CODE_PASSWD_RECOVERY) &&
      (ACCESS_LINE_CONSOLE == accessLine))
  {
    return cliPromptSet(tmpPrompt, argv, ewsContext, cliGetMode(L7_PRIVILEGE_USER_MODE));
  }
#endif

  /*
   * R/O user is required to authenticate to get to privilege exec mode.  If
   * no authentication is required, reject R/O user.  Otherwise, go ahead and
   * put the R/W user in privilege mode without further ado.
   */

  if (usmDbIsAuthenticationRequired(accessLine, ACCESS_LEVEL_ENABLE) != L7_TRUE)
  {
   if (L7_LOGIN_ACCESS_READ_WRITE > userLevel)
    {
      return cliPrompt(ewsContext);
    }
    else
    {
      cliCommon[handle].userAccess = accessLevel; /* for security */
      return cliPromptSet(tmpPrompt, argv, ewsContext, cliGetMode(L7_PRIVILEGE_USER_MODE));
    }
  }

  /*
   * Get the enable level.  -- For future use when the enable level can be specified.
   */
  if (argc == depth + 1)
  {
    if ((L7_SUCCESS != cliConvertTo32BitUnsignedInteger(argv[argc - 1], &accessLevel)) ||
        (L7_LOGIN_ACCESS_READ_ONLY  > accessLevel) ||
        (L7_LOGIN_ACCESS_READ_WRITE < accessLevel))
    {
      ewsTelnetWrite(ewsContext, "\r\nAn Invalid Integer was entered.");
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  password[0] = '\0';

  if( cliGetStringInputID() == CLI_INPUT_EMPTY)
  {
    osapiSnprintfAddBlanks (0, 0, 0, 2, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Enbl_2);
    cliAlternateCommandSet(buf);
    cliSetStringInputID(1, ewsContext, argv);
    cliSetStringPassword();

    if (L7_TRUE != usmDbIsAuthenticationPromptSupplied(accessLine, ACCESS_LEVEL_ENABLE))
    {
      return cliEnablePrompt(pStrInfo_base_LoginPasswdPrompt, argc, argv, ewsContext);
    }
    return pStrInfo_base_LoginPasswdPrompt;
  }
  else
  {
    stringInput = cliGetStringInput(); /* Will also clear the string input. */

    /* Validate length of password */
    if (strlen(stringInput) >= L7_PASSWORD_SIZE)
    {
#if 1
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_PasswdGiven);
      ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
      return NULL;
#else
      osapiSnprintf(buf, sizeof(buf), CLISYNTAX_INVALIDPASSWORD, L7_PASSWORD_SIZE);
      ewsTelnetWrite(ewsContext, buf);

      /* This is not a challenge so reset the challenge state. */
      memset(challengeState, 0, sizeof(challengeState));

      return cliPrompt(ewsContext);
#endif
    }
    else
    {
      osapiStrncpySafe(password, stringInput, sizeof(password));
    }
  }

  memset(challengePhrase, 0, sizeof(challengePhrase));
  rc = usmDbAuthenticateUserAllowChallenge(unit, userName, password,
                                           L7_USER_MGR_COMPONENT_ID,
                                           &tmpAccessLevel,  accessLine,
                                           ACCESS_LEVEL_ENABLE,
                                           L7_NULLPTR, L7_NULLPTR,
                                           challengeState,
                                           &challengeFlag, challengePhrase,
                                           &challengeFlags);

  if (L7_SUCCESS == rc)       /* authentication succeeded */
  {
    /* Challenge handling is complete so reset the challenge state. */
    memset(challengeState, 0, sizeof(challengeState));

    if (L7_LOGIN_ACCESS_READ_WRITE == accessLevel)
    {
      /* User is authenticated */
      cliCommon[handle].userAccess = accessLevel; /* for security */

      osapiSnprintf(commandLog, sizeof(commandLog),
                    "User %s logged in to enable mode.",
                    userName);
      cliWebCmdLoggerEntryAdd(commandLog, FD_WEB_DEFAULT_MAX_CONNECTIONS + 1);
      return cliPromptSet(tmpPrompt, argv, ewsContext, cliGetMode(L7_PRIVILEGE_USER_MODE));
    }
    else
    {
      ewsTelnetWrite(ewsContext, "Credentials presented do not have access to privileged mode.\r\n");
    }
  }
  else if (L7_FAILURE == rc)
  {
    if (L7_TRUE == challengeFlag) /* challenge response required */
    {
      /* Handle challenge */
      return cliEnablePrompt(challengePhrase, argc, argv, ewsContext);
    }
    else                      /* authentication was denied */
    {
      ewsTelnetWrite(ewsContext, pStrErr_base_PasswdGiven);

      /* Challenge handling is complete so reset the challenge state. */
      memset(challengeState, 0, sizeof(challengeState));
    }
  }
  else                        /* failure in authentication process */
  {
    /* Challenge handling is complete so reset the challenge state. */
    memset(challengeState, 0, sizeof(challengeState));
    ewsTelnetWrite(ewsContext, "Authentication failed.\r\n");
  }

  return cliPrompt(ewsContext);
}

#else
/*********************************************************************
* DEPRECATED ******** DEPRECATED ********* DEPRECATED ************
* @purpose Action function for the user priviledge mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliPriviledgeUserPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                        L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                        EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_char8 * pass;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_char8 currentUser[L7_LOGIN_SIZE];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  usmDbLoginSessionUserGet(unit, cliLoginSessionIndexGet(), currentUser);
  if (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) != L7_TRUE)
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_AuthorizedFailed);
    cliSyntaxBottom(ewsContext);
    return NULL;
  }
  if( cliGetStringInputID() == CLI_INPUT_EMPTY)
  {
    if( cliCheckNoForm(ewsContext) == NULL)
    {
      return NULL;
    }
    if( mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_USER_EXEC_MODE))
    {
      prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_USER_EXEC_MODE));
      if( prompt != NULL )
      {
        return prompt;
      }
    }
    if( argc == depth &&  mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_USER_EXEC_MODE))
    {
      osapiSnprintfAddBlanks (0, 0, 0, 2, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Enbl_2);
      cliAlternateCommandSet(buf);
      cliSetStringInputID(1, ewsContext, argv);
      cliSetStringPassword();
      return pStrInfo_base_LoginPasswdPrompt;
    }
    else
    {
      ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
      ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
      /* then just return previous prompt */
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    pass = cliGetStringInput();
    /* Log the password entered through the command logger */
    cliWebCmdLoggerEntryAdd(pass, FD_WEB_DEFAULT_MAX_CONNECTIONS+1);
    memset (buf, 0, sizeof(buf));
    if (usmDbLoginAuthenticateUserEnable(FD_USER_MGR_ADMIN_ACCESS_LEVEL, pass) == L7_SUCCESS)
    {
      return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_PRIVILEGE_USER_MODE) );
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_PasswdGiven);
      ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
      return NULL;

    }
  }
  ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
  ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  /* then just return previous prompt */
  return cliPrompt(ewsContext);
}
#endif

/*********************************************************************
*
* @purpose Action function for the VLAN mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliVlanPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                              L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  EwsCliCommandP vlanMode;

  if( cliCheckNoForm(ewsContext) == NULL)
  {
    return NULL;
  }
  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_PRIVILEGE_USER_MODE));
  if( prompt != NULL)
  {
    return prompt;
  }

  vlanMode = cliGetMode(L7_VLAN_MODE);
  if ( argc == depth && mainMenu[cliUtil.handleNum] != vlanMode)
  {
    /* It is assume that action function is associated with the database word in the tree*/
    return cliPromptSet(tmpPrompt,argv, ewsContext, vlanMode);
  }

  ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
  return cliPrompt(ewsContext);
}

#ifdef L7_DHCPS_PACKAGE
/*********************************************************************
*
* @purpose Action function for the DHCP mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax ip dhcp pool <name>
*
* @cmdhelp configures the DHCP server address pool
*
* @cmddescript
*   The pool name is a mandatory parameter and is case sensitive
*
* @end
*
*********************************************************************/
const L7_char8 *cliDhcpsPoolConfigPrompt (L7_uint32 depth, L7_char8 * tmpPrompt,
                                          L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_uint32 unit, poolIndex;
  L7_RC_t rc;
  L7_BOOL poolExists=L7_FALSE;
  L7_char8 poolName[L7_DHCPS_POOL_NAME_MAXLEN + 1];
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));
  if( prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_DHCP_POOL_CONFIG_MODE)))
  {
    unit = cliGetUnitId();

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbDhcpsPoolEntryFirst(unit,poolName,&poolIndex);
      while (rc == L7_SUCCESS)
      {
        if (strcmp(poolName,argv[argc-1]) == 0)
        {
          rc = L7_FAILURE;
          poolExists = L7_TRUE;
        }
        else
        {
          rc = usmDbDhcpsPoolEntryNext(unit, poolName, &poolIndex);
        }
      }

      if (poolExists == L7_TRUE)
      {

        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          OSAPI_STRNCPY_SAFE(EWSPOOLNAME(ewsContext), argv[argc-1]);
        }

        return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_DHCP_POOL_CONFIG_MODE));
      }
      else if (poolExists == L7_FALSE)
      {
        if (strlen(argv[argc-1]) > L7_DHCPS_POOL_NAME_MAXLEN)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_InvalidPoolName_1);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        else
        {
          OSAPI_STRNCPY_SAFE(name, argv[argc-1]);
          if (cliIsAlphaNum(name) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_InvalidPoolName_1);
          }
          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            if ( usmDbDhcpsPoolCreate(unit, name) != L7_SUCCESS )
            {
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantCreateDhcpPool);
              cliSyntaxBottom(ewsContext);
              return NULL;
            }
            else
            {
              OSAPI_STRNCPY_SAFE(EWSPOOLNAME(ewsContext), argv[argc-1]);
              return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_DHCP_POOL_CONFIG_MODE));
            }
          }
          else
          {
            return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_DHCP_POOL_CONFIG_MODE));
          }
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        OSAPI_STRNCPY_SAFE(name, argv[argc-1]);
        if ( usmDbDhcpsPoolDelete(unit, name) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelDhcpPool);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
      }
      else
      {
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}
#endif

/*********************************************************************
*
* @purpose Action function for the TACACS mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliTacacsPrompt(L7_uint32 depth,
                                L7_char8 * tmpPrompt,
                                L7_char8 * tmpUpPrompt,
                                L7_uint32 argc,
                                const L7_char8 * * argv,
                                EwsContext ewsContext,
                                L7_uint32 index)
{
  L7_char8 * prompt;
  L7_uint32 unit, numArg = 0, ipAddr = 0;
  L7_char8 strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();

  prompt = (L7_char8 *) cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                 argv, ewsContext,
                                                 cliGetMode
                                                 (L7_GLOBAL_CONFIG_MODE));
  if( prompt != NULL)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }
  else if ( argc == depth + 1 && mainMenu[cliCurrentHandleGet()] !=
           cliGetMode(L7_TACACS_MODE))
  {
    unit = cliGetUnitId();
    OSAPI_STRNCPY_SAFE(strHostAddr, argv[index + numArg]);
    if (cliIPHostAddressValidate(ewsContext, strHostAddr, &ipAddr, &type) != L7_SUCCESS)
    {
      /* Invalid Host Address*/
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
    }
    /* If the command is of type 'normal' the 'if' condition is executed otherwise 'else-if' condition is excuted */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (numArg != 1)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_HOST_HELP);
      }

      if(type == L7_IP_ADDRESS_TYPE_DNS)
      {
         if(strlen(argv[index + numArg]) > L7_DNS_HOST_NAME_LEN_MAX)
         {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsHostName);
         }
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        switch (usmDbTacacsHostNameServerAdd(type, strHostAddr))
        {
        case L7_SUCCESS:
        case L7_ALREADY_CONFIGURED:
          break;
        default:
          cliSyntaxTop(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsAddHost);
        }
      }
      OSAPI_STRNCPY_SAFE(EWSTACACSSRVHOST(ewsContext), strHostAddr);
      return cliPromptSet(tmpPrompt,argv, ewsContext,
                          cliGetMode(L7_TACACS_MODE));

    } /* end if CLI_NORMAL_CMD */
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbTacacsHostNameServerRemove(type, strHostAddr) != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsRemoveHost);
      }

      return cliPromptSet(tmpUpPrompt,argv, ewsContext,
                          cliGetMode(L7_GLOBAL_CONFIG_MODE));
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  }

  /*************Set Flag for Script Success******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose Action function for the Mail Server mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliMailServerPrompt(L7_uint32 depth,
                                L7_char8 * tmpPrompt,
                                L7_char8 * tmpUpPrompt,
                                L7_uint32 argc,
                                const L7_char8 * * argv,
                                EwsContext ewsContext,
                                L7_uint32 index)
{
  L7_char8 * prompt;
  L7_uint32 unit, numArg = 0;
  L7_char8 mailServerAddr[L7_LOG_EMAILALERT_MAIL_SERVER_LEN];
  L7_inet_addr_t serverAddr;		
  L7_uint32 ipAddr = L7_NULL;	
  L7_RC_t 	rc=L7_FAILURE;
  L7_IP_ADDRESS_TYPE_t   type = L7_IP_ADDRESS_TYPE_UNKNOWN;	

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  numArg = cliNumFunctionArgsGet();

  prompt = (L7_char8 *) cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                 argv, ewsContext,
                                                 cliGetMode
                                                 (L7_GLOBAL_CONFIG_MODE));
  if( prompt != NULL)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }
  else if ( argc == depth + 1 && mainMenu[cliCurrentHandleGet()] !=
           cliGetMode(L7_MAIL_SERVER_MODE))
  {
    unit = cliGetUnitId();
    OSAPI_STRNCPY_SAFE(mailServerAddr, argv[index + numArg]);

	  rc = usmDbIPHostAddressValidate(mailServerAddr,&ipAddr,&type);
	  if(rc == L7_ERROR)
	  {
	    /* Invalid Ip Address */
	    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
				  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
	  }
	  else if( rc == L7_FAILURE)
	  {
	    /* It can be either a invalid host name or can be a IPV6 address */
	    /* Convert the IP Address to Inet Address */
	    if(usmDbIPv6HostAddressValidate(mailServerAddr, &serverAddr,&type) != L7_SUCCESS)
	    {
	      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
	  	    ewsContext, pStrErr_common_MailServerInValIp);
	    }
	    if( (type != L7_IP_ADDRESS_TYPE_IPV6)&&(type != L7_IP_ADDRESS_TYPE_DNS))
	    {
	        /* Invalid Host Name*/
	        cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
	  		    ewsContext, pStrErr_base_InvalidHostName);
	    }
	  }

		
    /* If the command is of type 'normal' the 'if' condition is executed otherwise 'else-if' condition is excuted */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (numArg != 1)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_CfgEmailAlertingMailServerIpAdress);
      }


      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        switch (usmDbLogEmailAlertMailServerAddressSet(USMDB_UNIT_CURRENT,type,mailServerAddr))
        {
        case L7_SUCCESS:
        case L7_ALREADY_CONFIGURED:
          break;
        case L7_TABLE_IS_FULL:
          cliSyntaxTop(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_MaxMailServersExceeded);
        default:
          cliSyntaxTop(ewsContext);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_MailServerAddHost);
        }
      }
      OSAPI_STRNCPY_SAFE(EWSMAILSERVERHOST(ewsContext), mailServerAddr);
      return cliPromptSet(tmpPrompt,argv, ewsContext,
                          cliGetMode(L7_MAIL_SERVER_MODE));

    } /* end if CLI_NORMAL_CMD */
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbLogEmailAlertMailServerAddressRemove(USMDB_UNIT_CURRENT,type, mailServerAddr) != L7_SUCCESS)
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_MailServerRemoveHost);
      }

      return cliPromptSet(tmpUpPrompt,argv, ewsContext,
                          cliGetMode(L7_GLOBAL_CONFIG_MODE));
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  }

  /*************Set Flag for Script Success******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_RLIM_PACKAGE

#ifdef L7_IPV6_PACKAGE
/*********************************************************************
*
* @purpose Action function for the tunneling Mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax interface tunnel <tunnelId>
*
* @cmdhelp configures the Tunnel mode for IPV6
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliTunnelConfigPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                      L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                      EwsContext ewsContext)
{
  L7_uint32 unit;
  L7_uint32 tunnelID = 0;
  L7_uint32 intfNum;
  L7_char8 * prompt;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt,
                                                argc,argv, ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));
  if( prompt != NULL)
  {
    return prompt;
  }
  else if ( argc == depth && mainMenu[cliCurrentHandleGet()] !=
           cliGetMode(L7_TUNNEL_CONFIG_MODE))
  {
    /*************Set Flag for Script Execution******/
    ewsContext->actionFunctionStatusFlag=L7_EXECUTE_SCRIPT;

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(cliValidateTunnelId(ewsContext, (L7_char8 *)argv[argc-1],
                               &tunnelID) != L7_SUCCESS)
        {
          cliSyntaxTop(ewsContext);
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_common_TunnelOutOfRange,
                                  L7_CLIMIN_TUNNELID, L7_CLIMAX_TUNNELID);
        }

        if(usmDbRlimTunnelCreate(tunnelID) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_TunnelCreation);
        }

        if(usmDbRlimTunnelIntIfNumGet(tunnelID, &intfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidTunnel);
        }
        else
        {
          memset(EWSINTFMASK(ewsContext).value, 0, sizeof(EWSINTFMASK(ewsContext).value));
          L7_INTF_SETMASKBIT(EWSINTFMASK(ewsContext), intfNum);
          EWSTUNNELID(ewsContext) = tunnelID;

          if ((0 == intfNum) || (usmDbUnitSlotPortGet(intfNum, &EWSUNIT(ewsContext), &EWSSLOT(ewsContext), &EWSPORT(ewsContext)) != L7_SUCCESS))
          {
            EWSUNIT(ewsContext) = EWSSLOT(ewsContext) = EWSPORT(ewsContext) = 0;
          }
        }
      }
      osapiSnprintf(buf, sizeof(buf), "%s %d)#", tmpPrompt, tunnelID);
      return cliPromptSet(buf,argv, ewsContext, cliGetMode(L7_TUNNEL_CONFIG_MODE));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(cliValidateTunnelId(ewsContext, (L7_char8 *)argv[argc-1], &tunnelID)
           != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidTunnelId);
        }

        /* GET INTERFACE */
        if(usmDbRlimTunnelIntIfNumGet(tunnelID, &intfNum) == L7_SUCCESS)
        {
          if(cliIsStackingSupported() == L7_TRUE)
          {
            unit = EWSUNIT(ewsContext);
          }
          else
          {
            unit = cliGetUnitId();
          }

          /* Clear the local Address */
          if(usmDbRlimTunnelLocalAddrClear(unit, intfNum) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_LocalAddrDelFailed);

          }

          /* Clearing the remote Address */
          if(usmDbRlimTunnelRemoteAddrClear(unit, intfNum) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_RemoteAddrDelFailed);

          }

          /* Deleting the tunnel */
          if(usmDbRlimTunnelDelete(unit, intfNum) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TunnelDeletionFailed);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_TunnelNotFound);
        }
      }
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  }
  /*************Set Flag for Script FAILURE******/

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return cliPrompt(ewsContext);
}
#endif

/*********************************************************************
*
* @purpose Action function for the loopback interface Mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax interface loopback <loopback-id>
*
* @cmdhelp configures the Loopback mode for IPV6
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliLoopbackConfigPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                        L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,
                                        EwsContext ewsContext)
{
  L7_uint32 unit;
  L7_uint32 loopbackID = 0;
  L7_uint32 intfNum;
  L7_char8 * prompt;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv,
                                                ewsContext,cliGetMode(L7_PRIVILEGE_USER_MODE));
  if( prompt != NULL)
  {
    return prompt;
  }
  else if ( argc == depth && mainMenu[cliCurrentHandleGet()]
           != cliGetMode(L7_LOOPBACK_CONFIG_MODE))
  {

    /*************Set Flag for Script Failed******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Converts the loopbackId in to integer and validates it */
        if(cliValidateLoopbackId(ewsContext, (L7_char8 *)argv[argc-1],
                                 &loopbackID) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                                  pStrErr_common_LoopBackIdOutOfRange, L7_CLIMIN_LOOPBACKID, L7_CLIMAX_LOOPBACKID);
        }

        if(usmDbRlimLoopbackCreate(loopbackID) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidLoopBackId);
        }
        else
        {
          if(usmDbRlimLoopbackIntIfNumGet(loopbackID, &intfNum) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidLoopBackId);
          }
          else
          {
            memset(EWSINTFMASK(ewsContext).value, 0, sizeof(EWSINTFMASK(ewsContext).value));
            L7_INTF_SETMASKBIT(EWSINTFMASK(ewsContext), intfNum);
            EWSLOOPBACKID(ewsContext) = loopbackID;

            if ((0 == intfNum) || (usmDbUnitSlotPortGet(intfNum, &EWSUNIT(ewsContext), &EWSSLOT(ewsContext), &EWSPORT(ewsContext)) != L7_SUCCESS))
            {
              EWSUNIT(ewsContext) = EWSSLOT(ewsContext) = EWSPORT(ewsContext) = 0;
            }
          }
        }
      }
      osapiSnprintf(buf, sizeof(buf), "%s %d)#", tmpPrompt, loopbackID);
      return cliPromptSet(buf,argv, ewsContext, cliGetMode(L7_LOOPBACK_CONFIG_MODE));
    }
    else if(ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Converts the loopbackId in to integer and validates it */
        if(cliValidateLoopbackId(ewsContext, (L7_char8 *)argv[argc-1],
                                 &loopbackID) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidLoopBackId);
        }

        if(usmDbRlimLoopbackIntIfNumGet(loopbackID, &intfNum) == L7_SUCCESS)
        {
          if(cliIsStackingSupported() == L7_TRUE)
          {
            unit = EWSUNIT(ewsContext);
          }
          else
          {
            unit = cliGetUnitId();
          }

          if(usmDbRlimLoopbackDelete(unit, intfNum) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_LoopBackDeletionFailed);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_LoopBackIntfNotFound);
        }
      }
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return cliPrompt(ewsContext);
  }
  /*************Set Flag for Script FAILURE******/

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return cliPrompt(ewsContext);
}
#endif
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
/*********************************************************************
*
* @purpose Action function for the ipv6 DHCP mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax ipv6 dhcp pool <name>
*
* @cmdhelp configures the DHCP server address pool
*
* @cmddescript
*   The pool name is a mandatory parameter and is case sensitive
*
* @end
*
*********************************************************************/
const L7_char8 *cliDhcp6sPoolConfigPrompt (L7_uint32 depth, L7_char8 * tmpPrompt,
                                           L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt;
  L7_uint32 unit, poolIndex;
  L7_RC_t rc;
  L7_BOOL poolExists=L7_FALSE;
  L7_BOOL poolUsed = L7_FALSE;
  L7_char8 poolName[L7_DHCP6S_POOL_NAME_MAXLEN + 1];
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 intfNumber = 0;
  L7_DHCP6_MODE_t mode;
  L7_char8 poolNameGet[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 serverPref;
  L7_in6_addr_t relayAddr;
  L7_uint32 relayIfNum;
  L7_char8 remoteId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 optionFlags;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,argv, ewsContext,cliGetMode(L7_GLOBAL_CONFIG_MODE));
  if( prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_DHCP6S_POOL_CONFIG_MODE)))
  {
    unit = cliGetUnitId();
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbDhcp6sPoolEntryFirst(unit,poolName,&poolIndex);
      while (rc == L7_SUCCESS)
      {
        if (strcmp(poolName,argv[argc-1]) == 0)
        {
          rc = L7_FAILURE;
          poolExists = L7_TRUE;
        }
        else
        {
          rc = usmDbDhcp6sPoolEntryNext(unit, poolName, &poolIndex);
        }
      }

      if (poolExists == L7_TRUE)
      { /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          OSAPI_STRNCPY_SAFE(EWSPOOLNAME(ewsContext), argv[argc-1]);
        }
        return cliPromptSet(tmpPrompt,argv, ewsContext,cliGetMode(L7_DHCP6S_POOL_CONFIG_MODE));
      }
      else if (poolExists == L7_FALSE)
      {
        if (strlen(argv[argc-1]) > L7_DHCP6S_POOL_NAME_MAXLEN)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_InvalidIpv6DhcpPoolName);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        else
        {
          OSAPI_STRNCPY_SAFE(name, argv[argc-1]);
          if (cliIsAlphaNum(name) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_InvalidIpv6DhcpPoolName);
          }
          /*******Check if the Flag is Set for Execution*************/
          if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
          {
            rc = usmDbDhcp6sPoolCreate(unit, name);
            if ( rc != L7_SUCCESS )
            {
              if(rc == L7_TABLE_IS_FULL)
              {
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_MaxDhcpv6PoolNameReceahed);
              }
              else
              {
                ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_Dhcpv6PoolNameReceahed);
              }

              cliSyntaxBottom(ewsContext);
              return NULL;
            }
            else
            {
              OSAPI_STRNCPY_SAFE(EWSPOOLNAME(ewsContext), argv[argc-1]);
              return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_DHCP6S_POOL_CONFIG_MODE));
            }
          }
          else
          {
            return cliPromptSet(tmpPrompt,argv, ewsContext, cliGetMode(L7_DHCP6S_POOL_CONFIG_MODE));
          }
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        osapiStrncpySafe(name,argv[argc-1], sizeof(name));

        while(usmDbDhcp6sIntfGetNext(unit, &intfNumber) == L7_SUCCESS)
        {
          memset(poolNameGet, 0, L7_CLI_MAX_STRING_LENGTH);
          if(usmDbDhcp6sIntfModeGet(unit, intfNumber, &mode, poolNameGet, &serverPref, &relayAddr,
                                 &relayIfNum, remoteId, &optionFlags) == L7_SUCCESS)
          {

            if(strcmp(name,poolNameGet) == 0)
            {
              poolUsed = L7_TRUE;
              break;
            }
            else
            {
              continue;
            }
          }
        }

        if(poolUsed == L7_TRUE)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelIpv6DhcpPoolInUse);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        else if ( usmDbDhcp6sPoolDelete(unit, name) != L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_base_CantDelDhcpPool);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
      }
      else
      {/*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;

}
#endif
#endif

#ifdef L7_WIRELESS_PACKAGE
/*********************************************************************
*
* @purpose Local AP database function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax ap local <macaddr>
*
* @cmdhelp Creates an AP in Local AP Database and enters the AP Mode.
*
* @cmddescript
*   The MAC address is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessAPPrompt (L7_uint32 depth, L7_char8 * tmpPrompt,
                                     L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                     const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt = NULL;
  L7_RC_t rc = L7_SUCCESS;
  L7_enetMacAddr_t mac;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  memset (&mac, 0, sizeof (mac));
  prompt = (L7_char8 *) cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                 argv, ewsContext,
                                                 cliGetMode(L7_WIRELESS_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] !=
                            cliGetMode(L7_WIRELESS_AP_CONFIG_MODE)))
  {
    if (strlen (argv[argc-1]) >= L7_CLI_MAX_STRING_LENGTH)
    {
      ewsTelnetWrite (ewsContext, pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
      cliSyntaxBottom (ewsContext);
      return NULL;
    }
    else
    {
      memset (mac.addr, 0, sizeof(mac.addr));
      if (cliConvertMac ((L7_uchar8 *)argv[argc-1], mac.addr) != L7_TRUE)
      {
        ewsTelnetWrite (ewsContext, pStrInfo_common_CrLf);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
      }
      if (cliIsRestrictedFilterMac(mac.addr) == L7_TRUE)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_common_CrLf);
        ewsTelnetWriteAddBlanks(1,1,0,0, L7_NULLPTR, ewsContext, "Restricted MAC address entered. Entry not added to local AP database.");
        return cliPrompt(ewsContext);
      }
    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      rc = usmDbWdmAPEntryGet (mac);
      if (rc == L7_SUCCESS)
      {
        memcpy (EWSMACADDR(ewsContext), mac.addr, L7_MAC_ADDR_LEN);
        return cliPromptSet (tmpPrompt, argv, ewsContext,
                             cliGetMode (L7_WIRELESS_AP_CONFIG_MODE));
      }
      else
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          if ((rc=usmDbWdmAPEntryAdd(mac)) != L7_SUCCESS)
          {
            if (rc == L7_ERROR)
            {
              ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,
                                       "Invalid MAC address entered. Entry not added to local AP database.");
            }
            else
            {
              ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CantAddAp);
            }
            return NULL;
          }
          else
          {
            memcpy (EWSMACADDR(ewsContext), mac.addr, L7_MAC_ADDR_LEN);
            return cliPromptSet (tmpPrompt, argv, ewsContext,
                                 cliGetMode(L7_WIRELESS_AP_CONFIG_MODE));
          }
        }
        else
        {
          memcpy (EWSMACADDR(ewsContext), mac.addr, L7_MAC_ADDR_LEN);
          return cliPromptSet (tmpPrompt, argv, ewsContext,
                               cliGetMode(L7_WIRELESS_AP_CONFIG_MODE));
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbWdmAPEntryDelete (mac) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CantDelAp);
          cliSyntaxBottom (ewsContext);
          return NULL;
        }
        if (usmDbWdmManagedAPEntryGet (mac) == L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_ApValidApModifyMsg);
        }
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt (ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}

/*********************************************************************
*
* @purpose Wireless Network prompt function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax network <1-32>
*
* @cmdhelp Creates a wireless network configuration, if not present, and enters the network mode.
*
* @cmddescript
*   The network ID is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessNetworkPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                         L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                         const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt = NULL;
  L7_uint32 networkId = 0;
  L7_uint32 networkArg = 1;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                argv, ewsContext,
                                                cliGetMode(L7_WIRELESS_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) &&
      (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_WIRELESS_NETWORK_CONFIG_MODE)))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[networkArg]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    networkId = atoi((L7_char8 *)argv[networkArg]);

    if( (networkId > L7_WDM_NETWORK_ID_MAX) || (networkId < L7_WDM_NETWORK_ID_MIN))
    {
       osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, pStrErr_NetworkID_InvalidRange ,
                         L7_WDM_NETWORK_ID_MIN, L7_WDM_NETWORK_ID_MAX);
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, buf);
    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT )
      {
        if ((usmDbWdmNetworkEntryGet(networkId) == L7_SUCCESS) ||
            (usmDbWdmNetworkEntryAdd(networkId) == L7_SUCCESS))
        {
          EWSNETWORKID(ewsContext) = networkId;

          return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_WIRELESS_NETWORK_CONFIG_MODE));
        }
      }
      else
      {
         return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_WIRELESS_NETWORK_CONFIG_MODE));
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbWdmNetworkEntryGet(networkId) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_NwIdInvalid);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (usmDbWdmNetworkEntryDelete(networkId) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_NwDelFailed);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        cliSyntaxBottom(ewsContext);
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
} /* cliWirelessNetworkPrompt */

/*********************************************************************
*
* @purpose Wireless WDS AP Group configuration prompt function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax network <1-8>
*
* @cmdhelp Creates a WDS AP Group , if not present, and enters the WDS group configuration mode.
*
* @cmddescript
*   The network ID is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessWdsApGroupPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                         L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                         const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 *prompt = NULL;
  L7_uchar8 groupId = 0;
  L7_uint32 groupArg = 1;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                argv, ewsContext,
                                                cliGetMode(L7_WIRELESS_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) &&
      (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_WIRELESS_WDS_APGROUP_CONFIG_MODE)))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[groupArg]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    groupId = atoi((L7_char8 *)argv[groupArg]);

    if( (groupId > L7_WDM_WDS_GROUP_ID_MAX) || (groupId < L7_WDM_WDS_GROUP_ID_MIN))
    {
       osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, "Entered Group ID is not in range. Valid range is from 1 to 8." ,
                         L7_WDM_NETWORK_ID_MIN, L7_WDM_WDS_GROUP_ID_MAX);
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, buf);
    }
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT )
      {
        if ((usmDbWdmWdsApGroupEntryGet(groupId) == L7_SUCCESS) ||
            (usmDbWdmWdsApGroupEntryAdd(groupId) == L7_SUCCESS))
        {
          EWSWDSAPGRPID(ewsContext) = groupId;

          return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_WIRELESS_WDS_APGROUP_CONFIG_MODE));
        }
      }
      else
      {
         return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_WIRELESS_WDS_APGROUP_CONFIG_MODE));
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbWdmWdsApGroupEntryGet(groupId) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, "Group is not configured");
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (usmDbWdmWdsApGroupEntryDelete(groupId) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, "Deleting WDS AP Group has failed");
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        cliSyntaxBottom(ewsContext);
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}


/*********************************************************************
*
* @purpose AP Profile function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax ap profile <1-16>
*
* @cmdhelp Creates an AP Profile Index, if not present, and enters the AP Mode.
*
* @cmddescript
*   The Profile Index is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfilePrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                   L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                   const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt = NULL;
  L7_uint32 profileId = 0;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                argv, ewsContext,
                                                cliGetMode(L7_WIRELESS_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) &&
      (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_WIRELESS_AP_PROFILE_CONFIG_MODE)))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[2]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    profileId = atoi((L7_char8 *)argv[2]);

    if( (profileId > L7_WDM_PROFILE_ID_MAX) || (profileId < L7_WDM_PROFILE_ID_MIN))
    {
       osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, pStrInfo_wireless_ProfileOutOfRange,
                         L7_WDM_PROFILE_ID_MIN, L7_WDM_PROFILE_ID_MAX);
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, buf);

    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbWdmAPProfileEntryGet(profileId) == L7_SUCCESS)
      {
        EWSPROFILEID(ewsContext) = profileId;
        return cliPromptSet(tmpPrompt, argv, ewsContext,
                            cliGetMode (L7_WIRELESS_AP_PROFILE_CONFIG_MODE));
      }
      else
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          if (usmDbWdmAPProfileEntryAdd(profileId) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CantAddApProfile);
            cliSyntaxBottom (ewsContext);
            return NULL;
          }
          else
          {
            EWSPROFILEID(ewsContext) = profileId;
            return cliPromptSet(tmpPrompt, argv, ewsContext,
                                cliGetMode(L7_WIRELESS_AP_PROFILE_CONFIG_MODE));
          }
        }
        else
        {
          return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_WIRELESS_AP_PROFILE_CONFIG_MODE));
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (profileId == FD_WDM_DEFAULT_AP_PROFILE_ID)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CantRemoveDeflProfile);
        }
        if (usmDbWdmAPProfileEntryGet(profileId) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_CantDelApProfile);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (usmDbWdmAPProfileEntryDelete(profileId) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_ApProfileCant);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
} /* cliAPProfilePrompt */

/*********************************************************************
*
* @purpose AP Profile Radio function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax radio <1-2>
*
* @cmdhelp Enters the AP Profile Radio Configuration mode.
*
* @cmddescript
*   The Radio Index is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileRadioPrompt (L7_uint32 depth, L7_char8 * tmpPrompt,
                                         L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                         const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt = NULL;
  L7_uint32 profileId;
  L7_uint32 radioIndex = 0;
  L7_char8 strRadioIndex[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *) cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                 argv, ewsContext,
                                                 cliGetMode(L7_WIRELESS_AP_PROFILE_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] !=
                            cliGetMode(L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE)))
  {
    if( cliCheckNoForm(ewsContext) == NULL)
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
      return NULL;
    }

    if (strlen (argv[argc-1]) >= L7_CLI_MAX_STRING_LENGTH)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_UsrInputInvalidApProfileId);
      cliSyntaxBottom (ewsContext);
      return NULL;
    }
    else
    {
      OSAPI_STRNCPY_SAFE(strRadioIndex, argv[1]);
      if (cliCheckIfInteger (strRadioIndex) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
      }
      radioIndex = atoi (strRadioIndex);

      /* Verifying if the specified radio index 
      for the configured radio type */      
      if( usmDbWdmAPProfileRadioIndexGet(EWSPROFILEID(ewsContext), 
                     radioIndex) == L7_FAILURE )
      {        
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_wireless_InvalidRadioId);
      }
    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      profileId = EWSPROFILEID(ewsContext); 
      EWSPROFILEPROFILEID(ewsContext) = profileId;
      EWSPROFILERADIOID(ewsContext) = radioIndex;

      return cliPromptSet (tmpPrompt, argv, ewsContext,
                             cliGetMode (L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE));
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt (ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return cliPrompt(ewsContext);
  }

  ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
  ewsSetTelnetPrompt (ewsContext, cliCommon[cliUtil.handleNum].prompt);
  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return cliPrompt(ewsContext);
} /* cliAPProfileRadioPrompt */

/*********************************************************************
*
* @purpose AP Profile VAP function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax radio <1-2>
*
* @cmdhelp Enters the AP Profile VAP Configuration mode.
*
* @cmddescript
*   The VAP id is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileVAPPrompt (L7_uint32 depth, L7_char8 * tmpPrompt,
                                       L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                       const L7_char8 * * argv, EwsContext ewsContext)
{
  L7_char8 * prompt = NULL;
  L7_uint32 profileId;
  L7_uint32 radioIndex = 0;
  L7_uint32 vapId = 0;
  L7_char8 strVAPIndex[L7_CLI_MAX_STRING_LENGTH];  
  L7_uint32 hwTypeID = L7_NULL;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *) cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                 argv, ewsContext,
                                                 cliGetMode(L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] !=
                            cliGetMode(L7_WIRELESS_AP_PROFILE_VAP_CONFIG_MODE)))
  {
    if( cliCheckNoForm(ewsContext) == NULL)
    {
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
      return NULL;
    }

    if (strlen (argv[argc-1]) >= L7_CLI_MAX_STRING_LENGTH)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_UsrInputInvalidVapId);
      cliSyntaxBottom (ewsContext);
      return NULL;
    }
    else
    {
      OSAPI_STRNCPY_SAFE(strVAPIndex, argv[1]);
      if (cliCheckIfInteger (strVAPIndex) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
      }
      vapId = atoi (strVAPIndex);
      if( usmDbWdmAPProfileHardwareTypeGet(EWSPROFILEID(ewsContext), 
                     &hwTypeID) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_wireless_GetIntlData );
      } 
      if( usmDbWdmAPHWTypeVAPIndexGet(hwTypeID,EWSPROFILERADIOID(ewsContext), 
                     vapId) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                       ewsContext, pStrErr_wireless_InvalidVapId);
      }


    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          profileId  = EWSPROFILEPROFILEID(ewsContext);
          radioIndex = EWSPROFILERADIOID(ewsContext);

          EWSVAPAPPROFILEID(ewsContext)    = profileId;
          EWSVAPAPPROFILERADIO(ewsContext) = radioIndex;
          EWSVAPAPPROFILEVAPID(ewsContext) = vapId;

        }

        return cliPromptSet (tmpPrompt, argv, ewsContext,
                             cliGetMode (L7_WIRELESS_AP_PROFILE_VAP_CONFIG_MODE));
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt (ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return cliPrompt(ewsContext);
  }

  ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
  ewsSetTelnetPrompt (ewsContext, cliCommon[cliUtil.handleNum].prompt);
  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return cliPrompt(ewsContext);
} /* cliAPProfileVAPPrompt */

/*********************************************************************
*
* @purpose Device Location function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax device-location building <1-8>
*
* @cmdhelp Creates an building Index, if not present, and enters the 
*           building Mode.
*
* @cmddescript
*   The Building number is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliBuildingPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                  L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                  const L7_char8 * * argv, 
                                  EwsContext ewsContext)
{
  L7_char8 * prompt = NULL;
  L7_uint32 bldng = 0;
  L7_char8 desc[L7_WDM_MAX_DESCRIPTION+1];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                argv, ewsContext,
                                                cliGetMode(L7_WIRELESS_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }
 
  memset(desc, 0, L7_WDM_MAX_DESCRIPTION+1); 
  if ((argc == depth+1) &&
      (mainMenu[cliCurrentHandleGet()] != 
       cliGetMode(L7_WIRELESS_BUILDING_CONFIG_MODE)))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[2]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                                             ewsContext,
                                             pStrErr_common_InvalidInteger);
    }
    bldng = atoi((L7_char8 *)argv[2]);

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbWdmDevLocBuildingEntryGet (bldng) == L7_SUCCESS)
      {
        EWSDEVLOCBLDNG(ewsContext) = bldng;
        return cliPromptSet(tmpPrompt, argv, ewsContext,
                            cliGetMode (L7_WIRELESS_BUILDING_CONFIG_MODE));
      }
      else
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          if (usmDbWdmDevLocBuildingEntryAdd(bldng, desc) == L7_FAILURE)
          {
            ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                     pStrErr_wireless_CfgWsBldngEntryAdd);
            cliSyntaxBottom (ewsContext);
            return NULL;
          }
          else
          {
            EWSDEVLOCBLDNG(ewsContext) = bldng;
            return cliPromptSet(tmpPrompt, argv, ewsContext,
                            cliGetMode (L7_WIRELESS_BUILDING_CONFIG_MODE));
          }
        }
        else
        {
          return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_WIRELESS_BUILDING_CONFIG_MODE));
        }
      }
    }    
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbWdmDevLocBuildingEntryGet (bldng) != L7_SUCCESS)         
        {
          ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, 
                                   ewsContext,
                                   "Building number doesnot exist");
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (usmDbWdmDevLocBuildingEntryDelete(bldng)!= L7_SUCCESS)
        {
           ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR,
                                    ewsContext,
                                    pStrErr_wireless_CfgWsBldngEntryDel);   
           cliSyntaxBottom(ewsContext);
           return NULL;
        }
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,
                             pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}/*cliBuildingPrompt*/ 
      
/*********************************************************************
*
* @purpose Building Floor function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax floor <1-20>
*
* @cmdhelp Enters the Device location building floor Configuration mode.
*
* @cmddescript
*   
* @end
*
*********************************************************************/
const L7_char8 *cliFloorPrompt (L7_uint32 depth, L7_char8 * tmpPrompt,
                                L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                const L7_char8 * * argv, EwsContext ewsContext)    

{
  L7_char8 * prompt = NULL;
  L7_uint32 bldng;
  L7_uint32 flr = 0;
  L7_char8 strFloorIndex[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 desc[L7_WDM_MAX_DESCRIPTION+1];
  
  memset (desc, 0x00, sizeof(desc));

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *) cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                 argv, ewsContext,
                                                 cliGetMode(L7_WIRELESS_BUILDING_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }
  
  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] !=
                            cliGetMode(L7_WIRELESS_BUILDING_FLR_CONFIG_MODE)))
  {
    if (strlen (argv[argc-1]) >= L7_CLI_MAX_STRING_LENGTH)
    {
      ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, 
                              "Invalid Building number");
      cliSyntaxBottom (ewsContext);
      return NULL;
    }
    else
    {
      OSAPI_STRNCPY_SAFE(strFloorIndex, argv[1]);
      if (cliCheckIfInteger (strFloorIndex) != L7_SUCCESS)
      {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                                              ewsContext, 
                                              pStrErr_common_InvalidInteger);
      }
      flr = atoi (strFloorIndex);
    }
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
       if (usmDbWdmDevLocBldngFlrEntryGet(EWSDEVLOCBLDNG(ewsContext),
                     flr) == L7_SUCCESS )
       {
         bldng = EWSDEVLOCBLDNG(ewsContext);
         EWSDEVLOCBLDNGID(ewsContext) = bldng;
         EWSDEVLOCFLR(ewsContext) = flr;
         return cliPromptSet(tmpPrompt, argv, ewsContext,
                            cliGetMode (L7_WIRELESS_BUILDING_FLR_CONFIG_MODE));
       }
       else
       {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          bldng = EWSDEVLOCBLDNG(ewsContext);
          if (usmDbWdmDevLocBldngFlrEntryAdd (bldng, flr, desc) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR,
                                     ewsContext, 
                                     pStrErr_wireless_CfgWsBldngFlrEntryAdd);
            cliSyntaxBottom (ewsContext);
            return NULL;
          }
          else
          {
            bldng = EWSDEVLOCBLDNG(ewsContext);
            EWSDEVLOCBLDNGID(ewsContext) = bldng;
            EWSDEVLOCFLR(ewsContext) = flr;
            return cliPromptSet(tmpPrompt, argv, ewsContext,
                                cliGetMode(L7_WIRELESS_BUILDING_FLR_CONFIG_MODE));
          }
        }
        else
        {
          return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_WIRELESS_BUILDING_FLR_CONFIG_MODE));
        }
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
        /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        bldng = EWSDEVLOCBLDNGID(ewsContext);
        if (usmDbWdmDevLocBldngFlrEntryGet(bldng, flr) != L7_SUCCESS)
        {
           ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,
                                    "Building-Floor Entry doesnot exist");
           cliSyntaxBottom(ewsContext);
           return NULL;
        }
        if (usmDbWdmDevLocBldngFlrEntryDelete (bldng, flr) != L7_SUCCESS)
        {
           ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,
                                    "Cannot delete Building-Floor Entry");
           cliSyntaxBottom(ewsContext);
           return NULL;
        }
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    } 
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, 
                             pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt (ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return cliPrompt(ewsContext);
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return cliPrompt(ewsContext);
} /* cliFloorPrompt */

#endif /* L7_WIRELESS_PACKAGE */ 

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
/*********************************************************************
*
* @purpose Action function for the Captive Portal encoded image mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCaptivePortalEncodedImagePrompt(L7_uint32 depth, L7_char8 *tmpPrompt, L7_char8 *tmpUpPrompt, 
                                               L7_uint32 argc, const L7_char8 **argv, EwsContext ewsContext)
{
  L7_char8 *prompt = NULL;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH]; 

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc, argv, ewsContext,cliGetMode(L7_CAPTIVEPORTAL_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth) && (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_CPENCODED_IMAGE_MODE)))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      osapiSnprintf(stat, sizeof(stat), "%s)#", tmpPrompt);
  
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
      return cliPromptSet(stat, argv, ewsContext, cliGetMode(L7_CPENCODED_IMAGE_MODE));
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}


/*********************************************************************
*
* @purpose Action function for the Captive Portal instance mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCaptivePortalInstancePrompt(L7_uint32 depth, L7_char8 *tmpPrompt, L7_char8 *tmpUpPrompt, 
                                               L7_uint32 argc, const L7_char8 **argv, EwsContext ewsContext)
{
  L7_char8 *prompt = NULL;
  L7_uint32 cpId32 = 0, cpIdArg = 1;
  L7_ushort16 cpId = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH]; 

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc, argv, ewsContext,cliGetMode(L7_CAPTIVEPORTAL_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_CPINSTANCE_MODE)))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[cpIdArg]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    cpId32 = atoi((L7_char8 *)argv[cpIdArg]);
    cpId = (L7_ushort16)cpId32;

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if ((usmDbCpdmCPConfigGet(cpId) == L7_SUCCESS) || (usmDbCpdmCPConfigAdd(cpId) == L7_SUCCESS))
        {
          EWSCPID(ewsContext) = cpId;
          osapiSnprintf(stat, sizeof(stat), "%s%u)#", tmpPrompt, EWSCPID(ewsContext));
  
          /*************Set Flag for Script Success******/
          ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
          return cliPromptSet(stat, argv, ewsContext, cliGetMode(L7_CPINSTANCE_MODE));
        }
      }
      else
      {
        EWSCPID(ewsContext) = cpId;
        osapiSnprintf(stat, sizeof(stat), "%s%u)#", tmpPrompt, EWSCPID(ewsContext));

        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return cliPromptSet(stat, argv, ewsContext, cliGetMode(L7_CPINSTANCE_MODE));
      }     
    }

    else if (ewsContext->commType == CLI_NO_CMD)
    {
      cliSyntaxTop(ewsContext);
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {      
        if (cpId == CP_ID_MIN)
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNoDefaultDelete);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        if (usmDbCpdmCPConfigGet(cpId) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPIdNotFound);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }

        if (usmDbCpdmCPConfigDelete(cpId) != L7_SUCCESS)
        {
          ewsTelnetWrite (ewsContext, pStrErr_security_CPIdDeleteFailure);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
      }
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
      return NULL;
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}

/*********************************************************************
*
* @purpose Action function for the Captive Portal instance locale mode
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCaptivePortalInstanceLocalePrompt(L7_uint32 depth, L7_char8 *tmpPrompt, L7_char8 *tmpUpPrompt, 
                                               L7_uint32 argc, const L7_char8 **argv, EwsContext ewsContext)
{
  L7_char8 *prompt = NULL;
  L7_uint32 webId32 = 0, webIdArg = 1;
  webId_t webId = 0;
  cpId_t cpId = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc, argv, ewsContext,cliGetMode(L7_CPINSTANCE_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) && (mainMenu[cliCurrentHandleGet()] != cliGetMode(L7_CPINSTANCE_LOCALE_MODE)))
  {
    if (cliCheckIfInteger((L7_char8 *)argv[webIdArg]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    webId32 = atoi((L7_char8 *)argv[webIdArg]);
    webId = (webId_t)webId32;
    EWSWEBID(ewsContext) = webId;
    cpId = EWSCPID(ewsContext);

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if ((usmDbCpdmCPConfigWebIdGet(cpId,webId) == L7_SUCCESS) ||
           ((usmDbCpdmCPConfigWebIdAdd(cpId,webId) == L7_SUCCESS) && 
            (usmDbCpdmCPConfigWebAddDefaults(cpId,webId,WEB_DEF1_LANG_CODE,CP_ALL) == L7_SUCCESS)))
        {
          osapiSnprintf(stat, sizeof(stat), "%s%u)#", tmpPrompt, webId);
          /*************Set Flag for Script Success******/
          ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
          return cliPromptSet(stat, argv, ewsContext, cliGetMode(L7_CPINSTANCE_LOCALE_MODE));
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s%u)#", tmpPrompt, webId);
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return cliPromptSet(stat, argv, ewsContext, cliGetMode(L7_CPINSTANCE_LOCALE_MODE));
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      cliSyntaxTop(ewsContext);

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (webId == CP_WEB_ID_MIN)
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgLocaleNoDefaultDelete);
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (usmDbCpdmCPConfigWebIdGet(cpId, webId) != L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgLocaleDoesNotExist, webId);
          ewsTelnetWrite(ewsContext, stat);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        if (usmDbCpdmCPConfigWebIdDelete(cpId, webId) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgLocaleDeleteError);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }  
      }
      /*************Set Flag for Script Success******/
      ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
      return NULL;
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}

#endif /* L7_CAPTIVE_PORTAL_PACKAGE */

/*********************************************************************
*
* @purpose Dot1x user config function
*
* @param L7_uint32 depth
* @param L7_char8 *tmpPrompt
* @param L7_char8 *tmpUpPrompt
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param ewsContext ewsContext
* @param EwsCliCommandP upMode
* @param EwsCliCommandP currMode
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @cmdsyntax dot1x-user username <username>
*
* @cmdhelp Creates an user entry, if not present, and enters the 
*           user config Mode.
*
* @cmddescript
*   The username is a mandatory parameter.
*
* @end
*
*********************************************************************/
const L7_char8 *cliIntAuthServUserPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,
                                  L7_char8 * tmpUpPrompt, L7_uint32 argc,
                                  const L7_char8 * * argv, 
                                  EwsContext ewsContext)
{
  L7_char8 * prompt = NULL;
  L7_char8 username[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH * 2];
  L7_uint32 index = 0;
  L7_uint8 userNameIndex = 3;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  prompt = (L7_char8 *)cliCheckCommonConditions(depth, tmpUpPrompt, argc,
                                                argv, ewsContext,
                                                cliGetMode(L7_GLOBAL_CONFIG_MODE));
  if (prompt != NULL)
  {
    /*************Set Flag for Script Success******/
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return prompt;
  }

  if ((argc == depth+1) &&
      (mainMenu[cliCurrentHandleGet()] != 
       cliGetMode(L7_INT_AUTH_SERV_USER_CONFIG_MODE)))
  {
    osapiStrncpySafe(username, argv[userNameIndex], sizeof(username));

    /* validate username input */
    if (strlen(username) >= L7_LOGIN_SIZE)
    {
      sprintf(buf, CLISYNTAX_INVALIDUSERNAME, (L7_LOGIN_SIZE-1));
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return NULL;
    }
    else if (usmDbStringAlphaNumericCheck(username) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, CLI_USERNAMESYNTAX);
      cliSyntaxBottom(ewsContext);
      return NULL;
    }

    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbUserMgrIASUserDBUserIndexGet (username,&index) == L7_SUCCESS)
      {
        EWSDOT1XAUTHSERVUSERIDX(ewsContext) = index;
        return cliPromptSet(tmpPrompt, argv, ewsContext,
                            cliGetMode (L7_INT_AUTH_SERV_USER_CONFIG_MODE));
      }
      else
      {
        /*******Check if the Flag is Set for Execution*************/
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          if(usmDbUserMgrIASUserDBAvailableIndexGet(&index) == L7_FAILURE)
          {
            ewsTelnetWrite(ewsContext, CLI_TOOMANYUSERS);
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
 
          if (usmDbUserMgrIASUserDBUserNameSet(index, username) == L7_FAILURE)
          {
            ewsTelnetWrite(ewsContext, "\r\nFailed to add IAS user.");
            cliSyntaxBottom(ewsContext);
            return NULL;
          }
          else
          {
            EWSDOT1XAUTHSERVUSERIDX(ewsContext) = index;
            return cliPromptSet(tmpPrompt, argv, ewsContext,
                            cliGetMode (L7_INT_AUTH_SERV_USER_CONFIG_MODE));
          }
        }
        else
        {
          return cliPromptSet(tmpPrompt, argv, ewsContext,
                              cliGetMode(L7_INT_AUTH_SERV_USER_CONFIG_MODE));
        }
      }
    }    
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbUserMgrIASUserDBUserIndexGet (username,&index) != L7_SUCCESS)         
        {
          ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, 
                                   ewsContext,
                                   "IAS user does not exist");
          cliSyntaxBottom(ewsContext);
          return NULL;
        }
        if (usmDbUserMgrIASUserDBUserDelete(index)!= L7_SUCCESS)
        {
           ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR,
                                    ewsContext,
                                    "Failed to delete IAS user.");   
           cliSyntaxBottom(ewsContext);
           return NULL;
        }
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        /*************Set Flag for Script Success******/
        ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
        return NULL;
      }
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,
                             pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
    return NULL;
  }

  /*************Set Flag for Script FAILURE******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  return NULL;
}

