
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  cli_web_util.c
*
* @purpose   cli and web manager utility functions
*
* @component cliWebUI component
*
* @comments  none
*
* @create    03/26/2001
*
* @author    asuthan
*
* @end
*
**********************************************************************/
#include "cliapi.h"
#include <errno.h>

#include "cli_web_include.h"
#include "default_cnfgr.h"
#include "cli_web_user_mgmt.h"
#include "user_mgr_api.h"
#include "trap_inventory_api.h"
#include "cli_trap_inventory.h"
#include "cli.h"
#include "ews_api.h"
#include "log.h"
#include "osapi.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_sslt_api.h"
#include "usmdb_unitmgr_api.h"
#include "osapi_support.h"
#include "support_api.h"
#include "cli_web_mgr_api.h"
#include "cli_txtcfg_util.h"
#include "cli_txt_cfg_api.h"
#include "usmdb_user_mgmt_api.h"

#ifdef _L7_OS_VXWORKS_
#include "ewnet.h"
#include <ioLib.h>
#include <pipeDrv.h>
#include <selectLib.h>
#include <logLib.h>
#include <taskLib.h>
#endif

#ifdef _L7_OS_LINUX_
  #ifndef HAVE_CONFIG_H
    #define HAVE_CONFIG_H
  #endif
#endif

#include "ewnet.h"

#ifdef _L7_OS_VXWORKS_
  #include <ioLib.h>
  #include <pipeDrv.h>
  #include <selectLib.h>
  #include <logLib.h>
  #include <taskLib.h>
#endif

L7_int32              L7_cli_web_task_id;
L7_int32              L7_web_java_task_id;
L7_int32              L7_util_task_id;
cliWebCfgData_t       cliWebCfgData;

/* Semaphore to prevent simulataneous invocations of show running-config. */
void                  *cliRunCfgSema = L7_NULLPTR;

/*********SERVICEABILITY***************/
#ifdef LVL7_DEBUG_BREAKIN
consoleBreakinCfg_t consoleBreakinCfg;
static L7_char8 localString[L7_PASSWORD_SIZE] = CLI_SUPPORTCONSOLE_DEFAULTBREAKIN;
static L7_BOOL  breakin_flag = L7_TRUE;
#endif
/************************************/

static void * cliWebAccessSem = L7_NULL;


/* Parms for redirecting IO to the console */
#define CLI_WEB_REDIRECT_TASK 0

#ifdef _L7_OS_VXWORKS_
  #define CLI_IO_REDIRECT_PIPE_NAME      "/cliIoRedirectPipe/1"    /* Should not be greater
                                                                    than CLI_IO_REDIRECT_PIPE_NAME_LEN */

  #define CLI_IO_REDIRECT_PIPE_NAME_LEN  25           /* Arbitrary Number*/
#else
  #define CLI_IO_REDIRECT_PIPE_NAME      "/cliIoRedirectPipe"    /* Should not be greater
                                                                    than CLI_IO_REDIRECT_PIPE_NAME_LEN */

  #define CLI_IO_REDIRECT_PIPE_NAME_LEN  25           /* Arbitrary Number*/
#endif

/* Careful tweaking of the max msgs and msg len of the pipe are needed.
   If the max number of messages in the pipe is too small, then VxWorks builds
   will have an emWeb hang if IO is redirected while displaying certain messages
   (checkstack, i, osapiShowTasks).  If the maximum length of the message is much
   less than 100, formatting issues will occur.
   */

#define CLI_IO_REDIRECT_PIPE_MAX_MSGS  2048
#define CLI_IO_REDIRECT_PIPE_MSG_LEN   100


/* The task priority of the redirect task must be less than the task priority of the
   emWeb task in order to minimize the possiblity of a hang when the aforementioned
   routines are executed while IO is redirected. */
#define CLI_IO_REDIRECT_TASK_PRIORITY   (L7_DEFAULT_TASK_PRIORITY)


/* Start of parms which belong in xxx_debug.* */

#define CLI_WEB_TRACE(format,args...)                                         \
{                                                                             \
   if ( cliWebDebugTraceModeGet() == L7_ENABLE)                               \
   {                                                                          \
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_CLI_WEB_COMPONENT_ID,format,##args);  \
   }                                                                          \
}


#define CLI_WEB_IOREDIRECT_TRACE(format,args...)                              \
{                                                                             \
   if ( cliWebDebugIoRedirectTraceModeGet() == L7_ENABLE)                     \
   {                                                                          \
      CLI_WEB_TRACE(format,##args);                                           \
   }                                                                          \
}

L7_RC_t cliWebDebugTraceModeSetAll(L7_uint32 mode);
L7_RC_t cliWebDebugTraceModeSet(L7_uint32 mode);
L7_uint32 cliWebDebugTraceModeGet(void);
L7_RC_t cliWebDebugIoRedirectTraceModeSet(L7_uint32 mode);
L7_uint32 cliWebDebugIoRedirectTraceModeGet(void);

static L7_uint32 cliWebDebugTraceMode            = L7_DISABLE;
static L7_uint32 cliWebDebugIoRedirectTraceMode  = L7_DISABLE;

/* End of parms which belong in xxx_debug.* */

static L7_uint32 cliWebIORedirectTaskId = L7_NULL;
static L7_BOOL  cliWebIORedirectInitialized = L7_FALSE;
static  L7_BOOL       redirectTaskNeedsToBeAssigned = L7_FALSE;
static  L7_BOOL       redirectTaskNeedsToBeReassigned = L7_FALSE;
static L7_int32 cliWebIORedirectHandle;

static L7_int32 cliWebIOStdInFd = -1;
static L7_int32 cliWebIOStdOutFd = -1;
static L7_int32 cliWebIOStdErrFd = -1;
#ifdef _L7_OS_VXWORKS_
  static L7_int32 cliWebIOTaskID = L7_NULL;
#endif
#ifdef _L7_OS_LINUX_
  static L7_int32 cliWebIORedirectFd = -1;
#endif

extern int consoleFd;   /* fd of initial console device      */
static void cliWebIORedirectInit(void);

static void cliWebIOSetGlobalIo ( L7_int32 fileHandle );
static void cliWebIOResetGlobalIo ( L7_int32 fileHandle );
void cliWebIOStdIOFdStore ( void );
static L7_RC_t cliWebIORedirectIOTask(void);
static L7_RC_t cliWebIOLogRedirectInit ( );

extern  int ewaTaskStart( void );
extern L7_RC_t ewaNetTelnetSocketActivate(void);
extern L7_RC_t ewaNetTelnetSocketClose(void);
extern void closeWebConnections(void);
extern  L7_RC_t L7_ewsPhaseOneInit(L7_uint32, L7_uint32, L7_uint32, L7_uint32);
extern  void loginSessionInit();
extern  void cliPromptReset();
extern  cliWebCnfgrState_t cliWebCnfgrState;
extern void ewaNetVarsInit(void);
extern void l7UtilVarsInit(void);
extern L7_RC_t ewaNetHttpPortSet(L7_uint32 port);

/*********************************************************************
* @purpose  Checks if cli_web user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL cliWebHasDataChanged(void)
{
  return cliWebCfgData.cfgHdr.dataChanged;
}

void cliWebResetDataChanged(void)
{
  cliWebCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Print the current CLI/WEB Manager config values to
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cliWebConfigDump(void)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_char8 buf[66];

  if (cliWebCfgData.systemWebMode == L7_ENABLE)
    sprintf(buf,"%s","L7_ENABLE");
  else
    sprintf(buf,"%s","L7_DISABLE");
  printf("Web Mode - %s\n",buf);

  if (cliWebCfgData.systemJavaMode == L7_ENABLE)
    sprintf(buf,"%s","L7_ENABLE");
  else
    sprintf(buf,"%s","L7_DISABLE");
  printf("Java Mode - %s\n",buf);

  printf("Command Prompt - %s\n",cliWebCfgData.systemCommandPrompt);

  if (cliWebCfgData.systemTelnetNewSessions == L7_ENABLE)
    sprintf(buf,"%s","L7_ENABLE");
  else
    sprintf(buf,"%s","L7_DISABLE");
  printf("New Telnet Sessions - %s\n",buf);

  printf("Number of Telnet Sessions - %d\n",cliWebCfgData.systemTelnetNumSessions);
  printf("Telnet Timeout - %d\n",cliWebCfgData.systemTelnetTimeout);

  if (cliWebCfgData.systemTelnetAdminMode == L7_ENABLE)
    sprintf(buf,"%s","L7_ENABLE");
  else
    sprintf(buf,"%s","L7_DISABLE");
  printf("IP Telnet Server Connection - %s\n", buf);

  if (cliWebCfgData.userPrompt == L7_TRUE)
    sprintf(buf,"%s","L7_TRUE");
  else
    sprintf(buf,"%s","L7_FALSE");
  printf("\nUser Defined Prompt - %s\n",buf);

  printf("\nCurrent CLI banner is - %s\n", cliWebCfgData.cliBanner);

  printf("Telnet Listen Port - %d\n",cliWebCfgData.systemTelnetPortNum);
  printf("Http Listen Port - %d\n",cliWebCfgData.httpListenPortNum);

  return rc;
}

/*********************************************************************
* @purpose  Saves cli_web user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cliWebSave(void)
{
  if (cliWebCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    cliWebCfgData.cfgHdr.dataChanged = L7_FALSE;
    cliWebCfgData.checkSum = nvStoreCrc32((L7_char8 *)&cliWebCfgData,
                                          sizeof(cliWebCfgData) - sizeof(cliWebCfgData.checkSum));

    /* call save NVStore routine */
    if (sysapiCfgFileWrite(L7_CLI_WEB_COMPONENT_ID,
                           CLI_WEB_CFG_FILENAME,
                           (L7_char8 *)&cliWebCfgData,
                           sizeof (cliWebCfgData)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n", CLI_WEB_CFG_FILENAME);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default cli_web config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void cliWebBuildDefaultConfigData(L7_uint32 ver)
{
  memset(( void * )&cliWebCfgData, 0, sizeof( cliWebCfgData_t));
  strcpy(cliWebCfgData.cfgHdr.filename, CLI_WEB_CFG_FILENAME);
  cliWebCfgData.cfgHdr.version = ver;
  cliWebCfgData.cfgHdr.componentID = L7_CLI_WEB_COMPONENT_ID;
  cliWebCfgData.cfgHdr.type = L7_CFG_DATA;
  cliWebCfgData.cfgHdr.length = sizeof(cliWebCfgData);
  cliWebCfgData.cfgHdr.dataChanged = L7_FALSE;
  cliWebCfgData.cfgHdr.version = ver;

  cliWebCfgData.systemWebMode = FD_CLI_WEB_MODE;
  cliWebCfgData.webSessionHardTimeOut = FD_HTTP_SESSION_HARD_TIMEOUT_DEFAULT;
  cliWebCfgData.webSessionSoftTimeOut = FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
  cliWebCfgData.webNumSessions        = FD_HTTP_DEFAULT_MAX_CONNECTIONS;

  cliWebCfgData.userPrompt = L7_FALSE;
  memcpy(cliWebCfgData.systemCommandPrompt, FD_CLI_WEB_COMMAND_PROMPT, strlen(FD_CLI_WEB_COMMAND_PROMPT));
  cliWebCfgData.systemTelnetNewSessions = FD_CLI_WEB_TELNET_NEW_SESSIONS;
  cliWebCfgData.systemTelnetNumSessions = FD_CLI_WEB_DEFAULT_NUM_SESSIONS;
  cliWebCfgData.systemTelnetTimeout = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT;
  cliWebCfgData.systemTelnetAdminMode = FD_CLI_WEB_TELNET_DEFAULT_ADMIN_MODE;
  cliWebCfgData.systemJavaMode = FD_CLI_WEB_JAVA_MODE;
  memset(cliWebCfgData.cliBanner, 0, sizeof(cliWebCfgData.cliBanner));
  cliWebCfgData.linesForPagination = FD_CLI_LINES_FOR_PAGINATION;
  cliWebCfgData.systemTelnetPortNum = FD_TELNET_PORT_NUM;
  cliWebCfgData.httpListenPortNum   = FD_HTTP_PORT_NUM;
}

/*********************************************************************
*
* @purpose  Starts cli_web task function
*
* @notes    none
*
* @end
*********************************************************************/
void L7_cli_web_task (void)
{
  ewaTaskStart();
}

/*********************************************************************
* @purpose  cli_web set to factory defaults
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void cliWebInitData(void)
{
  if (( strcmp(cliWebCfgData.systemCommandPrompt, FD_CLI_WEB_COMMAND_PROMPT) != 0 )
      && ( cliWebCfgData.userPrompt == L7_FALSE ))
  {
    OSAPI_STRNCPY_SAFE(cliWebCfgData.systemCommandPrompt, FD_CLI_WEB_COMMAND_PROMPT);
  }
  cliPhaseThreeInitVars();
}

/*********************************************************************
* @purpose Initialize the cliWeb for Phase 1
*
* @param   void
*
* @returns L7_SUCCESS  Phase 1 completed
* @returns L7_FAILURE  Phase 1 incomplete
*
* @notes  If phase 1 is incomplete, it is up to the caller to call the fini
*         function if desired.  If this fails, it is due to an inability to
*         to acquire resources.
*
* @end
*********************************************************************/
L7_RC_t cliWebPhaseOneInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  do
  {
    /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
    cliCommon = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, sizeof(cliCommon_t) * FD_CLI_DEFAULT_MAX_CONNECTIONS);
    if(cliCommon == L7_NULLPTR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "%s: cliWebPhaseOneInit: %u: Failed to allocate memory for cliCommon.\n",
              __FILE__, __LINE__);
      return L7_FAILURE;
    }
    /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
    memset(cliCommon, 0, sizeof(cliCommon_t) * FD_CLI_DEFAULT_MAX_CONNECTIONS);

#if defined(L7_WEB_PACKAGE) || defined(L7_XWEB_PACKAGE)
    L7_web_java_task_id = osapiTaskCreate( "webJavaTask", L7_web_java_task, 0, 0,
                                           L7_DEFAULT_STACK_SIZE + (1024 * 32 *2),
                                           L7_DEFAULT_TASK_PRIORITY,
                                           L7_DEFAULT_TASK_SLICE);

    if (L7_web_java_task_id == L7_ERROR)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "Failed to Create Web_Java Task.\n");
#endif

    L7_util_task_id = osapiTaskCreate( "UtilTask", L7_util_task, 0, 0,
                                       FD_CNFGR_CLI_WEB_DEFAULT_STACK_SIZE,
                                       FD_CNFGR_CLI_WEB_DEFAULT_TASK_PRI,
                                       FD_CNFGR_CLI_WEB_DEFAULT_TASK_SLICE);
    if (L7_util_task_id == L7_ERROR)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "Failed to Create Util Task.\n");

    if (L7_ewsPhaseOneInit(0 /* max connections - use default */
                           ,256 /* max buffers - use default */ /* lvl7_P0006 */
                           ,0 /* buffer data size - use default */
                           ,0 /* port number - use default */) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    L7_cli_web_task_id = osapiTaskCreate( "cliWebTask", L7_cli_web_task, 0, 0,
                                          FD_CNFGR_CLI_WEB_DEFAULT_STACK_SIZE,
                                          FD_CNFGR_CLI_WEB_DEFAULT_TASK_PRI,
                                          FD_CNFGR_CLI_WEB_DEFAULT_TASK_SLICE);
    if (L7_cli_web_task_id == L7_ERROR)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "Failed to Create Cli_Web Task.\n");

    cliWebIORedirectInit();

    if (L7_NULLPTR == cliRunCfgSema)
    {
      cliRunCfgSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    }

    if (L7_NULLPTR == cliRunCfgSema)
    {
      rc = L7_FAILURE;
      break;
    }

  } while (0);
#ifdef LVL7_DEBUG_BREAKIN
  consoleBreakinCfgRead();
  consoleBreakinApplyConfigData();
#endif

  return(rc);
}

/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void cliWebPhaseOneFini(void)
{
  if(cliCommon != L7_NULLPTR)
  {
    osapiFree(L7_CLI_WEB_COMPONENT_ID, cliCommon);
  }

  if (L7_web_java_task_id != L7_ERROR)
  {
    osapiTaskDelete(L7_web_java_task_id);
  }

  if (L7_util_task_id != L7_ERROR)
  {
    osapiTaskDelete(L7_util_task_id);
  }

  if (L7_cli_web_task_id != L7_ERROR)
  {
    osapiTaskDelete(L7_cli_web_task_id);
  }

  ewsShutdown();

  cliWebCnfgrState = CLI_WEB_PHASE_INIT_0;
}

/*********************************************************************
* @purpose Initialize the cliWeb for Phase 2
*
* @param   void
*
* @returns L7_SUCCESS  Phase 2 completed
* @returns L7_FAILURE  Phase 2 incomplete
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t cliWebPhaseTwoInit(void)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t notifyFunctionList; /* nvstore Functions' Data Structure */

  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_CLI_WEB_COMPONENT_ID;
  notifyFunctionList.notifySave     = cliWebSave;
  notifyFunctionList.hasDataChanged = cliWebHasDataChanged;
  notifyFunctionList.notifyConfigDump     = cliWebConfigDump;
  notifyFunctionList.resetDataChanged = cliWebResetDataChanged;
  nvStoreRegister(notifyFunctionList);

  /* register with TRAPMGR to receive card trap notifications */
  if (trapMgrRegisterCardEvent(L7_CLI_WEB_COMPONENT_ID,
                               cliInventoryCardCallback) != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "Failed registration for TRAPMGR CARD EVENTS\n");

  cliPhaseTwoInitVars();
#ifdef LVL7_DEBUG_BREAKIN
  consoleBreakinRegister();
#endif

  return rc;
}

/*********************************************************************
* @purpose Free the resource for phase 2
*
* @param   void
*
* @returns void
*
* @notes  If phase 2 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void cliWebPhaseTwoFini(void)
{
  nvStoreFunctionList_t notifyFunctionList; /* nvstore Functions' Data Structure */

  /* nvstore Functions' Data Structure initialized to zero */
  memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));

  /* de-register by setting the values to zero and registering again */
  (void)nvStoreRegister(notifyFunctionList);

  /* deregister with TRAPMGR to receive card trap notifications */
  (void) trapMgrRegisterCardEvent(L7_CLI_WEB_COMPONENT_ID, L7_NULL);

  cliWebCnfgrState = CLI_WEB_PHASE_INIT_1;
  return;
}

/*********************************************************************
* @purpose Initialize the cliWeb for Phase 3
*
* @param   void
*
* @returns L7_SUCCESS  Phase 3 completed
* @returns L7_FAILURE  Phase 3 incomplete
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
L7_RC_t cliWebPhaseThreeInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = sysapiCfgFileGet(L7_CLI_WEB_COMPONENT_ID, CLI_WEB_CFG_FILENAME,
                        (L7_char8 *)&cliWebCfgData, sizeof(cliWebCfgData),
                        &cliWebCfgData.checkSum, CLI_WEB_CFG_VER_CURRENT,
                        cliWebBuildDefaultConfigData, cliWebMigrateConfigData);
  if (rc != L7_SUCCESS)
  {
    rc = L7_FAILURE;
  }
  else
  {
    cliWebCfgData.cfgHdr.dataChanged = L7_FALSE;
  }

  cliWebInitData();

  return(rc);
}

/*********************************************************************
* @purpose Reset the cliWeb to prior to phase 3
*
* @param   void
*
* @returns void
*
* @notes  If phase 3 is incomplete, it is up to the caller to call the fini
*         function if desired
*
* @end
*********************************************************************/
void cliWebPhaseThreeFini(void)
{
  L7_uint32 index, loginType, unit;
  L7_BOOL validConnection;
  EwsContext context = L7_NULL;
  L7_uchar8 buf[L7_PROMPT_SIZE];

  unit = usmDbThisUnitGet();

  /* config data initialized to zero */
  memset((void *) &cliWebCfgData, 0, sizeof(cliWebCfgData_t));

  if (usmDbUnitMgrNumberGet(&unit) != L7_SUCCESS)
  {
    unit = 1;
  }
  osapiSnprintf(cliUtil.systemPrompt, sizeof(cliUtil.systemPrompt), "\r\n(Unit %d)", unit);

  cliUtil.resetSerialConnection = L7_FALSE;                               /* flag to reset connection on next input */
  cliUtil.transferInProgress = L7_FALSE;

  /*old SSH TELNET TAG _TOTAL_CONNECTIONS*/
  for (index = 0; index < FD_CLI_DEFAULT_MAX_CONNECTIONS; index++)
  {
    usmDbLoginSessionValidEntry(unit, index, &validConnection);
    if (validConnection == L7_TRUE)
    {
      usmDbLoginSessionTypeGet(unit, index, &loginType);
      context = usmDbLoginSessionUserStorageGet(unit, index);

      if (loginType == L7_LOGIN_TYPE_SERIAL)
      {
        /* do not logout for transfer connections until the transfer is complete */
        usmDbLoginSessionLogout(unit, index);
      }
      else if ((loginType == L7_LOGIN_TYPE_TELNET) || (loginType == L7_LOGIN_TYPE_SSH))
      {
        usmDbLoginSessionLogout(unit, index);
        /* do not logout for transfer connections until the transfer is complete */

        /*
         * There is a window during which a user can logout, but the UtilTask
         * has already determined that the session should be logged out and
         * context could be 0 during that interval; when that is the case,
         * skip the forced logout
         */

        if (context)
        {
          /* Only attempt to write if the telnet session is still valid. */
          if (ewsTelnetSessionValidate(context) == L7_TRUE)
          {
            /*    ewsTelnetWrite(context, IDLE_DISCONNECT_MSG);*/
            ewsFlushAll(context);
          }
          ewsNetHTTPAbort(context);
        }
      }
    }
    cliInitConnection(index);
  }

  /* Tear down socket connections here */
#ifdef L7_MGMT_SECURITY_PACKAGE
  usmDbssltAdminModeSet(0, L7_DISABLE);
#endif
  cliWebCfgData.systemWebMode = L7_DISABLE;
  cliWebCfgData.systemTelnetNewSessions = 0;
  osapiSleep(2);

  cliCurrentHandleSet(CLI_SERIAL_HANDLE_NUM);
  cliChangeMode(L7_HIDDEN_COMMAND_MODE);

  cliCommon[CLI_SERIAL_HANDLE_NUM].adminUserFlag = L7_TRUE;

  osapiSnprintf(buf, sizeof(buf), "%s%s", cliUtil.systemPrompt, cliUtilGetRootPrompt());
  cliWriteSerial(buf);

  cliWebCnfgrState = CLI_WEB_PHASE_INIT_2;

  /* If it is clear configuration, "do not apply config" flag is set, in that case
     we should continue with invalidaton configuraton, then it will continue with default
     configuration.if it is move managemeent case then "do not apply config" flag is not
     set, then we should validate the configuration, then it  will apply the configuraton
     when it becomes manager */

  if (cliTxtCfgGetConfigFlag() == L7_TRUE)
  {
    cliTxtCfgSetConfigFlag(L7_FALSE);
  }
  else
  {
    sysapiTxtCfgValidSet(L7_TRUE);
  }
}

/*********************************************************************
*
* @purpose convert the provided char array into a 32 bit unsigned integer
*          the value is >= 0 and <= 4294967295.
*
* @param L7_char8 *buf, L7_uint32 * pVal
*
* @returns  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-numeric character
*              is included in buf.
*
* @notes This f(x) checks each letter inside the buf to make certain
*         it is an integer.  It initially verifies the number of digits
*         does not exceed the number of digits in Max 32 bit unsigned int.
*         Start with the last digit, convert each ascii character into its
*         integer equivalent.  Multiply each consecutive digit by the next
*         power of 10.  Verify adding the new digit to the old sum will not
*         exceed MAXINT.  If so, this indicates the value is too large to be
*         represented by a 32 bit int.  If ok, add the new digit.
*
*
* @end
*
*********************************************************************/
L7_RC_t cliWebConvertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal)
{
  L7_int32 f;
  L7_int32 j;
  L7_uint32 tempval;
#define MAXUINT 4294967295U
#define MAXMSB 4

  if ( buf[0] == '-' )
    return L7_FAILURE;

  j = strlen(buf);

  if ( j > 10 )
    return L7_FAILURE;

  if( (strlen(buf)== 10))
  {
    tempval = (L7_uint32)(buf[0] -'0');
    if (tempval > MAXMSB)
    {
      return L7_FAILURE;
    }
  }

  for ( *pVal = 0, f=1, j=j-1; j >= 0; j--, f=f*10)
  {
    tempval = (L7_uint32)(buf[j] -'0');

    if ( ( tempval > 9 ) || ( tempval < 0 ) )
      return L7_FAILURE;

    tempval = tempval * f;

    if ( (MAXUINT - tempval) < *pVal )
      return L7_FAILURE;
    else
      *pVal = *pVal + tempval;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose convert the provided char array in HEX form into a 32 bit unsigned integer
*          the value is >= 0 and <= 0xffffffff.
*
* @param L7_char8 *buf, L7_uint32 * pVal
*
* @returns  L7_SUCCESS  means that all chars are HEX integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-hex character
*              is included in buf.
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t cliWebConvertHexTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal)
{
  L7_uint32 f;
  L7_int32 j;
  L7_uint32 tempval;

  if ( buf[0] == '-' )
    return L7_FAILURE;

  j = strlen(buf);
  if ( j > 10 )
    return L7_FAILURE;

  *pVal=0;

  for(f=1,j=j-1;j>=2;j--,f=f*16)
  {
    if((buf[j]>='0') && (buf[j]<='9'))
      tempval = (L7_uint32)(buf[j] -'0');
    else if((buf[j]>='a') && (buf[j]<='z'))
      tempval = (L7_uint32)((buf[j] -'a')+10);
    else if((buf[j]>='A') && (buf[j]<='Z'))
      tempval = (L7_uint32)((buf[j] -'A')+10);
    else
      return L7_FAILURE;

    tempval = tempval * f;
    *pVal = *pVal + tempval;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Convert Hex representation of MAC address to the format
*           xx:xx:xx:xx:xx:xx
*
* @param    mac                  @b{(input)} MAC address in hex format
* @param    macBuf               @b{(output)} MAC address in ASCII string
*                                             as xx:xx:xx:xx:xx:xx
*
* @returns  L7_TRUE             If conversion succeeds
* @returns  L7_FALSE            Else
*
* @comments none.
*
* @notes    mac and macBuf must be allocated with atleast 6 and 17 bytes
*           respectively.
*
* @end
*
*********************************************************************/
L7_BOOL cliWebConvertHexMacToString(L7_uchar8 *mac, L7_uchar8 *macBuf)
{
  int val = 0;

  if(macBuf == NULL || mac == NULL)
  {
    return L7_FALSE;
  }

  val = sprintf(macBuf,"%02x:%02x:%02x:%02x:%02x:%02x",mac[0],mac[1],
                mac[2],mac[3],mac[4],mac[5]);
  macBuf[17]='\0';
  if(val < 17)
  {
    return L7_FALSE; /* On error reading */
  }
  return L7_TRUE;
}

/*********************************************************************
*
* @purpose convert the provided char array into a 32 bit signed integer
*          the value is >= -2,147,483,648 and <= 2,147,483,647.
*
* @param L7_char8 *buf
* @param L7_int32 * pVal
*
* @return  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 32 bit signed integer
* @return  L7_FAILURE  means the value does not represent a valid
*              32 bit signed integer.  I.e. the value is outside of
*              the valid range for a signed integer.
*
* @note This f(x) checks each letter inside the buf to make certain
*       it is an integer.  It initially verifies the number of digits
*       does not exceed the number of digits in Max 32 bit signed int.
*       Start with the last digit, convert each ascii character into its
*       integer equivalent.  Multiply each consecutive digit by the next
*       power of 10.  Verify adding the new digit to the old sum will not
*       exceed MAXINT.  If so, this indicates the value is too large to be
*       represented by a 32 bit int.  If ok, add the new digit.
*
*
* @end
*
*********************************************************************/
L7_RC_t cliWebConvertTo32BitSignedInteger(const L7_char8 *buf, L7_int32 *pVal)
{
  L7_char8 *endPtr;

  *pVal = strtol(buf, &endPtr, 10);
  if ((*endPtr) != 0)
  {
    return L7_FAILURE;
  }

  if (osapiErrnoGet() == ERANGE)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts a DSCP value to its associated keyword, if one
*           is available.  Otherwise, a string of the numeric dscpVal
*           input value is returned.
*
* @param    dscpVal         value to convert
* @param    dscpString      string to return
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for QOS Diffserv.
*
* @end
*********************************************************************/
L7_RC_t cliWebConvertDSCPValToString(L7_uint32 dscpVal, L7_char8* dscpString)
{
  L7_uchar8 *pFmtStr = L7_NULLPTR;

  if(dscpString == L7_NULLPTR)
  {
    return L7_FAILURE;  
  }

  switch(dscpVal)
  {
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11:
      pFmtStr = (L7_uchar8 *)"%u(af11)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12:
      pFmtStr = (L7_uchar8 *)"%u(af12)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13:
      pFmtStr = (L7_uchar8 *)"%u(af13)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21:
      pFmtStr = (L7_uchar8 *)"%u(af21)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22:
      pFmtStr = (L7_uchar8 *)"%u(af22)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23:
      pFmtStr = (L7_uchar8 *)"%u(af23)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31:
      pFmtStr = (L7_uchar8 *)"%u(af31)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32:
      pFmtStr = (L7_uchar8 *)"%u(af32)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33:
      pFmtStr = (L7_uchar8 *)"%u(af33)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41:
      pFmtStr = (L7_uchar8 *)"%u(af41)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42:
      pFmtStr = (L7_uchar8 *)"%u(af42)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43:
      pFmtStr = (L7_uchar8 *)"%u(af43)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE:
      pFmtStr = (L7_uchar8 *)"%u(be/cs0)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1:
      pFmtStr = (L7_uchar8 *)"%u(cs1)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2:
      pFmtStr = (L7_uchar8 *)"%u(cs2)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3:
      pFmtStr = (L7_uchar8 *)"%u(cs3)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4:
      pFmtStr = (L7_uchar8 *)"%u(cs4)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5:
      pFmtStr = (L7_uchar8 *)"%u(cs5)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6:
      pFmtStr = (L7_uchar8 *)"%u(cs6)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7:
      pFmtStr = (L7_uchar8 *)"%u(cs7)";
      break;
    case L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF:
      pFmtStr = (L7_uchar8 *)"%u(ef)";
      break;
    default:
      pFmtStr = (L7_uchar8 *)"%u";
      break;
  }

  sprintf ((char *)dscpString, (char *)pFmtStr, dscpVal);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the global CLI/WEB access semaphore.
*
* @param    none
*
* @returns  none
*
* @notes    The caller blocks until the semaphore is available.
*
* @end
*********************************************************************/
void cliWebAccessSemaGet(void)
{
  if (cliWebAccessSem == L7_NULL)
  {
    cliWebAccessSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

    if (cliWebAccessSem == L7_NULL)
    {
      L7_LOG_ERROR(0);
    }
  }

  if (osapiSemaTake (cliWebAccessSem, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOG_ERROR(0);
  }
}

/*********************************************************************
* @purpose  Free the global CLI/WEB access semaphore.
*
* @param    none
*
* @returns  none
*
* @notes    The caller blocks until the semaphore is available.
*
* @end
*********************************************************************/
void cliWebAccessSemaFree(void)
{
  if (cliWebAccessSem == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  osapiSemaGive(cliWebAccessSem);
}

/**************************************************************************
*
* @purpose  This function is responsible for the initiailization of the
*           IO redirection task.
*
* @param    none
*
* @returns  none
*
* @end
*
*************************************************************************/
void cliWebIORedirectInit(void)
{
  if (cliWebIOLogRedirectInit()== L7_SUCCESS)
  {
    cliWebIORedirectInitialized = L7_TRUE;

    cliWebIORedirectTaskId = osapiTaskCreate("cliWebIORedirectTask",
                                             cliWebIORedirectIOTask, 0, 0,
                                             L7_DEFAULT_STACK_SIZE,
                                             CLI_IO_REDIRECT_TASK_PRIORITY,
                                             L7_DEFAULT_TASK_SLICE);

    if (cliWebIORedirectTaskId == L7_ERROR)
    {
      /* Print the error but do not return failure. The system can operate
         effectively without this.  However, no messages printed from an
         interrupt context will be displayed. */

      cliWebIORedirectTaskId = L7_NULL;

      /* Purposely use printf since the system is not initialized as of yet */
      printf("%s: %d: Failed to create redirectIOTask\n",
             __FILE__, __LINE__);

      cliWebIORedirectInitialized = L7_FALSE;
    }
  } /* cliWebIOLogRedirectInit() */
}

/**************************************************************************
*
* @purpose  globally redirect stdio to a given file descriptor
*
* @param    none
*
* @returns  none
*
* @end
*
*************************************************************************/
void cliWebIOSetGlobalIo(L7_int32 fileHandle)
{
#ifdef _L7_OS_VXWORKS_
  taskLock();
  /* ioGlobalStdSet ( 0 , fileHandle ) ;*/ /* stdin */
  ioTaskStdSet(cliWebIOTaskID, 1, fileHandle); /* stdout */
  ioTaskStdSet(cliWebIOTaskID, 2, fileHandle); /* stderr */
  logFdAdd(fileHandle); /* calls to logMsg() are also sent here */
  taskUnlock();

  /* Clear out and residual data in the  pipe */
  osapiIoctl(fileHandle, FIOFLUSH, 0);

  CLI_WEB_IOREDIRECT_TRACE("IO Redirect: OS Global stdout and stderr set to %d\n", fileHandle);
#endif

#ifdef _L7_OS_LINUX_

  if (cliWebIORedirectFd == -1)
  {
    /* open up the pipe to write to */
    cliWebIORedirectFd = open(CLI_IO_REDIRECT_PIPE_NAME, O_WRONLY);
  }

  if (cliWebIORedirectFd != -1)
  {
    /* flush and then reopen stdout/stderr on the pipe */
    fflush(stdout);
    dup2(cliWebIORedirectFd, fileno(stdout));
    fflush(stderr);
    dup2(cliWebIORedirectFd, fileno(stderr));
  }
#endif
}

/**************************************************************************
*
* @purpose  globally redirect stdio back to std IO
*
* @param    none
*
* @returns  none
*
* @end
*
*************************************************************************/
void cliWebIOResetGlobalIo(L7_int32 fileHandle)
{
#ifdef _L7_OS_VXWORKS_
  taskLock();
  /* ioGlobalStdSet ( 0 , fileHandle ) ;*/ /* stdin */
  ioTaskStdSet(cliWebIOTaskID, 1, cliWebIOStdOutFd); /* stdout */
  ioTaskStdSet(cliWebIOTaskID, 2, cliWebIOStdErrFd); /* stderr */

  logFdDelete(fileHandle); /* calls to logMsg() are also sent here */
  taskUnlock();

  CLI_WEB_IOREDIRECT_TRACE("IO Redirect: OS Global IO Reset to stdout and stderr\n");
#endif

#ifdef _L7_OS_LINUX_
  fflush(stdout);
  dup2(cliWebIOStdOutFd, fileno(stdout));
  fflush(stderr);
  dup2(cliWebIOStdErrFd, fileno(stderr));
#endif
}

/**************************************************************************
*
* @purpose  Store stdio file descriptors
*
* @param    none
*
* @returns  none
*
* @end
*
*************************************************************************/
void cliWebIOStdIOFdStore(void)
{
#ifdef _L7_OS_VXWORKS_
  cliWebIOStdInFd  = ioGlobalStdGet ( 0 ) ; /* stdin */
  cliWebIOStdOutFd = ioGlobalStdGet(1); /* stdout */
  cliWebIOStdErrFd = ioGlobalStdGet(2); /* stderr */
  cliWebIOTaskID = taskIdSelf();
#endif

#ifdef _L7_OS_LINUX_
  cliWebIOStdInFd  = dup(fileno(stdin));   /* stdin */
  cliWebIOStdOutFd = dup(fileno(stdout));  /* stdout */
  cliWebIOStdErrFd = dup(fileno(stderr));  /* stderr */
#endif
}

int cliWebIORedirectStdInGet(void)
{
#ifdef _L7_OS_VXWORKS_
  return  consoleFd;
#else
  return cliWebIOStdInFd;
#endif
}

int cliWebIORedirectStdOutGet(void)
{
#ifdef _L7_OS_VXWORKS_
  return consoleFd;
#else
  return cliWebIOStdOutFd;
#endif
}

int cliWebIORedirectStdErrGet(void)
{
#ifdef _L7_OS_VXWORKS_
  return consoleFd;
#else
  return cliWebIOStdErrFd;
#endif
}

/**************************************************************************
*
* @purpose  Create a pipe and direct all standard output to the pipe.
*
* @param    none
*
* @returns  L7_SUCCESS  Pipe is created and output redirected.
* @returns  L7_ERROR    The pipe has not been created.
*
* @end
*
*************************************************************************/
L7_RC_t cliWebIOLogRedirectInit(void)
{
  L7_int32 pipeStat;

  /* stdout and stderr are globally redirected to pipe1, then this */
  /* task pends on that pipe and captures the output.  Create the  */
  /* pipe, then open it                                            */

  pipeStat = osapiPipeCreate (CLI_IO_REDIRECT_PIPE_NAME,CLI_IO_REDIRECT_PIPE_MAX_MSGS, CLI_IO_REDIRECT_PIPE_MSG_LEN);

  /* Do not redirect stdio until the CLI has almost completed P3 init */
  /* However, store stdio file descriptors so that they can be reused later on */
  /*  cliWebIOStdIOFdStore();*/

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Task that creates, then reads an osapiMsgQueue and prints
*           any data in the queue to the screen.
*
* @param    none
*
* @returns  L7_SUCCESS  If task runs correctly, this will never return.
* @returns  L7_ERROR    If queue fails to be created.
*
* @notes This is used with SYSAPI_PRINTF in order to solve the problem
*        with printing in interrupt context.
*
* @end
*
*************************************************************************/
L7_RC_t cliWebIORedirectIOTask(void)
{
  L7_int32        len;
  fd_set          ReadFds, *pReadFds = &ReadFds ;    /* read fds    */
  L7_char8        redirBuf[SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE];
  L7_BOOL         cliWebIODoLogRedirect;
  L7_int32        maxBytesToRead;
  L7_int32        i;
  EwsContext      context = L7_NULLPTR;

  /* Init parms */
  cliWebIODoLogRedirect = L7_FALSE;

  /* Store task stdio file descriptors for later resetting */

  /* Initialize the maximum message size to read from the IOCTL */
  maxBytesToRead = SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE-1;

  if (osapiPipeOpen(CLI_IO_REDIRECT_PIPE_NAME, O_RDONLY, 0, &cliWebIORedirectHandle) != L7_SUCCESS)
  {
    /* Purposely use printf since the system is not initialized as of yet */
    printf("%s: %d: Failed to create cliWebIORedirectHandle\n",
           __FILE__, __LINE__);
    return L7_FAILURE ;
  }

  for (;;)
  {
    FD_ZERO(pReadFds) ;
    FD_SET(cliWebIORedirectHandle, pReadFds);

    /* Read the I/O channel into the buffer. Reserve the last character in */
    /* the buffer for an EOS character so string operations work correctly */
    len = osapiRead(cliWebIORedirectHandle, &redirBuf[0], SYSAPI_PRINTF_QUEUE_MESSAGE_SIZE - 1);

    CLI_WEB_IOREDIRECT_TRACE("Post read: Number of bytes read from message in cliWebIORedirectHandle = %d\n", len);

    if (len > 0)
    {
      redirBuf[len] = '\0';
      /* Debug */
      CLI_WEB_IOREDIRECT_TRACE("Length of Message = %d", len);
      CLI_WEB_IOREDIRECT_TRACE("MSG: %s\n",redirBuf);
      /*... write the log file, do a printf() to send to console too ...*/

      for (i = 0; i < FD_CLI_DEFAULT_MAX_CONNECTIONS; i++)
      {
        /* Always write to the console */
        /* Only write to telnet sessions if console support display is enabled for that session */
        if ((cliCommon[i].debugDisplayCtl.consoleTraceDisplayEnabled == L7_TRUE) ||
            (i == CLI_SERIAL_HANDLE_NUM))
        {
          context = cliWebLoginSessionUserStorageGet(cliCommon[i].userLoginSessionIndex);
          if (context != L7_NULL)
          {
            /* Telnet sessions require a carriage return for proper display of output,
               particularly for devshell commands  */
            if (i != CLI_SERIAL_HANDLE_NUM)
            {
              /* Add \r after all \n */
              int j; /* index into buffer to inspect */
              for (j=0; j < strlen(redirBuf); j++)
              {
                if (redirBuf[j] == '\n' && (j == 0 || redirBuf[j-1] != '\r'))
                {
                  if(0 > write(context->net_handle->socket, "\r", 1)){}
                }
                if(0 > write(context->net_handle->socket, &redirBuf[j], 1)){}
              }
            }
            else
            {
              /* Display the message */
              if(0 > write(cliWebIOStdOutFd, redirBuf, len)){}
            }
          }
        }
      }
    }
  }  /* for (;;) */

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Redirect IO to the console and support enabled login sessions
*
* @param    none
*
* @returns  L7_SUCCESS  If task runs correctly, this will never return.
* @returns  L7_ERROR    If queue fails to be created.
*
* @notes
*
* @end
*
*************************************************************************/
void cliWebIORedirectIoToCLI(void)
{
  if (cliWebIORedirectInitialized == L7_TRUE)
  {
    cliWebIOSetGlobalIo(cliWebIORedirectHandle);
    redirectTaskNeedsToBeAssigned = L7_TRUE;

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "Redirecting IO to CLI Redirect Task\n");
  }
}

/**************************************************************************
*
* @purpose  Redirect IO from console and support enabled login sessions
*           back to standard I/O
*
* @param    none
*
* @returns  L7_SUCCESS  If task runs correctly, this will never return.
* @returns  L7_ERROR    If queue fails to be created.
*
* @notes
*
* @end
*
*************************************************************************/
void cliWebIORedirectIoToStdIO(void)
{
  if (cliWebIORedirectInitialized == L7_TRUE)
  {
    cliWebIOResetGlobalIo(cliWebIORedirectHandle);
    redirectTaskNeedsToBeReassigned = L7_TRUE;

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "Redirecting IO to stdout\n");
  }
}

/*********************************************************************
*
* @purpose    Begin IO redirection to the CLI
*
* @param      void
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t cliWebIoRedirectStart(void)
{
  /*
    Redirect stdout in order to allow for this display to
    be directed by the CLI to any login session.

    The redirection of stdout must wait both
    - for phase 3 init of the CLI to be completed, and
    - for the first user login.

    If redirection is done earlier, the password prompt will not
    appear on the serial port after the user logs in.

    Additionally, console and telnet hangs may occur.

    Do this redirection only once per instance of reboot.
    Redirection is not needed for every login session.
  */

  if (cliIoRedirectToCliGet() != L7_TRUE)
  {
    cliIoRedirectToCliSet(L7_TRUE);
    cliWebIORedirectIoToCLI();

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "IO Redirection To CLI Succeeded\n");
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose    End IO redirection to the CLI
*
* @param      void
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t cliWebIoRedirectStop(void)
{
  if (cliIoRedirectToCliGet() != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "IO Redirection To Stdout Failed\n");
    return L7_FAILURE;
  }
  /* Restore CLI redirection */
  cliWebIORedirectIoToStdIO();
  cliIoRedirectToCliSet(L7_FALSE);
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
          "IO Redirection To Stdout Succeeded\n");

  return L7_SUCCESS;
}

/*============================================================================*/
/*========================  START OF CLI WEB TRACE ROUTINES  =================*/
/*============================================================================*/

/**************************************************************************
*
* @purpose  Enable/Disable all CLI Web Trace Modes
*
* @param    mode    L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t cliWebDebugTraceModeSetAll(L7_uint32 mode)
{
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }
  cliWebDebugTraceModeSet(mode);
  cliWebDebugIoRedirectTraceModeSet(mode);

  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Enable/Disable Global CLI Web Trace Mode
*
* @param    mode    L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t cliWebDebugTraceModeSet(L7_uint32 mode)
{
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  cliWebDebugTraceMode = mode;
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get Global CLI Web Trace Mode
*
* @param    void
*
* @returns  L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 cliWebDebugTraceModeGet(void)
{
  return cliWebDebugTraceMode;
}

/**************************************************************************
*
* @purpose  Enable/Disable CLI Web IO Redirect Trace Mode
*
* @param    mode    L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t cliWebDebugIoRedirectTraceModeSet(L7_uint32 mode)
{
  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
    return L7_FAILURE;
  }

  cliWebDebugIoRedirectTraceMode = mode;
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Get CLI Web IO Redirect Trace Mode
*
* @param    void
*
* @returns  L7_ENABLE or L7_DISABLE
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 cliWebDebugIoRedirectTraceModeGet(void)
{
  return cliWebDebugIoRedirectTraceMode;
}

/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/

#include "sysapi.h"

static void cliWebCfgShow(void);

/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void cliWebBuildTestConfigData(L7_uint32 ver)
{
  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

  cliWebCfgData.systemWebMode = FD_CLI_WEB_MODE + 1;
  cliWebCfgData.userPrompt = L7_FALSE + 1;
  memcpy(cliWebCfgData.systemCommandPrompt, "TEST PROMPT", L7_PROMPT_SIZE - 1);
  cliWebCfgData.systemTelnetNewSessions = FD_CLI_WEB_TELNET_NEW_SESSIONS + 1;
  cliWebCfgData.systemTelnetNumSessions = FD_CLI_WEB_DEFAULT_NUM_SESSIONS + 1;
  cliWebCfgData.systemTelnetTimeout = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT + 1;
  cliWebCfgData.systemTelnetAdminMode   = L7_DISABLE;
  cliWebCfgData.systemJavaMode = L7_DISABLE;
  bzero(cliWebCfgData.cliBanner, sizeof(cliWebCfgData.cliBanner));
  cliWebCfgData.systemTelnetPortNum = 7000;
  cliWebCfgData.httpListenPortNum   = 8080;

  /* End of Component's Test Non-default configuration Data */

  /* Force write of config file */
  cliWebCfgData.cfgHdr.dataChanged = L7_TRUE;
  sysapiPrintf("Built test config data\n");
}

/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void cliWebCfgShow(void)
{
  /*--------------------------------*/
  /* cfgParms                       */
  /*                                */
  /* DO A RAW DUMP OF THE CONFIG    */
  /* FILE IF A PREVIOUSLY EXISTING  */
  /* DEBUG DUMP ROUTINE DOES NOT    */
  /* EXIST                          */
  /*--------------------------------*/
  sysapiPrintf("cliWebCfgData.systemWebMode = %d\n", cliWebCfgData.systemWebMode);
  sysapiPrintf("cliWebCfgData.userPrompt = %d\n", cliWebCfgData.userPrompt);
  sysapiPrintf("cliWebCfgData.systemTelnetNewSessions = %d\n", cliWebCfgData.systemTelnetNewSessions);
  sysapiPrintf("cliWebCfgData.systemTelnetNumSessions = %d\n", cliWebCfgData.systemTelnetNumSessions);
  sysapiPrintf("cliWebCfgData.systemTelnetTimeout = %d\n", cliWebCfgData.systemTelnetTimeout);
  sysapiPrintf("cliWebCfgData.systemTelnetAdminMode = %d\n", cliWebCfgData.systemTelnetAdminMode);
  sysapiPrintf("cliWebCfgData.systemJavaMode = %d\n", cliWebCfgData.systemJavaMode);
  sysapiPrintf("cliWebCfgData.systemTelnetPortNum = %d\n", cliWebCfgData.systemTelnetPortNum);
  sysapiPrintf("cliWebCfgData.httpListenPortNum = %d\n", cliWebCfgData.httpListenPortNum);

  sysapiPrintf("\n");
}

/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void cliWebConfigDataTestShow(void)
{
  L7_fileHdr_t  *pFileHdr;

  /*-----------------------------*/
  /* Config File Header Contents */
  /*-----------------------------*/
  pFileHdr = &(cliWebCfgData.cfgHdr);

  sysapiCfgFileHeaderDump (pFileHdr);

  /*--------------------------------*/
  /* cfgParms                       */
  /*--------------------------------*/

  cliWebCfgShow();

  /*-------------------------------*/
  /* Scaling Constants             */
  /*                               */
  /*                               */
  /* NOTE:                         */
  /* Print any limits of arrays    */
  /* or anything else in the       */
  /* confi files which might       */
  /* affect the size of the config */
  /* file.                         */
  /*                               */
  /*-------------------------------*/

  sysapiPrintf( "Scaling Constants\n");
  sysapiPrintf( "-----------------\n");

  sysapiPrintf( "L7_PROMPT_SIZE - %d\n", L7_PROMPT_SIZE);
  sysapiPrintf( "BUF_BANNER_DISPLAY_SIZE - %d\n", BUF_BANNER_DISPLAY_SIZE);

  /*-----------------------------*/
  /* Checksum                    */
  /*-----------------------------*/
  sysapiPrintf("cliWebCfgData.checkSum : %u\n", cliWebCfgData.checkSum);
}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

#ifdef LVL7_DEBUG_BREAKIN
/************************************SERVICEABILITY*****************************/
/*********************************************************************
* @purpose  Sets  console breakin string.
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS
*           L7_FAILURE if password is too long or index is too large
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t  cliWebConsoleBreakinstringSet(L7_char8 *password)
{
  L7_RC_t rc = L7_FAILURE;
  if (osapiConsoleBreakinstringSet(password) == L7_SUCCESS)
  {
    strncpy(localString, password, sizeof(localString));
    rc = L7_SUCCESS;
    return rc;
  }
  return rc;
}

/*********************************************************************
* @purpose  Apply console break-in config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t consoleBreakinApplyConfigData(void)
{
  osapiConsoleBreakinstringSet(consoleBreakinCfg.cfg.breakinString);
  osapiConsoleBreakinModeSet(consoleBreakinCfg.cfg.consoleBreakinFlag);

  consoleBreakinCfg.hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/***********************************************************************
*@purpose  Enable/Disable console break-in
*
* @param    enable_flag - L7_TRUE.
*                         L7_FALSE.
* @param    session     - CLI login session
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t cliWebConsoleBreakinModeSet(L7_BOOL enable_flag)
{
  L7_RC_t rc = L7_FAILURE;

  if (osapiConsoleBreakinModeSet(enable_flag) == L7_SUCCESS)
  {
    breakin_flag = enable_flag;
    rc = L7_SUCCESS;
    return rc;
  }
  return rc;
}

/*********************************************************************
* @purpose  Gets the console break-in mode setting for selected session.
*
*
*
* @returns  L7_TRUE -  display of Debug Console Mode Enabled.
*           L7_FALSE - display of Debug Console Mode Disabled.
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t cliWebConsoleBreakinModeGet(void)
{
  return osapiConsoleBreakinModeGet();
}

/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinRegister(void)
{
  supportDebugDescr_t supportDebugDescr;
  memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

  supportDebugDescr.componentId = L7_CLI_WEB_COMPONENT_ID;
  supportDebugDescr.userControl.notifySave = consoleBreakinSave;
  supportDebugDescr.userControl.hasDataChanged = consoleBreakinHasDataChanged;

  (void)supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
* @purpose  Saves CONSOLE BREAKIN configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    consoleBreakinCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t consoleBreakinSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  consoleBreakinCfgUpdate();

  if (consoleBreakinHasDataChanged() == L7_TRUE)
  {
    consoleBreakinCfg.hdr.dataChanged = L7_FALSE;
    consoleBreakinCfg.checksum = nvStoreCrc32((L7_uchar8 *)&consoleBreakinCfg,
                                              (L7_uint32)(sizeof(consoleBreakinCfg) - sizeof(consoleBreakinCfg.checksum)));

    /* call save NVStore routine */
    rc = sysapiSupportCfgFileWrite(L7_CLI_WEB_COMPONENT_ID,
                                   CLI_WEB_CFG_FILENAME, (L7_char8 *)&consoleBreakinCfg,
                                   (L7_uint32)sizeof(consoleBreakinCfg_t));
    if (rc == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
              "Error on call to osapiFsWrite routine on config file %s\n",
              CLI_WEB_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Save configuration settings for console break-in
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t consoleBreakinConfigSave()
{
  strcpy(consoleBreakinCfg.cfg.breakinString, localString);
  consoleBreakinCfg.cfg.consoleBreakinFlag = breakin_flag;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copy the configuration  settings to the config file
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinCfgUpdate(void)
{
  consoleBreakinConfigSave();
}

/*********************************************************************
* @purpose  Checks if console break-in config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL consoleBreakinHasDataChanged(void)
{
  return consoleBreakinCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Gets the console breakin string
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t cliWebConsoleBreakinstringGet(L7_char8 *password)
{
  L7_RC_t rc = L7_FAILURE;
  if (osapiConsoleBreakinstringGet(password) == L7_SUCCESS)
  {
    strcpy(consoleBreakinCfg.cfg.breakinString, password);
    rc = L7_SUCCESS;
    return rc;
  }
  return rc;
}

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinCfgRead(void)
{
  (void)sysapiSupportCfgFileGet(L7_CLI_WEB_COMPONENT_ID,
                                CLI_WEB_CFG_FILENAME, (L7_char8 *)&consoleBreakinCfg,
                                (L7_uint32)sizeof(consoleBreakinCfg_t), &consoleBreakinCfg.checksum,
                                CLI_WEB_CFG_VER_CURRENT, consoleBreakinBuildDefaultConfigData, L7_NULL);

  consoleBreakinCfg.hdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build default console break-in config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void consoleBreakinBuildDefaultConfigData(L7_uint32 ver)
{
  /* setup file header */
  consoleBreakinCfg.hdr.version = ver;
  consoleBreakinCfg.hdr.componentID = L7_CLI_WEB_COMPONENT_ID;
  consoleBreakinCfg.hdr.type = L7_CFG_DATA;
  consoleBreakinCfg.hdr.length = (L7_uint32)sizeof(consoleBreakinCfg_t);
  strcpy((L7_char8 *)consoleBreakinCfg.hdr.filename, CLI_WEB_CFG_FILENAME);
  consoleBreakinCfg.hdr.dataChanged = L7_FALSE;

  /* set the default values*/
  strcpy(consoleBreakinCfg.cfg.breakinString, CLI_SUPPORTCONSOLE_DEFAULTBREAKIN);
  consoleBreakinCfg.cfg.consoleBreakinFlag = L7_TRUE;
}

#endif

/*********************************************************************
* @purpose  To change the listening port of the telnet server
*
* @param    port - port on which the telnet runs
*
* @returns  L7_SUCCESS
*           L7_ADDR_INUSE If failed to bind since address given is already in use
*           L7_FAILURE
*
* @notes    Closes the existing listening socket and opens the listening socket and puts it
*                into the Listening state.
*
* @end
*********************************************************************/
L7_RC_t cliWebTelnetPortUpdate(void)
{
  L7_RC_t rc = L7_SUCCESS;

  rc = ewaNetTelnetSocketClose();

  osapiSleep(1);

  rc = ewaNetTelnetSocketActivate();

  return rc;
}

/*********************************************************************
* @purpose  To change the listening port of the http server
*
* @param    port - port on which the http runs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Closes the existing listening socet and opens the listening socket and puts it
*                into the Listening state.
*
* @end
*********************************************************************/
L7_RC_t cliWebHttpPortUpdate(L7_uint32 port)
{
  return ewaNetHttpPortSet(port);
}
