/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_txrc.c
*
* @purpose RADIUS Client transmit and receive
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/

#ifdef _L7_OS_VXWORKS_
#include <selectLib.h>
#endif
#ifdef _L7_OS_LINUX_
#include <sys/select.h>
#endif
#include "radius_include.h"
#include "dns_client_api.h"
#include "l7_socket.h"
#ifdef L7_ROUTING_PACKAGE
#include "l7_ip_api.h"
#endif
#include "osapi_support.h"

/* Mutex for shared data. */
extern void * radiusTaskSyncSema;
extern radiusCfg_t radiusCfg;

extern L7_uint32 radius_lg_bp_id;

/*********************************************************************
*
* @purpose Resolve the hostname to an IP address for All server Entries
*          configured except for the input server passed
*
* @param newEntry @b((input)) the Server Entry for which resolution should
*                             not be done
*
* @returns radiusServerEntry_t @b(output)
* @returns L7_NULL If name resolution fails
*
* @comments
*
* @end
*
*********************************************************************/
radiusServerEntry_t *radiusResolveGetNextServerIPAddress(radiusServerEntry_t *serverEntry)
{
  radiusServerEntry_t *nextEntry = radiusServerEntries;
  L7_RC_t rc = L7_FAILURE;

  while(nextEntry != L7_NULL)
  {
    if(nextEntry == serverEntry)
      continue;
    if(nextEntry->serverConfig.radiusServerDnsHostName.hostAddrType
            == L7_IP_ADDRESS_TYPE_DNS)
    {
      rc =  radiusResolveServerIPAddress(nextEntry);
      if(rc == L7_SUCCESS)
      {
        nextEntry->serverConfig.radiusServerConfigRowStatus =
                              RADIUS_SERVER_ACTIVE;
        return nextEntry;
      }
    }
    else
    {
      /* case for IP Address when no currentAuth server entry is present */
      nextEntry->serverConfig.radiusServerConfigRowStatus =
                              RADIUS_SERVER_ACTIVE;
      return nextEntry;
    }
    nextEntry = nextEntry->nextEntry;
  }
  return L7_NULL;
}

/*********************************************************************
*
* @purpose Resolve the hostname to an IP address for All server Entries
*          configured except for the input server passed
*
* @param radiusServerType_t Radius server type
*
* @returns
* @returns
*
* @comments
*  Do DNS name resolution of all Servers which are not resolved
*
* @end
*
*********************************************************************/
void radiusResolveAllServerIPAddresses(radiusServerType_t type)
{
  radiusServerEntry_t *nextEntry = radiusServerEntries;
  L7_RC_t rc = L7_FAILURE;

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Resolving all dns server entries.",
                     __FUNCTION__);

  while(nextEntry != L7_NULL)
  {
    if((nextEntry->serverConfig.radiusServerDnsHostName.hostAddrType
            == L7_IP_ADDRESS_TYPE_DNS) &&
       (nextEntry->serverConfig.radiusServerConfigRowStatus ==
                   RADIUS_SERVER_NOTREADY) &&
        (nextEntry->serverConfig.radiusServerConfigServerType == type))
    {
      rc =  radiusResolveServerIPAddress(nextEntry);
      if(rc == L7_SUCCESS)
      {
        RADIUS_DLOG(RD_LEVEL_INFO,"%s(): DNS entry resolved-: dnsName: %s IP: %s.",
                     __FUNCTION__,nextEntry->radiusServerDnsHostName.host.
                     hostName,
                     osapiInet_ntoa(nextEntry->serverConfig.
                     radiusServerConfigIpAddress));

        nextEntry->serverConfig.radiusServerConfigRowStatus =
                              RADIUS_SERVER_ACTIVE;
      }
    }
    nextEntry = nextEntry->nextEntry;
  }
  return;
}

/*********************************************************************
*
* @purpose Resolve the hostname to an IP address for the server Entry
*
* @param newEntry @b((input)) the new Server Entry
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusResolveServerIPAddress(radiusServerEntry_t *serverEntry)
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 ipaddr = 0;
  L7_char8  fqhostname[L7_DNS_DOMAIN_NAME_SIZE_MAX]; /* fully qualified host name */
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;

  rc = dnsClientNameLookup(serverEntry->radiusServerDnsHostName.host.hostName,
                           &status, fqhostname, &ipaddr);
  if(rc == L7_FAILURE)
    return rc;

  if(ipaddr == serverEntry->radiusServerConfigIpAddress)
  {
    /* no ip address change on dns lookup */
    return rc;
  }
  else
  {
    serverEntry->serverConfig.radiusServerConfigIpAddress = ipaddr;
    serverEntry->radiusServerConfigIpAddress = ipaddr;
    radiusCfg.hdr.dataChanged = L7_TRUE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Open the UDP socket connections for the RADIUS client.
*
* @param newEntry @b((input)) the new Server Entry
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerOpen(radiusServerEntry_t *newEntry)
{
  L7_uint32 serverSocket;
  L7_sockaddr_in_t baddr;
#ifdef L7_ROUTING_PACKAGE
  L7_uint32 intIfNum;
  L7_uint32 ifState;
#endif

  L7_uint32 ipAddrToBind;
  /* we will do the DNS resolution once more. If there is a time gap
     between the server configured and the usage of the server and there
     may be change in the DNS address. If not much of a gap DNS client
     would have this entry in the dynamic cache and would return immediately
   */
  if( (0 == newEntry->serverConfig.radiusServerConfigIpAddress) &&
      (newEntry->radiusServerDnsHostName.hostAddrType ==
           L7_IP_ADDRESS_TYPE_DNS)
    )
  {
    if(radiusResolveServerIPAddress(newEntry) == L7_SUCCESS)
    {
      newEntry->serverConfig.radiusServerConfigRowStatus =
                 RADIUS_SERVER_ACTIVE;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "%s(): dns resolution is not successful for server: %s ! ",
                 __FUNCTION__,
                 newEntry->serverConfig.radiusServerDnsHostName.host.hostName);
    }
  }

  /*
  ** Create a socket and the locally bound port number.
  */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, L7_NULL, &serverSocket) == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Failed to create socket\n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Use source IP Address if specified */
  /* Even though INADDR_ANY equals 0, following if statement is added for code readability. */
  ipAddrToBind = INADDR_ANY;
#ifdef L7_ROUTING_PACKAGE
  if (newEntry->serverConfig.sourceIpAddress != 0)
  {
    /* Only use this address if it belongs to a valid interface. */
    if (ipMapIpAddressToIntf(newEntry->serverConfig.sourceIpAddress, &intIfNum) == L7_SUCCESS)
    {
      if (ipMapRtrIntfOperModeGet(intIfNum, &ifState) == L7_SUCCESS)
      {
        if (ifState == L7_ENABLE)
        {
          ipAddrToBind = newEntry->serverConfig.sourceIpAddress;
        }
      }
    }
  }
#endif
  baddr.sin_addr.s_addr = ipAddrToBind;
  baddr.sin_family = L7_AF_INET;
  baddr.sin_port = osapiHtons(L7_NULL);
  if (osapiSocketBind(serverSocket, (L7_sockaddr_t *)&baddr,sizeof(baddr)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: Bind error\n");
    osapiSocketClose(serverSocket);
    return L7_FAILURE;
  }

  /*
  ** Initialize the new server entry with the socket and set the state.
  */
  newEntry->serverSocket = serverSocket;
  newEntry->status = RADIUS_SERVER_STATUS_UP;

  if (newEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
  {
    newEntry->radiusServerStats_t.authStats.radiusAuthServerAddress =
    newEntry->serverConfig.radiusServerConfigIpAddress;
    newEntry->radiusServerStats_t.authStats.radiusAuthClientServerPortNumber =
    newEntry->serverConfig.radiusServerConfigUdpPort;

    if( radiusServerAuthCurrentEntrySet(newEntry->serverNameIndex, newEntry->serverConfig.usageType, newEntry) != L7_SUCCESS)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to set the new current Auth. entry",__FUNCTION__);
      return L7_FAILURE;
    }

    radiusGlobalData.serverEntryChangeTime = osapiTimeMillisecondsGet();
  }
  else
  {
    newEntry->radiusServerStats_t.acctStats.radiusAcctServerAddress =
    newEntry->serverConfig.radiusServerConfigIpAddress;
    newEntry->radiusServerStats_t.acctStats.radiusAcctClientServerPortNumber =
    newEntry->serverConfig.radiusServerConfigUdpPort;

    if( radiusServerAcctCurrentEntrySet(newEntry->serverNameIndex,
                     newEntry) != L7_SUCCESS)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to set the new current Acct. entry",__FUNCTION__);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Handler for receiving the messages from RADIUS Auth & Acct servers.
*
* @param void.
*
* @returns None.
*
* @comments
*
* @end
*
*********************************************************************/
void radiusPacketReceive(void)
{
  radiusServerEntry_t *serverEntry;
  L7_int32 status;
  radiusPacket_t *packetSock = L7_NULL;
  fd_set rFd;
  L7_uint32 maxServerSocket;
  struct timeval timeout;
  L7_RC_t rc;
  socketRxStatus_t rxStatus;
  nwInterface_t tmpSocketInfo;
#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  L7_uint32 i;
  nwInterface_t *interface = &radiusGlobalData.nwInterfaces[0];
  radiusPacket_t *packetIntf = L7_NULL;
#endif


  /*
  ** Set all the fds for the open connections with Radius Servers to know
  ** whether they are ready to read.
  */
  FD_ZERO(&rFd);
  maxServerSocket = L7_NULL;

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
       serverEntry = serverEntry->nextEntry)
    {
      if (serverEntry->status != RADIUS_SERVER_STATUS_DOWN)
      {
        FD_SET(serverEntry->serverSocket, &rFd);
        serverEntry->status = RADIUS_SERVER_STATUS_UP_READING;
        if (serverEntry->serverSocket > maxServerSocket)
        {
          maxServerSocket = serverEntry->serverSocket;
        }
      }
    }

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
    for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
    {
      if( interface[i].socket >0 )
      {
        FD_SET(interface[i].socket,&rFd);
        if(interface[i].socket > maxServerSocket)
        {
          maxServerSocket = interface[i].socket;
        }
      }
    }
#endif

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_RADIUS_COMPONENT_ID, "RADIUS: Could not get the Task "
                            "Sync semaphore!. Resource issue with RADIUS Client service.\n");

    return;
  }


  /*
  ** Wait in the select until a RADIUS packet is received or the timeout period
  ** expires. This timeout will allow for changes to the valid server list to
  ** propagate and correct what sockets we are waiting on.
  */
  timeout.tv_sec  = L7_RADIUS_VALID_SERVER_WAIT;
  timeout.tv_usec = L7_NULL;

  status = osapiSelect(maxServerSocket + 1,
                       &rFd,
                        L7_NULL,
                        L7_NULL,
                        timeout.tv_sec,
                       timeout.tv_usec);

  if (RADIUS_LOCK_TAKE(radiusTaskSyncSema, L7_WAIT_FOREVER) == L7_SUCCESS)
  {
    if (status <= L7_NULL)
    {
      /* select timeout or error, retry again later */

      /* Reinstating the status that has been set to READING earlier.*/
      for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
        serverEntry = serverEntry->nextEntry)
      {
        if (serverEntry->status == RADIUS_SERVER_STATUS_UP_READING)
        {
          serverEntry->status = RADIUS_SERVER_STATUS_UP;
        }
      }
      (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);

      return;
    }

    /*
    ** Read the RADIUS responses from all the servers which are ready for reading.
    */
    for (serverEntry = radiusServerEntries; serverEntry != L7_NULLPTR;
       serverEntry = serverEntry->nextEntry)
    {
      if (serverEntry->status != RADIUS_SERVER_STATUS_UP_READING)
      {
        /* Not really reading on this server - it must have been added when the
           semaphore was released during the select above */
        continue;
      }
      serverEntry->status = RADIUS_SERVER_STATUS_UP;

      if( L7_NULL == FD_ISSET(serverEntry->serverSocket, &rFd))
      {
        continue; /* Read FD not set */
      }
      memset(&tmpSocketInfo,L7_NULL,sizeof(tmpSocketInfo));
      tmpSocketInfo.socket = serverEntry->serverSocket;

      RADIUS_DLOG(RD_LEVEL_INFO,"%s Rx data available from server(%s)'s socket %d ",
                     __FUNCTION__,
                     osapiInet_ntoa(serverEntry->serverConfig.
                     radiusServerConfigIpAddress),
                     serverEntry->serverSocket);



      if (packetSock == L7_NULL)
      {
        if (bufferPoolAllocate(radius_lg_bp_id, (L7_uchar8 **)&packetSock) != L7_SUCCESS)
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "RADIUS: Could not allocate a buffer for the packet\n");
          return;
        }

        (void)memset((void *)packetSock, L7_NULL, RADIUS_LG_BUF_SIZE);
      }

      rc = radiusSocketReceive(serverEntry,tmpSocketInfo,
                               &rxStatus, packetSock);
      if(rc != L7_SUCCESS)
      {
        if (packetSock != L7_NULL)
        {
          bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packetSock);
        }
        switch(rxStatus)
        {
            case L7_RADIUS_RX_STS_PKT_MALFORMED:
                 if (serverEntry->serverConfig.radiusServerConfigServerType ==
                         RADIUS_SERVER_TYPE_AUTH)
                 {
                   serverEntry->radiusServerStats_t.authStats.
                         radiusAuthClientMalformedAccessResponses++;
                 }
                 else
                 {
                   serverEntry->radiusServerStats_t.acctStats.
                         radiusAcctClientMalformedResponses++;
                 }
                 break;

            case L7_RADIUS_RX_STS_SENDER_UNKNOWN:
                 if (serverEntry->serverConfig.radiusServerConfigServerType ==
                         RADIUS_SERVER_TYPE_AUTH)
                 {
                   radiusGlobalData.radiusAuthClientInvalidServerAddresses++;
                 }
                 else
                 {
                   radiusGlobalData.radiusAcctClientInvalidServerAddresses++;
                 }
                 break;

            case L7_RADIUS_RX_STS_INVALID_MSG_CODE:
                 if (serverEntry->serverConfig.radiusServerConfigServerType ==
                         RADIUS_SERVER_TYPE_AUTH)
                 {
                   serverEntry->radiusServerStats_t.authStats.
                         radiusAuthClientUnknownTypes++;
                 }
                 else
                 {
                   serverEntry->radiusServerStats_t.acctStats.
                         radiusAcctClientUnknownTypes++;
                 }
                 break;

            default:
                 L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                         "RADIUS: (): packet receive failed with other status.\n");
                 break;
        }
        if(L7_ERROR == rc )
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return;
        }
      }
    } /* For */

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
    for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
    {
      if( L7_NULL == interface[i].ipAddr ||
          L7_NULL == FD_ISSET(interface[i].socket,&rFd) )
      {
        continue; /* Read FD not set */
      }

      RADIUS_DLOG(RD_LEVEL_INFO,"%s Rx data available from socket %d bound to i/f(%s) ",
                     __FUNCTION__,interface[i].socket,
                     osapiInet_ntoa(interface[i].ipAddr));

      if (packetIntf == L7_NULL)
      {
        if (bufferPoolAllocate(radius_lg_bp_id, (L7_uchar8 **)&packetIntf) != L7_SUCCESS)
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                  "RADIUS: Could not allocate a buffer to receive on Intf Socket\n");
          return;
        }
        (void)memset((void *)packetIntf, L7_NULL, RADIUS_LG_BUF_SIZE);
      }
      rc = radiusSocketReceive(radiusServerEntries,interface[i],
                               &rxStatus, packetIntf);
      if(L7_SUCCESS != rc)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "%s(): Failed to receive on socket: %d bound on Local IP: %s, closing socket\n",
                     __FUNCTION__,interface[i].socket,
                     osapiInet_ntoa(interface[i].ipAddr));

        (void)radiusNwInterfaceSocketClose(interface[i].ipAddr);

        if (packetIntf != L7_NULL)
        {
          bufferPoolFree(radius_lg_bp_id, (L7_uchar8 *)packetIntf);
        }

        if(L7_ERROR == rc )
        {
          (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
          return;
        }
      }
      else
      {
        interface[i].socketLastUsed = osapiTimeMillisecondsGet();
      }
    }
#endif

    (void)RADIUS_LOCK_GIVE(radiusTaskSyncSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_RADIUS_COMPONENT_ID, "RADIUS: Could not get the Task "
                            "Sync semaphore!. Resource issue with RADIUS Client service.\n");
  }
  return;
}

/*********************************************************************
*
* @purpose Receive the data on specified socket.
*
* @param radiusServerEntry_t *server @b((input)) Pointer to radius server structure.
* @param nwInterface_t nwSocket@b((input)) structure contains socket info.
* @param L7_uint32 interfaceSocket @b((input)) socket bound to local IPAddr.
* @param radiusPacket_t *packet @b((input)) buffer to store received data.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusSocketReceive(radiusServerEntry_t *server,
                              nwInterface_t nwSocket,
                              socketRxStatus_t *status,
                              radiusPacket_t *packet)
{
  radiusServerEntry_t *serverEntry;
  L7_uint32 pktLength;
  L7_sockaddr_in_t serverAddr;
  L7_uint32 from_len = sizeof(serverAddr);
  dnsHost_t hostAddr;

  if(L7_NULLPTR == server || L7_NULLPTR == status)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "%s(): Invalid parameters! \n",__FUNCTION__);
    return L7_FAILURE;
  }
  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): entered into the function ..",__FUNCTION__);

  *status = L7_RADIUS_RX_STS_ERROR_CODE_MAX;
  serverEntry = server;

   /*
    ** Read the RADIUS response and determine the packet length and validate the
    ** header contents.
    */
    if( osapiSocketRecvfrom(nwSocket.socket,
                           (L7_uchar8 *)packet,
                           RADIUS_MAX_MESSAGE_LENGTH,
                           L7_NULL,
                           (L7_sockaddr_t *)&serverAddr,&from_len,
                           &pktLength) != L7_SUCCESS )
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: osapiSocketRecvFrom "
              "returned error. Error while attempting to read data from the RADIUS server.\n");
      return L7_FAILURE;
    }

    serverAddr.sin_port = osapiNtohs(serverAddr.sin_port);
    serverAddr.sin_addr.s_addr = osapiNtohl(serverAddr.sin_addr.s_addr);

    if (pktLength < (L7_uint32)sizeof(radiusHeader_t))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Invalid packet length - %u."
                    " The RADIUS Client received an invalid message from the server.\n", pktLength);
      *status = L7_RADIUS_RX_STS_PKT_MALFORMED;
      return L7_FAILURE;
    }
    /* If the packet is received on a socket bound to a specific ip add.,
     *  sender add. should match with that of, one of the configured servers
     *  since it expects data from any one of them ..
     */
    if(L7_NULL != nwSocket.ipAddr)
    {
      while(serverEntry != L7_NULL)
      {
        if ((serverAddr.sin_addr.s_addr == serverEntry->serverConfig.radiusServerConfigIpAddress) &&
            (serverAddr.sin_port == (L7_ushort16) serverEntry->serverConfig.radiusServerConfigUdpPort))
        {
          break;
        }
        serverEntry = serverEntry->nextEntry;
      }
      if(L7_NULL == serverEntry)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Server address doesn't "
                        "match configured server. RADIUS Client received a server response from an "
                        "unconfigured  server.\n");
        *status = L7_RADIUS_RX_STS_SENDER_UNKNOWN;
        return L7_FAILURE;
      }
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Successfully received the pkt of %d bytes",
                     __FUNCTION__,pktLength);

    }

    /* If it is received on a server specific socket, add. of sender should
       match with that of, the specific server.*/

    else if ((serverAddr.sin_addr.s_addr != serverEntry->serverConfig.radiusServerConfigIpAddress) ||
            (serverAddr.sin_port != (L7_ushort16) serverEntry->serverConfig.radiusServerConfigUdpPort))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Server address doesn't "
                        "match configured server. RADIUS Client received a server response from an "
                        "unconfigured  server.\n");
      *status = L7_RADIUS_RX_STS_SENDER_UNKNOWN;
      return L7_FAILURE;
    }
    else
    {
      /* Hope port no. matches.. */
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Packet Rx successful(length:%d) on server specific socket %d  \
      Sender's Address matched with configured server Entry: %s, port:%d",
                     __FUNCTION__,pktLength,nwSocket.socket,
                     osapiInet_ntoa(serverEntry->serverConfig.
                     radiusServerConfigIpAddress),serverAddr.sin_port);
    }

    if (packet->header.code < RADIUS_CODE_START || packet->header.code > RADIUS_CODE_END)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
              "RADIUS: Invalid code - %u\n", packet->header.code);
      *status = L7_RADIUS_RX_STS_INVALID_MSG_CODE;
      return L7_FAILURE;
    }

    if ((osapiNtohs(packet->header.msgLength) < (L7_ushort16)sizeof(radiusHeader_t)) ||
        (osapiNtohs(packet->header.msgLength) > (L7_ushort16)pktLength))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Invalid data length - %u. "
                                 "The RADIUS Client received an invalid message from the server.\n",
                                 osapiNtohs(packet->header.msgLength));

      *status = L7_RADIUS_RX_STS_PKT_MALFORMED;
      return L7_FAILURE;
    }

    /*
    ** Make sure the allocated buffer for the response packet is big enough to
    ** accomodate the data returned from the server and the shared secret. It is
    ** this buffer that will be passed to MD5 for authenticator verification and
    ** it is required that the secret be appended to it at that time.
    */
    if (RADIUS_LG_BUF_SIZE >= (pktLength + (L7_RADIUS_MAX_SECRET+1)))
    {
      hostAddr.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
      hostAddr.host.ipAddr = serverAddr.sin_addr.s_addr;

      /* Queue up the RADIUS response for processing */
      if (radiusIssueCmd(RADIUS_RESPONSE_PACKET,
                          &hostAddr,
                          packet) == L7_SUCCESS)
      {
        /* RADIUS response has been queued */
        packet = L7_NULL;
        RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Packet Queued up for processing..",
                     __FUNCTION__);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
                "RADIUS: (): Failed to process received packet \n");
        *status = L7_RADIUS_RX_STS_ERROR_CODE_MAX;
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Buffer is too small "
                "for response processing. RADIUS Client attempted to build a response larger "
                "than resources allow.\n");
      *status = L7_RADIUS_RX_STS_ERROR_CODE_MAX;
      return L7_FAILURE;
    }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Sends a packet to the RADIUS server and increments counters.
*
* @param serverEntry @b((input)) the current Server Entry
* @param request @b((input)) the request packet to be sent
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  None
* @end
*
*********************************************************************/
L7_RC_t radiusPacketSend(radiusServerEntry_t *serverEntry,
                         radiusRequest_t *request)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 bytesSent = L7_NULL;
  L7_sockaddr_in_t saddr;
  L7_uint32 socket;

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sending radius request to %s port: %d",
                     __FUNCTION__,
                     osapiInet_ntoa(serverEntry->serverConfig.
                     radiusServerConfigIpAddress),
                     serverEntry->serverConfig.radiusServerConfigUdpPort);

  saddr.sin_family = L7_AF_INET;
  saddr.sin_port = osapiHtons( serverEntry->serverConfig.radiusServerConfigUdpPort);
  saddr.sin_addr.s_addr =
             osapiHtonl(serverEntry->serverConfig.radiusServerConfigIpAddress);

  if ((L7_NULL == request) || (L7_NULL == request->requestInfo))
  {
    return rc;
  }

  /* if the message has to be sent on socket bound to specific interface?*/
  if( L7_NULL != request->requestInfo->specificSocket)
  {
    socket = request->requestInfo->specificSocket;
  }
  else
  {
    socket = serverEntry->serverSocket;
  }

  rc = osapiSocketSendto(socket,(L7_uchar8 *)request->requestMessage,
                       osapiNtohs(request->requestMessage->header.msgLength),
                       L7_NULL, (L7_sockaddr_t *)&saddr,sizeof(saddr),
                       &bytesSent);
  /* Ensure that sendto transmited all data */
  if(L7_SUCCESS == rc && bytesSent == osapiNtohs(request->requestMessage->header.msgLength))
  {
    request->lastSentTime = osapiTimeMillisecondsGet();

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
    if(socket == request->requestInfo->specificSocket )
    {
      radiusNwInterfaceSocketTimeStampPut(socket);
    }
#endif
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Sent the packet successfully. ",
                     __FUNCTION__);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE,  L7_RADIUS_COMPONENT_ID, "RADIUS: Failed to send the request. "
                                            "A problem communicating with the RADIUS server.\n");
  }

  return rc;
}

/*********************************************************************
*
* @purpose Close the UDP (socket) connection to a RADIUS server.
*
* @param serverEntry @b((input)) the Server Entry to close
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusServerClose(radiusServerEntry_t *serverEntry, L7_uint32 index)
{
  if (serverEntry->status != RADIUS_SERVER_STATUS_DOWN)
  {
    serverEntry->status = RADIUS_SERVER_STATUS_DOWN;

    osapiSocketClose(serverEntry->serverSocket);

    if (serverEntry->serverConfig.radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
    {
      if( radiusServerAuthCurrentEntrySet(index, serverEntry->serverConfig.oldUsageType, L7_NULL) != L7_SUCCESS)
      {
        RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to flush the current Auth. server.",__FUNCTION__);
        return;
      }
    }
    else
    {
      if( radiusServerAcctCurrentEntrySet(index,L7_NULL) != L7_SUCCESS)
      {
        RADIUS_DLOG(RD_LEVEL_INFO,"RADIUS: %s() Failed to flush the current Acct. server.",__FUNCTION__);
        return;
      }
    }

    radiusPendingRequestsRelease(serverEntry, serverEntry->pendingRequests);
  }
}

#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED

/*********************************************************************
*
* @purpose Open the UDP socket on specified local ipAddress.
*
* @param L7_uint32 ipAddr @b((input)) the Server ipAddress.
* @param L7_uint32 *interfaceSocket @b((input)) Pointer to the socket.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketGet(L7_uint32 ipAddr,
                     L7_uint32 *interfaceSocket)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;
  /* some cases send ipAddr with NULL value.*/
  if( L7_NULL == ipAddr || L7_NULLPTR == interfaceSocket)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Invalid parameters \n",__FUNCTION__);
    return L7_FAILURE;
  }

  for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
  {
    if(radiusGlobalData.nwInterfaces[i].ipAddr == ipAddr)
    {
      *interfaceSocket = radiusGlobalData.nwInterfaces[i].socket;
      rc = L7_SUCCESS;
      break;
    }
  }
  if(L7_FAILURE == rc )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Could not find the socket bound on ip add: %s\n",
            __FUNCTION__,osapiInet_ntoa(ipAddr));
  }
  return rc;
}

/*********************************************************************
*
* @purpose Open the UDP socket on specified local ipAddress.
*
* @param L7_uint32 ipAddr @b((input)) the Server ipAddress.
* @param L7_uint32 *interfaceSocket @b((input)) Pointer to the socket.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketAddrGet(L7_uint32 interfaceSocket,
                     L7_uint32 *ipAddr)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if( L7_NULL == interfaceSocket || L7_NULLPTR == ipAddr)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Invalid parameters \n",__FUNCTION__);
    return L7_FAILURE;
  }

  for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
  {
    if(radiusGlobalData.nwInterfaces[i].socket == interfaceSocket)
    {
      *ipAddr  = radiusGlobalData.nwInterfaces[i].ipAddr;
      rc = L7_SUCCESS;
      break;
    }
  }
  if(L7_FAILURE == rc )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Could not find the ipAddr for specified socket : %d\n",
                     __FUNCTION__,interfaceSocket);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Put the time stamp on the specified socket.
*
* @param L7_uint32 interfaceSocket @b((input)) socket bound to local IPAddr.
*
* @returns L7_SUCCESS on success.
* @returns L7_FAILURE on failure.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketTimeStampPut(L7_uint32 interfaceSocket)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  if( L7_NULL == interfaceSocket )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %S(): Invalid parameters \n",__FUNCTION__);
    return L7_FAILURE;
  }

  for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
  {
    if(radiusGlobalData.nwInterfaces[i].socket == interfaceSocket)
    {
      radiusGlobalData.nwInterfaces[i].socketLastUsed =
                     osapiTimeMillisecondsGet();
      rc = L7_SUCCESS;
      break;
    }
  }
  if(L7_FAILURE == rc )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: radiusNwInterfaceSocketTimeStampPut(): Could not find the \
                     ipAddr for specified socket : %s\n",interfaceSocket);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Open the UDP socket connection on specified local ipAddress.
*
* @param L7_uint32 ipAddr @b((input)) the Server ipAddress.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketOpen(L7_uint32 ipAddr,
                     L7_uint32 *interfaceSocket)
{
  L7_uint32 i;
  L7_sockaddr_in_t baddr;
  L7_RC_t rc = L7_FAILURE;
  nwInterface_t *interface = &radiusGlobalData.nwInterfaces[0];

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Enetered the function..",__FUNCTION__);

  if( L7_NULL == ipAddr || L7_NULLPTR == interfaceSocket)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Invalid parameters \n",__FUNCTION__);
    return L7_FAILURE;
  }

  if( radiusNwInterfaceSocketGet(ipAddr,interfaceSocket) == L7_SUCCESS )
  {
    RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Socket already opened for ipAddr:%s with socket: %d ",
                     __FUNCTION__,osapiInet_ntoa(ipAddr),*interfaceSocket);
    return L7_ALREADY_CONFIGURED;
  }

  for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
  {
    if(L7_NULL == interface[i].ipAddr )
    {
      break;
    }
  }
  if( radiusGlobalData.totalNwInterfaces == i )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): No room for socket in data base. \n",__FUNCTION__);
    return L7_FAILURE;
  }

  /*
  ** Create a socket and the locally bound port number.
  */
  if (osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, L7_NULL,
                     interfaceSocket) == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Failed to create socket\n",__FUNCTION__);
    return L7_FAILURE;
  }

  baddr.sin_family = L7_AF_INET;
  baddr.sin_port = osapiHtons(L7_NULL);
  baddr.sin_addr.s_addr = osapiHtonl(ipAddr);

  rc = osapiSocketBind(*interfaceSocket, (L7_sockaddr_t *)&baddr,sizeof(baddr)) ;
  if(L7_SUCCESS == rc)
  {
    interface[i].ipAddr = ipAddr;
    interface[i].socket = *interfaceSocket;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS: %s(): Bind error\n",__FUNCTION__);
    osapiSocketClose(*interfaceSocket);
    return L7_FAILURE;
  }

  RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Opened and Bound socket: %d on ipAddr: %s successfully..",
                     __FUNCTION__,*interfaceSocket,osapiInet_ntoa(ipAddr));

  return rc;

}

/*********************************************************************
*
* @purpose Close the UDP socket connection on specified local ipAddress.
*
* @param L7_uint32 ipAddr @b((input)) the Server ipAddress.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusNwInterfaceSocketClose(L7_uint32 ipAddr)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;
  for(i=0;i<radiusGlobalData.totalNwInterfaces;i++)
  {
    if(radiusGlobalData.nwInterfaces[i].ipAddr == ipAddr)
    {
      RADIUS_DLOG(RD_LEVEL_INFO,"%s(): Closing the socket: %d bound on interface: %s",
                     __FUNCTION__,radiusGlobalData.nwInterfaces[i].socket,
                     osapiInet_ntoa(ipAddr));
      osapiSocketClose(radiusGlobalData.nwInterfaces[i].socket);
      memset(&radiusGlobalData.nwInterfaces[i],L7_NULL,sizeof(nwInterface_t));
      rc = L7_SUCCESS;
      break;
    }
  }
  if( L7_FAILURE == rc)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "RADIUS %s(): Could not find the socket bound on ip address: %s",
                     __FUNCTION__,osapiInet_ntoa(ipAddr));
  }
  return rc;
}
#endif

