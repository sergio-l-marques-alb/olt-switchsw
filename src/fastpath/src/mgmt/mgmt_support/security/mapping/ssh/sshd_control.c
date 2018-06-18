/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     sshd_control.c
*
* @purpose      SSH Control functions
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

#include <stdio.h>

#include "sysapi.h"

#include "sshd_include.h"

#include "sshd_util.h"
#include "osapi.h"
#include "sshd_exports.h"

extern sshdGlobal_t sshdGlobal;
extern sshdCfg_t    *sshdCfg;
extern void         *sshdGlobalSema;

void                *sshdTaskSyncSema;
void                *sshdStartupSema;
void                *sshdQueue;
char                *sshd_argv[1];
L7_uint32           sshdEvTaskId;
int                 sshd_exited = 1;

extern void         sshd_set_global_protolevel(int protoLevel);
extern int          sshd_main(int argc, char *argv[]);
extern void         sshd_doquit(int reason);
extern void         sshd_undoquit(void);


/*********************************************************************
*
* @purpose  Start the main task required by the SSL Tunnel code
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
sshdStartTasks(void)
{
    /* Semaphore creation for task protection */

    sshdTaskSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

    if (sshdTaskSyncSema == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the task semaphore. "
                                      "Failed to create semaphore for synchronization of SSHD startup.\n");
        return L7_FAILURE;
    }

    sshdStartupSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

    if (sshdStartupSema == L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the startup semaphore. "
                                  "Failed to create semaphore for protection of SSHD listen task startup.\n");
        return L7_FAILURE;
    }

    /*
    Make sure the global structure is originally set to 0
    since subsequent reconfigs depend on some of the data
    */

    bzero((L7_char8 *)&sshdGlobal, (L7_int32)sizeof(sshdGlobal));

    /* Create the sshdTask */

    sshdEvTaskId = (L7_uint32)osapiTaskCreate("sshdEvTask", (void *)sshdTask,
                                              L7_NULL, L7_NULL,
                                              sshdSidDefaultStackSize(),
                                              sshdSidDefaultTaskPriority(),
                                              sshdSidDefaultTaskSlice());

    if (sshdEvTaskId == L7_ERROR)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Failed to create the sshd task. "
                                             "osapiTaskCreate was unable to create the SSHD task.\n");
        return L7_FAILURE;
    }

    if (osapiWaitForTaskInit(L7_SSHD_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to initialize the sshd task. "
                  "osapiWaitForTaskInit returned an error while waiting for SSHD task initialization.\n");
        return L7_FAILURE;
    }

    return L7_SUCCESS;

} /* sshdStartTasks */


/*********************************************************************
*
* @purpose  The main SSH control Task.
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

void
sshdTask(void)
{
    sshdMsg_t   msg;
    L7_uint32   status;

    (void)osapiTaskInitDone(L7_SSHD_TASK_SYNC);

    /*
    Loop forever waiting for SSHD messages
    */

    while (L7_TRUE)
    {
        status = osapiMessageReceive(sshdQueue, (void *)&msg,
                                     (L7_uint32)sizeof(sshdMsg_t),
                                     L7_WAIT_FOREVER);

        if ((status == L7_SUCCESS) &&
            (osapiSemaTake(sshdTaskSyncSema, L7_WAIT_FOREVER)
             == L7_SUCCESS))
        {
            /*
            Process the message
            */

            sshdDispatchCmd(msg);

            (void)osapiSemaGive(sshdTaskSyncSema);
        }
        else if (status != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "osapiMessageReceive failed, rc = %d."
                                " osapiMessageReceive failed while waiting for a message on the sshd control "
                                "task.\n", status);
        }
    }

    return;

} /* sshdTask */


/*********************************************************************
*
* @purpose  Startup the SSHD Listen Task
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
sshdListenTaskStartup(void)
{
    L7_uint32   taskId;
    L7_uchar8   taskName[SSHD_LISTEN_TASK_NAME_LEN];

    /*
    Note: the sshdGlobalSema should be taken before entering this
    function
    */

    osapiSemaTake(sshdStartupSema, L7_WAIT_FOREVER);
    if (sshdGlobal.sshdListenTaskId == L7_NULL)
    {
        /* Create a task to listen on the secure port */

        sshd_exited = 0;
        sshd_undoquit();
        osapiSnprintf(taskName,sizeof("sshd"), "%s", "sshd" );
        sshd_argv[0] = "sshd"; /* doesn't matter */
        taskId = (L7_uint32)osapiTaskCreate(taskName, (void *)sshd_main,
                                            1, sshd_argv,
                                            L7_DEFAULT_STACK_SIZE,
                                            L7_DEFAULT_TASK_PRIORITY,
                                            L7_DEFAULT_TASK_SLICE);

        if (taskId == L7_ERROR)
        {
            sshd_exited = 1;
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Failed to create the sshdListenTask, "
                                      "exited = %d. The SSHD listen task could not be created.\n", sshd_exited);

            osapiSemaGive(sshdStartupSema);
            return L7_FAILURE;
        }

        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdListenTask 0x%x started, exited = %d. "
                                    "The SSHD listen task was properly started, also printing the value of the "
                                    "‘exited’ flag.\n", taskId, sshd_exited);

        /* Store this task id for use when the user wants to disable SSHD */

        sshdGlobal.sshdListenTaskId = taskId;

        /* Operationally SSHD is enabled */

        sshdGlobal.sshdOperMode = L7_ENABLE;
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdListenTask 0x%x already running, "
                          "exited = %d. The SSHD listen task was already running, also printing the value "
                          "of the ‘exited’ flag.\n", sshdGlobal.sshdListenTaskId, sshd_exited);
        osapiSemaGive(sshdStartupSema);
        return L7_FAILURE;
    }

    osapiSemaGive(sshdStartupSema);
    return L7_SUCCESS;

} /* sshdListenTaskStartup */


/*********************************************************************
*
* @purpose  Cleanup all resources associated with the SSHD Listen Task
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
sshdListenTaskCleanup(void)
{
    /*
    Note: the sshdGlobalSema should be taken before entering this
    function
    */

    /* Operationally SSHD is disabled */

    osapiSemaTake(sshdStartupSema, L7_WAIT_FOREVER);
    sshdGlobal.sshdOperMode = L7_DISABLE;

    if (sshdGlobal.sshdListenTaskId != L7_NULL)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "deleting sshdListenTask 0x%x, exited = %d."
                        " SSHD is being shut down, so the listen task is being deleted, also printing the value"
                        " of the ‘exited’ flag.\n", sshdGlobal.sshdListenTaskId, sshd_exited);

        /* Remove the sshdListenTask from the system */

        sshd_doquit(0);
        osapiSemaGive(sshdStartupSema);
        osapiSemaGive(sshdGlobalSema);
        while (sshd_exited != 1)
        {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID, "exited = %d\n", sshd_exited);
            osapiSleep(1);
        }
        osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
        osapiSemaTake(sshdStartupSema, L7_WAIT_FOREVER);

        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdListenTask 0x%x deleted, exited = %d."
                                " The SSHD listen task was properly exited, also printing the value of the "
                                "‘exited’ flag.\n", sshdGlobal.sshdListenTaskId, sshd_exited);

        /*
        osapiTaskDelete(sshdGlobal.sshdListenTaskId);
        */

        sshdGlobal.sshdListenTaskId = L7_NULL;
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdListenTask not running, can't delete. The "
                    "SSHD shut down process was unable to exit the SSHD listen task, because it was not running.\n");
        osapiSemaGive(sshdStartupSema);
        return L7_FAILURE;
    }

    osapiSemaGive(sshdStartupSema);
    return L7_SUCCESS;

} /* sshdListenTaskCleanup */


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
L7_RC_t sshdListenTaskRefresh(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (sshdGlobal.sshdAdminMode == L7_ENABLE)
  {
    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
    /* Terminate the SSHD Listen Task */
    sshdListenTaskCleanup();
    osapiSemaGive(sshdGlobalSema);
    /* must give up the semaphore so that the cleanup completes before the task starts up again */
    osapiSleep(1);

    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
    /* Start the SSHD Listen Task back up with the modified configuration */
    rc = sshdListenTaskStartup();
    osapiSemaGive(sshdGlobalSema);
  }

  return rc;
}

static void sshdEventKeyGenTask(L7_uint32 numargs, L7_uint32 *argv )
{
  sshdKeyType_t keyType = argv[0];
  L7_uint32 length = argv[1];

  if (sshdKeyFileCreate(keyType, length) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdKeyFileCreate failed. The SSHD key "
                                                              "file(s) were not able to be created.\n");
  }
  else
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Key file created. The SSHD key file(s)i"
                                                                " were properly created.\n");
  }

  /* Cannot take semaphore here because it will block the completion of sshdListenTaskCleanup */
  if (sshdListenTaskRefresh() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "SSHD: sshdListenTaskRefresh failed. The "
                           "SSHD listen task could not be refreshed (stopped and restarted) properly.\n");
  }

  /* clear the key generation flag, can get by without the semaphore, since
       we are not reading the flag value, only setting */
  sshdGlobal.sshdKeyGenerateFlags &= ~((L7_uchar8)(1 << keyType));
}

static  L7_uint32 keyGenArgv[2];
static L7_RC_t sshdEventKeyGenerate(sshdKeyType_t keyType,
                                    L7_uint32     length)
{
  L7_uint32           keyGenTaskId;

  keyGenArgv[0] = keyType;
  keyGenArgv[1] = length;

  keyGenTaskId = (L7_uint32)osapiTaskCreate("sshdEventKeyGenTask", (void *)sshdEventKeyGenTask,
                                            2, keyGenArgv,
                                            sshdSidDefaultStackSize(),
                                            sshdSidDefaultTaskPriority(),
                                            sshdSidDefaultTaskSlice());

  if (keyGenTaskId == L7_ERROR)
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Failed to create the sshdEventKeyGenTask task."
                                             " The task for generation of the SSHD keys could not be started.\n");
      return L7_FAILURE;
  }
  else
  {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Created the sshdEventKeyGenTask task. The "
                                              "task for generation of the SSHD keys was properly started.\n");
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Send the received message to the SSHD message queue.
*
* @param    event @b{(input)} UI event
* @param    data @b{(input)} pointer to the UI event data
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
sshdIssueCmd(L7_uint32 event, void *data)
{
    L7_RC_t rc;
    sshdMsg_t msg;

    msg.event = event;

    if ((rc = sshdFillMsg(data, &msg)) == L7_SUCCESS)
    {
        /* Send msg */

        if ((rc = osapiMessageSend(sshdQueue, &msg,
                                   (L7_uint32)sizeof(sshdMsg_t),
                                   L7_NO_WAIT, L7_MSG_PRIORITY_NORM))
            != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Msg Queue is full, event = %u. "
                                "Failed to send the message to the SSHD message queue as message queue "
                                "is full. %u indicates the event to be sent.\n", event, event);
        }
    }

    return rc;

} /* sshdIssueCmd */


/*********************************************************************
*
* @purpose  Fill the SSHD message with the appropriate data.
*
* @param    data @b{(input)} pointer to the data
* @param    msg @b{(input)} pointer to the SSHD message
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
sshdFillMsg(void *data, sshdMsg_t *msg)
{
    /*
    Events originating from users, CLI, WEB, etc..
    */

    switch (msg->event)
    {
        case SSHD_ADMIN_MODE_SET:
            /* Add to queue mode size */

            memcpy(&msg->data.mode, data, sizeof(L7_uint32));
            break;

        case SSHD_PROTO_LEVEL_SET:
            /* Add to queue number size */

            memcpy(&msg->data.number, data, sizeof(L7_uint32));
            break;

        case SSHD_PORT_NUMBER_SET:
            /* copy new port number */
            /* passthru */
        case SSHD_KEY_GENERATE_EVENT:
            /* copy key type */

            memcpy(&msg->data.number, data, sizeof(L7_uint32));
            break;

        case SSHD_CNFGR_EVENT:
            /* Add to queue a configurator cmd structure */

            memcpy(&msg->data.CmdData, data, sizeof(L7_CNFGR_CMD_DATA_t));
            break;

        default:
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Invalid event received. An event"
                                  " type that is invalid for SSHD was received on the SSHD event queue.\n");
            return L7_FAILURE;
            break;
    }

    return L7_SUCCESS;

} /* sshdFillMsg */


/*********************************************************************
*
* @purpose  Dispatch the received UI event to the appropriate sshd function.
*
* @param    msg @b{(input)} sshd message struct
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

void
sshdDispatchCmd(sshdMsg_t msg)
{
    L7_RC_t rc;

    switch (msg.event)
    {
        case SSHD_ADMIN_MODE_SET:
            osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
            rc = sshdEventAdminModeSet(msg.data.mode);
            if (rc != L7_SUCCESS)
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdEventAdminModeSet failed, event=%u\n", msg.event);
            }
            else
            {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID, "sshdEventAdminModeSet success, event=%u\n", msg.event);
            }
            osapiSemaGive(sshdGlobalSema);
            break;

        case SSHD_PROTO_LEVEL_SET:
            osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
            rc = sshdEventProtoLevelSet(msg.data.number);
            if (rc != L7_SUCCESS)
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdEventProtoLevelSet failed, event=%u\n", msg.event);
            }
            else
            {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID, "sshdEventProtoLevelSet success, event=%u\n", msg.event);
            }
            osapiSemaGive(sshdGlobalSema);
            sshdListenTaskRefresh();
            break;

    case SSHD_PORT_NUMBER_SET:
          osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
          rc = sshdEventPortNumberSet(msg.data.number);
          if (rc != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID,
                    "SSHD: sshdEventPortNumberSet failed, event=%u\n", msg.event);
          }
          osapiSemaGive(sshdGlobalSema);
          /* Cannot take semaphore here because it will block the completion of sshdListenTaskCleanup */
          rc = sshdListenTaskRefresh();
          if (rc != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID,
                    "SSHD: sshdListenTaskRefresh failed, event=%u\n", msg.event);
          }
          break;

    case SSHD_KEY_GENERATE_EVENT:

            rc = sshdEventKeyGenerate(msg.data.number, L7_SSHD_KEY_LENGTH);
            /* generate the persistent key file based on key type */
            /* the key generation can take awhile, currently the key generation
               is only invoked via the CLI which waits on completion by checking
               the status, it would really be better if the CLI just displayed
               a status and did not loop waiting for completion, if that is ever
               changed, then sshdKeyFileCreate should spawn a new task and exit
               after generating the keys so we don't block other msg handling
               on this task... */
            if (rc != L7_SUCCESS)
            {
                L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_SSHD_COMPONENT_ID, "sshdEventKeyGenerate failed, event=%u\n", msg.event);
            }
            else
            {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID, "sshdEventKeyGenerate success, event=%u\n", msg.event);
            }
            break;

        case SSHD_CNFGR_EVENT:
            /*
            Process cnfgr events without semaphore control.
            The semaphore will not be created until phase 1.
            */

            sshdCnfgrParse(&(msg.data.CmdData));
            break;

        default:
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unknown UI event in message, event=%u. "
                          "Failed to dispatch the UI event to the appropriate SSHD function as it’s an invalid "
                          "event. %u indicates the event to be dispatched.\n", msg.event, msg.event);
            break;
    }

    return;

} /* sshdDispatchCmd */


/*********************************************************************
*
* @purpose  Process the Admin Mode setting event
*
* @param    mode @b{(input)} enable or disable the admin mode
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
sshdEventAdminModeSet(L7_uint32 mode)
{
    L7_RC_t rc;

    if (mode != sshdGlobal.sshdOperMode)
    {
        if (mode == L7_ENABLE)
        {
            L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_SSHD_COMPONENT_ID, "Admin mode enable sshdListenTask");
            if ((rc = sshdListenTaskStartup()) != L7_SUCCESS)
            {
                return L7_FAILURE;
            }

            sshdGlobal.sshdAdminMode = L7_ENABLE;

        }
        else if (mode == L7_DISABLE)
        {
            /*
            Terminate the SSHD Listen Task

            Note: This will only prevent new SSHD connections from
            being created. It will not kill any SSHD connection
            which are already communicating. They will have to
            terminate on their own.
            */

            L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_SSHD_COMPONENT_ID, "Admin mode disable sshdListenTask");

            if ((rc = sshdListenTaskCleanup()) != L7_SUCCESS)
            {
                return L7_FAILURE;
            }

            sshdGlobal.sshdAdminMode = L7_DISABLE;
        }
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_SSHD_COMPONENT_ID, "mode %d unchanged", mode);
    }

    return L7_SUCCESS;

} /* sshdEventAdminModeSet */


/*********************************************************************
*
* @purpose  Process the Proto Level setting event.
*
* @param    protoLevel @b{(input)} protocol level
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
sshdEventProtoLevelSet(L7_uint32 protoLevel)
{
    L7_RC_t rc = L7_SUCCESS;

    if (protoLevel != sshdGlobal.sshdProtoLevel)
    {
        sshdGlobal.sshdProtoLevel = protoLevel;
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_SSHD_COMPONENT_ID, "protoLevel changed to %d", protoLevel);
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_SSHD_COMPONENT_ID, "protoLevel %d unchanged", protoLevel);
    }

    return rc;

} /* sshdEventProtoLevelSet */

/*********************************************************************
*
* @purpose  Process a port number change event
*
* @param    L7_uint32 port @b{(input)} port number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t sshdEventPortNumberSet(L7_uint32 port)
{

  if (port != sshdGlobal.sshdPort)
  {
    sshdGlobal.sshdPort = port;
  }

  return L7_SUCCESS;

} /* sshdEventPortNumberSet */


/*
The following 2 functions are only for use from devshell (debug)
*/

void
sshdShowGlobals(void)
{
    printf("\nsshdAdminMode:    0x%08x\n", sshdGlobal.sshdAdminMode);
    printf("sshdOperMode:     0x%08x\n", sshdGlobal.sshdOperMode);
    printf("sshdListenTaskId: 0x%08x\n", sshdGlobal.sshdListenTaskId);
    printf("sshdProtoLevel:   0x%08x\n", sshdGlobal.sshdProtoLevel);
    printf("sshdNumSessions:  0x%08x\n", sshdGlobal.sshdNumSessions);
    printf("sshdMaxSessions:  0x%08x\n", sshdGlobal.sshdMaxSessions);
    printf("sshdIdleTimeout:  0x%08x\n\n", sshdGlobal.sshdIdleTimeout);

    return;

} /* sshdShowGlobals */


void
sshdShowCfg(void)
{
    printf("\nsshdAdminMode:    0x%08x\n", sshdCfg->cfg.sshdAdminMode);
    printf("sshdProtoLevel:   0x%08x\n", sshdCfg->cfg.sshdProtoLevel);
    printf("sshdMaxSessions:  0x%08x\n", sshdCfg->cfg.sshdMaxSessions);
    printf("sshdIdleTimeout:  0x%08x\n", sshdCfg->cfg.sshdIdleTimeout);
    printf("sshdDataChanged:  0x%08x\n\n", sshdCfg->hdr.dataChanged);

    return;

} /* sshdShowCfg */


/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on sshdBuildDefaultConfigData.
*
* @end
*********************************************************************/

void
sshdMgrBuildTestConfigData(void)
{
    /*
    Build Non-Default Config Data
    */

    sshdCfg->cfg.sshdAdminMode = L7_ENABLE;
    sshdCfg->cfg.sshdProtoLevel = FD_SSHD_PROTO_LEVEL_V1_ONLY;
    sshdCfg->cfg.sshdMaxSessions = 2;
    sshdCfg->cfg.sshdIdleTimeout = 20;

    /* End of Component's Test Non-default configuration Data */

    /* Force write of config file */

    sshdCfg->hdr.dataChanged = L7_TRUE;
    sysapiPrintf("Built test config data\n");

    return;

} /* sshdMgrBuildTestConfigData */


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
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

void
sshdConfigDataShow(void)
{
    L7_fileHdr_t  *pFileHdr;
    L7_uint32     i;

    sysapiPrintf("\nssh config values: \n\n");

    /*
    Config File Header Contents
    */

    pFileHdr = &sshdCfg->hdr;

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

    /*
    cfgParms
    */

    sshdShowCfg();

    /*
    Checksum
    */

    sysapiPrintf("sshdCfg->checkSum : %u\n", sshdCfg->checkSum);

    return;

} /* sshdConfigDataShow */


void
sshdConfigDataTestShow(void)
{
    sshdConfigDataShow();

    return;

} /* sshdConfigDataTestShow */
