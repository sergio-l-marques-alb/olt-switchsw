/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr_util.c
*
* @purpose   user manager utility functions
*
* @component user mgr component
*
* @comments  none
*
* @create    09/27/2002
*
* @author    Jill Flanagan
*
* @end
*
**********************************************************************/

#include "commdefs.h"

#include <ctype.h>
#include "l7_common.h"
#include "cli_web_exports.h"
#include "snmp_exports.h"
#include "user_manager_exports.h"
#include "defaultconfig.h"
#include "user_mgr.h"
#include <stdio.h>
#include "pw_scramble_api.h"
#include "user_mgr_api.h"
#include "user_mgr_apl.h"
#include "user_mgr_ports.h"
#include "default_cnfgr.h"
#include "log.h"
#include "snmp_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

extern userMgrCfgData_t userMgrCfgData;

APL_t fd_login_apls[] = FD_LOGIN_APLS;

logins_t FD_logins[] = FD_LOGINS;

user_component_auth_t FD_nonConfiguredUserAuth[FD_NONCONFIGUREDAUTH_SIZE] =
{
  {
    L7_DEFAULT_APL_NAME,
    L7_USER_MGR_COMPONENT_ID
  },
  {
    L7_DEFAULT_APL_NAME,
    L7_DOT1X_COMPONENT_ID
  }
};

APL_t FD_ENABLE_APL = FD_ENABLE_APLS;
APL_t FD_WEB_APL    = FD_WEB_APLS;
APL_t FD_DOT1X_APL  = FD_DOT1X_APLS;

/*********************************************************************
* @purpose  Build default config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void
userMgrBuildDefaultConfigData(L7_uint32 ver)
{
  memset(( void * )&userMgrCfgData, 0, sizeof( userMgrCfgData_t));
  osapiStrncpy(userMgrCfgData.cfgHdr.filename, USER_MGR_CFG_FILENAME, L7_MAX_FILENAME);
  userMgrCfgData.cfgHdr.version = ver;
  userMgrCfgData.cfgHdr.componentID = L7_USER_MGR_COMPONENT_ID;
  userMgrCfgData.cfgHdr.type = L7_CFG_DATA;
  userMgrCfgData.cfgHdr.length = sizeof(userMgrCfgData);
  userMgrCfgData.cfgHdr.version = ver;

  userMgrConfigLoginsFactoryDefault();

  userMgrCfgData.cfgHdr.dataChanged = L7_FALSE;

}

/*********************************************************************
* @purpose  Returns login user encryption key per index
*
* @param    index       table index
* @param    encryptkey  pointer to login user encryption key
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    This routine is private to user manager and is therefore
*           not prototyped in a header file.
*
* @end
*********************************************************************/
L7_uint32
userMgrLoginUserEncryptionKeyGet(L7_uint32 index, L7_char8 *encryptKey)
{
	if (index >= L7_MAX_LOGINS)
		return(L7_FAILURE);

	pwUnscrambleLength(userMgrCfgData.systemLogins[index].encryptKey, encryptKey,
                     L7_ENCRYPTION_KEY_SIZE);

	return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Calls snmpUserSet if snmp is supported
*
* @param    index - offset into cfg structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrSnmpUserSet(L7_uint32 index)
{
  L7_char8 password[L7_PASSWORD_SIZE];
  L7_char8 encryptionKey[L7_ENCRYPTION_KEY_SIZE];

  if ( userMgrLoginUserPasswordClearTextGet(index, password ) == L7_SUCCESS &&
       userMgrLoginUserEncryptionKeyGet(index, encryptionKey ) == L7_SUCCESS )
  {

    if ( SnmpUserSet(
           userMgrCfgData.systemLogins[index].loginName,
           userMgrCfgData.systemLogins[index].snmpv3AccessMode,
           userMgrCfgData.systemLogins[index].authProt,
           userMgrCfgData.systemLogins[index].encryptProt,
           password,
           encryptionKey,
           L7_FALSE ) != L7_SUCCESS )
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Calls snmpUserDelete if snmp is supported
*
* @param    index - offset into cfg structure
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    does not check for snmp user supported
*
* @end
*********************************************************************/
L7_RC_t userMgrSnmpUserDelete(L7_uint32 index)
{
  if (SnmpUserDelete(userMgrCfgData.systemLogins[index].loginName,
                     userMgrCfgData.systemLogins[index].snmpv3AccessMode,
                     userMgrCfgData.systemLogins[index].authProt,
                     userMgrCfgData.systemLogins[index].encryptProt) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restore all user config data to snmp through usmdb for snmpv3
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrRestoreSnmpUsers(void)
{
  L7_uint32 i;
  L7_char8 password[L7_PASSWORD_SIZE];
  L7_char8 encryptionKey[L7_ENCRYPTION_KEY_SIZE];

  if (SNMP_USER_SUPPORTED == L7_TRUE )
  {
    for( i=0; i < L7_MAX_LOGINS; i++ )
    {
      if (userMgrCfgData.systemLogins[i].loginStatus == L7_ENABLE)
      {
        if ( userMgrLoginUserPasswordClearTextGet(i, password ) == L7_SUCCESS &&
             userMgrLoginUserEncryptionKeyGet(i, encryptionKey ) == L7_SUCCESS )
        {

          if ( SnmpUserSet(
                 userMgrCfgData.systemLogins[i].loginName,
                 userMgrCfgData.systemLogins[i].snmpv3AccessMode,
                 userMgrCfgData.systemLogins[i].authProt,
                 userMgrCfgData.systemLogins[i].encryptProt,
                 password,
                 encryptionKey,
                 L7_TRUE ) != L7_SUCCESS )
          {
            return L7_FAILURE;
          }
        }
      }
    } /* end for all logins */
  } /* end if snmpv3 is supported */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Pass all user config data to snmp through usmdb for snmpv3
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrInitSnmpUsers(void)
{
  L7_uint32 i;

  if (SNMP_USER_SUPPORTED == L7_TRUE )
  {
    for( i=0; i < L7_MAX_LOGINS; i++ )
    {
      if (userMgrCfgData.systemLogins[i].loginStatus == L7_ENABLE &&
          userMgrSnmpUserSet(i) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
    } /* end for all logins */
  } /* end if snmpv3 is supported */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set logins back to Factory Defaults
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void userMgrConfigLoginsFactoryDefault(void)
{
  L7_uint32     presentTime;
  L7_uint32     i;

  /* build checking on FD_logins structure */
#if (FD_USER_MGR_LOGINS_SIZE != L7_MAX_USERS_LOGINS)
#error USER_MGR Build Error
#endif

  memset(userMgrCfgData.systemLogins, 0, sizeof(userMgrCfgData.systemLogins));

  memcpy(userMgrCfgData.systemLogins, FD_logins, sizeof(FD_logins));

  userMgrCfgData.minPasswdLength     = FD_USER_MGR_PASSWD_MIN_LENGTH;
  userMgrCfgData.passwdHistoryLength = FD_USER_MGR_PASSWD_HIST_LENGTH;
  userMgrCfgData.passwdAgingDays     = FD_USER_MGR_PASSWD_AGING_PERIOD;
  userMgrCfgData.userLockoutCount    = FD_USER_MGR_PASSWD_USER_LOCKOUT;
  
  userMgrCfgData.minUppercaseLetters = FD_USER_MGR_PASSWD_MIN_UPPERCASE_CHARS;
  userMgrCfgData.minLowercaseLetters = FD_USER_MGR_PASSWD_MIN_LOWERCASE_CHARS;
  userMgrCfgData.minNumericChars     = FD_USER_MGR_PASSWD_MIN_NUMERIC_CHARS;
  userMgrCfgData.minSpecialChars     = FD_USER_MGR_PASSWD_MIN_SPL_CHARS;
  userMgrCfgData.maxConsecutiveChars = FD_USER_MGR_PASSWD_MAX_CONSECUTIVE_CHARS;
  userMgrCfgData.maxRepeatedChars    = FD_USER_MGR_PASSWD_MAX_REPEATED_CHARS;
  userMgrCfgData.minCharacterClasses = FD_USER_MGR_PASSWD_MIN_CHARACTER_CLASSES;

  userMgrCfgData.checkFlags = FD_USER_MGR_PASSWD_CHECK_FLAGS;

  presentTime = osapiUTCTimeNow();

  for (i = 0; i < FD_USER_MGR_LOGINS_SIZE; i++)
  {
    userMgrCfgData.systemLogins[i].passwdTimeStamp = presentTime;
  }

  /* build checking on FD_nonConfiguredUserAuth structure */
#if (FD_NONCONFIGUREDAUTH_SIZE != L7_APL_COMPONENT_COUNT)
#error USER_MGR NonConfigured User Build Error
#endif

  memcpy(userMgrCfgData.nonConfiguredUserAuth,
         FD_nonConfiguredUserAuth,
         sizeof(FD_nonConfiguredUserAuth));

  userMgrConfigAPLsFactoryDefault();

  userMgrPortInitializeAll( L7_DOT1X_DEFAULT_USER_PORT_ACCESS );
}

/*********************************************************************
* @purpose  Set APLs back to Factory Defaults
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void userMgrConfigAPLsFactoryDefault(void)
{
	int i = 0;

  /* build checking on FD_APLs structure */
#if (FD_APL_SIZE != FD_MAX_APL_COUNT)
#error USER_MGR APL List Build Error  FD_APL_SIZE != FD_MAX_APL_COUNT
#endif

  for (i = 0; i < sizeof(fd_login_apls) / sizeof(fd_login_apls[0]); i++)
  {
    memcpy(&userMgrCfgData.authListInfo.loginAuthLists[i], &fd_login_apls[i], sizeof(fd_login_apls[0]));
  }

  memcpy(&userMgrCfgData.authListInfo.enableAuthLists[0], &FD_ENABLE_APL, sizeof(FD_ENABLE_APL));
  memcpy(&userMgrCfgData.authListInfo.httpAuthLists[0], &FD_WEB_APL, sizeof(FD_WEB_APL));
  memcpy(&userMgrCfgData.authListInfo.httpsAuthLists[0], &FD_WEB_APL, sizeof(FD_WEB_APL));
  memcpy(&userMgrCfgData.authListInfo.dot1xAuthLists[0], &FD_DOT1X_APL, sizeof(FD_DOT1X_APL));

  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_CONSOLE][ACCESS_LEVEL_LOGIN], (userMgrCfgData.authListInfo.loginAuthLists[0].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_CONSOLE][ACCESS_LEVEL_ENABLE], (userMgrCfgData.authListInfo.enableAuthLists[0].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_TELNET][ACCESS_LEVEL_LOGIN], (userMgrCfgData.authListInfo.loginAuthLists[1].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_TELNET][ACCESS_LEVEL_ENABLE], (userMgrCfgData.authListInfo.enableAuthLists[0].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_SSH][ACCESS_LEVEL_LOGIN], (userMgrCfgData.authListInfo.loginAuthLists[1].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_SSH][ACCESS_LEVEL_ENABLE], (userMgrCfgData.authListInfo.enableAuthLists[0].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_CTS][ACCESS_LEVEL_LOGIN], (userMgrCfgData.authListInfo.loginAuthLists[0].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_CTS][ACCESS_LEVEL_ENABLE], (userMgrCfgData.authListInfo.enableAuthLists[0].name));

  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_HTTPS][ACCESS_LEVEL_LOGIN], (userMgrCfgData.authListInfo.httpAuthLists[0].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_HTTP][ACCESS_LEVEL_LOGIN], (userMgrCfgData.authListInfo.httpsAuthLists[0].name));
  strcpy(userMgrCfgData.authListInfo.authListName[ACCESS_LINE_DOT1X][ACCESS_LEVEL_LOGIN], (userMgrCfgData.authListInfo.dot1xAuthLists[0].name));
}

/*********************************************************************
* @purpose  Private utility function to initialize all data structure fields
*
* @param    index       table index
*
* @returns  void
*
* @end
*********************************************************************/
void userMgrInitialize(L7_uint32 index )
{
	memset( userMgrCfgData.systemLogins[index].loginName, 0, L7_LOGIN_SIZE );
	memset( userMgrCfgData.systemLogins[index].password, 0, L7_ENCRYPTED_PASSWORD_SIZE );
	userMgrCfgData.systemLogins[index].accessMode =  L7_AGENT_COMM_ACCESS_LEVEL_READ_ONLY;
  userMgrCfgData.systemLogins[index].loginStatus = L7_DISABLE;
	userMgrCfgData.systemLogins[index].authProt = L7_SNMP_USER_AUTH_PROTO_NONE;
	userMgrCfgData.systemLogins[index].encryptProt = L7_SNMP_USER_PRIV_PROTO_NONE;
	memset( userMgrCfgData.systemLogins[index].encryptKey, 0, L7_ENCRYPTION_KEY_SIZE );
	userMgrCfgData.systemLogins[index].snmpv3AccessMode =  L7_AGENT_COMM_ACCESS_LEVEL_READ_ONLY;

  memset(userMgrCfgData.systemLogins[index].userAuth[USER_MANAGER_COMPONENT_OFFSET].APLName, 0, L7_MAX_APL_NAME_SIZE + 1 );
  osapiStrncpy(userMgrCfgData.systemLogins[index].userAuth[USER_MANAGER_COMPONENT_OFFSET].APLName, L7_DEFAULT_APL_NAME, L7_MAX_APL_NAME_SIZE+1 );
  userMgrCfgData.systemLogins[index].userAuth[USER_MANAGER_COMPONENT_OFFSET].componentId = L7_USER_MGR_COMPONENT_ID;

  memset(userMgrCfgData.systemLogins[index].userAuth[DOT1X_COMPONENT_OFFSET].APLName, 0, L7_MAX_APL_NAME_SIZE + 1 );
  osapiStrncpy(userMgrCfgData.systemLogins[index].userAuth[DOT1X_COMPONENT_OFFSET].APLName, L7_DEFAULT_APL_NAME, L7_MAX_APL_NAME_SIZE+1 );
  userMgrCfgData.systemLogins[index].userAuth[DOT1X_COMPONENT_OFFSET].componentId = L7_DOT1X_COMPONENT_ID;


	userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  userMgrPortUserInitialize(index);
}

/*********************************************************************
* @purpose  Reset the methods for the APL at the specified index
*
* @param    index (input) index into APLs structure
*
* @returns  void
*
* @notes Intended for use within User Manager component.  Other
*        components should call the method that takes an APL name.
*
* @end
*********************************************************************/
void userMgrAPLResetMethodsIndex(L7_ACCESS_LINE_t line,
                                 L7_ACCESS_LEVEL_t level,
                                 L7_uint32 index)
{
  L7_uint32 i, maxNumMethods=0;
  APL_t *authList = NULL;

  switch (line)
  {
    case ACCESS_LINE_HTTP:
      memcpy(&userMgrCfgData.authListInfo.httpAuthLists[0], &FD_WEB_APL, sizeof(FD_WEB_APL));
      break;
    case ACCESS_LINE_HTTPS:
      memcpy(&userMgrCfgData.authListInfo.httpsAuthLists[0], &FD_WEB_APL, sizeof(FD_WEB_APL));
      break;
    case ACCESS_LINE_DOT1X:
      memcpy(&userMgrCfgData.authListInfo.dot1xAuthLists[0], &FD_DOT1X_APL, sizeof(FD_DOT1X_APL));
      break;
    case ACCESS_LINE_CONSOLE:
    case ACCESS_LINE_TELNET:
    case ACCESS_LINE_SSH:
    case ACCESS_LINE_CTS:
      /* The line has to be CTS for Console/Telnet/SSH as login and enable are
         applicable to any of these lines */
      if (level == ACCESS_LEVEL_LOGIN)
      {
        if (index < sizeof(fd_login_apls) / sizeof(fd_login_apls[0]))
        {
          memcpy(&userMgrCfgData.authListInfo.loginAuthLists[index], &fd_login_apls[index], sizeof(fd_login_apls[0]));
        }
        else
        {
          authList = &userMgrCfgData.authListInfo.loginAuthLists[index];
          memset(authList->name, 0, sizeof(authList->name));
          maxNumMethods = L7_LOGIN_MAX_AUTH_METHODS;
        }
      }
      else
      {
        if (index == 0)
        {
          memcpy(&userMgrCfgData.authListInfo.enableAuthLists[0], &FD_ENABLE_APL, sizeof(FD_ENABLE_APL));
        }
        else
        {
          authList = &userMgrCfgData.authListInfo.enableAuthLists[index];
          memset(authList->name, 0, sizeof(authList->name));
          maxNumMethods = L7_ENABLE_MAX_AUTH_METHODS;
        }
      }
      break;
    default:
      return;
      break;
  }

  for (i = 0; i < maxNumMethods; i++)
  {
    authList->methodList[i] = L7_AUTH_METHOD_UNDEFINED;
  }

  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return;
}

/*********************************************************************
* @purpose  Notify interested third parties of changes to user db
*
* @param    category   type of change (USER, AUTH, or ENCRYPT)
* @param    index       table index of changed entry
* @param    changeType add, modify, delete
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes Eventually, listeners will register.  For now, listeners
*        are hard coded.
* @notes There is no handling for a failure of user login changes
*        communicated to cliWeb.  For this reason, there is the possibility of the
*        user databases becoming out of sync.
*
* @end
*********************************************************************/
L7_RC_t
userMgrNotifyListeners(L7_uint32 category, L7_uint32 index, L7_uint32 changeType)
{
  switch (category)
  {
    case L7_USER_MGR_USER_LISTENER:
      /* Notify snmp */
      if (SNMP_USER_SUPPORTED == L7_TRUE )
      {
        if (changeType == L7_USER_MGR_CHANGE_USER_DELETE)
        {
          return userMgrSnmpUserDelete(index);
        }
        else
        {
          return userMgrSnmpUserSet(index);
        }
      }
      break;
    case L7_USER_MGR_AUTH_LISTENER:
      /* Notify snmp */
      if (SNMP_AUTH_SUPPORTED == L7_TRUE )
        return userMgrSnmpUserSet(index);
      break;
    case L7_USER_MGR_ENCRYPT_LISTENER:
      /* Notify snmp */
      if (SNMP_ENCRYPT_SUPPORTED == L7_TRUE )
        return userMgrSnmpUserSet(index);
      break;
    default:
      break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Dump contents of structures for debug
*
* @notes
*
* @end
*********************************************************************/
void dumpUserMgrStructs(void)
{
  L7_uint32 i,j;

  printf("\nUser logins:");
  for (i=0; i<L7_MAX_LOGINS; i++)
  {
    printf("\nUser %i: ", i);
    printf("%s ", userMgrCfgData.systemLogins[i].loginName );

    for (j=0; j<L7_APL_COMPONENT_COUNT; j++)
    {
      printf("[%s] ", userMgrCfgData.systemLogins[i].userAuth[j].APLName );
    }
  }
  printf("\n");

  printf("\nAPL list:");

	printf("\n loginAuthLists:");
  for (i=0; i<L7_MAX_LOGIN_AUTHLIST_COUNT; i++)
  {
    printf("%s ", userMgrCfgData.authListInfo.loginAuthLists[i].name);
  }
  printf("\n enableAuthLists:");
  for (i=0; i<L7_MAX_ENABLE_AUTHLIST_COUNT; i++)
  {
    printf("%s ", userMgrCfgData.authListInfo.enableAuthLists[i].name);
  }
  printf("\n httpAuthLists:");
  for (i=0; i<L7_MAX_HTTP_AUTHLIST_COUNT; i++)
  {
    printf("%s ", userMgrCfgData.authListInfo.httpAuthLists[i].name);
  }
  printf("\n httpsAuthLists:");
  for (i=0; i<L7_MAX_HTTPS_AUTHLIST_COUNT; i++)
  {
    printf("%s ", userMgrCfgData.authListInfo.httpsAuthLists[i].name);
  }
  printf("\n dot1xAuthLists:");
  for (i=0; i<L7_MAX_DOT1X_AUTHLIST_COUNT; i++)
  {
    printf("%s ", userMgrCfgData.authListInfo.dot1xAuthLists[i].name);
  }

  printf("\n authListName:\n         Login   Enable");
  for (i=0; i<L7_ACCESS_LINE_NUM; i++)
  {
    switch (i)
    {
      case ACCESS_LINE_UNKNOWN:printf("\n UNKNOWN:");break;
      case ACCESS_LINE_CONSOLE:printf("\n CONSOLE:");break;
      case ACCESS_LINE_TELNET:printf("\n TELNET:");break;
      case ACCESS_LINE_SSH:printf("\n SSH:");break;
      case ACCESS_LINE_HTTP:printf("\n HTTP:");break;
      case ACCESS_LINE_HTTPS:printf("\n HTTPS:");break;
      case ACCESS_LINE_DOT1X:printf("\n DOT1X:");break;
      case ACCESS_LINE_CTS:printf("\n CTS:");break;
    }
    for (j=0; j<L7_ACCESS_LEVEL_NUM; j++)
    {
      printf("%s ", userMgrCfgData.authListInfo.authListName[i][j]);
    }
  }

  printf("\nNonConfiguredUser APL:");

  for (i=0; i<L7_APL_COMPONENT_COUNT; i++)
  {
    printf("\nComponent %i: ", userMgrCfgData.nonConfiguredUserAuth[i].componentId );
    printf("%s ", userMgrCfgData.nonConfiguredUserAuth[i].APLName  );
  }
  printf("\n");

  return;
}

/*********************************************************************
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
void userMgrConvertToLowerCase(L7_char8 *buf)
{
  L7_char8 c_tmp;
  L7_uint32 i;
  for ( i = 0; i < strlen(buf); i++ )
  {
    if ( buf[i] != '\n' || buf[i] != '\0' )
    {
      c_tmp = (L7_char8)tolower(buf[i]);
      buf[i] = c_tmp;
    }
  }
  return;
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
L7_BOOL userMgrNoCaseCompare(L7_char8 *buf1, L7_char8 *buf2)
{
  L7_char8 lowerCaseBuf1[ L7_CLI_MAX_STRING_LENGTH +1];
  L7_char8 lowerCaseBuf2[ L7_CLI_MAX_STRING_LENGTH +1];

  /* First try compare without converting strings. */
  if (strcmp(buf1, buf2) == 0)
    return L7_TRUE;

  /* Make sure buffers are large enough */
  if (strlen(buf1) > sizeof(lowerCaseBuf1) || strlen(buf2) > sizeof(lowerCaseBuf2))
    return L7_FALSE;

  memset( lowerCaseBuf1, 0, sizeof(lowerCaseBuf1));
  osapiStrncpySafe( lowerCaseBuf1, buf1, sizeof(lowerCaseBuf1));
  userMgrConvertToLowerCase( lowerCaseBuf1 );

  memset( lowerCaseBuf2, 0, sizeof(lowerCaseBuf2));
  osapiStrncpySafe( lowerCaseBuf2, buf2, sizeof(lowerCaseBuf2));
  userMgrConvertToLowerCase( lowerCaseBuf2 );

  if (strcmp( lowerCaseBuf1, lowerCaseBuf2) == 0)
    return L7_TRUE;

  return L7_FALSE;
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
L7_BOOL userMgrCaseCompare(L7_char8 *buf1, L7_char8 *buf2)
{
  if (strcmp(buf1, buf2) == 0)
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Set password lockout counts fro all logins to Factory Defaults
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void
userMgrConfigPasswordLockoutFactoryDefault(void)
{
  L7_uint32 i;
  L7_BOOL val;
  for (i = 0; i < FD_USER_MGR_LOGINS_SIZE; i++)
  {
    userMgrCfgData.systemLogins[i].passwdLockoutCount = 0;

    if ((userMgrUserLockStatusGet(i, &val) == L7_SUCCESS) &&
        (val == L7_TRUE))
    {
      userMgrLoginUserBlockStatusSet(i,L7_FALSE);
    }
  }
}

/*
 * For DEBUG
 */

void debugPasswordConfTimeSet(L7_uint32 index, L7_uint32 time)
{
  if (index < FD_USER_MGR_LOGINS_SIZE)
  {
    userMgrCfgData.systemLogins[index].passwdTimeStamp = time;
  }
}

void debugPasswordEncrypt(L7_char8 *password)
{
  char key[L7_ENCRYPTED_PASSWORD_SIZE];
  memset(key, 0, sizeof(key));
  pwEncrypt(password, key, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);

  printf("\n key = %s ",key);
}


/*********************************************************************
* @purpose  Reverse the given string
*
* @param    str (input/output) - String
*
* @returns  L7_SUCCESS on success 
*           L7_FAILURE on failure
*
* @notes    The function reverse the given string to str
*
* @end
*********************************************************************/
L7_RC_t userMgrStringReverse(L7_char8 *str)
{
  L7_uint32 len=L7_NULL,
            ct;
  L7_char8  tmpCharBuf;

  if(str == L7_NULLPTR) 
  {
    return L7_FAILURE;
  }
  
  len = osapiStrnlen(str, L7_CLI_MAX_STRING_LENGTH);

  for (ct = 0; ct < len/2; ct++)
  {
    tmpCharBuf = str[ct];
    str[ct] = str[len - ct - 1];
    str[len - ct - 1] = tmpCharBuf;
  }

  return L7_SUCCESS;
}

/******************************************************************************
 * @purpose To compare two strings and check if the source string is present in
 *          the destination string
 *
 * @param   dstString - (input) Destination String is to compare for srcString
 *          srcString - (input) Source String is to be search for
 *
 * @returns L7_SUCCESS - if srcString is present in the dstString in any form
 *                       (case in-sensitive or reverse or substring) 
 *
 * @returns L7_FAILURE - if srcString is not present in the dstString (or)
 *                       src string length is greater than dst string length
 *
 * @returns L7_ERROR - if src or dst string are NULL pointers (or)
 *                     failed to Reverse the string
 *
 * @notes  
 * @end
 ******************************************************************************/
L7_RC_t userMgrStrNoCaseReverseCompareCheck(L7_char8 *dstString, L7_char8 *srcString)  
{
  L7_uint32 srcStrLen = L7_NULL;
  L7_uint32 dstStrLen = L7_NULL;
  L7_char8  srcLowercaseStr[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_char8  dstLowercaseStr[L7_CLI_MAX_STRING_LENGTH + 1]={0};
  L7_char8  *ptr;

  if((srcString == L7_NULLPTR) || (dstString == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  srcStrLen = osapiStrnlen(srcString, L7_CLI_MAX_STRING_LENGTH);
  dstStrLen = osapiStrnlen(dstString, L7_CLI_MAX_STRING_LENGTH);

  if(srcStrLen > dstStrLen)
  {
    return L7_FAILURE;
  }
     
  if(osapiStrncmp(dstString, srcString, srcStrLen) == 0)
  {
    return L7_SUCCESS;
  }

  osapiStrncpySafe(srcLowercaseStr, srcString, srcStrLen+1);
  userMgrConvertToLowerCase(srcLowercaseStr);

  osapiStrncpySafe(dstLowercaseStr, dstString, dstStrLen+1);
  userMgrConvertToLowerCase(dstLowercaseStr);

  /* check if srcString is present in dstString */
  ptr = osapiStrStr(dstLowercaseStr, srcLowercaseStr);   
  if((ptr != L7_NULLPTR) &&
     (osapiStrncmp(ptr, srcLowercaseStr, srcStrLen) == 0))
  {
    return L7_SUCCESS;
  }

  /* Reverse the string */
  if(userMgrStringReverse(srcLowercaseStr) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
 
  ptr = osapiStrStr(dstLowercaseStr, srcLowercaseStr);   
  if((ptr != L7_NULLPTR) &&
     (osapiStrncmp(ptr, srcLowercaseStr, srcStrLen) == 0))
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/******************************************************************************
 * @purpose To check for Repeated characters in the given string
 *
 * @param   string - (input) String 
 *          maxNum - (input) Maximum Number of repeated characters 
 *
 * @returns L7_SUCCESS - if string contains maxNum of repeated characters
 *
 * @returns L7_FAILURE - if string doesn't contain maxNum of repeated characters
 *
 * @returns L7_ERROR - if the given string is NULL pointer
 *
 * @notes  
 * @end
 ******************************************************************************/
L7_RC_t userMgrStrMaxNumRepeatedCharsCheck(L7_char8 *string, L7_ushort16 maxNum)
{
  L7_uint32 len=0,index;
  L7_uint32 repeat = 0;

  if(string == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  len = osapiStrnlen(string, L7_CLI_MAX_STRING_LENGTH);

  for(index=0; index < len; index++)
  {
      /* check for repeated characters */
    if(string[index] == string[index+1])
    {
      repeat++;
    }
    else if((repeat != 0) && (string[index] == string[index-1]))
    {
      repeat++;
      if((maxNum != 0) && (repeat > maxNum))
      {
        return L7_SUCCESS;
      }
      repeat=0;
    }
  }
  return L7_FAILURE;
}

/******************************************************************************
 * @purpose To check for Consecutive characters in the given string
 *
 * @param   string - (input) String 
 *          maxNum - (input) Maximum Number of consecutive characters 
 *
 * @returns L7_SUCCESS - if string contains maxNum of consecutive characters
 *
 * @returns L7_FAILURE - if string doesn't contain maxNum of consecutive characters
 *
 * @returns L7_ERROR - if the given string is NULL pointer
 *
 * @notes  
 * @end
 ******************************************************************************/
L7_RC_t userMgrStrMaxNumConsecutiveCharsCheck(L7_char8 *string, L7_ushort16 maxNum)
{
  L7_uint32 len=0,index;
  L7_uint32 consCt = 0;

  if(string == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  len = osapiStrnlen(string, L7_CLI_MAX_STRING_LENGTH);

  for(index=0; index < len; index++)
  {
    if(osapiIsAlnum(string[index]))
    { 
      /* check for consecutive characters */
      if(((index+1) < len) && (osapiIsAlnum(string[index+1])) 
          && (string[index]+1 == string[index+1]))
      {
        consCt++;
      }
      else if((consCt != 0) && (string[index] == string[index-1]+1))
      {
        consCt++;
        if((maxNum != 0) && (consCt > maxNum))
        {
          return L7_SUCCESS;
        }
        consCt=0;
      }
    }
  }
  
  return L7_FAILURE;
}

/**************************************************************************
*
* @purpose  The function shall test whether the value is a printable string
*
* @param    string  @b{(input)}  String
*
* @returns  L7_SUCCESS if string is a printable character; 
*           L7_FAILURE otherwise.
*
* @end
*
*************************************************************************/
L7_RC_t userMgrStringPrintableCheck(L7_char8 *string)
{
  L7_uint32 len,index;
  
  if(string == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  len = osapiStrnlen(string, L7_CLI_MAX_STRING_LENGTH);

  for(index =0; index < len; index++)
  {
    if(osapiIsPrint(string[index]) == L7_NULL)
    {
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  the function sort the strings in the given string list
*
* @param    strlist  @b{(input)}  string list
*           maxlen   #b{(input)}  max length of a string
*           count    #b{(input)}  number of strings in the given string list
*
* @returns  l7_success sort the list successfully
*           l7_failure if given strlist or maxlen or count is null
*
* @end
*
*************************************************************************/
L7_RC_t userMgrStringListSort(L7_char8 strlist[][L7_PASSWD_EXCLUDE_KEYWORD_MAX_LEN], 
                              L7_uint32 maxLen, 
                              L7_uint32 count)
{
  L7_uint32 i,j;
  L7_char8  temp[L7_CLI_MAX_STRING_LENGTH]={0};

  if(strlist == L7_NULLPTR || maxLen == L7_NULL || count == L7_NULL)
  {
    return L7_FAILURE;
  }

  for(i=0; i <= count; i++)
  {
    for(j=i+1; j <= count; j++)
    {
      if(memcmp(strlist[i], strlist[j], maxLen)>0)
      {
        memcpy(temp, strlist[i], maxLen);
        memcpy(strlist[i],strlist[j],maxLen);
        memcpy(strlist[j],temp,maxLen);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Validate restricted characters in the user password
 *
 * @param    password   @b{(input)} pointer to login user password
 * @param    encrypted  @b{(input)} indicates whether the password param 
 *                                  is in encrypted form
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if password is invalid
 *           L7_USER_MGR_PASSWD_INVALID_QUOTATION_CHAR if password has 
 *                            quotation as a character
 *           L7_USER_MGR_PASSWD_INVALID_QUESTIONMARK_CHAR if password has
 *                            question mark as a character
 *
 * @notes    none
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdRestrictedCharactersCheck(L7_char8 *password, L7_BOOL encrypted)
{
  L7_uchar8 newPasswd[L7_PASSWORD_SIZE]={0};
  L7_uint32 len, ct;

  if(encrypted == L7_TRUE)
  {
    if(pwDecrypt(newPasswd, password, L7_PASSWORD_SIZE-1) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    memcpy(newPasswd, password, osapiStrnlen(password, L7_PASSWORD_SIZE));
  }
  
  len = osapiStrnlen(newPasswd, sizeof(newPasswd));

  for(ct=0; ct < len; ct++)
  {
    if(newPasswd[ct] == 0x22)   /* Special Character " */
    {
      return L7_USER_MGR_PASSWD_INVALID_QUOTATION_CHAR; 
    } 
    else if(newPasswd[ct] == 0x3f)  /* Special character ? */
    {
      return L7_USER_MGR_PASSWD_INVALID_QUESTIONMARK_CHAR; 
    }
  }

  return L7_SUCCESS;
}



