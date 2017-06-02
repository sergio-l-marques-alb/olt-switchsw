/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_switch_user_mgmt.c
 *
 * @purpose create the cli for Switching User Acct Mgmt
 *
 * @component user interface
 *
 * @comments contains the code to build the tree for Switching Device
 * @comments also contains functions that allow tree navigation
 *
 * @create  07/10/20003
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
#include "usmdb_util_api.h"
#include "snmp_exports.h"

/*********************************************************************
*
* @purpose  Build the Switch User Acct Mgmt user commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreeGlobalSwUamUsers(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9, depth10;
  L7_BOOL snmpUser = L7_FALSE, snmpAuth = L7_FALSE, snmpEncr = L7_FALSE;
  L7_uint32 unit;
#if 0
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Usrs_1, pStrInfo_base_CfgUsrs, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* No Need for this command in the new User Manager */
  /* buildTreeGlobalAuthenticationUsersConfig(depth2); */

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_ApProfileShowRunningName, pStrInfo_base_CfgUsrName, commandUserName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_UsrnameOld, pStrInfo_base_CfgUsrsAddName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Unlock, pStrInfo_base_PasswdMgmtUn, commandUsersUnlock, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Passwd_3, pStrInfo_base_CfgUsrsPasswd, commandUsersPasswd, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Usrname, pStrInfo_base_CfgUsrsPasswd_1, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Encrypted, pStrInfo_base_CfgEncrypt_1, commandUsersPasswdEncrypted, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Passwd, pStrInfo_base_CfgEncrypt, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Usrname1, pStrInfo_base_CfgUsrs, 
                         commandUserName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_name_parm, pStrInfo_base_CfgUsrs, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Passwd_4, pStrInfo_base_CfgUsrPassWd , 
                         commandPassword, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Passwd, pStrInfo_base_CfgNewUsrPassWdString , 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_OverrideComplexityCheck, pStrInfo_base_PasswordStrengthOverrideComplexityCheck, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Encrypted, pStrInfo_base_CfgEncrypt1, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_OverrideComplexityCheck, pStrInfo_base_PasswordStrengthOverrideComplexityCheck, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Lvl, pStrInfo_base_CfgAccessLevel, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_Lvl1, pStrInfo_base_CfgAccessLevel, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_OverrideComplexityCheck, pStrInfo_base_PasswordStrengthOverrideComplexityCheck, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth8 = ewsCliAddNode(depth7, pStrInfo_base_Encrypted, pStrInfo_base_CfgEncrypt1, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth9 = ewsCliAddNode(depth8, pStrInfo_base_OverrideComplexityCheck, pStrInfo_base_PasswordStrengthOverrideComplexityCheck, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_NoPasswd, pStrInfo_base_CfgNoUsrPasswd, 
                         commandUserNopassword, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Lvl, pStrInfo_base_CfgAccessLevel, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Lvl1, pStrInfo_base_CfgAccessLevel, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_unlock, pStrInfo_base_CfgUserUnlock, 
                         commandUserUnlock, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, 
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();

  snmpUser = usmDbFeaturePresentCheck(unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_SUPPORTED);
  snmpAuth = usmDbFeaturePresentCheck(unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_AUTHENTICATION_FEATURE_ID);
  snmpEncr = usmDbFeaturePresentCheck(unit, L7_SNMP_COMPONENT_ID, L7_SNMP_USER_ENCRYPTION_FEATURE_ID);
  if (snmpUser == L7_TRUE || snmpAuth == L7_TRUE || snmpEncr == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Snmpv3_1, pStrInfo_base_CfgUsrsSnmpv3, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    if (snmpUser == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Accessmode, pStrInfo_base_UsrsSnmpv3AccessMode, commandUsersSnmpv3AccessMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_base_Usrname, pStrInfo_base_CfgUsrsDel, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Readonly, pStrInfo_base_CfgUsrsSnmpv3AccessMode, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Readwrite, pStrInfo_base_CfgUsrsSnmpv3AccessMode, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
    if (snmpAuth == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auth_1, pStrInfo_base_CfgUsrsAuth, commandUsersSnmpv3Authentication, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_base_Usrname, pStrInfo_base_CfgUsrsDel, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_None_3, pStrInfo_base_CfgUsrsAuthType, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Md5_1, pStrInfo_base_CfgUsrsAuthType, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Sha_1, pStrInfo_base_CfgUsrsAuthType, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
    if (snmpEncr == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Encryption, pStrInfo_base_CfgUsrsEncrypt, commandUsersSnmpv3Encryption, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_base_Usrname, pStrInfo_base_CfgUsrsDel, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_None_3, pStrInfo_base_CfgUsrsEncryptType, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Des_1, pStrInfo_base_CfgUsrsEncryptType, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Key, pStrInfo_base_CfgUsrsEncryptKey, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth5 = ewsCliAddNode(depth4, pStrInfo_base_Encrypted, pStrInfo_base_CfgSnmpv3Encrypt_1, 
                             commandUsersSnmpv3EcryptdKeyEncryption, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_base_Usrname, pStrInfo_base_CfgUsrsDel, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_base_Des_1, pStrInfo_base_CfgUsrsEncryptType, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Key, pStrInfo_base_CfgUsrsEncryptHexKey, NULL, L7_NO_OPTIONAL_PARAMS);
      depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    }
  }
}

/*********************************************************************
*
* @purpose  Build the Switch User Acct Mgmt loginsession tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreeUserExSwUamShowLogin(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Loginsession, pStrInfo_base_ShowLoginSession, commandShowLoginSession, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, "long", pStrInfo_base_ShowLoginSessionUsers, commandShowLoginSessionUserLongNames, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch User Acct Mgmt show users commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreePrivSwUamShowUsers(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5,depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Usrs_1, 
                         pStrInfo_base_ShowUsrs, commandShowUsers, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Accounts, 
                         pStrInfo_base_CfgUsrAccounts, commandShowUsersAccounts, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_detail, 
                         pStrInfo_base_ShowUsrsAccounts1, commandShowUsersAccounts, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, "long", pStrInfo_base_ShowUsrNames, commandShowUserLongNames, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* buildTreePrivShowAuthenticationUsers(depth3); */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_ShowLoginHistory, 
                         pStrInfo_base_CfgUsrLoginHistory, commandShowUsersLoginHistory, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Usrname, 
                         pStrInfo_base_CfgAuthUsrname, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, "long", pStrInfo_base_ShowUsrNames, commandShowLoginHistoryLongNames, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch User Acct Mgmt disconnect commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreePrivSwUamDisconnect(EwsCliCommandP depth1)
{
  /* depth2 = "Privileged Exec" */
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Disconnect_1, pStrInfo_base_Disconnect, commandDisconnect, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, NULL, pStrInfo_base_CfgLogSessionClose, NULL, 3, L7_NODE_UINT_RANGE, 0, (FD_UI_DEFAULT_MAX_CONNECTIONS-1));
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_base_DisconnectAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
