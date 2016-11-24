/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @file     usmDb_log_api.h
*
* @purpose   LOG API declarations
*
* @component USMDB
*
* @comments
*
* create 2/11/2004
*
* author Mclendon
*
**********************************************************************/

#ifndef __USMDB_LOG_API_H__
#define __USMDB_LOG_API_H__

#include "l7_common.h"
#include "usmdb_cmd_logger_api.h"
#include "osapi.h"
#include "default_cnfgr.h"
#include "log_api.h"

/**********************************************************************
* @purpose  get count of dropped log messages for the syslog server
*
* @param    logIndex @b{(input)} the server index
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

extern L7_RC_t usmDbLogHostMessagesDroppedGet(L7_uint32 logIndex,
                                    L7_uint32 * count);

/**********************************************************************
* @purpose  Get count of not logged  messages
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

extern L7_RC_t usmDbMessagesNotLoggedGet(L7_uint32 * count);

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

L7_RC_t usmDbLogGlobalAdminStatusSet( L7_ADMIN_MODE_t status );

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

L7_RC_t usmDbLogGlobalAdminStatusGet( L7_ADMIN_MODE_t *status );
/**********************************************************************
* @purpose  set admin mode of in memory log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogInMemoryAdminStatusSet(L7_uint32 unitIndex,
                                    L7_ADMIN_MODE_t status);

/**********************************************************************
* @purpose  get admin mode of in memory log
*
* @param    unitIndex @b{(input)} the unit index
* @param    status @b{(input)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogInMemoryAdminStatusGet(L7_uint32 unitIndex,
                                L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set severity filter of in memory log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogInMemorySeverityFilterSet(L7_uint32 unitIndex,
                                        L7_LOG_SEVERITY_t sev);

/**********************************************************************
* @purpose  get severity filter of in memory log
*
* @param    unitIndex @b{(input)} the unit index
* @param    sev @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogInMemorySeverityFilterGet(L7_uint32 unitIndex,
                                          L7_LOG_SEVERITY_t * sev);

/**********************************************************************
* @purpose  set component filter of in memory log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogInMemoryComponentFilterSet(L7_uint32 unitIndex,
                                                L7_COMPONENT_IDS_t comp);

/**********************************************************************
* @purpose  get component filter of in memory log
*
* @param    unitIndex @b{(input)} the unit index
* @param    comp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogInMemoryComponentFilterGet(L7_uint32 unitIndex,
                                          L7_COMPONENT_IDS_t * comp);

/**********************************************************************
* @purpose  set behavior of in memory log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogInMemoryBehaviorSet(L7_uint32 unitIndex,
                                              L7_LOG_FULL_BEHAVIOR_t beh);

/**********************************************************************
* @purpose  get behavior of in memory log
*
* @param    unitIndex @b{(input)} the unit index
* @param    beh @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogInMemoryBehaviorGet(L7_uint32 unitIndex,
                                      L7_LOG_FULL_BEHAVIOR_t * beh);

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

extern L7_RC_t usmDbLogInMemoryBufferSizeSet( L7_uint32 size );

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

extern L7_RC_t usmDbLogInMemoryBufferSizeGet( L7_uint32 *size );
/**********************************************************************
* @purpose  clear in memory log
*
* @param    unitIndex @b{(input)} the unit index
*
* @returns  L7_SUCCESS
*
* @notes  Resets buffered memory, clears buffered counter
*         May cause some messages in process of being logged to syslog
*         or console to be lost.
* @end
*********************************************************************/

extern L7_RC_t usmDbLogInMemoryLogClear(L7_uint32 unitIndex);


/**********************************************************************
* @purpose  set admin mode of console log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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
extern L7_RC_t usmDbLogConsoleAdminStatusSet(L7_uint32 unitIndex,
                                              L7_ADMIN_MODE_t status);

/**********************************************************************
* @purpose  Set the status for the given CLI login session in the
*           operational status parameter
*
* @param    L7_uint32 session      @b{(input)} CLI login session
* @param    L7_ADMIN_MODE_t status @b{(input)} status
*
* @returns  L7_SUCCESS if success
*           L7_NOT_SUPPORTED if invalid status
*           L7_ERROR if invalid session
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbLogDebugConsoleOperStatusSet(L7_uint32 session, L7_ADMIN_MODE_t status);

/**********************************************************************
* @purpose  get admin mode of console usmDb
*
* @param    unitIndex @b{(input)} the unit index
* @param    beh @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogConsoleAdminStatusGet(L7_uint32 unitIndex,
                                          L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set severity filter of console log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogConsoleSeverityFilterSet(L7_uint32 unitIndex,
                                          L7_LOG_SEVERITY_t sev);

/**********************************************************************
* @purpose  get severity filter of console usmDb
*
* @param    unitIndex @b{(input)} the unit index
* @param    sev @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogConsoleSeverityFilterGet(L7_uint32 unitIndex,
                                        L7_LOG_SEVERITY_t * sev);

/**********************************************************************
* @purpose  set component filter of console log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
* @param    the new component filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new component is same as current component
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogConsoleComponentFilterSet(L7_uint32 unitIndex,
                                          L7_COMPONENT_IDS_t comp);

/**********************************************************************
* @purpose  get component filter of console usmDb
*
* @param    unitIndex @b{(input)} the unit index
* @param    comp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogConsoleComponentFilterGet(L7_uint32 unitIndex,
                                            L7_COMPONENT_IDS_t * comp);

/**********************************************************************
* @purpose  set admin mode of persistent log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogPersistentAdminStatusSet(L7_uint32 unitIndex,
                                          L7_ADMIN_MODE_t status);

/**********************************************************************
* @purpose  get admin mode of persistent usmDb
*
* @param    unitIndex @b{(input)} the unit index
* @param    beh @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogPersistentAdminStatusGet(L7_uint32 unitIndex,
                                            L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set severity filter of persistent log (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogPersistentSeverityFilterSet(L7_uint32 unitIndex,
                                            L7_LOG_SEVERITY_t sev);

/**********************************************************************
* @purpose  get severity filter of persistent usmDb
*
* @param    unitIndex @b{(input)} the unit index
* @param    beh @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogPersistentSeverityFilterGet(L7_uint32 unitIndex,
                                          L7_LOG_SEVERITY_t * sev);

/**********************************************************************
* @purpose  clear the persistent operational log
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogPersistentLogClear( L7_uint32 unitIndex );
/**********************************************************************
* @purpose  set admin mode of syslog logging (if different from current value.)
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogSyslogAdminStatusSet(L7_uint32 unitIndex,
                                          L7_ADMIN_MODE_t status);

/**********************************************************************
* @purpose  get admin mode of syslog logging
*
* @param    unitIndex @b{(input)} the unit index
* @param    status @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogSyslogAdminStatusGet(L7_uint32 unitIndex,
                                      L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set default facility of syslog logging (if different from
*           current value.)
*
* @param    unitIndex @b{(input)} the unit index
* @param    the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new facility is same as current facility
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogSyslogDefaultFacilitySet(L7_uint32 unitIndex,
                                          L7_LOG_FACILITY_t fac);

/**********************************************************************
* @purpose  get default facility of syslog logging
*
* @param    unitIndex @b{(input)} the unit index
* @param    fac @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogSyslogDefaultFacilityGet(L7_uint32 unitIndex,
                                          L7_LOG_FACILITY_t * fac);

/**********************************************************************
* @purpose  get count of in memory log messages
*
* @param    unitIndex @b{(input)} the unit index
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogInMemoryLogCountGet(L7_uint32 unitIndex,
                                    L7_uint32 * count);

/**********************************************************************
* @purpose  get count of received messages
*
* @param    unitIndex @b{(input)} the unit index
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogMessagesReceivedGet(L7_uint32 unitIndex,
                                  L7_uint32 * count);

/**********************************************************************
* @purpose  get count of dropped messages due to error conditions
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogMessagesDroppedGet(L7_uint32 unitIndex,
                                  L7_uint32 * count);

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

L7_RC_t usmDbLogInMemoryMessagesDroppedGet( L7_uint32 *count );

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

L7_RC_t usmDbLogPersistentMessagesDroppedGet( L7_uint32 *count );

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

L7_RC_t usmDbLogConsoleMessagesDroppedGet( L7_uint32 *count );
/**********************************************************************
* @purpose  get count of syslog messages relayed
*
* @param    unitIndex @b{(input)} the unit index
* @param    beh @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogSyslogMessagesRelayedGet(L7_uint32 unitIndex,
                                          L7_uint32 * count);

/**********************************************************************
* @purpose  get last message received timestamp
*
* @param    unitIndex @b{(input)} the unit index
* @param    tstamp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogMessageReceivedTimeGet(L7_uint32 unitIndex,
                                        L7_uint32 * tstamp);

/**********************************************************************
* @purpose  get last syslog message delivered timestamp
*
* @param    unitIndex @b{(input)} the unit index
* @param    tstamp @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogSyslogMessageDeliveredTimeGet(L7_uint32 unitIndex,
                                              L7_uint32 * tstamp);

/**********************************************************************
* @purpose  get persistent log message count
*
* @param    unitIndex @b{(input)} the unit index
* @param    count @b{(output)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogPersistentLogCountGet(L7_uint32 unitIndex,
                                              L7_uint32 * count);

/**********************************************************************
* @purpose  Set the local port number of the LOG client.
*
* @param    unitIndex @b{(input)} the unit index
* @param    port number
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

extern L7_RC_t usmDbLogLocalPortSet(L7_uint32 unitIndex,
                            L7_ushort16 port);

/**********************************************************************
* @purpose  Get the local port number of the SNTP client.
*
* @param    unitIndex @b{(input)} the unit index
* @param    pointer to port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port is null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogLocalPortGet(L7_uint32 unitIndex,
                              L7_ushort16 * port);


/**
*
* Syslog host table manipulation routines
*
*
*
*
*/

/**********************************************************************
* @purpose  Set the Hostname or IP address of the host table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    address @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or
*                       null address pointer).
*
* @notes    Address is a null terminated string.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableHostSet(L7_uint32 unitIndex,
                                    L7_uint32 ndx, L7_char8 * address);

/**********************************************************************
* @purpose  Get the IP address of the host table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    address @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer).
*
* @notes    This is a null terminated string.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableIpAddressGet(L7_uint32 unitIndex,
                                  L7_uint32 ndx, L7_char8 * address);

/**********************************************************************
* @purpose  Set the row status of the host table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    status @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or u
*                       invalid status).
*
* @notes    Can only set row status to destroy, create and go,
*           or not in service.
*           Only client can set status to active. Preferred method of
*           creating a host entry is to use usmDbHostAdd.
*
*           Valid transitions are:
*
*
*           Old status      Requested Status    New Status
*           destroy         create and go       not in service
*           not in service  active              active (if valid address,
*                                               type, port), not in service
*                                               otherwise
*           active          destroy             destroy
*           active          not in service      not in service
*
*
*           destroy         create and go*      active if valid address and type
*
*           * this transition is only available through the usmDbHostAdd call.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableRowStatusSet(L7_uint32 unitIndex, L7_uint32 ndx,
                                              L7_ROW_STATUS_t status);

/**********************************************************************
* @purpose  Get the row status of the host table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    status @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument
* @returns  L7_NOT_EXIST if index out of range or no host configured.
*
* @notes    none.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableRowStatusGet(L7_uint32 unitIndex, L7_uint32 ndx,
                                          L7_ROW_STATUS_t * status);

/**********************************************************************
* @purpose  Get the row status of the next host table entry
*           specified by index.
*
* @param    index of current entry into table.
* @param    pointer to where result is stored
* @param    pointer to row index of returned result.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument
* @returns  L7_NOT_EXIST    if index out of range or no hosts exist.
*
* @notes    none.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbLogHostTableRowStatusGetNext(L7_uint32 unitIndex, L7_uint32 ndx,
                       L7_ROW_STATUS_t * status, L7_uint32 * rowNdx);


/**********************************************************************
* @purpose  Get the IP address type for the specified table entry.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    type @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableAddressTypeGet(L7_uint32 unitIndex, L7_uint32 ndx,
                                          L7_IP_ADDRESS_TYPE_t * type);

/**********************************************************************
* @purpose  Set the IP address type of the address of the
*           host for the specified index
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    type @b{(input)} the new address type
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableAddressTypeSet(L7_uint32 unitIndex, L7_uint32 ndx,
                                        L7_IP_ADDRESS_TYPE_t type);

/**********************************************************************
* @purpose  Get the IP port of the host for the specified entry.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    port @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTablePortGet(L7_uint32 unitIndex, L7_uint32 ndx,
                                  L7_ushort16 * port);

/**********************************************************************
* @purpose  Set the IP port of the
*           host for the specified index
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    port @b{(input)} the new port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTablePortSet(L7_uint32 unitIndex, L7_uint32 ndx,
                                    L7_ushort16 port);

/**********************************************************************
* @purpose  Get the component filter of the host for the specified index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableComponentGet(L7_uint32 unitIndex, L7_uint32 ndx,
                                          L7_COMPONENT_IDS_t * component);

/**********************************************************************
* @purpose  Set the component filter of the
*           host specified by the index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
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

extern L7_RC_t usmDbLogHostTableComponentSet(L7_uint32 unitIndex, L7_uint32 ndx,
                                      L7_COMPONENT_IDS_t component);

/**********************************************************************
* @purpose  Get the severity filter of the host for the specified index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
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

extern L7_RC_t usmDbLogHostTableSeverityGet(L7_uint32 unitIndex, L7_uint32 ndx,
                                    L7_LOG_SEVERITY_t * severity);

/**********************************************************************
* @purpose  Set the severity filter of the
*           host specified by the index.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_LOG_MAX_HOSTS]
* @param    severity @b{(input)} the new severity
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogHostTableSeveritySet(L7_uint32 unitIndex,
                            L7_uint32 ndx, L7_LOG_SEVERITY_t severity);

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

extern L7_RC_t usmDbLogHostTableDescriptionSet( L7_uint32 unitIndex,
                                                L7_uint32 ndx,
                                                L7_uchar8 *description );

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

extern L7_RC_t usmDbLogHostTableDescriptionGet( L7_uint32 unitIndex,
                                                L7_uint32 ndx,
                                                L7_uchar8 *description );
/**********************************************************************
* @purpose  Add a host to the host table in the first free slot.
*
* @param    unitIndex @b{(input)}  the unit index
* @param    host  @b{(input)} address or hostname of host.
* @param    type  @b{(input)} address type.
* @param    rowNdx  @b{(output)} the ndx of the added row
*
*
* @returns  L7_SUCCESS        if success
* @returns  L7_ERROR          if null pointer argument
* @returns  L7_TABLE_IS_FULL  if no room in table
*
* @notes    Update rowNdx to index of row written if successful.
*           Multiple hosts of the same address/type may exist.
*           Other parameters are set to default values.
*           Host status is set to not in service; use usmDbHostTableRowStatusSet
*           to make active.
* @end
*********************************************************************/

L7_RC_t usmDbLogHostAdd(L7_uint32 unitIndex, L7_char8 * host,
                        L7_IP_ADDRESS_TYPE_t type, L7_uint32 * rowNdx);

/**********************************************************************
* @purpose  Get the "next" row in the in-memory log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least L7_LOG_MESSAGE_LENGTH
*             bytes into which the message is written.
* @param    bufNdx @b{(output)} the true index in the log table of
*           the returned buffer.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry.
*
* @notes    The indices are continuously updated as messages are logging.
*           Therefore, a slow reader and a fast writer (lot's of logging)
*           may continuously retrieve the oldest message (which will not be
*           the same).
*           Use an ndx parameter of 0 to retrieve the oldest message in the usmDb.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogMsgInMemoryGetNext(L7_uint32 unitIndex, L7_uint32 ndx,
                              L7_char8 * buf, L7_uint32 * bufNdx);

/**********************************************************************
* @purpose  Get the specified row in the in-memory log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least L7_LOG_MESSAGE_LENGTH
*               bytes into which the message is written.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf is null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry.
*
* @notes    The indices are continuously updated as messages are logging.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogMsgInMemoryGet(L7_uint32 unitIndex, L7_uint32 ndx,
                                  L7_char8 * buf);

/**********************************************************************
* @purpose  Get the "next" row in the persistent log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)}- a buffer of at least L7_LOG_MESSAGE_LENGTH
*                 bytes into which the message is written.
* @param    bufNdx @b{(output)} the true index in the log table of
*                             the returned buffer.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry of the log.
*
* @notes    The indices are continuously updated as messages are logging.
*           Therefore, a slow reader and a fast writer (lot's of logging)
*           may continuously retrieve the oldest message (which will not be
*           the same).
*           Use an ndx parameter of 0 to retrieve the oldest message in the usmDb.
*           The startup log and the operation log are treated as one log by
*           this routine. Thus there will likely be a gap in the timestamps
*           and the indices for logging messages on systems that have been
*           running for some period of time.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogMsgPersistentGetNext(L7_uint32 unitIndex,
            L7_uint32 ndx, L7_char8 * buf, L7_uint32 * bufNdx);

/**********************************************************************
* @purpose  Get the specified row in the persistent log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least L7_LOG_MESSAGE_LENGTH
*                   bytes into which the message is written.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf is null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry of the log or less than the first entry.
*
* @notes    The indices are continuously updated as messages are logging.
*           The startup log and the operation log are treated as one log by
*           this routine. Thus there will likely be a gap in the timestamps
*           and the indices for logging messages on systems that have
*           been running for some period of time.
*
* @end
*********************************************************************/

extern L7_RC_t usmDbLogMsgPersistentGet(L7_uint32 unitIndex,
                              L7_uint32 ndx, L7_char8 * buf);

/*********************************************************************
* @purpose  Get the maximum number of syslog host entries that can be
*           simultaneously configured on this client.
*
* @param    unitIndex @b{(input)} the unit index
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

extern L7_RC_t usmDbLogMaxSyslogHostsGet(L7_uint32 unitIndex,
                              L7_uint32 * maxHosts);


/*********************************************************************
* @purpose  Attempts to get the persistent logs from another unit
*
* @param    unit  @b {(input)} unit number
*
* @returns  L7_SUCCESS, if any log retreived
*
* @notes    Copies the remote persistent logs to the local maxchine
*
* @end
*********************************************************************/
L7_RC_t usmDbLogMsgPersistentRemoteRetrieve(L7_uint32 unit);



/*********************************************************************
* @purpose      Gets the next entry from the event log
*               
*          
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 ptr       @b{(input)} Index to the next event log entry or NULL if requesting the first
* @param L7_uint32 *pstring  @b{(input)} Ptr to place the formatted event log requested
*	
* @returns      index to the next event log entry or NULL if last
*
* @notes none
*
* @end
*
*********************************************************************/
L7_uint32 usmDbLogEventLogEntryNextGet(L7_uint32 UnitIndex, L7_uint32 ptr, L7_char8 *pstring);

/*********************************************************************
* @purpose  Validate the current user configuration and Log CLI commands
*
* @param    L7_uint32 unit
* @param    L7_char8  *strHistory
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCommandLogger(L7_uint32 unit, L7_char8 *strHistory);

/*********************************************************************
* @purpose  Validate the current user configuration and Log CLI commands
*
* @param    L7_uint32  unit
* @param    L7_char8   *strHistory
* @param    L7_uint32  sessionId
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbCommandLoggerSession(L7_uint32 unit, L7_char8 *strHistory, L7_uint32 sessionId);
#endif
