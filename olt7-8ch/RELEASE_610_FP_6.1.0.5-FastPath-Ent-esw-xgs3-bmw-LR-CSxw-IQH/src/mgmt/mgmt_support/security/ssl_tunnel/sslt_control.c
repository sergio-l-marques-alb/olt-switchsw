/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt_control.c
*
* @purpose SSL Tunnel Control functions
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
#include "osapi.h"
#include "sslt_exports.h"
#include "dtlapi.h"

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "usmdb_cpcm_api.h"
#endif

void *ssltCertSema = L7_NULLPTR;
void *ssltTaskSyncSema = L7_NULLPTR;
void *ssltQueue = L7_NULLPTR;

BIO *acc1 = L7_NULLPTR;
BIO *acc2 = L7_NULLPTR;
BIO *acc3 = L7_NULLPTR;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
BIO *acc6_1 = L7_NULLPTR;
BIO *acc6_2 = L7_NULLPTR;
BIO *acc6_3 = L7_NULLPTR;
#endif

SSL_CTX *ctx = L7_NULLPTR;
L7_uint32 dhTaskId = L7_ERROR;

extern ssltGlobal_t ssltGlobal;

typedef struct sockaddr_union_s
{
  union{
      struct sockaddr     sa;
      struct sockaddr_in  sa4;
      struct sockaddr_in6 sa6;
  }u;
}sockaddr_union_t;

L7_uint32 ssltTaskId;
#define MAX_SSL_CONNECTIONS 32
L7_int32 numberOfsslTask = 0;

/*********************************************************************
*
* @purpose Return port based on secure type
*
* @returns L7_uint32 sslt secure port
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 ssltGetPortHelper(ssltSecureTypes_t secureType)
{
  switch (secureType)
  {
    case SSLT_SECURE_ADMIN:
    {
      return ssltGlobal.ssltSecurePort;
    }
    case SSLT_SECURE_AUX_PORT1:
    {
      return ssltGlobal.ssltAuxSecurePort1;
    }
    case SSLT_SECURE_AUX_PORT2:
    {
      return ssltGlobal.ssltAuxSecurePort2;
    }
    default:
    {
      return 0;
    }
  }
}

/*********************************************************************
*
* @purpose Return task id based on secure type
*
* @returns L7_uint32 sslt task id
*
* @comments
*
* @end
*
*********************************************************************/
L7_uint32 ssltGetTaskIdHelper(ssltSecureTypes_t secureType)
{
  switch (secureType)
  {
    case SSLT_SECURE_ADMIN:
    {
      return ssltGlobal.ssltListenTaskId;
    }
    case SSLT_SECURE_AUX_PORT1:
    {
      return ssltGlobal.ssltAuxListenTaskId1;
    }
    case SSLT_SECURE_AUX_PORT2:
    {
      return ssltGlobal.ssltAuxListenTaskId2;
    }
    default:
    {
      return 0;
    }
  }
}

/*********************************************************************
*
* @purpose Return BIO  based on secure type
*
* @returns BIO* global BIO
*
* @comments
*
* @end
*
*********************************************************************/
BIO * ssltGetBIOHelper(ssltSecureTypes_t secureType)
{
  switch (secureType)
  {
    case SSLT_SECURE_ADMIN:
    {
      return acc1;
    }
    case SSLT_SECURE_AUX_PORT1:
    {
      return acc2;
    }
    case SSLT_SECURE_AUX_PORT2:
    {
      return acc3;
    }
    default:
    {
      return L7_NULL;
    }
  }
}

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
/*********************************************************************
*
* @purpose Return global BIO IPV6 based on secure type
*
* @returns BIO* global BIO IPV6
*
* @comments
*
* @end
*
*********************************************************************/
BIO * ssltGetBIOV6Helper(ssltSecureTypes_t secureType)
{
  switch (secureType)
  {
    case SSLT_SECURE_ADMIN:
    {
      return acc6_1;
    }
    case SSLT_SECURE_AUX_PORT1:
    {
      return acc6_2;
    }
    case SSLT_SECURE_AUX_PORT2:
    {
      return acc6_3;
    }
    default:
    {
      return L7_NULL;
    }
  }
}
#endif

/*********************************************************************
*
* @purpose Start the main task required by the SSL Tunnel code
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltStartTasks(void)
{
  /* Semaphore creation for task protection */
  ssltTaskSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if (ssltTaskSyncSema == L7_NULL)
  {
    LOG_MSG("SSLT: Unable to create the task semaphore\n");
    return L7_FAILURE;
  }

  /* Create the ssltTask */
  ssltTaskId = (L7_uint32)osapiTaskCreate("ssltTask",
                                          (void *)ssltTask,
                                          L7_NULL,
                                          L7_NULL,
                                          ssltSidDefaultStackSize(),
                                          ssltSidDefaultTaskPriority(),
                                          ssltSidDefaultTaskSlice());

  if (ssltTaskId == L7_ERROR)
  {
    LOG_MSG("SSLT: Failed to create the ssltTask\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(L7_SSLT_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("SSLT: Unable to initialize ssltTask\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose The main SSL Tunnel Task.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltTask(void)
{
  ssltMsg_t msg;
  L7_uint32 status;

  (void)osapiTaskInitDone(L7_SSLT_TASK_SYNC);

  ssltSeedPrng();

  /* generate another task to generate DH parameters if
     they are not already on the box, we want to do this
     after the seed_prng() call above, this task will
     exit after the parameters are generated. */
  if (ssltDiffieHellmanParamsInit() != L7_SUCCESS)
  {
    /* create background task to generate parameters */
    dhTaskId = (L7_uint32)osapiTaskCreate("ssltDHCreate",
                                          (void *)ssltDiffieHellmanParamsCreate,
                                          L7_NULL,
                                          L7_NULL,
                                          L7_DEFAULT_STACK_SIZE,
                                          L7_DEFAULT_TASK_PRIORITY,
                                          L7_DEFAULT_TASK_SLICE);

    if (dhTaskId == L7_ERROR)
    {
      LOG_MSG("SSLT: Failed to create the ssltDiffieHellmanParamsCreate task.\n");
    }
  }

  /*
  ** Loop forever waiting for SSLT messages
  */
  while (L7_TRUE)
  {
    status = osapiMessageReceive(ssltQueue,
                                 (void *)&msg,
                                 (L7_uint32)sizeof(ssltMsg_t),
                                 L7_WAIT_FOREVER);

    if ((status == L7_SUCCESS) &&
        (osapiSemaTake(ssltTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS))
    {
      /*
      ** Process the message
      */
      ssltDispatchCmd(msg);

      (void)osapiSemaGive(ssltTaskSyncSema);
    }
    else if (status != L7_SUCCESS)
    {
      LOG_MSG("SSLT: osapiMessageReceive failed, rc = %d\n", status);
    }
  } /* while */

  return;
}

/*********************************************************************
*
* @purpose Startup the SSLT Listen Task
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltListenTaskStartup(ssltSecureTypes_t secureType)
{
  L7_uint32 taskId;
  L7_uchar8 taskName[SSLT_LISTEN_TASK_NAME_LEN];
  L7_uint32 argc;
  L7_uint32 **argv = L7_NULL;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTaskStartup enter\n");

  /* Init DH if we don't have a running task */
  if ((ssltGlobal.ssltListenTaskId == L7_NULL) &&
      (ssltGlobal.ssltAuxListenTaskId1 == L7_NULL) &&
      (ssltGlobal.ssltAuxListenTaskId2 == L7_NULL))
  {
    /* read DH params everytime we start the listen task,
       if files are loaded on the box we should load the parameters */
    if (ssltDiffieHellmanParamsInit() != L7_SUCCESS)
    {
      LOG_MSG("SSLT: DH parameters have not been generated.\n");
    }
    /* since we can generate certs on the fly, need to
       hold the cert configuration semaphore while we
       are reading credentials into the context */
    osapiSemaTake(ssltCertSema, L7_WAIT_FOREVER);
    if ((ctx = ssltContextCreate()) == L7_NULL)
    {
      osapiSemaGive(ssltCertSema);
      LOG_MSG("SSLT: Failed to create certificate.\n");
      return L7_FAILURE;
    }
    osapiSemaGive(ssltCertSema);
  }

  /* Check if the port is already active. If it is, return success */
  switch (secureType)
  {
    case SSLT_SECURE_ADMIN:
    {
      if ((L7_TRUE == ssltGlobal.ssltAuxOperMode) &&
          ((ssltGlobal.ssltSecurePort == ssltGlobal.ssltAuxSecurePort1) ||
           (ssltGlobal.ssltSecurePort == ssltGlobal.ssltAuxSecurePort2)))
      {
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout, "ssltListenTaskStartup - ssltSecurePort:%d is already listening.\n",ssltGlobal.ssltSecurePort);
        }
        return L7_SUCCESS;
      }
      break;
    }
    case SSLT_SECURE_AUX_PORT1:
    {
      if (((L7_TRUE == ssltGlobal.ssltOperMode) && (ssltGlobal.ssltAuxSecurePort1 == ssltGlobal.ssltSecurePort)) ||
          ((L7_TRUE == ssltGlobal.ssltAuxOperMode) && (ssltGlobal.ssltAuxSecurePort1 == ssltGlobal.ssltAuxSecurePort2)))
      {
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltListenTaskStartup - ssltSecureAuxPort1:%d is already listening.\n",ssltGlobal.ssltAuxSecurePort1);
        }
        return L7_SUCCESS;
      }
      break;
    }
    case SSLT_SECURE_AUX_PORT2:
    {
      if (((L7_TRUE == ssltGlobal.ssltOperMode) && (ssltGlobal.ssltAuxSecurePort2 == ssltGlobal.ssltSecurePort)) ||
          ((L7_TRUE == ssltGlobal.ssltAuxOperMode) && (ssltGlobal.ssltAuxSecurePort1 == ssltGlobal.ssltAuxSecurePort2)))
      {
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltListenTaskStartup - ssltSecureAuxPort2:%d is already listening.\n",ssltGlobal.ssltAuxSecurePort2);
        }
        return L7_SUCCESS;
      }
      break;
    }
  }

  /* Create a task to listen on the secure port */
  argc = 3;
  memset(taskName,0,SSLT_LISTEN_TASK_NAME_LEN);
  argv = osapiMalloc(L7_FLEX_SSLT_COMPONENT_ID, sizeof(L7_uint32 *) * argc);

  if ((SSLT_SECURE_ADMIN == secureType) && (ssltGlobal.ssltListenTaskId == L7_NULL))
  {
    osapiSnprintf(taskName, sizeof(taskName), "%s.%u", "ssltLSTN", ssltGlobal.ssltSecurePort);
  }
  else if ((SSLT_SECURE_AUX_PORT1 == secureType) && (ssltGlobal.ssltAuxListenTaskId1 == L7_NULL))
  {
    osapiSnprintf(taskName, sizeof(taskName), "%s.%u", "ssltLSTN", ssltGlobal.ssltAuxSecurePort1);
  }
  else if ((SSLT_SECURE_AUX_PORT2 == secureType) && (ssltGlobal.ssltAuxListenTaskId2 == L7_NULL))
  {
    osapiSnprintf(taskName, sizeof(taskName), "%s.%u", "ssltLSTN", ssltGlobal.ssltAuxSecurePort2);
  }
  else
  {
    LOG_MSG("SSLT: Failed to create a ssltListenTask, unknown secureType.\n");
    return L7_FAILURE;
  }

  argv[0] = (L7_uint32 *)secureType;
  argv[1] = (L7_uint32 *)ssltGetBIOHelper(secureType);
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  argv[2] = (L7_uint32 *)ssltGetBIOV6Helper(secureType);
#else
  argv[2] = 0;
#endif

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
  {
    fprintf(stdout,"ssltListenTaskStartup - Creating task:%s.\n",taskName);
  }
  taskId = (L7_uint32)osapiTaskCreate(taskName,
                                      (void *)ssltListenTask,
                                      argc,
                                      argv,
                                      L7_DEFAULT_STACK_SIZE,
                                      L7_DEFAULT_TASK_PRIORITY,
                                      L7_DEFAULT_TASK_SLICE);
  if (taskId == L7_ERROR)
  {
    LOG_MSG("SSLT: Failed to create the ssltListenTask: %s.\n",taskName);
    if (SSLT_SECURE_ADMIN == secureType)
    {
      ssltGlobal.ssltListenTaskId = L7_NULL;
    }
    else if (SSLT_SECURE_AUX_PORT1 == secureType)
    {
      ssltGlobal.ssltAuxListenTaskId1 = L7_NULL;
    }
    else if (SSLT_SECURE_AUX_PORT2 == secureType)
    {
      ssltGlobal.ssltAuxListenTaskId2 = L7_NULL;
    }
    return L7_FAILURE;
  }

  /* Store this task id for use when the user wants to disable SSLT */
  if (SSLT_SECURE_ADMIN == secureType)
  {
    ssltGlobal.ssltListenTaskId = taskId;
    ssltGlobal.ssltOperMode = L7_ENABLE;
  }
  else if (SSLT_SECURE_AUX_PORT1 == secureType)
  {
    ssltGlobal.ssltAuxListenTaskId1 = taskId;
    ssltGlobal.ssltAuxOperMode = L7_ENABLE;
  }
  else if (SSLT_SECURE_AUX_PORT2 == secureType)
  {
    ssltGlobal.ssltAuxListenTaskId2 = taskId;
    ssltGlobal.ssltAuxOperMode = L7_ENABLE;
  }

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
  {
     fprintf(stdout,"\nssltListenTaskStartup - Created task id = %x\n", taskId);
  }

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTaskStartup exit\n");

  osapiSleepMSec(50);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Listener socket ready
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltListenReady(BIO *rdy_acc, ssltSecureTypes_t secureType)
{
  BIO *client;
  SSL *ssl;
  L7_uint32 taskId;
  L7_uint32 session_id_context = SSLT_CONTEXT;
  L7_uint32 argc = 2;
  L7_uint32 **argv = L7_NULL;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenReady \n");

  /* Subsequent calls will sit and wait for an incoming connection */
  if (BIO_do_accept(rdy_acc) <= L7_NULL)
  {
    LOG_MSG("SSLT: Error accepting connection\n");

    /* Pause before trying again */
    osapiSleep(1);

    return;
  }

  client = BIO_pop(rdy_acc);

  if (!(ssl = SSL_new(ctx)))
  {
    LOG_MSG("SSLT: Error creating SSL context\n");
    BIO_free(client);

    /* Pause before trying again */
    osapiSleep(1);

    return;
  }

  /* Set the protocol options at the SSL connection level */
  SSL_set_options(ssl, ssltGlobal.ssltProtocolLevel);

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
    fprintf(stdout, "ssltListenTask - ssl = %x\n", (L7_uint32)ssl);

  SSL_set_accept_state(ssl);

  /* Connect the BIO to the SSL connection */
  SSL_set_bio(ssl, client, client);

  /* Set context for session reuse */
  SSL_CTX_set_session_id_context(ctx,
                                 (void *)&session_id_context,
                                 sizeof(session_id_context));

  /* turn off session caching so that memory is not tied up by old cached sessions */
  SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_OFF);

  if (numberOfsslTask >= MAX_SSL_CONNECTIONS)
  {
    LOG_MSG("SSLT: Exceeded maximum, ssltConnectionTask\n");

    /* Free the SSL connection and the connnected BIO */
    SSL_free(ssl);
  }
  else
  {
    argv = osapiMalloc(L7_FLEX_SSLT_COMPONENT_ID, sizeof(L7_uint32 *) * argc);
    argv[0] = (L7_uint32 *)secureType;
    argv[1] = (L7_uint32 *)ssl;

    /* Create a task to handle the SSLT Connection */
    taskId = (L7_uint32)osapiTaskCreate("ssltConnTask",
                                        (void *)ssltConnectionTask,
                                        argc,
                                        argv,
                                        L7_DEFAULT_STACK_SIZE,
                                        L7_DEFAULT_TASK_PRIORITY,
                                        L7_DEFAULT_TASK_SLICE);

    if (taskId == L7_ERROR)
    {
      L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Failed to create the "
             "ssltConnectionTask. Exceeded maximum allowed SSLT connections.\n");

      /* Free the SSL connection and the connnected BIO */
      SSL_free(ssl);
    }
    else
    {
      numberOfsslTask++;
    }
  }
}

#ifdef _L7_OS_VXWORKS_

/*****
 * if ipv6 package is defined, there are two choices for binding
 * sockets. with a linux kernel, bind once with the ipv6
 * unspecified address. v4 connections are then accepted as ipv6 v4-mapped.
 * a v4 bind cannot be performed as addr_in_use error is returned.
 * with other stacks, bind once for v4 inaddr_any and once for v6 unspecified.
 *
*****/
/*********************************************************************
*
* @purpose Task to listen for incomming SSLT connections
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltListenTask(int argc, L7_uint32 *argv[])
{
  ssltSecureTypes_t secureType = SSLT_SECURE_ADMIN;
  BIO* acc = L7_NULL;
  BIO* accV6 = L7_NULL;
  L7_uchar8 host_port[SSLT_HOST_PORT_STR_LEN+10];
  L7_uint32 port;
  int listen_s;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  int listen6_s;
#endif

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTask entry\n");

  if ((argc==3) && (argv != L7_NULL))
  {
    secureType = (ssltSecureTypes_t)argv[0];
    acc = (BIO*)argv[1];
    accV6 = (BIO*)argv[2];
    osapiFree(L7_FLEX_SSLT_COMPONENT_ID, argv);
  }
  else
  {
    return;
  }

  port = ssltGetPortHelper(secureType);
  osapiSnprintf(host_port, sizeof(host_port), "%u", port);

  if ((acc = BIO_new_accept(host_port)) == L7_NULL)
  {
    LOG_MSG("SSLT: Error creating Secure server socket\n");
    ssltListenTaskCleanup(secureType);
    return;
  }

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
  {
    fprintf(stdout, "ssltListenTask - acc = %x\n", (L7_uint32)acc);
  }

  BIO_set_bind_mode(acc, BIO_BIND_REUSEADDR);

  if (SSLT_SECURE_ADMIN == secureType)
  {
    acc1 = acc;
  }
  else if (SSLT_SECURE_AUX_PORT1 == secureType)
  {
    acc2 = acc;
  }
  else if (SSLT_SECURE_AUX_PORT2 == secureType)
  {
    acc3 = acc;
  }

  /* First call sets up the BIO listening socket */
  if (BIO_do_accept(acc) <= L7_NULL)
  {
    LOG_MSG("SSLT: Error binding Secure server socket\n");
    ssltListenTaskCleanup(secureType);
    return;
  }
  if( (listen_s = BIO_get_fd(acc,0)) <=0 )
  {
    LOG_MSG("SSLT: Error reading sockFD\n");
    ssltListenTaskCleanup(secureType);
    return;
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)

  osapiSnprintf(host_port, sizeof(host_port), "IPVER=6.%u", port);

  if ((accV6 = BIO_new_accept(host_port)) == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Error creating Secure server socket6. "
													    "Failed to create secure server socket for IPV6.\n"); 
    ssltListenTaskCleanup(secureType);
    return;
  }

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
    fprintf(stdout, "ssltListenTask - accV6 = %x\n", (L7_uint32)accV6);

  BIO_set_bind_mode(accV6, BIO_BIND_REUSEADDR);

  if (SSLT_SECURE_ADMIN == secureType)
  {
    acc6_1 = accV6;
  }
  else if (SSLT_SECURE_AUX_PORT1 == secureType)
  {
    acc6_2 = accV6;
  }
  else if (SSLT_SECURE_AUX_PORT2 == secureType)
  {
    acc6_3 = accV6;
  }

  /* First call sets up the BIO listening socket */
  if (BIO_do_accept(accV6) <= L7_NULL)
  {
    LOG_MSG("SSLT: Error binding Secure server socket (ipv6)\n");
    ssltListenTaskCleanup(secureType);
    return;
  }

  if( (listen6_s = BIO_get_fd(accV6,0)) <=0 ){
    LOG_MSG("SSLT: Error reading sockFD-v6\n");
    ssltListenTaskCleanup(secureType);
    return;
  }

#endif

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
  {
    fprintf(stdout,"ssltListenTask - Task:%x listening on port:%s\n",osapiTaskIdSelf(),host_port);
  }

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
  {
    fprintf(stdout,"ssltListenTask init complete\n");
  }

  /*
  ** Loop waiting for SSLT connections
  */
  while (L7_TRUE)
  {
    fd_set readfds;
    struct timeval tmout;
    int num,max_fd;
    tmout.tv_sec = 3;
    tmout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(listen_s,&readfds);
    max_fd = listen_s;
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    FD_SET(listen6_s,&readfds);
    if(listen6_s > max_fd) max_fd = listen6_s;
#endif

    num = select(max_fd + 1, &readfds, NULL, NULL, &tmout);

    if (num <= 0)
    {
        /*
        Timeout
        */
        continue;
    }
    if (acc != L7_NULL && FD_ISSET(listen_s, &readfds))
    {
      ssltListenReady(acc, secureType);
    }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    if (accV6 != L7_NULL && FD_ISSET(listen6_s, &readfds))
    {
      ssltListenReady(accV6, secureType);
    }
#endif
  } /* while */

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTask exit\n");

  return;
}
#endif

#ifdef _L7_OS_LINUX_

/*********************************************************************
*
* @purpose Task to listen for incomming SSLT connections
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltListenTask(int argc, L7_uint32 *argv[])
{
  ssltSecureTypes_t secureType = SSLT_SECURE_ADMIN;
  BIO* acc = L7_NULL;
  BIO* accV6 = L7_NULL;
  BIO **ppBio = L7_NULL;
  L7_uchar8 host_port[SSLT_HOST_PORT_STR_LEN+10];
  int listen_s;
  L7_uint32 port;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTask \n");

  if ((argc==3) && (argv != L7_NULL))
  {
    secureType = (ssltSecureTypes_t)argv[0];
    acc = (BIO*)argv[1];
    accV6 = (BIO*)argv[2];
    osapiFree(L7_FLEX_SSLT_COMPONENT_ID, argv);
  }
  else
  {
    return;
  }
  port = ssltGetPortHelper(secureType);

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  acc1 = L7_NULLPTR;
  acc2 = L7_NULLPTR;
  acc3 = L7_NULLPTR;
  osapiSnprintf(host_port, sizeof(host_port), "IPVER=6.%u", port);
  if (SSLT_SECURE_ADMIN == secureType)
  {
    acc6_1 = accV6;
    ppBio = &acc6_1;
  }
  else if (SSLT_SECURE_AUX_PORT1 == secureType)
  {
    acc6_2 = accV6;
    ppBio = &acc6_2;
  }
  else if (SSLT_SECURE_AUX_PORT2 == secureType)
  {
    acc6_3 = accV6;
    ppBio = &acc6_3;
  }
#else
  osapiSnprintf(host_port, sizeof(host_port), "%u", port);
  if (SSLT_SECURE_ADMIN == secureType)
  {
    acc1 = acc;
    ppBio = &acc1;
  }
  else if (SSLT_SECURE_AUX_PORT1 == secureType)
  {
    acc2 = acc;
    ppBio = &acc2;
  }
  else if (SSLT_SECURE_AUX_PORT2 == secureType)
  {
    acc3 = acc;
    ppBio = &acc3;
  }
#endif

  if ((*ppBio = BIO_new_accept(host_port)) == L7_NULL)
  {
    LOG_MSG("SSLT: Error creating Secure server socket\n");
    ssltListenTaskCleanup(secureType);
    return;
  }

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
    fprintf(stdout, "ssltListenTask - ppBio = %x\n", (L7_uint32)*ppBio);

  BIO_set_bind_mode(*ppBio, BIO_BIND_REUSEADDR);

  /* First call sets up the BIO listening socket */
  if (BIO_do_accept(*ppBio) <= L7_NULL)
  {
    LOG_MSG("SSLT: Error binding Secure server socket\n");
    ssltListenTaskCleanup(secureType);
    return;
  }
  if( (listen_s = BIO_get_fd(*ppBio,0)) <=0 )
  {
    LOG_MSG("SSLT: Error reading sockFD\n");
    ssltListenTaskCleanup(secureType);
    return;
  }

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTask init complete\n");

  /*
  ** Loop waiting for SSLT connections
  */
  while (L7_TRUE)
  {
    fd_set readfds;
    struct timeval tmout;
    int num,max_fd;

    tmout.tv_sec = 3;
    tmout.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(listen_s,&readfds);
    max_fd = listen_s;

    num = select(max_fd + 1, &readfds, NULL, NULL, &tmout);

    if (num <= 0)
    {
        /*
        Timeout
        */
        continue;
    }
    if (FD_ISSET(listen_s, &readfds))
        ssltListenReady(*ppBio, secureType);
  } /* while */

  ssltGlobal.ssltListenTaskId = L7_NULL;
  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
    fprintf(stdout,"ssltListenTask exit\n");

  return;
}
#endif

/*********************************************************************
*
* @purpose Handle the exchange of data for the SSL Tunnel
*
* @param numArgs @b{(input)} number of arguments passed in
* @param ssl @b{(input)} pointer to the SSL connection object
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/

void ssltConnectionTask(int argc, L7_uint32 *argv[])
{
  ssltSecureTypes_t secureType = SSLT_SECURE_ADMIN;
  SSL *ssl = L7_NULL;
  L7_int32 sock_fd, sslRc;
  struct sockaddr_in unSecureServerAddr;
  L7_int32 ssl_rfd, result;
  fd_set read_fds;
  struct timeval timeout;
  L7_uint32 addr_len;
  struct sockaddr_in6 addr;
  sockaddr_union_t u_saddr;
  int numBytes;
  int fd;
  int rc;
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  L7_inet_addr_t remote;
  L7_uint32 port = 0;
  L7_uint32 intfId = 0;
#endif

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
  {
    fprintf(stdout,"ssltConnectionTask\n");
  }

  if ((argc==2) && (argv != L7_NULL))
  {
    secureType = (ssltSecureTypes_t)argv[0];
    ssl = (SSL*)argv[1];
    osapiFree(L7_FLEX_SSLT_COMPONENT_ID, argv);
  }
  else
  {
    return;
  }

  opensslSecureNonBlockingSet(ssl);

  /* Remove any previous errors */
  ERR_clear_error();
  /*
  ** Open the SSL connection
  */
  while ((sslRc = SSL_accept(ssl)) <= L7_NULL)
  {
    switch (SSL_get_error(ssl, sslRc))
    {
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
      {
        timeout.tv_sec  = 10;
        timeout.tv_usec = L7_NULL;

        ssl_rfd = SSL_get_rfd(ssl);

        FD_ZERO(&read_fds);
        FD_SET(ssl_rfd, &read_fds);

        result = select(ssl_rfd + 1, &read_fds, L7_NULL, L7_NULL, &timeout);

        if (result <= L7_NULL)
        {
          LOG_MSG("SSLT: Error on accepting the SSL connection!\n");

          (void)SSL_clear(ssl);

          ssltConnTaskCleanup(ssl);
          return;
        }
      }

      break;

    default:
      (void)SSL_clear(ssl);

      ssltConnTaskCleanup(ssl);

      return;
    } /* switch */
    /* Remove any previous errors */
    ERR_clear_error();
  } /* while */

  /*
  ** SSL connection is now opened, begin to open the unsecure connection
  */
  bzero((L7_uchar8 *)&unSecureServerAddr, sizeof(unSecureServerAddr));

  unSecureServerAddr.sin_family = AF_INET;
  unSecureServerAddr.sin_addr.s_addr = osapiHtonl(ssltGlobal.ssltUnSecureServerAddr);
  unSecureServerAddr.sin_port = osapiHtons(ssltGlobal.ssltUnSecurePort);

  if ((sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < L7_NULL)
  {
    LOG_MSG("SSLT: Error opening unsecure connection to %s\n",
            osapiInet_ntoa((L7_ulong32)ssltGlobal.ssltUnSecureServerAddr));
    ssltConnTaskCleanup(ssl);

    return;
  }

  if ((result = connect(sock_fd, (struct sockaddr *)&unSecureServerAddr,
                        sizeof(unSecureServerAddr))) < L7_NULL)
  {
	L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Can't connect to unsecure "
			"server at %s, result = %u, errno = %d. Failed to open connection to unsecure server. "
			"%s is the unsecure server socket address. %u is the result returned from connect function "
			"and %d is the error code.\n", osapiInet_ntoa((L7_ulong32)ssltGlobal.ssltUnSecureServerAddr), 
		   result, errno, osapiInet_ntoa((L7_ulong32)ssltGlobal.ssltUnSecureServerAddr), result, errno);

    osapiSocketClose(sock_fd);
    ssltConnTaskCleanup(ssl);

    return;
  }

  /* Get the File Descriptor association for secure client IP Address retrieval */
  BIO_get_fd(SSL_get_wbio(ssl), &fd);

  memset(&u_saddr, 0x00, sizeof(u_saddr));
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  memset(&remote,0,sizeof(L7_inet_addr_t));
#endif
  addr_len = sizeof(struct sockaddr_in6);
  rc = getpeername(fd, (struct sockaddr *)&addr, &addr_len);
  if (rc == 0)
  {
    if (addr.sin6_family == AF_INET6)
    {
      memcpy(&u_saddr.u.sa6, &addr, sizeof(struct sockaddr_in6));
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#ifdef _L7_OS_VXWORKS_
       remote.addr.ipv4.s_addr = u_saddr.u.sa6.sin6_addr.in6.addr32[3];
#else
       remote.addr.ipv4.s_addr = u_saddr.u.sa6.sin6_addr.in6_u.u6_addr32[3];
#endif
#endif
    }
    else
    {
      memcpy(&u_saddr.u.sa, &addr, sizeof(struct sockaddr_in));
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
      remote.addr.ipv4.s_addr = osapiNtohl(u_saddr.u.sa4.sin_addr.s_addr);
#endif
    }
    numBytes = write(sock_fd, (char *)&u_saddr, sizeof(u_saddr));

    if (numBytes < 0)
    {
      LOG_MSG("SSLT: Couldn't write peer to emWeb, %d %s\n", errno, strerror(errno));
      osapiSocketClose(sock_fd);
      ssltConnTaskCleanup(ssl);
      return;
    }
  }
  else
  {
    LOG_MSG("SSLT: Couldn't write peername to emWeb, getpeername call failed.");
    osapiSocketClose(sock_fd);
    ssltConnTaskCleanup(ssl);
    return;
  }

  memset(&u_saddr, 0x00, sizeof(u_saddr));
  addr_len = sizeof(struct sockaddr_in6);
  rc = getsockname(fd, (struct sockaddr *)&addr, &addr_len);
  if (rc == 0)
  {
    if (addr.sin6_family == AF_INET6)
    {
      memcpy(&u_saddr.u.sa6, &addr, sizeof(struct sockaddr_in6));
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
      port = u_saddr.u.sa6.sin6_port;
#endif
    }
    else
    {
      memcpy(&u_saddr.u.sa, &addr, sizeof(struct sockaddr_in));
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
      port = u_saddr.u.sa4.sin_port;
#endif
    }
    numBytes = write(sock_fd, (char *)&u_saddr, sizeof(u_saddr));
    if (numBytes < 0)
    {
      LOG_MSG("SSLT: Couldn't write sockname to emWeb, %d %s\n", errno, strerror(errno));
      osapiSocketClose(sock_fd);
      ssltConnTaskCleanup(ssl);
      return;
    }
  }
  else
  {
    LOG_MSG("SSLT: Couldn't write sockname to emWeb, getsockname call failed.");
    osapiSocketClose(sock_fd);
    ssltConnTaskCleanup(ssl);
    return;
  }


#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  /*
  ** Validate the connection. Therefore, if the client is not a CP candidate,
  ** then it must be using the switch admin port.
  */
  {
    L7_BOOL fAllow = L7_FALSE;

    if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
    {
      fprintf(stdout,"SSLT: ssltConnectionTask - connection request Ip:0x%x port:%d.\n",remote.addr.ipv4.s_addr,port);
    }
    if (L7_SUCCESS==usmDbCpcmAIPStatusIntfIdGet(remote.addr.ipv4.s_addr,&intfId))
    {
      if ((port==ssltGlobal.ssltAuxSecurePort1) ||
          (port==ssltGlobal.ssltAuxSecurePort2))
      {
        if (L7_SUCCESS==usmDbCpcmAIPStatusPortSet(remote.addr.ipv4.s_addr,port))
        {
          fAllow = L7_TRUE;
        }
      }
    }
    else
    {
      if (ssltGlobal.ssltSecurePort==port)
      {
        fAllow = L7_TRUE;
      }
    }

    if (L7_FALSE==fAllow)
    {
      if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
      {
        fprintf(stdout,"SSLT: ssltConnectionTask - connection not allowed.\n");
      }
      SSL_clear(ssl);
      osapiSocketClose(sock_fd);
      ssltConnTaskCleanup(ssl);
      return;
    }
  }
#endif

  if (ssltConnectionLoop(ssl, sock_fd, secureType))
  {
    /* Client has terminated the secure connection */
    SSL_shutdown(ssl);
  }
  else
  {
    /* Server has terminated the secure connectin */
    SSL_clear(ssl);
  }

  /* Close the unsecure connection */
  osapiSocketClose(sock_fd);

  ssltConnTaskCleanup(ssl);
  return;
}

/*********************************************************************
*
* @purpose Send the received message to the SSLT message queue.
*
* @param event @b{(input)} UI event
* @param data @b{(input)} pointer to the UI event data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltIssueCmd(L7_uint32 event,
                     void *data)
{
  L7_RC_t rc;
  ssltMsg_t msg;

  msg.event = event;

  if ((rc = ssltFillMsg(data, &msg)) == L7_SUCCESS)
  {
    /* Send msg */
    if ((rc = osapiMessageSend(ssltQueue,
                               &msg,
                               (L7_uint32)sizeof(ssltMsg_t), L7_NO_WAIT,
                               L7_MSG_PRIORITY_NORM)) != L7_SUCCESS)
    {
	  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Msg Queue is full, event=%u. "
				"Failed to send the received message to the SSLT message queue as message queue is "
				"full. %u indicates the event to be sent.\n", event, event);
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose Fill the SSLT message with the approiate data.
*
* @param data @b{(input)} pointer to the data
* @param msg @b{(input)} pointer to the SSLT message
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltFillMsg(void *data,
                    ssltMsg_t *msg)
{
  if (data == L7_NULLPTR)
  {
    LOG_MSG("SSLT: Invalid data received\n");
    return L7_FAILURE;
  }
  switch (msg->event)
  {
  /**************************************************/
  /* events originating from users, CLI, WEB, etc.. */
  /**************************************************/
  case SSLT_ADMIN_MODE_SET:
  case SSLT_AUX_MODE_SET:
  case SSLT_PROTOCOL_SSL30_SET:
  case SSLT_PROTOCOL_TLS10_SET:
    /* add to queue mode size */
    memcpy(&msg->data.mode, data, sizeof(L7_uint32));

    break;

  case SSLT_SECURE_PORT_SET:
  case SSLT_AUX_SECURE_PORT1_SET:
  case SSLT_AUX_SECURE_PORT2_SET:
  case SSLT_UNSECURE_PORT_SET:
  case SSLT_UNSECURE_SERVER_ADDR_SET:
  case SSLT_CERTIFICATE_ACTIVE_SET:
    /* add to queue number size */
    memcpy(&msg->data.number, data, sizeof(L7_uint32));

    break;

  case SSLT_PEM_PASS_PHRASE_SET:
    /* add to queue a string */
    osapiStrncpySafe((L7_uchar8 *)&msg->data.string, (L7_uchar8 *)data, sizeof(msg->data.string));

  case SSLT_CNFGR_EVENT:
    /* add to queue a configurator cmd structure */
    memcpy(&msg->data.CmdData, data, sizeof(L7_CNFGR_CMD_DATA_t));

    break;

  /*******************/
  /* unmatched event */
  /*******************/
  default:
    LOG_MSG("SSLT: Invalid event received\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Dispatch the received UI event to the appropriate sslt function.
*
* @param msg @b{(input)} sslt message struct
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltDispatchCmd(ssltMsg_t msg)
{
  L7_RC_t rc;

  switch (msg.event)
  {
  case SSLT_ADMIN_MODE_SET:
    rc = ssltEventAdminModeSet(msg.data.mode);

    break;

  case SSLT_SECURE_PORT_SET:
    rc = ssltEventSecurePortSet(msg.data.number);

    break;

  case SSLT_UNSECURE_PORT_SET:
    rc = ssltEventUnSecurePortSet(msg.data.number);

    break;

  case SSLT_UNSECURE_SERVER_ADDR_SET:
    rc = ssltEventUnSecureServerAddrSet(msg.data.number);

    break;

  case SSLT_PEM_PASS_PHRASE_SET:
    rc = ssltEventPemPassPhraseSet(msg.data.string);

    break;

  case SSLT_PROTOCOL_SSL30_SET:
    rc = ssltEventProtocolLevelSet(L7_SSLT_PROTOCOL_SSL30, msg.data.mode);

    break;

  case SSLT_PROTOCOL_TLS10_SET:
    rc = ssltEventProtocolLevelSet(L7_SSLT_PROTOCOL_TLS10, msg.data.mode);

    break;

  case SSLT_CNFGR_EVENT:
    ssltCnfgrParse(&(msg.data.CmdData));

    break;

  case SSLT_CERTIFICATE_ACTIVE_SET:
    ssltEventCertificateActiveSet(msg.data.number);
    break;

  case SSLT_AUX_MODE_SET:
      rc = ssltEventAuxModeSet(msg.data.mode);

    break;

  case SSLT_AUX_SECURE_PORT1_SET:
      rc = ssltEventSecureAuxPort1Set(msg.data.number);

    break;

  case SSLT_AUX_SECURE_PORT2_SET:
      rc = ssltEventSecureAuxPort2Set(msg.data.number);

    break;

  default:
	L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "SSLT: Unknown UI event in message, event=%u. "
				"Failed to dispatch the received UI event to the appropriate SSLT function as it’s an invalid"
				" event. %u indicates the event to be dispatched.\n", msg.event, msg.event);
  }

  return;
}

/*********************************************************************
*
* @purpose Process the Admin Mode setting event
*
* @param mode @b{(input)} enable or disable the admin mode
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventAdminModeSet(L7_uint32 mode)
{
  L7_RC_t rc = L7_FAILURE;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
  {
    fprintf(stdout,"ssltEventAdminModeSet entered.\n");
  }

  if (mode != ssltGlobal.ssltOperMode)
  {
    if (mode == L7_ENABLE)
    {
      if (ssltGlobal.ssltSecurePort != 0)
      {
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltEventAdminModeSet starting up ssltSecurePort.\n");
        }
        if ((rc = ssltListenTaskStartup(SSLT_SECURE_ADMIN)) != L7_SUCCESS)
        {
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
          {
            fprintf(stdout,"ssltEventAdminModeSet listening port %d failed to start.\n",ssltGlobal.ssltSecurePort);
          }
          return L7_FAILURE;
        }
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltEventAdminModeSet listening port %d started.\n",ssltGlobal.ssltSecurePort);
        }
      }
      ssltGlobal.ssltAdminMode = L7_ENABLE;
    }
    else
    {
      /* Terminate the SSLT Listen Task
      **
      ** Note: This will only prevent new SSLT connections from
      ** being created. It will not kill any SSLT connection
      ** which are already communicating. They will have to
      ** terminate on their own.
      */
      ssltGlobal.ssltAdminMode = L7_DISABLE;
      if (ssltGlobal.ssltSecurePort!=ssltGlobal.ssltAuxSecurePort1)
      {
        ssltListenTaskCleanup(SSLT_SECURE_ADMIN);
      }
      else
      {
        ssltCheckAndListenTaskCleanup();
      }
    }
  }
  else if (mode == L7_DISABLE)
  {
    ssltGlobal.ssltAdminMode = L7_DISABLE;
    ssltCheckAndListenTaskCleanup();
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Process the Auxilary Mode setting event
*
* @param mode @b{(input)} enable or disable the auxilary ports
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventAuxModeSet(L7_uint32 mode)
{
  L7_BOOL port1Started = L7_FALSE;
  L7_BOOL port2Started = L7_FALSE;
  L7_RC_t rc;

  if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL1)
  {
    fprintf(stdout,"ssltEventAuxModeSet entered.\n");
  }
  if (mode != ssltGlobal.ssltAuxOperMode)
  {
    if (mode == L7_ENABLE)
    {
      /* Startup first auxilary port if its not already running */
      if ((ssltGlobal.ssltAuxSecurePort1 == ssltGlobal.ssltSecurePort) &&
          (ssltGlobal.ssltOperMode == L7_TRUE))
      {
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltEventAuxModeSet already listening on port %d.\n",ssltGlobal.ssltAuxSecurePort1);
        }
        port1Started = L7_TRUE;
      }
      else
      {
        if ((rc = ssltListenTaskStartup(SSLT_SECURE_AUX_PORT1)) != L7_SUCCESS)
        {
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
          {
            fprintf(stdout,"ssltEventAuxModeSet listening port %d failed to start.\n",ssltGlobal.ssltAuxSecurePort1);
          }
          return L7_FAILURE;
        }
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltEventAuxModeSet listening port %d started.\n",ssltGlobal.ssltAuxSecurePort1);
        }
        port1Started = L7_TRUE;
      }

      /* Startup second auxilary port if its configured and unique */
      if ((ssltGlobal.ssltAuxSecurePort2 != 0) &&
          (ssltGlobal.ssltAuxSecurePort2 != ssltGlobal.ssltSecurePort) &&
          (ssltGlobal.ssltAuxSecurePort2 != ssltGlobal.ssltAuxSecurePort1))
      {
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltEventAuxModeSet starting up ssltAuxSecurePort2 on port %d.\n",ssltGlobal.ssltAuxSecurePort2);
        }
        if ((rc = ssltListenTaskStartup(SSLT_SECURE_AUX_PORT2)) != L7_SUCCESS)
        {
          if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL3)
          {
            fprintf(stdout,"ssltEventAuxModeSet listening port %d failed to start.\n",ssltGlobal.ssltAuxSecurePort2);
          }
          return L7_FAILURE;
        }
        if (ssltGlobal.ssltDebugLevel >= SSLT_DEBUG_LVL2)
        {
          fprintf(stdout,"ssltEventAuxModeSet listening port %d started.\n",ssltGlobal.ssltAuxSecurePort2);
        }
        port2Started = L7_TRUE;
      }

      if ((L7_TRUE == port1Started) || (L7_TRUE == port2Started))
      {
        ssltGlobal.ssltAuxOperMode = L7_ENABLE;
      }
    }
    else
    {
      ssltGlobal.ssltAuxOperMode = L7_DISABLE;
      ssltCheckAndListenTaskCleanup();
    }

    /* Terminate the SSLT Listen Task
    **
    ** Note: This will only prevent new SSLT connections from
    ** being created. It will not kill any SSLT connection
    ** which are already communicating. They will have to
    ** terminate on their own.
    */
    if (L7_TRUE != port1Started)
    {
      if ((ssltGlobal.ssltAuxSecurePort1==ssltGlobal.ssltSecurePort) && (ssltGlobal.ssltAdminMode==L7_ENABLE))
      {
        /* Do nothing because ADMIN is using this port */
      }
      else
      {
        ssltListenTaskCleanup(SSLT_SECURE_AUX_PORT1);
      }
    }

    if (L7_TRUE != port2Started)
    {
      ssltListenTaskCleanup(SSLT_SECURE_AUX_PORT2);
    }

    if ((mode == L7_DISABLE) ||
        ((L7_TRUE != port1Started) && (L7_TRUE != port2Started)))
    {
      ssltGlobal.ssltAuxOperMode = L7_DISABLE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Process the Secure Port setting event.
*
* @param number @b{(input)} host port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventSecurePortSet(L7_uint32 number)
{
  L7_RC_t rc = L7_SUCCESS;

  if (number != ssltGlobal.ssltSecurePort)
  {
    ssltGlobal.ssltSecurePort = number;
    rc = ssltListenTaskRefresh(SSLT_SECURE_ADMIN);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Process the first auxiliary port setting event.
*
* @param number @b{(input)} host port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventSecureAuxPort1Set(L7_uint32 number)
{
  L7_RC_t rc = L7_SUCCESS;

  if (number != ssltGlobal.ssltAuxSecurePort1)
  {
    ssltGlobal.ssltAuxSecurePort1 = number;
    rc = ssltListenTaskRefresh(SSLT_SECURE_AUX_PORT1);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Process the second auxiliary port setting event.
*
* @param number @b{(input)} host port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventSecureAuxPort2Set(L7_uint32 number)
{
  L7_RC_t rc = L7_SUCCESS;

  if (number != ssltGlobal.ssltAuxSecurePort2)
  {
    ssltGlobal.ssltAuxSecurePort2 = number;
    rc = ssltListenTaskRefresh(SSLT_SECURE_AUX_PORT2);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Process the UnSecure Port setting event.
*
* @param number @b{(input)} port number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventUnSecurePortSet(L7_uint32 number)
{
  L7_RC_t rc;

  rc=L7_FAILURE;
  if (number != ssltGlobal.ssltUnSecurePort)
  {
    /* Future connection to the unsecure host will use this port */
    ssltGlobal.ssltUnSecurePort = number;
    rc = ssltListenTaskRefresh(SSLT_SECURE_ADMIN);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Process the UnSecure Server Addr setting event.
*
* @param number @b{(input)} Server IP address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventUnSecureServerAddrSet(L7_uint32 number)
{
  if (number != ssltGlobal.ssltUnSecureServerAddr)
  {
    /* Future connection to the unsecure host will use this IP */
    ssltGlobal.ssltUnSecureServerAddr = number;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the Pass Phrase string used to access the PEM-encoded
*          (Privacy Enhanced Mail) certificates and key files required
*          for the SSLT connections.
*
* @param string @b{(input)} pointer to the new PEM Pass Phrase string
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventPemPassPhraseSet(L7_uchar8 *string)
{
  L7_RC_t rc = L7_SUCCESS;

  if (string == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if (strcmp(string, ssltGlobal.ssltPemPassPhrase) != L7_NULL)
  {
    /* Future access to the PEM-encoded certificate and key filles will
    ** use this pass phrase.
    */
    osapiStrncpySafe(ssltGlobal.ssltPemPassPhrase, string, sizeof(ssltGlobal.ssltPemPassPhrase));

    if (L7_SUCCESS != ssltListenTaskRefresh(SSLT_SECURE_ADMIN))
    {
      rc = L7_FAILURE;
    }
    if (L7_SUCCESS != ssltListenTaskRefresh(SSLT_SECURE_AUX_PORT1))
    {
      rc = L7_FAILURE;
    }
    if (L7_SUCCESS != ssltListenTaskRefresh(SSLT_SECURE_AUX_PORT2))
    {
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose Specify the use of an SSL protocol by the SSL Tunnel code
*
* @param protocolId @b{(input)} pointer to the new PEM Pass Phrase string
* @param mode @b{(input)} enable/disable a specified SSLT supported protocol
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventProtocolLevelSet(L7_uint32 protocolId,
                                  L7_uint32 mode)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Enable the specified SSL protocol */
  if (mode == L7_ENABLE)
  {
    if (protocolId == L7_SSLT_PROTOCOL_SSL30)
    {
      /* Turn on SSL version 3.0 */
      ssltGlobal.ssltProtocolLevel &= ~SSL_OP_NO_SSLv3;
    }
    else if (protocolId == L7_SSLT_PROTOCOL_TLS10)
    {
      /* Turn on TLS version 1.0 */
      ssltGlobal.ssltProtocolLevel &= ~SSL_OP_NO_TLSv1;
    }
  }
  /* Disable the specified SSL protocol */
  else if (mode == L7_DISABLE)
  {
    if (protocolId == L7_SSLT_PROTOCOL_SSL30)
    {
      /* Turn off SSL version 3.0 */
      ssltGlobal.ssltProtocolLevel |= SSL_OP_NO_SSLv3;
    }
    else if (protocolId == L7_SSLT_PROTOCOL_TLS10)
    {
      /* Turn off TLS version 1.0 */
      ssltGlobal.ssltProtocolLevel |= SSL_OP_NO_TLSv1;
    }
  }

  if (L7_SUCCESS != ssltListenTaskRefresh(SSLT_SECURE_ADMIN))
  {
    rc = L7_FAILURE;
  }
  if (L7_SUCCESS != ssltListenTaskRefresh(SSLT_SECURE_AUX_PORT1))
  {
    rc = L7_FAILURE;
  }
  if (L7_SUCCESS != ssltListenTaskRefresh(SSLT_SECURE_AUX_PORT2))
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Change active certificate for SSL tunnel code
*
* @param  L7_uint32 number @b{(input)} certificate number
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments    
*
* @end
*
*********************************************************************/
L7_RC_t ssltEventCertificateActiveSet(L7_uint32 number)
{
  ssltGlobal.ssltCertificateNumber = number;
  return ssltListenTaskRefresh(SSLT_SECURE_ADMIN);
}


