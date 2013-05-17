/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_client_conn.c
*
* @purpose   Manage list of TCP connections that captive portal has
*            intercepted on a wired interface for use in authenticating
*            a client. Used to record the remote IP address the client
*            thinks he is talking to.
*
* @component captive portal wired interface owner
*
* @comments  Each entry in the connections list
*            represents a unique http(s) connection the client has tried to
*            initiate, and the wireless interface owner has hijacked and
*            redirected to the local authentication server. An entry on this
*            list is uniquely identified by the client's interface, IP address,
*            and TCP port. We record up to eight connections per client. A
*            client's connections are removed from the list as soon as the
*            client is authenticated. Each list entry records the IP address
*            of the remote station the client tried to contact.
*
*            We need to be able to quickly look up a connection when intercepting
*            outgoing packets to a client. Usage scenarios could vary. In some cases,
*            there could be a small number (even 1?) of clients on each wired
*            interface. In other cases, a large number of clients could be
*            concentrated on a small number of wired interfaces. To perform
*            well in either case, WIO maintains a single hash list of all
*            client connections. The hash list is indexed by
*            {intIfNum, client IP address, client TCP port}.
*            To avoid the dreaded dynamic memory allocation for entries on the
*            list, a pool of entries is allocated. The pool is shared across all
*            wired interfaces.
*
* @create    2/25/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#include "wio_util.h"
#include "log.h"
#include "buff_api.h"
#include "l7_hl_api.h"

#ifdef _L7_OS_LINUX_
#include <stddef.h>
#endif

extern wioInfo_t *wioInfo;


/*********************************************************************
* @purpose  Initialize pool of intercepted client connection entries.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
static L7_RC_t wioClientConnPoolCreate(void)
{
  L7_uint32 buffer_size = sizeof(wioClientConn_t);
  L7_uint32 num_buffers = WIO_MAX_INTERCEPTED_CONNECTIONS;

  if (bufferPoolInit(num_buffers, buffer_size, "CPWIO Connections",
                     &wioInfo->connPoolId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_CP_WIO_COMPONENT_ID,
            "Failed to allocate buffer pool for captive portal wired interface connection list.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete pool of client connection entries.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
static L7_RC_t wioClientConnPoolDelete(void)
{
   return bufferPoolTerminate(wioInfo->connPoolId);
}

/*********************************************************************
* @purpose  Create client connection list.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t wioClientConnListCreate(void)
{
  /* Create pool of connection list entries */
  if (wioClientConnPoolCreate() != L7_SUCCESS)
    return L7_FAILURE;

  if (hlInit(L7_CP_WIO_COMPONENT_ID, L7_HL_FLAGS_NONE,
             (2 * sizeof(L7_uint32)) + sizeof(L7_ushort16),
             offsetof(wioClientConn_t, intIfNum), L7_HL_LARGE_TABLE_SIZE,
             WIO_MAX_INTERCEPTED_CONNECTIONS, &wioInfo->wioClientConnList) != L7_SUCCESS)
  {
    wioClientConnPoolDelete();
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete client connection list.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t wioClientConnListDelete(void)
{
  wioClientConnPoolDelete();
  hlDestroy(wioInfo->wioClientConnList);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add an intercepted connection
*
* @param    intIfNum      @b((input))  internal interface number.
* @param    client        @b((input))  client list entry
* @param    clientTcpPort @b((input))  TCP port on client end of connection
* @param    remoteIpAddr  @b((input))  IP address of site client tried to connect to
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioClientConnAdd(L7_uint32 intIfNum, wioClient_t *client,
                         L7_ushort16 clientTcpPort, L7_uint32 remoteIpAddr)
{
  wioClientConn_t *connection;

  /* First verify that a matching entry does not already exist */
  connection = wioClientConnFind(intIfNum, client->clientIpAddr, clientTcpPort);
  if (connection)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(client->clientIpAddr, ipAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Duplicate client connection for TCP port %u on client %s on interface %s.",
            clientTcpPort, ipAddrStr, ifName);
    return L7_FAILURE;
  }

  /* Create new connection list entry */
  if (bufferPoolAllocate(wioInfo->connPoolId, (L7_uchar8**) &connection) != L7_SUCCESS)
  {
    /* just count this */
    wioInfo->debugStats.outOfConnBuffers++;
    return L7_FAILURE;
  }

  connection->intIfNum = intIfNum;
  connection->clientIpAddr = client->clientIpAddr;
  connection->clientTcpPort = clientTcpPort;
  connection->remoteIpAddr = remoteIpAddr;

  /* Insert new connection to hash list */
  if (hlInsert(wioInfo->wioClientConnList, (L7_uchar8*) &connection->intIfNum,
               (void*) connection) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 remIpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(connection->clientIpAddr, ipAddrStr);
    osapiInetNtoa(remoteIpAddr, remIpAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_CP_WIO_COMPONENT_ID,
            "Failed to record connection on interface %s from client %s:%u to %s",
            ifName, ipAddrStr, clientTcpPort, remIpAddrStr);
    bufferPoolFree(wioInfo->connPoolId, (L7_uchar8*) connection);
    return L7_FAILURE;
  }

  if (wioInfo->wioTraceFlags & WIO_TRACE_CONN_LIST)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 remIpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(connection->clientIpAddr, ipAddrStr);
    osapiInetNtoa(remoteIpAddr, remIpAddrStr);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                  "Intercepted connection on interface %s from client %s:%u to %s",
                  ifName, ipAddrStr, clientTcpPort, remIpAddrStr);
    wioTraceWrite(wioTrace);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove all intercepted connections for a given client on a given
*           interface.
*
* @param    intIfNum     @b((input))  internal interface number.
* @param    clientIpAddr @b((input))  client IP address. If 0, remove
*                                     connections for all clients on this interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioClientConnectionsRemove(L7_uint32 intIfNum, L7_uint32 clientIpAddr)
{
  wioClientConn_t *connection;
  wioClientConn_t *nextConn;

  L7_RC_t rc = hlGetFirst(wioInfo->wioClientConnList, (void**) &connection);
  while (rc == L7_SUCCESS)
  {
    rc = hlGetNext(wioInfo->wioClientConnList, (void*) connection, (void**) &nextConn);
    if ((intIfNum == connection->intIfNum) &&
        (!clientIpAddr || (connection->clientIpAddr == clientIpAddr)))
    {
      /* Remove connection */
      if (wioInfo->wioTraceFlags & WIO_TRACE_CONN_LIST)
      {
        L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 remIpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiInetNtoa(connection->clientIpAddr, ipAddrStr);
        osapiInetNtoa(connection->remoteIpAddr, remIpAddrStr);
        osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                      "Removing connection on interface %s from client %s:%u to %s",
                      ifName, ipAddrStr, connection->clientTcpPort, remIpAddrStr);
        wioTraceWrite(wioTrace);
      }

      if (hlDelete(wioInfo->wioClientConnList, (L7_uchar8*) &connection->intIfNum,
                   connection) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
               "Failed to delete client connection.");
      }
      bufferPoolFree(wioInfo->connPoolId, (L7_uchar8*) connection);
    }
    connection = nextConn;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find a specific client connection.
*
* @param    intIfNum     @b((input))  internal interface number.
* @param    clientIpAddr @b((input))  client IP address.
* @param    clientTcpPort @b((input)) TCP port number on client side
*
* @returns  connection list entry
*
* @notes
*
* @end
*********************************************************************/
wioClientConn_t *wioClientConnFind(L7_uint32 intIfNum, L7_uint32 clientIpAddr,
                                   L7_ushort16 clientTcpPort)
{
  wioClientConn_t connKey;
  wioClientConn_t *connection;

  connKey.intIfNum = intIfNum;
  connKey.clientIpAddr = clientIpAddr;
  connKey.clientTcpPort = clientTcpPort;

  if (hlFindFirst(wioInfo->wioClientConnList, (L7_uchar8*) &connKey.intIfNum,
                  (void**) &connection) == L7_SUCCESS)
    return connection;
  return NULL;
}

/*********************************************************************
* @purpose  Find the remote IP address for a given connection.
*
* @param    intIfNum     @b((input))  internal interface number.
* @param    clientIpAddr @b((input))  client IP address.
* @param    clientTcpPort @b((input)) TCP port number on client side
*
* @returns  IP address of remote end of connection, or 0 if no matching connection
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 wioConnRemoteAddrGet(L7_uint32 intIfNum, L7_uint32 clientIpAddr,
                               L7_ushort16 clientTcpPort)
{
  wioClientConn_t *connection = wioClientConnFind(intIfNum, clientIpAddr,
                                                  clientTcpPort);
  if (!connection)
    return 0;
  return connection->remoteIpAddr;
}

/*********************************************************************
* @purpose  Print the connection list for debugging purposes
*
* @param    intIfNum     @b((input))  internal interface number.
*                                     If 0, print all.
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void _wioConnListShow(L7_uint32 intIfNum)
{
  wioClientConn_t *connection;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 remIpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_RC_t rc;

  printf("\n%20s %24s %18s", "Interface", "Client Address", "Remote Address");

  rc = hlGetFirst(wioInfo->wioClientConnList, (void**) &connection);
  while (rc == L7_SUCCESS)
  {
    nimGetIntfName(connection->intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(connection->clientIpAddr, ipAddrStr);
    osapiInetNtoa(connection->remoteIpAddr, remIpAddrStr);

    printf("\n%20s %18s:%u %18s",
           ifName, ipAddrStr, connection->clientTcpPort, remIpAddrStr);
    rc = hlGetNext(wioInfo->wioClientConnList, connection, (void**) &connection);
  }
}
