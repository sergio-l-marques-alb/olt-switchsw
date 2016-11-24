
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_txtcfg.c
 *
 * @purpose   Text Based Configutaion Support
 *
 * @component CLI
 *
 * @comments  none
 *
 * @create    08/11/2006
 *
 * @create    Rama Sasthri, Kristipati
 *
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include <string.h>
#include <stdarg.h>

#include "ewnet.h"
#include "cliapi.h"
#include "cli_config_script.h"
#include "nvstoreapi.h"
#include "cardmgr_api.h"
#include "cli_web_user_mgmt.h"
#include "cli_web_util.h"
#include "cli_txtcfg_util.h"
#include "dll_api.h"
#include "usmdb_dhcps_api.h"
#include "osapi.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"
#include "cli_txt_cfg_api.h"
#include "unitmgr_api.h"

#define CLI_TXT_CFG_PASS_GLOB 0x10
#define CLI_TXT_CFG_PASS_INTF 0x11

#define CLI_TXT_CFG_DBGFLAG_NONE 0x00
#define CLI_TXT_CFG_DBGFLAG_APPLY 0x10
#define CLI_TXT_CFG_DBGFLAG_RCALL 0x20

/*
 * Work Area
 */
typedef struct
{
  L7_BOOL globalCfgDone;
  L7_BOOL intfCfgDone;
  L7_BOOL doNotApplyCfg;/* This flag is set when we did clear configuration,
                           then it will prevent form applying configuration. */

  void * syncSema;
  struct EwaNetHandle_s ewaNetHandle;
  struct EwsContext_s ewsContext;
  struct EwsTelnet_s ewsTelnet;

  cliCommon_t cliCommonSave;
  loginSessionsInfo_t loginInfoSave;
  EwsCliCommandP commandTreeSave;
  L7_uint32 currentModeSave;
  L7_uint32 currentHandleSave;
  L7_int32 emwebPipe;

  L7_BOOL doCompress;
  L7_BOOL doProf;
  L7_BOOL showDef;
  L7_uint32 dbgFlag;

  cliTxtCfgCmd_t cmd;

  L7_uint32 skipExit;
  L7_uint32 failCount;
  L7_uint32 passCount;
} cliTxtCfgWa_t;
typedef struct
{
  struct EwaNetHandle_s ewaNetHandle;
  struct EwsContext_s ewsContext;
  struct EwsTelnet_s ewsTelnet;
}cliTxtRunCfg_t;

cliTxtRunCfg_t *cliTxtRunCfg;

typedef struct
{
  L7_dll_member_t link;
  L7_uint32 sec;
  L7_uint32 nsec;
  L7_uint32 type;
  L7_char8 status[8];
  L7_char8 cmd[1];
} cliTxtCfgCmdTimeRecord_t;

static L7_uchar8 cliTxtCfgDebugFlag;

#define TRACE cliTxtCfgTrace
void cliTxtCfgTrace (const L7_char8 * fmt, ...);

/*
 * externally defined functions not in a header file.
 */
extern L7_RC_t cnfgrApiSystemStateGet (L7_CNFGR_STATE_t * currentState);
extern L7_uint32 cliGetCurrentMode (void);
L7_RC_t cliTxtCfgGenerateFile (L7_char8 * filename, L7_BOOL useComp);

/*
 * Externally defined global variables
 */
extern cliWebCnfgrState_t cliWebCnfgrState;
extern loginSessionsInfo_t loginInfo[FD_CLI_DEFAULT_MAX_CONNECTIONS];

/*
 * Local variabled for the file
 */
static cliTxtCfgWa_t * cliTxtCfgWap = L7_NULLPTR;

/*
* Global array to hold the interested components info to call back
* on text based config apply completion
*/
txtCfgApplyNotifyList_t txtCfgNotifyList[L7_LAST_COMPONENT_ID];
void * cliTxtGenerateSema;
void cliTxtCfgSetDebugFlag(L7_uchar8 flag)
{
  cliTxtCfgDebugFlag = flag;
}

void cliTxtCfgSetConfigFlag(L7_BOOL flag)
{
  cliTxtCfgWap->doNotApplyCfg = flag;
}

L7_BOOL cliTxtCfgGetConfigFlag(void)
{
  return (cliTxtCfgWap->doNotApplyCfg);
}


/*********************************************************************
* @purpose  This function set up the sesson for executing the CLI
   commands of text based configuration by backup the current
   session handles and fills the text configuration handles.
*
* @param    void
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
static void cliTxtCfgSetupSession (void)
{
  L7_uint32 sesnum, handnum;
  EwsContext context = &cliTxtCfgWap->ewsContext;

  cliTxtCfgWap->currentHandleSave = cliCurrentHandleGet ();
  cliCurrentHandleSet (CLI_SERIAL_HANDLE_NUM);

  /* backup current info */
  sesnum = 0;
  handnum = CLI_SERIAL_HANDLE_NUM;
  memcpy (&cliTxtCfgWap->cliCommonSave, &cliCommon[handnum], sizeof (cliCommon_t));
  memcpy (&cliTxtCfgWap->loginInfoSave, &loginInfo[sesnum], sizeof (loginSessionsInfo_t));
  cliTxtCfgWap->commandTreeSave = mainMenu[handnum];
  cliTxtCfgWap->currentModeSave = cliGetCurrentMode();
  cliTxtCfgWap->skipExit = 0;

  cliScrollSet (L7_FALSE);
  cliUserAccessSet (L7_LOGIN_ACCESS_READ_WRITE);
  cliWebLoginSessionUserSet (sesnum, pStrInfo_base_LoginUsrId);
  cliLoginSessionIndexSet (sesnum);

  cliCommandsInit ();
  cliChangeMode (L7_PRIVILEGE_USER_MODE);

  context->unbufferedWrite = TRUE;
  context->scriptActionFlag = L7_EXECUTE_SCRIPT;
  context->configScriptStateFlag = L7_CONFIG_SCRIPT_RUNNING;
  cliCmdScrollSet (L7_FALSE);
}

/*********************************************************************
* @purpose This function restores the previously taken backup config.
*
* @param    void
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
static void cliTxtCfgRestoreSession (void)
{
  L7_uint32 sesnum, handnum;
  EwsContext context = &cliTxtCfgWap->ewsContext;

  sesnum = 0;
  handnum = CLI_SERIAL_HANDLE_NUM;
  context->configScriptStateFlag = L7_CONFIG_SCRIPT_NOT_RUNNING;
  memcpy (&cliCommon[handnum], &cliTxtCfgWap->cliCommonSave, sizeof (cliCommon_t));
  memcpy (&loginInfo[sesnum], &cliTxtCfgWap->loginInfoSave, sizeof (loginSessionsInfo_t));
  cliChangeMode (cliTxtCfgWap->currentModeSave);
  cliSetTree (cliTxtCfgWap->commandTreeSave);
  cliCurrentHandleSet (cliTxtCfgWap->currentHandleSave);
}

/*
 * cliTxtCfgCmdTimeRecordCmp:
 *    Compare time records
 */
static L7_int32 cliTxtCfgCmdTimeRecordCmp (cliTxtCfgCmdTimeRecord_t * rp1,
                                           cliTxtCfgCmdTimeRecord_t * rp2)
{
  return (rp1->sec * 1000 * 1000 + rp1->nsec >= rp2->sec * 1000 * 1000 + rp2->nsec) ? 1 : -1;
}

/*
 * cliTxtCfgRecordTime:
 *    Creates sorted records of command executaion times
 */
static void cliTxtCfgRecordTime (L7_char8 * cmd, L7_char8 * status, L7_clocktime * ct1,
                                 L7_clocktime * ct2)
{
  static L7_dll_t * dll = L7_NULLPTR;
  cliTxtCfgCmdTimeRecord_t * rp;
  L7_uint32 len;
  L7_uint32 total;
  L7_RC_t rc;

  if (dll == L7_NULLPTR)
  {
    DLLCreate (&dll);
  }

  if (cmd == (L7_char8 *) 0)
  {
    total = 0;
    rc = DLLFirstGet (dll, (void *) &rp);
    while (rc == L7_SUCCESS)
    {
      printf ("%s: %u.%09u: %s\n", rp->status, rp->sec, rp->nsec, rp->cmd);
      rc = DLLNextGet ((void *) &rp);
      total = total + rp->sec * 1000 * 1000 * 1000 + rp->nsec;
    }
    printf ("COMPUTED Total = %u\n", total);
    return;
  }
  else if (cmd == (L7_char8 *) 1)
  {
    while (DLLTopDelete (dll, (void *) &rp) == L7_SUCCESS)
    {
      osapiFree (L7_CLI_WEB_COMPONENT_ID, rp);
    }
    return;
  }

  len = strlen (cmd) + sizeof (cliTxtCfgCmdTimeRecord_t);
  if (L7_NULLPTR == (rp = (cliTxtCfgCmdTimeRecord_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID, len)))
  {
    printf ("osapiMalloc %d failed\n", len);
    return;
  }
  if (ct2->nanoseconds > ct1->nanoseconds)
  {
    rp->sec = ct2->seconds - ct1->seconds;
    rp->nsec = ct2->nanoseconds - ct1->nanoseconds;
  }
  else
  {
    rp->sec = ct2->seconds - ct1->seconds - 1;
    rp->nsec = 1000000000 + ct2->nanoseconds - ct1->nanoseconds;
  }
  strcpy (rp->cmd, cmd);
  OSAPI_STRNCPY_SAFE (rp->status, status);
  DLLInsert (dll, (L7_dll_member_t * *) &rp, (void *) cliTxtCfgCmdTimeRecordCmp);
}

/*********************************************************************
* @purpose  This function executes commands of text based
*           configuration and tell status(pass or fail) of the command
*
* @param    L7_char8*  command  command to be executed.
* @param    EwsContext context  EmWeb context
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
static void cliTxtCfgCmdExecute (EwsContext context, L7_char8 * command)
{
  L7_clocktime ct1, ct2;
  L7_char8 * status;

  L7_uint32 preMode = cliGetCurrentMode ();

  cliProcessCharInput (command);

  context->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  if (cliTxtCfgWap->doProf == L7_TRUE)
  {
    osapiClockTimeRaw (&ct1);
    ewsCliDataConfigScript2 (context, command, mainMenu[cliCurrentHandleGet ()],
                             &cliTxtCfgWap->skipExit);
    osapiClockTimeRaw (&ct2);
  }
  else
  {
    if(cliGetStringInputID() != CLI_INPUT_EMPTY)
    {
      L7_uint32 i=0, len=0;
      len = strlen(command);
      for(i=0;i<len;i++)
        cliProcessStringInput(command+i);
    }
    ewsCliDataConfigScript2 (context, command, mainMenu[cliCurrentHandleGet ()],
                             &cliTxtCfgWap->skipExit);
  }

  if (context->actionFunctionStatusFlag == L7_ACTION_FUNC_FAILED)
  {
    status = cliTxtCfgWap->skipExit ? pStrInfo_base_Skip : pStrInfo_common_Fail;
    LOG_MSG ("FAIL CMD: '%s'\n", command);
    cliTxtCfgWap->failCount++;
  }
  else
  {
    status = pStrInfo_base_Pass_1;
    cliTxtCfgWap->passCount++;
  }

  TRACE ("%s CMD: '%s' Mode = %d - %d\n", status, command, preMode, cliGetCurrentMode ());
  if (cliTxtCfgWap->doProf == L7_TRUE)
  {
    cliTxtCfgRecordTime (command, status, &ct1, &ct2);
  }
}

/*********************************************************************
* @purpose  This function applies the text configuration to the switch
*           pass by pass
*
* @param    L7_char8* filename  text configuration file name
* @param    L7_uint32 pass      pass(Global or interface pass)
*
* @returns  rc
*
* @notes none
*
* @end
*********************************************************************/
static L7_RC_t cliTxtCfgApply (L7_uint32 pass, L7_char8 * filename)
{
  L7_char8 * command = L7_NULLPTR;
  L7_char8 memberCmd[16];
  L7_uint32 memberCmdLen;
  L7_int32 fd = 0;

  cliTxtCfgCmd_t * cmdp = &cliTxtCfgWap->cmd;
  EwsContext context = &cliTxtCfgWap->ewsContext;

  TRACE ("cliTxtCfgApply: pass = %d\n", pass);

  if (cliGlobalConfigurationSkipGet () == L7_TRUE && cliTxtCfgWap->dbgFlag != CLI_TXT_CFG_DBGFLAG_APPLY)
  {
    cliGlobalConfigurationSkipSet(L7_FALSE);
    return L7_SUCCESS;
  }

  /* Setup the session info */
  cliTxtCfgSetupSession ();

  /* note down the special cases cmd lengths - don't need to compute for every compare */
  /*  filename = filename ? filename : sysapiTxtCfgFileGet ();*/
  if (umSwitchoverConditionGet())
  {
    filename = sysapiTxtRunCfgFileGet();
    /*check if this file exists*/
    fd = osapiFsOpen(filename);
    if (fd != L7_ERROR)
    {
      osapiFsClose(fd);
    }
    else
    {
      /*running config does not exist continue with the startup-config*/
      filename = sysapiTxtCfgFileGet();
    }
  }
  else
  {
    filename = sysapiTxtCfgFileGet();
  }

  osapiSnprintf(memberCmd, sizeof(memberCmd), "%s ", pStrInfo_common_Mbr);
  memberCmdLen = strlen (memberCmd);

  /* open the file and initalise the strutures */
  cliTxtCfgCmdInit (filename, cmdp);

  /* Get the commands until end of file */
  while (L7_NULLPTR != (command = cliTxtCfgCmdGet (cmdp, L7_TRUE)))
  {
    if (pass == CLI_TXT_CFG_PASS_INTF)
    {
      /* Execute all the commands in interface phase */
      cliTxtCfgCmdExecute (context, command);
      continue;
    }

    /* Execute stack member commands in global mode */
    if (!strncmp (command, memberCmd, memberCmdLen))
    {
      /* change the mode to stack mode to execute the command */
      cliChangeMode (L7_STACK_MODE);
      cliTxtCfgCmdExecute (context, command);
      continue;
    }
  }
  /* close the file */
  cliTxtCfgCmdEnd (cmdp);

  /* Restore the backed session info */
  cliTxtCfgRestoreSession ();

#ifdef L7_DHCPS_PACKAGE
  if (pass == CLI_TXT_CFG_PASS_INTF)
  {
#ifdef L7_DHCPS_PACKAGE
    usmDbDhcpsInitMapLeaseConfigData ();
#endif
  }
#endif

  /*
   * Tell each component to save its configuration.  It won't actually save it
   * since the binary configuration save functions are now a noop.  However,
   * it will cause each component to reset its "configuration changed" flag.
   */
  nvStoreComponentSaveAll ();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function is registered with nvStore to get called to
*           save the configuration.
*
* @param    L7_char8 *filename  file to save the config to
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/
void cliTxtCfgSave (L7_char8 *filename)
{
  if (L7_TRUE == cliTxtCfgWap->intfCfgDone)
  {
    cliTxtCfgGenerateFile (filename, cliTxtCfgWap->doCompress);
  }
}

/*********************************************************************
* @purpose  This function wakeup the EmWeb task to get the faster
*           response.
*
* @param    void
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
static void cliTxtCfgWakeEmweb (void)
{
  L7_char8 one = 1;
  write (cliTxtCfgWap->emwebPipe, &one, 1);
}

/*********************************************************************
* @purpose  This function registered with cardmgr to get called
*           before and after ports creation. This function tells when
*           interface and global configuaiton should be applied.
*
* @param    L7_uint32 event Kind of event
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
static void cliTxtCfgCardConfigCallback (L7_uint32 event)
{
  TRACE ("\ncliTxtCfgCardConfigCallback: event = %d\n", event);

  if (cliGlobalConfigurationSkipGet () == L7_TRUE)
  {
    TRACE ("Configuration is invalidated so don't apply\n");
    return;
  }

  if (event == 0)
  {
    cliTxtCfgWap->globalCfgDone = L7_FALSE;
  }
  else if (event == 1)
  {
    cliTxtCfgWap->intfCfgDone = L7_FALSE;
  }

  /* wake up emweb */
  cliTxtCfgWakeEmweb ();

  /* Wait for emweb to apply the config */
  osapiSemaTake (cliTxtCfgWap->syncSema, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  This function sets CLI command prompt and refresh with
*           updated prompt .
*
* @param    L7_char8* prompt   text configuration file name
* @param    L7_BOOL   refresh  tells prompt should be refreshed or not
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
static void cliTxtCfgLoginPromptSet (L7_char8 * prompt, L7_BOOL refresh)
{
  osapiSnprintf (cliUtil.loginUserPrompt, sizeof (cliUtil.loginUserPrompt),
                 "%s%s", pStrInfo_common_CrLf, prompt);
  if (refresh == L7_TRUE)
  {
    cliWriteSerial (pStrInfo_common_CrLf);
    cliWriteSerial (cliUtil.loginUserPrompt);
  }
}

/*********************************************************************
* @purpose  This function writes text based configuraiton into file
*           block by block then write into flash.
*
* @param    L7_char8* filename     text configuration file name
* @param    L7_ConfigScript_t pCfgScr  buffer
* @param    L7_BOOL   useComp      compress/nocompress
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/
static L7_RC_t cliTxtCfgWriteFile (L7_char8 * filename, L7_ConfigScript_t * pCfgScr,
                                   L7_BOOL useComp)
{
  L7_uint32 scrLen, scrLenComp;
  L7_int32 fd;
  L7_uint32 i;
  L7_char8 * cp;
  cfgCommand_t * cmdp;
  L7_uint32 length;
  L7_uint32 count;
  L7_char8 cmdString[CONFIG_SCRIPT_MAX_COMMAND_SIZE];

  (void) osapiFsDeleteFile (filename);
  if (L7_SUCCESS != osapiFsFileCreate (filename, &fd))
  {
    LOG_MSG ("Failed to create '%s'\n", filename);
    return L7_ERROR;
  }

  scrLen = scrLenComp = 0;
  count = 0;
  for (cp = cmdString, cmdp = pCfgScr->cfgData; cmdp; cmdp = cmdp->next)
  {
    cmdString[0] = 0;
    cp = cmdp->cmdString;
    for (i = 0; i < cmdp->noOfCmds; i++)
    {
      if (cp[0] != 0 && cp[0] != ' ')
      {
        sprintf (cmdString, "%s%s\n", cmdString, cp);
        count++;
      }
      cp = cp + strlen (cp) + 1;
    }
    length = strlen (cmdString);
    scrLen += length;

    if (L7_SUCCESS != cliTxtCfgWriteBlock (fd, cmdString, &length, useComp))
    {
      LOG_MSG ("Failed to write command block\n");
    }
    scrLenComp += length;
  }
  LOG_MSG ("Text Configuration of length %d (%d CMDS) compressed to save %d%% Flash\n",
           scrLen, count, ((scrLen - scrLenComp) * 100) / scrLen);

  return osapiFsClose (fd);
}

/*********************************************************************
* @purpose  To genrate the text config and write to flash
*
* @param    filename     text configuration file name
* @param    useComp      compress/nocompress
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliTxtCfgGenerateFile (L7_char8 * filename, L7_BOOL useComp)
{
  L7_ConfigScript_t cfgScrData;
  EwsContext context = L7_NULLPTR;
  L7_uint32       unit;
  L7_uchar8       buf[80];

  unit = cliGetUnitId();

  resetLastError(); /* Reset previous error if any */

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if ((strcmp(filename,sysapiTxtRunCfgFileGet()) == 0) &&
      (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_RUN_CFG_FEATURE_ID) != L7_TRUE))
  {
    return L7_SUCCESS;
  }

  /* Semaphore here is neccessary as this routine can be called from two paths
   * User Save config path and periodic run cfg path
   */

  osapiSemaTake(cliTxtGenerateSema, L7_WAIT_FOREVER);

  if (strcmp(filename,sysapiTxtRunCfgFileGet()) == 0 )
  {
    context = &cliTxtRunCfg->ewsContext;
    context->showRunningDefaultFlag = L7_HIDE_DEFAULT;
  }
  else
  {
    context = &cliTxtCfgWap->ewsContext;
    context->showRunningDefaultFlag = cliTxtCfgWap->showDef;
  }
  filename = filename ? filename : pStrInfo_base_DbgcfgScr;

  while (L7_MAX_UNITS_PER_STACK + 1 == cliGetUnitId ())
  {
    osapiSleepMSec (250);
  }

  
  context->showRunningOutputFlag = L7_WRITE_CONFIG_SCRIPT;
  context->unbufferedWrite = L7_TRUE;
  context->configScriptData = &cfgScrData;
  (void) initialiseScriptBuffer (filename, &cfgScrData);
  (void) cliShowRunningConfigExecute (context);
  #if 0
  if (strcmp(filename,sysapiTxtRunCfgFileGet()) == 0 )
  {
    cliTxtCfgWriteFileToBuffer(filename,&cfgScrData,useComp);
  }
  else
  #endif
  {
    cliTxtCfgWriteFile (filename, &cfgScrData, useComp);
  }
  releaseScriptBuffer (&cfgScrData);

  osapiSnprintf (buf, sizeof(buf),
                 "\r\nConfig file '%s' created successfully %s.\r\n",
                 filename, getLastError ());
  cliWrite (buf);
  context->unbufferedWrite = L7_FALSE;
  osapiSemaGive(cliTxtGenerateSema);
  

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To initialise text config feature in configurator phases
*
* @param    phase      1/2/3
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliTxtCfgInit (L7_uint32 phase)
{
  L7_uint32       unit;

  unit = cliGetUnitId();

  TRACE ("cliTxtCfgInit: phase = %d\n", phase);

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  if (cliTxtCfgWap == L7_NULLPTR)
  {
    cliTxtCfgWap = (cliTxtCfgWa_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID, sizeof (*cliTxtCfgWap));
    if (cliTxtCfgWap == L7_NULLPTR)
    {
      LOG_ERROR (0);
      return L7_ERROR;
    }
    cliTxtCfgWap->doCompress = L7_TRUE;
    cliTxtCfgWap->doProf = L7_FALSE;
    cliTxtCfgWap->showDef = L7_HIDE_DEFAULT;

    cliTxtCfgWap->ewsContext.telnet = &cliTxtCfgWap->ewsTelnet;
    cliTxtCfgWap->ewsContext.net_handle = &cliTxtCfgWap->ewaNetHandle;
    cliTxtCfgWap->ewaNetHandle.context = &cliTxtCfgWap->ewsContext;
    cliTxtCfgWap->ewaNetHandle.handleNum = CLI_SERIAL_HANDLE_NUM;
    cliTxtCfgWap->ewaNetHandle.discard = L7_TRUE;
    cliTxtCfgWap->emwebPipe = -1;

    cliTxtCfgWap->globalCfgDone = L7_TRUE;
    cliTxtCfgWap->intfCfgDone = L7_TRUE;
    cliTxtCfgWap->doNotApplyCfg = L7_FALSE;

    cliTxtCfgWap->syncSema = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_EMPTY);
    cliTxtRunCfg = (cliTxtRunCfg_t *)osapiMalloc(L7_CLI_WEB_COMPONENT_ID, sizeof(cliTxtRunCfg_t));
    if (cliTxtRunCfg == L7_NULLPTR)
    {
      LOG_ERROR (0);
      return L7_ERROR;
    }
    memset(cliTxtRunCfg, 0, sizeof(cliTxtRunCfg_t));
    cliTxtRunCfg->ewsContext.telnet = &cliTxtRunCfg->ewsTelnet;
    cliTxtRunCfg->ewsContext.net_handle = &cliTxtRunCfg->ewaNetHandle;
    cliTxtRunCfg->ewaNetHandle.context = &cliTxtRunCfg->ewsContext;
    cliTxtGenerateSema = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);

  }

  if (phase == 2)
  {
    /* Initailize the txtCfgNotifyList */
    memset ((void *)&txtCfgNotifyList,0,
                 sizeof(txtCfgNotifyList));

    nvStoreTxtCfgRegister (cliTxtCfgSave);
    cliTxtCfgLoginPromptSet (pStrInfo_base_ApplyingCfgPleaseWait, L7_FALSE);
    cmgrRegisterCardConfig (cliTxtCfgCardConfigCallback);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Emweb task polling function
*
* @param    kick      file descriptor to write for emergency wakeup
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/
L7_int32 cliTxtCfgPoll (L7_int32 kick)
{
  L7_uint32 timeRef;
  L7_CNFGR_STATE_t systemState;
  L7_uint32       unit;

  unit = cliGetUnitId();

  /* if text config is not enabled - nothing to do */
  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) != L7_TRUE)
  {
    return -1;
  }

  /* override the pipe if emweb wants to use a different pipe as in vxworks */
  if (kick != -1 && cliTxtCfgWap->emwebPipe != kick)
  {
    cliTxtCfgWap->emwebPipe = kick;
  }

  /* wait for cliweb to be in execute state */
  if (CLI_WEB_PHASE_EXECUTE != cliWebCnfgrState)
  {
    return cliTxtCfgWap->emwebPipe;
  }

  /* wait for configurator to be in execute state */
  cnfgrApiSystemStateGet (&systemState);
  if (L7_CNFGR_STATE_E != systemState)
  {
    return cliTxtCfgWap->emwebPipe;
  }

  /* see if we already completed global config */
  if (L7_FALSE == cliTxtCfgWap->globalCfgDone)
  {
    cliTxtCfgLoginPromptSet (pStrInfo_base_ApplyingGlobalCfgPleaseWait, L7_TRUE);

    timeRef = osapiUpTimeRaw ();
    cliTxtCfgWap->failCount = cliTxtCfgWap->passCount = 0;
    cliTxtCfgApply (CLI_TXT_CFG_PASS_GLOB, L7_NULLPTR);
    timeRef = osapiUpTimeRaw () - timeRef;

    TRACE ("Global Config (%d Vs %d CMDS) took %d seconds to complete\n", cliTxtCfgWap->failCount,
           cliTxtCfgWap->passCount, timeRef);

    cliTxtCfgWap->globalCfgDone = L7_TRUE;

    /*
     * change the prompt for login
     * intentionally not refreshing here:
     * as most probably we will be back for interface config
     */
    cliTxtCfgLoginPromptSet (pStrInfo_base_LoginUsrPrompt, L7_FALSE);

    osapiSemaGive (cliTxtCfgWap->syncSema);
    cliTxtCfgWakeEmweb ();
  }

  /* see if we already completed interface config */
  if (L7_FALSE == cliTxtCfgWap->intfCfgDone)
  {
    cliTxtCfgLoginPromptSet (pStrInfo_base_ApplyingIntfCfgPleaseWait, L7_TRUE);

    timeRef = osapiUpTimeRaw ();
    cliTxtCfgWap->failCount = cliTxtCfgWap->passCount = 0;
    cliTxtCfgApply (CLI_TXT_CFG_PASS_INTF, L7_NULLPTR);
    timeRef = osapiUpTimeRaw () - timeRef;
    osapiSemaGive (cliTxtCfgWap->syncSema);

    TRACE ("Interface Config (%d Vs %d CMDS) took %d seconds to complete\n",
           cliTxtCfgWap->failCount, cliTxtCfgWap->passCount, timeRef);
    cliTxtCfgWap->intfCfgDone = L7_TRUE;
    cliTxtCfgWap->dbgFlag = CLI_TXT_CFG_DBGFLAG_NONE;

    /* Notify the interested Candidates to inform the completion of Apply config */
    {
      L7_int32 index;
      for (index=L7_FIRST_COMPONENT_ID;index<L7_LAST_COMPONENT_ID;index++)
      {
        if (txtCfgNotifyList[index].registrar_ID !=0)
        {
          txtCfgNotifyList[index].notify_txt_cfg_apply_complete(TXT_CFG_APPLY_SUCCESS);
        }
      }
    }


    /* change the prompt for login */
    cliTxtCfgLoginPromptSet (pStrInfo_base_LoginUsrPrompt, L7_TRUE);
#if defined(FEAT_METRO_CPE_V1_0)
/*Temporary workaround for defect# 91040 on customer's request
  TODO: This must be cleaned before merging to main.
*/
  {
    L7_uint32              unit_desc_id;
    if ((sysapiRegistryGet(UNIT_TYPE_ID, U32_ENTRY, (void *) &unit_desc_id) == L7_SUCCESS) &&
         (0x53115006 == unit_desc_id))
    {
        nimSetIntfAdminState(6,L7_DISABLE);
        osapiSleep (5);
        nimSetIntfAdminState(6,L7_ENABLE);
    }
  }
#endif
    cliTxtCfgWakeEmweb ();
  }

  return cliTxtCfgWap->emwebPipe;
}

/*********************************************************************
* @purpose  Copy the file to startup-config
*
* @param    filename   uncompressed file
*
* @returns rc
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliTxtCfgSaveTransConfig (L7_char8 * filename)
{
  TRACE ("cliTxtCfgSaveTransConfig: %s\n", filename);
  cliTxtCfgLoginPromptSet (pStrInfo_base_ApplyingCfgPleaseWait, L7_FALSE);
  cliTxtCfgCopyConfig (filename, L7_NULLPTR, cliTxtCfgWap->doCompress);
  usmDbUnitMgrPropagateCfg ();
  sysapiCfgFileReload (SYSAPI_CONFIG_FILENAME);
  cliGlobalConfigurationSkipSet(L7_FALSE);
  sysapiUnconfig ();
  return L7_SUCCESS;
}

/*************************** DEBUG **********************************/
/*************************** DEBUG **********************************/
/*************************** DEBUG **********************************/
void cliTxtCfgMemoryTrace (L7_char8 * entry)
{
  L7_uint32 i;
  L7_uint32 len = 0;
  L7_char8 * cp;
  cfgCommand_t * cmdp;
  static L7_ConfigScript_t logData;
  static L7_BOOL init = L7_FALSE;
  L7_char8 cmdString[CONFIG_SCRIPT_MAX_COMMAND_SIZE];

  if (entry == (L7_char8 *) 0)
  {
    for (cp = cmdString, cmdp = logData.cfgData; cmdp; cmdp = cmdp->next)
    {
      cmdString[0] = 0;
      cp = cmdp->cmdString;
      for (i = 0; i < cmdp->noOfCmds; i++)
      {
        if (cp[0] != 0 && cp[0] != ' ')
        {
          sprintf (cmdString, "%s%s\n", cmdString, cp);
        }
        cp = cp + strlen (cp) + 1;
      }
      sysapiPrintf ( cmdString);
      len += strlen (cmdString);
    }
    sysapiPrintf ("TOTAL LOG LENGTH: %d\n", len);
  }
  else if (entry == (L7_char8 *) 1)
  {
    releaseScriptBuffer (&logData);
  }
  else
  {
    if (init == L7_FALSE)
    {
      init = L7_TRUE;
      (void) initialiseScriptBuffer (pStrInfo_base_Memlog, &logData);
    }
    setNextConfigScriptCommand (entry, &logData);
  }
}

void cliTxtCfgTrace (const L7_char8 * fmt, ...)
{
  va_list ap;
  static char buffer[1024];

  if (cliTxtCfgDebugFlag == 0)
  {
    return;
  }

  memset (&ap, 0, sizeof (ap));
  memset (&buffer, 0, sizeof (buffer));
  va_start (ap, fmt);
  vsprintf (buffer, fmt, ap);
  va_end (ap);
  sysapiPrintf(buffer);

/*  cliTxtCfgMemoryTrace (buffer);*/
}

void cliTxtCfgDebugTestConfig (void)
{
  L7_uint32 i;
  L7_char8 command[1024];
  EwsContext context = &cliTxtCfgWap->ewsContext;

  cliTxtCfgSetupSession ();

  cliTxtCfgCmdExecute (context, pStrInfo_common_VlanDbase);
  for (i = 2; i <= L7_MAX_VLANS; i++)
  {
    osapiSnprintf (command, sizeof (command), pStrInfo_base_Vlan_2, i);
    cliTxtCfgCmdExecute (context, command);
    osapiSnprintf (command, sizeof (command), pStrInfo_common_VlanRouting_2, i);
    cliTxtCfgCmdExecute (context, command);
  }
  cliTxtCfgCmdExecute (context, pStrInfo_common_Exit);

  cliTxtCfgCmdExecute (context, pStrInfo_base_Cfgure_1);
  for (i = 1; i <= L7_MAX_NUM_LAG_INTF; i++)
  {
    osapiSnprintf (command, sizeof (command), pStrInfo_base_Lag_5, i);
    cliTxtCfgCmdExecute (context, command);
  }
  cliTxtCfgCmdExecute (context, pStrInfo_common_Exit);
  cliTxtCfgRestoreSession ();
}

void cliTxtCfgDebugCompSet (L7_int32 val)
{
  if (cliTxtCfgWap != L7_NULLPTR)
  {
    cliTxtCfgWap->doCompress = val ? L7_TRUE : L7_FALSE;
  }
}

void cliTxtCfgDebugShowDefSet (L7_int32 val)
{
  if (cliTxtCfgWap != L7_NULLPTR)
  {
    cliTxtCfgWap->showDef = val ? L7_SHOW_DEFAULT : L7_HIDE_DEFAULT;
  }
}

void cliTxtCfgDebugProfSet (L7_int32 val)
{
  if (cliTxtCfgWap != L7_NULLPTR)
  {
    cliTxtCfgWap->doProf = val ? L7_TRUE : L7_FALSE;
  }
}

void cliTxtCfgDebugApply (L7_uint32 dontKick)
{
  cliTxtCfgWap->globalCfgDone = L7_FALSE;
  cliTxtCfgWap->intfCfgDone = L7_FALSE;
  cliTxtCfgWap->dbgFlag = CLI_TXT_CFG_DBGFLAG_APPLY;

  if (dontKick == 0)
  {
    cliTxtCfgWakeEmweb ();
  }
}

L7_int32 cliTxtCfgDebugShowTimeRecords (L7_BOOL clr)
{
  if (clr == L7_FALSE)
  {
    cliTxtCfgRecordTime ((L7_char8 *) 0, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);
  }
  else
  {
    cliTxtCfgRecordTime ((L7_char8 *) 1, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR);
  }
  return clr;
}
/*********************************************************************
 * @purpose  Registers the interested components information to inform
 *           the config apply completion
 *
 * @param    registrar_ID   : component ID who is interested
 *           notifyFunction : The function name that will be called
 *
 * @returns  L7_SUCCESS  On proper registration
 *           L7_FAILURE  On registration failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t txtCfgApplyCompletionNotifyRegister(L7_uint32 registrar_ID,
                             L7_uint32 (*notifyFunction)(L7_uint32 event))
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("txtConfig  registrar_ID %d greater then the LAST_COMPONENT_ID\n",(L7_int32)registrar_ID);
    return(L7_FAILURE);
  }

  if (txtCfgNotifyList[registrar_ID].registrar_ID!= 0)
  {
    LOG_MSG("txtConfig Register:  registrar_ID %d already registered\n", (L7_int32)registrar_ID);
    return(L7_FAILURE);
  }

  txtCfgNotifyList[registrar_ID].registrar_ID = registrar_ID;
  txtCfgNotifyList[registrar_ID].notify_txt_cfg_apply_complete
                                  = notifyFunction;

  return(L7_SUCCESS);

}

/*********************************************************************
 * @purpose  Deregisters the interested components information to inform
 *           the config apply completion
 *
 * @param    registrar_ID   : component ID who is interested
 *           notifyFunction : The function name that will be called
 *
 * @returns  L7_SUCCESS  On proper deregistration
 *           L7_FAILURE  On deregistration failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t txtCfgApplyCompletionNotifyDeregister(L7_uint32 registrar_ID)
{
  if (registrar_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("txtConfig unregister:  registrar_ID %d greater then the LAST_COMPONENT_ID\n",(L7_int32)registrar_ID);
    return(L7_FAILURE);
  }

  if (txtCfgNotifyList[registrar_ID].registrar_ID == 0)
  {
    LOG_MSG("txtConfig unregister: registrar_ID %d already unregistered\n",(L7_int32)registrar_ID);
    return(L7_FAILURE);
  }

  memset ((void *)&txtCfgNotifyList[registrar_ID],0,
                 sizeof(txtCfgApplyNotifyList_t));

  return(L7_SUCCESS);

}

#if defined(L7_AUTO_INSTALL_PACKAGE) || defined(L7_TR069_PACKAGE)
/*********************************************************************
*
* @purpose  Function to save, validate and apply script from
*           local file system
*
* @param const L7_char8 *filename
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t cliAutoInstallConfigScriptApply(L7_char8 * filename)
{
  L7_RC_t           rc               = L7_SUCCESS;
  EwsContext        context          = &cliTxtCfgWap->ewsContext;
  L7_ConfigScript_t configScriptData;
  
  memset(&configScriptData, 0x00, sizeof(L7_ConfigScript_t));
  
  /* Setup the session info */
  cliTxtCfgSetupSession ();

  /* Download script from temporary script file to specified file and validate it */
  if(L7_SUCCESS == (rc = configScriptValidateAndDownload(context, AUTO_INSTALL_TEMP_SCRIPT_FILENAME, filename,0)))
  {
    /* Get the File data structure */
    if (L7_SUCCESS == (rc = readConfigScriptData(filename, &configScriptData)))
    {
      /***Set the Flag to Execute***/
      context->scriptActionFlag = L7_EXECUTE_SCRIPT;

      /* apply script */
      cliExecuteBuffer(context, &configScriptData);

      /* release buffer */
      releaseScriptBuffer(&configScriptData) ;
    }
  }
  
  cliTxtCfgRestoreSession ();
  /* Restore the backed session info */
 
  return rc;
}
#endif /*  defined(L7_AUTO_INSTALL_PACKAGE)  || defined(L7_TR069_PACKAGE)*/

/*********************************************************************
*
* @purpose Returns whether the configuration is to be skipped
*
*
* @param void
*
* @returntype L7_BOOL
*
* @returns    L7_FALSE  if the configuration is to be applied.
*             L7_TRUE   if the configuration is not to be applied.
*
* @end
*
*********************************************************************/
L7_BOOL cliGlobalConfigurationSkipGet( void )
{
  TRACE ("cliGlobalConfigurationSkipGet: val = %d\n", cliUtil.doNotApplyGlobalConfig);

  return( cliUtil.doNotApplyGlobalConfig );
}

/*********************************************************************
*
* @purpose Sets whether the configuration is to be skipped
*
*
* @param   skip   @((input))  L7_TRUE if config application to be skipped
*
* @returntype none
*
* @end
*
*********************************************************************/
void cliGlobalConfigurationSkipSet( L7_BOOL skip )
{
  TRACE ("cliGlobalConfigurationSkipSet: val = %d\n", skip);
  cliUtil.doNotApplyGlobalConfig = skip;
}

