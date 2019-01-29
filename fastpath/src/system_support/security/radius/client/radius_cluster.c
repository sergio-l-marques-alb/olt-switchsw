/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     radius_cluster.c
*
* @purpose      Radius component Clustering code
*
* @component    Wireless (for now, clustering someday soon)
*
* @comments     none
*
* @create       2/23/2008
*
* @author       mpolicharla
*
* @end
*
**********************************************************************/

#include <string.h>

#include "l7_common.h"
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "l7utils_api.h"
#include "defaultconfig.h"
#include "clustering_api.h"
#include "clustering_notify_api.h"

#include "radius_cluster.h"
#include "radius_cfg.h"
#include "radius_control.h"
#include "radius_debug.h"

#define BUFFER_SIZE_512 512

extern void * radiusServerDbSyncSema;
extern radiusCnfgrState_t radiusCnfgrState;

extern L7_uchar8 * clusterEventToStringConvert(clusterEvent evt);
static L7_uint32            clusterMembers = 0;
radiusClusterOprData_t      clusterOprData;
extern void * radiusClusteringQueue;

/* Internal Functions */
static L7_uchar8 * radiusClusterEventToStringConvert(radiusClusterState_t state);
static void radiusClusterQueueMsgHandle(radiusClusterMessage_t * pMsg);
static void radiusClusterChangeCurrentState(radiusClusterState_t newState);
static L7_RC_t radiusClusterCfgSendHandle(clusterMemberID_t * memberID);
static L7_RC_t radiusClusterRxCfgDataValidate(void);

/*********************************************************************
*
* @purpose  Initialize the Receive structure for clustering messages.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*
*********************************************************************/
static void radiusClientClusterRecvInit (void)
{
  memset (&clusterOprData.radiusClusterCfg, 0x00, sizeof(radiusClientClusterCfg_t));
} /* radiusClientClusterRecvInit */

/*********************************************************************
*
* @purpose  Validate the received RADIUS clustering configuration message
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*
*********************************************************************/
static L7_RC_t radiusClientClusterRecvValidate (void)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 index = 0, innerIndex = 0;
  L7_BOOL done = L7_FALSE;

  for (index = 0; (index < L7_RADIUS_MAX_ACCT_SERVERS) && (done == L7_FALSE);
                            index++)
  {
    for (innerIndex = index+1;
        (innerIndex < L7_RADIUS_MAX_ACCT_SERVERS) && (done == L7_FALSE);
         innerIndex++)
    {
      if (osapiStrncmp (clusterOprData.radiusClusterCfg.acctServer[index].serverName,
                        clusterOprData.radiusClusterCfg.acctServer[innerIndex].serverName,
                        L7_RADIUS_SERVER_NAME_LENGTH) == 0)
      {
        rc = L7_FAILURE;
        done = L7_TRUE;
      }
    }
  }

  return rc;
} /* radiusClientClusterRecvValidate */

/*********************************************************************
*
* @purpose  Initialize a a server entry based on inputs provided
*
* @param    radiusClientServer_t *server @b{(input)} Server Entry
* @param    L7_uint32 serverType @b{(input)} Server Type - Auth/Acct
* @param    dnsHost_t *hostname @b{(input)} RADIUS Server params
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t radiusClusterServerInit (radiusClientServer_t *server,
                            L7_uint32 serverType, dnsHost_t *hostname)
{
  memset (server, 0x00, sizeof(radiusClientServer_t));

  server->serverConfig.radiusServerDnsHostName.hostAddrType = hostname->hostAddrType;
  if (hostname->hostAddrType == L7_IP_ADDRESS_TYPE_DNS)
  {
    osapiStrncpy (server->serverConfig.radiusServerDnsHostName.host.hostName,
                            hostname->host.hostName,
                            L7_DNS_HOST_NAME_LEN_MAX-1);
    server->serverConfig.radiusServerDnsHostName.host.hostName[L7_DNS_HOST_NAME_LEN_MAX-1] = '\0';
  }
  else
  {
    server->serverConfig.radiusServerDnsHostName.host.ipAddr =
                            hostname->host.ipAddr;
  }

  server->serverConfig.radiusServerConfigServerEntryType = FD_RADIUS_SERVER_ENTRY_TYPE;
  server->serverConfig.radiusServerConfigSecret[0] = '\0';
  server->serverConfig.radiusServerIsSecretConfigured = L7_FALSE;
  server->serverConfig.radiusServerConfigPriority = FD_RADIUS_PRIORITY;
  server->serverConfig.incMsgAuthMode = FD_RADIUS_INC_MSG_AUTH_MODE;
  if (serverType == RADIUS_SERVER_TYPE_AUTH)
  {
    server->serverConfig.radiusServerConfigUdpPort = FD_RADIUS_AUTH_PORT;
  }
  else
  {
    server->serverConfig.radiusServerConfigUdpPort = FD_RADIUS_ACCT_PORT;
  }
  if (hostname->hostAddrType == L7_IP_ADDRESS_TYPE_DNS)
  {
    server->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_ACTIVE;
  }
  else
  {
    server->serverConfig.radiusServerConfigRowStatus = RADIUS_SERVER_NOTREADY;
  }

  if (serverType == RADIUS_SERVER_TYPE_AUTH)
  {
    osapiStrncpy (server->serverName, L7_RADIUS_SERVER_DEFAULT_NAME_AUTH,
                            L7_RADIUS_SERVER_NAME_LENGTH);
  }
  else
  {
    osapiStrncpy (server->serverName, L7_RADIUS_SERVER_DEFAULT_NAME_ACCT,
                            L7_RADIUS_SERVER_NAME_LENGTH);
  }
  server->serverName[L7_RADIUS_SERVER_NAME_LENGTH] = '\0';

  return L7_SUCCESS;
} /* radiusClusterServerInit */

/*********************************************************************
*
* @purpose  Check if entry exists for given host name.
*
* @param    L7_uint32 serverType @b{(input)} Server Type - Auth/Acct
* @param    dnsHost_t *hostname @b{(input)} RADIUS Server params
*
* @returns  radiusClientServer_t - Pointer to server entry
*
* @notes
*
* @end
*
*********************************************************************/
radiusClientServer_t *radiusClusterServerEntryGet (L7_uint32 serverType,
                            dnsHost_t *hostname)
{
  L7_uint32 index, max = 0, current = 0;
  radiusClientServer_t *server = NULL;

  if (hostname == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }

  if (serverType == RADIUS_SERVER_TYPE_AUTH)
  {
    server = &clusterOprData.radiusClusterCfg.authServer[0];
    max = L7_RADIUS_MAX_AUTH_SERVERS;
    current = clusterOprData.radiusClusterCfg.authServerCount;
  }
  else
  {
    server = &clusterOprData.radiusClusterCfg.acctServer[0];
    max = L7_RADIUS_MAX_ACCT_SERVERS;
    current = clusterOprData.radiusClusterCfg.acctServerCount;
  }

  for (index = 0; (index < max) && (index !=current); index++)
  {
    if (hostname->hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      if (server->serverConfig.radiusServerDnsHostName.host.ipAddr ==
                            hostname->host.ipAddr)
      {
        break;
      }
    }
    else
    {
      if (osapiStrncmp (server->serverConfig.radiusServerDnsHostName.host.hostName,
                            hostname->host.hostName,
                            L7_DNS_HOST_NAME_LEN_MAX-1) == 0)
      {
        break;
      }
    }
    server++;
  }

  if (index == max)
  {
    return L7_NULLPTR;
  }

  if (radiusClusterServerInit (server, serverType, hostname) != L7_SUCCESS)
  {
    return L7_NULLPTR;
  }

  if (serverType == RADIUS_SERVER_TYPE_AUTH)
  {
    if (index == clusterOprData.radiusClusterCfg.authServerCount)
    {
      clusterOprData.radiusClusterCfg.authServerCount++;
    }
  }
  else
  {
    if (index == clusterOprData.radiusClusterCfg.acctServerCount)
    {
      clusterOprData.radiusClusterCfg.acctServerCount++;
    }
  }

  return server;
} /* radiusClusterServerEntryGet */

/*********************************************************************
*
* @purpose  Process the received Clustering configuration message
*
* @param    L7_char8 *packet @b{(input)} Packet Buffer
* @param    L7_uint32 packetLen @b{(input)} Received packet length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t radiusClusterMsgRecv (L7_uchar8 *packet, L7_uint32 packetLen)
{
  radiusPduHdl_t pdu;
  radiusMsgElementHdr_t element;

  L7_uchar8 tempChar = 0;
  L7_ushort16 tempShort = 0;
  L7_uint32 tempInt = 0;

  L7_char8 buffer[BUFFER_SIZE_512];
  L7_uchar8 serverType = RADIUS_SERVER_TYPE_AUTH, addrType = 0;

  dnsHost_t dnsHost;
  radiusClientServer_t *radiusServer = L7_NULLPTR;

  if (packet == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  pdu.bufPtr = packet;
  pdu.offset = 0;

  if (clusterOprData.state != RADIUS_CLUSTER_CFG_RX_START_STATE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Packet received in invalid state %s.\n",
            radiusClusterEventToStringConvert(clusterOprData.state));
    return L7_FAILURE;
  }

  memset(&element, 0, sizeof(radiusMsgElementHdr_t));

  RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO, "Received Cluster-RADIUS-Client-Global-Message of size %d.\n",packetLen);

  while (pdu.offset < packetLen)
  {
    RADIUS_PKT_ELEMENT_HDR_MEMCPY_GET(element, pdu.bufPtr, pdu.offset);
    if ((packetLen - pdu.offset) < element.elementLen)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Received corrupt Cluster-RADIUS-Client-Global-Message, reading element 0x%x.\n",
              element.elementId);
      return L7_FAILURE;
    }
    switch (element.elementId)
    {
      case L7_IE_RADIUS_CLIENT_ACCT_ADMIN_MODE_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_ACCT_ADMIN_MODE_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        clusterOprData.radiusClusterCfg.acctAdminMode = tempChar;
        break;
      case L7_IE_RADIUS_CLIENT_MAX_NUM_RETRANS_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_MAX_NUM_RETRANS_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        clusterOprData.radiusClusterCfg.maxNumRetrans = tempChar;
        break;
      case L7_IE_RADIUS_CLIENT_TIMEOUT_DURATION_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_TIMEOUT_DURATION_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT16_GET(tempShort, pdu.bufPtr, pdu.offset);
        clusterOprData.radiusClusterCfg.timeOutDuration = tempShort;
        break;
      case L7_IE_RADIUS_CLIENT_DEADTIME_DURATION_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_DEADTIME_DURATION_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT16_GET(tempShort, pdu.bufPtr, pdu.offset);
        clusterOprData.radiusClusterCfg.radiusServerDeadTime = tempShort;
        break;
      case L7_IE_RADIUS_CLIENT_SRC_IP_ADDR_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SRC_IP_ADDR_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT32_GET(tempInt, pdu.bufPtr, pdu.offset);
        clusterOprData.radiusClusterCfg.sourceIpAddress = tempInt;
        break;
      case L7_IE_RADIUS_CLIENT_SECRET_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SECRET_SIZE,
                            pdu.offset);
        RADIUS_PKT_STRING_GET(buffer, L7_IE_RADIUS_CLIENT_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);
        osapiStrncpy ( clusterOprData.radiusClusterCfg.radiusServerSecret,
                            buffer, L7_IE_RADIUS_CLIENT_SECRET_SIZE);
        clusterOprData.radiusClusterCfg.radiusServerSecret[L7_IE_RADIUS_CLIENT_SECRET_SIZE] = '\0';
        break;
      case L7_IE_RADIUS_CLIENT_NAS_IP_MODE_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_NAS_IP_MODE_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        clusterOprData.radiusClusterCfg.nasIpMode = tempChar;
        break;
      case L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_SIZE,
                            pdu.offset);
        memcpy (&addrType, (pdu.bufPtr+pdu.offset), sizeof(L7_uchar8));
        pdu.offset += sizeof(L7_uchar8);
        if (addrType == L7_WDM_IP_ADDR_TYPE_V4)
        {
          pdu.offset += sizeof(L7_in6_addr_t) - sizeof(L7_IP_ADDR_t);
          RADIUS_PKT_INT32_GET(tempInt, pdu.bufPtr, pdu.offset);
          clusterOprData.radiusClusterCfg.nasIpAddress = tempInt;
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "Invalid IP Address format received. Expecting IPv4 only addresses.\n");
          return L7_FAILURE;
        }
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset)
        serverType = tempChar;
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset)
        dnsHost.hostAddrType = tempChar;
        RADIUS_PKT_STRING_GET(buffer, L7_DNS_HOST_NAME_LEN_MAX+1,
                            pdu.bufPtr, pdu.offset);

        /*
         * Maximum size of the Host Name is 255 in the RADIUS Client Db,
         * so copy only 254.
         */
        osapiStrncpy (dnsHost.host.hostName, buffer, L7_DNS_HOST_NAME_LEN_MAX-1);
        dnsHost.host.hostName[L7_DNS_HOST_NAME_LEN_MAX-1] = '\0';
        if (dnsHost.hostAddrType == L7_IP_ADDRESS_TYPE_IPV4)
        {
          tempInt = osapiInet_addr(dnsHost.host.hostName);
          dnsHost.host.hostName[0] = '\0';
          dnsHost.host.ipAddr = tempInt;
        }
        radiusServer = radiusClusterServerEntryGet (serverType, &dnsHost);
        if (radiusServer == L7_NULLPTR)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "Cannot add new RADIUS server %s of type %d.", buffer, serverType);
          return L7_FAILURE;
        }
        RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO, "Add new RADIUS server %s of type %d.", buffer, serverType);
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_PORT_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_PORT_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT16_GET(tempShort, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.radiusServerConfigUdpPort = tempShort;
        }
        break;
      case L7_IE_RADIUS_CLIENT_GROUP_NAME_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE,
                            pdu.offset);
        RADIUS_PKT_STRING_GET(buffer, L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE,
                            pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          osapiStrncpy (radiusServer->serverName, buffer,
                            L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE);
          radiusServer->serverName[L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE] = '\0';
        }
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_SECRET_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE,
                            pdu.offset);
        RADIUS_PKT_STRING_GET(buffer, L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          osapiStrncpy (radiusServer->serverConfig.radiusServerConfigSecret,
                            buffer, L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE);
          radiusServer->serverConfig.radiusServerConfigSecret[L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE] = '\0';
        }
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT32_GET(tempInt, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.radiusServerConfigPriority = tempInt;
        }
        break;
      case L7_IE_RADIUS_CLIENT_MSG_AUTH_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_MSG_AUTH_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.incMsgAuthMode = tempChar;
        }
        break;
      case L7_IE_RADIUS_CLIENT_PRIMARY_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_PRIMARY_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.radiusServerConfigServerEntryType = tempChar;
        }
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_CONFIG_MASK_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_CONFIG_MASK_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.localConfigMask = tempChar;
        }
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_USAGE_TYPE:

        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_USAGE_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.usageType = tempChar;
        }
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_MAX_NUM_RETRANS_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_MAX_NUM_RETRANS_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT8_GET(tempChar, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.maxNumRetrans = tempChar;
        }
        break;
      case L7_IE_RADIUS_CLIENT_SERVER_TIMEOUT_DURATION_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_TIMEOUT_DURATION_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT16_GET(tempShort, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.timeOutDuration = tempShort;
        }

        break;
      case L7_IE_RADIUS_CLIENT_SERVER_DEADTIME_DURATION_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_DEADTIME_DURATION_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT16_GET(tempShort, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.radiusServerDeadTime = tempShort;
        }

        break;
      case L7_IE_RADIUS_CLIENT_SERVER_SRC_IP_ADDR_TYPE:
        RADIUS_PKT_ELEMENT_SIZE_CHECK(element,
                            L7_IE_RADIUS_CLIENT_SERVER_SRC_IP_ADDR_SIZE,
                            pdu.offset);
        RADIUS_PKT_INT32_GET(tempInt, pdu.bufPtr, pdu.offset);
        if (radiusServer != L7_NULLPTR)
        {
          radiusServer->serverConfig.sourceIpAddress = tempInt;
        }

        break;
      default:
        pdu.offset += element.elementLen;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Invalid element 0x%x received in the message.\n",
                            element.elementId);
        break;
    }
  }

  return L7_SUCCESS;
} /* radiusClusterMsgRecv */

/*********************************************************************
*
* @purpose  Copy the received the clustering message
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*
*********************************************************************/
void radiusClusterCfgCopy (void)
{
  dnsHost_t *hostname = L7_NULLPTR;
  dnsHost_t nasIPAddr;
  dnsHost_t hostIPAddr;
  radiusServerConfigEntry_t *serverConfig = L7_NULLPTR;
  L7_int32 index = 0;
  L7_uint32 port = 0;
  L7_BOOL nasMode = L7_FALSE;
  radiusAuthServerInfo_t authServerInfo;

  /* Clear all the entries first */
  radiusServerRemoveAll ();

  authServerInfo.localGlobal = L7_RADIUSGLOBAL; /* Always Global for now. */
  authServerInfo.val.number  = clusterOprData.radiusClusterCfg.acctAdminMode;
  if (radiusIssueCmd (RADIUS_ACCT_ADMIN_MODE_SET, L7_NULL,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Failed to set RADIUS Accounting Admin Mode.\n");
  }

  hostIPAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
  hostIPAddr.host.ipAddr  = ALL_RADIUS_SERVERS;
  authServerInfo.localGlobal = L7_RADIUSGLOBAL; /* Always Global for now. */
  authServerInfo.val.number  = clusterOprData.radiusClusterCfg.maxNumRetrans;
  if (radiusIssueCmd (RADIUS_MAX_RETRANS_SET, &hostIPAddr,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Failed to set RADIUS Max. Number of retransmissions.\n");
  }
  hostIPAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
  hostIPAddr.host.ipAddr  = ALL_RADIUS_SERVERS;
  authServerInfo.localGlobal = L7_RADIUSGLOBAL; /* Always Global for now. */
  authServerInfo.val.number  = clusterOprData.radiusClusterCfg.timeOutDuration;
  if (radiusIssueCmd (RADIUS_TIMEOUT_SET, &hostIPAddr,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Failed to set RADIUS Timeout Duration.\n");
  }

  hostIPAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
  hostIPAddr.host.ipAddr  = ALL_RADIUS_SERVERS;
  authServerInfo.localGlobal = L7_RADIUSGLOBAL;
  authServerInfo.val.number = clusterOprData.radiusClusterCfg.radiusServerDeadTime;
  if(radiusIssueCmd (RADIUS_DEADTIME_SET, &hostIPAddr,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Failed to set RADIUS DeadTime Duration.\n");
  }
  hostIPAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
  hostIPAddr.host.ipAddr  = ALL_RADIUS_SERVERS;
  authServerInfo.localGlobal = L7_RADIUSGLOBAL;
  authServerInfo.val.number = clusterOprData.radiusClusterCfg.sourceIpAddress;
  if (radiusIssueCmd (RADIUS_SOURCEIP_SET, &hostIPAddr,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Failed to set Source IP Address and Mode.\n");
  }

  hostIPAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
  hostIPAddr.host.ipAddr  = ALL_RADIUS_SERVERS;
  authServerInfo.localGlobal = L7_RADIUSGLOBAL;
  osapiStrncpySafe(authServerInfo.val.str,
                    clusterOprData.radiusClusterCfg.radiusServerSecret,
                    sizeof(authServerInfo.val.str));
  if (radiusIssueCmd (RADIUS_AUTH_SECRET_SET, &hostIPAddr,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set RADIUS Auth Secret.\n");
  }

  nasIPAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
  nasIPAddr.host.ipAddr = clusterOprData.radiusClusterCfg.nasIpAddress;
  nasMode = clusterOprData.radiusClusterCfg.nasIpMode;
  if (radiusIssueCmd (RADIUS_ATTRIBUTE_4_SET, &nasIPAddr, (void *)&nasMode) !=
                            L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Failed to set RADIUS NAS IP Address and Mode.\n");
  }

  index = clusterOprData.radiusClusterCfg.authServerCount-1;
  for (;index >= 0; index--)
  {
    hostname = &clusterOprData.radiusClusterCfg.authServer[index].serverConfig.radiusServerDnsHostName;
    serverConfig = &clusterOprData.radiusClusterCfg.authServer[index].serverConfig;
    if (radiusIssueCmd (RADIUS_AUTH_IPADDR_ADD, hostname,
                            clusterOprData.radiusClusterCfg.authServer[index].serverName) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to add Authentication server to Db.\n");
      continue;
    }
    osapiSemaTake(radiusServerDbSyncSema, L7_WAIT_FOREVER);
    port = serverConfig->radiusServerConfigUdpPort;
    if (radiusIssueCmd (RADIUS_AUTH_PORT_SET, hostname, (void *)&port) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set RADIUS Auth Port.\n");
    }

    /* Max No of Retrans */
    authServerInfo.localGlobal = RADIUS_SERVER_CONFIG_TYPE_GET(
                                  serverConfig->localConfigMask,
                                  RADIUS_CONFIG_MAX_NUM_RETRANS_BIT_POS);
    authServerInfo.val.number = serverConfig->maxNumRetrans;
    if (radiusIssueCmd (RADIUS_MAX_RETRANS_SET, hostname,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set Max Retrans per server to Db.\n");

    }

    /* Time out Duration */
    authServerInfo.localGlobal = RADIUS_SERVER_CONFIG_TYPE_GET(
                                  serverConfig->localConfigMask,
                                  RADIUS_CONFIG_TIMEOUT_BIT_POS);
    authServerInfo.val.number = serverConfig->timeOutDuration;
    if (radiusIssueCmd (RADIUS_TIMEOUT_SET, hostname,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to add Timeout per server to Db.\n");

    }

    /* Dead Time */
    authServerInfo.localGlobal = RADIUS_SERVER_CONFIG_TYPE_GET(
                                  serverConfig->localConfigMask,
                                  RADIUS_CONFIG_DEAD_TIME_BIT_POS);
    authServerInfo.val.number = serverConfig->radiusServerDeadTime;
    if (radiusIssueCmd (RADIUS_DEADTIME_SET, hostname,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to add Dead Time per server to Db.\n");

    }

    /* Source IP */
    authServerInfo.localGlobal = RADIUS_SERVER_CONFIG_TYPE_GET(
                                  serverConfig->localConfigMask,
                                  RADIUS_CONFIG_SOURCE_IP_ADDRESS_BIT_POS);

    authServerInfo.val.number = serverConfig->sourceIpAddress;
    if (radiusIssueCmd (RADIUS_SOURCEIP_SET, hostname,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to Source IP per server to Db.\n");

    }
    if (strlen (serverConfig->radiusServerConfigSecret) >  0)
    {
      authServerInfo.localGlobal = RADIUS_SERVER_CONFIG_TYPE_GET(
                                  serverConfig->localConfigMask,
                                  RADIUS_CONFIG_SECRET_BIT_POS);
      osapiStrncpySafe(authServerInfo.val.str, serverConfig->radiusServerConfigSecret, sizeof(authServerInfo.val.str));
      if (radiusIssueCmd (RADIUS_AUTH_SECRET_SET, hostname,
                            (void *)&authServerInfo) !=
                            L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Failed to set RADIUS Auth Secret.\n");
      }
    }
    if (radiusIssueCmd (RADIUS_SERVER_TYPE_SET, hostname,
                            (void *)&serverConfig->radiusServerConfigServerEntryType) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set RADIUS Server Type.\n");
    }
    if (radiusIssueCmd (RADIUS_AUTH_SERVER_PRIORITY_SET, hostname,
                            (void *)&serverConfig->radiusServerConfigPriority) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set RADIUS Server Priority Type.\n");
    }
     /* Usage Type */
    if (radiusIssueCmd(RADIUS_AUTH_USAGE_TYPE_SET, hostname, (void *)&serverConfig->usageType) !=
                        L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set Auth Usage Type \n");
    }
    if (radiusIssueCmd (RADIUS_SERVER_INC_MSG_AUTH_MODE_SET, hostname,
                            (void *)&serverConfig->incMsgAuthMode) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set Msg. Auth Mode.\n");
    }
  }

  index = clusterOprData.radiusClusterCfg.acctServerCount-1;
  for (;index >= 0; index--)
  {
    hostname = &clusterOprData.radiusClusterCfg.acctServer[index].serverConfig.radiusServerDnsHostName;
    serverConfig = &clusterOprData.radiusClusterCfg.acctServer[index].serverConfig;
    if (radiusIssueCmd (RADIUS_ACCT_IPADDR_ADD, hostname,
                            clusterOprData.radiusClusterCfg.acctServer[index].serverName) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to add Accounting server to Db.\n");
      continue;
    }
    osapiSemaTake(radiusServerDbSyncSema, L7_WAIT_FOREVER);
    port = serverConfig->radiusServerConfigUdpPort;
    if (radiusIssueCmd (RADIUS_ACCT_PORT_SET, hostname, (void *)&port) !=
                            L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Failed to set RADIUS Acct Port.\n");
    }
    if (strlen (serverConfig->radiusServerConfigSecret) > 0)
    {
      if (radiusIssueCmd (RADIUS_ACCT_SECRET_SET, hostname,
                            (void *)&serverConfig->radiusServerConfigSecret) !=
                            L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Failed to set RADIUS Acct Secret.\n");
      }
    }
  }
} /* radiusClusterCfgCopy */

/*********************************************************************
*
* @purpose  Callback function to handle clustering messages.
*
* @param    L7_ushort16 msgType @b{(input)} Message Type
* @param    clusterMsgDeliveryMethod method @b{(input)} Delivery Method
* @param    L7_uint32 msgLength @b{(input)} Received packet length
* @param    L7_char8 *msgBuf @b{(input)} Packet Buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
void radiusClusterCfgMsgCallback(L7_ushort16 msgType,
                            clusterMsgDeliveryMethod method,
                            L7_uint32 msgLength, L7_uchar8 *msgBuf)
{
  switch (msgType)
  {
    case RADIUS_CLUSTER_CONFIG_MSG_TYPE:
      radiusDebugPktDump(RD_LEVEL_CLUSTER_RX_PKT, msgBuf, msgLength);
      if (radiusClusterMsgRecv (msgBuf, msgLength) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Failed to parse RADIUS Client Configuration message.\n");
      }
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS Client module received unknown clustering configuration message.\n");
      break;
  }
} /* radiusClusterCfgMsgCallback */

/*********************************************************************
* @purpose  Utility function to print state in string format
*
* @param    state      @b{(input)} radius cluster application state ID
*
* @returns  L7_uchar8 *
*
* @notes    Assumes that output buffer has sufficient size
*
* @notes
*
* @end
*********************************************************************/
static L7_uchar8 * radiusClusterEventToStringConvert(radiusClusterState_t state)
{
  static L7_uchar8 * names[RADIUS_CLUSTER_MAX_NUM_STATES] =
  { "RADIUS_CLUSTER_WAIT_STATE",
    "RADIUS_CLUSTER_CFG_TX_STATE",
    "RADIUS_CLUSTER_CFG_RX_START_STATE",
    "RADIUS_CLUSTER_CFG_RX_VALIDATE_STATE",
    "RADIUS_CLUSTER_CFG_RX_APPLY_STATE",
    "RADIUS_CLUSTER_CFG_RX_ABORT_STATE",
    "RADIUS_CLUSTER_READY_TO_START_STATE"
  };

  if (state < RADIUS_CLUSTER_MAX_NUM_STATES)
    return names[state];
  return "Unknown";
}/* radiusClusterEventToStringConvert */

/*********************************************************************
* @purpose  Function for updating radius cluster state
*
* @param    newState    @b{(input)} newradius cluster state to update
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void radiusClusterChangeCurrentState(radiusClusterState_t newState)
{
  RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO, "state change:  %s -> %s\n",
          radiusClusterEventToStringConvert(clusterOprData.state),
          radiusClusterEventToStringConvert(newState));
  clusterOprData.state = newState;
}/* radiusClusterChangeCurrentState */

/*********************************************************************
* @purpose  Function for updating radius cluster state
*
* @param    event    @b{(input)} Cluster event
* @param    memId    @b{(input)} Cluster memeber ID asssociated with the event
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radiusClusterEventCallback(clusterEvent event, clusterMemberID_t * memID)
{
  L7_uchar8  buf[ sizeof(clusterMemberID_t) * 3 ];
  radiusClusterMsgDesc_t mtype;

  memset(buf, 0, sizeof(buf));

  if (!memID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "cluster Memeber ID is NULL!\n");
    return;
  }

  RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO, "Event %d received from = %02x:%02x:%02x:%02x:%02x:%02x\n", event,
                            memID->addr[0], memID->addr[1], memID->addr[2],
                            memID->addr[3], memID->addr[4], memID->addr[5]);

  switch (event)
  {
    case CLUSTER_EVENT_LOCAL_SWITCH_JOINED:
      memcpy(&clusterOprData.selfID, memID, sizeof(clusterMemberID_t));
      clusterMembers++;
      mtype = RADIUS_CLUSTER_MEMBER_SELF_JOINED;
      break;
    case CLUSTER_EVENT_LOCAL_SWITCH_LEFT:
      memset(&clusterOprData.ctrID, 0, sizeof(clusterMemberID_t));
      clusterMembers--;
      mtype = RADIUS_CLUSTER_MEMBER_SELF_LEFT;
      break;
    case CLUSTER_EVENT_SWITCH_JOINED:
      clusterMembers++;
      mtype = RADIUS_CLUSTER_MEMBER_JOINED;
      break;
    case CLUSTER_EVENT_SWITCH_LEFT:
      if (0 == memcmp(&clusterOprData.ctrID, memID, sizeof(clusterMemberID_t)))
        memset(&clusterOprData.ctrID, 0, sizeof(clusterMemberID_t));
      clusterMembers--;
      mtype = RADIUS_CLUSTER_MEMBER_LEFT;
      break;
    case CLUSTER_EVENT_CONTROLLER_ELECTED:
      memcpy(&clusterOprData.ctrID, memID, sizeof(clusterMemberID_t));
      mtype = RADIUS_CLUSTER_MEMBER_CONTROLLER_ELECTED;
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Unknown event dropped without handling.\n");
      break;
  }
}/* radisuClusterEventCallback */


/*********************************************************************
* @purpose  Callback to initiate sending radius configuration to cluster memeber
*
* @param    pID    @b{(input)} Pointer to cluster member for sending radius configuration
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radiusCfgSendCallback(clusterMemberID_t * pID)
{
  radiusClusterMessage_t msg;

  if (!pID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Cluster member ID is NULL!.\n");
    return;
  }

  msg.mtype = RADIUS_CLUSTER_CFG_SEND;
  memcpy(&(msg.cmid), pID, sizeof(clusterMemberID_t));

  if (L7_SUCCESS != osapiMessageSend(radiusClusteringQueue, &msg,
                                     sizeof(radiusClusterMessage_t),
                                     L7_NO_WAIT, L7_MSG_PRIORITY_NORM))
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
             "%s: msg send failed to radiusClusteringQueue!\n", __FUNCTION__);
  }
}/* radiusCfgSendCallback */

/******************************************************************************
* @purpose  Function for handling radius configuration send message from cluster
*
* @param    memberID    @b{(input)} Cluster member ID to send radius configuration
*
* @returns  none
*
* @notes    It sends the appropriate message to the denoted peer (memberID),
*           and tells the clustering system that the send has been finished.
*
* @end
******************************************************************************/
static L7_RC_t radiusClusterCfgSendHandle(clusterMemberID_t * memberID)
{
  radiusPduHdl_t pdu;
  radiusMsgElementHdr_t *element = L7_NULLPTR;
  L7_uint32 authSrvrCount = 0;
  L7_uint32 acctSrvrCount = 0;
  L7_uint32 tempInt = 0;
  L7_ushort16 tempShort = 0;
  L7_uchar8 tempChar = 0;
  L7_BOOL tempBool = L7_FALSE;
  L7_IP_ADDR_t ipAddr = 0;
  L7_char8   buf[L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE+1];
  L7_char8   srvrSecret[L7_IE_RADIUS_CLIENT_SECRET_SIZE+1];
  L7_char8 host[L7_DNS_HOST_NAME_LEN_MAX+1];
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_RC_t  rc = L7_FAILURE;
  dnsHost_t dnsHost;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;

  if (L7_NULLPTR == memberID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Cluster member ID is null!");
    return rc;
  }

  authSrvrCount = radiusAuthServerCount();
  acctSrvrCount = radiusAcctServerCount();

  /* Send Global radius client configuration only once */
  memset (clusterOprData.sendMsgBuf, 0x00, sizeof (clusterOprData.sendMsgBuf));
  pdu.bufPtr = clusterOprData.sendMsgBuf;
  pdu.offset = 0;

  /* Maximum Number of retransmits */
  RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_MAX_NUM_RETRANS_TYPE,
                            L7_IE_RADIUS_CLIENT_MAX_NUM_RETRANS_SIZE,
                            pdu.bufPtr, pdu.offset);
  dnsHost.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
  dnsHost.host.ipAddr = ALL_RADIUS_SERVERS;
  radiusMaxNoOfRetransGet(&dnsHost, &tempInt, &paramStatus);
  tempChar = (L7_uchar8) tempInt;
  RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

  /* Accounting Admin Mode */
  RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_ACCT_ADMIN_MODE_TYPE,
                            L7_IE_RADIUS_CLIENT_ACCT_ADMIN_MODE_SIZE,
                            pdu.bufPtr, pdu.offset);
  tempBool = radiusAcctModeGet();
  tempChar = (L7_uchar8) tempBool;
  RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

  /* Radius Client Timeout Duration Type */
  RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_TIMEOUT_DURATION_TYPE,
                            L7_IE_RADIUS_CLIENT_TIMEOUT_DURATION_SIZE,
                            pdu.bufPtr, pdu.offset);
  radiusTimeOutDurationGet(&dnsHost, &tempInt, &paramStatus);
  tempShort = (L7_ushort16) tempInt;
  RADIUS_PKT_INT16_SET(tempShort, pdu.bufPtr, pdu.offset);

  /* Radius Client Deadtime Duration Type */
  RADIUS_PKT_ELEMENT_HDR_SET(element,L7_IE_RADIUS_CLIENT_DEADTIME_DURATION_TYPE,
                            L7_IE_RADIUS_CLIENT_DEADTIME_DURATION_SIZE,
                            pdu.bufPtr, pdu.offset);
  radiusServerDeadtimeGet(&dnsHost, &tempInt, &paramStatus);
  tempShort = (L7_ushort16) tempInt;
  RADIUS_PKT_INT16_SET(tempShort, pdu.bufPtr, pdu.offset);

  /* Radius Client Source IP address */
  RADIUS_PKT_ELEMENT_HDR_SET(element,L7_IE_RADIUS_CLIENT_SRC_IP_ADDR_TYPE,
                            L7_IE_RADIUS_CLIENT_SRC_IP_ADDR_SIZE,
                            pdu.bufPtr, pdu.offset);
  radiusServerSourceIPGet(&dnsHost, &tempInt, &paramStatus);
  RADIUS_PKT_INT32_SET(tempInt, pdu.bufPtr, pdu.offset);

  /* Radius Secret Type */
  memset(srvrSecret,0,L7_IE_RADIUS_CLIENT_SECRET_SIZE);
  RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SECRET_TYPE,
                            L7_IE_RADIUS_CLIENT_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);
  radiusHostNameServerSharedSecretGet(&dnsHost, srvrSecret, &paramStatus);
  RADIUS_PKT_STRING_SET(srvrSecret, L7_IE_RADIUS_CLIENT_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);
  /* NAS IP Mode */
  RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_NAS_IP_MODE_TYPE,
                            L7_IE_RADIUS_CLIENT_NAS_IP_MODE_SIZE,
                            pdu.bufPtr, pdu.offset);
  tempInt = L7_FALSE;
  radiusAttribute4Get(&tempInt,&ipAddr);
  tempChar = (L7_uchar8) tempInt;
  RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

  /* NAS IP Addr */
  RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_TYPE,
                            L7_IE_RADIUS_CLIENT_NAS_IP_ADDR_SIZE,
                            pdu.bufPtr, pdu.offset);
  tempChar = L7_WDM_IP_ADDR_TYPE_V4;
  RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

  pdu.offset += sizeof(L7_in6_addr_t) - sizeof(L7_IP_ADDR_t);
  tempInt = (L7_uint32)ipAddr;
  RADIUS_PKT_INT32_SET(tempInt, pdu.bufPtr, pdu.offset);

  /* Hex packet dump */
  radiusDebugPktDump(RD_LEVEL_CLUSTER_TX_PKT, pdu.bufPtr, pdu.offset);

  rc = clusterMsgSend(memberID, CLUSTER_MSG_DELIVERY_RELIABLE,
                            RADIUS_CLUSTER_CONFIG_MSG_TYPE,
                            pdu.offset, pdu.bufPtr);
  if(rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Cluster Message send failed to member ID = %02x:%02x:%02x:%02x:%02x:%02x\n",
            memberID->addr[0], memberID->addr[1],
            memberID->addr[2], memberID->addr[3],
            memberID->addr[4], memberID->addr[5]);
    return rc;
  }

  while (authSrvrCount > 0)
  {
    if (radiusServerFirstIPHostNameGet(host, &addrType) != L7_SUCCESS)
    {
      RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO, "\n No radius Auth server entries \n");
      authSrvrCount = 0;
    }

    dnsHost.hostAddrType = addrType;
    if (addrType == L7_IP_ADDRESS_TYPE_DNS)
    {
      strncpy(dnsHost.host.hostName, host, sizeof(L7_DNS_HOST_NAME_LEN_MAX));
    }
    else
    {
      dnsHost.host.ipAddr = osapiInet_addr(host);
    }

    /* Server Configuration */
    /* Start to form new message to send additional server configuration */
    memset (clusterOprData.sendMsgBuf, 0x00, sizeof(clusterOprData.sendMsgBuf));
    pdu.bufPtr = clusterOprData.sendMsgBuf;
    pdu.offset = 0;
    do
    {
      /* Server type, IP type and IP */
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempInt = 0;
      if (radiusHostNameServerTypeGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server Type Get Failed!");
      }
      /* server type */
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);
      /* IP type */
      tempChar = (L7_uchar8) addrType;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);
      /* IP address in string format always */
      RADIUS_PKT_STRING_SET(host, L7_DNS_HOST_NAME_LEN_MAX+1,
                            pdu.bufPtr, pdu.offset)

      /* Server Port */
      tempInt = 0;
      if (radiusHostNameServerPortNumGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server Port Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element,L7_IE_RADIUS_CLIENT_SERVER_PORT_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_PORT_SIZE ,
                            pdu.bufPtr, pdu.offset);
      tempShort = (L7_ushort16) tempInt;
      RADIUS_PKT_INT16_SET(tempShort, pdu.bufPtr, pdu.offset);

      /* Server Group Name */
      memset(buf,0,(L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE+1));
      if (radiusServerHostNameGet(host, addrType, buf) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server Name Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_GROUP_NAME_TYPE,
                            L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE,
                            pdu.bufPtr, pdu.offset);
      RADIUS_PKT_STRING_SET(buf, L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE,
                            pdu.bufPtr, pdu.offset);

      /* Server Secret */
      memset(srvrSecret,0,L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE);
      if (radiusHostNameServerSharedSecretGet(&dnsHost, srvrSecret, &paramStatus) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server secret Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_SECRET_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);
      RADIUS_PKT_STRING_SET(srvrSecret, L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);

      /* Server Priority */
      tempInt = 0;
      if (radiusHostNameServerPriorityGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server Priority Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_SIZE,
                            pdu.bufPtr, pdu.offset);
      RADIUS_PKT_INT32_SET(tempInt, pdu.bufPtr, pdu.offset);

      /* Usage Type */
      tempInt = 0;
      if (radiusServerUsageTypeGet(&dnsHost, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server Message Usage Type Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_USAGE_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_USAGE_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

      /* Message Authentication Mode */
      tempInt = 0;
      if (radiusHostNameServerIncMsgAuthModeGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server Message Authentication Mode Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_MSG_AUTH_TYPE,
                            L7_IE_RADIUS_CLIENT_MSG_AUTH_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

      /* Primary Server Designator */
      tempInt = 0;
      if (radiusHostNameServerEntryTypeGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Authentication Server Port Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_PRIMARY_TYPE,
                            L7_IE_RADIUS_CLIENT_PRIMARY_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

      /* Config Mask */
      tempInt = 0;
      if (radiusLocalConfigMaskGet(dnsHost, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Local Config Mask Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_CONFIG_MASK_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_CONFIG_MASK_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

      /* Max number of retransmissions */
      tempInt = 0;
      if (radiusMaxNoOfRetransGet(&dnsHost, &tempInt, &paramStatus) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Max Number of retransmissions Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_MAX_NUM_RETRANS_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_MAX_NUM_RETRANS_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

      /* Time out */
      tempInt = 0;
      if (radiusTimeOutDurationGet(&dnsHost, &tempInt, &paramStatus) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Time out Duration Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_TIMEOUT_DURATION_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_TIMEOUT_DURATION_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempShort = (L7_ushort16) tempInt;
      RADIUS_PKT_INT16_SET(tempShort, pdu.bufPtr, pdu.offset);

      /* Dead time */
      tempInt = 0;
      if (radiusServerDeadtimeGet(&dnsHost, &tempInt, &paramStatus) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Dead time Parameter Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_DEADTIME_DURATION_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_DEADTIME_DURATION_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempShort = (L7_ushort16) tempInt;
      RADIUS_PKT_INT16_SET(tempShort, pdu.bufPtr, pdu.offset);

      /* Source IP */
      tempInt = 0;
      if (radiusServerSourceIPGet(&dnsHost, &tempInt, &paramStatus) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Source IP address Parameter Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_SRC_IP_ADDR_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_SRC_IP_ADDR_SIZE,
                            pdu.bufPtr, pdu.offset);
      RADIUS_PKT_INT32_SET(tempInt, pdu.bufPtr, pdu.offset);

      /* Decrement the server count to send */
      authSrvrCount--;

      /*
       * If message size exceeds 8192 bytes need to split the server config to
       * next message
       */
      if ((pdu.offset >= sizeof(clusterOprData.sendMsgBuf)) ||
          ((pdu.offset + L7_IE_RADIUS_CLIENT_SRVR_CONFIG_SIZE) >
                                  sizeof(clusterOprData.sendMsgBuf)) ||
          (authSrvrCount == 0))
      {
        /* Hex packet dump */
        radiusDebugPktDump(RD_LEVEL_CLUSTER_TX_PKT, pdu.bufPtr, pdu.offset);

        rc = clusterMsgSend(memberID, CLUSTER_MSG_DELIVERY_RELIABLE,
                            RADIUS_CLUSTER_CONFIG_MSG_TYPE,
                            pdu.offset, pdu.bufPtr);
        if(rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "Cluster Message send failed to member ID  = %02x:%02x:%02x:%02x:%02x:%02x line %d\n",
                  memberID->addr[0], memberID->addr[1],
                  memberID->addr[2], memberID->addr[3],
                  memberID->addr[4], memberID->addr[5], __LINE__);
        }

        /* Start to form new message to send additional server configuration */
        memset (clusterOprData.sendMsgBuf, 0x00, sizeof (clusterOprData.sendMsgBuf));
        pdu.bufPtr = clusterOprData.sendMsgBuf;
        pdu.offset = 0;

        if (rc != L7_SUCCESS)
        {
          return rc;
        }
      }
    } while(radiusServerNextIPHostNameGet(host, host, &addrType) == L7_SUCCESS);
  }

  /* Send accounting server information */
  while (acctSrvrCount > 0)
  {
    if (radiusAcctServerFirstIPHostNameGet(host, &addrType) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "\n No radius Accounting server entries \n");
      acctSrvrCount = 0;
    }

    /* Accounting Configuration */
    /* Start to form new message to send additional server configuration */
    memset (clusterOprData.sendMsgBuf, 0x00, sizeof (clusterOprData.sendMsgBuf));
    pdu.bufPtr = clusterOprData.sendMsgBuf;
    pdu.offset = 0;
    do
    {
      /* Server type, IP type and IP */
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempInt = 0;
      if (radiusAcctHostNameServerTypeGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Accounting Server Type Get Failed!");
      }
      /* server type */
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);
      /* IP type */
      tempChar = (L7_uchar8) addrType;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);
      /* IP address in string format always */
      RADIUS_PKT_STRING_SET(host, L7_DNS_HOST_NAME_LEN_MAX+1,
                            pdu.bufPtr, pdu.offset)

      /* Server Port */
      tempInt = 0;
      if (radiusAcctServerPortNumGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Accounting Server Port Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element,L7_IE_RADIUS_CLIENT_SERVER_PORT_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_PORT_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempShort = (L7_ushort16) tempInt;
      RADIUS_PKT_INT16_SET(tempShort, pdu.bufPtr, pdu.offset);

      /* Server Group Name */
      memset(buf,0,(L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE+1));
      if (radiusServerAcctHostNameGet(host, addrType, buf) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Accounting Server Name Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_GROUP_NAME_TYPE,
                            L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE,
                            pdu.bufPtr, pdu.offset);
      RADIUS_PKT_STRING_SET(buf, L7_IE_RADIUS_CLIENT_GROUP_NAME_SIZE,
                            pdu.bufPtr, pdu.offset);

      /* Server Secret */
      memset(srvrSecret,0,L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE);
      if (radiusAcctHostNameServerSharedSecretGet(host, addrType, srvrSecret) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Accounting Server secret Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_SECRET_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);
      RADIUS_PKT_STRING_SET(srvrSecret, L7_IE_RADIUS_CLIENT_SERVER_SECRET_SIZE,
                            pdu.bufPtr, pdu.offset);

      /* Server Priority */
      tempInt = 0;
      if (radiusAcctHostNameServerPriorityGet(host, addrType, &tempInt) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Accounting Server Priority Get Failed!");
      }
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_TYPE,
                            L7_IE_RADIUS_CLIENT_SERVER_PRIORITY_SIZE ,
                            pdu.bufPtr, pdu.offset);
      RADIUS_PKT_INT32_SET(tempInt, pdu.bufPtr, pdu.offset);

      /* Message Authentication Mode */
      tempInt = 0;
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_MSG_AUTH_TYPE,
                            L7_IE_RADIUS_CLIENT_MSG_AUTH_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

      /* Primary Server Designator */
      tempInt = 0;
      RADIUS_PKT_ELEMENT_HDR_SET(element, L7_IE_RADIUS_CLIENT_PRIMARY_TYPE,
                            L7_IE_RADIUS_CLIENT_PRIMARY_SIZE,
                            pdu.bufPtr, pdu.offset);
      tempChar = (L7_uchar8) tempInt;
      RADIUS_PKT_INT8_SET(tempChar, pdu.bufPtr, pdu.offset);

      /* Decrement the server count to send */
      acctSrvrCount--;

      /*
       * If message size exceeds 8192 bytes need to split the server config to
       * next message
       */
      if ((pdu.offset >= sizeof(clusterOprData.sendMsgBuf)) ||
          ((pdu.offset + L7_IE_RADIUS_CLIENT_SRVR_CONFIG_SIZE) >
                          sizeof(clusterOprData.sendMsgBuf)) ||
          (acctSrvrCount == 0))
      {
        /* Hex packet dump */
        radiusDebugPktDump(RD_LEVEL_CLUSTER_TX_PKT, pdu.bufPtr, pdu.offset);

        rc = clusterMsgSend(memberID, CLUSTER_MSG_DELIVERY_RELIABLE,
                            RADIUS_CLUSTER_CONFIG_MSG_TYPE,
                            pdu.offset, pdu.bufPtr);
        if(rc != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "Cluster Message send failed to member ID = %02x:%02x:%02x:%02x:%02x:%02x line %d\n",
                            memberID->addr[0], memberID->addr[1],
                            memberID->addr[2], memberID->addr[3],
                            memberID->addr[4], memberID->addr[5], __LINE__);
        }

        /* Start to form new message to send additional server configuration */
        memset (clusterOprData.sendMsgBuf, 0x00, sizeof (clusterOprData.sendMsgBuf));
        pdu.bufPtr = clusterOprData.sendMsgBuf;
        pdu.offset = 0;

        if (rc != L7_SUCCESS)
        {
          return rc;
        }
      }
    } while(radiusAcctServerNextIPHostNameGet(host,host,&addrType)==L7_SUCCESS);
  }

  return rc;
} /* radiusClusterCfgSendHandle */

/******************************************************************************
* @purpose  Function for validating received radius configuration from cluster memeber
*
* @param    none
*
* @returns  L7_SUCCESS if validation success
* @returns  L7_FAILURE if validation fails
*
* @notes    none
*
* @end
******************************************************************************/

static L7_RC_t radiusClusterRxCfgDataValidate(void)
{
  return L7_SUCCESS;
}

/******************************************************************************
* @purpose  Function for validating received radius configuration from cluster memeber
*
* @param    cmd    @b{(input)} Command from cluster module for receiving radius configuration
*
* @returns  none
*
* @notes    none
*
* @end
******************************************************************************/
void radiusCfgRxCallback(clusterConfigCmd cmd)
{
  radiusClusterMessage_t msg;

  switch(cmd)
  {
    case CLUSTER_CONFIG_CMD_START_TRANSFER:
      msg.mtype = RADIUS_CLUSTER_CFG_RX_START;
      RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO, "Rx Callback : CLUSTER_CONFIG_CMD_START_TRANSFER\n");
      break;
    case CLUSTER_CONFIG_CMD_END_TRANSFER:
      msg.mtype = RADIUS_CLUSTER_CFG_RX_END;
      RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO,"Rx Callback : CLUSTER_CONFIG_CMD_END_TRANSFER\n");
      break;
    case CLUSTER_CONFIG_CMD_ABORT:
      msg.mtype = RADIUS_CLUSTER_CFG_RX_ABORT;
      RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO,"Rx Callback : CLUSTER_CONFIG_CMD_ABORT\n");
      break;
    case CLUSTER_CONFIG_CMD_APPLY:
      msg.mtype = RADIUS_CLUSTER_CFG_RX_APPLY;
      RADIUS_DLOG(RD_LEVEL_CLUSTER_INFO,"Rx Callback : CLUSTER_CONFIG_CMD_APPLY\n");
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "%s: unknown command %d\n", __FUNCTION__, cmd);
      return;
      break;
  }

  if (L7_SUCCESS != osapiMessageSend(radiusClusteringQueue, &msg,
                                     sizeof(radiusClusterMessage_t),
                                     L7_NO_WAIT, L7_MSG_PRIORITY_NORM))
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "%s: message send failed!\n", __FUNCTION__);
  }
}/* radiusCfgRxCallback */


/******************************************************************************
* @purpose  Function for processing message in radius cluster queue
*
* @param    pMsg    @b{(input)} Pointer to message
*
* @returns  none
*
* @notes    Helper routine used by the clustering task's main loop
*           for interpreting task messages.  Basically, it treats
*           messages like state machine events and handles the
*           transitions to new states.
*
* @end
******************************************************************************/
static void radiusClusterQueueMsgHandle(radiusClusterMessage_t * pMsg)
{
  if (L7_NULLPTR == pMsg)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Cluster message is NULL!\n");
    return;
  }

  switch(pMsg->mtype)
  {
    case RADIUS_CLUSTER_MEMBER_SELF_JOINED:
    case RADIUS_CLUSTER_MEMBER_SELF_LEFT:
    case RADIUS_CLUSTER_MEMBER_JOINED:
    case RADIUS_CLUSTER_MEMBER_LEFT:
      /*
       * If membership has changed, wipe out all of our
       * operational data except current state, and go
       * into a "hold" state until we're sure that a
       * controller is present.
       */
      clusterOprData.state   = RADIUS_CLUSTER_WAIT_STATE;
      break;

    case RADIUS_CLUSTER_MEMBER_CONTROLLER_ELECTED:
      radiusClusterChangeCurrentState(RADIUS_CLUSTER_READY_TO_START_STATE);
      break;

    case RADIUS_CLUSTER_CFG_SEND:
      /*
       * If we're already in RADIUS_CLUSTER_CFG_TX_STATE, do nothing.
       * If we're in an operational state, change to
       * RADIUS_CLUSTER_CFG_TX_STATE.  Otherwise, complain bitterly.
       */
      if (RADIUS_CLUSTER_IN_OPER_STATE(clusterOprData.state))
      {
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_CFG_TX_STATE);
      }
      else if (RADIUS_CLUSTER_CFG_TX_STATE != clusterOprData.state)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Invalid state %s to send cluster config.\n",
        radiusClusterEventToStringConvert(clusterOprData.state));
      }
      break;

    case RADIUS_CLUSTER_CFG_RX_START:
      if (RADIUS_CLUSTER_IN_OPER_STATE(clusterOprData.state))
      {
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_CFG_RX_START_STATE);
        radiusClientClusterRecvInit ();
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Invalid state %s to RX cluster config.\n",
                radiusClusterEventToStringConvert(clusterOprData.state));
      }
      break;

    case RADIUS_CLUSTER_CFG_RX_END:
      if (RADIUS_CLUSTER_CFG_RX_START_STATE == clusterOprData.state)
      {
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_CFG_RX_VALIDATE_STATE);
        radiusClientClusterRecvValidate ();
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Invalid state %s for RX ending\n",
                radiusClusterEventToStringConvert(clusterOprData.state));
        /*
         * The clustering component requires an rxDone()
         * response to this event to continue.
         * Make sure this happens!
         */
        clusterConfigRxDone(CLUSTER_CFG_ID(RADIUS), L7_FAILURE);
      }
      break;

    case RADIUS_CLUSTER_CFG_RX_APPLY:
      if (RADIUS_CLUSTER_CFG_RX_VALIDATE_STATE == clusterOprData.state)
      {
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_CFG_RX_APPLY_STATE);
        radiusClusterCfgCopy ();
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Invalid state %s to apply data.\n",
                radiusClusterEventToStringConvert(clusterOprData.state));
      }
      break;

    case RADIUS_CLUSTER_CFG_RX_ABORT:
      if (RADIUS_CLUSTER_CFG_RX_VALIDATE_STATE == clusterOprData.state)
      {
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_CFG_RX_ABORT_STATE);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Invalid state %s to handle RX Abort message.\n",
                radiusClusterEventToStringConvert(clusterOprData.state));
      }
      break;

    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "Invalid state %s, to hanlde msg %d dropping message.\n",
              radiusClusterEventToStringConvert(clusterOprData.state), pMsg->mtype);
      break;
  }
}

/*********************************************************************
*
* @purpose  Radius Clustering main task
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

void radius_cluster_task(void)
{
  radiusClusterMessage_t msg;
  L7_BOOL gotMsg = L7_FALSE;

  osapiTaskInitDone(L7_RADIUS_CLUSTER_TASK_SYNC);
  memset(&clusterOprData.ctrID, 0, sizeof(clusterMemberID_t));
  memset(&clusterOprData.selfID, 0, sizeof(clusterMemberID_t));

  /* You really shouldn't call clusterMsgRegister() until init_phase_2
   * of the configurator.
   */
  while (radiusCnfgrState != RADIUS_PHASE_EXECUTE)
  {
    osapiSleep(1);
  }

  if (L7_SUCCESS != clusterMemberNotifyRegister(radiusClusterEventCallback))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS_CLUSTER: Could not register event notification callback!!\n");
  }

  if (L7_SUCCESS != clusterMsgRegister(RADIUS_CLUSTER_CONFIG_MSG_TYPE,
                                       CLUSTER_MSG_DELIVERY_RELIABLE,
                                       radiusClusterCfgMsgCallback))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS_CLUSTER: Register radius config message with cluster failed.\n");
  }

  if (L7_SUCCESS != clusterConfigRegister(CLUSTER_CFG_ID(RADIUS),
                                          radiusCfgSendCallback,
                                          radiusCfgRxCallback))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS_CLUSTER: Couldn't register cfg callbacks!\n");
  }

  while (L7_TRUE)
  {
    gotMsg = (osapiMessageReceive(radiusClusteringQueue,
                                 (void *)&msg, sizeof(radiusClusterMessage_t),
                                  RX_WAIT_TIME_IN_MSECS) == L7_SUCCESS );
    if (L7_TRUE == gotMsg)
    {
      radiusClusterQueueMsgHandle(&msg);
    }

    switch(clusterOprData.state)
    {
      case RADIUS_CLUSTER_WAIT_STATE:
        {
          /* If controller is present transition ready to start state */

          clusterMemberID_t nobody;
          memset(&nobody, 0, sizeof(clusterMemberID_t));

          if (0 != memcmp(&nobody, &clusterOprData.ctrID, sizeof(clusterMemberID_t)))
          {
            radiusClusterChangeCurrentState(RADIUS_CLUSTER_READY_TO_START_STATE);
          }
        }
        break;

      case RADIUS_CLUSTER_CFG_TX_STATE:
        clusterConfigSendDone(CLUSTER_CFG_ID(RADIUS),
                              radiusClusterCfgSendHandle(&msg.cmid));
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_WAIT_STATE);
        break;

      case RADIUS_CLUSTER_CFG_RX_START_STATE:
        break;

      case RADIUS_CLUSTER_CFG_RX_VALIDATE_STATE:

        /* Validate received configuraation */
        clusterConfigRxDone(CLUSTER_CFG_ID(RADIUS),
             (L7_SUCCESS == radiusClusterRxCfgDataValidate()) ?
                            L7_SUCCESS : L7_FAILURE);
        break;
      case RADIUS_CLUSTER_CFG_RX_APPLY_STATE:
        /* Apply received configuration here, then signal "apply done" */
        clusterConfigRxApplyDone(CLUSTER_CFG_ID(RADIUS));
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_READY_TO_START_STATE);
        break;
      case RADIUS_CLUSTER_CFG_RX_ABORT_STATE:
        /* Configuration receive aborted, clear the received configuration */
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_READY_TO_START_STATE);
        break;
      case RADIUS_CLUSTER_READY_TO_START_STATE:
        break;
      default:
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "Unknown state (%d) !?\n", clusterOprData.state);
        radiusClusterChangeCurrentState(RADIUS_CLUSTER_WAIT_STATE);
        break;
    }
  }/* end while (L7_TRUE) */
}

