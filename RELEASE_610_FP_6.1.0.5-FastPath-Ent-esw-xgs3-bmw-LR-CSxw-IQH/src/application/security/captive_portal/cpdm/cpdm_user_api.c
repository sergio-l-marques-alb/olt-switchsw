/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpdm_user_api.c
*
* @purpose      Captive Portal Data Manager (CPDM) API functions
*
* @component    CPDM
*
* @comments     APIs for usmDb layer. These APIs can be called via usmDb
*               and by the captive portal components.
*
* @create       07/26/2007
*
* @author       rjindal,dcaugherty,darsen
*
* @end
*
**********************************************************************/

#include <string.h>
#include "osapi.h"
#include "l7_ip_api.h"
#include "pw_scramble_api.h"
#include "user_mgr_api.h"
#include "captive_portal_commdefs.h"
#include "cpdm_user_api.h"
#include "cpdm_connstatus_util.h"
#include "cpdm_connstatus_api.h"
#include "cpdm.h"

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);  
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);  

typedef enum {
  FIND_ASSOC_EXACT = 0,
  FIND_ASSOC_NEXT,
  FIND_ASSOC_ADD
} find_assoc_t;

static L7_uint32 userCount = 0;
static L7_uint32 groupCount = 0;
static L7_uint32 userGroupAssocEntryCount = 0;

/*********************************************************************
*
* @purpose  Find a tree entry with the given user id as a key
*
* @param    uId_t  uId   @b{(input)} the key, of course
* @param    L7_BOOL getNext @b{(input)} use next value if not found?
*
* @returns  void  pointer to cpdmUserGroupData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
L7_int32 cpdmUserEntryFindIndex(uId_t uId, L7_BOOL getNext)
{
  cpdmUser_t *pUserEntry = L7_NULLPTR;
  cpdmUser_t *pBase = L7_NULLPTR;
  L7_int32  rc  = -1;
  L7_uint32 idx = 0; 

  if (L7_NULLPTR == cpdmCfgData)
  {
    return -2;
  }
  if (0 == userCount)
  {
    return -3;
  }
  pBase = &(cpdmCfgData->cpUsersData.cpUsers[0]);
  while(idx < userCount)
  {
    pUserEntry = &(pBase[idx]);

    if (0 == pUserEntry->uId)
    {
      break; /* No users left */
    }

    if (L7_TRUE == getNext) /* "next" entry will do */
    {
      if (uId < pUserEntry->uId)
      {
        rc = (L7_int32) idx;
        break;
      }
    }
    else /* we want an exact match only */
    {
      if (uId == pUserEntry->uId)
      {
        rc = (L7_int32) idx;
        break;
      }
    }
    idx++;
  }
  return rc;
}

static cpdmUser_t * cpdmUserEntryFindUser(uId_t uId, L7_BOOL getNext)
{
  cpdmUser_t *pUserEntry = L7_NULLPTR;
  L7_int32 idx = cpdmUserEntryFindIndex(uId, getNext);

  if (idx >= 0)
  {
    pUserEntry = &(cpdmCfgData->cpUsersData.cpUsers[idx]);
  }
  return pUserEntry;
}

/*********************************************************************
*
* @purpose  Updates count of user entries after save/restore
*
* @param    none
*
* @returns  nothing
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmUserEntryCountUpdate(void)
{
  L7_uint32 count = 0;

  SEMA_TAKE(WRITE);
  if (L7_NULLPTR != cpdmCfgData)
  {
    cpdmUser_t *pBase = &(cpdmCfgData->cpUsersData.cpUsers[0]);
    for(count = 0; count < FD_CP_LOCAL_USERS_MAX; count++)
    {
      if (0 ==  strlen(pBase[count].loginName))
      {
      break;
      }
    }
  }
  userCount = count;
  SEMA_GIVE(WRITE);
}

/*********************************************************************
*
* @purpose  Add a new user entry
*
* @param    uId_t uId @b{(input)} user id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ALREADY_CONFIGURED
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryAdd(uId_t uId)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pData = L7_NULLPTR;
  cpdmUser_t *pBase = L7_NULLPTR;
  L7_uint32 idx;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  do 
  {
    pBase = &(cpdmCfgData->cpUsersData.cpUsers[0]);
    for(idx = 0; idx < userCount; idx++) 
    {
      pData = &(pBase[idx]);
      if (0 == pData->uId)
      {
        break; /* at end of entries, found our index */
      }
      else if (uId < pData->uId)
      {
        break;
      }
      else if (uId == pData->uId)
      {
        rc = L7_ALREADY_CONFIGURED;
        break;
      }
    }

    if (L7_ALREADY_CONFIGURED == rc)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
              "%s: User id %d already exists.",
              __FUNCTION__,uId);
      break;
    }

    if (FD_CP_LOCAL_USERS_MAX == idx)
    {
      LOG_MSG("%s: Already reached maximum user capacity.",__FUNCTION__);
      break;
    }

    if (userCount != idx)
    {
      /* push up later entries */
      memmove(&(pBase[idx + 1]), &(pBase[idx]), 
              (userCount - idx) * sizeof(cpdmUser_t));
    }
    else
    {
      pData = &(pBase[idx]);
    }

    memset(pData, 0, sizeof(cpdmUser_t));
    pData->uId = uId;
    userCount++;
    osapiSnprintf(pData->loginName, CP_USER_LOCAL_USERNAME_MAX, FD_CP_LOCAL_DEFAULT_USER_NAME, uId);
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
*
* @purpose  Delete existing user entry
*
* @param    uId_t uId @b{(input)} user id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Also de-authenticate the user if connected 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryDelete(uId_t uId)
{ 
  L7_uchar8 loginName[CP_USER_LOCAL_USERNAME_MAX + 1];
  cpdmUser_t   *pBase = L7_NULLPTR;
  L7_RC_t      rc = L7_FAILURE;
  L7_int32     idx;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  pBase = &(cpdmCfgData->cpUsersData.cpUsers[0]);
  do 
  {
    idx = cpdmUserEntryFindIndex(uId, L7_FALSE);
    if (0 > idx)
    {
      break; /* Doesn't exist, leave now */
    }
    userCount--;
    memmove(&(pBase[idx]), &(pBase[idx + 1]), 
      (userCount - idx) * sizeof(cpdmUser_t));
    memset(&(pBase[userCount]), 0, sizeof(cpdmUser_t));
    cpdmCfgData->hdr.dataChanged = L7_TRUE;

    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  if (L7_SUCCESS == rc)
  {
    memset(loginName,0,sizeof(loginName));
    cpdmUserEntryLoginNameGet(uId,loginName);
    rc = cpdmClientConnStatusByNameDelete(loginName);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Retrieve current count of user entries
*
* @param    L7_uint32 *count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryCountGet(L7_uint32 *count) 
{
  if ((L7_NULLPTR == count) || 
      (L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(READ);
  *count = userCount;
  SEMA_GIVE(READ);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Check if user exists
*
* @param    uId_t uId @b{(input)} user Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryGet(uId_t uId)  
{ 
  cpdmUser_t *pUser = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(READ);
  pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
  SEMA_GIVE(READ);

  if (L7_NULLPTR != pUser)
  {
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Get the next user entry
*
* @param    uId_t uId @b{(input)} current user id 
* @param    uId_t * nextUId @b{(output)} next user id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryNextGet(uId_t uId, uId_t *nextUId) 
{ 
  cpdmUser_t *pUser = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == nextUId) || 
      (L7_NULLPTR == cpdmCfgData))
  {
    return rc;
  }
  
  SEMA_TAKE(READ);

  pUser = cpdmUserEntryFindUser(uId, L7_TRUE);
  if (L7_NULLPTR != pUser)
  {
    *nextUId = pUser->uId;
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
*
* @purpose  Check if user exists
*
* @param    L7_char8 *loginName @b{(input)} user name
* @param    uId_t uId @b{(output)} assigned user id 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryByNameGet(L7_char8 *loginName, uId_t *uId) 
{ 
  L7_uchar8 tmpLoginName[CP_USER_LOCAL_USERNAME_MAX + 1];
  L7_RC_t rc = L7_FAILURE;
  uId_t tmpUId;

  if ((L7_NULLPTR == uId) || 
      (L7_NULLPTR == cpdmCfgData))
  {
    return rc;
  }

  tmpUId = 0;
  while (L7_SUCCESS==cpdmUserEntryNextGet(tmpUId,&tmpUId))
  {
    memset(tmpLoginName,0,sizeof(tmpLoginName));
    if (L7_SUCCESS!=cpdmUserEntryLoginNameGet(tmpUId,tmpLoginName))
    {
      break;
    }

    if (osapiStrncmp(tmpLoginName,loginName,CP_USER_LOCAL_USERNAME_MAX) == 0)
    {
      *uId = tmpUId;
      rc = L7_SUCCESS;
      break;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Delete all user entries with extreme prejudice
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Will also wipe out all active connections 
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryPurge(void)
{
  L7_uchar8 loginName[CP_USER_LOCAL_USERNAME_MAX + 1];
  L7_RC_t rc = L7_SUCCESS;
  uId_t uId = 0;

  /* Wipe out connections first.  This is because we run the risk of
   * writing faulty configuration data if we blow away the user DB
   * immediately.
   */
  while((L7_SUCCESS == rc) &&
        (L7_SUCCESS == cpdmUserEntryNextGet(uId, &uId)))
  {
    cpdmUserGroupAssocEntryByUIdDeleteAll(uId);
    memset(loginName,0,sizeof(loginName));
    cpdmUserEntryLoginNameGet(uId,loginName);
    rc = cpdmClientConnStatusByNameDelete(loginName);
  }

  if (L7_SUCCESS == rc)  /* Destroy all users */
  {
     SEMA_TAKE(WRITE);
     memset(&(cpdmCfgData->cpUsersData), 0, sizeof(cpdmUsersData_t));
     userCount = 0;
     cpdmOprData->cpGlobalStatus.users.authenticated = 0;
     cpdmCfgData->hdr.dataChanged = L7_TRUE;
     SEMA_GIVE(WRITE);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Set a new user/login name
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *userName @b{(input)} the new login name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryLoginNameSet(uId_t uId, L7_char8 *loginName)
{
  cpdmUser_t *pUser = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  uId_t tmpUId = 0;
  do
  {
    L7_uint32 login_len;

    if (L7_NULLPTR == loginName)
    {
      break;
    }

    /* Is login name valid? */
    login_len = strlen(loginName);
    if ((login_len <  CP_USER_LOCAL_USERNAME_MIN) || 
        (login_len >  CP_USER_LOCAL_USERNAME_MAX))
    {
      break;
    }

    if (L7_SUCCESS==cpdmUserEntryByNameGet(loginName,&tmpUId))
    {
      break; /* No duplicates allowed */
    }

    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (osapiStrncmp(pUser->loginName, loginName, CP_USER_LOCAL_USERNAME_MAX) != 0)
      {
        osapiStrncpySafe(pUser->loginName, loginName, CP_USER_LOCAL_USERNAME_MAX+1);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  return rc;
}

/*********************************************************************
*
* @purpose  Set a new user password
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *password @b{(input)} the new password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryPasswordSet(uId_t uId, L7_char8 *password)
{
  cpdmUser_t *pUser = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  do
  {
    L7_uint32 pwd_len;
    L7_char8 encryptedPassword[L7_ENCRYPTED_PASSWORD_SIZE];

    if (L7_NULLPTR == password)
    {
      break;
    }

    /* Is password valid? */
    pwd_len = strlen(password);
    if ((pwd_len <  CP_USER_LOCAL_PASSWORD_MIN) || 
        (pwd_len >  CP_USER_LOCAL_PASSWORD_MAX))
    {
      break;
    }

    memset(encryptedPassword, 0, sizeof(encryptedPassword));
    if (pwEncrypt(password, encryptedPassword, L7_PASSWORD_SIZE-1, 
                  L7_CAPTIVE_PORTAL_PASSWORD_ENCRYPT_ALG) != L7_SUCCESS)
    {
      break; /* Couldn't encrypt password  */
    }

    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (userMgrNoCaseCompare(pUser->password, encryptedPassword) != L7_TRUE)
      {
        memset(pUser->password, 0, L7_ENCRYPTED_PASSWORD_SIZE);
        osapiStrncpySafe(pUser->password, encryptedPassword, L7_ENCRYPTED_PASSWORD_SIZE);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Set a new user encryption password
*
* @param    uId_t uid                   (input) user id table index 
* @param    L7_char8 *EncryptedPassword (input) encrypted password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t cpdmUserEntryEncryptedPasswordSet(uId_t uId, L7_char8 *encryptedPassword)
{
  L7_char8 decryptedPassword[L7_PASSWORD_SIZE];
  cpdmUser_t *pUser = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 pwd_len;

  do
  {
    if (L7_NULLPTR == encryptedPassword)
    {
      break;
    }

    /* Is password valid? */
    pwd_len = strlen(encryptedPassword);
    if (0 == pwd_len)
    {
      break;
    }

    /* Attempt to decrypt in an effort to valid before we save it. */
    memset(decryptedPassword,0,L7_PASSWORD_SIZE);
    if (L7_SUCCESS!=pwDecrypt(decryptedPassword,encryptedPassword,L7_PASSWORD_SIZE-1))
    {
      break;
    }

    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (osapiStrncmp(pUser->password, encryptedPassword, L7_ENCRYPTED_PASSWORD_SIZE) != 0)
      {
        osapiStrncpySafe(pUser->password, encryptedPassword, L7_ENCRYPTED_PASSWORD_SIZE);
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  return rc;
}

/*********************************************************************
*
* @purpose  Set the session timeout. This is the total session time
*           allowed before being logged out
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 sessionTimeout @b{(input)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntrySessionTimeoutSet(uId_t uId, L7_uint32 sessionTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  do {
    cpdmUser_t  *pUser = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (pUser->sessionTimeout != sessionTimeout)
      {
        pUser->sessionTimeout = sessionTimeout;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusSessionTimeoutByUIdUpdate(uId, sessionTimeout);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Set the idle timeout. The session is logged out if the
*           session idle time is met.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 idleTimeout @b{(input)} idle in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryIdleTimeoutSet(uId_t uId, L7_uint32 idleTimeout)
{
  L7_RC_t rc = L7_FAILURE;

  do {
    cpdmUser_t  *pUser = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (pUser->idleTimeout != idleTimeout)
      {
        pUser->idleTimeout = idleTimeout;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusIdleTimeoutByUIdUpdate(uId, idleTimeout);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Set maximum client transmit rate. This limits the bandwidth
*           at which the client can send data into the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxBandwidthUp @b{(input)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxBandwidthUpSet(uId_t uId, L7_uint32 maxBandwidthUp)
{
  L7_RC_t rc = L7_FAILURE;

  do {
    cpdmUser_t  *pUser = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (pUser->maxBandwidthUp != maxBandwidthUp)
      {
        pUser->maxBandwidthUp = maxBandwidthUp;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  return rc;
}

/*********************************************************************
*
* @purpose  Set maximum client receive rate. This limits the bandwidth
*           at which the client can receive data from the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxBandwidthDown @b{(input)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxBandwidthDownSet(uId_t uId, L7_uint32 maxBandwidthDown)
{
  L7_RC_t rc = L7_FAILURE;

  do {
    cpdmUser_t  *pUser = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (pUser->maxBandwidthDown != maxBandwidthDown)
      {
        pUser->maxBandwidthDown = maxBandwidthDown;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);
  return rc;
}

/*********************************************************************
*
* @purpose  Set maximum number of octets the user is allowed to
*           transmit. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxInputOctets @b{(input)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxInputOctetsSet(uId_t uId, L7_uint32 maxInputOctets)
{
  L7_RC_t rc = L7_FAILURE;

  do {
    cpdmUser_t  *pUser = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (pUser->maxInputOctets != maxInputOctets)
      {
        pUser->maxInputOctets = maxInputOctets;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusMaxInputOctetsByUIdUpdate(uId, maxInputOctets);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Set maximum number of octets the user is allowed to
*           receive. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxOutputOctets @b{(input)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxOutputOctetsSet(uId_t uId, L7_uint32 maxOutputOctets)
{
  L7_RC_t rc = L7_FAILURE;

  do {
    cpdmUser_t  *pUser = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (pUser->maxOutputOctets != maxOutputOctets)
      {
        pUser->maxOutputOctets = maxOutputOctets;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusMaxOutputOctetsByUIdUpdate(uId, maxOutputOctets);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Set the maximum number of octets the user is allowed to
*           transmit and receieve. After this limit has been reached,
*           the user will be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 maxTotalOctets @b{(input)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxTotalOctetsSet(uId_t uId, L7_uint32 maxTotalOctets)
{
  L7_RC_t rc = L7_FAILURE;

  do {
    cpdmUser_t  *pUser = L7_NULLPTR;
    SEMA_TAKE(WRITE);
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR != pUser)
    {
      if (pUser->maxTotalOctets != maxTotalOctets)
      {
        pUser->maxTotalOctets = maxTotalOctets;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
      }
      rc = L7_SUCCESS;
    }
    SEMA_GIVE(WRITE);
  } while (0);

  if (L7_SUCCESS == rc)
  {
    rc = cpdmClientConnStatusMaxTotalOctetsByUIdUpdate(uId, maxTotalOctets);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the login name
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *loginName  @b{(output)} the login name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryLoginNameGet(uId_t uId, L7_char8 *loginName)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == loginName)
  {
    return L7_FAILURE;
  }
  
  SEMA_TAKE(READ);
  pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
  if (L7_NULLPTR != pUser)
  {
    if (strlen(pUser->loginName) > 0)
    {
      osapiStrncpySafe(loginName, pUser->loginName, CP_USER_LOCAL_USERNAME_MAX+1);
    }
    else
    {
      *loginName = '\0';
    }
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
*
* @purpose  Get the user password
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_char8 *password  @b{(output)} the password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryPasswordGet(uId_t uId, L7_char8 *password)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == password)
  {
    return L7_FAILURE;
  }
  
  SEMA_TAKE(READ);
  pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
  if (L7_NULLPTR != pUser)
  {
    if (strlen(pUser->password) > 0)
    {
      rc = pwDecrypt(password, pUser->password, L7_PASSWORD_SIZE-1);
    }
    else
    {
      *password = '\0';
    }
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* @purpose  Get the user password in encrypted format
*
* @param    uId_t    uId            @b{(input)} user id
* @param    L7_char8 *encryptedPwd  @b{(output)} encrypted password
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*       
* @end
*********************************************************************/
L7_RC_t cpdmUserEntryEncryptedPasswordGet(uId_t uId, L7_char8 *encryptedPwd)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == encryptedPwd)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(READ);
  pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
  if (L7_NULLPTR != pUser)
  {
    if (strlen(pUser->password) > 0)
    {
      strncpy(encryptedPwd, pUser->password, L7_ENCRYPTED_PASSWORD_SIZE-1);
      encryptedPwd[L7_ENCRYPTED_PASSWORD_SIZE-1] = '\0';
    }
    else
    {
      *encryptedPwd = '\0';
    }
    rc = L7_SUCCESS;
  }
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
*
* @purpose  Get the session timeout. This is the total session time
*           allowed before being logged out
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *sessionTimeout @b{(output)} timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntrySessionTimeoutGet(uId_t uId, L7_uint32 *pSessionTimeout)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == pSessionTimeout)
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR == pUser)
    {
      break; /* Entry did not exist? */
    }
    *pSessionTimeout = pUser->sessionTimeout; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
*
* @purpose  Get the idle timeout. The session is logged out if the
*           session idle time is met.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *idleTimeout @b{(output)} idle in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryIdleTimeoutGet(uId_t uId, L7_uint32 *pIdleTimeout)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == pIdleTimeout)
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR == pUser)
    {
      break; /* Entry did not exist? */
    }
    *pIdleTimeout = pUser->idleTimeout; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
*
* @purpose  Get maximum client transmit rate. This limits the bandwidth
*           at which the client can send data into the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxBandwidthUp @b{(output)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxBandwidthUpGet(uId_t uId, L7_uint32 *pMaxBandwidthUp)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == pMaxBandwidthUp)
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR == pUser)
    {
      break; /* Entry did not exist? */
    }
    *pMaxBandwidthUp = pUser->maxBandwidthUp; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}


/*********************************************************************
*
* @purpose  Get maximum client receive rate. This limits the bandwidth
*           at which the client can receive data from the network.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxBandwidthDown @b{(output)} bits per second
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxBandwidthDownGet(uId_t uId, L7_uint32 *pMaxBandwidthDown)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == pMaxBandwidthDown)
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR == pUser)
    {
      break; /* Entry did not exist? */
    }
    *pMaxBandwidthDown = pUser->maxBandwidthDown; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}


/*********************************************************************
*
* @purpose  Get maximum number of octets the user is allowed to
*           transmit. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxInputOctets @b{(output)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxInputOctetsGet(uId_t uId, L7_uint32 *pMaxInputOctets)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == pMaxInputOctets)
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR == pUser)
    {
      break; /* Entry did not exist? */
    }
    *pMaxInputOctets = pUser->maxInputOctets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}


/*********************************************************************
*
* @purpose  Get maximum number of octets the user is allowed to
*           receive. After this limit has been reached, the user will
*           be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxOutputOctets @b{(output)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxOutputOctetsGet(uId_t uId, L7_uint32 *pMaxOutputOctets)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == pMaxOutputOctets)
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR == pUser)
    {
      break; /* Entry did not exist? */
    }
    *pMaxOutputOctets = pUser->maxOutputOctets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}


/*********************************************************************
*
* @purpose  Get the maximum number of octets the user is allowed to
*           transmit and receive. After this limit has been reached,
*           the user will be disconnected.
*
* @param    uId_t uId @b{(input)} user id
* @param    L7_uint32 *maxTotalOctets @b{(output)} bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserEntryMaxTotalOctetsGet(uId_t uId, L7_uint32 *pMaxTotalOctets)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUser_t *pUser;

  if (L7_NULLPTR == pMaxTotalOctets)
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    pUser = cpdmUserEntryFindUser(uId, L7_FALSE);
    if (L7_NULLPTR == pUser)
    {
      break; /* Entry did not exist? */
    }
    *pMaxTotalOctets = pUser->maxTotalOctets; 
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
* USER GROUP ASSOCIATION RELATED APIs
*********************************************************************/

/*********************************************************************
*
* @purpose  Initialize ouser group association tree
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t cpdmUserGroupAssocEntryInit(void)
{
  if (L7_NULLPTR == cpdmCfgData)
  {
    return L7_FAILURE;
  }
  memset(&(cpdmCfgData->cpUserGroupAssocData[0]), 0, FD_CP_USER_GROUP_ASSOC_MAX);
  userGroupAssocEntryCount = 0;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Finalize user group association tree
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmUserGroupAssocEntryFinalize(void)
{
  if (L7_NULLPTR != cpdmCfgData)
  {
    memset(&(cpdmCfgData->cpUserGroupAssocData[0]), 0, FD_CP_USER_GROUP_ASSOC_MAX);
    userGroupAssocEntryCount = 0;
  }
}

/*********************************************************************
*
* @purpose  Update user group association count 
*
* @param    void
*
* @returns  void
*
* @comments updates count of active user group associations after
*           save/restore
*
* @end
*
*********************************************************************/
void cpdmUserGroupAssocEntryCountUpdate(void)
{
  L7_uint32 count = 0; 
  
  SEMA_TAKE(WRITE);
  if (L7_NULLPTR != cpdmCfgData)
  {
    for(count = 0; count < FD_CP_USER_GROUP_ASSOC_MAX; count++)
    {
        if (0 == cpdmCfgData->cpUserGroupAssocData[count].uId)
        {
          break;
        }
    }
  }
  userGroupAssocEntryCount = count;
  SEMA_GIVE(WRITE);
}

/*********************************************************************
*
* @purpose  Find the first tree entry for the given User and Group id.
*
* @param    uId_t    uId   @b{(input)} the key
* @param    gpId_t  gpId   @b{(input)} the key
* @param    find_assoc_t how @b{(input)} search type
*
* @returns  entry index
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
L7_int32 cpdmFindUserGroupAssocIndex(uId_t uId, 
                                     gpId_t gpId,
                                     find_assoc_t how)
{
  L7_uint32 idx = -1;
  cpdmUserGroupAssocData_t * pBase;
  L7_BOOL exact_match = L7_FALSE;

  if (L7_NULLPTR == cpdmCfgData)
    return idx;

  pBase = &(cpdmCfgData->cpUserGroupAssocData[0]);
  for(idx = 0; idx < userGroupAssocEntryCount; idx++)
  {
    if (pBase[idx].uId < uId)
    {
      continue;                      /* Keep looking */
    }

    if ((pBase[idx].uId == uId) &&
        (pBase[idx].gpId < gpId))
    {
      continue;                      /* Keep looking */                      
    }

    exact_match = ((pBase[idx].uId == uId) &&
                   (pBase[idx].gpId == gpId));
       
    if (FIND_ASSOC_EXACT == how)
    {
      if (exact_match)
        return idx;
      else
        return -2;
    }
    else if (exact_match)
      continue;

    break; /* Okay, we found something interesting */
  }

  if (idx == userGroupAssocEntryCount) /* If we've overshot our entries */
  {
    if (FIND_ASSOC_ADD == how)
    {
      if (userGroupAssocEntryCount == FD_CP_USER_GROUP_ASSOC_MAX)
        return -3;
    }
    else
      return -2;
  }
  return idx;
}


/*********************************************************************
*
* @purpose  Find a tree entry with the given User and Group id as key
*
* @param    uId_t  uId   @b{(input)} the key
* @param    gpId_t gpId  @b{(input)} the key
* @param    find_assoc_t how @b{(input)} search type
*
* @returns  pointer to cpdmUserGroupAssocData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
cpdmUserGroupAssocData_t * cpdmFindUserGroupAssoc(uId_t uId, gpId_t gpId)
{
  L7_int32 idx = cpdmFindUserGroupAssocIndex(uId, gpId, FIND_ASSOC_EXACT);
  if (0 > idx)
    return L7_NULLPTR;
  return &(cpdmCfgData->cpUserGroupAssocData[idx]);
}

/*********************************************************************
*
* @purpose  Add a user-group association to the assoc table.
*
* @param    uId_t uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupAssocEntryAdd(uId_t uId, gpId_t gpId)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 idx;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return L7_FAILURE;
  }

  if ((uId < 1) || 
      (uId > FD_CP_LOCAL_USERS_MAX))
  {
    return L7_FAILURE;
  }

  if ((gpId < GP_ID_MIN) || 
      (gpId > GP_ID_MAX))
  {
    return L7_FAILURE;
  }

  if (L7_SUCCESS!=cpdmUserEntryGet(uId))
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
            "%s: Can't associate, user id %d doesn't exist.",__FUNCTION__,uId);
    return L7_FAILURE;
  }

  if (L7_SUCCESS!=cpdmUserGroupEntryGet(gpId))
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
            "%s: Can't associate, group id %d doesn't exist.",__FUNCTION__,gpId);
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);   
  do 
  {
    cpdmUserGroupAssocData_t *pBase = 
      &(cpdmCfgData->cpUserGroupAssocData[0]);

    if (FD_CP_USER_GROUP_ASSOC_MAX == userGroupAssocEntryCount)
      break; /* if we can */

    idx = cpdmFindUserGroupAssocIndex(uId, gpId, FIND_ASSOC_ADD);

    if ((idx > 0) || ((0==idx) && (0<userGroupAssocEntryCount)))
    {
      /* push up any later entries, else we're appending an entry */
      memmove(&(pBase[idx + 1]), &(pBase[idx]), 
        (userGroupAssocEntryCount - idx) * sizeof(cpdmUserGroupAssocData_t));
    }

    pBase[idx].uId  = uId;
    pBase[idx].gpId = gpId;
    userGroupAssocEntryCount++;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(WRITE); 

  return rc;
}

/*********************************************************************
*
* @purpose  Delete a user-group association from the assoc table.
*
* @param    uId_t uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupAssocEntryDelete(uId_t uId, gpId_t gpId)
{ 
  cpdmUserGroupAssocData_t *pBase = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;
  L7_int32 idx;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  pBase = &(cpdmCfgData->cpUserGroupAssocData[0]);
  do 
  {
    idx = cpdmFindUserGroupAssocIndex(uId, gpId, FIND_ASSOC_EXACT);
    if (0 > idx)
    {
      break; /* Doesn't exist, leave now */
    }
    userGroupAssocEntryCount--;
    memmove(&(pBase[idx]), &(pBase[idx + 1]), 
      (userGroupAssocEntryCount - idx) * sizeof(cpdmUserGroupAssocData_t));
    memset(&(pBase[userGroupAssocEntryCount]), 0, sizeof(cpdmUserGroupAssocData_t));
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
*
* @purpose  Delete all user group associations from the assoc
*           table. This function basically loops through the table and
*           deletes all the entries for the given user id
*
* @param    uId_t uId @b{(input)} User Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupAssocEntryByUIdDeleteAll(uId_t uId)
{
  L7_int32 rc = L7_FAILURE;
  gpId_t gpId, nextGpId;
  uId_t nextUId;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return rc;
  }

  gpId = 0;
  nextGpId = 0;
  nextUId = 0;
  while (L7_SUCCESS==cpdmUserGroupAssocEntryNextGet(uId,gpId,&nextUId,&nextGpId))
  {
    if (uId == nextUId)
    {
      rc = cpdmUserGroupAssocEntryDelete(uId,nextGpId);
      gpId = 0;
      continue;
    }
    break;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Delete all user group associations from the assoc
*           table. This function basically loops through the table and
*           deletes all the entries for the given group id
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupAssocEntryByGpIdDeleteAll(gpId_t gpId)
{
  L7_int32 rc = L7_FAILURE;
  uId_t uId;
  gpId_t nextGpId;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return rc;
  }

  uId = 0;
  nextGpId = 0;
  while (L7_SUCCESS==cpdmUserGroupAssocEntryNextGet(uId,nextGpId,&uId,&nextGpId))
  {
    if (gpId == nextGpId)
    {
      rc = cpdmUserGroupAssocEntryDelete(uId,gpId);
      uId = 0;
      continue;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Check if user and group association exists.
*
* @param    uId_t uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupAssocEntryGet(uId_t uId, gpId_t gpId)
{ 
  cpdmUserGroupAssocData_t *pAssoc = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(READ);
  pAssoc = cpdmFindUserGroupAssoc(uId,gpId);
  SEMA_GIVE(READ);

  if (L7_NULLPTR != pAssoc)
  {
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Check if user and group association exists.
*
* @param    uId_t  uId @b{(input)} User Id
* @param    gpId_t gpId @b{(input)} Group Id
* @param    uId_t  *nextUId @b{(output)} Next user Id iteration
* @param    gpId_t *mextGpId @b{(output)} Next group Id iteration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupAssocEntryNextGet(uId_t uId, gpId_t gpId,
                                       uId_t *nextUId, gpId_t *nextGpId)
{
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == cpdmCfgData) ||
      (L7_NULLPTR == nextUId) ||
      (L7_NULLPTR == nextGpId))
  {
    return rc;
  }

  SEMA_TAKE(READ);   
  do
  {
    cpdmUserGroupAssocData_t *pEntry = L7_NULLPTR;
    L7_int32 idx = cpdmFindUserGroupAssocIndex(uId, gpId, FIND_ASSOC_NEXT);
    if (idx < 0) 
    {
      break;
    }
    pEntry  = &(cpdmCfgData->cpUserGroupAssocData[idx]);
    *nextUId  = pEntry->uId;
    *nextGpId = pEntry->gpId;
    rc = L7_SUCCESS;
  } while (0);
  SEMA_GIVE(READ);   

  return rc;
}

/*********************************************************************
*
* @purpose  Rename all matching group ids.
*
* @param    gpId_t oldGpId @b{(input)} existing group Id
* @param    gpId_t newGpId @b{(input)} replacement group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The old gpid is replaced with the new only if we don't
*           already have an association. But don't forget to
*           delete the old association if we got a match.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupAssocEntryGroupRename(gpId_t oldGpId, gpId_t newGpId)
{
  L7_uint32 idx = 0; 
  L7_int32  rc  = L7_FAILURE;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return rc;
  }

  if ((oldGpId < GP_ID_MIN) || 
      (oldGpId > GP_ID_MAX))
  {
    return rc;
  }

  if ((newGpId < GP_ID_MIN) || 
      (newGpId > GP_ID_MAX))
  {
    return rc;
  }

  if (oldGpId==newGpId)
  {
    return rc;
  }

  SEMA_TAKE(WRITE);
  while(idx < userGroupAssocEntryCount)
  {
    if (oldGpId == cpdmCfgData->cpUserGroupAssocData[idx].gpId)
    {
      /* Replace if entry does not already exist */
      if (L7_NULLPTR == cpdmFindUserGroupAssoc(cpdmCfgData->cpUserGroupAssocData[idx].uId,
                                               newGpId))
      {
        cpdmCfgData->cpUserGroupAssocData[idx].gpId = newGpId;
        cpdmCfgData->hdr.dataChanged = L7_TRUE;
        rc = L7_SUCCESS;
      }
    }
    idx++;
  }
  SEMA_GIVE(WRITE);

  /* Delete any user associations from the old group */
  if (cpdmUserGroupAssocEntryByGpIdDeleteAll(oldGpId) == L7_SUCCESS)
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* USER GROUP RELATED APIs
*********************************************************************/


/*********************************************************************
*
* @purpose  Initialize group tree
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryInit(void)
{
  memset(&(cpdmCfgData->cpUserGroupData[0]), 0, 
         FD_CP_USER_GROUP_MAX * sizeof(cpdmUserGroupData_t));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Finalize our group tree
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpdmUserGroupEntryFinalize(void)
{
  memset(cpdmCfgData->cpUserGroupData, 0, FD_CP_USER_GROUP_MAX * sizeof(cpdmUserGroupData_t));
  groupCount = 0;
  return;
}

/*********************************************************************
*
* @purpose  Create and initialize a default User Group
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryAddDefaults(void)
{
  L7_RC_t rc = L7_FAILURE;
  gpId_t gpId = 0;

  if (L7_SUCCESS != cpdmUserGroupEntryNextGet(gpId,&gpId))
  {
    rc = cpdmUserGroupEntryAdd(GP_ID_MIN);
    if (L7_SUCCESS==rc)
    {
      rc = cpdmUserGroupEntryNameSet(GP_ID_MIN,GP_DEFAULT_NAME);
    }
    groupCount = (L7_SUCCESS==rc)?1:0;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Find a tree entry with the given GP id as a key
*
* @param    gpId_t  gpId   @b{(input)} the key, of course
*
* @returns  void  pointer to cpdmUserGroupData_t (the value)
*
* @comments none  strictly for use within its own module!!!
*
* @end
*
*********************************************************************/
L7_int32 cpdmUserGroupEntryFindIndex(gpId_t gpId,
             L7_BOOL getNext)
{
  L7_uint32 idx = 0; 
  L7_int32  rc  = -1;

  if (L7_NULLPTR == cpdmCfgData)
  {
    return rc;
  }

  while(idx < groupCount)
  {
    if (0 == cpdmCfgData->cpUserGroupData[idx].gpId)
    {
      break; /* out of values */
    }

    if (L7_TRUE == getNext)
    {
      if (gpId < cpdmCfgData->cpUserGroupData[idx].gpId)
      {
        rc = (L7_int32) idx;
        break;
      }
    }
    else if (gpId == cpdmCfgData->cpUserGroupData[idx].gpId)
    {
      rc = (L7_int32) idx;
      break;
    }

    idx++;
  }
  return rc;
}

cpdmUserGroupData_t * cpdmUserGroupEntryFind(gpId_t gpId, L7_BOOL getNext)
{
  cpdmUserGroupData_t * pC = L7_NULLPTR;
  L7_int32 idx = cpdmUserGroupEntryFindIndex(gpId, getNext);
  
  if (0 <= idx)
  {
    pC = &(cpdmCfgData->cpUserGroupData[idx]);
  }
  return pC;
}

/*********************************************************************
*
* @purpose  Update user group count 
*
* @param    void
*
* @returns  void
*
* @comments updates count of active user groups after save/restore
*
* @end
*
*********************************************************************/
void cpdmUserGroupEntryCountUpdate(void)
{
  L7_uint32 count = 0; 
  
  SEMA_TAKE(WRITE);
  if (L7_NULLPTR != cpdmCfgData)
  {
    for(count = 0; count < GP_ID_MAX; count++)
    {
        if (0 == cpdmCfgData->cpUserGroupData[count].gpId)
        {
          break;
        }
    }
  }
  groupCount = count;
  SEMA_GIVE(WRITE);
}

/*********************************************************************
*
* @purpose  Add an entry to the Group table
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryAdd(gpId_t gpId)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmUserGroupData_t *pData = L7_NULLPTR;
  cpdmUserGroupData_t *pBase = L7_NULLPTR;
  L7_uint32 idx;

  if ((L7_NULLPTR == cpdmOprData) ||
      (L7_NULLPTR == cpdmCfgData))
  {
    return L7_FAILURE;
  }

  if ((gpId < GP_ID_MIN) || 
      (gpId > GP_ID_MAX))
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  do 
  {
    if (FD_CP_USER_GROUP_MAX == groupCount)
    {
      LOG_MSG("%s: Already reached maximum group capacity.",
              __FUNCTION__);
      break;
    }

    pBase = &(cpdmCfgData->cpUserGroupData[0]);
    for(idx = 0; idx < groupCount; idx++) 
    {
      pData = &(pBase[idx]);
      if (0 == pData->gpId)
      {
        break; /* at end of entries, found our index */
      }
      else if (gpId == pData->gpId)
      {
        rc = L7_ALREADY_CONFIGURED;
        break;
      }
      else if (gpId < pData->gpId)
      {
        break;
      }
    }

    if (L7_ALREADY_CONFIGURED == rc)
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
              "%s: Group %d already exists.",
              __FUNCTION__,gpId);
      break;
    }

    if (groupCount != idx)
    {
      /* push up later entries */
      memmove(&(pBase[idx + 1]), &(pBase[idx]), 
              (groupCount - idx) * sizeof(cpdmUserGroupData_t));
    }
    else
    {
      pData = &(pBase[idx]);
    }

    memset(pData, 0, sizeof(cpdmUserGroupData_t));
    pData->gpId = gpId;
    groupCount++;
    cpdmCfgData->hdr.dataChanged = L7_TRUE;
    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
*
* @purpose  Delete an entry from the Group table
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Although we'll also remove the associations from the
*           user group association table, we don't de-authenticate
*           existing users.
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryDelete(gpId_t gpId)  
{ 
  cpdmUserGroupData_t   *pBase = L7_NULLPTR;
  L7_RC_t               rc = L7_FAILURE;
  L7_int32              idx;

  do 
  {
    if ((L7_NULLPTR == cpdmOprData) ||
        (L7_NULLPTR == cpdmCfgData))
    {
      break;
    }

    if (GP_ID_MIN == gpId) 
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
              "%s: Refused to delete default group.",
              __FUNCTION__);
      break;
    }

    /* Delete any user associations for this group */
    cpdmUserGroupAssocEntryByGpIdDeleteAll(gpId);

    rc = L7_SUCCESS; /* so far so good anyway */
  } while(0);

  if (L7_SUCCESS != rc)
  {
    return rc;
  }

  rc = L7_FAILURE; /* Now try the rest */
  SEMA_TAKE(WRITE);
  pBase = &(cpdmCfgData->cpUserGroupData[0]);
  do 
  {
    idx = cpdmUserGroupEntryFindIndex(gpId, L7_FALSE);

    if (0 > idx)
    {
      break; /* Doesn't exist, leave now */
    }

    groupCount--;
    memmove(&(pBase[idx]), &(pBase[idx + 1]), 
      (groupCount - idx) * sizeof(cpdmUserGroupData_t));
    memset(&(pBase[groupCount]), 0, sizeof(cpdmUserGroupData_t));
    cpdmCfgData->hdr.dataChanged = L7_TRUE;

    rc = L7_SUCCESS;
  } while(0);
  SEMA_GIVE(WRITE);

  return rc;
}

/*********************************************************************
*
* @purpose  Check if group exists
*
* @param    gpId_t gpId @b{(input)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryGet(gpId_t gpId)  
{ 
  cpdmUserGroupData_t  *pGroup = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  SEMA_TAKE(READ);
  pGroup = cpdmUserGroupEntryFind(gpId, L7_FALSE);
  SEMA_GIVE(READ);

  if (L7_NULLPTR != pGroup)
  {
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Get the associated group id for given name
*
* @param    L7_char8 *groupName @b{(input)} Group name
* @param    gpId_t *gpId @b{(output)} Group Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryByNameGet(L7_char8 *groupName, gpId_t *gpId)  
{ 
  L7_char8 tmpGroupName[CP_USER_LOCAL_USERGROUP_MAX + 1];
  L7_BOOL fFound = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  gpId_t tmpGpId = 0;

  if (L7_NULLPTR == gpId)
  {
    return rc;
  }

  while ((L7_SUCCESS==cpdmUserGroupEntryNextGet(tmpGpId,&tmpGpId)) && (L7_FALSE==fFound))
  {
    cpdmUserGroupEntryNameGet(tmpGpId,tmpGroupName);
    if (osapiStrncmp(groupName,tmpGroupName,CP_USER_LOCAL_USERGROUP_MAX) == 0)
    {
      fFound = L7_TRUE;
    }
  }

  if (L7_TRUE==fFound)
  {
    rc = L7_SUCCESS;
    *gpId = tmpGpId;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Get the next group entry
*
* @param    gpId_t gpId @b{(input)} current GP ID 
* @param    gpId_t * nextGpId @b{(output)} next GP ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryNextGet(gpId_t gpId, gpId_t *nextGpId) 
{ 
  L7_RC_t rc = L7_FAILURE;

  if ((L7_NULLPTR == nextGpId) || 
      (L7_NULLPTR == cpdmCfgData))
  {
    return rc;
  }
  
  SEMA_TAKE(READ);
  do 
  {
    L7_int32 idx;
    cpdmUserGroupData_t *pBase = &(cpdmCfgData->cpUserGroupData[0]);
    cpdmUserGroupData_t *pGroup = L7_NULLPTR;

    if (0 == groupCount)
    {
      break;
    }
    if (0 == gpId)
    {
      idx = 0;
      pGroup = pBase;
    }
    else
    {
      idx = cpdmUserGroupEntryFindIndex(gpId, L7_TRUE);

      if (0 > idx) /* no such group? */
      {
        break;
      }
      pGroup = &(pBase[idx]);
      if (0 == pGroup->gpId)
      {
        break;
      }
    }
    *nextGpId = pGroup->gpId;
    rc = L7_SUCCESS;
  } while(0);

  SEMA_GIVE(READ);
  return rc;
}

/*********************************************************************
*
* @purpose  Get group name using the GP ID
*
* @param    gpId_t gpId @b{(input)} current GP ID 
* @param    L7_char8 *groupName @b{(output)} group name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryNameGet(gpId_t gpId, L7_char8 *groupName)
{ 
  L7_RC_t rc = L7_FAILURE; 

  if (L7_NULLPTR == groupName) 
  { 
     return rc;                 
  } 
  SEMA_TAKE(READ);
  do 
  {  
    cpdmUserGroupData_t *pGroup = cpdmUserGroupEntryFind(gpId, L7_FALSE); 
    if (L7_NULLPTR == pGroup)
    {
      break; 
    }
    osapiStrncpySafe(groupName, pGroup->groupName, CP_USER_LOCAL_USERGROUP_MAX+1);
    rc = L7_SUCCESS;
  } while(0); 

  SEMA_GIVE(READ); 
  return rc; 
}

/*********************************************************************
*
* @purpose  Set the group name for the given GP ID
*
* @param    gpId_t gpId @b{(input)} current GP ID 
* @param    gL7_char8 *groupName @b{(input)} group name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmUserGroupEntryNameSet(gpId_t gpId, L7_char8 *groupName)
{ 
  L7_RC_t rc = L7_FAILURE; 
  L7_uint32 len; 

  do 
  {  
    if (L7_NULLPTR == groupName) 
    {
      break;
    }
    len = strlen(groupName);
    if ((len<CP_USER_LOCAL_USERGROUP_MIN) ||
        (len>CP_USER_LOCAL_USERGROUP_MAX))
    {
      break;
    }

    {
      cpdmUserGroupData_t *pGroup = L7_NULLPTR;
      SEMA_TAKE(WRITE);
      pGroup = cpdmUserGroupEntryFind(gpId, L7_FALSE); 
      if (L7_NULLPTR != pGroup)
      {
        if (osapiStrncmp(pGroup->groupName, groupName, CP_USER_LOCAL_USERGROUP_MAX) != 0)
        {
          osapiStrncpySafe(pGroup->groupName, groupName, CP_USER_LOCAL_USERGROUP_MAX+1);
          cpdmCfgData->hdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;
      }
      SEMA_GIVE(WRITE); 
    }
  } while(0); 

  return rc; 
}
