/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename sntp_api.h
*
* @purpose SNTP Client public interface
*
* @component SNTP
*
* @comments Any manager may call any of these functions at anytime.
*           Some of these functions are blocking. No external locking
*           protocol is presumed. These functions are thread-safe.
*           These functions are reentrant. Some of the status functions
*           MAY present inconsistent data under circumstances where
*           the variables are being updated during the processing of
*           a packet. In this situation, the client may re-read the data.
*           Note that this is a very unlikely situation, but a finite
*           possibility does exist that it may occur.
*
* @create 1/12/2004
*
* @author mclendon
*
* @end
*
**********************************************************************/

#ifndef __SNTP_API_H__
#define __SNTP_API_H__

#include "l7_common.h"
#include "osapi.h"
#include "sntp_exports.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "sntp_exports.h"

/* FD Imports - Items below that can be configured are aliased to FD_xxx defines.
*  Items that may NOT be altered for some reason, e.g. they are fixed by
*  by the protocol, are defined here to make them available to users of SNTP.
*/

#define L7_SNTP_DEFAULT_STACK_SIZE       FD_CNFGR_SNTP_DEFAULT_STACK_SIZE
#define L7_SNTP_DEFAULT_TASK_SLICE       FD_CNFGR_SNTP_DEFAULT_TASK_SLICE
#define L7_SNTP_DEFAULT_TASK_PRI         FD_CNFGR_SNTP_DEFAULT_TASK_PRI
/* Maximum messages in SNTP configurator queue */
#define L7_SNTP_MSG_COUNT                FD_CNFGR_SNTP_MSG_COUNT

/* Start of CONFIGURATOR SNTP Component's Factory Defaults */

#define L7_SNTP_DEFAULT_CLIENT_MODE      FD_SNTP_DEFAULT_CLIENT_MODE
/* The default local client port to use. */
#define L7_SNTP_DEFAULT_CLIENT_PORT      FD_SNTP_DEFAULT_CLIENT_PORT
#define L7_SNTP_DEFAULT_UNICAST_CLIENT_PORT  FD_SNTP_DEFAULT_UNICAST_CLIENT_PORT
/* The default remote server port to use. */
#define L7_SNTP_DEFAULT_SERVER_PORT      FD_SNTP_DEFAULT_SERVER_PORT
#define L7_SNTP_DEFAULT_VERSION          FD_SNTP_DEFAULT_VERSION
#define L7_SNTP_DEFAULT_POLL_INTERVAL    FD_SNTP_DEFAULT_POLL_INTERVAL
#define L7_SNTP_DEFAULT_POLL_TIMEOUT     FD_SNTP_DEFAULT_POLL_TIMEOUT
#define L7_SNTP_DEFAULT_POLL_RETRY       FD_SNTP_DEFAULT_POLL_RETRY
#define L7_SNTP_DEFAULT_SERVER_PRIORITY  FD_SNTP_DEFAULT_SERVER_PRIORITY
#define L7_SNTP_DEFAULT_SERVER_POLL      FD_SNTP_DEFAULT_SERVER_POLL    

#define L7_SNTP_DEFAULT_TRUSTEDKEY       FD_SNTP_DEFAULT_TRUSTEDKEY
#define L7_SNTP_DEFAULT_AUTH_MODE        FD_SNTP_DEFAULT_AUTH_MODE

/* Dates before this value are silently discarded by the SNTP client. */
#define L7_SNTP_DISCARD_TIME             FD_SNTP_DISCARD_TIME

/* The minimum correction to the local clock. Timestamps that
* differ from the local clock by less than this amount are
* not applied to the local clock. They are still used in the
* clock filtering algorithm.
*/
#define L7_SNTP_MIN_CORRECTION           SNTP_MIN_CORRECTION

/* Maximum number of servers supported. Three is plenty. */
#define L7_SNTP_MAX_SERVERS              FD_SNTP_MAX_SERVERS

/* Maxiumum number of authentication keys supported */
#define L7_SNTP_MAX_AUTH_KEYS            FD_SNTP_MAX_AUTH_KEYS

/* Valid key string length values */
#define L7_SNTP_KEY_LEN_MIN              1
#define L7_SNTP_KEY_LEN_MAX              8

/* Maximum length of an address string - string is null terminated */
#define L7_SNTP_MIN_ADDRESS_LEN          1
#define L7_SNTP_MAX_ADDRESS_LEN          FD_SNTP_MAX_ADDRESS_LEN


/* Th maximum number of packets received from unconfigured sources before failing the poll */
#define SNTP_MAX_SPOOFED_PACKETS          FD_SNTP_MAX_SPOOFED_PACKETS

#define SNTP_MIN_SERVER_PRIORITY          1
#define SNTP_MAX_SERVER_PRIORITY          FD_SNTP_MAX_SERVERS
#define SNTP_MIN_SERVER_KEYID          1
#define SNTP_MAX_SERVER_KEYID          0xffffffff
#define SNTP_MIN_CLIENT_POOL_TIMER     64
#define SNTP_MAX_CLIENT_POOL_TIMER     1024
#define L7_SNTP_KEY_NUMBER_MIN         1
#define L7_SNTP_KEY_NUMBER_MAX         0xffffffff

#define SNTP_MIN_SERVER_KEYID          1
#define SNTP_MAX_SERVER_KEYID          0xffffffff
#define SNTP_MIN_CLIENT_POOL_TIMER     64
#define SNTP_MAX_CLIENT_POOL_TIMER     1024
#define L7_SNTP_KEY_NUMBER_MIN         1
#define L7_SNTP_KEY_NUMBER_MAX         0xffffffff

/* Version used in transmitted packets */
#define SNTP_VERSION_MIN                  1        /* The minimum valid version */
#define SNTP_VERSION_MAX                  4        /* The maximum valid version */


#define SNTP_STRATUM_MIN                  1         /* The minimum valid stratum */
#define SNTP_STRATUM_MAX                  15        /* The maximum valid stratum */

/* Poll intervals are in power of two seconds, e.g. 10 -> 1024 seconds, 6 -> 64 seconds, etc. */
#define SNTP_MIN_POLL_INTERVAL        6
#define SNTP_MAX_POLL_INTERVAL        10


/* Timeout values are in seconds, e.g. 10 -> 10 seconds */
#define SNTP_MIN_POLL_TIMEOUT       1
#define SNTP_MAX_POLL_TIMEOUT      30

/* Retry values have no units. Just a cardinal value. */
#define SNTP_MIN_POLL_RETRY       0
#define SNTP_MAX_POLL_RETRY       10

/* The "well-known" NTP multicast group address */
#define SNTP_MULTICAST_GROUP_ADDRESS  "224.0.1.1"

/* Client & server port range limits */
#define SNTP_MIN_CLIENT_PORT      1
#define SNTP_MAX_CLIENT_PORT      65535
#define SNTP_MIN_SERVER_PORT      1
#define SNTP_MAX_SERVER_PORT      65535


/*********************************************************************
* @purpose  Set the mode of the SNTP client.
*
* @param    SntpClientMode
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if mode is not valid
*                       (i.e. not supported or not a
*                       member of SntpClientMode)
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientModeSet(L7_SNTP_CLIENT_MODE_t mode);

/*********************************************************************
* @purpose  Get the mode of the SNTP client.
*
* @param    pointer to SntpClientMode variable
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/

extern L7_RC_t sntpClientModeGet(L7_SNTP_CLIENT_MODE_t * mode);

/*********************************************************************
* @purpose  Set the local port number of the SNTP client.
*
* @param    port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port number is not valid (e.g. 0)
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientPortSet(L7_ushort16 port);

/*********************************************************************
* @purpose  Get the local port number of the SNTP client.
*
* @param    pointer to port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port is null pointer
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientPortGet(L7_ushort16 * port);

/*********************************************************************
* @purpose  Set the local port number of the SNTP client to the default.
*
* @param    None
*
*
* @returns  L7_SUCCESS  if success
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientPortDefaultSet();

/*********************************************************************
* @purpose  Set the unicast poll interval in seconds as a power of two
*
* @param    power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes    FS Section 8.1.1
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollIntervalSet(L7_uint32 interval);

/*********************************************************************
* @purpose  Set the unicast poll interval to the default value.
*
* @param    None.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes    FS Section 8.1.1
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollIntervalDefaultSet();

/*********************************************************************
* @purpose  Get the unicast poll interval in seconds as a power of two
*
* @param    pointer to poll interval storage
*
*
* @returns  L7_SUCCESS  if storage updates
* @returns  L7_ERROR    if invalid pointer
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollIntervalGet(L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the unicast poll timeout in seconds
*
* @param    poll timeout
*
*
* @returns  L7_SUCCESS  if argument is in range
* @returns  L7_ERROR    if invalid poll timeout
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollTimeoutSet(L7_uint32 interval);

/*********************************************************************
* @purpose  Get the unicast poll timeout in seconds
*
* @param    pointer to where result is to be stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollTimeoutGet(L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the unicast poll timeout in seconds to the default
*
* @param    None.
*
*
* @returns  L7_SUCCESS  if argument is in range
* @returns  L7_ERROR    if invalid poll timeout
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollTimeoutDefaultSet();

/*********************************************************************
* @purpose  Set the unicast poll retry count
*
* @param    retry count
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if out of range retry count
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollRetrySet(L7_uint32 retry);

/*********************************************************************
* @purpose  Get the unicast poll interval in seconds as a power of two
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollRetryGet(L7_uint32 * retry);

/*********************************************************************
* @purpose  Set the unicast poll retry count to the default value
*
* @param    None
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if out of range retry count
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientUnicastPollRetryDefaultSet();

/*********************************************************************
* @purpose  Get the highest SNTP version the client supports
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientVersionGet(L7_uint32 * version);


/*********************************************************************
* @purpose  Get the supported mode
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*           This is a bitmap that is a logical 'or' of all the
*           supported modes from "L7_SNTP_SUPPORTED_MODE_t"
*
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientSupportedModeGet( L7_SNTP_SUPPORTED_MODE_t * version);


/*********************************************************************
* @purpose  Get the local date and time (UTC) the SNTP client
*           last updated the clock.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*           This is the last update time in seconds adjusted
*           for the particular machine, e.g. seconds since
*           00:00:00 UTC, January 1, 1970 for a un*x style machine.
*
* @end
*********************************************************************/
extern L7_RC_t sntpLastUpdateTimeGet(L7_uint32 * lastUpdateTime);

/*********************************************************************
* @purpose  Get the local date and time (UTC) of the last SNTP
*           request (unicast mode) or receipt of an unsolicited message
*           (broadcast mode).
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*           This is the last update time in seconds adjusted
*           for the particular machine, e.g. seconds since
*           00:00:00 UTC, January 1, 1970 for a un*x style machine.
*
* @end
*********************************************************************/
extern L7_RC_t sntpLastAttemptTimeGet(L7_uint32 * lastAttemptTime);

/*********************************************************************
* @purpose  Get the status of the last SNTP request (in unicast mode)
*           or unsolicited message (in broadcast mode).
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpLastAttemptStatusGet(L7_SNTP_PACKET_STATUS_t * status);

/*********************************************************************
* @purpose  Get the IP address of the server for the last received valid
*           packet.
*
* @param    pointer to where result is stored (must be at least 64 bytes)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1 - This is a null terminated string.
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerIpAddressGet(L7_char8 * address);

/*********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerAddressTypeGet(L7_SNTP_ADDRESS_TYPE_t * type);

/*********************************************************************
* @purpose  Get the claimed stratum of the server for the last received
*           valid packet.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerStratumGet(L7_uint32 * stratum);

/*********************************************************************
* @purpose  Get the reference clock identifier of the server for the
*           last received valid packet.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerRefIdGet(L7_char8 * refClockId);

/*********************************************************************
* @purpose  Get the mode of the server for the
*           last received valid packet.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerModeGet( L7_SNTP_SERVER_MODE_t * serverMode);

/*********************************************************************
* @purpose  Get the maximum number of unicast server entries that can be
*           simultaneously configured on this client.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerMaxEntriesGet(L7_uint32 * maxServerEntries);

/*********************************************************************
* @purpose  Get the current number of unicast server entries that are
*           simultaneously configured on this client.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerCurrentEntriesGet(L7_uint32 * currentServerEntries);

/*********************************************************************
* @purpose  Add a server entry in the next available free slot.
*
* @param    address - server address string (null terminated)
* @param    address type - server address type.
* @param    ndx - pointer to memory where index of newly added row is stored.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (null address or invalid address type).
* @returns  L7_ALREADY_CONFIGURED  if duplicate address/address type.
* @returns  L7_TABLE_IS_FULL  if no free slots in table.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerAdd(L7_char8 * address,
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
extern L7_RC_t sntpServerTableIpAddressAdd(L7_uint32 ndx, L7_char8 *address,
                                           L7_SNTP_ADDRESS_TYPE_t type);
/*********************************************************************
* @purpose  Delete a matching server entry.
*
* @param    address - server address string (null terminated)
* @param    address type - server address type.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or invalid status).
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerDelete(L7_char8 * address,
                       L7_SNTP_ADDRESS_TYPE_t type);

/*********************************************************************
* @purpose  Set the row status of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    row status - the new status of the row
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or invalid row status).
*
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableRowStatusSet(L7_uint32 ndx,
                              L7_SNTP_SERVER_STATUS_t  status);

/*********************************************************************
* @purpose  Get the IP address of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range
*           from[1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*           (of type L7_SNTP_SERVER_STATUS_t)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null status pointer).
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableRowStatusGet(L7_uint32 ndx,
                    L7_SNTP_SERVER_STATUS_t * status);

/**********************************************************************
* @purpose  Get the row status of the "next" server table entry
*           as specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
* @param    status @b{(output)} pointer to where result is stored
* @param    rowNdx @b{(output)} pointer to where index of
*           retrieved row is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (null status pointer).
* @returns  L7_NOT_EXIST    if invalid index
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableRowStatusGetNext(L7_uint32 ndx,
              L7_SNTP_SERVER_STATUS_t * status, L7_uint32 *rowNdx);


/*********************************************************************
* @purpose  Set the IP address of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range
*           from[1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to new address
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer).
*
* @notes    FS Section 8.2.1 - This is a null terminated string.
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableIpAddressSet(L7_uint32 ndx,
                                          L7_char8 * address);

/*********************************************************************
* @purpose  Get the IP address of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range
*           from [1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored (must be at least 64 bytes)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer).
*
* @notes    FS Section 8.2.1 - This is a null terminated string.
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableIpAddressGet(L7_uint32 ndx,
                                          L7_char8 * address);

/*********************************************************************
* @purpose  Set the IP address type of the server table entry
*           as specified by index.
*
* @param    index of entry into table. Must range from
*           [1..L7_MAX_SERVER_ENTRIES]
* @param    type - new address type
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid index of invalid address type
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableAddressTypeSet(L7_uint32 ndx,
                                L7_SNTP_ADDRESS_TYPE_t type);

/*********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableAddressTypeGet(L7_uint32 ndx,
                                    L7_SNTP_ADDRESS_TYPE_t * type);

/*********************************************************************
* @purpose  Set the priority of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    priority
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null address pointer)
*                       or entry does not exist.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePrioritySet(L7_uint32 ndx,
                                        L7_ushort16 priority);

/*********************************************************************
* @purpose  Set the poll of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    poll    
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null address pointer)
*                       or entry does not exist.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePollSet(L7_uint32 ndx,
                                        L7_ushort16 poll);

/**********************************************************************
* @purpose  Gets the polling status of the server for the specified entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null argument pointer or invalid index
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePollGet(L7_uint32 ndx, L7_ushort16 * poll);

/*********************************************************************
* @purpose  Get the priority of the server table entry
*           specified by index.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePriorityGet(L7_uint32 ndx,
                                          L7_ushort16 * priority);

/*********************************************************************
* @purpose  Set the priority of the server table entry
*           specified by index to the default value
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null address pointer)
*                       or entry does not exist.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePriorityDefaultSet(L7_uint32 ndx);
extern L7_RC_t SntpServerIndexGet(L7_char8 *ipAddr, L7_uint32 *index);
/*********************************************************************
* @purpose  Set the NTP version of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    NTP version of the server
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null address pointer)
*                       or entry does not exist.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableVersionSet(L7_uint32 ndx,
                                          L7_ushort16 version);

/*********************************************************************
* @purpose  Get the version of the server table entry
*           specified by index.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableVersionGet(L7_uint32 ndx,
                                          L7_ushort16 * version);

/*********************************************************************
* @purpose  Set the NTP version of the server table entry
*           specified by index to the default value.
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null address pointer)
*                       or entry does not exist.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTableVersionDefaultSet(L7_uint32 ndx);


/*********************************************************************
* @purpose  Set the NTP port of the server table entry
*           specified by index.
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    NTP port used by the server
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null address pointer)
*                       or entry does not exist.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePortSet(L7_uint32 ndx, L7_ushort16 port);

/*********************************************************************
* @purpose  Get the port of the server table entry
*           specified by index.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePortGet(L7_uint32 ndx, L7_ushort16 * port);

/*********************************************************************
* @purpose  Set the NTP port of the server table entry
*           specified by index to the default value.
*
* @param    index of entry into table. Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or null address pointer)
*                       or entry does not exist.
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpServerTablePortDefaultSet(L7_uint32 ndx);

/*********************************************************************
* @purpose  Get the current number of valid unsolicited broadcast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpBroadcastCountGet(L7_uint32 * broadcastCount);

/*********************************************************************
* @purpose  Get the current number of valid unsolicited multicast SNTP
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpMulticastCountGet(L7_uint32 * multicastCount);

/*********************************************************************
* @purpose  Set the broadcast poll interval in seconds as a power of two
*
* @param    power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes    FS Section 8.1.1
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientBroadcastPollIntervalSet(L7_uint32 interval);

/*********************************************************************
* @purpose  Get the broadcast poll interval in seconds as a power of two
*
* @param    pointer to poll interval storage
*
*
* @returns  L7_SUCCESS  if storage updates
* @returns  L7_ERROR    if invalid pointer
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientBroadcastPollIntervalGet(L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the broadcast poll interval to the default value.
*
* @param    power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes    FS Section 8.1.1
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientBroadcastPollIntervalDefaultSet();

/*********************************************************************
* @purpose  Set the broadcast poll interval in seconds as a power of two
*
* @param    power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes    FS Section 8.1.1
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientMulticastPollIntervalSet(L7_uint32 interval);


/*********************************************************************
* @purpose  Get the broadcast poll interval in seconds as a power of two
*
* @param    pointer to poll interval storage
*
*
* @returns  L7_SUCCESS  if storage updates
* @returns  L7_ERROR    if invalid pointer
*
* @notes    FS Section 8.1.1
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientMulticastPollIntervalGet(L7_uint32 * interval);

/*********************************************************************
* @purpose  Set the broadcast poll interval to the default value
*
* @param    None.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes    FS Section 8.1.1
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientMulticastPollIntervalDefaultSet();

/**********************************************************************
* @purpose  Get the SNTP last update time for the specified server entry.
*
* @param    index of entry into table. Must range
*           from [1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.2
*
* @end
*********************************************************************/
L7_RC_t sntpServerStatsLastUpdateTimeGet(L7_uint32 ndx, L7_uint32 * time);

/**********************************************************************
* @purpose  Get the SNTP last attempt time for the specified server entry.
*
* @param    index of entry into table. Must range
*           from[1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.2
*
* @end
*********************************************************************/
L7_RC_t sntpServerStatsLastAttemptTimeGet(L7_uint32 ndx, L7_uint32 * time);

/**********************************************************************
* @purpose  Get the SNTP last update status for the specified server entry.
*
* @param    index of entry into table. Must range
*           from[1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 8.2.2
*
* @end
*********************************************************************/
L7_RC_t sntpServerStatsLastUpdateStatusGet(L7_uint32 ndx, L7_SNTP_PACKET_STATUS_t * status);

/**********************************************************************
* @purpose  Get the SNTP unicast server requests sent to the specified
*           server entry.
*
* @param    index of entry into table. Must range from
*           [1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 9.0.2
*
* @end
*********************************************************************/
L7_RC_t sntpServerStatsUnicastServerNumRequestsGet(L7_uint32 ndx,
                                            L7_uint32 * requests);

/**********************************************************************
* @purpose  Get the SNTP unicast server requests sent to the specified
*           server entry.
*
* @param    index of entry into table. Must range from
*           [1..L7_MAX_SERVER_ENTRIES]
* @param    pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes    FS Section 9.0.2
*
* @end
*********************************************************************/
L7_RC_t sntpServerStatsUnicastServerNumFailedRequestsGet(L7_uint32 ndx,
                                                L7_uint32 * requests);


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
L7_RC_t sntpClientAuthModeGet(L7_BOOL *enable);

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
L7_RC_t sntpClientAuthModeSet(L7_BOOL enable);

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
L7_RC_t sntpServerTableAuthKeySet(L7_uint32 index, 
                                  L7_uint32 keyIndex);

/*********************************************************************
* @purpose  Get the authentication key for an SNTP server table entry
*           
* @param    L7_uint32  index     @b{(input)} index of server table entry.
*                                           Must range from[1..L7_SNTP_MAX_SERVERS]
*
* @param    L7_uint32 *keyIndex  @b{(output)} index of authenticate table entry.
*                                             range 1-L7_SNTP_MAX_AUTH_KEYS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableAuthKeyGet(L7_uint32  index, 
                                  L7_uint32 *keyIndex);

/*********************************************************************
* @purpose  Add a new authentication table entry specified by key number.                       
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
L7_RC_t sntpAuthTableKeyAdd(L7_uint32  keyNumber,
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
L7_RC_t sntpAuthTableKeyDelete(L7_uint32  keyNumber);

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
L7_RC_t sntpAuthTableIndexGet(L7_uint32  keyNumber,
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
L7_RC_t sntpAuthTableRowStatusSet(L7_uint32  index,
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
* @returns  L7_NOT_EXIST if index out of range
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpAuthTableRowStatusGet(L7_uint32                index,
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
* @returns  L7_ERROR    if invalid argument (null status pointer or
*                       null rowIndex pointer).
* @returns  L7_NOT_EXIST    if index out of range [0..L7_MAX_SNTP_AUTHENTICATE_ENTRIES-1]
*
* @notes   use index of 0 to get first valid index.
*
* @end
*********************************************************************/
L7_RC_t sntpAuthTableRowStatusGetNext(L7_uint32                index,
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
L7_RC_t sntpAuthTableKeyNumberSet(L7_uint32 index,
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
L7_RC_t sntpAuthTableKeyNumberGet(L7_uint32  index,
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
L7_RC_t sntpAuthTableKeyValueSet(L7_uint32 index,
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
L7_RC_t sntpAuthTableKeyValueGet(L7_uint32  index,
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
L7_RC_t sntpAuthTableKeyTrustedSet(L7_uint32 index,
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
L7_RC_t sntpAuthTableKeyTrustedGet(L7_uint32  index,
                                   L7_BOOL   *trusted);

#endif
