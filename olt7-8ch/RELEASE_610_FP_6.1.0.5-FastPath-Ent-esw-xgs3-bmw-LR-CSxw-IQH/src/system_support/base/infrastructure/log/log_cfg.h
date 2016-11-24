/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename log_cfg.h
*
* @purpose defines data structures and prototypes for CFG
*
* @component LOG
*
* @comments none
*
* @create 2/3/2004
*
* @author McLendon
* @end
*
**********************************************************************/

#ifndef __LOG_CFG_H__
#define __LOG_CFG_H__

#include "l7_common.h"
#include "log_server.h"
#include "log_api.h"

#define L7_LOG_CFG_FILENAME    "log.cfg"
#define L7_LOG_CFG_VER_1       0x1
#define L7_LOG_CFG_VER_CURRENT L7_LOG_CFG_VER_1

#define L7_LOG_DEFAULT_CLIENT_PORT             FD_LOG_DEFAULT_CLIENT_PORT
#define L7_LOG_DEFAULT_SERVER_PORT             FD_LOG_DEFAULT_SERVER_PORT
#define L7_LOG_DEFAULT_FACILITY                FD_LOG_DEFAULT_LOG_FACILITY

/** The SyslogCfgData contains information regarding the
* configured log collectors. An unconfigured collector will
* have a status of L7_ROW_STATUS_INVALID (has never been
* configured) or L7_ROW_STATUS_DESTROY
* (was deleted by management).
*/

typedef struct syslogCfgData_s
{
  /* Our row status */
  L7_ADMIN_MODE_t     status;
  /** The collector address type */
  L7_IP_ADDRESS_TYPE_t   addressType;
  /** The collector address */
  L7_char8            address[L7_LOG_HOST_MAX_ADDRESS_LEN];
  /** The collector port */
  L7_ushort16         port;
  /** The component forwarding filter. */
  L7_COMPONENT_IDS_t  componentFilter;
  /** The time server priority */
  L7_LOG_SEVERITY_t   severityFilter;
  /** The user's description of the host */
  L7_char8            description[L7_LOG_HOST_DESCRIPTION_LEN+1];
}syslogCfgData_t;

/**
*
* The logCfgMsg contains information broadcast to the
* non-top of stack units to allow them to configure what
* will be sent to the top of stack or to their local
* log files.
*
*/

struct logCfgMsg_s
{
  L7_uint32               flag;
  /** To log or not to log, that is the question. */
  L7_ADMIN_MODE_t         inMemoryAdminStatus;
  /** The in memory log severity filter. */
  L7_LOG_SEVERITY_t       inMemorySeverityFilter;
  /** The in memory log component filter. */
  L7_COMPONENT_IDS_t      inMemoryComponentFilter;
  /** The in memory log full behavior. */
  L7_LOG_FULL_BEHAVIOR_t  inMemoryBehavior;

  /** The persistent log severity filter. Applies to
  * both operation and startup log.
  */
  L7_LOG_SEVERITY_t       persistentSeverityFilter;
  /** The persistent log admin status.
  * Applies to both operation and startup log.
  */
  L7_ADMIN_MODE_t         persistentAdminStatus;

  /** To log or not to log, that is the question. */
  L7_ADMIN_MODE_t         consoleAdminStatus;
  /** The console log severity filter. */
  L7_LOG_SEVERITY_t       consoleSeverityFilter;
  /** The console log component filter. */
  L7_COMPONENT_IDS_t      consoleComponentFilter;
};

/* The SntpCfgData structure contains LOG persistent configuration data.
* The reader is referred to the LOG Functional Specification
* section identified below which describes
* each of this elements of this data structure in detail.
*/

typedef struct logCfgData_s
{
  /*
   * Global admin status.
   */
  L7_ADMIN_MODE_t         globalAdminStatus;
  /** To log or not to log, that is the question. */
  L7_ADMIN_MODE_t         inMemoryAdminStatus;
  /** The in memory log severity filter. */
  L7_LOG_SEVERITY_t       inMemorySeverityFilter;
  /** The in memory log component filter. */
  L7_COMPONENT_IDS_t      inMemoryComponentFilter;
  /** The in memory log full behavior. */
  L7_LOG_FULL_BEHAVIOR_t  inMemoryBehavior;
  /** The size of the in memory log (number of entries) */
  /*  NOTE - this configuration item was added to allow for an implementation 
   *         that allowed the logging buffer to be resized by user commands.
   *         This has been disabled since it required the logging component to   
   *         access configuration earlier than the system architecture provides
   *         to access this parameter.  This config item and the API functions to
   *         set and get it are left in place in case this feature is restored.
   *         It is currently deprecated.
   */
  L7_uint32               inMemorySize;

  /** The persistent log severity filter. Applies to
  * both operation and startup log.
  */
  L7_LOG_SEVERITY_t       persistentSeverityFilter;
  /** The persistent log admin status.
  * Applies to both operation and startup log.
  */
  L7_ADMIN_MODE_t         persistentAdminStatus;

  /** To log or not to log, that is the question. */
  L7_ADMIN_MODE_t         consoleAdminStatus;
  /** The console log severity filter. */
  L7_LOG_SEVERITY_t       consoleSeverityFilter;
  /** The console log component filter. */
  L7_COMPONENT_IDS_t      consoleComponentFilter;

  /** FS 8.1.1 - The collector status */
  L7_ADMIN_MODE_t         syslogAdminStatus;
  /** FS 8.1.1 - The default facility */
  L7_LOG_FACILITY_t       syslogDefaultFacility;
  /* FS 8.1.1 - The local port number */
  L7_ushort16             localPort;
  /** Array of syslog servers/relays. */
  syslogCfgData_t  host[L7_LOG_MAX_HOSTS];
}logCfgData_t;

/*
* The LogCfg structure contains the system level
* persistent data definition.
*/

typedef struct logCfg_s
{
  L7_fileHdr_t          hdr;
  logCfgData_t   cfg;
  L7_uint32             checkSum;
}logCfg_t;

/*
* Mark the cfg data as needing saving.
*/
extern L7_RC_t logCfgDataChangedSet();

/*
* Has the cfg data changed?
*/

extern L7_BOOL logCfgHasDataChanged();
extern void logCfgResetDataChanged(void);

/*
* Set the cfg data to the default values.
*/

extern void logCfgDefaultsSet(L7_uint32 ver);

/*
* Handle commands from the configurator
*
*/
extern void logApiCnfgrCommand(L7_CNFGR_CMD_DATA_t * pCmdData);


/*
* Startup function for the log component.
*/ 
extern void loginit();

/*
* Intialize the logging subsystem. Allocate config data here.
*/
extern void loggingInit();

/*
* Show the status of the log server 
*/
extern  void logStatusShow();

/*
* Get this unit's number
*/
extern L7_RC_t usmDbUnitMgrNumberGet(L7_uint32 *unit_number);

/*
*  Configuration migration function.
*/

extern void logMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* INCLUDE_LOG_CFG_H*/
