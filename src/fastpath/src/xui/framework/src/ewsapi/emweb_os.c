
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename emweb_os.c
*
* @purpose
*
* @component EMWEB
*
* @comments
*
* @create 04/17/2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "fastpath_inc.h"

#ifndef L7_OLDCLIWEB_PACKAGE

#include "ew_types.h"
#include "ewnet.h"

#if defined(_L7_OS_LINUX_) || defined(_L7_OS_ECOS_)
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <termios.h>
int ewaStdInConfig (void)
{
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  struct termios tio;

  tcgetattr (0, &tio);

  tio.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL);
  tio.c_lflag &= ~(ECHO | ECHONL | ICANON);
#ifndef _L7_OS_ECOS_
  tio.c_cc[VSUSP] = _POSIX_VDISABLE;
  /*
     Defined in man pages but not in .h file
     tio.c_cc[VDSUSP] = _POSIX_VDISABLE;
   */
#endif
  tcflush (0, TCIFLUSH);
  tcsetattr (0, TCSANOW, &tio);
#endif

  setvbuf (stdin, 0, _IONBF, BUFSIZ);
  setvbuf (stdout, 0, _IONBF, BUFSIZ);

  return 0;
}
unsigned long ewaGetHostByname (char *host)
{
  unsigned long tmp;
  struct hostent *addr;
  if ((addr = gethostbyname (host)) == NULL)
  {
    return 0;
  }
  memcpy ((char *) &tmp, addr->h_addr, addr->h_length);
  return tmp;
}
#else

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <dosFsLib.h>
#include <sysLib.h>
#include <string.h>
#include <time.h>
#include <stat.h>
#include <memLib.h>
#include <ramDrv.h>
#include <lstLib.h>
#include <intLib.h>
#include <semLib.h>
#include <time.h>
#include <rebootLib.h>
#include <netBufLib.h>
#include <errnoLib.h>
#include <usrLib.h>
#include <usrConfig.h>
#include <a_out.h>
#include <ioLib.h>
1#include "sslt_exports.h"

int ewaStdInConfig (void)
{
  int tmp;

  tmp = ioctl (STD_IN, FIOGETOPTIONS, 0);
  tmp &= ~OPT_LINE;
  tmp &= ~OPT_ECHO;
  tmp &= ~OPT_MON_TRAP;

  if (ioctl (STD_IN, FIOSETOPTIONS, tmp) == ERROR)
  {
    return -1;
  }

  setvbuf (stdin, 0, _IONBF, BUFSIZ);
  setvbuf (stdout, 0, _IONBF, BUFSIZ);

  return STD_IN;
}

unsigned long ewaGetHostByname (char *host)
{
  int addr;
  if ((addr = hostGetByName (host)) == ERROR)
  {
    return 0;
  }
  return addr;
}
#endif

void ewaRecordPeerAddress (ew_sockaddr_union_t * peer, L7_inet_addr_t * remote)
{
#if 0
  if (peerp->u.sa.sa_family == AF_INET6)
  {
    if (L7_IP6_IS_ADDR_V4MAPPED (&peerp->u.sa6.sin6_addr))
    {
      remote.family = L7_AF_INET;
      remote.addr.ipv4.s_addr = peerp->u.sa6.sin6_addr.in6_u.u6_addr32[3];
      peerp->u.sa4.sin_addr.s_addr = peerp->u.sa6.sin6_addr.in6_u.u6_addr32[3];
    }
    else
    {
      remote.family = L7_AF_INET6;
      remote.addr.ipv6 = *(L7_in6_addr_t *) & peerp->u.sa6.sin6_addr;
    }
  }
  else
#endif
  {
    remote->family = L7_AF_INET;
    remote->addr.ipv4.s_addr = peer->u.sa4.sin_addr.s_addr;
  }
}

int ewaRecordLocalAddress (EwaNetHandle connection, ew_sockaddr_union_t * peerIn)
{
  L7_uint32 peerAddress;
  L7_uint32 numBytes = 0;
  ew_sockaddr_union_t localAddr;
  L7_uint32 addrlen = sizeof (ew_sockaddr_union_t);
  ew_sockaddr_union_t peer2;
  ew_sockaddr_union_t sslhost;
  ew_sockaddr_union_t *peerp = (ew_sockaddr_union_t *)peerIn;

  /*
   * Get the text form of the IP address for ewaNetLocalHostName
   * For each connection it is necessary to get the IP address
   * of the local side of the connection, so that when submit
   * pages are responded to, the switch will have the proper IP
   * address to use in the response.
   */
  addrlen = sizeof (localAddr);
  if ((getsockname (connection->socket, (struct sockaddr *) &localAddr, &addrlen)))
  {
    connection->localName[0] = '\000';
    EMWEB_WARN (("Can't read local socket address\n"));
  }
  else
  {
    peerAddress = osapiHtonl (peerp->u.sa4.sin_addr.s_addr);
    if (peerAddress == L7_SSLT_UNSECURE_SERVER_ADDR)
    {
      numBytes = read(connection->socket, (L7_char8 *)&peer2, sizeof (ew_sockaddr_union_t));
      if (numBytes < 0)
      {
        return 1;
      }
      memcpy (&connection->peer, &peer2, sizeof (ew_sockaddr_union_t));

      numBytes = read(connection->socket, (L7_char8 *)&sslhost, sizeof (ew_sockaddr_union_t));
      if (numBytes < 0)
      {
        return 1;
      }
      memcpy (&localAddr, &sslhost, sizeof (ew_sockaddr_union_t));
      connection->connection_type = EW_CONNECTION_HTTPS;
    }
    else
    {
      memcpy (&connection->peer, peerp, sizeof (ew_sockaddr_union_t));
      connection->connection_type = EW_CONNECTION_HTTP;
    }

#if 0                           /* defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE) */

    if (localAddr.u.sa.sa_family == AF_INET6)
    {
      inet_ntop (localAddr.u.sa.sa_family,
                 (void *) &((struct sockaddr_in6 *) &localAddr)->sin6_addr, connection->localName,
                 LOCAL_NAME_MAX);
      if ((peerp->u.sa.sa_family == AF_INET6)
          && (L7_IP6_IS_ADDR_V4MAPPED (&peerp->u.sa6.sin6_addr)))
      {
        rem_v4_v6_map = strrchr (connection->localName, ':');
        strcpy (buf, ++rem_v4_v6_map);
        memset (connection->localName, 0, sizeof (connection->localName));
        strcpy (connection->localName, buf);
      }
    }
    else
    {
      inet_ntop (localAddr.u.sa.sa_family, (void *) &((struct sockaddr_in *) &localAddr)->sin_addr,
                 connection->localName, LOCAL_NAME_MAX);
    }
#else
    osapiInetNtop (L7_AF_INET, (void *) &((struct sockaddr_in *) &localAddr)->sin_addr,
                   (unsigned char *)connection->localName, LOCAL_NAME_MAX);
#endif
  }

  return 0;

}

#if 0
void ewaSetSockNonBlockingMode (int sock)
{
#if (L7_SERIAL_COM_ATTR)
  struct termios tio;

  tcgetattr (connection->socket, &tio);

  tio.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL);
  tio.c_lflag &= ~(ECHO | ECHONL | ICANON);
  tio.c_cc[VSUSP] = _POSIX_VDISABLE;
  /*
     Defined in man pages but not in .h file
     tio.c_cc[VDSUSP] = _POSIX_VDISABLE;
   */

  tcflush (connection->socket, TCIFLUSH);
  tcsetattr (connection->socket, TCSANOW, &tio);
#endif
  osapiSocketNonBlockingModeSet (connection->socket, L7_TRUE);
}
#endif



void *ewaAlloc (uintf bytes)
{
  /* return calloc (1, bytes); */
  return osapiMalloc ( L7_CLI_WEB_COMPONENT_ID, bytes );  /* LVL7_P0006 */
}

void ewaFree (void *ptr)
{
  /*free (ptr);*/
  osapiFree ( L7_CLI_WEB_COMPONENT_ID, ptr ) ;  /* LVL7_P0006 */
}

void *ebAlloc (int size)
{
  /* return calloc (1, size); */
  return osapiMalloc ( L7_CLI_WEB_COMPONENT_ID, size );  /* LVL7_P0006 */
}

void ebFree (void *ptr)
{
  /*free (ptr);*/
  osapiFree ( L7_CLI_WEB_COMPONENT_ID, ptr ) ;  /* LVL7_P0006 */
}

int emwebSockServer (int ipaddr, int port, int v6, int retry, int max)
{
  const char *func = "emwebSockServer";
  struct sockaddr_in addr4;
  struct sockaddr_in6 addr6;
  void *addr;
  int addrLen;
  struct linger linger;

  int one = 1;                  /* used to set socket options */
  int err = 1;
  int sock;

#if defined(SO_SNDBUF) || defined(SO_RCVBUF)
  int rcvBufLen = 4096;
  int sndBufLen = 4096;
#endif

  do
  {
    /* Create socket */
    ClearErrno ();
    sock = socket (v6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
    if (SOCKET_IS_INVALID (sock))
    {
      EMWEB_ERROR (("%s: socket() call failed: %s\n", func, strerror (errno)));
      break;
    }

    /* allowing address reuse allows the server to be stopped and restarted */
    ClearErrno ();
    if ((setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof (one))) < 0)
    {
      EMWEB_ERROR (("%s: setsockopt REUSEADDR failed: %s\n", func, strerror (errno)));
      break;
    }

#if defined(SO_SNDBUF) || defined(SO_RCVBUF)
    ClearErrno ();
    if ((setsockopt (sock, SOL_SOCKET, SO_RCVBUF, (char *) &rcvBufLen, (int) sizeof (rcvBufLen)))
        < 0)
    {
      EMWEB_ERROR (("http setsockopt SO_RCVBUF failed: %s\n", strerror (errno)));
    }

    ClearErrno ();
    if ((setsockopt (sock, SOL_SOCKET, SO_SNDBUF, (char *) &sndBufLen, (int) sizeof (sndBufLen)))
        < 0)
    {
      EMWEB_ERROR (("http setsockopt SO_SNDBUF failed: %s\n", strerror (errno)));
    }

#endif

    /* finish sending pending data before closing tcp connection */
    if (v6 == 0)
    {
      ClearErrno ();
      linger.l_onoff = 1;       /* enable linger on close */
      linger.l_linger = 0;
      if ((setsockopt (sock, SOL_SOCKET, SO_LINGER, (void *) &linger, sizeof (linger))) < 0)
      {
        EMWEB_ERROR (("%s: setsockopt LINGER failed %s\n", func, strerror (errno)));
        break;
      }
    }

    ClearErrno ();
    if (v6 == 0)
    {
      memset (&addr4, 0, sizeof (addr4));
      addr4.sin_family = AF_INET;
      addr4.sin_port = htons (port);
      addr4.sin_addr.s_addr = htonl (ipaddr);   /* only for ipv4 */
      addr = (void *) &addr4;
      addrLen = sizeof (addr4);
    }
    else
    {
      memset (&addr6, 0, sizeof (addr6));
      addr6.sin6_family = AF_INET6;
      addr6.sin6_port = htons (port);
      addr = (void *) &addr6;
      addrLen = sizeof (addr6);
    }

    while (retry-- > 0 && bind (sock, (struct sockaddr *) addr, addrLen) < 0)
    {
      sysapiPrintf ("Failed to bind port %d retry %d\n", port, retry);
      osapiSleep (1);
    }

    if (retry <= 0)
    {
      EMWEB_ERROR (("%s: bind failed: %s\n", func, strerror (errno)));
      break;
    }

    if (max != 0 && listen (sock, max) < 0)
    {
      EMWEB_ERROR (("%s: listen failed: %s\n", func, strerror (errno)));
      break;
    }

    ClearErrno ();
    err = 0;
  }
  while (0);

  if (err != 0)
  {
    if (sock != INVALID_SOCKET)
    {
      close (sock);
    }
    sock = INVALID_SOCKET;
  }
  return sock;
}

int start_sock_servers (void)
{
  emwebSockServer (0, 80, 0, 60, 10);   /* IPV4 http server */
#if 0
  emwebSockServer (0, 23, 1, 60, 10);   /* IPV6 telnet server */
#endif
  return 0;
}

#endif

