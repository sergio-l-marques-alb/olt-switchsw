/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     sshd_api.c
*
* @purpose      SSHD API functions
*
* @component    sshd
*
* @comments     none
*
* @create       09/15/2003
*
* @author       dcbii
*
* @end
*
**********************************************************************/

#include "sshd_include.h"
#include "sshd_api.h"
#include "sshd_util.h"
#include "key.h"
#include "sshd_exports.h"
#include "transfer_exports.h"

extern sshdCfg_t    *sshdCfg;
extern sshdGlobal_t sshdGlobal;
extern unsigned     ConnectionsActiveSSH;
extern void         *sshdGlobalSema;

/* use Key type from FreSSH vendor code */
extern void key_free(Key *k);

/*********************************************************************
*
* @purpose  Set the Admin Mode of the SSL Tunnel server.
*
* @param    mode @b{(input)} value of new Admin mode setting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t sshdAdminModeSet(L7_uint32 mode)
{
  L7_char8 keyFile[L7_MAX_FILENAME];

  if ((mode != L7_ENABLE) &&
      (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (mode != sshdCfg->cfg.sshdAdminMode)
  {
#ifndef FEAT_METRO_CPE_V1_0
    if ((mode == L7_ENABLE) &&
        ((sshdKeyFileGet(SSHD_KEY_TYPE_RSA,
                         keyFile) != L7_SUCCESS) ||
         (sshdKeyFileGet(SSHD_KEY_TYPE_DSA,
                         keyFile) != L7_SUCCESS)))
#else
    if ((mode == L7_ENABLE) &&
        ((sshdKeyFileGet(SSHD_KEY_TYPE_RSA,
                         keyFile) != L7_SUCCESS)&& 
         (sshdKeyFileGet(SSHD_KEY_TYPE_DSA,
                         keyFile) != L7_SUCCESS)))
#endif
    {
      LOG_COMPONENT_MSG(L7_FLEX_SSHD_COMPONENT_ID, "Failed to find SSHD key files");
      osapiSemaGive(sshdGlobalSema);
      return L7_FAILURE;
    }
    if (sshdIssueCmd(SSHD_ADMIN_MODE_SET, &mode) != L7_SUCCESS)
    {
      LOG_COMPONENT_MSG(L7_FLEX_SSHD_COMPONENT_ID, "Failed to set admin mode %d", mode);
      osapiSemaGive(sshdGlobalSema);
      return L7_FAILURE;
    }

    sshdCfg->cfg.sshdAdminMode = mode;
    sshdCfg->hdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the Admin Mode of the SSHD server.
*
* @param    mode @b{(input)} location to store the Admin mode setting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
sshdAdminModeGet(L7_uint32 *mode)
{
    *mode = sshdCfg->cfg.sshdAdminMode;

    return L7_SUCCESS;

} /* sshdAdminModeGet */


/*********************************************************************
*
* @purpose  Find out how many SSH sessions are active
*
* @param    NumSessions @b{(input)} location to set with result
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
sshdNumSessionsGet(L7_uint32 *NumSessions)
{
    *NumSessions = (L7_uint32) ConnectionsActiveSSH;

    return L7_SUCCESS;

} /* sshdNumSessionsGet */


/*********************************************************************
*
* @purpose  get the number of ssh sessions allowed
*
* @param    none
*
* @returns  L7_unit32
*
* @comments none
*
* @end
*
*********************************************************************/

L7_uint32
cliSshdMaxNumSessionsGet()
{
   return sshdGlobal.sshdMaxSessions;

} /* cliSshdMaxNumSessionsGet */


/*********************************************************************
*
* @purpose  Set the protocol level of the SSH server to 1, 2, or both.
*
* @param    protoLevel @b{(input)} value of new protocol level
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
sshdProtoLevelSet(L7_uint32 protoLevel)
{
    L7_RC_t rc;

    if ((protoLevel >= SSHD_PROTO_LEVEL_V1_ONLY) &&
        (protoLevel <= SSHD_PROTO_LEVEL_ALL))
    {
        if (protoLevel != sshdCfg->cfg.sshdProtoLevel)
        {
            /* Issue the command */

            if ((rc = sshdIssueCmd(SSHD_PROTO_LEVEL_SET, &protoLevel)) != L7_SUCCESS)
            {
                return L7_FAILURE;
            }

            sshdCfg->cfg.sshdProtoLevel = protoLevel;

            sshdCfg->hdr.dataChanged = L7_TRUE;
        }
    }
    else
    {
        /* Protocol level is not in the valid range */

        return L7_FAILURE;
    }

    return L7_SUCCESS;

} /* sshdProtoLevelSet */


/*********************************************************************
*
* @purpose  Get the protocol level of the SSH server.
*
* @param    mode @b{(input)} location to store the mode setting
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
sshdProtoLevelGet(L7_uint32 *mode)
{
    *mode = sshdCfg->cfg.sshdProtoLevel;

    return L7_SUCCESS;

} /* sshdProtoLevelGet */


/*********************************************************************
*
* @purpose  Set max SSH sessions allowed
*
* @param    NumSessions @b{(input)} location to set with result
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
cliSshdMaxNumSessionsSet(L7_uint32 *NumSessions)
{
    if (sshdCfg->cfg.sshdMaxSessions != *NumSessions)
    {
      sshdGlobal.sshdMaxSessions = *NumSessions;
      sshdCfg->cfg.sshdMaxSessions = *NumSessions;
      sshdCfg->hdr.dataChanged = L7_TRUE;
    }

    return L7_SUCCESS;

} /* cliSshdMaxNumSessionsSet */


/*********************************************************************
*
* @purpose  Set ssh idle timeout value
*
* @param    timeOut @b{(input)} value of new timeout in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
cliSshdIdleTimeoutSet(L7_uint32 *timeOut)
{
    if (sshdCfg->cfg.sshdIdleTimeout != *timeOut)
    {
      sshdGlobal.sshdIdleTimeout = *timeOut;
      sshdCfg->cfg.sshdIdleTimeout = *timeOut;
      sshdCfg->hdr.dataChanged = L7_TRUE;
    }

    return L7_SUCCESS;

} /* cliSshdIdleTimeoutSet */


/*********************************************************************
*
* @purpose  Get the ssh idle timeout value
*
* @param    NumSessions @b{(input)} location to set with result
*
* @returns  L7_uint32
*
* @comments none
*
* @end
*
*********************************************************************/

L7_uint32
cliSshdIdleTimeoutGet()
{
   return sshdGlobal.sshdIdleTimeout;

} /* cliSshdIdleTimeoutGet */



/*********************************************************************
*
* @purpose Set the Port Number to be used for SSH Server.
*
* @param L7_uint32 portNum @b{(input)} port number set by user
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdPortNumSet(L7_uint32 portNum)
{
  if ((portNum < L7_SSHD_PORT_NUMBER_MIN) ||
      (portNum > L7_SSHD_PORT_NUMBER_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (portNum != sshdCfg->cfg.sshdPort)
  {
    /* Issue the command */

    if (sshdIssueCmd(SSHD_PORT_NUMBER_SET, &portNum) != L7_SUCCESS)
    {
      osapiSemaGive(sshdGlobalSema);
      return L7_FAILURE;
    }

    sshdCfg->cfg.sshdPort = portNum;
    sshdCfg->hdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Port Number for the ssh server
*
* @param L7_uint32 *portNum @b{(output)} configured port number
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdPortNumGet(L7_uint32 *portNum)
{
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  *portNum = sshdCfg->cfg.sshdPort;
  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set Public-Key Authentication Mode of the SSHD server.
*
* @param L7_uint32 mode @b{(input)} enable or disable
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t sshdPubKeyAuthModeSet(L7_uint32 mode)
{
  if ((mode != L7_ENABLE) &&
      (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdCfg->cfg.sshdPubKeyAuthMode == mode)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_SUCCESS;
  }
  sshdCfg->cfg.sshdPubKeyAuthMode = mode;
  sshdCfg->hdr.dataChanged = L7_TRUE;
  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get Public-Key Authentication Mode of the SSHD server.
*
* @param L7_uint32 mode @b{(output)} enable or disable
*
* @returns L7_SUCCESS
* @retruns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdPubKeyAuthModeGet(L7_uint32 *mode)
{
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  *mode = sshdCfg->cfg.sshdPubKeyAuthMode;
  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose generate ssh server public/private key pair
*
* @param sshdKeyType_t type @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS if the Key is generated
* @retruns L7_FAILURE if the key is not generated
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyGenerate(sshdKeyType_t type)
{
  L7_uchar8 flag = (L7_uchar8)(1 << type);

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }
#ifndef FEAT_METRO_CPE_V1_0
  if (sshdCfg->cfg.sshdAdminMode == L7_ENABLE)
  {
    return L7_FAILURE;
  }
#endif
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);

  if ((sshdGlobal.sshdKeyGenerateFlags & flag) != 0)
  {
    /* key generation in progress for specified key type */
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }

  /* set flag to indicate key generation is in progress for
     the specified key type, generate key asynchronously */
  sshdGlobal.sshdKeyGenerateFlags |= flag;
  osapiSemaGive(sshdGlobalSema);

  if (sshdIssueCmd(SSHD_KEY_GENERATE_EVENT, &type) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* there is no corresponding config data */
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To verify whether a key pair of specified type exists
*
* @param sshdKeyType_t type @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS if key type exists
* @returns L7_FAILURE invalid argument or key generation in progress
* @returns L7_NOT_EXIST if key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyExists(sshdKeyType_t type)
{
  L7_uchar8 flag = (L7_uchar8)(1 << type);
  L7_char8  keyFile[L7_MAX_FILENAME];

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);

  if ((sshdGlobal.sshdKeyGenerateFlags & flag) != 0)
  {
    /* key generation requested and in progress */
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }

  if (sshdKeyFileGet(type, keyFile) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_NOT_EXIST;
  }

  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To get key data for a specific key type.
*
* @param sshdKeyType_t type    @b{(input)}   RSA or DSA.
* @param L7_char8     *keyData @b((output))  hex format
*
* @returns L7_SUCCESS keyData returned
* @retruns L7_FAILURE key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyDataGet(sshdKeyType_t type,
                       L7_char8 *keyData)
{
  L7_RC_t   rc = L7_SUCCESS;
  Key      *pkey = L7_NULLPTR;
  L7_int32 fd = -1;
  FILE *f;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }

  if( (osapiFsFileCreate(FD_SSHD_PATH_SSH_USER_DIR "tmpSSHKey", &fd) != L7_SUCCESS)  || (fd ==0))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdPubKeyGet(type, &pkey) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }
  osapiSemaGive(sshdGlobalSema);

  f = fdopen(fd, "w");
  if (f==L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /* format the data */
  rc = key_write(pkey, f);
  key_free(pkey);
  fclose(f);
  osapiFsClose(fd);

  osapiFsRead(FD_SSHD_PATH_SSH_USER_DIR "tmpSSHKey",keyData, L7_SSHD_KEY_DATA_SIZE_MAX);

  osapiFsDeleteFile(FD_SSHD_PATH_SSH_USER_DIR "tmpSSHKey");
  return ((rc==1)?L7_SUCCESS:L7_FAILURE);
}

/*********************************************************************
*
* @purpose To get key fingerprint in hex format for a specific key type.
*
* @param sshdKeyType_t type        @b{(input)}   RSA or DSA.
* @param L7_char8     *fingerprint @b((output))  hex format
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyFingerprintHexGet(sshdKeyType_t type,
                                 L7_char8 *fingerprint)
{
  L7_RC_t   rc = L7_FAILURE;
  Key      *pkey = L7_NULLPTR;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdPubKeyGet(type, &pkey) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }
  osapiSemaGive(sshdGlobalSema);

  /* format the data */
  rc = sshdPubKeyMD5HexPrint(pkey, fingerprint);
  key_free(pkey);
  return rc;
}

/*********************************************************************
*
* @purpose To get key fingerprint in bubble babble format for a specific key type.
*
* @param sshdKeyType_t type        @b{(input)}   RSA or DSA.
* @param L7_char8     *fingerprint @b((output))  bubble babble format.
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE key type does not exist
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdKeyFingerprintBBGet(sshdKeyType_t type,
                                L7_char8 *fingerprint)
{
  L7_RC_t   rc = L7_FAILURE;
  Key      *pkey = L7_NULLPTR;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdPubKeyGet(type, &pkey) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }
  osapiSemaGive(sshdGlobalSema);

  /* format the data */
  rc = sshdPubKeySHA1BBPrint(pkey, fingerprint);
  key_free(pkey);
  return rc;
}

/*********************************************************************
*
* @purpose Add public key configuration for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS user added
* @retruns L7_FAILURE pubkey chain table full
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyAdd(L7_char8 *username,
                                  sshdKeyType_t type)
{
  L7_uint32 i = 0;
  L7_uint32 j = 0;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }
  if ((username == L7_NULLPTR) ||
      (strlen(username) < L7_SSHD_USERNAME_SIZE_MIN) ||
      (strlen(username) > L7_SSHD_USERNAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  /* if last entry is specified, table is full */
  if (sshdCfg->cfg.sshdPubKeys[L7_SSHD_PUBKEY_USERS_MAX-1].username[0] != 0)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }

  for (i = 0; i < L7_SSHD_PUBKEY_USERS_MAX; i++)
  {
    L7_int32 result = 0;
    if (sshdCfg->cfg.sshdPubKeys[i].username[0] == 0)
    {
      /* insert at end */
      break;
    }
    result = strcmp(username, sshdCfg->cfg.sshdPubKeys[i].username);
    if (result == 0)
    {
      if (sshdCfg->cfg.sshdPubKeys[i].type == type)
      {
        /* entry already exists, do nothing */
        osapiSemaGive(sshdGlobalSema);
        return L7_SUCCESS;
      } else
      {
        /* update the type and clear the key */
        sshdCfg->cfg.sshdPubKeys[i].type = type;
        memset(sshdCfg->cfg.sshdPubKeys[i].key, 0, L7_SSHD_KEY_DATA_SIZE_MAX);
        sshdCfg->hdr.dataChanged = L7_TRUE;
        osapiSemaGive(sshdGlobalSema);
        return L7_SUCCESS;
      }
    }
    if (result < 0)
    {
      break;
    }
  }

  /* need to move up all entries above index */
  for (j = (L7_SSHD_PUBKEY_USERS_MAX - 1); j > i; j--)
  {
    if (sshdCfg->cfg.sshdPubKeys[j-1].username[0] == 0)
    {
      continue;
    }
    memcpy(&sshdCfg->cfg.sshdPubKeys[j],
           &sshdCfg->cfg.sshdPubKeys[j-1],
           sizeof(sshdPubKeyEntry_t));
  }
  memset(&sshdCfg->cfg.sshdPubKeys[i], 0, sizeof(sshdPubKeyEntry_t));
  strcpy(sshdCfg->cfg.sshdPubKeys[i].username, username);
  sshdCfg->cfg.sshdPubKeys[i].type = type;
  sshdCfg->hdr.dataChanged = L7_TRUE;
  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Remove public key configuration for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
*
* @returns L7_SUCCESS user added
* @retruns L7_FAILURE pubkey chain table full
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyRemove(L7_char8 *username,
                                     sshdKeyType_t type)
{
  L7_uint32 index = 0;
  L7_uint32 j = 0;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }
  if ((username == L7_NULLPTR) ||
      (strlen(username) < L7_SSHD_USERNAME_SIZE_MIN) ||
      (strlen(username) > L7_SSHD_USERNAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdUserPubKeyIndexGet(username, type, &index) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }
  memset(&sshdCfg->cfg.sshdPubKeys[index], 0, sizeof(sshdPubKeyEntry_t));

  /* move entries after removed entry down one */
  for (j=index+1; j < L7_SSHD_PUBKEY_USERS_MAX; j++)
  {
    if (sshdCfg->cfg.sshdPubKeys[j].username[0] == 0)
    {
      break;
    }
    memcpy(&sshdCfg->cfg.sshdPubKeys[j-1], &sshdCfg->cfg.sshdPubKeys[j], sizeof(sshdPubKeyEntry_t));
    memset(&sshdCfg->cfg.sshdPubKeys[j], 0, sizeof(sshdPubKeyEntry_t));
  }
  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Configure public key for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
* @param L7_char8     *key      @b{(input)} key data SSHv2 format.
*
* @returns L7_SUCCESS key set
* @retruns L7_FAILURE entry not found, invalid key
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyStringSet(L7_char8 *username,
                                        sshdKeyType_t type,
                                        L7_char8 *key)
{
  L7_uint32 index = 0;
  L7_char8  keyData[L7_SSHD_KEY_DATA_SIZE_MAX];
  L7_uint32 len;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }
  if ((username == L7_NULLPTR) ||
      (strlen(username) < L7_SSHD_USERNAME_SIZE_MIN) ||
      (strlen(username) > L7_SSHD_USERNAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }

  /* fix up the input, may or may not include ssh-dss, ssh-rsa strings */
  if (sshdUserPubKeyStringFormat(key, type, keyData) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* verify key string contains valid key */
  if (sshdUserPubKeyStringVerify(keyData, type) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdUserPubKeyIndexGet(username, type, &index) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }

  memset(sshdCfg->cfg.sshdPubKeys[index].key, 0, L7_SSHD_KEY_DATA_SIZE_MAX);
  strcpy(sshdCfg->cfg.sshdPubKeys[index].key, keyData);
  len= strlen(sshdCfg->cfg.sshdPubKeys[index].key);
  if (len< L7_SSHD_KEY_DATA_SIZE_MAX )
  {
     sshdCfg->cfg.sshdPubKeys[index].key[len+1]='\0';
  }
  else
  {
     sshdCfg->cfg.sshdPubKeys[index].key[L7_SSHD_KEY_DATA_SIZE_MAX]='\0';
  }
  sshdCfg->hdr.dataChanged = L7_TRUE;
  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get next public key configured for remote user/device.
*
* @param L7_char8      *username @b{(input/output)} user string name (up to 48 characters).
* @param sshdKeyType_t *type     @b{(input/output)} RSA or DSA.
*
* @returns L7_SUCCESS next entry returned
* @retruns L7_FAILURE last entry
*
* @comments username of "" will return first public key chain table entry.
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyNextGet(L7_char8      *username,
                                      sshdKeyType_t *type)
{
  L7_uint32 index = 0;

  if ((type == L7_NULLPTR) ||
      (username == L7_NULLPTR) ||
      (strlen(username) > L7_SSHD_USERNAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);

  if (strlen(username) == 0)
  {
    /* return first entry */
    if (sshdCfg->cfg.sshdPubKeys[0].username[0] == 0)
    {
      osapiSemaGive(sshdGlobalSema);
      return L7_FAILURE;
    }
    strcpy(username, sshdCfg->cfg.sshdPubKeys[0].username);
    *type = sshdCfg->cfg.sshdPubKeys[0].type;
    osapiSemaGive(sshdGlobalSema);
    return L7_SUCCESS;
  }

  if (sshdUserPubKeyIndexGet(username, *type, &index) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }
  index++;
  if ((index < L7_SSHD_PUBKEY_USERS_MAX) &&
      (sshdCfg->cfg.sshdPubKeys[index].username[0] != 0))
  {
    strcpy(username, sshdCfg->cfg.sshdPubKeys[index].username);
    *type = sshdCfg->cfg.sshdPubKeys[index].type;
    osapiSemaGive(sshdGlobalSema);
    return L7_SUCCESS;
  }
  osapiSemaGive(sshdGlobalSema);
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get public key string for remote user/device.
*
* @param L7_char8     *username @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type     @b{(input)} RSA or DSA.
* @param L7_char8     *keyData  @b{(output)} key data.
*
* @returns L7_SUCCESS key data returned
* @retruns L7_FAILURE entry not found
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyDataGet(L7_char8 *username,
                                      sshdKeyType_t type,
                                      L7_char8 *keyData)
{
  L7_uint32 index = 0;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }
  if ((username == L7_NULLPTR) ||
      (strlen(username) < L7_SSHD_USERNAME_SIZE_MIN) ||
      (strlen(username) > L7_SSHD_USERNAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }
  if (keyData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdUserPubKeyIndexGet(username, type, &index) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    return L7_FAILURE;
  }
  strcpy(keyData, sshdCfg->cfg.sshdPubKeys[index].key);
  osapiSemaGive(sshdGlobalSema);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get public key string for remote user/device.
*
* @param L7_char8     *username    @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type        @b{(input)} RSA or DSA.
* @param L7_char8     *fingerprint @b{(output)} fingerprint in hex format.
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE entry not found
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyFingerprintHexGet(L7_char8 *username,
                                                sshdKeyType_t type,
                                                L7_char8 *fingerprint)
{
  Key *pkey = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }
  if ((username == L7_NULLPTR) ||
      (strlen(username) < L7_SSHD_USERNAME_SIZE_MIN) ||
      (strlen(username) > L7_SSHD_USERNAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }
  if (fingerprint == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  pkey = key_new(-1, KEY_UNSPEC);
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdUserPubKeyGet(username, type, pkey) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    key_free(pkey);
    return L7_FAILURE;
  }
  osapiSemaGive(sshdGlobalSema);

  rc = sshdPubKeyMD5HexPrint(pkey, fingerprint);
  key_free(pkey);
  return rc;
}

/*********************************************************************
*
* @purpose Get public key string for remote user/device.
*
* @param L7_char8     *username    @b{(input)} user string name (up to 48 characters).
* @param sshdKeyType_t type        @b{(input)} RSA or DSA.
* @param L7_char8     *fingerprint @b{(output)} fingerprint in bubble babble format.
*
* @returns L7_SUCCESS fingerprint returned
* @retruns L7_FAILURE entry not found
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t sshdPubKeyChainUserKeyFingerprintBBGet(L7_char8 *username,
                                               sshdKeyType_t type,
                                               L7_char8 *fingerprint)
{
  Key *pkey = L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if ((type != SSHD_KEY_TYPE_RSA) &&
      (type != SSHD_KEY_TYPE_DSA))
  {
    return L7_FAILURE;
  }
  if ((username == L7_NULLPTR) ||
      (strlen(username) < L7_SSHD_USERNAME_SIZE_MIN) ||
      (strlen(username) > L7_SSHD_USERNAME_SIZE_MAX))
  {
    return L7_FAILURE;
  }
  if (fingerprint == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  pkey = key_new(-1, KEY_UNSPEC);
  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdUserPubKeyGet(username, type, pkey) != L7_SUCCESS)
  {
    osapiSemaGive(sshdGlobalSema);
    key_free(pkey);
    return L7_FAILURE;
  }
  osapiSemaGive(sshdGlobalSema);

  rc = sshdPubKeySHA1BBPrint(pkey, fingerprint);
  key_free(pkey);
  return rc;
}

