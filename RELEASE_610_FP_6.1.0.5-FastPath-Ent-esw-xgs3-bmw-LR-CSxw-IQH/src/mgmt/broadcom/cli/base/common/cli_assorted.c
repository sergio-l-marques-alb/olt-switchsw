/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cli_assorted.c
*
* @purpose assorted cli commands that do not need their own file
*
* @component user interface
*
* @comments Currently contains the following trees/commands:
* @comments save tree
* @comments reset system (only command in the tree)
* @comments ping
* @comments logout
* @comments help
* @comments devshell
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
#include "cli_web_include.h"
#include "osapi_support.h"
#include "usmdb_ping_api.h"
#include "usmdb_log_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include "sysapi.h"
#include "login_sessions_api.h"
#include "usmdb_dns_client_api.h"
#include "util_pstring.h"
#include "usmdb_support.h"
#include "usmdb_user_mgmt_api.h"
#include "ping_exports.h"

extern cliWebCfgData_t cliWebCfgData;
static osapiTimerDescr_t * pProcessingIndicator = L7_NULLPTR;
/*******************************************************************n
*
* @purpose  to quit a serial or telnet session
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns NULL
*
* @notes none
*
* @cmdsyntax [User Exec]>logout
*
* @cmdhelp  Exit this session. Any unsaved changes are lost.
*
* @cmddescript
*   This command will end a telnet session or reset a serial connection.
*   When you have finished using the terminal interface, ensure you
*   have saved and applied all configuration changes before you log out.
*
* @end
*
*********************************************************************/
const L7_char8 *commandLogout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL hasDataChanged;
  L7_char8 currentUser[L7_LOGIN_SIZE];
  L7_char8 checkChar;
  L7_uint32 unit, numArgs;
  L7_uint32 sessionIndex;
  L7_BOOL clearConfig;
  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_LogOut_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  usmDbUnsavedConfigCheck(unit, &hasDataChanged);
  usmDbSysapiClearConfigFlagGet(&clearConfig); 
  sessionIndex = cliLoginSessionIndexGet();
  usmDbLoginSessionUserGet(unit, sessionIndex, currentUser);

  if (( (hasDataChanged == L7_TRUE) || (clearConfig == L7_TRUE)) && (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) == L7_TRUE))
  {
    if ( cliGetCharInputID() == 1 )
    {
      checkChar = tolower(cliGetCharInput());
      if ( checkChar == 'y' )
      {                                                                   /* yes */
        usmDbSwDevCtrlSaveConfigurationSet(unit, 1);
        ewsTelnetWrite(ewsContext, pStrInfo_common_MsgCfgSaved);
        cliSyntaxBottom(ewsContext);

      }
      else if ( checkChar == 'n')                        /* no */
      {
        ewsTelnetWrite(ewsContext, pStrErr_common_MsgCfgNotSaved);
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        return cliSyntaxReturnPrompt(ewsContext, pStrErr_common_MsgCfgNotSaved);
      }
    }
    else
    {
      cliSetCharInputID(1, ewsContext, argv);
      cliAlternateCommandSet(pStrInfo_base_Logout_1);
      return pStrInfo_base_QuestionSaveChgs;
    }
  }

  /* Log the user logouts in the command logger */
  cliWebCmdLoggerEntryAdd(pStrInfo_base_Logout_1, FD_WEB_DEFAULT_MAX_CONNECTIONS+1);

  cliContextLogout(ewsContext);
  return " ";
}

/*********************************************************************
*
* @purpose  Shows cli hotkeys help
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes this function is run on error from root or from the help command
*
* @cmddescript
*   Show special keys or other help on error.
*
* @end
*
*********************************************************************/
const L7_char8 *commandError(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  cliSyntaxTop(ewsContext);
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_base_Help);
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (0, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_SpecialKeys);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_DelBsDelPreviousChar);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlAGoToBeginningOfLine);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlEGoToEndOfLine);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlFGoForwardOneChar);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlBGoBackwardOneChar);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlDDelCurrentChar);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlUXDelToBeginningOfLine);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlKDelToEndOfLine);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlWDelPreviousWord);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlTTransposePreviousChar);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlPGoToPreviousLineInHistoryBuf);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlRRewritesOrPastesLine);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlNGoToNextLineInHistoryBuf);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlYPrintLastDeldChar);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlQEnblsSerialFlow);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlSDsblsSerialFlow);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CtrlZReturnToRootCmdPrompt);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_TabSpaceCmdLineCompletion);
  ewsTelnetWriteAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ExitGoToNextLowerCmdPrompt);
  return cliSyntaxReturnPromptAddBlanks (0, 1, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_base_ListChoices);

  /* These ctrl key sequences are captured at a lower level within fastpath
 * but are documented for user
 *                                             ^S:  disable serial flow
 *
 *                                             ^Q:  enable serial flow
 */

}

/*********************************************************************
*
* @purpose  runs the devshell
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  [Privilged Exec]> devshell argument
*
* @cmdhelp runs the devshell for debugging purposes
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDevShell(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_BOOL runDevShell = L7_TRUE;

  cliSyntaxTop(ewsContext);

  /*-----------------*/
  /*  Error checking */
  /*-----------------*/
  if ( cliAdminUserFlagGet()!=L7_TRUE)                                     /* admin user only */
  {
      runDevShell = L7_FALSE;
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_AuthorizedCheckedFailed);
  }

  if ( cliNumFunctionArgsGet() != 1 )
  {                                                                       /*check to for invalid # of arguments */
      runDevShell = L7_FALSE;
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_Devshell);
  }

   /* Only run devshell on
        1. the serial port console
        2. telnet connections enabled for the support console display */
  if (cliTelnetConnectionGet() == L7_TRUE)
  {
      /* This is a telnet session */
      if (cliCommon[cliUtil.handleNum].debugDisplayCtl.consoleTraceDisplayEnabled != L7_TRUE)
      {
          runDevShell = L7_FALSE;
     ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_AuthorizedCheckedFailed);
      }
  }

   /*----------------------------------------*/
   /*  Invoke devshell or the syntax message */
   /*----------------------------------------*/
   if (runDevShell == L7_TRUE)
   {
       sysapiPrintf("\n");
       sysapiPrintf("Devshell output (Session #%d): %s\n", cliLoginSessionIndexGet(), argv[index+1]);
       cliDevShell((L7_char8 *)&argv[1][0]);
       return cliPrompt(ewsContext);
   }
   else
   {
    return cliSyntaxReturnPrompt (ewsContext, "");
   }
}
/*********************************************************************
*
* @purpose  runs the BCM shell
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  [Privilged Exec]> bcm "command"
*
* @cmdhelp runs the devshell for debugging purposes
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDrvShell(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{


  cliSyntaxTop(ewsContext);
  // PTin modified
  if ( /*(cliTelnetConnectionGet() != L7_FALSE) ||*/                  /* also only from serial connection */
            (cliAdminUserFlagGet()!=L7_TRUE))                                     /* admin user only */
  {
    ewsTelnetWrite(ewsContext, pStrErr_base_AuthorizedCheckedFailed);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if ( cliNumFunctionArgsGet() != 1 )
  {                                                                       /*check to for invalid # of arguments */
    ewsTelnetWrite(ewsContext, pStrErr_base_Devshell);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  ewsTelnetWrite(ewsContext, "\r\n");

  printf("\n");

  sysapiDebugDriverShell((L7_char8 *)&argv[1][0]);

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enable/Disable the display of the support command on enabled sessions
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  techsupport {enable|disable}
*
* @cmdhelp Hide/unhide hidden support command tree
*
* @cmddescript
*   This is a hidden command to be used only by authorized support
*   personnel.  This command enables the display of a hidden support
*   command tree.  Once the tree is not hidden, the tree is managed
*   just as any other command tree.
*
* @end
*
*********************************************************************/
const L7_char8 *commandTechSupport(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 argMode=1;
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_int32  current_handle;
   L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   cliSyntaxTop(ewsContext);

   /*-----------------*/
   /*  Error checking */
   /*-----------------*/
  if ( cliAdminUserFlagGet()!=L7_TRUE)                                     /* admin user only */
  {
    /* This is a hidden command, to be input only by official
       support personnel.  Indicate a failure to the user but
       do not state what the correct syntax is.  If they are
       authorized, they would know the correct syntax. */
    cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_Input_2);
  }

   /* get switch ID based on presence/absence of STACKING package */
   unit = cliGetUnitId();
   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   numArg = cliNumFunctionArgsGet();

   /* Error Checking for Number of Arguments */
   if (numArg != 1)
   {
       /* This is a hidden command, to be input only by official
          support personnel.  Indicate a failure to the user but
          do not state what the correct syntax is.  If they are
          authorized, they will know. */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_Input_1);
  }

   if (strlen(argv[index+argMode]) >= sizeof(buf))
   {
     /* This is a hidden command, to be input only by official
        support personnel.  Indicate a failure to the user but
        do not state what the correct syntax is.  If they are
        authorized, they would know the correct syntax. */
     cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_Input_2);
   }
   /* Checks for the mode */

   /* FUTURE_FUNCTION:  use a scrambled password or key */
  if ((strcmp(argv[index+argMode], pStrInfo_common_Enbl_2) == 0) ||
      (strcmp(argv[index+argMode], pStrInfo_common_Dsbl2) == 0))
   {

       current_handle  = cliCurrentHandleGet ();

       /* 'techsupport <enable/disable>' */
    if (strcmp(argv[index+argMode], pStrInfo_common_Enbl_2) == 0)
      {

        if ( cliGetCharInputID() == CLI_INPUT_EMPTY && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
        {
          cliSyntaxTop(ewsContext);
          cliSetCharInputID(1, ewsContext, argv);

        osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_TechSupport_1, argv[index+argMode]);
          cliAlternateCommandSet(buf);

        return pStrInfo_base_VerifyTechSupportAccessReq;
        }

        if ( tolower(cliGetCharInput()) != 'y' && ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
        {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_base_TechSupportAccessDenied );
      }

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
            loginSessionSupportFeatureHiddenModeSet(cliCommon[current_handle].userLoginSessionIndex,L7_ENABLE);
      }
    }

    else if (strcmp(argv[index+argMode],pStrInfo_common_Dsbl2) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
             loginSessionSupportFeatureHiddenModeSet(cliCommon[current_handle].userLoginSessionIndex,L7_DISABLE);
      }
    }
      /* Check for configuration of dynamic VLAN error*/

   }
   else
  { /* Error condition for invalid mode */
       /* This is a hidden command, to be input only by official
          support personnel.  Indicate a failure to the user but
          do not state what the correct syntax is.  If they are
          authorized, they will know. */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_Input_1);
   }

   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

   return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  save all settings to nvram
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
* @cmdsyntax  [Privilged Exec]> copy system:running-config nvram:startup-config
*
* @cmdhelp Save current settings to NVRAM
*
* @cmddescript
*   To permanently save configuration changes to NVRAM.
*
* @end
*
*********************************************************************/
const L7_char8  *commandCopySystemRunningConfigNvramStartupConfig(EwsContext ewsContext,
                                                                  L7_uint32 argc,
                                                                 const L7_char8 * * argv,
                                                                  L7_uint32 index)
{
  L7_uint32 numArgs, unit;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CopySysRunningCfgNvramStartupCfg);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() == CLI_INPUT_EMPTY )
  {
    cliSetCharInputID(1, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_CopySysRunningCfgNvramStartupCfgCmd);
    ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_MsgMgmtIntfsUnAvailable);
    return pStrErr_common_CopySysRunningCfgNvramStartupCfgMsg;
  }
  else if ( cliGetCharInputID() == 1 )
  {
    if ( tolower(cliGetCharInput()) == 'y' )
    {                                                                     /* yes */
      dispProcessingIndicator();                    /* Copying might take some time. Start processing indicator timer. */
      usmDbSwDevCtrlSaveConfigurationSet(unit, 1);  /*  inside usmdb now nvStoreSave(); saveConfigPermanently(); */
     if (pProcessingIndicator != L7_NULLPTR )
     {
        osapiTimerFree(pProcessingIndicator);   /* Free processing indicator timer */
     }
      ewsTelnetWrite(ewsContext, pStrInfo_common_MsgCfgSaved);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    }
    else
    {                                                                     /* no */
      ewsTelnetWrite(ewsContext, pStrErr_common_MsgCfgNotSaved);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    }
    cliSyntaxBottom(ewsContext);
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Callback for the ping session
*
* @param EwsContext ewsContext
* @param L7_ushort16 seqNo
* @param L7_uint32 rtt
*
* @returns L7_SUCCESS
*
* @notes
*       Prints response for one probe
*
* @end
*
*********************************************************************/
L7_RC_t pingCallback( void * ctxt, L7_ushort16 seqNo, L7_uint32 rtt, L7_uint32 type, L7_uchar8 *srcIp )
{
  L7_char8 str[L7_CLI_MAX_STRING_LENGTH];
  if (type == PING_PROBE_RESP_TYPE_UNREACH)
  {
    sprintfAddBlanks(1, 0, 0, 0, L7_NULLPTR,str,pStrInfo_base_ReplyFrom,srcIp,pStrInfo_base_PingDestUnreachable );
  }
  else
  {

    sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, str,pStrInfo_base_ReplyFromForIcmpSeqTimeUsec, srcIp,seqNo, rtt);
  }
  cliWrite( str );
  return L7_SUCCESS;
}

void dispProcessingIndicator()
{
  cliWrite("#");
  osapiTimerAdd ( (void *)dispProcessingIndicator,L7_NULL,L7_NULL,2000,&pProcessingIndicator);
}

/*********************************************************************
*
* @purpose  see if a remote computer is alive
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
* @cmdsyntax  [User Exec]>      ping <ip address|hostname> [count <count>]
*                                    [interval <interval>][size <size>]
*             [Privilged Exec]> ping <ip address|hostname> [count <count>]
*                                    [interval <interval>][size <size>]
*
* @cmdhelp Send pings to a specified IP address.
*
* @cmddescript
*   The switch provides a ping utility that you can use to check
*   connectivity between devices in a network. To use ping, the switch
*   must be configured correctly for network (in-band) connection.
*   The source and target devices must have the ping utility enabled and
*   running on top of TCP/IP. The switch can be pinged from any IP
*   workstation with which the switch is connected through the Default
*   VLAN (VLAN 1) (as long as there is a physical path between the switch
*   and the workstation). The terminal interface allows you to send one
*   ping, three pings or a continuous ping (one every second) to the
*   target station.
*
* @end
*
*********************************************************************/
const L7_char8 *commandPing(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 ipAddr_check = L7_NULL;
  L7_char8 str[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 hostAddressStr[L7_DNS_HOST_NAME_LEN_MAX + 1];
  L7_ushort16 handle;
  L7_ushort16 probeSent, probeSucc, probeFail;
  L7_uint32   minRtt, maxRtt, avgRtt;
  L7_ushort16 probeCount = PING_DEFAULT_PROBE_COUNT;
  L7_ushort16 probeInterval = PING_DEFAULT_PROBE_INTERVAL;
  L7_ushort16 probeSize = PING_DEFAULT_PROBE_SIZE;
  L7_BOOL operStatus;
  L7_uint32 vrfId = L7_VALID_VRID;
  L7_char8    domainName[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_RC_t     rc = L7_FAILURE;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_inet_addr_t inetAddr;

  L7_uint32 unit, numArgs;

  cliSyntaxTop(ewsContext);
  

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs > 6 || ((numArgs % 2) != 0) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Ping_2);
  }

  OSAPI_STRNCPY_SAFE(hostAddressStr, argv[index]);

  while ( numArgs > 0 )
  {
    strcpy( buf, argv[ index + 1 ] );
    if (cliCheckIfInteger((L7_char8 *)argv[index+2]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ( strcmp( buf, L7_PING_PACKET_COUNT ) == 0 )
    {
      probeCount = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, L7_PING_INTERVAL ) == 0 )
    {
      probeInterval = atoi((L7_char8 *)argv[index+2]);
    }

    if ( strcmp( buf, L7_PING_PACKET_SIZE ) == 0 )
    {
      probeSize = atoi((L7_char8 *)argv[index+2]);
    }

    numArgs -= 2;
    index += 2;
  }

  /* validate the ip address as ipv4*/
  if (cliIPHostAddressValidate(ewsContext, hostAddressStr, &ipAddr_check, &addrType) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }
  if (addrType != L7_IP_ADDRESS_TYPE_IPV4)
  {
    inetAddressReset(&inetAddr);
    rc = usmDbDNSClientInetNameLookup(L7_AF_INET, hostAddressStr, &status,
                                       domainName, &inetAddr);
    if (rc == L7_SUCCESS)
    {
      inetAddressGet(L7_AF_INET, &inetAddr, &ipAddr_check);
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR,
             ewsContext, pStrErr_base_DnsLookupFailed);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( vrfId == L7_VR_NO_VRID )
  {
    sprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, buf, pStrErr_base_VirtualRtrContext );
    ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    return cliSyntaxReturnPrompt (ewsContext, buf );
  }

  if ( probeSize > PING_MAX_SIZE )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_PingSize, PING_MIN_SIZE, PING_MAX_SIZE );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }
  if ( probeCount > PING_MAX_PROBES || probeCount < PING_MIN_PROBES )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_PingProbeCount, PING_MIN_PROBES, PING_MAX_PROBES );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }
  if ( probeInterval > PING_MAX_INTERVAL || probeInterval < PING_MIN_INTERVAL )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR,  buf, pStrErr_base_PingIntvl_1, PING_MIN_INTERVAL, PING_MAX_INTERVAL );
    return cliSyntaxReturnPrompt (ewsContext, buf);
  }

  sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, str,pStrInfo_base_PingingWithBytesOfData, hostAddressStr, probeSize);
  cliWrite(str);
  memset(str,0,sizeof(str));
  cliWrite(pStrInfo_common_CrLf);
  if ( usmDbPingStart( "", "", L7_TRUE, vrfId, ipAddr_check, probeCount, probeSize, probeInterval, pingCallback, NULL, &ewsContext, &handle ) != L7_SUCCESS )
  {
    sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, str, pStrErr_base_PingFail );
  }
  else
  {
    if ( usmDbPingQuery(handle, &operStatus, &probeSent, &probeSucc, &probeFail, &minRtt, &maxRtt, &avgRtt ) != L7_SUCCESS )
    {
      sprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, str, pStrErr_base_PingFail );
    }
    else
    {
      sprintfAddBlanks (1, 1, 0, 0, L7_NULLPTR, str,pStrInfo_base_PingStats,hostAddressStr);
      ewsTelnetWrite( ewsContext, str);
      osapiSnprintfAddBlanks (0, 1, 0, 0, L7_NULLPTR, str, sizeof(str), pStrInfo_base_PktsTxedPktsRcvdPktLoss, probeSent, probeSucc,(probeSent == 0) ? 0 : (probeSent-probeSucc)*100/probeSent );
      ewsTelnetWrite( ewsContext, str);
      osapiSnprintf(str, sizeof(str), pStrInfo_base_RoundTripMsecMinAvgMax, minRtt/PING_RTT_MULTIPLIER, avgRtt/PING_RTT_MULTIPLIER, maxRtt/PING_RTT_MULTIPLIER );
    }
    usmDbPingSessionFree( handle );
  }

  return cliSyntaxReturnPrompt (ewsContext, str);
}

/*********************************************************************
*
* @purpose  reset the switch
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
* @cmdsyntax [Privilged Exec]> reload
*
* @cmdhelp Reset the switch.
*
* @cmddescript
*   You can reset the switch without powering it off. Reset means that
*   all network connections are terminated and the boot code executes.
*   The switch uses the stored configuration to initialize the switch.
*   You are prompted for confirmation if you want the reset to proceed.
*   A successful reset is indicated by the LEDs on the switch.
*
* @end
*
*********************************************************************/
const L7_char8 *commandReload(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_int32 rc;
  L7_BOOL hasDataChanged;
  L7_BOOL clearConfig;
  L7_uint32 unit, numArgs;
  enum
  {
    saveChangesQuestion = 1,
    resetSystemQuestion = 2
  };

  /* NOTE: No need to check the value of `unit` as
   *       this command is only valid on a standalone switch
   *       having ID `U_IDX` (=> 1).
   *
   *       In a stacking environment, commandSwitchReload() is used.
   */
  unit = cliGetUnitId();
  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {        /*check to for invalid # of arguments */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_Reload_1);
  }

  cliAlternateCommandSet(pStrErr_common_ReloadCmd);
  usmDbSysapiClearConfigFlagGet(&clearConfig);
  switch (cliGetCharInputID())
  {
  case saveChangesQuestion:
    if ( tolower(cliGetCharInput()) == 'y' )
    {
      if (clearConfig == L7_TRUE)
      {
        rc = osapiFsDeleteFile(SYSAPI_CONFIG_FILENAME);
        clearConfig = L7_FALSE;
      }
      usmDbSwDevCtrlSaveConfigurationSet(unit, 1); /*  inside usmdb now nvStoreSave(); saveConfigPermanently(); */
      ewsTelnetWrite(ewsContext, pStrInfo_common_MsgCfgSaved);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_MsgSysRestart);
      ewsFlushAll(ewsContext);
      osapiSleep(1);                    /* allow the buffers to get sent out before a reset */

      rc = usmDbSwDevCtrlResetSet(unit, 2);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrErr_common_MsgCfgNotSaved);
      cliSetCharInputID(resetSystemQuestion, ewsContext, argv);
      return pStrInfo_base_QuestionResetSys;
    }
    break;

  case resetSystemQuestion:
    if ( tolower(cliGetCharInput()) == 'y' )
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_MsgSysRestart);
      ewsFlushAll(ewsContext);
      osapiSleep(1);                     /* allow the buffers to get sent out before a reset */

      rc = usmDbSwDevCtrlResetSet(unit, 2);

    }
    break;

  default:
    usmDbUnsavedConfigCheck(unit, &hasDataChanged);
    if (hasDataChanged == L7_TRUE || clearConfig == L7_TRUE)
    {
      cliSetCharInputID(saveChangesQuestion, ewsContext, argv);
      return pStrInfo_base_QuestionSaveChgs;
    }
    else
    {
      cliSetCharInputID(resetSystemQuestion, ewsContext, argv);
      return pStrInfo_base_QuestionResetSys;
    }
    break;
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  turn HTTP web access on or off
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
* @cmdsyntax  [no] ip http server
*
* @cmdhelp Enable/Disable the Web interface.
*
* @cmddescript
*   Used to enable or disable access to the switch through the Web
*   interface. When enabled, you can login to the switch from the
*   Web interface. When disable is selected, you cannot login to
*   the switch's Web server. Specifying Disable provides for more
*   secure access to the switch. The default is Enable.
*   Note: Disabling the Web interface will not disable Web sessions
*   that are in progress; no new Web sessions will be started.
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpServer(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{

 L7_uint32 unit;
 L7_uint32 numArg;
 L7_uint32 val;
 L7_uint32 rc;

 cliSyntaxTop(ewsContext);
 unit = cliGetUnitId();

 numArg = cliNumFunctionArgsGet();

 /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

 if (ewsContext->commType == CLI_NORMAL_CMD)
 {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpHttpSrvr);
    }

    val = L7_ENABLE;

 }
 else if (ewsContext->commType == CLI_NO_CMD)
 {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpHttpSrvrNo);
    }

    val = L7_DISABLE;
 }
 else
 {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpHttpSrvr);
 }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
   rc = usmDbSwDevCtrlWebMgmtModeSet(unit, val);

   if (rc != L7_SUCCESS && ewsContext->commType == CLI_NORMAL_CMD)
   {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_HttpSrvr);
   }
   else if (rc != L7_SUCCESS && ewsContext->commType == CLI_NO_CMD)
   {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_HttpSrvrNo);
   }
 }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enable or disable java mode
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes To configure the javamode for the box. Either Enable or disable
*
*
* @cmdsyntax network javamode
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpJava(EwsContext ewsContext, L7_uint32 argc,
                                  const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 mode = 0;
  L7_uint32 numArg;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgIpHttpJava);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgNoIpHttpJava);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
      (usmDbWebJavaModeSet(0, mode) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_HttpJava);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Set the maximum number of allowable http sessions
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
* @cmdsyntax  ip http session maxsessions <0-16>
*
* @cmdhelp Set the maximum number of allowable http sessions.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpMaxSessions(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 max;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_PrintCfgHttpMaxSessions, 0, FD_HTTP_DEFAULT_MAX_CONNECTIONS);
    }
    else
    {
      (void) cliConvertTo32BitUnsignedInteger(argv[index+1], &max);
    }
  }
  else /* ewsContext->commType == CLI_NO_CMD) */
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHttpMaxSessionsNo);
    }
    else
    {
      max = FD_HTTP_DEFAULT_MAX_CONNECTIONS;
    }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbCliWebHttpNumSessionsSet(max) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_HttpMaxSessions);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Set the http session soft timeout
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
* @cmdsyntax  ip http session soft-timeout <0-60>
*
* @cmdhelp Set the http session soft timeout in minutes.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpSoftTimeout(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 val;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                              pStrErr_base_PrintCfgHttpSoftTimeout,
                    FD_HTTP_SESSION_SOFT_TIMEOUT_MIN,
                    FD_HTTP_SESSION_SOFT_TIMEOUT_MAX);
    }
    else
    {
      (void) cliConvertTo32BitUnsignedInteger(argv[index+1], &val);
    }
  }
  else /* ewsContext->commType == CLI_NO_CMD) */
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHttpSoftTimeoutNo);
    }
    else
    {
      val = FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
    }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbCliWebHttpSessionSoftTimeOutSet(val) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_HttpSoftTimeout);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Set the http session hard timeout
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
* @cmdsyntax  ip http session hard-timeout <0-168>
*
* @cmdhelp Set the http session hard timeout in hours.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpHardTimeout(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 val;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                              pStrErr_base_PrintCfgHttpHardTimeout,
                    FD_HTTP_SESSION_HARD_TIMEOUT_MIN,
                    FD_HTTP_SESSION_HARD_TIMEOUT_MAX);
    }
    else
    {
      (void) cliConvertTo32BitUnsignedInteger(argv[index+1], &val);
    }
  }
  else /* ewsContext->commType == CLI_NO_CMD) */
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgHttpHardTimeoutNo);
    }
    else
    {
      val = FD_HTTP_SESSION_HARD_TIMEOUT_DEFAULT;
    }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbCliWebHttpSessionHardTimeOutSet(val) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_HttpHardTimeout);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*******************************************************************n
*
* @purpose  to quit a serial or telnet session
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns NULL
*
* @notes none
*
* @cmdsyntax [User Exec]>quit
*
* @cmdhelp  Exit this session. Any unsaved changes are lost.
*
* @cmddescript
*   This command will end a telnet session or reset a serial connection.
*   When you have finished using the terminal interface, ensure you
*   have saved and applied all configuration changes before you log out.
*
* @end
*
*********************************************************************/
const L7_char8 *commandQuit(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL   hasDataChanged;
  L7_BOOL   clearConfig;
  L7_char8  currentUser[L7_LOGIN_SIZE];
  L7_char8  checkChar;
  L7_uint32 unit, numArgs;
  L7_uint32 sessionIndex;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_Quit);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  usmDbUnsavedConfigCheck(unit, &hasDataChanged);
  sessionIndex = cliLoginSessionIndexGet();
  usmDbLoginSessionUserGet(unit, sessionIndex, currentUser);
  usmDbSysapiClearConfigFlagGet(&clearConfig);

  if (( (hasDataChanged == L7_TRUE) || (clearConfig == L7_TRUE)) && (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) == L7_TRUE))
  {
    if ( cliGetCharInputID() == 1 )
    {
      checkChar = tolower(cliGetCharInput());
      if ( checkChar == 'y' )
      {                                                                   /* yes */
        usmDbSwDevCtrlSaveConfigurationSet(unit, 1);
        ewsTelnetWrite(ewsContext, pStrInfo_common_MsgCfgSaved);
        cliSyntaxBottom(ewsContext);
      }
      else if ( checkChar == 'n')                                         /* no */
      {
        ewsTelnetWrite(ewsContext, pStrErr_common_MsgCfgNotSaved);
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        return cliSyntaxReturnPrompt(ewsContext, pStrErr_common_MsgCfgNotSaved);
      }
    }
    else
    {
      cliSetCharInputID(1, ewsContext, argv);
      cliAlternateCommandSet(pStrErr_base_QuitCmd);
      return pStrInfo_base_QuestionSaveChgs;
    }
  }

  /* Log the user logouts in the command logger */
  cliWebCmdLoggerEntryAdd(pStrErr_base_QuitCmd, FD_WEB_DEFAULT_MAX_CONNECTIONS+1);

  cliContextLogout(ewsContext);
  return " ";
}

/*********************************************************************
*
* @purpose  save all settings to nvram
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
* @cmdsyntax  [Privilged Exec]> write memory
*
* @cmdhelp Save current settings to NVRAM
*
* @cmddescript
*   To permanently save configuration changes to NVRAM.
*
* @end
*
*********************************************************************/
const L7_char8  *commandWriteMemory(EwsContext ewsContext, L7_uint32 argc,
                                   const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArgs, unit;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ( numArgs > 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_Write);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() == CLI_INPUT_EMPTY )
  {
    cliSetCharInputID(1, ewsContext, argv);

    cliAlternateCommandSet(pStrErr_base_WriteMemoryCmd);
    ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_MsgMgmtIntfsUnAvailable);
    return pStrErr_common_CopySysRunningCfgNvramStartupCfgMsg;
  }
  else if ( cliGetCharInputID() == 1 )
  {
    if ( tolower(cliGetCharInput()) == 'y' )
    {                                                       /* yes */
      usmDbSwDevCtrlSaveConfigurationSet(unit, 1);          /*  inside usmdb now nvStoreSave(); saveConfigPermanently(); */
      ewsTelnetWrite(ewsContext, pStrInfo_common_MsgCfgSaved);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    }
    else
    {                                                        /* no */
      ewsTelnetWrite(ewsContext, pStrErr_common_MsgCfgNotSaved);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    }
    cliSyntaxBottom(ewsContext);
  }
  return cliPrompt(ewsContext);
}

#ifdef LVL7_DEBUG_BREAKIN
/*********************************************************************
*
* @purpose  Enable/Disable console break-in.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
L7_NO_OPTIONAL_PARAMS* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  [Privilged Exec]> [no] console break-in
*
* @cmdhelp Enable or Disable the console break-in.
*
* @cmddescript
*   To enable or disable console break-in.
*
* @end
*
*********************************************************************/
const L7_char8  *commandSupportConsoleBreakin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{

  L7_uint32 unit, numArgs;
  L7_RC_t   rc = 0;
  L7_int32 current_handle;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,
                                           ewsContext, pStrErr_common_UnitId_1);
  }

  current_handle = cliCurrentHandleGet ();

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArgs != L7_NULL)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                             ewsContext, pStrErr_base_ConsoleBreakin);
    }

    /*******Check if the Flag is Set for Execution***********/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbConsoleBreakinModeSet(L7_TRUE, current_handle); /*Enable console break-in*/
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArgs != L7_NULL)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                             ewsContext, pStrErr_base_NoConsoleBreakin);

    }
    /*******Check if the Flag is Set for Execution***********/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
     rc = usmDbConsoleBreakinModeSet(L7_FALSE, current_handle);  /*Disable console break-in*/
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_base_ConsoleBreakin);

  }

  /*******Check if the Flag is Set for Execution************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  if (rc != L7_SUCCESS)
  {
    cliSyntaxNewLine( ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,
                                           ewsContext, pStrErr_common_ConsoleBreakinMode);
  }

  /*************Set Flag for Script Successful****/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}
/*********************************************************************
*
* @purpose changing the console break-in string.
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
* @cmdsyntax  [Privilged Exec]> [no] console break-in
*
* @cmdhelp change the console breakin string.
*
* @cmddescript
*   To change the console break-in string.
*
* @end
*
*********************************************************************/
const L7_char8  *commandSupportConsoleBreakinString(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 **argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 tempPw[L7_PASSWORD_SIZE];
  L7_uint32 unit;
  L7_char8 currentUser[L7_LOGIN_SIZE];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_common_UnitId_1);
  }

  cliSyntaxTop(ewsContext);

  memset(tempPw, 0x00, sizeof(tempPw));
  usmDbLoginSessionUserGet(unit,cliLoginSessionIndexGet(), currentUser);

  if (cliSecurityLevelCheck(READWRITE, currentUser) != L7_TRUE)
  {
    cliSyntaxTop(ewsContext);
    ewsTelnetWrite(ewsContext, pStrErr_base_AuthorizedCheckedFailed);
    return cliPrompt(ewsContext);
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(cliNumFunctionArgsGet() == 0)
    {
      osapiStrncpy( tempPw, argv[index], sizeof(tempPw));
      if (strlen(tempPw) >= L7_PASSWORD_SIZE)
      {
        ewsTelnetWrite(ewsContext,pStrErr_base_InvalidPasswd);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      if (usmDbStringPrintableCheck(tempPw) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_PasswdFormat);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
     }
    }
    else
    {
     return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                            ewsContext, pStrErr_base_ConsoleBreakInString);

    }

    rc = usmDbConsoleBreakinstringSet(cliCurrentHandleGet(),tempPw);
    if( rc == L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, pStrInfo_common_BreakinStringChanged);
    }
    else
    {
      ewsTelnetWrite( ewsContext, "Internal error");
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
     if(cliNumFunctionArgsGet() == 0)
     {
       memset(tempPw, 0x00, sizeof(tempPw));
       /* set to default break-in string*/
       strcpy(tempPw, CLI_SUPPORTCONSOLE_DEFAULTBREAKIN);
       usmDbConsoleBreakinstringSet(cliCurrentHandleGet(),tempPw);
     }
     else
     {
       return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                              ewsContext, pStrErr_base_NoConsoleBreakInString);
     }

  }
  return cliPrompt(ewsContext);
}
#endif
