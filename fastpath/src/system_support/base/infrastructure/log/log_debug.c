/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    log_debug.c
*
* @purpose     Debug functions for logging
*
* @component   hardware
*
* @create      10/15/04
*
* @author      Wynona Jacobs
*
* @end
*
*********************************************************************/

#include <stdarg.h>
#include "l7_common.h"
#include "usmdb_unitmgr_api.h"
#include "l7_resources.h"
#include "log.h"
#include "log_api.h"
#include "string.h"
#include "stdio.h"
#include "log.h"
#include "osapi.h"
#include "log_cfg.h"



/* Pointer to the configuration data. */
static struct logCfg_s * logCfg = L7_NULL;


/*********************************************************************
 * * @purpose  Initialize the log debug.
 * *
 * * @param    cfg @b{(input)} pointer to the config data.
 * * @param    ops @b{(input)} pointer to the operational data.
 * *
 * *
 * * @returns
 * * @returns
 * *
 * * @notes
 * *
 * * @end
 * *********************************************************************/

void logDebugInit(struct logCfg_s * cfg, struct logOps_s * ops)
{
  if ((cfg == L7_NULL) || (ops == L7_NULL))
          return;
  logCfg = cfg;
  /*logOps = ops;*/
}


/**********************************************************************
* @purpose  Add an entry to the event log
*
* @param    event_code @b{(input)} event code to add
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void logDebugEventLogAdd(L7_uint32 event_code)
{
    L7_LOG_EVENT(event_code);
}




/**********************************************************************
* @purpose  Add multiple entries to the event log
*
* @param    event_code @b{(input)} event code to add
* @param    numEntries @b{(input)} number of entries to add
*
* @returns  void
*
* @notes
*
*
* @end
*********************************************************************/
void logDebugEventLogAddN(L7_uint32 event_code, L7_uint32 numEntries)
{
    L7_uint32 i;

    for (i=1; i <= numEntries; i++)
    {
        L7_LOG_EVENT(event_code);
    }
}


/**********************************************************************
* @purpose  Add a message to the log
*
* @param    numEntries @b{(input)} number of entries to add
*
* @returns  void
*
* @notes    This tests uses L7_LOGF only, as this is the most commonly used
*           log macro.
*
*
* @end
*********************************************************************/
void logDebugMsgLogAddN( L7_uint32 numEntries)
{
    L7_uint32 i;

    for (i=1; i <= numEntries; i++)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_ID,
                "LOG number %d - Message test start                           message test end"
                , i);
    }
}

/**********************************************************************
* @purpose  Add a message to the log
*
* @param    void
*
* @returns  void
*
* @notes    This tests uses L7_LOGF only, as this is the most commonly used
*           log macro.
*
*
* @end
*********************************************************************/
void logDebugMsgLogAdd( void)
{
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_ID,
            "LOG - Message test start                                                "
            "                              message test end");
}


/**
*
* SECTION: Unit tests
*
*
*
*/

void logUnitTest()
{
  L7_uint32 ndx;
  /*logCfgDefaultsSet(L7_LOG_CFG_VER_1);*/
  logHostTableSeveritySet(1, L7_LOG_SEVERITY_DEBUG);
  logHostTableSeveritySet(2, L7_LOG_SEVERITY_DEBUG);
  logHostAdd("192.168.21.50", L7_IP_ADDRESS_TYPE_IPV4, &ndx);
  logHostAdd("192.168.21.51", L7_IP_ADDRESS_TYPE_IPV4, &ndx);
  logHostTableDescriptionSet(1, "Host 1");
  logHostTableDescriptionSet(2, "Host 2");
  for (ndx = 0; ndx < 50; ndx++)
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "LOG - critical error message!");
  logStatusShow();
}

void logUnitTest2()
{
  L7_uint32 ndx;
  /** Test that the host API works for sets. */
  /*logCfgDefaultsSet(L7_LOG_CFG_VER_1);*/
  if (logHostAdd("192.168.21.50", L7_IP_ADDRESS_TYPE_IPV4, &ndx) != L7_SUCCESS)
    sysapiPrintf("log unit test 2-a failed on host add\n");

  if (logCfg->cfg.host[0].status != L7_ROW_STATUS_ACTIVE)
    sysapiPrintf("log unit test 2-b failed on host status\n");

  if (logHostTablePortSet(1,2) != L7_SUCCESS)
    sysapiPrintf("log unit test 2-c failed on port set\n");

  if (logCfg->cfg.host[0].port != 2)
    sysapiPrintf("log unit test 2-d failed on port retrieve\n");

  if (logHostTableComponentSet(1,3) != L7_SUCCESS)
    sysapiPrintf("log unit test 2-e failed on component set\n");

  if (logCfg->cfg.host[0].componentFilter != 3)
    sysapiPrintf("log unit test 2-f failed on component retrieve\n");

  if (logHostTableSeveritySet(1,4) != L7_SUCCESS)
    sysapiPrintf("log unit test 2-h failed on severity set\n");

  if (logCfg->cfg.host[0].severityFilter != 4)
    sysapiPrintf("log unit test 2-i failed on severity retrieve\n");

  logStatusShow();
}

void logUnitTest3()
{
  L7_uint32 ndx;
  /** Test that the host API works for sets. */
  /*logCfgDefaultsSet(L7_LOG_CFG_VER_1);*/
  if (logHostAdd("198.82.162.213", L7_IP_ADDRESS_TYPE_IPV4, &ndx) != L7_SUCCESS)
    sysapiPrintf("log unit test 3-a failed on host add\n");

  L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "LOG - Critical severity message!");
  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID,
    "LOG - message length start test                                             end message length test ");
  L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_DRIVER_COMPONENT_ID, "LOGF - "
    "Formatted output: %d %d %d %d", 1,2,3,4);
  L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID, "LOGF - Alert severity message!");
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, "LOGF - Formatted output %d", 1);
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID, "Info severity message!");

  logStatusShow();
}

void logUnitTest4()
{
  /** Test that the console logging works. */
  /*logCfgDefaultsSet(L7_LOG_CFG_VER_1);*/
  logConsoleAdminStatusSet(L7_ADMIN_MODE_DISABLE);
  logConsoleSeverityFilterSet(L7_LOG_SEVERITY_DEBUG);
  L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "LOGF - Error severity message!");
  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, "LOGF - Message test start"
  "                                                     "
  "message test end");
  L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "LOGF - Message test start "
  "                                                        "
  "message test end");
  L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_DRIVER_COMPONENT_ID, "LOGF - "
  "Formatted output: %d %d %d %d", 1,2,3,4);
  L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DRIVER_COMPONENT_ID, "LOGF - Alert severity message!");
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, "LOGF - Formatted output %d", 1);
  L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DRIVER_COMPONENT_ID, "LOGF - Formatted output %d", 1);
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID, "Info severity message!");
  L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_DRIVER_COMPONENT_ID, "Emergency severity message!");
  logStatusShow();
}

void logUnitTest5()
{
  L7_uint32 ndx;
  L7_uint32 loopNdx;
  L7_char8 buf[L7_LOG_MESSAGE_LENGTH];
  L7_uint32 unit;

  usmDbUnitMgrNumberGet(&unit);

  /*logCfgDefaultsSet(L7_LOG_CFG_VER_1);*/
  logConsoleAdminStatusSet(L7_ADMIN_MODE_DISABLE);
  logConsoleSeverityFilterSet(L7_LOG_SEVERITY_DEBUG);
  for (ndx = 0; ndx < 500; ndx++)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_DRIVER_COMPONENT_ID, "--%d", ndx);
    if ((ndx % 10) == 0)
      osapiSleep(1);
  }
  logStatusShow();
  osapiSleep(10);

  sysapiPrintf("\n\n\n\n\n\nIn memory log --\n");
  ndx = 0;
  do
  {
    L7_RC_t rc = logMsgInMemoryGetNext(ndx, buf, &ndx);
    if ((rc == L7_ERROR) || (rc == L7_NOT_EXIST))
      break;
    sysapiPrintf("%d - ", ndx);
    sysapiPrintf(buf);
    sysapiPrintf("\n");

  } while (ndx);
  sysapiPrintf("\n\n\n\n\n\nPersistent log --\n");
  ndx = 0;
  for (loopNdx = 0; loopNdx < L7_LOG_PERSISTENT_STARTUP_LOG_COUNT; loopNdx++)
  {
    L7_RC_t rc = logMsgPersistentGetNext(unit, ndx, buf, &ndx);
    if ((rc == L7_ERROR) || (rc == L7_NOT_EXIST))
      break;
    sysapiPrintf("%d - ", ndx);
    sysapiPrintf(buf);
    sysapiPrintf("\n");

  }
  ndx = 500 - L7_LOG_PERSISTENT_OPERATION_LOG_COUNT;
  for (loopNdx = 0; loopNdx < L7_LOG_PERSISTENT_OPERATION_LOG_COUNT; loopNdx++)
  {
    L7_RC_t rc = logMsgPersistentGetNext(unit, ndx, buf, &ndx);
    if ((rc == L7_ERROR) || (rc == L7_NOT_EXIST))
      break;
    sysapiPrintf("%d - ", ndx);
    sysapiPrintf(buf);
    sysapiPrintf("\n");

  }
}

void logUnitTest6()
{
  L7_uint32 ndx;
  L7_uint32 loopNdx;
  L7_char8 buf[L7_LOG_MESSAGE_LENGTH];
  L7_uint32 unit;

  usmDbUnitMgrNumberGet(&unit);

  /*logCfgDefaultsSet(L7_LOG_CFG_VER_1);*/
  logConsoleAdminStatusSet(L7_ADMIN_MODE_DISABLE);
  logConsoleSeverityFilterSet(L7_LOG_SEVERITY_DEBUG);
  for (ndx = 0; ndx < 500; ndx++)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_DRIVER_COMPONENT_ID, "--%d", ndx);
    if ((ndx % 10) == 0)
      osapiSleep(1);
  }
  logStatusShow();
  osapiSleep(10);

  sysapiPrintf("\n\n\n\n\n\nIn memory log --\n");
  ndx = 0;
  do
  {
    L7_RC_t rc = logMsgInMemoryGetNext(ndx, buf, &ndx);
    if ((rc == L7_ERROR) || (rc == L7_NOT_EXIST))
      break;
    sysapiPrintf("%d - ", ndx);
    sysapiPrintf(buf);
    sysapiPrintf("\n");

  } while (ndx);
  sysapiPrintf("\n\n\n\n\n\nPersistent log --\n");
  ndx = 0;
  for (loopNdx = 0; loopNdx < L7_LOG_PERSISTENT_STARTUP_LOG_COUNT; loopNdx++)
  {
    L7_RC_t rc = logMsgPersistentGetNext(2, ndx, buf, &ndx);
    if ((rc == L7_ERROR) || (rc == L7_NOT_EXIST))
      break;
    sysapiPrintf("%d - ", ndx);
    sysapiPrintf(buf);
    sysapiPrintf("\n");

  }
  ndx = 500 - L7_LOG_PERSISTENT_OPERATION_LOG_COUNT;
  for (loopNdx = 0; loopNdx < L7_LOG_PERSISTENT_OPERATION_LOG_COUNT; loopNdx++)
  {
    L7_RC_t rc = logMsgPersistentGetNext(2, ndx, buf, &ndx);
    if ((rc == L7_ERROR) || (rc == L7_NOT_EXIST))
      break;
    sysapiPrintf("%d - ", ndx);
    sysapiPrintf(buf);
    sysapiPrintf("\n");

  }
}

void logUnitTestSeverity()
{
  L7_LOG_SEVERITY_t severity;

  for (severity = L7_LOG_SEVERITY_EMERGENCY; severity <= L7_LOG_SEVERITY_DEBUG; severity++)
    L7_LOGF(severity, L7_DRIVER_COMPONENT_ID, "severity %d", severity);
}


void logUnitTestComponent()
{
  L7_COMPONENT_IDS_t  component;
  for (component = L7_FIRST_COMPONENT_ID; component < L7_LAST_COMPONENT_ID; component++)
  {
      L7_RC_t rc;
      L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

      if ((rc = cnfgrApiComponentNameGet(component, name)) != L7_SUCCESS)
      {
        osapiStrncpySafe(name, "Unknown", 8);
      }

      L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, component, "component %d, %s", component, name);
  }
}

void logUnitTestCustom(L7_LOG_SEVERITY_t severity, L7_COMPONENT_IDS_t component, L7_char8 *message)
{
  if (severity == 0 && component == 0 && message == L7_NULLPTR)
  {
    sysapiPrintf("logUnitTestCustom(<severity %d-%d>,<component %d-%d>,'message')\n", L7_LOG_SEVERITY_EMERGENCY, L7_LOG_SEVERITY_DEBUG, L7_FIRST_COMPONENT_ID+1, L7_LAST_COMPONENT_ID-1);
    return;
  }

  if (severity < L7_LOG_SEVERITY_EMERGENCY || severity > L7_LOG_SEVERITY_DEBUG)
  {
    sysapiPrintf("Severity must be between %d and %d.\n", L7_LOG_SEVERITY_EMERGENCY, L7_LOG_SEVERITY_DEBUG);
    return;
  }

  if (component <= L7_FIRST_COMPONENT_ID || component >= L7_LAST_COMPONENT_ID)
  {
    sysapiPrintf("Component must be between %d and %d.\n", L7_FIRST_COMPONENT_ID+1, L7_LAST_COMPONENT_ID-1);
    return;
  }

  L7_LOGF(severity, component, message);
}


typedef struct commandLinkList_s{
  struct commandLinkList_s *next_ptr;
  char *command;
} commandLinkList_t;

commandLinkList_t *commandList_p = L7_NULLPTR;

/* Wait forever instead of reseting the box */
static int log_error_wait_forever = 0;

/*
 * Add debug functions to be called as a result of log_error
 */
void log_error_debug_func_add(char *command)
{
  commandLinkList_t *tmp_p = L7_NULLPTR;

  do {
    tmp_p = osapiMalloc(L7_DRIVER_COMPONENT_ID,sizeof(commandLinkList_t));

    if (tmp_p == L7_NULLPTR) break;

    tmp_p->next_ptr = L7_NULLPTR;
    tmp_p->command = osapiMalloc(L7_DRIVER_COMPONENT_ID,strlen(command)+1);

    if (tmp_p->command == L7_NULLPTR)
    {
      osapiFree(L7_DRIVER_COMPONENT_ID,tmp_p);
      tmp_p = L7_NULLPTR;
      break;
    }

    strcpy(tmp_p->command,command);

    tmp_p->next_ptr = commandList_p;
    commandList_p = tmp_p;

  } while(0);

  if (tmp_p && tmp_p->command)
  {
    sysapiPrintf("%s was added\n",command);
  }
  else
  {
    sysapiPrintf("%s was not added\n",command);
  }

  return;
}

/* Iterate and execute over the function list */
void log_error_func_exec(void)
{
#define CMD_MAX_SIZE    256
  commandLinkList_t *tmp_p = commandList_p;
  int sizeOfCommand = 0;
  char command[CMD_MAX_SIZE];

  while (tmp_p != L7_NULLPTR)
  {
    sysapiPrintf("%s\n",tmp_p->command);
    sizeOfCommand = strlen(tmp_p->command)+1;
    if (sizeOfCommand >= CMD_MAX_SIZE) sizeOfCommand = CMD_MAX_SIZE - 1;
    strncpy(command,tmp_p->command,sizeOfCommand);
    command[sizeOfCommand] = 0;
    osapiDevShellExec(command);
    osapiSleep(1);
    tmp_p = tmp_p->next_ptr;
  }

  if (log_error_wait_forever < 0)
  {
    while (1) { osapiSleep(1); }
  }
  else
  {
    osapiSleep(log_error_wait_forever);
  }

  return;
}


/* list the contents of the function list */
void log_error_debug_func_list(void)
{
  commandLinkList_t *tmp_p = commandList_p;

  while (tmp_p != L7_NULLPTR)
  {
    sysapiPrintf("%s\n",tmp_p->command);
    tmp_p = tmp_p->next_ptr;
  }
}

/* Set the log_error duration at -1(forever) or time in seconds */
void log_error_debug_wait_set(int time)
{
  log_error_wait_forever = time;
}

/* get rid of the contents of the function list */
void log_error_debug_func_list_flush(void)
{
  commandLinkList_t *tmp_p;
  commandLinkList_t *curr_p = commandList_p;
  sysapiPrintf("functions being removed\n");
  log_error_debug_func_list();

  while (curr_p)
  {
    tmp_p = curr_p->next_ptr;
    osapiFree(L7_DRIVER_COMPONENT_ID,curr_p->command);
    osapiFree(L7_DRIVER_COMPONENT_ID,curr_p);
    curr_p = tmp_p;
  }

  commandList_p = L7_NULLPTR;
}


void log_error_debug_help(void)
{
  sysapiPrintf("log_error_debug_func_add('func(args)') - add func and args to function list\n");
  sysapiPrintf("log_error_debug_func_list_flush()      - clear the func list\n");
  sysapiPrintf("log_error_debug_wait_set(sec)          - set time to wait after exec(-1=forever)\n");
  sysapiPrintf("log_error_debug_func_list()            - list funcs to be executed\n");
  sysapiPrintf("log_error_func_exec()                  _ execute func list\n");
  return;
}
static int log_interval;   /* In milliseconds */
static int log_async_exit = 0;
static int async_task_id = 0;
static int async_event_number = 0;
static int log_msg_size = 64;
int logSizeSet (int size)
{
 if ((size < 0) || (size > LOG_MSG_MAX_MSG_SIZE))
  {
   printf("Invalid size: %d. Max size is %d\n", size, LOG_MSG_MAX_MSG_SIZE);
   return 0;
 }
  log_msg_size = size;
  return 0;
}

void logAsyncTask (void)
{
 unsigned char log_buf [LOG_MSG_MAX_MSG_SIZE];
int i;
 char ch;
 do {
  ch = 'a';
  for (i = 0; i < (log_msg_size -1); i++)
   {
   log_buf [i] = ch;
    ch++;
   if (ch > 'z')
   {
     ch = 'a';
    }
   }
   log_buf[i] = 0;
   L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID,           "Test Event #%d", async_event_number );
   async_event_number++;
  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID,           "%s", log_buf );
   osapiSleepMSec (log_interval);
    } while (log_async_exit == 0);
 async_task_id = 0;
}

int logAsyncStart(int interval)
{
 log_async_exit = 0;
 log_interval = interval;
  if (async_task_id == 0)
  {
    async_task_id = (L7_uint32)osapiTaskCreate("ASYNC_LOG",
                                            (void *)logAsyncTask,
                                            L7_NULL,
                                            L7_NULL,
                                            L7_DEFAULT_STACK_SIZE,
                                            L7_DEFAULT_TASK_PRIORITY,
                                            L7_DEFAULT_TASK_SLICE);
  }
 return 0;
}

int logAsyncStop (void)
{
  log_async_exit = 1;
  return 0;
}

