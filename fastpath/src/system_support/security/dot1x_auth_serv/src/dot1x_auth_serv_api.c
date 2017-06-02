/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
* @filename  dot1x_auth_serv_api.c
*
* @purpose   Dot1x configuration manager api functions (used by cli, web, snmp)
*
* @component Dot1x Authentication Server component
*
* @comments  none
*
* @create    11/17/2009
*
* @author    pradeepk
*
* @end
*
**********************************************************************/

#include <strings.h>
#include "l7_common.h"
#include "osapi.h"
#include "cli_web_exports.h"
#include "pw_scramble_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "dot1x_auth_serv.h"
#include "dot1x_auth_serv_util.h"
#include "dot1x_auth_serv_exports.h"

dot1xAuthServCfgData_t dot1xAuthServCfgData;
osapiRWLock_t     dot1xAuthServUserDBRWLock;

/*********************************************************************
 * @purpose  Returns user name per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *name    @b{(output)} user name. 
 * @param    nameLen @b{(input)}  username length. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserNameGet(L7_uint32 index, L7_char8 *name, L7_uint32 nameLen)
{
  if (index >= L7_MAX_IAS_USERS)
    return(L7_FAILURE);

  if (name == L7_NULL)
    return L7_FAILURE;

  if (nameLen > L7_LOGIN_SIZE)
    nameLen = L7_LOGIN_SIZE;

  (void)osapiReadLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  osapiStrncpySafe(name, dot1xAuthServCfgData.dot1xUsers[index].name, nameLen);

  /* release semaphore*/
  (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Returns table index of user name
 *
 * @param    *name    @b{(input)}  user name. 
 * @param    *index    @b{(output)}  table index. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserIndexGet(L7_char8 *name, L7_uint32 *index)
{
  L7_uint32 i;
  L7_uint32 nameLen;

  if (name == L7_NULL)
  {
    return(L7_FAILURE);
  }

  nameLen = strlen(name);
  /* First check whether the argument name string
   * has some value inside it or not
   */
  if (nameLen == 0)
  {
    return(L7_FAILURE);
  }

  (void)osapiReadLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  for (i = 0; i < L7_MAX_IAS_USERS; i++)
  {
    if (nameLen == strlen(dot1xAuthServCfgData.dot1xUsers[i].name))
    {
      if (strcasecmp(name, dot1xAuthServCfgData.dot1xUsers[i].name) == 0)
        break;
    }
  }

  if (i == L7_MAX_IAS_USERS)
  {
    (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);
    return(L7_FAILURE);
  }
  *index = i;
  /* release semaphore*/
  (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Returns table index of next available (vacant) table index
 *
 * @param    *index    @b{(input)}  table index. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE if no available index exists
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBAvailableIndexGet(L7_uint32 *index)
{
  L7_uint32 i;

  (void)osapiReadLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  for (i=0; i < L7_MAX_IAS_USERS; i++)
  {
    if (strlen(dot1xAuthServCfgData.dot1xUsers[i].name) == 0)
      break;
  }

  if (i == L7_MAX_IAS_USERS)
  {
    (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);
    return(L7_FAILURE);
  }
  *index = i;
  /* release semaphore*/
  (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Sets a user name per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *name    @b{(input)}  user name. 
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if name is too long or
 *                      if name contains an invalid character
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserNameSet(L7_uint32 index, L7_char8 *name)
{
  L7_uint32 oldIndex;

  if (name == L7_NULL)
    return L7_FAILURE;
  
  if ((index >= L7_MAX_IAS_USERS) || (strlen(name) >= L7_LOGIN_SIZE) ||
      (usmDbStringAlphaNumericCheck(name) != L7_SUCCESS))
    return(L7_FAILURE);

  /* Check if this name already exists for another user */
  if (dot1xAuthServUserDBUserIndexGet(name, &oldIndex) == L7_SUCCESS &&
      oldIndex != index )
  {
    return L7_FAILURE;
  }

  (void)osapiWriteLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  osapiStrncpySafe(dot1xAuthServCfgData.dot1xUsers[index].name, name, L7_LOGIN_SIZE);

  dot1xAuthServCfgData.dot1xUsers[index].userStatus = L7_ENABLE;
  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_TRUE;

  (void)osapiWriteLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Returns user encrypted password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(output)}  encrypted user password. 
 * @param    pwdLen    @b{(input)}  encrypted user password length. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserPasswordGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen)
{
  if (index >= L7_MAX_IAS_USERS)
    return(L7_FAILURE);

  if (password == L7_NULL)
    return L7_FAILURE;
 
  if (pwdLen > L7_ENCRYPTED_PASSWORD_SIZE)
    pwdLen = L7_ENCRYPTED_PASSWORD_SIZE;
 
  (void)osapiReadLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  osapiStrncpySafe(password, dot1xAuthServCfgData.dot1xUsers[index].password, pwdLen);

  /* release semaphore*/
  (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns encrypted user password
*
* @param    *password    @b{(input)}  clear text password. 
* @param    *encryptedpwd    @b{(output)}  encrypted password. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1xAuthServUserDBUserPasswordEncrypt(L7_char8 *password, L7_char8 *encryptedpwd)
{
  if ((password == L7_NULL) || (encryptedpwd == L7_NULL))
    return L7_FAILURE;

  return pwEncrypt(password, encryptedpwd, L7_PASSWORD_SIZE - 1, L7_PASSWORD_ENCRYPT_ALG);
}

/*********************************************************************
 * @purpose  Returns user cleartext password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(output)}  user password in clear text format.
 * @param    pwdLen    @b{(input)}  clear text user password length. 
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserPasswordClearTextGet(L7_uint32 index, L7_char8 *password, L7_uint32 pwdLen)
{
  L7_RC_t rc;

  if (index >= L7_MAX_IAS_USERS)
    return L7_FAILURE;
 
  if (password == L7_NULL)
    return L7_FAILURE;

  if (pwdLen > L7_PASSWORD_SIZE)
    pwdLen = L7_PASSWORD_SIZE;

  (void)osapiReadLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  if (strlen(dot1xAuthServCfgData.dot1xUsers[index].password) == 0)
  {
    *password = '\0';

    /* release semaphore*/
    (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);
    return L7_SUCCESS;
  }

  rc = pwDecrypt(password, dot1xAuthServCfgData.dot1xUsers[index].password, pwdLen-1);
  /* release semaphore*/
  (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);

  return rc;
}

/*********************************************************************
 * @purpose  Sets a user password per index
 *
 * @param    index    @b{(input)}  table index. 
 * @param    *password    @b{(input)}  user password. 
 * @param    encrypted    @b{(input)}  indicates whether the password param is in
 *                                      encrypted form. 
 *
 * @returns  L7_SUCCESS
 *           L7_FAILURE if password is too long or index is too large
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted)
{
  L7_char8 oldPassword[L7_ENCRYPTED_PASSWORD_SIZE];

  if (index >= L7_MAX_IAS_USERS)
    return(L7_FAILURE);

  (void)osapiWriteLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  osapiStrncpySafe( oldPassword, dot1xAuthServCfgData.dot1xUsers[index].password, L7_ENCRYPTED_PASSWORD_SIZE);

  if (encrypted == L7_TRUE)
  {
    osapiStrncpySafe(dot1xAuthServCfgData.dot1xUsers[index].password, password, L7_ENCRYPTED_PASSWORD_SIZE);
  }
  else
  {
    pwEncrypt(password, dot1xAuthServCfgData.dot1xUsers[index].password, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
  }

  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_TRUE;
  /* release semaphore*/
  (void)osapiWriteLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Returns user status per index
*
* @param    index    @b{(input)}  table index. 
* @param    *status    @b{(output)}  status of the user. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1xAuthServUserDBUserStatusGet(L7_uint32 index, L7_uint32 *status)
{
  if (index >= L7_MAX_IAS_USERS)
    return(L7_FAILURE);

  (void)osapiReadLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  *status = dot1xAuthServCfgData.dot1xUsers[index].userStatus;

  /* release semaphore*/
  (void)osapiReadLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Sets the status of the user
*
* @param    index    @b{(input)}  table index. 
* @param    status    @b{(input)}  status of the user. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1xAuthServUserDBUserStatusSet(L7_uint32 index, L7_uint32 status)
{
  if (index >= L7_MAX_IAS_USERS)
    return(L7_FAILURE);

  (void)osapiWriteLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);

  dot1xAuthServCfgData.dot1xUsers[index].userStatus = status;
  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_TRUE;
  /* release semaphore*/
  (void)osapiWriteLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Delete a user entry from the Dot1x user database
 *
 * @param    index    @b{(input)}  table index. 
 *
 * @returns  L7_SUCCESS
 * @returns L7_FAILURE if attempt to remove admin
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t dot1xAuthServUserDBUserDelete(L7_uint32 index)
{
  if (index >= L7_MAX_IAS_USERS)
    return(L7_FAILURE);

  (void)osapiWriteLockTake(dot1xAuthServUserDBRWLock, L7_WAIT_FOREVER);
 
  memset( dot1xAuthServCfgData.dot1xUsers[index].name, 0, L7_LOGIN_SIZE );
  memset( dot1xAuthServCfgData.dot1xUsers[index].password, 0, L7_ENCRYPTED_PASSWORD_SIZE );
  dot1xAuthServCfgData.dot1xUsers[index].userStatus= L7_DISABLE;
  dot1xAuthServCfgData.cfgHdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(dot1xAuthServUserDBRWLock);

  return(L7_SUCCESS);
}

