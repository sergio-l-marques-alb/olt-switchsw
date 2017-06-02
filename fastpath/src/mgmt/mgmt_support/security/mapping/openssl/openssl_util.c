/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename openssl_util.c
*
* @purpose SSL Tunnel Utility functions
*
* @component openssl
*
* @comments none
*
* @create 11/09/2006
*
* @author jshaw
*
* @end
*
**********************************************************************/
#include "openssl_include.h"
#include "osapi.h"
#include "dtlapi.h"

/* Storage for all of the Static Locking Semaphores required by OpenSSL. */
static L7_uint32 *semaBuf = L7_NULL;

static L7_uint32 opensslDebugLevel = 0;
/* Struct for the Dynamic Locking Semaphores required by OpenSSL. */
struct CRYPTO_dynlock_value
{
  void *semaphore;
};

L7_BOOL opensslInitialized = L7_FALSE;

/*********************************************************************
*
* @purpose Initialize the SSL Library and its supporting functions.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslInit(L7_BOOL selfInit)
{
  if (selfInit == L7_TRUE)
  {
    /*
    Initialize the OpenSSL library here
    */

    if (opensslInitialized == L7_TRUE)
    {
        return L7_SUCCESS;
    }

    if (opensslSemaSetup() != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_OPENSSL_COMPONENT_ID, "OpenSSL initialization failed!\n");

      return L7_FAILURE;
    }

    (void) SSL_library_init(); /* Always returns 1 */
    SSLeay_add_all_algorithms();
    SSL_load_error_strings();

    opensslInitialized = L7_TRUE;

    return L7_SUCCESS;
  }
  else
  {
    L7_uint32 i = 0;

    /*
    Other components must wait for initialization of OpenSSL to complete
    For now, wait 10 seconds before returning failure
    */

    while (i < 40)
    {
      if (opensslInitialized == L7_TRUE)
      {
          return L7_SUCCESS;
      }

      osapiSleepMSec(250);
      i++;
    }

    return L7_FAILURE;
  }
}

/*********************************************************************
*
* @purpose OpenSSL required callback function for locking/unlocking a
*          Static Locking semaphore.
*
* @param mode @b{(input)} LOCK or UN-LOCK the semaphore
* @param n @b{(input)} the index of the semaphore to be manipulated
* @param file @b{(input)} pointer to the callers file name
* @param line @b{(input)} pointer to the callers line number
*
* @returns void
*
* @comments The parameters and return values must match the callback
*           function call as specified within the OpenSSL library.
*
* @end
*
*********************************************************************/
static void openssl_locking_function(int mode,
                                     int n,
                                     const char * file,
                                     int line)
{
  if (mode & CRYPTO_LOCK)
  {
    /* OpenSSL wants to protect a resource */

    if (opensslDebugLevel >= OPENSSL_DEBUG_LVL4)
      fprintf(stderr, "openssl_locking_function LOCK - n=%d, semaBuf[n]=%x\n", n, semaBuf[n]);

    osapiSemaTake((void *)semaBuf[n], L7_WAIT_FOREVER);
  }
  else
  {
    /* OpenSSL wants to unlock a resource */
    if (opensslDebugLevel >= OPENSSL_DEBUG_LVL4)
      fprintf(stderr, "openssl_locking_function UN-LOCK - n=%d, semaBuf[n]=%x\n", n, semaBuf[n]);

    osapiSemaGive((void *)semaBuf[n]);
  }

  return;
}

/*********************************************************************
*
* @purpose OpenSSL required callback function for identifing the task
*          which is using the OpenSSL Library.
*
* @returns the current Task Id
*
* @comments The parameters and return values must match the callback
*           function call as specified within the OpenSSL library.
*
* @end
*
*********************************************************************/
static unsigned long openssl_id_function(void)
{
  L7_int32 taskId;

  (void)osapiTaskIDSelfGet(&taskId);

  if (opensslDebugLevel >= OPENSSL_DEBUG_LVL4)
    fprintf(stderr, "openssl_id_function - taskId=%x\n", taskId);

  return((unsigned long)taskId);
}

/*********************************************************************
*
* @purpose OpenSSL required callback function for creating a Dynamic
*          Locking semaphore.
*
* @param file @b{(input)} pointer to the callers file name
* @param line @b{(input)} pointer to the callers line number
*
* @returns a pointer to the Dynamic Locking stuct
*
* @comments The parameters and return values must match the callback
*           function call as specified within the OpenSSL library.
*
* @end
*
*********************************************************************/
static struct CRYPTO_dynlock_value *openssl_dyn_create_function(const char *file,
                                                                int line)
{
  struct CRYPTO_dynlock_value *sem;

  if ((sem = (struct CRYPTO_dynlock_value *)osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE)) == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_OPENSSL_COMPONENT_ID,
            "OPENSSL: Could not delete the dynamic semaphore!\n");

    return L7_NULL;
  }

  if (opensslDebugLevel >= OPENSSL_DEBUG_LVL4)
    fprintf(stderr, "openssl_dyn_create_function - sem=%x\n", (L7_uint32)sem);

  return sem;
}

/*********************************************************************
*
* @purpose OpenSSL required callback function for locking/unlocking a
*          Dynamic Locking semaphore.
*
* @param mode @b{(input)} LOCK or UN-LOCK the semaphore
* @param l @b{(input)} pointer to the semaphore to be manipulated
* @param file @b{(input)} pointer to the callers file name
* @param line @b{(input)} pointer to the callers line number
*
* @returns void
*
* @comments The parameters and return values must match the callback
*           function call as specified within the OpenSSL library.
*
* @end
*
*********************************************************************/
static void openssl_dyn_lock_function(int mode,
                                      struct CRYPTO_dynlock_value *l,
                                      const char *file,
                                      int line)
{
  if (mode & CRYPTO_LOCK)
  {
    /* OpenSSL wants to LOCK the seamphore */
    if (opensslDebugLevel >= OPENSSL_DEBUG_LVL4)
      fprintf(stderr, "dyn_lock_function LOCK - l=%x\n", (L7_uint32)l);

    osapiSemaTake(l, L7_WAIT_FOREVER);
  }
  else
  {
    /* OpenSSL wants to UN-LOCK the seamphore */
    if (opensslDebugLevel >= OPENSSL_DEBUG_LVL4)
      fprintf(stderr, "dyn_lock_function UN-LOCK - l=%x\n", (L7_uint32)l);

    osapiSemaGive(l);
  }

  return;
}

/*********************************************************************
*
* @purpose OpenSSL required callback function for deleting a dynamic
*          locking semaphore.
*
* @param l @b{(input)} pointer to the semaphore to be deleted
* @param file @b{(input)} pointer to the callers file name
* @param line @b{(input)} pointer to the callers line number
*
* @returns void
*
* @comments The parameters and return values must match the callback
*           function call as specified within the OpenSSL library.
*
* @end
*
*********************************************************************/
static void openssl_dyn_destroy_function(struct CRYPTO_dynlock_value *l,
                                         const char *file,
                                         int line)
{
  L7_RC_t rc;

  if ((rc = osapiSemaDelete(l)) == L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_OPENSSL_COMPONENT_ID,
            "OPENSSL: Could not delete the dynamic semaphore!\n");
  }

  return;
}

/*********************************************************************
*
* @purpose Create the Mutual Exclusion Semaphores that are required by
*          the OpenSSL library in a multi-threaded environment.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslSemaSetup(void)
{
  int i;

  /* Allocate space to store the CRYPTO mutex Semaphores */
  if ((semaBuf = osapiMalloc(L7_FLEX_OPENSSL_COMPONENT_ID, CRYPTO_num_locks() * sizeof(void *))) == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_OPENSSL_COMPONENT_ID,
            "OPENSSL: Could not allocate space for the mutex semaphore!\n");

    return L7_FAILURE;
  }

  /* Create the Mutual Exclusion Semaphores that OpenSSL will need */
  for (i = L7_NULL; i < CRYPTO_num_locks(); i++)
  {
    semaBuf[i] = (L7_uint32)osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  }

  /*
  ** Register the Static Locking semaphore callback functions with OpenSSL
  */
  CRYPTO_set_locking_callback(openssl_locking_function);
  CRYPTO_set_id_callback(openssl_id_function);

  /*
  ** Register the Dynamic Locking semaphore callback functions with OpenSSL
  */
  CRYPTO_set_dynlock_create_callback(openssl_dyn_create_function);
  CRYPTO_set_dynlock_lock_callback(openssl_dyn_lock_function);
  CRYPTO_set_dynlock_destroy_callback(openssl_dyn_destroy_function);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Cleanup all resources associated with the OpenSSL Locking
*          semaphores.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void opensslSemaCleanup(void)
{
  L7_uint32 i;
  L7_RC_t rc;

  if (semaBuf == L7_NULL)
  {
    /* Nothing to do */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_OPENSSL_COMPONENT_ID,
            "OPENSSL: Did not need to clean up the SSL semaphores!\n");

    return;
  }

  CRYPTO_set_id_callback(L7_NULL);
  CRYPTO_set_locking_callback(L7_NULL);
  CRYPTO_set_dynlock_create_callback(L7_NULL);
  CRYPTO_set_dynlock_lock_callback(L7_NULL);
  CRYPTO_set_dynlock_destroy_callback(L7_NULL);

  for (i = L7_NULL; i < CRYPTO_num_locks(); i++)
  {
    if ((rc = osapiSemaDelete((void *)semaBuf[i])) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_OPENSSL_COMPONENT_ID,
              "OPENSSL: Could not delete the SSL semaphores!\n");
    }
  }

  osapiFree(L7_FLEX_OPENSSL_COMPONENT_ID, semaBuf);

  semaBuf = L7_NULL;

  return;
}

/*********************************************************************
*
* @purpose Set the OPENSSL Debug Level
*
* @param level @b{(input)} Debug level
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments This is for internal use only. It will allow OPENSSL
* @comments   printf's to appear on the console.
*
* @end
*
*********************************************************************/
L7_RC_t opensslDebugLevelSet(L7_uint32 level)
{
  opensslDebugLevel = level;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Read DH params from PEM file
*
* @param    L7_char8  *file  @b{(input)}
* @param    DH       **dh    @b((output))  diffie hellman parameters
*
*
* @returns  L7_SUCCESS, file written
* @returns  L7_FAILURE, failed to read pem file
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t
opensslDiffieHellmanParamsRead(L7_char8 *file, DH **dh)
{
    FILE    *fp = L7_NULLPTR;

    fp = opensslFileOpen(file);

    if (fp == L7_NULLPTR)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_OPENSSL_COMPONENT_ID,
                "OPENSSL: opensslFileOpen could not open %s\n", file);
        return L7_FAILURE;
    }

    (*dh) = (DH *)PEM_read_DHparams(fp, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);

    if ((*dh) == L7_NULLPTR)
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_OPENSSL_COMPONENT_ID,
                "OPENSSL: PEM DH params not read\n");

        fclose(fp);
        return L7_FAILURE;
    }

    fclose(fp);

    return L7_SUCCESS;

} /* opensslDiffieHellmanParamsRead */


