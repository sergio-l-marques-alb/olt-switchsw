/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename sntp_client.h
*
* @purpose Declares SNTP Internal Manifest Constants
*           and internal data structures.
*
* @component Simple Network Time Protocol Cfg (RFC 2030)
*
* @comments none
*
* @create 1/15/2004
*
* @author McLendon
* @end
*
**********************************************************************/

#ifndef __L7_SNTP_CLIENT_H__
#define __L7_SNTP_CLIENT_H__

/*************************************************************

*************************************************************/


#include "l7_common.h"
#include "flex.h"

/** The sntpServerCfgData contains information regarding the
* configured servers. An unconfigured server will have a status of
* L7_SNTP_SERVER_STATUS_INVALID (has never been configured) or
* L7_SNTP_SERVER_STATUS_DESTROY (was deleted by management).
*/


struct sntpServerCfgData_s
{
  /** FS 8.2.1 - The server status */
  L7_SNTP_SERVER_STATUS_t status;
  /** FS 8.2.1 - The time server address type */
  L7_SNTP_ADDRESS_TYPE_t  addressType;
  /** FS 8.2.1 - The time server address */
  L7_char8              address[L7_SNTP_MAX_ADDRESS_LEN];
  /** FS 8.2.1 - The time server port */
  L7_ushort16           port;
  /** FS 8.2.1 - The time server SNTP version */
  L7_ushort16           version;
  /** FS 8.2.1 - The time server priority */
  L7_ushort16           priority;
  /* authenticate key index range is 1-L7_SNTP_MAX_AUTH_KEYS */
  L7_uint32             keyIndex;
  /* To keep track of whthr poll req sent to this server, reset to 0 when priority changes */
  L7_BOOL               isRequestSent;
  /* enables/disables unicast polling */
  L7_uint32             poll;
};

#define SNTP_AUTH_KEY_NOT_CONFIGURED  0

typedef struct sntpKeyCfgData_s
{
  /* the row status, treated the same as server status values */
  L7_SNTP_SERVER_STATUS_t status;
  /* SNTP key id */
  L7_uint32               id;  /* keyNumber, always referred to as key id in spec */
  /* ASCII MD5 key string */
  L7_char8                value[L7_SNTP_KEY_LEN_MAX+1];
  /* whether or not this key is globally "trusted" */
  L7_BOOL                 trusted;
} sntpKeyCfgData_t;

/* The sntpCfgData_s structure contains SNTP persistent configuration data.
* The reader is referred to the SNTP Functional Specification
* for explanation of each of this elements of this data structure in detail.
*/

struct sntpCfgData_s
{
  /** An SNTP client may operate in one of several modes. */
  L7_SNTP_CLIENT_MODE_t   sntpClientMode;
  /** The client's local port */
  L7_ushort16             sntpClientPort;
  /** The unicast poll interval (in secs as a power of two) */
  L7_ushort16             sntpUnicastPollInterval;
  /** The client poll timeout value in seconds */
  L7_ushort16             sntpUnicastPollTimeout;
  /** The client poll retry count */
  L7_ushort16             sntpUnicastPollRetry;
  /** The broadcast poll interval (in secs as a power of two) */
  L7_ushort16             sntpBroadcastPollInterval;
  /** The multicast poll interval (in secs as a power of two) */
  L7_ushort16             sntpMulticastPollInterval;
  /** The unicast server count - maintained dynamically. */
  L7_ushort16             sntpUnicastServerCount;

  /** Array of SNTP servers. */
  struct sntpServerCfgData_s server[L7_SNTP_MAX_SERVERS];

  /* The client's authenticate mode */
  L7_BOOL                 sntpAuthMode;
  /* The client's authenticate keys */
  struct sntpKeyCfgData_s sntpKeys[L7_SNTP_MAX_AUTH_KEYS]; 

};

/** The server statistics table. We reuse the indices and rowStatus
 *   of the sntpCfg->server table.
 */

typedef struct sntpServerStats_s
{
  /** The local data and time (UTC) that the response
  *   from this server was used to update the system clock.
  */
  L7_uint32         lastUpdateTime;
  /** The local data and time (UTC) that this server was last queried. */
  L7_uint32         lastAttemptTime;
  /** The status of the last SNTP request to this server. */
  L7_uint32         lastAttemptStatus;
  /** The number of SNTP requests made to this server since last
  * agent reboot. Includes failed requests.
  */
  L7_uint32         unicastServerNumRequests;
  /** The number of failed (from any cause) SNTP requests
  * made to this server since last reboot.
  */
  L7_uint32         unicastServerNumFailedRequests;
} sntpServerStats_t;

/*********************************************************************
* @purpose  Attempt to obtain the SNTP client mutex
*
* @end
*********************************************************************/

extern L7_RC_t sntpTaskLock();
/*********************************************************************
* @purpose  Release the client task mutex
*
* @end
*********************************************************************/

extern L7_RC_t sntpTaskUnlock();

/*********************************************************************
* @purpose System Initialization for SNTP client
*
* @end
*
*********************************************************************/

extern L7_RC_t sntpClientInit(struct sntpCfgData_s * cfg);


/*********************************************************************
* @purpose System Deinitialization for SNTP client
*
* @end
*
*********************************************************************/

extern void sntpClientTerminate();

/*********************************************************************
* @purpose Zero out the client configuration data.
*
* @end
*
*********************************************************************/

extern void sntpCfgDataClear();

/*********************************************************************
* @purpose Set the config header to it's default values.
*
* @end
*
*********************************************************************/

extern void sntpCfgHeaderDefaultsSet(L7_uint32 ver);

/*********************************************************************
*
* @purpose Zero out the statistics
*
* @end
*
*********************************************************************/
extern void sntpStatisticsClear();

/**********************************************************************
* @purpose  Update the local clock from broadcast packet sent by the top of stack
*
* @end
*********************************************************************/

#if defined(L7_STACKING_PACKAGE)
extern void sntpHandleStackBroadcast(L7_uchar8 * msg, L7_uint32 len);
#endif

/**********************************************************************
* @purpose  verify auth mode or client mode configuration change.
*
* @param    L7_SNTP_CLIENT_MODE_t clientMode @b{(input)}  client mode config
* @param    L7_BOOL               authEnable @b((input))  auth enabled/disabled
*
* @returns  L7_SUCCESS mode update is valid
* @returns  L7_FAILURE mode update is invalid
*
* @notes    This is a utility routine to verify either a client or authenticate
*           mode change based on the current key configuration, do not
*           allow auth to be enabled or mode to be changed if the right
*           key configuration does not exist.
*
* @end
*********************************************************************/
L7_RC_t sntpClientAuthModeVerify(L7_SNTP_CLIENT_MODE_t clientMode,
                                 L7_BOOL               authEnable);

/**********************************************************************
* @purpose  select client mode
*
* @returns  L7_SUCCESS, mode set
*           L7_FAILURE, mode invalid based on auth configuration
*
* @notes    may cause the loop in sntpTask to fall thru if
*           it's blocked on a socket.
*
* @end
*********************************************************************/
L7_RC_t sntpClientModeSelect(L7_SNTP_CLIENT_MODE_t mode);

/**********************************************************************
* @purpose  select disabled mode
*
* @end
*********************************************************************/

extern void sntpDisabledModeSelect();


/**********************************************************************
* @purpose  Get the IP address of the server for the last received valid
*           packet.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientServerIpAddressGet(L7_char8 * address);

/**********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientServerAddressTypeGet(L7_SNTP_ADDRESS_TYPE_t * type);

/**********************************************************************
* @purpose  Get the claimed stratum of the server for the last received
*           valid packet.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientServerStratumGet(L7_uint32 * stratum);

/**********************************************************************
* @purpose  Get the reference clock identifier of the server for the
*           last received valid packet.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientServerRefIdGet(L7_char8 * refClockId);

/**********************************************************************
* @purpose  Get the mode of the server for the
*           last received valid packet.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientServerModeGet( L7_SNTP_SERVER_MODE_t * serverMode);

/**********************************************************************
* @purpose  This routine closes the local socket.
*
* @end
*********************************************************************/

extern L7_RC_t sntpLocalSocketClose();


/**********************************************************************
* @purpose  Get the current number of unsolicited broadcast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientBroadcastCountGet(L7_uint32 * broadcastCount);

/**********************************************************************
* @purpose  Get the current number of unsolicited multicast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
* @end
*********************************************************************/
extern L7_RC_t sntpClientMulticastCountGet(L7_uint32 * multicastCount);

/**********************************************************************
* @purpose  Get the index of the active server.
*
* @end
*********************************************************************/
extern L7_uint32 sntpActiveServerGet();

/**********************************************************************
* @purpose  Force reselection of the currently active server (if any).
*
* @end
*********************************************************************/

extern void sntpActiveServerReselect();

/**********************************************************************
* @purpose  Get last attempt status for unicast mode
*
* @end
*********************************************************************/

extern L7_RC_t sntpClientLastAttemptStatusGet(L7_SNTP_PACKET_STATUS_t * status);

/**********************************************************************
* @purpose  Get last attempted poll time for unicast mode
*
* @end
*********************************************************************/

extern L7_RC_t sntpClientLastAttemptTimeGet(L7_uint32 * attemptTime);

/**********************************************************************
* @purpose  Get last update time for any mode
*
* @end
*********************************************************************/

extern L7_RC_t sntpClientLastUpdateTimeGet(L7_uint32 * updateTime);

#endif
