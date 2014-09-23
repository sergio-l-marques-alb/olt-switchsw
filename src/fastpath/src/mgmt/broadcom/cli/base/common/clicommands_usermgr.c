/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_usermgr.c
 *
 * @purpose create the tree for CLI Authentication commands
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/22/2003
 *
 * @author  chinmoyb
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "clicommands_usermgr.h"
#include "cli_web_exports.h"

/*********************************************************************
*
* @purpose  Build the authentication configuration tree for HTTP and
*           HTTPS connection
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
* @author  Sudip Midya
* @end
*
*********************************************************************/
void buildTreeGlobalHTTPConfig(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Http_1, pStrInfo_base_HttpAuthHelp,NULL , 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Auth_1, pStrInfo_base_Auth, commandHttpAuthenticationLogin, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* list is used to authenticate https and http connection */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Method1, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Method2, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Method3, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_Method4, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Https_1, pStrInfo_base_HttpsAuthHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Auth_1, pStrInfo_base_Auth, commandHttpsAuthenticationLogin, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* list is used to authenticate https and http connection */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Method1, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Method2, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Method3, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_Method4, pStrInfo_base_AuthOption, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
*
* @purpose  To build the Internal Authentication Server User config 
*           tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreeIntAuthServUserConfig()
{
  EwsCliCommandP    depth1, depth2, depth3, depth4, depth5;

  depth1 = ewsCliAddNode (NULL, NULL, NULL, cliIntAuthServUserConfigMode,
                         L7_NO_OPTIONAL_PARAMS);

  cliSetMode (L7_INT_AUTH_SERV_USER_CONFIG_MODE, depth1);

  /* exit */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit,
                         pStrInfo_common_ToExitMode, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr,
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /* password */
  depth2 = ewsCliAddNode (depth1,pStrInfo_base_Passwd_4,
                          pStrInfo_base_CfgUsrPassWd,
                          commandIntAuthServUserPassword,2,
                          L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  depth3 = ewsCliAddNode (depth2,pStrInfo_base_Passwd,
                          pStrInfo_base_CfgUsrPassWdString,
                          NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr,
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Encrypted, 
                         pStrInfo_base_CfgEncrypt1, NULL, 
                         L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL, 
                         L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Cr,
                          pStrInfo_common_NewLine, NULL ,2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

}

/*********************************************************************
 *
 * @purpose  To build the Dot1x Authentication Server User config mode
 *
 * @param void
 *
 * @returntype void
 *
 * @note
 *
 * @end
 *
 *********************************************************************/
void buildTreeIntAuthServUserDBConfig(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* ias-user */
  depth2 = ewsCliAddNode (depth1, pStrInfo_base_IASUser_1,
                          pStrInfo_base_CfgIASUsersHelp, NULL, 2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* aaa ias-user username <user>*/
  depth3 = ewsCliAddNode (depth2, pStrInfo_base_Usrname1, pStrInfo_base_CfgUserName,
                          cliIntAuthServUserConfigMode,
                          2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, pStrInfo_base_IASUserName,
                          pStrInfo_base_CfgAuthUsrname,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED,
                          L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                          NULL, 4,
                          L7_NO_COMMAND_SUPPORTED,
                          L7_STATUS_BOTH, L7_NODE_TYPE_MODE,
                          L7_INT_AUTH_SERV_USER_CONFIG_MODE);
}
/*********************************************************************
*
* @purpose  Build the authentication configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalAuthenticationConfig(EwsCliCommandP depth1)
{
  /* depth1 = Global Config Mode */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, enableTree, loginTree;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Auth_1, pStrInfo_base_Auth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* dot1x authentication List */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dot1x_2, pStrInfo_base_AuthLogin, commandAuthenticationDot1x, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_PassDefl, pStrInfo_common_CfgDeflAuth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_NoneAuth, pStrInfo_base_HelpNoneAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_InternalAuth, pStrInfo_base_HelpInternalAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LocalAuth, pStrInfo_base_HelpLocalAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_RadiusAuth, pStrInfo_base_HelpRadiusAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Enable Authentication List */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Enbl_2, pStrInfo_base_AuthLogin, commandAuthenticationEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_PassDefl, pStrInfo_common_CfgDeflAuth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  enableTree = depth4;

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_EnableAuth, pStrInfo_base_HelpEnableAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LineAuth, pStrInfo_base_HelpLineAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_RadiusAuth, pStrInfo_base_HelpRadiusAuthMethod, NULL, 2, L7_OPTIONS_NODE, enableTree);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_TacacsAuth, pStrInfo_base_HelpTacacsAuthMethod, NULL, 2, L7_OPTIONS_NODE, enableTree);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_NoneAuth, pStrInfo_base_HelpNoneAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Listname, pStrInfo_base_AuthLoginListName, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, enableTree);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /* Login Authentication List */
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Login_1, pStrInfo_base_AuthLogin, commandAuthenticationLogin, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_PassDefl, pStrInfo_common_CfgDeflAuth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  loginTree = depth4;
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_EnableAuth, pStrInfo_base_HelpEnableAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LineAuth, pStrInfo_base_HelpLineAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_RadiusAuth, pStrInfo_base_HelpRadiusAuthMethod, NULL, 2, L7_OPTIONS_NODE, loginTree);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_TacacsAuth, pStrInfo_base_HelpTacacsAuthMethod, NULL, 2, L7_OPTIONS_NODE, loginTree);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_NoneAuth, pStrInfo_base_HelpNoneAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_LocalAuth, pStrInfo_base_HelpLocalAuthMethod, NULL, 2, L7_SYNTAX_NORMAL, "");
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Listname, pStrInfo_base_AuthLoginListName, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, loginTree);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

}

/*********************************************************************
*
* @purpose  Build the authentication configuration for non-configured users commands ftree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalAuthenticationUsersConfig(EwsCliCommandP depth2)
{
  /* depth2 = users */
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Defllogin, pStrInfo_base_AuthUsrsDeflLogin, commandUsersDefaultLogin, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Listname, pStrInfo_base_AuthLoginListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Login_1, pStrInfo_base_AuthUsrsLogin, commandUsersLogin, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Usr, pStrInfo_common_AuthUsrName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Listname, pStrInfo_base_AuthLoginListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the authentication show commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivShowAuthentication(EwsCliCommandP depth2)
{
  /* depth2 = show */
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Auth_1, CLISHOW_AUTHENTICATION_HELP,NULL , L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Methods, CLISHOW_AUTHENTICATIONMETHODS_HELP, commandShowAuthenticationMethods, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose    Build the show command for Dot1x Authentication Server in the Priv EXEC mode.
*
* @param      void
*
* @returntype struct EwsCliCommandP
*
* @notes      none
*
* @end
*
*********************************************************************/
void buildTreeShowIntAuthServUsers (EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_AAA, pStrInfo_base_ShowAllIASUsersHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IASUser, pStrInfo_base_ShowAllIASUsersHelp, commandShowIntAuthServUsers, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IASUserName, pStrInfo_base_ShowIASUserHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose    Build the Clear command for Dot1x Authentication Server in the Priv EXEC mode.
*
* @param      void
*
* @returntype struct EwsCliCommandP
*
* @notes      none
*
* @end
*
*********************************************************************/
void buildTreePrivilegedIntAuthServUsersClear (EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4,depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_AAA, pStrInfo_base_IASClearUserDBHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IASUser, pStrInfo_base_IASClearUserDBHelp, commandClearIntAuthServUsers, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the authentication show users commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivShowAuthenticationUsers(EwsCliCommandP depth3)
{
  /* depth3 = users */
  EwsCliCommandP depth4, depth5;

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auth_1, pStrInfo_base_AuthUsrs, commandShowUsersAuthentication, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the password management tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalConfigPasswordMgmt(EwsCliCommandP depth2)
{
  /* depth2 = Global Config Mode */
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Passwds, pStrInfo_base_PasswdMgmtPasswds, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* command passwords aging <age> */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Aging, pStrInfo_base_PasswdMgmtAging, commandPasswordsAging, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_UsePasswdsAgingRange1to365,
                         L7_SYNTAX_NO,     pStrErr_base_UseNoPasswdsAging);
  depth5 = ewsCliAddNode(depth4,  /*<1-365>*/ NULL, pStrInfo_base_PasswdMgmtAgingVal, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_MIN_PASSWD_AGING, L7_MAX_PASSWD_AGING);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command passwords history <historylength> */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_History, pStrInfo_base_PasswdMgmtHistory, commandPasswordsHistory, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_UsePasswdsHistoryRange0to10,
                         L7_SYNTAX_NO,     pStrErr_base_UseNoPasswdsHistory);
  depth5 = ewsCliAddNode(depth4,  /*<0-10>*/ NULL, pStrInfo_base_PasswdMgmtHistoryVal, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_MIN_HIST_LENGTH, L7_MAX_HIST_LENGTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command passwords lock-out <attempts> */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LockOut, pStrInfo_base_PasswdMgmt, commandPasswordsLockOut, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_UsePasswdsLockOutRange1to5,
                         L7_SYNTAX_NO,     pStrErr_base_UseNoPasswdsLockOut);
  depth5 = ewsCliAddNode(depth4,  /*<1-5>*/ NULL, pStrInfo_base_PasswdMgmt_1, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_MIN_LOCKOUT_COUNT, L7_MAX_LOCKOUT_COUNT);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command passwords min-length <length >*/
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_MinLen,pStrInfo_base_PasswdMgmtMinLen, commandPasswordsMinlength, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_UsePasswdsMinLenRange8to64,
                         L7_SYNTAX_NO,     pStrErr_base_MinLenNo);
  depth5 = ewsCliAddNode(depth4,  /*<8-64>*/ NULL, pStrInfo_base_PasswdMgmtMinLenVal, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_MIN_PASSWORD_SIZE, L7_PASSWORD_SIZE-1);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command passwords strength-check */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_StrengthCheck, pStrInfo_base_PasswordStrengthCheck, commandPasswordsStrengthCheckConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthCmd,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthCmdNo);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


  /* command passwords strength minimum {uppercase | lowercase | numeric | special | characterclasses} [<value>] */ 
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Strength, pStrInfo_base_PasswordStrength, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);


  /* command passwords strength exclude-keyword <keyword> */ 
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_ExcludeKeyword, pStrInfo_base_PasswordStrengthExcludeKeyword, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthExcludeKeyword,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthExcludeKeywordNo);
  depth6 = ewsCliAddNode(depth5,  pStrInfo_base_Keyword, pStrInfo_base_PasswordStrengthExcludeKeywordString2to64, NULL, 5,
                         L7_NODE_STRING_RANGE, L7_PASSWD_EXCLUDE_KEYWORD_MIN_LEN, L7_PASSWD_EXCLUDE_KEYWORD_MAX_LEN-1,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command passwords strength minimum */ 
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Minimum, pStrInfo_base_PasswordStrengthMinimum, NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* command passwords strength minimum uppercase-letters [<value>] */ 
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_UppercaseLetters, pStrInfo_base_PasswordStrengthUppercaseLetters, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthMinUppercaseLetters,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthMinUppercaseLettersNo);
  depth7 = ewsCliAddNode(depth6,  NULL, pStrInfo_base_PasswordStrengthUppercaseLetters, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_PASSWD_UPPERCASE_CHARS_MIN_LEN, L7_PASSWD_UPPERCASE_CHARS_MAX_LEN);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command passwords strength minimum lowercase-letters [<value>] */ 
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LowercaseLetters, pStrInfo_base_PasswordStrengthLowercaseLetters, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthMinLowercaseLetters,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthMinLowercaseLettersNo);
  depth7 = ewsCliAddNode(depth6,  NULL, pStrInfo_base_PasswordStrengthLowercaseLetters, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_PASSWD_LOWERCASE_CHARS_MIN_LEN, L7_PASSWD_LOWERCASE_CHARS_MAX_LEN);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command passwords strength minimum numeric-characters [<value>] */ 
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_NumericCharacters, pStrInfo_base_PasswordStrengthNumericCharacters, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthMinNumericChars,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthMinNumericCharsNo);
  depth7 = ewsCliAddNode(depth6,  NULL, pStrInfo_base_PasswordStrengthNumericCharacters, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_PASSWD_NUMERIC_CHARS_MIN_LEN, L7_PASSWD_NUMERIC_CHARS_MAX_LEN);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command passwords strength minimum special-characters [<value>] */ 
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_SpecialCharacters, pStrInfo_base_PasswordStrengthSpecialCharacters, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthMinSpecialChars,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthMinSpecialCharsNo);
  depth7 = ewsCliAddNode(depth6,  NULL, pStrInfo_base_PasswordStrengthSpecialCharacters, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_PASSWD_SPECIAL_CHARS_MIN_LEN, L7_PASSWD_SPECIAL_CHARS_MAX_LEN);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command passwords strength minimum character-classes [<value>] */ 
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_CharacterClasses, pStrInfo_base_PasswordStrengthCharacterClasses, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthMinCharacterClasses,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthMinCharacterClassesNo);
  depth7 = ewsCliAddNode(depth6,  NULL, pStrInfo_base_PasswordStrengthCharacterClasses, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_PASSWD_CHARACTER_CLASSES_MIN, L7_PASSWD_CHARACTER_CLASSES_MAX);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command passwords strength maximum {repeated-characters | consecutive-characters} [<value>] */
  /* command passwords strength maximum */ 
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Maximum, pStrInfo_base_PasswordStrengthMaximum, NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  /* command passwords strength maximum repeated-characters [<value>] */ 
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_RepeatedCharacters, pStrInfo_base_PasswordStrengthRepeatedCharacters, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthMaxRepeatedChars,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthMaxRepeatedCharsNo);
  depth7 = ewsCliAddNode(depth6,  NULL, pStrInfo_base_PasswordStrengthRepeatedCharacters, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_PASSWD_MAX_REPEATED_CHARS_MIN_LEN, L7_PASSWD_MAX_REPEATED_CHARS_MAX_LEN);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* command passwords strength maximum consecutive-characters [<value>] */ 
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_ConsecutiveCharacters, pStrInfo_base_PasswordStrengthConsecutiveCharacters, 
                         commandPasswordsStrengthCharacterClassesConfig, 6,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,
                         L7_SYNTAX_NORMAL, pStrErr_base_CfgPasswordStrengthMaxConsecutiveChars,
                         L7_SYNTAX_NO,     pStrErr_base_CfgPasswordStrengthMaxConsecutiveCharsNo);
  depth7 = ewsCliAddNode(depth6,  NULL, pStrInfo_base_PasswordStrengthConsecutiveCharacters, NULL, 3,
                         L7_NODE_UINT_RANGE, L7_PASSWD_MAX_CONSECUTIVE_CHARS_MIN_LEN, L7_PASSWD_MAX_CONSECUTIVE_CHARS_MAX_LEN);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build Show passwords Management configuration
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivShowPasswordsConfiguration(EwsCliCommandP depth2)
{
  /* depth3 = users */
  EwsCliCommandP depth3,depth4,depth5;
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Passwds, pStrInfo_base_PasswdMgmtShowPasswds,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cfg_8, pStrInfo_base_PasswdMgmtCfg,
                         commandShowPasswordsConfiguration, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);


  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Result, pStrInfo_base_PasswdLastSetResult,
                         commandShowPasswordsResult, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the password tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes New function to add password command in the user exec mode
*
* @end
*
*********************************************************************/
void buildTreeNewPasswd(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3 = 0;
#if 0
  depth2 = ewsCliAddNode (depth1, pStrInfo_base_Passwd_3, pStrInfo_base_CfgUsrsPasswd,
                          commandNewPasswdOld, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
#else
  depth2 = ewsCliAddNode (depth1, pStrInfo_base_Passwd_4, pStrInfo_base_CfgUsrsPasswd,
                          commandNewPasswd, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
#endif

}

/*********************************************************************
 * *
 * * @purpose  Build the Line config login tree
 * *
 * * @param struct EwsCliCommandP
 * *
 * * @returntype void
 * *
 * * @notes none
 * *
 * * @end
 * *
 * *********************************************************************/
                                                                                                                                                             
void buildTreeLineConfigSWMgmtLogin(EwsCliCommandP depth2)
{
  /* depth1 = Line Config Mode */
                                                                                                                                                             
  EwsCliCommandP depth3, depth4,depth5,depth6;
                                                                                                                                                             
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Login_1, pStrInfo_base_CfgLoginAuth ,NULL  , 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
                                                                                                                                                             
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auth_1, pStrInfo_base_AuthCfgrLogin, commandLineLoginAuthentication  , 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
                                                                                                                                                             
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_PassDefl, pStrInfo_common_CfgDeflAuth,NULL,L7_NO_OPTIONAL_PARAMS );
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
                                                                                                                                                             
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Listname, pStrInfo_base_AuthLoginListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
                                                                                                                                                             
}

/********************************************************************
*
* @purpose  Build the Line config  enable tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeLineConfigSWMgmtEnable(EwsCliCommandP depth2)
{
  /* depth1 = Line Config Mode */

  EwsCliCommandP depth3, depth4,depth5,depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Enbl_2, pStrInfo_base_CfgLoginAuth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auth_1, pStrInfo_base_AuthCfgrLogin, commandLineEnableAuthentication  , 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_PassDefl, pStrInfo_common_CfgDeflAuth,NULL,L7_NO_OPTIONAL_PARAMS );
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Listname, pStrInfo_base_AuthLoginListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the line password config tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeLinePasswordConfig(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4,depth5,depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Passwd_4, pStrInfo_base_LinePassword, commandLinePassword, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Passwd, pStrInfo_base_PasswdVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Encrypted, pStrInfo_base_CfgLineEncrypt,NULL,L7_NO_OPTIONAL_PARAMS );
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
