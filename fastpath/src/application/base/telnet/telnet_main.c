/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   telnet_main.c
*
* @purpose    Outbound Telnet Implementation
*
* @component  Outbound Telnet Component
*
* @comments   none
*
* @create     02/27/2004
* @modify     08/18/2004
*
* @author     anindya
*
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/

#include "l7_telnetinclude.h"

extern telnetMapCtrl_t     telnetMapCtrl_g;
extern L7_telnetMapCfg_t   *pTelnetMapCfgData;

typedef struct {
  options_t  options;
  modes_t    modes;

#ifdef _L7_OS_LINUX_
  term_t term;
#endif
  L7_uint32  key;
  L7_BOOL    valid;
  L7_uint32  sockout;
} session_t;

session_t session[FD_TELNET_DEFAULT_MAX_SESSIONS];


/*********************************************************************
* @purpose  Initialize all the global and static external variables
*
* @param    *telnetParams_t  telnetParams  @b((input)) Parameter structure
*
* @returns  void
*
* @notes    L7_SUCCESS
* @returns  L7_ERROR
*
* @end
*********************************************************************/

L7_RC_t telnetInitialize(telnetParams_t *telnetParams)
{
  L7_uint32 sessionIndex;

  if (telnetSessionIndexAdd(telnetParams->sock, telnetParams->sockout, &sessionIndex) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "telnetInitialize: unable to add session; socket=%d\n",telnetParams->sock);
    return L7_ERROR;
  }

  memset((void *)&session[sessionIndex].options, 0, sizeof(session[sessionIndex].options));
  session[sessionIndex].modes.debugMode   = telnetParams->modes[0];
  session[sessionIndex].modes.lineMode    = telnetParams->modes[1];
  session[sessionIndex].modes.noechoMode  = telnetParams->modes[2];

#ifdef _L7_OS_LINUX_
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  (void)tcgetattr(telnetParams->sock, &(session[sessionIndex].term.termios_exit));

  memcpy(&session[sessionIndex].term.termios_def, &session[sessionIndex].term.termios_exit, sizeof(session[sessionIndex].term.termios_exit));
  memcpy(&session[sessionIndex].term.termios_raw, &session[sessionIndex].term.termios_exit, sizeof(session[sessionIndex].term.termios_exit));
  cfmakeraw(&(session[sessionIndex].term.termios_raw));
#endif
#endif

  return L7_SUCCESS;
}



/*************************************************************************
* @purpose  Create and allocte a socket on the remote host
*
* @param    telnetParams_t   telnetParams @b((input)) Parameter structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if error
*
* @notes    none
*
* @end
**************************************************************************/

L7_int32 telnetNewCon(telnetParams_t *telnetParams)
{
  L7_int32        telnetSocketS;  /* server socket */
  struct in_addr  host; /* host IP */
  L7_char8        buf[L7_MAX_BUFFER_LENGTH];

  memset((void *)&host, 0, sizeof(host));

  if (telnetInitialize(telnetParams) == L7_ERROR)
  {
    telnetSessionIndexRemove(telnetParams->sock);
    return L7_ERROR;
  }

  if (telnetServerIpGet(telnetParams, &host) == L7_ERROR)
  {
    telnetSessionIndexRemove(telnetParams->sock);
    return L7_ERROR;
  }

  bzero(buf, sizeof(buf));
  sprintf(buf, "\r\nTrying %s...\r\n", telnetParams->ipAddr);
  osapiWrite(telnetParams->sockout, buf, strlen(buf));

  telnetSocketS = telnetRemoteConnect(host, telnetParams->port);
  if (telnetSocketS != L7_ERROR)
  {
    bzero(buf, sizeof(buf));
    sprintf(buf, "\r\nConnected to %s", telnetParams->ipAddr);
    osapiWrite(telnetParams->sockout, buf, strlen(buf));
  }
  else
  {
    telnetSessionIndexRemove(telnetParams->sock);
  }

  return telnetSocketS;

}


/*********************************************************************
* @purpose  Convert the hostname to the corresponding IP address
*
* @param    telnetParams        @b{(input)}  Pointer to parameter list
* @param    pHost               @b{(input)}  Pointer to host
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetServerIpGet(telnetParams_t *telnetParams, struct in_addr *pHost)
{
  L7_char8 buf[L7_MAX_BUFFER_LENGTH];


#ifdef _L7_OS_LINUX_
  struct hostent  *pHostent;

  if ((pHostent = gethostbyname(telnetParams->ipAddr)) == L7_NULL)
  {
    bzero(buf, sizeof(buf));
    strcpy(buf, "\r\n\r\nHost name lookup failure.");
    osapiWrite(telnetParams->sockout, buf, strlen(buf));
    return L7_ERROR;
  }
  else
  {
    memcpy(pHost, pHostent->h_addr, sizeof(struct in_addr));
    return L7_SUCCESS;
  }
#endif

#ifdef _L7_OS_VXWORKS_
  L7_ulong32 addr;

  if ((addr = inet_addr(telnetParams->ipAddr)) == -1)
  {
    if ((addr = hostGetByName(telnetParams->ipAddr)) == L7_NULL)
    {
      bzero(buf, sizeof(buf));
      strcpy(buf, "\r\n\r\nHost name lookup failure.");
      osapiWrite(telnetParams->sockout, buf, strlen(buf));
      return L7_ERROR;
    }
    else
    {
      pHost->s_addr = osapiHtonl(addr);
      return L7_SUCCESS;
    }
  }
  else
  {
    pHost->s_addr = osapiHtonl(addr);
    return L7_SUCCESS;
  }
#endif
}


/*********************************************************************
* @purpose  Allocate a socket and connect to it
*
* @param    addr         @b{(input)} IP address of the server host
* @param    port         @b{(input)} Port number of the server host
*
* @returns  Socket created
* @returns  L7_ERROR, if error
*
* @notes    none
*
* @end
*********************************************************************/

L7_int32 telnetRemoteConnect(struct in_addr addr, L7_uint32 port)
{
  L7_uint32             telnetSocketS;
  L7_RC_t               obtRc;
  L7_sockaddr_in_t         saddr;


  obtRc = osapiSocketCreate(L7_AF_INET, L7_SOCK_STREAM, 0, &telnetSocketS);
  if (obtRc == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "telnetRemoteConnect: unable to create server socket\n");
    return L7_ERROR;
  }

  memset((void *)&saddr, 0, sizeof(saddr));
  saddr.sin_family = L7_AF_INET;
  saddr.sin_port = osapiHtons(port);
  saddr.sin_addr.s_addr = addr.s_addr;
  obtRc = osapiConnect(telnetSocketS, (L7_sockaddr_t *)&saddr, sizeof(saddr));
  if (obtRc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "telnetRemoteConnect: unable to connect to remote host\n");
    /* Close the socket connection */
    osapiSocketClose(telnetSocketS);
    return L7_ERROR;
  }

  pTelnetMapCfgData->telnet.telnetNoOfActiveSessions++;  /* no of active OBT sessions */
  return telnetSocketS;
}


/*********************************************************************
* @purpose  Set the outbound telnet operational mode
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetConModeSet(ioHandle_t *ioInfo)
{
  L7_char8  buf[L7_MAX_BUFFER_LENGTH];
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return;

  if (session[sessionIndex].modes.lineMode == L7_FALSE)
  {
    bzero(buf, sizeof(buf));
    strcpy(buf, "Entering character mode...\r\nEscape character is'^^'.\r\n");
    osapiWrite(ioInfo->clientSockWrite, buf, strlen(buf));

    telnetCharModeSet(ioInfo);
  }
  else
  {
    bzero(buf, sizeof(buf));
    strcpy(buf, "Entering linemode...\r\nEscape character is'^^'.\r\n");
    osapiWrite(ioInfo->clientSockWrite, buf, strlen(buf));

    telnetLineModeSet(ioInfo);
  }
}


/************************************************************************
* @purpose  Set Outbound Telnet operational mode as CHARACTER mode
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    void
*
* @end
************************************************************************/

L7_RC_t telnetCharModeSet(ioHandle_t *ioInfo)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

#ifdef _L7_OS_LINUX_
  if (session[sessionIndex].modes.noechoMode == L7_TRUE)
  {
    session[sessionIndex].term.termios_raw.c_lflag &= ~ECHO;
  }
  else
  {
    session[sessionIndex].term.termios_raw.c_lflag |= ECHO;
  }
  /* PTin removed: serial port */
  #if (L7_SERIAL_COM_ATTR)
  (void)tcsetattr(ioInfo->clientSock, TCSAFLUSH, &(session[sessionIndex].term.termios_raw));
  #endif

  return L7_SUCCESS;
#endif

#ifdef _L7_OS_VXWORKS_
  if (session[sessionIndex].modes.noechoMode == L7_FALSE)
  {
    (void)ioctl (ioInfo->clientSock, FIOOPTIONS, OPT_RAW | OPT_ECHO);
  }
  else
  {
    (void)ioctl (ioInfo->clientSock, FIOOPTIONS, OPT_RAW);
  }

  return L7_SUCCESS;
#endif
}


/***********************************************************************
* @purpose  Set Outbound Telnet operational mode as LINE mode
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    void
*
* @end
************************************************************************/

L7_RC_t telnetLineModeSet(ioHandle_t *ioInfo)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

#ifdef _L7_OS_LINUX_
  if (session[sessionIndex].modes.noechoMode == L7_TRUE)
  {
    session[sessionIndex].term.termios_def.c_lflag &= ~ECHO;
  }
  else
  {
    session[sessionIndex].term.termios_def.c_lflag |= ECHO;
  }
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  if (tcsetattr(ioInfo->clientSock, TCSAFLUSH, &(session[sessionIndex].term.termios_def)) < 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "telnetLineModeSet: unable to set line mode terminal parameters; socket=%d\n",ioInfo->clientSock);
  }
#endif

  return L7_SUCCESS;
#endif

#ifdef _L7_OS_VXWORKS_
  if (session[sessionIndex].modes.noechoMode == L7_TRUE)
  {
    if (ioctl (ioInfo->clientSock, FIOOPTIONS, OPT_TERMINAL & ~OPT_ECHO) < 0)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
              "telnetLineModeSet: unable to set line mode terminal parameters\n");
  }
  else if (ioctl (ioInfo->clientSock, FIOOPTIONS, OPT_TERMINAL) < 0)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
              "telnetLineModeSet: unable to set line mode terminal parameters\n");

  return L7_SUCCESS;
#endif
}


/***********************************************************************
* @purpose  Reset the switch mode before exiting Outbound Telnet
*
* @param    clientSock          @b{(input)}  Client socket
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    void
*
* @end
************************************************************************/

L7_RC_t telnetExitModeSet(L7_uint32 clientSock)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

#ifdef _L7_OS_LINUX_
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  (void)tcsetattr(clientSock, TCSANOW, &(session[sessionIndex].term.termios_exit));
#endif
#endif

#ifdef _L7_OS_VXWORKS_
  (void)ioctl (clientSock, FIOOPTIONS, OPT_TANDEM);
#endif

  (void)telnetSessionIndexRemove(clientSock);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Process the data/control information coming from the
*           client host
*
* @param    clientSock      @b{(input)}  Client socket
* @param    serverSock          @b{(input)}  Server socket
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetClientInputHandle(L7_uint32 clientSock, L7_uint32 serverSock)
{
  L7_RC_t    obtRc;
  L7_int32   i,j;
  L7_short16 len;
  ioHandle_t ioInfo;
  L7_char8   buf[L7_MAX_BUFFER_LENGTH];
  L7_uchar8  *pBuf;
  L7_uint32 sessionIndex;

  if (telnetSessionIndexFind(clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  memset((void *)&ioInfo, 0, sizeof(ioHandle_t));
  memset((void *)buf, 0, sizeof(buf));

  len = osapiRead(clientSock, buf, sizeof(buf));

  if (len <= 0)
    return L7_FAILURE;

  ioInfo.clientSock = clientSock;   /* client socket */
  ioInfo.serverSock = serverSock;   /* server socket */

  /* global buffer */
  ioInfo.buf = buf;
  ioInfo.len = len;

  pBuf = ioInfo.buf;  /* buffer pointer */

  for (i = len; i > 0; i--, pBuf++)
  {
    if (*pBuf == 0x1e) /* escape sequence */
    {
      obtRc = telnetConEscape(&ioInfo);
      return (obtRc);
    }

    if (*pBuf == 0xff)  /* ignoring ASCII code 255 */
      *pBuf = 0x7f;
  }

  for (i=0; i<len; i++)
  {
   if( buf[i] == 13 && ( buf[i+1] == 10 || buf[i+1] == 0))
   {
     for(j = i+1; j <len-1;j++)
     {
       buf[j] = buf[j+1];
     }
     if ((i+1) < (len-1))
     {
       len--;
     }
   }
  }

  osapiWrite(serverSock, buf, len);
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Process the data/control information coming from the
            server host
*
* @param    clientSock      @b{(input)}  Client socket
* @param    serverSock          @b{(input)}  Server socket
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetServerInputHandle(L7_uint32 clientSock, L7_uint32 serverSock)
{
  L7_int32   i;
  L7_int32   cstart = 0;
  L7_short16 len;
  L7_uchar8  ch;
  L7_char8   buf[L7_MAX_BUFFER_LENGTH];
  L7_char8   nullBuf[L7_MAX_BUFFER_LENGTH];
  static ioHandle_t ioInfo;
  L7_uint32 sessionIndex;

  if (telnetSessionIndexFind(clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  bzero(buf, sizeof(buf));
  bzero(nullBuf, sizeof(nullBuf));

  ioInfo.clientSock = clientSock;   /* client socket */
  ioInfo.serverSock = serverSock;   /* server socket */
  ioInfo.clientSockWrite = session[sessionIndex].sockout;

  len = osapiRead(serverSock, buf, sizeof(buf));

  if (len <= 0)
    return L7_FAILURE;

  /* global buffer */
  ioInfo.buf = buf;
  ioInfo.len = len;
  ioInfo.iacbuf = nullBuf;

  for (i = 0; i < len; i++)
  {
    ch = ioInfo.buf[i];

    if (ioInfo.telstate == 0)    /* state == 0 for most of the time */
    {
      if (ch == IAC)
      {
        cstart = i;
        ioInfo.telstate = TS_IAC;
      }
    }
    else
    {
      switch (ioInfo.telstate)
      {
      case TS_0:
               if (ch == IAC)
                 ioInfo.telstate = TS_IAC;
               else
                 ioInfo.buf[cstart++] = ch;

               break;
      case TS_IAC:
               if (ch == IAC)       /*  IAC IAC => 0xFF */
               {
                 ioInfo.buf[cstart++] = ch;
                 ioInfo.telstate = TS_0;
                 break;
               }
               /* else */
               switch (ch)
               {
               case SB:
                      ioInfo.telstate = TS_SUB1;
                      break;
               case DO:
               case DONT:
               case WILL:
               case WONT:
                     ioInfo.telwish =  ch;
                     ioInfo.telstate = TS_OPT;
                     break;
               default:
                     ioInfo.telstate = TS_0;
                     break;
               }
               break;
      case TS_OPT: /* WILL, WONT, DO, DONT */
               telnetOptionsMode(&ioInfo, ch);
               ioInfo.telstate = TS_0;
               break;
      case TS_SUB1:  /* Subnegotiation */
      case TS_SUB2:  /* Subnegotiation */
               if (telnetSubnegMode(&ioInfo, ch) == L7_SUCCESS)
                 ioInfo.telstate = TS_0;
               break;
      }
    }
  }

  if (ioInfo.telstate)
  {
    if (ioInfo.iaclen)
      telnetBufferFlush(&ioInfo);

    if (ioInfo.telstate == TS_0)
      ioInfo.telstate = 0;

    ioInfo.len = cstart;
  }

  if (ioInfo.len)
    osapiWrite(ioInfo.clientSockWrite, buf, ioInfo.len);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Handle the outbound telnet escape sequence commands
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetConEscape(ioHandle_t *ioInfo)
{
  L7_char8 ch;
  L7_char8 buf[L7_MAX_BUFFER_LENGTH];
  L7_char8 iobuf[L7_MAX_BUFFER_LENGTH];
  L7_uint32 sessionIndex;

  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "OBT: Error finding session for socket %d for escape processing.", ioInfo->clientSock);
    return L7_FAILURE;
  }

  if (session[sessionIndex].modes.lineMode == L7_TRUE)
    telnetCharModeSet(ioInfo);

  if (osapiRead(ioInfo->clientSock, &ch, 1) <= 0)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "OBT: Error reading socket %d for escape processing in session %d.", ioInfo->clientSock, sessionIndex);
    return L7_FAILURE;
  }

  /* outbound telnet commands */
  switch (ch)
  {
  case 'b': /* break */
  case 'B':
          bzero(iobuf, sizeof(iobuf));
          ioInfo->iacbuf = iobuf;
          telnetBufferFlush(ioInfo);
          telnetBufferAdd(ioInfo, IAC);
          telnetBufferAdd(ioInfo, BREAK);
          telnetBufferFlush(ioInfo);
          bzero(buf, sizeof(buf));
          strcpy(buf, "\n");
          osapiWrite(ioInfo->serverSock, buf, 1);
          break;
  case 'c': /* interrupt process */
  case 'C':
          bzero(iobuf, sizeof(iobuf));
          ioInfo->iacbuf = iobuf;
          telnetBufferFlush(ioInfo);
          telnetBufferAdd(ioInfo, IAC);
          telnetBufferAdd(ioInfo, IP);
          telnetBufferFlush(ioInfo);

          break;
  case 'h': /* erase character */
  case 'H':
          bzero(iobuf, sizeof(iobuf));
          ioInfo->iacbuf = iobuf;
          telnetBufferFlush(ioInfo);
          telnetBufferAdd(ioInfo, IAC);
          telnetBufferAdd(ioInfo, EC);
          telnetBufferFlush(ioInfo);

          break;
  case 't': /* are you there */
  case 'T':
          bzero(iobuf, sizeof(iobuf));
          ioInfo->iacbuf = iobuf;
          telnetBufferFlush(ioInfo);
          telnetBufferAdd(ioInfo, IAC);
          telnetBufferAdd(ioInfo, AYT);
          telnetBufferFlush(ioInfo);
          bzero(buf, sizeof(buf));
          strcpy(buf, "\n");
          osapiWrite(ioInfo->serverSock, buf, 1);

          break;
  case 'u': /* erase line */
  case 'U':
          bzero(iobuf, sizeof(iobuf));
          ioInfo->iacbuf = iobuf;
          telnetBufferFlush(ioInfo);
          telnetBufferAdd(ioInfo, IAC);
          telnetBufferAdd(ioInfo, EL);
          telnetBufferFlush(ioInfo);

          break;
  case 'e': /* exit outbound telnet */
  case 'E':
          bzero(buf, sizeof(buf));
          strcpy(buf, "\r\n");
          osapiWrite(ioInfo->clientSockWrite, buf, strlen(buf));
          return L7_FAILURE;

          break; /* not reached */
  case '?': /* outbound telnet help */
          bzero(buf, sizeof(buf));
          strcpy(buf, "\r\n[Special telnet escape help]\r\n"
                       " ^^B  sends telnet BREAK\r\n"
                       " ^^C  sends telnet IP\r\n"
                       " ^^H  sends telnet EC\r\n"
                       " ^^T  sends telnet AYT\r\n"
                       " ^^U  sends telnet EL\r\n"
                       " ^^E  exit current telnet session\r\n"
                       " ^^?  displays telnet commands\r\n");
          osapiWrite(ioInfo->clientSockWrite, buf, strlen(buf));
          bzero(buf, sizeof(buf));
          strcpy(buf, "\n");
          osapiWrite(ioInfo->serverSock, buf, 1);

          break;
  default:
          break;
  }

  if (session[sessionIndex].modes.lineMode == L7_TRUE)
    telnetLineModeSet(ioInfo);

  return L7_SUCCESS;
}



/*************************************************************************
* @purpose  Process different telnet options
*
* @param    ioInfo          @b{(input)} Pointer to i/o structure
* @param    telopt          @b{(input)} Telnet option
*
* @returns  void
*
* @notes    void
*
* @end
**************************************************************************/

void telnetOptionsMode(ioHandle_t *ioInfo, L7_uchar8 telopt)
{
  switch (telopt)
  {
  case TELOPT_BINARY:
                    switch (ioInfo->telwish)
                    {
                    case DO:
                    case DONT:
                             telnetSendBinary(ioInfo);
                             break;
                    case WILL:
                    case WONT:
                             telnetReceiveBinary(ioInfo);
                             break;
                    default:
                             break;
                    }
                    break;
  case TELOPT_ECHO:
                   telnetEcho(ioInfo);
                   break;
  case TELOPT_SGA:
                   telnetSga(ioInfo);
                   break;
  case TELOPT_STATUS:
                   telnetStatus(ioInfo);
                   break;
  case TELOPT_TTYPE:
                   telnetTtype(ioInfo);
                   break;
  default:
                   telnetNotSupported(ioInfo, telopt);
                   break;
  }
}

/*************************************************************************
* @purpose  Process different telnet suboption negotiation
*
* @param    ioInfo       @b{(input)} Pointer to i/o structure
* @param    tel          @b{(input)} Telnet option/command/state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    void
*
* @end
**************************************************************************/

L7_RC_t telnetSubnegMode(ioHandle_t *ioInfo, L7_uchar8 tel)
{
  switch (ioInfo->telstate)
  {
  case TS_SUB1:
              switch (tel)
              {
              case TELOPT_TTYPE:
                      telnetTtypeSend(ioInfo);
                      break;
              case IAC:
                      ioInfo->telstate = TS_SUB2;
                      break;
              default:
                      break;
              }
  case TS_SUB2:
              if (tel == SE)
                return L7_SUCCESS;

              ioInfo->telstate = TS_SUB1;
  }
  return L7_FAILURE;
}



/*************************************************************************
* @purpose  Send reply back to the server for telnet BINARY option
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Client sends reply corresponding to WILL/WONT from server host
*
* @end
**************************************************************************/

L7_RC_t telnetReceiveBinary(ioHandle_t *ioInfo)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ioInfo->telwish)
  {
  case DO:
         telnetBuffer2Add(ioInfo, WONT, TELOPT_BINARY);
         break;
  case DONT:
         break;
  case WILL:
         if (session[sessionIndex].options.rcvbinary == L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, DO, TELOPT_BINARY);
           session[sessionIndex].options.rcvbinary = L7_TRUE;
         }
         break;
  case WONT:
         if (session[sessionIndex].options.rcvbinary != L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, DONT, TELOPT_BINARY);
           session[sessionIndex].options.rcvbinary = L7_FALSE;
         }
  default:
         break;
  }
  return L7_SUCCESS;
}



/*************************************************************************
* @purpose  Send reply back to the server for telnet BINARY option
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Client sends reply corresponding to DO/DONT from server host
*
* @end
**************************************************************************/

L7_RC_t telnetSendBinary(ioHandle_t *ioInfo)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ioInfo->telwish)
  {
  case DO:
         if (session[sessionIndex].options.sndbinary == L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, WILL, TELOPT_BINARY);
           session[sessionIndex].options.sndbinary = L7_TRUE;
         }
         break;
  case DONT:
         if (session[sessionIndex].options.sndbinary != L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, WONT, TELOPT_BINARY);
           session[sessionIndex].options.sndbinary = L7_FALSE;
         }
         break;
  default:
         break;
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Send reply back to the server for telnet ECHO option
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetEcho(ioHandle_t *ioInfo)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  if (ioInfo->telwish == DO)
  {
    session[sessionIndex].modes.noechoMode = L7_FALSE;
    telnetBuffer2Add(ioInfo, WILL, TELOPT_ECHO);
  }
  else if (ioInfo->telwish == DONT)
  {
    session[sessionIndex].modes.noechoMode = L7_TRUE;
    telnetBuffer2Add(ioInfo, WONT, TELOPT_ECHO);
  }

  if (session[sessionIndex].modes.lineMode == L7_FALSE)
    telnetCharModeSet(ioInfo);
  else
    telnetLineModeSet(ioInfo);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Send reply back to the server for telnet SUPPRESS
*           GO-AHEAD option
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Server host always sends will/wont, client do/dont
*
* @end
*********************************************************************/

L7_RC_t telnetSga(ioHandle_t *ioInfo)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ioInfo->telwish)
  {
  case DO:
         telnetBuffer2Add(ioInfo, WONT, TELOPT_SGA);
         break;
  case DONT:
         break;
  case WILL:
         if (session[sessionIndex].options.sga == L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, DO, TELOPT_SGA);
           session[sessionIndex].options.sga = L7_TRUE;
         }
         break;
  case WONT:
         if (session[sessionIndex].options.sga != L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, DONT, TELOPT_SGA);
           session[sessionIndex].options.sga = L7_FALSE;
         }
         break;
  default:
         break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send reply back to the server for telnet STATUS option
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetStatus(ioHandle_t *ioInfo)
{
  L7_char8 buf[L7_MAX_BUFFER_LENGTH];
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ioInfo->telwish)
  {
  case WILL:
           telnetBuffer2Add(ioInfo, DO, TELOPT_STATUS);

           telnetBufferAdd(ioInfo, IAC);
           telnetBufferAdd(ioInfo, SB);
           telnetBufferAdd(ioInfo, TELOPT_STATUS);
           telnetBufferAdd(ioInfo, TELQUAL_SEND);
           telnetBufferAdd(ioInfo, IAC);
           telnetBufferAdd(ioInfo, SE);

           telnetBufferFlush(ioInfo);
           break;
  case WONT:
           telnetBuffer2Add(ioInfo, DONT, TELOPT_STATUS);
           break;
  default:
           telnetBuffer2Add(ioInfo, WONT, TELOPT_STATUS);
           break;
  }

  telnetConModeSet(ioInfo);
  bzero(buf, sizeof(buf));
  strcpy(buf, "\r\n");
  osapiWrite(ioInfo->clientSockWrite, buf, sizeof(buf));

  if (session[sessionIndex].modes.debugMode == L7_TRUE)
    telnetDebugShow(ioInfo);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Send reply back to the server for telnet TERMINAL TYPE
*           option
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Server host always sends do/dont, client will/wont
*
* @end
**********************************************************************/

L7_RC_t telnetTtype(ioHandle_t *ioInfo)
{
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ioInfo->telwish)
  {
  case DO:
         if (session[sessionIndex].options.termtype == L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, WILL, TELOPT_TTYPE);
           session[sessionIndex].options.termtype = L7_TRUE;
         }
         break;
  case DONT:
         if (session[sessionIndex].options.termtype != L7_FALSE)
         {
           telnetBuffer2Add(ioInfo, WONT, TELOPT_TTYPE);
           session[sessionIndex].options.termtype = L7_FALSE;
         }
         break;
  default:
         break;
  }
/* Binary Transmission is not supported by SunOS

  if (session[sessionIndex].options.termtype == L7_TRUE)
  {
    ioInfo->telwish = WILL;
    telnetReceiveBinary(ioInfo);
    ioInfo->telwish = DO;
    telnetSendBinary(ioInfo);
  }*/

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Send the terminal type of the client to the server
*
* @param    * @param    ioInfo      @b{(input)}  Pointer to i/o structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetTtypeSend(ioHandle_t *ioInfo)
{
  L7_short16 i;
  L7_uchar8  *termType; /* terminal name */

  termType = getenv("TERM"); /* get terminal type */
  if (termType == L7_NULLPTR )
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TELNET_MAP_COMPONENT_ID,
            "telnetTtype: unable to get terminal type...defaulting to \"ansi\"\n");
    termType = "ansi";      /* assigning default terminal type */
  }

  telnetBufferAdd(ioInfo, IAC);
  telnetBufferAdd(ioInfo, SB);
  telnetBufferAdd(ioInfo, TELOPT_TTYPE);
  telnetBufferAdd(ioInfo, TELQUAL_IS);

  for (i=0;i<=strlen(termType);i++)
  {
    telnetBufferAdd(ioInfo, *(termType+i));
  }

  telnetBufferAdd(ioInfo, IAC);
  telnetBufferAdd(ioInfo, SE);

  telnetBufferFlush(ioInfo);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Send reply back to the server for unsupported telnet options
*
* @param    ioInfo          @b{(input)} Pointer to i/o structure
* @param    telopt          @b{(input)} Telnet option
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetNotSupported(ioHandle_t *ioInfo, L7_char8 telopt)
{
  if (ioInfo->telwish == WILL)
    telnetBuffer2Add(ioInfo, DONT, telopt);
  else if (ioInfo->telwish == DO)
      telnetBuffer2Add(ioInfo, WONT, telopt);
}



/*********************************************************************
* @purpose  Add a single character to the buffer
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
* @param    telopt          @b{(input)} Character to add
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetBufferAdd(ioHandle_t *ioInfo, L7_uchar8 telopt)
{
  ioInfo->iacbuf[(ioInfo->iaclen)++] = telopt;
}

/*********************************************************************
* @purpose  Add two characters to the buffer
*
* @param    ioInfo          @b{(input)} Pointer to i/o structure
* @param    tewish          @b{(input)} DO/DONT/WISH/WONT
* @param    telopt          @b{(input)} telnet option
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/

void telnetBuffer2Add(ioHandle_t *ioInfo, L7_uchar8 telwish, L7_uchar8 telopt)
{
  telnetBufferAdd(ioInfo, IAC);
  telnetBufferAdd(ioInfo, telwish);
  telnetBufferAdd(ioInfo, telopt);

  telnetBufferFlush(ioInfo);
}


/*********************************************************************
* @purpose  Flush the buffer content to the server host
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetBufferFlush(ioHandle_t *ioInfo)
{
  osapiWrite(ioInfo->serverSock, ioInfo->iacbuf, ioInfo->iaclen);
  ioInfo->iaclen = 0;
}



/*********************************************************************
* @purpose  Displays the Outbound Telnet options in effect
*
* @param    ioInfo          @b{(input)}  Pointer to i/o structure
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetDebugShow(ioHandle_t *ioInfo)
{
  L7_char8 buf[L7_MAX_BUFFER_LENGTH];
  L7_uint32 sessionIndex;
  if (telnetSessionIndexFind(ioInfo->clientSock, &sessionIndex) != L7_SUCCESS)
    return;

  bzero(buf, sizeof(buf));
  strcpy(buf, "Outbound Telnet options in effect are...");

  if (session[sessionIndex].options.termtype == L7_TRUE)
    strcat(buf, "\r\n    WILL TERMINAL-TYPE    ");

  if (session[sessionIndex].options.rcvbinary == L7_TRUE)
    strcat(buf, "\r\n    DO BINARY    ");

  if (session[sessionIndex].options.sndbinary == L7_TRUE)
    strcat(buf, "\r\n    WILL BINARY    ");

  strcat(buf, "\r\n    DO ECHO    ");

  if (session[sessionIndex].modes.noechoMode == L7_FALSE)
    strcat(buf, "\r\n    WILL ECHO    ");

  if (session[sessionIndex].options.sga == L7_TRUE)
    strcat(buf, "\r\n    DO SUPPRESS GO AHEAD    ");

  strcat(buf, "\r\n    DO STATUS    \r\n\r\n");

  osapiWrite(ioInfo->clientSockWrite, buf, strlen(buf));
}


/*********************************************************************
* @purpose  Close an Outbound Telnet session
*
* @param    clientSock          @b{(input)}  Client socket
* @param    serverSock          @b{(input)}  Server socket
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/

void telnetDisconnect(L7_uint32 clientSock, L7_uint32 serverSock)
{
  telnetExitModeSet(clientSock);
  osapiSocketClose(serverSock);
  pTelnetMapCfgData->telnet.telnetNoOfActiveSessions--;  /* no of active OBT sessions */
}

/*********************************************************************
* @purpose  Find index for an Outbound Telnet session
*
* @param    clientSock            @b{(input)}   Client socket
* @param    sessionIndex        @b{(output)}  Pointer to index
*
* @returns  L7_TRUE             index found
* @returns  L7_FALSE            index not found
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t telnetSessionIndexFind(L7_uint32 clientSock, L7_uint32 *sessionIndex)
{
  L7_uint32 index;
  L7_RC_t found = L7_FAILURE;

  /* find term associated with clientSock */
  for (index = 0; index<FD_TELNET_DEFAULT_MAX_SESSIONS; index++)
  {
    if (session[index].key == clientSock && session[index].valid == L7_TRUE)
    {
      found = L7_SUCCESS;
      *sessionIndex=index;
      break;
    }
  }
  return found;
}

/*********************************************************************
* @purpose  Delete index for an Outbound Telnet session
*
* @param    clientSock            @b{(input)}  Client socket
*
* @returns  L7_TRUE             index found
* @returns  L7_FALSE            index not found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetSessionIndexRemove(L7_uint32 clientSock)
{
  L7_uint32 index;
  L7_RC_t found = L7_FAILURE;

  /* find term associated with clientSock */
  for (index = 0; index<FD_TELNET_DEFAULT_MAX_SESSIONS; index++)
  {
    if (session[index].key == clientSock && session[index].valid == L7_TRUE)
    {
      found = L7_SUCCESS;
      memset(&session[index], 0, sizeof(session_t));
      break;
    }
  }
  return found;
}

/*********************************************************************
* @purpose  Add index for an Outbound Telnet session
*
* @param    clientSock            @b{(input)}   Client socket
* @param    sessionIndex        @b{(output)}  Pointer to index
*
* @returns  L7_TRUE             index added
* @returns  L7_FALSE            index not added
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetSessionIndexAdd(L7_uint32 clientSock, L7_uint32 clientSockWrite, L7_uint32 *sessionIndex)
{
  L7_uint32 index;
  L7_RC_t added = L7_FAILURE;

  if (telnetSessionIndexFind(clientSock,sessionIndex) == L7_SUCCESS)
    return added;

  /* find term associated with clientSock */
  for (index = 0; index<FD_TELNET_DEFAULT_MAX_SESSIONS; index++)
  {
    if (session[index].valid == L7_FALSE)
    {
      memset(&session[index], 0, sizeof(session_t));
      session[index].key   = clientSock;
      session[index].valid = L7_TRUE;
      session[index].sockout = clientSockWrite;
      added = L7_SUCCESS;
      *sessionIndex=index;
      break;
    }
  }
  return added;
}

/*********************************************************************
* @purpose  Delete all stored indexes for Outbound Telnet sessions
*
* @param    void
*
* @returns  L7_TRUE             index added
* @returns  L7_FALSE            index not added
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t telnetSessionRemoveAll(void)
{
  L7_uint32 index;

  /* find term associated with clientSock */
  for (index = 0; index<FD_TELNET_DEFAULT_MAX_SESSIONS; index++)
  {
    memset(&session[index], 0, sizeof(session_t));
  }
  return L7_SUCCESS;
}
