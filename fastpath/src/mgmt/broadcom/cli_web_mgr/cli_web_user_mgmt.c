/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\mgmt\cli_web_mgr\cli_web_user_mgmt.c
*
* @purpose interface for user information and login session information
*
* @component cli web manager
*
* @comments tba
*
* @create 09/26/2002
*
* @author Jill Flanagan
*
* @end
**********************************************************************/

/*************************************************************

*************************************************************/



#include "l7_common.h"
#include "cli_web_exports.h"
#include "user_manager_exports.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "cli_web_mgr_api.h"
#include "cli_web_user_mgmt.h"
#include "cli_web_util.h"
#include "login_sessions_api.h"
#include "user_mgr_api.h"
#include "session.h"
#include <string.h>
#include <stdio.h>

extern  cliWebCnfgrState_t cliWebCnfgrState;


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
void
cliWebConfigLoginsFactoryDefault(void)
{
    userMgrConfigLoginsFactoryDefault();
}

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
L7_uint32
cliWebLoginUserNameGet(L7_uint32 index, L7_char8 *name)
{
	return(userMgrLoginUserNameGet(index, name));
}

/*********************************************************************
* @purpose  Returns table index of Login user name
*
* @param    name     pointer to loing user name
* @param    index    pointer to table index
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginIndexGet(L7_char8 *name, L7_uint32 *index)
{
	return(userMgrLoginIndexGet(name, index));
}

/*********************************************************************
*
* @purpose Get the next valid index.
*
* @param L7_uint32 *index     pointer to table index
*
* @returns L7_SUCCESS, if the next index exist and it is enable
* @returns L7_FAILURE, if the next index doesn't exist
*
* @notes Returns success if the next index exists.
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginUserAvailableIndexGet(L7_uint32 *index)
{
  return userMgrLoginAvailableIndexGet(index);
}

/*********************************************************************
* @purpose  Sets a login user name per index
*
* @param    index       table index
* @param    commName    pointer to Community name
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    also sets login status to L7_enable and initializes all fields.
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserNameSet(L7_uint32 index, L7_char8 *name)
{
	return(userMgrLoginUserNameSet(index,name));

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
L7_uint32
cliWebLoginUserPasswordGet(L7_uint32 index, L7_char8 *password)
{
    return userMgrLoginUserPasswordClearTextGet(index, password);
}

/*********************************************************************
* @purpose  Returns login user password per index in encrypted format
*
* @param    index       table index
* @param    password    pointer to login user encrypted password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserPasswordEncryptedGet(L7_uint32 index, L7_char8 *password)
{
    return userMgrLoginUserPasswordGet(index, password);
}

/*********************************************************************
* @purpose  Sets a login user password per index
*
* @param    index       table index
* @param    password    pointer to login user password
* @param    encrypted   indicates whether the password param is in encrypted form
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted)
{
    L7_RC_t    rc;
    L7_uint32  status;
    L7_char8   name[L7_LOGIN_SIZE];

    rc = userMgrLoginUserStatusGet(index, &status);
    if ((rc == L7_SUCCESS) && (status == L7_ENABLE))
    {
      rc = userMgrLoginUserNameGet(index, name);
      /* user mgr will callback after password set to update ewaBasic...*/
    }
    return userMgrLoginUserPasswordSet(index, password, encrypted);

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
L7_uint32
cliWebLoginUserStatusGet(L7_uint32 index, L7_uint32 *status)
{
	return(userMgrLoginUserStatusGet(index,status));
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
void
cliWebLoginUsersFactoryDefaultSet(void)
{
	userMgrUserLoginsFactoryDefaultSet();
}

/*********************************************************************
* @purpose Get the remote IP address for current telnet session.
*
* @param L7_uint32 index      index value
* @param L7_uint32 *val       ptr to IP address
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionRemoteIpAddrGet(L7_uint32 index, L7_inet_addr_t *val)
{
  L7_inet_addr_t *pVal;

  pVal = loginSessionIPAddrGet(index);
  if(!pVal)
      return L7_FAILURE;
  *val = *pVal;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the remote IP address for current telnet session.
*
* @param L7_uint32 index      index value
* @param L7_uint32 val        ip address
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionRemoteIpAddrSet(L7_uint32 index, L7_inet_addr_t *val)
{
  loginSessionIPAddrSet(index, val);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the telnet session type.
*
* @param L7_uint32 index      index value
* @param L7_uint32 *val       ptr to session type
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionTypeGet( L7_uint32 index, L7_uint32 *val)
{
  if (CLI_WEB_IS_READY)
  {
    *val = loginSessionConnectionTypeGet(index);
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}


/*********************************************************************
*
* @purpose Set the telnet session type.
*
* @param L7_uint32 index      index value
* @param L7_uint32 val        session type
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionTypeSet(L7_uint32 index, L7_uint32 val)
{
  loginSessionConnectionTypeSet(index, val);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the telnet slot and port.
*
* @param L7_uint32 index      index value
* @param L7_char8  *buf       ptr buf contents
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionSlotPortGet( L7_uint32 index, L7_char8 *buf)
{
  sprintf(buf, "%d.%d", loginSessionSlotGet(index), loginSessionPortGet(index));
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the telnet slot and port.
*
* @param L7_uint32 index      index value
* @param L7_uint32 unit       unit number
* @param L7_uint32 slot       slot number
* @param L7_uint32 port       port number
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionUnitSlotPortSet(L7_uint32 index,
                                         L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  loginSessionUSPSet(index, unit, slot, port);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the login session interface index.
*
* @param L7_uint32 index      index value
* @param L7_uint32 *ifIndex   interface number
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionIfIndexGet( L7_uint32 index, L7_uint32 *ifIndex)
{
  *ifIndex = loginSessionIfIndexGet(index);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the login session interface index.
*
* @param L7_uint32 index      index value
* @param L7_uint32 ifIndex    interface number
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionIfIndexSet(L7_uint32 index, L7_uint32 ifIndex)
{
  loginSessionIfIndexSet(index, ifIndex);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the login session username.
*
* @param L7_uint32 index      index value
* @param L7_char8  *buf       ptr buf contents
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionUserGet( L7_uint32 index, L7_char8 *buf)
{
  L7_char8 *username;

  if ((username = loginSessionUserNameGet(index)) != L7_NULL)
  {
    memcpy(buf, username, L7_LOGIN_SIZE);
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Set the login session username.
*
* @param L7_uint32 index      index value
* @param L7_char8  *buf       ptr buf contents
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionUserSet(L7_uint32 index, L7_char8 *buf)
{
  loginSessionUserNameSet(index, buf);
  userMgrAddToLoginHistory(buf, loginSessionConnectionTypeGet(index), loginSessionIPAddrGet(index));
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the login session time.
*
* @param L7_uint32 index      index value
* @param L7_uint32 *time      time
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionTimeGet(L7_uint32 index, L7_uint32 *time)
{
  *time = loginSessionConnectionDurationGet(index);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the login session connection time.
*
* @param L7_uint32 index      index value
* @param L7_uint32 *time      time
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionStartTimeGet(L7_uint32 index, L7_uint32 *time)
{
  *time = loginSessionLoginTimeGet(index);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the login session idle time.
*
* @param L7_uint32 index      index value
* @param L7_uint32 *time      time
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionIdleTimeGet(L7_uint32 index, L7_uint32 *time)
{
  *time = loginSessionLoginIdleTimeGet(index);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Update the users activity time.
*
* @param L7_uint32 index      index value
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes Called whenever a user does something.
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionUpdateActivityTime(L7_uint32 index)
{
  loginSessionUpdateActivityTime(index);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the telnet session status.
*
* @param L7_uint32 index      index value
* @param L7_uint32 *val       ptr buf contents
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionStatusGet(L7_uint32 index, L7_uint32 *val)
{
  return L7_ERROR;
}


/*********************************************************************
*
* @purpose Set the telnet session status.
*
* @param L7_uint32 index      index value
* @param L7_uint32 val        buf contents
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionStatusSet(L7_uint32 index, L7_uint32 val)
{
  return L7_ERROR;
}


/*********************************************************************
*
* @purpose Get the void pointer of user defined data.
*
* @param L7_uint32 index      index value
*
* @returns *val       ptr to EwsContext for this session
*
* @notes none
*
* @end
*
*********************************************************************/
void *cliWebLoginSessionUserStorageGet(L7_uint32 index)
{
  return loginSessionUserStorageGet(index);
}


/*********************************************************************
*
* @purpose Set the void pointer of user defined data.
*
* @param L7_uint32 index      index value
* @param void      *val       ptr buf contents
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionUserStorageSet(L7_uint32 index, void *val)
{
  loginSessionUserStorageSet(index, val);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get if the connection should be reset or not.
*
* @param L7_uint32 index            index value
* @param L7_BOOL   *resetConnection L7_TRUE if connection should be reset
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionResetConnectionGet(L7_uint32 index, L7_BOOL *resetConnection)
{
  *resetConnection = loginSessionResetConnectionGet(index);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set a flag for the connection to be reset.
*
* @param L7_uint32 index      index value
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionResetConnectionSet(L7_uint32 index)
{
  loginSessionResetConnectionSet(index);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set a flag for ALL the connections to be reset.
*
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionResetConnectionAllSet(void)
{
  L7_uint32 index;

  for (index = 0; index < FD_CLI_DEFAULT_MAX_CONNECTIONS; index++)
  {
    loginSessionResetConnectionSet(index);
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Get the user's login information index.
*
* @param L7_char8  *name   pointer to login user name
* @param L7_uint32 *index  pointer to table index
*
* @returns L7_SUCCESS, if the user exists and the login index was set
* @returns L7_FAILURE, if the user does not exist or the index was not set
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginUserIndexGet(L7_char8 *name, L7_uint32 *index)
{
  return userMgrLoginIndexGet(name, index);
}


/*********************************************************************
*
* @purpose Checks if the table index is a valid index.
*
* @param L7_uint32 index      table index
*
* @returns L7_SUCCESS, if the index exist and it is enable
* @returns L7_FAILURE, if the index doesn't exist
*
* @notes Returns success if the next index exists.
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginUserIndexValidGet(L7_uint32 index)
{
  L7_uint32 status;

  if ( (userMgrLoginUserStatusGet(index, &status) == L7_SUCCESS) && (status == L7_ENABLE) )
    return L7_SUCCESS;

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Get the next valid index.
*
* @param L7_uint32 *index     pointer to table index
*
* @returns L7_SUCCESS, if the next index exist and it is enable
* @returns L7_FAILURE, if the next index doesn't exist
*
* @notes Returns success if the next index exists.
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginUserIndexNextValidGet(L7_uint32 *index)
{
  if ( cliWebLoginUserIndexValidGet(*index) == L7_SUCCESS)
  {
    *index = (*index)+1;
    return cliWebLoginUserIndexValidGet(*index);
  }
  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Set the login user status per index.
*
* @param L7_uint32 index      table index
* @param L7_uint32 val        pointer to login user status
*                             (L7_ENABLE of L7_DISABLE)
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginUserStatusSet(L7_uint32 index, L7_uint32 val)
{
  return userMgrLoginUserStatusSet(index, val);
}

/*********************************************************************
*
* @purpose Set the login user status per index.
*
* @param L7_uint32 index      table index   
* @param L7_uint32 val        pointer to login user status
*                             (L7_ENABLE of L7_DISABLE)   
*
* @returns L7_SUCCESS or 
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginUserBlockStatusSet(L7_uint32 index, L7_BOOL val)
{
  return userMgrLoginUserBlockStatusSet (index, val);
}

/*********************************************************************
*
* @purpose Checks an input enteredString against a stored pw.
*
* @param L7_char8 *pw              the password (already encrypted)
* @param L7_char8 *enteredString   the string to check (unencrypted)
*
* @returns L7_SUCCESS or   - if passwords match
* @returns L7_FAILURE      - if passwords don't match
*
* @notes All password scrambling/unscrambling is handled by user mgr.
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginUserPasswordIsValid(L7_char8 *pw, L7_char8 *enteredString,
                                       L7_PASSWORD_ENCRYPT_ALG_t alg)
{
  L7_RC_t    rc = L7_SUCCESS;
  L7_char8 enteredPwdKey[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8 enteredPwdText[L7_PASSWORD_SIZE];

  /* if both are null */
  if (strlen(pw) == 0 && strlen(enteredString) == 0)
  {
    return L7_SUCCESS;
  }

  if(strlen(enteredString) >= L7_PASSWORD_SIZE)
  {
    return L7_FAILURE;
  }

  /* encrypt entered string for comparison */
  memset(enteredPwdKey, 0, sizeof(enteredPwdKey));
  memset(enteredPwdText, 0, sizeof(enteredPwdText));
  osapiStrncpySafe(enteredPwdText, enteredString, sizeof(enteredPwdText));
  pwEncrypt(enteredPwdText, enteredPwdKey, L7_PASSWORD_SIZE-1, alg);

  /* since these are hex strings, use case insensitive comparison */
  if (userMgrNoCaseCompare(pw, enteredPwdKey) != L7_TRUE)
          rc = L7_FAILURE;

  return rc;
}


/*********************************************************************
*
* @purpose Deletes a user name and all settings for the user
*
* @param L7_uint32 index      table index
*
* @returns L7_SUCCESS
* @returns L7_FAILURE if the attempt is to delete the admin
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginsDelete(L7_uint32 index)
{
  if ( index != 0 )   /* can't remove admin */
  {
     if ( userMgrLoginUserDelete( index ) == L7_SUCCESS )
     {
         /* user mgr will callback to update ewaBasic...*/
         return L7_SUCCESS;
     }
  }

  return L7_FAILURE;
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
L7_uint32
cliWebLoginUserAuthenticationSet(L7_uint32 index, L7_uint32 authProt)
{
	return(userMgrLoginUserAuthenticationSet(index,authProt));
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
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserEncryptionSet(L7_uint32 index, L7_uint32 encryptProt,
                             L7_char8 *encryptKey)
{
	return(userMgrLoginUserEncryptionSet(index,encryptProt,encryptKey));
}


/*********************************************************************
* @purpose  Sets a login user access level
*
* @param    index       table index
* @param    accessLevel readonly or readwrite
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserAccessModeSet(L7_uint32 index, L7_uint32 accessLevel )
{
	return(userMgrLoginUserAccessModeSet(index,accessLevel));
}


/*********************************************************************
* @purpose  Sets a login user snmpv3 access level
*
* @param    index       table index
* @param    accessLevel readonly or readwrite
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserSnmpv3AccessModeSet(L7_uint32 index, L7_uint32 accessLevel )
{
	return(userMgrLoginUserSnmpv3AccessModeSet(index,accessLevel));
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
L7_uint32
cliWebLoginUserAuthenticationGet(L7_uint32 index, L7_uint32 *authProt)
{
    return userMgrLoginUserAuthenticationGet(index,authProt);
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
L7_uint32
cliWebLoginUserEncryptionGet(L7_uint32 index, L7_uint32 *encryptProt)
{

    return userMgrLoginUserEncryptionGet(index,encryptProt);
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
L7_uint32
cliWebLoginUserEncryptionKeyGet(L7_uint32 index, L7_char8 *encryptKey)
{
  return userMgrLoginUserEncryptKeyGet(index, encryptKey);
}


/*********************************************************************
* @purpose  Return a login user access level
*
* @param    index       (input) table index
* @param    accessLevel (output) readonly or readwrite
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserAccessModeGet(L7_uint32 index, L7_uint32 *accessLevel )
{
	return(userMgrLoginUserAccessModeGet(index,accessLevel));
}

/*********************************************************************
* @purpose  Return a login user snmpv3 access level
*
* @param    index       (input) table index
* @param    accessLevel (output) readonly or readwrite
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
cliWebLoginUserSnmpv3AccessModeGet(L7_uint32 index, L7_uint32 *accessLevel )
{
	return(userMgrLoginUserSnmpv3AccessModeGet(index,accessLevel));
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
L7_uint32
cliWebLoginUserDelete(L7_uint32 index)
{
	return(cliWebLoginsDelete(index));
}


/*********************************************************************
* @purpose Call the session login.
*
* @param L7_uint32 *val       ptr to login sesion
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE   - if reached max connections
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionLogin( L7_uint32 *val)
{
  *val = loginSessionLogin();

  if ((*val > FD_CLI_DEFAULT_MAX_CONNECTIONS) || (*val < 0))
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Call the session logout.
*
* @param L7_uint32 index      index value
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionLogout(L7_uint32 index)
{
  return loginSessionLogout(index);
}


/*********************************************************************
* @purpose Determine if his login session exists.
*
* @param L7_uint32 index      index value
* @param L7_BOOL *val       boolean determination of whether this session exists.
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t cliWebLoginSessionValidEntry(L7_uint32 index, L7_BOOL *val)
{
  if (CLI_WEB_IS_READY)
  {
    *val = loginSessionDoesLoginExist(index);
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*********************************************************************
*
* @purpose Scans the session table for sessions that have timed out and
*          removes them
*
* @returns
*
* @end
*
*********************************************************************/
void cliWebEmWebSessionTableTimeoutScan()
{
  EwaSessionTableTimeoutScan();
}


