/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_usermgr.c
 *
 * @purpose User Manager show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/18/2003
 *
 * @author  chinmoyb
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "user_manager_exports.h"
#include "osapi.h"
#include "snmp_exports.h"
#include "usmdb_util_api.h"
#include "cliapi.h"

#include "datatypes.h"
#include "default_cnfgr.h"

#include "clicommands_usermgr.h"
#include "usmdb_user_mgmt_api.h"
#include "cli_web_user_mgmt.h"
#include "osapi_support.h"

/*********************************************************************
*
* @purpose   Shows User Manager Authentication Login Detailed
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
* @cmdsyntax  show authentication methods
*
*
* @cmdhelp
*
* @cmddescript  Displays authentication methods for all the
* authentication lists.
*
* @end
*
*********************************************************************/
const L7_char8  *commandShowAuthenticationMethods(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 **argv,
                                           uintf index)
{
  /* String to contain the list name */
  L7_char8  strAuthListName[L7_MAX_APL_NAME_SIZE + 1];

  /* String to contain the prev list name */
  L7_char8  strAuthListNamePrev[L7_MAX_APL_NAME_SIZE + 1];

  /* variable authMode to keep track of the mode */
  L7_ACCESS_LEVEL_t authMode = 0;

  /* variable accessMode to keep track of access mode */
  L7_ACCESS_LINE_t accessMode = 0;

  /* loop variable */
  L7_uint32 i =0;

  /*The string variable to hold all the methods pertaining to a list */
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /* The methods used in the authentication */
  L7_USER_MGR_AUTH_METHOD_t method;

  /* The stack unit */
  L7_uint32 unit = 0;

  /* Return code from usmDb functions */
  L7_uint32 usmDbReturnCode = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* Get the arguments passed to the command */
  if ( cliNumFunctionArgsGet() != 0 )
  {
    ewsTelnetWrite(ewsContext, CLISYNTAX_SHOWAUTHENTICATIONLOGININFO);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /* Loop for authMode, namely Login and Enable mode */
  for (authMode = ACCESS_LEVEL_LOGIN; authMode <= ACCESS_LEVEL_ENABLE; authMode++)
  {
    /* Display according to the authMode */
    if (authMode == ACCESS_LEVEL_LOGIN)
    {
      ewsTelnetWrite(ewsContext,"\r\nLogin Authentication Method Lists");
      ewsTelnetWrite(ewsContext,"\r\n---------------------------------");
    }

    /* Display according to the authMode */
    else if (authMode ==  ACCESS_LEVEL_ENABLE)
    {
      ewsTelnetWrite(ewsContext,"\r\n\r\nEnable Authentication Method Lists");
      ewsTelnetWrite(ewsContext,    "\r\n----------------------------------");
    }

    /* Else its an error, though not feasable */
    else
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_AUTHENTICATIONMODEGETERROR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* Resetting order of the methods */
    i = 0;

    /* Clearing both the list names buffer */
    bzero(strAuthListName, sizeof(strAuthListName));
    bzero(strAuthListNamePrev, sizeof(strAuthListNamePrev));

    /* Get the first list name for the corresponding Mode
     * If return code is  a failure, return with the proper error code
     * When there are no lists corresponding, its not an error
     */

    if ((usmDbReturnCode = usmDbAPLListGetFirst(ACCESS_LINE_CTS,
                                                authMode,
                                                strAuthListName))==
        L7_FAILURE)
    {
      ewsTelnetWrite(ewsContext,
                     CLIERROR_SHOWAUTHENTICATIONLISTGETERROR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* The returnCode at this instant is L7_SUCCESS or
     * some other error code.
     */
    /* If the return code is not L7_SUCCESS, do nothing */
    if (usmDbReturnCode == L7_SUCCESS)
    {
      /* do while loop to print the ListName along with the
       * corresponding methods
       */
      do
      {
        /* Initialising the stat buffer to zero */
        bzero(stat,sizeof(stat));
        sprintf(stat, "\r\n%-20s:  ",strAuthListName);

        /* for loop for every methods possible */
        for (i = 0; i < L7_MAX_AUTH_METHODS; i++)
        {
          /* Check for the return code for getting the methods from the
           * list.
           */
          usmDbReturnCode = usmDbAPLAuthMethodGet(ACCESS_LINE_CTS,
                                                  authMode,
                                                  strAuthListName,
                                                  i,
                                                  &method);
          if (usmDbReturnCode == L7_SUCCESS)
          {
            /* Switch loop based on method */
            switch (method)
            {
              case L7_AUTH_METHOD_LOCAL:
                osapiSnprintf(buf,sizeof(buf),"%-9s", CLILOCAL_AUTH);
                osapiStrncat(stat, buf, sizeof(stat)-strlen(stat));
                break;

              case L7_AUTH_METHOD_ENABLE:
                osapiSnprintf(buf,sizeof(buf),"%-9s", CLIENABLE_AUTH);
                osapiStrncat(stat, buf, sizeof(stat)-strlen(stat));
                break;

              case L7_AUTH_METHOD_RADIUS:
                osapiSnprintf(buf,sizeof(buf),"%-9s", CLIRADIUS_AUTH);
                osapiStrncat(stat, buf, sizeof(stat)-strlen(stat));
                break;

              case L7_AUTH_METHOD_TACACS:
                osapiSnprintf(buf,sizeof(buf),"%-9s", CLITACACS_AUTH);
                osapiStrncat(stat, buf, sizeof(stat)-strlen(stat));
                break;

              case L7_AUTH_METHOD_NONE:
                osapiSnprintf(buf,sizeof(buf),"%-9s", CLINONE_AUTH);
                osapiStrncat(stat, buf, sizeof(stat)-strlen(stat));
                break;

              case L7_AUTH_METHOD_LINE:
                osapiSnprintf(buf,sizeof(buf),"%-9s", CLILINE_AUTH);
                osapiStrncat(stat, buf, sizeof(stat)-strlen(stat));
                break;

              default:
                break;
            }
          }/*End of if ((usmDbReturnCode = usmDbAPLAuthMethodGet(unit,*/

        }/* End of for (i=0; */
        ewsTelnetWrite(ewsContext,stat);
        /* Copy the strAuthListName to strAuthListNamePrev as it'll be
         * required in usmDbAPLTempListGetNext function to get the next
         * list name.
         */
        strcpy(strAuthListNamePrev, strAuthListName);
        bzero(strAuthListName, sizeof(strAuthListName));
      } while (usmDbAPLListGetNext(ACCESS_LINE_CTS,
                                   authMode,
                                   strAuthListNamePrev,
                                   strAuthListName) == L7_SUCCESS);
    }/* End of if (usmDbReturnCode == L7_SUCCESS....*/
  }/* End of for (authMode =0; authMode < ACCESS_LEVEL_HTTPS;.. */

   /* Clear the contents of strAuthListName & strAuthListNamePrev to be used in
    * Line Console mode
    */
  bzero(strAuthListName, sizeof(strAuthListName));
  bzero(strAuthListNamePrev, sizeof(strAuthListNamePrev));

  /* Setting authMode to Login  mode */
  authMode = ACCESS_LEVEL_LOGIN;

  /* For printing the Line configuration mode */
  ewsTelnetWrite(ewsContext,"\r\n\r\nLine     Login Method List    Enable Method List");
  ewsTelnetWrite(ewsContext,    "\r\n-------  -----------------    ------------------");

  /* Get the list name for the different line modes, namely for Console,
   * Telnet and SSH
   */
  for (accessMode = ACCESS_LINE_CONSOLE; accessMode <= ACCESS_LINE_SSH; accessMode++)
  {
    /* Clear the stat buffer */
    bzero(stat,sizeof(stat));

    /* print Console, Telnet or SSH */
    if (accessMode == ACCESS_LINE_CONSOLE)
      sprintf(stat,"\r\n%-9s", "Console");

    else if (accessMode ==ACCESS_LINE_TELNET )
      sprintf(stat,"\r\n%-9s", "Telnet");

    else if (accessMode ==ACCESS_LINE_SSH )
      sprintf(stat,"\r\n%-9s", "SSH");

    /* for every auth method, namely Login and Enable */
    for (authMode = ACCESS_LEVEL_LOGIN; authMode <= ACCESS_LEVEL_ENABLE;
         authMode++)
    {
      /* Get the first list name for the Console Mode
       * If the return code is some failure, return with the proper error
       * code.When there are no lists corresponding, its not an error
       */
      bzero(strAuthListName, sizeof(strAuthListName));
      if ((usmDbReturnCode = usmDbAPLLineGet(accessMode,
                                             authMode,
                                             strAuthListName)) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext,
                       CLIERROR_SHOWAUTHENTICATIONLINELISTGETERROR);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      /*The returnCode at this instant can be L7_SUCCESS
       * If there are no lists in Login Mode, do nothing
       */
      osapiSnprintf(buf, sizeof(buf), "%-21s", strAuthListName);
      osapiStrncat(stat, buf, sizeof(stat)-strlen(stat));
    }/* for (authMode = ACCESS_LEVEL_LOGIN; authMode <= ACCESS_LEVEL_EN...*/
    ewsTelnetWrite(ewsContext,stat);
  }/* End of for (accessMode = 0; accessMode < ACCESS_LINE_NUM;...*/

   /* Clear the contents of strAuthListName & strAuthListNamePrev to be used
    * in various protocols case.
    */
  bzero(strAuthListName, sizeof(strAuthListName));
  bzero(strAuthListNamePrev, sizeof(strAuthListNamePrev));

  /* setting authMode to L7_AUTH_HTTPS_MODE */
  authMode = ACCESS_LEVEL_UNKNOWN;

  /* Resetting usmDbReturnCode */
  usmDbReturnCode = L7_FAILURE;

  cliSyntaxTop(ewsContext);
  /* Display for HTTPS, HTTP and Dot1x */
  for (accessMode = ACCESS_LINE_HTTPS; accessMode <= ACCESS_LINE_DOT1X; accessMode++)
  {
    /* Clear the contents of stat */
    bzero(stat,sizeof(stat));

    /* Clear the contents of strAuthListName */
    bzero(strAuthListName,sizeof(strAuthListName));

    if (accessMode == ACCESS_LINE_HTTP)
      strcpy(strAuthListName, CLI_HTTPLISTNAME_AUTH);
    else if (accessMode == ACCESS_LINE_HTTPS)
      strcpy(strAuthListName, CLI_HTTPSLISTNAME_AUTH);
    else/* must be L7_CLI_TWO */
      strcpy(strAuthListName, CLI_DOT1XLISTNAME_AUTH);

    if (accessMode==ACCESS_LINE_HTTP)
    {
      sprintf(stat, "\r\n%-12s:", "HTTP");
      ewsTelnetWrite(ewsContext,stat);
    }

    else if (accessMode==ACCESS_LINE_HTTPS)
    {
      sprintf(stat, "\r\n%-12s:", "HTTPS");
      ewsTelnetWrite(ewsContext,stat);
    }

    else
    {
      sprintf(stat, "\r\n%-12s:", "DOT1X");
      ewsTelnetWrite(ewsContext,stat);
    }

    /* Initialising the stat buffer to zero */
    bzero(stat,sizeof(stat));

    /* for loop for every methods possible */
    for (i = 0; i < L7_MAX_AUTH_METHODS; i++)
    {
      /* Check for the return code for getting the methods from the list.
       * Till, all the methods have been returned, the loop is continued
       */
      usmDbReturnCode = usmDbAPLAuthMethodGet(accessMode,
                                              authMode,
                                              strAuthListName,
                                              i,
                                              &method);
      if (usmDbReturnCode == L7_SUCCESS)
      {
        bzero(stat,sizeof(stat));
        /* Switch loop based on method */
        switch (method)
        {
          case L7_AUTH_METHOD_LOCAL:
            sprintf(stat,"%-9s ",CLILOCAL_AUTH);
            break;

          case L7_AUTH_METHOD_RADIUS:
            sprintf(stat,"%-9s ",CLIRADIUS_AUTH);
            break;

          case L7_AUTH_METHOD_ENABLE:
            sprintf(stat,"%-9s ",CLIENABLE_AUTH);
            break;

          case L7_AUTH_METHOD_TACACS:
            sprintf(stat,"%-9s ",CLITACACS_AUTH);
            break;

          case L7_AUTH_METHOD_NONE:
            sprintf(stat,"%-9s ",CLINONE_AUTH);
            break;

          case L7_AUTH_METHOD_LINE:
            sprintf(stat,"%-9s ",CLILINE_AUTH);
            break;

          default:
            /*sprintf(stat,"%-10s  ",CLIUNDEFINED_AUTH);*/
            break;
        }
        ewsTelnetWrite(ewsContext,stat);
      }/* End of if ((usmDbReturnCode = usmDbAuthenticationListAuth */
    }
    authMode++;
  }/* End of for (lineIndex = 0;lineIndex < L7_LINE_MODE;...*/

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose   Shows User Manager Authentication Login Users
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
* @cmdsyntax  show authentication users <listname>
*
*
* @cmdhelp
*
* @cmddescript  Displays information about users assigned to specific authentication login list.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowAuthenticationUsers(EwsContext ewsContext,
                                               uintf argc,
                                               const L7_char8 * * argv,
                                               uintf index)
{
  L7_uint32 argList = 1;
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8 strUserName[L7_LOGIN_SIZE];
  L7_char8 strUserNamePrev[L7_LOGIN_SIZE];
  L7_uint32 pComponent;
  L7_uint32 pComponentPrev;
  L7_uint32 unit;
  L7_uint32 i;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  if ( cliNumFunctionArgsGet() != 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowAuthLoginUsrs);
  }

  if (strlen(argv[index+argList]) >= sizeof(strAuthListName))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidLoginListLen, L7_MAX_APL_NAME_SIZE);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  OSAPI_STRNCPY_SAFE(strAuthListName,argv[index+argList]);
  if ((cliIsAlphaNum(strAuthListName) != L7_SUCCESS) || 
      (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, strAuthListName, &i) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
  }

  memset (strUserName, 0, sizeof(strUserName));
  if ( usmDbAPLAllUsersGetFirst(unit, strAuthListName, strUserName, &pComponent) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_ShowAuthListNoUsrError);
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameComp);
  ewsTelnetWrite( ewsContext, "\r\n----------  ----------------  ");

  do
  {
    ewsTelnetPrintf (ewsContext, "\r\n%-13s  ",strUserName);

    memset (stat, 0,sizeof(stat));
    if (pComponent == L7_USER_MGR_COMPONENT_ID)
    {
      osapiSnprintf(stat,sizeof(stat),"%-10s  ",pStrInfo_base_SysLogin);
    }
    else if (pComponent == L7_DOT1X_COMPONENT_ID)
    {
      osapiSnprintf(stat,sizeof(stat),"%-10s  ",pStrInfo_base_Dot1x);
    }
    ewsTelnetWrite(ewsContext,stat);

    OSAPI_STRNCPY_SAFE(strUserNamePrev, strUserName);
    pComponentPrev = pComponent;
    memset (strUserName, 0, sizeof(strUserName));
  } while ( usmDbAPLAllUsersGetNext(unit, strAuthListName, strUserNamePrev, pComponentPrev, strUserName, &pComponent) == L7_SUCCESS);

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Shows User Manager Login Users
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
* @cmdsyntax  show users authentication
*
*
* @cmdhelp
*
* @cmddescript  Displays all user and all authentication login information.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowUsersAuthentication(EwsContext ewsContext,
                                               uintf argc,
                                               const L7_char8 * * argv,
                                               uintf index)
{
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8 strAuthListNameDot1x[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8 strUserName[L7_LOGIN_SIZE];
  L7_uint32 unit;
  L7_uint32 userCount;

  cliSyntaxTop(ewsContext);

  if ( cliNumFunctionArgsGet() != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowUsrsAuth);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  ewsTelnetWriteAddBlanks (1, 0, 17, 7, L7_NULLPTR,  ewsContext,pStrInfo_base_AuthLoginLists);
  ewsTelnetWrite( ewsContext,"\r\n                                                  ");
  ewsTelnetWriteAddBlanks (1, 0, 3, 7, L7_NULLPTR,  ewsContext,pStrInfo_base_UsrSysLogin8021x);
  ewsTelnetWrite( ewsContext,"\r\n----------     ----------------    -------------  ");

  for (userCount = 0; userCount < L7_MAX_LOGINS; userCount++ )
  {
    memset (strUserName, 0, sizeof(strUserName));
    if (cliWebLoginUserNameGet(userCount, strUserName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ShowAuthNoUsrError);
    }

    /* skip user if empty.  The name list will contain blanks if a user is deleted.  */
    if (strcmp(strUserName, pStrInfo_common_EmptyString) == 0)
    {
      continue;
    }

    memset (strAuthListName, 0, sizeof(strAuthListName));
    if (usmDbAPLUserGet(unit, strUserName, L7_USER_MGR_COMPONENT_ID, strAuthListName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_ShowAuthSysListGetError);
    }

    memset (strAuthListNameDot1x, 0, sizeof(strAuthListNameDot1x));
    if (usmDbAPLUserGet(unit, strUserName, L7_DOT1X_COMPONENT_ID, strAuthListNameDot1x) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_ShowAuth802Dot1XListGetError);
    }

    ewsTelnetPrintf (ewsContext, "\r\n%-17s%-18s%-12s",strUserName, strAuthListName, strAuthListNameDot1x);
  }

  /* Get the settings for the default user */
  OSAPI_STRNCPY_SAFE(strUserName, L7_USER_MGR_DEFAULT_USER_STRING);
  if ( usmDbAPLNonConfiguredUserGet(unit, L7_USER_MGR_COMPONENT_ID, strAuthListName) != L7_SUCCESS ||
      usmDbAPLNonConfiguredUserGet(unit, L7_DOT1X_COMPONENT_ID, strAuthListNameDot1x) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_ShowAuth802Dot1XListGetError);
  }

  return cliSyntaxReturnPrompt (ewsContext,"\r\n%-17s%-18s%-12s",strUserName, strAuthListName, strAuthListNameDot1x);
}

/*********************************************************************
*
* @purpose   show passwords managemet informarion
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
* @cmdsyntax  show passwords configuration
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowPasswordsConfiguration(EwsContext ewsContext,
                                                  uintf argc,
                                                  const L7_char8 * * argv,
                                                  uintf index)
{
  L7_ushort16 val;

  cliSyntaxTop(ewsContext);

  /* print headings */
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_base_PasswdsCfg);
  ewsTelnetWrite(ewsContext, "\r\n-----------------------\r\n");

  /* display information about password length */
  cliFormat(ewsContext, pStrInfo_base_MinPasswdLen);
  if (usmDbMinPassLengthGet(&val) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, "%d", val);
  }

  /* display information about password history */
  cliFormat(ewsContext, pStrInfo_base_PasswdHistory);
  if (usmDbHistoryLengthGet(&val) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, "%d", val);
  }

  /* display information about password aging */
  cliFormat(ewsContext, pStrInfo_base_PasswdAgingDays);
  if (usmDbPasswdAgingValueGet(&val) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, "%d", val);
  }

  /* display information about password userlockout */
  cliFormat(ewsContext, pStrInfo_base_LockoutAttempts);
  if (usmDbLockoutAttemptsGet(&val) == L7_SUCCESS)
  {
    ewsTelnetPrintf (ewsContext, "%d", val);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}/* end of the function */

/*********************************************************************
*
* @purpose   Shows User Manager user accounts
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
* @cmdsyntax  show users accounts
*
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowUsersAccounts(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{
  L7_char8  strAccessLevel[L7_CLI_STRING_LENGTH + 1];
  L7_char8  strPasswordAging[L7_CLI_STRING_LENGTH + 1];
  L7_char8  strPasswordExpiry[L7_CLI_MAX_STRING_LENGTH + 1];
  L7_char8  strLockout[L7_CLI_MAX_STRING_LENGTH + 1];
  L7_char8  strUserName[L7_LOGIN_SIZE];
  L7_uint32 unit;
  L7_uint32 userCount;
  L7_uint32 accessLevel;
  L7_uint32 expireTime;
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL val;
  L7_ushort16 val1;

  /**** Error Checking for command Type*****/
  CLI_COMMANDTYPE_NORMAL;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);
  if ( cliNumFunctionArgsGet() != 0 )
  {
    ewsTelnetWrite(ewsContext, CLISYNTAX_SHOWUSERSAUTHENTICATION);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  ewsTelnetWrite( ewsContext,"\r\n UserName             Privilege   Password         Password         Lockout");
  ewsTelnetWrite( ewsContext,"\r\n                                   Aging          Expiry date              ");
  ewsTelnetWrite( ewsContext,"\r\n--------------------  ---------  ----------  ---------------------  -------");

  for (userCount = 0; userCount < L7_MAX_LOGINS; userCount++ )
  {
    bzero(strUserName, sizeof(strUserName));
    if (cliWebLoginUserNameGet(userCount, strUserName) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, CLIERROR_SHOWAUTHENTICATIONNOUSERERROR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* skip user if empty.  The name list will contain blanks if a user is deleted.  */
    if (strcmp(strUserName, CLIEMPTYSTRING) == 0)
    {
      continue;
    }
    bzero(strAccessLevel, sizeof(strAccessLevel));
    if (usmDbUserAccessLevelGet(unit, userCount, &accessLevel) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLIERROR_SHOWAUTHENTICATIONSYSLISTGETRERROR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    else
    {
      osapiSnprintf(strAccessLevel,sizeof(strAccessLevel),"%d",accessLevel);
    }
    bzero(strPasswordAging, sizeof(strPasswordAging));
    bzero(strPasswordExpiry, sizeof(strPasswordExpiry));
    if (usmDbPasswdAgingStatusGet() == L7_SUCCESS)
    {
      if (usmDbPasswdAgingValueGet(&val1) == L7_SUCCESS)
      {
        osapiSnprintf(strPasswordAging, sizeof(strPasswordAging), "%d", val1);
      }
      else
      {
        osapiSnprintf(strPasswordAging, sizeof(strPasswordAging), "---");
      }
      if (usmDbPasswordExpireTimeGet(unit, userCount, &expireTime) == L7_SUCCESS)
      {
        /* convert integer to time string */
        cligettimeinMonthsAndDays(expireTime, strPasswordExpiry);
      }
      else
      {
        osapiSnprintf(strPasswordExpiry, sizeof(strPasswordExpiry), "%s", "---");
      }
    }
    else
    {
      osapiSnprintf(strPasswordAging, sizeof(strPasswordAging), "%s", "---");
      osapiSnprintf(strPasswordExpiry, sizeof(strPasswordExpiry), "%s", "---");
    }
    bzero(strLockout, sizeof(strLockout));
    if (usmDbUserLockStatusGet(unit, userCount, &val) == L7_SUCCESS)
    {
      if (val == L7_TRUE)
      {
        osapiSnprintf(strLockout, sizeof(strLockout), "%s", "True");
      }
      else
      {
        osapiSnprintf(strLockout, sizeof(strLockout), "%s", "False");
      }
    }
    else
    {
      osapiSnprintf(strLockout, sizeof(strLockout), "---");
    }

    bzero(stat,sizeof(stat));
    osapiSnprintf(stat, sizeof(stat), "\r\n%-22.20s%-11s%-12s%-23s%-12s",
                  strUserName, strAccessLevel, strPasswordAging, strPasswordExpiry, strLockout);
    ewsTelnetWrite(ewsContext,stat);
  }

  cliSyntaxBottom(ewsContext);
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Shows User Manager Login history
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
* @cmdsyntax  show users login-history [<username>]
*
*
* @cmdhelp
*
* @cmddescript  Displays all user login history
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandShowUsersLoginHistory(EwsContext ewsContext,
                                              uintf argc,
                                              const L7_char8 **argv,
                                              uintf index)
{
  L7_RC_t   rc=L7_FAILURE;
  L7_char8  strLogintime[L7_CLI_MAX_STRING_LENGTH + 1];
  L7_char8  strProtocol[L7_CLI_MAX_STRING_LENGTH + 1];
  L7_char8  strLocation[L7_CLI_MAX_STRING_LENGTH + 1];
  L7_char8  strUserName[L7_LOGIN_SIZE];
  L7_char8  strUserNameInput[L7_LOGIN_SIZE];
  L7_uint32 unit, numArgs;
  L7_inet_addr_t location;
  L7_char8 checkChar;
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 historyIndex = 0;
  L7_uint32 time;
  L7_uint32 count = 0;
  L7_LOGIN_TYPE_t protocol;
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];

  /**** Error Checking for command Type*****/
  CLI_COMMANDTYPE_NORMAL;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);


  numArgs = cliNumFunctionArgsGet();
  if ( numArgs > 1 )
  {
    ewsTelnetWrite(ewsContext, CLISYNTAX_SHOWUSERSAUTHENTICATION);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (numArgs == 1)
  {
    if (strlen(argv[index+1]) >= sizeof(strUserNameInput))
    {
      ewsTelnetWrite( ewsContext, CLIERROR_CONFIGAUTHENTICATIONLOGININVALIDUSERNAME);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bzero(strUserNameInput, sizeof(strUserNameInput));
    osapiStrncpy(strUserNameInput, argv[index+1], sizeof(strUserNameInput));
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY )
  {                                                /* if our question has been answered */
    checkChar = tolower(cliGetCharInput());
    if (( checkChar == 'q' ) ||
        ( checkChar == 26 )) /*ctrl-z*/
    {
      historyIndex = 0;
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  ewsTelnetWrite( ewsContext,"\r\nLogin Time             Username              Protocol   Location       ");
  ewsTelnetWrite( ewsContext,"\r\n---------------------  --------------------  ---------  ---------------");
  count = count + 2;

  if (usmdbUserMgrLoginHistoryGetFirst(unit,&historyIndex,strUserName,&protocol,&location,&time) == L7_SUCCESS)
    do
    {

      if ((numArgs == 0) ||
          (numArgs == 1 && usmDbStringCaseInsensitiveCompare(strUserName, strUserNameInput) == L7_SUCCESS))
      {
        bzero(strLogintime, sizeof(strLogintime));
        cliGetUtcTimeInMonthsAndDays(time*SECONDS_PER_MINUTE, strLogintime);

        bzero(strProtocol, sizeof(strProtocol));
        switch (protocol)
        {
          case L7_LOGIN_TYPE_SERIAL:
            osapiSnprintf(strProtocol, sizeof(strProtocol), "Serial");
            break;
          case L7_LOGIN_TYPE_TELNET:
            osapiSnprintf(strProtocol, sizeof(strProtocol), "Telnet");
            break;
          case L7_LOGIN_TYPE_SSH:
            osapiSnprintf(strProtocol, sizeof(strProtocol), "SSH");
            break;
          case L7_LOGIN_TYPE_HTTP:
            osapiSnprintf(strProtocol, sizeof(strProtocol), "HTTP");
            break;
          case L7_LOGIN_TYPE_HTTPS:
            osapiSnprintf(strProtocol, sizeof(strProtocol), "HTTPS");
            break;
          case L7_LOGIN_TYPE_SNMP:
            osapiSnprintf(strProtocol, sizeof(strProtocol), "SNMPv3");
            break;
          default:
            break;
        }
        bzero(strLocation, sizeof(strLocation));
        if (location.family == L7_AF_INET)
        {
          usmDbInetNtoa(osapiNtohl(location.addr.ipv4.s_addr), strLocation);
        }
        else if (location.family == L7_AF_INET6)
        {
          osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&location.addr.ipv6, strLocation, L7_CLI_MAX_STRING_LENGTH);
        }

        bzero(stat,sizeof(stat));
        osapiSnprintf(stat, sizeof(stat), "\r\n%-23s%-22.20s%-11s%s",strLogintime, 
                      strUserName, strProtocol, strLocation);
        ewsTelnetWrite(ewsContext,stat);
        count++;

        if (count >= CLI_MAX_SCROLL_LINES - 6)
        {
          break;
        }
      }
      rc = usmdbUserMgrLoginHistoryGetNext(unit, historyIndex, &historyIndex, 
                                           strUserName, &protocol, &location, &time);

    }while (rc == L7_SUCCESS);
  if (rc == L7_SUCCESS)
  {
    if (historyIndex == 0)
    {
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
    cliSyntaxBottom(ewsContext);
    if (numArgs == 1)
    {
      osapiSnprintf(cmdBuf, sizeof(cmdBuf), "show users login-history ");
      strcat(cmdBuf, argv[index+1]);
      cliAlternateCommandSet(cmdBuf);
    }
    else
    {
      cliAlternateCommandSet("show users login-history");
    }
    return CLINAME;
  }
  else
  {
    historyIndex = 0;
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
}

/*********************************************************************
*
* @purpose   Shows User login statistics for failed logins
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
* @notes    This function is only available as a l7support command
*
* @cmdsyntax  show users login-stats
*
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowUsersLoginStats(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 * * argv,
                                           uintf index)
{
  L7_char8 strTotalLockouts[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strCurrentFailures[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strTotalFailures[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strPasswordExpiry[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strLockout[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strUserName[L7_LOGIN_SIZE];
  L7_uint32 unit;
  L7_uint32 userCount;
  L7_uint32 val;
  L7_uint32 expireTime;
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  ewsTelnetWriteAddBlanks (1, 0, 1, 7, L7_NULLPTR,  ewsContext,pStrInfo_base_UsrNameCurrentTotalTotalLockoutPasswd);
  ewsTelnetWriteAddBlanks (1, 0, 21, 5, L7_NULLPTR,  ewsContext,pStrInfo_base_FailuresFailuresLockoutsStatusExpiryDate);
  ewsTelnetWrite( ewsContext,"\r\n-------------------- -------- -------- -------- ------- ---------------------");

  for (userCount = 0; userCount < L7_MAX_LOGINS; userCount++ )
  {
    memset (strUserName, 0, sizeof(strUserName));
    if (cliWebLoginUserNameGet(userCount, strUserName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ShowAuthNoUsrError);
    }

    /* skip user if empty.  The name list will contain blanks if a user is deleted.  */
    if (strcmp(strUserName, pStrInfo_common_EmptyString) == 0)
    {
      continue;
    }

    memset(strTotalLockouts, 0, sizeof(strTotalLockouts));
    if (usmDbUserTotalLockoutCountGet(userCount, &val) == L7_SUCCESS)
    {
      osapiSnprintf(strTotalLockouts, sizeof(strTotalLockouts), "%d", val);
    }

    memset(strCurrentFailures, 0, sizeof(strCurrentFailures));
    if (usmDbUserCurrentLockoutCountGet(userCount, &val) == L7_SUCCESS)
    {
      osapiSnprintf(strCurrentFailures, sizeof(strCurrentFailures), "%d", val);
    }

    memset(strTotalFailures, 0, sizeof(strTotalFailures));
    if (usmDbUserTotalLoginFailuresGet(userCount, &val) == L7_SUCCESS)
    {
      osapiSnprintf(strTotalFailures, sizeof(strTotalFailures), "%d", val);
    }

    if (usmDbPasswdAgingStatusGet() == L7_SUCCESS)
    {
      rc = usmDbPasswordExpireTimeGet(unit, userCount, &expireTime);
      if (rc == L7_SUCCESS)
      {
        /* convert integer to time string */
        osapiSnprintf(strPasswordExpiry, 21, usmDbConvertTimeToDateString(expireTime));
      }
      else
      {
        osapiSnprintf(strPasswordExpiry, sizeof(strPasswordExpiry), "---");
      }
    }
    else
    {
      osapiSnprintf(strPasswordExpiry, sizeof(strPasswordExpiry), "---");
    }

    if (usmDbUserLockStatusGet(unit, userCount, &val) == L7_SUCCESS)
    {
      if (val == L7_TRUE)
      {
        osapiSnprintf(strLockout, sizeof(strLockout), pStrInfo_common_True2);
      }
      else
      {
        osapiSnprintf(strLockout, sizeof(strLockout), pStrInfo_common_False2);
      }
    }
    else
    {
      osapiSnprintf(strLockout, sizeof(strLockout), "---");
    }

    ewsTelnetPrintf (ewsContext, "\r\n%-21s%-9s%-9s%-9s%-8s%-23s",
                  strUserName, strCurrentFailures, strTotalFailures, strTotalLockouts, strLockout, strPasswordExpiry);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  display users logged in via telnet, ssh, and the web
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
* @notes add argument checking
*
* @cmdsyntax  show loginsession
*
* @cmdhelp Display login session info.
*
* @cmddescript
*   Show current web, telnet and ssh login sessions.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoginSession(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 buf2[L7_CLI_MAX_STRING_LENGTH],
           buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;
  L7_inet_addr_t aval;
  L7_uint32 i = 0;
  L7_BOOL validLogin = L7_FALSE;
  L7_timespec ts;
  L7_uint32 unit;
  L7_uint32 numArg;         /* New variable Added */
  L7_BOOL splitAddr = L7_FALSE;
  L7_char8 * p1 = L7_NULL,
  * p2 = L7_NULL;
  EwaSession session;
  L7_uint32 current_time;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowLoginSession_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_base_IdUsrNameConnFromIdleTimeSessionTimeSessionType);
  ewsTelnetWrite(ewsContext,"\r\n--   -------------   ---------------------        ----------- ------------ ------------ ");

  for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
  {
    usmDbLoginSessionValidEntry(unit, i, &validLogin);
    if (validLogin == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

      ewsTelnetPrintf (ewsContext, "%-5.2d", i);

      memset (buf, 0,sizeof(buf));
      memset (stat, 0,sizeof(stat));
      rc = usmDbLoginSessionUserGet(unit, i, buf);
      ewsTelnetPrintf (ewsContext, "%-16.13s", buf);

      memset (buf, 0,sizeof(buf));
      memset (buf2, 0,sizeof(buf2));
      memset (stat, 0,sizeof(stat));
      rc = usmDbLoginSessionRemoteIpAddrGet(unit, i, &aval);
      if (aval.family == L7_AF_INET)
      {
        rc = usmDbInetNtoa(osapiNtohl(aval.addr.ipv4.s_addr), buf);
      }
      else if (aval.family == L7_AF_INET6)
      {
        osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&aval.addr.ipv6, buf, L7_CLI_MAX_STRING_LENGTH);
      }
      else
      {
        OSAPI_STRNCPY_SAFE(buf,pStrInfo_base_Eia232);
      }
      OSAPI_STRNCPY_SAFE(buf2, buf);
      if( strlen(buf2) > 21)
      {
        splitAddr = L7_TRUE;
        p1 = buf2;
        p2 = buf2+21;
      }
      else
      {
        p1 = buf;
        p2 = NULL;
      }
      ewsTelnetPrintf (ewsContext, "%-29.39s", buf);

      memset (stat, 0,sizeof(stat));
      memset (buf, 0,sizeof(buf));
      rc = usmDbLoginSessionIdleTimeGet(unit, i, &val);
      osapiConvertRawUpTime(val, &ts);
      osapiSnprintf(buf,sizeof(buf),"%02d:%02d:%02d", (ts.days * 24) + ts.hours, ts.minutes, ts.seconds);
      ewsTelnetPrintf (ewsContext, "%-12.11s", buf);

      memset (stat, 0,sizeof(stat));
      memset (buf, 0,sizeof(buf));
      rc = usmDbLoginSessionTimeGet(unit, i, &val);
      osapiConvertRawUpTime(val, &ts);
      osapiSnprintf(buf,sizeof(buf),"%02d:%02d:%02d",(ts.days * 24) + ts.hours, ts.minutes, ts.seconds);
      ewsTelnetPrintf (ewsContext, "%-13.12s", buf);

      /* SSH TELNET */
      memset (stat, 0,sizeof(stat));
      memset (buf, 0,sizeof(buf));
      rc = usmDbLoginSessionTypeGet(unit, i, &val);
      if (val == 1)
      {
        ewsTelnetPrintf (ewsContext, "%-13.12s", pStrInfo_base_SerialPort);
      }
      else
      if (val == 2)
      {
        ewsTelnetPrintf (ewsContext, "%-13.12s", pStrInfo_base_Telnet2);

      }
      else
      if (val == 3)
      {
        ewsTelnetPrintf (ewsContext, "%-13.12s", pStrInfo_base_Ssh);
      }
      if( splitAddr == L7_TRUE)
      {
        if( p2 != NULL)
        {
          ewsTelnetPrintf (ewsContext, "\r\n                   %-21.20s", p2);
        }
      }
    }
  }

  /* Web Sessions */

  EwaSessionTableTimeoutScan();   /* Clean out any expired sessions  */

  for (i=0; i < FD_UI_DEFAULT_MAX_CONNECTIONS; i++)
  {
    if (EwaSessionGet(i, &session) != L7_SUCCESS)
    {
      continue;
    }
    /* ID    User Name    Connection From       Idle Time   Session Time Session Type */
    /* -- --------------- --------------------- ----------- ------------ ------------ */
    if (session.active == L7_TRUE)
    {
      /* ID */
      ewsTelnetPrintf (ewsContext, "\r\n%-5.2d", i+FD_CLI_DEFAULT_MAX_CONNECTIONS);

      /* User Name */
      ewsTelnetPrintf (ewsContext, "%-16.15s", session.uname);

      /* Location (Connection From) */
      if (session.inetAddr.family == L7_AF_INET)
      {
        if (usmDbInetNtoa(osapiNtohl(session.inetAddr.addr.ipv4.s_addr), buf) != L7_SUCCESS)
        {
          osapiStrncpySafe(buf, pStrInfo_common_Unknown_1, sizeof(buf));
        }
      }
      else  /* L7_AF_INET6 */
      {
        osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&session.inetAddr.addr.ipv6, buf, sizeof(buf));
        
      }
      ewsTelnetPrintf (ewsContext, "%-29.39s", buf);

      current_time = osapiUpTimeRaw();
      /* Idle Time */
      osapiConvertRawUpTime(current_time - session.last_access, &ts);
      osapiSnprintf(buf, sizeof(buf), "%02d:%02d:%02d", (ts.days * 24) + ts.hours, ts.minutes, ts.seconds);
      ewsTelnetPrintf (ewsContext, "%-12.11s", buf);

      /* Session Time */
      osapiConvertRawUpTime(current_time - session.first_access, &ts);
      osapiSnprintf(buf, sizeof(buf), "%02d:%02d:%02d",(ts.days * 24) + ts.hours, ts.minutes, ts.seconds);
      ewsTelnetPrintf (ewsContext, "%-13.12s", buf);

      /* Session Type */
      if (session.type == L7_LOGIN_TYPE_HTTP)
      {
        osapiStrncpySafe(buf, pStrInfo_base_Http, sizeof(buf));
      }
      else              /* L7_LOGIN_TYPE_HTTPS */
      {
        osapiStrncpySafe(buf, pStrInfo_base_Https, sizeof(buf));
      }
      ewsTelnetPrintf (ewsContext, "%-13.12s", buf);
    }
  }  /* end of for loop - web sessions */

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  display users, acces mode and status along with snmpv3 settings
*           if supported
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add argument checking
*
* @cmdsyntax  show users
*
* @cmdhelp Display Users and User Accounts.
*
* @cmddescript
*   Display a list of users, their access mode and their status.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowUsers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 val;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH +1];
  L7_uint32 numArg;         /* New variable Added */

  L7_int32 tempInt = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 columnNames[120];
  L7_char8 columnNamesTop[120];
  L7_char8 columnUnderscore[120];
  L7_uint32 snmpUserSupported = L7_FALSE;
  L7_uint32 snmpAuthEncryptSupported = L7_FALSE;

  L7_uint32 accessLevel, auth, encrypt;
  L7_uint32 unit;

  memset (columnNamesTop, 0, sizeof(columnNames));
  memset (columnNames, 0, sizeof(columnNames));
  memset (columnUnderscore, 0, sizeof(columnUnderscore));
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_ShowUsrs_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  osapiSnprintf( columnNamesTop, sizeof(columnNamesTop),     "\r\n                              ");
  osapiSnprintfAddBlanks (1, 0, 0, 2, L7_NULLPTR,  columnNames, sizeof(columnNames),           pStrInfo_base_UsrNameUsrAccessMode);
  osapiSnprintf( columnUnderscore, sizeof(columnUnderscore), "\r\n----------  ----------------  ");


  if (usmDbFeaturePresentCheck(
        unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_SUPPORTED) == L7_TRUE)
  {
    snmpUserSupported = L7_TRUE;
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 2, 5, L7_NULLPTR,  columnNamesTop,  pStrInfo_base_Snmpv3);
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 2, L7_NULLPTR,  columnNames,     pStrInfo_common_AccessMode);
    OSAPI_STRNCAT( columnUnderscore,"-----------  ");
  }
  if (usmDbFeaturePresentCheck(
        unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_AUTHENTICATION_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(
        unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID) == L7_TRUE)
  {
    snmpAuthEncryptSupported = L7_TRUE;

    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 4, 4, L7_NULLPTR,  columnNamesTop,  pStrInfo_base_Snmpv3Snmpv3);
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 2, L7_NULLPTR,  columnNames,     pStrInfo_base_AuthEncryption);
    OSAPI_STRNCAT( columnUnderscore,"--------------  ----------  ");
  }

  ewsTelnetWrite( ewsContext, columnNamesTop);
  ewsTelnetWrite( ewsContext, columnNames);
  ewsTelnetWrite( ewsContext, columnUnderscore);

  tempInt = 0;
  for (; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    memset (buf, 0, sizeof(buf));
    rc = cliWebLoginUserNameGet(tempInt, buf);
    if (strcmp(buf, pStrInfo_common_EmptyString) != 0)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%-10s  ",buf);

      memset (stat, 0,sizeof(stat));
      rc = cliWebLoginUserAccessModeGet(tempInt, &val);                         /* usmDB is hard coded */
      switch (val)                                                           /* val =  invalid, read only, read/write */
      {
      case L7_LOGIN_ACCESS_READ_ONLY:
        osapiSnprintf(stat,sizeof(stat),"%-16s  ",pStrInfo_base_ReadOnly);
        break;
      case L7_LOGIN_ACCESS_READ_WRITE:
        osapiSnprintf(stat,sizeof(stat),"%-16s  ",pStrInfo_base_ReadWrite);
        break;
      default:
        osapiSnprintf(stat,sizeof(stat),"%-16s  ",pStrInfo_base_ReadOnly);
      }
      ewsTelnetWrite(ewsContext,stat);

      if (snmpUserSupported == L7_TRUE)
      {
        if (cliWebLoginUserSnmpv3AccessModeGet(tempInt, &accessLevel) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_AttemptingToShowUsrs);
        }
        /* access mode */
        memset (stat, 0,sizeof(stat));
        switch (accessLevel)                                                            /* val =  invalid, read only, read/write */
        {
        case L7_AGENT_COMM_ACCESS_LEVEL_READ_ONLY:
          osapiSnprintf(stat,sizeof(stat),"%-11s  ",pStrInfo_base_ReadOnly);
          break;
        case L7_AGENT_COMM_ACCESS_LEVEL_READ_WRITE:
          osapiSnprintf(stat,sizeof(stat),"%-11s  ",pStrInfo_base_ReadWrite);
          break;
        default:
          osapiSnprintf(stat,sizeof(stat),"%-11s  ",pStrInfo_base_ReadOnly);
        }
        ewsTelnetWrite(ewsContext,stat);
      }

      if (snmpAuthEncryptSupported == L7_TRUE)
      {
        if (cliWebLoginUserAuthenticationGet( tempInt, &auth ) != L7_SUCCESS ||
            cliWebLoginUserEncryptionGet( tempInt, &encrypt ) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_AttemptingToShowUsrs);
        }
        /* authentication type */
        memset (stat, 0,sizeof(stat));
        switch (auth)
        {
        case L7_SNMP_USER_AUTH_PROTO_HMACMD5:
          osapiSnprintf(stat,sizeof(stat),"%-14s  ",pStrInfo_common_Md5);                  /* MD5 */
          break;
        case L7_SNMP_USER_AUTH_PROTO_HMACSHA:
          osapiSnprintf(stat,sizeof(stat),"%-14s  ", pStrInfo_base_Sha);
          break;
        case L7_SNMP_USER_AUTH_PROTO_NONE:
          osapiSnprintf(stat,sizeof(stat),"%-14s  ", pStrInfo_common_None_1);                 /* None */
          break;
        default:
          osapiSnprintf(stat,sizeof(stat),"%-14s  ", pStrInfo_base_NotApplicable2);                /* N/A*/
          break;
        }
        ewsTelnetWrite(ewsContext,stat);

        /* encryption type */
        memset (stat, 0,sizeof(stat));
        switch (encrypt)
        {
        case L7_SNMP_USER_PRIV_PROTO_DES:
          osapiSnprintf(stat,sizeof(stat),"%-10s  ",pStrInfo_base_Des);
          break;
        case L7_SNMP_USER_PRIV_PROTO_NONE:
          osapiSnprintf(stat,sizeof(stat),"%-10s  ", pStrInfo_common_None_1);                 /* None */
          break;
        default:
          osapiSnprintf(stat,sizeof(stat),"%-10s  ", pStrInfo_base_NotApplicable2);                /* N/A*/
          break;
        }
        ewsTelnetWrite(ewsContext,stat);
      }    /* end if auth or encrypt supported */

    }   /* end if user name is not blank */

  }  /* end of for loop */

  return cliSyntaxReturnPrompt (ewsContext, "");

}


/*********************************************************************
*
* @purpose  display users complete names
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add argument checking
*
* @cmdsyntax  show usernames
*
* @cmdhelp Display Users.
*
* @cmddescript
*   Display a list of users.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowUserLongNames(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH +1];

  L7_int32 tempInt = 0;

  L7_char8 columnNames[120];
  L7_char8 columnNamesTop[120];
  L7_char8 columnUnderscore[120];

  L7_uint32 unit;

  memset (columnNamesTop, 0, sizeof(columnNames));
  memset (columnNames, 0, sizeof(columnNames));
  memset (columnUnderscore, 0, sizeof(columnUnderscore));

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  osapiSnprintf( columnNamesTop, sizeof(columnNamesTop),     "\r\n                              ");
  osapiSnprintfAddBlanks (1, 0, 0, 2, L7_NULLPTR,  columnNames, sizeof(columnNames),           "User Name");
  osapiSnprintf( columnUnderscore, sizeof(columnUnderscore), "\r\n------------");

  ewsTelnetWrite( ewsContext, columnNamesTop);
  ewsTelnetWrite( ewsContext, columnNames);
  ewsTelnetWrite( ewsContext, columnUnderscore);

  tempInt = 0;
  for (; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    memset (buf, 0, sizeof(buf));
    rc = cliWebLoginUserNameGet(tempInt, buf);
    if (strcmp(buf, pStrInfo_common_EmptyString) != 0)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%s  ",buf);
    }   /* end if user name is not blank */

  }  /* end of for loop */

  return cliSyntaxReturnPrompt (ewsContext, "");

}
/*********************************************************************
*
* @purpose  display complete user names logged in via telnet, ssh, and the web
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
* @notes add argument checking
*
* @cmdsyntax  show loginsession
*
* @cmdhelp Display login session info.
*
* @cmddescript
*   Show current web, telnet and ssh login sessions.
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowLoginSessionUserLongNames(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i = 0;
  L7_BOOL validLogin = L7_FALSE;
  L7_uint32 unit;
  EwaSession session;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,"User Name");
  ewsTelnetWrite(ewsContext,"\r\n----------------- ");

  for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
  {
    usmDbLoginSessionValidEntry(unit, i, &validLogin);
    if (validLogin == L7_TRUE)
    {
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

      memset (buf, 0,sizeof(buf));
      rc = usmDbLoginSessionUserGet(unit, i, buf);
      ewsTelnetPrintf (ewsContext, "%-16s", buf);

      memset (buf, 0,sizeof(buf));
    }
  }

  /* Web Sessions */

  EwaSessionTableTimeoutScan();   /* Clean out any expired sessions  */

  for (i=0; i < FD_UI_DEFAULT_MAX_CONNECTIONS; i++)
  {
    if (EwaSessionGet(i, &session) != L7_SUCCESS)
    {
      continue;
    }
    /* ID    User Name    Connection From       Idle Time   Session Time Session Type */
    /* -- --------------- --------------------- ----------- ------------ ------------ */
    if (session.active == L7_TRUE)
    {

      /* User Name */
      ewsTelnetPrintf (ewsContext, "\r\n%-16s", session.uname);

    }
  }  /* end of for loop - web sessions */

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose   Shows User Manager Login history long names
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
* @cmdsyntax  show users login-history long
*
*
* @cmdhelp
*
* @cmddescript  Displays all user login history
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandShowLoginHistoryLongNames(EwsContext ewsContext,
                                              uintf argc,
                                              const L7_char8 **argv,
                                              uintf index)
{
  L7_RC_t   rc=L7_FAILURE;
  L7_char8  strUserName[L7_LOGIN_SIZE];
  L7_uint32 unit;
  L7_inet_addr_t location;
  static L7_uint32 historyIndex = 0;
  L7_uint32 time;
  L7_uint32 count = 0;
  L7_LOGIN_TYPE_t protocol;
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  /**** Error Checking for command Type*****/
  CLI_COMMANDTYPE_NORMAL;
  cliSyntaxTop(ewsContext);

    /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  ewsTelnetWrite( ewsContext,"\r\nUsername");
  ewsTelnetWrite( ewsContext,"\r\n----------------------");
  count = count + 2;

    if (usmdbUserMgrLoginHistoryGetFirst(unit,&historyIndex,strUserName,&protocol,&location,&time) == L7_SUCCESS)
    do
    {
        bzero(stat,sizeof(stat));
        osapiSnprintf(stat, sizeof(stat), "\r\n%s",strUserName);
        ewsTelnetWrite(ewsContext,stat);
        count++;

        if (count >= CLI_MAX_SCROLL_LINES - 6)
        {
          break;
        }
      rc = usmdbUserMgrLoginHistoryGetNext(unit, historyIndex, &historyIndex, strUserName, &protocol, &location, &time);

     }while (rc == L7_SUCCESS);

  cliSyntaxBottom(ewsContext);
  return cliSyntaxReturnPrompt (ewsContext, "");
}
