/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt_util.c
*
* @purpose SSL Tunnel Utility functions
*
* @component sslt
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#include "sslt_include.h"
#include "openssl_api.h"
#include "sslt_exports.h"
#include "transfer_exports.h"

extern BIO *acc1;
extern BIO *acc2;
extern BIO *acc3;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
extern BIO *acc6_1;
extern BIO *acc6_2;
extern BIO *acc6_3;
#endif
extern SSL_CTX *ctx;
extern ssltGlobal_t ssltGlobal;
extern L7_int32 numberOfsslTask;

extern void *ssltCertSema;
extern L7_uint32 dhTaskId;

extern L7_uint32 ssltGetTaskIdHelper(ssltSecureTypes_t secureType);

static DH *dh512 = L7_NULL;
static DH *dh1024 = L7_NULL;



/*********************************************************************
*
* @purpose Seed the OpenSSL Random Number Generator
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltSeedPrng(void)
{
  L7_char8 randomFile[] = "./sslt.rnd";
  opensslSeedPrng(randomFile);
  return;
}

/*********************************************************************
*
* @purpose Initialize parameters for the Diffie-Hellman Key
*          exchange protocol.
*
* @returns L7_SUCCESS, parameters read
*          L7_FAILURE, failed to read DH pem files.
*
* @comments  ssltCertSema should be held for this call.
*
* @end
*
*********************************************************************/
L7_RC_t ssltDiffieHellmanParamsExist(void)
{
  if ((dh512 != L7_NULLPTR) &&
      (dh1024 != L7_NULLPTR))
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Initialize parameters for the Diffie-Hellman Key
*          exchange protocol.
*
* @returns L7_SUCCESS, parameters read
*          L7_FAILURE, failed to read DH pem files.
*
* @comments  called during startup, will use sema to assign static globals
*
* @end   cert semaphore should be held for this call
*
*********************************************************************/
L7_RC_t ssltDiffieHellmanParamsInit(void)
{
  return opensslDiffieHellmanParamsInit(L7_SSLT_DHWEAK_PEM, L7_SSLT_DHSTRONG_PEM,
                                        &dh512, &dh1024);
}

/*********************************************************************
*
* @purpose Free parameters for the Diffie-Hellman Key
*          exchange protocol.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltDiffieHellmanParamsFree(void)
{
  /* make sure these are freed on any failure */
  opensslDiffieHellmanParamsFree(&dh512, &dh1024);
  return;
}

/*********************************************************************
*
* @purpose Initialize parameters for the Diffie-Hellman Key
*          exchange protocol.
*
* @returns void
*
* @comments  This function will generate the parameters if required
*            it should always be called on a separate task, these
*            take a long time, will use sema to assign static globals.
*
* @end
*
*********************************************************************/
void ssltDiffieHellmanParamsCreate(void)
{
  opensslDiffieHellmanParamsCreate(&ssltCertSema, &dh512, &dh1024);
  return;
}

/*********************************************************************
*
* @purpose Establish a callback for the Diffie-Hellman Key exchange protocol.
*
* @param ssl @b{(input)} pointer to the SSL connection object
* @param is_export @b{(input)} N/A
* @param keylength @b{(input)} selected key length
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
DH *ssltDiffieHellmanCallback(SSL *ssl,
                              int is_export,
                              int keylength)
{
  DH *dhParms;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltDiffieHellmanCallback\n");

  switch (keylength)
  {
    /* Selecting Diffie-Hellman weak key exchange parms */
    case 512:
      dhParms = dh512;

      break;

    /* Selecting Diffie-Hellman strong key exchange parms */
    case 1024:
    default:
      dhParms = dh1024;

      break;
  }

  return dhParms;
}

/*********************************************************************
*
* @purpose Get PEM file name for a certificate number.
*
* @param L7_uint32 number   @b{(input)}  certificate number
* @param L7_char8 *filename @b((output)) PEM filename where cert chain is stored
*
*
* @returns L7_SUCCESS, indicated filename exists
* @returns L7_FAILURE, indicated filename does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltServerCertFileGet(L7_uint32  number,
                              L7_char8  *filename)
{
  L7_uint32 size = 0;

  if (number == 1)
  {
    strcpy(filename, L7_SSLT_SERVER_CERT_PEM_ONE);
  } else
  {
    strcpy(filename, L7_SSLT_SERVER_CERT_PEM_TWO);
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
* @purpose Get PEM key file name for a certificate number.
*
* @param L7_uint32 number   @b{(input)}  certificate number
* @param L7_char8 *filename @b((output)) PEM filename where key is stored
*
*
* @returns L7_SUCCESS, indicated filename exists
* @returns L7_FAILURE, indicated filename does not exist
*
* @comments  filename is always returned
*
* @end
*
*********************************************************************/
L7_RC_t ssltServerKeyFileGet(L7_uint32 number,
                             L7_char8 *filename)
{
  L7_uint32 size = 0;

  if (number == 1)
  {
    strcpy(filename, L7_SSLT_SERVER_KEY_PEM_ONE);
  } else
  {
    strcpy(filename, L7_SSLT_SERVER_KEY_PEM_TWO);
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
* @purpose Read server key from PEM file specified by number
*
* @param L7_uint32 number   @b{(input)}  certificate number
* @param EVP_PKEY **pkey     @b((output)) server key
*
*
* @returns L7_SUCCESS, key returned
* @returns L7_FAILURE, indicated key file corrupt or does not exist
*
* @comments no pass phrase support at this time
*
* @end
*
*********************************************************************/
static L7_RC_t ssltServerKeyRead(L7_uint32 number, EVP_PKEY **pkey)
{
  L7_char8 keyFile[L7_MAX_FILENAME];

  if (ssltServerKeyFileGet(number, keyFile) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (opensslServerKeyRead(keyFile,pkey) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Write server key to PEM file specified by number
*
* @param L7_uint32 number   @b{(input)}  certificate number
* @param EVP_PKEY **pkey     @b((output)) server key
*
*
* @returns L7_SUCCESS, file written
* @returns L7_FAILURE, failed to write key file
*
* @comments no pass phrase support at this time
*
* @end
*
*********************************************************************/
static L7_RC_t ssltServerKeyWrite(L7_uint32 number, EVP_PKEY *pkey)
{
  L7_char8 keyFile[L7_MAX_FILENAME];
  L7_uint32 fd = 0;

  if (ssltServerKeyFileGet(number, keyFile) != L7_SUCCESS)
  {
    if ((osapiFsFileCreate(keyFile, &fd) != L7_SUCCESS) ||
        (fd == 0))
    {
      return L7_FAILURE;
    }
  }

  return opensslServerKeyWrite(keyFile, pkey);
}

/*********************************************************************
*
* @purpose Write server certificate to PEM file specified by number
*
* @param L7_uint32 number   @b{(input)}  certificate number
* @param X509      *cert    @b((input))  server certificate
*
*
* @returns L7_SUCCESS, file written
* @returns L7_FAILURE, failed to write certificate file
*
* @comments
*
* @end
*
*********************************************************************/
static L7_RC_t ssltServerCertWrite(L7_uint32 number, X509 *cert)
{
  L7_char8 certFile[L7_MAX_FILENAME];
  L7_uint32 fd = 0;

  if (ssltServerCertFileGet(number, certFile) != L7_SUCCESS)
  {
    if ((osapiFsFileCreate(certFile, &fd) != L7_SUCCESS) ||
        (fd == 0))
    {
      return L7_FAILURE;
    }
  }

  return opensslServerCertWrite(certFile, cert);
}

/*********************************************************************
*
* @purpose Read server certificate from PEM file specified by number
*
* @param L7_uint32 number   @b{(input)}  certificate number
* @param L7_char8  *certPEM @b((output)) server certificate PEM format
* @param X509      *cert    @b((output)) server certificate internal format
*
*
* @returns L7_SUCCESS,
* @returns L7_FAILURE, failed to read cert file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltServerCertRead(L7_uint32  number,
                           L7_char8  *certPEM,
                           X509     **certX509)
{
  L7_char8  certFile[L7_MAX_FILENAME];

  if (ssltServerCertFileGet(number, certFile) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (opensslServerCertRead(certFile, certPEM, certX509) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((*certX509) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Create a SSL context for the tunnel.
*
* @returns SSL_CTX, pointer to the SSL context or L7_NULL if error
*
* @comments
*
* @end
*
*********************************************************************/
SSL_CTX *ssltContextCreate(void)
{
  L7_BOOL  success = L7_FALSE;
  SSL_CTX  *ctx = L7_NULLPTR;
  EVP_PKEY *pkey = L7_NULLPTR;
  X509     *cert = L7_NULLPTR;
  FILE     *fp = L7_NULLPTR;
  L7_char8 cipher_list[256];

  /* unfortunately we cannot avoid some memory allocation
     using openssl, use a do-while such that we have one
     exit point and ensure we free all openssl objects */
  do
  {
    L7_char8 certFile[L7_MAX_FILENAME];

    ctx = SSL_CTX_new(SSLv23_method());
    if (ctx == L7_NULLPTR)
    {
      break;
    }
    /* the root certificate is stored within the server PEM file, we don't need
       to also set the server root via SSL_CTX_load_verify_locations, likewise
       SSL_CTX_set_default_verify_paths will set the CTX to look up trusted certs
       from standard locations, we shouldn't have any stored anywhere else */
    if ((ssltServerCertFileGet(ssltGlobal.ssltCertificateNumber,
                               certFile) != L7_SUCCESS))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: Error getting certificate file for certficate number %u\n",
              ssltGlobal.ssltCertificateNumber);
      break;
    }
    /* read server certificate and optional certificate chain */
    fp = opensslFileOpen(certFile);
    if (fp == L7_NULLPTR)
    {
      break;
    }
    cert = PEM_read_X509(fp, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);
    if (cert == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: Error reading certificate file for certficate number %u\n",
              ssltGlobal.ssltCertificateNumber);
      break;
    }
    /* load into context */
    if (!SSL_CTX_use_certificate(ctx, cert))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error loading certificate from "
                "file %s. Failed while loading the SSLcertificate from specified file. %s indicates the"
                " file from where the certificate is being read.\n", certFile, certFile);
      break;
    }
    X509_free(cert);
    cert = L7_NULLPTR;

    /* add certificate chain, if any are included, not required */
    while ((cert = PEM_read_X509(fp, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR)) != L7_NULLPTR)
    {
      if (!SSL_CTX_add_extra_chain_cert(ctx, cert))
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error loading certificate from "
                "file %s. Failed while loading the SSLcertificate from specified file. %s indicates the"
                " file from where the certificate is being read.\n", certFile, certFile);
        break;
      }
      X509_free(cert);
      cert = L7_NULLPTR;
    }

    if (ssltServerKeyRead(ssltGlobal.ssltCertificateNumber, &pkey) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error loading private key from file. "
                                                     "Failed while loading private key for SSL connection.\n");
      break;
    }
    if (!SSL_CTX_use_PrivateKey(ctx, pkey))
    {
      /* failed opening/using key, so maybe it's in the old format */
      if (SSL_CTX_load_verify_locations(ctx, L7_SSLT_ROOT_PEM, L7_SSLT_PEM_DIR) != 1)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: Did not find SSL PEM file %s%s, OK to continue\n", L7_SSLT_PEM_DIR, L7_SSLT_ROOT_PEM);
        break;
      }

      if (SSL_CTX_set_default_verify_paths(ctx) != 1)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: Error loading SSL PEM file and/or directory\n");
        break;
      }
      if (SSL_CTX_use_certificate_chain_file(ctx, L7_SSLT_SERVER_PEM) != 1)
      {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error loading certificate from "
                "file %s. Failed while loading the SSLcertificate from specified file. %s indicates the"
                " file from where the certificate is being read.\n", certFile, certFile);
        break;
      }

      if (pkey != L7_NULLPTR)
      {
        EVP_PKEY_free(pkey);
        pkey = L7_NULLPTR;
      }
      if (ssltServerKeyRead(ssltGlobal.ssltCertificateNumber, &pkey) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error loading private key from file. "
                                                     "Failed while loading private key for SSL connection.\n");
        break;
      }

      /* now try again */
      if (!SSL_CTX_use_PrivateKey(ctx, pkey))
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error loading private key from file. "
                                                     "Failed while loading private key for SSL connection.\n");
        break;
      }
    }

  /* Set the supported protocols and options */
    SSL_CTX_set_verify_depth(ctx, 4);
    SSL_CTX_set_options(ctx, ssltGlobal.ssltProtocolLevel);
    SSL_CTX_set_tmp_dh_callback(ctx, ssltDiffieHellmanCallback);
    /* turn off session caching so that memory is not tied up by old cached sessions */
    SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);
    opensslCipherListGet(cipher_list);
    if (!SSL_CTX_set_cipher_list(ctx, cipher_list))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error setting cipher list "
                                         "(no valid ciphers). Failed while setting cipher list.\n");
      break;
    }

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
            "SSLT: Successfully loaded all required SSL PEM files\n");
    success = L7_TRUE;

  } while (0);

  if (fp != L7_NULLPTR)
  {
    fclose(fp);
  }
  if (pkey != L7_NULLPTR)
  {
    EVP_PKEY_free(pkey);
  }
  if (cert != L7_NULLPTR)
  {
    X509_free(cert);
  }

  if (success != L7_TRUE)
  {
    if (ctx != L7_NULLPTR)
    {
      SSL_CTX_free(ctx);
      ctx = L7_NULLPTR;
    }
  }

  return ctx;
}

/*********************************************************************
*
* @purpose Refresh the SSLT Listen Task if it is configured to be running.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltListenTaskRefresh(ssltSecureTypes_t secureType)
{
  L7_RC_t rc = L7_SUCCESS;

  switch (secureType)
  {
    case SSLT_SECURE_ADMIN:
    {
      if (ssltGlobal.ssltAdminMode == L7_ENABLE)
      {
        /* Terminate the SSLT Listen Task */
        ssltListenTaskCleanup(SSLT_SECURE_ADMIN);
        if (ssltGlobal.ssltSecurePort != 0)
        {
          rc = ssltListenTaskStartup(SSLT_SECURE_ADMIN);
        }
      }
      break;
    }
    case SSLT_SECURE_AUX_PORT1:
    {
      if (ssltGlobal.ssltAuxOperMode == L7_ENABLE)
      {
        /* Terminate the SSLT Aux Listen Task */
        ssltListenTaskCleanup(SSLT_SECURE_AUX_PORT1);
        if (ssltGlobal.ssltAuxSecurePort1 != 0)
        {
          rc = ssltListenTaskStartup(SSLT_SECURE_AUX_PORT1);
        }
      }
      break;
    }
    case SSLT_SECURE_AUX_PORT2:
    {
      if (ssltGlobal.ssltAuxOperMode == L7_ENABLE)
      {
        /* Terminate the SSLT Aux Listen Task */
        ssltListenTaskCleanup(SSLT_SECURE_AUX_PORT2);
        if (ssltGlobal.ssltAuxSecurePort2 != 0)
        {
          rc = ssltListenTaskStartup(SSLT_SECURE_AUX_PORT2);
        }
      }
      break;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Cleanup all resources associated with the SSLT Listen Task.
*           This function cleans up the task whenever the task is shared
*           between the admin port and the first auxiliary port.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltCheckAndListenTaskCleanup(void)
{
  if ((L7_ENABLE!=ssltGlobal.ssltAdminMode) && (L7_ENABLE!=ssltGlobal.ssltAuxOperMode))
  {
    if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
    {
      fprintf(stdout, "ssltCheckAndListenTaskCleanup - both modes are disabled\n");
    }
    if (L7_NULL!=ssltGlobal.ssltListenTaskId)
    {
      if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
      {
        fprintf(stdout, "ssltCheckAndListenTaskCleanup - cleanup SSLT_SECURE_ADMIN\n");
      }
      ssltListenTaskCleanup(SSLT_SECURE_ADMIN);
    }
    if (L7_NULL!=ssltGlobal.ssltAuxListenTaskId1)
    {
      if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
      {
        fprintf(stdout, "ssltCheckAndListenTaskCleanup - cleanup SSLT_SECURE_AUX_PORT1\n");
      }
      ssltListenTaskCleanup(SSLT_SECURE_AUX_PORT1);
    }
  }
}

/*********************************************************************
*
* @purpose Cleanup all resources associated with the SSLT Listen Task
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltListenTaskCleanup(ssltSecureTypes_t secureType)
{
  L7_int32 tempTaskId = ssltGetTaskIdHelper(secureType);

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTaskCleanup\n");

  /* Remove the ssltListenTask and resources from the system */
  if ((0!=tempTaskId) && (osapiTaskIdVerify(tempTaskId) == L7_SUCCESS))
  {
    switch (secureType)
    {
      case SSLT_SECURE_ADMIN:
      {
        ssltGlobal.ssltOperMode = L7_DISABLE;
        ssltGlobal.ssltListenTaskId = L7_NULL;
        if (L7_NULL != acc1)
        {
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
          {
            fprintf(stdout, "ssltListenTaskCleanup - acc1 = %x\n", (L7_uint32)acc1);
          }
          /* Free the single BIO */
          BIO_free(acc1);
          acc1 = L7_NULL;
        }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
        if (L7_NULL != acc6_1)
        {
          /* Free the single BIO */
          BIO_free(acc6_1);
          acc6_1 = L7_NULL;
        }
#endif
        break;
      }
      case SSLT_SECURE_AUX_PORT1:
      {
        if (L7_NULL==ssltGlobal.ssltAuxListenTaskId2)
        {
          ssltGlobal.ssltAuxOperMode = L7_DISABLE;
        }
        ssltGlobal.ssltAuxListenTaskId1 = L7_NULL;

        if (L7_NULL != acc2)
        {
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
          {
            fprintf(stdout, "ssltListenTaskCleanup - acc2 = %x\n", (L7_uint32)acc2);
          }
          /* Free the single BIO */
          BIO_free(acc2);
          acc2 = L7_NULL;
        }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
        if (L7_NULL != acc6_2)
        {
          /* Free the single BIO */
          BIO_free(acc6_2);
          acc6_2 = L7_NULL;
        }
#endif
        break;
      }
      case SSLT_SECURE_AUX_PORT2:
      {
        if (L7_NULL==ssltGlobal.ssltAuxListenTaskId1)
        {
          ssltGlobal.ssltAuxOperMode = L7_DISABLE;
        }
        ssltGlobal.ssltAuxListenTaskId2 = L7_NULL;
        if (L7_NULL != acc3)
        {
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
          {
            fprintf(stdout, "ssltListenTaskCleanup - acc3 = %x\n", (L7_uint32)acc3);
          }
          /* Free the single BIO */
          BIO_free(acc3);
          acc3 = L7_NULL;
        }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
        if (L7_NULL != acc6_3)
        {
          /* Free the single BIO */
          BIO_free(acc6_3);
          acc6_3 = L7_NULL;
        }
#endif
        break;
      }
    }

    /* Remove if nobody is listening */
    if ((ctx != L7_NULL) &&
        (ssltGlobal.ssltListenTaskId == L7_NULL) &&
        (ssltGlobal.ssltAuxListenTaskId1 == L7_NULL) &&
        (ssltGlobal.ssltAuxListenTaskId2 == L7_NULL))
    {
      if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
      {
        fprintf(stdout,"ssltListenTaskCleanup - Free ctx = %p\n", (void *)ctx);
      }

      /* Free an allocated SSL Context object */
      SSL_CTX_free(ctx);
      ctx = L7_NULL;
    }

    /* Remove the ssltListenTask from the system */
    if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
    {
      fprintf(stdout,"ssltListenTaskCleanup - Deleting task = %x\n", tempTaskId);
    }
    osapiTaskDelete(tempTaskId);
  }

  osapiSleepMSec(50);
  return;
}

/*********************************************************************
*
* @purpose Cleanup all resources associated with the SSLT Connection Task
*
* @param ssl @b{(input)} pointer to the SSL connection object
*
* @returns void
*
* @comments This may only be called from the SSLT Connection Task itself.
*
* @end
*
*********************************************************************/
void ssltConnTaskCleanup(SSL *ssl)
{
  L7_RC_t rc;
  L7_uint32 taskId;

  /* Return to blocking for the SSL connection for easier closing */
  opensslSecureBlockingSet(ssl);

  SSL_free(ssl);

  ERR_remove_state(L7_NULL);

  if (numberOfsslTask)
    numberOfsslTask--;

  /* End the task as it is no-longer needed */
  if ((rc = osapiTaskIDSelfGet(&taskId)) == L7_SUCCESS)
  {
    osapiTaskDelete(taskId);
  }

  return;
}

static L7_uint32 certGenArgv[3];
static L7_char8 s_common_name[L7_SSLT_SUBJECT_DN_FIELD_MAX + 1];
static L7_char8 s_org_name[L7_SSLT_SUBJECT_DN_FIELD_MAX + 1];
static L7_char8 s_org_unit[L7_SSLT_SUBJECT_DN_FIELD_MAX + 1];
static L7_char8 s_location[L7_SSLT_SUBJECT_DN_FIELD_MAX + 1];
static L7_char8 s_state[L7_SSLT_SUBJECT_DN_FIELD_MAX + 1];
static L7_char8 s_country[L7_SSLT_SUBJECT_DN_COUNTRY_SIZE + 1];
static L7_char8 s_email[L7_SSLT_SUBJECT_DN_FIELD_MAX + 1];

static void ssltCertGenTask(L7_int32 numargs, L7_uint32 *argv )
{
  L7_char8 *commonName = s_common_name;
  L7_char8 *orgName    = s_org_name;
  L7_char8 *orgUnit    = s_org_unit;
  L7_char8 *location   = s_location;
  L7_char8 *state      = s_state;
  L7_char8 *country    = s_country;
  L7_char8 *email      = s_email;
  L7_uint32 number;

  if (strlen(s_common_name) == 0)
  {
    commonName = L7_NULLPTR;
  }
  if (strlen(s_org_name) == 0)
  {
    orgName = L7_NULLPTR;
  }
  if (strlen(s_org_unit) == 0)
  {
    orgUnit = L7_NULLPTR;
  }
  if (strlen(s_location) == 0)
  {
    location = L7_NULLPTR;
  }
  if (strlen(s_state) == 0)
  {
    state = L7_NULLPTR;
  }
  if (strlen(s_country) == 0)
  {
    country = L7_NULLPTR;
  }
  if (strlen(s_email) == 0)
  {
    email = L7_NULLPTR;
  }

  number = argv[0];

  if (ssltServerCredentialsCreate(argv[0], argv[1],
                                  commonName,
                                  orgName,
                                  orgUnit,
                                  location,
                                  state,
                                  country,
                                  email,
                                  argv[2]) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
            "SSHD: ssltServerCredentialsCreate failed\n");
  }

  /* If this is the active certificate need to reload the
     certificate, this is the same as setting to active */
  osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
  if (ssltGlobal.ssltCertificateNumber == number)
  {
    if (ssltIssueCmd(SSLT_CERTIFICATE_ACTIVE_SET, &number) != L7_SUCCESS)
    {
      osapiSemaGive(ssltCertSema);
    }
  }

  osapiSemaGive(ssltCertSema);

  ssltGlobal.ssltCertGenerateFlag = 0;

}

L7_RC_t ssltCertGenerate(L7_uint32 number,
                         L7_uint32 keyLength,
                         L7_char8 *commonName,
                         L7_char8 *orgName,
                         L7_char8 *orgUnit,
                         L7_char8 *location,
                         L7_char8 *state,
                         L7_char8 *country,
                         L7_char8 *email,
                         L7_uint32 days)
{
  L7_int32 certGenTaskId;
  if (commonName != L7_NULLPTR)
  {
    osapiStrncpySafe(s_common_name, commonName, sizeof(s_common_name));
  }
  if (orgName != L7_NULLPTR)
  {
    osapiStrncpySafe(s_org_name, orgName, sizeof(s_org_name));
  }
  if (orgUnit != L7_NULLPTR)
  {
    osapiStrncpySafe(s_org_unit, orgUnit, sizeof(s_org_unit));
  }
  if (location != L7_NULLPTR)
  {
    osapiStrncpySafe(s_location, location, sizeof(s_location));
  }
  if (state != L7_NULLPTR)
  {
    osapiStrncpySafe(s_state, state, sizeof(s_state));
  }
  if (country != L7_NULLPTR)
  {
    osapiStrncpySafe(s_country, country, sizeof(s_country));
  }
  if (email != L7_NULLPTR)
  {
    osapiStrncpySafe(s_email, email, sizeof(s_email));
  }
  certGenArgv[0] = number;
  certGenArgv[1] = keyLength;
  certGenArgv[2] = days;

  certGenTaskId = osapiTaskCreate("ssltCertGenTask", (void *)ssltCertGenTask,
                                  3, &certGenArgv,
                                  ssltSidDefaultStackSize(),
                                  ssltSidDefaultTaskPriority(),
                                  ssltSidDefaultTaskSlice());

  if (certGenTaskId == L7_ERROR)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSHD: Failed to create the ssltCertGenTask task\n");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Create X509 subject name as specified by parameters
*
* @param L7_char8   *commonName @b((input)) subject DN common name field
* @param L7_char8   *orgName    @b((input)) subject DN organization name field
* @param L7_char8   *orgUnit    @b((input)) subject DN organization unit field
* @param L7_char8   *location   @b((input)) subject DN location field
* @param L7_char8   *state      @b((input)) subject DN state field
* @param L7_char8   *country    @b((input)) subject DN country field
* @param L7_char8   *email      @b((input)) subject DN email field
* @param X509_NAME **pname      @b((output)) X509 name object pointer
*
* @returns L7_SUCCESS,
* @returns L7_FAILURE, failed to generate name object
*
* @comments Assumes all input parameters have been verified and are valid.
*
* @end
*
*********************************************************************/
static L7_RC_t ssltX509NameCreate(L7_char8   *commonName,
                                  L7_char8   *orgName,
                                  L7_char8   *orgUnit,
                                  L7_char8   *location,
                                  L7_char8   *state,
                                  L7_char8   *country,
                                  L7_char8   *email,
                                  X509_NAME **pname)
{
  int rc = 0;

  X509_NAME *name = X509_NAME_new();
  if (name == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* build subject name */
  if (commonName != L7_NULLPTR) {
    rc = X509_NAME_add_entry_by_NID(name, NID_commonName, MBSTRING_ASC, commonName, -1, -1, 0);
  }

  if ((1 == rc) && (orgName != L7_NULLPTR)) {
    rc = X509_NAME_add_entry_by_NID(name, NID_organizationName, MBSTRING_ASC, orgName, -1, -1, 0);
  }

  if ((1 == rc) && (orgUnit != L7_NULLPTR)) {
    rc = X509_NAME_add_entry_by_NID(name, NID_organizationalUnitName, MBSTRING_ASC, orgUnit, -1, -1, 0);
  }

  if ((1 == rc) && (location != L7_NULLPTR)) {
    rc = X509_NAME_add_entry_by_NID(name, NID_localityName, MBSTRING_ASC, location, -1, -1, 0);
  }

  if ((1 == rc) && (state != L7_NULLPTR)) {
    rc = X509_NAME_add_entry_by_NID(name, NID_stateOrProvinceName, MBSTRING_ASC, state, -1, -1, 0);
  }

  if ((1 == rc) && (country != L7_NULLPTR)) {
    rc = X509_NAME_add_entry_by_NID(name, NID_countryName, MBSTRING_ASC, country, -1, -1, 0);
  }

  if ((1 == rc) && (email != L7_NULLPTR)) {
    rc = X509_NAME_add_entry_by_NID(name, NID_pkcs9_emailAddress, MBSTRING_ASC, email, -1, -1, 0);
  }

  if (1 != rc) {
    X509_NAME_free(name);
    return L7_FAILURE;
  }

  *pname = name;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Create certificate request as specified by parameters
*
* @param L7_uint32  number     @b{(input)} use key pair for this certificate
* @param L7_char8  *commonName @b((input)) subject DN common name field
* @param L7_char8  *orgName    @b((input)) subject DN organization name field
* @param L7_char8  *orgUnit    @b((input)) subject DN organization unit field
* @param L7_char8  *location   @b((input)) subject DN location field
* @param L7_char8  *state      @b((input)) subject DN state field
* @param L7_char8  *country    @b((input)) subject DN country field
* @param L7_char8  *email    @b((input)) subject DN email field
* @param L7_char8  *req        @b((output)) certificate request in DER format
*                                            (base-64 encoded X509)
*
* @returns L7_SUCCESS, X509 request generated
* @returns L7_FAILURE, failed to generate request
*
* @comments Assumes all input parameters have been verified and are valid.
*
* @end
*
*********************************************************************/
L7_RC_t ssltServerCertReqCreate(L7_uint32  number,
                                L7_char8  *commonName,
                                L7_char8  *orgName,
                                L7_char8  *orgUnit,
                                L7_char8  *location,
                                L7_char8  *state,
                                L7_char8  *country,
                                 L7_char8  *email,
                                L7_char8  *req)
{
  L7_RC_t    rc = L7_FAILURE;
  X509_REQ  *reqX509 = L7_NULLPTR;
  X509_NAME *name = L7_NULLPTR;
  EVP_PKEY  *pkey = L7_NULLPTR;
  BIO       *bio = L7_NULLPTR;

  memset(req, 0, L7_SSLT_PEM_BUFFER_SIZE_MAX);
  /* unfortunately we cannot avoid some memory allocation
     using openssl, use a do-while such that we have one
     exit point and ensure we free all openssl objects */
  do
  {
    L7_uint32 length = 0;
    L7_char8 *p = L7_NULLPTR;

    /* read existing key pair */
    if (ssltServerKeyRead(number, &pkey) != L7_SUCCESS)
    {
      break;
    }
    if (ssltX509NameCreate(commonName, orgName, orgUnit, location,
                           state, country, email, &name) != L7_SUCCESS)
    {
      break;
    }
    reqX509 = X509_REQ_new();
    if (reqX509 == L7_NULLPTR)
    {
      break;
    }
    bio = BIO_new(BIO_s_mem());
    if (bio == L7_NULLPTR)
    {
      break;
    }
     if (!X509_REQ_set_version(reqX509, L7_SSLT_CERT_X509_VERSION))
    {
      break;
    }
    /* assign subject name */
    if (!X509_REQ_set_subject_name(reqX509, name))
    {
      break;
    }
    name = L7_NULLPTR;  /* req takes ownership of name object */

    /* assign public key, req does NOT take ownership of key object */
    if (!X509_REQ_set_pubkey(reqX509, pkey))
    {
      break;
    }
    /* sign the request */
    if (!X509_REQ_sign(reqX509, pkey, EVP_sha1()))
    {
      break;
    }
    /* convert internal X509 to PEM format */
    /* make sure request buffer is big enough */
    if (!PEM_write_bio_X509_REQ(bio, reqX509))
    {
      break;
    }
    length = BIO_get_mem_data(bio, &p);
    if ((length < 0) ||
        (length > (L7_SSLT_PEM_BUFFER_SIZE_MAX-1)))
    {
      break;
    }
    memcpy(req, p, length);
    rc = L7_SUCCESS;

  } while(0);

  if (pkey != L7_NULLPTR)
  {
    EVP_PKEY_free(pkey);
  }
  if (reqX509 != L7_NULLPTR)
  {
    X509_REQ_free(reqX509);
  }
  if (name != L7_NULLPTR)
  {
    X509_NAME_free(name);
  }
  if (bio != L7_NULLPTR)
  {
    BIO_free(bio);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Create SSL PEM files as specified by parameters
*
* @param L7_uint32  number     @b{(input)} certificate number
* @param L7_uint32  keyLength  @b{(input)} RSA key length, 0 will use existing key.
* @param L7_char8  *commonName @b((input)) subject DN common name field
* @param L7_char8  *orgName    @b((input)) subject DN organization name field
* @param L7_char8  *orgUnit    @b((input)) subject DN organization unit field
* @param L7_char8  *location   @b((input)) subject DN location field
* @param L7_char8  *state      @b((input)) subject DN state field
* @param L7_char8  *country    @b((input)) subject DN country field
* @param L7_char8  *email      @b((input)) subject DN email field
* @param L7_uint32  days       @b((input)) days certificate will be valid
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Assumes all input parameters have been verified and are valid.
*
* @end
*
*********************************************************************/
L7_RC_t ssltServerCredentialsCreate(L7_uint32 number,
                                    L7_uint32 keyLength,
                                    L7_char8 *commonName,
                                    L7_char8 *orgName,
                                    L7_char8 *orgUnit,
                                    L7_char8 *location,
                                    L7_char8 *state,
                                    L7_char8 *country,
                                    L7_char8 *email,
                                    L7_uint32 days)
{
  L7_RC_t    rc = L7_FAILURE;
  RSA       *rsa = L7_NULLPTR;
  EVP_PKEY  *pkey = L7_NULLPTR;
  X509_NAME *subject = L7_NULLPTR;
  X509_NAME *issuer = L7_NULLPTR;
  X509      *cert = L7_NULLPTR;

  /* unfortunately we cannot avoid some memory allocation
     using openssl, use a do-while such that we have one
     exit point and ensure we free all openssl objects */
  do
  {
    if (keyLength != 0)
    {
      /* generate new RSA public/private key pair */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: creating server RSA key pair...\n");
      rsa = RSA_generate_key(keyLength, RSA_F4, L7_NULLPTR, L7_NULLPTR);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: verifying RSA key pair...\n");
      if ((rsa == L7_NULLPTR) || (!RSA_check_key(rsa)))
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: ssltServerCredentialsCreate, RSA key generation failed\n");
        break;
      }
      pkey = EVP_PKEY_new();
      if (pkey == L7_NULLPTR)
      {
        break;
      }
      /* assign RSA key pair */
      if (!EVP_PKEY_assign_RSA(pkey, rsa))
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: ssltServerCredentialsCreate, RSA key assignment failed\n");
        break;
      }
      rsa = L7_NULLPTR;  /* pkey takes ownership of RSA object */
    } else
    {
      osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
      /* read existing key pair */
      if (ssltServerKeyRead(number, &pkey) != L7_SUCCESS)
      {
        osapiSemaGive(ssltCertSema);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: ssltServerCredentialsCreate, failed to read RSA key file\n");
        break;
      }
      osapiSemaGive(ssltCertSema);
    }

    cert = X509_new();
    if (cert == L7_NULLPTR)
    {
      break;
    }

    /* create the subject name */
    if (ssltX509NameCreate(commonName, orgName, orgUnit, location,
                           state, country, email, &subject) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to create subject name\n");
      break;
    }

    /* assign the subject name */
    if (!X509_set_subject_name(cert, subject))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to assign certificate subject\n");
      break;
    }
    subject = L7_NULLPTR; /* X509 object takes ownership of name object */

    /* create the issuer name */
    if (ssltX509NameCreate(L7_SSLT_ISSUER_COMMON_NAME, orgName, orgUnit, location,
                           state, country, email, &issuer) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to create issuer name\n");
      break;
    }
    /* assign the issuer name */
    if (!X509_set_issuer_name(cert, issuer))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to assign certificate issuer\n");
      break;
    }
    issuer = L7_NULLPTR; /* X509 object takes ownership of name object */

    /* set the version */
    if (!X509_set_version(cert, L7_SSLT_CERT_X509_VERSION))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to set X509 version\n");
      break;
    }
    /* set the serial number to the certificate number */
    if (!ASN1_INTEGER_set(X509_get_serialNumber(cert), (long)number))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to assign serial number\n");
      break;
    }
    /* set the public key, object does NOT take ownership of key object */
    if (!X509_set_pubkey(cert, pkey))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to assign certificate public key\n");
      break;
    }
    /* set the valid dates */
    if (((!X509_gmtime_adj(X509_get_notBefore(cert),0))) ||
        (!X509_gmtime_adj(X509_get_notAfter(cert), (long)(60*60*24*days))))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to set certificate valid days\n");
      break;
    }
    /* last step, self-sign with our private key */
    if (!X509_sign(cert, pkey, EVP_sha1()))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to sign certificate\n");
      break;
    }
    /* write key file and certificate file */
    if (keyLength != 0)
    {
      osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
      if (ssltServerKeyWrite(number, pkey) != L7_SUCCESS)
      {
        osapiSemaGive(ssltCertSema);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: ssltServerCredentialsCreate, failed to write RSA key file\n");
        break;
      }
      osapiSemaGive(ssltCertSema);
    }
    osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
    if (ssltServerCertWrite(number, cert) != L7_SUCCESS)
    {
      osapiSemaGive(ssltCertSema);
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsCreate, failed to write server certificate file\n");
      break;
    }
    osapiSemaGive(ssltCertSema);

    rc = L7_SUCCESS;

  } while (0);

  if (rsa)
  {
    RSA_free(rsa);
  }
  if (pkey)
  {
    EVP_PKEY_free(pkey);
  }
  if (subject)
  {
    X509_NAME_free(subject);
  }
  if (issuer)
  {
    X509_NAME_free(issuer);
  }
  if (cert)
  {
    X509_free(cert);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Write SSL certificate PEM file from imported certificate
*
* @param L7_uint32  number     @b{(input)} certificate number
* @param L7_char8   *cert      @b((input)) CA signed certificate,
*                                          may include certificate chain.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltServerCredentialsImport(L7_uint32 number,
                                    L7_char8 *cert)
{
  L7_RC_t    rc = L7_FAILURE;
  EVP_PKEY  *pkey = L7_NULLPTR;
  X509      *certX509 = L7_NULLPTR;
  BIO       *bio = L7_NULLPTR;
  L7_char8   certFile[L7_MAX_FILENAME];

  /* unfortunately we cannot avoid some memory allocation
     using openssl, use a do-while such that we have one
     exit point and ensure we free all openssl objects */
  do
  {
    /* create a read-only bio from cert buffer */
    bio = BIO_new_mem_buf(cert, strlen(cert));
    if (bio == L7_NULLPTR)
    {
      break;
    }
    /* read X509 format */
    certX509 = PEM_read_bio_X509(bio, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);
    if (certX509 == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsImport: failed to parse certificate\n");
      break;
    }

    /* read existing key pair */
    if (ssltServerKeyRead(number, &pkey) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsImport, failed to read RSA key file\n");
      break;
    }

    /* verify our private key matches this certificate */
    if (!X509_check_private_key(certX509, pkey))
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsImport: failed to verify certficate against private key\n");
      break;
    }

    /* write (or overwrite) the certificate file */
    if (ssltServerCertFileGet(number, certFile) == L7_SUCCESS)
    {
      if (osapiFsDeleteFile(certFile) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
                "SSLT: ssltServerCredentialsImport: failed to delete previous certificate\n");
      }
    }
    if (ssltServerCertWrite(number, certX509) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: ssltServerCredentialsImport: failed to save certificate\n");
      break;
    }

    rc = L7_SUCCESS;

  } while (0);

  if (pkey != L7_NULLPTR)
  {
    EVP_PKEY_free(pkey);
  }
  if (certX509 != L7_NULLPTR)
  {
    X509_free(certX509);
  }
  if (bio != L7_NULLPTR)
  {
    BIO_free(bio);
  }

  return rc;
}

static const char *mon[12]=
{
  "Jan","Feb","Mar","Apr","May","Jun",
  "Jul","Aug","Sep","Oct","Nov","Dec"
};

/*********************************************************************
*
* @purpose Print generalized ASN1 time string to a buffer
*
* @param char                 *bp @b{(output)} pointer to output buffer
* @param ASN1_GENERALIZEDTIME *tm @b((input))  pointer to ASN1 time object
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  This was copied directly from ASN1_GENERALIZEDTIME_print,
*            which prints to an open BIO, we want to avoid using BIOs that
*            allocate memory on the fly and openssl does not provide another
*            method, the only change here is to print to a buffer,
*            everything else is left the same so we can refer back to
*            the original function if needed.
*
* @end
*
*********************************************************************/
static int sslt_ASN1_GENERALIZEDTIME_print(char *bp, ASN1_GENERALIZEDTIME *tm)
{
  char *v;
  int gmt=0;
  int i;
  int y=0,M=0,d=0,h=0,m=0,s=0;

  i=tm->length;
  v=(char *)tm->data;

  if (i < 12) return(0);
  if (v[i-1] == 'Z') gmt=1;
  for (i=0; i<12; i++)
      if ((v[i] > '9') || (v[i] < '0')) return(0);
  y= (v[0]-'0')*1000+(v[1]-'0')*100 + (v[2]-'0')*10+(v[3]-'0');
  M= (v[4]-'0')*10+(v[5]-'0');
  if ((M > 12) || (M < 1)) return(0);
  d= (v[6]-'0')*10+(v[7]-'0');
  h= (v[8]-'0')*10+(v[9]-'0');
  m=  (v[10]-'0')*10+(v[11]-'0');
  if (  (v[12] >= '0') && (v[12] <= '9') &&
      (v[13] >= '0') && (v[13] <= '9'))
      s=  (v[12]-'0')*10+(v[13]-'0');
  if (sprintf(bp,"%s %2d %02d:%02d:%02d %d%s",
      mon[M-1],d,h,m,s,y,(gmt)?" GMT":"") <= 0)
      return(0);
  return(1);
}

/*********************************************************************
*
* @purpose Print UTC ASN1 time string to a buffer
*
* @param char                 *bp @b{(output)} pointer to output buffer
* @param ASN1_UTCTIME         *tm @b((input))  pointer to ASN1 time object
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  This was copied directly from ASN1_UTCTIME_print,
*            which prints to an open BIO, we want to avoid using BIOs that
*            allocate memory on the fly and openssl does not provide another
*            method, the only change here is to print to a buffer,
*            everything else is left the same so we can refer back to
*            the original function if needed.
*
* @end
*
*********************************************************************/
static int sslt_ASN1_UTCTIME_print(char *bp, ASN1_UTCTIME *tm)
{
  char *v;
  int gmt=0;
  int i;
  int y=0,M=0,d=0,h=0,m=0,s=0;

  i=tm->length;
  v=(char *)tm->data;

  if (i < 10) return(0);
  if (v[i-1] == 'Z') gmt=1;
  for (i=0; i<10; i++)
      if ((v[i] > '9') || (v[i] < '0')) return(0);
  y= (v[0]-'0')*10+(v[1]-'0');
  if (y < 50) y+=100;
  M= (v[2]-'0')*10+(v[3]-'0');
  if ((M > 12) || (M < 1)) return(0);
  d= (v[4]-'0')*10+(v[5]-'0');
  h= (v[6]-'0')*10+(v[7]-'0');
  m=  (v[8]-'0')*10+(v[9]-'0');
  if (  (v[10] >= '0') && (v[10] <= '9') &&
      (v[11] >= '0') && (v[11] <= '9'))
      s=  (v[10]-'0')*10+(v[11]-'0');

  if (sprintf(bp,"%s %2d %02d:%02d:%02d %d%s",
      mon[M-1],d,h,m,s,y+1900,(gmt)?" GMT":"") <= 0)
      return(0);
  return(1);
}


/*********************************************************************
*
* @purpose Print ASN1 time string to a buffer
*
* @param ASN1_TIME *tm @b((input)) pointer to ASN1 time object
* @param L7_char8  *bp @b{(output)} pointer to output buffer
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   wrap the functions copied from openssl above.
*
* @end
*
*********************************************************************/
L7_RC_t ssltASN1TimePrint(ASN1_TIME *tm,
                          L7_char8  *bp)
{
  int rc = 0;

  if (tm->type == V_ASN1_UTCTIME)
  {
    rc = sslt_ASN1_UTCTIME_print(bp, tm);
  }
  if (tm->type == V_ASN1_GENERALIZEDTIME)
  {
    rc = sslt_ASN1_GENERALIZEDTIME_print(bp, tm);
  }

  if (rc == 0)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Print MD5 certificate fingerprint to a buffer
*
* @param X509      *cert @b((input)) pointer to ASN1 time object
* @param L7_char8  *bp   @b{(output)} pointer to output buffer
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  buffer must be at least L7_SSLT_FINGERPRINT_MD5_SIZE_MAX
*
* @end
*
*********************************************************************/
L7_RC_t ssltX509DigestPrint(X509      *cert,
                            L7_char8  *bp)

{
  L7_uint32 i = 0;
  L7_uint32 n = 0;
  L7_uchar8  md[EVP_MAX_MD_SIZE];
  L7_uchar8 *p = L7_NULLPTR;

  if (!X509_digest(cert, EVP_md5(), md, &n))
  {
    return L7_FAILURE;
  }

  p = bp;
  for (i = 0; i < n; i++)
  {
    p += sprintf(p, "%02X", md[i]);
  }
  *p = '\0';
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Set the SSLT Debug Level
*
* @param level @b{(input)} Debug level
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is for internal use only. It will allow SSLT
* @comments   printf's to appear on the console.
*
* @end
*
*********************************************************************/
L7_RC_t ssltDebugLevelSet(L7_uint32 level)
{
  ssltGlobal.ssltDebugLevel = level;

  return L7_SUCCESS;
}

