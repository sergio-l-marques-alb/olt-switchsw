
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  tftp_client.c
*
* @purpose   This file implements TFTP functions.
*
* @component software
*
* @comments
*
* @create
*
* @author    Kunal Kapila
*
* @end
*
*********************************************************************/

#include <string.h>
#include "l7_tftp.h"   /*TFTP Header file*/
#include "log.h"
#include "osapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "osapi_support.h"
/* #include "ipcom_bsdsock.h" */
#include "tftp_client_debug.h"
#include "l7_socket.h"
#include "errno.h"

#define TFTP_WRITE_BUFFER_SIZE  (1024 * 16)
static L7_uint32 tftpNumBytesInBuffer = 0;
static L7_int32  tftpBufferedFileDescriptor = 0;
static L7_uchar8 tftpRxBuffer [TFTP_WRITE_BUFFER_SIZE];

/*********************************************************************
* @purpose  Write data to a file
* @param    fd         @b{(input)}  file descriptor of file
* @param    *newbuffer @b{(input)}  buffer which contains data
* @param    datalength @b{(input)}  length of data which is to be written
*
* @returns  Number of bytes written to the file.
*
* @end
*********************************************************************/
static L7_int32 tftpFileWrite (L7_int32 fd, L7_TFTP_CALLBACK_t callback, 
                               void *cbarg, L7_char8 * buf, L7_int32 len)
{
  L7_int32 count;
  if(callback != L7_NULLPTR)
  {
    count = callback(cbarg, buf, len);
  }
  else
  {
    count=osapiWrite(fd, buf, len);
  }
  return count;
}

/*********************************************************************
* @purpose  read data from a file
* @param    fd         @b{(input)}  file descriptor of file
* @param    *newbuffer @b{(input)}  buffer which contains data
* @param    datalength @b{(input)}  length of data which is to be written
*
* @returns  Number of bytes written to the file.
*
* @end
*********************************************************************/
static L7_int32 tftpFileRead (L7_int32 fd, L7_TFTP_CALLBACK_t callback, 
                               void *cbarg, L7_char8 * buf, L7_int32 len)
{
  L7_int32 count;
  if(callback != L7_NULLPTR)
  {
    count = callback(cbarg, buf, len);
  }
  else
  {
    count=osapiRead(fd, buf, len);
  }
  return count;
}

/*********************************************************************
* @purpose  Write data to a buffer. If buffer is full then write th
*           buffer to a file.
* @param    fd         @b{(input)}  file descriptor of file
* @param    *newbuffer @b{(input)}  buffer which contains data
* @param    datalength @b{(input)}  length of data which is to be written
*
* @returns  Number of bytes written to the file.
*
* @notes    This implementation buffers only one file transfer.
*
* @end
*********************************************************************/
static L7_int32 tftpBufferWrite (L7_int32 fd, L7_char8 * newbuffer,
                                 L7_int32 datalength, 
                                 L7_TFTP_CALLBACK_t callback, void *cbarg)
{
  L7_int32 count;
  if (tftpNumBytesInBuffer == 0)
  {
    tftpBufferedFileDescriptor = fd;
  }
  if (fd != tftpBufferedFileDescriptor)
  {
    count=tftpFileWrite(fd, callback, cbarg, newbuffer, datalength);
    return count;
  }
  if ((tftpNumBytesInBuffer + datalength) >  TFTP_WRITE_BUFFER_SIZE)
  {
    count=tftpFileWrite(fd, callback, cbarg, tftpRxBuffer, tftpNumBytesInBuffer);
    tftpNumBytesInBuffer = 0;
  }

  memcpy (&tftpRxBuffer[tftpNumBytesInBuffer], newbuffer, datalength);
  tftpNumBytesInBuffer += datalength;
  return datalength;
}

/*********************************************************************
* @purpose  Write remaining bytes in the buffer to the file.
*
* @param    fd         @b{(input)}  file descriptor of file
*
* @returns  L7_SUCCESS - Buffer written successfully
* @returns  L7_FAILURE - Buffer write failed.
*
* @notes    none
*
* @end
*********************************************************************/
static L7_TRANSFER_STATUS_t tftpBufferFlush (L7_int32 fd, L7_TFTP_CALLBACK_t callback, void *cbarg)
{
  L7_RC_t   rc = L7_FLASH_FAILED;
  L7_uint32 bytesWritten;

  if (fd != tftpBufferedFileDescriptor)
  {
    return rc;
  }

  if (tftpNumBytesInBuffer == 0)
  {
    return L7_TRANSFER_SUCCESS;
  }

  bytesWritten = tftpFileWrite(fd, callback, cbarg, tftpRxBuffer, tftpNumBytesInBuffer);

  if (bytesWritten == tftpNumBytesInBuffer)
  {
    rc = L7_TRANSFER_SUCCESS;
  }

  tftpNumBytesInBuffer = 0;
  return rc;
}

/*********************************************************************
* @purpose  To Get or put an ascii or binary file from/to the TFTP server
* @param    inet_address  @b{(input)}  inet-address of TFTP server
* @param    pFilename    @b{(input)}  name of the file
* @param    pCommand     @b{(input)}  command which is to be executed
* @param    pMode        @b{(input)}  mode of file which is to be transferred
* @param    fd           @b{(input)}  file descriptor
* @param    requestDelay @b{(input)}  delay between requests
* @returns  L7_SUCCESS or L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_TRANSFER_STATUS_t L7_TFTP_COPY (L7_inet_addr_t *inet_address,L7_char8 *pFilename,
                                   L7_char8 *pCommand, L7_char8 *pMode,L7_int32 fd,
                                   L7_uint8 requestDelay, L7_TFTP_CALLBACK_t callback, void *cbarg)
{
  L7_TRANSFER_STATUS_t rc = L7_TRANSFER_SUCCESS;
  if ((pCommand[0]=='g'||pCommand[0] =='G')&&(pCommand[1]=='e'||pCommand[1] =='E')
      &&(pCommand[2]=='t'||pCommand[2] =='T'))
  {
    if ((rc = L7_TFTP_GET(inet_address,pFilename,pMode,fd,
                          requestDelay, callback, cbarg)) != L7_TRANSFER_SUCCESS)
    {
      tftpBufferFlush (fd, callback, cbarg);
      LOG_MSG("Error while getting the file");
      return rc;
    }
    rc = tftpBufferFlush (fd, callback, cbarg);
  } /*end of if*/

  else if ((pCommand[0]=='p'||pCommand[0] =='P')&&(pCommand[1]=='U'||pCommand[1] =='u')
           &&(pCommand[2]=='t'||pCommand[2] =='T'))
  {
    if (L7_FAILURE==L7_TFTP_PUT(inet_address,pFilename,pMode,fd, callback, cbarg))
    {
      LOG_MSG("Error while putting the file");
      return L7_TRANSFER_FAILED;  /*L7_FAILURE in TFTP_PUT*/

    }
  }/*end of else*/
  else
  {
    LOG_MSG("Wrong value for TFTP command");
    return L7_TRANSFER_FAILED;
  }
  return rc;
}/*end of L7_TFTP_COPY*/

/*********************************************************************
* @purpose  To build TFTP read/write request packet
* @param    opcode    @b{(input)}  TFTP opcode for read/write
* @param    filename  @b{(input)}  name of the file
* @param    Mode      @b{(input)}  mode of file which is to be transferred
* @returns  *L7_TFTP_PACKET_t       pointer to TFTP Packet structure
* @notes    none
*
* @end
*********************************************************************/
L7_TFTP_PACKET_t*
tftpRWQPacketBuild(L7_ushort16 opcode,L7_char8 *filename,
                   L7_char8 *mode, L7_TFTP_PACKET_t *tftpPacket)
{
  L7_int32 len;
  L7_int32 modeLen;

  bzero((L7_char8 *)tftpPacket,sizeof(L7_TFTP_PACKET_t));
  tftpPacket->th_opcode =  osapiHtons(opcode);
  OSAPI_STRNCPY_SAFE(tftpPacket->th_request,filename); /*put filename and mode in packet*/
  len=strlen(filename) + 1;

  /* modelen is the remaining space in th_request including an extra null */
  modeLen = sizeof(tftpPacket->th_request) - len - 1;
  osapiStrncpy(tftpPacket->th_request + len,mode,modeLen);
  return(tftpPacket);
}

/*********************************************************************
* @purpose  To build TFTP acknowledgement packet
* @param    blockNo   @b{(input)}  Block no. of the data packet
* @returns  *L7_TFTP_PACKET_t       pointer to TFTP Packet structure
* @notes    none
*
* @end
*********************************************************************/
L7_TFTP_PACKET_t*
tftpACKPacketBuild(L7_ushort16 blockNo, L7_TFTP_PACKET_t *tftpACKPacket)
{
  bzero((L7_char8 *)tftpACKPacket,sizeof(L7_TFTP_PACKET_t));
  tftpACKPacket->th_opcode =  osapiHtons(TFTP_ACK);
  tftpACKPacket->th_block = osapiHtons(blockNo);/* put block no in the Packet*/
  return(tftpACKPacket);
}

/*********************************************************************
* @purpose  To parse the received TFTP packet
* @param    receivedBuffer   @b{(input)}  Buffer which is received from server
* @returns  *L7_TFTP_PACKET_t       pointer to TFTP Packet structure
* @notes    none
*
* @end
*********************************************************************/
L7_TFTP_PACKET_t*
tftpParse(L7_char8 *receivedBuffer)
{
  L7_TFTP_PACKET_t *parsedPacket=(L7_TFTP_PACKET_t*)receivedBuffer;
  return(parsedPacket);
}

/*********************************************************************
* @purpose  To write data to the known file
* @param    fd         @b{(input)}  file descriptor of file
* @param    *newbuffer @b{(input)}  buffer which contains data
* @param    datalength @b{(input)}  length of data which is to be written
* @param    *mode      @b{(input)}  mode of file
* @returns  L7_SUCCESS or L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t fileWrite(L7_int32 fd,L7_char8 *newbuffer,L7_int32 datalength,
                  L7_char8 *mode,L7_TFTP_CALLBACK_t callback, void *cbarg)
{
  L7_int32 j,lastcr=0,count;
  L7_char8 c;
  L7_char8 *pbuffer=L7_NULLPTR;
  if ((mode[0]=='b'||mode[0] =='B')&&(mode[1]=='i'||mode[1] =='I')
      &&(mode[2]=='n'||mode[2] =='N')&&(mode[3]=='a'||mode[3] =='A')
      &&(mode[4]=='r'||mode[4] =='R')&&(mode[5]=='Y'||mode[5] =='y') )
  {
    /*file is binary*/
    count=tftpBufferWrite(fd,newbuffer,datalength, callback, cbarg);
    if (count <0)
    {
      LOG_MSG("error while writing the file");
      return L7_FAILURE;
    }
  }
  else if ((mode[0]=='o'||mode[0] =='O')&&(mode[1]=='c'||mode[1] =='C') &&
           (mode[2]=='t'||mode[2] =='T')&&(mode[3]=='e'||mode[3] =='E') &&
           (mode[4]=='t'||mode[4] =='T'))
  {
    /*file is binary*/
    count=tftpBufferWrite(fd,newbuffer,datalength, callback, cbarg);
    if (count <0)
    {
      LOG_MSG("error while writing the file");
      return L7_FAILURE;
    }
  }

  else if ((mode[0]=='a'||mode[0] =='A')&&(mode[1]=='s'||mode[1] =='S')
           &&(mode[2]=='C'||mode[2] =='c')&&(mode[3]=='i'||mode[3] =='I')
           &&(mode[4]=='i'||mode[4] =='I') )
  {                                             /*file is in ascii mode*/
    pbuffer=newbuffer;                        /* conversions require for
                                                 ascii mode*/
    for (j=0;j<datalength;j++)
    {
      c=*pbuffer++;
      if (lastcr)
      {
        if (c=='\n')
          c='\n';
        else if (c=='\0')
          c= '\r';
        else
        {
          LOG_MSG("Error while reading the buffer");
          return L7_FAILURE;
        }
        lastcr=0;
      }
      else if (c == '\r')
      {
        lastcr=1;
        continue;
      }
      if (osapiWrite(fd,&c,1) < 1)
      {
        LOG_MSG("Error while putting data into the file");
        return L7_FAILURE;
      }
    }/* end of for*/
  }/* end of else*/
  else
  {
    LOG_MSG(" Wrong value of mode entered");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Creates and binds socket for TFTP requests
*
* @param    L7_inet_addr_t @b{(input)} ip address of TFTP server
*
* @notes    none
*
* @end
*********************************************************************/
L7_int32 tftpGetSocketPrepare(L7_inet_addr_t *inet_address)
{
  L7_sockaddr_t     *baddr = L7_NULLPTR;
  L7_sockaddr_in6_t bindAddr6;
  L7_sockaddr_in_t  bindAddr4;
  L7_uint32         baddr_len = L7_NULL;
  L7_uint32         get_port  = 7700;

  L7_uint32 broadcastModeValue  = 0;
  L7_int32  clientSocket = L7_NULL;
  L7_int32  optionSetOn  = 1;
  
  L7_uint32 servPortConfigMode = L7_SYSCONFIG_MODE_NOT_COMPLETE;
  L7_uint32 networkConfigMode  = L7_SYSCONFIG_MODE_NOT_COMPLETE;
  
  L7_uchar8 bindIntfName[16];
    
  memset(bindIntfName, 0, sizeof(bindIntfName));    
    
  memset(&bindAddr4, 0, sizeof(bindAddr4));
  memset(&bindAddr6, 0, sizeof(bindAddr6));  

  if (inet_address->family == L7_AF_INET)
  {
    bindAddr4.sin_family      = L7_AF_INET;
    bindAddr4.sin_port        = osapiHtons(get_port);
    bindAddr4.sin_addr.s_addr = L7_INADDR_ANY;

    baddr = (L7_sockaddr_t *)&bindAddr4;
    baddr_len = sizeof(bindAddr4);
    
    if (inet_address->addr.ipv4.s_addr == L7_IP_LTD_BCAST_ADDR)
    {
      broadcastModeValue = 1;
    }
  }
  
  else if (inet_address->family == L7_AF_INET6)
  {
    /* check this */
    memset(&bindAddr6, 0, sizeof(bindAddr6));
    bindAddr6.sin6_family = L7_AF_INET6;
    bindAddr6.sin6_port   = osapiHtons(get_port);

    baddr = (L7_sockaddr_t *)&bindAddr6;
    baddr_len = sizeof(bindAddr6);

    if (L7_INET_IS_ADDR_BROADCAST(inet_address))
    {
      broadcastModeValue = 1;
    }
  }
  
  else
  {
    LOG_MSG("tftpSocketPrepare(): Invalid Address family");
    return L7_FAILURE;
  }

  if (osapiSocketCreate(inet_address->family,
                        L7_SOCK_DGRAM,
                        0,
                        &clientSocket ) != L7_SUCCESS)
  {
    LOG_MSG("tftpSocketPrepare(): Failed to create socket");
    return L7_NULL;
  }

  if (osapiSetsockopt(clientSocket,
                       L7_SOL_SOCKET,
                       L7_SO_REUSEADDR,
                       (L7_char8 *) &optionSetOn,
                       sizeof(optionSetOn)) != L7_SUCCESS)
  {
    LOG_MSG("tftpSocketPrepare(): osapiSetsockopt SO_REUSEADDR failed");
    osapiSocketClose (clientSocket);
    return L7_NULL;
  }

  if (inet_address->family == L7_AF_INET)
  {
    servPortConfigMode = simGetServPortConfigMode();
    networkConfigMode  = simGetSystemConfigMode();
  
#ifdef _L7_OS_VXWORKS_
    if( L7_FILE_TYPE_AUTO_INSTALL_SCRIPT == simGetTransferDownloadFileType())
    {
      /* TFTP traffic must be binded to active port */
      /* Get service port name */
      if ((servPortConfigMode == L7_SYSCONFIG_MODE_DHCP)  ||
          (servPortConfigMode == L7_SYSCONFIG_MODE_BOOTP) ||
          (simGetServPortIPAddr() != L7_NULL))
        
      {
        osapiSnprintf(bindIntfName, sizeof(bindIntfName), "%s0", bspapiServicePortNameGet());
      }
      /* Get network port name*/
      else if ((networkConfigMode == L7_SYSCONFIG_MODE_DHCP)  ||
               (networkConfigMode == L7_SYSCONFIG_MODE_BOOTP) ||
               (simGetSystemIPAddr() != L7_NULL))
      {
        osapiSnprintf(bindIntfName, sizeof(bindIntfName), "%s0", L7_DTL_PORT_IF);
      }  
    
      if (osapiSetsockopt(clientSocket,
                          L7_SOL_SOCKET,
                          L7_SO_BINDTODEVICE,
                          bindIntfName,
                          sizeof(bindIntfName)) != L7_SUCCESS)
      {
        LOG_MSG("tftpSocketPrepare(): Set socket SO_BINDTODEVICE option error");
        osapiSocketClose(clientSocket);
        return L7_NULL;
      }
    } /*  if( L7_FILE_TYPE_AUTO_INSTALL_SCRIPT == simGetTransferDownloadFileType()) */
#endif 

    if (broadcastModeValue == L7_NULL)
    {
      /* Set TTL for unicast mode */
      L7_uint32 packetTTLValue = TFTP_DEFAULT_IP_TTL_NUMBER;  
      
      if (osapiSetsockopt(clientSocket,
                          IPPROTO_IP,
                          L7_IP_TTL,
                          (L7_uchar8*)&packetTTLValue,
                          sizeof(packetTTLValue)) != L7_SUCCESS)
      {
        LOG_MSG("tftpSocketPrepare(): Set socket L7_IP_TTL option error");
        osapiSocketClose (clientSocket);
        return L7_NULL;
      }
    }
    else
    {
      /* Set TTL for broadcast mode */  
      L7_uchar8 packetTTLValue = TFTP_DEFAULT_IP_TTL_NUMBER;  
      
      if (osapiSetsockopt(clientSocket,
                          IPPROTO_IP,
                          L7_IP_MULTICAST_TTL,
                          &packetTTLValue,
                          sizeof(packetTTLValue)) != L7_SUCCESS)
      {
        LOG_MSG("tftpSocketPrepare(): Set socket IP_MULTICAST_TTL option error");
        osapiSocketClose(clientSocket);
        return L7_NULL;
      }
    }
    
    if (osapiSetsockopt(clientSocket,
                        L7_SOL_SOCKET,
                        L7_SO_BROADCAST,
                        (L7_uchar8 *)&broadcastModeValue,
                        sizeof(broadcastModeValue)) != L7_SUCCESS)
    {
      LOG_MSG("tftpSocketPrepare(): Set socket SO_BROADCAST option error");
      osapiSocketClose(clientSocket);
      return L7_NULL;
    }
  }
      
  if (osapiSocketBind(clientSocket, baddr, baddr_len) != L7_SUCCESS)
  {
    LOG_MSG("tftpSocketPrepare(): Bind error");
    osapiSocketClose(clientSocket);
    return L7_NULL;
  }
  
  return clientSocket;  
}

/*********************************************************************
* @purpose  Sends TFTP read request packet
*
* @param    transferParams   @b{(input)}  transfer parameters for
*           downloading process
*
* @param    fileMode         @b{(input)}  string contains transfer mode
*
* @param    fileName         @b{(input)}  string contains filename to
*           download
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t tftpGetRequestSend(tftpTransferParams_t *transferParams,
                           L7_char8 *fileMode,
                           L7_char8 *fileName)
{
  L7_uint32 bytesSent     = L7_NULL;
  L7_uint32 requestLength = L7_NULL;

  L7_TFTP_PACKET_t  tftpPacket;
  L7_uint32 saddr_len = sizeof(L7_sockaddr_t);
  
  memset(&tftpPacket, 0, sizeof(tftpPacket));

  if(transferParams->family == L7_AF_INET)
  {
    saddr_len = sizeof(L7_sockaddr_in_t);
  }
  else
  {
    saddr_len = sizeof(L7_sockaddr_in6_t);
  }

  requestLength = TFTP_REQUEST_PKT_HDR_SIZE + strlen(fileMode) + strlen(fileName); 
  
  if (transferParams->lastReceivedBlock != 0)
  {
    tftpDebugTrace("Send TFTP acknowledgement for block #%d\n", transferParams->lastReceivedBlock);

    /*build acknowledgement packet to send*/
    tftpACKPacketBuild(transferParams->lastReceivedBlock, &tftpPacket);
  }
  
  else
  {
    if (transferParams->family == L7_AF_INET)
    {
      L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];
    
      ipString[OSAPI_INET_NTOA_BUF_SIZE-1] = '\0';  
      osapiInetNtoa(((L7_sockaddr_in_t*)transferParams->saddr)->sin_addr.s_addr, ipString);
      tftpDebugTrace("Send TFTP request for %s file to %s\n", fileName, ipString);
    }
    else if (transferParams->family == L7_AF_INET6)
    {
      L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];

      memset(ipString, 0, sizeof(ipString));
      osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&(((L7_sockaddr_in6_t *)transferParams->saddr)->sin6_addr),
                    ipString, sizeof(ipString));
      tftpDebugTrace("Send TFTPv6 request for %s file to %s\n", fileName, ipString);
    }

    /* build request packet for specified file */  
    tftpRWQPacketBuild(TFTP_RRQ, fileName, fileMode, &tftpPacket);
  }
      
  if (osapiSocketSendto(transferParams->clientSocket, (L7_char8 *)&tftpPacket, requestLength, 0,
                        transferParams->saddr, saddr_len, &bytesSent) != L7_SUCCESS)
  {
    LOG_MSG("tftpRequestSend(): Send to error. Errno(%d)", errno);
    return L7_FAILURE;
  }

  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  Handles reply from TFTP server
*
* @param    transferParams   @b{(input)}  transfer parameters for
*           downloading process
*
* @param    fileMode         @b{(input)}  string contains transfer mode
*
* @param    fileName         @b{(input)}  string contains filename to
*           download
* @param    source_port      @b{(input)}  IP Port number used for the connection.
*
* @notes    none
*
* @end
*********************************************************************/
L7_TRANSFER_STATUS_t tftpGetReplyProcess(tftpTransferParams_t *transferParams,
                                         L7_char8 *fileMode,
                                         L7_char8 *fileName,
					 L7_ushort16 *source_port)
{
  L7_char8 *tempBuffer = L7_NULLPTR;
  L7_char8 *tftpBuffer = L7_NULLPTR;
  L7_char8 *copyBuffer = L7_NULLPTR;

  L7_TFTP_PACKET_t  *tftpContent = L7_NULLPTR;
  L7_TFTP_PACKET_t  tftpPacket;
  L7_TFTP_PACKET_t  buffer;
  
  L7_sockaddr_t     *raddr        = L7_NULLPTR;  
  L7_sockaddr_t     *saddr        = L7_NULLPTR;  
  L7_sockaddr_in_t  responseAddress4;
  L7_sockaddr_in6_t responseAddress6;  
  
  L7_uint32         from_len = L7_NULL;
  L7_int32          data_len = L7_NULL;
  L7_int32        actual_len = TFTP_SEGSIZE;
  L7_uint32         bytesCompleted;

  memset(&buffer,  0, sizeof (buffer));
  memset(&tftpPacket,       0, sizeof (tftpPacket));
  memset(&responseAddress4, 0, sizeof(responseAddress4));
  memset(&responseAddress6, 0, sizeof(responseAddress6));  
  
  tempBuffer = (L7_char8 *) &buffer;
  tftpBuffer = (L7_char8 *) &tftpPacket;
  
  saddr = transferParams->saddr;
  
  if (transferParams->family == L7_AF_INET)
  {
    raddr = (L7_sockaddr_t*)&responseAddress4;
  }
  else if(transferParams->family == L7_AF_INET6)
  {
    raddr = (L7_sockaddr_t*)&responseAddress6;
  }
  else
  {
    LOG_MSG("tftpGetReplyProcess(): Invalid Address family");
    return L7_TRANSFER_FAILED;
  }   
  
  if (transferParams->family == L7_AF_INET)
  {
    from_len = sizeof(L7_sockaddr_t);
  }
  else
  {
    from_len = sizeof(L7_sockaddr_in6_t);
  }

  if (osapiSocketRecvfrom(transferParams->clientSocket, tftpBuffer, sizeof(L7_TFTP_PACKET_t),
                          0, raddr, &from_len, &data_len) != L7_SUCCESS)
  {
    LOG_MSG("tftpGetReplyProcess(): Recv from error");
    return L7_TRANSFER_FAILED;
  }

  if (transferParams->family == L7_AF_INET)
  {
    L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];
    
    ipString[OSAPI_INET_NTOA_BUF_SIZE-1] = '\0';
    osapiInetNtoa(((L7_sockaddr_in_t*)raddr)->sin_addr.s_addr, ipString);
    tftpDebugTrace("TFTP reply from %s\n", ipString);
  }

  /* for parsing data which is received by client */        
  tftpContent = tftpParse(tftpBuffer);

  if (osapiNtohs(tftpContent->th_opcode) == TFTP_DATA) /* compare data opcode */
  {
    tftpDebugTrace("Data reply from TFTP server : block #%d\n", tftpContent->th_block);

    if (osapiNtohs(tftpContent->th_block) == (transferParams->lastReceivedBlock + 1))
    {
      /* Consume this data*/
      copyBuffer = (L7_char8 *)tftpContent->th_data;
      data_len  -= TFTP_DATA_PKT_HDR_SIZE;  /* Sizeof TFTP header */
      actual_len = data_len;
      simTransferBytesCompletedGet(&bytesCompleted);
      bytesCompleted += actual_len;
      simTransferBytesCompletedSet(bytesCompleted);

      while (data_len-- > 0)
      {
        *tempBuffer++ = *copyBuffer++;      /* copy data to buffer*/
      }

      transferParams->lastReceivedBlock++;  /* increase the block number */   
    
      /* write data to file */    
      if (fileWrite(transferParams->fd, (L7_char8 *) &buffer, actual_len, fileMode,
                    transferParams->callback, transferParams->cbarg) == L7_FAILURE)
      {
        LOG_MSG("Error while writing data to file");
        return L7_FAILURE;
      }
    }
    else
    {
      tftpDebugTrace("TFTP: received unexpected block #%d\n",osapiNtohs(tftpContent->th_block));
    }

    if (transferParams->family == L7_AF_INET)
    {
      *source_port = osapiNtohs(((L7_sockaddr_in_t*)raddr)->sin_port);  
      ((L7_sockaddr_in_t*)saddr)->sin_port = ((L7_sockaddr_in_t*)raddr)->sin_port;  

      memcpy(&(((L7_sockaddr_in_t*)saddr)->sin_addr.s_addr),
             &(((L7_sockaddr_in_t*)raddr)->sin_addr.s_addr),
             sizeof(((L7_sockaddr_in_t*)raddr)->sin_addr.s_addr));
    }
    else if(transferParams->family == L7_AF_INET6)
    {
      *source_port = osapiNtohs(((L7_sockaddr_in6_t*)raddr)->sin6_port);      
      ((L7_sockaddr_in6_t*)saddr)->sin6_port = ((L7_sockaddr_in6_t*)raddr)->sin6_port;      
      
      memcpy(&(((L7_sockaddr_in6_t*)saddr)->sin6_addr.in6),
             &(((L7_sockaddr_in6_t*)raddr)->sin6_addr.in6),
             sizeof(((L7_sockaddr_in6_t*)raddr)->sin6_addr.in6));
    }

    /*send acknowledgement for data packet received*/
    if (tftpGetRequestSend(transferParams, fileMode, fileName) != L7_SUCCESS)
    {
      return L7_TRANSFER_FAILED;
    }
        
    if (actual_len < TFTP_SEGSIZE)
    {
      return L7_TRANSFER_SUCCESS;
    }       
    else 
    {
      return L7_NEXT_DATA_BLOCK_RECEIVED;
    }   
  }
  
  else if (osapiNtohs(tftpContent->th_opcode) == TFTP_ERROR)
  {     
    tftpDebugTrace("TFTP server replied with error message: %s\n", (tftpContent->th_errMsg));
    return L7_TRANSFER_FILE_NOT_FOUND;
  }
  
  else
  {
    LOG_MSG("tftpGetReplyProcess(): TFTP client has received non TFTP reply");
    return L7_TRANSFER_FAILED;
  }
  
  return L7_TRANSFER_FAILED;
}

/*********************************************************************
* @purpose  To get an ascii or binary file from TFTP server to FASTPATH switch
* @param    inet_address  @b{(input)}  inet-address of TFTP server
* @param    fileName  @b{(input)}  name of the file
* @param    fileMode  @b{(input)}  mode of file which is to be transferred
* @param    fd        @b{(input)}  file descriptor 
* @returns  one of L7_TRANSFER_STATUS_t values
* @notes    none
*
* @end
*********************************************************************/
L7_TRANSFER_STATUS_t L7_TFTP_GET (L7_inet_addr_t * inet_address, L7_char8 * fileName,
                                  L7_char8 * fileMode, L7_int32 fd, L7_uint8 requestDelay,
                                  L7_TFTP_CALLBACK_t callback, void *cbarg)
{
  L7_sockaddr_t      *saddr        = L7_NULLPTR;
  L7_uint32          saddr_len     = L7_NULL;
  L7_uint32          requestLength = L7_NULL;
  L7_sockaddr_in_t   saddr4;
  L7_sockaddr_in6_t  saddr6;
  fd_set             readFds;

  L7_BOOL   broadcastModeStatus = L7_FALSE;
  L7_int32       clientSocket   = L7_NULL;
  L7_uint32      totalTimeouts  = L7_NULL;
  L7_uint32      requestTimeout = requestDelay;  
  L7_ushort16    source_port;
  
  L7_TRANSFER_STATUS_t  transferRC = L7_TRANSFER_FAILED;
  tftpTransferParams_t  transferParams;

  simTransferBytesCompletedSet(0);

  memset(&saddr4, 0, sizeof(saddr4));
  memset(&saddr6, 0, sizeof(saddr6));  
  FD_ZERO(&readFds);

  requestLength = TFTP_REQUEST_PKT_HDR_SIZE + strlen(fileMode) + strlen(fileName); 

  if(inet_address->family == L7_AF_INET)
  {
    saddr4.sin_family = L7_AF_INET;
    saddr4.sin_port   = osapiHtons(TFTP_PORT);
    memcpy(&(saddr4.sin_addr.s_addr), &(inet_address->addr.ipv4),
           sizeof(inet_address->addr.ipv4));

    saddr = (L7_sockaddr_t *)&saddr4;
    saddr_len = sizeof(saddr4);
    
    if (inet_address->addr.ipv4.s_addr == L7_IP_LTD_BCAST_ADDR)
    {
      broadcastModeStatus = L7_TRUE;
    }   
  }
  else if(inet_address->family == L7_AF_INET6)
  {
    /* check this */

    saddr6.sin6_family = L7_AF_INET6;
    saddr6.sin6_port   = osapiHtons(TFTP_PORT);
    memcpy(&(saddr6.sin6_addr.in6), &(inet_address->addr.ipv6),
           sizeof(inet_address->addr.ipv6));

    saddr = (L7_sockaddr_t *)&saddr6;
    saddr_len = sizeof(saddr6);

    if (L7_INET_IS_ADDR_BROADCAST(inet_address))
    {
      broadcastModeStatus = L7_TRUE;
    }
  }
  else
  {
    LOG_MSG("tftp_get(): Invalid Address family");
    return L7_TRANSFER_FAILED;
  }

  if ((clientSocket = tftpGetSocketPrepare(inet_address)) == L7_NULL)
  {
    LOG_MSG("tftp_get(): Failed to create socket");  
    return L7_TRANSFER_FAILED;
  }
  
  transferParams.fd                = fd;
  transferParams.cbarg             = cbarg;
  transferParams.callback          = callback;
  transferParams.lastReceivedBlock = L7_NULL;   
  transferParams.clientSocket      = clientSocket; 
  transferParams.family            = inet_address->family;
  transferParams.saddr             = saddr;

  requestTimeout = requestDelay;
  
  /*send read request to server*/  
  if (tftpGetRequestSend(&transferParams, fileMode, fileName) != L7_SUCCESS)
  {
    osapiSocketClose(clientSocket);  
    LOG_MSG("tftp_get(): Send to error");
    return L7_TRANSFER_FAILED;
  }
  
  while (totalTimeouts++ <= TFTP_TIMEOUT_RETRY) /* do while loop */
  {
    FD_ZERO(&readFds);
    FD_SET(clientSocket, &readFds);
    
    /* Subsequently handle all the reply packets in socket to find data reply.
       Then download the file */
    while (osapiSelect (clientSocket + 1, &readFds, NULL, NULL, requestTimeout, 0) > 0)
    {
      /* if some reply from server */
      transferRC = tftpGetReplyProcess(&transferParams, fileMode, fileName, &source_port);

      /* If we've received next good data block then zero timeouts counter.
         So in case of unsuccessful transfer of next data block, it will be 
         1+TFTP_TIMEOUT_RETRY times tried to download */
      if (transferRC == L7_NEXT_DATA_BLOCK_RECEIVED)
      {
        totalTimeouts = 0;
      } 
      
      else if (transferRC == L7_TRANSFER_FILE_NOT_FOUND)
      {
        if (broadcastModeStatus == L7_FALSE)
        {
          osapiSocketClose(clientSocket);       
          return L7_TRANSFER_FILE_NOT_FOUND;
        }
      }

      else
      {
        osapiSocketClose(clientSocket);       
        return transferRC;
      }       
      
    }  /* end of "while ( osapiSelect..." */
    
    tftpDebugTrace("TFTP transfer failed. Total timeouts %d\n", totalTimeouts); 

    /* If no reply from server or error message(s) "File Not Found" is(are) sent
       in response to broadcast GET request try to rerequest */                     
    if (totalTimeouts <= TFTP_TIMEOUT_RETRY)
    {
      /* In case non default TFTP timeout is used,
         increments it every cycle if no one server replied with data block */        
      if ((transferRC == L7_TRANSFER_FAILED) && (requestTimeout != TFTP_TIMEOUT))
      {
        requestTimeout++;     
      }
      
      if(inet_address->family == L7_AF_INET)
      {
        saddr4.sin_family = L7_AF_INET;
        saddr4.sin_port   = osapiHtons(source_port);
        memcpy(&(saddr4.sin_addr.s_addr), &(inet_address->addr.ipv4), sizeof(inet_address->addr.ipv4));     
      }
      else
      {
        saddr6.sin6_family = L7_AF_INET6;
        saddr6.sin6_port   = osapiHtons(source_port);
        memcpy(&(saddr6.sin6_addr.in6), &(inet_address->addr.ipv6), sizeof(inet_address->addr.ipv6));       
      }
          
      if (tftpGetRequestSend(&transferParams, fileMode, fileName) != L7_SUCCESS)
      {
        osapiSocketClose(clientSocket);       
        return L7_TRANSFER_FAILED;
      }
    }
    
  } /* end of "while( ++totalTimeouts..." */
  
  tftpDebugTrace("tftp_get(): No data reply from TFTP server - Connection Timeout");

  osapiSocketClose(clientSocket);  
  return transferRC;

} /* end of TFTP_GET*/

/*********************************************************************
* @purpose  To read data from the file and put it to TFTP structure
* @param    fileDesc          @b{(input)}  file descriptor of file
* @param    *mode             @b{(input)}  mode of file
* @param    *tftpPacket       @b{(input)}  pointer to TFTP packet structure
* @param    blockNo           @b{(input)}  block number of data
* @param    *count            @b{(output)} number of bytes read
* @returns  L7_SUCCESS or L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t fileRead(L7_int32 fileDesc,L7_char8 * mode,L7_TFTP_PACKET_t *tftpPacket,
                 L7_int32 blockNo,L7_uint32 *count,
                 L7_TFTP_CALLBACK_t callback, void *cbarg)
{
  L7_int32 newline=0,prevchar=-1;
  L7_char8 c;
  L7_char8 *fileBuffer=L7_NULLPTR;
  L7_char8 *buffer=L7_NULLPTR;

  *count=0;
  fileBuffer=(L7_char8*)tftpPacket->th_data;
  tftpPacket->th_opcode =  osapiHtons(TFTP_DATA);/* form data Packet*/
  tftpPacket->th_block = osapiHtons(blockNo);
  if ((mode[0]=='b'||mode[0] =='B')&&(mode[1]=='i'||mode[1] =='I')
      &&(mode[2]=='n'||mode[2] =='N')&&(mode[3]=='a'||mode[3] =='A')
      &&(mode[4]=='r'||mode[4] =='R')&&(mode[5]=='Y'||mode[5] =='y') )
  {
    /* mode is binary*/
    *count=tftpFileRead(fileDesc, callback, cbarg, fileBuffer, TFTP_SEGSIZE);
    if (*count <0)
    {
      LOG_MSG("Error while reading file");
      return L7_FAILURE;
    }
    return(L7_SUCCESS);
  }/* end of binary*/
  else if ((mode[0]=='o'||mode[0] =='O')&&(mode[1]=='c'||mode[1] =='C')
           &&(mode[2]=='t'||mode[2] =='T')&&(mode[3]=='e'||mode[3] =='E')
           &&(mode[4]=='t'||mode[4] =='T') )
  {
#if defined(QUANTA_LB4M)
    /* mode is octet*/
    if(fileDesc == -1)
    {
      if(blockNo > 0x8000)
      {
        *count = 0;
      }
      else
      {
        *count = TFTP_SEGSIZE;
        memcpy((void *)fileBuffer, (void *)(0xFE000000 + (TFTP_SEGSIZE * (blockNo - 1))), TFTP_SEGSIZE);
      }
    }
    else if(fileDesc == -2)
    {
      if(blockNo > 0x8000)
      {
        *count = 0;
      }
      else
      {
        *count = TFTP_SEGSIZE;
        memcpy((void *)fileBuffer, (void *)(0xFF000000 + (TFTP_SEGSIZE * (blockNo - 1))), TFTP_SEGSIZE);
      }
    }
    else
    {
#endif
      *count=tftpFileRead(fileDesc, callback, cbarg, fileBuffer,TFTP_SEGSIZE);
#if defined(QUANTA_LB4M)
    }
#endif
    if (*count <0)
    {
      LOG_MSG("Error while reading file");
      return L7_FAILURE;
    }
    return(L7_SUCCESS);
  }/* end of octet*/
  else if ((mode[0]=='a'||mode[0] =='A')&&(mode[1]=='s'||mode[1] =='S')
           &&(mode[2]=='C'||mode[2] =='c')&&(mode[3]=='i'||mode[3] =='I')
           &&(mode[4]=='i'||mode[4] =='I') )
  {
    buffer=L7_NULLPTR;
    buffer=fileBuffer;
    for (*count=0;*count < TFTP_SEGSIZE; (*count)++)
    /*perform modifications for ascii file*/
    {
      if (newline)
      {
        if (prevchar == '\n')      /*cr is carriage return,lf is linefeed*/
          c = '\n';              /* newline to cr,lf */
        else
          c = '\0';              /* cr to cr,NULL */
        newline = 0;
      }
      else
      {
        if (osapiRead(fileDesc,&c,1) < 1)
        {
          break;
        }
        if (c == '\n' || c == '\r')
        {
          prevchar = c;
          c = '\r';
          newline = 1;
        }
      }
      *buffer++ = c;
    }/*end of FOR*/
    return(L7_SUCCESS);
  }/*end of else*/
  else
  {
    LOG_MSG("Wrong value of mode entered");
    return L7_FAILURE;
  }
}/*end of file read function*/

/*********************************************************************
* @purpose  To put an ascii or binary file from FASTPATH switch to TFTP server
* @param    inet_address  @b{(input)}  inet-address of TFTP server
* @param    filename  @b{(input)}  name of the file
* @param    fileMode  @b{(input)}  mode of file which is to be transferred
* @param    fileDesc  @b{(input)}  file descriptor
* @returns  L7_SUCCESS or L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t L7_TFTP_PUT(L7_inet_addr_t *inet_address, L7_char8 *filename,
                    L7_char8 *fileMode, L7_int32 fileDesc,
                    L7_TFTP_CALLBACK_t callback, void *cbarg)
{
  L7_TFTP_PACKET_t  *tftpPacket, *tftpPacketData, *tftpContent;
  L7_TFTP_PACKET_t  l7_tftp_packet, mem_tftpDataBuffer;
  L7_sockaddr_t     *baddr, *saddr, *raddr;
  L7_sockaddr_in_t  baddr4, saddr4, raddr4;
  L7_sockaddr_in6_t baddr6, saddr6, raddr6;
  L7_uint32         baddr_len, saddr_len, from_len, bytesSent, bytesRcvd, requestLength;
  L7_int32          Clientsocket, actual_len, last_good_block=0;
  L7_ushort16       port;
  L7_BOOL           resendFlag;
  fd_set            readFds;
  L7_uint32         bytesCompleted;

  L7_int32 data_len        = 0;
  L7_int32 put_port        = 7800;
  L7_int32 total_timeouts  = 0;
  L7_char8 *tftpBuffer     = L7_NULLPTR;
  L7_char8 *tftpDataBuffer = L7_NULLPTR;

  simTransferBytesCompletedSet(0);

  requestLength = 9 + strlen(filename); /* check this */

  if(inet_address->family == L7_AF_INET)
  {
    baddr4.sin_family      = L7_AF_INET;
    baddr4.sin_port        = osapiHtons(put_port);
    baddr4.sin_addr.s_addr = L7_INADDR_ANY;

    saddr4.sin_family      = L7_AF_INET;
    saddr4.sin_port        = osapiHtons(TFTP_PORT);
    memcpy(&(saddr4.sin_addr.s_addr), &(inet_address->addr.ipv4),
           sizeof(inet_address->addr.ipv4));

    baddr = (L7_sockaddr_t *)&baddr4;
    saddr = (L7_sockaddr_t *)&saddr4;
    raddr = (L7_sockaddr_t *)&raddr4;
    baddr_len = sizeof(baddr4);
    saddr_len = sizeof(saddr4);
    from_len  = sizeof(raddr4);
  }
  else
  {
    /* check this */
    memset(&baddr6, 0, sizeof(baddr6));
    baddr6.sin6_family      = L7_AF_INET6;
    baddr6.sin6_port        = osapiHtons(put_port);

    saddr6.sin6_family      = L7_AF_INET6;
    saddr6.sin6_port        = osapiHtons(TFTP_PORT);
    memcpy(&(saddr6.sin6_addr.in6), &(inet_address->addr.ipv6),
           sizeof(inet_address->addr.ipv6));

    baddr = (L7_sockaddr_t *)&baddr6;
    saddr = (L7_sockaddr_t *)&saddr6;
    raddr = (L7_sockaddr_t *)&raddr6;
    baddr_len = sizeof(baddr6);
    saddr_len = sizeof(saddr6);
    from_len  = sizeof(raddr6);
  }

  if ( osapiSocketCreate(inet_address->family, L7_SOCK_DGRAM, 0,
                         &Clientsocket ) == L7_FAILURE)
  {
    LOG_MSG("tftp_put(): Failed to create socket");
    return L7_FAILURE;
  }

  if (osapiSocketBind(Clientsocket, baddr, baddr_len) != L7_SUCCESS)
  {
    LOG_MSG("tftp_put(): Bind error");
    osapiSocketClose(Clientsocket);
    return L7_FAILURE;
  }
  while (1)
  {
    /* build write request*/
    tftpPacket = tftpRWQPacketBuild(TFTP_WRQ, filename, fileMode, &l7_tftp_packet);

    /* send write request packet to TFTP server*/
    if (osapiSocketSendto(Clientsocket, (L7_char8 *)tftpPacket, requestLength, 0,
                          saddr, saddr_len, &bytesSent) != L7_SUCCESS )
    {
      LOG_MSG("tftp_put(): Send to error");
      osapiSocketClose(Clientsocket);
      return L7_FAILURE;
    }

    FD_ZERO(&readFds);
    FD_SET(Clientsocket, &readFds);

    if (osapiSelect (Clientsocket+1, &readFds, NULL, NULL, TFTP_TIMEOUT,0) <= 0)
    /*no reply from server*/
    {
      if (++total_timeouts > TFTP_TIMEOUT_RETRY)
      {
        LOG_MSG("TFTP Timeout no reply from TFTP server");
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
    }
    else
    {
      total_timeouts = 0;
      tftpBuffer = (L7_char8 *) &l7_tftp_packet;
      bzero ((L7_char8 *) tftpBuffer, sizeof (L7_TFTP_PACKET_t));
      if (osapiSocketRecvfrom(Clientsocket, tftpBuffer, sizeof(L7_TFTP_PACKET_t),
                              0,raddr, &from_len, &data_len) != L7_SUCCESS)
      {
        LOG_MSG("tftp_put(): recv from error");
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
      if(inet_address->family == L7_AF_INET)
      {
        port = osapiNtohs(raddr4.sin_port);
      }
      else
      {
        port = osapiNtohs(raddr6.sin6_port);
      }
      tftpContent   = tftpParse(tftpBuffer);/*parse the receieved buffer*/
      if (osapiNtohs(tftpContent->th_opcode) == TFTP_ACK)
      {
        /*Write Request Accepted--send data*/
        break;
      }
      else if (osapiNtohs(tftpContent->th_opcode) == TFTP_ERROR)
      {
        LOG_MSG("Error is: %s\n with Error code:: %d\n",
                (tftpContent->th_errMsg),osapiNtohs(tftpContent->th_error));
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
      else
      {
        LOG_MSG("Wrong protocol used");
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }

    }/*end of else*/
  }/*end of while*/

  /*sending data*/
  resendFlag = L7_FALSE;
  last_good_block =1;
  tftpPacketData = &l7_tftp_packet;

  do
  {
    if ((osapiNtohs((tftpContent->th_block) != last_good_block) && (resendFlag == L7_FALSE)) || 
        (last_good_block==1))
    /*for sending the data packet*/
    {

      bzero((L7_char8 *)tftpPacketData,sizeof(L7_TFTP_PACKET_t));
      if (fileRead(fileDesc, fileMode, tftpPacketData, last_good_block,
                   &data_len, callback, cbarg) != L7_SUCCESS)
      {
        LOG_MSG("Error while reading file");
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
    }
    actual_len = data_len + TFTP_DATA_PKT_HDR_SIZE;

    simTransferBytesCompletedGet(&bytesCompleted);
    bytesCompleted += data_len;
    simTransferBytesCompletedSet(bytesCompleted);

    if(inet_address->family == L7_AF_INET)
    {
      saddr4.sin_family      = L7_AF_INET;
      saddr4.sin_port = osapiHtons(port);
      memcpy(&(saddr4.sin_addr.s_addr), &(raddr4.sin_addr.s_addr),
             sizeof(raddr4.sin_addr.s_addr));
    }
    else
    {
      saddr6.sin6_family      = L7_AF_INET6;
      saddr6.sin6_port = osapiHtons(port);
      memcpy(&(saddr6.sin6_addr.in6), &(raddr6.sin6_addr.in6),
             sizeof(raddr6.sin6_addr.in6));
    }
    if (osapiSocketSendto(Clientsocket, (L7_char8 *)tftpPacketData, actual_len, 0,
                          saddr, saddr_len, &bytesSent ) != L7_SUCCESS )
    {
      LOG_MSG("tftp_put(): Send to error");
      osapiSocketClose(Clientsocket);
      return L7_FAILURE;
    }

    FD_ZERO(&readFds);
    FD_SET(Clientsocket, &readFds);

    if (osapiSelect (Clientsocket+1, &readFds, NULL, NULL, TFTP_TIMEOUT,0) <= 0)
    {
      if (++total_timeouts > TFTP_TIMEOUT_RETRY)
      {
        LOG_MSG("TFTP Timeout no ACK from TFTP server... aborting transfer");
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
      resendFlag = L7_TRUE;
      LOG_MSG("TFTP Timeout %u no ACK from TFTP server... resending block", total_timeouts);

    }
    else
    {
      tftpContent = L7_NULLPTR;
      tftpDataBuffer = (L7_char8 *) &mem_tftpDataBuffer;
      bzero((L7_char8 *)tftpDataBuffer,sizeof(L7_TFTP_PACKET_t));
      if (osapiSocketRecvfrom(Clientsocket, tftpDataBuffer, sizeof(L7_TFTP_PACKET_t),
                              0, raddr, &from_len,&bytesRcvd) != L7_SUCCESS)
      {

        LOG_MSG("tftp_put(): recv from  error");
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
      tftpContent   = tftpParse(tftpDataBuffer);   /*parse the received buffer*/
      if(inet_address->family == L7_AF_INET)
      {
        port = osapiNtohs(raddr4.sin_port);
      }
      else
      {
        port = osapiNtohs(raddr6.sin6_port);
      }
      if (osapiNtohs(tftpContent->th_opcode) == TFTP_ACK)
      {
        if (osapiNtohs(tftpContent->th_block) == last_good_block)
        {
          last_good_block++;
          resendFlag = L7_FALSE;
        }
        else
        {
          /*server acknowledgement block# not matches with the block# of data sent*/
          LOG_MSG("Send block %d\n, got ACK for #%d\n",
                  last_good_block, osapiNtohs(tftpContent->th_block));
          /*Data Packet not acknowledged
          last_good_block=last_good_block - 1;  */
          resendFlag = L7_TRUE;

        }
      }
      else if (osapiNtohs(tftpContent->th_opcode) == TFTP_ERROR)
      {
        LOG_MSG(" Server error message is: %s\n with error code:: %d\n",
                (tftpContent->th_errMsg), osapiNtohs(tftpContent->th_error));
        /*print error string*/
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
      else
      {
        LOG_MSG(" Wrong protocol used");
        osapiSocketClose(Clientsocket);
        return L7_FAILURE;
      }
    }/* end of osapiSelect else part*/

  }while (data_len==TFTP_SEGSIZE);
  /* end of do-while loop*/
  osapiSocketClose(Clientsocket);
  return L7_SUCCESS; /*Return L7_SUCCESS if everything goes fine*/

}/*end of TFTP_PUT*/

