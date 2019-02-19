/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename sntp_api.c
*
* @purpose SNTP Client public interface
*
* @component SNTP
*
* @comments This module implements the management interfaces which
* require access to component local variables.
* Declarations for the below are located in sntp_api.h.
*
* This file is subdivied into sections as follows:
*
*   Initialization
*   Manifest contant access
*   Client Access and Control
*   Server Table Access and Control
*
* @create 1/12/2004
*
* @author mclendon
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "log.h"
#include "default_cnfgr.h"
#include "sntp_api.h"
#include "sntp_client.h"
#include "sntp_cfg.h"



static struct sntpCfgData_s * sntpCfgData = L7_NULL;
static struct sntpServerStats_s * serverStats = L7_NULL;

/*
*
* SECTION: Initialization
*
*
*/

/*********************************************************************
* @purpose  Initialize the API private data pointers.
*
* @param  cfg @b{(input)} pointer to configuration data
* @param  src @b{(input)} pointer to operational data
* @notes  May whine to log file.
*
* @end
*********************************************************************/

void sntpApiInit(struct sntpCfgData_s * cfg, struct sntpServerStats_s * srv)
{

  if (cfg == L7_NULL)
    LOG_MSG("sntpApiInit: cannot initialize API with cfg data\n");
  if (srv == L7_NULL)
    LOG_MSG("sntpApiInit: cannot initialize API with server stats data\n");
  sntpCfgData = cfg;
  serverStats = srv;
}

/*
*
* SECTION: Manifest Constant Access
*
*
*/

/*********************************************************************
* @purpose  Get the highest SNTP version the client supports
*
* @param    version @b{(output)} @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientVersionGet(L7_uint32 * version)
{
  if (version == L7_NULL)
    return L7_ERROR;
  *version =  L7_SNTP_DEFAULT_VERSION;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the supported mode
*
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
L7_RC_t sntpClientSupportedModeGet(L7_SNTP_SUPPORTED_MODE_t * mode)
{
  if (mode == L7_NULL)
    return L7_ERROR;

  *mode = 0;

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) == L7_TRUE)
  {
    *mode |= L7_SNTP_SUPPORTED_UNICAST;
  }

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) == L7_TRUE)
  {
    *mode |= L7_SNTP_SUPPORTED_BROADCAST;
  }

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_MULTICAST_FEATURE_ID) == L7_TRUE)
  {
    *mode |= L7_SNTP_SUPPORTED_MULTICAST;
  }
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) == L7_TRUE)
  {
   if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) == L7_TRUE)
   {
     *mode = L7_SNTP_SUPPORTED_UNICAST_AND_BROADCAST;
   }
  }


  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the maximum number of unicast server entries that can be
*           simultaneously configured on this client.
*
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
L7_RC_t sntpServerMaxEntriesGet(L7_uint32 * maxServerEntries)
{
  if (maxServerEntries == L7_NULL)
    return L7_ERROR;
  *maxServerEntries = L7_SNTP_MAX_SERVERS;
  return L7_SUCCESS;
}


/*
*
* SECTION: Client Access and Control
*
*
*/


/**********************************************************************
* @purpose  Set the mode of the SNTP client.
*
* @param    mode @b{(input)} L7_SNTP_CLIENT_MODE_t
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if mode is not valid
*                       (i.e. not supported or not a member of
*                       L7_SNTP_CLIENT_MODE_t)
*
* @notes    We really don't care what the client is doing
*           at this point. If it is waiting on a select, that's ok,
*           the error checking will handle it. If it is waiting
*           on a packet, closing the socket will drop it
*           through the loop. Note that with this strategy,
*           mode changes are NOT guaranteed to take effect immediately.
*
* @end
*********************************************************************/
L7_RC_t sntpClientModeSet(L7_SNTP_CLIENT_MODE_t mode)
{
  switch (mode)
  {
  case L7_SNTP_CLIENT_BROADCAST:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_BROADCAST_FEATURE_ID) == L7_TRUE)
    {
      /* need to verify mode with auth configuration */
      return sntpClientModeSelect(L7_SNTP_CLIENT_BROADCAST);
    }
    break;

  case L7_SNTP_CLIENT_UNICAST:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_UNICAST_FEATURE_ID) == L7_TRUE)
    {
      /* need to verify mode with auth configuration */
      return sntpClientModeSelect(L7_SNTP_CLIENT_UNICAST);
    }
    break;

  case L7_SNTP_CLIENT_MULTICAST:
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_SNTP_COMPONENT_ID, L7_SNTP_MULTICAST_FEATURE_ID) == L7_TRUE)
    {
      /* need to verify mode with auth configuration */
      return sntpClientModeSelect(L7_SNTP_CLIENT_MULTICAST);
    }
    break;

  case L7_SNTP_CLIENT_DISABLED:
    sntpDisabledModeSelect();
    return L7_SUCCESS;
    break;

  default:
    return L7_ERROR;
  }

  return L7_ERROR;
}

/**********************************************************************
* @purpose  Get the mode of the SNTP client.
*
* @param    mode @b{(output)} pointer to L7_SNTP_CLIENT_MODE_t variable
*           is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t sntpClientModeGet(L7_SNTP_CLIENT_MODE_t * mode)
{
  if (mode == L7_NULL)
    return L7_ERROR;
  *mode = sntpCfgData->sntpClientMode;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the local port number of the SNTP client.
*
* @param    port @b{(output)} port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port number is not valid (e.g. 0)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientPortSet(L7_ushort16 port)
{
  if (sntpCfgData->sntpClientPort != port)
  {
    sntpTaskLock();
    sntpCfgData->sntpClientPort = port;
    sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Set the local port number of the SNTP client to the default.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port number is not valid (e.g. 0)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientPortDefaultSet()
{
  return sntpClientPortSet(L7_SNTP_DEFAULT_CLIENT_PORT);
}

/**********************************************************************
* @purpose  Get the local port number of the SNTP client.
*
* @param    port @b{(output)} pointer to storage for port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if port is null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientPortGet(unsigned short * port)
{
  if (port == L7_NULL)
    return L7_ERROR;
  *port = sntpCfgData->sntpClientPort;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the unicast poll interval in seconds as a power of two
*
* @param    interval @b{(input)} power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes    Example: 6 implies a poll interval of 2^6 or 64 seconds.
*           The interval change will take effect on the NEXT poll,
*           not the current poll.
*
* @end
*********************************************************************/
L7_RC_t sntpClientUnicastPollIntervalSet(L7_uint32 interval)
{
  if ((interval < SNTP_MIN_POLL_INTERVAL) || (interval > SNTP_MAX_POLL_INTERVAL))
    return L7_ERROR;
  if (sntpCfgData->sntpUnicastPollInterval != interval)
  {
    sntpCfgData->sntpUnicastPollInterval = interval;
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the unicast poll interval in seconds as a power of two
*
* @param    interval @b{(output)} pointer to interval storage
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientUnicastPollIntervalGet(L7_uint32 * interval)
{
  if (interval == L7_NULL)
    return L7_ERROR;
  *interval = sntpCfgData->sntpUnicastPollInterval;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Set the unicast poll interval to the default value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*           The interval change will take effect on the NEXT poll,
*           not the current poll.
*
* @end
*********************************************************************/
L7_RC_t sntpClientUnicastPollIntervalDefaultSet()
{
  return sntpClientUnicastPollIntervalSet(L7_SNTP_DEFAULT_POLL_INTERVAL);
}

/**********************************************************************
* @purpose  Set the unicast poll timeout in seconds
*
* @param    timeout @b{(input)} The timeout value
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if value is out of range
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientUnicastPollTimeoutSet(L7_uint32 timeout)
{
  if ((timeout < SNTP_MIN_POLL_TIMEOUT) || (timeout > SNTP_MAX_POLL_TIMEOUT))
    return L7_ERROR;
  if (sntpCfgData->sntpUnicastPollTimeout != timeout)
  {
    sntpTaskLock();
    sntpCfgData->sntpUnicastPollTimeout = timeout;
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the unicast poll timeout
*
* @param    timeout @b{(output)} pointer to where result is to be stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientUnicastPollTimeoutGet(L7_uint32 * timeout)
{
  if (timeout == L7_NULL)
    return L7_ERROR;
  *timeout = sntpCfgData->sntpUnicastPollTimeout;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the unicast poll timeout in seconds
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if value is out of range
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientUnicastPollTimeoutDefaultSet()
{
  return sntpClientUnicastPollTimeoutSet(L7_SNTP_DEFAULT_POLL_TIMEOUT);
}

/**********************************************************************
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
L7_RC_t sntpClientUnicastPollRetrySet(L7_uint32 retry)
{
  if ((retry < SNTP_MIN_POLL_RETRY) || (retry > SNTP_MAX_POLL_RETRY))
    return L7_ERROR;
  if (sntpCfgData->sntpUnicastPollRetry != retry)
  {
    sntpCfgData->sntpUnicastPollRetry = retry;
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the unicast poll retry count
*
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
L7_RC_t sntpClientUnicastPollRetryGet(L7_uint32 * retry)
{
  if (retry == L7_NULL)
    return L7_ERROR;
  *retry = sntpCfgData->sntpUnicastPollRetry;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Set the unicast poll retry count to the default value.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if out of range retry count
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientUnicastPollRetryDefaultSet()
{
  return sntpClientUnicastPollRetrySet(L7_SNTP_DEFAULT_POLL_RETRY);
}


/**********************************************************************
* @purpose  Get the status of the last SNTP request (in unicast mode)
*           or unsolicited message (in broadcast mode).
*
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
L7_RC_t sntpLastAttemptStatusGet(L7_SNTP_PACKET_STATUS_t * status)
{
  return sntpClientLastAttemptStatusGet(status);
}

/**********************************************************************
* @purpose  Get the last attempt time of the client (in unicast mode)
*
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
L7_RC_t sntpLastAttemptTimeGet(L7_uint32 * attemptTime)
{
  return sntpClientLastAttemptTimeGet(attemptTime);
}

/**********************************************************************
* @purpose  Get the last update time of the client (in any mode)
*
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
L7_RC_t sntpLastUpdateTimeGet(L7_uint32 * updateTime)
{
  return sntpClientLastUpdateTimeGet(updateTime);
}

/**********************************************************************
* @purpose  Get the IP address of the server for the last received valid
*           packet.
*
* @param    address @b{(output)} pointer to where result is stored
*           (must be at least FD_CNFGR_SNTP_MAX_ADDRESS_LEN bytes)
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes     - This is a null terminated string.
*
* @end
*********************************************************************/
L7_RC_t sntpServerIpAddressGet(L7_char8 * address)
{
  return sntpClientServerIpAddressGet(address);
}

/**********************************************************************
* @purpose  Get the IP address type of the address of the
*           server for the last received valid packet.
*
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
L7_RC_t sntpServerAddressTypeGet(L7_SNTP_ADDRESS_TYPE_t * type)
{
  return sntpClientServerAddressTypeGet(type);
}

/**********************************************************************
* @purpose  Get the claimed stratum of the server for the last received
*           valid packet.
*
* @param    stratum @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerStratumGet(L7_uint32 * stratum)
{
  return sntpClientServerStratumGet(stratum);
}

/**********************************************************************
* @purpose  Get the reference clock identifier of the server for the
*           last received valid packet.
*
* @param    refClockId @b{(output)} pointer to where result is stored
*            (storage MUST be at least 26 bytes).
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes     - Writes a null terminated ASCII string
*           not exceeding 26 bytes including the null terminator.
*
* @end
*********************************************************************/


L7_RC_t sntpServerRefIdGet(L7_char8 * refClockId)
{
  return sntpClientServerRefIdGet(refClockId);
}

/**********************************************************************
* @purpose  Get the mode of the server for the
*           last received valid packet.
*
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
L7_RC_t sntpServerModeGet( L7_SNTP_SERVER_MODE_t * serverMode)
{
  return sntpClientServerModeGet(serverMode);
}

/**********************************************************************
* @purpose  Get the current number of unicast server entries that are
*           simultaneously configured on this client.
*
* @param    currentServerEntries @b{(output)} pointer to where
*           result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerCurrentEntriesGet(L7_uint32 * currentServerEntries)
{
  if (currentServerEntries == L7_NULL)
    return L7_ERROR;
  *currentServerEntries = sntpCfgData->sntpUnicastServerCount;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the current number of unsolicited broadcast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
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
L7_RC_t sntpBroadcastCountGet(L7_uint32 * broadcastCount)
{
  return sntpClientBroadcastCountGet(broadcastCount);
}
/**********************************************************************
* @purpose  Set the broadcast poll interval in seconds as a power of two
*
* @param    interval @b{(input)} power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*           The interval change will take effect on the NEXT poll,
*           not the current poll.
*
* @end
*********************************************************************/
L7_RC_t sntpClientBroadcastPollIntervalSet(L7_uint32 interval)
{
  if ((interval < SNTP_MIN_POLL_INTERVAL) || (interval > SNTP_MAX_POLL_INTERVAL))
    return L7_ERROR;
  if (sntpCfgData->sntpBroadcastPollInterval != interval)
  {
    sntpTaskLock();
    sntpCfgData->sntpBroadcastPollInterval = interval;
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_BROADCAST)
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the broadcast poll interval in seconds as a power of two
*
* @param    interval @b{(output)} pointer to interval storage
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientBroadcastPollIntervalGet(L7_uint32 * interval)
{
  if (interval == L7_NULL)
    return L7_ERROR;
  *interval = sntpCfgData->sntpBroadcastPollInterval;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Set the broadcast poll interval to the default value
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*           The interval change will take effect on the NEXT poll,
*           not the current poll.
*
* @end
*********************************************************************/
L7_RC_t sntpClientBroadcastPollIntervalDefaultSet()
{
  return sntpClientBroadcastPollIntervalSet(L7_SNTP_DEFAULT_POLL_INTERVAL);
}

/**********************************************************************
* @purpose  Get the current number of unsolicited multicast SNTP messages
*           messages that have been received and processed by the SNTP
*           client since last reboot.
*
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
L7_RC_t sntpMulticastCountGet(L7_uint32 * multicastCount)
{
  return sntpClientMulticastCountGet(multicastCount);
}

/**********************************************************************
* @purpose  Set the multicast poll interval in seconds as a power of two
*
* @param    interval @b{(input)} power of two of poll interval
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*           The interval change will take effect on the NEXT poll,
*           not the current poll.
*
* @end
*********************************************************************/
L7_RC_t sntpClientMulticastPollIntervalSet(L7_uint32 interval)
{
  if ((interval < SNTP_MIN_POLL_INTERVAL) ||
      (interval > SNTP_MAX_POLL_INTERVAL))
    return L7_ERROR;
  if (sntpCfgData->sntpMulticastPollInterval != interval)
  {
    sntpTaskLock();
    sntpCfgData->sntpMulticastPollInterval = interval;
    if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_MULTICAST)
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the multicast poll interval in seconds as a power of two
*
* @param    interval @b{output)} pointer to interval storage
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpClientMulticastPollIntervalGet(L7_uint32 * interval)
{
  if (interval == L7_NULL)
    return L7_ERROR;
  *interval = sntpCfgData->sntpMulticastPollInterval;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Set the multicast poll interval to the default value
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid poll interval
*
* @notes
*           Example: 6 implies a poll interval of 2^6 or 64 seconds.
*           The interval change will take effect on the NEXT poll,
*           not the current poll.
*
* @end
*********************************************************************/
L7_RC_t sntpClientMulticastPollIntervalDefaultSet()
{
  return sntpClientMulticastPollIntervalSet(L7_SNTP_DEFAULT_POLL_INTERVAL);
}

/**
*
* SECTION: Server Table Access and Control
*
*/

/**********************************************************************
* @purpose  Update the status of the server entry
*
* @param    ndx @b{(input)} the index of the server entry to update.
*
*
* @notes    This is a utility routine that updates the status of a server table
*           entry based on the settings of the address and address type.
*           Entries are only transitioned from active to not in service
*           and vice-versa.
*
* @end
*********************************************************************/
void sntpServerStatusUpdate(register L7_uint32 ndx)
{
  if ((sntpCfgData->server[ndx].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
      ((sntpCfgData->server[ndx].address[0] == '\0') ||
       (sntpCfgData->server[ndx].addressType == L7_SNTP_ADDRESS_UNKNOWN)))
  {
     sntpCfgData->server[ndx].status = L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
  }
  else if ((sntpCfgData->server[ndx].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE) &&
           (sntpCfgData->server[ndx].address[0] != '\0') &&
           (sntpCfgData->server[ndx].addressType != L7_SNTP_ADDRESS_UNKNOWN))
  {
     sntpCfgData->server[ndx].status = L7_SNTP_SERVER_STATUS_ACTIVE;
  }
}

/**********************************************************************
* @purpose  Add a new SNTP server. The IP address and address type
*           must be specified. Everything else is defaulted.
*
* @param    address @b{(input}) address of the new server
* @param    type @b{(input}) address type of the new server address
*           (all types supported)
* @param    ndx @b{(output}) pointer to memory where index
*           of newly added row is stored.
*
* @returns  L7_SUCCESS            if success
* @returns  L7_ERROR              if null pointer
# @returns  L7_ALREADY CONFIGURED if duplicate address/address type
# @returns  L7_TABLE_IS_FULL      if no more room in table
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerAdd(char * address, L7_SNTP_ADDRESS_TYPE_t type,
                            L7_uint32 * ndx)
{
  register L7_int32 i;
  int  firstEmptySlot = NO_SERVER_SELECTED;

  if ((address == L7_NULL) || (address[0] == '\0') || (ndx == L7_NULL))
    return L7_ERROR;

  if ((type != L7_SNTP_ADDRESS_IPV4) &&
      (type != L7_SNTP_ADDRESS_DNS))
    return L7_ERROR;

  /** Look for existing entry */
  sntpTaskLock();
  for (i = 0; i < L7_SNTP_MAX_SERVERS; i++)
  {
    if (((sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_DESTROY) ||
         (sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_INVALID)) &&
        firstEmptySlot == NO_SERVER_SELECTED)
    {
      firstEmptySlot = i;
    }
    else if ((sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) ||
             (sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
    {
      /** Duplicate entry? */
      if (strncmp(address, sntpCfgData->server[i].address, L7_SNTP_MAX_ADDRESS_LEN) == 0)
      {
        /* return the index to this entry */
        *ndx = i+1;
        sntpTaskUnlock();
        return L7_ALREADY_CONFIGURED;
      }
    }
  }
  /** Just writing the info directly is faster than allocating a message,
     copying all of this info into the message, queueing the message and
     then dealing with it on the other end. */
  if (firstEmptySlot >= 0)
  {
    /** We have an empty slot */
    osapiStrncpySafe(sntpCfgData->server[firstEmptySlot].address, address, L7_SNTP_MAX_ADDRESS_LEN);
    sntpCfgData->server[firstEmptySlot].addressType = type;
    sntpCfgData->server[firstEmptySlot].port = L7_SNTP_DEFAULT_SERVER_PORT;
    sntpCfgData->server[firstEmptySlot].version = L7_SNTP_DEFAULT_VERSION;
    sntpCfgData->server[firstEmptySlot].priority = L7_SNTP_DEFAULT_SERVER_PRIORITY;
    sntpCfgData->server[firstEmptySlot].poll = L7_SNTP_DEFAULT_SERVER_POLL;
    serverStats[firstEmptySlot].lastUpdateTime = 0;
    serverStats[firstEmptySlot].lastAttemptTime = 0;
    serverStats[firstEmptySlot].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[firstEmptySlot].unicastServerNumRequests = 0;
    serverStats[firstEmptySlot].unicastServerNumFailedRequests = 0;
    /** Now make it available to the SNTP client */
    sntpCfgData->server[firstEmptySlot].status = L7_SNTP_SERVER_STATUS_ACTIVE;
    *ndx = firstEmptySlot + 1;
    if (sntpCfgData->sntpUnicastServerCount < L7_SNTP_MAX_SERVERS)
      /** This will allow the main loop to fall out of the initial condition */
      sntpCfgData->sntpUnicastServerCount++;
  }
  else
  {
    sntpTaskUnlock();
    return L7_TABLE_IS_FULL;
  }
  if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
  {
    /** Start the search for a new server at the beginning */
    sntpActiveServerReselect();
    /** Cause the client to begin looking now. */
    sntpLocalSocketClose();
  }
  sntpTaskUnlock();
  sntpCfgMarkDataChanged();
  return L7_SUCCESS;
}
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
L7_RC_t sntpServerTableIpAddressAdd(L7_uint32 ndx, L7_char8 *address,
                                    L7_SNTP_ADDRESS_TYPE_t type)
{
  /* Invalid input */
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (address == L7_NULL))
  {
    return L7_ERROR;
  }
 
  sntpTaskLock();
  /* Check if server is not yet configured */
  if ((sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    osapiStrncpySafe(sntpCfgData->server[ndx-1].address, address, L7_SNTP_MAX_ADDRESS_LEN);
    sntpCfgData->server[ndx-1].addressType = type;
    sntpCfgData->server[ndx-1].port = L7_SNTP_DEFAULT_SERVER_PORT;
    sntpCfgData->server[ndx-1].version = L7_SNTP_DEFAULT_VERSION;
    sntpCfgData->server[ndx-1].priority = L7_SNTP_DEFAULT_SERVER_PRIORITY;
    sntpCfgData->server[ndx-1].poll = L7_SNTP_DEFAULT_SERVER_POLL;
    serverStats[ndx-1].lastUpdateTime = 0;
    serverStats[ndx-1].lastAttemptTime = 0;
    serverStats[ndx-1].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[ndx-1].unicastServerNumRequests = 0;
    serverStats[ndx-1].unicastServerNumFailedRequests = 0;
    /** Now make it available to the SNTP client */
    sntpCfgData->server[ndx-1].status = L7_SNTP_SERVER_STATUS_ACTIVE;
    if (sntpCfgData->sntpUnicastServerCount < L7_SNTP_MAX_SERVERS)
      /** This will allow the main loop to fall out of the initial condition */
      sntpCfgData->sntpUnicastServerCount++;

    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
    return L7_SUCCESS;
  }
 
  /* Server already configured. Change the IP Address */
  if (strncmp(sntpCfgData->server[ndx - 1].address, address, L7_SNTP_MAX_ADDRESS_LEN) != 0)
  {
    osapiStrncpySafe(sntpCfgData->server[ndx-1].address, address, L7_SNTP_MAX_ADDRESS_LEN);
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    serverStats[ndx-1].lastUpdateTime = 0;
    serverStats[ndx-1].lastAttemptTime = 0;
    serverStats[ndx-1].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[ndx-1].unicastServerNumRequests = 0;
    serverStats[ndx-1].unicastServerNumFailedRequests = 0;
    sntpServerStatusUpdate(ndx-1);
    sntpCfgMarkDataChanged();
  }
  sntpTaskUnlock();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Delete a server.
*
* @param    IP Address @b{(input)} the address of the SNTP server
*           (character string)
* @param    IP Address Type @b{(input)} the address type of the
*           server IP address
*
* @returns  L7_SUCCESS  if server added. Row status will reflect
*           'active' immediately.
*
*
* @returns  L7_ERROR    if NULL address or invalid address type or
*                       max servers already configured.
*
* @returns  L7_NOT_EXIST    if server does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerDelete(L7_char8 * address, L7_SNTP_ADDRESS_TYPE_t type)
{
  register L7_int32 i;
  L7_char8 address_dummy[L7_SNTP_MAX_ADDRESS_LEN];
  if (address == L7_NULL)
    return L7_ERROR;
  if (sntpCfgData->sntpUnicastServerCount == 0)
    return L7_NOT_EXIST;

  memset((void*)address_dummy, L7_NULL, sizeof(address_dummy));
  sntpTaskLock();
  for (i = 0; i < L7_SNTP_MAX_SERVERS; i++)
  {
    if ((sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) ||
        (sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
    {
      if ((memcmp((void *)sntpCfgData->server[i].address, (void *)address_dummy, L7_SNTP_MAX_ADDRESS_LEN) != 0) &&
          (sntpCfgData->server[i].addressType == type) &&
          (strncmp(address, sntpCfgData->server[i].address, L7_SNTP_MAX_ADDRESS_LEN) == 0))
      {
        *sntpCfgData->server[i].address = '\0';
        sntpCfgData->server[i].status = L7_SNTP_SERVER_STATUS_DESTROY;

        serverStats[i].lastUpdateTime = 0;
        serverStats[i].lastAttemptTime = 0;
        serverStats[i].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
        serverStats[i].unicastServerNumRequests = 0;
        serverStats[i].unicastServerNumFailedRequests = 0;

        if (sntpCfgData->sntpUnicastServerCount)
          sntpCfgData->sntpUnicastServerCount--;
        /** Did we just kill the current server? */
        if (sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST)
        {
          if ((sntpActiveServerGet() == i) || (sntpCfgData->sntpUnicastServerCount == 0))
          {
            sntpActiveServerReselect();
            sntpLocalSocketClose();
          }
        }
        sntpTaskUnlock();
        sntpCfgMarkDataChanged();
        return L7_SUCCESS;
      }
    }
  }
  sntpTaskUnlock();
  return L7_NOT_EXIST;

}
/**********************************************************************
* @purpose  Set the IP address of the server table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    address @b{(output)} pointer to where result
*           is stored (must be at least 64 bytes)
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
L7_RC_t sntpServerTableIpAddressSet(L7_uint32 ndx, L7_char8 * address)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (address == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  if (strncmp(sntpCfgData->server[ndx - 1].address, address, L7_SNTP_MAX_ADDRESS_LEN) != 0)
  {
    sntpTaskLock();
    osapiStrncpySafe(sntpCfgData->server[ndx-1].address, address, L7_SNTP_MAX_ADDRESS_LEN);
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    serverStats[ndx-1].lastUpdateTime = 0;
    serverStats[ndx-1].lastAttemptTime = 0;
    serverStats[ndx-1].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[ndx-1].unicastServerNumRequests = 0;
    serverStats[ndx-1].unicastServerNumFailedRequests = 0;
    sntpServerStatusUpdate(ndx-1);
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the IP address of the server table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    address @b{(output)} pointer to where result is stored.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range
*                       or null address pointer).
*
* @notes     - This is a null terminated string.
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableIpAddressGet(L7_uint32 ndx, L7_char8 * address)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (address == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  strncpy(address, sntpCfgData->server[ndx-1].address, L7_SNTP_MAX_ADDRESS_LEN);
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the row status of the server table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    status @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid argument (ndx out of range or
*                       invalid status).
*
* @notes    Can only set row status to destroy, create and go,
*           or not in service. Only client can set status to active.
*           Preferred method of creating a server entry is to use
*           sntpServerAdd.
*
*           Valid transitions are:
*
*           Old status      Requested Status    New Status
*           destroy         create and go       not in service
*           not in service  active              active (if valid
*                                               address and type),
*                                               not in service otherwise
*           active          destroy             destroy
*           active          not in service      not in service
*
*
*           destroy         create and go*      active if valid address and type
*
*           * this transition is only available through the sntpServerAdd call.
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableRowStatusSet(L7_uint32 ndx, L7_SNTP_SERVER_STATUS_t status)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      ((status != L7_SNTP_SERVER_STATUS_DESTROY) &&
       (status != L7_SNTP_SERVER_STATUS_CREATE_AND_GO) &&
       (status != L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE) &&
       (status != L7_SNTP_SERVER_STATUS_ACTIVE)))
  {
    return L7_ERROR;
  }
  if ((status == L7_SNTP_SERVER_STATUS_CREATE_AND_GO) &&
      ((sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY)))
  {
    sntpCfgData->server[ndx-1].address[0] = '\0';
    sntpCfgData->server[ndx-1].addressType = L7_SNTP_ADDRESS_UNKNOWN;
    sntpCfgData->server[ndx-1].port = L7_SNTP_DEFAULT_SERVER_PORT;
    sntpCfgData->server[ndx-1].version = L7_SNTP_DEFAULT_VERSION;
    sntpCfgData->server[ndx-1].priority = L7_SNTP_DEFAULT_SERVER_PRIORITY;
    sntpCfgData->server[ndx-1].poll = L7_SNTP_DEFAULT_SERVER_POLL;
    serverStats[ndx-1].lastUpdateTime = 0;
    serverStats[ndx-1].lastAttemptTime = 0;
    serverStats[ndx-1].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[ndx-1].unicastServerNumRequests = 0;
    serverStats[ndx-1].unicastServerNumFailedRequests = 0;
    /** Now make it available to the SNTP client */
    sntpCfgData->server[ndx-1].status = L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
    if (sntpCfgData->sntpUnicastServerCount < L7_SNTP_MAX_SERVERS)
      sntpCfgData->sntpUnicastServerCount++;
    sntpCfgMarkDataChanged();
    return L7_SUCCESS;
  }
  if ((status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
  {
    sntpServerStatusUpdate(ndx-1);
    sntpCfgMarkDataChanged();
    return L7_SUCCESS;
  }
  if (status == L7_SNTP_SERVER_STATUS_DESTROY)
  {
    sntpTaskLock();
    sntpCfgData->server[ndx-1].status = L7_SNTP_SERVER_STATUS_DESTROY;
    if (sntpCfgData->sntpUnicastServerCount)
      sntpCfgData->sntpUnicastServerCount--;
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
    return L7_SUCCESS;
  }
  if ((status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE) &&
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_ACTIVE))
  {
    sntpTaskLock();
    sntpCfgData->server[ndx-1].status = L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
    return L7_SUCCESS;
  }
  return L7_ERROR;
}


/**********************************************************************
* @purpose  Get the IP address of the server table entry
*           specified by index.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    status @b{(output)} pointer to where result is stored
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
L7_RC_t sntpServerTableRowStatusGet(L7_uint32 ndx, L7_SNTP_SERVER_STATUS_t * status)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (status == L7_NULL))
  {
    return L7_ERROR;
  }
  if ((sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_NOT_EXIST;
  }
  *status =  sntpCfgData->server[ndx-1].status;
  return L7_SUCCESS;
}

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
L7_RC_t sntpServerTableRowStatusGetNext(L7_uint32 ndx,
              L7_SNTP_SERVER_STATUS_t * status, L7_uint32 *rowNdx)
{
  if ((status == L7_NULL) || (rowNdx == L7_NULL))
    return L7_ERROR;

  if (ndx >= L7_SNTP_MAX_SERVERS)
  {
    return L7_NOT_EXIST;
  }
  /* Return "next" valid entry in table. */
  while (ndx < L7_SNTP_MAX_SERVERS)
  {
    if ((sntpCfgData->server[ndx].status == L7_SNTP_SERVER_STATUS_INVALID) ||
        (sntpCfgData->server[ndx].status == L7_SNTP_SERVER_STATUS_DESTROY))
    {
      ndx++;
      continue;
    }
    *status = sntpCfgData->server[ndx].status;
    *rowNdx = ndx + 1;
    return L7_SUCCESS;
  }
  return L7_NOT_EXIST;
}
/**********************************************************************
* @purpose  Get the IP address type for the specified table entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
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
L7_RC_t sntpServerTableAddressTypeGet(L7_uint32 ndx,
                        L7_SNTP_ADDRESS_TYPE_t * type)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (type == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *type = sntpCfgData->server[ndx-1].addressType;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the IP address type of the address of the
*           server for the specified index
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    type @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes     - Clears the individual server stats if address type is changed.
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableAddressTypeSet(L7_uint32 ndx,
                                  L7_SNTP_ADDRESS_TYPE_t type)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      ((type != L7_SNTP_ADDRESS_UNKNOWN) &&
       (type != L7_SNTP_ADDRESS_IPV4) &&
       (type != L7_SNTP_ADDRESS_DNS)) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  if (sntpCfgData->server[ndx-1].addressType != type)
  {
    sntpCfgData->server[ndx-1].addressType = type;

    /* Clear the stats - new address type is a new server. */
    serverStats[ndx-1].lastUpdateTime = 0;
    serverStats[ndx-1].lastAttemptTime = 0;
    serverStats[ndx-1].lastAttemptStatus = L7_SNTP_STATUS_OTHER;
    serverStats[ndx-1].unicastServerNumRequests = 0;
    serverStats[ndx-1].unicastServerNumFailedRequests = 0;

    sntpTaskLock();
    sntpServerStatusUpdate(ndx-1);
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the IP port of the server for the specified entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
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
L7_RC_t sntpServerTablePortGet(L7_uint32 ndx, L7_ushort16 * port)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (port == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *port = sntpCfgData->server[ndx-1].port;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the IP port of the
*           server for the specified index
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    port @b{(input)} new port number
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTablePortSet(L7_uint32 ndx, L7_ushort16 port)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (port == 0) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  if (sntpCfgData->server[ndx-1].port != port)
  {
    sntpTaskLock();
    sntpCfgData->server[ndx-1].port = port;
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Set the IP port of the
*           server for the specified index to the default value
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTablePortDefaultSet(L7_uint32 ndx)
{
  return sntpServerTablePortSet(ndx, L7_SNTP_DEFAULT_SERVER_PORT);
}
/**********************************************************************
* @purpose  Get the IP priority of the server for the specified entry.
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
L7_RC_t sntpServerTablePriorityGet(L7_uint32 ndx, L7_ushort16 * priority)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (priority == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *priority = sntpCfgData->server[ndx-1].priority;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the IP priority type of the
*           server for the specified index
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    priority @b{(input)} new priority
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTablePrioritySet(L7_uint32 ndx, L7_ushort16 priority)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (priority < SNTP_MIN_SERVER_PRIORITY) ||
      (priority > SNTP_MAX_SERVER_PRIORITY) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }

  if (sntpCfgData->server[ndx-1].priority != priority)
  {
    sntpTaskLock();
    sntpCfgData->server[ndx-1].priority = priority;
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the IP priority type of the
*           server for the specified index
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if invalid index
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTablePriorityDefaultSet(L7_uint32 ndx)
{
  return sntpServerTablePrioritySet(ndx, L7_SNTP_DEFAULT_SERVER_PRIORITY);
}

/**********************************************************************
* @purpose  Enables polling of the server for the specified index
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    poll @b{(input)} L7__ENABLE/L7_DISABLE
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTablePollSet(L7_uint32 ndx, L7_ushort16 poll)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      ((poll != L7_ENABLE) &&
      (poll != L7_DISABLE)) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }

  if (sntpCfgData->server[ndx-1].poll != poll)
  {
    sntpTaskLock();
    sntpCfgData->server[ndx-1].poll = poll;
    if ((sntpCfgData->sntpClientMode == L7_SNTP_CLIENT_UNICAST) &&
        (sntpActiveServerGet() == (ndx - 1)))
      sntpLocalSocketClose();
    sntpTaskUnlock();
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

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
L7_RC_t sntpServerTablePollGet(L7_uint32 ndx, L7_ushort16 * poll)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (poll == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *poll = sntpCfgData->server[ndx-1].poll;
  return L7_SUCCESS;
}

L7_RC_t SntpServerIndexGet(L7_char8 *ipAddr, L7_uint32 *index)
{
  L7_uint32 i;
  for (i = 0 ;i<L7_SNTP_MAX_SERVERS;i++)
  {
    if ((strncmp(ipAddr,sntpCfgData->server[i].address,L7_SNTP_MAX_ADDRESS_LEN) == 0) && (
        (sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_ACTIVE)||
        (sntpCfgData->server[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE)))
    {
      break;
    }
  }
  if (i == L7_SNTP_MAX_SERVERS)
    return L7_TABLE_IS_FULL;
  *index =i+1;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the SNTP version of the server for the specified entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
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
L7_RC_t sntpServerTableVersionGet(L7_uint32 ndx, L7_ushort16 * version)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (version == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *version = sntpCfgData->server[ndx-1].version;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the SNTP version of the
*           server for the specified index
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    version @b{(output)} the new version
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableVersionSet(L7_uint32 ndx, L7_ushort16 version)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (version < SNTP_VERSION_MIN) ||
      (version > SNTP_VERSION_MAX) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  if (sntpCfgData->server[ndx-1].version != version)
  {
    sntpCfgData->server[ndx-1].version = version;
    sntpCfgMarkDataChanged();
  }
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the SNTP version of the
*           server for the specified index to the default value.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableVersionDefaultSet(L7_uint32 ndx)
{
  return sntpServerTableVersionSet(ndx, L7_SNTP_DEFAULT_VERSION);
}

/**********************************************************************
* @purpose  Get the SNTP last update time for the specified server entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    time @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerStatsLastUpdateTimeGet(L7_uint32 ndx, L7_uint32 * time)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (time == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *time = serverStats[ndx-1].lastUpdateTime;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the SNTP last attempt time for the specified server entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
* @param    time @b{(output)} pointer to where result is stored
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if null pointer
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerStatsLastAttemptTimeGet(L7_uint32 ndx, L7_uint32 * time)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (time == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *time = serverStats[ndx-1].lastAttemptTime;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the SNTP last update status for the specified server entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
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
L7_RC_t sntpServerStatsLastUpdateStatusGet(L7_uint32 ndx, L7_SNTP_PACKET_STATUS_t * status)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (status == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *status = serverStats[ndx-1].lastAttemptStatus;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the SNTP unicast server requests sent to the specified server entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
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
L7_RC_t sntpServerStatsUnicastServerNumRequestsGet(L7_uint32 ndx,
                                        L7_uint32 * requests)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (requests == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *requests = serverStats[ndx-1].unicastServerNumRequests;
  return L7_SUCCESS;
}
/**********************************************************************
* @purpose  Get the SNTP unicast server requests sent to the
*           specified server entry.
*
* @param    ndx @b{(input)} index of entry into table.
*           Must range from[1..L7_MAX_SERVER_ENTRIES]
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
L7_RC_t sntpServerStatsUnicastServerNumFailedRequestsGet(L7_uint32 ndx,
                                                L7_uint32 * requests)
{
  if ((ndx < 1) ||
      (ndx > L7_SNTP_MAX_SERVERS) ||
      (requests == L7_NULL) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[ndx-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    return L7_ERROR;
  }
  *requests = serverStats[ndx-1].unicastServerNumFailedRequests;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Verify deletion of a key entry, and remove references if
*           we are allowed to delete the key.
*
* @param    L7_uint32   index @b((input))  authenticate key index
*
* @returns  L7_SUCCESS  key can be deleted, all references removed
* @returns  L7_FAILURE  its invalid to delete this key
*
* @notes    This is a utility routine to verify if its valid to delete
*           an auth key entry based on the current configuration.
*           For now, just verify we will have at least one trusted key
*           if authentication is enabled, we may want to modify this
*           later to enforce a key configuration for unicast servers.
*
* @end
*********************************************************************/
static L7_RC_t sntpAuthKeyDeleteVerify(L7_uint32 index)
{
  L7_uint32 i = 0;

  /* we only care if this is an active entry */
  if (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_ACTIVE)
  {
    /* let all success conditions fall through to the end, so we are
       sure to clean up any references to this key in the server table */
    if (sntpCfgData->sntpAuthMode == L7_TRUE)
    {
      /* all client modes, ensure this is not the only trusted key */
      if (sntpCfgData->sntpKeys[index-1].trusted == L7_TRUE)
      {
        L7_BOOL foundKey = L7_FALSE;
        for (i=0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
        {
          if ((i != (index-1)) &&
              (sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
              (sntpCfgData->sntpKeys[i].trusted == L7_TRUE))
          {
            /* found another trusted key */
            foundKey = L7_TRUE;
            break;
          }
        }
        if (foundKey != L7_TRUE)
        {
          return L7_FAILURE;
        }
      }
    }
    /* if we get here its valid to remove all references from unicast servers */
    for (i=0; i < L7_SNTP_MAX_SERVERS; i++)
    {
      if (sntpCfgData->server[i].keyIndex == index)
      {
        sntpCfgData->server[i].keyIndex = SNTP_AUTH_KEY_NOT_CONFIGURED;
      }
    }
  }

  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Verify modifying a key entry to untrusted.
*
* @param    L7_uint32   index @b((input))  authenticate key index
*
* @returns  L7_SUCCESS its invalid to set this key to untrusted
* @returns  L7_FAILURE key can not be set to untrusted
*
* @notes    This is a utility routine to verify if its valid to change
*           an auth key to untrusted based on current configuration.
*
* @end
*********************************************************************/
static L7_RC_t sntpAuthKeySetUntrustedVerify(L7_uint32 index)
{
  L7_uint32 i = 0;

  /* we only care if authenticate is enabled, and this is an active key entry */
  if ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
      (sntpCfgData->sntpAuthMode == L7_TRUE))
  {
    /* all client modes, ensure this is not the only active trusted key */
    for (i=0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
    {
      if ((i != (index-1)) &&
          (sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
          (sntpCfgData->sntpKeys[i].trusted == L7_TRUE))
      {
        /* found another trusted key */
        return L7_SUCCESS;
      }
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

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
L7_RC_t sntpClientAuthModeGet(L7_BOOL *enable)
{
  if (enable == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  *enable = sntpCfgData->sntpAuthMode;
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
L7_RC_t sntpClientAuthModeSet(L7_BOOL enable)
{
  if (enable != L7_TRUE &&
      enable != L7_FALSE)
  {
    return L7_FAILURE;
  }

  sntpTaskLock();
  if (sntpCfgData->sntpAuthMode == enable)
  {
    sntpTaskUnlock();
    return L7_SUCCESS;
  }
  if (sntpClientAuthModeVerify(sntpCfgData->sntpClientMode,
                               enable) != L7_SUCCESS)
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  sntpCfgData->sntpAuthMode = enable;
  sntpCfgMarkDataChanged();
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
* @notes   can only configure server with an active key
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableAuthKeySet(L7_uint32 index, 
                                  L7_uint32 keyIndex)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_SERVERS) ||
      (keyIndex < 1) ||
      (keyIndex > L7_SNTP_MAX_AUTH_KEYS))
  {
    return L7_FAILURE;
  }

  sntpTaskLock();
  if ((sntpCfgData->server[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY) ||
      (sntpCfgData->sntpKeys[keyIndex-1].status != L7_SNTP_SERVER_STATUS_ACTIVE))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }

  if (sntpCfgData->server[index-1].keyIndex != keyIndex)
  {
    sntpCfgData->server[index-1].keyIndex = keyIndex;
    sntpCfgMarkDataChanged();
  }
  sntpTaskUnlock();
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Get the authentication key for an SNTP server table entry
*           
* @param    L7_uint32  index    @b{(input)} index of server table entry.
*                                           Must range from[1..L7_SNTP_MAX_SERVERS]
*
* @param    L7_uint32 *keyIndex @b{(output)} index of authenticate table entry.
*                                            range 1-L7_SNTP_MAX_AUTH_KEYS
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if invalid argument (index out of range)
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sntpServerTableAuthKeyGet(L7_uint32  index, 
                                  L7_uint32 *keyIndex)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_SERVERS) ||
      (keyIndex == L7_NULL))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((sntpCfgData->server[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->server[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  *keyIndex = sntpCfgData->server[index-1].keyIndex;
  sntpTaskUnlock();
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Update the status of an auth table entry
*
* @param    L7_uint32 index @b{(input)} the index of the entry to update.
*
*
* @notes    This is a utility routine that updates the status of an auth 
*           table entry based on the settings of the required fields.
*           Entries are only transitioned from active to not in service
*           and vice-versa.
*
* @end
*********************************************************************/
static void sntpAuthTableStatusUpdate(L7_uint32 index)
{
  /* require key identifier and value */
  if ((sntpCfgData->sntpKeys[index].status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
      ((sntpCfgData->sntpKeys[index].id == SNTP_AUTH_KEY_NOT_CONFIGURED) ||
       (sntpCfgData->sntpKeys[index].value[0] == '\0')))
  {
    sntpCfgData->sntpKeys[index].status = L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
  }
  else if ((sntpCfgData->sntpKeys[index].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE) &&
           (sntpCfgData->sntpKeys[index].id != SNTP_AUTH_KEY_NOT_CONFIGURED) &&
           (sntpCfgData->sntpKeys[index].value[0] != '\0'))
  {
    sntpCfgData->sntpKeys[index].status = L7_SNTP_SERVER_STATUS_ACTIVE;
  }
  return;
}

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
                            L7_uint32 *index)
{
  L7_uint32 i = 0;
  L7_int32  firstEmptySlot = -1;

  if ((keyNumber < L7_SNTP_KEY_NUMBER_MIN) ||
      (keyNumber > L7_SNTP_KEY_NUMBER_MAX) ||
      (keyValue == L7_NULLPTR) ||
      (strlen(keyValue) < L7_SNTP_KEY_LEN_MIN) ||
      (strlen(keyValue) > L7_SNTP_KEY_LEN_MAX) ||
      (index == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  sntpTaskLock();
  for (i = 0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
  {
    if ((sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_DESTROY) ||
        (sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_INVALID))
    {
      /* add here */
      if (firstEmptySlot < 0)
      {
        firstEmptySlot = i;
      }
      continue;
    }
    if ((sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) ||
        (sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
    {
      /** Duplicate entry? */
      if (sntpCfgData->sntpKeys[i].id == keyNumber)
      {
        *index = i+1;
        sntpTaskUnlock();
        return L7_ALREADY_CONFIGURED;
      }
    }
  }

  if (firstEmptySlot >= 0)
  {
    /** We have an empty slot */
    sntpCfgData->sntpKeys[firstEmptySlot].id = keyNumber;
    osapiStrncpy(sntpCfgData->sntpKeys[firstEmptySlot].value, keyValue, sizeof(sntpCfgData->sntpKeys[firstEmptySlot].value));
    sntpCfgData->sntpKeys[firstEmptySlot].trusted = L7_FALSE;
    /** Now make it available to the SNTP client */
    sntpCfgData->sntpKeys[firstEmptySlot].status = L7_SNTP_SERVER_STATUS_ACTIVE;
    *index = firstEmptySlot + 1;
    sntpCfgMarkDataChanged();
    sntpTaskUnlock();
    return L7_SUCCESS;
  }
  sntpTaskUnlock();
  return L7_TABLE_IS_FULL;
}

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
L7_RC_t sntpAuthTableKeyDelete(L7_uint32  keyNumber)
{
  L7_uint32 i = 0;

  if ((keyNumber < L7_SNTP_KEY_NUMBER_MIN) ||
      (keyNumber > L7_SNTP_KEY_NUMBER_MAX))
  {
    return L7_FAILURE;
  }

  sntpTaskLock();
  for (i = 0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
  {
    if ((sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) ||
        (sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
    {
      if (sntpCfgData->sntpKeys[i].id == keyNumber)
      {
        /* verify that deleting this key is okay, and remove references */
        if (sntpAuthKeyDeleteVerify(i+1) != L7_SUCCESS)
        {
          break;
        }
        memset(&sntpCfgData->sntpKeys[i], 0, sizeof(sntpKeyCfgData_t));
        /* why do we care if status is destroy or invalid? */
        sntpCfgData->sntpKeys[i].status = L7_SNTP_SERVER_STATUS_DESTROY;
        sntpCfgMarkDataChanged();
        sntpTaskUnlock();
        return L7_SUCCESS;
      }
    }
  }
  sntpTaskUnlock();
  return L7_FAILURE;
}

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
                              L7_uint32 *index)
{
  L7_uint32 i = 0;

  if ((keyNumber < L7_SNTP_KEY_NUMBER_MIN) ||
      (keyNumber > L7_SNTP_KEY_NUMBER_MAX))
  {
    return L7_FAILURE;
  }

  sntpTaskLock();
  for (i = 0; i < L7_SNTP_MAX_AUTH_KEYS; i++)
  {
    if ((sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_ACTIVE) ||
        (sntpCfgData->sntpKeys[i].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
    {
      if (sntpCfgData->sntpKeys[i].id == keyNumber)
      {
        *index = i+1;
        sntpTaskUnlock();
        return L7_SUCCESS;
      }
    }
  }
  sntpTaskUnlock();
  return L7_FAILURE;
}

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
                                  L7_SNTP_SERVER_STATUS_t status)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      ((status != L7_SNTP_SERVER_STATUS_DESTROY) &&
       (status != L7_SNTP_SERVER_STATUS_CREATE_AND_GO) &&
       (status != L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE) &&
       (status != L7_SNTP_SERVER_STATUS_ACTIVE)))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((status == L7_SNTP_SERVER_STATUS_CREATE_AND_GO) &&
      ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
       (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY)))
  {
    memset(&sntpCfgData->sntpKeys[index-1], 0, sizeof(sntpKeyCfgData_t));
    sntpCfgData->sntpKeys[index-1].status = L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
  }
  else if ((status == L7_SNTP_SERVER_STATUS_ACTIVE) &&
           (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE))
  {
    sntpAuthTableStatusUpdate(index-1);
  }
  else if (status == L7_SNTP_SERVER_STATUS_DESTROY)
  {
    /* verify that deleting this key is okay, and remove references */
    if (sntpAuthKeyDeleteVerify(index) != L7_SUCCESS)
    {
      sntpTaskUnlock();
      return L7_FAILURE;
    }
    memset(&sntpCfgData->sntpKeys[index-1], 0, sizeof(sntpKeyCfgData_t));
    sntpCfgData->sntpKeys[index-1].status = L7_SNTP_SERVER_STATUS_DESTROY;
  }
  else if ((status == L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE) &&
           (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_ACTIVE))
  {
    /* verify that changing this key is okay, and remove references */
    if (sntpAuthKeyDeleteVerify(index) != L7_SUCCESS)
    {
      sntpTaskUnlock();
      return L7_FAILURE;
    }
    sntpCfgData->sntpKeys[index-1].status = L7_SNTP_SERVER_STATUS_NOT_IN_SERVICE;
  }
  else 
  {
    /* invalid status transition */
    sntpTaskUnlock();
    return L7_FAILURE;
  }

  sntpCfgMarkDataChanged();
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
                                  L7_SNTP_SERVER_STATUS_t *status)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      (status == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  *status = sntpCfgData->sntpKeys[index-1].status;
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
                                      L7_uint32               *nextIndex)
{
  if ((status == L7_NULLPTR) || 
      (nextIndex == L7_NULLPTR) ||
      (index >= L7_SNTP_MAX_AUTH_KEYS))
  {
    return L7_FAILURE;
  }

  sntpTaskLock();
  /* Return "next" valid entry in table. */
  while (index < L7_SNTP_MAX_AUTH_KEYS)
  {
    if ((sntpCfgData->sntpKeys[index].status == L7_SNTP_SERVER_STATUS_INVALID) ||
        (sntpCfgData->sntpKeys[index].status == L7_SNTP_SERVER_STATUS_DESTROY))
    {
      index++;
      continue;
    }
    *status = sntpCfgData->sntpKeys[index].status;
    *nextIndex = index + 1;
    sntpTaskUnlock();
    return L7_SUCCESS;
  }
  sntpTaskUnlock();
  return L7_NOT_EXIST;
}

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
                                  L7_uint32 keyNumber)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      (keyNumber < L7_SNTP_KEY_NUMBER_MIN) ||
      (keyNumber > L7_SNTP_KEY_NUMBER_MAX))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  if (sntpCfgData->sntpKeys[index-1].id != keyNumber)
  {
    sntpCfgData->sntpKeys[index-1].id = keyNumber;
    sntpCfgMarkDataChanged();
  }
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
                                  L7_uint32 *keyNumber)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      (keyNumber == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  *keyNumber = sntpCfgData->sntpKeys[index-1].id;
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
                                 L7_char8 *keyValue)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      (keyValue == L7_NULLPTR) ||
      (strlen(keyValue) < L7_SNTP_KEY_LEN_MIN) ||
      (strlen(keyValue) > L7_SNTP_KEY_LEN_MAX))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  if (strcmp(sntpCfgData->sntpKeys[index-1].value, keyValue) != 0)
  {
    memset(sntpCfgData->sntpKeys[index-1].value, 0, L7_SNTP_KEY_LEN_MAX+1);
    strcpy(sntpCfgData->sntpKeys[index-1].value, keyValue);
    sntpCfgMarkDataChanged();
  }
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
                                 L7_char8  *keyValue)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      (keyValue == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  strcpy(keyValue, sntpCfgData->sntpKeys[index-1].value);
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
                                   L7_BOOL   trusted)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      ((trusted != L7_TRUE) &&
       (trusted != L7_FALSE)))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  if (sntpCfgData->sntpKeys[index-1].trusted != trusted)
  {
    if (trusted == L7_FALSE)
    {
      /* check if this is valid, do not untrust last trusted key 
                if authentication is enabled */
      if (sntpAuthKeySetUntrustedVerify(index) != L7_SUCCESS)
      {
        sntpTaskUnlock();
        return L7_FAILURE;
      }
    }
    sntpCfgData->sntpKeys[index-1].trusted = trusted;
    sntpCfgMarkDataChanged();
  }
  sntpTaskUnlock();
  return L7_SUCCESS;
}

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
                                   L7_BOOL   *trusted)
{
  if ((index < 1) ||
      (index > L7_SNTP_MAX_AUTH_KEYS) ||
      (trusted == L7_NULLPTR))
  {
    return L7_FAILURE;
  }
  sntpTaskLock();
  if ((sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_INVALID) ||
      (sntpCfgData->sntpKeys[index-1].status == L7_SNTP_SERVER_STATUS_DESTROY))
  {
    sntpTaskUnlock();
    return L7_FAILURE;
  }
  *trusted = sntpCfgData->sntpKeys[index-1].trusted;
  sntpTaskUnlock();
  return L7_SUCCESS;
}


