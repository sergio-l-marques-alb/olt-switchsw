/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_usermgr.h
 *
 * @purpose header for user manager commands in clicommands.c
 *
 * @component user interface
 *
 * @comments
 *
 * @create  07/18/2003
 *
 * @author  chinmoyb
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_USERMGR_H
#define CLICOMMANDS_USERMGR_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"

#define CLIENABLEPASSWORD_HELP                 "Set the password for the enable privilege level."
#define CLILINEPASSWORD_SYNTAX_HELP       "Specify the password."
#define CLIPASSWORDENCRYPTED_SYNTAX_HELP  "Specify whether the password entered copied from another device configuration as encrypted."
#define CLIERROR_UNIT_ID                    "\r\nERROR: Could not get switch ID."
#define CLISYNTAX_CONFIGENABLEPASSWORD          "\r\nIncorrect input! Use 'enable password password [encrypted]'."
#define CLIERROR_CONFIGENABLEPASSWORDLENGTHERROR        "\r\nError: Password length must be between %u and %u characters."
#define CLI_PASSWDMUSTBEPRINTABLECHARACTERS "\r\nUser password must be printable characters."
#define CLI_PASSWDSYNTAX      "\r\nPassword must not contain the character \""
#define CLIERROR_CONFIGENABLEPASSWORD          "\r\n Error: Could not set enable password."
#define CLISYNTAX_CONFIGNOENABLEPASSWORD      "\r\nIncorrect input! Use 'no enable password'."
#define CLISYNTAX_CONFIGADDUSER                 "\r\nIncorrect input! Use 'users name <username>'."
#define CLI_PASSWDLENRANGESET        "\r\nCould not set user password! \r\npassword should be in range of %u-%u characters in length \r\nor set minimum password length to 0 by using 'passwords min-length 0' command."
#define CLISYNTAX_INVALIDUSERNAME           "\r\nInvalid user name.  User name can be up to %u characters in length."
#define CLI_USERNAMESYNTAX        "\r\nError: Characters in the user name must be alphanumeric and can contain dashes or underscores."
#define CLI_USEREXISTSALREADY       "\r\nUser already exists!"
#define CLI_TOOMANYUSERS        "\r\nThe maximum number of users has already been created."
#define CLISYNTAX_CONFIGDELETEUSER              "\r\nIncorrect input! Use 'no users name <username>'."
#define CLI_USERNOTEXISTS       "\r\nUser does not exist!"
#define CLI_USERNOTDELETED        "\r\nUnable to delete the user!"

#define    CLIERROR_PWDMGMT_NOPASSWORD_ERR  "\r\nError! Set minimum password length to 0 first by using 'passwords min-length 0' command."
#define CLISYNTAX_CONFIGPASSWDUSER              "\r\nIncorrect input! Use 'username <name> password <password> [level <level>] [encrypted]'."
#define CLISYNTAX_INVALIDUSERNAME           "\r\nInvalid user name.  User name can be up to %u characters in length."
#define CLI_USERNAMESYNTAX        "\r\nError: Characters in the user name must be alphanumeric and can contain dashes or underscores."
#define CLISYNTAX_CONFIGUNLOCKUSER              "\r\nIncorrect input! Use 'username <name> unlock'."
#define CLIEMPTYSTRING ""
#define CLI_PASSWDALREADYPRESENT "\r\nCould not set user password! password already present in password history."
#define CLISYNTAX_INVALIDACCESSLEVEL        "\r\nInvalid access level. Access level can be either 0, 1 or 15."
#define CLISYNTAX_CONFIGPASSWDUSER_NO              "\r\nIncorrect input! Use 'no users passwd <username>'."
#define CLI_PASSWDNOTSET        "\r\nCould not set user password!"
#define CLISYNTAX_NO_PASSWORD_RESET   "\r\nError! Since %s is enabled, password can not be reset."
#define CLISYNTAXAUTHENTICATION  "authentication\0"
#define CLI_USERCREATEFAILED       "\r\nFailed to create User:'%s'"
#define CLISYNTAX_PASSWORD_TOO_SHORT  "\r\nError! Since %s is enabled, password must be at least %u characters in length."
#define CLISYNTAXENCRYPTION  "encryption\0"
#define CLI_PASSWDRESET         "\r\nPassword reset!"
#define CLISYNTAX_SHOWUSERSAUTHENTICATION           "\r\nIncorrect input! Use 'show users authentication <listname>'"
#define CLIERROR_SHOWAUTHENTICATIONNOUSERERROR            "\r\nError getting User."
#define CLIERROR_SHOWAUTHENTICATIONSYSLISTGETRERROR       "\r\nError getting Authentication Login List for System Login component"
#define CLISYNTAX_SHOWUSERSAUTHENTICATION           "\r\nIncorrect input! Use 'show users authentication <listname>'"
#define CLINAME      "\r\n--More-- or (q)uit"
#define CLI_COMMANDTYPE_NORMAL                                   \
  {                                                              \
    if (ewsContext->commType == CLI_NO_CMD)                      \
    {                                                            \
      ewsTelnetWrite(ewsContext,CLISYNTAX_COMMANDTYPE_NORMAL );  \
      cliSyntaxBottom(ewsContext);                               \
      return cliPrompt(ewsContext);                              \
    }                                                            \
  }
#define CLIERROR_CONFIGAUTHENTICATIONLOGININVALIDUSERNAME "\r\nInvalid User Name."
#define CLISYNTAX_COMMANDTYPE_NORMAL   "\r\nIncorrect input! The 'no' option is not valid for above command."

#define CLISHOW_AUTHENTICATION_HELP "Display ordered methods for authentication lists"
#define CLISHOW_AUTHENTICATIONMETHODS_HELP  "Display methods attached to the authenticated lists."
#define CLISYNTAX_SHOWAUTHENTICATIONLOGININFO     "\r\nIncorrect input! Use 'show authentication'"
#define CLIERROR_SHOWAUTHENTICATIONLINELISTGETERROR "\r\nError retrieving Authentication Line List'"
#define CLISYNTAX_AUTHENTICATIONMODEGETERROR    "\r\nIncorrect mode option!."
#define CLIERROR_SHOWAUTHENTICATIONLISTGETERROR           "\r\nError retrieving Authentication Login List"
#define CLILOCAL_AUTH    "local"
#define CLITACACS_AUTH     "tacacs"
#define CLINONE_AUTH       "none"
#define CLILINE_AUTH       "line"
#define CLIENABLE_AUTH     "enable"
#define CLIRADIUS_AUTH     "radius"
#define CLIIAS_AUTH     "ias"
#define CLI_PASSWD_MUST_NOT_CONTAIN_LOGINNAME "\r\nError! User password should not contain Login name in any form (sub-string, case-insensitive or reverse)."
#define CLI_PASSWD_MUST_NOT_CONTAIN_EXCLUDE_KEYWORDS "\r\nCould not set user password! \n\rUser password contains the Exclude keyword '%s' (in one of the forms sub-string, case-insensitive or reverse)."
#define CLI_PASSWD_REPEAT_CHAR_LEN_RANGESET      "\r\nCould not set user password! \r\npassword should not contain repeated characters more than %u in length."
#define CLI_PASSWD_CONSECUTIVE_CHAR_LEN_RANGESET "\r\nCould not set user password! \r\npassword should not contain consecutive characters more than %u in length.\r\n"
#define CLI_PASSWD_CHARACTER_CLASSES_RANGESET "\r\nCould not set user password! \r\npassword should contain at least %u character classes."
#define CLI_PASSWD_UPPERCASE_LEN_RANGESET "\r\nCould not set user password! \r\npassword should contain at least %u uppercase letters."
#define CLI_PASSWD_LOWERCASE_LEN_RANGESET "\r\nCould not set user password! \r\npassword should contain at least %u lowercase letters."
#define CLI_PASSWD_NUMERIC_LEN_RANGESET "\r\nCould not set user password! \r\npassword should contain at least %u numeric characters."
#define CLI_PASSWD_SPECIAL_LEN_RANGESET "\r\nCould not set user password! \r\npassword should contain at least %u special characters."
#define CLI_PASSWD_RESTRICTED_QUOTATION_CHAR "\r\nError! User password should not contain quotation mark (\") special character."
#define CLI_PASSWD_RESTRICTED_QUESTIONMARK_CHAR "\r\nError! User password should not contain question mark (?) special character."
#define CLI_PASSWD_UPPERCASE_CHARS_EXCEED_MAX_LEN "\r\nCould not set user password! \r\npassword exceeds uppercase characters maximum length limit %u."
#define CLI_PASSWD_LOWERCASE_CHARS_EXCEED_MAX_LEN "\r\nCould not set user password! \r\npassword exceeds lowercase characters maximum length limit %u."
#define CLI_PASSWD_NUMERIC_CHARS_EXCEED_MAX_LEN "\r\nCould not set user password! \r\npassword exceeds numeric characters maximum length limit %u."
#define CLI_PASSWD_SPECIAL_CHARS_EXCEED_MAX_LEN "\r\nCould not set user password! \r\npassword exceeds special characters maximum length limit %u."

/*****************************
**
** BEGIN USER MANAGER FUNCTIONS HELP
**
******************************/

/* string help for the show functions  */

/**********************************************************************/
/* ACTION FUNCTIONS - Authentication */

/* Config */
const L7_char8 *commandAuthenticationLogin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandAuthenticationDot1x(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandAuthenticationEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandHttpsAuthenticationLogin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandHttpAuthenticationLogin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUsersDefaultLogin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUsersLogin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandEnablePasswdEncrypted(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandEnablePasswd(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandEnablePassword(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLinePassword(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandUserNameOld(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUserName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUsersPasswdEncrypted(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUsersPasswd(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUserNopassword(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandPassword(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandUserUnlock(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

const L7_char8 *commandDisconnect(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNewPasswdOld(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandNewPasswd(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandLineLoginAuthentication(EwsContext ewsContext, uintf argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8  *commandLineEnableAuthentication(EwsContext ewsContext, uintf argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandPasswordsStrengthCheckConfig(EwsContext ewsContext,  uintf argc,
                                                    const L7_char8 ** argv, uintf index);
const L7_char8 *commandPasswordsStrengthCharacterClassesConfig(EwsContext ewsContext, uintf argc,
                                                               const L7_char8 **argv, uintf index);


/* Show */
const L7_char8 *commandShowAuthenticationMethods(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowAuthenticationUsers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowUsersAuthentication(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPasswordsMinlength(EwsContext ewsContext, uintf argc,
                                          const L7_char8 * * argv,
                                          uintf index);

const L7_char8 *commandPasswordsHistory(EwsContext ewsContext, uintf argc,
                                        const L7_char8 * * argv,
                                        uintf index);

const L7_char8 *commandPasswordsAging(EwsContext ewsContext, uintf argc,
                                      const L7_char8 * * argv,
                                      uintf index);

const L7_char8 *commandPasswordsLockOut(EwsContext ewsContext, uintf argc,
                                        const L7_char8 * * argv,
                                        uintf index);

const L7_char8 *commandUsersUnlock(EwsContext ewsContext, uintf argc,
                                   const L7_char8 * * argv,
                                   uintf index);
const L7_char8 *commandShowPasswordsConfiguration(EwsContext ewsContext,
                                                  uintf argc,
                                                  const L7_char8 * * argv,
                                                  uintf index);
const L7_char8 *commandShowUsersAccounts(EwsContext ewsContext,
                                         uintf argc,
                                         const L7_char8 * * argv,
                                         uintf index);
const L7_char8  *commandShowUsersLoginHistory(EwsContext ewsContext, 
                         L7_uint32 argc, 
                        const L7_char8 **argv, 
                        L7_uint32 index);

const L7_char8 *commandShowUsersLoginStats(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 * * argv,
                                           uintf index);

const L7_char8 *commandShowUsers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowLoginSession(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowUserLongNames(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowLoginSessionUserLongNames(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8  *commandShowLoginHistoryLongNames(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

const L7_char8 *commandShowPasswordsResult(EwsContext ewsContext,
                                           uintf argc,
                                           const L7_char8 * * argv,
                                           uintf index);
/**********************************************************************/

/**********************************************************************/
/* TREE FUNCTIONS - User management */

void buildTreeEnablePasswd(EwsCliCommandP depth1);
void buildTreeNewPasswd(EwsCliCommandP depth1);

L7_RC_t cliRunningEnablePasswd(EwsContext ewsContext);
L7_RC_t cliRunningConfigPwdMgmtInfo(EwsContext ewsContext, L7_uint32 unit);

void buildTreeIntAuthServUserConfig();
void buildTreeShowIntAuthServUsers(EwsCliCommandP depth1);
void buildTreePrivilegedIntAuthServUsersClear(EwsCliCommandP depth1);

/**********************************************************************/
const L7_char8 *commandIntAuthServUserPassword(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandShowIntAuthServUsers(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);
const L7_char8 *commandClearIntAuthServUsers(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/**********************************************************************/

#endif
