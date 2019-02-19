/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_sntp_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments Any manager may call any of these functions at anytime.
*           Some of these functions are blocking. No external locking
*           protocol is presumed. These functions are thread-safe.
*           These functions are reentrant. Some of the status functions
*           MAY present inconsistent data under circumstances where
*           the variables are being updated during the processing of
*           an SNTP packet. In this situation, the client may re-read
*           the data. Note that this is a very unlikely
*           situation, but a finite possibility does exist that it may occur.
*
* @create 1/12/2004
*
* @author mclendon
*
* @end
*
**********************************************************************/

#ifndef __USMDB_SNTP_API_H__
#define __USMDB_SNTP_API_H__

#include "l7_common.h"
#include "sntp_api.h"
#include "sntp_exports.h"

/*********************************************************************
* @purpose  Set the mode of the SNTP client.
*
* @param    unitIndex @b{(input)} stackable index
* @param    SntpClientMode
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if mode is not valid
*                       (L7_uint32 unitIndex, i.e. not supported or not a member of SntpClientMode)
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientModeSet(L7_uint32 unitIndex,
                              L7_SNTP_CLIENT_MODE_t mode);

/*********************************************************************
* @purpose  Get the mode of the SNTP client.
*
* @param    unitIndex @b{(input)} stackable index
* @param    mode @b{(output)} pointer to SntpClientMode variable
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

extern L7_RC_t usmDbSntpClientModeGet(L7_uint32 unitIndex,
                              L7_SNTP_CLIENT_MODE_t * mode);

/*********************************************************************
* @purpose  Set the local port number of the SNTP client.
*
* @param    unitIndex @b{(input)} stackable index
* @param    port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port number is not valid (e.g. 0)
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientPortSet(L7_uint32 unitIndex,
                              L7_ushort16 port);

/*********************************************************************
* @purpose  Get the local port number of the SNTP client.
*
* @param    unitIndex @b{(input)} stackable index
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
extern L7_RC_t usmDbSntpClientPortGet(L7_uint32 unitIndex,
                                  L7_ushort16 * port);

/*********************************************************************
* @purpose  Set the local port number of the SNTP client to the default.
*
* @param    unitIndex @b{(input)} stackable index
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port number is not valid (e.g. 0)
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientPortDefaultSet(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Set the unicast poll interval in seconds as a power of two
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval @b{(input)} power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollIntervalSet(L7_uint32 unitIndex,
                                    L7_uint32 interval);

/*********************************************************************
* @purpose  Get the unicast poll interval in seconds as a power of two
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval @b{(output)} pointer to poll interval storage
*
*
* @returns  L7_SUCCESS  if storage updates
* @returns  L7_ERROR    if invalid pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollIntervalGet(L7_uint32 unitIndex,
                                          L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the unicast poll interval to the default
*
* @param    unitIndex @b{(input)} stackable index
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollIntervalDefaultSet(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Set the unicast poll timeout in seconds
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval - the unicast poll timeout in seconds
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid interval
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollTimeoutSet(L7_uint32 unitIndex,
                                          L7_uint32 interval);

/*********************************************************************
* @purpose  Get the unicast poll timeout in seconds
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval @b{(output)} pointer to where result is to be stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollTimeoutGet(L7_uint32 unitIndex,
                                              L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the unicast poll timeout to the default
*
* @param    unitIndex @b{(input)} stackable index
*
*
* @returns  L7_SUCCESS  if argument is in range
* @returns  L7_ERROR    if invalid poll timeout
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollTimeoutDefaultSet(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Set the unicast poll retry count
*
* @param    retry @b{(input)} retry count
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if out of range retry count
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollRetrySet(L7_uint32 unitIndex,
                                              L7_uint32 retry);

/*********************************************************************
* @purpose  Get the unicast poll interval in seconds as a power of two
*
* @param    unitIndex @b{(input)} stackable index
* @param    retry @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollRetryGet(L7_uint32 unitIndex,
                                            L7_uint32 * retry);

/*********************************************************************
* @purpose  Set the unicast poll retry count to the default
*
* @param    unitIndex @b{(input)} stackable index
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if out of range retry count
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientUnicastPollRetryDefaultSet(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Get the highest SNTP version the client supports
*
* @param    unitIndex @b{(input)} stackable index
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
extern L7_RC_t usmDbSntpClientVersionGet(L7_uint32 unitIndex,
                                              L7_uint32 * version);

/*********************************************************************
* @purpose  Get the supported mode
*
* @param    unitIndex @b{(input)} stackable index
* @param    mode @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*           This is a bitmap that is a logical 'or' of all the
*           supported modes from "L7_SNTP_SUPPORTED_MODE_t"
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientSupportedModeGet(L7_uint32 unitIndex,
                                        L7_SNTP_SUPPORTED_MODE_t * mode);

/*********************************************************************
* @purpose  Get the local date and time (L7_uint32 unitIndex, UTC)
*           the SNTP client last updated the clock.
*
* @param    unitIndex @b{(input)} stackable index
* @param    lastUpdateTime @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*           This is the last update time in seconds adjusted
*           for the particular machine, e.g. seconds since
*           00:00:00 UTC, January 1, 1970 for a un*x style machine.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpLastUpdateTimeGet(L7_uint32 unitIndex,
                                        L7_uint32 * lastUpdateTime);

/*********************************************************************
* @purpose  Get the local date and time (L7_uint32 unitIndex, UTC)
*           of the last SNTP request (unicast mode) or receipt
*           of an unsolicited message (broadcast mode).
*
* @param    unitIndex @b{(input)} stackable index
* @param    lastAttemptTime @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*           This is the last update time in seconds adjusted
*           for the particular machine, e.g. seconds since
*           00:00:00 UTC, January 1, 1970 for a un*x style machine.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpLastAttemptTimeGet(L7_uint32 unitIndex,
                                    L7_uint32 * lastAttemptTime);

/*********************************************************************
* @purpose  Get the status of the last SNTP request (in unicast mode)
*           or unsolicited message (in broadcast mode).
*
* @param    unitIndex @b{(input)} stackable index
* @param    status @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpLastAttemptStatusGet(L7_uint32 unitIndex,
                              L7_SNTP_PACKET_STATUS_t * status);

/*********************************************************************
* @purpose  Get the IP address of the server for the last received valid
*           packet.
*
* @param    unitIndex @b{(input)} stackable index
* @param    address @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    This is a null terminated string.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerIpAddressGet(L7_uint32 unitIndex,
                                          L7_char8 * address);

/*********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
* @param    unitIndex @b{(input)} stackable index
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
extern L7_RC_t usmDbSntpServerAddressTypeGet(L7_uint32 unitIndex,
                                        L7_SNTP_ADDRESS_TYPE_t * type);

/*********************************************************************
* @purpose  Get the claimed stratum of the server for the last received
*           valid packet.
*
* @param    unitIndex @b{(input)} stackable index
* @param    stratuim @b{output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerStratumGet(L7_uint32 unitIndex,
                                            L7_uint32 * stratum);

/*********************************************************************
* @purpose  Get the reference clock identifier of the server for the
*           last received valid packet.
*
* @param    unitIndex @b{(input)} stackable index
* @param    refClockId @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerRefIdGet(L7_uint32 unitIndex,
                                        L7_char8 * refClockId);

/*********************************************************************
* @purpose  Get the mode of the server for the
*           last received valid packet.
*
* @param    unitIndex @b{(input)} stackable index
* @param    serverMode @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerModeGet(L7_uint32 unitIndex,
                                L7_SNTP_SERVER_MODE_t * serverMode);

/*********************************************************************
* @purpose  Get the maximum number of unicast server entries that can be
*           simultaneously configured on this client.
*
* @param    unitIndex @b{(input)} stackable index
* @param    maxServerEntries @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerMaxEntriesGet(L7_uint32 unitIndex,
                                    L7_uint32 * maxServerEntries);

/*********************************************************************
* @purpose  Get the current number of unicast server entries that are
*           simultaneously configured on this client.
*
* @param    unitIndex @b{(input)} stackable index
* @param    currentServerEntries @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerCurrentEntriesGet(L7_uint32 unitIndex,
                                L7_uint32 * currentServerEntries);

/*********************************************************************
* @purpose  Get the current number of unsolicited broadcast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @param    unitIndex @b{(input)} stackable index
* @param    broadcastCount @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpBroadcastCountGet(L7_uint32 unitIndex,
                                L7_uint32 * broadcastCount);

/*********************************************************************
* @purpose  Get the current number of valid unsolicited multicast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @param    unitIndex @b{(input)} stackable index
* @param    multicastCount @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpMulticastCountGet(L7_uint32 unitIndex,
                                    L7_uint32 * multicastCount);

/*********************************************************************
* @purpose  Set the multicast poll interval in seconds as a power of two
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval @b{(input)} power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientMulticastPollIntervalSet(L7_uint32 unitIndex,
                                              L7_uint32 interval);

/*********************************************************************
* @purpose  Get the multicast poll interval in seconds as a power of two
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval @b{input)} pointer to poll interval storage
*
*
* @returns  L7_SUCCESS  if storage updates
* @returns  L7_ERROR    if invalid pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientMulticastPollIntervalGet(L7_uint32 unitIndex,
                                                L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the multicast poll interval to the default
*
* @param    unitIndex @b{(input)} stackable index
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientMulticastPollIntervalDefaultSet(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Set the broadcast poll interval in seconds as a power of two
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval @b{input)} power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientBroadcastPollIntervalSet(L7_uint32 unitIndex,
                                                    L7_uint32 interval);

/*********************************************************************
* @purpose  Get the broadcast poll interval in seconds as a power of two
*
* @param    unitIndex @b{(input)} stackable index
* @param    interval @b{output)} pointer to poll interval storage
*
*
* @returns  L7_SUCCESS  if storage updates
* @returns  L7_ERROR    if invalid pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientBroadcastPollIntervalGet(L7_uint32 unitIndex,
                                                    L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the broadcast poll interval to the default
*
* @param    unitIndex @b{(input)} stackable index
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpClientBroadcastPollIntervalDefaultSet(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Add a server entry in the next available free slot.
*
* @param    unitIndex @b{(input)} stackable index
* @param    address @b{(input)} server address string (L7_uint32 unitIndex, null terminated)
* @param    address type @b{(input)} server address type.
* @param    ndx @b{(output)} pointer to storage where index of newly added row is stored.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (null address or invalid address type).
* @returns  L7_ALREADY_CONFIGURED  if duplicate address/address type.
* @returns  L7_TABLE_IS_FULL  if no free slots in table.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerAdd(L7_uint32 unitIndex, L7_char8 * address,
                            L7_SNTP_ADDRESS_TYPE_t type, L7_uint32 * ndx);
/**********************************************************************
* @purpose  Set/Create the IP address of the server table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    address @b{(output)} pointer to where result
*           is stored (must be at least 64 bytes)
* @param    type @b{(input}) address type of the new server address
*           (all types supported)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer).
*
* @notes     - Address is a null terminated string.
*           Clears the individual server stats if address is changed.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableIpAddressAdd(L7_uint32 ndx, L7_char8 *address,
                                                L7_SNTP_ADDRESS_TYPE_t type);
/*********************************************************************
* @purpose  Delete a matching server entry.
*
* @param    unitIndex @b{(input)} stackable index
* @param    address @b{(input)} server address string (null terminated)
* @param    address type @b{(input)} server address type.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or invalid status).
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerDelete(L7_uint32 unitIndex, L7_char8 * address,
                                    L7_SNTP_ADDRESS_TYPE_t type);

/*********************************************************************
* @purpose  Set the status of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    status @b{(input)} new row status
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or invalid status).
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableRowStatusSet(L7_uint32 unitIndex,
                                  L7_uint32 ndx,
                                  L7_SNTP_SERVER_STATUS_t status);

/*********************************************************************
* @purpose  Get the row status of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (null status pointer).
* @returns  L7_NOT_EXIST    if ndx out of range
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableRowStatusGet(L7_uint32 unitIndex,
                                  L7_uint32 ndx,
                                  L7_SNTP_SERVER_STATUS_t * status);
/*********************************************************************
* @purpose  Get the row status of the "next" server table entry
*           as specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
* @param    pointer to where result is stored
* @param    pointer to index of retrieved row
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (null status pointer or
*                       null rowNdx pointer).
* @returns  L7_NOT_EXIST    if ndx out of range [0..L7_MAX_SERVER_ENTRIES-1]
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableRowStatusGetNext(L7_uint32 unitIndex,
    L7_uint32 ndx, L7_SNTP_SERVER_STATUS_t * status, L7_uint32 * rowNdx);

/*********************************************************************
* @purpose  Set the IP address of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to new address string (L7_uint32 unitIndex,
*                            NULL terminated)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (unitIndex,
*                  ndx out of range or null address pointer).
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableIpAddressSet(L7_uint32 unitIndex,
                  L7_uint32 ndx, L7_char8 * address);

/*********************************************************************
* @purpose  Get the IP address of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or
*                       null address pointer).
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableIpAddressGet(L7_uint32 unitIndex,
                                L7_uint32 ndx, L7_char8 * address);

/*********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of table. Ranges
*                           from [1..L7_SNTP_MAX_SERVERS]
* @param    type @b{(input)} new address type
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid index or invalid address type
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableAddressTypeSet(L7_uint32 unitIndex,
                        L7_uint32 ndx, L7_SNTP_ADDRESS_TYPE_t type);

/*********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of table.
*           Ranges from [1..L7_SNTP_MAX_SERVERS]
* @param    type @b{(output)} pointer to where result is stored.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableAddressTypeGet(L7_uint32 unitIndex,
                            L7_uint32 ndx, L7_SNTP_ADDRESS_TYPE_t * type);

/*********************************************************************
* @purpose  Set the priority of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry. Must
*                 range from[1..L7_MAX_SERVER_ENTRIES]
* @param    priority @b{(input)} new priority
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer) or entry does not exist.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTablePrioritySet(L7_uint32 unitIndex,
                                L7_uint32 ndx, L7_ushort16 priority);

/*********************************************************************
* @purpose  Enables SNTP polling of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} index of stack unit
* @param    ndx @b{(input)} index of entry in server table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    poll @b{(input)}  enable/disable
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer)
*                       or entry does not exist.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTablePollSet(L7_uint32 unitIndex,
                              L7_uint32 ndx, L7_ushort16 poll);

/*********************************************************************
* @purpose  Gets the status of SNTP polling of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} index of stack unit
* @param    ndx @b{(input)} index of entry in server table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    poll @b{(input)}  enable/disable
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer)
*                       or entry does not exist.
*
* @end
*********************************************************************/
extern L7_BOOL usmDbSntpServerTablePollGet(L7_uint32 unitIndex,
                              L7_uint32 ndx, L7_ushort16 * poll);

/*********************************************************************
* @purpose  Get the priority of the server table entry
*           specified by index.
*
* @param    priority @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTablePriorityGet(L7_uint32 unitIndex,
                            L7_uint32 ndx, L7_ushort16 * priority);

extern L7_RC_t usmDbSntpServerIndexGet(L7_char8 *ipAddr, L7_uint32 *index);
/*********************************************************************
* @purpose  Set the priority of the server table entry
*           specified by index to the default.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry
*                         Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer)
*                       or entry does not exist.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTablePriorityDefaultSet(L7_uint32 unitIndex,
                                                    L7_uint32 ndx);

/*********************************************************************
* @purpose  Set the NTP version of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    version @b{(input)} new NTP version of the server
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer) or entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableVersionSet(L7_uint32 unitIndex,
                                  L7_uint32 ndx, L7_ushort16 version);

/*********************************************************************
* @purpose  Get the version of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    version @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableVersionGet(L7_uint32 unitIndex,
                              L7_uint32 ndx, L7_ushort16 * version);

/*********************************************************************
* @purpose  Set the NTP version of the server table entry
*           specified by index to the default.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                   Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer)
*                       or entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTableVersionDefaultSet(L7_uint32 unitIndex,
                                                          L7_uint32 ndx);

/*********************************************************************
* @purpose  Set the NTP port of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                 Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    port @b{(output)} NTP port used by the server
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null
*                       address pointer) or entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTablePortSet(L7_uint32 unitIndex,
                                  L7_uint32 ndx, L7_ushort16 port);

/*********************************************************************
* @purpose  Get the port of the server table entry
*           specified by index.
*
* @param    unitIndex @b{(input)} stackable index
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
extern L7_RC_t usmDbSntpServerTablePortGet(L7_uint32 unitIndex,
                            L7_uint32 ndx, L7_ushort16 * port);

/*********************************************************************
* @purpose  Set the NTP port of the server table entry
*           specified by index to the default.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                         Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer)
*                       or entry does not exist.
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSntpServerTablePortDefaultSet(L7_uint32 unitIndex,
                                                      L7_uint32 ndx);


/**********************************************************************
* @purpose  Get the SNTP last update time for the specified server entry.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                         Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    ttime @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpServerStatsLastUpdateTimeGet(L7_uint32 unitIndex,
                                L7_uint32 ndx, L7_uint32 * ttime);

/**********************************************************************
* @purpose  Get the SNTP last attempt time for the specified server entry.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                 Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    ttime @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpServerStatsLastAttemptTimeGet(L7_uint32 unitIndex,
                                  L7_uint32 ndx, L7_uint32 * ttime);

/**********************************************************************
* @purpose  Get the SNTP last update status for the specified server entry.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                     Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    status @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpServerStatsLastUpdateStatusGet(L7_uint32 unitIndex,
                            L7_uint32 ndx, L7_SNTP_PACKET_STATUS_t * status);

/**********************************************************************
* @purpose  Get the SNTP unicast server requests sent to the specified
*           server entry.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                 Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    requests @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpServerStatsUnicastServerNumRequestsGet(L7_uint32 unitIndex,
                          L7_uint32 ndx, L7_uint32 * requests);

/**********************************************************************
* @purpose  Get the SNTP unicast server requests sent to the
*           specified server entry.
*
* @param    unitIndex @b{(input)} stackable index
* @param    ndx @b{(input)} index of server table entry.
*                       Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    requests @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpServerStatsUnicastServerNumFailedRequestsGet(L7_uint32 unitIndex,
                                        L7_uint32 ndx, L7_uint32 * requests);

/*********************************************************************
* @purpose  Get the current authentication mode for this client
*
* @param    L7_BOOL *enable @b{(output)} pointer to return value L7_TRUE/L7_FALSE
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE  
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpClientAuthenticateModeGet(L7_BOOL *enable); 

/*********************************************************************
* @purpose  Set the current authentication mode for this client
*
* @param    L7_BOOL enable @b{(input)} L7_TRUE to enable, L7_FALSE to disable
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpClientAuthenticateModeSet(L7_BOOL enable);

/*********************************************************************
* @purpose  Set the authentication key for an SNTP server table entry
*           
* @param    L7_uint32 index     @b{(input)} index of server table entry.
*                                           Must range from[1..L7_SNTP_MAX_SERVERS]
*
* @param    L7_uint32 keyIndex  @b{(input)} index of authenticate table entry.
*                                           range 1-L7_SNTP_MAX_AUTH_KEYS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpServerTableAuthenticateKeySet(L7_uint32 index, 
                                               L7_uint32 keyIndex);

/*********************************************************************
* @purpose  Get the authentication key for a particular sntp server
*           
*
* @param    L7_uint32  index      @b{(input)} index of server table entry.
*                                       Must range from[1..L7_SNTP_MAX_SERVERS]
*
* @param    L7_uint32 *keyIndex   @b{(output)} index of authenticate table entry.
*                                              range 1-L7_SNTP_MAX_AUTH_KEYS
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (index out of range) or 
*                       server doesn't have any authentication key assigned
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpServerTableAuthenticateKeyGet(L7_uint32  index, 
                                               L7_uint32 *keyIndex);

/* TBD - REMOVE THIS API, OR CHANGE (per key?), should not return SNMP formatted buffer! */
/**********************************************************************
* @purpose  Get the SNTP Client Authentication algorithm used. E.g Md5 
*           specified server entry.
*
* @param    buffer @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpClientAuthenticateTypeGet(L7_char8 *buffer);
/* TBD - REMOVE THIS API, OR CHANGE (per key?), should not return SNMP formatted buffer! */

/*********************************************************************
* @purpose  Add a new authentication table entry specified by key number and value.                       
*
*
* @param    L7_uint32  keyNumber @b{(input)}  SNTP key id, range L7_SNTP_KEY_NUMBER_MIN - 
*                                                                L7_SNTP_KEY_NUMBER_MAX
*                             
* @param    L7_char8  *keyValue  @b{(input)}  ASCII key string L7_SNTP_KEY_LEN_MIN-
*                                                              L7_SNTP_KEY_LEN_MAX
* @param    L7_uint32 *index     @b{(output)} index of authenticate table entry.
*                                              range 1-L7_SNTP_MAX_AUTH_KEYS
*
*
* @returns  L7_SUCCESS        if success
* @returns  L7_FAILURE        if invalid argument, key out of range 
*           L7_TABLE_IS_FULL  no more authentication keys can be added
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyAdd(L7_uint32  keyNumber,
                                         L7_char8  *keyValue,
                                         L7_uint32 *index);

/*********************************************************************
* @purpose  Delete an authentication table entry specified by key number.                       
*
*
* @param    L7_uint32 keyNumber  @b{(input)} SNTP key id, range L7_SNTP_KEY_NUMBER_MIN - 
*                                                               L7_SNTP_KEY_NUMBER_MAX
*                             
*
* @returns  L7_SUCCESS        if success
* @returns  L7_FAILURE        if invalid argument, key does not exist
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyDelete(L7_uint32  keyNumber);

/*********************************************************************
* @purpose  Get an authentication table index specified by key number.                       
*
*
* @param    L7_uint32 keyNumber  @b{(input)} SNTP key id, range L7_SNTP_KEY_NUMBER_MIN - 
*                                                               L7_SNTP_KEY_NUMBER_MAX
*                             
* @param    L7_uint32 *index     @b{(output)} index of authenticate table entry.
*                                              range 1-L7_SNTP_MAX_AUTH_KEYS
*
*
* @returns  L7_SUCCESS        if success
* @returns  L7_FAILURE        if invalid argument, key out of range 
*
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableIndexGet(L7_uint32  keyNumber,
                                           L7_uint32 *index);

/*********************************************************************
* @purpose  Set the status of the authenticate table entry
*           specified by index.
*
* @param    L7_uint32             index  @b{(input)} index of authenticate table entry.
*                                                    range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_SNTP_SERVER_STATUS status @b{(input)} new row status
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range
*                       or invalid status).
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableRowStatusSet(L7_uint32  index,
                                                      L7_SNTP_SERVER_STATUS_t status);

/*********************************************************************
* @purpose  Get the row status of the authenticate table entry
*           specified by index.
*
* @param    L7_uint32             index  @b{(input)}  index of authenticate table entry
*                                                     range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_SNTP_SERVER_STATUS status @b{(output)} pointer to return current row status
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (null status pointer).
* @returns  L7_NOT_EXIST    if index out of range
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableRowStatusGet(L7_uint32                index,
                                                      L7_SNTP_SERVER_STATUS_t *status);

/*********************************************************************
* @purpose  Get the row status of the "next" server table entry
*           as specified by the current index.
*
* @param    L7_uint32             index     @b{(input)}  index of current authenticate table entry
*                                                        range 0-L7_SNTP_MAX_AUTH_KEYS-1
* @param    L7_SNTP_SERVER_STATUS status    @b{(output)} pointer to return next row status
* @param    L7_uint32             nextIndex @b((output)) pointer to return next row index
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (null status pointer or
*                       null rowIndex pointer).
* @returns  L7_NOT_EXIST    if index out of range [0..L7_SNTP_MAX_AUTH_KEYS-1]
*
* @notes   use index of 0 to get first valid index.
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableRowStatusGetNext(L7_uint32                index,
                                                          L7_SNTP_SERVER_STATUS_t *status,
                                                   L7_uint32               *nextIndex);

/*********************************************************************
* @purpose  Set the key number of the authenticate table entry
*           specified by index.
*
* @param    L7_uint32   index     @b{(input)} index of authenticate table entry
*                                             range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_uint32   keyNumber @b{(input)} SNTP key id, range L7_SNTP_KEY_NUMBER_MIN - 
*                                                                L7_SNTP_KEY_NUMBER_MAX
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyNumberSet(L7_uint32 index,
                                                      L7_uint32 keyNumber);

/*********************************************************************
* @purpose  Get the key number of the authenticate table entry
*           specified by index.
*
* @param    L7_uint32   index     @b{(input)} index of authenticate table entry
*                                             range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_uint32  *keyNumber @b{(output)} pointer to store key number 1 - 4294967295
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyNumberGet(L7_uint32  index,
                                                      L7_uint32 *keyNumber);

/*********************************************************************
* @purpose  Set the key value of the authenticate table entry
*           specified by index.
*
* @param    L7_uint32   index     @b{(input)} index of authenticate table entry
*                                             range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_char8   *keyValue  @b{(input)} ASCII key string L7_SNTP_KEY_LEN_MIN-
*                                                              L7_SNTP_KEY_LEN_MAX
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyValueSet(L7_uint32 index,
                                              L7_char8 *keyValue);

/*********************************************************************
* @purpose  Get the key number of the authenticate table entry
*           specified by index.
*
* @param    L7_uint32   index     @b{(input)}  index of authenticate table entry
*                                              range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_char8   *keyValue  @b{(output)} ASCII key string L7_SNTP_KEY_LEN_MIN-
*                                                               L7_SNTP_KEY_LEN_MAX
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyValueGet(L7_uint32  index,
                                              L7_char8  *keyValue);

/*********************************************************************
* @purpose  Set a key as trusted or untrusted specified by the authenticate 
*            table entry index.
*
* @param    L7_uint32   index     @b{(input)}  index of authenticate table entry
*                                              range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_BOOL     trusted   @b{(input)}  L7_TRUE or L7_FALSE
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyTrustedSet(L7_uint32 index,
                                                     L7_BOOL   trusted);

/*********************************************************************
* @purpose  Get the key number of the authenticate table entry
*           specified by index.
*
* @param    L7_uint32   index     @b{(input)}  index of authenticate table entry
*                                              range 1-L7_SNTP_MAX_AUTH_KEYS
* @param    L7_BOOL    *trusted   @b{(output)}  pointer to L7_TRUE or L7_FALSE
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbSntpAuthenticateTableKeyTrustedGet(L7_uint32  index,
                                                     L7_BOOL   *trusted);
#endif
