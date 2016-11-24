/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename                        tcpbind.c
 *
 * @purpose                         Binding of upper layer to TCP/UDP
 *                                  socket interface
 *
 * @component                       Routing TCP Component
 *
 * @comments
 *     External Routines:
 *
 * TcpUdpBind_Init
 * TcpUdpBind_Destroy
 * TcpUdpBind_Open
 * TcpUdpBind_Close
 * TcpUdpBind_Send
 * TcpUdpBind_Receive
 * TcpUdpBind_Status
 * TcpUdpBind_RemoteInfo
 * TcpUdpBind_NotifyPrmChange
 *
 * Internal Routines:
 *
 * checkConnStatus
 * unpackRx
 * indicateStatusToUser
 * unpackStat
 * tcpKickTransmitter
 *
 * @create                        08/01/1999
 *
 *
 * @author                        Igor Achkinazi
 *                                Dan Dovolsky
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/tcp/tcpbind.c,v 1.1 2011/04/18 17:10:52 mruas Exp $";
#endif


/* --- standard include files --- */

#include <string.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef  _L7_OS_LINUX_
#include <sys/errno.h>
#include <sys/time.h>
#define USE_POLL 0
#include <sys/poll.h>
#endif /* _L7_OS_LINUX */
#ifdef  _L7_OS_VXWORKS_
#define USE_POLL 0
#include <selectLib.h>
#endif /* _L7_OS_LINUX */

/* --- external object interfaces --- */

#include "std.h"
#include "local.h"
#include "xx.ext"
#include "os_xxcmn.h"
#include "buffer.h"
#include "buffer.ext"
#include "frame.h"
#include "frame.ext"
#include "ll.ext"
#include "log.h"
#include "timer.ext"

/* --- specific include files --- */

#include "tcpbind.ext"
#include "ipstk_api.h"
#include "l7_packet.h"

/* Include socket specific header files depending on operating system */

    #if (RTOS == WINDOWS_RTOS) && defined(_WINDOWS)
    #include <windows.h>
    #include <ws2tcpip.h>
    int LastSocketErr;
    #define IS_NONBLOCKING()    (WSAEWOULDBLOCK == (LastSocketErr=WSAGetLastError()))
    #define IS_CONNECTED()      (WSAEISCONN == (LastSocketErr=WSAGetLastError()))
    #define IS_RESET()          (WSAECONNRESET == (LastSocketErr=WSAGetLastError()))
    #define IS_EINVAL()         (WSAEINVAL == (LastSocketErr=WSAGetLastError()))
    #define IS_EINPROGRESS()    (WSAEINPROGRESS == (LastSocketErr=WSAGetLastError()))
    #define errnoGet()          WSAGetLastError()
    #define FD_SIZE
                 1
    #elif RTOS == VXWORKS_RTOS
    #define SD_SEND                  1
    #define IS_NONBLOCKING()         (EWOULDBLOCK == errno)
    #define IS_CONNECTED()           (EISCONN == errno)
    #define IS_RESET()               (ECONNRESET == errno)
    #define IS_EINVAL()              (EINVAL == errno)
    #define IS_EINPROGRESS()         (EINPROGRESS == errno)
    #define FD_SIZE                  FD_SETSIZE

    #elif RTOS == LINUX_RTOS
#if 0
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/errno.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif
    #define SD_SEND                  1
    #define IS_NONBLOCKING()         (EWOULDBLOCK == errno)
    #define IS_CONNECTED()           (EISCONN == errno)
    #define IS_RESET()               (ECONNRESET == errno)
    #define IS_EINVAL()              (EINVAL == errno)
    #define IS_EINPROGRESS()         (EINPROGRESS == errno)
    #define FD_SIZE                  FD_SETSIZE
    #else
    #error Socket binding is not supported
    #endif

#include "l7_socket.h"
#include "osapi_support.h"


/* --- Internal (static) function prototypes --- */

/* Connection state */
#define STATUS_CLOSED         0
#define WAIT_FOR_ACCEPT       1
#define WAIT_FOR_CONNECT      2
#define WAIT_FOR_REMOTE_CLOSE 3
#define SEND_STATUS_ACTIVE    4
#define STATUS_ESTABLISHED    5

/* Connection flags */
#define TCP_F_INTXQ           0x1    /* TCP transmitter in the kicker */
#define COMM_F_LA_VALID       0x2    /* Local address is valid        */
#define COMM_F_RA_VALID       0x4    /* Remote address is valid       */
#define IP_F_HEADERINC        0x8    /* Application provides IP header*/
                                     /* IP_HDRINCL option is set      */
#define TCP_GRACE_CLOSE       0x10   /* Is stream socket closed       */
                                     /* gracefully?                   */

typedef enum e_ConnStat
{
   SOC_STAT_Valid  = 23190,
   SOC_STAT_Invalid = 0
} e_ConnStat;

/* The largest data size with can be received/send */
#define DFLT_BUF_LENGTH     0x1000



#define DEBUG_MODE    0

/* If sendto fails wait this long before retrying. 50 ms. */
#define SENDTO_RETRY_TIMEOUT   50000


/* Internal connection object definition */
typedef struct t_ConnInfo
{
   /* Pointers to keep the object on the double linked list   */
   /* created per TCP/UDP instance object.                    */
   struct t_ConnInfo *next;
   struct t_ConnInfo *prev;

   e_ConnStat validStatus;  /* Connection status              */
   t_Handle sockObj;        /* Parent TCP/UDP instance object */
   int connId;              /* Underlying socket object handle*/
   e_TcpUdpType type;       /* Connection type TCP/UDP/IP     */
   byte state;              /* Internal connection state      */
   word flags;              /* Connection flags               */
   word tx_flags;           /* Flags passed to the send(to)   */
   Bool client;             /* Client - TRUE, Server - FALSE  */
   word localPort;          /* Local port                     */
   ulng localAddress;       /* Local IP address               */
   word remotePort;         /* Peer port                      */
   ulng remoteAddress;      /* Peer address                   */
   ulng netMask;            /* Subnet mask of the interface   */

    /* Transmit queue */
   t_Handle   TcpTransmitQueue;

   /* Callbacks parameters */
   t_Handle   userId;       /* Upper object handle            */
   t_Handle   threadId;     /* Thread for rx/state reports    */
   F_SockRx   f_Rx;         /* Receive callback or NULLP      */
   F_RxQueue  f_RxQueue;    /* Rx queue callback              */
   F_SockStat f_Stat;       /* Connection state callback      */
} t_ConnInfo;


/* Tcp/Udp instance object definition */
typedef struct t_SockBind
{
   struct t_SockBind *next;
   struct t_SockBind *prev;
   t_ConnInfo *connList;
} t_SockBind;

static t_Handle  permThreadId;         /* Permanent thread ID for connections status checks */
static t_SockBind *sockBindObjList;    /* All tcp/udp instance objects list */

static e_Err checkConnStatus(void *dummy);
static e_Err unpackRx(void *xxInfo);
static void indicateStatusToUser(t_ConnInfo *connInfo, word status);
static e_Err unpackStat(void *xxInfo);


#if DEBUG_MODE
static char text[100];
#endif

/* Some debug stats */
static L7_uint32 peekError = 0;
static L7_uint32 lastPeekError = 0;
static L7_uint32 bufError = 0;
static L7_uint32 bufAllocFail = 0;
static L7_uint32 recvError = 0;
static L7_uint32 lastRecvError = 0;
static L7_uint32 recvLenError = 0;
static L7_uint32 recvTrunc = 0;
static L7_uint32 frameAllocFail = 0;
static L7_uint32 xxCallError[L7_ROUTING_MAX_QUEUES];
static L7_uint32 xxQError = 0;
static L7_uint32 rxPackets = 0;
static L7_uint32 sendError = 0;
static L7_uint32 lastSendError = 0;

void ospfDebugRxThreadStatsClear(void)
{
  L7_uint32 i;

  peekError = 0;
  lastPeekError = 0;
  bufError = 0;
  bufAllocFail = 0;
  recvError = 0;
  lastRecvError = 0;
  recvLenError = 0;
  recvTrunc = 0;
  frameAllocFail = 0;
  for (i = 0; i < L7_ROUTING_MAX_QUEUES; i++)
  {
    xxCallError[i] = 0;
  }
  xxQError = 0;
  rxPackets = 0;
  sendError = 0;
  lastSendError = 0;
}

void ospfDebugRxThreadStats(void)
{
  L7_uint32 i;
  L7_uchar8 msg[100];
  XX_Print("OSPFv2 receive thread statistics");
  sprintf(msg, "peek errors............................%u", peekError); XX_Print(msg);
  sprintf(msg, "last peek error........................%s", strerror(lastPeekError)); XX_Print(msg);
  sprintf(msg, "buffer mgmt errors.....................%u", bufError); XX_Print(msg);
  sprintf(msg, "buffer allocation failures.............%u", bufAllocFail); XX_Print(msg);
  sprintf(msg, "receive errors.........................%u", recvError); XX_Print(msg);
  sprintf(msg, "last receive error.....................%s", strerror(lastRecvError)); XX_Print(msg);
  sprintf(msg, "receive length errors..................%u", recvLenError); XX_Print(msg);
  sprintf(msg, "receive packet truncated...............%u", recvTrunc); XX_Print(msg);
  sprintf(msg, "frame allocation failure...............%u", frameAllocFail); XX_Print(msg);
  for (i = 0; i < L7_ROUTING_MAX_QUEUES; i++)
  {
    sprintf(msg, "XX_Call() errors queue %d............%u", i, xxCallError[i]); XX_Print(msg);
  }
  sprintf(msg, "Error initializing message for Q.......%u", xxQError); XX_Print(msg);
  sprintf(msg, "Packets received.......................%u", rxPackets); XX_Print(msg);
}

void ospfDebugTxThreadStats(void)
{
  L7_uchar8 msg[100];
  XX_Print("OSPFv2 send thread statistics");
  sprintf(msg, "send errors.........................%u", sendError); XX_Print(msg);
  sprintf(msg, "last send error.....................%s", strerror(lastSendError)); XX_Print(msg);
}

 /*********************************************************************
 * @purpose             Creates TCP/UDP instance object
 *
 *
 * @param p_ObjId       @b{(input)}   Pointer to store succesfully created
 *                                    TCP/UDP instance object
 * @param dummy         @b{(input)}   For compatability with another
 *                                    TcpUdpBind implementation
 * @param threadName    @b{(input)}   Name of thread created to read from socket
 *
 * @returns             E_OK	     - Success
 * @returns             Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_Init(t_Handle *p_ObjId, t_Handle dummy, char *threadName)
{
   t_SockBind *sockBindObj;

   /* Reinitialize receive debug stats */
   ospfDebugRxThreadStatsClear();

   if(!p_ObjId)
   {
      ASSERT(FALSE);
      return E_BADPARM;
   }

   sockBindObj = (t_SockBind *)XX_Malloc(sizeof(t_SockBind));
   if(!sockBindObj)
   {
      return E_NOMEMORY;
   }

   memset(sockBindObj, 0, sizeof(t_SockBind));

   XX_AddToDLList(sockBindObj, sockBindObjList);

   if(!permThreadId)
   {
#if (RTOS == NO_RTOS) || (RTOS == WINDOWS_RTOS)
      /* Contains details of the Winsock implementation */
      WSADATA  WSAData;
      /* Initialize Winsock2 */
      if (WSAStartup (MAKEWORD(2,2), &WSAData) != 0)
      {
         char text[100];
         sprintf(text, "WSAStartup failed! Error: %d", errnoGet());
         XX_Print(text);
         return E_FAILED;
      }
#endif /* #if (RTOS == NO_RTOS) || (RTOS == WINDOWS_RTOS) */
      XX_CreateThread(255, checkConnStatus, TRUE, &permThreadId, threadName);
   }

   *p_ObjId = (t_Handle)sockBindObj;
   return E_OK;
}



 /*********************************************************************
 * @purpose            Destroys TCP/UDP instance object
 *
 *
 * @param p_ObjId      @b{(input)}  Pointer to of the TCP/UDP instance
 *                                  object, zeroed on success
 *
 * @returns         E_OK	     - Success
 * @returns         Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_Destroy(t_Handle *p_ObjId)
{
   t_SockBind *sockBindObj;
   t_ConnInfo *connInfo;

   if(!p_ObjId || !(sockBindObj = (t_SockBind *)*p_ObjId))
   {
      ASSERT(FALSE);
      return E_BADPARM;
   }

   while((connInfo = sockBindObj->connList))
   {
      close(connInfo->connId);
      XX_DelFromDLList(connInfo, sockBindObj->connList);
      connInfo->validStatus = SOC_STAT_Invalid;
      XX_Free(connInfo);
   }

   XX_DelFromDLList(sockBindObj, sockBindObjList);
   if(!sockBindObjList)
   {
      XX_KillThreadSelf(permThreadId);
      permThreadId = NULLP;
#if (RTOS == NO_RTOS) || (RTOS == WINDOWS_RTOS)
      WSACleanup ();
#endif /* #if (RTOS == NO_RTOS) || (RTOS == WINDOWS_RTOS) */
   }

   XX_Free(sockBindObj);
   *p_ObjId = NULL;
   return E_OK;
}

 /*********************************************************************
 * @purpose     Opens a new TCP/UDP connection
 *
 *
 * @param objId      @b{(input)}  TCP/UDP instance object returned by
 *                                TcpUdpBind_Init
 * @param userId     @b{(input)}  The user ID of new connection for indications
 * @param param      @b{(input)}  Parameters for connection creation
 * @param userFun    @b{(input)}  The user events notification function
 * @param p_connId   @b{(input)}  Pointer to store succesfully created TCP/UDP
 *                                connection handle
 *
 * @returns     E_OK	     - Success
 * @returns     Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_Open(t_Handle objId, t_Handle userId, t_TcpUdpParam *param, t_Handle *p_connId)
{
   t_SockBind *sockBindObj = (t_SockBind *)objId;
   t_ConnInfo *conn;
   int prm = 1;
   int sockType;

   if(!param || !param->f_Stat || !sockBindObj || !p_connId)
   {
      ASSERT(FALSE);
      return E_BADPARM;
   }

   conn = (t_ConnInfo*)XX_Malloc(sizeof(t_ConnInfo));
   if(!conn)
   {
      return E_NOMEMORY;
   }

   memset(conn, 0, sizeof(t_ConnInfo));

   switch(param->connType)
   {
      case tcpConnType: 
        XX_Free(conn);
        ASSERT(FALSE);
        return E_BADPARM;
      case udpConnType: 
        sockType = SOCK_DGRAM;
        break;
      case ipConnType:  
        sockType = SOCK_RAW;
        break;
      default:          
        XX_Free(conn);
        ASSERT(FALSE);
        return E_BADPARM;
   }

   /* Open socket */
   if((conn->connId = socket(AF_INET, sockType, param->protocol)) < 0)
   {
      char msg[120];

      osapiSnprintf(msg, sizeof(msg), "socket() error %d", osapiErrnoGet());
      XX_Print(msg);
      ASSERT(FALSE);
      XX_Free(conn);
      return E_FAILED;
   }

   conn->localPort     = param->localPort;
   if(!param->localAddress)
      param->localAddress = INADDR_ANY;
   conn->localAddress  = param->localAddress;
   conn->remotePort    = param->remotePort;
   conn->remoteAddress = param->remoteAddress;
   conn->netMask       = param->netMask;

   /* Set SO_REUSEADDR option */
   if(param->adrReUse &&
      setsockopt(conn->connId, SOL_SOCKET, SO_REUSEADDR, (byte*)&prm, sizeof(prm)) < 0)
   {
      char msg[120];

      osapiSnprintf(msg, sizeof(msg), 
              "setsockopt(SOL_SOCKET, SO_REUSEADDR) error %d",
              osapiErrnoGet());
      XX_Print(msg);
      ASSERT(FALSE);
      close(conn->connId);
      XX_Free(conn);
      return E_FAILED;
   }

   /* Set SO_DONTROUTE option */
   if(param->dontRoute &&
      setsockopt(conn->connId, SOL_SOCKET, SO_DONTROUTE, (byte*)&prm, sizeof(prm)) < 0)
   {
      char msg[120];

      osapiSnprintf(msg, sizeof(msg), 
              "setsockopt(SOL_SOCKET, SO_DONTROUTE) error %d",
              osapiErrnoGet());
      XX_Print(msg);
      ASSERT(FALSE);
      close(conn->connId);
      XX_Free(conn);
      return E_FAILED;
   }

   /* Set total reserved receive buffer space */
   if(param->rxBufSize && setsockopt(conn->connId, SOL_SOCKET, SO_RCVBUF,
      (byte*)&param->rxBufSize, sizeof(int)) < 0)
   {
     L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
             "Failed to set socket %d receive buffer to %u bytes",
             conn->connId, param->rxBufSize);
      /* better to muddle through with smaller receive buffer than 
       * to kill the socket and not be able to receive at all. continue. */
   }

   if(param->ipHeaderInc && (param->connType == ipConnType))
   {
      /* This option is valid only for Raw sockets */
      int on = 1;
      if(setsockopt(conn->connId, IPPROTO_IP, IP_HDRINCL,
         (byte*)&on, sizeof(int)) < 0)
      {
         char msg[120];

         osapiSnprintf(msg, sizeof(msg),
            "setsockopt(IPPROTO_IP, IP_HDRINCL) error: %d",
            osapiErrnoGet());
         XX_Print(msg);
         ASSERT(FALSE);
         close(conn->connId);
         XX_Free(conn);
         return E_FAILED;
      }
      conn->flags |= IP_F_HEADERINC;
   }

   /* Set total reserved transmit buffer space */
   if(param->txBufSize && setsockopt(conn->connId, SOL_SOCKET, SO_SNDBUF,
      (byte*)&param->txBufSize, sizeof(int)) < 0)
   {
      L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_OSPF_MAP_COMPONENT_ID,
             "Failed to set socket %d send buffer to %u bytes",
             conn->connId, param->rxBufSize);
      /* better to muddle through with smaller send buffer than 
       * to not be able to send at all. continue. */
   }

   if((!param->dontBind && (param->connType != tcpConnType)) ||
      ((param->connType == tcpConnType) && !param->client))
   {
      /* Bind to specific address & port only for server */
      ulng   bindAddr;
      struct sockaddr_in sa;
      memset(&sa, 0, sizeof(sa));
      sa.sin_family = AF_INET;
      bindAddr = param->localAddress; 

      L7_HTONL(&bindAddr, &sa.sin_addr.s_addr);



      L7_HTONS(&param->localPort, &sa.sin_port);

      /* Bind to local host */
      if(bind(conn->connId, (struct sockaddr *)&sa, sizeof(sa)) < 0)
      {
         char msg[120];

         osapiSnprintf(msg, sizeof(msg),
                       "bind(%x, %x) error %d",
                        (unsigned int)param->localAddress,
                        (unsigned int)param->localPort,
                       (unsigned int)osapiErrnoGet());
         XX_Print(msg);
         ASSERT(FALSE);
         close(conn->connId);
         XX_Free(conn);
         return E_FAILED;
      }
   }

   /* Set Non-blocking mode */
   if(osapiSocketNonBlockingModeSet(conn->connId, prm) != L7_SUCCESS)
   {
      char msg[120];

      osapiSnprintf(msg, sizeof(msg),
              "l7SktIoctl(FIONBIO) error %d", osapiErrnoGet());
      XX_Print(msg);
      ASSERT(FALSE);
      close(conn->connId);
      XX_Free(conn);
      return E_FAILED;
   }

   if(param->connType == tcpConnType)
   {
     /* TCP sockets are no longer supported */
     ASSERT(FALSE);
     close(conn->connId);
     XX_Free(conn);
     return E_FAILED;
   }   

  /* UDP & IP socket */
  /* This can be enabled only for systems supporting this option
  * Disable loopback of the multicast packets.
  * Ignore failure as its not supported on all platforms and we check
  * for duplicate packets in sockReceive
  */
  prm = 0;
  if(setsockopt(conn->connId, IPPROTO_IP, IP_MULTICAST_LOOP,
               (char *)&prm, sizeof(prm)) < 0) 
  {
#if DEBUG_MODE
    sprintf(text, "setsockopt(IP_MULTICAST_LOOP) error %s",
         strerror(osapiErrnoGet()));
    XX_Print(text);
#endif
  }

  conn->state = SEND_STATUS_ACTIVE;

   /* copy rest parameters */
   conn->sockObj  = sockBindObj;
   conn->userId   = userId;
   conn->type     = param->connType;
   conn->threadId = param->threadId;
   conn->f_Rx     = param->f_Rx;
   conn->f_RxQueue= param->f_RxQueue;
   conn->f_Stat   = param->f_Stat;
   conn->tx_flags = param->tx_flags & TCP_SEND_URGENT;
   conn->client   = param->client;

   XX_AddToDLList(conn, sockBindObj->connList);
   conn->validStatus = SOC_STAT_Valid;

   *p_connId = (t_Handle)conn;

   return E_OK;
}



 /*********************************************************************
 * @purpose           Closes existing TCP/UDP connection
 *
 *
 * @param connId      @b{(input)}  TCP/UDP connection handle returned by
 *                                 TcpUdpBind_Open
 *
 * @returns           E_OK	     - Success
 * @returns           Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/

e_Err TcpUdpBind_Close(t_Handle connId)
{
   t_ConnInfo *connInfo = (t_ConnInfo *)connId;
   t_SockBind *sockBindObj;
   void *p;

   if(connInfo == NULLP || connInfo->connId < 0 ||
      connInfo->validStatus != SOC_STAT_Valid ||
      connInfo->state == STATUS_CLOSED)
   {
      return E_FAILED;
   }

   sockBindObj = (t_SockBind *)connInfo->sockObj;
   if(!sockBindObj )
   {
      return E_FAILED;
   }

   /* Delete trasmission queue for TCP connection */
   if(connInfo->TcpTransmitQueue)
   {
      /* Release frames currently enrolled for transmission */
      while( (p = LL_Get(connInfo->TcpTransmitQueue)) )
         F_Delete(p);

      /* Delete the linked list itself */
      LL_Delete( connInfo->TcpTransmitQueue );
      connInfo->TcpTransmitQueue = 0;
   }

   /* Kill sockets if they are already done or no message exchange needs */
   if((connInfo->type != tcpConnType) ||
      (connInfo->state == WAIT_FOR_ACCEPT) ||
      (connInfo->state == WAIT_FOR_CONNECT)||
      !(connInfo->flags & TCP_GRACE_CLOSE))
   {
      close(connInfo->connId);
      XX_DelFromDLList(connInfo, sockBindObj->connList);
      connInfo->validStatus = SOC_STAT_Invalid;
#if DEBUG_MODE
      sprintf(text, "Closed socket:%x address:%x (%s)",
         (unsigned int)connInfo , (unsigned int)connInfo->localAddress,
         connInfo->client ? "Client":"Server");
      XX_Print(text);
#endif
      XX_Free(connInfo);
      return E_OK;
   }

   shutdown(connInfo->connId, SD_SEND);
   connInfo->state = WAIT_FOR_REMOTE_CLOSE;

   return E_OK;
}

 /*********************************************************************
 * @purpose         Add multicast address to be listened
 *
 *
 * @param connId    @b{(input)}  TCP/UDP connection handle as returned by
 *                               TcpUdpBind_Open
 * @param intIfNum  @b{(input)}  interface where we listen
 * @param addr      @b{(input)}  Multicast address
 *
 * @returns         E_OK	     - Success
 * @returns         Not E_OK  - Failure, look at std.h for details
 *
 * @notes           This call configures the socket owned by connId to 
 *                  receive multicast frames on the interface who's address
 *                  is ifaddr.
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_AddMulticastAddr(IN t_Handle connId, ulng intIfNum, ulng addr)
{
   /* UDP or RAW IP socket */
   struct ip_mreqn    ipMreq;
   L7_uchar8 ifName[IFNAMSIZ];
   L7_uint32 ifIndex;
   t_ConnInfo        *conn = (t_ConnInfo*)connId;
   ulng ifaddr = 0;

   /* Check input parameters */
   if((connId == NULLP) || conn->connId < 0 || ((addr & 0xe0000000) != 0xe0000000))
   {
      ASSERT(FALSE);
      return E_BADPARM;
   }

   /* NOTE: multicast sockets must be bound to INADDR_ANY 
    * Therefore, localAddress must be INADDR_ANY if this
    * socket is to receive multicast datagrams. 
    */
   if(conn->localAddress != INADDR_ANY)
   {
     unsigned char groupStr[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa(addr, groupStr);
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
             "Failure adding OSPF to multicast group %s on interface %u."
             " Socket not bound to INADDR_ANY.", groupStr, intIfNum);
     return E_BADPARM;
   } 

    if (osapiIfNameStringGet(intIfNum, ifName, IFNAMSIZ) != L7_SUCCESS)
    {
      unsigned char groupStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(addr, groupStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
              "Failure adding OSPF to multicast group %s on interface %u."
              " Failed to get interface name.", groupStr, intIfNum);

      return E_FAILED;
    }
    if (ipstkStackIfIndexGet(ifName, &ifIndex) != L7_SUCCESS)
    {
      unsigned char groupStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(addr, groupStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
              "Failure adding OSPF to multicast group %s on interface %u."
              " Failed to get interface index.", groupStr, intIfNum);
      return E_FAILED;
    }

   memset(&ipMreq, 0, sizeof(ipMreq));

   /* Set Multicast address */
   L7_HTONL(&addr, &ipMreq.imr_multiaddr.s_addr);

   /* unicast interface addr on local addr. No longer used. IP stack
    * identifies interface by interface index. */
#ifdef _L7_OS_VXWORKS_
   L7_HTONL(&ifaddr, &ipMreq.imr_interface.s_addr);
#endif
#ifdef _L7_OS_LINUX_
   L7_HTONL(&ifaddr, &ipMreq.imr_address.s_addr);
#endif

   
   ipMreq.imr_ifindex = (int) ifIndex;

   /* set the socket option to join the MULTICAST group */
   if (setsockopt(conn->connId, IPPROTO_IP, IP_ADD_MEMBERSHIP,
      (char *)&ipMreq, sizeof (ipMreq)) < 0)
   {
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
             "setsockopt(IP_ADD_MEMBERSHIP, %u, %x, %x) error: (%d) %s",
             (unsigned int)intIfNum,
             (unsigned int)addr,
             (unsigned int)conn->localAddress,
             osapiErrnoGet(),
             strerror(osapiErrnoGet()));
      return E_FAILED;
   }

   return E_OK;
}

 /*********************************************************************
 * @purpose         Multicast address to be dropped
 *
 *
 * @param connId    @b{(input)}  TCP/UDP connection handle as returned by
 *                               TcpUdpBind_Open
 * @param intIfNum  @b{(input)}  interface where we have been listening to group
 * @param addr      @b{(input)}  Multicast address
 *
 * @returns         E_OK	     - Success
 * @returns         Not E_OK  - Failure, look at std.h for details
 *
 * @notes           This call configures the socket owned by connId to 
 *                  stop receiving multicast frames on the interface who's 
 *                  address is ifaddr.
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_DropMulticastAddr(IN t_Handle connId, ulng intIfNum, ulng addr)
{
   /* UDP or RAW IP socket */
  struct ip_mreqn    ipMreq;
  L7_uchar8 ifName[IFNAMSIZ];
  L7_uint32 ifIndex, error;
  t_ConnInfo        *conn = (t_ConnInfo*)connId;
  ulng ifaddr = 0;

   if((connId == NULLP) || conn->connId < 0)
   {
     /* connection (socket) does not exist, so nothing to do! */
      return E_OK;
   }

   /* Check input parameters */
   if((connId == NULLP) || conn->connId < 0 || ((addr & 0xe0000000) != 0xe0000000))
   {
      ASSERT(FALSE);
      return E_BADPARM;
   }

   /* NOTE: multicast sockets must be bound to INADDR_ANY 
    */
   if(conn->localAddress != INADDR_ANY)
   {
     unsigned char groupStr[OSAPI_INET_NTOA_BUF_SIZE];
     osapiInetNtoa(addr, groupStr);
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
             "Failure removing OSPF from multicast group %s on interface %u."
             " Socket not bound to INADDR_ANY.", groupStr, intIfNum);
     return E_BADPARM;
   }

   if (osapiIfNameStringGet(intIfNum, ifName, IFNAMSIZ) != L7_SUCCESS)
    {
      unsigned char groupStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(addr, groupStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
              "Failure removing OSPF from multicast group %s on interface %u."
              " Failed to get interface name.", groupStr, intIfNum);

      return E_FAILED;
    }
    if (ipstkStackIfIndexGet(ifName, &ifIndex) != L7_SUCCESS)
    {
      unsigned char groupStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(addr, groupStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
              "Failure removing OSPF from multicast group %s on interface %u."
              " Failed to get interface index.", groupStr, intIfNum);
      return E_FAILED;
    }

    memset(&ipMreq, 0, sizeof(ipMreq));

   /* Set Multicast address */
   L7_HTONL(&addr, &ipMreq.imr_multiaddr.s_addr);

   /* IP stack now identifies interface by interface index. */
#ifdef _L7_OS_VXWORKS_
   L7_HTONL(&ifaddr, &ipMreq.imr_interface.s_addr);
#endif
#ifdef _L7_OS_LINUX_
   L7_HTONL(&ifaddr, &ipMreq.imr_address.s_addr);
#endif

    
   ipMreq.imr_ifindex = (int) ifIndex;

   /* set the socket option to drop the MULTICAST group */
   if (setsockopt(conn->connId, IPPROTO_IP, IP_DROP_MEMBERSHIP,
      (char *)&ipMreq, sizeof (ipMreq)) < 0)
   {
      if((error = osapiErrnoGet()) != EADDRNOTAVAIL)
      {
        /* EADDRNOTAVAIL means an incorrect address was specified for
         * the imr_multiaddr or imr_ifindex value is wrong.
         * imr_multiaddr can't be wrong.
         *
         * imr_ifindex can be wrong if the interface got deleted in the
         * stack by the time we reached here!! which shouldn't happen generally.
         * In that case, its ok for us though.
         * Hence, we log the failure only if the error is not EADDRNOTAVAIL.
         */
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID,
                "setsockopt(IP_DROP_MEMBERSHIP, %u, %x, %x) error: (%d), %s",
                (unsigned int)intIfNum,
                (unsigned int)addr,
                (unsigned int)conn->localAddress,
                error,
                strerror(error));
        return E_FAILED;
      }
   }

   return E_OK;
}


 /*********************************************************************
 * @purpose           Sends data over TCP/UDP connection
 *
 *
 * @param connId      @b{(input)}  TCP/UDP/IP connection handle returned by
 *                                 TcpUdpBind_Open
 * @param frame       @b{(input)}  Data to send
 * @param address     @b{(input)}  Destination IP address for UDP connections
 * @param port        @b{(input)}  Destination logical port for UDP connections
 * @param flags       @b{(input)}  Sending flags
 *
 * @returns           Number of bytes sent or -1 on error
 *
 * @notes             
 *
 * @end
 * ********************************************************************/
int TcpUdpBind_Send(t_Handle connId, t_Handle frame, ulng address, word port, word flags)
{
   t_ConnInfo *connInfo = (t_ConnInfo *)connId;
   t_Handle    pBuf;
   Bool        Last;
   int         num;
   byte        array[DFLT_BUF_LENGTH];
   byte        *pArray;
   word        dataLen;
   struct      sockaddr_in remSa;

   if(frame == NULLP)
   {
      ASSERT(FALSE);
      return (-1);
   }

   if(!connInfo || connInfo->connId < 0 || !connInfo->sockObj ||
      connInfo->validStatus != SOC_STAT_Valid ||
      (connInfo->state != STATUS_ESTABLISHED &&
       connInfo->state != SEND_STATUS_ACTIVE))
   {
      ASSERT(FALSE);
      num = (-1);
      goto sendExit;
   }

   /* UDP/IP connections or TCP if transmit queue is not in use */
   dataLen = F_GetLength(frame);
   if((pArray = F_Frame2Array(frame)) == NULLP)
   {
      for(pArray=array,pBuf = F_GetFirstBuf(frame); pBuf; pBuf = B_GetNextBuffer(pBuf, &Last))
      {
         word  len_head, len_tail;
         byte  *ptr_head, *ptr_tail;
         /* check if the data starts at the tail */
         if( !B_BIG_TAIL(pBuf) && (len_tail = B_GetDataTailLength(pBuf)) > 0)
         {
            /* get second BD (first bytes in data buffer) */
            ptr_tail = (byte *)B_GetDataTail(pBuf);

            /* get first BD (last bytes in data buffer) */
            len_head = B_GetLength(pBuf) - len_tail;
            ptr_head = B_GetData(pBuf);
         }
         else
         {
            len_head = B_GetLength(pBuf);
            ptr_head = B_GetData(pBuf);
            len_tail = 0;
            ptr_tail = NULLP;
         }
         /* Copy to array from head */
         memcpy(pArray, ptr_head, len_head);
         pArray += len_head;
         if(len_tail > 0)
         {
            /* Copy to array from tail */
            memcpy(pArray, ptr_tail, len_tail);
            pArray += len_tail;
         }
      }
      pArray=array;
   }

   if(connInfo->type == tcpConnType)
      num = send(connInfo->connId, pArray, dataLen, connInfo->tx_flags);
   else
   {
      memset(&remSa, 0, sizeof(remSa));
      remSa.sin_family = AF_INET;

      L7_HTONL(&address, &remSa.sin_addr.s_addr);
      L7_HTONS(&port, &remSa.sin_port);

      num = sendto(connInfo->connId, pArray, dataLen, connInfo->tx_flags,
                   (struct sockaddr*)&remSa, sizeof(remSa));
      if (num < 0)
      {
        /* retry briefly. The danger here is that the thread that's blocking
         * on this send may be responsible for reading incoming packets from
         * the IP stack. The packet buffers needed to send may be queued to
         * this thread's receive socket. So blocking here prevents the thread
         * from freeing packet buffers by draining the receive queue. */
        fd_set writefds; /* fds ready to write */
        struct timeval timeout;
        L7_uint32 numFd;

        timeout.tv_sec = 0;
        timeout.tv_usec = SENDTO_RETRY_TIMEOUT;
        
        FD_ZERO(&writefds);
        FD_SET(connInfo->connId, &writefds);
        numFd = select(connInfo->connId + 1, NULL, &writefds, NULL, &timeout);
        if (numFd > 0)
        {
          /* socket now available for write */
          num = sendto(connInfo->connId, pArray, dataLen, connInfo->tx_flags,
                       (struct sockaddr*)&remSa, sizeof(remSa));
        }
      }
   }

   if(num < 0)
   {
     L7_uchar8 destIpStr[OSAPI_INET_NTOA_BUF_SIZE];
     sendError++;
     lastSendError = osapiErrnoGet();
     osapiInetNtoa(address, destIpStr);
     L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_OSPF_MAP_COMPONENT_ID, 
             "sendto() error %d (%s) on OSPFv2 socket %d. Dest IP %s.",
             lastSendError, strerror(lastSendError), connInfo->connId,
             destIpStr);
     if(!(IS_NONBLOCKING()))
       num = -1;
   }

sendExit:
   F_Delete(frame);
   return num;
}


 /*********************************************************************
 * @purpose           Receives data from TCP/UDP connection
 *
 *
 * @param  connId     @b{(input)}  TCP/UDP connection handle returned by
 *                                 TcpUdpBind_Open
 * @param  p_frame    @b{(input)}  Pointer to store frame with incoming data
 * @param  p_Address  @b{(input)}  Pointer to store remote IP address for UDP
 *                                 connections
 * @param  p_Port     @b{(input)}  Pointer to store remote logical port for UDP
 *                                connections
 *
 * @returns           Number of bytes received
 *
 * @notes
 *
 * @end
 * ********************************************************************/
word TcpUdpBind_Receive(t_Handle connId, t_Handle *p_frame, ulng *p_Address, word *p_Port)
{
   /* Not supported */
   return 0;
}


 /*********************************************************************
 * @purpose          Retrieves connection status
 *
 *
 * @param connId     @b{(input)}  TCP/UDP connection handle returned by
 *                                TcpUdpBind_Open
 * @param p_status   @b{(input)}  Pointer to store status of the
 *                                   connection
 *
 * @returns          E_OK	     - Success
 * @returns          Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_Status(t_Handle connId, word *p_status)
{
   t_ConnInfo *connInfo = (t_ConnInfo *)connId;

   /* Validate input parameters */
   if(!connInfo || connInfo->connId < 0 || !connInfo->sockObj ||
      !p_status || connInfo->validStatus != SOC_STAT_Valid)
   {
      ASSERT(FALSE);
      return E_FAILED;
   }

   switch(connInfo->state)
   {
   case WAIT_FOR_CONNECT:
         *p_status = TCPUDP_CONN_STATUS_PENDING;
         break;
   case WAIT_FOR_ACCEPT:
         *p_status = TCPUDP_CONN_STATUS_LISTENING;
         break;
   case SEND_STATUS_ACTIVE:
   case STATUS_ESTABLISHED:
         *p_status = TCPUDP_CONN_STATUS_ACTIVE;
         break;
   default:
         *p_status = TCPUDP_CONN_STATUS_CLOSED;
         break;
   }

   return E_OK;
}


 /*********************************************************************
 * @purpose           Retrieves remote side info the connection
 *                    connected to
 *
 *
 * @param connId      @b{(input)}  TCP/UDP connection handle returned by
 *                                 TcpUdpBind_Open
 * @param p_Address   @b{(input)}  Pointer to return peer IP address
 * @param p_Port      @b{(input)}  Pointer to return peer logical port
 *
 * @returns           E_OK	     - Success
 * @returns           Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_RemoteInfo(t_Handle connId, ulng *p_Address, word *p_Port)
{
   t_ConnInfo *connInfo = (t_ConnInfo *)connId;

   if(!connInfo || connInfo->connId < 0 || !connInfo->sockObj ||
      connInfo->validStatus != SOC_STAT_Valid)
      return E_FAILED;

   if(!connInfo->client && !(connInfo->flags & COMM_F_RA_VALID))
   {
      /* Server. The remote address is unknown on socket creation */
      struct sockaddr sa;
      struct sockaddr_in *pSaIn = (struct sockaddr_in*)&sa;
      int len = sizeof(struct sockaddr);

      memset(&sa, 0, sizeof(sa));
      if(getpeername(connInfo->connId, (struct sockaddr *)&sa, &len) < 0)
      {
#if DEBUG_MODE
         sprintf( text, "getpeername() error %d socket 0x%X",
                  osapiErrnoGet(), (unsigned int)connInfo);
         XX_Print(text);
#endif
         return E_FAILED;
      }
      if(sa.sa_family != AF_INET)
         return E_FAILED;

      /* The socket address is retrieved successfully */
      L7_NTOHL(&(pSaIn->sin_addr.s_addr), &connInfo->remoteAddress);
      L7_NTOHS(&(pSaIn->sin_port), &connInfo->remotePort);
      /* Local address is valid now */
      connInfo->flags |= COMM_F_RA_VALID;
   }

   /* Server with known local address */
   if(p_Address)
      *p_Address = connInfo->remoteAddress;
   if(p_Port)
      *p_Port    = connInfo->remotePort;

   return E_OK;
}


 /*********************************************************************
 * @purpose           Retrieves local connection parameters
 *
 *
 * @param connId      @b{(input)}  TCP/UDP connection handle returned by
 *                                 TcpUdpBind_Open
 * @param p_Address   @b{(input)}  Pointer to store local IP address
 * @param p_Port      @b{(input)}  Pointer to store local port number
 *
 * @returns     E_OK	     - Success
 * @returns     Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_LocalInfo(t_Handle connId, ulng *p_Address, word *p_Port)
{
   t_ConnInfo *connInfo = (t_ConnInfo *)connId;

   if(!connInfo || connInfo->connId < 0 || !connInfo->sockObj ||
      connInfo->validStatus != SOC_STAT_Valid)
      return E_FAILED;

   if(!(connInfo->flags & COMM_F_LA_VALID))
   {
      /* The local address can be 0 on socket creation */
      struct sockaddr sa;
      struct sockaddr_in *pSaIn = (struct sockaddr_in*)&sa;
      int len = sizeof(struct sockaddr);

      memset(&sa, 0, sizeof(sa));
      if(getsockname(connInfo->connId, (struct sockaddr *)&sa, &len) < 0)
      {
#if DEBUG_MODE
         sprintf( text, "getsockname() error %d socket 0x%X",
                  osapiErrnoGet(), (unsigned int)connInfo);
         XX_Print(text);
#endif
         return E_FAILED;
      }
      if(sa.sa_family != AF_INET)
         return E_FAILED;

      /* The socket address is retrieved successfully */
      L7_NTOHL(&(pSaIn->sin_addr.s_addr), &connInfo->localAddress);
      L7_NTOHS(&(pSaIn->sin_port), &connInfo->localPort);

      /* Local address is valid now */
      connInfo->flags |= COMM_F_LA_VALID;
   }

   /* Server with known local address */
   if(p_Address)
      *p_Address = connInfo->localAddress;
   if(p_Port)
      *p_Port    = connInfo->localPort;

   return E_OK;
}


/*********************************************************************
 * @purpose          Changes receive and status callbacks pointers
 *
 *
 * @param connId     @b{(input)}  TCP/UDP connection handle returned by
 *                                 TcpUdpBind_Open
 * @param UserId     @b{(input)}  new user id
 * @param f_Rx       @b{(input)}  new receive callback
 * @param f_Stat     @b{(input)}  new status callback
 *
 * @returns          E_OK      - Success (can't fail)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_NotifyPrmChange(t_Handle connId, t_Handle UserId, F_SockRx f_Rx, F_SockStat f_Stat)
{
   t_ConnInfo *connInfo = (t_ConnInfo *)connId;
   if(!connInfo)
   {
      ASSERT(FALSE);
      return E_BADPARM;
   }

   connInfo->userId = UserId;
   connInfo->f_Rx = f_Rx;
   connInfo->f_Stat = f_Stat;

   return E_OK;
}

/* Check socket connection thread function */
static e_Err checkConnStatus(void *dummy)
{
 t_SockBind *sockBindObj;
 t_ConnInfo *connInfo;
 t_ConnInfo *next;
 int fromLen = sizeof(L7_sockaddr_in_t);
 int toLen = sizeof(L7_sockaddr_in_t);
 t_Handle frame;
 void *p_buf = NULLP;
 L7_sockaddr_in_t sa;  /* source IP address of OSPF message */
 L7_sockaddr_in_t da;  /* destination IP address of OSPF message */
 e_Err rc = E_OK;
 int pktReceived = 1;
 ulng rxQueue = 0;
 L7_RC_t rc2;
 int rxLength;           /* Number of bytes to read from socket */
 int reqLength;          /* Number of bytes requested */
 L7_uint32 rxIntIfNum;   /* internal interface number of receive interface */
 L7_uint32 rxHopCount;   /* unused */
 int peekFlags = MSG_PEEK | MSG_TRUNC, noPeekFlags = 0;
 L7_ipHeader_t ipHdr;

 while(pktReceived)
 {
   pktReceived = 0;

   for(sockBindObj = sockBindObjList; sockBindObj; sockBindObj = sockBindObj->next)
   {
      for(connInfo = sockBindObj->connList; connInfo; connInfo = next)
      {
         next = connInfo->next;

         switch(connInfo->state)
         {
         case WAIT_FOR_CONNECT:
           /* not supported for UDP/IP sockets */
           ASSERT(FALSE);
           continue;

         case WAIT_FOR_ACCEPT:
            /* not supported for UDP/IP sockets */
            ASSERT(FALSE);
            continue;

         case SEND_STATUS_ACTIVE:
            /* connection becames active */
            indicateStatusToUser(connInfo, TCPUDP_CONN_STATUS_ACTIVE);
            connInfo->state = STATUS_ESTABLISHED;
            /* This case intentionally falls to the next case */

         case STATUS_ESTABLISHED:
            /* This case intentionally falls to the next case */

         case WAIT_FOR_REMOTE_CLOSE:

           /* check if there is any data available on this socket 
            * and get the length of the data available */
           rxLength = sizeof(L7_ipHeader_t);
           peekFlags = (MSG_PEEK | MSG_TRUNC);
           rc2 = osapiPktInfoRecv(connInfo->connId, (L7_uchar8 *)&ipHdr,
                                  &rxLength, &peekFlags, (L7_sockaddr_t*)&sa, &fromLen,
                                  (L7_sockaddr_t*)&da, &toLen, &rxIntIfNum, 
                                   &rxHopCount, NULL);
           if ((rc2 != L7_SUCCESS) || (rxLength < 0))
           {    
             /* socket is non-blocking. Just continue if no message avail. */
             if((rxLength < 0) && (!IS_NONBLOCKING()) && (!IS_EINVAL()))
             {
               peekError++;
               lastPeekError = osapiErrnoGet();
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                        "osapiPktInfoRecv() with MSG_PEEK error %d (%s) on socket %#x",
                       lastPeekError, strerror(lastPeekError), (unsigned int)connInfo);

               indicateStatusToUser(connInfo, TCPUDP_CONN_STATUS_ERROR);
             }
             continue;
           }

            rxLength = osapiNtohs(ipHdr.iph_len);

            /* Now get entire packet in a correctly sized buffer */
            if (p_buf != NULLP)
            {
              B_Delete(p_buf, TRUE);
              L7_LOG(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                     "Memory management error in checkConnStatus(). p_buf not NULL.");
              bufError++;
            }
            p_buf = B_NewEx(0, 0, (word)rxLength);
            if (p_buf == NULLP)
            {
              bufAllocFail++;
               rc = E_FAILED;
               goto CheckExit;
            }

            reqLength = rxLength;
            noPeekFlags = 0;
            rc2 = osapiPktInfoRecv(connInfo->connId, B_GetData(p_buf),
                                   &rxLength, &noPeekFlags, (L7_sockaddr_t*)&sa, &fromLen,
                                   (L7_sockaddr_t*)&da, &toLen, &rxIntIfNum, 
                                   &rxHopCount, NULL);

            if ((rc2 != L7_SUCCESS) || (rxLength < 0))
            {
              /* since we peeked above, there should always be a packet available here */
              recvError++;
              lastRecvError = osapiErrnoGet();
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                      "osapiPktInfoRecv() error %d (%s) on socket %#x rc = %d",
                      lastRecvError, strerror(lastRecvError), 
                      (unsigned int)connInfo, rc2);
              if((rxLength < 0) && (!IS_NONBLOCKING()) && (!IS_EINVAL()))
              {
                indicateStatusToUser(connInfo, TCPUDP_CONN_STATUS_ERROR);
              }
              B_Delete(p_buf, TRUE);
              p_buf = NULLP;
              continue;
            }
            
            if (reqLength != rxLength)
            {
              /* should never happen */
              recvLenError++;
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                     "OSPF receive thread requested %d bytes. Received %d bytes.");
            }
            if (noPeekFlags & MSG_TRUNC)
            {
              /* should never happen */
              recvTrunc++;
              L7_LOG(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                     "OSPF receive message truncated.");
            }
            
             L7_NTOHL(&sa.sin_addr.s_addr, &connInfo->remoteAddress);
             L7_NTOHS(&sa.sin_port, &connInfo->remotePort);

            if(!connInfo->f_Rx)
            {
               indicateStatusToUser(connInfo, TCPUDP_CONN_STATUS_DATA_READY);
               B_Delete(p_buf, TRUE);
               p_buf = NULLP;
               continue;
            }

            /* allocate receive frame */
            if((frame = F_New(p_buf)) == NULLP)
            {
              frameAllocFail++;
               rc = E_FAILED;
               goto CheckExit;
            }

            B_SetLength(p_buf, rxLength);
            F_GetLength(frame) = rxLength;
            p_buf = NULLP;  /* Do not deallocate the buffer on exit */

            if(connInfo->threadId)
            {
               t_XXCallInfo *xxci;

               /* get the receive queue */
               rxQueue = 0; /* default */
               if(connInfo->f_RxQueue)
                 rxQueue = connInfo->f_RxQueue(connInfo->userId, frame);                 

               /* Post message to defined thread */
               PACKET_INIT_MQUEUE(xxci, unpackRx, 0, 0, rxQueue, 5, (ulng)connInfo);
               if(xxci)
               {
                  PACKET_PUT(xxci, (ulng)frame);
                  PACKET_PUT(xxci, connInfo->remoteAddress);
                  PACKET_PUT(xxci, connInfo->remotePort);
                  PACKET_PUT(xxci, rxIntIfNum);
                  if (XX_Call(connInfo->threadId, xxci) != E_OK)
                  {
                    xxCallError[rxQueue]++;
                    F_Delete(frame);
                  }
               }
               else
               {
                 xxQError++;
                  L7_LOG(L7_LOG_SEVERITY_ERROR, L7_OSPF_MAP_COMPONENT_ID, 
                         "Memory allocation failure: Failure placing received "
                         "OSPF packet on OSPF message queue.");
                  F_Delete(frame);
               }

               rxPackets++;
               pktReceived = 1;
            }
            else
               connInfo->f_Rx(connInfo->userId, frame, connInfo->remoteAddress,
                              connInfo->remotePort, rxIntIfNum);
         default:
            break;
         }
      }
   }
 }

CheckExit:
   if(p_buf != NULLP)
      B_Delete(p_buf, TRUE);
   return rc;
}

/* XX_Call Unpack packet receive notification function */
static e_Err unpackRx(void *xxInfo)
{
   t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)xxInfo;
   t_ConnInfo *connInfo = (t_Handle)PACKET_GET(xxCallInfo, 0);
   t_Handle frame = (t_Handle)PACKET_GET(xxCallInfo, 1);
   ulng address = PACKET_GET(xxCallInfo, 2);
   word port = (word)PACKET_GET(xxCallInfo, 3);
   L7_uint32 intIfNum = PACKET_GET(xxCallInfo, 4);

   return connInfo->f_Rx(connInfo->userId, frame, address, port, intIfNum);
}

/* Indicate status to user by XX_Call or directly */
static void indicateStatusToUser(t_ConnInfo *connInfo, word status)
{
   t_XXCallInfo *xxci;
   e_Err e;

   if(connInfo->threadId)
   {
      /* Post message to defined thread */
      PACKET_INIT(xxci, unpackStat, 0, 0, 2, (ulng)connInfo);
      if(xxci)
      {
         PACKET_PUT(xxci, (ulng)status);
         e = XX_Call(connInfo->threadId, xxci);
         if (e != E_OK)
         {
           L7_LOGFNC(L7_LOG_SEVERITY_ERROR, 
                     "indicateStatusToUser() failed with error %d", e);
         }
      }
   }
   else
      connInfo->f_Stat(connInfo->userId, (t_Handle)connInfo, status);
}

/* XX_Call Unpack packet status notification function */
static e_Err unpackStat(void *xxInfo)
{
   t_XXCallInfo *xxCallInfo = (t_XXCallInfo *)xxInfo;
   t_ConnInfo *connInfo = (t_ConnInfo*)PACKET_GET(xxCallInfo, 0);
   word status = (word)PACKET_GET(xxCallInfo, 1);

   connInfo->f_Stat(connInfo->userId, (t_Handle)connInfo, status);
   return E_OK;
}


e_Err TcpUdpBind_GetTCPStatistics(IN t_Handle objId,
                                  IN TCP_Stats *p_stat, Bool fReset)
{
   /* Not supported */
   return E_OK;
}

e_Err l7SocketSetOpt(IN t_Handle connId, int level, int optname, char *optval, int optlen)
{
  t_ConnInfo        *conn = (t_ConnInfo*)connId;

  if (setsockopt(conn->connId, level, optname, optval, optlen) < 0)
  {
    return E_FAILED;
  }

  return E_OK;
}



/*--------------------------------------------------------------------------------*/
/* Note: This was added to prevent the connInfo local address and port to         */
/* get reset on a call to TcpUdpBind_LocalInfo when we actually wanted to         */
/* retain this information                                                        */
/*--------------------------------------------------------------------------------*/




 /*********************************************************************
 * @purpose           Sets the local address flag valid
 *
 *
 * @param connId      @b{(input)}  TCP/UDP connection handle returned by
 *                                 TcpUdpBind_Open
 *
 * @returns           E_OK	     - Success
 * @returns           Not E_OK  - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TcpUdpBind_SetLocalAddressValid(t_Handle connId)
{
   t_ConnInfo *connInfo = (t_ConnInfo *)connId;

   if(!connInfo || connInfo->connId < 0 || !connInfo->sockObj ||
      connInfo->validStatus != SOC_STAT_Valid)
      return E_FAILED;

   /* Local address is valid now */
   connInfo->flags |= COMM_F_LA_VALID;

   return E_OK;
}


