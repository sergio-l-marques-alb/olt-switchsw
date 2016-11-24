/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @file     log_api.h
*
* @purpose   Log API declarations
*
* @component LOG
*
* @comments
*
* create 2/11/2004
*
* author Mclendon
*
**********************************************************************/

#ifndef __LOG_API_H__
#define __LOG_API_H__

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"

/* Number of bytes that the callout function is required to save.
*/
#define LOG_CALLOUT_BUF_SIZE  64

/* Fatal log format types. If format changes then create new constants.
*/
#define LOG_ERROR_FORMAT 0xA1A1A1A1
#define LOG_FAULT_FORMAT 0xB2B2B2B2



/**
* SECTION: MANAGEMENT
*
* This section implements the management interfaces for the log component.
*
*/

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

L7_RC_t logGlobalAdminStatusSet( L7_ADMIN_MODE_t status );

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

L7_RC_t logGlobalAdminStatusGet( L7_ADMIN_MODE_t *status );

/**********************************************************************
* @purpose  set admin mode of in memory log (if different from
*           current value.)
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

extern L7_RC_t logInMemoryAdminStatusSet(L7_ADMIN_MODE_t status);

/**********************************************************************
* @purpose  get admin mode of in memory log
*
* @param    address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t logInMemoryAdminStatusGet(L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set severity filter of in memory log (if different
*           from current value.)
*
* @param    status @b{(input)} the new severity filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new severity is same as
*           current severity
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logInMemorySeverityFilterSet(L7_LOG_SEVERITY_t sev);

/**********************************************************************
* @purpose  get severity filter of in memory log
*
* @param    sev @b{(input)} address of memory to be written
*
* @returns  L7_ERROR if null pointer argument
* @returns  L7_SUCCESS if caller memory written.
*
* @notes  Modifies memory pointed to by pointer argument.
*
* @end
*********************************************************************/

extern L7_RC_t logInMemorySeverityFilterGet(L7_LOG_SEVERITY_t * sev);

/**********************************************************************
* @purpose  set component filter of in memory log (if different.)
*
* @param    comp @b{(input)} the new component filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new component is same as
*           current component
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logInMemoryComponentFilterSet(L7_COMPONENT_IDS_t comp);

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

extern L7_RC_t logInMemoryComponentFilterGet(L7_COMPONENT_IDS_t * comp);

/**********************************************************************
* @purpose  set behavior of in memory log (if different from current value.)
*
* @param    beh @b{(input)} the new behavior
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new behavior is same
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logInMemoryBehaviorSet(L7_LOG_FULL_BEHAVIOR_t beh);

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

extern L7_RC_t logInMemoryBehaviorGet(L7_LOG_FULL_BEHAVIOR_t * beh);

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

extern L7_RC_t logInMemoryBufferSizeSet( L7_uint32 size );

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

extern L7_RC_t logInMemoryBufferSizeGet( L7_uint32 *size );

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

extern L7_RC_t logInMemoryBufferClear();

/**********************************************************************
* @purpose  set admin mode of console log (if different.)
*
* @param    status @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new status is same
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logConsoleAdminStatusSet(L7_ADMIN_MODE_t status);

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

extern L7_RC_t logConsoleAdminStatusGet(L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set severity filter of console log (if different.)
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

extern L7_RC_t logConsoleSeverityFilterSet(L7_LOG_SEVERITY_t sev);

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

extern L7_RC_t logConsoleSeverityFilterGet(L7_LOG_SEVERITY_t * sev);

/**********************************************************************
* @purpose  set component filter of console log (if different.)
*
* @param    comp @b{(input)} the new component filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new component is same
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logConsoleComponentFilterSet(L7_COMPONENT_IDS_t comp);

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

extern L7_RC_t logConsoleComponentFilterGet(L7_COMPONENT_IDS_t * comp);

/**********************************************************************
* @purpose  set admin mode of persistent log (if different.)
*
* @param    status @b{(input)} the new admin status
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new status is same as current
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logPersistentAdminStatusSet(L7_ADMIN_MODE_t status);

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

extern L7_RC_t logPersistentAdminStatusGet(L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set severity filter of persistent log (if different.)
*
* @param    sev @b{(input)} the new severity filter
*
* @returns  L7_NOT_SUPPORTED if invalid argument
* @returns  L7_ALREADY_CONFIGURED if new severity is same as current
* @returns  L7_SUCCESS if status updated.
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logPersistentSeverityFilterSet(L7_LOG_SEVERITY_t sev);

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

extern L7_RC_t logPersistentSeverityFilterGet(L7_LOG_SEVERITY_t * sev);

/**********************************************************************
* @purpose  clear the persistent operational log
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logPersistentBufferClear( void );

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

extern L7_RC_t logSyslogAdminStatusSet(L7_ADMIN_MODE_t status);

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

extern L7_RC_t logSyslogAdminStatusGet(L7_ADMIN_MODE_t * status);

/**********************************************************************
* @purpose  set default facility of syslog logging (if different.)
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

extern L7_RC_t logSyslogDefaultFacilitySet(L7_LOG_FACILITY_t fac);

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

extern L7_RC_t logSyslogDefaultFacilityGet(L7_LOG_FACILITY_t * fac);

/**********************************************************************
* @purpose  get count of in memory log messages
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

extern L7_RC_t logInMemoryLogCountGet(L7_uint32 * count);

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

extern L7_RC_t logMessagesReceivedGet(L7_uint32 * count);


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

extern L7_RC_t logMessagesDroppedGet(L7_uint32 * count);

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

L7_RC_t logInMemoryMessagesDroppedGet( L7_uint32 *count );

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

L7_RC_t logPersistentMessagesDroppedGet( L7_uint32 *count );

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

L7_RC_t logConsoleMessagesDroppedGet( L7_uint32 *count );

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
                                    L7_uint32 * count);

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

extern L7_RC_t logSyslogMessagesRelayedGet(L7_uint32 * count);

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

extern L7_RC_t logMessageReceivedTimeGet(L7_uint32 * tstamp);

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

extern L7_RC_t logSyslogMessageDeliveredTimeGet(L7_uint32 * tstamp);

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

extern L7_RC_t logPersistentLogCountGet(L7_uint32 * count);

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

extern L7_RC_t logLocalPortSet(L7_ushort16 port);

/**********************************************************************
* @purpose  Get the local port number of the SNTP client.
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

extern L7_RC_t logLocalPortGet(L7_ushort16 * port);


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
* @param    ndx @b{(input)} index of current entry into table.
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

extern L7_RC_t logHostTableHostSet(L7_uint32 ndx, L7_char8 * address);

/**********************************************************************
* @purpose  Get the IP address of the host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of current entry into table.
* @param    address @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or
*                                     null address pointer).
*
* @notes    This is a null terminated string.
*
* @end
*********************************************************************/

extern L7_RC_t logHostTableIpAddressGet(L7_uint32 ndx, L7_char8 * address);

/**********************************************************************
* @purpose  Set the row status of the host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of current entry into table.
* @param    status @b{(input)} the desired status
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or invalid status).
*
* @notes    Can only set row status to destroy,
*           create and go, or not in service.
*           Only client can set status to active. Preferred method of
*            creating a host entry is to use logHostAdd.
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
*           * this transition is only available through the logHostAdd call.
*
* @end
*********************************************************************/

extern L7_RC_t logHostTableRowStatusSet(L7_uint32 ndx, L7_ROW_STATUS_t status);


/**********************************************************************
* @purpose  Get the row status of the host table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of current entry into table.
* @param    status @b{(output)} the row status
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or invalid status).
*
* @notes    none
*
* @end
*********************************************************************/

extern L7_RC_t logHostTableRowStatusGet(L7_uint32 ndx, L7_ROW_STATUS_t * status);


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
                  L7_ROW_STATUS_t * status, L7_uint32 * rowNdx);

/**********************************************************************
* @purpose  Get the IP address type for the specified table entry.
*
* @param    ndx @b {(input)} index of entry into table.
* @param    type @b{(input)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logHostTableAddressTypeGet(L7_uint32 ndx,
                                        L7_IP_ADDRESS_TYPE_t * type);

/**********************************************************************
* @purpose  Set the IP address type of the address of the
*           host for the specified index
*
* @param    ndx @b {(input)} index of entry into table.
* @param    type @b{(input)} the desired address type
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logHostTableAddressTypeSet(L7_uint32 ndx, L7_IP_ADDRESS_TYPE_t type);

/**********************************************************************
* @purpose  Get the IP port of the host for the specified entry.
*
* @param    ndx @b {(input)} index of entry into table.
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

extern L7_RC_t logHostTablePortGet(L7_uint32 ndx, L7_ushort16 * port);

/**********************************************************************
* @purpose  Set the IP port of the
*           host for the specified index
*
* @param    ndx @b {(input)} index of entry into table.
* @param    port @b{(input)} the desired port
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logHostTablePortSet(L7_uint32 ndx, L7_ushort16 port);

/**********************************************************************
* @purpose  Get the component filter of the host for the specified index.
*
* @param    ndx @b {(input)} index of entry into table.
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

extern L7_RC_t logHostTableComponentGet(L7_uint32 ndx,
                                 L7_COMPONENT_IDS_t * component);

/**********************************************************************
* @purpose  Set the component filter of the
*           host specified by the index.
*
* @param    ndx @b{(input)} index of entry into table.
*                Must range from[1..L7_LOG_MAX_HOSTS]
* @param    component @b{(input)} the desired component
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t logHostTableComponentSet(L7_uint32 ndx,
                                 L7_COMPONENT_IDS_t component);

/**********************************************************************
* @purpose  Get the severity filter of the host for the specified index.
*
* @param    index of entry into table. Must range from[1..L7_LOG_MAX_HOSTS]
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

extern L7_RC_t logHostTableSeverityGet(L7_uint32 ndx,
                          L7_LOG_SEVERITY_t * severity);

/**********************************************************************
* @purpose  Set the severity filter of the
*           host specified by the index.
*
* @param    index of entry into table. Must range from[1..L7_LOG_MAX_HOSTS]
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

extern L7_RC_t logHostTableSeveritySet(L7_uint32 ndx, L7_LOG_SEVERITY_t severity);

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

extern L7_RC_t logHostTableDescriptionSet( L7_uint32 ndx, L7_uchar8 *description );

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

extern L7_RC_t logHostTableDescriptionGet( L7_uint32 ndx, L7_uchar8 *description );

/**********************************************************************
* @purpose  Add a host to the host table in the first free slot.
*
* @param    host - address of host or host name.
* @param    type - address type.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    Update rowNdx to index of row written if successful.
*           Multiple hosts of the same address/type may exist.
*           Other parameters are set to default values.
*           Host status is set to not in service; use logHostTableRowStatusSet
*           to make active.
* @end
*********************************************************************/

extern L7_RC_t logHostAdd(L7_char8 * host, L7_IP_ADDRESS_TYPE_t type,
                    L7_uint32 * rowNdx);

/**********************************************************************
* @purpose  Get the "next" row in the in-memory log table.
*
* @param    ndx - the putative index of a log table entry.
* @param    buf - a buffer of at least L7_LOG_MESSAGE_LENGTH bytes into
*                 which the message is written.
* @param    bufNdx - the true index in the log table of the returned buffer.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry.
*
* @notes    The indices are continuously updated as messages are logged.
*           Therefore, a slow reader and a fast writer (lot's of logging)
*           may continuously retrieve the oldest message (which will not
*           be the same).
*           Use an ndx parameter of 0 to retrieve the oldest message
*           in the log.
*
* @end
*********************************************************************/

extern L7_RC_t logMsgInMemoryGetNext(L7_uint32 ndx, L7_char8 * buf,
                              L7_uint32 * bufNdx);
/**********************************************************************
* @purpose  Get the specified row in the in-memory log table.
*
* @param    ndx - the putative index of a log table entry.
* @param    buf - a buffer of at least L7_LOG_MESSAGE_LENGTH bytes into
*                 which the message is written.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is equal to or grater than
*           the index of the last entry.
*
* @notes    The indices are continuously updated as messages are logged.
*
* @end
*********************************************************************/

extern L7_RC_t logMsgInMemoryGet(L7_uint32 ndx, L7_char8 * buf);

/**********************************************************************
* @purpose  Get the "next" row in the persistent log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least L7_LOG_MESSAGE_LENGTH bytes into
*                 which the message is written.
* @param    bufNdx @b{(output)} the true index in the log table of the returned buffer.
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
*           Use an ndx parameter of 0 to retrieve the oldest message in the log.
*           The startup log and the operation log are treated as one log by
*           this routine. Thus there will likely be a gap in the timestamps
*           and the indices for logged messages on systems that have
*           been running for some period of time.
*
* @end
*********************************************************************/

extern L7_RC_t logMsgPersistentGetNext(L7_uint32 unitIndex, L7_uint32 ndx,
                                L7_char8 * buf, L7_uint32 * bufNdx);

/**********************************************************************
* @purpose  Get the specified row in the persistent log table.
*
* @param    unitIndex @b{(input)} the unit index
* @param    ndx @b{(input)} the putative index of a log table entry.
* @param    buf @b{(output)} a buffer of at least L7_LOG_MESSAGE_LENGTH bytes into
*                 which the message is written.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if invalid parameter (buf or bufNdx are null).
* @returns  L7_NOT_EXIST if ndx parameter is 0 or equal to or grater than
*           the index of the last entry of the log.
*
* @notes    The indices are continuously updated as messages are logged.
*           The startup log and the operation log are treated as one log by
*           this routine. Thus there will likely be a gap in the timestamps
*           and the indices for logged messages on systems that have been
*           running for some period of time.
*
* @end
*********************************************************************/


extern L7_RC_t logMsgPersistentGet(L7_uint32 unitIndex,
                                  L7_uint32 ndx, L7_char8 * buf);


/*********************************************************************
* @purpose  Get the maximum number of syslog host entries that can be
*           simultaneously configured on this client.
*
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

extern L7_RC_t logMaxSyslogHostsGet(L7_uint32 * maxHosts);


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

extern L7_RC_t logMsgPersistentRemoteRetrieve(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Intialize the logging subsystem.
*           Allocate config data here.
*           Start the log server task.
*
* @param    void
*
* @returns  L7_SUCCESS - The thread and queue were successfully created
* @returns  L7_FAILURE - otherwise
*
* @notes    This function runs in the system startup thread and should
*           avoid blocking. Note that no functions may be called from here
*           that invoke logging. This sets up the logging system with the
*           default configuration. We have no other option because the
*           configurator is not running at this time.
* @end
*********************************************************************/
void loggingInit();


/*********************************************************************
* @purpose  BSP uses this routine to register functions that can 
*           be used bu the log facility to store and retrieve
*           information for fatal errors. 
*
*           The "store" function must be callable from interrupt and
*           fault handlers. Under VxWorks the function shouldn't use
*           any OS services because the OS may be corrupted. 
*
* @param    logNvStoreFuncPtrLocal(L7_uchar8 * buf)
* @param    logNvRetrieveFuncPtrLocal (L7_uchar8 * buf)
*
* @returns  none
*
* @notes    The buffer size is defined by LOG_CALLOUT_BUF_SIZE
* @notes    If callouts are not registered then the information
* @notes    about fatal errors is not saved accross resets.
*
* @end
*********************************************************************/
void loggingCalloutsInit(void (*logNvStoreFuncPtrLocal)(L7_uchar8 *buf),
                         void (*logNvRetrieveFuncPtrLocal)(L7_uchar8 *buf));

/*********************************************************************
* @purpose  Validate the current user configuration and Log CLI commands
*
* @param    L7_char8  *strHistory
* @param    L7_uint32 sessionId
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t cliCommandLoggerSession(L7_char8 *strHistory, L7_uint32 sessionId);

/*********************************************************************
* @purpose  Set the CLI command logging Admin Mode
*
* @param    L7_uint32       mode
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
logCommandLoggerAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the CLI command logging Admin Mode
*
* @param    L7_uint32      *mode
*
* returns   L7_SUCCESS
*
* returns   L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
logCommandLoggerAdminModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Validate the current user configuration and Log CLI commands
*
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
L7_RC_t cliCommandLogger(L7_char8 *strHistory);

#endif
