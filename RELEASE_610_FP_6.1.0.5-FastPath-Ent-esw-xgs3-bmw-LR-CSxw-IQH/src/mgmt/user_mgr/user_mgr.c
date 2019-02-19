/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  user_mgr.c
*
* @purpose   user configuration manager api functions (used by cli, web, snmp)
*
* @component userMgr component
*
* @comments  none
*
* @create    09/26/2002
*
* @author    Jill Flanagan
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "snmp_exports.h"
#include "cli_web_exports.h"
#include "user_manager_exports.h"
#include "user_mgr.h"
#include "user_mgr_apl.h"
#include "usmdb_snmp_api.h"
#include "pw_scramble_api.h"
#include "user_mgr_util.h"
#include "user_mgr_migrate.h"
#include "user_mgr_radius.h"
#include "user_mgr_tacacs.h"
#include "log.h"
#include "cli_web_mgr_api.h"
#include "cli_web_user_mgmt.h"
#include "trapapi.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"

userMgrCfgData_t userMgrCfgData;

static L7_BOOL userMgrReadyFlag = L7_FALSE;

extern userMgrPortUserCfg_t **userMgrPortOper_g;

/*********************************************************************
* @purpose  Returns login user name per index
*
* @param    index    table index
* @param    name     pointer to login user name
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserNameGet(L7_uint32 index, L7_char8 *name)
{
  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  osapiStrncpySafe(name, userMgrCfgData.systemLogins[index].loginName, L7_LOGIN_SIZE);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns table index of Login user name
*
* @param    name     pointer to login user name
* @param    index    pointer to table index
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginIndexGet(L7_char8 *name, L7_uint32 *index)
{
  L7_uint32 i, length;

  if (name == NULL)
  {
    return(L7_FAILURE);
  }

  length = strlen(name);
  /* First check whether the argument name string
   * has some value inside it or not
   */
  if (length == 0)
  {
    return(L7_FAILURE);
  }

  for (i = 0; i < L7_MAX_LOGINS; i++)
  {
    if (length == strlen(userMgrCfgData.systemLogins[i].loginName))
    {
      if (L7_TRUE == userMgrNoCaseCompare (name ,userMgrCfgData.systemLogins[i].loginName))
        break;
    }
  }

  if (i == L7_MAX_LOGINS)
    return(L7_FAILURE);

  *index = i;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns table index of next available (vacant) table index
*
* @param    index    pointer to table index
*
* @returns  L7_SUCCESS or L7_FAILURE if no available index exists
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginAvailableIndexGet(L7_uint32 *index)
{
  L7_uint32 i;
  for (i=0; i < L7_MAX_LOGINS; i++)
  {
    if (strlen(userMgrCfgData.systemLogins[i].loginName) == 0)
      break;
  }

  if (i == L7_MAX_LOGINS)
    return(L7_FAILURE);

  *index = i;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets a login user name per index
*
* @param    index       table index
* @param    commName    pointer to Community name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if name is too long or
*                      if name contains an invalid character
*
* @notes    also sets login status to L7_enable
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserNameSet(L7_uint32 index, L7_char8 *name)
{
  L7_char8 prevName[L7_LOGIN_SIZE];
  L7_uint32 oldIndex;

  if (index >= L7_MAX_LOGINS || strlen(name) >= L7_LOGIN_SIZE ||
      usmDbStringAlphaNumericCheck(name) != L7_SUCCESS )
    return(L7_FAILURE);

  osapiStrncpySafe( prevName, userMgrCfgData.systemLogins[index].loginName, L7_LOGIN_SIZE );

  /* Check if this name already exists for another user */

  if (userMgrLoginIndexGet( name, &oldIndex) == L7_SUCCESS &&
      oldIndex != index )
  {
    return L7_FAILURE;
  }

  /* On a user name change, notify any interested listeners*/
  if ((prevName[0] != 0) &&
      (userMgrNoCaseCompare( prevName, name) != L7_TRUE) &&
      userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_DELETE ) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  osapiStrncpySafe( userMgrCfgData.systemLogins[index].loginName, name, L7_LOGIN_SIZE );

  if ( userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_ADD ) != L7_SUCCESS )
  {
    /* attempt to restore and return failure. */
    osapiStrncpySafe( userMgrCfgData.systemLogins[index].loginName, prevName, L7_LOGIN_SIZE );
    userMgrSnmpUserSet(index);
    return L7_FAILURE;
  }

  userMgrCfgData.systemLogins[index].loginStatus = L7_ENABLE;
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns login user password per index
*
* @param    index       table index
* @param    password    pointer to login user password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserPasswordGet(L7_uint32 index, L7_char8 *password)
{
  if (index >= L7_MAX_USERS_LOGINS)
    return(L7_FAILURE);
  osapiStrncpySafe(password, userMgrCfgData.systemLogins[index].password, L7_ENCRYPTED_PASSWORD_SIZE);
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns encrypted user password
*
* @param    password    input - pointer to clear text password
* @param    encryptedpwd    output = pointer to AES encrypted password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd)
{
  return pwEncrypt(password, encryptedpwd, L7_PASSWORD_SIZE - 1, L7_PASSWORD_ENCRYPT_ALG);
}

/*********************************************************************
* @purpose  Returns login user password per index
*
* @param    index       table index
* @param    password    pointer to login user password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserPasswordClearTextGet(L7_uint32 index, L7_char8 *password)
{
  if (index >= L7_MAX_USERS_LOGINS)
    return(L7_FAILURE);
  if (strlen(userMgrCfgData.systemLogins[index].password) == 0)
  {
    *password = '\0';
    return L7_SUCCESS;
  }
  return pwDecrypt(password, userMgrCfgData.systemLogins[index].password, L7_PASSWORD_SIZE-1);
}

/*********************************************************************
* @purpose  Sets a login user password per index
*
* @param    index       table index
* @param    password    pointer to login user password
* @param    encrypted   indicates whether the password param is in encrypted form
*
* @returns  L7_SUCCESS
*           L7_FAILURE if password is too long or index is too large
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted)
{
  L7_char8 oldPassword[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uint32   count;

  if (index >= L7_MAX_USERS_LOGINS)
    return(L7_FAILURE);

  osapiStrncpySafe( oldPassword, userMgrCfgData.systemLogins[index].password, L7_ENCRYPTED_PASSWORD_SIZE);

  if (encrypted == L7_TRUE)
  {
    osapiStrncpySafe(userMgrCfgData.systemLogins[index].password, password, L7_ENCRYPTED_PASSWORD_SIZE);
  }
  else
  {
    pwEncrypt(password, userMgrCfgData.systemLogins[index].password, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
  }
  /*
    If the Passwd history is enabled, add the passwd to the history table
    if that was not already added.So before adding just make sure that it is
    already not there and then insert into the history table.
  */

  if (userMgrHistoryStatusGet () == L7_SUCCESS)
  {
    for (count=0;
         count < (userMgrCfgData.systemLogins[index].passwdHistoryCount);
         count++)
    {
      if (userMgrCfgData.systemLogins[index].passwdHistory[count] == NULL)
      {
        break;
      }

      if (userMgrNoCaseCompare(userMgrCfgData.systemLogins[index].passwdHistory[count],
                               userMgrCfgData.systemLogins[index].password) != L7_TRUE)
      {
        continue;
      }

      osapiStrncpySafe(userMgrCfgData.systemLogins[index].password, oldPassword,
                       L7_ENCRYPTED_PASSWORD_SIZE);
      LOG_MSG("User %s attemping to reuse an old password\n", userMgrCfgData.systemLogins[index].loginName);
      return L7_ERROR;

    }

    if (userMgrCfgData.systemLogins[index].passwdHistoryCount
        < userMgrCfgData.passwdHistoryLength)
    {
      osapiStrncpySafe (userMgrCfgData.systemLogins[index].passwdHistory[count],
                        userMgrCfgData.systemLogins[index].password,
                        L7_ENCRYPTED_PASSWORD_SIZE);
      userMgrCfgData.systemLogins[index].passwdHistoryCount++;
    }
    else
    {
      /*
        If the history table is full,remove the oldest password entry from the
        history table and then insert the new entry at the end.
      */
      for (count=0;count<((userMgrCfgData.systemLogins[index].passwdHistoryCount-1) );count++)
      {
        osapiStrncpySafe ((L7_char8 *)(userMgrCfgData.systemLogins[index].passwdHistory[count]),
                          (L7_char8 *)(userMgrCfgData.systemLogins[index].passwdHistory[count + 1]),
                          L7_ENCRYPTED_PASSWORD_SIZE);
      }
      osapiStrncpySafe ((L7_char8 *)(userMgrCfgData.systemLogins[index].passwdHistory[count]),
                        (L7_char8 *)(userMgrCfgData.systemLogins[index].password),
                        L7_ENCRYPTED_PASSWORD_SIZE);
    }
  }

  userMgrCfgData.systemLogins[index].passwdTimeStamp = osapiUTCTimeNow();

  if (userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
                              index,
                              L7_USER_MGR_CHANGE_USER_MODIFY ) != L7_SUCCESS )
  {
    osapiStrncpySafe( userMgrCfgData.systemLogins[index].password, oldPassword, L7_ENCRYPTED_PASSWORD_SIZE);
    return L7_FAILURE;
  }

  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns login user status per index
*
* @param    index    table index
* @param    status   pointer to login user status (L7_ENABLE of L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserStatusGet(L7_uint32 index, L7_uint32 *status)
{
  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  *status = userMgrCfgData.systemLogins[index].loginStatus;

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets the status of the login user
*
* @param    index       table index
* @param    password    pointer to login user password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserStatusSet(L7_uint32 index, L7_uint32 status)
{

  L7_BOOL change = L7_FALSE;
  L7_uint32 priorStatus;

  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  priorStatus = userMgrCfgData.systemLogins[index].loginStatus;

  if (status != priorStatus)
  {
    change = L7_TRUE;
  }

  userMgrCfgData.systemLogins[index].loginStatus = status;

  if ( userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_MODIFY ) != L7_SUCCESS )
  {
    userMgrCfgData.systemLogins[index].loginStatus = priorStatus;
    return L7_FAILURE;
  }

  if (change == L7_TRUE)
  {
    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return(L7_SUCCESS);

}

/*********************************************************************
* @purpose  Resets user logins back to Factory Defaults
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void userMgrUserLoginsFactoryDefaultSet(void)
{
  userMgrConfigLoginsFactoryDefault();
}


/*********************************************************************
 * @purpose  Sets a login user authentication protocol
 *
 * @param    index       table index
 * @param    authProt    authentication protocol (none, md5, or sha)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAuthenticationSet(L7_uint32 index, L7_uint32 authProt)
{
  L7_uint32 prevAuthType;

  if (index >= L7_MAX_LOGINS || SNMP_AUTH_SUPPORTED == L7_FALSE )
    return(L7_FAILURE);

  prevAuthType =  userMgrCfgData.systemLogins[index].authProt;

  switch( authProt )
  {
    case L7_SNMP_USER_AUTH_PROTO_HMACMD5: /* fallthrough */
    case L7_SNMP_USER_AUTH_PROTO_HMACSHA: /* fallthrough */
    case L7_SNMP_USER_AUTH_PROTO_NONE:
      userMgrCfgData.systemLogins[index].authProt = authProt;
      break;

    default:  /* unrecognized authentication protocol */
      return L7_FAILURE;
  }

  /* pass changes to listeners */

  if ( userMgrNotifyListeners( L7_USER_MGR_AUTH_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_MODIFY ) != L7_SUCCESS )
  {
    userMgrCfgData.systemLogins[index].authProt = prevAuthType;
    return L7_FAILURE;
  }

  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}


/*********************************************************************
 * @purpose  Sets a login user encryption protocol and key
 *
 * @param    index       table index
 * @param    encryptProt encryption protocol (none, md5, or sha)
 * @param    encryptKey  encryption key (ignored if prot = none)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    User mgr will scramble the provided key before storing
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserEncryptionSet(L7_uint32 index, L7_uint32 encryptProt,
    L7_char8 *encryptKey)
{
  L7_uint32 prevEncryptProt;
  L7_char8  prevEncryptKey[L7_ENCRYPTION_KEY_SIZE];

  if (index >= L7_MAX_LOGINS ||
      SNMP_ENCRYPT_SUPPORTED == L7_FALSE ||
      strlen(encryptKey) >= L7_ENCRYPTION_KEY_SIZE )
  {
    return(L7_FAILURE);
  }

  memset( prevEncryptKey, 0, sizeof(prevEncryptKey));
  osapiStrncpySafe( prevEncryptKey, userMgrCfgData.systemLogins[index].encryptKey, L7_ENCRYPTION_KEY_SIZE );
  prevEncryptProt = userMgrCfgData.systemLogins[index].encryptProt;

  memset( userMgrCfgData.systemLogins[index].encryptKey, 0, L7_ENCRYPTION_KEY_SIZE );

  switch( encryptProt )
  {
    case L7_SNMP_USER_PRIV_PROTO_DES:
      pwScrambleLength(encryptKey, userMgrCfgData.systemLogins[index].encryptKey,
      L7_ENCRYPTION_KEY_SIZE);
      /* fallthrough */

    case L7_SNMP_USER_PRIV_PROTO_NONE:
      userMgrCfgData.systemLogins[index].encryptProt = encryptProt;
      break;

    default:  /* unrecognized encryption protocol */
      return L7_FAILURE;
  }

  /* pass changes to interested listeners*/

  if ( userMgrNotifyListeners( L7_USER_MGR_ENCRYPT_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_MODIFY ) != L7_SUCCESS )
  {
    userMgrCfgData.systemLogins[index].encryptProt= prevEncryptProt;
    osapiStrncpySafe(userMgrCfgData.systemLogins[index].encryptKey, prevEncryptKey, L7_ENCRYPTION_KEY_SIZE);
    return L7_FAILURE;
  }
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}


/*********************************************************************
 * @purpose  Sets a login user access level
 *
 * @param    index       table index
 * @param    accessLevel L7_LOGIN_ACCESS_READ_ONLY or L7_LOGIN_ACCESS_READ_WRITE
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAccessModeSet(L7_uint32 index, L7_uint32 accessLevel )
{
  L7_uint32 priorAccesslevel;

  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  priorAccesslevel = userMgrCfgData.systemLogins[index].accessMode;

  userMgrCfgData.systemLogins[index].accessMode = accessLevel;

  if ( userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_MODIFY ) != L7_SUCCESS )
  {
    userMgrCfgData.systemLogins[index].accessMode = priorAccesslevel;
    return L7_FAILURE;
  }

  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}


/*********************************************************************
 * @purpose  Sets the snmpv3 accessmode for a login user
 *
 * @param    index       table index
 * @param    accessLevel L7_LOGIN_ACCESS_READ_ONLY or L7_LOGIN_ACCESS_READ_WRITE
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserSnmpv3AccessModeSet(L7_uint32 index, L7_uint32 accessLevel )
{
  L7_uint32 priorAccessLevel;

  if (index >= L7_MAX_LOGINS || SNMP_USER_SUPPORTED == L7_FALSE )
    return(L7_FAILURE);

  priorAccessLevel = userMgrCfgData.systemLogins[index].snmpv3AccessMode;

  switch (accessLevel)
  {
    case L7_SNMP_USER_ACCESS_LEVEL_READ_ONLY:
      /* fallthrough */
    case L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE:
      userMgrCfgData.systemLogins[index].snmpv3AccessMode = accessLevel;
      break;
    default:  /* unrecognized access level */
      return L7_FAILURE;
  }

  /* pass the changes to snmp*/

  if (userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_MODIFY ) != L7_SUCCESS )
  {
    userMgrCfgData.systemLogins[index].snmpv3AccessMode = priorAccessLevel;
    return L7_FAILURE;
  }
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Return a login user authentication protocol and key
 *
 * @param    index       (input) table index
 * @param    authProt    (outupt) authentication protocol (none, md5, or sha)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAuthenticationGet(L7_uint32 index, L7_uint32 *authProt)
{

  if (index >= L7_MAX_USERS_LOGINS)
    return(L7_FAILURE);

  *authProt = userMgrCfgData.systemLogins[index].authProt;
  return L7_SUCCESS;
}


/*********************************************************************
 * @purpose  Returns a login user encryption protocol and key
 *
 * @param    index       table index
 * @param    encryptProt (output) encryption protocol (none, md5, or sha)
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserEncryptionGet(L7_uint32 index, L7_uint32 *encryptProt)
{

  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  *encryptProt = userMgrCfgData.systemLogins[index].encryptProt;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Returns a login user encryption key
*
* @param    index       table index
* @param    encryptKey (output) encryption Key
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 userMgrLoginUserEncryptKeyGet(L7_uint32 index, L7_char8 *encryptKey)
{
  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  pwUnscrambleLength(userMgrCfgData.systemLogins[index].encryptKey, encryptKey,
                     L7_ENCRYPTION_KEY_SIZE);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Return a login user cli/web access level
 *
 * @param    index       (input) table index
 * @param    accessLevel (output) L7_LOGIN_ACCESS_READ_ONLY or L7_LOGIN_ACCESS_READ_WRITE
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserAccessModeGet(L7_uint32 index, L7_uint32 *accessLevel )
{

  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  *accessLevel = userMgrCfgData.systemLogins[index].accessMode;
  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Return a login user snmpv3 access level
 *
 * @param    index       (input) table index
 * @param    accessLevel (output) L7_LOGIN_ACCESS_READ_ONLY or L7_LOGIN_ACCESS_READ_WRITE
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserSnmpv3AccessModeGet(L7_uint32 index, L7_uint32 *accessLevel )
{

  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  *accessLevel = userMgrCfgData.systemLogins[index].snmpv3AccessMode;
  return(L7_SUCCESS);
}
/*********************************************************************
 * @purpose  Delete a login user name and settings
 *
 * @param    index       table index
 *
 * @returns  L7_SUCCESS
 * @returns L7_FAILURE if attempt to remove admin
 *
 * @notes    also sets login status to L7_disable
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserDelete(L7_uint32 index)
{
  if (index >= L7_MAX_LOGINS || userMgrCfgData.systemLogins[index].deleteMode == L7_FALSE)
    return(L7_FAILURE);

  if ( userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_DELETE ) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  userMgrInitialize(index);

  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}


/*********************************************************************
 * @purpose  Saves cli_web user config file to NVStore
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrSave(void)
{
  if (userMgrCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    userMgrCfgData.cfgHdr.dataChanged = L7_FALSE;
    userMgrCfgData.checkSum = nvStoreCrc32((L7_char8 *)&userMgrCfgData,
    sizeof(userMgrCfgData) - sizeof(userMgrCfgData.checkSum));

    /* call save NVStore routine */
    if (sysapiCfgFileWrite(L7_USER_MGR_COMPONENT_ID,
      USER_MGR_CFG_FILENAME,
      (L7_char8 *)&userMgrCfgData,
      sizeof (userMgrCfgData)) == L7_ERROR)
    {
      LOG_MSG("Error on call to osapiFsWrite routine on config file %s\n", USER_MGR_CFG_FILENAME);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Checks if cli_web user config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL userMgrHasDataChanged(void)
{
  return userMgrCfgData.cfgHdr.dataChanged;
}
void userMgrResetDataChanged(void)
{
  userMgrCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
 * @purpose  Print the current config values to
 *           serial port
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrConfigDump(void)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_char8 buf[66];
  L7_uint32 i;
  L7_uint32 j, count;

  /*------------------------------------------------------*/
  /* Dump logins structure
   */
  /*------------------------------------------------------*/
  printf("Logins: \n");
  printf("\n  L7_MAX_LOGINS = %d ", L7_MAX_LOGINS);
  for(i=0; i<L7_MAX_LOGINS; i++)
  {
    printf("\nLogin Name - %s\n",userMgrCfgData.systemLogins[i].loginName);


    /* Do NOT print sensitive info on shipping product. Only allow this for
       internal testing */
    /*

       printf("\n  L7_ENCRYPTED_PASSWORD_SIZE = %d ", L7_ENCRYPTED_PASSWORD_SIZE);
       printf("\nPassword - **************** REMOVE *********************  " );
       printf("\nPassword - **************** REMOVE *********************  " );
       printf("\nPassword - **************** REMOVE *********************  " );
       for (j=0; j<L7_ENCRYPTED_PASSWORD_SIZE; j++)
       {
       printf("[%x] ", userMgrCfgData.systemLogins[i].password[j] );
       }
     */

    printf("\nAccess Mode = %x ", userMgrCfgData.systemLogins[i].accessMode);
    printf("\nLogin Status = %x ", userMgrCfgData.systemLogins[i].loginStatus);


    printf("\nSNMP fields " );
    printf("\n authProt = %x ", userMgrCfgData.systemLogins[i].authProt);
    printf("\n encryptProt = %x ", userMgrCfgData.systemLogins[i].encryptProt);


    printf("\n encryptKey = " );
    for (j=0; j<L7_ENCRYPTION_KEY_SIZE; j++)
    {
      printf("[%x] ", userMgrCfgData.systemLogins[i].encryptKey[j] );
    }
    printf("\n snmpv3AccessMode = %x ", userMgrCfgData.systemLogins[i].snmpv3AccessMode);

    for (j=0; j<L7_APL_COMPONENT_COUNT; j++)
    {
      printf("[%s] ", userMgrCfgData.systemLogins[i].userAuth[j].APLName );
    }

    if (userMgrCfgData.systemLogins[i].accessMode == L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE)
      sprintf(buf,"%s","L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE");
    else
      sprintf(buf,"%s","L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY");
    printf("Access Mode - %s\n",buf);

    if (userMgrCfgData.systemLogins[i].loginStatus == L7_ENABLE)
      sprintf(buf,"%s","L7_ENABLE");
    else
      sprintf(buf,"%s","L7_DISABLE");
    printf("Login Status - %s\n",buf);

    if (userMgrCfgData.systemLogins[i].blockStatus == L7_TRUE)
      sprintf(buf,"%s","L7_ENABLE");
    else
      sprintf(buf,"%s","L7_DISABLE");
    printf("Lock Status - %s\n",buf);

    printf("Password lock count - %u\n", userMgrCfgData.systemLogins[i].passwdLockoutCount);
    printf("Total password failures - %u\n", userMgrCfgData.systemLogins[i].passwdFailureTotal);
    printf("Number of account lockouts - %u\n", userMgrCfgData.systemLogins[i].passwdNumLockouts);
    printf("Password history count - %u\n", userMgrCfgData.systemLogins[i].passwdHistoryCount);
    printf("Password timestamp - %d\n", userMgrCfgData.systemLogins[i].passwdTimeStamp);

    printf("Password History:\n");
    for (count=0; count < userMgrCfgData.systemLogins[i].passwdHistoryCount; count++)
    {
      printf("%s\n",userMgrCfgData.systemLogins[i].passwdHistory[count]);
    }
  }

  /*------------------------------------------------------*/
  /* Dump user_component_auth_t structure
   */
  /*------------------------------------------------------*/

  printf("user_component_auth_t: \n");
  printf("\n  L7_APL_COMPONENT_COUNT = %d ", L7_APL_COMPONENT_COUNT);
  for (i=0; i < L7_APL_COMPONENT_COUNT; i++)
  {
    printf("\n APLName [%s] ", userMgrCfgData.nonConfiguredUserAuth[i].APLName );
    printf("\n APLName [%d] ", userMgrCfgData.nonConfiguredUserAuth[i].componentId );
  }

  /*------------------------------------------------------*/
  /* Dump userMgrPortUserCfg_t structure
   */
  /*------------------------------------------------------*/

  dumpUserMgrPortStruct();

  /*------------------------------------------------------*/
  /* Dump APL_t structure
   */
  /*------------------------------------------------------*/

#if 0
  printf("APL_t: \n");
  printf("\n  L7_MAX_APL_COUNT = %d ", L7_MAX_APL_COUNT);
  for (i=0; i < L7_MAX_APL_COUNT; i++)
  {
    printf("\n  L7_MAX_APL_NAME_SIZE = %d ", L7_MAX_APL_NAME_SIZE);
    printf("\n APLName [%s] ", userMgrCfgData.APLs[i].name );


    printf("\n  L7_MAX_AUTH_METHODS = %d ", L7_MAX_AUTH_METHODS);
    printf("\n  methodList = ");
    for (j=0; j < L7_MAX_AUTH_METHODS; j++)
    {
      printf("[%i] ", userMgrCfgData.APLs[i].methodList[j] );
    }
  }
#endif

  return rc;
}

/*********************************************************************
 *
 * @purpose  Dump the contents of the config data.
 *
 * @param    void
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void userMgrConfigDataTestShow(void)
{
  L7_fileHdr_t  *pFileHdr;
  L7_uint32 i;

  /*-----------------------------*/
  /* Config File Header Contents */
  /*-----------------------------*/
  pFileHdr = &userMgrCfgData.cfgHdr;
  sysapiPrintf("pFileHdr->filename    : %s\n", (char *)pFileHdr->filename);
  sysapiPrintf("pFileHdr->version     : %u\n", pFileHdr->version);
  sysapiPrintf("pFileHdr->componentID : %u\n", pFileHdr->componentID);
  sysapiPrintf("pFileHdr->type        : %u\n", pFileHdr->type);
  sysapiPrintf("pFileHdr->length      : %u\n", pFileHdr->length);
  sysapiPrintf("pFileHdr->dataChanged : %u\n", pFileHdr->dataChanged);

  /* Start of release I file header changes */

  sysapiPrintf("pFileHdr->savePointId : %u\n", pFileHdr->savePointId);
  sysapiPrintf("pFileHdr->targetDevice: %u\n", pFileHdr->targetDevice);
  for (i = 0; i < L7_FILE_HDR_PAD_LEN; i++)
  {
    sysapiPrintf("pFileHdr->pad[i]      : %u\n", pFileHdr->pad[i]);
  }

  /*-----------------------------*/
  /* cfgParms                    */
  /*-----------------------------*/
  userMgrConfigDump();

  /*-----------------------------*/
  /* Checksum                    */
  /*-----------------------------*/
  sysapiPrintf("userMgrCfgData.checkSum : %u\n", userMgrCfgData.checkSum);
}

/*********************************************************************
 * @purpose  Build test  config data
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void userMgrBuildTestConfigData(void)
{
  L7_uint32 i,j;

  /* Ensure at least one entry in each array has a unique value to validate
     accurate migration */

  /*------------------------------------------------------*/
  /* Build logins structure                               */
  /*------------------------------------------------------*/

  for (i=0; i < L7_MAX_USERS_LOGINS; i++)
  {
    osapiStrncpySafe( userMgrCfgData.systemLogins[i].loginName, "TEST", L7_LOGIN_SIZE );

    userMgrCfgData.systemLogins[i].accessMode = L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE;
    userMgrCfgData.systemLogins[i].authProt   = L7_SNMP_USER_AUTH_PROTO_NONE;
    osapiStrncpySafe( userMgrCfgData.systemLogins[i].encryptKey, "TEST", L7_ENCRYPTION_KEY_SIZE );
    userMgrCfgData.systemLogins[i].encryptProt  = L7_SNMP_USER_PRIV_PROTO_DES;
    userMgrCfgData.systemLogins[i].loginStatus  = L7_ENABLE;
    userMgrCfgData.systemLogins[i].snmpv3AccessMode  = L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE;

    for (j = 0; j < L7_APL_COMPONENT_COUNT; j++)
    {
      userMgrCfgData.systemLogins[i].userAuth[j].componentId = 4 + j + i;
      osapiStrncpySafe( userMgrCfgData.systemLogins[i].userAuth[j].APLName, "APLTEST", L7_MAX_APL_NAME_SIZE );
    }
  }

  /*------------------------------------------------------*/
  /* Build user_component_auth_t structure                */
  /*------------------------------------------------------*/

  for (i = 0; i < L7_APL_COMPONENT_COUNT; i++)
  {
    userMgrCfgData.nonConfiguredUserAuth[i].componentId = 4;
    osapiStrncpySafe( userMgrCfgData.nonConfiguredUserAuth[i].APLName, "APLTEST", L7_MAX_APL_NAME_SIZE );
  }

  /*------------------------------------------------------*/
  /* Build user_component_auth_t structure                */
  /*------------------------------------------------------*/

  for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    userMgrCfgData.portData[i].portUsers = 0xFFFFFF00 + i;

    nimConfigIdGet(i,&userMgrCfgData.portData[i].configID);
  }

  /*------------------------------------------------------*/
  /* Build user_component_auth_t structure                */
  /*------------------------------------------------------*/

#if 0
  for (i = 0; i < L7_MAX_APL_COUNT; i++)
  {
    osapiStrncpySafe( userMgrCfgData.APLs[i].name, "APLTEST", L7_MAX_APL_NAME_SIZE );
    for (j = 0; j < L7_MAX_AUTH_METHODS; j++)
    {
      userMgrCfgData.APLs[i].methodList[j] = 0xAB + j + i;
    }
  }
#endif

  /* Force write of config file */
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  sysapiPrintf("Built test config data\n");
}

/*********************************************************************
 * @purpose  Build test config data
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void userMgrBuildTestIntfConfigData(void)
{
  L7_uint32 i;

  /*------------------------------------------------------*/
  /* Build user_component_auth_t structure                */
  /*------------------------------------------------------*/

  for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++)
  {
    userMgrCfgData.portData[i].portUsers = 0xABCDABCD + i ;

    (void)nimConfigIdGet(i,&userMgrCfgData.portData[i].configID);
  }

  /* Force write of config file */
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  sysapiPrintf("Built test config data\n");
}

/*********************************************************************
 * @purpose  Enables the user manager component to be reset by changing
 *           the user manager ready flag to false.
 *
 * @returns  L7_SUCCESS
 *
 * @notes    Intended to be called before userMgrSysInit in case there
 *           is ever a need to re-initialize the user manager.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPrepareToReset(void)
{
  userMgrReadyFlag = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Indicates when the user manager has completed loading config
 *
 * @returns  L7_SUCCESS  If userMgr has completed loading config
 * @returns  L7_FAILURE  If userMgr has NOT completed loading config
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL userMgrReady(void)
{
  return userMgrReadyFlag;
}

/*********************************************************************
 * @purpose  Restores the users into snmpv3 tables
 *
 * @returns  L7_SUCCESS  Retrored successfully
 * @returns  L7_FAILURE  Retore failed
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrSnmpUsersRestore(void)
{
  if (userMgrReady() == L7_TRUE)
  {
    return userMgrRestoreSnmpUsers();
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Indicates when the user manager has completed loading config
 *
 * @returns  L7_SUCCESS  If userMgr has completed loading config
 * @returns  L7_FAILURE  If userMgr has NOT completed loading config
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrSnmpConfigSet(void)
{
  if (userMgrReady() == L7_TRUE)
  {
    return userMgrInitSnmpUsers();
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   request (input/output) pointer to structure containing
 *          data used for authentication.  Refer to the definition
 *          of the data structure for more information about each
 *          field in the structure.
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with the line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This version of the interface does NOT support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUser(userMgrAuthRequest_t *request)
{
  L7_uchar8 state[ L7_USER_MGR_STATE_FIELD_SIZE ];
  L7_char8 challengePhrase[L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH];

  memset(state, 0, sizeof(state));
  memset(challengePhrase, 0, sizeof(challengePhrase));

  request->mayChallenge     = L7_FALSE;
  request->pState           = state;
  request->isChallenged     = L7_FALSE;
  request->pChallengePhrase = challengePhrase;
  request->challengeFlags   = 0;

  return userMgrAuthenticateUserAllowChallenge(request);
}

/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   request (input/output) pointer to structure containing
 *          data used for authentication.  Refer to the definition
 *          of the data structure for more information about each
 *          field in the structure.
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with the line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This version of the interface DOES support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUserAllowChallenge(userMgrAuthRequest_t *request)
{
  L7_char8 aplName[L7_MAX_APL_NAME_SIZE + 1];
  int i;
  L7_USER_MGR_AUTH_METHOD_t method;
  L7_uint32 rc = L7_ERROR;  /* authentication unknown */
  L7_BOOL authenticationResolved = L7_FALSE;

  request->isChallenged   = L7_FALSE;
  request->challengeFlags = 0;
  memset(aplName, 0, sizeof(aplName));
  /* Get the APL for this line */
  if (userMgrAPLLineGet(request->line, request->mode, aplName) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_USER_MGR_COMPONENT_ID,
            "Failed to get APL for line %d, mode %d.", request->line, request->mode);
    return L7_NOT_SUPPORTED;
  }

  for (i = 0; i < L7_MAX_AUTH_METHODS; i++)   /* check all methods until success or fail */
  {
    if (userMgrAPLAuthMethodGet(request->line, request->mode, aplName, i, &method) == L7_FAILURE)
    {
      /* Should not occur.  TO DO: Log failure message. */
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_USER_MGR_COMPONENT_ID,
              "Failed to get authentication method %d for line %d, mode %d.", i, request->line, request->mode);
      return L7_FAILURE;
    }
    switch (method)
    {
      case L7_AUTH_METHOD_UNDEFINED:
        /* skip and go to next method */
        break;

      case L7_AUTH_METHOD_LOCAL:
        /* attempt local authentication */
        rc = userMgrAuthenticateUserLocal(request->pUserName, request->pPwd, &request->accessLevel);
        if (L7_SUCCESS == rc || L7_FAILURE == rc)
        {
          authenticationResolved = L7_TRUE;
        }
        break;
      case L7_AUTH_METHOD_NONE:
        /* no authentication required - give full access */
        rc = L7_SUCCESS;
        authenticationResolved = L7_TRUE;
        request->accessLevel = FD_USER_MGR_ADMIN_ACCESS_LEVEL;
        break;

      case L7_AUTH_METHOD_LINE:
        /* attempt line authentication */
        rc = userMgrAuthenticateUserLine(request->line, request->pPwd);
        if (L7_SUCCESS == rc || L7_FAILURE == rc)
        {
          authenticationResolved = L7_TRUE;
        }
        break;

      case L7_AUTH_METHOD_ENABLE:
        /* attempt enable authentication */
        rc = userMgrAuthenticateUserEnable(request->accessLevel, request->pPwd);
        if (L7_SUCCESS == rc || L7_FAILURE == rc)
        {
          authenticationResolved = L7_TRUE;
        }
        break;

      case L7_AUTH_METHOD_RADIUS:
        /* use the username "$enabx$" where x is the privilege level, right now x is always 15 */
        if (request->mode == ACCESS_LEVEL_ENABLE)
        {
          osapiStrncpy(request->pUserName, L7_USER_MGR_ENABLE_USER_STRING, L7_LOGIN_SIZE);
        }
        /* attempt authentication with RADIUS */
        rc = userMgrAuthenticateUserRadius(request->pUserName, request->pPwd,
                                           request->pState, &request->accessLevel,
                                           &request->isChallenged,
                                           request->pChallengePhrase);
        if (L7_SUCCESS == rc || L7_FAILURE == rc)
        {
          authenticationResolved = L7_TRUE;
        }
        /*
         * if rc == L7_ERROR, this indicates a timeout and the
         * next method in the APL should be attempted.
         */
        break;

#ifdef L7_TACACS_PACKAGE
      case L7_AUTH_METHOD_TACACS:
        /* attempt authentication with TACACS+ */
        rc = userMgrAuthenticateUserTacacs(request);

        if (( L7_SERVICE_AUTHEN == request->servType) && (ACCESS_LEVEL_LOGIN == request->mode) &&
           (( L7_LOGIN_TYPE_HTTP == request->line) || ( L7_LOGIN_TYPE_HTTPS == request->line)) &&
            (L7_SUCCESS == rc) )
        {
           (void)userMgrCmdAuthorTacacs(request); /* not interested in return value for now */
        }

        if (L7_SUCCESS == rc || L7_FAILURE == rc)
        {
          authenticationResolved = L7_TRUE;
        }
        /*
         * if rc == L7_ERROR, this indicates a timeout and the
         * next method in the APL should be attempted.
         */
        break;
#endif  /* L7_TACACS_PACKAGE */

      case L7_AUTH_METHOD_REJECT:
        /* this user is never authenticated */
        rc = L7_FAILURE;
        authenticationResolved = L7_TRUE;
        break;
      default:
        /* try the next method */
        break;
    }

    if (L7_TRUE == authenticationResolved)
    {
      break;
    }
  }

  if ((rc == L7_FAILURE) &&
      (L7_TRUE != request->isChallenged))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USER_MGR_COMPONENT_ID, 
              "User %s Failed to login because of authentication failures", request->pUserName);
    trapMgrFailedUserLoginTrap(request->pUserName);
  }

  return rc;
}

/*********************************************************************
* @purpose  Determine if authentication is required (configured)
*
* @param   line (input) line application requesting authentication
* @param   mode (input) access level of application's auth sequence
*
* @returns L7_TRUE  If authentication is required, or other error
* @returns L7_FALSE  If authentication is not required, allow anyone access
*
* @notes   The APL associated with this user is used to determine the
*          appropriate authentication method(s) for this user.  If the
*          user is not configured locally, the APL for non-configured users
*          is consulted to determine handling.
* @end
*********************************************************************/
L7_BOOL userMgrIsAuthenticationRequired(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t mode)
{
  L7_char8 aplName[L7_MAX_APL_NAME_SIZE +1];
  L7_uint32 method;
  L7_BOOL rc = L7_TRUE;  /* authentication unknown */

  memset(aplName, 0, sizeof(aplName));
  /* Get the APL for this line */
  if (userMgrAPLLineGet(line, mode, aplName) == L7_FAILURE)
    return rc;

  /* we only care about the first method at index 0 */
  if (userMgrAPLAuthMethodGet(line, mode, aplName, 0, &method) == L7_FAILURE)
  {
    return rc;
  }

  if (method == L7_AUTH_METHOD_NONE)
    return L7_FALSE;

  return rc;
}

/*********************************************************************
* @purpose  Determine if authentication requirs a username
*
* @param   line (input) line application requesting authentication
* @param   mode (input) access level of application's auth sequence
*
* @returns L7_TRUE  If username is required, or other error
* @returns L7_FALSE  If username is not required, allow anyone access
*
* @notes   The APL associated with this user is used to determine the
*          appropriate authentication method(s) for this user.  If the
*          user is not configured locally, the APL for non-configured users
*          is consulted to determine handling.
* @end
*********************************************************************/
L7_BOOL userMgrIsAuthenticationUsernameRequired(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t mode)
{
  L7_char8 aplName[L7_MAX_APL_NAME_SIZE +1];
  L7_uint32 method;
  L7_BOOL rc = L7_TRUE;  /* authentication unknown */

  memset(aplName, 0, sizeof(aplName));
  /* Get the APL for this line */
  if (userMgrAPLLineGet(line, mode, aplName) == L7_FAILURE)
    return rc;

  /* we only care about the first method at index 0 */
  if (userMgrAPLAuthMethodGet(line, mode, aplName, 0, &method) == L7_FAILURE)
  {
    return rc;
  }

  if ((method == L7_AUTH_METHOD_NONE) ||
      (method == L7_AUTH_METHOD_ENABLE) ||
      (method == L7_AUTH_METHOD_LINE))
    return L7_FALSE;

  return rc;
}

/*********************************************************************
* @purpose  Determine if authentication method prompts for input
*
* @param   line (input) line application requesting authentication
* @param   mode (input) access level of application's auth sequence
*
* @returns L7_TRUE   If authentication method provides prompts
* @returns L7_FALSE  If authentication does not prompt
*
* @notes   The APL associated with this line is used to determine the
*          appropriate authentication method(s).
*
* @end
*********************************************************************/
L7_BOOL userMgrIsAuthenticationPromptSupplied(L7_ACCESS_LINE_t line, L7_ACCESS_LEVEL_t mode)
{
  L7_char8 aplName[L7_MAX_APL_NAME_SIZE +1];
  L7_uint32 method;
  L7_BOOL rc = L7_FALSE;  /* authentication unknown */

  memset(aplName, 0, sizeof(aplName));
  /* Get the APL for this line */
  if (userMgrAPLLineGet(line, mode, aplName) == L7_FAILURE)
    return rc;

  /* we only care about the first method at index 0 */
  if (userMgrAPLAuthMethodGet(line, mode, aplName, 0, &method) == L7_FAILURE)
  {
    return rc;
  }

  /*
   * Hard coded for now, should abstract this and make it an attribute of the method.
   */
  if (method == L7_AUTH_METHOD_TACACS)
  {
/*
 * This feature is not yet supported.
 *
    return L7_TRUE;
*/
  }

  return rc;
}

/*********************************************************************
 * @purpose  Attempt to authenticate a user with local configuration
 *
 * @param   pUserName (input) ptr to user name to authenticate
 * @param   pPwd (input) ptr to user provided password
 * @param   pAccessLevel (output) access of authenticated user
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, or system error
 *
 * @notes Local authentication does not incorporate the concept of timeout.
 *        All requests will either succeed or fail.
 *
 * @notes User login status must be set to enable for local authentication to succeed.
 *
 * @notes If the user access level can not be determined, READ_ONLY access will be set
 *        and a message will be logged.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUserLocal(L7_char8 *pUserName, L7_char8 *pPwd, L7_uint32 *pAccessLevel)
{
  L7_uint32 index;
  L7_char8 pass[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_uint32 status;
  L7_RC_t   rc;
  L7_BOOL val;

  /* Allow "null" Password in case of "no password min-length" - Defect 72365 Fix */
#if 0
  if (pPwd == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
#endif /* 0 */

  memset(pass, 0, sizeof(pass));

  /* Get the index into the login structure and the password for this user */

  if (userMgrLoginIndexGet(pUserName, &index) == L7_FAILURE ||
      userMgrLoginUserPasswordGet(index, pass) == L7_FAILURE ||
      userMgrLoginUserStatusGet(index, &status) == L7_FAILURE)
    return L7_FAILURE;

  /* if password lockout enabled and user has reached the limit, just fail */
  if ((userMgrLockoutStatusGet() == L7_SUCCESS) &&
      (userMgrUserLockStatusGet(index, &val) == L7_SUCCESS) &&
      (val == L7_TRUE))
  {
    return L7_FAILURE;
  }

  if ((cliWebLoginUserPasswordIsValid(pass, pPwd, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS) &&
      (status == L7_ENABLE))
  {
    if (userMgrLoginUserAccessModeGet(index, pAccessLevel) == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_USER_MGR_COMPONENT_ID, "Access level for user %s could not be "
                "determined.  Setting to READ_ONLY. Invalid access level specified for the user. The "
                "access level is set to READ_ONLY. %s indicates the username.\n", pUserName, pUserName );
      *pAccessLevel =  L7_LOGIN_ACCESS_READ_ONLY;
    }
    /* reset lockout count to 0 on a successful login */
    userMgrCfgData.systemLogins[index].passwdLockoutCount = 0;
    return L7_SUCCESS;
  }

  if (status == L7_ENABLE)
  {

    rc = userMgrLockoutStatusGet();
    if (rc == L7_SUCCESS)
    {
      /* add a check to see if user has already been locked out */
      /* this is possible during ssh login attempts, where the number of Login attempts in ssh is
         greater than the maximum lockout count */
      if ((userMgrUserLockStatusGet(index, &val) == L7_SUCCESS) &&
          (val != L7_TRUE))
      {
        userMgrCfgData.systemLogins[index].passwdLockoutCount++;
        userMgrCfgData.systemLogins[index].passwdFailureTotal++;

        if (userMgrCfgData.systemLogins[index].passwdLockoutCount == userMgrCfgData.userLockoutCount)
        {
          userMgrCfgData.systemLogins[index].passwdNumLockouts++;
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_USER_MGR_COMPONENT_ID, 
                    "User %s locked out on authentication failure", userMgrCfgData.systemLogins[index].loginName);
          userMgrLoginUserBlockStatusSet(index, L7_TRUE);
          trapMgrLockedUserLoginTrap(pUserName);
        }
      }
      return L7_FAILURE;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Attempt to authenticate a user with line configuration
*
* @param   pUserName (input) ptr to user name to authenticate
* @param   pPwd (input) ptr to user provided password
* @param   pAccessLevel (output) access of authenticated user
*
* @returns L7_SUCCESS  If user is authenticated
* @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, or system error
*
* @notes Local authentication does not incorporate the concept of timeout.
*        All requests will either succeed or fail.
*
* @notes User login status must be set to enable for local authentication to succeed.
*
* @notes If the user access level can not be determined, READ_ONLY access will be set
*        and a message will be logged.
*
* @end
*********************************************************************/
L7_RC_t userMgrAuthenticateUserLine(L7_ACCESS_LINE_t line, L7_char8 *pPwd)
{
  L7_char8 pass[L7_ENCRYPTED_PASSWORD_SIZE];

  if (pPwd == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  memset(pass, 0, sizeof(pass));

  /* Get the password and then validate */

  if (userMgrLinePasswordGet(line, pass) == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  /* if line password has not been set, we should allow empty password */
  if (strlen(pass) == 0 && strlen(pPwd) == 0)
  {
    return L7_SUCCESS;
  }

  if (cliWebLoginUserPasswordIsValid(pass, pPwd, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS)
  {
      return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  Attempt to authenticate a user with enable configuration
 *
 * @param   pPwd (input) ptr to user provided password
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password, or system error
 *
 * @notes Local authentication does not incorporate the concept of timeout.
 *        All requests will either succeed or fail.
 *
 * @notes User login status must be set to enable for local authentication to succeed.
 *
 * @notes If the user access level can not be determined, READ_ONLY access will be set
 *        and a message will be logged.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrAuthenticateUserEnable(L7_uint32 level, L7_char8 *pPwd)
{
  L7_char8 pass[L7_ENCRYPTED_PASSWORD_SIZE];

  if (pPwd == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  memset(pass, 0, sizeof(pass));

  /* Get the password and then validate */

  if (userMgrEnablePasswordGet(level, pass) == L7_FAILURE)
  {
    return L7_FAILURE;
  }
  /* if enable password has not been set, we should allow empty password */
  if (strlen(pass) == 0 && strlen(pPwd) == 0)
  {
    return L7_SUCCESS;
  }

  if (cliWebLoginUserPasswordIsValid(pass, pPwd, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}


/*********************************************************************
 * @purpose Initialize the userMgr for Phase 1
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 1 completed
 * @returns L7_FAILURE  Phase 1 incomplete
 *
 * @notes  If phase 1 is incomplete, it is up to the caller to call the fini
 *         function if desired.  If this fails, it is due to an inability to
 *         to acquire resources.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPhaseOneInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* config data initialized to zero */
  memset(&userMgrCfgData, 0, sizeof(userMgrCfgData_t));

  /* inialize/allocate the oper port data */
  userMgrPortOper_g = osapiMalloc(L7_USER_MGR_COMPONENT_ID, L7_MAX_INTERFACE_COUNT * sizeof(userMgrPortUserCfg_t *));

  if (userMgrPortOper_g == L7_NULLPTR)
  {
    LOG_ERROR(L7_NULL);
    rc = L7_FAILURE;
  }
  else
  {
    memset(userMgrPortOper_g, 0, L7_MAX_INTERFACE_COUNT * sizeof(userMgrPortUserCfg_t *));

    /* reset the ready flag: QSCAN (Maybe not needed any more) */
    userMgrReadyFlag = L7_FALSE;

    if (userMgrRadiusPhaseOneInit() != L7_SUCCESS)
    {
      rc = L7_FAILURE;
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose Initialize the userMgr for Phase 2
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 2 completed
 * @returns L7_FAILURE  Phase 2 incomplete
 *
 * @notes  If phase 2 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPhaseTwoInit(void)
{
  L7_RC_t rc;
  nvStoreFunctionList_t notifyFunctionList; /* nvstore Functions' Data Structure */

  /* nvstore Functions' Data Structure initialized to zero */
  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));

  notifyFunctionList.registrar_ID   = L7_USER_MGR_COMPONENT_ID;
  notifyFunctionList.notifySave     = userMgrSave;
  notifyFunctionList.hasDataChanged = userMgrHasDataChanged;
  notifyFunctionList.notifyConfigDump     = userMgrConfigDump;
  notifyFunctionList.resetDataChanged = userMgrResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else if (userMgrRadiusPhaseTwoInit() != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
#ifdef L7_TACACS_PACKAGE
  else if (userMgrTacacsPhaseTwoInit() != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
#endif
  else if ((rc = nimRegisterIntfChange(L7_USER_MGR_COMPONENT_ID, userMgrIntfStateChangeCallback)) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
 * @purpose Initialize the userMgr for Phase 3
 *
 * @param   void
 *
 * @returns L7_SUCCESS  Phase 3 completed
 * @returns L7_FAILURE  Phase 3 incomplete
 *
 * @notes  If phase 3 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPhaseThreeInit(void)
{
  L7_RC_t rc;

  rc = sysapiCfgFileGet(L7_USER_MGR_COMPONENT_ID, USER_MGR_CFG_FILENAME,
      (L7_char8 *)&userMgrCfgData, sizeof(userMgrCfgData),
      &userMgrCfgData.checkSum, USER_MGR_CFG_VER_CURRENT,
      userMgrBuildDefaultConfigData, userMgrMigrateConfigData);
  if (rc != L7_SUCCESS )
  {
    rc = L7_FAILURE;
  }
  else
  {
    /* User Manager data is now available */
    userMgrReadyFlag = L7_TRUE;
    userMgrCfgData.cfgHdr.dataChanged = L7_FALSE;
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Release all resources collected during phase 1
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void userMgrPhaseOneFini(void)
{
  /* config data initialized to zero */
  memset(&userMgrCfgData, 0, sizeof(userMgrCfgData_t));

  if (userMgrPortOper_g != L7_NULLPTR)
  {
    osapiFree(L7_USER_MGR_COMPONENT_ID, userMgrPortOper_g);
  }

  /* reset the ready flag: QSCAN (Maybe not needed any more) */
  userMgrReadyFlag = L7_FALSE;

  userMgrRadiusPhaseOneFini();
}

/*********************************************************************
 * @purpose  Phase userMgr to configurator phase unconfigurator
 *
 * @returns  void
 *
 * @notes    Nothing to apply for config, only reset the memory
 *
 * @end
 *********************************************************************/
void userMgrUnconfigure(void)
{
  /* config data initialized to zero */
  memset(&userMgrCfgData, 0, sizeof(userMgrCfgData_t));

  if (userMgrPortOper_g != L7_NULLPTR)
  {
    memset(userMgrPortOper_g, 0, L7_MAX_INTERFACE_COUNT * sizeof(userMgrPortUserCfg_t *));
  }

  return;
}

/*********************************************************************
 * @purpose Free the resource for phase 2
 *
 * @param   void
 *
 * @returns void
 *
 * @notes  If phase 2 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
void userMgrPhaseTwoFini(void)
{
  nvStoreFunctionList_t notifyFunctionList; /* nvstore Functions' Data Structure */

  /* nvstore Functions' Data Structure initialized to zero */
  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));

  /* de-register by setting the values to zero and registering again */
  (void)nvStoreRegister(notifyFunctionList);

  (void)userMgrRadiusPhaseTwoFini();

#ifdef L7_TACACS_PACKAGE
  (void)userMgrTacacsPhaseTwoFini();
#endif

  (void)nimRegisterIntfChange(L7_USER_MGR_COMPONENT_ID, L7_NULLPTR);

  return;
}

/*********************************************************************
 * @purpose Reset the userMgr to prior to phase 3
 *
 * @param   void
 *
 * @returns void
 *
 * @notes  If phase 3 is incomplete, it is up to the caller to call the fini
 *         function if desired
 *
 * @end
 *********************************************************************/
void userMgrPhaseThreeFini(void)
{
  /* config data initialized to zero */
  memset(&userMgrCfgData, 0, sizeof(userMgrCfgData_t));

  return;
}

/*********************************************************************
 * @purpose To configure the min password length
 *
 * @param val -Min password length
 *
 * @returns L7_SUCCESS -if the value supplied is correct
*
 * @returns L7_FAILURE -if the value supplied is not correct
*
 * @notes This function is used to configure the min password length and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrMinPasswdSet(L7_uint32 val)
{
  L7_BOOL change = L7_FALSE;
  L7_uint32 priorStatus;

  priorStatus = userMgrCfgData.minPasswdLength;

  if (val != priorStatus)
  {
    change = L7_TRUE;
  }

  if(val == L7_DISABLE)
  {
    userMgrCfgData.minPasswdLength = L7_MIN_PASSWORD_SIZE;
    if (change == L7_TRUE)
    {
      userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    return L7_SUCCESS;
  }

  if ((val < L7_MIN_PASSWORD_SIZE) || (val >= L7_PASSWORD_SIZE))
    return L7_FAILURE;

  userMgrCfgData.minPasswdLength = val;
  if (change == L7_TRUE)
  {
    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To check whether the given password is of valid length or not
 *
 * @param  password - Password of the user
 *
 * @returns L7_SUCCESS -if the password is valid
*
 * @returns L7_FAILURE -if the password is not valid
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdLengthValid(L7_char8 *password, L7_BOOL encryptedFlag)
{
  L7_ushort16  lenOfPasswd;
  if(password == NULL)
  {
    return L7_FAILURE;
  }

  if (strlen(password) < userMgrCfgData.minPasswdLength) 
    return L7_FAILURE;

   if (L7_TRUE == encryptedFlag)
     lenOfPasswd = L7_ENCRYPTED_PASSWORD_SIZE;
   else
     lenOfPasswd = L7_PASSWORD_SIZE;
     
    if (strlen (password) >= lenOfPasswd)
      return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To get the min password length value
 *
 * @param   val - (output) min number of characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrMinPassLengthGet(L7_ushort16 *val)
{
  *val = userMgrCfgData.minPasswdLength;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To know whether the min password feature is enabled or not
*
* @param   none
*
* @returns  L7_SUCCESS if enabled
*           L7_FAILURE  if not enabled
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t userMgrMinPassStatusGet(void)
{
  if(userMgrCfgData.minPasswdLength > 0)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose To configure the History table size
 *
 * @param val -History table size
 *
 * @returns L7_SUCCESS -if the value supplied is correct
*
 * @returns L7_FAILURE -if the value supplied is not correct
*
 * @notes This function is used to configure the history table size and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdHistoryLengthSet(L7_uint32 val)
{
  L7_uint32 priorStatus, index, count;

  priorStatus = userMgrCfgData.passwdHistoryLength;

  if((val < L7_MIN_HIST_LENGTH)||(val > L7_MAX_HIST_LENGTH ))
    return L7_FAILURE;

  userMgrCfgData.passwdHistoryLength = val;

  if (val != priorStatus)
  {
    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  /* store the current password for each user in the history table */
  if (val > 0)
  {
    for (index=0; index < L7_MAX_LOGINS; index++)
    {
      for (count=0;
           count < (userMgrCfgData.systemLogins[index].passwdHistoryCount);
           count++)
      {
        if (userMgrCfgData.systemLogins[index].passwdHistory[count] == NULL)
        {
          break;
        }
      }

      if (userMgrCfgData.systemLogins[index].passwdHistoryCount
          < userMgrCfgData.passwdHistoryLength)
      {
        osapiStrncpySafe (userMgrCfgData.systemLogins[index].passwdHistory[count],
                          userMgrCfgData.systemLogins[index].password,
                          L7_ENCRYPTED_PASSWORD_SIZE);
        userMgrCfgData.systemLogins[index].passwdHistoryCount++;
      }
      else
      {
        /*
          If the history table is full,remove the oldest password entry from the
          history table and then insert the new entry at the end.
        */
        for (count=0;count<((userMgrCfgData.systemLogins[index].passwdHistoryCount-1) );count++)
        {
          osapiStrncpySafe ((L7_char8 *)(userMgrCfgData.systemLogins[index].passwdHistory[count]),
                            (L7_char8 *)(userMgrCfgData.systemLogins[index].passwdHistory[count + 1]),
                            L7_ENCRYPTED_PASSWORD_SIZE);
        }
        osapiStrncpySafe ((L7_char8 *)(userMgrCfgData.systemLogins[index].passwdHistory[count]),
                          (L7_char8 *)(userMgrCfgData.systemLogins[index].password),
                          L7_ENCRYPTED_PASSWORD_SIZE);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To know whether the password history is enabled or not
 *
 * @param   none
 *
 * @returns  L7_SUCCESS if enabled
 *           L7_FAILURE  if not enabled
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrHistoryStatusGet(void)
{
  if(userMgrCfgData.passwdHistoryLength > 0)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  To get the history length value
 *
 * @param   val - (output) num passwords to store for prevention of reuse
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrHistoryLengthGet(L7_ushort16 *val)
{
  *val = userMgrCfgData.passwdHistoryLength;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To configure the lockout count for the password
 *
 * @param val -indiactes the lock out count of the password
 *
 * @returns L7_SUCCESS -if the value supplied is correct
*
 * @returns L7_FAILURE -if the value supplied is not correct
*
 * @notes This function is used to configure the lockout count of the password
 *         and to disble the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdsLockoutSet(L7_uint32 val)
{
  L7_BOOL change = L7_FALSE;
  L7_uint32 priorStatus;

  priorStatus = userMgrCfgData.userLockoutCount;

  if (val != priorStatus)
  {
    change = L7_TRUE;
  }

  if (val == L7_DISABLE)
  {
    userMgrCfgData.userLockoutCount = L7_DISABLE;
    /* Defect 49921: password lockout no longer functions after being disabled then re-enabled*/
    /* reset the password lockout count for each of the local users */
    userMgrConfigPasswordLockoutFactoryDefault();

    if (change == L7_TRUE)
    {
      userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    return L7_SUCCESS;
  }

  if ((val < L7_MIN_LOCKOUT_COUNT) ||
      (val > L7_MAX_LOCKOUT_COUNT))
  {
    return L7_FAILURE;
  }

  if (change == L7_TRUE)
  {
    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  userMgrCfgData.userLockoutCount = val;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To know whether the user lockout is enabled or not
 *
 * @param   none
 *
 * @returns  L7_SUCCESS if enabled
 *           L7_FAILURE  if not enabled
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrLockoutStatusGet(void)
{
  if(userMgrCfgData.userLockoutCount > 0)
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  To get the no of user lockout attempts
 *
 * @param   val - (output) num failed attempts allowed before lockout
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrLockoutAttemptsGet(L7_ushort16 *val)
{
  *val = userMgrCfgData.userLockoutCount;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To make the locked user unlocked
 *
 * @param none
 *
 * @returns L7_SUCCESS  -user is unlocked
*
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrUserUnLock(L7_uint32 index)
{
  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  userMgrCfgData.systemLogins[index].passwdLockoutCount = 0;
  userMgrLoginUserBlockStatusSet(index, L7_FALSE);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To know whether the user is been locked or not
 *
 * @param  index - (input) index for user in login structure
 * @param  val   - (output) whether the user account is locked out
 *
 * @returns L7_SUCCESS  value retrieved
 * @returns L7_FAILURE  value not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrUserLockStatusGet(L7_uint32 index, L7_BOOL *val)
{
  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  *val = userMgrCfgData.systemLogins[index].blockStatus;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To configure the aging value of the password
 *
 * @param val -indiactes for how many days the password is valid
 *
 * @returns L7_SUCCESS -if the value supplied is correct

 * @returns L7_FAILURE -if the value supplied is not correct

 * @notes This function is used to configure the aging of the password and to
 *        disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdAgingSet(L7_uint32 val)
{
  L7_uint32    priorValue;
  L7_BOOL      change = L7_FALSE;

  priorValue = userMgrCfgData.passwdAgingDays;

  if (val != priorValue)
  {
    change = L7_TRUE;
  }

  if (val == L7_DISABLE)
  {
    userMgrCfgData.passwdAgingDays= L7_DISABLE;
    if (change == L7_TRUE)
    {
      userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
    }
    return L7_SUCCESS;
  }

  if((val < L7_MIN_PASSWD_AGING)||(val > L7_MAX_PASSWD_AGING ))
    return L7_FAILURE;


  userMgrCfgData.passwdAgingDays= val;
  if (change == L7_TRUE)
  {
    userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To know whether the aging is enabled or disabled
 *
 * @param none
 *
 * @returns L7_SUCCESS -enabled

 * @returns L7_FAILURE -disabled

 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdAgingStatusGet()
{
  if(userMgrCfgData.passwdAgingDays > 0)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
 * @purpose  To get the password aging value
 *
 * @param   val - (output) number of days the passwords are valid
 *
 * @returns L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdAgingValueGet(L7_ushort16 *val)
{
  *val = userMgrCfgData.passwdAgingDays;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To get the time when the password was created
 *
 * @param index -indiactes the corresponding user
 *
 * @output time - time when the password was set
 * @returns L7_SUCCESS -if the user index valid

 * @returns L7_FAILURE -if the user index is not valid

 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswordChangeTimeGet(L7_uint32 index, L7_uint32 *time)
{
  if(time == NULL)
    return L7_FAILURE;

  if (index >= L7_MAX_LOGINS)
    return L7_FAILURE;

  *time = userMgrCfgData.systemLogins[index].passwdTimeStamp;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose To get the aging count of the password
 *
 * @param index -indiactes the corresponding user
 *
 * @output time - time when the password was set
 * @returns L7_SUCCESS -if the user index valid

 * @returns L7_FAILURE -if the user index is not valid

 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswdValidationTimeGet(L7_uint32 *tempInt)
{
  if(tempInt == NULL)
    return L7_FAILURE;

  *tempInt = userMgrCfgData.passwdAgingDays;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  decides whther the password is expired or not
 *
 * @param    index    (input ) index of user in login structure
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrPasswordExpiredCheck (L7_uint32 index)
{
  L7_uint32 presentTime;
  L7_uint32 passwdConfTime;
  L7_uint32 passwdChngTime;
  if (userMgrPasswordChangeTimeGet (index, &passwdChngTime) == L7_FAILURE ||
      userMgrPasswdValidationTimeGet (&passwdConfTime) == L7_FAILURE)
  {
    return L7_ERROR;
  }
  presentTime = osapiUTCTimeNow();

  if ((presentTime - passwdChngTime) >
      (passwdConfTime * SECONDS_PER_DAY))
  {
    return L7_SUCCESS;
  }
  else
  {
    /* If the difference is -ve, dont return L7_FAILURE */
    return L7_FAILURE;
  }
}

/*********************************************************************
 * @purpose  Gets the expiry time of the user at given the user table index
 *
 * @param    index    (input ) index of user in login structure
 * @param    time     (output ) expiry time (past or future)
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrExpireTimeGet(L7_uint32 index, L7_uint32 *time)
{
  L7_uint32 presentTime;
  L7_uint32 passwdConfTime;
  L7_uint32 passwdChngTime;
  L7_uint32 expiryTime;

  if (userMgrPasswordChangeTimeGet (index, &passwdChngTime) == L7_FAILURE ||
      userMgrPasswdValidationTimeGet (&passwdConfTime) == L7_FAILURE)
  {
    return L7_ERROR;
  }

  if (passwdConfTime == 0)
  {
    return L7_FAILURE;
  }

  presentTime = osapiUTCTimeNow();
  expiryTime =  passwdChngTime + (passwdConfTime * SECONDS_PER_DAY);

  if ((expiryTime - presentTime) > 0)
  {
    *time = expiryTime;
    return L7_SUCCESS;
  }
  else
  {
    /* If the difference is -ve, dont return L7_FAILURE */
    return L7_FAILURE;
  }
}

/*********************************************************************
 * @purpose  Blocks/Unblocks a user
 *
 * @param    index       table index
 * @param    blockStatus L7_TRUE or L7_FALSE
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 userMgrLoginUserBlockStatusSet (L7_uint32 index, L7_BOOL blockStatus )
{
  L7_BOOL priorAccesslevel;

  if (index >= L7_MAX_LOGINS)
    return(L7_FAILURE);

  priorAccesslevel = userMgrCfgData.systemLogins[index].blockStatus;

  userMgrCfgData.systemLogins[index].blockStatus = blockStatus;

  if ( userMgrNotifyListeners( L7_USER_MGR_USER_LISTENER,
    index,
    L7_USER_MGR_CHANGE_USER_MODIFY ) != L7_SUCCESS )
  {
    userMgrCfgData.systemLogins[index].blockStatus = priorAccesslevel;
    return L7_FAILURE;
  }

  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Gets the total number of times the user account
 *           has been locked
 *
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrTotalLockoutCountGet(L7_uint32 index, L7_uint32 *val)
{
  if (index >= L7_MAX_LOGINS)
  {
    return L7_FAILURE;
  }

  *val = userMgrCfgData.systemLogins[index].passwdNumLockouts;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Gets the total number of password failures for the user account
 *
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrTotalLoginFailuresGet(L7_uint32 index, L7_uint32 *val)
{
  if (index >= L7_MAX_LOGINS)
  {
    return L7_FAILURE;
  }

  *val = userMgrCfgData.systemLogins[index].passwdFailureTotal;
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Gets the current password failure count used for determining
 *           when the account should be locked.  The value is reset on
 *           a successful login.
 *
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t userMgrCurrentLockoutCountGet(L7_uint32 index, L7_uint32 *val)
{
  if (index >= L7_MAX_LOGINS)
  {
    return L7_FAILURE;
  }

  *val = userMgrCfgData.systemLogins[index].passwdLockoutCount;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose To add an entry into the history table
*
* @param none
*
* @returns L7_SUCCESS  -user is unlocked
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t userMgrAddToLoginHistory(L7_char8 *name, L7_LOGIN_TYPE_t protocol, L7_inet_addr_t *location)
{
  if (userMgrCfgData.latest == L7_MAX_LOGIN_HISTORY_SIZE)
  {
    userMgrCfgData.latest = L7_USER_MGR_ZERO;
  }

  osapiStrncpySafe(userMgrCfgData.loginHistory[userMgrCfgData.latest].loginName, name, L7_LOGIN_SIZE);
  userMgrCfgData.loginHistory[userMgrCfgData.latest].protocol = protocol;
  memcpy(&userMgrCfgData.loginHistory[userMgrCfgData.latest].location, location, sizeof(L7_inet_addr_t));
  userMgrCfgData.loginHistory[userMgrCfgData.latest].inUse = L7_TRUE;
  userMgrCfgData.loginHistory[userMgrCfgData.latest].passwdTimeStamp =usmDbAdjustedTimeGet();
 

  userMgrCfgData.latest++;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose To add an entry into the history table
*
* @param none
*
* @returns L7_SUCCESS  -user is unlocked

* @notes none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginHistoryGetFirst(L7_uint32 *index, L7_char8 *name, L7_LOGIN_TYPE_t *protocol, L7_inet_addr_t *location, L7_uint32 *time)
{
  L7_uint32 lindex;

  if (*index == L7_USER_MGR_ZERO)
  {
    lindex = userMgrCfgData.latest;
  }
  else
  {
    lindex = *index;

    if (lindex >= L7_MAX_LOGIN_HISTORY_SIZE)
    {
      return L7_FAILURE;
    }

    if ((lindex == L7_USER_MGR_ZERO) ||(userMgrCfgData.loginHistory[lindex].inUse ==L7_FALSE))
    {
      return L7_FAILURE;
    }
  }

  lindex -= L7_USER_MGR_ONE;
  strcpy(name, userMgrCfgData.loginHistory[lindex].loginName);
  *protocol = userMgrCfgData.loginHistory[lindex].protocol;
  memcpy(location, &userMgrCfgData.loginHistory[lindex].location, sizeof(L7_inet_addr_t));
  *time = userMgrCfgData.loginHistory[lindex].passwdTimeStamp;
  *index = lindex;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose To add an entry into the history table
*
* @param none
*
* @returns L7_SUCCESS  -user is unlocked
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t userMgrLoginHistoryGetNext(L7_uint32 index, L7_uint32 *nextIndex, L7_char8 *name, L7_LOGIN_TYPE_t *protocol, L7_inet_addr_t *location, L7_uint32 *time)
{
  L7_uint32  lindex;

  lindex= index;

  if (lindex == L7_USER_MGR_ZERO)
    lindex = L7_MAX_LOGIN_HISTORY_SIZE;

  lindex--;

  /* If all the entries in the table are not filled up. */
  if ((lindex == userMgrCfgData.latest) ||(userMgrCfgData.loginHistory[lindex].inUse ==L7_FALSE))
  {
    return L7_FAILURE;
  }
  else
  {
    strcpy(name, userMgrCfgData.loginHistory[lindex].loginName);
    *protocol = userMgrCfgData.loginHistory[lindex].protocol;
    memcpy(location, &userMgrCfgData.loginHistory[lindex].location, sizeof(L7_inet_addr_t));
    *time = userMgrCfgData.loginHistory[lindex].passwdTimeStamp;
    *nextIndex = lindex;
    return L7_SUCCESS;
  }
}

/*********************************************************************
* @purpose  Set the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (input) password to set
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLinePasswordGet(L7_ACCESS_LINE_t accessLine,
                               L7_char8 *password)
{
  if (password == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiStrncpySafe(password, userMgrCfgData.linePassword[accessLine], L7_ENCRYPTED_PASSWORD_SIZE);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (input) password to set
* @param    encrypted       (input) flag indicating whether the password param is encrypted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLinePasswordSet(L7_ACCESS_LINE_t accessLine, L7_char8 *password, L7_BOOL encrypted)
{
  /* TODO scramble */
  L7_char8 key[L7_ENCRYPTED_PASSWORD_SIZE];
  if (password == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (encrypted == L7_FALSE)
  {
    pwEncrypt(password, key, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
    osapiStrncpy(userMgrCfgData.linePassword[accessLine], key, L7_ENCRYPTED_PASSWORD_SIZE);
  }
  else
  {
    osapiStrncpy(userMgrCfgData.linePassword[accessLine], password, L7_ENCRYPTED_PASSWORD_SIZE);
  }
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove the line password
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrLinePasswordRemove(L7_ACCESS_LINE_t accessLine)
{
  /* TODO unscramble */
  memset(userMgrCfgData.linePassword[accessLine], 0, sizeof(userMgrCfgData.linePassword[accessLine]));
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set enable password at a level 1-15
*
* @param    level       (input) enable level at which the password should be added
* @param    password    (input) enable password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrEnablePasswordGet(L7_uint32 level,
                                 L7_char8 *password)
{
  /* TODO unscramble */
  if (password == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiStrncpySafe(password, userMgrCfgData.enablePassword[level], L7_ENCRYPTED_PASSWORD_SIZE);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set enable password at a level 1-15
*
* @param    level       (input) enable level at which the password should be added
* @param    password    (input) enable password
* @param    encrypted   (input) flag indicating whether the password param is encrypted
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrEnablePasswordSet(L7_uint32 level, L7_char8 *password, L7_BOOL encrypted)
{
  L7_char8 key[L7_ENCRYPTED_PASSWORD_SIZE];

  if (password == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (encrypted == L7_FALSE)
  {
    pwEncrypt(password, key, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
    osapiStrncpySafe(userMgrCfgData.enablePassword[level], key, L7_ENCRYPTED_PASSWORD_SIZE);
  }
  else
  {
    osapiStrncpySafe(userMgrCfgData.enablePassword[level], password, L7_ENCRYPTED_PASSWORD_SIZE);
  }
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Removes the password for a specific user at a certain level
*
* @param    level  (input) enable level at which the password should be removed
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t userMgrEnablePasswordRemove(L7_uint32 level)
{
  memset(userMgrCfgData.enablePassword[level], 0, sizeof(userMgrCfgData.enablePassword[level]));
  userMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}
/********************************************************************* * 
@purpose used to check the node level
 *
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL userMgrNodeLevelCheck(unsigned int user_level,unsigned int level,char cond)
{
/*
This function needs to re-implemented as per the requirements
*/
  if(level == 0 )
    return L7_TRUE;
 #if 0 
  if(user_level > level   && cond == USER_MGR_ACCESS_MORE)
    return L7_TRUE;
  
  else if(level == user_level && cond == USER_MGR_ACCESS_EQUAL)
    return L7_TRUE;
  
  else if(user_level < level   && cond == USER_MGR_ACCESS_MORE)
    return L7_TRUE;
#endif
  if(user_level >= level)
    return L7_TRUE;
    
  return L7_FALSE;
}

