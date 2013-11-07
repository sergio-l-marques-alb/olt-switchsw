/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cliutil.c
*
* @purpose assorted functions for cli
*
* @component user interface
*
* @comments none
*
* @create   06/08/2000
*
* @author  Forrest Samuels and Qun He
* @end
*
**********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include <stdlib.h>
#include <errno.h>

#include "clicommands.h"
#include "clicommands_card.h"
#include "clicommands_usermgr.h"
#include "cliutil.h"
#include "unitmgr_api.h"
#include "log.h"
#include "ews.h"
#include "ews_cli.h"
#include "osapi_support.h"
#include "cli_web_include.h"

#include "ewnet.h"
#include "cliapi.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "diffserv_exports.h"
#include "snmp_exports.h"
#include "timezone_exports.h"
#include "user_manager_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_timezone_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"

#ifdef L7_CHASSIS
#include "usmdb_slotmapper.h"
#endif

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
  #include "clicommands_tunnel.h"
#endif
#endif

#ifdef _L7_OS_LINUX_
  #include "l7_socket.h"
#else
#include "vxworks_config.h"       /* for DEFAULT_MAX_CONNECTIONS */
#endif /* _L7_OS_LINUX_ */

#include "config_script_api.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_user_mgmt_api.h"
#include "usmdb_filter_api.h"
#include "usmdb_registry_api.h"

cliUtil_t cliUtil;
L7_char8 userName[L7_LOGIN_SIZE];                                         /*find define*/
L7_char8 userNameSSH[L7_LOGIN_SIZE];                                      /*find define*/

L7_uint32 cliEscapeSeq[] = {
  UPARROW,                                                                /* UP */
  DOWNARROW,                                                              /* DOWN */
  RIGHTARROW,                                                             /* RIGHT */
  LEFTARROW,                                                              /* LEFT */
  0
};

L7_uint32 cliEscapeSeqRedirect[] = {
  EWS_CLI_CHAR_UP,                                                        /* UP */
  EWS_CLI_CHAR_DOWN,                                                      /* DOWN */
  EWS_CLI_CHAR_FORWARD,                                                   /* RIGHT */
  EWS_CLI_CHAR_BACKWARD,                                                  /* LEFT */
  0
};

extern const L7_uint32 daysInMonth[];
extern const L7_uchar8 *monthsOfYear[];
extern const L7_uchar8 *daysOfWeek[];
extern cliWebCfgData_t cliWebCfgData;
extern int isalpha(int c);
extern int isspace(int c);

/********************************************************************
*
* @purpose convert all letters inside a buffer (char8) to lower case
*
* @param L7_char8 *buf
*
* @returns  void
*
* @notes   This f(x) returns the same letter in the same buffer but all
*          lower case, checking the buffer for empty string
* @end
*
*********************************************************************/
void cliConvertToLowerCase(L7_char8 * buf)
{
  L7_char8 c_tmp;
  L7_uint32 i;
  for ( i = 0; i < strlen(buf); i++ )
  {
    if ( buf[i] != '\n' || buf[i] != '\0' )
    {
      c_tmp = (L7_char8)  tolower(buf[i]);
      buf[i] = c_tmp;
    }
  }
  return;
}

/*********************************************************************
*
* @purpose convert all letters inside a buffer (char8) to capital case
*
* @param L7_char8 *buf
*
* @returns  L7_SUCCESS  It was able to convert all chars to capital case
* @returns  L7_FAILURE  should never return failure
*
* @notes   This f(x) returns the same letter in the same buffer but all
*          capital case, checking the buffer for empty string
* @end
*
*********************************************************************/
L7_RC_t cliConvertToUpperCase(L7_char8 * buf)
{
  L7_char8 c_tmp;
  L7_uint32 i;
  for ( i = 0; i < strlen(buf); i++ )
  {
    if ( buf[i] != '\n' || buf[i] != '\0' )
    {
      c_tmp = (L7_char8)  toupper(buf[i]);
      buf[i] = c_tmp;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose check to see if all characters inside a buffer(char8) are integers
*
* @param L7_char8 *buf
*
* @returns  L7_SUCCESS  means that all chars are integers
* @returns  L7_FAILURE  means that there exist some char that is not an integer.
*
* @notes This f(x) checks each letter inside the buf to make certain
         it is an integer
* @end
*
*********************************************************************/
L7_RC_t cliCheckIfInteger(L7_char8 * buf)
{
  L7_uint32 i;
  L7_uint32 j = 0;

  if (buf != '\0')
   {
      if (buf[j] == '-')
      {
         j=1;
         /* If the string  starts with '-', then there should be at least one digit
            following '-' like -1, or, -20. */
      if (strlen(buf) == j)
      {                     /* No digit after '-' */
        return L7_FAILURE;
      }
      }

      for (i = j; i < strlen(buf) && buf != '\0'; i++)
      {
         if ((buf[i] < '0' || buf[i] > '9') && (buf != '\0'))
         {
            return L7_FAILURE;
         }
      }
   }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To check if the entered value is float type.
*
* @param L7_char8 *buf
*
* @returns  L7_SUCCESS  Upon finding the entered value is float.
* @returns  L7_FAILURE  No.
*
* @notes This f(x) searches for decimal point to declare the entered 
*        value as float.
* @end
*
*********************************************************************/
L7_RC_t cliCheckIfFloat(L7_char8 * buf)
{
  L7_uint32 i,j=0,strLen=0;
  L7_RC_t rc=L7_FAILURE;
  L7_BOOL decPoint = L7_FALSE;
  
  if (L7_NULLPTR == buf )
  {
    return L7_FAILURE;
  }

  strLen = strlen(buf);

  if( strLen == L7_NULL )
  {
    return L7_FAILURE;
  }

  if( '-' == buf[j] )
  {
    j = 1;
    if( strLen == j)
    {
      return L7_FAILURE;
    }
  }

  for(i=j; i<strLen; i++)
  {
    if(  buf[i] == '.' )
    {
      decPoint = L7_TRUE;
      continue;
    }
    else if( buf[i]>='0' && buf[i]<='9' ) 
    {
      continue;
    }
    else
    {
      break;
    }
  }

  if( i == strLen && decPoint == L7_TRUE )
  {
    rc = L7_SUCCESS;
  }

  return rc;
}
/*********************************************************************
*
* @purpose set the depth, store previous depth, set up prompts, handle incorrect input
*
*
* @param @b{pointer} to mmc_fabric struct
*
* @returntype L7_char8 *
* @returns cliPrompt(ewsContext)
* @returns NULL
*
* @notes this function is called for every command that acts like a directory
*
*
* @end
*
*********************************************************************/
const L7_char8 *cliDepth(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext)
{

  /*
   * If "exit", restore depth to upper level
   */

  if ( argc != 0 && (strcmp(argv[argc-1], pStrInfo_common_Exit) == 0) && (depth != 0) )
  {
    ewsCliDepth(ewsContext, (depth-1), argv);
    cliPrevDepthSet(depth-1);

    memset(cliCommon[cliUtil.handleNum].prompt, 0, sizeof(cliCommon[cliUtil.handleNum].prompt));
    if ( (depth-1) == 0 )
    {
      osapiSnprintf(cliCommon[cliUtil.handleNum].prompt,sizeof(cliCommon[cliUtil.handleNum].prompt),"%s%s",cliUtil.systemPrompt, pStrInfo_base_RootPrompt);
    }
    else
    {
      osapiSnprintf(cliCommon[cliUtil.handleNum].prompt,sizeof(cliCommon[cliUtil.handleNum].prompt),"%s%s",cliUtil.systemPrompt, tmpUpPrompt);
    }
    return cliPrompt(ewsContext);
  }
  else if ( argc != 0 && (strcmp(argv[argc-1], pStrInfo_base_End) == 0)  && (depth != 0) )
  {
    ewsCliDepth(ewsContext, 0, argv);
    cliPrevDepthSet(0);
    osapiSnprintf(cliCommon[cliUtil.handleNum].prompt,sizeof(cliCommon[cliUtil.handleNum].prompt),"%s%s",cliUtil.systemPrompt, pStrInfo_base_RootPrompt);
    return cliPrompt(ewsContext);
  }
  else if ( argc == depth )
  {
    osapiSnprintf(cliCommon[cliUtil.handleNum].prompt,sizeof(cliCommon[cliUtil.handleNum].prompt),"%s%s",cliUtil.systemPrompt, tmpPrompt);
    ewsCliDepth(ewsContext, depth, argv);
    cliPrevDepthSet(depth);
    return cliPrompt(ewsContext);
  }
  /* to "hide" devshell */
  else if ( (strcmp(argv[0], pStrInfo_base_Devshell_1) == 0) && (depth == 0) &&           /* only run devshell from root */
      (cliAdminUserFlagGet()==L7_TRUE) )                          /* admin user only */
  {
    L7_BOOL runDevShell = L7_TRUE;

    /* Only run devshell on
       1. the serial port console
       2. telnet connections enabled for the support console display */
    if (cliTelnetConnectionGet() == L7_TRUE)
    {
      /* This is a telnet session */
      if (cliCommon[cliUtil.handleNum].debugDisplayCtl.consoleTraceDisplayEnabled != L7_TRUE)
      {
        runDevShell = L7_FALSE;
      }
    }

    if (runDevShell == L7_TRUE)
  {
    return commandDevShell(ewsContext, argc, argv, depth);
  }
    else
    {
      ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
      ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);  /* then just return previous prompt */
      return cliCommon[cliUtil.handleNum].prompt;
    }
  }
  /* to "hide" techsupport */
  else if ( (strcmp(argv[0], pStrInfo_base_TechSupport_1) == 0) && (depth == 0) &&           /* only run techsupport from root */
      (cliAdminUserFlagGet()==L7_TRUE) )                           /* admin user only */
  {
    return commandTechSupport(ewsContext, argc, argv, depth);
  }
  else                                                                    /* Error Prompt */
  {
    ewsTelnetWriteAddBlanks (2, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_CfgTrapFlagsOspf);
    ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);  /* then just return previous prompt */
    return cliCommon[cliUtil.handleNum].prompt;
  }
}

/*********************************************************************
*
* @purpose set and return the current prompt
*
*
* @param EwsContext ewsContext
*
* @returntype L7_char8 *
* @returns cliCommon[cliUtil.handleNum].prompt
*
* @notes this function keeps the current prompt correct
*
*
* @end
*
*********************************************************************/
L7_char8 *cliPrompt(EwsContext ewsContext)
{
  /* check if the card-trap message has already been displayed */
  if (cliUtil.trapMsgDisplay == L7_TRUE)
  {
    ewsCliTrapNotificationMsgGet(ewsContext);
    cliUtil.trapMsgDisplay = L7_FALSE;
  }

  ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
  return cliCommon[cliUtil.handleNum].prompt;
}

/*********************************************************************
*
* @purpose Indicates if a connnection is to be logged out after the
*          current command completes.
*
* @param EwsContext context
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @end
*
*********************************************************************/
L7_BOOL cliLogoutDeferredGet(EwsContext ewsContext)
{
  if ((L7_NULLPTR != ewsContext) &&
      (L7_NULLPTR != ewsContext->telnet))
  {
    return ewsContext->telnet->logoutDeferred;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Sets a connnection to be logged out after the
*          current command completes.
*
* @param EwsContext context
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @end
*
*********************************************************************/
L7_RC_t cliLogoutDeferredSet(EwsContext ewsContext, L7_BOOL defer)
{
  if ((L7_NULLPTR != ewsContext) &&
      (L7_NULLPTR != ewsContext->telnet))
  {
    ewsContext->telnet->logoutDeferred = defer;
  }
  else
  {
    return L7_ERROR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose to logout the user on the connection in context
*
* @param EwsContext context
*
* @returntype none
*
* @end
*
*********************************************************************/
void cliContextLogout(EwsContext ewsContext)
{
  if (L7_NULLPTR == ewsContext)
  {
    return;
  }

  if (L7_NULLPTR == ewsContext->net_handle)
  {
    ewsNetHTTPAbort(ewsContext);
    return;
  }

  cliCurrentHandleSet(ewsContext->net_handle->handleNum);

  if (TRUE == cliLogoutDeferredGet(ewsContext))
  {
    cliWebCmdLoggerEntryAdd("Deferred logout", cliLoginSessionIndexGet());
    cliLogoutDeferredSet(ewsContext, L7_FALSE);
  }

  if (CLI_SERIAL_HANDLE_NUM == cliCurrentHandleGet())
  {                                                                       /* if serial connection */
    usmDbLoginSessionLogout(U_IDX, cliLoginSessionIndexGet());
    cliResetSerialConnectionSet(L7_TRUE);

    cliBannerDisplay(cliWebCfgData.cliBanner);

#ifdef NOT_IMPLEMENTED_ON_FP
    if (L7_TRUE != userMgrIsAuthenticationRequired(ACCESS_LINE_CONSOLE, ACCESS_LEVEL_LOGIN))
    {
      memset(prompt, 0, sizeof(prompt));
      cliWebGetSystemCommandPrompt(prompt);
      osapiSnprintf(buf, sizeof(buf) - 1, "\r\n%s>", prompt);
      ewsTelnetWrite(ewsContext, buf);
    }
    else if (L7_TRUE != userMgrIsAuthenticationUsernameRequired(ACCESS_LINE_CONSOLE, ACCESS_LEVEL_LOGIN))
    {
      ewsTelnetWrite(ewsContext, LOGIN_PASSWORD_PROMPT);
    }
    else
    {
      ewsTelnetWrite(ewsContext, LOGIN_USER_PROMPT);
    }
#else
    cliWriteSerial(cliUtil.loginUserPrompt);
#endif
  }
  else
  {
    ewsNetHTTPAbort(ewsContext);
  }
}

/*********************************************************************
*
* @purpose to verify and set up user logins
*
*
* @param L7_char8 *enteredInfo, EwsContext context
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes this function is run twice to first get the username then password
* @notes if user and pass match that stored, returns L7_TRUE
* @notes currently is not set up for the system super password
*
*
* @end
*
*********************************************************************/
L7_BOOL cliLoginCheck(L7_char8 * enteredInfo, EwsContext context)
{
  L7_uint32 rc;
  L7_uint32 userIndex;
  static L7_uchar8 challengeState[ L7_USER_MGR_STATE_FIELD_SIZE + 1 ];
  L7_BOOL challengeFlag = L7_FALSE;
  L7_char8 challengePhrase[L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH + 1 ];
  L7_char8 buf[L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH + CLI_MAXSIZE_LOGIN_CHALLENGE_PROMPT + 1];
  L7_char8 commandLog[L7_CLI_MAX_LARGE_STRING_LENGTH];
  L7_uint32 unit;
  L7_inet_addr_t gaddr;
  L7_ACCESS_LINE_t accessLine;
  L7_uint32 challengeFlags = 0;
  L7_uint32 accessLevel =  FD_USER_MGR_GUEST_ACCESS_LEVEL;


  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, context, pStrErr_common_UnitId_1);
    ewsFlushAll(context);
    return L7_FALSE;
  }

  if (cliTelnetConnectionGet() == L7_FALSE)
  {
    accessLine = ACCESS_LINE_CONSOLE;
  }
  else
  {
    accessLine = ACCESS_LINE_TELNET;
  }

  if (usmDbIsAuthenticationUsernameRequired(accessLine, ACCESS_LEVEL_LOGIN) != L7_TRUE)
  {
    cliCommon[cliUtil.handleNum].haveUser = L7_TRUE;
  }

  /* if we do not have a username */
  if ( cliCommon[cliUtil.handleNum].haveUser == L7_FALSE )
  {
    if (strcmp(enteredInfo, pStrInfo_common_EmptyString) == 0)
    {
      /* Display the banner */
      cliBannerDisplay(cliWebCfgData.cliBanner);
      ewsTelnetWrite(context, cliUtil.loginUserPrompt);
      ewsFlushAll(context);
      return L7_FALSE;
    }
    /* Validate length of name */
    if (strlen(enteredInfo) >= L7_LOGIN_SIZE)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_InvalidUsrName, L7_LOGIN_SIZE - 1);
        ewsTelnetWrite(context, buf);
        /* Display the banner */
        cliBannerDisplay(cliWebCfgData.cliBanner);
      ewsTelnetWrite(context, cliUtil.loginUserPrompt);
        ewsFlushAll(context);
        return L7_FALSE;
    }
    OSAPI_STRNCPY_SAFE(userName, enteredInfo);
    cliCommon[cliUtil.handleNum].haveUser = L7_TRUE;

    /* This is not a challenge so reset the challenge state. */
    memset( challengeState, 0, sizeof(challengeState));
    ewsTelnetWrite(context, pStrInfo_base_LoginPasswdPrompt);
    ewsFlushAll(context);
    return L7_FALSE;
  }
  /* if we do have a username and password */
  else
  {
   /* Validate length of password */
   if (strlen(enteredInfo) >= L7_PASSWORD_SIZE)
   {
       /* Allow the user to respecify user id. */
       cliCommon[cliUtil.handleNum].haveUser = L7_FALSE;
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_InvalidPasswd, L7_PASSWORD_SIZE-1 );
       ewsTelnetWrite(context, buf);
       /* Display the banner */
       cliBannerDisplay(cliWebCfgData.cliBanner);
       if (usmDbIsAuthenticationUsernameRequired(accessLine, ACCESS_LEVEL_LOGIN) != L7_TRUE)
        ewsTelnetWrite(context, pStrInfo_base_LoginPasswdPrompt);
       else
        ewsTelnetWrite(context, cliUtil.loginUserPrompt);
       ewsFlushAll(context);
       return L7_FALSE;
    }

    memset( challengePhrase, 0, sizeof(challengePhrase));
    rc = usmDbAuthenticateUserAllowChallenge(unit, userName, enteredInfo,
                                                  L7_USER_MGR_COMPONENT_ID,
                                                  &accessLevel, accessLine,
                                                  ACCESS_LEVEL_LOGIN,
                                                  L7_NULLPTR, L7_NULLPTR, challengeState,
                                                  &challengeFlag, challengePhrase, &challengeFlags);
    if (rc == L7_SUCCESS)
    {
        /* User is authenticated */

        /* if connection is serial */
        if ( cliTelnetConnectionGet() == L7_FALSE )
        {
          /* serial connection login */
          usmDbLoginSessionLogin(unit, &userIndex);
          cliLoginSessionIndexSet(userIndex);
      memset(&gaddr,0,sizeof(gaddr));
          usmDbLoginSessionRemoteIpAddrSet(unit,
                                           cliLoginSessionIndexGet(),
                                           &gaddr);
          usmDbLoginSessionTypeSet(unit, cliLoginSessionIndexGet(), L7_LOGIN_TYPE_SERIAL);
          usmDbLoginSessionUserStorageSet(unit, cliLoginSessionIndexGet(), context);
        }
        usmDbLoginSessionUserSet(unit, cliLoginSessionIndexGet(), userName);

        /* Store access level */
        cliCommon[cliUtil.handleNum].userAccess = accessLevel;                /* for security */

        /* Check for admin user and set flag */
      if (strcmp(userName,  pStrInfo_base_LoginUsrId) == 0 )
      {                                             /* "admin" */
        cliCommon[cliUtil.handleNum].adminUserFlag = L7_TRUE;
      }
      else
      {
        cliCommon[cliUtil.handleNum].adminUserFlag = L7_FALSE;
      }

      /* Challenge handling is complete so reset the challenge state. */
      memset( challengeState, 0, sizeof(challengeState));
        /* If user is authenticated then log the same */
      osapiSnprintf(commandLog,sizeof(commandLog),pStrInfo_base_UsrLoggedIn,userName);
        cliWebCmdLoggerEntryAdd(commandLog, FD_WEB_DEFAULT_MAX_CONNECTIONS+1);
        return L7_TRUE;
    }
    else if (rc == L7_FAILURE && challengeFlag == L7_TRUE )
    {
        /* Handle challenge */
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LoginChallengePrompt, challengePhrase);
        ewsTelnetWrite(context, buf);
        ewsFlushAll(context);
        return L7_FALSE;
    }
    else
    {
        if(rc == L7_REQUEST_DENIED)
        {
          osapiSnprintf(buf,sizeof(buf), "\n\rUser '%s' locked out because of authentication failure\n\r",userName);
          ewsTelnetWrite(context, buf);
        }
        if (usmDbIsAuthenticationUsernameRequired(accessLine, ACCESS_LEVEL_LOGIN) != L7_TRUE)
        {
          cliCommon[cliUtil.handleNum].haveUser = L7_TRUE;
        }
        else
        {
          /* user could not be authenticated */
          cliCommon[cliUtil.handleNum].haveUser = L7_FALSE;
        }

        /* Challenge handling is complete so reset the challenge state. */
        memset( challengeState, 0, sizeof(challengeState));
    }

  }
  /* Display the banner */
  cliBannerDisplay(cliWebCfgData.cliBanner);
  if (usmDbIsAuthenticationUsernameRequired(accessLine, ACCESS_LEVEL_LOGIN) != L7_TRUE)
    ewsTelnetWrite(context, pStrInfo_base_LoginPasswdPrompt);
  else
    ewsTelnetWrite(context, cliUtil.loginUserPrompt);
  ewsFlushAll(context);
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose to verify and set up user logins
*
*
* @param L7_char8 *enteredInfo, EwsContext context
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes this function is run after an SSH login where the password
* @notes and username have already been acquired through SSH
* @notes however a RADIUS challenge may need a response
* @notes if user and pass match that stored, returns L7_TRUE
* @notes currently is not set up for the system super password
*
*
* @end
*
*********************************************************************/
L7_BOOL cliSSHLoginCheck(L7_char8 *enteredInfo, L7_uint32 accessLevel, 
                         L7_char8 *challengePhrase, EwsContext context,
                         L7_uint32 fromIpAddr)
{
    L7_uint32 rc;
    static L7_uchar8 challengeState[L7_USER_MGR_STATE_FIELD_SIZE + 1];
    L7_BOOL challengeFlag = L7_FALSE;
  L7_char8 buf[L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH + CLI_MAXSIZE_LOGIN_CHALLENGE_PROMPT + 1];
    L7_uint32 unit;
    L7_ACCESS_LINE_t accessLine;
    L7_uint32 challengeFlags = 0;

    unit = cliGetUnitId();

    accessLine = ACCESS_LINE_SSH;

    if (usmDbIsAuthenticationUsernameRequired(ACCESS_LINE_SSH, ACCESS_LEVEL_LOGIN) != L7_TRUE)
    {
    /* For ssh, this function is called twice: once for username, once for password.
       If we are coming in to check the username and the username is not required for login,
       just return L7_TRUE. This function will be called a second time to check the password.
    */
      if (cliCommon[cliUtil.handleNum].haveUser == L7_FALSE)
      {
        cliCommon[cliUtil.handleNum].haveUser = L7_TRUE;
        return L7_TRUE;
      }
      cliCommon[cliUtil.handleNum].haveUser = L7_TRUE;
    }

    /* if we do not have a username */
    if (cliCommon[cliUtil.handleNum].haveUser == L7_FALSE)
    {
    OSAPI_STRNCPY_SAFE(userNameSSH, enteredInfo);
        cliCommon[cliUtil.handleNum].haveUser = L7_TRUE;

        /* This is not a challenge so reset the challenge state. */
        memset(challengeState, 0, sizeof(challengeState));
        return L7_FALSE;
    }
    /* if we do have a username and password */
    else
    {
      if (strlen(challengePhrase) == L7_NULL)
      {
        rc = L7_SUCCESS;
      }
      else
      {
        rc = L7_FAILURE;
        challengeFlags= L7_TRUE;
      }
      if (L7_SUCCESS == rc)
        {
            memset(challengePhrase, 0, sizeof(challengePhrase));
            /* User is authenticated */
            usmDbLoginSessionUserSet(U_IDX, cliLoginSessionIndexGet(), userNameSSH);

            /* Store access level */
            cliCommon[cliUtil.handleNum].userAccess = accessLevel; /* for security */

            /* Check for admin user and set flag */
      if (strcmp(userNameSSH, pStrInfo_base_LoginUsrId) == 0)
      {                                                         /* "admin" */
        cliCommon[cliUtil.handleNum].adminUserFlag = L7_TRUE;
      }
      else
      {
        cliCommon[cliUtil.handleNum].adminUserFlag = L7_FALSE;
      }

      /* Challenge handling is complete so reset the challenge state. */
      memset(challengeState, 0, sizeof(challengeState));
            return L7_TRUE;
        }
        else if (rc == L7_FAILURE && challengeFlag == L7_TRUE)
        {
            /* Handle challenge */
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LoginChallengePrompt, challengePhrase);
            ewsTelnetWrite(context, buf);
            ewsFlushAll(context);
            return L7_FALSE;
        }
        else
        {
          if (usmDbIsAuthenticationUsernameRequired(ACCESS_LINE_SSH, ACCESS_LEVEL_LOGIN) != L7_TRUE)
          {
            cliCommon[cliUtil.handleNum].haveUser = L7_TRUE;
          }
          else
          {
            /* user could not be authenticated */
            cliCommon[cliUtil.handleNum].haveUser = L7_FALSE;
          }

            /* Challenge handling is complete so reset the challenge state. */
            memset(challengeState, 0, sizeof(challengeState));
        }
    }
    /* Display the banner */
    cliBannerDisplay(cliWebCfgData.cliBanner);
    if (usmDbIsAuthenticationUsernameRequired(ACCESS_LINE_SSH, ACCESS_LEVEL_LOGIN) != L7_TRUE)
      ewsTelnetWrite(context, pStrInfo_base_LoginPasswdPrompt);
    else
      ewsTelnetWrite(context, cliUtil.loginUserPrompt);
    ewsFlushAll(context);
    return L7_FALSE;
}

/*********************************************************************
*
* @purpose to determine the number of arguments passed to a command
*
*
* @param L7_uint32 argc, L7_uint32 index
*
* @returntype L7_uint32
* @returns (argc - (index + 1))
*
* @notes returns the number of command line arguments after the last command
* @notes that is calculated with the ints argc and index
*
*
* @end
*
*********************************************************************/
L7_uint32 cliNumberOfArguments(L7_uint32 argc, L7_uint32 index)
{
  return argc - (index + 1);
}

/*********************************************************************
* @purpose to allow security checking based on a security level
*
* @param L7_uint32 argc, L7_uint32 index
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes returns L7_TRUE if the user is allowed access
* @notes this allows access if the user has the level passed or higher
* @notes currently a check is only done to see if the user is user #0 (the r/w user)
*
*
* @end
*
*********************************************************************/
L7_BOOL cliSecurityLevelCheck(lvl7SecurityLevels_t level, L7_char8 * userName)
{
  lvl7SecurityLevels_t userLevel = cliUserAccessGet();

  if ( userLevel >= level )
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose generic function used by every command function to set up security, arguments, etc
*
*
* @param EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index
*
* @returntype void
* @returns void
*
* @notes currently only does a security check and determines # of arguments
*
*
* @end
*
*********************************************************************/
void cliExamine(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  cliNumFunctionArgsSet(cliNumberOfArguments(argc, index));

}

/* syntax */
/*********************************************************************
*
* @purpose for adding a space at the top of output functions, for cosmetic purposes
*
*
* @param EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index
*
* @returntype void
* @returns void
*
* @notes prints out a newline
*
*
* @end
*
*********************************************************************/
void cliSyntaxTop(EwsContext ewsContext)
{
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
}

/*********************************************************************
*
* @purpose for adding a space at the bottom of output functions, for cosmetic purposes
*
*
* @param EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index
*
* @returntype void
* @returns void
*
* @notes prints out a newline
*
*
* @end
*
*********************************************************************/
void cliSyntaxBottom(EwsContext ewsContext)
{
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
}

/*********************************************************************
*
* @purpose for printing a new line
*
*
* @param EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index
*
* @returntype void
* @returns void
*
* @notes prints out a newline
*
*
* @end
*
*********************************************************************/
void cliSyntaxNewLine(EwsContext ewsContext)
{
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
}

/*********************************************************************
*
* @purpose adds "..." to the end of output for formating
*
*
* @param EwsContext ewsContext, const L7_char8 *description
*
* @returntype void
* @returns void
*
* @notes this function will print out the L7_char8* passed in with trailing "..."s
*
*
* @end
*
*********************************************************************/
void cliFormatExtended(EwsContext ewsContext, const L7_char8 * description, L7_uint32 format_ind, L7_uint32 format_num)
{
  L7_uint32 charnum;
  L7_uint32 dotnum;
  L7_char8 temp[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 temp1[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 temp2[L7_CLI_MAX_STRING_LENGTH];

  /* dummy is dum, please make sure it is long enough to use */
  const L7_char8 * dummy = "............................................................";

  if ( format_ind >= format_num + 4 )                                     /*use default if input will not work */
  {
    format_ind = FORMAT_IND;
    format_num = FORMAT_NUM;
  }

  charnum = strlen(description);
  if ( charnum>format_ind )
  {
    osapiStrncpySafe(temp,description,min(format_ind+1,sizeof(temp)));
    osapiStrncpySafe(temp2,dummy,min((format_num - format_ind),sizeof(temp2)));
    osapiSnprintf(temp1,sizeof(temp1),"\r\n%s%s ",temp,temp2);
  }
  else
  {
    dotnum = format_num - charnum - 1;
    osapiStrncpySafe(temp,dummy,min(dotnum+1,sizeof(temp)));
    osapiSnprintf(temp1,sizeof(temp1),"\r\n%s%s ",description,temp);
  }

  ewsTelnetWrite(ewsContext,temp1);
}

/*********************************************************************
*
* @purpose adds "..." to the end of output for formating
*
*
* @param EwsContext ewsContext, const L7_char8 *description
*
* @returntype void
* @returns void
*
* @notes this function will print out the L7_char8* passed in with trailing "..."s
*
*
* @end
*
*********************************************************************/
void cliFormat(EwsContext ewsContext, const L7_char8 * description)
{
  cliFormatExtended(ewsContext, description, FORMAT_IND, FORMAT_NUM);
}

/*********************************************************************
*
* @purpose adds "..." to the end of output for formating
*
*
* @param EwsContext ewsContext, const L7_char8 *description
*
* @returntype void
* @returns void
*
* @notes This function will print out the L7_char8* passed in with trailing "..."s
*        This allows for longer "description" and "values".
*
* @end
*
*********************************************************************/
void ipv6CliFormat(EwsContext ewsContext, const L7_char8 * description)
{
  L7_uint32 strLen;
  L7_uint32 strDummyLen;
  L7_char8 strTemp[MAX_STRING_LEN];
  L7_char8 strTemp1[MAX_STRING_LEN];

  /* dummy is dum, please make sure it is long enough to use */
  const L7_char8 * dummy = "..........................................................";

  strLen = strlen(description);
  strDummyLen = strlen(dummy);
  osapiStrncpySafe(strTemp,description,min((strLen+1),sizeof(strTemp)));
  osapiStrncpySafe(strTemp1,dummy,min((strDummyLen - strLen),sizeof(strTemp1)));
  ewsTelnetPrintf (ewsContext, "\r\n%s%s ",strTemp,strTemp1);
}

/*********************************************************************
*
* @purpose adds "..." to the end of a description for formatting
*
*
* @param EwsContext ewsContext, const L7_char8 *description
*
* @returntype void
* @returns void
*
* @notes this function will print out the L7_char8* passed in with
*        trailing "..."s.  The allowed text length is shorter than
*        the text length allowed with cliFormat().  This allows for
*        shorter "description" and longer "values".
*
*
* @end
*
*********************************************************************/
void cliFormatShortText(EwsContext ewsContext, const L7_char8 * description)
{
  cliFormatExtended(ewsContext, description, FORMAT_SHORT_IND, FORMAT_SHORT_NUM);
}

/*********************************************************************
 * @purpose  CLI helper routine to line wrap string output greater
 *           than screen length of 80 characters
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  buf         @b{(input)) pointer to the string to be formatted
 *
 * @returns  void
 *
 * @comments Use this function for the shorter FORMAT_SHORT_NUM description length.
 *
 * @end
 ********************************************************************/
void cliFormatShortStringDisplay(EwsContext ewsContext, L7_char8 * buf)
{
  cliFormatStringDisplayGeneral(ewsContext, buf, FORMAT_SHORT_NUM);
  return;
}

/*********************************************************************
 * @purpose  CLI helper routine to line wrap string output greater
 *           than screen length of 80 characters
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  buf         @b{(input)) pointer to the string to be formatted
 *
 * @returns  void
 *
 * @comments Use this function for the normal FORMAT_NUM description length.
 *
 * @end
 ********************************************************************/
void cliFormatStringDisplay(EwsContext ewsContext, L7_char8 * buf)
{
  cliFormatStringDisplayGeneral(ewsContext, buf, FORMAT_NUM);
  return;
}

/*********************************************************************
 * @purpose  CLI helper routine to line wrap string output greater
 *           than screen length of 80 characters
 *
 * @param  ewsContext  @b{(input)) the context structure
 * @param  buf         @b{(input)) pointer to the string to be formatted
 * @param  descrLen    @b{(input)) length of displayed text description field
 *
 * @returns  void
 *
 * @comments The descrLen value is typically one of FORMAT_NUM or FORMAT_SHORT_NUM.
 *
 * @end
 ********************************************************************/
void cliFormatStringDisplayGeneral(EwsContext ewsContext, L7_char8 * buf, L7_int32 descrLen)
{
  L7_int32 len, lineLen;
  L7_BOOL first = L7_TRUE;
  L7_char8 tmp_1[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tmp_2[L7_CLI_MAX_STRING_LENGTH];

  /* NOTE: In case of strings extending beyond screen size (MAX_STRING_LEN):
   *
   *       Emweb code takes `\n` to be an `enter` and inserts a blank line
   *       between the description. To avoid this blank line, the last
   *       character (MAX_STRING_LEN-1) should be left blank.
   *       Hence, CHARS_LESS_MAX_STRING_LEN is set to 2 to handle comma (if any)
   *       as the second-last character AND the last blank character.
   */
  lineLen = (MAX_STRING_LEN - CHARS_LESS_MAX_STRING_LEN);
  len = lineLen - descrLen;
  if (len <= 0)
  {
    /* caller has not provided enough space to wrap the output, so display it as is */
    ewsTelnetWrite(ewsContext, buf);
    return;
  }

  osapiSnprintf(tmp_1, sizeof(tmp_1), cliProcessStringDisplay(buf, len));

  do
  {
    if (first == L7_TRUE)
    {
      first = L7_FALSE;
    }
    else
    {
      ewsTelnetPrintf (ewsContext, "\r\n%-*s", descrLen, " ");
    }
    osapiSnprintf(tmp_2, sizeof(tmp_2), "%-*s", len+1, tmp_1);
    ewsTelnetWrite(ewsContext, tmp_2);

    osapiSnprintf(tmp_1, sizeof(tmp_1), cliProcessStringDisplay(buf, len));
  } while (strcmp(tmp_1, buf) != 0);

  return;
}

/*********************************************************************
 * @purpose  CLI helper routine to format string output
 *
 * @param  buffer  @b{(input)) pointer to the string to be formatted
 * @param  len     @b{(input)) number of chars to be displayed per line
 *
 * @returns  Formatted string
 *
 * @comments This routine gets called from cliFormatStringDisplayGeneral().
 *
 * @end
 ********************************************************************/
L7_char8 *cliProcessStringDisplay(L7_char8 * buffer, L7_uint32 len)
{
  L7_uint32 charnum, i, bufLength;
  static L7_uint32 length = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 tmpString[L7_CLI_MAX_STRING_LENGTH];

  charnum = strlen(buffer);
  bufLength = len;

  /* check if the string will fit in the alotted space in CLI */
  if (charnum < len)
  {
    return buffer;
  }

  memset (buf, 0, (L7_int32)sizeof(buf));
  if (length < charnum)
  {
    for (i = 0; i <= len; i++)
    {
      /* if not end of string */
      if (buffer[length] != '\0')
      {
        /* skip first char in line, if space */
        if ((i != 0) || (buffer[length] != ' '))
        {
          buf[i] = buffer[length];
          length++;
        }
        else
        {
          buf[i] = buffer[length+1];
          length += 2;
        }
      }
      else
      {
        length++;
        break;
      }
    }

    if (length < charnum)
    {
      while ((buf[bufLength] != ' ') && (buf[bufLength] != ','))
      {
        bufLength--;
        length--;
      }
    }
  }
  else
  {
    length = 0;
    return buffer;
  }

  osapiStrncpySafe(tmpString, buf, min(bufLength+2, (sizeof(tmpString)-1)));
  return tmpString;
}
/*********************************************************************
*
* @purpose sets all variables to default for any unit
*
*
* @param void
*
* @returntype void
* @returns void
*
* @notes none
*
*
* @end
*
*********************************************************************/
void cliPhaseTwoInitVars()
{
  L7_uint32 unit;
  L7_uint32 i = 0;
  L7_uchar8 buf[L7_PROMPT_SIZE];

  cliUtil.resetSerialConnection = L7_FALSE;                               /* flag to reset connection on next input */
  cliUtil.transferInProgress = L7_FALSE;

  if (usmDbUnitMgrNumberGet(&unit) != L7_SUCCESS)
  {
      unit = 1;
  }

  osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, cliUtil.systemPrompt,sizeof(cliUtil.systemPrompt),pStrInfo_base_Unit, unit);

  /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
  for ( ; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++ )
  {
    cliInitConnection(i);
  }

  cliCommon[CLI_SERIAL_HANDLE_NUM].adminUserFlag = L7_TRUE;

  osapiSnprintf(buf,sizeof(buf),"%s%s", cliUtil.systemPrompt, pStrInfo_base_RootPrompt);
  cliWriteSerial(buf);

}

/*********************************************************************
*
* @purpose sets all variables for mgmt unit only
*
*
* @param void
*
* @returntype void
* @returns void
*
* @notes none
*
*
* @end
*
*********************************************************************/
void cliPhaseThreeInitVars()
{
  L7_uint32 unit;
  L7_uint32 rc;
  L7_uint32 i = 0;
  L7_char8 buf[81];

  cliUtil.resetSerialConnection = L7_TRUE;                               /* flag to reset connection on next input */
  cliUtil.transferInProgress = L7_FALSE;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  memset (cliUtil.systemPrompt, 0, L7_PROMPT_SIZE);
  rc = usmDbCommandPromptGet(unit, buf);
  osapiSnprintf(cliUtil.systemPrompt, sizeof(cliUtil.systemPrompt), "\r\n(%s) ", buf);

  /*old SSH TELNET TAG REMOVE "_TOTAL"_CONNECTIONS*/
  for ( ; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++ )
  {
    cliInitConnection(i);
  }
  /* Restore the banner file in config structure */
  cliWebSystemReadBannerFile(CLI_BANNER_FILE_NAME);
  /* Display the banner */
  cliBannerDisplay(cliWebCfgData.cliBanner);
  cliWriteSerial(cliUtil.loginUserPrompt);

  osapiStrncpySafeAddBlanks (1, 0, 0, 0, L7_NULLPTR, cliUtil.loginUserPrompt, pStrInfo_base_LoginUsrPrompt, sizeof(cliUtil.loginUserPrompt));
}

/*********************************************************************
*
* @purpose sets variables to default needed for each new connection
*
*
* @param L7_uint32 handle
*
* @returntype void
* @returns void
*
* @notes none
*
*
* @end
*
*********************************************************************/
void cliInitConnection(L7_uint32 handle)
{

  osapiSnprintf(cliCommon[handle].prompt,sizeof(cliCommon[handle].prompt),"%s%s",cliUtil.systemPrompt, pStrInfo_base_RootPrompt);  /* set up the root prompt */

  cliCommon[handle].scroll = L7_FALSE;                                    /* do we need to scroll or not? */
  cliCommon[handle].haveUser = L7_FALSE;                                  /* has a username been stored? */
  cliCommon[handle].charInputID = CLI_INPUT_EMPTY;                        /* ID associated with current character input (0 for none */
  cliCommon[handle].stringInputID = CLI_INPUT_EMPTY;                      /* ID associated with current string input (0 for none) */
  cliCommon[handle].stringIndex = 0;                                      /* index of next position in string buffer */
  cliCommon[handle].stringPassword = L7_FALSE;                            /* specifies string input is a password */
  cliCommon[handle].prevDepth = 0;                                        /* saves the depth for 'root' execution of commands */
  cliCommon[handle].functionArguments = 0;                                /* number of arguments passed into function */
  cliCommon[handle].userLoginSessionIndex = -1;                           /* index into loginsession information, -1 is invalid default */
  cliCommon[handle].scrollLine = CLI_MAX_SCROLL_LINES;                    /* current scroll line number (0 based) */
  cliCommon[handle].scrollLineMax = CLI_MAX_SCROLL_LINES;                 /* max number of lines */
  cliCommon[handle].devshellActive = L7_FALSE;
  cliCommon[handle].isScroll = L7_TRUE;
  cliCommon[handle].debugDisplayCtl.consoleTraceDisplayEnabled = L7_FALSE;               /*Resets oper. parm for showing support (internal) traces &/or devshell on new telnet session*/
  cliCommon[handle].debugDisplayCtl.supportConsoleDisplayEnabled = L7_FALSE;               /*Stops showing support (internal) traces &/or devshell on new telnet session*/
  cliCommon[handle].debugDisplayCtl.debugConsoleDisplayEnabled = L7_FALSE;               /*Stops showing debug (external) traces on new telnet session*/
  cliCommon[handle].loginStatus = L7_FALSE;
  cliCommon[handle].passwdExpStatus = L7_FALSE;
  cliCommon[handle].callCount = 0;
}

/*********************************************************************
 *
 * @purpose sets variables to default needed for each new connection
 *
 *
 * @param L7_uint32 handle
 *
 * @returntype void
 * @returns void
 *
 * @notes none
 *
 *
 * @end
 *
 *********************************************************************/
void cliDebugSessionInfoDump(L7_uint32 handle)
{

  osapiSnprintf (cliCommon[handle].prompt, sizeof(cliCommon[handle].prompt), "%s%s",cliUtil.systemPrompt, pStrInfo_base_RootPrompt);  /* set up the root prompt */

  sysapiPrintf("cliCommon[handle].telnetConnection = %d\n", cliCommon[handle].telnetConnection );
  sysapiPrintf("cliCommon[handle].scroll = %d\n", cliCommon[handle].scroll );
  sysapiPrintf("cliCommon[handle].haveUser = %d\n", cliCommon[handle].haveUser );
  sysapiPrintf("cliCommon[handle].allowAccess = %d\n", cliCommon[handle].allowAccess );
  sysapiPrintf("cliCommon[handle].charInputID = %d\n", cliCommon[handle].charInputID );
  sysapiPrintf("cliCommon[handle].stringInputID = %d\n", cliCommon[handle].stringInputID );
  sysapiPrintf("cliCommon[handle].stringIndex = %d\n",  cliCommon[handle].stringIndex );
  sysapiPrintf("cliCommon[handle].stringPassword = %d\n",cliCommon[handle].stringPassword );
  sysapiPrintf("cliCommon[handle].prevDepth = %d\n", cliCommon[handle].prevDepth );
  sysapiPrintf("cliCommon[handle].functionArguments = %d\n", cliCommon[handle].functionArguments );
  sysapiPrintf("cliCommon[handle].userAccess = %d\n", cliCommon[handle].userAccess );
  sysapiPrintf("cliCommon[handle].adminUserFlag = %d\n", cliCommon[handle].adminUserFlag );
  sysapiPrintf("cliCommon[handle].userLoginSessionIndex = %d\n", cliCommon[handle].userLoginSessionIndex );
  sysapiPrintf("cliCommon[handle].scrollLine = %d\n",cliCommon[handle].scrollLine );
  sysapiPrintf("cliCommon[handle].scrollLineMax = %d\n",cliCommon[handle].scrollLineMax );
  sysapiPrintf("cliCommon[handle].socket = %d\n",cliCommon[handle].socket );
  sysapiPrintf("cliCommon[handle].devshellActive = %d\n", cliCommon[handle].devshellActive );
  sysapiPrintf("cliCommon[handle].charInput = %d\n", cliCommon[handle].charInput );
  sysapiPrintf("cliCommon[handle].isScroll = %d\n", cliCommon[handle].isScroll );
  sysapiPrintf("cliCommon[handle].debugDisplayCtl.consoleTraceDisplayEnabled = %d\n",
      cliCommon[handle].debugDisplayCtl.consoleTraceDisplayEnabled);
  sysapiPrintf("cliCommon[handle].debugDisplayCtl.supportConsoleDisplayEnabled = %d\n",
      cliCommon[handle].debugDisplayCtl.supportConsoleDisplayEnabled );
  sysapiPrintf("cliCommon[handle].debugDisplayCtl.debugConsoleDisplayEnabled = %d\n",cliCommon[handle].debugDisplayCtl.debugConsoleDisplayEnabled );
}

/*********************************************************************
*
* @purpose checks to see if alternate input is pending
*
*
* @param void
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes alternate input is L7_TRUE if a L7_char8 or string ID has been set (!= 0)
*
*
* @end
*
*********************************************************************/
L7_BOOL cliCheckAltInput()
{
  if ( cliCommon[cliUtil.handleNum].charInputID   != CLI_INPUT_EMPTY ||
       cliCommon[cliUtil.handleNum].stringInputID != CLI_INPUT_EMPTY )
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose reset charInputID and data holder
*
*
* @param void
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes set charInput to NULL and reset the ID to 0
*
*
* @end
*
*********************************************************************/
void cliClearCharInput()
{
  cliCommon[cliUtil.handleNum].charInput = 0;
  cliCommon[cliUtil.handleNum].charInputID = CLI_INPUT_EMPTY;

}

/*********************************************************************
*
* @purpose reset stringInputID, data array, index and password flag
*
*
* @param void
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes bzero's out the string array and resets all flasgs and placeholders
*
*
* @end
*
*********************************************************************/
void cliClearStringInput()
{
  memset (cliCommon[cliUtil.handleNum].stringInput, 0, (L7_int32)sizeof(cliCommon[cliUtil.handleNum].stringInput));
  cliCommon[cliUtil.handleNum].stringInputID = CLI_INPUT_EMPTY;
  cliCommon[cliUtil.handleNum].stringIndex = 0;
  cliCommon[cliUtil.handleNum].stringPassword = L7_FALSE;
}

/*********************************************************************
*
* @purpose put 1 char in to the charInput array and set datap to an enter
*
*
* @param void
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes "\n" makes EmWeb code think that an enter was sent after we capture the char
*
*
* @end
*
*********************************************************************/
void cliProcessCharInput(L7_char8 * datap)
{
  L7_char8 charBuffer[2];

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
  {
    /* copy our input character to the cliCommon structure */
    cliCommon[cliUtil.handleNum].charInput = *datap;
    /* null terminate the string (1 char) */
    memset(charBuffer, 0, sizeof(charBuffer));
    *charBuffer = *datap;
    /* send the string (1 char) to the screen */
    if (cliGetCharInputID() != CLI_INPUT_NOECHO)
    {
      cliWrite(charBuffer);
    }
    /* write a return or newline so the user knows input was taken */
    cliWrite(pStrInfo_common_CrLf);
    /* replace the input with an enter so it looks like that is what the user hit */
    *datap = '\n';
  }
}

/*********************************************************************
*
* @purpose put up to 80 chars in to the stringInput array and set datap to an enter
*
*
* @param void
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes "\n" makes EmWeb code think that an enter was sent after we capture the string
* @notes returns L7_TRUE when an enter is encountered or the array is full
* @notes also handles backspaces and keeps track of location in array with stringIndex
*
*
* @end
*
********************************************************************/
L7_BOOL cliProcessStringInput(L7_char8 * datap)
{
  if ( cliGetStringInputID() != CLI_INPUT_EMPTY )
  {
    if ( cliCommon[cliUtil.handleNum].stringIndex >= L7_CLI_MAX_STRING_LENGTH-1 ||
        *datap == '\n' || *datap == '\r' )
    {
      cliCommon[cliUtil.handleNum].stringInput[cliCommon[cliUtil.handleNum].stringIndex]=0;
      *datap = '\n';
    }
    else
    {
      if ((*datap == '\b')||(*datap =='\177'))
      {
        if ( cliCommon[cliUtil.handleNum].stringIndex > 0 )
        {
          cliCommon[cliUtil.handleNum].stringIndex--;
          /* Note: strlen() of this string is 3, not 4, so need +1 for the n parm */
          osapiStrncpySafe(datap, "\b \b\0", strlen("\b \b\0")+1);
        }
        else
        {
          *datap = '\0';
        }
      }
      else
      {
        if ( *datap >= ' ' && *datap <= 'z' )
        {
          cliCommon[cliUtil.handleNum].stringInput[cliCommon[cliUtil.handleNum].stringIndex] = *datap;
          cliCommon[cliUtil.handleNum].stringIndex++;
          /* *(datap+1) = '\0'; */
        }
        else
        {
          *datap = '\0';
        }
      }
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose For use when the output for the CLI is greater than
*          L7_CLI_MAX_STRING_LENGTH.  This will put up to 80 chars in
*          to the stringOutput array and can be called within a loop to
*          get the next line of the output buffer.
*
*
* @param void
*
* @returntype L7_char8 *
* @returns
*
* @notes "\n" makes EmWeb code think that an enter was sent after we capture the string
* @notes look where it is already used for examples on how to use it -
* @notes used in show traplog, show protocol summary, show protocol detailed
*
* @end
*
********************************************************************/
L7_char8 *cliProcessStringOutput(L7_char8 * outputLine, L7_uint32 outputLength)
{
  L7_uint32 charnum;
  L7_uint32 x, bufLength;   /*bufLength is outputLength*/
  static L7_uint32 length = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 tmpString[L7_CLI_MAX_STRING_LENGTH];

  memset(tmpString, 0, sizeof(tmpString));
  charnum = strlen(outputLine);
  memset(buf, 0, sizeof(buf));
  bufLength = outputLength;

  if (charnum < outputLength)                 /*the string will fit in the alotted space in the CLI*/
  {
    return outputLine;
  }

  if (length < charnum)
  {
    for ( x = 0; x <= outputLength; x++ )
    {
      if (outputLine[length] != '\0')         /*the end of the string has not been reached */
      {
        if ( (x != 0) || (outputLine[length] != ' ') )    /*if the first character in the line is a space, skip it*/
        {
          buf[x] = outputLine[length];
          length++;
        }
        else
        {
          buf[x] = outputLine[length+1];
          length += 2;
        }
      }
      else
      {
        length++;
        break;
      }
    }
    if (length < charnum)
    {
      while ((buf[bufLength] != ' ') && (buf[bufLength] != ','))
      {
        bufLength--;
        length--;
      }
    }
  }
  else
  {
    length = 0;
    return outputLine;
  }

  osapiStrncpy(tmpString, buf, min((bufLength+1), (sizeof(tmpString)-1)));  /* bufLength + 1 inorder to capture the comma. */
  return tmpString;

}

/*********************************************************************
*
* @purpose For use when 2 rows in the CLI both need to wrap.
*
*
* @param void
*
* @returntype L7_char8 *
* @returns
*
* @notes used in show sysinfo
*
* @end
*
********************************************************************/
L7_char8 *cliCombineStringOutput(L7_char8 * outputLine1, L7_uint32 outputLength1, L7_char8 * outputLine2, L7_uint32 outputLength2)
{
  L7_uint32 charnum1, charnum2;
  L7_uint32 x, bufLength1, bufLength2;   /*bufLength is outputLength*/
  static L7_uint32 length1 = 0;
  static L7_uint32 length2 = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 tmpString1[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 tmpString2[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 finalString[L7_CLI_MAX_STRING_LENGTH];

  memset(finalString, 0, sizeof(finalString));
  memset(tmpString1, 0, sizeof(tmpString1));
  memset(tmpString2, 0, sizeof(tmpString2));
  charnum1 = strlen(outputLine1);
  charnum2 = strlen(outputLine2);
  memset(buf, 0, sizeof(buf));
  bufLength1 = outputLength1;
  bufLength2 = outputLength2;

  if ((length1 >= charnum1) && (length2 >= charnum2))
  {
    length1 = 0;
    length2 = 0;
    return outputLine1;
  }

  if ((charnum1 < outputLength1) && (charnum2 < outputLength2))                 /*the strings will fit in the alotted space in the CLI*/
  {
    OSAPI_STRNCPY_SAFE(tmpString1, outputLine1);
    while (strlen(tmpString1) != outputLength1)
    {
      OSAPI_STRNCAT(tmpString1, " ");             /*add enough spaces to fill up the column*/
    }
    OSAPI_STRNCAT(tmpString1, "  ");              /*most cli summary shows have 2 spaces b/w columns*/
    OSAPI_STRNCAT(tmpString1, outputLine2);
    length1 = charnum1;
    length2 = charnum2;
    return tmpString1;
  }

  if (length1 < charnum1)
  {
    for ( x = 0; x < outputLength1; x++ )
    {
      if (outputLine1[length1] != '\0')         /*the end of the string has not been reached */
      {
        if ( (x != 0) || (outputLine1[length1] != ' ') )    /*if the first character in the line is a space, skip it*/
        {
          buf[x] = outputLine1[length1];
          length1++;
        }
        else
        {
          buf[x] = outputLine1[length1+1];
          length1 += 2;
        }
      }
      else
      {
        length1++;
        break;
      }
    }
    if (length1 < charnum1)
    {
      /* Initiallly bufLength1 is the length of the first output field */
      while ( (bufLength1 > 0 ) &&
              ((buf[bufLength1-1] != ' ') && (buf[bufLength1-1] != ',')))
      {
        bufLength1--;
        length1--;
      }

      if ( bufLength1 == 0 )
      {
        /* This string does not contain a space or a comma.
           Set bufLength1 back to max output size.  String
           will be wrapped at max width.
          **/

        bufLength1 = outputLength1;
        length1 = outputLength1;
      }
    }

    osapiStrncpy(tmpString1, buf, min(bufLength1, (sizeof(tmpString1)-1)));  /* bufLength + 1 inorder to caputre the comma. */
    while (strlen(tmpString1) != outputLength1)
    {
      OSAPI_STRNCAT(tmpString1, " ");             /*add enough spaces to fill up the column*/
    }

  }
  else
  {
    memset (tmpString1, 0, L7_CLI_MAX_STRING_LENGTH);
    for (x = 0; x < outputLength1; x++)
    {
      OSAPI_STRNCAT(tmpString1, " ");             /*add enough spaces to fill up column before 2nd row*/
    }
  }

  memset (buf, 0, sizeof(buf));
  if (length2 < charnum2)
  {
    for ( x = 0; x < outputLength2; x++ )
    {
      if (outputLine2[length2] != '\0')         /*the end of the string has not been reached */
      {
        if ( (x != 0) || (outputLine2[length2] != ' ') )    /*if the first character in the line is a space, skip it*/
        {
          buf[x] = outputLine2[length2];
          length2++;
        }
        else
        {
          buf[x] = outputLine2[length2+1];
          length2 += 2;
        }
      }
      else
      {
        length2++;
        break;
      }
    }
    if (length2 < charnum2)
    {

      /* Initiallly bufLength2 is the length of the second output field */
      while ( (bufLength2 > 0 ) &&
              ((buf[bufLength2-1] != ' ') && (buf[bufLength2-1] != ',')))
      {
        bufLength2--;
        length2--;
      }

      if ( bufLength2 == 0 )
      {
        /* This string does not contain a space or a comma.
           Set bufLength2 back to max output size.  String
           will be wrapped at max width.
          **/

        bufLength2 = outputLength2;
        length2 = outputLength2;
      }
    }
    osapiStrncpy(tmpString2, buf, min(bufLength2, (sizeof(tmpString2)-1)));  /* bufLength + 1 inorder to caputre the comma. */

  }

  OSAPI_STRNCAT(tmpString1, "  ");                 /*most cli summary shows have 2 spaces b/w columns*/
  OSAPI_STRNCAT(tmpString1, tmpString2);
  osapiSnprintf(finalString, sizeof(finalString), tmpString1);
  return finalString;

}

/*********************************************************************
*
* @purpose outputs the buffers for scrolling if more then 1 page is to be printed out
*
*
* @param EwsContext context, L7_uchar8* datap, L7_uint32 *bytesp
*
* @returntype L7_uint32
* @returns 0
*
* @notes none
*
*
* @end
*
********************************************************************/
L7_uint32 cliProcessScrollInput(EwsContext context, L7_char8 * datap, L7_uint32 * bytesp)
{
  L7_char8 * scrollData;
  L7_uint32 i;

  if (( strncmp(datap, pStrInfo_base_Q_1, 1) == 0 ) ||
      ( datap[0] == 26 ))                                   /* if quit recieved */
  {
    *bytesp = *bytesp - 1;
    cliWrite(pStrInfo_common_CrLf);
    /* print the last line of the scroll buffer only -- the prompt */
    scrollData = &cliCommon[cliUtil.handleNum].scrollBuffer[cliCommon[cliUtil.handleNum].scrollLineMax][0];
    cliWrite(scrollData);

    cliCommon[cliUtil.handleNum].scroll = L7_FALSE;
    cliCommon[cliUtil.handleNum].scrollLine = CLI_MAX_SCROLL_LINES;

    for ( i = 0; i < SCROLL_BUF_LINES; i++ )
    {                                                                     /* scrollBuffer[240][80] */
      memset (cliCommon[cliUtil.handleNum].scrollBuffer[i], 0, SCROLL_BUF_LINE_LENGTH);
    }

    return 0;   /* we don't need to process any more characters, so return 0 */
  }

  *bytesp = *bytesp - 1;
  cliWrite(pStrInfo_common_CrLf);
  cliScrollBufferFlush();
  return 0;
}

/*********************************************************************
*
* @purpose return the string aquired from cliProcessStringInput()
*
*
* @param void
*
* @returntype L7_char8 *
* @returns tmpString
*
* @notes none
*
*
* @end
*
********************************************************************/
L7_char8 *cliGetStringInput()
{
  static L7_char8 tmpString[L7_CLI_MAX_STRING_LENGTH];

  OSAPI_STRNCPY_SAFE(tmpString, cliCommon[cliUtil.handleNum].stringInput);
  cliClearStringInput();
  return tmpString;
}

/*********************************************************************
*
* @purpose return the string aquired from cliProcessCharInput()
*
*
* @param void
*
* @returntype L7_char8
* @returns tmpChar
*
* @notes none
*
*
* @end
*
********************************************************************/
L7_char8 cliGetCharInput()
{
  static L7_char8 tmpChar;

  tmpChar = 0;

  tmpChar= cliCommon[cliUtil.handleNum].charInput;
  cliClearCharInput();
  return tmpChar;
}

/*********************************************************************
*
* @purpose set the char input ID for multiple single char answer questions
*
*
* @param void
*
* @returntype void
* @returns void
*
* @notes this function also sets the depth to root so you can execute the command again from root
*
*
* @end
*
********************************************************************/
void cliSetCharInputID(L7_uint32 id, EwsContext ewsContext, const L7_char8 * * argv)
{
  ewsCliDepth(ewsContext, 0, argv);
  cliCommon[cliUtil.handleNum].charInputID = id;
}

/*********************************************************************
*
* @purpose set the string input ID for multiple string answer questions
*
*
* @param void
*
* @returntype void
* @returns void
*
* @notes this function also sets the depth to root so you can execute the command again from root
*
*
* @end
*
********************************************************************/
void cliSetStringInputID(L7_uint32 id, EwsContext ewsContext, const L7_char8 * * argv)
{
  cliClearStringInput();
  ewsCliDepth(ewsContext, 0, argv);
  cliCommon[cliUtil.handleNum].stringInputID = id;
}

/*********************************************************************
*
* @purpose return the current char ID
*
*
* @param void
*
* @returntype L7_uint32
* @returns cliCommon[cliUtil.handleNum].charInputID
*
* @notes none
*
*
* @end
*
********************************************************************/
L7_uint32 cliGetCharInputID()
{
  return cliCommon[cliUtil.handleNum].charInputID;
}

/*********************************************************************
*
* @purpose return the current string ID
*
*
* @param void
*
* @returntype L7_uint32
* @returns cliCommon[cliUtil.handleNum].stringInputID
*
* @notes none
*
*
* @end
*
********************************************************************/
L7_uint32 cliGetStringInputID()
{
  return cliCommon[cliUtil.handleNum].stringInputID;
}

/*********************************************************************
*
* @purpose return L7_TRUE if the input is a password and should be treated differently
*
*
* @param void
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes none
*
*
* @end
*
********************************************************************/
L7_BOOL cliGetStringPassword()
{
  return cliCommon[cliUtil.handleNum].stringPassword;
}

/*********************************************************************
*
* @purpose set stringPassword L7_BOOL to L7_TRUE for a password input
*
*
* @param void
*
* @returntype void
* @returns void
*
* @notes none
*
*
* @end
*
********************************************************************/
void cliSetStringPassword()
{
  cliCommon[cliUtil.handleNum].stringPassword = L7_TRUE;
}

/* for checking scroll L7_BOOL */
L7_BOOL cliCheckScrollInput()
{

  if ( cliCommon[cliUtil.handleNum].scroll == L7_TRUE )
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}

void cliScrollBufferFlush()
{
  L7_char8 * scrollData;
  L7_uint32 i;

  for ( i = 0; i < CLI_MAX_SCROLL_LINES-1; i++ )
  {
    scrollData = &cliCommon[cliUtil.handleNum].scrollBuffer[cliCommon[cliUtil.handleNum].scrollLine][0];

    cliWrite(scrollData);

    /* if we are done flushing ALL buffers then set scrolling to L7_FALSE, reset scrollLine,
     * and clear out all buffers for that handleNum
     */

    cliCommon[cliUtil.handleNum].scrollLine++;

    if ( cliCommon[cliUtil.handleNum].scrollLine > (cliCommon[cliUtil.handleNum].scrollLineMax) )
    {
      cliCommon[cliUtil.handleNum].scroll = L7_FALSE;
      cliCommon[cliUtil.handleNum].scrollLine = CLI_MAX_SCROLL_LINES;

      for ( i = 0; i < SCROLL_BUF_LINES; i++ )
      {                                                                   /* scrollBuffer[240][80] */
        memset (cliCommon[cliUtil.handleNum].scrollBuffer[i], 0, SCROLL_BUF_LINE_LENGTH);
      }

      return;
    }
  }
  cliWrite(pStrInfo_common_ScrollMsg);
}

void cliWriteSerial(L7_char8 * string)
{
#ifdef _L7_OS_VXWORKS_
  extern int consoleFd;   /* fd of initial console device      */
  write(consoleFd, string, strlen(string));
#else
  int consolefd = cliWebIORedirectStdOutGet();

  if (consolefd != -1)
  {
    if(0 > write(consolefd, string , strlen(string))){}
  }
  else
  {
    fwrite(string ,sizeof(L7_char8), strlen(string), stdout);
    fflush(stdout);
  }
#endif
}

/*********************************************************************
*
* @purpose writes text string to client
*
*
* @param L7_char8
*
* @returntype void
* @returns void
*
* @notes This is unbuffered write
*
*
* @end
*
********************************************************************/

void cliWrite(L7_char8 * string)
{
  L7_int32    string_len;
  L7_int32    totalBytesSent=0;
  L7_int32    bytesSent;

  if ( cliUtil.handleNum == CLI_SERIAL_HANDLE_NUM )                       /* serial port */
  {
    cliWriteSerial(string);
  }
  else
  {
    string_len = strlen(string);
    /* added cliCommon[cliUtil.handleNum].socket for socket 6.27.00 */
    while (totalBytesSent < string_len)
    {
      if ((bytesSent = send( cliCurrentSocketGet(),
                             string     + totalBytesSent,
                             string_len - totalBytesSent,
                             0 ))   < 0 )
      {
         L7_uint32   error;
         error = osapiErrnoGet();
         if (error == EWOULDBLOCK)
         {
           osapiSleepMSec(50);
         }
         else
         {
           L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "Telnet Send Failed errno = 0x%x. "
                           "Failed to send text string to the telnet client.\r\n", error);
           break;
         }
      }
      else
      {
        totalBytesSent = totalBytesSent + bytesSent;
      }
    }
  }
}

L7_BOOL cliEscapeSequence(L7_char8 * datap, L7_uint32 * bytesp, L7_BOOL logged_in)
{
  static L7_uint32 escseq       = 0;
  static L7_uint32 key          = 0;
  static L7_uint32 j            = 0;
  L7_uint32    numBytes         = 0;
  L7_char8 * tmpDatap        = NULL;

  numBytes = *bytesp;
  tmpDatap = datap;

  /*make variable for every socket*/

  do
  {
    if ( escseq == 2 )
    {
      *bytesp = *bytesp - 1;
      if ( numBytes > 1 )
      {
        tmpDatap = tmpDatap + sizeof(L7_char8);
      }
      else
      {
        tmpDatap = tmpDatap;
      }
      key = (key << 8) | *tmpDatap;

      for ( j = 0; cliEscapeSeq[j]; j++ )
      {
        if ( key == cliEscapeSeq[j] && logged_in == L7_TRUE )
        {
          *datap     = cliEscapeSeqRedirect[j];
          escseq     = 0;
          key        = 0;
          j          = 0;
          return L7_FALSE;
        }
      }
      escseq = 0;
      key    = 0;
      j      = 0;
      return L7_TRUE;
    }

    if ( escseq == 1 )
    {
      *bytesp = *bytesp - 1;
      if ( numBytes > 1 )
      {
        tmpDatap = tmpDatap + sizeof(L7_char8);
      }
      else
      {
        tmpDatap = tmpDatap;
      }
      key = (key << 8) | *tmpDatap;

      if ( *tmpDatap != ARROWESC && *tmpDatap != FUNCTIONESC )
      {
        *bytesp = 1;
        *datap  = *tmpDatap;
        escseq  = 0;
        key     = 0;
        return L7_TRUE;
      }

      escseq = 2;
    }

    if ( *tmpDatap == ESC )
    {
      *bytesp = *bytesp - 1;
      escseq = 1;
      key = *tmpDatap;
    }

  }
  while ( *bytesp !=0 && escseq !=0 );

  if ( escseq !=0 )
  {
    return L7_TRUE;
  }
  else
  {
    *bytesp = 1;
    return L7_FALSE;
  }
}

L7_RC_t cliSlotPortToInterface(const L7_char8 * unit_slot_port,
    L7_uint32 * unit,
    L7_uint32 * slot,
    L7_uint32 * port,
    L7_uint32 * intIfNum)
{
  L7_char8 * now;
  L7_char8 * charSlot;
  L7_char8 * charPort;
  L7_char8 tempSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;

  OSAPI_STRNCPY_SAFE(tempSlotPort, unit_slot_port);

  now = strchr(tempSlotPort, '/');

  if ((now != NULL) && ((now+1) != NULL))
  {
    charPort = now+1;
    now[0]='\0';
    charSlot = tempSlotPort;
    if ((cliConvertTo32BitUnsignedInteger(charPort, &val) == L7_SUCCESS) &&
        (cliConvertTo32BitUnsignedInteger(charSlot, &val) == L7_SUCCESS) &&
        (strcmp(now, tempSlotPort) != 0))
    {
      *slot = atoi(charSlot);
      *port = atoi(charPort);
      *unit = cliGetUnitId();

      if (*slot <= L7_MAX_SLOTS_PER_UNIT)
      {
        return usmDbIntIfNumFromUSPGet(*unit, *slot, *port, intIfNum);
      }
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose to find the 0 based index of the SNMP Community
*
* @param  name
*
* @notes none
*
* @cmdsyntax
*
* @cmdhelp
*
* @cmddescript
*
* @end
*********************************************************************/
L7_int32 cliFindSNMPCommunityIndex(L7_char8 * name)
{
  /* can also use simGetSnmpCommCommunityIndex */
  L7_char8 commName[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 index;
  L7_uint32 rc, unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return CLI_INDEX_NOT_FOUND;
  }

  for ( index = 0; index < L7_MAX_SNMP_COMM; index++ )
  {
    rc = usmDbSnmpCommunityNameGet(unit, index, commName);
    if ( strcmp(commName, name) == 0 )
    {
      return index;
  }
  }
  return CLI_INDEX_NOT_FOUND;
}

/*********************************************************************
* @purpose  to find the first free 0 based index of an SNMP Community
*
* @param void
*
* @notes the first free one will be one with a status of 3 (DELETE)
*
* @end
*
*********************************************************************/
L7_int32 cliFindEmptySNMPCommunityIndex()
{
  L7_uint32 index;
  L7_uint32 status;
  L7_uint32 rc, unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return CLI_INDEX_NOT_FOUND;
  }

  for ( index = 0; index < L7_MAX_SNMP_COMM; index++ )
  {
    rc = usmDbSnmpCommunityStatusGet(unit, index, &status);
    if ( status == L7_SNMP_COMMUNITY_STATUS_DELETE)
    {
      return index;
    }
  }
  return -1;
}

/*********************************************************************
* @purpose  to find the 0 based index of an SNMP Trap Community
*
* @param name - trap community name
* @param ip - trap community ip
*
* @returns  -1  no snmp community exist
* @returns  An + Integer means that there exist a snmp community
*
* @notes
*
* @end
*
*********************************************************************/
L7_int32 cliFindSNMPTrapCommunityIndex(L7_char8 * name, L7_uint32 ipAddr)
{
  L7_char8 commName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 commIp;
  L7_int32 index;
  L7_RC_t rc, unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return CLI_INDEX_NOT_FOUND;
  }

  for ( index = 0; index < L7_MAX_SNMP_COMM; index++ )
  {
    rc = usmDbTrapManagerCommIpGet(unit, index, commName);
    rc = usmDbTrapManagerIpAddrGet(unit, index, &commIp);

    if ( (strcmp(commName, name) == 0) && (commIp == ipAddr) )
    {
      return index;
  }

  }
  return -1;
}

/*********************************************************************
 * @purpose  to find the 0 based index of an SNMP Trap Community
 *
 * @param name - trap community name
 * @param ip - trap community ip
 *
 * @returns  -1  no snmp community exist
 * @returns  An + Integer means that there exist a snmp community
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_int32 cliFindSNMPTrapCommunityV6Index(L7_char8 * name, L7_uchar8 * v6Addr)
{
  L7_char8 commName[L7_CLI_MAX_STRING_LENGTH];

  L7_int32 index;
  L7_RC_t rc, unit;

  L7_in6_addr_t ipv6Addr;
  L7_uchar8 str_addr[40];

  memset(&ipv6Addr, 0, sizeof( L7_in6_addr_t));
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return CLI_INDEX_NOT_FOUND;
  }

  for ( index = 0; index < L7_MAX_SNMP_COMM; index++ )
  {
    memset(str_addr, 0, sizeof(str_addr));
    memset(commName, 0, sizeof(commName));
    memset(&ipv6Addr, 0, sizeof( L7_in6_addr_t));

    rc = usmDbTrapManagerCommIpGet(unit, index, commName);

    rc =  usmDbTrapManagerIPv6AddrGet(unit, index, &ipv6Addr);
    osapiInetNtop(L7_AF_INET6, (char *)&ipv6Addr, str_addr, sizeof(str_addr));
    if ( (strcmp(commName, name) == 0) && (strcmp(str_addr,v6Addr)==0) )
    {
      return index;
    }

  }
  return -1;
}
/*********************************************************************
* @purpose  to find the first free 0 based index of an SNMP Trap Community
*
* @param EwsContext ewsContext
*
* @returns  -1  there
* @returns  An + Integer
*
* @notes the first free one will be one with a status of 3 (DELETE)
*
* @end
*********************************************************************/
L7_int32 cliFindEmptySNMPTrapCommunityIndex()
{
  L7_uint32 index;
  L7_uint32 status;
  L7_RC_t rc, unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return CLI_INDEX_NOT_FOUND;
  }

  for ( index = 0; index < L7_MAX_SNMP_COMM; index++ )
  {
    rc = usmDbTrapManagerStatusIpGet(unit, index, &status);
    if ( status == L7_SNMP_COMMUNITY_STATUS_DELETE)
    {
      return index;
    }
  }
  return -1;
}

/*********************************************************************
* @purpose  used to convert ascii input from cli to hex format.
*
* @param    buf         contains ascii string "xx:xx:xx"
* @param    oui         return hex data
*
* @returns
*            L7_TRUE/L7_FALSE depending on success/failure
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL cliConvertOUI(L7_uchar8 * buf, L7_uchar8 * oui)
{
  L7_uint32 i,j, digit_count=0;
  L7_uchar8 oui_val[L7_CLI_MAX_STRING_LENGTH];

  if (strlen(buf) != 8)
  {      /* test string length */
    return L7_FALSE;
  }

  for ( i=0,j=0; i<8; i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      oui_val[j]=0x0;
      break;
    case '1':
      oui_val[j]=0x1;
      break;
    case '2':
      oui_val[j]=0x2;
      break;
    case '3':
      oui_val[j]=0x3;
      break;
    case '4':
      oui_val[j]=0x4;
      break;
    case '5':
      oui_val[j]=0x5;
      break;
    case '6':
      oui_val[j]=0x6;
      break;
    case '7':
      oui_val[j]=0x7;
      break;
    case '8':
      oui_val[j]=0x8;
      break;
    case '9':
      oui_val[j]=0x9;
      break;
    case 'a':
    case 'A':
      oui_val[j]=0xA;
      break;
    case 'b':
    case 'B':
      oui_val[j]=0xB;
      break;
    case 'c':
    case 'C':
      oui_val[j]=0xC;
      break;
    case 'd':
    case 'D':
      oui_val[j]=0xD;
      break;
    case 'e':
    case 'E':
      oui_val[j]=0xE;
      break;
    case 'f':
    case 'F':
      oui_val[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      break;
    }
  }

  for ( i = 0; i < 3; i++ )
  {
    oui[i] = ( (oui_val[(i*2)] << 4) + oui_val[(i*2)+1] );
  }
  return L7_TRUE;
}



/*********************************************************************
* @purpose  used for laging, creates a char array of slot.ports included in lag
*
* @param    buf         contains ascii string "xx:xx:xx:xx:xx:xx"
* @param    macLength   provides the length of the MAC address (different between IVL/SVL)
* @param    mac         return hex data
*
* @returns void
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL cliConvertMac(L7_uchar8 * buf, L7_uchar8 * mac)
{
  L7_uint32 i,j, digit_count=0;
  L7_uchar8 mac_address[L7_CLI_MAX_STRING_LENGTH];

  if (strlen(buf) != 17)
  {      /* test string length */
    return L7_FALSE;
  }

  for ( i=0,j=0; i<17; i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      break;
    }
  }

  for ( i = 0; i < 6; i++ )
  {
    mac[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  used for vlan/mac combinations, creates a char array of slot.ports
*
* @param    buf         contains ascii string "xx:xx:xx:xx:xx:xx:xx:xx"
* @param    mac         return hex data
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL cliConvertVlanMac(L7_uchar8 * buf, L7_uchar8 * mac)
{
  L7_uint32 i,j, digit_count=0;
  L7_uchar8 mac_address[L7_CLI_MAX_STRING_LENGTH];

  if (strlen(buf) != 23)
  {      /* test string length */
    return L7_FALSE;
  }

  for ( i=0,j=0; i<23; i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      break;
    }
  }

  /*  IVL mac has 8 bytes */
  for ( i = 0; i < 8; i++ )
  {
    mac[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  converts Ethertype arg string to an L7_ushor16
*
* @param    buf         contains ascii string "0xNNNN"
* @param    etype       returne value
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL cliConvertEtypeCustomValue(L7_uchar8 * buf, L7_uint32 * val)
{
  L7_uint32 i, j;
  L7_uchar8 val_digit[4];
  L7_uint32 value;

  if (strlen(buf) != 6)
  {      /* test string length */
    return L7_FALSE;
  }

  if (buf[0] != '0')
  {
    return L7_FALSE;
  }

  if ((buf[1] != 'x') && (buf[1] != 'X'))
  {
    return L7_FALSE;
  }

  for ( i=2,j=0; i<6; i++ )
  {
    switch ( buf[i] )
    {
    case '0':
      val_digit[j++]=0x0;
      break;
    case '1':
      val_digit[j++]=0x1;
      break;
    case '2':
      val_digit[j++]=0x2;
      break;
    case '3':
      val_digit[j++]=0x3;
      break;
    case '4':
      val_digit[j++]=0x4;
      break;
    case '5':
      val_digit[j++]=0x5;
      break;
    case '6':
      val_digit[j++]=0x6;
      break;
    case '7':
      val_digit[j++]=0x7;
      break;
    case '8':
      val_digit[j++]=0x8;
      break;
    case '9':
      val_digit[j++]=0x9;
      break;
    case 'a':
    case 'A':
      val_digit[j++]=0xA;
      break;
    case 'b':
    case 'B':
      val_digit[j++]=0xB;
      break;
    case 'c':
    case 'C':
      val_digit[j++]=0xC;
      break;
    case 'd':
    case 'D':
      val_digit[j++]=0xD;
      break;
    case 'e':
    case 'E':
      val_digit[j++]=0xE;
      break;
    case 'f':
    case 'F':
      val_digit[j++]=0xF;
      break;
    default:
      return L7_FALSE;
      break;
    }
  }

  value = 0;
  for ( j=0; j<4; j++ )
  {
    value += val_digit[j] << (4 * (3-j));
  }

  *val = value;
  return L7_TRUE;
}

EwsCliCommandP cliAddBlankNode(EwsCliCommandP node)
{
  EwsCliCommandP nextNode;
  nextNode = ewsCliAddNode ( node, pStrInfo_common_EmptyString, pStrInfo_common_EmptyString, commandError, L7_NO_OPTIONAL_PARAMS);
  return nextNode;
}

/*********************************************************************
*
* @purpose check user's input to determine if it is alphanumeric, specifically,
*                    'a'-'z', 'A'-'Z', '0'-'9', '-', '_' or ' '
*
*
* @param *input         the argument entered by user
*
* @returns              L7_SUCCESS if valid
* @returns                          L7_FAILURE if invalid
*
* @notes                none.
*
*
* @end
*
********************************************************************/
L7_RC_t cliIsAlphaNum(L7_char8 * input)
{
  L7_int32 i;

  for (i=0; i<strlen(input); i++)
  {
    if (!(isalnum(input[i]) || input[i]=='-' || input[i]=='_' || input[i]==' '))
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose check user's input to determine if it is alphanumeric and a
*                     geographic location (city, state, country, etc...), specifically,
*                    'a'-'z', 'A'-'Z', '0'-'9', '-', '_', ' ', or ','
*
*
* @param *input         the argument entered by user
*
* @returns              L7_SUCCESS if valid
* @returns                          L7_FAILURE if invalid
*
* @notes                none.
*
*
* @end
*
********************************************************************/
L7_RC_t cliIsLocation(L7_char8 * input)
{
  L7_int32 i;

  for (i=0; i<strlen(input); i++)
  {
    if (!(isalnum(input[i]) || input[i]=='-' || input[i]=='_' || input[i]==' ' || input[i]==','))
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose check user's input to determine if it is alphanumeric, specifically,
*                    'a'-'z', 'A'-'Z', '0'-'9', '-', '_' or ' ' or '@' or '.'
*
*
* @param *input         the argument entered by user
*
* @returns              L7_SUCCESS if valid
* @returns                          L7_FAILURE if invalid
*
* @notes                none.
*
*
* @end
*
********************************************************************/
L7_RC_t cliIsValidSysContact(L7_char8 * input)
{
  L7_int32 i;

  for (i=0; i<strlen(input); i++)
  {
    if (!(isalnum(input[i]) || input[i]=='-' || input[i]=='_' || input[i]==' '
          || input[i]=='@'|| input[i]=='.'))
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose check user's input to determine if it is alphanumeric, specifically,
*                    'a'-'z', 'A'-'Z', '0'-'9', '-', '_', '.' or ' '
*
*
* @param *input         the argument entered by user
*
* @returns              L7_SUCCESS if valid
* @returns                          L7_FAILURE if invalid
*
* @notes                none.
*
*
* @end
*
********************************************************************/
L7_RC_t cliPromptIsAlphaNum(L7_char8 * input)
{
  L7_int32 i;

  for (i=0; i<strlen(input); i++)
  {
    if (!(isalnum(input[i]) || input[i]=='-' || input[i]=='_' ||
          input[i]==' ' || input[i]=='.'))
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose check input to determine if it consists of a blank line,
*          namely all whitespace characters
*
*
* @param *input         string pointer
*
* @returns              L7_SUCCESS if line is blank
* @returns              L7_FAILURE if line is non-blank
*
* @notes                none.
*
*
* @end
*
********************************************************************/
L7_RC_t cliIsBlankLine(L7_char8 * input)
{
  L7_int32 i;

  for (i=0; i<strlen(input); i++)
  {
    if (!(isspace((int)input[i])))
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Resets the command prompt for every connention including the serial connection
*
*
* @param void
*
* @returns void
*
* @notes none
*
*
* @end
*
*********************************************************************/
void cliPromptReset()
{
  L7_uint32 unit;
  L7_uint32 rc;
  L7_uint32 index = 0;
  L7_char8 buf[81];
  L7_uint32 loginType = 0;
  EwsContext context = L7_NULL;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  rc = usmDbCommandPromptGet(unit, buf);
  osapiSnprintf(cliUtil.systemPrompt, sizeof(cliUtil.systemPrompt), "\r\n(%s) ", buf);

  for (index = 0 ; index < FD_CLI_DEFAULT_MAX_CONNECTIONS; index++ )
  {
    osapiSnprintf(cliCommon[index].prompt, sizeof(cliCommon[index].prompt), "%s%s", cliUtil.systemPrompt, pStrInfo_base_RootPrompt);  /* set up the root prompt */
    /*logout all telnet sessions even those with transfer in progress*/
    usmDbLoginSessionTypeGet(unit, index, &loginType);
    context = usmDbLoginSessionUserStorageGet(unit, index);
    if (loginType == L7_LOGIN_TYPE_TELNET)
    {
      ewsNetHTTPAbort(context);
    }

  }
}

/*********************************************************************
*
* @purpose Is the mac address one of the restricted addresses
*
* @param
*
* @returns L7_TRUE if restricted mac, L7_FALSE otherwise
*
* @end
*
*********************************************************************/
L7_BOOL cliIsRestrictedFilterMac(L7_uchar8 * macaddr)
{
    return usmDbFilterIsRestrictedFilterMac(macaddr);
}

/*********************************************************************
*
* @purpose  convert slot port combination to interface number
*
* @param EwsContext ewsContext
* @param const L7_char8 *slot_port
* @param L7_uint32 *slot
* @param L7_uint32 *port
* @param L7_uint32 *intIfNum*
* @returns L7_RC_t
*
* @notes none
*
*********************************************************************/

L7_RC_t cliSlotPortToIntNum(EwsContext ewsContext,
    const L7_char8 * slot_port,
    L7_uint32 * slot,
    L7_uint32 * port,
    L7_uint32 * intIfNum)

{
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (cliSlotPortToInterface(slot_port, &unit, slot, port, intIfNum)
      != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose convert the provided char array into a 32 bit unsigned integer
*          the value is >= 0 and <= 4294967295.
*
* @param L7_char8 *buf, L7_uint32 * pVal
*
* @returns  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-numeric character
*              is included in buf.
*
* @notes This f(x) checks each letter inside the buf to make certain
*         it is an integer.  It initially verifies the number of digits
*         does not exceed the number of digits in Max 32 bit unsigned int.
*         Start with the last digit, convert each ascii character into its
*         integer equivalent.  Multiply each consecutive digit by the next
*         power of 10.  Verify adding the new digit to the old sum will not
*         exceed MAXINT.  If so, this indicates the value is too large to be
*         represented by a 32 bit int.  If ok, add the new digit.
*
*
* @end
*
*********************************************************************/
L7_RC_t cliConvertTo32BitUnsignedInteger(const L7_char8 * buf, L7_uint32 * pVal)
{
  /* NOTE: Moved core of this function to a common location so that
   *       it can be used by both CLI and Web.
   */
  return cliWebConvertTo32BitUnsignedInteger(buf, pVal);
}

/*********************************************************************
*
* @purpose convert the provided char array in HEX form into a 32 bit unsigned integer
*          the value is >= 0 and <= 0xffffffff.
*
* @param L7_char8 *buf, L7_uint32 * pVal
*
* @returns  L7_SUCCESS  means that all chars are HEX integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-hex character
*              is included in buf.
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t cliConvertHexTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal)
{
  /* NOTE: Moved core of this function to a common location so that
   *       it can be used by both CLI and Web.
   */
  return cliWebConvertHexTo32BitUnsignedInteger(buf, pVal);
}

/*********************************************************************
*
* @purpose convert the provided char array into a 32 bit signed integer
*          the value is >= -2,147,483,648 and <= 2,147,483,647.
*
* @param L7_char8 *buf
* @param L7_int32 * pVal
*
* @return  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 32 bit signed integer
* @return  L7_FAILURE  means the value does not represent a valid
*              32 bit signed integer.  I.e. the value is outside of
*              the valid range for a signed integer.
*
* @note This f(x) checks each letter inside the buf to make certain
*       it is an integer.  It initially verifies the number of digits
*       does not exceed the number of digits in Max 32 bit signed int.
*       Start with the last digit, convert each ascii character into its
*       integer equivalent.  Multiply each consecutive digit by the next
*       power of 10.  Verify adding the new digit to the old sum will not
*       exceed MAXINT.  If so, this indicates the value is too large to be
*       represented by a 32 bit int.  If ok, add the new digit.
*
*
* @end
*
*********************************************************************/
L7_RC_t cliConvertTo32BitSignedInteger(const L7_char8 * buf, L7_int32 * pVal)
{
  /* NOTE: Moved core of this function to a common location so that
   *       it can be used by both CLI and Web.
   */
  return cliWebConvertTo32BitSignedInteger(buf, pVal);
}

/*********************************************************************
*
* @purpose convert a string identifying a VLAN range to two integers
*          giving the start and end of the range.
*
* @param vlanRange @b((input)) VLAN range as a string
* @param rangeLow  @b((output)) First VLAN in range
* @param rangeHigh @b((output)) Last VLAN in range
*
* @return  L7_SUCCESS
* @return  L7_FAILURE
*
* @note   The VLAN range can be a single VLAN ID or two VLAN IDs
*         separated by a hyphen. If user enters a single VLAN ID,
*         rangeLow and rangeHigh are set to the same value.
*
*         Makes sure VLAN IDs are valid.
*
* @end
*
*********************************************************************/
L7_RC_t cliConvertVlanRange(const L7_char8 *vlanRange, L7_uint32 *rangeLow,
                            L7_uint32 *rangeHigh)
{
  L7_uchar8 vlan1[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 vlan2[L7_CLI_MAX_STRING_LENGTH];

  /* Look for hyphen in string. */
  char *hyphen = strchr(vlanRange, '-');
  memset(vlan1, 0, sizeof(vlan1));
  memset(vlan2, 0, sizeof(vlan2));
  if (hyphen)
  {
    /* this is a range */
    L7_uint32 vlanLen = (L7_uint32) hyphen - (L7_uint32) vlanRange;
    osapiStrncpy(vlan1, vlanRange, vlanLen);
    if (cliConvertTo32BitUnsignedInteger(vlan1, rangeLow) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    osapiStrncpy(vlan2, hyphen + 1, L7_CLI_MAX_STRING_LENGTH);
    if (cliConvertTo32BitUnsignedInteger(vlan2, rangeHigh) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    /* single VLAN ID */
    if (cliConvertTo32BitUnsignedInteger(vlanRange, rangeLow) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    *rangeHigh = *rangeLow;
  }
  /* Validate VLAN IDs */
  if (*rangeHigh < *rangeLow)
    return L7_FAILURE;
  if (*rangeHigh > L7_PLATFORM_MAX_VLAN_ID)
    return L7_FAILURE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Check if a given slot.port is the CPU
*
* @param L7_uint32 *slot
* @param L7_uint32 *port
* @returns L7_RC_t
*
* @notes none
*
*********************************************************************/
L7_RC_t cliSlotPortCpuCheck(L7_uint32 slot,
                            L7_uint32 port)
{
  if ((slot==L7_CPU_SLOT_NUM)&&
      (port==1))
  {
    return L7_FAILURE;
  }
  else
  {
    return L7_SUCCESS;
  }
}

/*********************************************************************
*
* @purpose  Get switch ID
*
* @param  void
*
* @returntype  L7_uint32
* @returns  Unit ID
*
* @notes
*
* @end
*
********************************************************************/
L7_uint32 cliGetUnitId()
{
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (usmDbUnitMgrMgrNumberGet(&unit) != L7_SUCCESS)
    {
      /* This is a temporary fix until the CLI changes are made to handle a unit of 0 */
      unit = L7_MAX_UNITS_PER_STACK + 1;
  }
  }
  else
  {
    unit = (L7_uint32)U_IDX;
  }

  return unit;
}

/*********************************************************************
*
* @purpose Get the Bandwidth of the specified interface
*
* @param intIfNum  internal interface number
*
* @param *bw       bandwidth of the specified internal interface
*
* @returns L7_SUCCESS id the speed was successfully obtained and
*                     the interface is a physical interface
*          L7_ERROR   otherwise
*
* @end
*
*********************************************************************/
L7_uint32 cliWebIntfBWGet(L7_uint32 intIfNum, L7_uint32 * bw)
{
  L7_uint32 maskIntfType;
  L7_uint32 unit;
  L7_uint32 val;
  L7_RC_t   rc;

  if (bw == NULL)
  {
    return L7_ERROR;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return L7_ERROR;
  }

  rc = usmDbIntIfNumTypeMaskGet(unit, intIfNum, &maskIntfType);
  if (rc == L7_SUCCESS)
  {
    if (maskIntfType & USM_PHYSICAL_INTF)
    {
      rc = usmDbIfSpeedGet(unit, intIfNum, &val);
      if (rc == L7_SUCCESS)
      {
        switch (val)
        {
        case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100FX:
          *bw = 100;
          break;
        case L7_PORTCTRL_PORTSPEED_HALF_10T:
        case L7_PORTCTRL_PORTSPEED_FULL_10T:
          *bw = 10;
          break;
        case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
          *bw = 1000;
          break;
        /* PTin added: Speed 2.5G */
        case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
          *bw = 2500;
          break;
        /* PTin end */
        case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
          *bw = 10000;
          break;
        /* PTin added: Speed 40G */
        case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
          *bw = 40000;
          break;
        /* PTin added: Speed 100G */
        case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
          *bw = 100000;
          break;
        /* PTin end */
        default:
          *bw = 0;
          rc = L7_ERROR;
          break;
        }
      }
      else
      {
        *bw = 0;
        rc = L7_ERROR;
      }
    }
    else /* Not a physical interface ... no checking */
    {
      *bw = 0;
      rc = L7_ERROR;
    }
  }
  else
  {
    rc = L7_ERROR;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Compare the first n characters of the two string.
*
* @param char *buf, char *buf, int i
*
* @returns  L7_SUCCESS  If two string matched each other.
* @returns  L7_FAILURE  otherwise
*
* @end
*
*********************************************************************/
L7_RC_t cliStrCaseCmp(char * str1, char * str2, int n)
{
  int i;
  for ( i = 0; i < n; i++ )
  {
    /* Verify that none of the string reach to end, \0 and \n shwos i
       that string reach to its end*/
    if (  (str1[i] != '\n' && str1[i] != '\0' ) &&
          (str2[i] != '\n' && str2[i] != '\0' ))
    {
      if( tolower(str1[i]) != (tolower(str2[i])))
      {
          return L7_FAILURE;
      }
    }
    else
    {
        return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose remove quotes from input string
*
*
* @param *input         the argument entered by user
*
* @returns              L7_FAILURE if could not be checked
*
* @notes                The input must be a null-terminated string
*                       and must be shorter in length than L7_CLI_MAX_STRING_LENGTH
*                       including trailing null.
*
*
* @end
*
********************************************************************/
L7_RC_t cliRemoveQuotes(L7_char8 * input)
{
  L7_int32 i, j;
  L7_uint32 inputLength;
  L7_char8 temp[L7_CLI_MAX_STRING_LENGTH];

  /* since strlen does not count the trailing '\0', assume the input
   * buffer is at least (inputLength+1) bytes long
   */
  inputLength = strlen(input);

  if (inputLength >= L7_CLI_MAX_STRING_LENGTH)
  {
    return L7_FAILURE;
  }

  memset( temp, 0, sizeof(temp));
  for (i=0, j=0; i<inputLength; i++)
  {
      if (input[i] != '"')
      {
          temp[j] = input[i];
          j++;
      }
  }
  osapiStrncpySafe(input, temp, inputLength+1);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose compare 2 buffers.
*
* @param L7_char8 *buf1
* @param L7_char8 *buf2
*
* @returns  L7_TRUE if buffers are the same.  L7_FALSE otherwise.
*
* @notes If the buffers can not be compared, L7_FALSE is returned.
* @notes The buffers must be null terminated.
*
* @end
*
*********************************************************************/
L7_BOOL cliNoCaseCompare(L7_char8 * buf1, L7_char8 * buf2 )
{
    L7_char8 lowerCaseBuf1[ L7_CLI_MAX_STRING_LENGTH ];
    L7_char8 lowerCaseBuf2[ L7_CLI_MAX_STRING_LENGTH ];

    /* First try compare without converting strings. */
    if (strcmp(buf1, buf2) == 0)
  {
    return L7_TRUE;
  }

    /* Make sure buffers are large enough */
    if (strlen(buf1) > sizeof(lowerCaseBuf1) || strlen(buf2) > sizeof(lowerCaseBuf2))
  {
    return L7_FALSE;
  }

  OSAPI_STRNCPY_SAFE( lowerCaseBuf1, buf1);
    cliConvertToLowerCase( lowerCaseBuf1 );

  OSAPI_STRNCPY_SAFE( lowerCaseBuf2, buf2);
    cliConvertToLowerCase( lowerCaseBuf2 );

    if (strcmp( lowerCaseBuf1, lowerCaseBuf2) == 0)
  {
    return L7_TRUE;
  }

    return L7_FALSE;
}
/*********************************************************************
*
* @purpose To set if user output need to be in scroll format or not.
*
*
* @param L7_BOOL scroll
*
* @returns void
*
* @notes none
*
*
* @end
*
********************************************************************/
void cliCmdScrollSet(L7_BOOL scroll)
{
  cliCommon[cliUtil.handleNum].isScroll = scroll;
}

/*********************************************************************
*
* @purpose  To get the scroll value.
*
*
* @param void
*
* @returntype L7_BOOL
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes none
*
*
* @end
*
********************************************************************/
L7_BOOL cliCmdScrollGet()
{
  return cliCommon[cliUtil.handleNum].isScroll;
}

/*********************************************************************
*
* @purpose convert the provided char array into a 64 bit unsigned integer
*          the value is >= 0 and <= 18446744073709551615U.
*
* @param L7_char8 *buf, L7_uint64 * pVal
*
* @returns  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 64 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 64 bit int or a non-numeric character
*              is included in buf.
*
* @notes This f(x) checks each letter inside the buf to make certain
*         it is an integer.  It initially verifies the number of digits
*         does not exceed the number of digits in Max 64 bit unsigned int.
*         Start with the last digit, convert each ascii character into its
*         integer equivalent.  Multiply each consecutive digit by the next
*         power of 10.  Verify adding the new digit to the old sum will not
*         exceed MAXINT.  If so, this indicates the value is too large to be
*         represented by a 64 bit int.  If ok, add the new digit.
*
*
* @end
*
*********************************************************************/
L7_RC_t cliConvertTo64BitUnsignedInteger(const L7_char8 * buf, L7_uint64 * pVal)
{

  L7_int32 f;
  L7_int32 j;
  L7_uint64 tempval;
  L7_uint64 maxUlong = 18446744073709551615ULL;

  if ( buf[0] == '-' )
  {
    return L7_FAILURE;
  }

  j = strlen(buf);

  if ( j > 20 )
  {
    return L7_FAILURE;
  }

  for ( *pVal = 0, f=1, j=j-1; j >= 0; j--, f=f*10)
  {
    tempval = (L7_uint64)(buf[j] -'0');

    if ( ( tempval > 9 ) || ( tempval < 0 ) )
    {
      return L7_FAILURE;
    }

    tempval = tempval * f;

    if ( (maxUlong - tempval) < *pVal )
    {
      return L7_FAILURE;
    }
    else
    {
      *pVal = *pVal + tempval;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose convert the provided char array into a 64 bit signed integer
*          the value is >= -9223372036854775808 and <= 9223372036854775807.
*
* @param L7_char8 *buf
* @param L7_int64 * pVal
*
* @return  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 64 bit signed integer
* @return  L7_FAILURE  means the value does not represent a valid
*              64 bit signed integer.  I.e. the value is outside of
*              the valid range for a signed integer.
*
* @note This f(x) checks each letter inside the buf to make certain
*       it is an integer.  It initially verifies the number of digits
*       does not exceed the number of digits in Max 64 bit signed int.
*       Start with the last digit, convert each ascii character into its
*       integer equivalent.  Multiply each consecutive digit by the next
*       power of 10.  Verify adding the new digit to the old sum will not
*       exceed MAXINT.  If so, this indicates the value is too large to be
*       represented by a 64 bit int.  If ok, add the new digit.
*
*
* @end
*
*********************************************************************/
L7_RC_t cliConvertTo64BitSignedInteger(const L7_char8 * buf, L7_int64 * pVal)
{

  L7_int64 f;
  L7_int64 j;
  L7_int64 tempval;
  L7_BOOL negativeValue = L7_FALSE;
  L7_int64 firstNumIndex = 0;
  L7_int64 maxLong = 9223372036854775807LL;
  L7_int64 minLongLessOne = -9223372036854775807LL;

  if ( buf[0] == '-' )
  {
      firstNumIndex=1;
      negativeValue = L7_TRUE;
  }

  j = strlen(buf);

  if ( ( negativeValue == L7_TRUE && j > 20 ) || /* 11 chars in min int */
      ( negativeValue == L7_FALSE && j > 19 ) )
  {                                             /* 10 chars in max int */
    return L7_FAILURE;
  }

  for ( *pVal = 0, f=1, j=j-1; j >= firstNumIndex; j--, f=f*10)
  {
    tempval = (L7_int64)(buf[j] -'0');   /* convert to numeric */

    if ( ( tempval > 9 ) || ( tempval < 0 ) )
    {                                           /* non-numeric */
      return L7_FAILURE;
    }

    tempval = tempval * f;

    if (negativeValue == L7_FALSE)
    {
        if ( (L7_int64)(maxLong - tempval) < *pVal )
      {
        return L7_FAILURE;
      }
      else
      {
        *pVal = *pVal + tempval;
      }
    }
    else
    {
        if ( (L7_int64)(minLongLessOne + tempval) >= *pVal )
      {
        return L7_FAILURE;
      }
      else
      {
            *pVal = *pVal - tempval;
    }
  }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build tree helper routine
*
* @param  depth  @b{(input)) the command structure
* @param  count  @b{(input)) the count of variable args
* @param  opt  @b{(input)) the node option
* @param  componentId  @b{(input)) the component id
*
* @returns  EwsCliCommandP  the command structure
*
* @comments
*
* @end
*********************************************************************/
EwsCliCommandP buildTreeInterfaceHelp(EwsCliCommandP depth, L7_uint32 count, EwsCliCommandOptions opt, L7_COMPONENT_IDS_t componentId, EwsCliNodeNoFormStatus noFormStatus)
{
  EwsCliCommandP nextDepth;

#ifdef L7_STACKING_PACKAGE
  nextDepth = ewsCliAddNode(depth, pStrErr_common_AclIntfsStacking, pStrInfo_common_SlotPortWithUnit, NULL, count + 4,
                            L7_NO_COMMAND_SUPPORTED, noFormStatus, L7_NODE_DATA_TYPE, L7_USP_DATA_TYPE,
                            opt, componentId);
#else
  nextDepth = ewsCliAddNode(depth, pStrErr_common_AclIntfs, pStrInfo_common_SlotPortWithoutUnit, NULL, count + 4,
                            L7_NO_COMMAND_SUPPORTED, noFormStatus, L7_NODE_DATA_TYPE, L7_USP_DATA_TYPE,
                            opt, componentId);
#endif

  return nextDepth;
}

/*********************************************************************
* @purpose  Build tree helper routine
*
* @param  depth  @b{(input)) the command structure
* @param  count  @b{(input)) the count of variable args
* @param  opt  @b{(input)) the node option
* @param  componentId  @b{(input)) the component id
*
* @returns  EwsCliCommandP  the command structure
*
* @comments
*
* @end
*********************************************************************/
EwsCliCommandP buildTreeInterfaceRangeHelp(EwsCliCommandP depth, L7_uint32 count, EwsCliCommandOptions opt, L7_COMPONENT_IDS_t componentId, EwsCliNodeNoFormStatus noFormStatus)
{
  EwsCliCommandP nextDepth;

#ifdef L7_STACKING_PACKAGE
  nextDepth = ewsCliAddNode(depth, pStrErr_common_IntfRange, pStrInfo_common_SlotPortRangeWithUnit, NULL, count + 4,
                            L7_NO_COMMAND_SUPPORTED, noFormStatus, L7_NODE_DATA_TYPE, L7_USP_RANGE_DATA_TYPE,
                            opt, componentId);
#else
  nextDepth = ewsCliAddNode(depth, pStrErr_common_IntfRange, pStrInfo_common_SlotPortRangeWithoutUnit, NULL, count + 4,
                            L7_NO_COMMAND_SUPPORTED, noFormStatus, L7_NODE_DATA_TYPE, L7_USP_RANGE_DATA_TYPE,
                            opt, componentId);
#endif

  return nextDepth;
}

/*********************************************************************
* @purpose  Build tree helper routine
*
* @param  depth  @b{(input)) the command structure
*
* @returns  EwsCliCommandP  the command structure
*
* @comments
*
* @end
*********************************************************************/
EwsCliCommandP buildTreeLogInterfaceHelp(EwsCliCommandP depth, EwsCliNodeNoFormStatus noFormStatus)
{
  EwsCliCommandP nextDepth;

#ifdef L7_STACKING_PACKAGE
  nextDepth = ewsCliAddNode (depth, pStrErr_common_AclIntfsStacking, pStrInfo_common_CfgLogIntfWithUnit, NULL, 2, L7_NO_COMMAND_SUPPORTED, noFormStatus);
#else
  nextDepth = ewsCliAddNode (depth, pStrErr_common_AclIntfs, pStrInfo_common_CfgLogIntfWithoutUnit, NULL, 2, L7_NO_COMMAND_SUPPORTED, noFormStatus);
#endif

  return nextDepth;
}

/*********************************************************************
* @purpose  Check validity of user-input ip address
*
* @param  *buf   @b{(input)) pointer to user-input string
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Valid input is of the format x.x.x.x
*
* @end
*********************************************************************/
L7_BOOL cliValidIPAddrCheck(const L7_char8 * buf)
{
  L7_uint32 buf_len, int1, int2, int3, int4;
  L7_char8  tmp_buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * input, * oct1, * oct2, * oct3, * oct4;
  const L7_char8 * dot = ".";
  const L7_char8 * end = pStrInfo_common_EmptyString;

  buf_len = (L7_uint32)strlen(buf);
  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FALSE;
  }

  memset (tmp_buf, 0, (L7_int32)sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);

  if (strcmp(tmp_buf, pStrInfo_common_EmptyString) == 0)
  {
    return L7_FALSE;
  }

  /* NOTE: Input of type <x.x.x.x> only is valid.
   *
   * Check for erroneous inputs.
   */
  input = tmp_buf;
  oct1 = cliStringSeparateHelp(&input, dot);
  oct2 = cliStringSeparateHelp(&input, dot);
  oct3 = cliStringSeparateHelp(&input, dot);
  oct4 = cliStringSeparateHelp(&input, end);

  if ((oct1 == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(oct1, &int1) != L7_SUCCESS) ||
      (oct2 == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(oct2, &int2) != L7_SUCCESS) ||
      (oct3 == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(oct3, &int3) != L7_SUCCESS) ||
      (oct4 == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(oct4, &int4) != L7_SUCCESS))
  {
    return L7_FALSE;
  }

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine specific interface from user-input
*
* @param  *buf   @b{(input)) pointer to user-input string
* @param  *unit  @b{(output)) pointer to the unit index
* @param  *slot  @b{(output)) pointer to the slot index
* @param  *port  @b{(output)) pointer to the port index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If stacking is supported, valid input is `unit/slot/port`
*
* @end
*********************************************************************/
L7_RC_t cliValidSpecificUSPCheck(const L7_char8 * buf, L7_uint32 * unit,
    L7_uint32 * slot, L7_uint32 * port)
{
  L7_uint32 buf_len;
  L7_char8  tmp_buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 *u, *s, *p;
  L7_char8 *input;
  const L7_char8 * slash = "/";
  const L7_char8 * end = pStrInfo_common_EmptyString;


  buf_len = (L7_uint32)strlen(buf);
  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  memset (tmp_buf, 0, (L7_int32)sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);

  if (strcmp(tmp_buf, pStrInfo_common_EmptyString) == 0)
  {
    return L7_FAILURE;
  }

  /* NOTE: Input of type <a/b/c> only is valid.
   *
   * Check for erroneous inputs, i.e, of type <a>, <a/b>, <a/b/c/d> etc.
   */
  input = tmp_buf;
  if (cliIsStackingSupported() == L7_TRUE)
  {
  u = cliStringSeparateHelp(&input, slash);
    if ((u == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(u, unit) != L7_SUCCESS))
    {
      return L7_FAILURE;
    }
    if (*unit > L7_UNITMGR_MAX_UNIT_NUMBER)
    {
      return L7_ERROR;
    }
  }
  else
  {
    *unit = cliGetUnitId();
  }

  s = cliStringSeparateHelp(&input, slash);
  if(s != L7_NULLPTR)
  {
     if(strlen(s)== 0)
     {
      return L7_FAILURE;
     }
  }

  p = cliStringSeparateHelp(&input, end);
  if ((s == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(s, slot) != L7_SUCCESS) ||
      (p == L7_NULLPTR) || (cliConvertTo32BitUnsignedInteger(p, port) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

#ifdef L7_CHASSIS
  {
  if ((*slot > L7_MAX_SLOTS_PER_UNIT) ||
      (*port == 0) ||
      (*port > L7_MAX_PORTS_PER_SLOT))
        (*slot > (L7_MAX_SLOTS_PER_CHASSIS+
                  L7_MAX_LOGICAL_SLOTS_PER_UNIT+L7_MAX_CPU_SLOTS_PER_UNIT)) ||
        (*port == 0))
    {
      return L7_ERROR;
    }
  }
#else
  {
    if ((*slot > L7_MAX_SLOTS_PER_UNIT) ||
        (*port == 0))
    {
      return L7_ERROR;
    }
  }
#endif

  return L7_SUCCESS;

}

/*********************************************************************
*
* @purpose   This function parses a string to determine the intIfNum. 
             Returns L7_ERROR if the string is invalid.
*            The string format should be:
*            <unit>/<slot>/<port> or <slot>/<port>
*
* @returntype L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliIntfStringToIntIfNum(const L7_uchar8 *intfString, L7_uint32 *intIfNum)
{
  L7_uint32  unit, slot, port;

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(intfString, &unit, &slot, &port) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
     /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, intIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    if (cliSlotPortToInterface(intfString, &unit, &slot, &port, intIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  
  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose Parse the range input given in comma separated values
*          with no spaces and '-' designates consecutive input range.
*
* @param  *buf        @b{(input))  Text input
* @param  *count      @b{(input))  Count of interfaces found.
* @param  *list       @b{(input))  Array of internal interface numbers
*
*
* @comments
* Use hyphen to designate range 
* Use comma with no spaces between non-consecutive IDs
*
* @end
*
*********************************************************************/
L7_RC_t cliUSPParse(const L7_char8 *buf, L7_uint32 *list, L7_uint32 *count)
{
  L7_int32 i, j, index = 0;
  L7_uint32 buf_len, start;
  L7_char8 *comma = ",";
  L7_char8 *hyphen = "-";
  L7_char8 *retstring;
  L7_char8  tmp_buf[L7_CLI_MAX_STRING_LENGTH], *input;
  L7_char8  startbuf[10], endbuf[10];
  L7_char8  buffer[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intfNum = 0;
  L7_uint32 startIntfNum = 0;
  L7_uint32 endIntfNum = 0;
  L7_uint32 startUnit, endUnit, tempSlot, tempPort;

  buf_len = (L7_uint32)strlen(buf);

  if (strlen(buf) >= sizeof(tmp_buf))
    return L7_FAILURE;

  bzero(tmp_buf, (L7_int32)sizeof(tmp_buf));

  memcpy(tmp_buf, buf, buf_len);

  if (strcmp(tmp_buf, "") == 0)
    return L7_FAILURE;

  input = tmp_buf;

  for (i = 0; i < strlen(input); i++)
  {
    if (!(isalnum((L7_uchar8)input[i]) || input[i] == '-' || input[i] == ',' || input[i] == '/'))
    {
      return L7_FAILURE;
    }
  }

  index = 0;
  i = 0;
  start = L7_FALSE;

  do
  {
    if (input[i] == ',')
    {
      /* end of string, ',' '/' and '-' after ',' are not expected */
      if ((input[i + 1] != '\0') && (input[i + 1] != ',') && (input[i + 1] != '-'))
      {
        retstring = cliStringSeparateHelp(&input, comma);
        if (start == L7_FALSE)
        {
          osapiStrncpySafe(buffer, retstring, 
                        min(strlen(retstring)+1 , sizeof(buffer)));
          if (cliIntfStringToIntIfNum(buffer, &intfNum) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }
          list[index] = intfNum;
          index++;
        }
        else /* value after hyphen */
        { 
          osapiStrncpySafe(endbuf, retstring, 
                     min(strlen(retstring)+1, sizeof(endbuf)));
          if (cliIntfStringToIntIfNum(endbuf, &endIntfNum) != L7_SUCCESS)
          {
            return L7_FAILURE;
          }

          /* Consecutive interfaces are expected to be from the same
           * unit. Slot has no significance here as some Services interface
           * format does not specify slot.
           */
          usmDbUnitSlotPortGet(endIntfNum, &endUnit, &tempSlot, &tempPort);

          /* Check for improper range input */
          if (startUnit != endUnit)
            return L7_FAILURE;

          if (startIntfNum >= endIntfNum)
            return L7_FAILURE;

          for (j = startIntfNum; j <= endIntfNum; j++)
          {
            list[index] = j;
            index++;
          }
          start = L7_FALSE;
        }
        i = 0;
      } /* if ((input[i + 1] != '\0') && ........ */
      else /* Invalid char found at the end of the string */
      {
        return L7_FAILURE;
      }
    }
    else if (input[i] == '-')
    {
      if ((start == L7_FALSE) && (input[i + 1] != '\0'))
      {
        retstring = cliStringSeparateHelp(&input, hyphen); 
        osapiStrncpySafe(startbuf, retstring, 
                         min(strlen(retstring) + 1, sizeof(startbuf)));
        if (cliIntfStringToIntIfNum(startbuf, &startIntfNum) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        usmDbUnitSlotPortGet(startIntfNum, &startUnit, &tempSlot, &tempPort);
        start = L7_TRUE;
        i = 0;
      }
      else /* Invalid char found at the end of the string */
      {
        return L7_FAILURE;
      }
    }
    else if (input[i] == '\0')
    {
      if (start == L7_FALSE) /* Last value in the string */
      {
        osapiStrncpySafe(buffer, input, 
                         min(strlen(input) + 1, sizeof(buffer)));
        if (cliIntfStringToIntIfNum(buffer, &intfNum) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        list[index] = intfNum;
        index++;
      }
      else /* value after hyphen */
      {
        osapiStrncpySafe(endbuf, input,
                           min(strlen(input) + 1, sizeof(endbuf)));
        if (cliIntfStringToIntIfNum(endbuf, &endIntfNum) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        /* Consecutive interfaces are expected to be from the same
         * unit. Slot has no significance here as some Services interface
         * format does not specify slot.
         */
        usmDbUnitSlotPortGet(endIntfNum, &endUnit, &tempSlot, &tempPort);

        /* Check for improper range input */
        if (startUnit != endUnit)
          return L7_FAILURE;

        if (startIntfNum >= endIntfNum)
          return L7_FAILURE;

        for (j = startIntfNum; j <= endIntfNum; j++)
        {
          list[index] = j;
          index++;
        }
        start = L7_FALSE;
      }
      break;
    }
    else
    {
      i++;
    }

  } while ((strcmp(input, "") != 0)); /* till end of string */

  *count = index;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Determine specific prefix/prefix_length from user-input
*
* @param  *buf   @b{(input)) pointer to user-input string
* @param  *prefix  @b{(output)) pointer to the prefix index
* @param  *prefix-len  @b{(output)) pointer to the prefix length index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If stacking is supported, valid input is `<prefix>/<prefix-length>`
*
* @end
*********************************************************************/
L7_RC_t cliValidPrefixPrefixLenCheck(const L7_char8 * buf, L7_in6_addr_t * prefix,
    L7_uint32 * prefixLen)
{

  L7_uint32 buf_len;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  tmp_buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * p, * plen;

  L7_char8 * input;
  const L7_char8 * slash = "/";
  const L7_char8 * end = pStrInfo_common_EmptyString;

  buf_len = (L7_uint32)strlen(buf);
  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  memset (tmp_buf, 0, (L7_int32)sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);

  if (strcmp(tmp_buf, pStrInfo_common_EmptyString) == 0)
  {
    return L7_FAILURE;
  }

  input = tmp_buf;
  p = cliStringSeparateHelp(&input, slash);
  plen= cliStringSeparateHelp(&input, end);

  if (p != '\0')
  {
     if (strlen(p) >= sizeof(strIPaddr))
    {
      return L7_FAILURE;
    }

    OSAPI_STRNCPY_SAFE(strIPaddr, p);
  }
  if(osapiInetPton(L7_AF_INET6,strIPaddr, (L7_uchar8 *) prefix) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (plen == L7_NULLPTR || cliConvertTo32BitUnsignedInteger(plen, prefixLen) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

/* Put specific prefix range checking here
      return L7_ERROR;                     */

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Given a string containing <prefix>/<mask len>, parse the two
*           items, returning the prefix as a character string and the 
*           prefix length as the corresponding mask value.
*
* @param  *buf     @b{(input)) pointer to user-input string
* @param  *prefix  @b{(output)) output location for prefix (as string)
* @param  *mask    @b{(output)) output location for network mask (as integer)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t cliValidPrefixPrefixLenCheckIPV4(const L7_char8 *buf, L7_uchar8 *prefix,
                                         L7_uint32 *mask)
{
  L7_uint32 prefixLen;
  L7_uchar8 *slash = strchr(buf, '/');   /* string starting at first slash */

  if (slash == NULL)
  {
    return L7_FAILURE;
  }

  strncpy(prefix, buf, strlen(buf) - strlen(slash));

  /* length of string starting */
  if ((strlen(slash) < 2) || (strlen(slash) > 3))
  {
    return L7_FAILURE;
  }

  /* Convert what follows / to an integer */
  if (cliConvertTo32BitUnsignedInteger(slash + 1, &prefixLen) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* convert integer to mask (e.g., 24 to 0xFFFFFF00) */
  if (usmDbIpSubnetMaskGet(mask, prefixLen) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  CLI helper routine to get interface from user-input
*
* @param  **input  @b{(input)) pointer to user-input string
* @param  *delim   @b{(input)) pointer to the delimiter
*
* @returns  Token string
*
* @comments
*
* @end
*********************************************************************/
L7_char8 *cliStringSeparateHelp(L7_char8 * * input, const L7_char8 * delim)
{
  const L7_char8 * tmp_delim;
  L7_char8 * tmp_input, * tok;
    L7_int32 i, j;

    if ((tmp_input = *input) == NULL)
  {
    return NULL;
  }

    for (tok = tmp_input;;)
  {
    i = *tmp_input++;
    tmp_delim = delim;
    do
    {
      if ((j = *tmp_delim++) == i)
      {
        if (i == 0)
        {
          tmp_input = NULL;
        }
        else
        {
          *(tmp_input-1) = 0;
        }

        *input = tmp_input;
        return tok;
      }
    } while (j != 0);
  }
}

/*********************************************************************
* @purpose  CLI helper routine for interface syntax string
*
* @param  void
*
* @returns  Interface syntax string
*
* @comments  Returned string reflects STACKING component support.
*
* @end
*********************************************************************/
L7_char8 *cliSyntaxInterfaceHelp(void)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_IntfStack);
#else
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_Intf_5);
#endif

  osapiSnprintf(intf_string, sizeof(intf_string), tmp_string);
  return intf_string;
}

/*********************************************************************
* @purpose  CLI helper routine for interface and tunnel syntax string
*
* @param  void
*
* @returns  Interface syntax string
*
* @comments  Returned string reflects STACKING component support.
*
* @end
*********************************************************************/
#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
L7_char8 *cliSyntaxInterfaceTunnelHelp(void)
{
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

  osapiSnprintf(intf_string, sizeof(intf_string), pStrInfo_base_Tunnel, cliSyntaxInterfaceHelp(), L7_CLIMIN_TUNNELID, L7_CLIMAX_TUNNELID);
  return intf_string;
}
#endif
#endif

/*********************************************************************
* @purpose  CLI helper routine for logical interface syntax string
*
* @param    void
*
* @returns  Logical interface syntax string
*
* @comments  Returned string reflects STACKING component support.
*
* @end
*********************************************************************/
L7_char8 *cliSyntaxLogInterfaceHelp(void)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_IntfLogicalStack);
#else
  osapiSnprintf(tmp_string, sizeof(tmp_string), pStrInfo_base_IntfLogical);
#endif

  osapiSnprintf(intf_string, sizeof(intf_string), tmp_string);
  return intf_string;
}

/*********************************************************************
* @purpose  CLI helper routine for interface display format
*
* @param  unit  @b{(input)) the unit index
* @param  slot  @b{(input)) the slot index
* @param  port  @b{(input)) internal interace number
*
* @returns  Interface display string
*
* @comments Returned string reflects STACKING component support
*
* @end
*********************************************************************/
L7_char8 *cliDisplayInterfaceHelp(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(tmp_string, sizeof(tmp_string), "%u/%u/%u", unit, slot, port);
#else
  osapiSnprintf(tmp_string, sizeof(tmp_string), "%u/%u", slot, port);
#endif

  osapiSnprintf(intf_string, sizeof(intf_string), tmp_string);

  return intf_string;
}

/*********************************************************************
* @purpose  CLI helper routine for hashMode display format
*
* @param    hashMode  @b{(input)) the unit index
*
* @returns  hashMode display string
*
* @end
*********************************************************************/
L7_char8 *cliDisplayHashModeHelp(L7_uint32 hashMode)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];

  osapiSnprintf(tmp_string, sizeof(tmp_string), "%u", hashMode);

  return tmp_string;
}

/*********************************************************************
* @purpose  Check validity of input unit id
*
* @param  *buf       @b{(input)) the user-input string
* @param  *unit      @b{(output)) pointer to the unit index
* @param  *allUnits  @b{(output)) pointer to boolean value,
*                                 L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_NOT_EXIST
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t cliValidUnitCheck(const L7_char8 * buf, L7_uint32 * unit, L7_BOOL * allUnits)
{
  L7_uint32 u = 0, buf_len;
  L7_char8  tmp_buf[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_CHASSIS
  L7_uint32 chassisIntUnit,intSlot;
#endif

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

  /* check user input for `all` units */
  if (strcmp(tmp_buf, pStrInfo_common_All) == 0)
  {
    *allUnits = L7_TRUE;
    *unit = u;

    if (usmDbUnitMgrStackMemberGetFirst(&u) != L7_SUCCESS)
    {
      return L7_NOT_EXIST;
    }

    *unit = u;
    return L7_SUCCESS;
  }

  /* user specified a unit number */
  if (cliConvertTo32BitUnsignedInteger(tmp_buf, &u) == L7_SUCCESS)
  {
    *allUnits = L7_FALSE;
    *unit = u;
#ifdef L7_CHASSIS
    if (usmDbSlotMapUSIntUnitSlotGet(*unit, &chassisIntUnit, &intSlot) != L7_SUCCESS)
    {
      return L7_ERROR;
    }
    else
    {
      *unit = chassisIntUnit;
    }
#else
    /* check if unit is within the allowed range */
    if ((u < L7_UNITMGR_MIN_UNIT_NUMBER) || (u > L7_UNITMGR_MAX_UNIT_NUMBER))
    {
      return L7_ERROR;
    }
#endif

    /* check if unit exists */
    if (cliIsUnitPresent(*unit) != L7_TRUE)
    {
      return L7_NOT_EXIST;
    }
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

#ifdef L7_QOS_PACKAGE

/*********************************************************************
* @purpose  builds a valid string possible for DSCP keywords.
*
* @param    buf  The buffer to be filled with DSCP keyword values.
 * @param    bufSize  Size of the output buffer.
*
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/
void cliDiffservBuildValDscpKeyWords(L7_char8 * buf, L7_uint32 bufSize)
{
  L7_uint32 unit;
  L7_uint32 buflen;

  unit = cliGetUnitId();
if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_PHB_OTHER_FEATURE_ID) == L7_TRUE)
    strcpy(buf, "Enter a DSCP value in the range of 0 to 63 or a DSCP keyword (");
  else
  {
    strcpy (buf, "Enter a DSCP value (");
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF1X_FEATURE_ID) == L7_TRUE)
      strcat(buf, pStrInfo_base_10_12_14);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF2X_FEATURE_ID) == L7_TRUE)
      strcat(buf, pStrInfo_base_18_20_22);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF3X_FEATURE_ID) == L7_TRUE)
      strcat(buf, pStrInfo_base_26_28_30);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF4X_FEATURE_ID) == L7_TRUE)
      strcat(buf, pStrInfo_base_34_36_38);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_CS_FEATURE_ID) == L7_TRUE)
      strcat(buf, pStrInfo_base_0_8_16_24_32_40_48_56);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_EF_FEATURE_ID) == L7_TRUE)
    {
      strcat(buf, pStrInfo_base_46);
    }
    else if (buf != '\0')
    {
      buflen = strlen(buf);
      buf[buflen-2] = '.' ;
      buf[buflen-1] = '\0';
    }
    strcat(buf, pStrInfo_base_orDSCPkeyword);
  }




  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF1X_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Af11Af12Af13, bufSize);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF2X_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Af21Af22Af23, bufSize);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF3X_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Af31Af32Af33, bufSize);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_AF4X_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Af41Af42Af43, bufSize);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Be_2, bufSize);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_CS_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Cs0Cs1Cs2Cs3Cs4Cs5Cs6Cs7, bufSize);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_PHB_EF_FEATURE_ID) == L7_TRUE)
  {
    osapiStrncatAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Ef_3, bufSize);
  }

    buflen = strlen(buf);
  if (buflen > 2)
  {
    if (buf[buflen-1] == '(')
    {
    buf[buflen-2] = '.' ;
    buf[buflen-1] = '\0';
  }
    else if (buf[buflen-1] == ' ')
    {
      buf[buflen-2] = ')' ;
      buf[buflen-1] = '.';
      buf[buflen] = '\0';
    }
  }
}

/*********************************************************************
* @purpose  used for diffserv, Converts a DSCP keyword string to the
*           associated value, if one is available.  If the string holds
*           an integer value, the integer value is returned.
*
* @param    dscpVal         value to return
* @param    dscpString      string to parse
*
* @returns rc
*
* @notes This function should be used for classifier match conditions,
*        where DSCP feature support checking is not needed.
*
* @end
*********************************************************************/

L7_RC_t cliDiffservConvertDSCPStringToValNoVerify(L7_char8 * dscpString, L7_uint32 * dscpVal)
{
  if (strcmp(dscpString, pStrInfo_common_Af11_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af12_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af13_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af21_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af22_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af23_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af31_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af32_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af33_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af41_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af42_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42;
    }
  else if (strcmp(dscpString, pStrInfo_common_Af43_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43;
    }
  else if (strcmp(dscpString, pStrInfo_common_Be_1) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs0) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS0;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs1_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs2_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs3_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs4_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs5_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs6_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6;
    }
  else if (strcmp(dscpString, pStrInfo_common_Cs7_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7;
    }
  else if (strcmp(dscpString, pStrInfo_common_Ef_2) == 0)
    {
        *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF;
    }
    else
    {
        /* verify if the specified  argument is an integer */
        if ( cliCheckIfInteger(dscpString) != L7_SUCCESS)
        {
          return L7_ERROR;
        }

        /* check for overflow of integer value */
        if ( cliConvertTo32BitUnsignedInteger(dscpString, dscpVal) != L7_SUCCESS)
        {
          return L7_ERROR;
        }
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  used for diffserv, Converts a DSCP keyword string to the
*           associated value, if one is available.  If the string holds
*           an integer value, the integer value is returned.
*
* @param    dscpVal         value to return
* @param    dscpString      string to parse
*
* @returns rc
*
* @notes Use this function when checking variousDiffServ actions, such as
*        marking or policing, where DSCP value feature support is required.
*
* @end
*********************************************************************/

L7_RC_t cliDiffservConvertDSCPStringToVal(L7_char8 * dscpString, L7_uint32 * dscpVal)
{
  L7_uint32      unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return L7_FAILURE;
  }

    if (cliDiffservConvertDSCPStringToValNoVerify(dscpString, dscpVal) != L7_SUCCESS)
    {
        return L7_ERROR;
    }

    /* Verify that this value is supported by this diffserv configuration */
    switch (*dscpVal)
    {
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF:  /* ef */
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_PHB_EF_FEATURE_ID) == L7_FALSE)
        {
            return L7_FAILURE;
        }
        break;

    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_PHB_AF1X_FEATURE_ID) == L7_FALSE)
        {
            return L7_FAILURE;
        }
        break;

    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_PHB_AF2X_FEATURE_ID) == L7_FALSE)
        {
            return L7_FAILURE;
        }
        break;

    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_PHB_AF3X_FEATURE_ID) == L7_FALSE)
        {
            return L7_FAILURE;
        }
        break;

    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_PHB_AF4X_FEATURE_ID) == L7_FALSE)
        {
            return L7_FAILURE;
        }
        break;

    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS0:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6:
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_PHB_CS_FEATURE_ID) == L7_FALSE)
        {
            return L7_FAILURE;
        }
        break;

    default:
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_PHB_OTHER_FEATURE_ID) == L7_FALSE)
        {
            return L7_FAILURE;
        }
        break;
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  used for diffserv, Converts a DSCP value to the associated
*           keyword, if one is available.  Otherwise, a string of the
*           input dscpVal is returned.
*
* @param    dscpVal         value to convert
* @param    dscpString      string to return
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t cliDiffservConvertDSCPValToString(L7_uint32 dscpVal, L7_char8 * dscpString)
{
    /* NOTE: Moved core of this function to a common location so that
     *       it can be used by both CLI and Web.
     */
    return cliWebConvertDSCPValToString(dscpVal, dscpString);
}

/*********************************************************************
* @purpose  used for diffserv, Converts a DSCP value to the associated
*           keyword, if one is available.  Otherwise, a string of the
*           input dscpVal is returned.
*
* @param    dscpVal         value to convert
* @param    dscpString      string to return
 * @param    dscpStringSize  size of output buffer
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t cliDiffservConvertDSCPValToKeyword(L7_uint32 dscpVal, L7_char8 * dscpString, L7_uint32 dscpStringSize)
{
  L7_uchar8 * pStr;
  L7_uchar8 dscpValStr[8];

    switch(dscpVal)
    {
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11:
      pStr = pStrInfo_common_Af11_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12:
      pStr = pStrInfo_common_Af12_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13:
      pStr = pStrInfo_common_Af13_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21:
      pStr = pStrInfo_common_Af21_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22:
      pStr = pStrInfo_common_Af22_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23:
      pStr = pStrInfo_common_Af23_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31:
      pStr = pStrInfo_common_Af31_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32:
      pStr = pStrInfo_common_Af32_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33:
      pStr = pStrInfo_common_Af33_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41:
      pStr = pStrInfo_common_Af41_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42:
      pStr = pStrInfo_common_Af42_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43:
      pStr = pStrInfo_common_Af43_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS0:
      pStr = pStrInfo_common_Cs0;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1:
      pStr = pStrInfo_common_Cs1_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2:
      pStr = pStrInfo_common_Cs2_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3:
      pStr = pStrInfo_common_Cs3_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4:
      pStr = pStrInfo_common_Cs4_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5:
      pStr = pStrInfo_common_Cs5_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6:
      pStr = pStrInfo_common_Cs6_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7:
      pStr = pStrInfo_common_Cs7_2;
        break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF:
      pStr = pStrInfo_common_Ef_2;
        break;
    default:
      osapiSnprintf(dscpValStr, sizeof(dscpValStr), "%u", dscpVal);
      pStr = dscpValStr;
      break;
  }
  osapiSnprintf(dscpString, dscpStringSize, pStr);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  used for diffserv, Converts a Ethertype keyword string to the
*           associated keyword id,
*
* @param    strKeyword      string to parse
* @param    etypeKeyId      keyword id to return
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t cliDiffservConvertEtherTypeStringToKeyId(L7_char8 * strKeyword, L7_uint32 * etypeKeyId)
{
  if (strcmp(strKeyword, L7_QOS_ETYPE_STR_APPLETALK) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_APPLETALK;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_ARP) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_ARP;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_IBMSNA) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_IBMSNA;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_IPV4) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_IPV4;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_IPV6) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_IPV6;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_IPX) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_IPX;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_MPLSMCAST) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_MPLSMCAST;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_MPLSUCAST) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_MPLSUCAST;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_NETBIOS) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_NETBIOS;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_NOVELL) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_NOVELL;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_PPPOE) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_PPPOE;
  }
  else if (strcmp(strKeyword, L7_QOS_ETYPE_STR_RARP) == 0)
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_RARP;
  }
  else
  {
      *etypeKeyId = L7_QOS_ETYPE_KEYID_NONE;
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  used for diffserv, Converts a Ethertype keyword ID to its
*           associated keyword string
*
* @param    etypeKeyId      keyword id to return
* @param    strKeyword      string to parse
 * @param    strKeywordSize  size of output buffer
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t cliDiffservConvertEtherTypeKeyIdToString(L7_uint32 etypeKeyId, L7_char8 * strKeyword, L7_uint32 strKeywordSize)
{
  L7_char8 * pStr = L7_NULLPTR;

  switch (etypeKeyId)
  {
    case L7_QOS_ETYPE_KEYID_APPLETALK:
      pStr = L7_QOS_ETYPE_STR_APPLETALK;
      break;

    case L7_QOS_ETYPE_KEYID_ARP:
      pStr = L7_QOS_ETYPE_STR_ARP;
      break;

    case L7_QOS_ETYPE_KEYID_IBMSNA:
      pStr = L7_QOS_ETYPE_STR_IBMSNA;
      break;

    case L7_QOS_ETYPE_KEYID_IPV4:
      pStr = L7_QOS_ETYPE_STR_IPV4;
      break;

    case L7_QOS_ETYPE_KEYID_IPV6:
      pStr = L7_QOS_ETYPE_STR_IPV6;
      break;

    case L7_QOS_ETYPE_KEYID_IPX:
      pStr = L7_QOS_ETYPE_STR_IPX;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSMCAST:
      pStr = L7_QOS_ETYPE_STR_MPLSMCAST;
      break;

    case L7_QOS_ETYPE_KEYID_MPLSUCAST:
      pStr = L7_QOS_ETYPE_STR_MPLSUCAST;
      break;

    case L7_QOS_ETYPE_KEYID_NETBIOS:
      pStr = L7_QOS_ETYPE_STR_NETBIOS;
      break;

    case L7_QOS_ETYPE_KEYID_NOVELL:
      pStr = L7_QOS_ETYPE_STR_NOVELL;
      break;

    case L7_QOS_ETYPE_KEYID_PPPOE:
      pStr = L7_QOS_ETYPE_STR_PPPOE;
      break;

    case L7_QOS_ETYPE_KEYID_RARP:
      pStr = L7_QOS_ETYPE_STR_RARP;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  } /* endswitch */

  osapiStrncpySafe(strKeyword, pStr, strKeywordSize);

  return L7_SUCCESS;
}

#endif

/*********************************************************************
*
* @purpose       Read the default banner
*
* @param  *buffer     @b{(input))  Global array to read the banner
* @param  *buffer     @b{(output)) Global array to read the banner
*
* @returns  void
*
* @comments No default banner to be displayed for FASTPATH.
*
* @end
*
**********************************************************************/

void cliReadDefaultBanner(L7_char8 * buffer, L7_uint32 bufferSize)
{
    /* No default banner to be displayed for FASTPATH */
/*  char tempdefbuffer0[] =
     "\r\n"
     "Fastpath\r\n"
     "\r\n"
     "   LL   VV  VV LL   7777777\r\n"
     "   LL   VV  VV LL   7   77\r\n"
     "   LL   VV  VV LL      77\r\n"
     "   LL    VVVV  LL     77\r\n"
     "   LLLLL  VV   LLLLL 77\r\n"
     "\r\n";

  buffer[0] = '\0';
      osapiStrncpySafe(buffer, tempdefbuffer0, bufferSize); */

  memset (buffer, 0, bufferSize);
}

/*********************************************************************
*
* @purpose       Display the banner
*
* @param  *buffer    @b{(input))  Global array which is displayed
* @param  *context   @b{(input))
*
* @returns  void
*
* @comments
*
* @end
*
**********************************************************************/
void cliBannerDisplay(L7_char8 * buffer)
{
  cliWrite(pStrInfo_common_CrLf);
  cliWrite(buffer);
}

/*********************************************************************
*
* @purpose       Display the banner
*
* @param  *buffer    @b{(input))  Global array which is displayed
* @param  *context   @b{(input))
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void cliBannerDisplaySerial(L7_char8 * buffer)
{
  cliWriteSerial(pStrInfo_common_CrLf);
  cliWriteSerial(buffer);
}

/*********************************************************************
*
* @purpose   Returns the calculated Mask Length using the net mask
*            address as the input.
*
* @param L7_uint32 mask
*
* @returntype const L7_uint32
*
* @returns mask length
*
* @notes
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*********************************************************************/
L7_uint32 usmCliMaskLength(L7_uint32 mask)
{
  L7_uint32 length=0;
  L7_uint32 b;

  b = 0X80000000;

  while(mask & b)
  {
    length++;
    b = b >> 1;
  }

  return length;
}
/*********************************************************************
*
* @purpose   This function fills the max and min unit numbers in the
*            stacking buildthe input.
*
* @param L7_uint32* max
*
* @param L7_uint32* min
*
* @returntype L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliGetMaxMinUnitnum(L7_uint32 * max, L7_uint32 * min)
{

  L7_uint32 numUnits,unit;
  L7_RC_t rc;

  *min = 1;
  rc = usmDbUnitMgrStackMemberGetFirst(&unit);
  numUnits = 0;
  while(rc == L7_SUCCESS)
  {
     numUnits++;
     rc = usmDbUnitMgrStackMemberGetNext(unit, &unit);
  }
  *max = numUnits;
  rc = L7_SUCCESS;
  return rc;
}

/*********************************************************************
*
* @purpose    This function truncates the string and appends "..." such the
*             string does not exceed the size specified by the user and prints
*             it to screen.
*
* @param      EwsContext ewsContext
* @param      L7_char8 *buf
* @param      L7_uint32 *len
*
* @returntype void
*
* @notes      Updates the "buf" with trucation string "..." and prints it.
*
* @end
*
*********************************************************************/
void cliFormatStringTruncate(EwsContext ewsContext,L7_char8 * buf,L7_uint32 len)
{
  L7_char8 message[L7_CLI_MAX_LARGE_STRING_LENGTH];

  (void)osapiStrncpySafe(message, buf, sizeof(message));
  cliStringTruncate(message, len);
  ewsTelnetWrite(ewsContext, message);
}

/*********************************************************************
*
* @purpose    This function truncates the string and appends "..." such the
*             string does not exceed the size specified by the user.
*
* @param      L7_char8 *buf
* @param      L7_uint32 *len - The length to which the string has to be trucated to.
*
* @returntype void
*
* @notes      Updates the "buf" with trucation string "..."
*
* @end
*
*********************************************************************/
void cliStringTruncate (L7_char8 * buf, L7_uint32 len)
{
  L7_uint32     numDots;

  if (strlen(buf) > len)
  {
    numDots = min(3, len);

    while (numDots > 0)
    {
      buf[len-numDots] = '.';
      numDots--;
    }
    buf[len] = L7_EOS;
  }
}

L7_char8 *cliTrimLeft (L7_char8 * str)
{
  if(str)
  {
    while (str && (str[0] == ' ' || str[0] == '\t'))
    {
      str++;
    }
  }
  return str;
}

L7_char8 *cliTrimRight (L7_char8 * str)
{
  if(str && str[0])
  {
    L7_char8 * temp = str + strlen (str) - 1;
    while ((*temp == ' ' || *temp == '\t') && temp >= str)
    {
      *temp-- = 0;
    }
  }
  return str;
}

L7_char8 *cliTrim (L7_char8 * str)
{
  cliTrimRight (str);
  return cliTrimLeft (str);
}

L7_BOOL checkPrintRemain(L7_uint32 count, snmpAttribute * attrib)
{
  int i;
  for(i = 0; i < count; i++)
  {
    if(attrib[i].flagRemain == L7_TRUE)
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

void printRemain(EwsContext ewsContext, L7_uint32 count, snmpAttribute * attrib)
{
  L7_uchar8 temp[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  for(i = 0; i < count; i++)
  {
    if(attrib[i].flagRemain == L7_TRUE)
    {
      memset(buf, 0, sizeof(buf));
      memset(temp, 0, sizeof(temp));
      osapiSnprintf(buf, sizeof(buf), "%*.*s", -attrib[i].width, attrib[i].precision, attrib[i].remainStr);
      if(strlen(attrib[i].remainStr) > (attrib[i].width))
      {
        strncpy(temp,&(attrib[i].remainStr[attrib[i].precision]),(strlen(attrib[i].remainStr) - attrib[i].precision));
        strcpy(attrib[i].remainStr,temp);
        attrib[i].flagRemain = L7_TRUE;
      }
      else
      {
        attrib[i].flagRemain = L7_FALSE;
      }
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "%*.*s", -attrib[i].width, attrib[i].precision, "");
    }
    ewsTelnetWrite(ewsContext,buf);
  }
}

/*********************************************************************
 *
 * @purpose to check whether the password got expired for a user
 *
 *
 * @param L7_char8 *enteredInfo, EwsContext context
 *
 * @returntype L7_BOOL
 * @returns L7_TRUE
 * @returns L7_FALSE
 *
 * @notes this function is run twice to first get the password and confirm
 * @notes password, if both match that stored, returns L7_TRUE
 * @notes currently is not set up for the system super password
 *
 *
 * @end
 *
 *********************************************************************/
L7_BOOL cliPasswdExpireCheck (L7_char8 * enteredInfo, EwsContext context)
{
  L7_RC_t rc;
  L7_uint32 unit;
  L7_uint32 userIndex;
  L7_char8 currentUser[L7_LOGIN_SIZE];
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(context, CLIERROR_UNIT_ID);
    ewsFlushAll(context);
    return L7_FALSE;
  }
  usmDbLoginSessionUserGet(unit, cliLoginSessionIndexGet(), currentUser);
  usmDbUserLoginIndexGet(unit, currentUser, &userIndex);
  rc = usmDbPasswdAgingStatusGet();

  if (rc == L7_SUCCESS)
  {
    rc = usmDbPasswordExpiredCheck(unit, userIndex);
    if (rc == L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, context, pStrErr_base_PasswdAgeTimedout);
      ewsFlushAll(context);
      return L7_FALSE;
    }
  }

  return L7_TRUE;
}

/*********************************************************************
 *
 * @purpose  change an existing user password
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32  * pCallCnt
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes add functionality, currently has NONE,
 * @notes also make passwords not case sensitive
 *
 * @cmdsyntax  password <CR>
 *
 * @cmdhelp Change an existing user's password.
 *
 * @cmddescript
 *   The password can be up to eight alphanumeric characters and is not
 *   case sensitive. A blank password indicates no password. The default
 *   value is blank.
 *
 * @end
 *
 *********************************************************************/
L7_BOOL configExpiredPasswd(EwsContext context, L7_char8 * enteredInfo, L7_uint32 * pCallCnt)
{
  L7_char8 currentUser[L7_LOGIN_SIZE];
  L7_uint32 loginIndex;
  L7_RC_t rc;
  
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  if (!(*pCallCnt))
  {
    if (enteredInfo != L7_NULL)
    {
      osapiStrncpySafe(cliCommon[cliUtil.handleNum].password, enteredInfo, sizeof(cliCommon[cliUtil.handleNum].password));
    }

    *(pCallCnt) = 1;
    ewsTelnetWrite(context, pStrInfo_base_ConfirmPasswd_1);
    ewsFlushAll(context);
    return L7_FALSE;
  }

  usmDbLoginSessionUserGet(0,cliLoginSessionIndexGet(), currentUser);
  usmDbUserLoginIndexGet (0, currentUser, &loginIndex);

  if (strcmp(enteredInfo, cliCommon[cliUtil.handleNum].password) == 0)
  {
    memset(cliCommon[cliUtil.handleNum].password, 0, sizeof(cliCommon[cliUtil.handleNum].password));
    rc = usmDbPasswordSet(0, loginIndex, enteredInfo, L7_FALSE);
    if (rc != L7_SUCCESS)
    {
      cliUserMgrPasswdErrorStringGet(enteredInfo, rc, buf, sizeof(buf));
      ewsTelnetWrite(context,buf);
      ewsTelnetWrite (context, pStrInfo_base_NewPasswd);
      ewsFlushAll(context);
      *(pCallCnt) = 0;
      return L7_FALSE;
    }

    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, context, pStrInfo_base_PasswdChgd);
    return L7_TRUE;
  }

  *(pCallCnt) = 0;
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, context, pStrInfo_base_PasswdNotMatched);
  ewsTelnetWrite(context, pStrInfo_base_NewPasswd);
  ewsFlushAll(context);

  return L7_FALSE;
}

/*********************************************************************
 *
 * @purpose parse a string into xfer type. ip, dir and filename
 *
 * @param L7_char8 *in_str
 * @param L7_char8 *ipaddr
 * @param L7_uint32 ipaddrSize
 * @param L7_char8 *path
 * @param L7_uint32 pathSize
 * @param L7_char8 *filename
 * @param L7_uint32 filenameSize
 * @param L7_char8 *xferToken
 * @param L7_uint32 xferTokenSize
 *
 * @returns L7_uint32
 *
 * @end
 *
 *********************************************************************/
L7_uint32 cliUrlParser (L7_char8 * in_str, L7_char8 * ipaddr, L7_uint32 ipaddrSize,
    L7_char8 * path, L7_uint32 pathSize, L7_char8 * filename,
    L7_uint32 filenameSize, L7_char8 * xferToken, L7_uint32 xferTokenSize)
{
  L7_char8 * pbuf;
  L7_char8 * ptmp;
  L7_uint32 colonIndex;
  L7_uint32 fileIndex;
  L7_uint32 pathIndex;
  L7_uint32 tokenLen = strlen (pStrInfo_base_Xmodem_1);
  L7_uint32 inStrLen = strlen (in_str);

  if (inStrLen < 1)             /* At least tftp:x should be there */
  {
    return L7_FAILURE;
  }

  if ((osapiStrncmp (in_str, pStrInfo_base_Xmodem_1, tokenLen) == 0) ||
      (osapiStrncmp (in_str, pStrInfo_base_Ymodem_1, tokenLen) == 0) ||
      (osapiStrncmp (in_str, pStrInfo_base_Zmodem_1, tokenLen) == 0))
  {
    osapiStrncpySafe (xferToken, in_str, xferTokenSize);
    *ipaddr = '\0';
    *path = '\0';
    *filename = '\0';
    return L7_SUCCESS;
  }

  /* check for ':' */
  pbuf = strchr (in_str, ':');
  if (pbuf == L7_NULLPTR)       /* token separator not found */
  {
    return L7_FAILURE;
  }

  /* Location of first ':' */
  colonIndex = inStrLen - strlen (pbuf);
  if (colonIndex == strlen (pStrInfo_common_Tftp_1))
  {
    tokenLen = colonIndex;
  }
  else
  {
    return L7_FAILURE;
  }

  if (osapiStrncmp (in_str, pStrInfo_common_Tftp_1, tokenLen) != 0)
  {
    return L7_FAILURE;
  }

  osapiStrncpySafe (xferToken, in_str, min ((tokenLen + 1), xferTokenSize));

  /* Get filename */
  /* check for last '/' */
  pbuf = strrchr (in_str, '/');
  if (pbuf == L7_NULLPTR)
  {
    return L7_FAILURE;          /* No filename */
  }

  /* filename start index */
  fileIndex = inStrLen - strlen (pbuf);
  tokenLen = strlen (pbuf) - 1;
  if (tokenLen > 0)
  {
    osapiStrncpySafe (filename, in_str + fileIndex + 1, min ((tokenLen + 1), filenameSize));
  }
  else
  {
    return L7_FAILURE;          /* No filename */
  }

  /* Get path */
  /* check for first '/' after 'tftp://' */
  ptmp = strstr (in_str, "//");
  if (ptmp == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  pbuf = strchr (ptmp + 2, '/');
  if (pbuf == L7_NULLPTR)
  {
    return L7_FAILURE;          /* No filename */
  }

  /* path start index */
  pathIndex = inStrLen - strlen (pbuf);
  if (pathIndex == fileIndex)
  {
    strcpy (path, "./");        /* Default path current dir */
    tokenLen = 0;
  }
  else
  {
    tokenLen = fileIndex - pathIndex - 1;
  }

  if (tokenLen > 0)
  {
    osapiStrncpySafe (path, in_str + pathIndex + 1, min ((tokenLen + 1), pathSize));
  }

  /* tftp xfer type */
  /* ip addres is from colonIndex to pathIndex */
  tokenLen = pathIndex - colonIndex - 3;
  if (tokenLen > 0)
  {
    osapiStrncpySafe (ipaddr, in_str + colonIndex + 3, min ((tokenLen + 1), ipaddrSize));
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*
 * Helper functions to give access pStrings
 * called from the emweb server code
 */
char *cliUtilGetRootPrompt (void)
{
  return pStrInfo_base_RootPrompt;
}

char *cliutilGetTelnetDisabledError(void)
{
  return pStrErr_base_TelnetDsbld;
}

char *cliutilGetMaxConnectionsError(void)
{
  return pStrErr_base_MaxConns;
}

char *cliutilGetConnectionClosedMsg(void)
{
  return pStrInfo_base_ConnClosed;
}

L7_BOOL cliIsPromptRespQuit (void)
{
  switch(cliGetCharInput())
  {
    case 'q':
    case 'Q':
    case 26:
      return L7_TRUE;
    default:
      break;
  }
  return L7_FALSE;
}

L7_RC_t cliUtilsCodeVersionGet(L7_uint32 unit, L7_char8 *buf, L7_uint32 bufSize)
{
  usmDbCodeVersion_t ver;
  L7_RC_t rc = L7_SUCCESS;

  if (cliIsStackingSupported() == L7_FALSE)
  {
    return usmDbSwVersionGet(unit, buf);
  }

  rc = usmDbUnitMgrDetectCodeVerRunningGet(unit, &ver);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  if (isalpha(ver.rel))
  {
    osapiSnprintf(buf, bufSize, "%c.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }
  else
  {
    osapiSnprintf(buf, bufSize, "%d.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose Write the output to either buffer or session based on flags
 *          Also takes care of prepending and appending CR and blanks
 *
 * @param L7_uint32 lcr
 * @param L7_uint32 tcr
 * @param L7_uint32 lsp
 * @param L7_uint32 tsp
 * @param L7_char8 *prefix
 * @param EwsContext
 * @param context
 * @param L7_char8 *text
 * @param CLI_EWS_WRITE_FLAGS_t flags
 *
 * @returns L7_BOOL
 *
 * @end
 *
 *********************************************************************/
L7_BOOL cliEwsWriteBuffer (L7_uint32 lcr, L7_uint32 tcr, L7_uint32 lsp, L7_uint32 tsp,
    L7_char8 * prefix, EwsContext context, L7_char8 * text, CLI_EWS_WRITE_FLAGS_t flags)
{
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 * scriptCommand;

#ifdef PSTRING_BUG_SIZE
  char buf[1024];
  pStringBufAddBlanks(lcr, tcr, lsp, tsp, prefix, buf, sizeof(buf), text);
#else
  char * buf = text;
#endif

  if(context->runningConfig.init == L7_TRUE)
  {
    for(i = 0; i < sizeof(context->runningConfig.showExit)/sizeof(context->runningConfig.showExit[0]); i++)
    {
      context->runningConfig.showExit[i] = L7_FALSE;
      context->runningConfig.showMode[i] = L7_FALSE;
      memset(context->runningConfig.modeStr[i], 0, sizeof(context->runningConfig.modeStr[i]));
    }
    context->runningConfig.init = L7_FALSE;
  }

  if (flags == CLI_EWS_WRITE_FLAGS_NORMAL ||
      flags == CLI_EWS_WRITE_FLAGS_MAIN || (
        context->runningConfig.hideMode == L7_FALSE && (
          flags == CLI_EWS_WRITE_FLAGS_MODE_START ||
          flags == CLI_EWS_WRITE_FLAGS_MODE_END)))
  {
    if (buf[0] == '\0')
    {
      /* Because of zero length strings some modes are displayed even there is no
         configuraion in those modes */
      return L7_FALSE;
    }

    /* if the modestr is set - display it */
    /* We need to check up to top level Modes, all are already displayed or not
     * in nested modes case */
    if(context->runningConfig.hideMode == L7_TRUE && context->runningConfig.showMode[context->runningConfig.current] == L7_TRUE)
    {
      for(i = 0; i < context->runningConfig.count; i++)
      {
        if (context->runningConfig.showMode[i] == L7_TRUE)
        {
          if(context->showRunningOutputFlag == L7_WRITE_TERMINAL)
          {
            ewsTelnetWrite(context, context->runningConfig.modeStr[i]);
            rc = L7_SUCCESS;
          }
          else
          {
            rc = setNextConfigScriptCommand(context->runningConfig.modeStr[i], context->configScriptData);
          }
          context->runningConfig.showMode[i] = L7_FALSE;
          context->runningConfig.showExit[i] = L7_TRUE;
        }
      }
    }

    if (rc == L7_SUCCESS)
    {
      /*Based on the flag value it will go to terminal or buffer*/
      if(context->showRunningOutputFlag == L7_WRITE_TERMINAL)
      {
         /* The direct show running config output is not formatted.
            So we have to format or aligne using trim functions*/
         scriptCommand = trimLeft( buf);
         scriptCommand = trimLeftBlanks(scriptCommand);
         if(strlen(scriptCommand) != 0)
         {
           trimRightBlanks(scriptCommand);
           ewsTelnetWrite(context, "\r\n");
           ewsTelnetWrite(context, scriptCommand);
         }
         rc = L7_SUCCESS;
      }
      else
      {
         rc = setNextConfigScriptCommand(buf, context->configScriptData);
      }
    }
  }
  else if(flags == CLI_EWS_WRITE_FLAGS_MODE_START)
  {
    context->runningConfig.showMode[context->runningConfig.count] = L7_TRUE;
    context->runningConfig.current = context->runningConfig.count;
    /* New mode is started we should raise the count one */
    osapiStrncpySafe(context->runningConfig.modeStr[context->runningConfig.current], buf, sizeof(context->runningConfig.modeStr[context->runningConfig.current]));
    context->runningConfig.count++;
    return L7_FALSE;
  }
  else if(flags == CLI_EWS_WRITE_FLAGS_MODE_END)
  {
    context->runningConfig.showMode[context->runningConfig.current] = L7_FALSE;

    if(context->runningConfig.showExit[context->runningConfig.current] == L7_TRUE)
    {
      if(context->showRunningOutputFlag == L7_WRITE_TERMINAL)
      {
         ewsTelnetWrite(context, buf);
         rc = L7_SUCCESS;
      }
      else
      {
         rc = setNextConfigScriptCommand(buf, context->configScriptData);
      }
      context->runningConfig.showExit[context->runningConfig.current] = L7_FALSE;
      memset(context->runningConfig.modeStr[context->runningConfig.current], 0, sizeof(context->runningConfig.modeStr[context->runningConfig.current]));
    }
    /* we have out from the current mode.So we should reduce count by one */
    context->runningConfig.count--;
    context->runningConfig.current--;
  }
  else if(flags == CLI_EWS_WRITE_FLAGS_MODE_HIDE)
  {
    context->runningConfig.hideMode = L7_TRUE;
    return L7_FALSE;
  }
  else if(flags == CLI_EWS_WRITE_FLAGS_MODE_NOHIDE)
  {
    context->runningConfig.hideMode = L7_FALSE;
    return L7_FALSE;
  }
  else if(flags == CLI_EWS_WRITE_FLAGS_MODE_SHOW)
  {
    sysapiPrintf("\r\ncount = %d context->runningConfig.current = %d", context->runningConfig.count,
     context->runningConfig.current);
    for(i = 0; i < sizeof(context->runningConfig.showExit)/sizeof(context->runningConfig.showExit[0]); i++)
    {
      if(context->runningConfig.showExit[i] == L7_FALSE)
      {
        sysapiPrintf("\r\n %d Exit FALSE\r\n",i);
      }
      else
      {
        sysapiPrintf("\r\n %d Exit TRUE\r\n",i);
      }

      if(context->runningConfig.showMode[i] == L7_FALSE)
      {
        sysapiPrintf(" %d Mode FALSE\r\n",i);
      }
      else
      {
        sysapiPrintf(" %d Mode FALSE\r\n",i);
      }
    }
    return L7_FALSE;
  }
  else
  {
    return L7_FALSE;
  }

  if(rc == L7_FAILURE && flags == CLI_EWS_WRITE_FLAGS_MAIN)
  {
    ewsTelnetWrite(context, getLastError());
    releaseScriptBuffer(context->configScriptData);
    cliSyntaxBottom(context);
    context->unbufferedWrite = L7_FALSE;
    return L7_TRUE;
  }
  return L7_FALSE;
}

/* Starting of debugging functions */
void cliSetRunningModeHide(L7_BOOL flag)
{
  if(flag == L7_FALSE)
  {
    cliEwsWriteBuffer(0,0,0,0,L7_NULLPTR, 0, 0, CLI_EWS_WRITE_FLAGS_MODE_HIDE);
  }
  else
  {
    cliEwsWriteBuffer(0,0,0,0,L7_NULLPTR, 0, 0, CLI_EWS_WRITE_FLAGS_MODE_NOHIDE);
  }
}

void cliSetRunningModeShow(void)
{
  cliEwsWriteBuffer(0,0,0,0,L7_NULLPTR, 0, 0, CLI_EWS_WRITE_FLAGS_MODE_SHOW);
}
/* Ending of debugging functions */

/*********************************************************************
*
* @purpose Validate Host address string and covert it to a 32 bit integer
*          and return type of address (IPV4 or DNS).
*
* @param EwsContext           context    Ews Context.
* @param L7_uchar8            *hostAddr  Host Address.
* @param L7_uint32            *ival      32 bit integer representation
* @param L7_IP_ADDRESS_TYPE_t *addrType   Address Type.
*
* @returns L7_SUCCESS, or
* @returns L7_FAILURE
* @returns L7_ERROR
*
* @notes Can use hex, octal or decimal input.  Places result in
* @notes location pointed to by 'ival'in HOST BYTE ORDER.
*
* @end
*
*********************************************************************/
L7_RC_t cliIPHostAddressValidate(EwsContext context, L7_uchar8 *hostAddr,
                               L7_uint32 *ival, L7_IP_ADDRESS_TYPE_t *addrType)
{
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbIPHostAddressValidate(hostAddr, ival, addrType);
  if (rc == L7_ERROR)
  {
    /* Invalid Ip Address */
    cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
           context, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    return L7_ERROR;
  }
  else if (rc == L7_FAILURE)
  {
    /* Invalid Host Name*/
    cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
           context, pStrErr_base_InvalidHostName_space);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/* This is for Password Recovery*/
/******************************************************************************
*
* @purpose To Get the environmental varible used to check whether the passoword
           flag is set for default passoword for passoword recovery or Not.
*
* @param L7_int32 *envVar
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************************/
L7_RC_t passwdRecoveryFlagGet(L7_int32 *envVar)
{

 envVar_t envVaribles;
 memset(&envVaribles,0x00,sizeof(envVar_t));
 if(envVar == L7_NULLPTR)
 {
   return L7_ERROR;
 }
 /* This implemenation was modified bcz linux implementaiton of
 * bootConfig.c was a script file and used text based file handling
 */

#ifdef _L7_OS_VXWORKS_
  /* Get the data from a binary file bcz its a binary file for this OS */

  /* Open the file and read the contents inside it */
  if(osapiFsRead("envFile",(L7_char8 *)&envVaribles,sizeof(envVar_t)) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

#elif _L7_OS_LINUX_

 /* Get the data from a text file because its text file for this OS */
 L7_char8 passwordBuff[50],*tempPtr = L7_NULLPTR;

 memset(passwordBuff,0x00,sizeof(passwordBuff));

 /* Open the text file and convert the text data to integer */
 /* PTin modified: paths */
 if(osapiFsRead("/usr/local/ptin/log/fastpath/envFile",(L7_char8 *)passwordBuff,sizeof(passwordBuff)) != L7_SUCCESS)
 {
    return L7_FAILURE;
 }
 tempPtr = strstr(passwordBuff,"defaultPwdFlag=");
 if(tempPtr == L7_NULL)
 {
     return L7_ERROR;
 }

 tempPtr = tempPtr + strlen("defaultPwdFlag=");
 envVaribles.defaultPasswordFlag = atoi(tempPtr);
#endif
*envVar = envVaribles.defaultPasswordFlag;
return L7_SUCCESS;

}
/****************************************************************************
*
@purpose To Set the environmental varible used to check whether the passoword
           flag is set for default passoword for passoword recovery or Not.
*
* @param L7_int32 value
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
******************************************************************************/

L7_RC_t passwdRecoveryFlagSet(L7_uint32 value)
{

/* This implemenation was modified bcz linux implementaiton of
 * bootConfig.c was a script file and used text based file handling
 */
  envVar_t envVaribles;

  memset(&envVaribles,0x00,sizeof(envVar_t));

  envVaribles.defaultPasswordFlag = value;

#ifdef _L7_OS_VXWORKS_
  /* The file exists now,Write the flag into file */
  if(osapiFsWrite("envFile",(L7_char8 *)&envVaribles,sizeof(envVar_t)) != L7_SUCCESS)
  {
     return L7_ERROR;
  }
#elif _L7_OS_LINUX_
  L7_char8 passwordBuff[50];

    memset(passwordBuff,0x00,sizeof(passwordBuff));
  /* Open the text file and Write a text data to the file */
  osapiSnprintf(passwordBuff,sizeof(passwordBuff),"defaultPwdFlag=%d",envVaribles.defaultPasswordFlag);
    /* PTin modified: paths */
    if(osapiFsWrite("/usr/local/ptin/log/fastpath/envFile",passwordBuff,sizeof(passwordBuff)) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
#endif
    return L7_SUCCESS;
}

L7_RC_t cliParseRangeInput(const L7_char8 *buf, L7_uint32 *count, L7_uint32 *list, L7_uint32 listSize)
{
  L7_int32 i, j, index = 0;
  L7_int32 buf_len, start, end;
  L7_char8 *comma = ",";
  L7_char8 *hyphen = "-";
  L7_char8 *retstring;
  L7_char8  tmp_buf[L7_CLI_MAX_STRING_LENGTH], *input;

  buf_len = (L7_uint32)strlen(buf);

  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  strncpy(tmp_buf, buf, sizeof( tmp_buf ) - 1);

  if (strcmp(tmp_buf, "") == 0)
  {
    return L7_FAILURE;
  }

  input = tmp_buf;

  for (i = 0; i < strlen(input); i++)
  {
    if (!(isdigit(input[i]) || input[i]=='-' || input[i]==','))
    {
      return L7_FAILURE;
    }
  }

  index = 0;
  i = 0;
  start = -1;

  do
  {
    if (input[i] == ',')
    {
      if (i==0)
      { /* can't start with separation character */
        return L7_FAILURE;
      }
      /* end of string, ',' and '-' after ',' are not expected */
      if ((input[i+1] != '\0') && (input[i+1] != ',') && (input[i+1] != '-'))
      {
        retstring = cliStringSeparateHelp(&input, comma);
        if (start < 0)
        {
          if (index >= listSize)
          {
            return L7_FAILURE;
          }
          list[index] = atoi(retstring);
          index++;
        }
        else /* value after hyphen */
        {
          end = atoi(retstring);

          /* Improper range input */
          if (end < start)
          {
            return L7_FAILURE;
          }

          for (j = start; j <= end; j++)
          {
            if (index >= listSize)
            {
              return L7_TABLE_IS_FULL;
            }
            list[index] = j;
            index++;
          }
          start = -1;
          end = 0;
        }
        i = 0;
      }
      else /* Invalid char found at the end of the string */
      {
        return L7_FAILURE;
      }
    }
    else if (input[i] == '-')
    {
      if (i==0)
      { /* can't start with separation character */
        return L7_FAILURE;
      }
      if ((start < 0) && (input[i+1] != '\0'))
      {
        retstring = cliStringSeparateHelp(&input, hyphen);
        i = 0;
        start = atoi(retstring);
      }
      else /* Invalid char found at the end of the string */
      {
        return L7_FAILURE;
      }
     }
    else if (input[i] == '\0')
    {
      if (start < 0) /* Last value in the string */
      {
        if (index >= listSize)
        {
          return L7_FAILURE;
        }
        list[index] = atoi(input);
        index++;
      }
      else /* value after hyphen */
      {
        end = atoi(input);

        /* Improper range input */
        if (end < start)
        {
          return L7_FAILURE;
        }

        for (j = start; j <= end; j++)
        {
          if (index >= listSize)
          {
            return L7_FAILURE;
          }
          list[index] = j;
          index++;
        }
      }
      break;
    }
    else
    {
      i++;
    }

  } while ((strcmp(input,"") != 0)); /* till end of string */

  *count = index;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function takes input an array of numbers and sort
*           those numbers
* @param *x[]       @b{(input/output)) input array of integers
* @param n          @b{(input)) number of elements
*
* @returntype void
*
* @notes need to add argument checking
*
* @author Ravindra
*
* @cmddescript  Utility functions that shows the address table entry fields
*
* @end
*
*********************************************************************/
void bubbleSort(int x[], int n)
{
  L7_uint32 hold, j, pass;
  L7_uint32  switched = 1;

  for (pass = 0; (pass < n-1) && (switched == 1); pass++)
  {
    switched = 0;
    for (j = 0; j < n - pass - 1; j++)
    {
      if (x[j] > x[j+1])
      {
        switched = 1;
        hold = x[j];
        x[j] = x[j+1];
        x[j+1] = hold;
      }
    } /* end of inner for loop */
  }/* end of main for loop */
} /* end of bubble function */
void displayRangeFormat(EwsContext ewsContext, L7_uint32 *vids, L7_uint32 numVids,  L7_char8 * commandString)
{
  L7_uint32 firstVid = 0;
  L7_uint32 lastVid = 0;
  L7_uint32 i = 0;
  L7_uint32 cmdLen  = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  bubbleSort(vids, numVids);

  firstVid = lastVid = vids[0];
  cmdLen   = 0;

  for ( i = 1; i < numVids; i++ )
  {
    if ( vids[i] > lastVid + 1 ) /* If there is a gap */
    {
      cmdLen = addIncludeVid(stat, firstVid, lastVid, cmdLen, commandString);
      if ( cmdLen > CLI_CONSOLE_WIDTH - 11 ) /* 11 is max number of chars for a range */
      {
          /*
           * Output the string to avoid a buffer overflow.
           */
         EWSWRITEBUFFER(ewsContext, stat);
         cmdLen = 0;
      }
      firstVid = vids[i];
    }

    lastVid = vids[i];
  }
  addIncludeVid(stat, firstVid, lastVid, cmdLen, commandString);
  EWSWRITEBUFFER(ewsContext, stat);
}

L7_int32 addIncludeVid(L7_uchar8 *buffer, L7_uint32 firstVid, L7_uint32 lastVid, L7_int32 cmdLen, L7_char8 * commandString)
{
  L7_char8 *commaString = ",";

  if ( 0 == cmdLen )
  {
    cmdLen     = sprintf(buffer, "%s", commandString);
    commaString = "";
  }

  if ( firstVid != lastVid ) /* It is a range */
  {
    cmdLen += sprintf(&buffer[cmdLen], "%s%u-%u", commaString, firstVid, lastVid);
  }
  else                    /* It is a single vid */
  {
    cmdLen += sprintf(&buffer[cmdLen], "%s%u", commaString, firstVid);
  }

  return cmdLen;
}
/*********************************************************************
*
* @purpose This function converts internal interface number list
*          into a interface range string
*
* @param   EwsContext ewsContext
* @param   L7_uint32 *intfList  Internal Interface List
* @param   L7_uint32 intfCount  Number of interfaces in the list
* @param   L7_char8 * commandString  Interanl interface range string 
*          is displayed along with this passed commandString.
*
* @returntype void
*
*********************************************************************/
void displayUSPRangeFormat(EwsContext ewsContext, L7_uint32 *intfList, L7_uint32 intfCount,  L7_char8 * commandString)
{
  L7_uint32 startIntf = 0;
  L7_uint32 endIntf = 0;
  L7_uint32 i = 0;
  L7_uint32 cmdLen  = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  bubbleSort(intfList, intfCount);

  startIntf = endIntf = intfList[0];
  cmdLen   = 0;

  for ( i = 1; i < intfCount; i++ )
  {
    if ( intfList[i] > endIntf + 1 ) /* If there is a gap */
    {
      cmdLen = addIncludeIntf(stat, startIntf, endIntf, cmdLen, commandString);
      if ( cmdLen > CLI_CONSOLE_WIDTH - 11 ) /* 11 is max number of chars for a range */
      {
          /*
           * Output the string to avoid a buffer overflow.
           */
         EWSWRITEBUFFER(ewsContext, stat);
         cmdLen = 0;
      }
      startIntf = intfList[i];
    }

    endIntf = intfList[i];
  }
  addIncludeIntf(stat, startIntf, endIntf, cmdLen, commandString);
  EWSWRITEBUFFER(ewsContext, stat);
}

L7_int32 addIncludeIntf(L7_uchar8 *buffer, L7_uint32 startIntf, L7_uint32 endIntf, L7_int32 cmdLen, L7_char8 * commandString)
{
  L7_char8 *commaString = ",";
  L7_uint32 unit, slot, port;

  if ( 0 == cmdLen )
  {
    cmdLen     = sprintf(buffer, "%s", commandString);
    commaString = "";
  }

  if ( startIntf != endIntf ) /* It is a range */
  {
    if (usmDbUnitSlotPortGet(startIntf, &unit, &slot, &port) == L7_SUCCESS)
    {
      if (cliIsStackingSupported() == L7_TRUE)
      {
        cmdLen += sprintf(&buffer[cmdLen], "%s%u/%u/%u", commaString, unit, slot, port);
      }
      else
      {
        cmdLen += sprintf(&buffer[cmdLen], "%s%u/%u", commaString, slot, port);
      }
    }
    if (usmDbUnitSlotPortGet(endIntf, &unit, &slot, &port) == L7_SUCCESS)
    {
      if (cliIsStackingSupported() == L7_TRUE)
      {
        cmdLen += sprintf(&buffer[cmdLen], "-%u/%u/%u", unit, slot, port);
      }
      else
      {
        cmdLen += sprintf(&buffer[cmdLen], "-%u/%u", slot, port);
      }
    }
  }
  else                    /* It is a single vid */
  {
    if (usmDbUnitSlotPortGet(startIntf, &unit, &slot, &port) == L7_SUCCESS)
    {
      if (cliIsStackingSupported() == L7_TRUE)
      {
        cmdLen += sprintf(&buffer[cmdLen], "%s%u/%u/%u", commaString, unit, slot, port);
      }
      else
      {
        cmdLen += sprintf(&buffer[cmdLen], "%s%u/%u", commaString, slot, port);
      }
    }
  }

  return cmdLen;
}

/*********************************************************************
*
* @purpose Split Long names in short name (18 characters in a row) and
*          give it to caller.
*
*
* @param EwsContext ewsContext
* @param L7_char8 **name String to print
* @param L7_char8 *splitName split name.
*
* @returntype L7_RC_t
* @returns    L7_SUCCESS, L7_FAILURE
*
* @notes this function will split out the L7_char8* passed in with
*        18 character in a row and pass it to calling function.
*
* @end
*
*********************************************************************/
L7_RC_t cliSplitLongNameGet(L7_uchar8 **name, L7_uchar8 *splitName)
{
  L7_uchar8 printLabel[CLI_MAX_LABLE_LENGTH];
  L7_uchar8 *printStart;
  L7_uchar8 *printTraverse;
  L7_uint32 index = 0;

  memset(printLabel, L7_NULL, sizeof(printLabel));
  printStart = *name;
  printTraverse = *name;

  while ((printTraverse - printStart) < CLI_MAX_CHARACTERS_PER_ROW)
  {
    printLabel[index] = *printTraverse;
    if (*printTraverse == L7_EOS) break;
    printTraverse++;
    index++;
  }

  if (*printTraverse == L7_EOS)
  {
    strcpy(splitName, printLabel);
    *name = printTraverse;
    return L7_FAILURE;
  }
  else
  {
    strcpy(splitName, printLabel);
    *name = printTraverse;
    return L7_SUCCESS;
  }
}


/*********************************************************************
*
* @purpose Convert month name to number value
*
* @param   *monthName     -- Name of the month
* @param   *monthNumber   --  number equivalent to monthName
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end
* @author Harvinder Singh
*********************************************************************/

L7_RC_t cliMonthNumberGet(const char *monthName, L7_uint32 *monthNumber)
{
  L7_uint32     i;

  for(i=1; i<=12; i++)
  {
    if(cliNoCaseCompare((L7_char8 *)monthName, (L7_char8 *)monthsOfYear[i]) == L7_TRUE)
    {
      *monthNumber = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Convert month name to number value
*
* @param   *dayName     -- Name of the day
* @param   *dayNumber   --  number equivalent to dayName
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end
* @author Harvinder Singh
*********************************************************************/
L7_RC_t cliDayOfWeekGet(const char *dayName, L7_uint32 *dayNumber)
{
  L7_uint32     i;

const L7_uchar8 *daysOfWeek[] = {
          "",
    "Sun",
    "Mon",
   "Tue",
 "Wed",
  "Thu",
    "Fri",
  "Sat"
};

  for(i=1; i<=7; i++)
  {
    if(cliNoCaseCompare((L7_char8 *)dayName, (L7_char8 *)daysOfWeek[i]) == L7_TRUE)
    {
      *dayNumber = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose Convert day-of-the-week name to number value
*
* @param   *dayName     -- Name of the day
* @param   *dayNumber   --  number equivalent to dayName
*
* @returns   L7_SUCCESS or L7_FAILURE
*
* @end
* @author Siva Mannem
*********************************************************************/
L7_RC_t cliFullDayOfWeekGet(const char *dayName, L7_uchar8 *dayNumber)
{
  L7_uchar8     i;

  for(i=1; i<=7; i++)
  {
    if(cliNoCaseCompare((L7_char8 *)dayName, (L7_char8 *)daysOfWeek[i]) == L7_TRUE)
    {
      *dayNumber = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}



/*********************************************************************

* @purpose      Validates the recurring and non-recurring summertime
*               parameters.
*
* @comments
*
* @end

**********************************************************************/
L7_RC_t cliSummerTimeValidate (L7_uint32 stMonth,
                                L7_uint32 stDay, L7_uint32 stYear, L7_uint32 stWeek,
                                L7_uint32 endMonth, L7_uint32 endDay,
                                L7_uint32 endYear, L7_uint32 endWeek, L7_BOOL recurFlag)
{
  if (recurFlag == L7_FALSE)
  {

    if ((stMonth >= 1 && stDay <= daysInMonth [stMonth]) &&
        (stDay >= 1 && stDay <= daysInMonth [stMonth]) &&
        (stYear >= L7_START_SUMMER_TIME_YEAR && stYear <= L7_END_SUMMER_TIME_YEAR) &&
        (endMonth >= 1 && endMonth <= daysInMonth [endMonth]) &&
        (endDay >= 1 && endDay <= daysInMonth [endMonth]) &&
        (endYear >= L7_START_SUMMER_TIME_YEAR && endYear <= L7_END_SUMMER_TIME_YEAR)
        )
    {
      if (stDay != 0 && stMonth != 0 && stYear != 0 && endDay != 0 && endMonth != 0 && endYear != 0)
      {
        return L7_SUCCESS;
      }
    }
  }
  {
    if ((stWeek >= L7_WEEK_CNT_MIN && stWeek <= L7_WEEK_CNT_MAX) &&
        (stDay >= L7_WEEK_DAY_CNT_MIN && stDay <= L7_WEEK_DAY_CNT_MAX) &&
        (stMonth >= L7_MONTH_CNT_MIN && stMonth <= L7_MONTH_CNT_MAX) &&
        (endWeek >= L7_WEEK_CNT_MIN && endWeek <= L7_WEEK_CNT_MAX) &&
        (endDay >= L7_WEEK_DAY_CNT_MIN && endDay <= L7_WEEK_DAY_CNT_MAX) &&
        (endMonth >= L7_MONTH_CNT_MIN && endMonth <= L7_MONTH_CNT_MAX)
        )
    {
      if (stWeek != 0 && stDay != 0 && stMonth != 0 && endDay != 0 && endWeek != 0 && endMonth != 0)
      {
        return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To display the date in the format hr:mn:sec dd/mm/yyyy
* @param rawtime -time in seconds **argv
* @param buf - contains the date
* @author
*
* @cmddescript
*
* @end
*
*********************************************************************/
void cliGetUtcTimeInMonthsAndDays(L7_uint32 utcTime, L7_char8 *buf)
{
  struct tm date, *datep;
  L7_char8 mon_name[12][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0",
                              "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0",
                              "Nov\0", "Dec\0" };

  datep = &date;
  datep = localtime((time_t *)&utcTime);
  sprintf(buf, "%s %.2d %d %.2d:%.2d:%.2d", mon_name[datep->tm_mon], datep->tm_mday, datep->tm_year + 1900,
          datep->tm_hour, datep->tm_min, datep->tm_sec);
}

/*********************************************************************
* @purpose  To display the date in the format hr:mn:sec dd/mm/yyyy
* @param rawtime -time in seconds **argv
* @param buf - contains the date
* @author
*
* @cmddescript
*
* @end
*
*********************************************************************/
void cligettimeinMonthsAndDays(L7_uint32 rawTime, L7_char8 *buf)
{
  struct tm date, *datep;
  L7_char8 mon_name[12][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0",
                              "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0",
                              "Nov\0", "Dec\0" };
  L7_uint32 utcTime;

  osapiRawTimeToUTCTime(rawTime, &utcTime);

  datep = &date;
  datep = localtime((time_t *)&utcTime);
  sprintf(buf, "%s %.2d %d %.2d:%.2d:%.2d", mon_name[datep->tm_mon], datep->tm_mday, datep->tm_year + 1900,
          datep->tm_hour, datep->tm_min, datep->tm_sec);
}

void cliDisplayVlanFailureInfo(EwsContext ewsContext, cliFailureData_t *failureData)
{
  L7_uint32 indx, start = 0, end, count = 0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL flag = L7_FALSE;

  if (failureData->Count > 0)
  {
    ewsTelnetWrite(ewsContext, "\r\n          Failure Information");
    ewsTelnetWrite(ewsContext, "\r\n---------------------------------------");
    osapiSnprintf(buf, sizeof(buf),               "\r\n   VLANs failed to be configured : %d", failureData->Count);
    ewsTelnetWrite(ewsContext, buf);
    ewsTelnetWrite(ewsContext, "\r\n---------------------------------------");
    ewsTelnetWrite(ewsContext, "\r\n   VLAN             Error");
    ewsTelnetWrite(ewsContext, "\r\n---------------------------------------");

    for (indx = 0; indx < failureData->Count; indx++)
    {
      while ((indx < failureData->Count) &&
             (failureData->String[indx] == failureData->String[indx + 1]) &&
             (failureData->Interface[indx] == failureData->Interface[indx + 1] - 1))
      {
        if (flag == L7_FALSE)
        {
          start = failureData->Interface[indx];
          flag = L7_TRUE;
        }
        indx++;
      }
      if (flag == L7_TRUE)
      {
        end = failureData->Interface[indx];
        osapiSnprintf(buf, sizeof(buf), "\r\nVLAN %4d-%-4d ", start, end);
        flag = L7_FALSE;
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\nVLAN %9d ", failureData->Interface[indx]);
      }
      count++;
      if (count == 10)
      {
        ewsTelnetWrite(ewsContext, "\r\nToo many errors to show.");
        break;
      }
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWrite(ewsContext, failureData->String[indx]);
    }
  }
}

void cliDisplayFailureInfo(EwsContext ewsContext, cliFailureData_t *failureData)
{
  L7_uint32 indx, start = 0, end, count = 0;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 bufIntf[20];
  L7_uchar8 bufIntfStart[20];
  L7_uchar8 bufIntfEnd[20];
  L7_BOOL flag = L7_FALSE;

  if (failureData->Count > 0)
  {
    ewsTelnetWrite(ewsContext, "\r\n          Failure Information");
    ewsTelnetWrite(ewsContext, "\r\n---------------------------------------");
    osapiSnprintf(buf, sizeof(buf),               "\r\nInterfaces failed to be configured : %d", failureData->Count);
    ewsTelnetWrite(ewsContext, buf);
    ewsTelnetWrite(ewsContext, "\r\n---------------------------------------");
    ewsTelnetWrite(ewsContext, "\r\nInterface             Error");
    ewsTelnetWrite(ewsContext, "\r\n---------------------------------------");

    for (indx = 0; indx < failureData->Count; indx++)
    {
      while ((indx < failureData->Count) &&
             (failureData->String[indx] == failureData->String[indx + 1]) &&
             (failureData->Interface[indx] == failureData->Interface[indx + 1] - 1))
      {
        if (flag == L7_FALSE)
        {
          start = failureData->Interface[indx];
          flag = L7_TRUE;
        }
        indx++;
      }
      if (flag == L7_TRUE)
      {
        end = failureData->Interface[indx];
        cliIntIfNumToIntfString(end, bufIntfEnd);
        cliIntIfNumToIntfString(start, bufIntfStart);
        osapiSnprintf(buf, sizeof(buf), "\r\n%5s-%-7s", bufIntfStart, bufIntfEnd);
        flag = L7_FALSE;
      }
      else
      {
        cliIntIfNumToIntfString(failureData->Interface[indx], bufIntf);
        osapiSnprintf(buf, sizeof(buf), "\r\n%-14s", bufIntf);
      }
      count++;
      if (count == 10)
      {
        ewsTelnetWrite(ewsContext, "\r\nToo many errors to show.");
        break;
      }
      ewsTelnetWrite(ewsContext, buf);
      ewsTelnetWrite(ewsContext, failureData->String[indx]);
    }
  }
}

/*********************************************************************
*
* @purpose Verify proper input of interface range given in comma separated values
*          with no spaces and '-' designates consecutive input range.
*
* @param  *buf        @b{(input))   Text input
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
*
*********************************************************************/
/* Use hyphen to designate range */
/* Use comma with no spaces between non-consecutive IDs */
L7_RC_t cliValidateEthernetRangeInput(const L7_char8 *buf)
{
  L7_uint32 buf_len;
  L7_uchar8 tmp_buf[1000];
  L7_uchar8 tmp_buf_out[1000];
  char *str_ptr = L7_NULLPTR;
  char *ptr = L7_NULLPTR;

  L7_uint32 startUnitNum;
  L7_uint32 endUnitNum;

  L7_uint32 startSlotNum;
  L7_uint32 endSlotNum;

  L7_uint32 startPortNum;
  L7_uint32 endPortNum;

  L7_uint32 match;

  buf_len = (L7_uint32)strlen(buf);

  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  memset(tmp_buf, 0, sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);

  if (strcmp(tmp_buf, "") == 0)
  {
    return L7_FAILURE;
  }

  ptr = strtok_r(tmp_buf, ",", &(str_ptr));

  while (L7_NULLPTR != ptr)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      match = sscanf(ptr, "%u/%u/%u-%u/%u/%u", &startUnitNum, &startSlotNum, &startPortNum, &endUnitNum, &endSlotNum, &endPortNum);
  
      /* if only one interface is specified*/
      if (3 == match)
      {
        sprintf(tmp_buf_out, "%u/%u/%u", startUnitNum, startSlotNum, startPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }

        /* check that a trailing - is not present */
        if ('-' == ptr[strlen(ptr)-1])
        {
          return L7_ERROR;
        }
      }
      else if (6 == match)
      {
        sprintf(tmp_buf_out, "%u/%u/%u-%u/%u/%u", startUnitNum, startSlotNum, startPortNum, endUnitNum, endSlotNum, endPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }

        if ((0 == startUnitNum || 0 == endUnitNum) && ((startUnitNum != endUnitNum) || (startSlotNum != endSlotNum)))
        {
          return L7_ERROR;
        }
      }
      else
      {
        return L7_ERROR;
      }
    }
    else
    {
      match = sscanf(ptr, "%u/%u-%u/%u", &startSlotNum, &startPortNum, &endSlotNum, &endPortNum);
  
      /* if only one interface is specified*/
      if (2 == match)
      {
        sprintf(tmp_buf_out, "%u/%u", startSlotNum, startPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }

        /* check that a trailing - is not present */
        if ('-' == ptr[strlen(ptr)-1])
        {
          return L7_ERROR;
        }
      }
      else if (4 == match)
      {
        sprintf(tmp_buf_out, "%u/%u-%u/%u", startSlotNum, startPortNum, endSlotNum, endPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }
      }
      else
      {
        return L7_ERROR;
      }
    }
    ptr = strtok_r(NULL, ",", &str_ptr);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Parse the range input given in comma separated values
*          with no spaces and '-' designates consecutive input range.
*
* @param  *buf        @b{(input))   Text input
* @param  *count      @b{(output))  Count of interfaces found.
* @param  *list       @b{(output))  Array of internal interface numbers
* @param  *ewscontext @b{(input))   EmWeb context
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
/* Use hyphen to designate range */
/* Use comma with no spaces between non-consecutive IDs */
L7_RC_t cliParseEthernetRangeInput(const L7_char8 *buf, L7_INTF_MASK_t *ifMask,
                                   L7_uint32 *ifTypeMask)
{
  L7_uint32 buf_len, i;
  L7_uchar8 tmp_buf[1000];
  L7_uchar8 tmp_buf_out[1000];
  char *str_ptr = L7_NULLPTR;
  char *ptr = L7_NULLPTR;

  L7_uint32 startUnitNum;
  L7_uint32 endUnitNum;

  L7_uint32 startSlotNum;
  L7_uint32 endSlotNum;

  L7_uint32 startPortNum;
  L7_uint32 endPortNum;

  L7_uint32 startIntIfNum;
  L7_uint32 endIntIfNum;
  L7_uint32 tmpIntIfNum;

  L7_uint32 match;
  L7_uint32 retVal;

  L7_uint32 tempIfTypeMask = 0;

  L7_uint32 unit = cliGetUnitId();

  buf_len = (L7_uint32)strlen(buf);

  for (i = 0; i < buf_len; i++)
  {
    if (!(isdigit(buf[i]) || buf[i]=='-' || buf[i]==',' || buf[i]=='/' ))
    {
      return L7_FAILURE;
    }
  }

  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  memset(tmp_buf, 0, sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);

  if (strcmp(tmp_buf, "") == 0)
  {
    return L7_FAILURE;
  }

  ptr = strtok_r(tmp_buf, ",", &(str_ptr));

  memset(ifMask->value, 0, sizeof(ifMask->value));
  *ifTypeMask = 0;

  while (L7_NULLPTR != ptr)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      match = sscanf(ptr, "%u/%u/%u-%u/%u/%u", &startUnitNum, &startSlotNum, &startPortNum, &endUnitNum, &endSlotNum, &endPortNum);
  
      /* if only one interface is specified*/
      if (3 == match)
      {
        sprintf(tmp_buf_out, "%u/%u/%u", startUnitNum, startSlotNum, startPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }

        /* check that a trailing - is not present */
        if ('-' != ptr[strlen(ptr)-1] &&
            usmDbIntIfNumFromUSPGet(startUnitNum, startSlotNum, startPortNum, &startIntIfNum) == L7_SUCCESS &&
            usmDbVisibleInterfaceCheck(unit, startIntIfNum, &retVal) == L7_SUCCESS)
        {
          L7_INTF_SETMASKBIT(*ifMask, startIntIfNum);
          if (usmDbIntIfNumTypeMaskGet(unit, startIntIfNum, &tempIfTypeMask) == L7_SUCCESS)
          {
            *ifTypeMask |= tempIfTypeMask;
          }
        }
        else
        {
          return L7_ERROR;
        }
      }
      else if (6 == match)
      {
        sprintf(tmp_buf_out, "%u/%u/%u-%u/%u/%u", startUnitNum, startSlotNum, startPortNum, endUnitNum, endSlotNum, endPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }

        if ((0 == startUnitNum || 0 == endUnitNum) && ((startUnitNum != endUnitNum) || (startSlotNum != endSlotNum)))
        {
          return L7_ERROR;
        }
        if (usmDbIntIfNumFromUSPGet(startUnitNum, startSlotNum, startPortNum, &startIntIfNum) == L7_SUCCESS &&
            usmDbIntIfNumFromUSPGet(endUnitNum, endSlotNum, endPortNum, &endIntIfNum) == L7_SUCCESS)
        {
          if (startIntIfNum > endIntIfNum)
          {
            return L7_ERROR;
          }

          for (tmpIntIfNum = startIntIfNum; tmpIntIfNum <= endIntIfNum; tmpIntIfNum++)
          {
            if (usmDbVisibleInterfaceCheck(unit, tmpIntIfNum, &retVal) == L7_SUCCESS)
            {
              L7_INTF_SETMASKBIT(*ifMask, tmpIntIfNum);

              if (usmDbIntIfNumTypeMaskGet(unit, tmpIntIfNum, &tempIfTypeMask) == L7_SUCCESS)
              {
                *ifTypeMask |= tempIfTypeMask;
              }
            }
          }
        }
        else
        {
          return L7_ERROR;
        }
      }
      else
      {
        return L7_ERROR;
      }
    }
    else
    {
      match = sscanf(ptr, "%u/%u-%u/%u", &startSlotNum, &startPortNum, &endSlotNum, &endPortNum);
  
      /* if only one interface is specified*/
      if (2 == match)
      {
        sprintf(tmp_buf_out, "%u/%u", startSlotNum, startPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }
        /* check that a trailing - is not present */
        if ('-' != ptr[strlen(ptr)-1] &&
            usmDbIntIfNumFromUSPGet(unit, startSlotNum, startPortNum, &startIntIfNum) == L7_SUCCESS &&
            usmDbVisibleInterfaceCheck(unit, startIntIfNum, &retVal) == L7_SUCCESS)
        {
          L7_INTF_SETMASKBIT(*ifMask, startIntIfNum);
          if (usmDbIntIfNumTypeMaskGet(unit, startIntIfNum, &tempIfTypeMask) == L7_SUCCESS)
          {
            *ifTypeMask |= tempIfTypeMask;
          }
        }
        else
        {
          return L7_ERROR;
        }
      }
      else if (4 == match)
      {
        sprintf(tmp_buf_out, "%u/%u-%u/%u", startSlotNum, startPortNum, endSlotNum, endPortNum);
        if (strlen(tmp_buf_out) != strlen(ptr))
        {
          return L7_ERROR;
        }

        if (usmDbIntIfNumFromUSPGet(unit, startSlotNum, startPortNum, &startIntIfNum) == L7_SUCCESS &&
            usmDbIntIfNumFromUSPGet(unit, endSlotNum, endPortNum, &endIntIfNum) == L7_SUCCESS)
        {
          if (startIntIfNum > endIntIfNum)
          {
            return L7_ERROR;
          }

          for (tmpIntIfNum = startIntIfNum; tmpIntIfNum <= endIntIfNum; tmpIntIfNum++)
          {
            if (usmDbVisibleInterfaceCheck(unit, tmpIntIfNum, &retVal) == L7_SUCCESS)
            {
              L7_INTF_SETMASKBIT(*ifMask, tmpIntIfNum);
              if (usmDbIntIfNumTypeMaskGet(unit, tmpIntIfNum, &tempIfTypeMask) == L7_SUCCESS)
              {
                *ifTypeMask |= tempIfTypeMask;
              }
            }
          }
        }
        else
        {
          return L7_ERROR;
        }
      }
      else
      {
        return L7_ERROR;
      }
    }
    ptr = strtok_r(NULL, ",", &str_ptr);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Verify proper input of interface range given in comma separated values
*          with no spaces and '-' designates consecutive input range.
*
* @param  *buf        @b{(input))   Text input
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
*
*********************************************************************/
/* Use hyphen to designate range */
/* Use comma with no spaces between non-consecutive IDs */
void cliTestEthernetRangeInput(const L7_char8 *buf)
{
  L7_INTF_MASK_t ifMask;
  L7_uint32 interface, unit, s, p;
  L7_uint32 ifTypeMask = 0;

  memset(&ifMask, 0, sizeof(ifMask));
  if (cliParseEthernetRangeInput(buf, &ifMask, &ifTypeMask) != L7_SUCCESS)
  {
    sysapiPrintf("cliParseEthernetRangeInput = FAILURE\n");
  }
  else 
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (L7_INTF_ISMASKBITSET(ifMask, interface))
      {
        if(usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        sysapiPrintf("%d: %d/%d/%d\n", interface, unit, s, p);
      }
    }
  }
}

/*********************************************************************
*
* @purpose   This function converts an intIfNum to an interface string.
*            The string format is:
*            <unit>/<slot>/<port_num> if stacking is supported
*            <slot>/<port_num>        if stacking is not supported
*
* @returntype L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliIntIfNumToIntfString(L7_uint32 intIfNum, L7_uchar8 *intfString)
{
  L7_RC_t rc;
  L7_uint32 unit, slot, port;

  if (intfString == L7_NULL)
  {
    return L7_ERROR;
  }

  intfString[0] = 0;

  /* convert intIfNum to USP */
  rc = usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

#ifdef L7_STACKING_PACKAGE
  sprintf(intfString, "%d/%d/%d", unit, slot, port);
#else 
  sprintf(intfString, "%d/%d", slot, port);
#endif

  return L7_SUCCESS;
}
/**************************************************************************
* Dummy handler 
***************************************************************************/
const L7_char8 * commandDummy (EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 **argv, L7_uint32 index)
{
  cliSyntaxBottom (ewsContext);
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose For use when the output for the CLI is greater than
*          L7_CLI_MAX_STRING_LENGTH.  This will put up to 80 chars in
*          to the stringOutput array and can be called within a loop to
*          get the next line of the output buffer.
*
*
* @param void
*
* @returntype L7_char8 *
* @returns
*
* @notes "\n" makes EmWeb code think that an enter was sent after we capture the string
* @notes look where it is already used for examples on how to use it -
* @notes used in show traplog, show protocol summary, show protocol detailed
*
* @end
*
********************************************************************/
L7_char8 *cliProcessStringSeparate(L7_char8 *outputLine, L7_uint32 outputLength)
{
  L7_uint32 charnum;
  L7_uint32 x, bufLength;   /* bufLength is outputLength */
  static L7_uint32 length = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 tmpString[L7_CLI_MAX_STRING_LENGTH];

  memset(tmpString, 0, sizeof(tmpString));
  charnum = strlen(outputLine);
  memset(buf, 0, sizeof(buf));
  bufLength = outputLength;

  if (charnum < outputLength)                 /* the string will fit in the alotted space in the CLI */
  {
    return outputLine;
  }

  if (length < charnum)
  {
    for (x = 0; x <= outputLength; x++)
    {
      if (outputLine[length] != '\0')         /* the end of the string has not been reached */
      {
        if ((x != 0) || (outputLine[length] != ' '))    /* if the first character in the line is a space, skip it */
        {
          buf[x] = outputLine[length];
          length ++;
        }
        else
        {
          buf[x] = outputLine[length + 1];
          length += 2;
        }
      }
      else
      {
        length ++;
        break;
      }
    }
    if (length < charnum)
    {
      while ((buf[bufLength] != ' ') && (buf[bufLength] != ','))
      {
        bufLength --;
        length --;
      }
    }
  }
  else
  {
    length = 0;
    return outputLine;
  }
  strncpy(tmpString, buf, bufLength + 1);  /* bufLength + 1 inorder to capture the comma. */
  return tmpString;
}


/*********************************************************************
*
* @purpose Check whether the login user is allowed without Login Name 
*
* @returns  L7_TRUE if the user is allowed the access
*           L7_FALSE if the user is not allowed the access
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL cliAccessLoginUserIsAllow(L7_uint32 accessLineType)
{
  L7_char8 aplName[L7_MAX_APL_NAME_SIZE + 1]={0};
  L7_USER_MGR_AUTH_METHOD_t method=0;
  L7_BOOL allowUserFlg = L7_FALSE;
  
  if(accessLineType == ACCESS_LINE_SSH || accessLineType == ACCESS_LINE_TELNET)
  {
    /* Get the APL for this line */
    (void)usmDbAPLLineGet(accessLineType, ACCESS_LEVEL_LOGIN, aplName);

    (void)usmDbAPLAuthMethodGet(accessLineType, ACCESS_LEVEL_LOGIN, aplName, 0, &method);

   /*  
    * Allow the users dependent on RADIUS, TACACS, LINE and ENABLE
    * Login Index Get fails as the users for RADIUS and TACACS authentication 
    * are not exists on Local Database, LINE and ENABLE are not dependent on 
    * Login names it only deals with Passwords.
    * 
    */
    if((method == L7_AUTH_METHOD_TACACS) || (method == L7_AUTH_METHOD_RADIUS) || 
       (method == L7_AUTH_METHOD_LINE) || (method == L7_AUTH_METHOD_ENABLE))
    {
      allowUserFlg = L7_TRUE;
    }    
  }
  return allowUserFlg;
}

