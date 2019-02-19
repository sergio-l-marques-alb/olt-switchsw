/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  log_cfg.c
*
* @purpose   Implements the stacking initialization sequence
*
* @component LOG
*
* @comments   The main configurator dictates what sequence of state
*             changes by using the apt command/request pair. The
*             current initialization sequence for non-management units
*             is: INIT_0, INIT_1, INIT_2 and WMU.
*             Management units have the sequence: INIT_0, INIT_1, INIT_2,
*             WMU and INIT_3. After the management unit (MU) has
*             transitioned into INIT_3 the configurator issues a
*             command to all units (MU and non-MU) to move into the
*             EXECUTE phase.
*
*             When a "clear config" command is issued, the sequence
*             of state changes is: EXECUTE,UNCONFIG_1, UNCONFIG_2, WMU.
*             The MU then goes into INIT_3 and subsequently all
*             units go into EXECUTE phase.
*
*             None the state transitions are enforced by the LOG
*             component; the main configurator ensures the order
*             of execution.
*
*             Logging implements the configuration state changes
*             somewhat differently than other system components.
*             Logging is started from the BSP startup code so
*             that it is available to all components during
*             the startup phases. During this time,
*             logging is performed locally (since all stackables
*             are the "master"). When the master of the stack
*             is established, log messages are forwarded to
*             the master in addition to being logged locally.
*             The advantage of this approach is that the
*             eventual stack master will have all local messages
*             logged and messages on stack units subsequent to
*             entering EXECUTE are logged on the stack master.
*
*             In order to avoid issues with logging, errors
*             in logging are written to the console instead
*             of being "logged."
*
* @create    2/3/2004
*
* @author    McLendon
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "l7_ip_api.h"
#include "sysapi_hpc.h"
#include "flex.h"
#if defined (L7_STACKING_PACKAGE)
#include "fftp_api.h"
#endif

#include "log_api.h"
#include "log.h"
#include "log_cfg.h"
#include "log_server.h"
#include "osapi_support.h"
#define LOG_CNFGR_QUEUE    "LogCfgQ"
#define LOG_CNFGR_TASK     "LOGC"
#define LOG_CNFGR_MSG_CNT  16

#include <stdlib.h>

/* Types of message passed to the LOG cfg task
   via the LOG processing queue */
typedef enum {
    LOG_CNFGR_CMD = 0,
    LOG_ADMIN_MODE,
    LOG_RESTORE_EVENT,
    LOG_SAVE_EVENT
} logCfgMessage_t;


/* Message received by Log config processing task through Log
* processing queue
*/
typedef struct
{
    logCfgMessage_t msgType;
    union
    {
        L7_CNFGR_CMD_DATA_t cmdData;
    } type;

} logCfgMsg_t;

/* This is a pointer to the log configuration data structure. */
static struct logCfg_s  * logCfg = L7_NULL;

/* Different phases of the log component. */
typedef enum {
  LOG_PHASE_INIT_0 = 0,
  LOG_PHASE_INIT_1,
  LOG_PHASE_INIT_2,
  LOG_PHASE_WMU,
  LOG_PHASE_INIT_3,
  LOG_PHASE_EXECUTE,
  LOG_PHASE_UNCONFIG_1,
  LOG_PHASE_UNCONFIG_2,
  LOG_PHASE_LAST_ELEMENT
} logCnfgrState_t;


/**********************************************************************
* @purpose  Fills out the Config data header with defaults
*
* @param    ver @b{(input)} version of the config data
*
* @returns  None.
*
* @notes    none
*
* @end
*********************************************************************/

void logCfgHeaderDefaultsSet(L7_uint32 ver)
{
    /** setup file header */
  logCfg->hdr.version = ver;
  logCfg->hdr.componentID = L7_LOG_COMPONENT_ID;
  logCfg->hdr.type = L7_CFG_DATA;
  logCfg->hdr.length = (L7_uint32)sizeof(struct logCfg_s);
  strcpy((L7_char8 *)logCfg->hdr.filename, L7_LOG_CFG_FILENAME);
  logCfg->hdr.dataChanged = L7_FALSE;
}


/**********************************************************************
* @purpose  Fills out the Config data header with defaults
*
* @param    @b{(input)} version of the config data
*
* @returns  None.
*
* @notes    Caller must NOT hold log task semaphore.
*
* @end
*********************************************************************/

void logCfgDefaultsSet(L7_uint32 ver)
{
  /* TODO - Clear statistics? */
  L7_uint32 ndx;

  /*
   * Global admin default
   */
  logCfg->cfg.globalAdminStatus  = FD_LOG_DEFAULT_GLOBAL_ADMIN_STATUS;
  /* Console logging defaults. */
  logCfg->cfg.consoleAdminStatus =  FD_LOG_DEFAULT_CONSOLE_ADMIN_STATUS;
  logCfg->cfg.consoleComponentFilter = FD_LOG_DEFAULT_CONSOLE_COMPONENT_FILTER;
  logCfg->cfg.consoleSeverityFilter = FD_LOG_DEFAULT_CONSOLE_SEVERITY_FILTER;
  /* Buffered logging defaults. */
  logCfg->cfg.inMemoryAdminStatus = FD_LOG_DEFAULT_IN_MEMORY_ADMIN_STATUS;
  logCfg->cfg.inMemoryBehavior = FD_LOG_DEFAULT_IN_MEMORY_BEHAVIOR;
  logCfg->cfg.inMemoryComponentFilter = FD_LOG_DEFAULT_IN_MEMORY_COMPONENT_FILTER;
  logCfg->cfg.inMemorySeverityFilter = FD_LOG_DEFAULT_IN_MEMORY_SEVERITY_FILTER;
  logCfg->cfg.inMemorySize           = L7_LOG_IN_MEMORY_LOG_DEFAULT_COUNT;

  /* Persistent log defaults. */
  logCfg->cfg.persistentAdminStatus = FD_LOG_DEFAULT_PERSISTENT_ADMIN_STATUS;
  logCfg->cfg.persistentSeverityFilter = FD_LOG_DEFAULT_PERSISTENT_SEVERITY_FILTER;

  /* Syslog defaults. */
  logCfg->cfg.syslogAdminStatus = FD_LOG_DEFAULT_SYSLOG_ADMIN_STATUS;
  logCfg->cfg.syslogDefaultFacility = FD_LOG_DEFAULT_LOG_FACILITY;
  logCfg->cfg.localPort = FD_LOG_DEFAULT_CLIENT_PORT;
  for (ndx = 0; ndx < L7_LOG_MAX_HOSTS; ndx++)
  {
    logCfg->cfg.host[ndx].status = L7_ROW_STATUS_INVALID;
    logCfg->cfg.host[ndx].address[0] = '\0';
    logCfg->cfg.host[ndx].addressType = L7_IP_ADDRESS_TYPE_UNKNOWN;
    logCfg->cfg.host[ndx].port = FD_LOG_DEFAULT_SERVER_PORT;
    logCfg->cfg.host[ndx].componentFilter = L7_ALL_COMPONENTS;
    logCfg->cfg.host[ndx].severityFilter = FD_LOG_DEFAULT_SYSLOG_SEVERITY_FILTER;
    logCfg->cfg.host[ndx].description[0] = '\0';
  }
}

/**********************************************************************
* @purpose  Checks if LOG config data has changed
*
* @param    void
*
* @returns  L7_TRUE if changed or L7_FALSE if not
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL logCfgHasDataChanged()
{
  return logCfg->hdr.dataChanged;
}
void logCfgResetDataChanged(void)
{
  logCfg->hdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Saves log configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    logCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t logSave()
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 checksum = 0;

  if (logCfgHasDataChanged() == L7_TRUE)
  {
    logCfg->hdr.dataChanged = L7_FALSE;
    logTaskLock();
    checksum = nvStoreCrc32((L7_uchar8 *)logCfg,
                      sizeof(struct logCfg_s) - sizeof(L7_uint32));
    logCfg->checkSum = checksum;
    /* call save NVStore routine */
    if ((rc = sysapiCfgFileWrite(L7_LOG_COMPONENT_ID, L7_LOG_CFG_FILENAME,
                (L7_char8 *)logCfg, sizeof(struct logCfg_s))) == L7_ERROR)
    {
      sysapiPrintf("logSave: Error %d return from osapiFsWrite for file %s\n",
              osapiErrnoGet(), L7_LOG_CFG_FILENAME);
    }
    logTaskUnlock();
  }
  return rc;
}

/*********************************************************************
* @purpose  Build default LOG config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
static void logBuildDefaultConfigData(L7_uint32 ver)
{

  memset(logCfg, 0, sizeof(struct logCfg_s));
  logCfgHeaderDefaultsSet(ver);
  logCfgDefaultsSet(ver);
}

/*********************************************************************
* @purpose  Restores log configuration to defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    logCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t logRestoreDefaults()
{
  logBuildDefaultConfigData(L7_LOG_CFG_VER_CURRENT);
  logCfg->hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Read LOG config data
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t logConfigRead()
{
  /* Load our configuration. */
  if (sysapiCfgFileGet(L7_LOG_COMPONENT_ID, L7_LOG_CFG_FILENAME,
                       (L7_char8 *)logCfg, sizeof(struct logCfg_s),
                       &logCfg->checkSum, L7_LOG_CFG_VER_CURRENT,
                       logBuildDefaultConfigData, logMigrateConfigData) != L7_SUCCESS)
  {
    sysapiPrintf("logReadConfig: %s(%d) - unable to read config file %s\n",
                    __FILE__, __LINE__, L7_LOG_CFG_FILENAME);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

#if defined (L7_STACKING_PACKAGE)
/**********************************************************************
* @purpose broadcast a config message to all non-management stack members
*
* @param  None
*
* @returns  None
*
* @notes  This executes on the calling task thread.
*         Access to config data is on a best effort basis (may be invalid).
*
* @end
*********************************************************************/

static void logBroadcastCfg()
{
  struct logCfgMsg_s  msg;

  if (sysapiHpcTopOfStackGet() == SYSAPI_STACK_ROLE_MANAGEMENT_UNIT)
  {
    /* Format a configuration message from top of stack. */
    msg.flag = 0xffffffff; /* Distinguish this message from all others. */
    msg.consoleAdminStatus = logCfg->cfg.consoleAdminStatus;
    msg.consoleComponentFilter = logCfg->cfg.consoleComponentFilter;
    msg.consoleSeverityFilter = logCfg->cfg.consoleSeverityFilter;
    msg.inMemoryAdminStatus = logCfg->cfg.inMemoryAdminStatus;
    msg.inMemoryBehavior = logCfg->cfg.inMemoryBehavior;
    msg.inMemoryComponentFilter = logCfg->cfg.inMemoryComponentFilter;
    msg.inMemorySeverityFilter = logCfg->cfg.inMemorySeverityFilter;
    msg.persistentAdminStatus = logCfg->cfg.persistentAdminStatus;
    msg.persistentSeverityFilter = logCfg->cfg.persistentSeverityFilter;

    sysapiHpcBroadcastMessageSend(L7_LOG_COMPONENT_ID, sizeof(struct logCfgMsg_s), (L7_uchar8 *)&msg);
  }
}
#endif /* L7_STACKING_PACKAGE */

/*********************************************************************
* @purpose  Mark the configuration data as changed.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t logCfgDataChangedSet()
{
#if defined (L7_STACKING_PACKAGE)
  logBroadcastCfg();
#endif
  logCfg->hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}


/*********************************************************************
 * * @purpose  Initialize the log cfg.
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

void logCfgInit(struct logCfg_s * cfg, struct logOps_s * ops)
{
  if ((cfg == L7_NULL) || (ops == L7_NULL))
          return;
  logCfg = cfg;
  /*logOps = ops;*/
}

