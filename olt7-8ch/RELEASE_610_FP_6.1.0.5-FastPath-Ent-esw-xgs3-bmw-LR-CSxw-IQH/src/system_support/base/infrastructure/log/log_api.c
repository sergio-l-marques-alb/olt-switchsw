 /*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename log_api.c
*
* @purpose LOG public interface
*
* @component LOG
*
* @comments Implementation of functions that are provided by the
*           log subsystem directly.
*
* @create 2/11/2004
*
* @author mclendon
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "default_cnfgr.h"
#include "log_api.h"
#include "log_cfg.h"
#include "usmdb_log_api.h"
#include "usmdb_user_mgmt_api.h"
#include "cli_web_mgr_api.h"
#include "log_server.h"

    
/* Externs for persistent log code which was moved to the BSP */
/* This is crossing boundaries, but persistent logging belongs in BSP,not in application */
extern L7_RC_t logServerLogPersistentLogCountGet(L7_uint32 * count);
extern L7_RC_t logServerMsgPersistentLogCountGet(L7_uint32 * count);
extern L7_RC_t logServerLogMsgPersistentGet(L7_uint32 unitIndex,
                                            L7_uint32 ndx, L7_char8 * buf);

extern L7_RC_t logServerLogMsgPersistentGetNext(L7_uint32 unitIndex, 
                                                L7_uint32 ndx, L7_char8 * buf,
                                                L7_uint32 * bufNdx); 
extern L7_RC_t logServerLogMsgPersistentRemoteRetrieve(L7_uint32 unit);
extern L7_RC_t logServerMsgPersistentGetNext(L7_uint32 unitIndex,L7_uint32 ndx,
                                L7_char8 *buf,L7_uint32 *bufIndex);

extern void logServerLogPersistentClear( void );

/* Pointers to the configuration and operational data. */
static struct logCfg_s * logCfg = L7_NULL;
static struct logOps_s * logOps = L7_NULL;

/*********************************************************************
* @purpose  Get the maximum number of syslog host entries that can be
*           simultaneously configured on this client.
*
* @param    maxHosts @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logMaxSyslogHostsGet(L7_uint32 * maxHosts)
{
  if (maxHosts == L7_NULL)
    return L7_ERROR;
  *maxHosts = L7_LOG_MAX_HOSTS;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  set global admin mode (if different from current value.)
*
* @param    status @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_SUCCESS if new status is same as current status
* @returns  L7_SUCCESS if status updated.
*
* @notes    Disabling the global admin status does NOT disable the
*           console log.
*
* @end
*********************************************************************/

L7_RC_t logGlobalAdminStatusSet( L7_ADMIN_MODE_t status )
{
  if ( ( status != L7_ADMIN_MODE_DISABLE ) &&
       ( status != L7_ADMIN_MODE_ENABLE ) )
  {
    return( L7_NOT_SUPPORTED );
  }
#if 0
  if ( status == logCfg->cfg.globalAdminStatus )
  {
    return( L7_SUCCESS );
  }
#endif
  logCfg->cfg.globalAdminStatus = status;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  get global admin mode
*
* @param    status @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logGlobalAdminStatusGet( L7_ADMIN_MODE_t *status )
{
  if ( L7_NULL == status )
  {
    return( L7_ERROR );
  }
  *status = logCfg->cfg.globalAdminStatus;
  return( L7_SUCCESS );
}

/**********************************************************************
* @purpose  set admin mode of in memory log (if different from current value.)
*
* @param    status @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new status is same as current status
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logInMemoryAdminStatusSet(L7_ADMIN_MODE_t status)
{
  if ((status != L7_ADMIN_MODE_DISABLE) && (status != L7_ADMIN_MODE_ENABLE))
    return L7_NOT_SUPPORTED;
  if (status == logCfg->cfg.inMemoryAdminStatus)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.inMemoryAdminStatus = status;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  get admin mode of in memory log
*
* @param    status @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logInMemoryAdminStatusGet(L7_ADMIN_MODE_t * status)
{
  if (status == L7_NULL)
    return L7_ERROR;
  *status = logCfg->cfg.inMemoryAdminStatus;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  set severity filter of in memory log (if different from current value.)
*
* @param    sev @b{(input)} the new severity filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new severity is same as current severity
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logInMemorySeverityFilterSet(L7_LOG_SEVERITY_t sev)
{
  if ((sev < L7_LOG_SEVERITY_EMERGENCY) || (sev > L7_LOG_SEVERITY_DEBUG))
    return L7_NOT_SUPPORTED;
  if (sev == logCfg->cfg.inMemorySeverityFilter)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.inMemorySeverityFilter = sev;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  get severity filter of in memory log
*
* @param    sev @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logInMemorySeverityFilterGet(L7_LOG_SEVERITY_t * sev)
{
  if (sev == L7_NULL)
    return L7_ERROR;
  *sev = logCfg->cfg.inMemorySeverityFilter;
  return L7_SUCCESS;
}


/**********************************************************************
* @purpose  set component filter of in memory log (if different from current value.)
*
* @param    comp @b{(input)} the new component filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new component is same as current component
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logInMemoryComponentFilterSet(L7_COMPONENT_IDS_t comp)
{
  if ((comp < L7_FIRST_COMPONENT_ID) || ((comp > L7_LAST_COMPONENT_ID) && (comp != L7_ALL_COMPONENTS)))
    return L7_NOT_SUPPORTED;
  if (comp == logCfg->cfg.inMemoryComponentFilter)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.inMemoryComponentFilter = comp;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  get component filter of in memory log
*
* @param    comp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logInMemoryComponentFilterGet(L7_COMPONENT_IDS_t * comp)
{
  if (comp == L7_NULL)
    return L7_ERROR;
  *comp = logCfg->cfg.inMemoryComponentFilter;
  return L7_SUCCESS;
}


/**********************************************************************
* @purpose  set behavior of in memory log (if different from current value.)
*
* @param    beh @b{(input)} the new behavior
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new component is same as current component
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logInMemoryBehaviorSet(L7_LOG_FULL_BEHAVIOR_t beh)
{
  if ((beh != L7_LOG_WRAP_BEHAVIOR) && (beh != L7_LOG_STOP_ON_FULL_BEHAVIOR))
    return L7_NOT_SUPPORTED;
  if (beh == logCfg->cfg.inMemoryBehavior)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.inMemoryBehavior = beh;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  get behavior of in memory log
*
* @param    beh @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logInMemoryBehaviorGet(L7_LOG_FULL_BEHAVIOR_t * beh)
{
  if (beh == L7_NULL)
    return L7_ERROR;
  *beh = logCfg->cfg.inMemoryBehavior;
  return L7_SUCCESS;
}

/* NOTE - the following Set and Get functions are not in use.  See comment
*         in log_cfg.h near where the inMemorySize parameter is declared.
*/
/**********************************************************************
* @purpose  Sets the size of the in memory log
*
* @param    size @b((input)) number of log entries
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    This size is not set until the switch is reset
*
* @end
*********************************************************************/

L7_RC_t logInMemoryBufferSizeSet( L7_uint32 size )
{
#if 0
  if ( ( L7_LOG_IN_MEMORY_LOG_MIN_COUNT > size ) ||
       ( L7_LOG_IN_MEMORY_LOG_MAX_COUNT < size ) )
  {
    return( L7_ERROR );
  }

  logCfg->cfg.inMemorySize = size;
  logCfgDataChangedSet();
  return( L7_SUCCESS );
#else
  return( L7_NOT_SUPPORTED );
#endif
}

/**********************************************************************
* @purpose  Gets the size of the in memory log
*
* @param    size @b((output)) number of log entries
*
* @returns  L7_SUCCESS
*
* @notes    This size is the configured size, not necessarily the
*           current buffer size.
*
* @end
*********************************************************************/

L7_RC_t logInMemoryBufferSizeGet( L7_uint32 *size )
{
#if 0
  *size = logCfg->cfg.inMemorySize;
  return( L7_SUCCESS );
#else
  return( L7_NOT_SUPPORTED );
#endif
}

/**********************************************************************
* @purpose  clear the in-memory log
*
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logInMemoryBufferClear()
{
  logServerLogInMemoryClear();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  clear the persistent operational log
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logPersistentBufferClear( void )
{
  logServerLogPersistentClear();
  return( L7_SUCCESS );
}

/**********************************************************************
* @purpose  set admin mode of console log (if different from current value.)
*
* @param    status @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new status is same as current status
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logConsoleAdminStatusSet(L7_ADMIN_MODE_t status)
{
  if ((status != L7_ADMIN_MODE_DISABLE) && (status != L7_ADMIN_MODE_ENABLE))
    return L7_NOT_SUPPORTED;


  if (status == logCfg->cfg.consoleAdminStatus)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.consoleAdminStatus = status;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  get admin mode of console log
*
* @param    status @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logConsoleAdminStatusGet(L7_ADMIN_MODE_t * status)
{
  if (status == L7_NULL)
    return L7_ERROR;
  *status = logCfg->cfg.consoleAdminStatus;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  set severity filter of console log (if different from current value.)
*
* @param    sev @b{(input)} the new severity filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new severity is same as current severity
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logConsoleSeverityFilterSet(L7_LOG_SEVERITY_t sev)
{
  if ((sev < L7_LOG_SEVERITY_EMERGENCY) || (sev > L7_LOG_SEVERITY_DEBUG))
    return L7_NOT_SUPPORTED;
  if (sev == logCfg->cfg.consoleSeverityFilter)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.consoleSeverityFilter = sev;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  get severity filter of console log
*
* @param    sev @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logConsoleSeverityFilterGet(L7_LOG_SEVERITY_t * sev)
{
  if (sev == L7_NULL)
    return L7_ERROR;
  *sev = logCfg->cfg.consoleSeverityFilter;
  return L7_SUCCESS;
}


/**********************************************************************
* @purpose  set component filter of console log (if different from current value.)
*
* @param    comp @b{(input)} the new component filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new component is same as current component
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logConsoleComponentFilterSet(L7_COMPONENT_IDS_t comp)
{
  if ((comp < L7_FIRST_COMPONENT_ID) || ((comp > L7_LAST_COMPONENT_ID) && (comp != L7_ALL_COMPONENTS)))
    return L7_NOT_SUPPORTED;
  if (comp == logCfg->cfg.consoleComponentFilter)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.consoleComponentFilter = comp;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  get component filter of console log
*
* @param    comp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logConsoleComponentFilterGet(L7_COMPONENT_IDS_t * comp)
{
  if (comp == L7_NULL)
    return L7_ERROR;
  *comp = logCfg->cfg.consoleComponentFilter;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  set admin mode of persistent log (if different from current value.)
*
* @param    status @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new status is same as current status
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logPersistentAdminStatusSet(L7_ADMIN_MODE_t status)
{
  if ((status != L7_ADMIN_MODE_DISABLE) && (status != L7_ADMIN_MODE_ENABLE))
    return L7_NOT_SUPPORTED;
  if (status == logCfg->cfg.persistentAdminStatus)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.persistentAdminStatus = status;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  get admin mode of persistent log
*
* @param    status @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logPersistentAdminStatusGet(L7_ADMIN_MODE_t * status)
{
  if (status == L7_NULL)
    return L7_ERROR;
  *status = logCfg->cfg.persistentAdminStatus;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  set severity filter of persistent log (if different from current value.)
*
* @param    sev @b{(input)} the new severity filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new severity is same as current severity
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logPersistentSeverityFilterSet(L7_LOG_SEVERITY_t sev)
{
  if ((sev < L7_LOG_SEVERITY_EMERGENCY) || (sev > L7_LOG_SEVERITY_DEBUG))
    return L7_NOT_SUPPORTED;
  if (sev == logCfg->cfg.persistentSeverityFilter)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.persistentSeverityFilter = sev;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  get severity filter of persistent log
*
* @param    sev @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logPersistentSeverityFilterGet(L7_LOG_SEVERITY_t * sev)
{
  if (sev == L7_NULL)
    return L7_ERROR;
  *sev = logCfg->cfg.persistentSeverityFilter;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  set admin mode of syslog logging (if different from current value.)
*
* @param    status @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new status is same as current status
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logSyslogAdminStatusSet(L7_ADMIN_MODE_t status)
{
  if ((status != L7_ADMIN_MODE_DISABLE) && (status != L7_ADMIN_MODE_ENABLE))
    return L7_NOT_SUPPORTED;
  if (status == logCfg->cfg.syslogAdminStatus)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.syslogAdminStatus = status;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  get admin mode of syslog logging
*
* @param    status @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logSyslogAdminStatusGet(L7_ADMIN_MODE_t * status)
{
  if (status == L7_NULL)
    return L7_ERROR;
  *status = logCfg->cfg.syslogAdminStatus;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  set default facility of syslog logging (if different from current value.)
*
* @param    fac @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new facility is same as current facility
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logSyslogDefaultFacilitySet(L7_LOG_FACILITY_t fac)
{
  if ((fac < L7_LOG_FACILITY_KERNEL) ||
      ((fac > L7_LOG_FACILITY_LOCAL7) && (fac != L7_LOG_FACILITY_ALL)))
    return L7_NOT_SUPPORTED;
  if (fac == logCfg->cfg.syslogDefaultFacility)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.syslogDefaultFacility = fac;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  get default facility of syslog logging
*
* @param    fac @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logSyslogDefaultFacilityGet(L7_LOG_FACILITY_t * fac)
{
  if (fac == L7_NULL)
    return L7_ERROR;
  *fac = logCfg->cfg.syslogDefaultFacility;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  get count of in memory log messages
*
* @param    count @b{(input)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logInMemoryLogCountGet(L7_uint32 * count)
{
  return logServerLogInMemoryCountGet(count);
}
/**********************************************************************
* @purpose  get count of received messages
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logMessagesReceivedGet(L7_uint32 * count)
{
  return logServerLogMessagesReceivedGet(count);
}
/**********************************************************************
* @purpose  get count of dropped messages due to error conditions
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged due to error conditions
*
* @end
*********************************************************************/

L7_RC_t logMessagesDroppedGet(L7_uint32 * count)
{
  return logServerLogMessagesDroppedGet(count);
}

/**********************************************************************
* @purpose  get count of dropped messages
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged due to severity
*
* @end
*********************************************************************/

L7_RC_t logInMemoryMessagesDroppedGet( L7_uint32 *count )
{
  return( logServerLogInMemoryMessagesDroppedGet( count ) );
}

/**********************************************************************
* @purpose  get count of dropped messages
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged due to severity
*
* @end
*********************************************************************/

L7_RC_t logPersistentMessagesDroppedGet( L7_uint32 *count )
{
  return( logServerLogPersistentMessagesDroppedGet( count ) );
}

/**********************************************************************
* @purpose  get count of dropped messages
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged due to severity
*
* @end
*********************************************************************/

L7_RC_t logConsoleMessagesDroppedGet( L7_uint32 *count )
{
  return( logServerLogConsoleMessagesDroppedGet( count ) );
}

/**********************************************************************
* @purpose  get count of dropped messages for a given syslog server 
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                 from[1..L7_LOG_MAX_HOSTS]  
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
* @notes  Returns count of messages not logged for a given syslog server
*         due to socket delivery issues
*
* @end
*********************************************************************/
L7_RC_t logHostMessagesDroppedGet(L7_uint32 logIndex,
                                    L7_uint32 * count)
{
  return(logServerLogSyslogMessagesDroppedGet(logIndex,count));
}

/**********************************************************************
* @purpose  get count of syslog messages relayed
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logSyslogMessagesRelayedGet(L7_uint32 * count)
{
  return logServerLogSyslogMessagesRelayedGet(count);
}

/**********************************************************************
* @purpose  get last message received timestamp
*
* @param    tstamp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logMessageReceivedTimeGet(L7_uint32 * tstamp)
{
  return logServerLogMessageReceivedTimeGet(tstamp);
}

/**********************************************************************
* @purpose  get last syslog message delivered timestamp
*
* @param    tstamp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logSyslogMessageDeliveredTimeGet(L7_uint32 * tstamp)
{
  return logServerLogSyslogMessageDeliveredTimeGet(tstamp);
}

/**********************************************************************
* @purpose  get persistent log message count
*
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

L7_RC_t logPersistentLogCountGet(L7_uint32 * count)
{
  return logServerLogPersistentLogCountGet(count);
}


/**********************************************************************
* @purpose  Set the local port number of the LOG client.
*
* @param    port @b{(input)} port number
*
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new status is same as current status
* @returns  L7_SUCCESS if status updated.
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port number is not valid (e.g. 0)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logLocalPortSet(L7_ushort16 port)
{
  if (port == 0)
    return L7_NOT_SUPPORTED;
  if (port == logCfg->cfg.localPort)
    return L7_ALREADY_CONFIGURED;
  logCfg->cfg.localPort = port;
  logLocalSocketClose();
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the local port number of the LOG client.
*
* @param    port @b{(output)} pointer to port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port is null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logLocalPortGet(L7_ushort16 * port)
{
  if (port == L7_NULL)
    return L7_ERROR;
  *port = logCfg->cfg.localPort;
  return L7_SUCCESS;
}


/**
*
* Host table manipulation routines
*
*
*
*
*/

/**********************************************************************
* @purpose  Set the Hostname or IP address of the host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
*                           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    address @b{(output)} pointer to where result is
*                           stored (must be at least 64 bytes)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                           or null address pointer).
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTableHostSet(L7_uint32 ndx, L7_char8 * address)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (address == L7_NULL) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  logTaskLock();
  osapiStrncpy(logCfg->cfg.host[ndx-1].address, address, L7_LOG_HOST_MAX_ADDRESS_LEN);
  
  /* check to see if a RowStatus transition should occur */
  if (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_NOT_READY)
  {
    logCfg->cfg.host[ndx-1].status = L7_ROW_STATUS_ACTIVE;
  }
  /* Force resolution of address on next send */
  logOps[ndx-1].remoteAddr = L7_NULL_IP_ADDR;
  logTaskUnlock();
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the IP address of the host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
*                           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    address @b{(output)} pointer to where result is
*                           stored (must be at least 64 bytes)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or
*                       null address pointer).
*
* @notes    This is a null terminated string.
*
* @end
*********************************************************************/
L7_RC_t logHostTableIpAddressGet(L7_uint32 ndx, L7_char8 * address)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (address == L7_NULL) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  strncpy(address, logCfg->cfg.host[ndx-1].address, L7_LOG_HOST_MAX_ADDRESS_LEN);
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Set the row status of the host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                           from[1..L7_LOG_MAX_HOSTS]
* @param    status @b{(output)} the new status
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or
*                       invalid status).
*
* @notes    Can only set row status to destroy, create and go, or
*           not in service. Only client can set status to active.
*           Preferred method of creating a host entry is to use
*           logHostAdd.
*
*           Valid transitions are:
*
*
*           Old status      Requested Status    New Status
*           destroy         create and go       not ready
*           not ready       *                   active (if valid address, 
*                                               type, port)
*           not in service  active              active (if valid address,
*                                               type, port), not in
*                                               service otherwise.
*           active          destroy             destroy
*           active          not in service      not in service
*
*
*           destroy         create and go*      active if valid address and type
*
*           * this transition is only available through the logHostAdd call.
*
* @end
*********************************************************************/
L7_RC_t logHostTableRowStatusSet(L7_uint32 ndx, L7_ROW_STATUS_t status)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      ((status != L7_ROW_STATUS_DESTROY) &&
       (status != L7_ROW_STATUS_CREATE_AND_GO) &&
       (status != L7_ROW_STATUS_NOT_IN_SERVICE) &&
       (status != L7_ROW_STATUS_ACTIVE)))
  {
    return L7_ERROR;
  }

  logTaskLock();
  if ((status == L7_ROW_STATUS_ACTIVE) &&
      (logCfg->cfg.host[ndx-1].address[0] != '\0') &&
      (logCfg->cfg.host[ndx-1].addressType != L7_IP_ADDRESS_TYPE_UNKNOWN) &&
      (logCfg->cfg.host[ndx-1].port != 0))
  {
    logCfg->cfg.host[ndx-1].status = L7_ROW_STATUS_ACTIVE;
    logOps[ndx-1].remoteAddr = L7_NULL_IP_ADDR;
    logTaskUnlock();
    logCfgDataChangedSet();
    return L7_SUCCESS;
  }
  if ((status == L7_ROW_STATUS_CREATE_AND_GO) &&
      ((logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY)))
  {
    logCfg->cfg.host[ndx-1].address[0] = '\0';
    logCfg->cfg.host[ndx-1].addressType = L7_IP_ADDRESS_TYPE_UNKNOWN;
    logCfg->cfg.host[ndx-1].port = L7_LOG_DEFAULT_SERVER_PORT;
    logCfg->cfg.host[ndx-1].componentFilter = L7_ALL_COMPONENTS;
    logCfg->cfg.host[ndx-1].severityFilter = L7_LOG_SEVERITY_CRITICAL;
    logCfg->cfg.host[ndx-1].status = L7_ROW_STATUS_NOT_READY;
    logOps[ndx-1].remoteAddr = L7_NULL_IP_ADDR;
    logOps[ndx-1].syslogMessageForwardCount = 0;
    logOps[ndx-1].syslogMessageIgnoredCount = 0;
    logTaskUnlock();
    logCfgDataChangedSet();
    return L7_SUCCESS;
  }
  if (status == L7_ROW_STATUS_DESTROY)
  {
    logCfg->cfg.host[ndx-1].status = L7_ROW_STATUS_DESTROY;
    logCfg->cfg.host[ndx-1].address[0] = '\0';
    logCfg->cfg.host[ndx-1].addressType = L7_IP_ADDRESS_TYPE_UNKNOWN;
    logOps[ndx-1].remoteAddr = L7_NULL_IP_ADDR;
    logOps[ndx-1].syslogMessageForwardCount = 0;
    logOps[ndx-1].syslogMessageIgnoredCount = 0;
    logTaskUnlock();
    logCfgDataChangedSet();
    return L7_SUCCESS;
  }
  if (status == L7_ROW_STATUS_NOT_IN_SERVICE)
  {
    logCfg->cfg.host[ndx-1].status = L7_ROW_STATUS_NOT_IN_SERVICE;
    logOps[ndx-1].remoteAddr = L7_NULL_IP_ADDR;
    logTaskUnlock();
    logCfgDataChangedSet();
    return L7_SUCCESS;
  }
  logTaskUnlock();
  return L7_ERROR;
}

/**********************************************************************
* @purpose  Get the row status of the host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                           from[1..L7_LOG_MAX_HOSTS]
* @param    status @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument
* @returns  L7_NOT_EXIST    if ndx out of range or invalid row status.
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t logHostTableRowStatusGet(L7_uint32 ndx, L7_ROW_STATUS_t * status)
{
  if (status == L7_NULL)
  {
    return L7_ERROR;
  }
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS))
  {
    return L7_NOT_EXIST;
  }

  if ((logCfg->cfg.host[ndx - 1].status == L7_ROW_STATUS_NOT_IN_SERVICE) ||
      (logCfg->cfg.host[ndx - 1].status == L7_ROW_STATUS_ACTIVE) ||
      (logCfg->cfg.host[ndx - 1].status == L7_ROW_STATUS_NOT_READY))
  {
    *status = logCfg->cfg.host[ndx-1].status;
    return L7_SUCCESS;
  }
  else
  {
    return L7_NOT_EXIST;
  }
}
/**********************************************************************
* @purpose  Get the row status of the next host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of current entry into table.
* @param    status @b{(output)} pointer to where status result is stored
* @param    rowNdx @b{(output)} pointer to row index of returned result.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument
* @returns  L7_NOT_EXISTS    if index out of range or no hosts exist.
*
* @notes    none.
*
* @end
*********************************************************************/
L7_RC_t logHostTableRowStatusGetNext(L7_uint32 ndx,
                  L7_ROW_STATUS_t * status, L7_uint32 * rowNdx)
{
  if ((rowNdx == L7_NULL) ||
      (status == L7_NULL))
  {
    return L7_ERROR;
  }
  if (ndx >= L7_LOG_MAX_HOSTS)
    return L7_NOT_EXIST;

  for ( /* Nothing */ ; ndx < L7_LOG_MAX_HOSTS; ndx++)
  {
    if ((logCfg->cfg.host[ndx].status == L7_ROW_STATUS_NOT_IN_SERVICE) ||
        (logCfg->cfg.host[ndx].status == L7_ROW_STATUS_ACTIVE) ||
        (logCfg->cfg.host[ndx].status == L7_ROW_STATUS_NOT_READY))
    {
      *rowNdx = ndx + 1;
      *status = logCfg->cfg.host[ndx].status;
      return L7_SUCCESS;
    }
  }
  return L7_NOT_EXIST;
}

/**********************************************************************
* @purpose  Get the IP address type for the specified table entry.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                   from[1..L7_LOG_MAX_HOSTS]
* @param    type @b{(output)} pointer to where type result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTableAddressTypeGet(L7_uint32 ndx, L7_IP_ADDRESS_TYPE_t * type)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (type == L7_NULL) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *type = logCfg->cfg.host[ndx-1].addressType;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the IP address type of the address of the
*           host for the specified index
*
* @param    ndx @b{(input)} index of entry into table. Must range
*             from[1..L7_LOG_MAX_HOSTS]
* @param    type @b{(input)} new address type
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTableAddressTypeSet(L7_uint32 ndx, L7_IP_ADDRESS_TYPE_t type)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      ((type != L7_IP_ADDRESS_TYPE_UNKNOWN) &&
       (type != L7_IP_ADDRESS_TYPE_IPV4) &&
       (type != L7_IP_ADDRESS_TYPE_DNS)) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  logCfg->cfg.host[ndx-1].addressType = type;
  logOps[ndx-1].remoteAddr = L7_NULL_IP_ADDR;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the IP port of the host for the specified entry.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                 from[1..L7_LOG_MAX_HOSTS]
* @param    port @b{(output)} pointer to where port result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTablePortGet(L7_uint32 ndx, L7_ushort16 * port)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (port == L7_NULL) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *port = logCfg->cfg.host[ndx-1].port;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the IP port of the
*           host for the specified index
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                           from[1..L7_LOG_MAX_HOSTS]
* @param    port @b{(input)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTablePortSet(L7_uint32 ndx, L7_ushort16 port)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (port == 0) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  logCfg->cfg.host[ndx-1].port = port;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the component filter of the host for the specified index.
*
* @param    ndx @b{(input)} index of entry into table. Must
*                           range from[1..L7_LOG_MAX_HOSTS]
* @param    component @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTableComponentGet(L7_uint32 ndx,
                            L7_COMPONENT_IDS_t * component)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (component == L7_NULL) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *component = logCfg->cfg.host[ndx-1].componentFilter;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the component filter of the
*           host specified by the index.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                 from[1..L7_LOG_MAX_HOSTS]
* @param    component @b{(input)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTableComponentSet(L7_uint32 ndx,
                                L7_COMPONENT_IDS_t component)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (component <= L7_FIRST_COMPONENT_ID) ||
      ((component >= L7_LAST_COMPONENT_ID) && (component != L7_ALL_COMPONENTS)) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  logCfg->cfg.host[ndx-1].componentFilter = component;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the severity filter of the host for the specified index.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                   from[1..L7_LOG_MAX_HOSTS]
* @param    severity @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTableSeverityGet(L7_uint32 ndx, L7_LOG_SEVERITY_t * severity)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (severity == L7_NULL) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *severity = logCfg->cfg.host[ndx-1].severityFilter;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the severity filter of the
*           host specified by the index.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                         from[1..L7_LOG_MAX_HOSTS]
* @param    severity @b{(input)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t logHostTableSeveritySet(L7_uint32 ndx, L7_LOG_SEVERITY_t severity)
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (severity < L7_LOG_SEVERITY_EMERGENCY) ||
      (severity > L7_LOG_SEVERITY_DEBUG)  ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  logCfg->cfg.host[ndx-1].severityFilter = severity;
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the description of the host specified by the index.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                         from[1..L7_LOG_MAX_HOSTS]
* @param    description @b{(input)} user's description of host
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t logHostTableDescriptionSet( L7_uint32 ndx, L7_uchar8 *description )
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (NULL == description) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }

  memset(logCfg->cfg.host[ndx-1].description,0,sizeof(logCfg->cfg.host[ndx-1].description));
  strncpy( logCfg->cfg.host[ndx-1].description, description, L7_LOG_HOST_DESCRIPTION_LEN );
  if (strlen(description) >= L7_LOG_HOST_DESCRIPTION_LEN )
      logCfg->cfg.host[ndx-1].description[L7_LOG_HOST_DESCRIPTION_LEN]='\0';
  logCfgDataChangedSet();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the description of the host specified by the index.
*
* @param    ndx @b{(input)} index of entry into table. Must range
*                         from[1..L7_LOG_MAX_HOSTS]
* @param    description @b{(output)} user's description of host
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    It is the responsibility of the caller to ensure that
*           description points to a buffer at least
*           L7_LOG_HOST_DESCRIPTION_LEN bytes long.
*
* @end
*********************************************************************/

L7_RC_t logHostTableDescriptionGet( L7_uint32 ndx, L7_uchar8 *description )
{
  if ((ndx < 1) ||
      (ndx > L7_LOG_MAX_HOSTS) ||
      (NULL == description) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_INVALID) ||
      (logCfg->cfg.host[ndx-1].status == L7_ROW_STATUS_DESTROY))
  {
    return L7_ERROR;
  }

  strncpy( description, logCfg->cfg.host[ndx-1].description, L7_LOG_HOST_DESCRIPTION_LEN+1);
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Add a host to the host table in the first free slot.
*
* @param    host @b{(input)} address of host or hostname.
* @param    type @b{(input)} address type.
* @param    rowNdx @b{(output)}  pointer to where index of new
*                                 entry is stored.
*
*
* @returns  L7_SUCCESS          if success
* @returns  L7_ERROR            if null pointer
* @returns  L7_TABLE_IS_FULL    if no room in table
*
* @notes    Update rowNdx to index of row written if successful.
*           Multiple hosts of the same address/type may exist.
*           Other parameters are set to default values.
*           Host status is set to active.
* @end
*********************************************************************/

L7_RC_t logHostAdd(L7_char8 * host, L7_IP_ADDRESS_TYPE_t type,
                      L7_uint32 * rowNdx)
{
  register L7_uint32 ndx;

  if ((host == L7_NULL) || (host[0] == '\0') || (rowNdx == L7_NULL))
    return L7_ERROR;

  if ((type != L7_IP_ADDRESS_TYPE_IPV4) &&
      (type != L7_IP_ADDRESS_TYPE_DNS))
    return L7_ERROR;

  logTaskLock();


  /* Check for possible duplicate addition */
  for (ndx = 0; ndx < L7_LOG_MAX_HOSTS; ndx++)
  {
      /* Check for valid entry in table */
      if ((logCfg->cfg.host[ndx].status != L7_ROW_STATUS_INVALID) &&
          (logCfg->cfg.host[ndx].status != L7_ROW_STATUS_DESTROY))
      {
          if (strncmp(logCfg->cfg.host[ndx].address,host,L7_LOG_HOST_MAX_ADDRESS_LEN) == 0)
          {
              /* Entry is already in table */
              logTaskUnlock();
              return L7_ERROR;
          }
      }
  }

  /* Add the entry */
  for (ndx = 0; ndx < L7_LOG_MAX_HOSTS; ndx++)
  {
    if ((logCfg->cfg.host[ndx].status == L7_ROW_STATUS_INVALID) ||
        (logCfg->cfg.host[ndx].status == L7_ROW_STATUS_DESTROY))
    {
      osapiStrncpySafe(logCfg->cfg.host[ndx].address, host, L7_LOG_HOST_MAX_ADDRESS_LEN);
      logCfg->cfg.host[ndx].addressType = type;
      logCfg->cfg.host[ndx].port = L7_LOG_DEFAULT_SERVER_PORT;
      logCfg->cfg.host[ndx].componentFilter = L7_ALL_COMPONENTS;
      logCfg->cfg.host[ndx].severityFilter = L7_LOG_SEVERITY_CRITICAL;
      logCfg->cfg.host[ndx].description[0] = '\0';
      logCfg->cfg.host[ndx].status = L7_ROW_STATUS_ACTIVE;
      logOps[ndx].remoteAddr = L7_NULL_IP_ADDR;
      logOps[ndx].syslogMessageForwardCount = 0;
      logOps[ndx].syslogMessageIgnoredCount = 0;
      logCfg->hdr.dataChanged = L7_TRUE;
      *rowNdx = ndx + 1;
      logTaskUnlock();
      return L7_SUCCESS;
    }
  }
  logTaskUnlock();
  return L7_TABLE_IS_FULL;
}

/**********************************************************************
* @purpose  Get the "next" row in the in-memory log table.
*
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least L7_LOG_MESSAGE_LENGTH
*                   bytes into which the message is written.
* @param    bufNdx @b{(output)} the true index in the log table of the
*                   returned buffer.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry.
*
* @notes    The indices are continuously updated as messages are logged.
*           Therefore, a slow reader and a fast writer (lot's of logging)
*           may continuously retrieve the oldest message (which will not be
*           the same).
*           Use an ndx parameter of 0 to retrieve the oldest message in the log.
*
* @end
*********************************************************************/

L7_RC_t logMsgInMemoryGetNext(L7_uint32 ndx, L7_char8 * buf, L7_uint32 * bufNdx)
{
  return logServerLogMsgInMemoryGetNext(ndx, buf, bufNdx);
}
/**********************************************************************
* @purpose  Get the specified row in the in-memory log table.
*
* @param    ndx @b{(output)} the putative index of a log table entry.
* @param    buf @b{(output)}  a buffer of at least L7_LOG_MESSAGE_LENGTH
*                   bytes into which the message is written.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf is null).
* @returns  L7_NOT_EXIST if ndx parameter specifies an entry that does not exist.
*
* @notes    The indices are continuously updated as messages are logged.
*           The possible range of indices is:
*           [logMessagesReceived - L7_LOG_IN_MEMORY_COUNT .. logMessagesReceived]
* @end
*********************************************************************/

L7_RC_t logMsgInMemoryGet(L7_uint32 ndx, L7_char8 * buf)
{
  return logServerLogMsgInMemoryGet(ndx, buf);
}

/**********************************************************************
* @purpose  Get the "next" row in the persistent log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least
*                 L7_LOG_MESSAGE_LENGTH bytes into which the message is
*                 written.
* @param    bufNdx @b{(output)} the true index in the log
*                               table of the returned buffer.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry of the log.
*
* @notes    The indices are continuously updated as messages are logged.
*           Therefore, a slow reader and a fast writer (lot's of logging)
*           may continuously retrieve the oldest message (which will not be
*           the same).
*           Use an ndx parameter of 0 to retrieve the oldest message
*           in the log. The startup log and the operation log are
*           treated as one log by this routine. Thus there will
*           likely be a gap in the timestamps
*           and the indices for logged messages on systems that have been
*           running for some period of time.
*
* @end
*********************************************************************/


L7_RC_t logMsgPersistentGetNext(L7_uint32 unitIndex, L7_uint32 ndx,
                          L7_char8 * buf, L7_uint32 * bufNdx)
{
  return logServerLogMsgPersistentGetNext(unitIndex, ndx, buf, bufNdx);

}
/**********************************************************************
* @purpose  Get the specified row in the persistent log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx maxHosts @b{(input)} - the putative index of a log table entry.
* @param    buf maxHosts @b{(output)} - a buffer of at least
*           L7_LOG_MESSAGE_LENGTH bytes into which the message is written.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter does not identify an existing
*           row in the table(s).
*
* @notes    The indices are continuously updated as messages are logged.
*           On a system that has been operating for an extended period of time,
*           there will likely be a gap in the indices of the startup table
*           (always [1..L7_LOG_PERSISTENT_STARTUP_LOG_COUNT]) and the
*           indices of the operation table.
*
* @end
*********************************************************************/

L7_RC_t logMsgPersistentGet(L7_uint32 unitIndex, L7_uint32 ndx, L7_char8 * buf)
{
  return logServerLogMsgPersistentGet(unitIndex, ndx, buf);
}
/**********************************************************************
* @purpose  Copy the persistent logs from a remote unit to the local unit.
*
* @param    unitIndex @b{(input)} the unit index
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid unable to copy.
*
* @end
*********************************************************************/

L7_RC_t logMsgPersistentRemoteRetrieve(L7_uint32 unitIndex)
{
  return logServerLogMsgPersistentRemoteRetrieve(unitIndex);
}

/*********************************************************************
 * * @purpose  Initialize the log api.
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

void logApiInit(struct logCfg_s * cfg, struct logOps_s * ops)
{
  if ((cfg == L7_NULL) || (ops == L7_NULL))
          return;
  logCfg = cfg;
  logOps = ops;
}


