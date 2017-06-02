/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename dns_client_txrx.c
*
* @purpose DNS client transmit and receive functions
*
* @component DNS client
*
* @comments none
*
* @create 03/07/05
*
* @author dfowler
*
* @end
*
**********************************************************************/
#include "l7_socket.h"
#include "l7_common.h"
#include "osapi_support.h"
#include "dns_client.h"
#include "dns_client_packet.h"
#include "dns_client_util.h"
#include "dns_client_txrx.h"


extern void                     *dnsQueue;
extern dnsOprData_t             *dnsOprData;
extern void                     *dnsSemaphore;

/*********************************************************************
*
* @purpose Get active entry for a name server.
*
* @param L7_inet_addr_t         serverAddr @b((input))  (v4 & v6) address
* @param dnsNameServerEntry_t **server     @b((output)) active server entry
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
static L7_RC_t dnsNameServerActiveEntryGet(L7_inet_addr_t *serverAddr,
                                           dnsNameServerEntry_t **server)
{
  L7_uint32 i = 0;

  for (i = 0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if ((dnsClientIsInetAddrFamilyNotSet(&dnsOprData->serverTbl[i].serverAddr)
         != L7_TRUE)&&
        (L7_INET_IS_ADDR_EQUAL(&dnsOprData->serverTbl[i].serverAddr,
                               serverAddr) == L7_TRUE))
    {
      *server = &dnsOprData->serverTbl[i];
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Process server removed from configuration
*
* @param L7_inet_addr_t    serverAddr @b((input))  (v4 & v6) address
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dnsNameServerActiveEntryRemove(L7_inet_addr_t *serverAddr)
{
  dnsNameServerEntry_t *entry = L7_NULLPTR;
  L7_uint32 i = 0;

  /* clean up pending requests and the active data for this server */
  if (dnsNameServerActiveEntryGet(serverAddr, &entry) == L7_SUCCESS)
  {
    if (entry->requests > 0)
    {
      for (i = 0; i < L7_DNS_CONCURRENT_REQUESTS; i++)
      {
        if ((dnsClientIsInetAddrFamilyNotSet(&dnsOprData->requestTbl[i].
            server) != L7_TRUE)
            && (L7_INET_IS_ADDR_EQUAL(&dnsOprData->requestTbl[i].server,
                                      serverAddr) == L7_TRUE))
        {
          dnsRequestQueryNextServer(&dnsOprData->requestTbl[i],
                                    DNS_LOOKUP_STATUS_NO_SERVER);
        }
      }
      dnsNameServerClose(entry);
    }
    memset(entry, 0, sizeof(dnsNameServerEntry_t));
  }
  return;
}

/*********************************************************************
* @purpose  Decrement pending request for a server.
*
* @param    L7_inet_addr_t   serverAddr @b((input)) server
*                                                   (v4 or v6) address.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dnsNameServerRequestRemove(L7_inet_addr_t *serverAddr)
{
  dnsNameServerEntry_t *entry = L7_NULLPTR;

  if (dnsNameServerActiveEntryGet(serverAddr, &entry) == L7_SUCCESS)
  {
    if (entry->requests > 0)
    {
      entry->requests--;
    }
    if (entry->requests == 0)
    {
      dnsNameServerClose(entry);
    }
  }
  return;
}

/*********************************************************************
*
* @purpose Open UDP socket for a name server.
*
* @param L7_uint32            serverAddr @b((input))  server address
* @param dnsNameServerEntry_t **server   @b((output)) active server entry
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
static L7_RC_t dnsNameServerOpen(L7_inet_addr_t *serverAddr,
                                 dnsNameServerEntry_t **server)
{
  dnsNameServerEntry_t *entry = L7_NULLPTR;
  L7_uint32 i = 0;
  L7_sockaddr_union_t addr;
  L7_uint32      opt = 1;
  L7_uint32      broadcastEnable = (L7_INET_IS_ADDR_BROADCAST(&serverAddr))?1:0;
  L7_uchar8      family = 0;

  /* Get the family from server address */
  family = L7_INET_GET_FAMILY(serverAddr);

  /* find or create the active entry for this server, config entries
     are reordered on add/remove, so we do not use the config index */
  for (i = 0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if ((dnsClientIsInetAddrFamilyNotSet(&dnsOprData->serverTbl[i].
        serverAddr) != L7_TRUE)&&
        (L7_INET_IS_ADDR_EQUAL(&dnsOprData->serverTbl[i].serverAddr,
                               serverAddr) == L7_TRUE))
    {
      entry = &dnsOprData->serverTbl[i];
      break;
    }
    if ((entry == L7_NULLPTR) &&
       (dnsClientIsInetAddressZero(&dnsOprData->serverTbl[i].
                                   serverAddr) == L7_TRUE))
    {
      entry = &dnsOprData->serverTbl[i];
    }
  }

  if (entry == L7_NULLPTR)
  {
    /* this would only happen if we forgot to clean up a server entry */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "DNS Client: Bad active server configuration\n");
    return L7_FAILURE;
  }
  else
  {
    inetCopy(&entry->serverAddr, serverAddr);
  }
  if (entry->requests == 0)
  {
    /* create a socket for this server entry */
    if (osapiSocketCreate(family,
                          L7_SOCK_DGRAM,
                          L7_NULL,
                          &entry->socket) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
              "DNS Client: Failed to create socket\n");
      return L7_FAILURE;
    }

    if (osapiSetsockopt(entry->socket, L7_SOL_SOCKET, L7_SO_REUSEADDR,
                       (L7_uchar8 *)&opt, sizeof(opt)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DNS_CLIENT_COMPONENT_ID, "dnsNameServerOpen(): osapiSetsockopt SO_REUSEADDR failed errorno %d\n",
                   osapiErrnoGet());
      return L7_FAILURE;
    }

    if (family == L7_AF_INET)
    {
      /* Supporting broadcast reqests */
      if (osapiSetsockopt(entry->socket,
                          L7_SOL_SOCKET,
                          L7_SO_BROADCAST,
                          (L7_uchar8 *)&broadcastEnable,
                          sizeof(broadcastEnable) ) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: Set socket SO_BROADCAST option error");
        osapiSocketClose(entry->socket);
        return L7_FAILURE;
      }

      if (osapiSetsockopt(entry->socket, L7_SOL_SOCKET, L7_SO_BROADCAST, (L7_uchar8 *)&opt, sizeof(opt))!= L7_SUCCESS)
      {
         L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DNS_CLIENT_COMPONENT_ID, "dnsNameServerOpen(): osapiSetsockopt SO_BROADCAST failed\n");
      }
      addr.u.sa4.sin_family = L7_AF_INET;
      addr.u.sa4.sin_port = osapiHtons(DNS_UDP_PORT);
      addr.u.sa4.sin_addr.s_addr = L7_INADDR_ANY;
      if (osapiSocketBind(entry->socket,(L7_sockaddr_t *)&addr.u.sa4,sizeof(addr.u.sa4)) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: Bind error on addr 0x%X\n", serverAddr);
        osapiSocketClose(entry->socket);
        return L7_FAILURE;
      }
    }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
    else if (family == L7_AF_INET6)
    {
      /* bind IN6ADDR_ANY */
      memset(&addr.u.sa6, 0, sizeof(addr.u.sa6));
      addr.u.sa6.sin6_family = L7_AF_INET6;
      addr.u.sa6.sin6_port = osapiHtons(DNS_UDP_PORT);
      if (osapiSocketBind(entry->socket,(L7_sockaddr_t *)&addr.u.sa6,sizeof(addr)) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: Bind error on addr 0x%X\n", serverAddr);
        osapiSocketClose(entry->socket);
        return L7_FAILURE;
      }
    }
#endif
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
              "DNS Client: Invalid Family");
      osapiSocketClose(entry->socket);
      return L7_FAILURE;
    }
  }

  *server = entry;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Determine if any of the DNS sockets are ready to read
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dnsNameServerPacketReceive(void)
{
  L7_int32    status = 0;
  L7_uint32   length = 0;
  L7_uint32   i = 0;
  L7_uchar8   packet[DNS_UDP_MSG_SIZE_MAX];
  fd_set      sockets;
  L7_uint32   maxfd = 0;
  L7_BOOL     requests = L7_FALSE;
  struct timeval timeout;
  L7_sockaddr_in_t rx_addr;
  L7_int32         addrlen;
  L7_uchar8   srvAddr[DNS_INET_ADDR_LEN];

  /* set all the fds for servers we have sent requests to... */
  FD_ZERO(&sockets);
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  for (i = 0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
  {
    if (dnsOprData->serverTbl[i].requests != 0)
    {
      requests = L7_TRUE;
      FD_SET(dnsOprData->serverTbl[i].socket, &sockets);
      if (dnsOprData->serverTbl[i].socket > maxfd)
      {
        maxfd = dnsOprData->serverTbl[i].socket;
      }
    }
  }
  osapiSemaGive(dnsSemaphore);
  if (requests == L7_FALSE)
  {
    /* no pending requests */
    return L7_FAILURE;
  }

  /* wait until a packet is received or a timeout */
  /*
  numFds = osapiSelect(maxfd + 1,
                       &sockets,
                       L7_NULL,
                       L7_NULL,
                       DNS_SERVER_SOCKET_TIMEOUT,
                       L7_NULL); */

  timeout.tv_sec  = DNS_SERVER_SOCKET_TIMEOUT;
  timeout.tv_usec = L7_NULL;
  status = select(maxfd + 1,
                  (fd_set *)&sockets,
                  L7_NULL,
                  L7_NULL,
                  &timeout);


  if (status > 0)
  {
    /* there are file descriptors to read */
    osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
    for (i = 0; i < L7_DNS_NAME_SERVER_ENTRIES; i++)
    {
      dnsMgmtMsg_t        msg;
      dnsRequestEntry_t  *request = L7_NULLPTR;
      dnsMessageHeader_t *header = L7_NULLPTR;
      L7_ushort16 id = 0;
      L7_uint32   j = 0;

      /* we close the socket whenever pending requests for a
         server goes to zero, if a request has timed out or
         a server removed since we set up the select we do
         not want to try and read from the socket  */
      if ((dnsOprData->serverTbl[i].requests == 0) ||
          (!FD_ISSET(dnsOprData->serverTbl[i].socket, &sockets)))
      {
        continue;
      }

      /* read packet, determine length */
      memset(packet, 0, DNS_UDP_MSG_SIZE_MAX);
      addrlen = sizeof(rx_addr);
      if (osapiSocketRecvfrom(dnsOprData->serverTbl[i].socket,
                              (L7_uchar8 *)packet,
                              DNS_UDP_MSG_SIZE_MAX,
                              L7_NULL,
                              ( L7_sockaddr_t *)&rx_addr, &addrlen, &length) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: osapiSocketRecvFrom returned error for addr 0x%X."
                " Indicates there is a stack error in receiving the DNS response"
                " packet from the server.",
                inetAddrPrint(&dnsOprData->serverTbl[i].serverAddr, srvAddr));
        continue;
      }

      if (length < sizeof(dnsMessageHeader_t))
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: received invalid packet length - %u\n", length);
        continue;
      }

      /* verify this is a response packet */
      header = (dnsMessageHeader_t *)packet;
      if ((header->bits & DNS_MSG_HDR_QR_BIT_MASK) == 0)
      {
        /* received invalid packet, not a response  */
       /*
         * logging this message is causing the flood on the console when no name servers are configured/
         * when trying to lookup for the unqualified domain name used for an application say syslog server.
         * In this case the destination name server ip will be is broadcast address, 
         * It seems on interpeak whatever we are sending out are also looped back to us. 
         * Hence RecvFrom is receiving the sending packtes. Hence we are not logging this.
         * 
        */
        continue;
      }

      /* verify qdcount, all of our queries are one record */
      if (header->qdcount != osapiHtons(1))
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: received invalid response packet\n");
        dnsOprData->counters.unparsed++;
        continue;
      }

      /* try to match against a pending request */
      id = osapiNtohs(header->id);
      for (j = 0; j < L7_DNS_CONCURRENT_REQUESTS; j++)
      {
        if (id == dnsOprData->requestTbl[j].id)
        {
          request = &dnsOprData->requestTbl[j];
          if ((dnsClientIsInetAddrFamilyNotSet(&dnsOprData->serverTbl[i].
                                  serverAddr) != L7_TRUE) &&
              (L7_INET_IS_ADDR_EQUAL(&dnsOprData->serverTbl[i].serverAddr,
                                     &request->server) == L7_TRUE))
          {
            break;
          } else
          {
            /* silently discard this request, we may hit this
               condition if one request times out, we are waiting
               on other requests for the same server, and we
               get a delayed response for the matching request */
            dnsOprData->counters.martians++;
            continue;
          }
        }
      }

      if (request == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: received response for invalid request id %u\n", id);
        dnsOprData->counters.martians++;
        continue;
      }
      if (request->family == L7_AF_INET)
      {
        /* responses to v4 queries */
        dnsOprData->counters.v4responses++;
      }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
      else
      {
        /* responses to v6 queries */
        dnsOprData->counters.v6responses++;
      }
#endif
      /* verify question section matches what we sent */
      if ((length < request->length) ||
          (memcmp(request->packet + sizeof(dnsMessageHeader_t),
                  packet + sizeof(dnsMessageHeader_t),
                  request->length - sizeof(dnsMessageHeader_t)) != 0))
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: received response for request id %u with mismatched question\n", id);
        dnsOprData->counters.unparsed++;
        continue;
      }

      /* copy response to the request and process on the DNS client task  */
      if (dnsOprData->serverTbl[i].requests > 0)
      {
        dnsOprData->serverTbl[i].requests--;
      }
      if (dnsOprData->serverTbl[i].requests == 0)
      {
        dnsNameServerClose(&dnsOprData->serverTbl[i]);
      }
      request->queryTimer = 0;
      memset(request->packet, 0, DNS_UDP_MSG_SIZE_MAX);
      memcpy(request->packet, packet, length);
      request->length = length;
      msg.msgId = dnsMsgResponse;
      msg.u.request = request;
      dnsResponsePacketTrace(&request->server, packet);
      if (osapiMessageSend(dnsQueue,
                           &msg,
                           DNS_CLIENT_MSG_SIZE,
                           L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
                "DNS Client: failed to send response message for request id %u\n", id);
        continue;
      }
    }
    osapiSemaGive(dnsSemaphore);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Sends a standard query packet to the DNS name server.
*
* @param L7_inet_addr_t serverAddr @b((input))  server v4 or v6 address
* @param L7_uchar8      *query     @b((input))  query packet
* @param L7_uint32      length     @b((output)) packet length
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dnsNameServerPacketSend(L7_inet_addr_t  *serverAddr,
                                L7_uchar8       *packet,
                                L7_ushort16     length)
{
  dnsNameServerEntry_t *server = L7_NULLPTR;
  L7_uint32 bytes = 0;
  L7_uint32 serverIp = 0;
  L7_sockaddr_union_t saddr;
  L7_inet_addr_t networkSrvAddr;
  L7_uchar8      family = 0;

  /* Get the family (ipv4 or ipv6) from server address */
  family = L7_INET_GET_FAMILY(serverAddr);

  inetAddressZeroSet(family, &networkSrvAddr);
  /* keep send and receive synchronized */
  osapiSemaTake(dnsSemaphore, L7_WAIT_FOREVER);
  /* open socket for current server if we haven't already */
  if (dnsNameServerOpen(serverAddr, &server) != L7_SUCCESS)
  {
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }


  if (family == L7_AF_INET)
  {
    /* Getting ip address from server address*/
    inetAddressGet(family, serverAddr, &serverIp);
    saddr.u.sa4.sin_family = L7_AF_INET;
    saddr.u.sa4.sin_port = osapiHtons(DNS_UDP_PORT);
    saddr.u.sa4.sin_addr.s_addr = osapiHtonl(serverIp);
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if (family == L7_AF_INET6)
  {
    saddr.u.sa6.sin6_family = L7_AF_INET6;
    saddr.u.sa6.sin6_port = osapiHtons(DNS_UDP_PORT);
    memcpy (&(saddr.u.sa6.sin6_addr), &(serverAddr->addr.ipv6), sizeof(serverAddr->addr.ipv6));
  }
#endif
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "\r\nInvalid Family");
    osapiSocketClose(server->socket);
    osapiSemaGive(dnsSemaphore);
  }

  if (inetAddrHton(&server->serverAddr, &networkSrvAddr) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "\r\nUnable to convert host to network order");
    osapiSocketClose(server->socket);
    osapiSemaGive(dnsSemaphore);
    return L7_FAILURE;
  }
  dnsRequestPacketTrace(&networkSrvAddr, packet);
  if (family == L7_AF_INET)
  {
    if (osapiSocketSendto(server->socket,
                          packet,
                          length,
                          L7_NULL,
                          (L7_sockaddr_t *)&saddr.u.sa4,sizeof(saddr.u.sa4),
                          &bytes) != L7_SUCCESS)
    {
      osapiSocketClose(server->socket);
      osapiSemaGive(dnsSemaphore);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DNS_CLIENT_COMPONENT_ID,
              "DNS Client: Failed to send query packet."
              " Indicates there is a stack error in sending"
              " the DNS query packet to the server.");
      return L7_FAILURE;
    }
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else if (family == L7_AF_INET6)
  {
    if (osapiSocketSendto(server->socket,
                          packet,
                          length,
                          L7_NULL,
                          (L7_sockaddr_t *)&saddr.u.sa6,sizeof(saddr),
                          &bytes) != L7_SUCCESS)
    {
      osapiSocketClose(server->socket);
      osapiSemaGive(dnsSemaphore);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DNS_CLIENT_COMPONENT_ID,
              "DNS Client: Failed to send query packet."
              " Indicates there is a stack error in sending"
              " the DNS query packet to the server.");
      return L7_FAILURE;
    }
  }
#endif
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "\r\nInvalid Family");
    osapiSocketClose(server->socket);
    osapiSemaGive(dnsSemaphore);
  }

  /* ensure that sendto transmited all data */
  if (bytes != length)
  {
    osapiSemaGive(dnsSemaphore);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DNS_CLIENT_COMPONENT_ID,
            "DNS Client: Failed to send all of the query\n");
    return L7_FAILURE;
  }
  server->requests++;
  if (family == L7_AF_INET)
  {
    dnsOprData->counters.v4queries++;
  }
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  else
  {
    dnsOprData->counters.v6queries++;
  }
#endif
  osapiSemaGive(dnsSemaphore);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Close the UDP (socket) to a DNS name server.
*
* @param dnsNameServerEntry_t *entry @b((input))  active server entry
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void dnsNameServerClose(dnsNameServerEntry_t *entry)
{
  osapiSocketClose(entry->socket);
  return;
}

