/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/security/captive_portal/cli_config_cp_user_local.c
*
* @purpose config commands for Captive portal user local 
*
* @component user interface
*
* @comments none
*
* @create  07/12/2007
*
* @author  rjain, rjindal
*
* @end
*
**********************************************************************/

#include "strlib_security_cli.h"
#include "cliapi.h"
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "ews.h"
#include "clicommands_cp.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_user_api.h"
#include "captive_portal_defaultconfig.h"

/*********************************************************************
* @purpose  Delete a Captive Portal local user
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  no user <user-id>
*
* @cmdhelp  Delete a Captive Portal local user
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserDelete(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if ((ewsContext->commType != CLI_NO_CMD) || (cliNumFunctionArgsGet() != 0))
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPUserDelete);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserGroupAssocEntryByUIdDeleteAll(uId))
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserDeleteError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (usmDbCpdmUserEntryDelete(uId) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserDeleteError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user group
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> group <group-id>
* @cmdsyntax  no user <user-id> group <group-id>
*
* @cmdhelp  Configure Captive Portal user group.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  gpId_t gId=0;
  L7_uint32 uId32, gId32;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliNumFunctionArgsGet() != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupID);
    }
    else if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoGroupID);
    }
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+1], &gId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if ((gId32 < GP_ID_MIN) || (gId32 > GP_ID_MAX))
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserGroupIDOutOfRange, GP_ID_MIN, GP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  gId = (L7_ushort16)gId32;

  if (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_GroupIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if ((rc=usmDbCpdmUserGroupAssocEntryAdd(uId, gId)) != L7_SUCCESS)
      {
        if (rc == L7_ALREADY_CONFIGURED)
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserGroupIDAlreadyConfigured, gId);
          ewsTelnetWrite(ewsContext, stat);
        }
        else
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupIDAddError);
        }
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      uId_t uId_next = 0;
      gpId_t gId_next = 0;
      L7_uint32 count = 0;

      while ((usmDbCpdmUserGroupAssocEntryNextGet(uId, gId_next, &uId_next, &gId_next) == L7_SUCCESS) && (uId == uId_next))
      {
        count++;
      }

      if (1 == count)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserCantBeRemoved);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (usmDbCpdmUserGroupAssocEntryDelete(uId, gId) != L7_SUCCESS)        
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupIDDeleteError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user idle timeout
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> idle-timeout <time>
* @cmdsyntax  no user <user-id> idle-timeout
*
* @cmdhelp  Configure Captive Portal user idle timeout.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserIdleTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32, timeout=FD_CP_LOCAL_USER_IDLE_TIMEOUT;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserIdleTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((timeout < CP_USER_LOCAL_IDLE_TIMEOUT_MIN) || (timeout > CP_USER_LOCAL_IDLE_TIMEOUT_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgIdleTimeoutOutOfRange, CP_USER_LOCAL_IDLE_TIMEOUT_MIN, CP_USER_LOCAL_IDLE_TIMEOUT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoIdleTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    timeout = FD_CP_LOCAL_USER_IDLE_TIMEOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntryIdleTimeoutSet(uId, timeout) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserIdleTimeoutSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user name
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> name <name>
*
* @cmdhelp  Configure Captive Portal user name.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserName(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32;
  L7_char8 name[CP_USER_LOCAL_USERNAME_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc = L7_FAILURE;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPUserName);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliIsAlphaNum((L7_char8 *)argv[index+1]) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNameAlphaNumeric);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strlen(argv[index+1]) > CP_USER_LOCAL_USERNAME_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgNameLength, CP_USER_LOCAL_USERNAME_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  osapiStrncpy(name, argv[index+1], CP_USER_LOCAL_USERNAME_MAX);
  name[CP_USER_LOCAL_USERNAME_MAX] = '\0';

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
    {
      /* add user */
      if (usmDbCpdmUserEntryAdd(uId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserAddError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    /* set user name */
    if (usmDbCpdmUserEntryLoginNameSet(uId, name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNameSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* add group association */
    if ((rc=usmDbCpdmUserGroupAssocEntryAdd(uId, GP_ID_MIN)) != L7_SUCCESS)
    {
      if (rc != L7_ALREADY_CONFIGURED)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupIDAddError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Add a Captive Portal local user
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> password
*
* @cmdhelp  Add a Captive Portal local user.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserAddUpdate(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32;
  static L7_char8 pwd[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 0)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPUserAdd);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (cliGetStringInputID() == CLI_INPUT_EMPTY)
  {
    /* first pass through, set the alternate command to get the password */
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), pStrInfo_security_CPUserAddCmd, uId);
    osapiSnprintf(stat, sizeof(stat), "%s ", cmdBuf);
    cliAlternateCommandSet(stat);
    cliSetStringInputID(1, ewsContext, argv);
    cliSetStringPassword();
    return pStrInfo_security_CPUserPwd;
  }
  else if (cliGetStringInputID() == 1)
  {
    osapiStrncpy(pwd, cliGetStringInput(), L7_CLI_MAX_STRING_LENGTH-1);
    pwd[L7_CLI_MAX_STRING_LENGTH-1] = '\0';

    if ((strlen(pwd) < CP_USER_LOCAL_PASSWORD_MIN) || (strlen(pwd) > CP_USER_LOCAL_PASSWORD_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserPwdError, CP_USER_LOCAL_PASSWORD_MIN, CP_USER_LOCAL_PASSWORD_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* second pass through, set the alternate command to confirm the password */
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), pStrInfo_security_CPUserAddCmd, uId);
    osapiSnprintf(stat, sizeof(stat), "%s ", cmdBuf);
    cliAlternateCommandSet(stat);
    cliSetStringInputID(2, ewsContext, argv);
    cliSetStringPassword();
    return pStrInfo_security_CPUserPwdReEnter;
  }
  else /* cliGetStringInputID() == 2 */
  {
    if (strcmp(pwd, cliGetStringInput()) != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserPwdMismatch);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
    {
      /* add user */
      if (usmDbCpdmUserEntryAdd(uId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserAddError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if ((rc=usmDbCpdmUserGroupAssocEntryAdd(uId, GP_ID_MIN)) != L7_SUCCESS)
      {
        if (rc == L7_ALREADY_CONFIGURED)
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserGroupIDAlreadyConfigured, GP_ID_MIN);
          ewsTelnetWrite(ewsContext, stat);
        }
        else
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupIDAddError);
        }
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    /* add/update user password */
    if (usmDbCpdmUserEntryPasswordSet(uId, pwd) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserPwdSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user password in encrypted format
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> password encrypted <encrypted-password>
*
* @cmdhelp  Configure Captive Portal user password in encrypted format (128 hex characters).
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserPasswordEncrypted(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32;
  L7_char8 pwd[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-2], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;


  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPUserPwdEncrypted);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  osapiStrncpy(pwd, argv[index+1], sizeof(pwd));
  pwd[L7_ENCRYPTED_PASSWORD_SIZE-1] = '\0';

  if ((strlen(pwd) != L7_ENCRYPTED_PASSWORD_SIZE-1) || (usmDbStringHexadecimalCheck(pwd) != L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPUserPwdEncryptedError);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
    {
      /* add user */
      if (usmDbCpdmUserEntryAdd(uId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserAddError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if ((rc=usmDbCpdmUserGroupAssocEntryAdd(uId, GP_ID_MIN)) != L7_SUCCESS)
      {
        if (rc == L7_ALREADY_CONFIGURED)
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserGroupIDAlreadyConfigured, GP_ID_MIN);
          ewsTelnetWrite(ewsContext, stat);
        }
        else
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupIDAddError);
        }
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    /* add/update user password */
    if (usmDbCpdmUserEntryEncryptedPasswordSet(uId, pwd) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserPwdEncryptedSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user session timeout
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> session-timeout <time>
* @cmdsyntax  no user <user-id> session-timeout
*
* @cmdhelp  Configure Captive Portal user session timeout.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserSessionTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32, timeout=FD_CP_LOCAL_USER_SESSION_TIMEOUT;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserSessionTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((timeout < CP_USER_LOCAL_SESSION_TIMEOUT_MIN) || (timeout > CP_USER_LOCAL_SESSION_TIMEOUT_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgSessionTimeoutOutOfRange, CP_USER_LOCAL_SESSION_TIMEOUT_MIN, CP_USER_LOCAL_SESSION_TIMEOUT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoSessionTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    timeout = FD_CP_LOCAL_USER_SESSION_TIMEOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntrySessionTimeoutSet(uId, timeout) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserSessionTimeoutSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user max bandwidth up
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> max-bandwidth-up <bytes>
* @cmdsyntax  no user <user-id> max-bandwidth-up
*
* @cmdhelp  Configure Captive Portal user max bandwidth up.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserMaxBandwidthUp(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32, bytes=CP_DEF_USER_UP_RATE;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserMaxBandwidthUp);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_BW_UP_MIN) || (bytes > CP_USER_LOCAL_MAX_BW_UP_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_BW_UP_MIN, CP_USER_LOCAL_MAX_BW_UP_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoMaxBandwidthUp);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_UP_RATE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* convert from bytes to bits */
    if (usmDbCpdmUserEntryMaxBandwidthUpSet(uId, bytes*8) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user max bandwidth down
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> max-bandwidth-down <bytes>
* @cmdsyntax  no user <user-id> max-bandwidth-down
*
* @cmdhelp  Configure Captive Portal user max bandwidth down.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserMaxBandwidthDown(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32, bytes=CP_DEF_USER_DOWN_RATE;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserMaxBandwidthDown);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_BW_DOWN_MIN) || (bytes > CP_USER_LOCAL_MAX_BW_DOWN_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_BW_DOWN_MIN, CP_USER_LOCAL_MAX_BW_DOWN_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoMaxBandwidthDown);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_DOWN_RATE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* convert from bytes to bits */
    if (usmDbCpdmUserEntryMaxBandwidthDownSet(uId, bytes*8) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user max input octets
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> max-input-octets <bytes>
* @cmdsyntax  no user <user-id> max-input-octets
*
* @cmdhelp  Configure Captive Portal user max input octets.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserMaxInputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32, bytes=CP_DEF_USER_INPUT_OCTETS;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserMaxInputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN) || (bytes > CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoMaxInputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_INPUT_OCTETS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntryMaxInputOctetsSet(uId, bytes) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user max output octets
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> max-output-octets <bytes>
* @cmdsyntax  no user <user-id> max-output-octets
*
* @cmdhelp  Configure Captive Portal user max output octets.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserMaxOutputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32, bytes=CP_DEF_USER_OUTPUT_OCTETS;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserMaxOutputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN) || (bytes > CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoMaxOutputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_OUTPUT_OCTETS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntryMaxOutputOctetsSet(uId, bytes) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user max total octets
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user <user-id> max-total-octets <bytes>
* @cmdsyntax  no user <user-id> max-total-octets
*
* @cmdhelp  Configure Captive Portal user max total octets.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalUserMaxTotalOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  uId_t uId=0;
  L7_uint32 uId32, bytes=CP_DEF_TOTAL_OCTETS;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &uId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (uId32 > FD_CP_LOCAL_USERS_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserIDOutOfRange, FD_CP_LOCAL_USERS_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  uId = (L7_ushort16)uId32;

  if (usmDbCpdmUserEntryGet(uId) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_UserIdDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserMaxTotalOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN) || (bytes > CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoMaxTotalOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_TOTAL_OCTETS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserEntryMaxTotalOctetsSet(uId, bytes) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure a Captive Portal user group
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  user group <group-id>
* @cmdsyntax  no user group <group-id>
*
* @cmdhelp  Configure a Captive Portal user group.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPUserGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  gpId_t gId=0;
  L7_uint32 gId32;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroup);      
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserNoGroup);
    }
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+1], &gId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if ((gId32 < GP_ID_MIN) || (gId32 > GP_ID_MAX))
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserGroupIDOutOfRange, GP_ID_MIN, GP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  gId = (L7_ushort16)gId32;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS)
      {
        if (usmDbCpdmUserGroupEntryAdd(gId) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupAddError);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupExists);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (gId == GP_ID_MIN)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupNoDefaultDelete);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      if (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_GroupIdDoesNotExist);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }

      /* replace gId's associations with the default group, 
       * gId is replaced with GP_ID_MIN only if there is no association already
       */
      (void)usmDbCpdmUserGroupAssocEntryGroupRename(gId, GP_ID_MIN);

      if (usmDbCpdmUserGroupEntryDelete(gId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupDeleteError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal user group name
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
* @cmdsyntax  user group <group-id> name <name>
*
* @cmdhelp  Configure Captive Portal user group name.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPUserGroupName(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 gId32;
  gpId_t gId=0;
  L7_char8 name[CP_USER_LOCAL_USERGROUP_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupName);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &gId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if ((gId32 < GP_ID_MIN) || (gId32 > GP_ID_MAX))
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgGroupOutOfRange, GP_ID_MIN, GP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  gId = (L7_ushort16)gId32;

  if (cliIsAlphaNum((L7_char8 *)argv[index+1]) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNameAlphaNumeric);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strlen(argv[index+1]) > CP_USER_LOCAL_USERGROUP_MAX)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgNameLength, CP_USER_LOCAL_USERGROUP_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  osapiStrncpy(name, argv[index+1], CP_USER_LOCAL_USERGROUP_MAX);
  name[CP_USER_LOCAL_USERGROUP_MAX] = '\0';

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS)
    {
      if (usmDbCpdmUserGroupEntryAdd(gId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupAddError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    if (usmDbCpdmUserGroupEntryNameSet(gId, name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupNameSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Moveusers 'moves' existing users from one user group to another
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
* @cmdsyntax  user group <group-id> moveusers <new-group-id>
*
* @cmdhelp  moves existing users from one user group to another
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPUserGroupMoveUsers(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 gId32;
  gpId_t gId_old=0, gId_new=0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPUserGroupMoveUsers);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index-1], &gId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if ((gId32 < GP_ID_MIN) || (gId32 > GP_ID_MAX))
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgGroupOutOfRange, GP_ID_MIN, GP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  gId_old = (L7_ushort16)gId32;

  if (cliConvertTo32BitUnsignedInteger(argv[index+1], &gId32) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if ((gId32 < GP_ID_MIN) || (gId32 > GP_ID_MAX))
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgGroupOutOfRange, GP_ID_MIN, GP_ID_MAX);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }
  gId_new = (L7_ushort16)gId32;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmUserGroupEntryGet(gId_old) != L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_GroupIdOldDoesNotExist, gId_old);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (usmDbCpdmUserGroupEntryGet(gId_new) != L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_GroupIdOldDoesNotExist, gId_new);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* replace gId_old's associations with gId_new, 
     * gId_old is replaced with gId_new only if there is no association already
     */
    if (usmDbCpdmUserGroupAssocEntryGroupRename(gId_old, gId_new) != L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPUserGroupRenameError, gId_old);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

