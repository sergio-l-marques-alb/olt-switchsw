/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_usermgr.c
 *
 * @purpose User Manager configuration commands for the cli
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
#include "cli_mode.h"
#include "cli_show_running_config.h"

/*********************************************************************
*
* @purpose   Config User Manager authnetication login lists
*
*
* @param input EwsContext ewsContext
* @param input uintf argc
* @param input const L7_char8 **argv
* @param input index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  aaa authentication login {default | list-name} method1 [method2...]
* @noform     aaa authentication login {default | list-name}
*
* @cmdhelp
*
* @cmddescript  creates/deletes/assigns authentication methods
* to authentication login list
*
* @author  P.Murali Krishna
* @end
*
*********************************************************************/
const L7_char8 *commandAuthenticationLogin(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 **argv,
                                           uintf index)
{
  /* to store the position of listname */
  L7_uint32 argListName = 1;
  L7_uint32 unit=0, listIndex=0, numArgs=0;
  L7_uint32 order=0, argLocation=0,argLocationNext=0;

  /* Temporary variable to hold the authentication methods */
  L7_USER_MGR_AUTH_METHOD_t authMethod = L7_AUTH_METHOD_UNDEFINED;

  /* String to contain the list name  */
  L7_char8  strAuthListName[L7_MAX_AUTHENTICATIONLIST_NAME_SIZE + 1];

  /* The string to contain the method names pased as arguments */
  L7_char8  strMethod[L7_CLI_MAX_STRING_LENGTH];

  /* The temp string to hold for method duplication search case */
  L7_char8  strMethod1[L7_CLI_MAX_STRING_LENGTH];

  /* Return Code */
  L7_RC_t        rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* number of arguments in the command */
  numArgs = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Verify number of arguments for normal command*/
    if ((numArgs < 2) || (numArgs > L7_MAX_LOGINAUTH_METHODS+1))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgAuthLoginCreate);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* verify the arguments for no form of the command */
    if (numArgs != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgAuthNoLoginCreate);
    }
  }

  /* Verify the length of Authentication List Name */
  if (strlen(argv[index+argListName]) >= sizeof(strAuthListName))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_InvalidLoginListLen, L7_MAX_AUTHENTICATIONLIST_NAME_SIZE);
  }

  bzero(strAuthListName,sizeof(strAuthListName));
  osapiStrncpySafe(strAuthListName, argv[index+argListName], sizeof(strAuthListName));

  if (usmDbStringCaseInsensitiveCompare(strAuthListName, L7_USER_MGR_DEFAULT_USER_STRING) == L7_SUCCESS)
  {
    bzero(strAuthListName,sizeof(strAuthListName));
    strcpy(strAuthListName, L7_DEFAULT_APL_NAME);
  }
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    /*Need to prompt user for invalid authentication method
      before creating authentication list name */

    for (argLocation=2; argLocation <= numArgs; argLocation++)
    {
      bzero(strMethod,sizeof(strMethod));
      osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
      cliConvertToLowerCase(strMethod);

      if ((strcmp(strMethod, pStrInfo_base_EnableAuth) != 0) &&
          (strcmp(strMethod, pStrInfo_base_LineAuth) != 0)   &&
          (strcmp(strMethod, pStrInfo_base_LocalAuth) != 0)  &&
          (strcmp(strMethod, pStrInfo_base_NoneAuth) != 0)   &&
          (strcmp(strMethod, pStrInfo_base_RadiusAuth) != 0) &&
          (strcmp(strMethod, pStrInfo_base_TacacsAuth) != 0))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectMethod,  ewsContext, pStrErr_base_InvalidLoginAuth);
      }/* End of checking the validity of the methods entered */

      /*Need to prompt user for duplicate authentication method
       * before creating authentication list name
       */

      /*argLocation=2 is starting position of methods and
       *argLocationNext is to access the next method in
       * comparision for duplicated method
       */

      for (argLocationNext=(argLocation+1); argLocationNext <= numArgs;
          argLocationNext++)
      {
        bzero(strMethod,sizeof(strMethod));
        osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
        osapiStrncpySafe(strMethod1, argv[index + argLocationNext], sizeof(strMethod1));
        if (strcmp(strMethod,strMethod1)== 0)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidDuplicateMethod, strMethod);
        }
      }/* end of checking duplicated methods*/
    }  /* End of for(argLocation=2;argLocation<= ..... */

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Set an ordered list of methods in the authentication list */
      if (( L7_SUCCESS != cliIsAlphaNum(strAuthListName) ) ||
          ( L7_SUCCESS != usmDbAuthenticationListIndexGetByName(ACCESS_LINE_CTS,
                                                                ACCESS_LEVEL_LOGIN,
                                                                strAuthListName,
                                                                &listIndex) ))
      {
        /* Create an authentication logon list */

        rc = usmDbAPLCreate(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN,
                                           strAuthListName);

        if (rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_CreateAuthListError);
        }
        else if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_CreateAuthListError2);
        }
      }
      order = 0; /* order of auth methods in an APL */
      for (argLocation=2; order < L7_MAX_AUTH_METHODS; argLocation++, order++)
      {
        bzero(strMethod,sizeof(strMethod));
        if (argLocation <= numArgs)
        {
          osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
          cliConvertToLowerCase(strMethod);
        }

        if (strcmp(strMethod, pStrInfo_base_EnableAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_ENABLE;
        }
        else if (strcmp(strMethod, pStrInfo_base_LineAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_LINE;
        }
        else if (strcmp(strMethod, pStrInfo_base_LocalAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_LOCAL;
        }
        else if (strcmp(strMethod, pStrInfo_base_NoneAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_NONE;
        }
        else if (strcmp(strMethod, pStrInfo_base_RadiusAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_RADIUS;
        }
        else if (strcmp(strMethod, pStrInfo_base_TacacsAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_TACACS;
        }
        else
        {
          authMethod = L7_AUTH_METHOD_UNDEFINED;
        }
        /* Adding the method in the APL in proper order */

        if ( usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_LOGIN,
                                                  strAuthListName,
                                                  order,
                                                  authMethod ) !=
             L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext, pStrErr_base_SetAuthListError);
        }
      } /* end of for loop for (argLocation=2 .....*/
    } /* end of if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) */
  } /* end of if( ewsContext->commType == CLI_NORMAL_CMD) */
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Delete the specified authentication List */
      if (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_CTS,
                                                ACCESS_LEVEL_LOGIN,
                                                strAuthListName, &listIndex)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
      }
      /* if any  line entry has a reference to the list then that list cannot deleted */
      if (usmDbAPLDelete(ACCESS_LINE_CTS,
                                        ACCESS_LEVEL_LOGIN,
                                        strAuthListName) != L7_SUCCESS)
      {
        /* default list can also be deleted
         * application function usmDbAPLDelete  need to be change accordingly

        if (usmDbAuthenticationListDefaultCheck(ACCESS_LINE_CTS,
                                                ACCESS_LEVEL_LOGIN,
                                                strAuthListName) == L7_TRUE)
        {
          ewsTelnetWrite(ewsContext, CLI_DELETEDEFAULTAPL_ERR);
        }
        else*/
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ErrorDeleteList);
        }
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }/* end of deletion */
    } /* end of if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) */
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config User Manager authentication dot1x lists
*
*
* @param input EwsContext ewsContext
* @param input uintf argc
* @param input const L7_char8 **argv
* @param input index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  aaa authentication dot1x {default} method1 [method2...]
* @noform     aaa authentication dot1x {default}
*
* @cmdhelp
*
* @cmddescript  creates/deletes/assigns authentication methods
* to authentication dot1x list
*
* @author  P.Murali Krishna
* @end
*
*********************************************************************/
const L7_char8 *commandAuthenticationDot1x(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 **argv,
                                           uintf index)
{
  /* to store the position of listname */
  L7_uint32 unit=0, numArgs=0;
  L7_uint32 order=0, argLocation=0,argLocationNext=0;

  /* Temporary variable to hold the authentication methods */
  L7_USER_MGR_AUTH_METHOD_t authMethod = L7_AUTH_METHOD_UNDEFINED;

  /* The string to contain the method names pased as arguments */
  L7_char8  strMethod[L7_CLI_MAX_STRING_LENGTH];

  /* The temp string to hold for method duplication search case */
  L7_char8  strMethod1[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* number of arguments in the command */
  numArgs = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Verify number of arguments for normal command*/
    if ((numArgs < 2) || (numArgs > 4))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IncorrectDot1xAuth);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* verify the arguments for no form of the command */
    if (numArgs != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IncorrectNoDot1xAuth);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    /*Need to prompt user for invalid authentication method
      before creating authentication list name */

    for (argLocation=2; argLocation <= numArgs; argLocation++)
    {
      bzero(strMethod,sizeof(strMethod));
      osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
      cliConvertToLowerCase(strMethod);

      if ((strcmp(strMethod, pStrInfo_base_NoneAuth) != 0)   &&
          (strcmp(strMethod, pStrInfo_base_RadiusAuth) != 0) &&
          (strcmp(strMethod, pStrInfo_base_LocalAuth) != 0) )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectMethod, ewsContext, pStrErr_base_InvalidDot1xAuth);
      }/* End of checking the validity of the methods entered */

      /*Need to prompt user for duplicate authentication method
       * before creating authentication list name
       */

      /*argLocation=2 is starting position of methods and
       *argLocationNext is to access the next method in
       * comparision for duplicated method
       */

      for (argLocationNext=(argLocation+1); argLocationNext <= numArgs;
          argLocationNext++)
      {
        bzero(strMethod,sizeof(strMethod));
        osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
        osapiStrncpySafe(strMethod1, argv[index + argLocationNext], sizeof(strMethod1));
        if (strcmp(strMethod,strMethod1)== 0)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidDuplicateMethod, strMethod);
        }
      }/* end of checking duplicated methods*/
    }  /* End of for(argLocation=2;argLocation<= ..... */

    /* Set an ordered list of methods in the authentication list */
    order = 0; /* order of auth methods in an APL */
    for (argLocation=2; order < L7_MAX_AUTH_METHODS; argLocation++, order++)
    {
      bzero(strMethod,sizeof(strMethod));
      if (argLocation <= numArgs)
      {
        osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
        cliConvertToLowerCase(strMethod);
      }

      if (strcmp(strMethod, pStrInfo_base_NoneAuth) == 0)
      {
        authMethod = L7_AUTH_METHOD_NONE;
      }
      else if (strcmp(strMethod, pStrInfo_base_RadiusAuth) == 0)
      {
        authMethod = L7_AUTH_METHOD_RADIUS;
      }
      else if (strcmp(strMethod, pStrInfo_base_LocalAuth) == 0)
      {
        authMethod = L7_AUTH_METHOD_LOCAL;
      }
      else
      {
        authMethod = L7_AUTH_METHOD_UNDEFINED;
      }
      /* Adding the method in the APL in proper order */

      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if ( usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                                  ACCESS_LEVEL_LOGIN,
                                                  "default",
                                                  order,
                                                  authMethod ) !=
             L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgAuthDot1xSetError);
        }
      }
    } /* end of for loop for (argLocation=2 .....*/
  } /* end of if( ewsContext->commType == CLI_NORMAL_CMD) */
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      for (order = 0; order < 3; order++)
      {
        if ( usmDbAPLAuthMethodSet(ACCESS_LINE_DOT1X,
                                                  ACCESS_LEVEL_LOGIN,
                                                  "default",
                                                  order,
                                                  L7_AUTH_METHOD_UNDEFINED ) !=
             L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgAuthDot1xSetError);
        }
      } /* end of for loop for (argLocation=2 .....*/
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config User Manager authentication enable lists
*
*
* @param input EwsContext ewsContext
* @param input uintf argc
* @param input const L7_char8 **argv
* @param input uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  aaa authentication enable {default | list-name} method1 [method2...]
* @noform     no aaa authentication enable (default | list-name}
*
* @cmdhelp
*
* @cmddescript  creates/deletes/assigns authentication methods
* to authentication enable list
*
* @author P.Murali Krishna
* @end
*
*********************************************************************/
const L7_char8  *commandAuthenticationEnable(EwsContext ewsContext, uintf argc,
                                             const L7_char8 **argv,
                                             uintf index)
{
  L7_uint32 argListName = 1 ;
  L7_uint32 unit=0, listIndex=0;
  L7_uint32 numArgs=0, order=0, argLocation=0,argLocationNext=0;

  /* Temporary variable to hold the authentication methods */
  L7_USER_MGR_AUTH_METHOD_t authMethod = L7_AUTH_METHOD_UNDEFINED;
  /* String to contain the list name  */
  L7_char8  strAuthListName[L7_MAX_AUTHENTICATIONLIST_NAME_SIZE + 1];

  /* The string to contain the method names pased as arguments */
  L7_char8  strMethod[L7_CLI_MAX_STRING_LENGTH];

  /* The temp string to hold for method duplication search case */
  L7_char8  strMethod1[L7_CLI_MAX_STRING_LENGTH];

  /* Return Code */
  L7_RC_t        rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* number of arguments passed to the command */
  numArgs = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Verify number of arguments for normal command*/
    if ((numArgs < 2) || (numArgs > L7_MAX_ENABLEAUTH_METHODS+1))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IncorrectEnableAuth);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* verify the arguments for no form of the command */
    if (numArgs != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_IncorrectNoEnableAuth);
    }
  }

  /* Verify the length of Authentication List Name */
  if (strlen(argv[index+argListName]) >= sizeof(strAuthListName))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidLoginListLen, L7_MAX_AUTHENTICATIONLIST_NAME_SIZE);
  }
  bzero(strAuthListName,sizeof(strAuthListName));
  osapiStrncpySafe(strAuthListName, argv[index+argListName], sizeof(strAuthListName));

  if (usmDbStringCaseInsensitiveCompare(strAuthListName, L7_USER_MGR_DEFAULT_USER_STRING) == L7_SUCCESS)
  {
    bzero(strAuthListName,sizeof(strAuthListName));
    strcpy(strAuthListName, L7_ENABLE_APL_NAME);
  }
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    /*Need to prompt user for invalid authentication method
      before creating authentication list name */

    for (argLocation=2; argLocation <= numArgs; argLocation++)
    {
      bzero(strMethod,sizeof(strMethod));
      osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
      cliConvertToLowerCase(strMethod);

      if ((strcmp(strMethod, pStrInfo_base_EnableAuth) != 0) &&
          (strcmp(strMethod, pStrInfo_base_LineAuth) != 0)   &&
          (strcmp(strMethod, pStrInfo_base_NoneAuth) != 0)   &&
          (strcmp(strMethod, pStrInfo_base_RadiusAuth) != 0) &&
          (strcmp(strMethod, pStrInfo_base_TacacsAuth) != 0))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectMethod, ewsContext, pStrErr_base_InvalidEnableAuth);
      }/* end  of checking for invalid authentication method */

      /*Need to prompt user for duplicate authentication method
       *before creating authentication list name
       */

      /*argLocation=2 is starting position of methods and
       *argLocationNext is to access the next method in
       * comparision for duplicated method
       */

      for (argLocationNext=(argLocation+1); argLocationNext <= numArgs;
          argLocationNext++)
      {
        bzero(strMethod,sizeof(strMethod));
        osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
        osapiStrncpySafe(strMethod1, argv[index + argLocationNext], sizeof(strMethod1));
        if (strcmp(strMethod,strMethod1)== 0)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidDuplicateMethod, strMethod);
        }
      } /* end of checking for duplicated method */
    }/*end of for loop for(argLocation =2 ;argLocation <=  .....*/

    /*******Check if the Flag is Set for Execution*************/
    if ( L7_EXECUTE_SCRIPT == ewsContext->scriptActionFlag )
    {
      /* Set an ordered list of methods in the authentication list */
      if ((cliIsAlphaNum(strAuthListName) != L7_SUCCESS) ||
          (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_CTS,
                                                 ACCESS_LEVEL_ENABLE,
                                                 strAuthListName, &listIndex)
           != L7_SUCCESS))
      {
        /* Create an authentication enable list */

        rc = usmDbAPLCreate(ACCESS_LINE_CTS, ACCESS_LEVEL_ENABLE,
                                           strAuthListName);

        if (rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_CreateAuthListError);
        }
        else if (rc == L7_TABLE_IS_FULL)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_CreateAuthListError2);
        }
      }

      order = 0; /* order of auth methods in an APL */
      for (argLocation=2; order < L7_MAX_AUTH_METHODS; argLocation++, order++)
      {
        bzero(strMethod,sizeof(strMethod));
        if (argLocation <= numArgs)
        {
          osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
          cliConvertToLowerCase(strMethod);
        }

        if (strcmp(strMethod, pStrInfo_base_EnableAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_ENABLE;
        }
        else if (strcmp(strMethod, pStrInfo_base_LineAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_LINE;
        }
        else if (strcmp(strMethod, pStrInfo_base_NoneAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_NONE;
        }
        else if (strcmp(strMethod, pStrInfo_base_RadiusAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_RADIUS;
        }
        else if (strcmp(strMethod, pStrInfo_base_TacacsAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_TACACS;
        }
        else
        {
          authMethod = L7_AUTH_METHOD_UNDEFINED;
        }
        if ( usmDbAPLAuthMethodSet(ACCESS_LINE_CTS,
                                                  ACCESS_LEVEL_ENABLE,
                                                  strAuthListName,
                                                  order,
                                                  authMethod ) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgAuthEnableSetError);
        }

      } /* end of for loop for (argLocation=2 .....*/
    } /* end of if ( L7_EXECUTE_SCRIPT == ewsContext->scriptActionFlag ) */
  } /* end of if( ewsContext->commType == CLI_NORMAL_CMD) */
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if ( L7_EXECUTE_SCRIPT == ewsContext->scriptActionFlag )
    {
      /*checking for list creation */
      if (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_CTS,
                                                ACCESS_LEVEL_ENABLE,
                                                strAuthListName, &listIndex)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
      }

      /* before delete a perticular list check whether that list is used by any mode
       * if any  line entry has a reference to the list then that list cannot be deleted
       */
      if (usmDbAPLDelete(ACCESS_LINE_CTS,
                                        ACCESS_LEVEL_ENABLE,
                                        strAuthListName) != L7_SUCCESS)
      {
        /* Default list can also be deleted
         * application function usmDbAPLDelete  need to be change accordingly

        if (usmDbAuthenticationListDefaultCheck(ACCESS_LINE_CTS,
                                                ACCESS_LEVEL_ENABLE,
                                                strAuthListName) == L7_TRUE)
        {
          ewsTelnetWrite(ewsContext, CLI_DELETEDEFAULTAPL_ERR);
        }
        else*/
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_ErrorDeleteList);
        }
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }/* end of deletion */
    }/* end of if ( L7_EXECUTE_SCRIPT == ewsContext->scriptActionFlag ) */
  }/* end of no command */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Set the authentication methods for https server users
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8 *
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  ip https authentication method1 [method2]
*             no ip https authentication
* @author     Sudip Midya
*
* @end
*
*********************************************************************/
const L7_char8  *commandHttpsAuthenticationLogin(EwsContext ewsContext,
                                                 uintf argc,
                                                 const L7_char8 **argv,
                                                 uintf index)
{
  /* the number of arguments passed */
  L7_uint32 numArgs =0,argLocation = 0, argLocationNext =0,unit=0,order=0;

  /* Temporary variable to hold the authentication methods */
  L7_USER_MGR_AUTH_METHOD_t authMethod = L7_AUTH_METHOD_UNDEFINED;

  /* String to contain the list name of https list name */
  L7_char8  strAuthListName[L7_MAX_APL_NAME_SIZE + 1];

  /* The string to contain the method names pased as arguments */
  L7_char8  strMethod[L7_CLI_MAX_STRING_LENGTH];

  /* The temp string to hold for method duplication search case */
  L7_char8  strMethod1[L7_CLI_MAX_STRING_LENGTH];

  /* Authication mode variable(2 for https) */
  L7_ACCESS_LEVEL_t authMode = 2;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* Get the number of arguments passed in the command */
  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);
  /* The num of arguments passed */
  numArgs = cliNumFunctionArgsGet();

  /* Verify number of arguments */
  /* The min value of numArgs is 1
   */
  if ((((numArgs < L7_HTTPS_MIN_AUTH_METHODS)||
        (numArgs > (L7_HTTPS_MAX_AUTH_METHODS))) &&
       (ewsContext->commType == CLI_NORMAL_CMD))||
      ((numArgs > 0) && (ewsContext->commType == CLI_NO_CMD)))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_InvalidHttpsAuth);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_InvalidNoHttpsAuth);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  bzero(strAuthListName,sizeof(strAuthListName));
  strcpy(strAuthListName,CLI_HTTPSLISTNAME_AUTH);

  /* If the command is a NORMAL command */
  if (ewsContext->commType == CLI_NORMAL_CMD &&
      ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {

    /* If the number of arguments is within the range */
    if ((numArgs >= 1)&&(numArgs <= L7_HTTPS_MAX_AUTH_METHODS))
    {
      /* Need to prompt user for invalid authentication method
       * before creating authentication list name
       */
      for (argLocation=1; argLocation <= numArgs; argLocation++)
      {
        bzero(strMethod,sizeof(strMethod));
        osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
        cliConvertToLowerCase(strMethod);

        if ((strcmp(strMethod, pStrInfo_base_LocalAuth) != 0) &&
            (strcmp(strMethod, pStrInfo_base_RadiusAuth) != 0) &&
            (strcmp(strMethod, pStrInfo_base_TacacsAuth) != 0) &&
            (strcmp(strMethod, pStrInfo_base_NoneAuth) != 0))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectMethod,  ewsContext, pStrErr_base_InvalidLoginAuth);
        }

        /* Now checking the duplicacy of the methods entered */
        for (argLocationNext=(argLocation+1); argLocationNext <= numArgs;
            argLocationNext++)
        {
          bzero(strMethod,sizeof(strMethod));
          osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
          osapiStrncpySafe(strMethod1, argv[index + argLocationNext], sizeof(strMethod1));
          if (strcmp(strMethod,strMethod1)== 0)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidDuplicateMethod, strMethod);
          }
        }/* End of checking for duplicated method */
      }/* End of checking the validity of the methods entered */


      /* Adding all the methods in the argument list */
      /* We are not checking the correctness of the arguments now,
       * as it has been checked earlier.
       */
      order = 0;
      for (argLocation=1; order < L7_MAX_AUTH_METHODS; argLocation++, order++)
      {
        bzero(strMethod,sizeof(strMethod));
        if (argLocation <= numArgs)
        {
          osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
          cliConvertToLowerCase(strMethod);
        }

        if (strcmp(strMethod, pStrInfo_base_LocalAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_LOCAL;
        }
        else if (strcmp(strMethod, pStrInfo_base_TacacsAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_TACACS;
        }
        else if (strcmp(strMethod, pStrInfo_base_RadiusAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_RADIUS;
        }
        else if (strcmp(strMethod, pStrInfo_base_NoneAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_NONE;
        }
        else
        {
          authMethod = L7_AUTH_METHOD_UNDEFINED;
        }

        /* Adding the method in the APL in proper order */
        if (usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                  authMode,
                                  strAuthListName,
                                  order,
                                  authMethod) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext, pStrErr_base_SetAuthListError);
        }
      }/* End of for (argLocation=1; argLocation <... */
    }/* End of if ((numArgs >= 1)&&(numArgs */
  }/* End of if(ewsContext->commType == CLI_NORMAL_CMD) */

  /* If the comand is a no command, remove the methods in the APL and insert
   * the local method in the corresponding APL
   */
  else if (ewsContext->commType == CLI_NO_CMD &&
           ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Remove the methods of the list */
    if (usmDbAuthenticationListMethodRemove(ACCESS_LINE_HTTPS, 0,
                                            strAuthListName) !=
        L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidRemoveAuth);
    }

    /* If the methods are removed, insert the Local method as default
     * in the list Set the APL to default
     * Insert in the list CLILOCAL_AUTH method
     */
    if (usmDbAPLAuthMethodSet(ACCESS_LINE_HTTPS,
                                             authMode,
                                             strAuthListName,
                                             0,
                                             L7_AUTH_METHOD_LOCAL) !=
        L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext, pStrErr_base_SetAuthListError);
    }
  }/* End of the no command if loop */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Set the authentication methods for http server users
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8 *
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  ip http authentication method1 [method2]
*             no ip http authentication
* @author     Sudip Midya
*
* @end
*
*********************************************************************/
const L7_char8  *commandHttpAuthenticationLogin(EwsContext ewsContext,
                                                uintf argc,
                                                const L7_char8 **argv,
                                                uintf index)
{
  /* the number of arguments passed */
  L7_uint32 numArgs =0,argLocation = 0, argLocationNext =0,unit=0,order=0;

  /* Temporary variable to hold the authentication methods */
  L7_USER_MGR_AUTH_METHOD_t authMethod = L7_AUTH_METHOD_UNDEFINED;

  /* String to contain the list name of http list name */
  L7_char8  strAuthListName[L7_MAX_APL_NAME_SIZE + 1];

  /* The string to contain the method names pased as arguments */
  L7_char8  strMethod[L7_CLI_MAX_STRING_LENGTH];

  /* The temp string to hold for method duplication search case */
  L7_char8  strMethod1[L7_CLI_MAX_STRING_LENGTH];

  /* Authication mode variable(3 for http) */
  L7_ACCESS_LEVEL_t authMode = 3;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* Get the number of arguments passed in the command */
  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* The num of arguments passed */
  numArgs = cliNumFunctionArgsGet();

  /* Verify number of arguments */
  /* The min value of numArgs is 1
   */
  if ((((numArgs < L7_HTTP_MIN_AUTH_METHODS)||
        (numArgs > (L7_HTTP_MAX_AUTH_METHODS))) &&
       (ewsContext->commType == CLI_NORMAL_CMD))||
      ((numArgs > 0) && (ewsContext->commType == CLI_NO_CMD)))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_InvalidHttpAuth);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_InvalidNoHttpAuth);
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* Copy the string name reserved for http authentication to strAuthListName*/
  bzero(strAuthListName,sizeof(strAuthListName));
  strcpy(strAuthListName,CLI_HTTPLISTNAME_AUTH);

  /* If the command is a NORMAL command */
  if (ewsContext->commType == CLI_NORMAL_CMD &&
      ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* If the number of arguments is within the range */
    if ((numArgs >= 1)&&(numArgs <= L7_HTTP_MAX_AUTH_METHODS))
    {
      /* Need to prompt user for invalid authentication method
       * before creating authentication list name
       */
      for (argLocation=1; argLocation <= numArgs; argLocation++)
      {
        bzero(strMethod,sizeof(strMethod));
        osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
        cliConvertToLowerCase(strMethod);

        if ((strcmp(strMethod, pStrInfo_base_LocalAuth) != 0) &&
            (strcmp(strMethod, pStrInfo_base_RadiusAuth) != 0) &&
            (strcmp(strMethod, pStrInfo_base_TacacsAuth) != 0) &&
            (strcmp(strMethod, pStrInfo_base_NoneAuth) != 0))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectMethod,  ewsContext, pStrErr_base_InvalidLoginAuth);
        }

        /* Now checking the duplicacy of the methods entered */
        for (argLocationNext=(argLocation+1); argLocationNext <= numArgs;
            argLocationNext++)
        {
          bzero(strMethod,sizeof(strMethod));
          osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
          osapiStrncpySafe(strMethod1, argv[index + argLocationNext], sizeof(strMethod1));
          if (strcmp(strMethod,strMethod1)== 0)
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidDuplicateMethod, strMethod);
          }
        }/* End of checking for duplicated method */
      }/* End of checking the validity of the methods entered */


      /* Adding all the methods in the argument list */
      /* We are not checking the correctness of the arguments now,
       * as it has been checked earlier.
       */
      order = 0;
      for (argLocation=1; order < L7_MAX_AUTH_METHODS; argLocation++, order++)
      {
        bzero(strMethod,sizeof(strMethod));
        if (argLocation <= numArgs)
        {
          osapiStrncpySafe(strMethod, argv[index + argLocation], sizeof(strMethod));
          cliConvertToLowerCase(strMethod);
        }

        if (strcmp(strMethod, pStrInfo_base_LocalAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_LOCAL;
        }
        else if (strcmp(strMethod, pStrInfo_base_TacacsAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_TACACS;
        }
        else if (strcmp(strMethod, pStrInfo_base_RadiusAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_RADIUS;
        }
        else if (strcmp(strMethod, pStrInfo_base_NoneAuth) == 0)
        {
          authMethod = L7_AUTH_METHOD_NONE;
        }
        else
        {
          authMethod = L7_AUTH_METHOD_UNDEFINED;
        }

        /* Adding the method in the APL in proper order */
        if ( usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                                  authMode,
                                                  strAuthListName,
                                                  order,
                                                  authMethod) !=
             L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext, pStrErr_base_SetAuthListError);
        }
      }/* End of for (argLocation=1; argLocation <... */
    }/* End of if ((numArgs >= 1)&&(numArgs */
  }/* End of if(ewsContext->commType == CLI_NORMAL_CMD) */

  /* If the comand is a no command */
  else if (ewsContext->commType == CLI_NO_CMD &&
           ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Remove the methods of the list */
    if (usmDbAuthenticationListMethodRemove( ACCESS_LINE_HTTP, 0,
                                             strAuthListName) !=
        L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_InvalidRemoveAuth);
    }

    /* If the methods are removed, insert the Local method as default
     * in the list Set the APL to default
     * Insert in the list CLILOCAL_AUTH method
     */
    if (usmDbAPLAuthMethodSet(ACCESS_LINE_HTTP,
                                             authMode,
                                             strAuthListName,
                                             0,
                                             L7_AUTH_METHOD_LOCAL) !=
        L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext, pStrErr_base_SetAuthListError);
    }
  }/* End of else if (ewsContext->commType == CLI_NO_CMD) */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config User Manager General Information
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
* @cmdsyntax  users defaultlogin <listname>
*
*
* @cmdhelp
*
* @cmddescript  assigns authentication login list to non-configured user.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersDefaultLogin(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index)
{
  L7_uint32 argList = 1;
  L7_uint32 unit;
  L7_uint32 i;
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE + 1];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgUsrsDeflLogin);
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

  OSAPI_STRNCPY_SAFE(strAuthListName, argv[index+argList]);
  if ((cliIsAlphaNum(strAuthListName) != L7_SUCCESS) || 
      (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, strAuthListName, &i) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbAPLNonConfiguredUserSet(unit, L7_USER_MGR_COMPONENT_ID, strAuthListName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgAuthLoginAsSignError);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config User Manager General Information
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
* @cmdsyntax  users login <user> <listname>
*
*
* @cmdhelp
*
* @cmddescript  assigns authentication login list to specific user
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersLogin(EwsContext ewsContext,
                                  uintf argc,
                                  const L7_char8 * * argv,
                                  uintf index)
{
  L7_uint32 argUser = 1;
  L7_uint32 argList = 2;
  L7_uint32 unit;
  L7_char8 strAuthListName[L7_MAX_APL_NAME_SIZE + 1];
  L7_char8 strUserName[L7_LOGIN_SIZE];
  L7_uint32 i;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgUsrsLogin);
  }

  if (strlen(argv[index+argUser]) >= sizeof(strUserName))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_InvalidUsrName, L7_LOGIN_SIZE - 1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  OSAPI_STRNCPY_SAFE(strUserName, argv[index+argUser]);
  if (usmDbUserLoginIndexGet(unit, strUserName, &i) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_UsrName);
  }

  if (strlen(argv[index+argList]) >= sizeof(strAuthListName))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidLoginListLen, L7_MAX_APL_NAME_SIZE);
  }

  OSAPI_STRNCPY_SAFE(strAuthListName, argv[index+argList]);
  if ((cliIsAlphaNum(strAuthListName) != L7_SUCCESS) || 
      (usmDbAuthenticationListIndexGetByName(ACCESS_LINE_CTS, ACCESS_LEVEL_LOGIN, strAuthListName, &i) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbAPLUserSet(unit, strUserName, L7_USER_MGR_COMPONENT_ID, strAuthListName) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgAuthLoginUsrAsSignError);

    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#if 0
/*********************************************************************
*
* @purpose  change an existing user password
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
* @notes add functionality, currently has NONE,
* @notes also make passwords not case sensitive
*
* @cmdsyntax  users passwd <username>
*
* @cmdhelp Change an existing user's password.
*
* @cmddescript
*The password can be between the configured min and 64 alphanumeric characters and is
*   case sensitive. A blank password indicates no password. The default
*   value is blank.
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersPasswd(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 tempInt = 0;
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tempPw[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uint32 unit;
  L7_uint32 encryptProtocol;
  L7_uint32 authProtocol;
  L7_char8 * scriptEmptyString = "\" \"";
  L7_ushort16 minLength = L7_MIN_PASSWORD_SIZE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* this command is not valid in a configuration script,
     just fail if we are validating a script or running a script */
  if ((ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT) ||
      (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    return cliPrompt(ewsContext);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  cliSyntaxTop(ewsContext);

  if (cliWebLoginIndexGet((L7_char8 *)argv[index+1], &tempInt) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliGetStringInputID() == CLI_INPUT_EMPTY)
    {
      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_UsrsPasswd, argv[index+1]);
      cliAlternateCommandSet(buf);
      cliSetStringInputID(1, ewsContext, argv);
      cliSetStringPassword();
      return pStrInfo_base_OldPasswd;
    }

    if (cliGetStringInputID() == 1)
    {
      memset(buf, 0, sizeof(buf));
      if (cliWebLoginUserPasswordEncryptedGet(tempInt, buf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
      if (usmDbPasswordIsValid(unit, buf, cliGetStringInput()) != 0)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_IncorrectPasswd);
      }
      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_UsrsPasswd, argv[index+1]);
      cliAlternateCommandSet(buf);
      cliSetStringInputID(2, ewsContext, argv);
      cliSetStringPassword();
      return pStrInfo_base_NewPasswd;

    }

    if (cliGetStringInputID() == 2)
    {
      OSAPI_STRNCPY_SAFE(tempPw, cliGetStringInput());

      /* When run from script, treat " " as null string */
      if(ewsContext->configScriptStateFlag==L7_CONFIG_SCRIPT_RUNNING)
      {
        if(strcmp(tempPw, scriptEmptyString) == 0)
        {
          OSAPI_STRNCPY_SAFE(tempPw, pStrInfo_common_EmptyString);
        }
      }

      usmDbMinPassLengthGet(&minLength);
      /* checking the length of  the password */
      if (usmDbPasswdLengthValid (unit, tempPw, L7_FALSE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_CfgPasswdLenError,
                                minLength, L7_PASSWORD_SIZE-1 );
      }
      /* Restricting new passwd not to contain quotes*/
      if(strchr(tempPw,'"'))
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_PasswdSyntax);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt(ewsContext, buf);
      }

      if (usmDbStringPrintableCheck(tempPw) != L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_PasswdMustBePrintAbleChars);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt(ewsContext, buf);
      }

      /* New password must at least meet minimum length requirements for SNMP if
       * authentication or encryption is set.
       */
      if (strlen(tempPw) < L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN)
      {
        if ( cliWebLoginUserAuthenticationGet(tempInt, &authProtocol) != L7_SUCCESS )
        {
          /* Generic failure */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
        }

        if (authProtocol != L7_SNMP_USER_AUTH_PROTO_NONE)
        {
          /* "\r\nError! Since authentication is enabled, password must be %u characters in length." */
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrErr_base_PasswdTooShort,  pStrInfo_common_Auth_1, L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN);
        }
      }

      if (strlen(tempPw) < L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN)
      {
        if ( cliWebLoginUserEncryptionGet(tempInt, &encryptProtocol) != L7_SUCCESS )
        {
          /* Generic failure */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
        }

        if (encryptProtocol != L7_SNMP_USER_PRIV_PROTO_NONE)
        {
          /* "\r\nError! Since encryption is enabled, password must be %u characters in length." */
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrErr_base_PasswdTooShort,  pStrInfo_base_Encryption, L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN);
        }
      }

      OSAPI_STRNCPY_SAFE(cliCommon[cliUtil.handleNum].password, tempPw);
      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_UsrsPasswd, argv[index+1]);
      cliAlternateCommandSet(buf);
      cliSetStringInputID(3, ewsContext, argv);
      cliSetStringPassword();
      return pStrInfo_base_ConfirmPasswd_1;
    }
    if (cliGetStringInputID() == 3)
    {
      OSAPI_STRNCPY_SAFE(tempPw, cliGetStringInput());

      /* When run from script, treat " " as null string */
      if(ewsContext->configScriptStateFlag==L7_CONFIG_SCRIPT_RUNNING)
      {
        if(strcmp(tempPw, scriptEmptyString) == 0)
        {
          OSAPI_STRNCPY_SAFE(tempPw, pStrInfo_common_EmptyString);
        }
      }

      if (strcmp(cliCommon[cliUtil.handleNum].password, tempPw) == 0)
      {
        rc = cliWebLoginUserPasswordSet(tempInt, tempPw, L7_FALSE);

        if (rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetUsrPasswdPleaseAvoidReusingOldPasswds);
        }
        else if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
        }
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_PasswdChgd);
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_PasswdNotMatched);
        cliSyntaxBottom(ewsContext);
      }
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  } /* end of if block for changing user's password*/
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( cliWebLoginUserAuthenticationGet(tempInt, &authProtocol) != L7_SUCCESS )
    {
      /* Generic failure */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
    }

    if (authProtocol != L7_SNMP_USER_AUTH_PROTO_NONE)
    {
      /* "\r\nError! Since authentication is enabled, password can not be reset." */
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrErr_base_NoPasswdReset,  pStrInfo_common_Auth_1);
    }
    if ( cliWebLoginUserEncryptionGet(tempInt, &encryptProtocol) != L7_SUCCESS )
    {
      /* Generic failure */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
    }

    if (encryptProtocol != L7_SNMP_USER_PRIV_PROTO_NONE)
    {
      /* "\r\nError! Since encryption is enabled, password can not be reset." */
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_NoPasswdReset,  pStrInfo_base_Encryption);
    }

    if (cliGetStringInputID() == CLI_INPUT_EMPTY)
    {
      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_base_NoUsrsPasswd, argv[index+1]);
      cliAlternateCommandSet(buf);
      cliSetStringInputID(1, ewsContext, argv);
      cliSetStringPassword();
      return pStrInfo_base_OldPasswd;
    }

    if (cliGetStringInputID() == 1)
    {
      memset(buf, 0, sizeof(buf));
      if (cliWebLoginUserPasswordEncryptedGet(tempInt, buf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
      if (usmDbPasswordIsValid(unit, buf, cliGetStringInput()) != 0)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_IncorrectPasswd);
      }
      else
      {
        rc = cliWebLoginUserPasswordSet(tempInt, pStrInfo_common_EmptyString, L7_FALSE);
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetUsrPasswdPleaseAvoidReusingOldPasswds);
        }
        else if (rc != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_PasswdReset);
        }
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
  }
  return cliPrompt(ewsContext);

}
#endif
/*********************************************************************
*
* @purpose  change an existing user password
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
* @notes add functionality, currently has NONE,
* @notes also make passwords not case sensitive
*
* @cmdsyntax  users passwd <username>
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
const L7_char8 *commandPassword(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 tempInt = 0;
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH * 2];
  L7_char8 password[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 username[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 encryptProtocol;
  L7_uint32 authProtocol;
  L7_ushort16   minPassLen;
  L7_int32 argUsername = -1;
  L7_int32 argPassword = 1;
  L7_int32 argLevel = 3;
  L7_int32 accessLevel = FD_USER_MGR_GUEST_ACCESS_LEVEL;
  L7_BOOL levelFlag = L7_FALSE;
  L7_BOOL encryptedFlag = L7_FALSE;
  L7_BOOL userExists = L7_FALSE;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg > 4 || numArg < 1)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGPASSWDUSER);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpySafe(username, argv[index+argUsername], sizeof(username));
    osapiStrncpySafe(password, argv[index+argPassword], sizeof(password));

    if (numArg > 1)
    {
      if (usmDbStringCaseInsensitiveCompare(argv[index+argLevel-1], "level") == L7_SUCCESS)
      {
        if (cliConvertTo32BitUnsignedInteger(argv[index+argLevel], &accessLevel) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGPASSWDUSER);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        levelFlag = L7_TRUE;
      }
    }

    if (numArg == 4 || numArg == 2) /* check for encrypted */
    {
      if ((levelFlag == L7_TRUE) &&
          (usmDbStringCaseInsensitiveCompare(argv[index+4], "encrypted") == L7_SUCCESS))
        encryptedFlag = L7_TRUE;
      else if (usmDbStringCaseInsensitiveCompare(argv[index+2], "encrypted") == L7_SUCCESS)
        encryptedFlag = L7_TRUE;
    }

    if (usmDbLoginUserIndexGet(unit, username, &tempInt) == L7_SUCCESS)
    {
      userExists = L7_TRUE;
    }
    else if (usmDbLoginUserIndexNextAvailableGet(&tempInt) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, CLI_TOOMANYUSERS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* check the password validity */
    {
      /* before creating/modifying the user, we must validate the password */
      /* Restricting new passwd not to contain quotes*/
      if (strchr(password, '"'))
      {
        ewsTelnetWrite(ewsContext, CLI_PASSWDSYNTAX);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (usmDbStringPrintableCheck(password) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, CLI_PASSWDMUSTBEPRINTABLECHARACTERS);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      /* New password must be MinLength */
      rc = usmDbPasswdLengthValid(unit, password, encryptedFlag);
      if (rc == L7_FAILURE)
      {
        (void)usmDbMinPassLengthGet(&minPassLen);
        sprintf(buf, CLI_PASSWDLENRANGESET, minPassLen, L7_PASSWORD_SIZE-1);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    /* if the user doesn't exist, create it */
    if (userExists != L7_TRUE)
    {
      /* validate username input */
      /* create the user */
      if (strlen(username) >= L7_LOGIN_SIZE)
      {
        sprintf(buf, CLISYNTAX_INVALIDUSERNAME, (L7_LOGIN_SIZE-1));
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      else if (usmDbStringAlphaNumericCheck(username) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, CLI_USERNAMESYNTAX);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        rc = cliWebLoginUserNameSet(tempInt, username);
        rc = cliWebLoginUserStatusSet(tempInt, L7_ENABLE);
        rc = cliWebLoginUserBlockStatusSet(tempInt, L7_FALSE);
      }
    }

    /* user now exists, set up other user data */
    rc = cliWebLoginUserNameGet(tempInt, username);

    /* New password must at least meet minimum length requirements for SNMP if
     * authentication or encryption is set.
     */
    if (strlen(password) < L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN)
    {
      if (cliWebLoginUserAuthenticationGet(tempInt, &authProtocol) != L7_SUCCESS)
      {
        /* Generic failure */
        ewsTelnetWrite(ewsContext, CLI_PASSWDNOTSET);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (authProtocol != L7_SNMP_USER_AUTH_PROTO_NONE)
      {
        /* "\r\nError! Since authentication is enabled, password must be %u characters in length." */
        sprintf(buf, 
                CLISYNTAX_PASSWORD_TOO_SHORT, 
                CLISYNTAXAUTHENTICATION, 
                L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    if (strlen(password) < L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN)
    {
      if (cliWebLoginUserEncryptionGet(tempInt, &encryptProtocol) != L7_SUCCESS)
      {
        /* Generic failure */
        ewsTelnetWrite(ewsContext, CLI_PASSWDNOTSET);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (encryptProtocol != L7_SNMP_USER_PRIV_PROTO_NONE)
      {
        /* "\r\nError! Since encryption is enabled, password must be %u characters in length." */
        sprintf(buf, CLISYNTAX_PASSWORD_TOO_SHORT, CLISYNTAXENCRYPTION, L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = cliWebLoginUserPasswordSet(tempInt, password, encryptedFlag);
      if (rc == L7_ERROR)
      {
        ewsTelnetWrite(ewsContext, CLI_PASSWDALREADYPRESENT);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    if (levelFlag == L7_TRUE)
    {
      if (accessLevel == FD_USER_MGR_GUEST_ACCESS_LEVEL
          || accessLevel == FD_USER_MGR_ADMIN_ACCESS_LEVEL
          || accessLevel == FD_USER_MGR_BLCKD_ACCESS_LEVEL)
      {
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
          usmDbUserAccessLevelSet(0, tempInt, accessLevel);
      }
      else
      {
        ewsTelnetWrite(ewsContext, CLISYNTAX_INVALIDACCESSLEVEL);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (accessLevel != FD_USER_MGR_BLCKD_ACCESS_LEVEL)
      {

        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          cliWebLoginUserBlockStatusSet(tempInt, L7_FALSE);
          usmDbUserUnLock(0, tempInt);
        }
      }
    }
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);

  } /* end of if block for changing user's password*/
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {           
      /*check to see if anything other than 1 argument was passed */
      ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGPASSWDUSER_NO);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(username) >= sizeof(buf))
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGPASSWDUSER_NO);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    else
    {
      osapiStrncpySafe(username, argv[index+argUsername], sizeof(username));
      osapiStrncpySafe(password, argv[index+argPassword], sizeof(password));
      /* if correct input, execute this section of code */
      if (usmDbLoginUserIndexGet(unit, username, &tempInt) == L7_SUCCESS)
      {
        if (cliWebLoginUserAuthenticationGet(tempInt, &authProtocol) != L7_SUCCESS)
        {
          /* Generic failure */
          ewsTelnetWrite(ewsContext, CLI_PASSWDNOTSET);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }

        if (authProtocol != L7_SNMP_USER_AUTH_PROTO_NONE)
        {
          /* "\r\nError! Since authentication is enabled, password can not be reset." */
          sprintf(buf, CLISYNTAX_NO_PASSWORD_RESET, CLISYNTAXAUTHENTICATION);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        if (cliWebLoginUserEncryptionGet(tempInt, &encryptProtocol) != L7_SUCCESS)
        {
          /* Generic failure */
          ewsTelnetWrite(ewsContext, CLI_PASSWDNOTSET);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }

        if (encryptProtocol != L7_SNMP_USER_PRIV_PROTO_NONE)
        {
          /* "\r\nError! Since encryption is enabled, password can not be reset." */
          sprintf(buf, CLISYNTAX_NO_PASSWORD_RESET, CLISYNTAXENCRYPTION);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }

        memset(buf, 0, sizeof(buf));
        rc = cliWebLoginUserPasswordGet(tempInt, buf);
        (void)usmDbMinPassLengthGet(&minPassLen);
        if (minPassLen != 0)
        {
          sprintf(buf, CLI_PASSWDLENRANGESET, minPassLen, L7_PASSWORD_SIZE-1);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        else
        {  /* clear the password */
          rc = cliWebLoginUserPasswordSet(tempInt, CLIEMPTYSTRING, L7_FALSE);
          ewsTelnetWrite(ewsContext, CLI_PASSWDRESET);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    } /* end of else */
    ewsTelnetWrite(ewsContext, CLI_USERNOTEXISTS);
    cliSyntaxBottom(ewsContext);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}


/*********************************************************************
*
* @purpose  delete an existing user password
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
* @notes add functionality, currently has NONE,
* @notes also make passwords not case sensitive
*
* @cmdsyntax  [no] username <username> [nopassword]
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
const L7_char8 *commandUserNopassword(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 tempInt = 0;
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 username[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 authProtocol;
  L7_int32 argUsername = -1;
  L7_int32 argLevel = 2;
  L7_int32 accessLevel = FD_USER_MGR_GUEST_ACCESS_LEVEL;
  L7_BOOL levelFlag = L7_FALSE;
  L7_BOOL userExists = L7_FALSE;
  L7_ushort16 minPassLen;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  usmDbMinPassLengthGet(&minPassLen);
  if (minPassLen != 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_PWDMGMT_NOPASSWORD_ERR);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg > 2 || numArg < 0)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGPASSWDUSER);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpySafe(username, argv[index+argUsername], sizeof(username));

    if (numArg > 1)
    {
      if (usmDbStringCaseInsensitiveCompare(argv[index+argLevel-1], "level") == L7_SUCCESS)
      {
        if (convertTo32BitUnsignedInteger(argv[index+argLevel],&accessLevel) != L7_SUCCESS)
        {
          ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGPASSWDUSER);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        levelFlag = L7_TRUE;
      }
    }

    if (usmDbLoginUserIndexGet(unit, username, &tempInt) == L7_SUCCESS)
    {
      userExists = L7_TRUE;
    }
    else if (usmDbLoginUserIndexNextAvailableGet(&tempInt) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLI_TOOMANYUSERS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* if the user doesn't exist, create it */
    if (userExists != L7_TRUE)
    {
      /* validate username input */
      /* create the user */
      if (strlen(username) >= L7_LOGIN_SIZE)
      {
        osapiSnprintf(buf, sizeof(buf), CLISYNTAX_INVALIDUSERNAME, (L7_LOGIN_SIZE-1));
        ewsTelnetWrite( ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      else if (usmDbStringAlphaNumericCheck(username) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, CLI_USERNAMESYNTAX);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        rc = cliWebLoginUserNameSet(tempInt, username);
        rc = cliWebLoginUserStatusSet(tempInt, L7_ENABLE);
        rc = cliWebLoginUserBlockStatusSet (tempInt, L7_FALSE);
      }
    }

    /* user now exists, set up other user data */
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = cliWebLoginUserPasswordSet(tempInt, CLIEMPTYSTRING, L7_FALSE);
      if (rc == L7_ERROR)
      {
        ewsTelnetWrite( ewsContext, CLI_PASSWDALREADYPRESENT);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    if (levelFlag == L7_TRUE)
    {
      if (accessLevel == FD_USER_MGR_GUEST_ACCESS_LEVEL
          || accessLevel == FD_USER_MGR_ADMIN_ACCESS_LEVEL
          || accessLevel == FD_USER_MGR_BLCKD_ACCESS_LEVEL)
      {
        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
          usmDbUserAccessLevelSet (0, tempInt, accessLevel);
      }
      else
      {
        ewsTelnetWrite( ewsContext, CLISYNTAX_INVALIDACCESSLEVEL);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (accessLevel != FD_USER_MGR_BLCKD_ACCESS_LEVEL)
      {

        if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
        {
          cliWebLoginUserBlockStatusSet (tempInt, L7_FALSE);
          usmDbUserUnLock(0, tempInt);
        }
      }
    }
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);

  } /* end of if block for changing user's password*/
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {                                                                       /*check to see if anything other than 1 argument was passed */
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGPASSWDUSER_NO);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(username) >= sizeof(buf))
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGPASSWDUSER_NO);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    else
    {
      osapiStrncpySafe(username, argv[index+argUsername], sizeof(username));
      /* if correct input, execute this section of code */
      if (usmDbLoginUserIndexGet(unit, username, &tempInt) == L7_SUCCESS)
      {
        if (cliWebLoginUserAuthenticationGet(tempInt, &authProtocol) != L7_SUCCESS)
        {
          /* Generic failure */
          ewsTelnetWrite( ewsContext, CLI_PASSWDNOTSET);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }

        if (authProtocol != L7_SNMP_USER_AUTH_PROTO_NONE)
        {
          /* "\r\nError! Since authentication is enabled, password can not be reset." */
          osapiSnprintf(buf, sizeof(buf),  CLISYNTAX_NO_PASSWORD_RESET,  CLISYNTAXAUTHENTICATION);
          ewsTelnetWrite(ewsContext, buf);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        rc = cliWebLoginUserPasswordSet(tempInt, CLIEMPTYSTRING, L7_FALSE);
        ewsTelnetWrite( ewsContext, CLI_PASSWDRESET);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    } /* end of else */
    ewsTelnetWrite( ewsContext, CLI_USERNOTEXISTS);
    cliSyntaxBottom(ewsContext);
  }
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config password Manager parameters
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
* @cmdsyntax  users <username> unlock
* @noform     not supported
*
* @cmdhelp
*
* @cmddescript  password lockout configuration.when a lockout count is
*               configured,then the user that is logging is must enter
*               the correct password with in the count.
* @end
*
*********************************************************************/
const L7_char8 *commandUserUnlock(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 tempInt = 0;
  L7_RC_t rc;
  L7_char8 username[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_int32 argUsername = -1;
  L7_BOOL userExists = L7_FALSE;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg > 0)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGUNLOCKUSER);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpySafe(username, argv[index+argUsername], sizeof(username));

    if (usmDbLoginUserIndexGet(unit, username, &tempInt) == L7_SUCCESS)
    {
      userExists = L7_TRUE;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    /* user now exists, set up other user data */
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbUserUnLock(unit, tempInt);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_UserUnlockCantBeSet);
      }
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_USER_MGR_COMPONENT_ID,
            "User %s is unlocked", username);
    }

    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);

  }
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#if 0 
/* Deprecated */
/*********************************************************************
*
* @purpose  change an existing user password in encrypted form
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
* @notes add functionality, currently has NONE,
* @notes also make passwords not case sensitive
*
* @cmdsyntax  users passwd <username> encrypted <passwd>
*             Normal command only
*
* @cmdhelp Change an existing user's password.
*
* @cmddescript
*   The encrypted password must be 128 hex characters. A blank password
*   indicates no password. The default
*   value is blank, but value is possible only after a clear config.
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersPasswdEncrypted(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 tempInt = 0;
  L7_char8 tempPw[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uint32 encryptProtocol;
  L7_uint32 authProtocol;
  L7_char8 * scriptEmptyString = "\" \"";

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  cliSyntaxTop(ewsContext);

  OSAPI_STRNCPY_SAFE(tempPw, argv[index+1]);

  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* if correct input, execute this section of code */
    if (cliWebLoginIndexGet((L7_char8 *)argv[index-1], &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }
  }

  /* When run from script, treat " " as null string */
  if(ewsContext->configScriptStateFlag==L7_CONFIG_SCRIPT_RUNNING)
  {
    if(strcmp(tempPw, scriptEmptyString) == 0)
    {
      OSAPI_STRNCPY_SAFE(tempPw, pStrInfo_common_EmptyString);
    }
  }

  /*
   * hex check the password string
   */
  if (strlen(tempPw) != L7_ENCRYPTED_PASSWORD_SIZE-1 || usmDbStringHexadecimalCheck(tempPw) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgEncrypt_2);
  }
  else if (strchr(tempPw,'"'))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_PasswdSyntax);
  }

  /* New password must at least meet minimum length requirements for SNMP if
   * authentication or encryption is set.
   */

  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (strlen(tempPw) < L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN)
    {
      if ( cliWebLoginUserAuthenticationGet(tempInt, &authProtocol) != L7_SUCCESS )
      {
        /* Generic failure */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
      }

      if (authProtocol != L7_SNMP_USER_AUTH_PROTO_NONE)
      {
        /* "\r\nError! Since authentication is enabled, password must be %u characters in length." */
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrErr_base_PasswdTooShort,  pStrInfo_common_Auth_1, L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN);
      }
    }

    if (strlen(tempPw) < L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN)
    {
      if ( cliWebLoginUserEncryptionGet(tempInt, &encryptProtocol) != L7_SUCCESS )
      {
        /* Generic failure */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
      }

      if (encryptProtocol != L7_SNMP_USER_PRIV_PROTO_NONE)
      {
        /* "\r\nError! Since encryption is enabled, password must be %u characters in length." */
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrErr_base_PasswdTooShort,  pStrInfo_base_Encryption, L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN);
      }
    }
  }

  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (cliWebLoginUserPasswordSet(tempInt, tempPw, L7_TRUE) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_PasswdNotSet);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
#endif

/*********************************************************************
*
* @purpose  add a user if all slots are not full or remove an existing user account
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
* @cmdsyntax users name <username>
*
* @cmdhelp Add a new user (Max 8 characters - alphanumeric, dash, or underscore).
*
* @cmddescript
*   User name can be up to eight alphanumeric characters and is not
*   case sensitive. Up to six user names (accounts) can be defined;
*   one with read/write access mode and five with read only access mode.
*
* @end
*
*********************************************************************/

const L7_char8 *commandUserNameOld(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 tempInt = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  OSAPI_STRNCPY_SAFE(name, argv[index+1]);

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(name) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_base_InvalidUsrName, (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(name) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_UsrNameSyntax);
    }

    /* checking for default user.preventing creation of default user */
    if (usmDbStringCaseInsensitiveCompare(argv[index+1],
                                          L7_USER_MGR_DEFAULT_USER_STRING) == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNonCfgured);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (cliWebLoginIndexGet(name, &tempInt) == L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrExistsAlready);
      }

      if (cliWebLoginUserAvailableIndexGet(&tempInt) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_TooManyUsrs);
      }
      if (cliWebLoginUserNameSet(tempInt, name) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotAdded);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (strlen(argv[index+1]) >= sizeof(buf))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgDelUsr);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (cliWebLoginIndexGet(name, &tempInt) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
      }

      if(tempInt == 0) /* can't delete admin */
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrInfo_base_UsrAdminNotDeld);
      }
      if (cliWebLoginUserDelete( tempInt) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotDeld);
      }
    }
  } /* end of code block for no option */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  add a user if all slots are not full or remove an existing user account
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
* @cmdsyntax users name <username>
*
* @cmdhelp Add a new user (Max 8 characters - alphanumeric, dash, or underscore).
*
* @cmddescript
*   User name can be up to eight alphanumeric characters and is not
*   case sensitive. Up to six user names (accounts) can be defined;
*   one with read/write access mode and five with read only access mode.
*
* @end
*
*********************************************************************/
const L7_char8 *commandUserName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 tempInt = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH * 2];
  L7_char8 username[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;        /* New variable Added */
  L7_uint32 argUsername = 1;
  L7_uint32 unit;
  L7_ushort16 minPassLen;
  EwaSession session;
  L7_uint32 i;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  numArg = cliNumFunctionArgsGet();
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
     /*check to see if anything other than 1 argument was passed */
      ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGADDUSER);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpySafe(username, argv[index+argUsername], sizeof(username));
    (void)usmDbMinPassLengthGet(&minPassLen);
    if (minPassLen != 0)
    {
      sprintf(buf, CLI_PASSWDLENRANGESET, minPassLen, L7_PASSWORD_SIZE-1);
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    else
    {
      if (strlen(username) >= L7_LOGIN_SIZE)
      {
        sprintf(buf, CLISYNTAX_INVALIDUSERNAME, (L7_LOGIN_SIZE-1));
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      else if (usmDbStringAlphaNumericCheck(username) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, CLI_USERNAMESYNTAX);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbLoginUserIndexGet(unit, username, &tempInt) == L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, CLI_USEREXISTSALREADY);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        if (usmDbLoginUserIndexNextAvailableGet(&tempInt) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, CLI_TOOMANYUSERS);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }

        rc = cliWebLoginUserNameSet(tempInt, username);
        rc = cliWebLoginUserStatusSet(tempInt, L7_ENABLE);
        rc = cliWebLoginUserBlockStatusSet(tempInt, L7_FALSE);
      }
    }/* end else */
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 1)
    {   
      /*check to see if anything other than 1 argument was passed */
      ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGDELETEUSER);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (strlen(argv[index+argUsername]) >= sizeof(username))
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGDELETEUSER);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    else
    {
      osapiStrncpySafe(username, argv[index+argUsername], sizeof(username));
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbLoginUserIndexGet(unit, username, &tempInt) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, CLI_USERNOTEXISTS);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }

        if (cliWebLoginUserDelete(tempInt) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, CLI_USERNOTDELETED);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        else
        { /* deletion succeeded, now remove all telnet/ssh sessions by this username */
          memset(buf, 0, sizeof(buf));
          for (tempInt = 0; tempInt < FD_CLI_DEFAULT_MAX_CONNECTIONS; tempInt++)
          {
            if ((usmDbLoginSessionUserGet(unit, tempInt, buf) == L7_SUCCESS) &&
                (cliNoCaseCompare(buf, username) == L7_TRUE))
            {
              usmDbLoginSessionResetConnectionSet(unit, tempInt);
            }
          }
          for (i = 0; i < L7_MAX_WEB_LOGINS; i++)
          {
            if (EwaSessionGet(i, &session) != L7_SUCCESS)
            {
              continue;
            }
            if (session.active == L7_TRUE)
            {
              if (strcmp(session.uname, username) == 0)
              {
                EwaSessionRemove(session.sid);
              }
            }
          }
        }
      }
    }                                                                       /* end else */
  } /* end of code block for no option */

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}


/*********************************************************************
*
* @purpose  configure the snmpv3 access mode for a user
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
* @cmdsyntax   users snmpv3 accessmode <username> {readonly|readwrite}
*
* @cmdhelp
*
* @cmddescript
*   User name must be an existing user name (created with configUsersAdd).
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersSnmpv3AccessMode(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 userName[L7_LOGIN_SIZE];
  L7_char8 strAccessMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 accessMode = 0;
  L7_uint32 tempInt = 0;
  L7_uint32 numArg;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if ( numArg != 2 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgUsrSnmpV3AccessMode);
    }

    if (strlen(argv[index+1]) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_UsrNameMaxChars, (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameMustBeAlnumChars);
    }

    OSAPI_STRNCPY_SAFE( userName, argv[index+1]);

    if (cliWebLoginIndexGet(userName, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    /* convert accessmode parm to lower case */
    if (strlen(argv[index+2]) <= sizeof(strAccessMode))
    {
      OSAPI_STRNCPY_SAFE(strAccessMode, argv[index+2]);
      cliConvertToLowerCase(strAccessMode);
    }

    if (strcmp(strAccessMode,pStrInfo_base_Readonly) == 0 )
    {
      accessMode = L7_SNMP_USER_ACCESS_LEVEL_READ_ONLY;
    }
    else if (strcmp(strAccessMode,pStrInfo_base_Readwrite) == 0 )
    {
      accessMode = L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE;
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgUsrSnmpV3AccessModeNo);
    }
    if (strlen(argv[index+1]) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_UsrNameMaxChars, (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameMustBeAlnumChars);
    }

    OSAPI_STRNCPY_SAFE( userName, argv[index+1]);

    if (cliWebLoginIndexGet(userName, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    /* By default, the accessmode values are readwrite for admin user; readonly for all other users */
    if (strcmp(userName,pStrInfo_base_LoginUsrId) == 0)
    {
      accessMode = L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE;
    }
    else
    {
      accessMode = L7_SNMP_USER_ACCESS_LEVEL_READ_ONLY;
    }
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Pass to cliWeb */
    if ( cliWebLoginUserSnmpv3AccessModeSet(tempInt, accessMode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_SettingSnmpv3AccessMode);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  configure the authentication protocol and key for a user
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
* @cmdsyntax  users snmpv3 authentication <username> {none | md5 | sha}
*
* @cmdhelp
*
* @cmddescript
*   User name must be an existing user name (created with configUsersAdd).
*   If an authentication protocol is specified and a key is not provided,
*   a prompt will be displayed requesting the key
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersSnmpv3Authentication(EwsContext ewsContext, L7_uint32 argc,
                                                 const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 userName[L7_LOGIN_SIZE];
  L7_char8 strAuthProt[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 authProt = 0, encryptProt;
  L7_uint32 tempInt = 0;
  L7_char8 password[L7_PASSWORD_SIZE];
  L7_uint32 numArg;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( numArg != 2 )
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgUsrAuth);
    }
  } 
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_base_CfgUsrAuthNo);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+1]) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_UsrNameMaxChars, (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameMustBeAlnumChars);
    }

    OSAPI_STRNCPY_SAFE( userName, argv[index+1]);

    if (cliWebLoginIndexGet(userName, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    /* convert authentication protocol parm to lower case */
    if (strlen(argv[index+2]) <= L7_CLI_MAX_STRING_LENGTH)
    {
      OSAPI_STRNCPY_SAFE(strAuthProt, argv[index+2]);
      cliConvertToLowerCase(strAuthProt);
    }

    if (strcmp(strAuthProt,pStrInfo_common_None_3) == 0 )
    {
      authProt = L7_SNMP_USER_AUTH_PROTO_NONE;

      /* Verify encryption is not set for this user */
      if ( cliWebLoginUserEncryptionGet(tempInt, &encryptProt) != L7_SUCCESS )
      {
        /* Generic failure */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetUsrAuthParams);
      }

      if (encryptProt != L7_SNMP_USER_PRIV_PROTO_NONE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_AuthIsRequiredForUsrSinceEncryptionIsEnbld);
      }
    }
    else if (strcmp(strAuthProt,pStrInfo_base_Md5_1) == 0 )
    {
      if (cliWebLoginUserPasswordGet(tempInt, password) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_GenericFailure);
      }
      if (strlen(password) < L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN)
      {
        /* Error! Cannot enable authentication when password is less then %u characters in length. */
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_PasswdTooShortForAuth, L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN);
      }
      authProt = L7_SNMP_USER_AUTH_PROTO_HMACMD5;
    }
    else if (strcmp(strAuthProt,pStrInfo_base_Sha_1) == 0 )
    {
      if (cliWebLoginUserPasswordGet(tempInt, password) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_GenericFailure);
      }
      if (strlen(password) < L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN)
      {
        /* Error! Cannot enable authentication when password is less then %u characters in length. */
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_PasswdTooShortForAuth, L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN);
      }
      authProt = L7_SNMP_USER_AUTH_PROTO_HMACSHA;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgUsrAuth);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (strlen(argv[index+1]) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_UsrNameMaxChars, (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameMustBeAlnumChars);
    }

    OSAPI_STRNCPY_SAFE( userName, argv[index+1]);

    if (cliWebLoginIndexGet(userName, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    /* The authentication protocol will be 'none' for no option of the command */
    authProt = L7_SNMP_USER_AUTH_PROTO_NONE;

    /* Verify encryption is not set for this user */
    if ( cliWebLoginUserEncryptionGet(tempInt, &encryptProt) != L7_SUCCESS )
    {
      /* Generic failure */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetUsrAuthParams);
    }

    if (encryptProt != L7_SNMP_USER_PRIV_PROTO_NONE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_AuthIsRequiredForUsrSinceEncryptionIsEnbld);
    }
  }
    /* Pass to cliWeb */
    if ( cliWebLoginUserAuthenticationSet(tempInt, authProt) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_SettingAuthVals);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  configure the encryption protocol and key for a user
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
* @cmdsyntax  users snmpv3 encryption <username> {none | des [key]}
*
* @cmdhelp
*
* @cmddescript
*   User name must be an existing user name (created with configUsersAdd).
*   If an encryption protocol is specified and a key is not provided,
*   a prompt will be displayed requesting the key
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersSnmpv3Encryption(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 userName[L7_LOGIN_SIZE];
  L7_char8 password[L7_PASSWORD_SIZE];
  L7_char8 strEncryptProt[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 encryptKey[L7_ENCRYPTION_KEY_SIZE];
  L7_char8 commandString[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 encryptProt =0, authProt;
  L7_uint32 tempInt = 0;
  L7_uint32 numArg;        /* New variable Added */

  /* this command is not valid in a configuration script,
     just fail if we are validating a script or running a script */
  if ((ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT) ||
      (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    return cliPrompt(ewsContext);
  }

  cliSyntaxTop(ewsContext);
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if ( numArg < 2 || numArg > 3 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgUsrEncrypt);
    }

    if (strlen(argv[index+1]) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_UsrNameMaxChars, (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameMustBeAlnumChars);
    }

    OSAPI_STRNCPY_SAFE( userName, argv[index+1]);

    if (cliWebLoginIndexGet(userName, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    /* convert encryption protocol parm to lower case */
    if (strlen(argv[index+2]) <= L7_CLI_MAX_STRING_LENGTH)
    {
      OSAPI_STRNCPY_SAFE(strEncryptProt, argv[index+2]);
      cliConvertToLowerCase(strEncryptProt);
    }

    /* Verify encryption protocol parm is valid */
    if (strcmp( strEncryptProt, pStrInfo_base_Des_1) /* "des" */ == 0 )
    {
      encryptProt = L7_SNMP_USER_PRIV_PROTO_DES;
    }
    else if ( strcmp( strEncryptProt, pStrInfo_common_None_3) /* "none" */ == 0 )
    {
      encryptProt = L7_SNMP_USER_PRIV_PROTO_NONE;
    }
    else
    {
      /* invalid encrypt prot */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgUsrEncrypt);
    }

    memset (encryptKey, 0, sizeof(encryptKey));
    /* If encryption other than none is used, password must be at least the minimum length. */
    if (encryptProt != L7_SNMP_USER_PRIV_PROTO_NONE)
    {
      if (cliWebLoginUserPasswordGet(tempInt, password) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailureSettingEncryption);
      }
      if (strlen(password) < L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN)
      {
        /* Error! Cannot enable encryption when password is less then %u characters in length. */
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_PasswdTooShortForEncryption, L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN);
      }
    }

    /* If encryption protocol is "des", validate authentication type
       since this is required to set encryption. */
    if (encryptProt == L7_SNMP_USER_PRIV_PROTO_DES)
    {
      if ( cliWebLoginUserAuthenticationGet(tempInt, &authProt) != L7_SUCCESS )
      {
        /* Generic failure */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetUsrEncryptionParams);
      }

      if (authProt == L7_SNMP_USER_AUTH_PROTO_NONE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_AuthMustBeEnbldForUsrBeforeEncryptionCanBeSet);
      }

      if (numArg == 2)
      {
        /* prompt for key */
        if (cliGetStringInputID() == CLI_INPUT_EMPTY)
        {
          osapiSnprintf(commandString, sizeof(commandString), "%s %s %s",
                        pStrInfo_base_UsrsSnmpv3Encryption,
                        argv[index+1],
                        argv[index+2]);
          cliAlternateCommandSet(commandString);
          cliSetStringInputID(1, ewsContext, argv);
          cliSetStringPassword();
          return pStrInfo_base_EnterEncryptionKey;
        }
        else if (cliGetStringInputID() == 1)
        {
          OSAPI_STRNCPY_SAFE(buf, cliGetStringInput());
          if (strlen(buf) >= sizeof(encryptKey) || strlen(buf) <L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN )
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrInfo_base_EncryptionKeyMustBeToCharsInLen, L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN, (L7_ENCRYPTION_KEY_SIZE-1));
          }
          OSAPI_STRNCPY_SAFE(encryptKey, buf);
        }
      }
      else
      { /* numArg == 3 */
        if (strlen(argv[index+3]) >= sizeof(encryptKey) || strlen(argv[index+3]) <L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN )
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrInfo_base_EncryptionKeyMustBeToCharsInLen, L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN, (L7_ENCRYPTION_KEY_SIZE-1));
        }
        OSAPI_STRNCPY_SAFE( encryptKey, argv[index+3]);
      }
    } /* end if des */
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgUsrEncryptNo);
    }

    if (strlen(argv[index+1]) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_UsrNameMaxChars, (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameMustBeAlnumChars);
    }

    OSAPI_STRNCPY_SAFE(userName, argv[index+1]);

    if (cliWebLoginIndexGet(userName, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    encryptProt = L7_SNMP_USER_PRIV_PROTO_NONE;

    memset (encryptKey, 0, sizeof(encryptKey)); /* The encrypt key is not specified with 'none' protocol */
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* Pass to cliWeb */
    if ( cliWebLoginUserEncryptionSet(tempInt, encryptProt, encryptKey) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_SettingEncryptionVals);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  configure the encryption protocol and key for a user
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
* @cmdsyntax  users snmpv3 encryption encrypted <username> des <key>
*
* @cmdhelp
*
* @cmddescript
*   User name must be an existing user name (created with configUsersAdd).
*
* @end
*
*********************************************************************/
const L7_char8 *commandUsersSnmpv3EcryptdKeyEncryption(EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 userName[L7_LOGIN_SIZE];
  L7_char8 password[L7_PASSWORD_SIZE];
  L7_char8 encryptKey[L7_ENCRYPTION_KEY_SIZE];
  L7_char8 tempPw[(L7_ENCRYPTION_KEY_SIZE*2) + 1];
  L7_uint32 encryptProt =0, authProt;
  L7_uint32 tempInt = 0;
  L7_uint32 numArg;        /* New variable Added */

  cliSyntaxTop(ewsContext);
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for Number of Arguments */
  if ( numArg != 3 )
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, 
                                          pStrErr_base_CfgUsrEncryptKey); 
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (strlen(argv[index+1]) >= L7_LOGIN_SIZE)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_base_UsrNameMaxChars, 
      (L7_LOGIN_SIZE-1));
    }
    else if (usmDbStringAlphaNumericCheck(argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNameMustBeAlnumChars);
    }

    OSAPI_STRNCPY_SAFE( userName, argv[index+1]);
    if (cliWebLoginIndexGet(userName, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }  

    encryptProt = L7_SNMP_USER_PRIV_PROTO_DES;
    memset (encryptKey, 0, sizeof(encryptKey));
    /* If encryption other than none is used, password must be at least the minimum length. */
    if (encryptProt != L7_SNMP_USER_PRIV_PROTO_NONE)
    {
      if (cliWebLoginUserPasswordGet(tempInt, password) != L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_FailureSettingEncryption);
      }
      if (strlen(password) < L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN)
      {
        /* Error! Cannot enable encryption when password is less then %u characters in length. */
        return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, pStrErr_common_Error,ewsContext, 
                                              pStrErr_base_PasswdTooShortForEncryption, L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN);
      }
    }

    /* If encryption protocol is "des", validate authentication type since this is required to set encryption. */
    if (encryptProt == L7_SNMP_USER_PRIV_PROTO_DES)
    {
      if ( cliWebLoginUserAuthenticationGet(tempInt, &authProt) != L7_SUCCESS )
      {
        /* Generic failure */
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              pStrInfo_base_SetUsrEncryptionParams);
      }
 
      if (authProt == L7_SNMP_USER_AUTH_PROTO_NONE)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error, ewsContext, 
                                              pStrInfo_base_AuthMustBeEnbldForUsrBeforeEncryptionCanBeSet);
      }

      OSAPI_STRNCPY_SAFE(tempPw, argv[index+3]);
      /* hex check the password string */
      if((strlen(tempPw) != (L7_ENCRYPTED_PASSWORD_SIZE-1)) || (usmDbStringHexadecimalCheck(tempPw) != L7_SUCCESS))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgEncrypt_2);
      }
 
      if(pwDecrypt(encryptKey, tempPw, L7_PASSWORD_SIZE-1) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgUsrEncryptedKey);
      } 

      if (strlen(encryptKey) < L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN )
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                              pStrInfo_base_EncryptionKeyMustBeToCharsInLen, 
                                              L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN, (L7_ENCRYPTION_KEY_SIZE-1));
      }           
    } /* end if des */
  
    /* Pass to cliWeb */
    if ( cliWebLoginUserEncryptionSet(tempInt, encryptProt, encryptKey) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_Error, ewsContext, 
                                            pStrInfo_base_SettingEncryptionVals);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  close an active telnet session
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
* @cmdsyntax disconnect {<sessionID>|all}
*
* @cmdhelp Close an active telnet session.
*
* @cmddescript
*   Used to close an active telnet session.
*
* @end
*
*********************************************************************/
const L7_char8 *commandDisconnect(EwsContext ewsContext, L7_uint32 argc,
                                  const L7_char8 * * argv, L7_uint32 index)
{
  L7_int32 sessionIndex = -1;
  L7_int32 type;
  L7_BOOL validSession = L7_NULL;
  L7_uint32 numArg;        /* New variable Added */

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_CfgLoginSessionClose, FD_CLI_WEB_DEFAULT_NUM_SESSIONS);
  }
  else
  {
    if (strcmp(argv[index+1], pStrInfo_common_All) == 0)
    {
      cliWebLoginSessionResetConnectionAllSet();
      EwaSessionRemoveAll(0);
    }
    else
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index+1], &sessionIndex)!=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_InvalidSessionId);
      }

      if (sessionIndex >= FD_CLI_DEFAULT_MAX_CONNECTIONS)
      {
        /* Pass the session index by subtracting it from CLI Sessions as because
         * indexing is maintained for web sessions Starting from zero
         */
        validSession = EwaSessionIsActive(sessionIndex - FD_CLI_DEFAULT_MAX_CONNECTIONS);
        if(validSession != L7_FALSE)
        {
          EwaSessionRemoveByIndex(sessionIndex-FD_CLI_DEFAULT_MAX_CONNECTIONS);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_InvalidSessionId);    
        }
      }
      else
      {
        cliWebLoginSessionValidEntry(sessionIndex, &validSession);
        cliWebLoginSessionTypeGet(sessionIndex, &type);
        if (type != L7_LOGIN_TYPE_SERIAL)
        {
          if (validSession == L7_TRUE)
          {
            cliWebLoginSessionResetConnectionSet(sessionIndex);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_InvalidSessionId);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_OnlyTelnetSshSessions);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#if 0
/*********************************************************************
*
* @purpose  change the priviledge user password
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
* @cmdsyntax  enable passwd
*
* @cmdhelp Change priviledge user passwd
*
* @cmddescript
*   The password should be between min-length and 64 alphanumeric characters and is
*   case sensitive. A blank password indicates no password. The default
*   value is blank.
*
* @end
*
*********************************************************************/
const L7_char8 *commandEnablePasswd(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 tempPw[L7_PASSWORD_SIZE];
  L7_uint32 unit;
  L7_char8 currentUser[L7_LOGIN_SIZE];
  L7_ushort16 minLength = L7_MIN_PASSWORD_SIZE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* this command is not valid in a configuration script,
     just fail if we are validating a script or running a script */
  if ((ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT) ||
      (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    return cliPrompt(ewsContext);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  cliSyntaxTop(ewsContext);

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliGetStringInputID() == CLI_INPUT_EMPTY)
    {
      usmDbLoginSessionUserGet(unit,cliLoginSessionIndexGet(), currentUser);

      if (cliSecurityLevelCheck(L7_LOGIN_ACCESS_READ_WRITE, currentUser) != L7_TRUE)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrErr_base_AuthorizedCheckedFailed);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }

      cliAlternateCommandSet(pStrInfo_base_EnblPasswdCmd);
      if( strcmp(pStrInfo_base_LoginUsrId, currentUser) != 0)
      {
        cliSetStringInputID(1, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_OldPasswd;
      }
      else
      {
        cliSetStringInputID(2, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_NewPasswd;
      }
    }

    if (cliGetStringInputID() == 1)
    {
      rc = usmDbEnablePasswordGet(FD_USER_MGR_ADMIN_ACCESS_LEVEL, buf);
      if( rc != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_InternalError);
        ewsSetTelnetPrompt(ewsContext, cliCommon[cliUtil.handleNum].prompt);
        return NULL;
      }

      if (usmDbPasswordIsValid(unit, buf, cliGetStringInput()) != 0)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_PasswdGiven);
      }
      else
      {
        cliAlternateCommandSet(pStrInfo_base_EnblPasswdCmd);
        cliSetStringInputID(2, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_NewPasswd;
      }
    }

    if (cliGetStringInputID() == 2)
    {
      OSAPI_STRNCPY_SAFE(tempPw, cliGetStringInput());

      usmDbMinPassLengthGet(&minLength);
      /* checking the length of  the password */
      if (usmDbPasswdLengthValid (unit, tempPw) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_CfgPasswdLenError,
                                minLength, L7_PASSWORD_SIZE-1 );
      }

      if (usmDbStringPrintableCheck(tempPw) != L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_PasswdMustBePrintAbleChars);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt(ewsContext, buf);
      }
      OSAPI_STRNCPY_SAFE(cliCommon[cliUtil.handleNum].password, tempPw);
      cliAlternateCommandSet(pStrInfo_base_EnblPasswdCmd);
      cliSetStringInputID(3, ewsContext, argv);
      cliSetStringPassword();
      return pStrInfo_base_ConfirmPasswd_1;
    }
    if (cliGetStringInputID() == 3)
    {
      OSAPI_STRNCPY_SAFE(tempPw, cliGetStringInput());
      if (strcmp(cliCommon[cliUtil.handleNum].password, tempPw) == 0)
      {
        rc = usmDbEnablePasswordSet(FD_USER_MGR_ADMIN_ACCESS_LEVEL, tempPw, L7_FALSE);
        if( rc ==L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_PasswdChgd);
        }
        else
        {
          ewsTelnetWrite( ewsContext, pStrInfo_base_InternalError);

        }
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_base_PasswdNotMatched);
        cliSyntaxBottom(ewsContext);
      }
    }
  }
  else /* 'no' command */
  {
    usmDbEnablePasswordRemove(FD_USER_MGR_ADMIN_ACCESS_LEVEL);
  }
  return cliPrompt(ewsContext);
}
#endif

/*********************************************************************
*
* @purpose   Config enable password per auth level
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
* @cmdsyntax  enable password [level <level>] <password> [encrypted]
* @noform     no enable password [level <level>]
*
* @cmdhelp
*
* @cmddescript sets the password for the enable mode, per auth level.
*              if no level given, level 15 is used.
*
* @end
*
*********************************************************************/
const L7_char8 *commandEnablePassword(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32   argPassword = 1;
  /*L7_uint32   argEncrypted = 2;*/
  /* unit id in stacking */
  L7_uint32   unit;
  /* integer to hold number of arguments in the command*/
  L7_uint32   numArgs;
  /* string to contain  password */
  L7_char8    strPassword[L7_PASSWORD_SIZE ];
  /* string buffer to intermediate strings*/
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32   level = L7_USER_MGR_MAX_ACCESSLEVEL;
  L7_ushort16   minLength = L7_MIN_PASSWORD_SIZE;
  L7_BOOL     encrypted = L7_FALSE;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* get the number of arguments in the command*/
  numArgs = cliNumFunctionArgsGet();
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWrite(ewsContext, CLIERROR_UNIT_ID);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /* check for the normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    usmDbMinPassLengthGet(&minLength);
    /*checking the number of arguments in normal command */
    if ( numArgs != 1 && numArgs != 2)
    {
      ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGENABLEPASSWORD);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (numArgs == 2)
    {
      if (usmDbStringCaseInsensitiveCompare(argv[index+argPassword+1], "encrypted") == L7_SUCCESS)
        encrypted = L7_TRUE;
      else
      {
        ewsTelnetWrite(ewsContext, CLISYNTAX_CONFIGENABLEPASSWORD);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    bzero(strPassword,sizeof(strPassword));
    osapiStrncpySafe(strPassword, argv[index+argPassword], sizeof(strPassword));
    /* checking the length of  the password */
    if (usmDbPasswdLengthValid (unit, strPassword, encrypted) != L7_SUCCESS)
    {
      bzero( buf, sizeof(buf));
      sprintf( buf, CLIERROR_CONFIGENABLEPASSWORDLENGTHERROR,
               minLength, L7_PASSWORD_SIZE-1 );
      ewsTelnetWrite( ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    /*
     * alpha numeric check the password string
     */
    if (usmDbStringPrintableCheck(strPassword) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext,
                     CLI_PASSWDMUSTBEPRINTABLECHARACTERS);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    else if (strchr(strPassword,'"'))
    {
      ewsTelnetWrite(ewsContext,
                     CLI_PASSWDSYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    /*
     * set the new password
     */
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbEnablePasswordSet(level, strPassword, encrypted) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, CLIERROR_CONFIGENABLEPASSWORD);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }/* end of assigning list*/
    }/* endof if(ewsContext->script....*/
  }/* end of normal command*/
  else if ( ewsContext->commType == CLI_NO_CMD)
  {
    /* checking for number of arguments in the no command */
    if ( numArgs != 0 )
    {
      ewsTelnetWrite(ewsContext,CLISYNTAX_CONFIGNOENABLEPASSWORD);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbEnablePasswordRemove(level) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext,CLIERROR_CONFIGENABLEPASSWORD );
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }/* end of assigning default list*/
    }/*endof if(ewsContext.....*/
  }/* end of no command*/

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose   Config enable password in encrypted form
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
* @cmdsyntax  enable password <password> [encrypted]
* @noform     no enable password
*
* @cmdhelp
*
* @cmddescript sets the password for the enable mode
*
* @end
*
*********************************************************************/
const L7_char8 *commandEnablePasswdEncrypted(EwsContext ewsContext,
                                             uintf argc,
                                             const L7_char8 * * argv,
                                             uintf index)
{
  /* string to contain  password */
  L7_char8 strPassword[L7_ENCRYPTED_PASSWORD_SIZE];
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  OSAPI_STRNCPY_SAFE(strPassword, argv[index+1]);

  /* check for the normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*
     * hex check the password string
     */
    if (strlen(strPassword) != L7_ENCRYPTED_PASSWORD_SIZE-1 || usmDbStringHexadecimalCheck(strPassword) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_CfgEncrypt_2);
    }
    else if (strchr(strPassword,'"'))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                               pStrErr_base_PasswdSyntax);
    }
    /*
     * set the new password
     */
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbEnablePasswordSet(FD_USER_MGR_ADMIN_ACCESS_LEVEL, strPassword, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_CfgEnblPasswd_1);
      }/* end of assigning list*/
    }/* endof if(ewsContext->script....*/
  }/* end of normal command*/
  else /* NO COMMAND */
  {
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      memset(strPassword, 0, sizeof(strPassword));
      if (usmDbEnablePasswordSet(FD_USER_MGR_ADMIN_ACCESS_LEVEL, strPassword, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_CfgEnblPasswd_1);
      }/* end of assigning list*/
    }/* endof if(ewsContext->script....*/
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose   Config password Manager parameters
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
* @cmdsyntax  passwords min-length length
* @noform     no passwords min-length
*
* @cmdhelp
*
* @cmddescript  set minimum length of password
*               administrator can only execute this command
*
* @end
*
*********************************************************************/
const L7_char8 *commandPasswordsMinlength(EwsContext ewsContext, uintf argc,
                                          const L7_char8 * * argv,
                                          uintf index)
{
  L7_uint32 unit;
  L7_uint32 pwdMinLength = FD_USER_MGR_PASSWD_MIN_LENGTH;
  L7_uint32 argMinLength = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /* normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argMinLength], &pwdMinLength) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    pwdMinLength = FD_USER_MGR_PASSWD_MIN_LENGTH;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbMinPasswdSet(unit, pwdMinLength) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_PasswdMinLenCantBeSet);
    }
  } /* end of if(ewsContext->scriptActionFlag...*/

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}/* end of the function */

/*********************************************************************
*
* @purpose   Config password Manager parameters
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
* @cmdsyntax  passwords min-length length
* @noform     no passwords min-length
*
* @cmdhelp
*
* @cmddescript  set the number of pervious passwords that shall be stored
*               only administrator can execute theis command
*
* @end
*
*********************************************************************/
const L7_char8 *commandPasswordsHistory(EwsContext ewsContext, uintf argc,
                                        const L7_char8 * * argv,
                                        uintf index)
{
  L7_uint32 argHistoryVal = 1;
  L7_uint32 unit;
  L7_uint32 pwdHistoryVal = FD_USER_MGR_PASSWD_HIST_LENGTH;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /* normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argHistoryVal], &pwdHistoryVal) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    pwdHistoryVal = FD_USER_MGR_PASSWD_HIST_LENGTH;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbPasswdHistoryLengthSet(unit, pwdHistoryVal)!= L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_PasswdHistoryCantBeSet);
    }
  } /* end of if(ewsContext->scriptActionFlag...*/

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}/* end of function */

/*********************************************************************
*
* @purpose   Config password Manager parameters
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
* @cmdsyntax  passwords aging <1-365>
* @noform     no passwords aging
*
* @cmdhelp
*
* @cmddescript  set aging on passwords.user needs to change passwords
*               when they expire.administrator can only execute this comamnd
*
* @end
*
*********************************************************************/
const L7_char8 *commandPasswordsAging(EwsContext ewsContext, uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index)
{
  L7_uint32 argAgingVal = 1;
  L7_uint32 unit;
  L7_uint32 pwdAgingVal = FD_USER_MGR_PASSWD_AGING_PERIOD;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argAgingVal], &pwdAgingVal) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    pwdAgingVal = FD_USER_MGR_PASSWD_AGING_PERIOD;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbPasswdAgingSet(unit, pwdAgingVal) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_PasswdAgingCantBeSet);
    }
  } /* end of if(ewsContext->scriptActionFlag...*/

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}/* end of the function */

/*********************************************************************
*
* @purpose   Config password Manager parameters
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
* @cmdsyntax  passwords lock-out <1-5>
* @noform     no passwords lock-out
*
* @cmdhelp
*
* @cmddescript  password lockout configuration.when a lockout count is
*               configured,then the user that is logging is must enter
*               the correct password with in the count.
* @end
*
*********************************************************************/
const L7_char8 *commandPasswordsLockOut(EwsContext ewsContext, uintf argc,
                                        const L7_char8 * * argv,
                                        uintf index)
{
  L7_uint32 argLockOutVal = 1;
  L7_uint32 unit;
  L7_uint32 pwdLockOutVal = FD_USER_MGR_PASSWD_USER_LOCKOUT;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /* normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index+argLockOutVal], &pwdLockOutVal) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    pwdLockOutVal = FD_USER_MGR_PASSWD_USER_LOCKOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbPasswdsLockoutSet(unit, pwdLockOutVal)!= L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_PasswdLockOutCantBeSet);
    }
  } /* end of if(ewsContext->scriptActionFlag...*/
    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#if 0 /*Replaced by commanduserUnlock by somebody*/
/*********************************************************************
*
* @purpose   Config password Manager parameters
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
* @cmdsyntax  users <username> unlock
* @noform     not supported
*
* @cmdhelp
*
* @cmddescript  password lockout configuration.when a lockout count is
*               configured,then the user that is logging is must enter
*               the correct password with in the count.
* @end
*
*********************************************************************/
const L7_char8 *commandUsersUnlock(EwsContext ewsContext, uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index)
{
  L7_uint32 argUserName = -1;
  L7_uint32 unit, tempInt;
  L7_char8 name[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  OSAPI_STRNCPY_SAFE(name, argv[index+argUserName]);

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (cliWebLoginIndexGet(name, &tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_UsrNotExists);
    }

    if (usmDbUserUnLock(unit, tempInt) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_PasswdLockOutCantBeSet);
    }
  } /* end of if(ewsContext->scriptActionFlag...*/
    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
#endif

/*********************************************************************
*
* @purpose  Build Tree for specifying enable password
*
* @param EwsCliCommandP  depth1 - parent node
*
* @returntype void
*
* @notes For priv exec mode
*
* @end
*
*********************************************************************/
void buildTreeEnablePasswd(EwsCliCommandP depth1)
{
#if 0
  EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Enbl_2, pStrInfo_base_EnblPasswd, NULL, 2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_base_Passwd_3, pStrInfo_base_CfgPasswdEnbl, commandEnablePasswd, 6,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                          L7_SYNTAX_NORMAL, pStrErr_base_CfgEnblPasswd,
                          L7_SYNTAX_NO, pStrErr_base_CfgEnblPasswdNo);
  ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Encrypted, pStrInfo_base_CfgEncrypt_1, commandEnablePasswdEncrypted, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Passwd, pStrInfo_base_CfgEncrypt, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
  EwsCliCommandP depth2, depth3, depth4,depth5,depth6;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Enbl_2, 
                         CLIENABLEPASSWORD_HELP,NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, 
                         pStrInfo_base_Passwd_4, 
                         CLILINEPASSWORD_SYNTAX_HELP,
                         commandEnablePassword, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr,pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Passwd,CLILINEPASSWORD_SYNTAX_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Encrypted, 
                         CLIPASSWORDENCRYPTED_SYNTAX_HELP,NULL,L7_NO_OPTIONAL_PARAMS );
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


}

/*********************************************************************
*
* @purpose  To print the running configuration of enable password
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t cliRunningEnablePasswd(EwsContext ewsContext)
{
  L7_char8 password[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  memset(password, 0, sizeof(password));
  if (usmDbEnablePasswordGet(FD_USER_MGR_ADMIN_ACCESS_LEVEL, password) == L7_SUCCESS && strlen(password) > 0)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_EnblPasswdEncrypted, password);
    EWSWRITEBUFFER(ewsContext,stat);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of passwd mgmt Info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliRunningConfigPwdMgmtInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_ushort16 val;

  if (usmDbMinPassLengthGet (&val) == L7_SUCCESS)
  {
    cliShowCmdInt (ewsContext, val, FD_USER_MGR_PASSWD_MIN_LENGTH, pStrInfo_base_PasswdsMinLen);
  }
  if (usmDbHistoryLengthGet (&val) == L7_SUCCESS)
  {
    cliShowCmdInt (ewsContext, val, FD_USER_MGR_PASSWD_HIST_LENGTH, pStrInfo_base_PasswdsHistory);
  }
  if (usmDbLockoutAttemptsGet (&val) == L7_SUCCESS)
  {
    cliShowCmdInt (ewsContext, val, FD_USER_MGR_PASSWD_USER_LOCKOUT, pStrInfo_base_PasswdsLockOut);
  }
  if (usmDbPasswdAgingValueGet (&val) == L7_SUCCESS)
  {
    cliShowCmdInt (ewsContext, val, FD_USER_MGR_PASSWD_AGING_PERIOD, pStrInfo_base_PasswdsAging);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Change the users password
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax     password <CR>
*
* @cmdhelp
*
* @cmddescript This commands takes no options
*
* @end
*
*********************************************************************/
const L7_char8 *commandNewPasswdOld(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32     unit;
  L7_char8      buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8      tempPw[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_ushort16   minPassLen;
  L7_RC_t       rc;
  L7_uint32     tempInt;
  L7_char8 currentUser[L7_LOGIN_SIZE];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* this command is not valid in a configuration script,
     just fail if we are validating a script or running a script */
  if ((ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT) ||
      (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    return cliPrompt(ewsContext);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  usmDbLoginSessionUserGet(unit,cliLoginSessionIndexGet(), currentUser);

  tempInt = 0;
  for (; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    rc = cliWebLoginUserNameGet(tempInt, buf);
    if (strcmp(buf, currentUser) == 0)
    {                                                                   /*user does exist */

      if (cliGetStringInputID() == CLI_INPUT_EMPTY)
      {
        osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_base_Passwd_3, argv[index]);
        cliAlternateCommandSet(buf);
        cliSetStringInputID(1, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_OldPasswd;
      }
      if (cliGetStringInputID() == 1)
      {
        memset(buf, 0, sizeof(buf));
        rc = cliWebLoginUserPasswordEncryptedGet(tempInt, buf);
        if (usmDbPasswordIsValid(unit, buf, cliGetStringInput()) != L7_SUCCESS)
          /* if (strcmp(buf, cliGetStringInput()) != 0)*/
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_IncorrectPasswd);
        }
        osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_base_Passwd_3, argv[index]);
        cliAlternateCommandSet(buf);
        cliSetStringInputID(2, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_NewPasswd;

      }

      if (cliGetStringInputID() == 2)
      {
        osapiStrncpySafe(tempPw, cliGetStringInput(), sizeof(tempPw));
        if (strlen(tempPw) > L7_PASSWORD_SIZE-1)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_InvalidPasswd, L7_PASSWORD_SIZE-1 );
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt(ewsContext, buf);
        }
        /* Restricting new passwd not to contain quotes*/
        if(strchr(tempPw,'"'))
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_PasswdSyntax);
          return cliSyntaxReturnPrompt(ewsContext, buf);
        }

        if (usmDbStringPrintableCheck(tempPw) != L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_PasswdMustBePrintAbleChars);
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt(ewsContext, buf);
        }

        /* New password must be MinLength */
        rc = usmDbPasswdLengthValid (unit, tempPw, L7_FALSE);
        if (rc == L7_FAILURE)
        {
          usmDbMinPassLengthGet(&minPassLen);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_CfgPasswdLenError,
                                  minPassLen, L7_PASSWORD_SIZE-1);
        }
        osapiStrncpySafe(cliCommon[cliUtil.handleNum].password, tempPw, sizeof(cliCommon[cliUtil.handleNum].password));
        osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_base_Passwd_3, argv[index]);
        cliAlternateCommandSet(buf);
        cliSetStringInputID(3, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_ConfirmPasswd_1;
      }
      if (cliGetStringInputID() == 3)
      {
        osapiStrncpySafe(tempPw, cliGetStringInput(), sizeof(tempPw));
        if (strcmp(cliCommon[cliUtil.handleNum].password, tempPw) == 0)
        {
          rc = cliWebLoginUserPasswordSet(tempInt, tempPw, L7_FALSE);

          if (rc == L7_ERROR)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetUsrPasswdPleaseAvoidReusingOldPasswds);
          }

          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_PasswdChgd);
          cliSyntaxBottom(ewsContext);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_base_PasswdNotMatched);
          cliSyntaxBottom(ewsContext);
        }
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
  }
  return cliPrompt (ewsContext);
}

/*********************************************************************
*
* @purpose  Change the users password
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax     password <CR>
*
* @cmdhelp
*
* @cmddescript This commands takes no options
*
* @end
*
*********************************************************************/
const L7_char8 *commandNewPasswd(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32     unit;
  L7_char8      buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8      tempPw[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_ushort16   minPassLen;
  L7_RC_t       rc;
  L7_uint32     tempInt;
  L7_char8 currentUser[L7_LOGIN_SIZE];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* this command is not valid in a configuration script,
     just fail if we are validating a script or running a script */
  if ((ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT) ||
      (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    return cliPrompt(ewsContext);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  usmDbLoginSessionUserGet(unit,cliLoginSessionIndexGet(), currentUser);

  tempInt = 0;
  for (; tempInt < L7_MAX_LOGINS; tempInt++)
  {
    rc = cliWebLoginUserNameGet(tempInt, buf);
    if (strcmp(buf, currentUser) == 0)
    {                                                                   /*user does exist */

      if (cliGetStringInputID() == CLI_INPUT_EMPTY)
      {
        osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_base_Passwd_4, argv[index]);
        cliAlternateCommandSet(buf);
        cliSetStringInputID(1, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_OldPasswd;
      }
      if (cliGetStringInputID() == 1)
      {
        memset(buf, 0, sizeof(buf));
        rc = cliWebLoginUserPasswordEncryptedGet(tempInt, buf);
        if (usmDbPasswordIsValid(unit, buf, cliGetStringInput()) != L7_SUCCESS)
          /* if (strcmp(buf, cliGetStringInput()) != 0)*/
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_IncorrectPasswd);
        }
        osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_base_Passwd_4, argv[index]);
        cliAlternateCommandSet(buf);
        cliSetStringInputID(2, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_NewPasswd;

      }

      if (cliGetStringInputID() == 2)
      {
        osapiStrncpySafe(tempPw, cliGetStringInput(), sizeof(tempPw));
        if (strlen(tempPw) > L7_PASSWORD_SIZE-1)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_InvalidPasswd, L7_PASSWORD_SIZE-1 );
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt(ewsContext, buf);
        }
        /* Restricting new passwd not to contain quotes*/
        if(strchr(tempPw,'"'))
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_PasswdSyntax);
          return cliSyntaxReturnPrompt(ewsContext, buf);
        }

        if (usmDbStringPrintableCheck(tempPw) != L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_PasswdMustBePrintAbleChars);
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt(ewsContext, buf);
        }

        /* New password must be MinLength */
        rc = usmDbPasswdLengthValid (unit, tempPw, L7_FALSE);
        if (rc == L7_FAILURE)
        {
          usmDbMinPassLengthGet(&minPassLen);
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_CfgPasswdLenError,
                                  minPassLen, L7_PASSWORD_SIZE-1);
        }
        osapiStrncpySafe(cliCommon[cliUtil.handleNum].password, tempPw, sizeof(cliCommon[cliUtil.handleNum].password));
        osapiSnprintf(buf, sizeof(buf), "%s", pStrInfo_base_Passwd_4, argv[index]);
        cliAlternateCommandSet(buf);
        cliSetStringInputID(3, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_ConfirmPasswd_1;
      }
      if (cliGetStringInputID() == 3)
      {
        osapiStrncpySafe(tempPw, cliGetStringInput(), sizeof(tempPw));
        if (strcmp(cliCommon[cliUtil.handleNum].password, tempPw) == 0)
        {
          rc = cliWebLoginUserPasswordSet(tempInt, tempPw, L7_FALSE);

          if (rc == L7_ERROR)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_base_SetUsrPasswdPleaseAvoidReusingOldPasswds);
          }

          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_PasswdChgd);
          cliSyntaxBottom(ewsContext);
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext,pStrInfo_base_PasswdNotMatched);
          cliSyntaxBottom(ewsContext);
        }
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
  }
  return cliPrompt (ewsContext);
}

/*********************************************************************
*
* @purpose   Config line mode login lists
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
* @cmdsyntax  login authentication {default| <listname>}.
* @noform     no login authentication
*
* @cmdhelp
*
* @cmddescript assigns authentication login list to Line
* @author       P.Muralikrishna
*
* @end
*
*********************************************************************/

const L7_char8  *commandLineLoginAuthentication(EwsContext ewsContext,
                                                uintf argc,
                                                const L7_char8 **argv,
                                                L7_uint32 index)
{
  /* position of list name in the command */
  L7_uint32   argLoginListName = 1;
  /* unit id in stacking */
  L7_uint32   unit;
  /* integer to store index of authentication list */
  L7_uint32   listIndex;
  /* integer to hold number of arguments in the command*/
  L7_uint32   numArgs;
  /* string to contain  authentication list name */
  L7_char8    strAuthListName[L7_MAX_AUTHENTICATIONLIST_NAME_SIZE + 1];
  /* string buffer to intermediate strings*/
  L7_uint32  accessLine = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  /* To get line mode */
  accessLine = EWSLINECONFIGTYPE(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check for the normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* checking for number of arguments for normal command */
    if ( numArgs != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgLoginAuthErr);
    }
    /* checking the length of  the Authentication List Name */
    else if (strlen(argv[index+argLoginListName]) >= sizeof(strAuthListName))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidLoginListLen, L7_MAX_AUTHENTICATIONLIST_NAME_SIZE);
    }/* end of checking authentication list name*/

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* checking for authentication list creation.if
       * it is not created earlier return an error
       */
      OSAPI_STRNCPY_SAFE(strAuthListName, argv[index+argLoginListName]);
      if (usmDbStringCaseInsensitiveCompare(strAuthListName, L7_USER_MGR_DEFAULT_USER_STRING) == L7_SUCCESS)
      {
        bzero(strAuthListName,sizeof(strAuthListName));
        if (accessLine == ACCESS_LINE_CONSOLE)
        {
          strcpy(strAuthListName, FD_APL_DEFAULT_LIST_NAME);
        }
        else
        {  /* telnet and ssh have different default list than console */
          strcpy(strAuthListName, L7_DEFAULT_NETWORK_APL_NAME);
        }
      }

      if (( L7_SUCCESS != cliIsAlphaNum(strAuthListName) ) ||
          ( L7_SUCCESS != usmDbAuthenticationListIndexGetByName(accessLine,
                                                                ACCESS_LEVEL_LOGIN,
                                                                strAuthListName,
                                                                &listIndex) ))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
      }
      /* assigning the authentication list login list created by
       * aaa authentication login  to line
       */
      if (usmDbAPLLineSet(accessLine ,
                          ACCESS_LEVEL_LOGIN,strAuthListName)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_SetAuthListError);
      }/* end of assigning list*/
    }/* end of if (ewscontext->script......*/
  }/* end of noraml command*/
  else if ( ewsContext->commType == CLI_NO_CMD)
  {
    /* checking the  number of arguments in the command*/
    if ( numArgs != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgAuthNoLoginCreate_2);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (accessLine == ACCESS_LINE_CONSOLE)
      {
        strcpy(strAuthListName, FD_APL_DEFAULT_LIST_NAME);
      }
      else
      {
        strcpy(strAuthListName, L7_DEFAULT_NETWORK_APL_NAME);
      }

      /*assigning the default authentication list set by
       *aaa authentication login command
       */
      if (usmDbAPLLineSet(accessLine ,
                          ACCESS_LEVEL_LOGIN,strAuthListName)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_SetAuthListError);
      }/* end of assigning default list*/
    }/*end of if(ewsContext....*/
  } /* end of no command */

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config Line mode enable lists
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
* @cmdsyntax  enable authentication {default| <listname>}.
* @noform     no enable authentication
*
* @cmdhelp
*
* @cmddescript assigns  authentication login lists to line
* @author       P.Muralikrishna
*
* @end
*
*********************************************************************/

const L7_char8  *commandLineEnableAuthentication(EwsContext ewsContext,
                                                 uintf argc,
                                                 const L7_char8 **argv,
                                                 uintf index)
{
  /* position of list name in the command */
  L7_uint32   argLoginListName = 1;
  /* unit id in stacking */
  L7_uint32   unit;
  /* integer to store index of authentication list */
  L7_uint32   listIndex;
  /* integer to hold number of arguments in the command*/
  L7_uint32   numArgs;
  /* string to contain  authentication list name */
  L7_char8    strAuthListName[L7_MAX_AUTHENTICATIONLIST_NAME_SIZE + 1];
  /* string buffer to intermediate strings*/
  L7_uint32  accessLine = 0;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* To get line mode */
  accessLine = EWSLINECONFIGTYPE(ewsContext);

  /* get the number of arguments in the command*/
  numArgs = cliNumFunctionArgsGet();
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check for the normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*checking the number of arguments in normal command */
    if ( numArgs != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgEnableAuthErr);
    }
    /* checking the length of  the Authentication List Name */
    else if (strlen(argv[index+argLoginListName]) >= sizeof(strAuthListName))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_InvalidLoginListLen, L7_MAX_AUTHENTICATIONLIST_NAME_SIZE);
    }
    /* checking for authentication list creation.if it is not
     * created earlier return an error
     */
    bzero(strAuthListName,sizeof(strAuthListName));
    osapiStrncpySafe(strAuthListName, argv[index+argLoginListName], sizeof(strAuthListName));
    if (usmDbStringCaseInsensitiveCompare(strAuthListName, L7_USER_MGR_DEFAULT_USER_STRING) == L7_SUCCESS)
    {
      bzero(strAuthListName,sizeof(strAuthListName));
      strcpy(strAuthListName, L7_ENABLE_APL_NAME);
    }
    if ((cliIsAlphaNum(strAuthListName) != L7_SUCCESS) ||
        (usmDbAuthenticationListIndexGetByName(accessLine,
                                               ACCESS_LEVEL_ENABLE,
                                               strAuthListName, &listIndex)
         != L7_SUCCESS))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_CfgAuthLoginInvalidListName);
    }
    /* assigning the authentication list login list created by
     * aaa authentication login  to line
     */
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbAPLLineSet(accessLine ,
                          ACCESS_LEVEL_ENABLE,strAuthListName)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_SetAuthListError);
      }/* end of assigning list*/
    }/* endof if(ewsContext->script....*/
  }/* end of normal command*/
  else if ( ewsContext->commType == CLI_NO_CMD)
  {
    /* checking for number of arguments in the no command */
    if ( numArgs != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_CfgAuthNoEnableCreate_2);
    }
    bzero(strAuthListName,sizeof(strAuthListName));
    strcpy(strAuthListName, L7_ENABLE_APL_NAME);
    /*assigning the defaultauthentication list set by
     * aaa authentication login command
     */
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbAPLLineSet(accessLine,
                          ACCESS_LEVEL_ENABLE,strAuthListName)
          != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_SetAuthListError);
      }/* end of assigning default list*/
    }/*endof if(ewsContext.....*/
  }/* end of no command*/

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose   Config line password per console, telnet, ssh
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
* @cmdsyntax  password <password> [encrypted]
* @noform     no password
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8  *commandLinePassword(EwsContext ewsContext,
                                     uintf argc,
                                     const L7_char8 **argv,
                                     uintf index)
{
  L7_uint32   argPassword = 1;
  /* unit id in stacking */
  L7_uint32   unit;
  /* integer to hold number of arguments in the command*/
  L7_uint32   numArgs;
  /* string to contain  password */
  L7_char8    strPassword[L7_PASSWORD_SIZE + 1];
  /* string buffer to intermediate strings*/
  L7_char8    buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32  accessLine = 0;
  L7_ushort16 minLength = L7_MIN_PASSWORD_SIZE;
  L7_BOOL     encrypted = L7_FALSE;
  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /* To get line mode */
  accessLine = EWSLINECONFIGTYPE(ewsContext);

  /* get the number of arguments in the command*/
  numArgs = cliNumFunctionArgsGet();
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* check for the normal command */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    usmDbMinPassLengthGet(&minLength);
    /*checking the number of arguments in normal command */
    if ( numArgs != 1 && numArgs != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_UsePasswd);
    }

    if (numArgs == 2)
    {
      if (usmDbStringCaseInsensitiveCompare(argv[index+argPassword+1], "encrypted") == L7_SUCCESS)
        encrypted = L7_TRUE;
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_UsePasswd);
      }
    }

    /* checking the length of  the password */
    if (usmDbPasswdLengthValid (unit, (L7_char8 *)argv[index+argPassword],encrypted) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_base_CfgPasswdLenError,
                                minLength, L7_PASSWORD_SIZE-1 );
    }
    else
    {
       bzero(strPassword,sizeof(strPassword));
       osapiStrncpySafe(strPassword, argv[index+argPassword], sizeof(strPassword));
    }
    /*
     * alpha numeric check the password string
     */
    if (usmDbStringPrintableCheck(strPassword) != L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_PasswdMustBePrintAbleChars);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt(ewsContext, buf);
    }
    else if (strchr(strPassword,'"'))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrErr_base_PasswdSyntax);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt(ewsContext, buf);
    }
    /*
     * set the new password
     */
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLinePasswordSet(accessLine, strPassword, encrypted) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_InvalidLinePasswd);
      }/* end of assigning list*/
    }/* endof if(ewsContext->script....*/
  }/* end of normal command*/
  else if ( ewsContext->commType == CLI_NO_CMD)
  {
    /* checking for number of arguments in the no command */
    if ( numArgs != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_base_UseNoPasswd);
    }

    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbLinePasswordRemove(accessLine) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_base_InvalidLinePasswd);
      }/* end of assigning default list*/
    }/*endof if(ewsContext.....*/
  }/* end of no command*/

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
