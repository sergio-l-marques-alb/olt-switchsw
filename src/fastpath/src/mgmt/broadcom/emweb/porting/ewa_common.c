/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename  ewa_common.c
 *
 * @purpose   Provide common emweb porting functions
 *
 * @component emweb
 *
 * @comments
 *
 * @create    06/07/2009
 *
 * @author    Rama Sasthri, Kristipati
 *
 * @end
 *
 **********************************************************************/

#include "osapi_support.h"
#include "posix_config.h"
#include "ew_config.h"
#include "ewnet.h"
#include "ews_ctxt.h"

#include "ewa_common.h"

int use_connection_socket_select = 1;
int connection_socket_select_timeout_counter = 0;
int connection_socket_send_error_counter = 0;

void ewaSocketBufferSet (int sock, int recv, int send, char *which)
{
#if defined (SO_SNDBUF) || defined (SO_RCVBUF)
  errno = 0;
  if ((setsockopt (sock, SOL_SOCKET, SO_RCVBUF, (char *) &recv, (int) sizeof (recv))) < 0)
  {
    EWA_LOGWARN ("%s setsockopt SO_RCVBUF failed: %s\n", which, strerror (errno));
  }

  errno = 0;
  if ((setsockopt (sock, SOL_SOCKET, SO_SNDBUF, (char *) &send, (int) sizeof (send))) < 0)
  {
    EWA_LOGWARN ("%s setsockopt SO_SNDBUF failed: %s\n", which, strerror (errno));
  }
#endif
}

/* allow address reuse allows the server to be stopped and restarted? */
void ewaSocketReUseAddrSet (int sock, int val, char *which)
{
  errno = 0;
  if ((setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof (val))) < 0)
  {
    EWA_LOGWARN ("%s setsockopt REUSEADDR failed: %s\n", which, strerror (errno));
  }
}

void ewaSocketLingerSet (int sock, int onoff, int val, char *which)
{
  struct linger linger;

  errno = 0;
  linger.l_onoff = onoff;
  linger.l_linger = val;
  if ((setsockopt (sock, SOL_SOCKET, SO_LINGER, (void *) &linger, sizeof (linger))) < 0)
  {
    EWA_LOGWARN ("%s setsockopt LINGER failed %s\n", which, strerror (errno));
  }
}

void ewaSocketKeepAliveSet (int sock, int val, char *which)
{
  errno = 0;
  if ((setsockopt (sock, SOL_SOCKET, SO_KEEPALIVE, (char *) &val, sizeof (val))) < 0)
  {
    EWA_LOGWARN ("%s setsockopt KEEPALIVE failed: %s\n", which, strerror (errno));
  }
}

void ewaSocketSend (int sock, char *dataPtr, int bytes_to_be_sent)
{
  int bytes = 0;
  int retryCount = 0;
  int bytesSent = 0;
  while (bytes < bytes_to_be_sent && retryCount < 100)
  {
    int osapiSelectRC;
    fd_set writefds;            /* fds ready to write */
    FD_ZERO (&writefds);
    FD_SET (sock, &writefds);
    osapiSocketNonBlockingModeSet (sock, L7_FALSE);
    osapiSelectRC = osapiSelect (sock + 1, NULL, &writefds, NULL, 0, 20000);
    osapiSocketNonBlockingModeSet (sock, L7_TRUE);
    if (osapiSelectRC > 0)
    {
      bytesSent = send (sock, dataPtr + bytes, bytes_to_be_sent - bytes, 0);
      if (bytesSent >= 0)
      {
        bytes += bytesSent;
      }
      else if (errno == EWOULDBLOCK || errno == EAGAIN)
      {
        connection_socket_send_error_counter++;
        retryCount++;
        osapiSleepMSec (100);
      }
      else if (errno == ECONNRESET)
      {
        EWA_LOGDBG ("send %d/%d failed [%d]: %s\n", bytes, bytes_to_be_sent, retryCount,
                    strerror (errno));
        break;
      }
      else
      {
        /* error occured with send */
        EWA_LOGWARN ("send %d/%d failed [%d]: %s\n", bytes, bytes_to_be_sent, retryCount,
                     strerror (errno));
        bytes = bytes_to_be_sent;
        break;
      }
    }
    else if (osapiSelectRC == 0)
    {
      connection_socket_select_timeout_counter++;
      retryCount++;
    }
    else
    {
      /* error occured with select */
      EWA_LOGWARN ("select %d/%d failed [%d]: %s\n", bytes, bytes_to_be_sent, retryCount,
                   strerror (errno));
      bytes = bytes_to_be_sent;
      break;
    }
  }
  use_connection_socket_select = (retryCount >= 100) ? 0 : use_connection_socket_select;
  bytes = bytes_to_be_sent;     /* either completed or error, nothing to send */
}

int use_connection_socket_select_toggle (void)
{
  use_connection_socket_select = 1 - use_connection_socket_select;
  printf ("connection_socket_select_timeout_counter = %d\n",
          connection_socket_select_timeout_counter);
  printf ("connection_socket_send_error_counter = %d\n", connection_socket_send_error_counter);
  return use_connection_socket_select;
}

void ewaNetHandleSpecialDiscardObjectAllocate(void)
{
  if(ewaNetHandleSpecialDiscardObject == NULL)
  {
    ewaNetHandleSpecialDiscardObject = (struct EwaNetHandle_s *) ewaAlloc(sizeof (*ewaNetHandleSpecialDiscardObject));
  }
}

L7_RC_t ewaNetClientInfoGet(EwsContext context, L7_inet_addr_t *pRemote, L7_uint32 *pPort)
{
  EwaNetHandle handle = ewsContextNetHandle(context);

  *pPort = 0;
  memset(pRemote,0,sizeof(L7_inet_addr_t));

  if (handle->peer.u.sa.sa_family == AF_INET6)
  {
    if(L7_IP6_IS_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr))
    {
      pRemote->family = L7_AF_INET;
      pRemote->addr.ipv4.s_addr = L7_IP6_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr);
      *pPort = handle->listeningPort;
    }
    else
    {
      pRemote->family = L7_AF_INET6;
      pRemote->addr.ipv6 = *(L7_in6_addr_t *)&handle->peer.u.sa6.sin6_addr;
    }
  }
  else
  {
    pRemote->family = L7_AF_INET;
    pRemote->addr.ipv4.s_addr = osapiNtohl(handle->peer.u.sa4.sin_addr.s_addr);
    *pPort = handle->listeningPort;
  }
  return L7_SUCCESS;
}

