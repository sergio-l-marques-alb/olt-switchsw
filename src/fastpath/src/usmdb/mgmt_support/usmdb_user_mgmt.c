/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src\application\unitmgr\umdb\usmdb_user_mgmt.c
 *
 * @purpose interface for user information and login session information
 *
 * @component unitmgr
 *
 * @comments (jflanagan 9/26/02) this file should be removed once all changes are
 *           made in cli and web to call cliWeb directly.
 *
 * @create 04/10/2001
 *
 * @author fsamuels
 *
 * @end
 **********************************************************************/

#include <string.h>
#include <stdio.h>
#include "l7_common.h"
#include "l7_resources.h"
#include "defaultconfig.h"
#include "osapi.h"
#include "cli_web_mgr_api.h"
#include "login_sessions_api.h"
#include "usmdb_user_mgmt_api.h"
#include "user_mgr_apl.h"
#include "user_mgr_api.h"
#include "cli_web_user_mgmt.h"
#include "l7_web_session_api.h"


/*********************************************************************
 * @purpose Call the session login.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 *val       ptr to login sesion
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE   - if reached max connections
 *
 * @notes This should be eliminated once the web and cli are updated
 *        to call cliWeb directly.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbLoginSessionLogin(L7_uint32 UnitIndex, L7_uint32 *val)
{
  return cliWebLoginSessionLogin(val);
}

/*********************************************************************
 * @purpose Call the session logout.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionLogout(L7_uint32 UnitIndex, L7_uint32 index)
{
  return cliWebLoginSessionLogout(index);
}

/*********************************************************************
 * @purpose Determine the number of active login sessions
 *
 * @param L7_BOOL allSessions     Get a count of all active sessions
 * @param L7_uint32 sessionType   Session type to count
 *
 * @returns Number of active sessions
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_uint32 usmDbLoginNumSessionsActiveGet(L7_BOOL allSessions, L7_uint32 sessionType)
{
  L7_uint32 i, val;
  L7_BOOL   bVal;
  L7_uint32 numSessions;

  numSessions = 0;

  for (i=0; i<FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
  {
    if (cliWebLoginSessionValidEntry(i, &bVal) == L7_SUCCESS)
    {
      if (bVal == L7_TRUE)
      {
    if (allSessions == L7_TRUE)
    {
      numSessions++;
    }
    else
    {
      if (cliWebLoginSessionTypeGet(i, &val) == L7_SUCCESS)
      {
        if (sessionType == val)
        {
          numSessions++;
        }
      }
    }
      }
    }
  }

  return numSessions;
}

/*********************************************************************
 * @purpose Determine if his login session exists.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionValidEntry(L7_uint32 UnitIndex, L7_uint32 index, L7_BOOL *val)
{
  if (index >= FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    if (EwaSessionIsActive(index-FD_CLI_DEFAULT_MAX_CONNECTIONS) == L7_TRUE)
    {
      *val = L7_TRUE;
    }
    else
    {
      *val = L7_FALSE;
    }
    return L7_SUCCESS;
  }
  else
  {
    return cliWebLoginSessionValidEntry(index, val);
  }
}

/*********************************************************************
 * @purpose Get the remote IP address for current telnet session.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionRemoteIpAddrGet(L7_uint32 UnitIndex, L7_uint32 index, L7_inet_addr_t *val)
{
  if (index >= FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return EwaSessionInetAddrGet(index-FD_CLI_DEFAULT_MAX_CONNECTIONS, val);
  }
  else
  {
    return cliWebLoginSessionRemoteIpAddrGet(index, val);
  }
}


/*********************************************************************
 *
 * @purpose Set the remote IP address for current telnet session.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionRemoteIpAddrSet(L7_uint32 UnitIndex, L7_uint32 index, L7_inet_addr_t *val)
{
  return cliWebLoginSessionRemoteIpAddrSet(index, val);
}


/*********************************************************************
 *
 * @purpose Get the telnet session type.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionTypeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  if (index >= FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return EwaSessionTypeGet(index-FD_CLI_DEFAULT_MAX_CONNECTIONS, val);
  }
  else
  {
    return cliWebLoginSessionTypeGet(index, val);
  }
}


/*********************************************************************
 *
 * @purpose Set the telnet session type.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionTypeSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 val)
{
  return cliWebLoginSessionTypeSet(index, val);
}


/*********************************************************************
 *
 * @purpose Get the telnet slot and port.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionSlotPortGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return cliWebLoginSessionSlotPortGet(index, buf);
}


/*********************************************************************
 *
 * @purpose Set the telnet slot and port.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionUnitSlotPortSet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  return cliWebLoginSessionUnitSlotPortSet(index, unit, slot, port);
}


/*********************************************************************
 *
 * @purpose Get the login session interface index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionIfIndexGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *ifIndex)
{
  return cliWebLoginSessionIfIndexGet(index, ifIndex);
}


/*********************************************************************
 *
 * @purpose Set the login session interface index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionIfIndexSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 ifIndex)
{
  return cliWebLoginSessionIfIndexSet(index, ifIndex);
}


/*********************************************************************
 *
 * @purpose Get the login session username.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionUserGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  if (index >= FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return EwaSessionUserGet(index-FD_CLI_DEFAULT_MAX_CONNECTIONS, buf);
  }
  else
  {
    return cliWebLoginSessionUserGet(index, buf);
  }
}


/*********************************************************************
 *
 * @purpose Set the login session username.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionUserSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return cliWebLoginSessionUserSet(index, buf);
}


/*********************************************************************
 *
 * @purpose Get the login session time.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time)
{
  if (index >= FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return EwaSessionUpTimeGet(index-FD_CLI_DEFAULT_MAX_CONNECTIONS, time);
  }
  else
  {
    return cliWebLoginSessionTimeGet(index, time);
  }
}


/*********************************************************************
 *
 * @purpose Get the login session connection time.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionStartTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time)
{
  return cliWebLoginSessionStartTimeGet(index, time);
}


/*********************************************************************
 *
 * @purpose Get the login session idle time.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionIdleTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time)
{
  if (index >= FD_CLI_DEFAULT_MAX_CONNECTIONS)
  {
    return EwaSessionIdleTimeGet(index-FD_CLI_DEFAULT_MAX_CONNECTIONS, time);
  }
  else
  {
    return cliWebLoginSessionIdleTimeGet(index, time);
  }
}


/*********************************************************************
 *
 * @purpose Update the users activity time.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionUpdateActivityTime(L7_uint32 UnitIndex, L7_uint32 index)
{
  return cliWebLoginSessionUpdateActivityTime(index);
}


/*********************************************************************
 *
 * @purpose Get the telnet session status.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionStatusGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return cliWebLoginSessionStatusGet(index, val);
}


/*********************************************************************
 *
 * @purpose Set the telnet session status.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionStatusSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 val)
{
  return cliWebLoginSessionStatusSet(index, val);
}


/*********************************************************************
 *
 * @purpose Get the void pointer of user defined data.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      index value
 *
 * @returns *val       ptr to EwsContext for this session
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void *usmDbLoginSessionUserStorageGet(L7_uint32 UnitIndex, L7_uint32 index)
{
  return cliWebLoginSessionUserStorageGet(index);
}


/*********************************************************************
 *
 * @purpose Set the void pointer of user defined data.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionUserStorageSet(L7_uint32 UnitIndex, L7_uint32 index, void *val)
{
  return cliWebLoginSessionUserStorageSet(index, val);
}


/*********************************************************************
 *
 * @purpose Get if the connection should be reset or not.
 *
 * @param L7_uint32 UnitIndex        the unit for this operation
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
L7_RC_t usmDbLoginSessionResetConnectionGet(L7_uint32 UnitIndex, L7_uint32 index, L7_BOOL *resetConnection)
{
  return cliWebLoginSessionResetConnectionGet(index, resetConnection);
}


/*********************************************************************
 *
 * @purpose Set a flag for the connection to be reset.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginSessionResetConnectionSet(L7_uint32 UnitIndex, L7_uint32 index)
{
  return cliWebLoginSessionResetConnectionSet(index);
}


/*********************************************************************
 *
 * @purpose Set a flag for ALL the connections to be reset.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbLoginSessionResetConnectionAllSet(L7_uint32 UnitIndex)
{
  return cliWebLoginSessionResetConnectionAllSet();
}


/*********************************************************************
 *
 * @purpose Get the user's login information index.
 *
 * @param L7_uint32 UnitIndex  unit for this operation
 * @param L7_char8  *name      pointer to login user name
 * @param L7_uint32 *index     pointer to table index
 *
 * @returns L7_SUCCESS, if the user exists and the login index was set
 * @returns L7_FAILURE, if the user does not exist or the index was not set
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserLoginIndexGet(L7_uint32 UnitIndex, L7_char8 *name, L7_uint32 *index)
{
  return cliWebLoginUserIndexGet(name, index);
}


/*********************************************************************
 *
 * @purpose Get the login user name per index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_char8  *buf       pointer to login user name
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbLoginsGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return cliWebLoginUserNameGet(index, buf);
}


/*********************************************************************
 *
 * @purpose Set a new user name and password.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_char8  *buf       pointer to login user name
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbLoginsSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return cliWebLoginUserNameSet(index, buf);
}


/*********************************************************************
 *
 * @purpose Checks if the table index is a valid index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginUserIndexValidGet(L7_uint32 UnitIndex, L7_uint32 index)
{
  return cliWebLoginUserIndexValidGet(index);
}


/*********************************************************************
 *
 * @purpose Get the next valid index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginUserIndexNextValidGet(L7_uint32 UnitIndex, L7_uint32 *index)
{
  return cliWebLoginUserIndexNextValidGet(index);
}


/*********************************************************************
 *
 * @purpose Get the login user status per index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_uint32 *val       pointer to login user status
 *                             (L7_ENABLE of L7_DISABLE)
 *
 * @returns  L7_SUCCESS or
 * @returns  L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbLoginStatusGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  return cliWebLoginUserStatusGet(index, val);
}


/*********************************************************************
 *
 * @purpose Set the login user status per index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginStatusSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 val)
{
  return cliWebLoginUserStatusSet(index, val);
}


/*********************************************************************
 *
 * @purpose Get the login user password per index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_char8  *buf       pointer to login user password
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbPasswordGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return cliWebLoginUserPasswordGet(index, buf);
}


/*********************************************************************
 *
 * @purpose Set a login user password per index.
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_char8  *buf       pointer to login user password
 * @param L7_BOOL   encrypted  (input) flag indicating whether the password param is encrypted
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbPasswordSet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf, L7_BOOL encrypted)
{
  return cliWebLoginUserPasswordSet(index, buf, encrypted);
}


/*********************************************************************
 *
 * @purpose Checks an input enteredString against a stored pw.
 *
 * @param L7_uint32 UnitIndex       The unit for this operation
 * @param L7_uint32 pw              the password
 * @param L7_uint32 enteredString   the string to check
 *
 * @returns L7_SUCCESS or   - if passwords match
 * @returns L7_FAILURE      - if passwords don't match
 *
 * @notes Pass in current platform encryption
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbPasswordIsValid(L7_uint32 UnitIndex, L7_char8 *pw, L7_char8 *enteredString)
{
  return cliWebLoginUserPasswordIsValid(pw, enteredString, L7_PASSWORD_ENCRYPT_ALG);
}


/*********************************************************************
 *
 * @purpose Set the authentication parameters for a user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_uint32 authProt   authentication protocol
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes The user login password is used as the authentication key.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAuthSet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 authProt)
{
  return cliWebLoginUserAuthenticationSet(index, authProt);
}

/*********************************************************************
 *
 * @purpose Set the encryption parameters for a user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_uint32 encryptProt   encryption protocol
 * @param L7_char8 *encryptKey    encryption key
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserEncryptSet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 encryptProt, L7_char8 *encryptKey )
{
  return cliWebLoginUserEncryptionSet(index, encryptProt, encryptKey);
}


/*********************************************************************
 *
 * @purpose Set the access level for a user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
 * @param L7_uint32 index      table index
 * @param L7_uint32 accessMode
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAccessLevelSet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 accessLevel )
{
  return cliWebLoginUserAccessModeSet(index, accessLevel);
}


/*********************************************************************
 *
 * @purpose Return the authentication parameters for a user
 *
 * @param L7_uint32 UnitIndex  (input) the unit for this operation
 * @param L7_uint32 index      (input) table index
 * @param L7_uint32 *authProt   (output) authentication protocol
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAuthGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *authProt)
{
  return cliWebLoginUserAuthenticationGet(index, authProt);
}

/*********************************************************************
 *
 * @purpose Return the encryption protocol for a user
 *
 * @param L7_uint32 UnitIndex  (input) the unit for this operation
 * @param L7_uint32 index      (input) table index
 * @param L7_uint32 *encryptProt   (output) encryption protocol
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserEncryptGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *encryptProt )
{
  return cliWebLoginUserEncryptionGet(index, encryptProt);
}


/*********************************************************************
 *
 * @purpose Return the access level for a user
 *
 * @param L7_uint32 UnitIndex  (input) the unit for this operation
 * @param L7_uint32 index      (input) table index
 * @param L7_uint32 * accessLevel (output)
 *
 * @returns L7_SUCCESS or
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbUserAccessLevelGet(L7_uint32 UnitIndex, L7_uint32 index,
    L7_uint32 *accessLevel )
{
  return cliWebLoginUserAccessModeGet(index, accessLevel);
}

/*********************************************************************
 *
 * @purpose Deletes a user name and all settings for the user
 *
 * @param L7_uint32 UnitIndex  the unit for this operation
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
L7_RC_t usmDbLoginsDelete(L7_uint32 UnitIndex, L7_uint32 index)
{
  return cliWebLoginUserDelete(index);
}


/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param   pUserName (input) ptr to user name to authenticate
 * @param   pPwd (input) ptr to user provided password
 * @param   pAccessLevel (output) access of authenticated user
 * @param   line (input) which line type the user is using
 * @param   mode (input) which mode to authenticate (login/enable)
 * @param   port (input) pointer to text describing the port
 * @param   rem_addr (input) pointer to the IP address initiating access
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with this line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This (simple) interface does NOT support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAuthenticateUser(L7_uint32 UnitIndex,
                              L7_char8 *pUserName, L7_char8 *pPwd,
                              L7_uint32 *pAccessLevel,
                              L7_ACCESS_LINE_t line,
                              L7_ACCESS_LEVEL_t mode,
                              L7_uchar8 *port,
                              L7_uchar8 *rem_addr)
{
  L7_RC_t rc;
  userMgrAuthRequest_t request;

  request.pUserName    = pUserName;
  request.pPwd         = pPwd;
  request.component    = L7_USER_MGR_COMPONENT_ID;
  request.accessLevel  = *pAccessLevel;
  request.line         = line;
  request.mode         = mode;
  request.port         = port;
  request.rem_addr     = rem_addr;
  request.mayChallenge = L7_FALSE;
  request.isChallenged = L7_FALSE;

  rc = userMgrAuthenticateUser(&request);
  *pAccessLevel = request.accessLevel;
  return rc;
}

/*********************************************************************
 * @purpose  Determine if a user is authenticated
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param   pUserName (input) ptr to user name to authenticate
 * @param   pPwd (input) ptr to user provided password
 * @param   component (input) component requesting authentication
 * @param   pAccessLevel (output) access of authenticated user
 * @param   line (input) which line type the user is using
 * @param   mode (input) which mode to authenticate (login/enable)
 * @param   port (input) pointer to text describing the port
 * @param   rem_addr (input) pointer to the IP address initiating access
 * @param   pState (input/output) state returned on previous challenge (if applicable)
 * @param   pChallengeFlag (output) indicates if authentication is challenged
 * @param   pChallengePhrase (output) challenge phrase
 * @param   pChallengeFlags (output) pointer to flags returned by authentication method
 *
 * @returns L7_SUCCESS  If user is authenticated
 * @returns L7_FAILURE  If user is not authenticated due to unknown user, bad password,
 *                      challenge, or system error
 * @returns L7_ERROR  If all configured authentication methods failed to provide response due to timeout, etc.
 * @returns L7_NOT_SUPPORTED If specified component does not support APLs.
 *
 * @notes   The APL associated with this line is used to determine the
 *          appropriate authentication method(s) for this user.
 *
 * @notes This version of the interface DOES support challenges
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAuthenticateUserAllowChallenge(L7_uint32 UnitIndex,
                                            L7_char8 *pUserName, L7_char8 *pPwd,
                                            L7_uint32 component, L7_uint32 *pAccessLevel,
                                            L7_ACCESS_LINE_t line,
                                            L7_ACCESS_LEVEL_t mode,
                                            L7_uchar8 *port,
                                            L7_uchar8 *rem_addr,
                                            L7_uchar8 *pState, L7_BOOL *pChallengeFlag,
                                            L7_uchar8 *pChallengePhrase, L7_uint32 *pChallengeFlags)
{
  L7_RC_t rc;
  userMgrAuthRequest_t request;

  request.pUserName        = pUserName;
  request.pPwd             = pPwd;
  request.component        = L7_USER_MGR_COMPONENT_ID;
  request.accessLevel      = *pAccessLevel;
  request.line             = line;
  request.mode             = mode;
  request.port             = port;
  request.rem_addr         = rem_addr;
  request.mayChallenge     = L7_FALSE; /* This is not supported at this time */
  request.pState           = pState;
  request.isChallenged     = L7_FALSE;
  request.pChallengePhrase = pChallengePhrase;
  request.challengeFlags   = 0;

  rc = userMgrAuthenticateUserAllowChallenge(&request);
  *pAccessLevel    = request.accessLevel;
  *pChallengeFlag  = request.isChallenged;
  *pChallengeFlags = request.challengeFlags;
  return rc;
}

/*********************************************************************
 * @purpose  Attempt to authenticate a user with enable configuration
 *
 * @param   enteredPwd (input) ptr to user provided password
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
L7_RC_t usmDbLoginAuthenticateUserEnable(L7_uint32 level, L7_char8 *enteredPwd)
{
  return userMgrAuthenticateUserEnable(level, enteredPwd);
}

/*********************************************************************
 * @purpose  Creates a named APL with no authentication methods
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if name is empty or too long or if max number of APLs has
 *              been reached.
 *
 * @notes pName must be null terminated.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLCreate(L7_ACCESS_LINE_t accessLine,
                                      L7_ACCESS_LEVEL_t accessLevel,
                                      L7_char8 *pName )
{
  return userMgrAPLCreate(accessLine, accessLevel, pName);
}

/*********************************************************************
 * @purpose  Deletes a named APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if APL does not exist or users have this APL assigned
 *             and therefore it cannot be deleted or if APL name is not valid.
 *
 * @notes  pName must be null terminated.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLDelete(L7_ACCESS_LINE_t accessLine,
                                      L7_ACCESS_LEVEL_t accessLevel,
                                      L7_char8 *pName )
{
  return userMgrAPLDelete(accessLine, accessLevel, pName);
}

/*********************************************************************
 * @purpose  Determines if the specified APL is the system default
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to APL name
 *
 * @returns  L7_TRUE if the specified APL is the defaults
 *           L7_FALSE if the specified APL is not the default
 *
 * @notes  This routine is helpful to determine why an attempt to delete
 *         an APL fails.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLDefaultCheck(L7_ACCESS_LINE_t accessLine,
                                            L7_ACCESS_LEVEL_t accessLevel,
                                            L7_char8 *pName )
{
  return userMgrAPLDefaultCheck(accessLine, accessLevel, pName);
}

/*********************************************************************
 * @purpose  Returns the APL at the specified index in the list
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if index is not valid or if there is not
 *                      an APL at the specified index.
 *
 * @notes index begins with 0
 *
 * @end
 *********************************************************************/
L7_RC_t
usmDbAPLListGetIndex(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessLevel,
                     L7_uint32 index, L7_char8 *pName )
{
  return userMgrAPLGetNameByIndex(accessLine, accessLevel, index, pName);
}

/*********************************************************************
 * @purpose  Returns the first configured APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE
 *
 * @notes pName must be at least L7_MAX_APL_NAME_SIZE+1 in length
 *
 * @end
 *********************************************************************/
  L7_RC_t
usmDbAPLListGetFirst(L7_ACCESS_LINE_t accessLine,
                                        L7_ACCESS_LEVEL_t accessLevel,
                                        L7_char8 *pName )
{
  return userMgrAPLListGetFirst(accessLine, accessLevel, pName);
}

/*********************************************************************
 * @purpose  Gets the APL following the specified name in the APL list
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    pointer to current APL name
 * @param    pNextName pointer to next APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if current APL name does not correspond to a
 *              configured APL or if the end of the list has been reached.
 *
 * @notes pName must be NULL terminated
 * @notes pNextName must be at least L7_MAX_APL_NAME_SIZE+1 in length
 *
 * @end
 *********************************************************************/
  L7_RC_t
usmDbAPLListGetNext(L7_ACCESS_LINE_t accessLine,
                                       L7_ACCESS_LEVEL_t accessLevel,
                                       L7_char8 *pName,
                                       L7_char8 *pNextName )
{
  return userMgrAPLListGetNext(accessLine, accessLevel, pName, pNextName );
}

/*********************************************************************
 * @purpose  Adds an authentication method to an APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (input) pointer to APL name
 * @param    order    (input) order this method should be attempted in the APL
 * @param    method   (input) authentication method value
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if order is >= L7_MAX_AUTH_METHODS or
 *                      if method is not recognized or
 *                      if name does not match a configured APL
 *
 * @notes Order begins with 0
 * @notes To remove an auth method, issue a set with L7_AUTH_METHOD_UNDEFINED.
 *        All auth methods will be attempted in the APL until a definitive
 *        response is found.  If there is an UNDEFINED entry in the middle of
 *        the APL, that entry will be skipped and the remaining entries will
 *        be used.
 *
 * @end
 *********************************************************************/
  L7_RC_t
usmDbAPLAuthMethodSet(L7_ACCESS_LINE_t accessLine,
                                             L7_ACCESS_LEVEL_t accessLevel,
                                             L7_char8 *pName,
                                             L7_uint32 order,
                                             L7_USER_MGR_AUTH_METHOD_t method )
{
  return userMgrAPLAuthMethodSet(accessLine, accessLevel, pName, order, method);
}

/*********************************************************************
 * @purpose  Returns the authentication method at the specified index in
 *           the APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pName    (input) pointer to APL name
 * @param    order    (input) order this method should be attempted in the APL
 * @param    pMethod   (output) authentication method value
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if order is >= L7_MAX_AUTH_METHODS or
 *                      if method is not recognized or
 *                      if name does not match a configured APL
 *
 * @notes Order begins with 0
 * @notes The authentication methods should be attempted in order
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLAuthMethodGet(L7_ACCESS_LINE_t accessLine,
                                             L7_ACCESS_LEVEL_t accessLevel,
                                             L7_char8 *pName,
                                             L7_uint32 order,
                                             L7_USER_MGR_AUTH_METHOD_t *pMethod )
{
  return userMgrAPLAuthMethodGet(accessLine, accessLevel, pName, order, pMethod);
}

/*********************************************************************
 * @purpose  Assign an APL to a user for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pUser    (input) pointer to user name
 * @param    component (input)ID of component to use APL
 * @param    pAPL     (input) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if APL name is not a configured APL or
 *                      if user name is not a configured user or
 *                      if component id is not a component that handles APLs
 *
 * @notes All users will always have an APL configured for all components that
 *        expect them.  Therefore, APL assignments can not be removed just re-set.
 * @notes APL name must be NULL terminated.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLUserSet(L7_uint32 UnitIndex, L7_char8 *pUser, L7_uint32 component, L7_char8 *pAPL )
{
  return userMgrAPLUserSet(pUser, component, pAPL);
}

/*********************************************************************
 * @purpose  Return an APL assigned to a user for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pUser    (input ) pointer to user name
 * @param    component (input) ID of component to use APL
 * @param    pAPL     (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if component id is not a component that handles APLs
 *
 * @notes If the user is not configured, the APL for non-configured users
 *        is returned.  Therefore, this function can not be used to determine
 *        if a user is configured locally.
 *
 * @notes The APL name must be at least L7_MAX_APL_NAME_SIZE+1 in length.  The
 *        value set will be NULL terminated.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLUserGet(L7_uint32 UnitIndex, L7_char8 *pUser, L7_uint32 component, L7_char8 *pAPL )
{
  return userMgrAPLUserGet(pUser, component, pAPL);
}


/*********************************************************************
 * @purpose  Return an APL assigned to a user (by index) for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (input ) index of user in login structure
 * @param    component (input) ID of component to use APL
 * @param    pAPL     (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if the index is not valid or
 *                       if the component is not valid
 *
 * @notes This function will only return the APL for the locally configured
 *        user at the specified index.
 *
 * @notes The APL name must be at least L7_MAX_APL_NAME_SIZE+1 in length.  The
 *        value set will be NULL terminated.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLUserGetIndex(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 component, L7_char8 *pAPL )
{
  return userMgrAPLUserGetIndex(index, component, pAPL);
}

/*********************************************************************
 * @purpose  Assign an APL for nonconfigured users for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    component (input)ID of component to use APL
 * @param    pAPL     (input) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if APL name is not a configured APL or
 *                      if component id is not a component that handles APLs
 *
 * @notes pAPL must be NULL terminated
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLNonConfiguredUserSet(L7_uint32 UnitIndex, L7_uint32 component, L7_char8 *pAPL )
{
  return userMgrAPLNonConfiguredUserSet(component, pAPL);
}

/*********************************************************************
 * @purpose  Return the APL assigned to nonconfigured users for a specific component
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    component (input) ID of component to use APL
 * @param    pAPL     (output) pointer to APL name
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if component id is not a component that handles APLs
 *
 * @notes pAPL must be at least L7_MAX_APL_NAME_SIZE+1 in length
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLNonConfiguredUserGet(L7_uint32 UnitIndex, L7_uint32 component, L7_char8 *pAPL )
{
  return userMgrAPLNonConfiguredUserGet(component,  pAPL);
}

/*********************************************************************
 * @purpose  Return the first user in the login structure using the specified APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pAPLName (input) ptr to APL name
 * @param    pUserName (output) ptr to user name
 * @param    pComponent (output) ptr to component
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if there are no users assigned this APL or
 *                       if the APL name is not configured
 *
 * @notes If the nonconfigured user is assigned this APL, the user name is set
 *        with text that indicates non-configured user.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLAllUsersGetFirst(L7_uint32 UnitIndex, L7_char8 *pAPLName, L7_char8 *pUserName, L7_uint32 *pComponent )
{
  return userMgrAPLAllUsersGetFirst(pAPLName, pUserName, pComponent);
}

/*********************************************************************
 * @purpose  Return the first user in the login structure using the specified APL
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    pAPLName (input) ptr to APL name
 * @param    pCurrentUserName (input) ptr to user name from getFirst or priorGetNext
 * @param    currentComponent (input) component ID from getFirst or priorGetNext
 * @param    pNextUserName (output) ptr to next user name
 * @param    pNextComponent (output) ptr to next component ID
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE  if there are no more users assigned this APL.
 *           L7_ERROR  if the APL name is not configured or if the specified
 *                     user name or component id is not valid
 *
 * @notes If the nonconfigured user is assigned this APL, the user name is set
 *        with text that indicates non-configured user.
 *
 * @end
 *********************************************************************/
  L7_uint32
usmDbAPLAllUsersGetNext(L7_uint32 UnitIndex, L7_char8 *pAPLName,
    L7_char8 *pCurrentUserName, L7_uint32 currentComponent,
    L7_char8 *pNextUserName, L7_uint32 *pNextComponent )
{
  return userMgrAPLAllUsersGetNext( pAPLName, pCurrentUserName, currentComponent,
      pNextUserName, pNextComponent );
}

/*********************************************************************
 * @purpose  configures the min password
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val     min length of the password
 *
 * @returns  L7_SUCCESS  if the min length of the password is set
 *           L7_FAILURE  if the min length of the password is not set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbMinPasswdSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return userMgrMinPasswdSet(val);
}

/*********************************************************************
 * @purpose  To get the min password length value
 *
 * @param   val - (output) min number of characters required for passwords
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbMinPassLengthGet(L7_ushort16 *val)
{
  return userMgrMinPassLengthGet(val);
}

/*********************************************************************
 * @purpose  To get the min password length value
 *
 * @param   none
 *
 * @returns  min password length is returned
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdLengthValid(L7_uint32 UnitIndex, L7_char8 * password, L7_BOOL encryptedFlag)
{
  return userMgrPasswdLengthValid(password, encryptedFlag);
}

/*********************************************************************
 * @purpose  configures the history table size
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val      length of the history table
 *
 * @returns  L7_SUCCESS if the history table size is set
 *           L7_FAILURE  if the history table size is not  set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdHistoryLengthSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return userMgrPasswdHistoryLengthSet(val);
}

/*********************************************************************
 * @purpose  configures the aging of the password
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val      aging value of the password
 *
 * @returns  L7_SUCCESS if the aging value is set
 *           L7_FAILURE  if the aging value is not set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdAgingSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return userMgrPasswdAgingSet(val);
}

/*********************************************************************
 * @purpose  configures the lockout count of the password
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    val      lock out count
 *
 * @returns  L7_SUCCESS if the lockout count is set
 *           L7_FAILURE  if the lockout count is not set
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdsLockoutSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return userMgrPasswdsLockoutSet(val);
}

/*********************************************************************
 * @purpose  Gives the time when the password got set
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (input ) index of user in login structure
 * @param    val      lock out count
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswordChangeTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time)
{
  return userMgrPasswordChangeTimeGet(index,time);
}

/*********************************************************************
 * @purpose  Gives the current time
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param   tempint   (output )pointer to the time variable
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdValidationTimeGet(L7_uint32 UnitIndex, L7_uint32 *tempInt)
{
  return userMgrPasswdValidationTimeGet(tempInt);
}

/*********************************************************************
* @purpose  decides whther the password is expired or not
*
* @param   UnitIndex (input) the unit for this operation
* @param    index    (input ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/


L7_BOOL usmDbPasswordExpiredCheck(L7_uint32 UnitIndex,L7_uint32 index)
{
    if (usmDbPasswdAgingStatusGet() != L7_SUCCESS)
    { /* password aging is disabled */
        return L7_FAILURE;
    }
    return userMgrPasswordExpiredCheck(index);
}

/*********************************************************************
 * @purpose  Gets the time the user's password expired
 *
 * @param   UnitIndex (input) the unit for this operation
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
L7_RC_t usmDbPasswordExpireTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *time)
{
  return userMgrExpireTimeGet(index, time);
}

/*********************************************************************
 * @purpose  decides whther the password is expired or not
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (output ) index of user in login structure
 * @param    name    (input ) user name
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbLoginUserIndexGet(L7_uint32 UnitIndex, L7_char8 *name, L7_uint32 *index)
{
  return(userMgrLoginIndexGet(name, index));
}

/*********************************************************************
 * @purpose  To know whether the password aging is enabled or not
 *
 * @param   none
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdAgingStatusGet()
{
  return userMgrPasswdAgingStatusGet();
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
L7_RC_t usmDbHistoryStatusGet()
{
  return userMgrHistoryStatusGet();
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
L7_RC_t usmDbLockoutStatusGet()
{
  return userMgrLockoutStatusGet();
}

/*********************************************************************
 * @purpose  To get the password aging value
 *
 * @param   val - (output) number of days the passwords are valid
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbPasswdAgingValueGet(L7_ushort16 *val)
{
  return userMgrPasswdAgingValueGet(val);
}

/*********************************************************************
 * @purpose  To get the history length value
 *
 * @param   val - (output) num passwords to store for prevention of reuse
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbHistoryLengthGet(L7_ushort16 *val)
{
  return userMgrHistoryLengthGet(val);
}

/*********************************************************************
 * @purpose  To get the number of user lockout attempts
 *
 * @param   val - (output) num failed attempts allowed before lockout
 *
 * @returns  L7_SUCCESS  if value retrieved
 *           L7_FAILURE  if value was not retrieved
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbLockoutAttemptsGet(L7_ushort16 *val)
{
  return userMgrLockoutAttemptsGet(val);
}

/*********************************************************************
 * @purpose  To know whether the user is locked or not
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (input ) index of user in login structure
 * @param    val      (output ) lock out status value
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserLockStatusGet(L7_uint32 UnitIndex, L7_uint32 index, L7_BOOL *val)
{
  return userMgrUserLockStatusGet(index, val);
}

/*********************************************************************
 * @purpose  To unlock the locked user
 *
 * @param   UnitIndex (input) the unit for this operation
 * @param    index    (output ) index of user in login structure
 *
 * @returns  L7_SUCCESS if the  value is proper
 *           L7_FAILURE  if the value is not proper
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserUnLock(L7_uint32 UnitIndex, L7_uint32 index)
{
  return userMgrUserUnLock(index);
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
L7_RC_t usmDbUserTotalLockoutCountGet(L7_uint32 index, L7_uint32 *val)
{
  return userMgrTotalLockoutCountGet(index, val);
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
L7_RC_t usmDbUserTotalLoginFailuresGet(L7_uint32 index, L7_uint32 *val)
{
  return userMgrTotalLoginFailuresGet(index, val);
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
L7_RC_t usmDbUserCurrentLockoutCountGet(L7_uint32 index, L7_uint32 *val)
{
  return userMgrCurrentLockoutCountGet(index, val);
}

/*********************************************************************
 * @purpose  Set enable password
 *
 * @param    password    (input) enable password
 * @param    encrypted   (input) flag indicating whether the password param is encrypted
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEnablePasswordSet(L7_uint32 level, L7_char8 *password, L7_BOOL encrypted)
{
  return userMgrEnablePasswordSet(level, password, encrypted);
}

/*********************************************************************
 * @purpose  Get enable password
 *
 * @param    password    (output) enable password
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEnablePasswordGet(L7_uint32 level,
                               L7_char8 *password)
{
  return userMgrEnablePasswordGet(level, password);
}
/*********************************************************************
 * @purpose  Removes the enable password
 *
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbEnablePasswordRemove(L7_uint32 level)
{
  return userMgrEnablePasswordRemove(level);
}
/*********************************************************************
 * @purpose  Set the Terminal Lines for show running-config
 *
 * @param unitIndex @b((input)) the unit for this operation
 *
 * @param    termLine   value of terminal lines for pagination<5-48>
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbTerminalLineSet(L7_uint32 unitIndex, L7_uint32 termLine)
{
  return cliWebSetTerminalLine(termLine);
}

/*********************************************************************
 * @purpose  Get the Terminal Lines for show running-config
 *
 * @param unitIndex @b((input)) the unit for this operation
 *
 * @param    *termLine   value of terminal lines for pagination<5-48>
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbTerminalLineGet(L7_uint32 UnitIndex, L7_uint32 *termLine)
{
  return cliWebGetTerminalLine(termLine);
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
void usmDbWebSessionTableTimeoutScan()
{
    cliWebEmWebSessionTableTimeoutScan();
}

/*********************************************************************
*
* @purpose Get the next valid index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 *index     pointer to table index
*
* @returns L7_SUCCESS, if the next index exist and it is vacant
* @returns L7_FAILURE, if the next index doesn't exist
*
* @notes Returns success if the next index exists.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbLoginUserIndexNextAvailableGet(L7_uint32 *index)
{
  return cliWebLoginUserAvailableIndexGet(index);
}

/*********************************************************************
* @purpose  Get the APL assigned to an access line
*
* @param    component (input) ID of component to use APL
* @param    pAPL     (output) pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if component id is not a component that handles APLs
*
* @notes pAPL must be at least L7_MAX_APL_NAME_SIZE+1 in length
*
* @end
*********************************************************************/
L7_RC_t usmDbAPLLineGet(L7_ACCESS_LINE_t accessLine,
                        L7_ACCESS_LEVEL_t accessLevel,
                        L7_char8 *pAPL)
{
    return userMgrAPLLineGet(accessLine, accessLevel, pAPL);
}

/*********************************************************************
* @purpose  Set the APL assigned to an access line
*
* @param    component (input) ID of component to use APL
* @param    pAPL     (output) pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE  if component id is not a component that handles APLs
*
* @notes pAPL must be at least L7_MAX_APL_NAME_SIZE+1 in length
*
* @end
*********************************************************************/
L7_RC_t usmDbAPLLineSet(L7_ACCESS_LINE_t accessLine,
                        L7_ACCESS_LEVEL_t accessLevel,
                        L7_char8 *pAPL)
{
    return userMgrAPLLineSet(accessLine, accessLevel, pAPL);
}

/*********************************************************************
* @purpose  Return the index of the specified APL in the APL list
*
* @param    UnitIndex (input) the unit for this operation
* @param    accessLevel (input) to diffentiate login and enable
* @param    pAPLName (input) name of APL
* @param    pIndex (output) ptr to APL index
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL name is not configured
*
* @notes This function can be used to find the first blank entry in the
*        APL list.  Therefore, a name of size 0 is valid.
*
* @end
*********************************************************************/
L7_RC_t usmDbAuthenticationListIndexGetByName(L7_ACCESS_LINE_t accessLine,
                                              L7_ACCESS_LEVEL_t accessLevel,
                                              L7_char8 *pName,
                                              L7_uint32 *pIndex )
{
  return userMgrAuthListIndexGet(accessLine, accessLevel, pName, pIndex);
}

/*********************************************************************
* @purpose  Removes the methods from a specific APL
*
* @param    UnitIndex (input) the unit for this operation
* @param    authMode <input>  the authentication Mode for this operation
* @param    pName    pointer to APL name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if APL does not exist or users have this APL assigned
*             and therefore it cannot be deleted or if APL name is not valid.
*
* @notes  pName must be null terminated.
*
* @end
*********************************************************************/
L7_RC_t usmDbAuthenticationListMethodRemove(L7_ACCESS_LINE_t accessLine,
                                            L7_ACCESS_LEVEL_t accessLevel,
                                            L7_char8 *pName)
{
  return userMgrAPLAuthMethodsRemove(accessLine, accessLevel, pName);
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


L7_RC_t usmDbMinPassStatusGet()
{
 return userMgrMinPassStatusGet();
}

/*********************************************************************
* @purpose  To add the entry into the login history table.
*
* @param   UnitIndex (input) the unit for this operation
* @param    index    (output ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t  usmdbUserMgrAddToLoginHistory(L7_uint32 UnitIndex, L7_char8 *name, L7_LOGIN_TYPE_t protocol, L7_inet_addr_t *location  )
{
   return userMgrAddToLoginHistory(name, protocol, location);
}
/*********************************************************************
* @purpose  To get the first entry from the history table.
*
* @param   UnitIndex (input) the unit for this operation
* @param    index    (output ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t  usmdbUserMgrLoginHistoryGetFirst(L7_uint32 UnitIndex, L7_uint32 *index, L7_char8 *name, L7_LOGIN_TYPE_t *protocol, L7_inet_addr_t  *location, L7_uint32 *time )
{
  return    userMgrLoginHistoryGetFirst(index,name,protocol,location,time);
}
/*********************************************************************
* @purpose  To get the next entry from the history table.
*
* @param   UnitIndex (input) the unit for this operation
* @param    index    (output ) index of user in login structure
*
* @returns  L7_SUCCESS if the  value is proper
*           L7_FAILURE  if the value is not proper
*
* @notes none
*
* @end
*********************************************************************/

L7_RC_t usmdbUserMgrLoginHistoryGetNext(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *nextIndex,L7_char8 *name, L7_LOGIN_TYPE_t *protocol, L7_inet_addr_t  *location, L7_uint32 *time )
{
  return userMgrLoginHistoryGetNext(index,nextIndex,name,protocol,location,time);
}

/*********************************************************************
* @purpose  Set the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (input) password to set
* @param    encrypted       (input) flag indicating whether the password param is encrypted
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinePasswordSet(L7_ACCESS_LINE_t accessLine,
                             L7_char8 *password,
                             L7_BOOL encrypted)
{
  return userMgrLinePasswordSet(accessLine, password, encrypted);
}

/*********************************************************************
* @purpose  Get the line password
*
* @param    accessLine      (input) line type (Console/Telnet/SSH))
* @param    password        (output) password to set
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbLinePasswordGet(L7_ACCESS_LINE_t accessLine,
                              L7_char8 *password)
{
  return userMgrLinePasswordGet(accessLine, password);
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
L7_RC_t usmDbLinePasswordRemove(L7_ACCESS_LINE_t accessLine)
{
  return userMgrLinePasswordRemove(accessLine);
}

/*********************************************************************
* @purpose  Check if Authentication is required
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAuthenticationRequired(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessMode)
{
  return userMgrIsAuthenticationRequired(accessLine, accessMode);
}

/*********************************************************************
* @purpose  Check if Authentication is required for this User
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAuthenticationUsernameRequired(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessMode)
{
  return userMgrIsAuthenticationUsernameRequired(accessLine, accessMode);
}

/*********************************************************************
* @purpose  Check if Prompt is required
*
* @param    accessLine   (input) line type (Console/Telnet/SSH))
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL usmDbIsAuthenticationPromptSupplied(L7_ACCESS_LINE_t accessLine, L7_ACCESS_LEVEL_t accessMode)
{
  return userMgrIsAuthenticationPromptSupplied(accessLine, accessMode);
}



/******** STRONG PASSWORD USMDB APIs ********/

/*********************************************************************
 * @purpose To configure the min uppercase letters password length
 *
 * @param val -Min uppercase letters length length
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the min uppercase letters length and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinUppercaseCharsLengthSet(L7_uint8 val)
{ 
  return userMgrPasswdMinUppercaseCharsLengthSet(val);
}

/*********************************************************************
 * @purpose  To get the min uppercase letters password length value
 *
 * @param   val - (output) min number of uppercase characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinUppercaseCharsLengthGet(L7_uint8 *val)
{ 
  return userMgrPasswdMinUppercaseCharsLengthGet(val);
}

/*********************************************************************
 * @purpose To configure the min lowercase letters password length
 *
 * @param val -Min lowercase letters length length
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the min lowercase letters length and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinLowercaseCharsLengthSet(L7_uint8 val)
{ 
  return userMgrPasswdMinLowercaseCharsLengthSet(val);
}


/*********************************************************************
 * @purpose  To get the min lowercase letters password length value
 *
 * @param   val - (output) min number of lowercase characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinLowercaseCharsLengthGet(L7_uint8 *val)
{ 
  return userMgrPasswdMinLowercaseCharsLengthGet(val);
}

/*********************************************************************
 * @purpose To configure the min Numeric letters password length
 *
 * @param val -Min numeric letters length length
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the min numeric letters length and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinNumericCharsLengthSet(L7_uint8 val)
{ 
  return userMgrPasswdMinNumericCharsLengthSet(val);
}


/*********************************************************************
 * @purpose  To get the min Numeric characters password length value
 *
 * @param   val - (output) min number of numeric characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinNumericCharsLengthGet(L7_uint8 *val)
{ 
  return userMgrPasswdMinNumericCharsLengthGet(val);
}

/*********************************************************************
 * @purpose To configure the min Special characters password length
 *
 * @param val -Min special characters length length
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the min special characters length and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinSpecialCharsLengthSet(L7_uint8 val)
{ 
  return userMgrPasswdMinSpecialCharsLengthSet(val);
}

/*********************************************************************
 * @purpose  To get the min Special characters password length value
 *
 * @param   val - (output) min number of Special characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinSpecialCharsLengthGet(L7_uint8 *val)
{ 
  return userMgrPasswdMinSpecialCharsLengthGet(val);
}

/*********************************************************************
 * @purpose To configure the max Consecutive characters password length
 *
 * @param val -Max Consecutive characters length length
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the maximum consecutive characters length and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxConsecutiveCharsLengthSet(L7_uint8 val)
{ 
  return userMgrPasswdMaxConsecutiveCharsLengthSet(val);
}

/*********************************************************************
 * @purpose  To get the max consecutive characters password length value
 *
 * @param   val - (output) max number of consecutive characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxConsecutiveCharsLengthGet(L7_uint8 *val)
{ 
  return userMgrPasswdMaxConsecutiveCharsLengthGet(val);
}


/*********************************************************************
 * @purpose To configure the max Repeated characters password length
 *
 * @param val -Max Repeated characters length length
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the maximum repeated characters length and to
 * disable the already configued value.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxRepeatedCharsLengthSet(L7_uint8 val)
{ 
  return userMgrPasswdMaxRepeatedCharsLengthSet(val);
}

/*********************************************************************
 * @purpose  To get the max repeated characters password length value
 *
 * @param   val - (output) max number of repeated characters required for passwords
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxRepeatedCharsLengthGet(L7_uint8 *val)
{ 
  return userMgrPasswdMaxRepeatedCharsLengthGet(val);
}


/*********************************************************************
 * @purpose To retrieve the min Character Classes configured
 *
 * @param val - Min Character classes 
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the min character classes set
 *        should be present in a Password among Uppercase, Lowercase, 
 *        numeric or special character.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinCharacterClassesGet(L7_uint8 *val)
{ 
  return userMgrPasswdMinCharacterClassesGet(val);
}


/*********************************************************************
 * @purpose To configure the min Character Classes
 *
 * @param val - Min Character classes 
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the min character classes set
 *        should be present in a Password among Uppercase, Lowercase, 
 *        numeric or special character.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdMinCharacterClassesSet(L7_uint8 val)
{ 
  return userMgrPasswdMinCharacterClassesSet(val);
}

/*********************************************************************
 * @purpose To retrieve the next valid exclude keyword
 *
 * @param   pKeyword (output) Reference to the keyword
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the index to the excluded 
 *        keywords configured
 *        
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeKeywordNextGet(L7_char8 *pKeyword)
{
  return userMgrPasswdExcludeKeywordNextGet(pKeyword);
}

/*********************************************************************
 * @purpose To configure the keyword for a password exclude list
 *
 * @param   keyword (input) Reference to the keyword
 *
 * @returns L7_SUCCESS - on successfully configured
 * @returns L7_TABLE_IS_FULL -if it reaches Max Exclude keywords, no free
 *                            slot to configure the new keyword.
 * @returns L7_ERROR  - if the length of the given keyword is invalid
 * @returns L7_FAILURE -if the given keyword is NULL
 *
 * @notes This function is used to configure the keyword to retrict it
 *        from the password while configuring.
 *
 * @end
 **********************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeKeywordSet(L7_uchar8 *keyword)
{
  return userMgrPasswdExcludeKeywordSet(keyword);
}

/*********************************************************************
 * @purpose Delete the keyword from a list of password exclude keywords
 *
 * @param   keyword (input) Reference to the keyword
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the keyword to retrict it
 *        from the password while configuring.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeKeywordDelete(L7_uchar8 *keyword)
{
  return userMgrPasswdExcludeKeywordDelete(keyword);
}

/*********************************************************************
 * @purpose Delete All keywords from a list of password exclude keywords
 *
 * @param   void
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the keyword to retrict it
 *        from the password while configuring.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeKeywordDeleteAll()
{
  return userMgrPasswdExcludeKeywordDeleteAll();
}

/*********************************************************************
 * @purpose To Check whether the given keyword is present inthe list
 *
 * @param   pKeyword (output) Reference to the keyword
 *
 * @returns L7_SUCCESS -if the string is exist in the list
 *
 * @returns L7_FAILURE -if the string is not exist in the list
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdIsExcludeKeywordExist(L7_char8 *pKeyword)
{
  return userMgrPasswdIsExcludeKeywordExist(pKeyword);
}

/*********************************************************************
 * @purpose To Enable/Disable Password Strength feature
 *
 * @param val - Boolean value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the Password strength support
 *        to the password management.
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswordStrengthCheckConfigSet(L7_BOOL val)
{ 
  return userMgrPasswordStrengthCheckConfigSet(val);
}

/*********************************************************************
 * @purpose  To get the Password Strength Enable value
 *
 * @param   val - (output) Password Strength configured value
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswordStrengthCheckConfigGet(L7_BOOL *val)
{ 
  return userMgrPasswordStrengthCheckConfigGet(val);
}

/*********************************************************************
 * @purpose To Enable/Disable Password Strength Override Check
 *
 * @param loginIndex - (input) Index to the login name
 * @param val        - (input) Boolean value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes Enabling the override check restricts the password strength
 *        validation
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrLoginPasswdStrengthOverrideCheckSet(L7_uint32 loginIndex, L7_BOOL val)
{
  return userMgrLoginPasswdStrengthOverrideCheckSet(loginIndex, val);
}

/*********************************************************************
 * @purpose  To get the Password Strength override check value
 *
 * @param   loginIndex - (input) Index to the login name
 * @param   val        - (output) Password Strength override check value
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrLoginPasswdStrengthOverrideCheckGet(L7_uint32 loginIndex, L7_BOOL *val)
{
  return userMgrLoginPasswdStrengthOverrideCheckGet(loginIndex, val);
}

/*********************************************************************
 * @purpose To Verify if the given Password is Strong
 *
 * @param    userName    pointer to login user Name
 * @param    password    pointer to login user password
 * @param    encrypted   indicates whether the password param is in encrypted form
 *
 * @returns L7_SUCCESS -if it validates as Strong Password
 *
 * @returns L7_USER_MGR_ERROR_xxx - If it is not a strong password
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswordIsStrong(L7_char8  *userName,
                                     L7_char8  *password,
                                     L7_BOOL   encryptedFlag)
{ 
  return userMgrPasswordIsStrong(userName,
                                 password,
                                 encryptedFlag);
}


/*************************************************************************
 * @purpose To retreive the exclude Login Name Check for a Strong Password 
 *          validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the Login Name Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeLoginNameCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdExcludeLoginNameCheckGet(enable);
}


/**************************************************************************
 * @purpose To configure the exclude Login Name Check for a Strong Password 
 *          validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the Login Name Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeLoginNameCheckSet(L7_BOOL enable)
{
  return userMgrPasswdExcludeLoginNameCheckSet(enable);
}

/*************************************************************************
 * @purpose To retreive the exclude keywords Check for a Strong Password 
 *          validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retreive the exclude keyword Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeKeywordsCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdExcludeKeywordsCheckGet(enable);
}

/**************************************************************************
 * @purpose To configure the exclude keywords for a Strong Password 
 *          validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to configure the exclude keyword Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdExcludeKeywordsCheckSet(L7_BOOL enable)
{
  return userMgrPasswdExcludeKeywordsCheckSet(enable);
}

/*************************************************************************
 * @purpose To retrieve the minimum character classes Check for a Strong 
 *          Password validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the minimum character classes 
 *        Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinCharacterClassesCheckGet(L7_BOOL *enable)
{ 
  return userMgrPasswdMinCharacterClassesCheckGet(enable);
}


/**************************************************************************
 * @purpose To configure the minimum character classes Check for a Strong 
 *          Password validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes   This function is used to configure the minimum character classes
 *          Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinCharacterClassesCheckSet(L7_BOOL enable)
{
  return userMgrPasswdMinCharacterClassesCheckSet(enable);
}


/*************************************************************************
 * @purpose To retrieve the minimum uppercase letters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the minimum upercase letters 
 *        Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinUppercaseLettersCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdMinUppercaseLettersCheckGet(enable);
}



/**************************************************************************
 * @purpose To configure the minimum uppercase letters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes   This function is used to configure the minimum uppercase letters
 *          Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinUppercaseLettersCheckSet(L7_BOOL enable)
{
  return userMgrPasswdMinUppercaseLettersCheckSet(enable);
}


/*************************************************************************
 * @purpose To retrieve the minimum lowercase letters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the minimum lowercase letters 
 *        Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinLowercaseLettersCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdMinLowercaseLettersCheckGet(enable);
}


/**************************************************************************
 * @purpose To configure the minimum lowercase letters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes   This function is used to configure the minimum lowerase letters
 *          Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinLowercaseLettersCheckSet(L7_BOOL enable)
{
  return userMgrPasswdMinLowercaseLettersCheckSet(enable);
}


/*************************************************************************
 * @purpose To retrieve the minimum numeric characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the minimum numeric characters 
 *        Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinNumericCharactersCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdMinNumericCharactersCheckGet(enable);
}

/**************************************************************************
 * @purpose To configure the minimum numeric characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes   This function is used to configure the minimum numeric characters
 *          Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinNumericCharactersCheckSet(L7_BOOL enable)
{
  return userMgrPasswdMinNumericCharactersCheckSet(enable);
}

/*************************************************************************
 * @purpose To retrieve the minimum special characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the minimum special characters 
 *        Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinSpecialCharactersCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdMinSpecialCharactersCheckGet(enable);
}

/**************************************************************************
 * @purpose To configure the minimum special characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes   This function is used to configure the minimum special characters
 *          Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdMinSpecialCharactersCheckSet(L7_BOOL enable)
{
  return userMgrPasswdMinSpecialCharactersCheckSet(enable);
}

/*************************************************************************
 * @purpose To retrieve the maximum consecutive characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the max consecutive characters 
 *        Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxConsecutiveCharactersCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdMaxConsecutiveCharactersCheckGet(enable);
}

/**************************************************************************
 * @purpose To configure the maximum consecutive characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes   This function is used to configure the max consecutive characters
 *          Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxConsecutiveCharactersCheckSet(L7_BOOL enable)
{
  return userMgrPasswdMaxConsecutiveCharactersCheckSet(enable);
}

/*************************************************************************
 * @purpose To retrieve the maximum repeated characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (output) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes This function is used to retrieve the max repeated characters 
 *        Check
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxRepeatedCharactersCheckGet(L7_BOOL *enable)
{
  return userMgrPasswdMaxRepeatedCharactersCheckGet(enable);
}

/**************************************************************************
 * @purpose To configure the maximum repeated characters Check for a Strong 
 *          Password validation
 *
 * @param   enable - (input) Boolean Value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes   This function is used to configure the maximum repeated characters
 *          Check
 *
 * @end
 **************************************************************************/
L7_RC_t usmDbUserMgrPasswdMaxRepeatedCharactersCheckSet(L7_BOOL enable)
{
  return userMgrPasswdMaxRepeatedCharactersCheckSet(enable);
}

/*********************************************************************
 * @purpose To Set the Password Strength score
 *
 * @param   val (input) - score value
 *
 * @returns L7_SUCCESS -if the value supplied is correct
 *
 * @returns L7_FAILURE -if the value supplied is not correct
 *
 * @notes
 * 
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdStrengthScoreSet(L7_uint8 val)
{
  return userMgrPasswordStrengthScoreSet(val);
}


/*********************************************************************
 * @purpose  To get the Password Strength score value
 *
 * @param   val - (output) Password Strength score value
 *
 * @returns  L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswdStrengthScoreGet(L7_uint8 *val)
{
  return userMgrPasswordStrengthScoreGet(val);
}


/*********************************************************************
 * @purpose To Get the Last Password Result
 *
 * @param    authMethod      pointer to the Authentication Method
 * @param    index           table index to the auth Method
 * @param    strengthCheck   pointer to password strength check
 * @param    result          pointer to result of the last password configuration 
 *
 * @returns L7_SUCCESS/L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrLastPasswordResultGet(L7_uint32 *authMethod,
                                          L7_uint32 *index,
                                          L7_BOOL   *strengthCheck,
                                          L7_uint32 *result)
{
  return userMgrLastPasswordResultGet(authMethod, index, strengthCheck, result);
}

/*********************************************************************
 * @purpose  Get the Password Set Result in string format
 *
 * @param    pResult  (output)  reference to the Password Result
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 * @end
 *********************************************************************/
L7_RC_t usmDbUserMgrPasswordLastSetResultStringGet(L7_char8 *pResult)
{
  return userMgrPasswordLastSetResultStringGet(pResult);
}



/******** STUB FUNCTIONS **********/
void usmDbUserMgrStubCalls()
{
  L7_BOOL enable;

  (void)usmDbUserMgrPasswdExcludeLoginNameCheckGet(&enable); 
  (void)usmDbUserMgrPasswdExcludeKeywordsCheckGet(&enable);
  (void)usmDbUserMgrPasswdMinCharacterClassesCheckGet(&enable);
  (void)usmDbUserMgrPasswdMinUppercaseLettersCheckGet(&enable);
  (void)usmDbUserMgrPasswdMinLowercaseLettersCheckGet(&enable);
  (void)usmDbUserMgrPasswdMinNumericCharactersCheckGet(&enable);
  (void)usmDbUserMgrPasswdMinSpecialCharactersCheckGet(&enable);
  (void)usmDbUserMgrPasswdMaxConsecutiveCharactersCheckGet(&enable);
  (void)usmDbUserMgrPasswdMaxRepeatedCharactersCheckGet(&enable);
}

