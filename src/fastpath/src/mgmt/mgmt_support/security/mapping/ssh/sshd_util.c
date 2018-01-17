/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sshd_util.c
*
* @purpose SSL Tunnel Utility functions
*
* @component sshd
*
* @comments none
*
* @create 04/29/05
*
* @author dfowler
*
* @end
*
**********************************************************************/

#include <openssl/rsa.h>
#include <openssl/dsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include "sshd_include.h"
#include "key.h"
#include "sshd_exports.h"
#include "transfer_exports.h"

/* use function in OpenSSH vendor code to save SSH key type */
extern int key_save_private(Key *key, const char *filename,
                            const char *passphrase, const char *comment);

extern Key *
key_load_private(const char *filename, const char *passphrase,
    char **commentp);

extern sshdCfg_t    *sshdCfg;
extern void         *sshdGlobalSema;

/*********************************************************************
*
* @purpose Get SSHv2 file name for a key type.
*
* @param sshdKeytype_t type      @b{(input)}  key type
* @param L7_char8      *filename @b((output)) filename where key is stored
*
*
* @returns L7_SUCCESS, indicated filename exists
* @returns L7_FAILURE, bad key type, or indicated filename does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdKeyFileGet(sshdKeyType_t  type,
                       L7_char8      *filename)
{
  L7_uint32 size = 0;

  memset(filename, 0, L7_MAX_FILENAME);
  if (type == SSHD_KEY_TYPE_RSA)
  {
    strcpy(filename, L7_SSHD_PATH_SERVER_PRIVKEY_RSA);
  } else
  {
    strcpy(filename, L7_SSHD_PATH_SERVER_PRIVKEY_DSA);
  }

  if ((osapiFsFileSizeGet(filename, &size) != L7_SUCCESS) ||
      (size == 0))
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Read SSHv2 public/private key from PEM file specified by type
*
* @param sshdKeyType_t type   @b{(input)}  RSA or DSA
* @param EVP_PKEY    **pkey   @b((output)) internal key format
*
*
* @returns L7_SUCCESS, key returned
* @returns L7_FAILURE, indicated key file corrupt or does not exist
*
* @comments   key must be freed!
*
* @end
*
*********************************************************************/
L7_RC_t sshdPubKeyGet(sshdKeyType_t type,
                      Key         **key)
{
  L7_char8 keyFile[L7_MAX_FILENAME];
  Key       *tempKey = NULL;

  if (sshdKeyFileGet(type, keyFile) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  tempKey = key_load_private(keyFile, "", NULL);
  if (tempKey == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  (*key) = tempKey;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Read SSHv1 private key
*
* @param EVP_PKEY    **pkey   @b((output)) internal key format
*
*
* @returns L7_SUCCESS, key returned
* @returns L7_FAILURE, indicated key file corrupt or does not exist
*
* @comments   key must be freed!
*
* @end
*
*********************************************************************/
L7_RC_t sshdKeyRSA1Get(Key **key)
{
  L7_char8  keyFile[L7_MAX_FILENAME];
  Key       *tempKey = NULL;
  L7_uint32 size = 0;

  memset(keyFile, 0, L7_MAX_FILENAME);
  strcpy(keyFile, L7_SSHD_PATH_SERVER_PRIVKEY);

  if ((osapiFsFileSizeGet(keyFile, &size) != L7_SUCCESS) ||
      (size == 0))
  {
    return L7_FAILURE;
  }

  tempKey = key_load_private(keyFile, "", NULL);
  if (tempKey == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  (*key) = tempKey;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Write SSHv1 RSA key to file
*
* @param EVP_PKEY     *pkey   @b((input)) server key
*
* @returns L7_SUCCESS, file written
* @returns L7_FAILURE, failed to write key file
*
* @comments
*
* @end
*
*********************************************************************/
static L7_RC_t sshdRSAv1KeyFileWrite(RSA *rsa)
{
  Key   key;

  key.type = KEY_RSA1;
  key.rsa = rsa;

  if (key_save_private(&key, L7_SSHD_PATH_SERVER_PRIVKEY, "", "") == 0)
  {
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Write SSHv2 server key to PEM file specified by type
*
* @param sshdKeyType_t type   @b{(input)}  RSA or DSA
* @param EVP_PKEY     *pkey   @b((input)) server key
*
*
* @returns L7_SUCCESS, file written
* @returns L7_FAILURE, failed to write key file
*
* @comments
*
* @end
*
*********************************************************************/
static L7_RC_t sshdKeyFileWrite(sshdKeyType_t type,
                                EVP_PKEY *pkey)
{
  L7_char8 keyFile[L7_MAX_FILENAME];
  L7_uint32 fd = 0;
  FILE *fp = L7_NULLPTR;

  if (sshdKeyFileGet(type, keyFile) != L7_SUCCESS)
  {
    if ((osapiFsFileCreate(keyFile, &fd) != L7_SUCCESS) ||
        (fd == 0))
    {
      return L7_FAILURE;
    }
  } else
  {
    if (osapiFsOpen(keyFile, &fd) == L7_ERROR)
    {
      return L7_FAILURE;
    }
  }

  fp = fdopen((int)fd, "w");
  if (fp == L7_NULLPTR)
  {
    osapiFsClose(fd);
    return L7_FAILURE;
  }

  if (!PEM_write_PrivateKey(fp, pkey, L7_NULLPTR, L7_NULLPTR, 0, 0, L7_NULLPTR))
  {
    fclose(fp);
    return L7_FAILURE;
  }
  fclose(fp);

  /* use same RSA key for SSHv1 and SSHv2 */
  if (type == SSHD_KEY_TYPE_RSA)
  {
    /* write in SSHv1 file format */
    if (sshdRSAv1KeyFileWrite(pkey->pkey.rsa) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Generate RSA or DSA key and write file.
*
* @param sshdKeyType_t type   @b{(input)}  RSA or DSA
* @param L7_uint32     length @b((input))  key length
*
* @returns L7_SUCCESS, key generated
* @returns L7_FAILURE, failed to generate key
*
* @comments  If type is RSA, will save both SSHv1 and SSHv2 formats.
*
* @end
*
*********************************************************************/
L7_RC_t sshdKeyFileCreate(sshdKeyType_t type,
                          L7_uint32     length)
{
  L7_RC_t    rc = L7_FAILURE;
  EVP_PKEY  *pkey = L7_NULLPTR;
  RSA       *rsa = L7_NULLPTR;
  DSA       *dsa = L7_NULLPTR;

  /* unfortunately we cannot avoid some memory allocation
     using openssl, use a do-while such that we have one
     exit point and ensure we free all openssl objects */
  do
  {
    pkey = EVP_PKEY_new();
    if (pkey == L7_NULLPTR)
    {
      break;
    }
    /* generate new RSA public/private key pair */
    if (type == SSHD_KEY_TYPE_RSA)
    {
      rsa = RSA_generate_key(length, RSA_F4, L7_NULLPTR, L7_NULLPTR);
      if ((rsa == L7_NULLPTR) || (!RSA_check_key(rsa)))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdKeyFileCreate, RSA key generation failed\n");
        break;
      }
      /* assign RSA key pair */
      if (!EVP_PKEY_assign_RSA(pkey, rsa))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdKeyFileCreate, RSA key assignment failed\n");
        break;
      }
      rsa = L7_NULLPTR;  /* pkey takes ownership of RSA object */
    } else if (type == SSHD_KEY_TYPE_DSA)
    {
      dsa = DSA_generate_parameters(length, L7_NULLPTR, 0, L7_NULLPTR,
                                    L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);
      if (dsa == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdKeyFileCreate, DSA generate parameters failed\n");
        break;
      }
      if (DSA_generate_key(dsa) == 0)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdKeyFileCreate, DSA key generation failed\n");
        break;
      }
      /* assign DSA key pair */
      if (!EVP_PKEY_assign_DSA(pkey, dsa))
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdKeyFileCreate, DSA key assignment failed\n");
        break;
      }
      dsa = L7_NULLPTR;  /* pkey takes ownership of RSA object */
    } else
    {
      break;
    }

    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
    if (sshdKeyFileWrite(type, pkey) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdKeyFileCreate, failed to write key file.\n");
      osapiSemaGive(sshdGlobalSema);
      break;
    }
    osapiSemaGive(sshdGlobalSema);

    rc = L7_SUCCESS;

  } while(0);

  if (pkey != L7_NULLPTR)
  {
    EVP_PKEY_free(pkey);
  }
  if (rsa != L7_NULLPTR)
  {
    RSA_free(rsa);
  }
  if (dsa != L7_NULLPTR)
  {
    DSA_free(dsa);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Print MD5 hex formatted fingerprint to a buffer
*
* @param EVP_PKEY  *pkey @b((input))  pointer to internal key data
* @param L7_char8  *bp   @b((output)) md5 hex format fingerprint
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  buffer must be at least L7_SSHD_KEY_FINGERPRINT_MD5_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t sshdPubKeyMD5HexPrint(Key        *pkey,
                              L7_char8   *bp)

{
  L7_char8 *tempBuf=L7_NULLPTR;
  if ( (tempBuf=key_fingerprint(pkey,
                      SSH_FP_MD5,
                      SSH_FP_HEX))
                       == NULL)
  {
    return L7_FAILURE;
  }
  osapiStrncpySafe(bp,tempBuf,L7_SSHD_KEY_FINGERPRINT_SIZE_MAX);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Print SHA1 bubble babble formatted fingerprint to a buffer
*
* @param EVP_PKEY  *pkey @b((input))  pointer to internal key data
* @param L7_char8  *bp   @b((output)) sha1 bb format fingerprint
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  buffer must be at least L7_SSHD_KEY_FINGERPRINT_BB_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t sshdPubKeySHA1BBPrint(Key        *pkey,
                              L7_char8   *bp)

{
  L7_char8 *tempBuf = L7_NULLPTR;
  if ( (tempBuf=key_fingerprint(pkey,
                      SSH_FP_SHA1,
                      SSH_FP_BUBBLEBABBLE))
                      == NULL)
  {
    return L7_FAILURE;
  }
  osapiStrncpySafe(bp,tempBuf,L7_SSHD_KEY_FINGERPRINT_SIZE_MAX);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Ensure input strings are all formatted SSHv2 style
*
* @param L7_char8      *key       @b((input))  input key string
* @param sshdKeyType_t  type      @b((input))  key type
* @param L7_char8      *keyData   @b((output)) key data to read/save
*
* @returns L7_SUCCESS,
* @returns L7_FAILURE, not enough room
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdUserPubKeyStringFormat(L7_char8 *key,
                                   sshdKeyType_t type,
                                   L7_char8 *keyData)
{
  L7_char8 *prefix = L7_NULLPTR;

  memset(keyData, 0, L7_SSHD_KEY_DATA_SIZE_MAX);
  if (type == SSHD_KEY_TYPE_RSA)
  {
    prefix = "ssh-rsa ";
  } else if (type == SSHD_KEY_TYPE_DSA)
  {
    prefix = "ssh-dss ";
  } else {
    return L7_FAILURE;
  }
  if (strncmp(key, prefix, strlen(prefix)) != 0)
  {
    if ((strlen(prefix) + strlen(key) + 1) > L7_SSHD_KEY_DATA_SIZE_MAX)
    {
      return L7_FAILURE;
    }
    strcpy(keyData, prefix);
  }
  strcat(keyData, key);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Verify user public key string
*
* @param L7_char8       *keyData @b((input))  key string
* @param sshdKeyType_t   type    @b((input))  key type
*
* @returns L7_SUCCESS, valid string
* @returns L7_FAILURE, invalid string
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdUserPubKeyStringVerify(L7_char8     *key,
                                   sshdKeyType_t type)
{
  L7_RC_t   rc = L7_FAILURE;
  Key *k = key_new(-1, KEY_UNSPEC);

  if (key_read(k,&key) == 1)
  {
    if ((type == SSHD_KEY_TYPE_RSA) &&
        (k->type == KEY_RSA))
    {
      rc = L7_SUCCESS;
    } else if ((type == SSHD_KEY_TYPE_DSA) &&
               (k->type == KEY_DSA))
    {
      rc = L7_SUCCESS;
    }
  }
  key_free(k);
  return rc;
}

/*********************************************************************
*
* @purpose Lookup username and type in public key configuration
*
* @param L7_char8       *username @b((input))  public key user
* @param sshdKeyType_t   type     @b((input))  key type
*
* @returns L7_SUCCESS, index returned
* @returns L7_FAILURE, user and type not found
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdUserPubKeyIndexGet(L7_char8        *username,
                               sshdKeyType_t    type,
                               L7_uint32       *index)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_SSHD_PUBKEY_USERS_MAX; i++)
  {
    if (sshdCfg->cfg.sshdPubKeys[i].username[0] == 0)
    {
      break;
    }
    if ((strcmp(username, sshdCfg->cfg.sshdPubKeys[i].username) == 0) &&
        (type == sshdCfg->cfg.sshdPubKeys[i].type))
    {
      *index = i;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Lookup username and type in public key configuration
*
* @param L7_char8       *username @b((input))  public key user
* @param sshdKeyType_t   type     @b((input))  key type
*
* @returns L7_SUCCESS, index returned
* @returns L7_FAILURE, user and type not found
*
* @comments  pkey MUST be freed with key_free!!!
*
* @end
*
*********************************************************************/
L7_RC_t sshdUserPubKeyGet(L7_char8     *username,
                          sshdKeyType_t type,
                          Key          *pkey)
{
  L7_uint32 index = 0;
  L7_char8 *p = L7_NULLPTR;
  if (sshdUserPubKeyIndexGet(username, type, &index) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  p = sshdCfg->cfg.sshdPubKeys[index].key;

  if (key_read(pkey, &p) < 0)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Supply callback for vendor code to verify public key
*
* @param L7_char8  *user @b((input))  user name
* @param Key       *pkey @b((input))  pointer to ssh internal key data
*
* @returns L7_SUCCESS, key authenticated
* @returns L7_FAILURE, authentication failed
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t sshdUserPubKeyAuthCallback(L7_char8 *user,
                                   Key      *pkey)
{
  L7_RC_t rc = L7_FAILURE;
  Key *k = key_new(-1, KEY_UNSPEC);

  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_SSHD_COMPONENT_ID,
          "sshdUserPubKeyAuthCallback, username %s\n", user);

  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  if (sshdCfg->cfg.sshdPubKeyAuthMode == L7_ENABLE)
  {
    sshdKeyType_t type = (pkey->type == KEY_RSA) ? SSHD_KEY_TYPE_RSA : SSHD_KEY_TYPE_DSA;
    /* get user key from configuration */
    if (sshdUserPubKeyGet(user, type, k) == L7_SUCCESS)
    {
      if (key_equal(pkey, k) != 0)
      {
        rc = L7_SUCCESS;
      }
    }
  }
  key_free(k);
  osapiSemaGive(sshdGlobalSema);
  return rc;
}
