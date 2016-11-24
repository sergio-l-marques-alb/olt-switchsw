/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mfc_v6.c
*
* @purpose    Implements the Packet Reception/Transmission mechanisms
*             of the Multicast Forwarding Cache (MFC) module for IPv6
*
* @component  Multicast Forwarding Cache (MFC)
*
* @comments   none
*
* @create     10-Jan-06
*
* @author     ddevi.
* @end
*
**********************************************************************/
#include "mfc_api.h"
#include "osapi.h"
#include "osapi_support.h"
#include "defaultconfig.h"
#include "l3_comm_structs.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_mcast_commdefs.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv6.h"
#include "rtiprecv.h"
#include "ipstk_api.h"
#include "buff_api.h"
#include "rtmbuf.h"
#include "l7_ip6_api.h"
#include "mcast_defs.h"
#include "mcast_debug.h"
#include "mfc.h"
#include "mfc_debug.h"
#include "mfc_v6.h"
#include "l7_mcast_api.h"
#include "mfc_map.h"
#include "mcast_util.h"
#include "mcast_map.h"

/* The last 4 bytes in the MAC-DA and IPv6 DA should match */
#define MCAST_MAC_IP6_DA_MATCH_BYTES  4 

/* The first 2 bytes in the Mcast MAC are 0x3333 */
#define MCAST_MAC_IP6_DA_PREFIX_LEN   2 

#define MCAST_ERROR                  -1

static L7_uchar8 MCAST_MAC_IP6_DA_PREFIX[] = {0x33, 0x33};
static L7_int32 mfcIp6SockId = MCAST_ERROR;
static L7_int32 mfcIp6SendSockId = MCAST_ERROR;

/*********************************************************************
* @purpose  Initializes the Data Rx/Tx sockets for MFC operation
*
* @param    none
*
* @returns  L7_SUCCESS  Initialization successful.
* @returns  L7_FAILURE  Initialization failed.
*
* @comments    As of now, we only have IPv6 socket support. IPv4 packets
*           do not use the socket mechanism but rather the sysnet hooks.
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SockInit()
{
  L7_sockaddr_in6_t sock;
  L7_BOOL           sockOpt;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry. ");

  /* Is the socket already initialized? */
  if (mfcIp6SockId != -1)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES, " IPv6 socket already initialized. Sock ID = %d ",
                mfcIp6SockId);
    return L7_FAILURE;
  }
  /* Create the IPv6 socket to listen for IPv6 Multicast Data packets on */
    /* Linux stack assumes MRoute socket is not of type IPPROTO_RAW */
  if (osapiSocketCreate(L7_AF_INET6, L7_SOCK_RAW, IPPROTO_UDP, &mfcIp6SockId)
      != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to create MFC Recv IPv6 socket ");
    mfcIp6SockId = MCAST_ERROR;
    return L7_FAILURE;
  }
  /* We use a seperate socket to send the Multicast routed packets */
  if (osapiSocketCreate(L7_AF_INET6, L7_SOCK_RAW, IPPROTO_RAW, &mfcIp6SendSockId)
      != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to create MFC Send IPv6 socket ");
    mfcIp6SendSockId = MCAST_ERROR;
    return L7_FAILURE;
  }
  /* Bind the socket to IPv6 INADDR_ANY */
  memset(&sock, 0, sizeof(sock));
  sock.sin6_family = L7_AF_INET6;
  if (osapiSocketBind(mfcIp6SockId, (L7_sockaddr_t *)(&sock), sizeof(sock))
      != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to bind IPv6 socket, socket = %d ",
                mfcIp6SockId);  
    osapiSocketClose(mfcIp6SockId);
    mfcIp6SockId = MCAST_ERROR;
    return L7_FAILURE;
  }
  /* Bind the send socket to IPv6 INADDR_ANY */
  memset(&sock, 0, sizeof(sock));
  sock.sin6_family = L7_AF_INET6;
  sock.sin6_port = 0;
  if (osapiSocketBind(mfcIp6SendSockId, (L7_sockaddr_t *)(&sock), sizeof(sock))
      != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, "Failed to bind IPv6 Send socket, socket = %d ",
                mfcIp6SendSockId);  
    osapiSocketClose(mfcIp6SendSockId);
    mfcIp6SendSockId = MCAST_ERROR;
    return L7_FAILURE;
  }
  /* Enable Rx/Tx of Multicast packets on this socket */
  sockOpt = L7_TRUE;
  if (osapiSetsockopt(mfcIp6SockId, IPPROTO_IPV6,  L7_IPV6_RECVPKTINFO,
                      (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, 
                "Failed to set RECVPKT_INFO IPv6 socket option, socket = %d ",
                mfcIp6SockId);  
  }

  /* On the send socket, don't receive a copy of multicast packets sent */
  sockOpt = L7_FALSE;
  if (osapiSetsockopt(mfcIp6SendSockId, IPPROTO_IPV6, L7_IPV6_MULTICAST_LOOP, 
		      (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, 
                "Failed to set MULTICAST_LOOP IPv6 socket option, socket = %d ",
                mfcIp6SendSockId);  
  }

  if (osapiIpIpv6MRTSet(mfcIp6SockId, IPPROTO_IPV6, L7_TRUE, 
                        (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    MFC_LOG_MSG(L7_AF_INET6,"Multicast Enable in IPv6 Stack Failed; sockFD = %d",
                mfcIp6SockId);  
    osapiSocketClose(mfcIp6SockId);
    mfcIp6SockId = MCAST_ERROR;
    return L7_FAILURE;
  }
  /* Register the created socket with the packet receiver for Rx waiting */
  if(mcastMapPktRcvrSocketFdRegister (mfcIp6SockId, MFC_IPV6_DATA_PKT_RECV_EVENT, 
                                      mcastV6DataBufferPoolIdGet(),
                                      L7_IPV6_PKT_RCVR_ID_MFC) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES," \n IPv6 socket registration"
              "with Packet receiver failed ", __FUNCTION__, __LINE__);
    osapiSocketClose(mfcIp6SockId);
    mfcIp6SockId = MCAST_ERROR;
    return L7_FAILURE;
  }
  MFC_DEBUG(MFC_DEBUG_EVENTS, "Socket init success ");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  DeInitializes the Data Rx/Tx sockets opened for MFC operation
*
* @param    none
*
* @returns  L7_SUCCESS   DeInitialization successful.
* @returns  L7_FAILURE   DeInitialization failed.
*
* @comments    As of now, we only have IPv6 socket support. IPv4 packets
*           do not use the socket mechanism but rather the sysnet hooks.
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SockDeInit()
{
  L7_RC_t    retCode = L7_SUCCESS;
  L7_BOOL    sockOpt = L7_TRUE;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry ");

  /* Is the socket initialized? */
  if (mfcIp6SockId == MCAST_ERROR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES, " IPv6 Socket not initialized");
    return L7_FAILURE;
  }
  if (mfcIp6SendSockId == MCAST_ERROR)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES, " IPv6 Send Socket not initialized");
    return L7_FAILURE;
  }

  /* Dis-associate the socket from the packet receiver */
  if (mcastMapPktRcvrSocketFdDeRegister(L7_IPV6_PKT_RCVR_ID_MFC) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to deregister with packet receiver,"
                 "socket = %d ",mfcIp6SockId );
    retCode = L7_FAILURE;
  }
  if (osapiIpIpv6MRTSet(mfcIp6SockId, IPPROTO_IPV6, L7_FALSE, 
                        (L7_uchar8 *)&sockOpt, sizeof(sockOpt)) == L7_FAILURE)
  {
    MFC_LOG_MSG(L7_AF_INET6, "Multicast Disable in IPv6 Stack Failed; sockFD = %d",
                mfcIp6SockId );  
    retCode = L7_FAILURE;
  }

  /* Close the socket */
  osapiSocketClose(mfcIp6SockId);
  mfcIp6SockId = MCAST_ERROR;
  osapiSocketClose(mfcIp6SendSockId);
  mfcIp6SendSockId = MCAST_ERROR;
  return retCode;
}
/*********************************************************************
* @purpose  Transmit the given IPv6 packet on a specific outgoing interface
*
* @param    rtrIfNum  @b{(input)}router interface number of the outgoing interface
* @param    pBuf      @b{(input)}buffer containing the packet to be sent
*
* @returns  L7_SUCCESS, Packet forwarded successfully.
* @returns  L7_FAILURE, Packet forwarding failed.
*
* @comments CAUTION : 1. The given buffer is in-line modified to suit
*                        transmission.
*                     2. Both the given frame-buffer and the rtm-buffer would
*                        be freed-up by this function irrespective of the return value.
*
* @end
*
*********************************************************************/
#ifdef MFC_TBD
  Note: This is supposed to be called for Linux 
   once Linux raw_sendmsg is clean.
/* #ifdef _L7_OS_LINUX_ */
L7_RC_t mfcIpv6PktTransmit(L7_uint32 rtrIfNum, struct rtmbuf *pBuf)
{
  L7_ip6Header_t   *pIp6Hdr = L7_NULLPTR;
  L7_sockaddr_in6_t sockFrom, sockTo;
  L7_uint32         fromLen, toLen, intIfNum, bytesSent = 0;
  L7_RC_t           ret;
  L7_uint32         flags;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , rtrIfNum = %d, pBuf = %x", rtrIfNum, pBuf);
  MFC_DEBUG(MFC_DEBUG_TX,"\n Transmission of Ipv6 pkt ");

  /* Compute the internal interface number for the given outgoing interface */
  if (ip6MapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
             " Failed to convert to internal interface number for rtrIfNum : %d",
             rtrIfNum);
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
    rtm_freem(pBuf);
    return L7_FAILURE;
  }

  pIp6Hdr = rtmtod(pBuf, L7_ip6Header_t *);

  memset(&sockFrom, 0, sizeof(sockFrom));
  sockFrom.sin6_family = L7_AF_INET6;
  sockFrom.sin6_len = sizeof(sockFrom);
  /* Leave sockFrom at INADDR_ANY */
  fromLen = sizeof(L7_sockaddr_in6_t);

  memset(&sockTo, 0, sizeof(sockTo));
  sockTo.sin6_family = L7_AF_INET6;
  sockTo.sin6_len = sizeof(sockTo);
  sockTo.sin6_port = 0;  
  memcpy(&sockTo.sin6_addr, pIp6Hdr->dst, sizeof(L7_in6_addr_t));
  sockTo.sin6_port = 0; /* Must be 0 for IPPROTO_RAW SOCK_RAW socket */
  toLen = sizeof(L7_sockaddr_in6_t);

   --pIp6Hdr->hoplim;
  flags = 0;

  if ((ret = osapiPktInfoSend(mfcIp6SendSockId, pBuf->rtm_data ,
                              pBuf->rtm_len, flags,
                              (L7_sockaddr_t *)&sockFrom, fromLen,
                              (L7_sockaddr_t *)&sockTo, toLen, intIfNum,
                              &bytesSent, 0, 0, 0,
                              L7_FALSE, L7_FALSE, L7_FALSE)) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
             " Failed to transmit pkt from socket = %d",mfcIp6SendSockId);   
    MFC_DEBUG(MFC_DEBUG_TX,"\n Failed to transmit pkt from socket = %d",
              mfcIp6SendSockId);
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
    rtm_freem(pBuf);
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
  rtm_freem(pBuf);

  return L7_SUCCESS;
}
#endif

/* NOTE: This is called only for Vxworks */
L7_RC_t mfcIpv6PktTransmit(L7_uint32 rtrIfNum, struct rtmbuf *pBuf)
{
  L7_ip6Header_t   *pIp6Hdr = L7_NULLPTR;
  L7_sockaddr_in6_t sockFrom, sockTo;
  L7_uint32         fromLen, toLen, intIfNum, bytesSent, trafficClass, hopLimit, nextHdr;
  L7_uint32         flags, ifIndex = 0;
  L7_RC_t           ret;

  MFC_DEBUG(MFC_DEBUG_APIS,"\n Entry , rtrIfNum = %d, pBuf = %x", rtrIfNum, pBuf);
  MFC_DEBUG(MFC_DEBUG_TX,"\n Transmission of Ipv6 pkt ");

  /* Compute the internal interface number for the given outgoing interface */
  if (ip6MapRtrIntfToIntIfNum(rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
             " Failed to convert to internal interface number for rtrIfNum : %d",
             rtrIfNum);
    if ((pBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
    }
    rtm_freem(pBuf);
    return L7_FAILURE;
  }

  pIp6Hdr = (L7_ip6Header_t *)&pBuf->m_hdr.mh_data;

  memset(&sockFrom, 0, sizeof(sockFrom));
  sockFrom.sin6_family = L7_AF_INET6;
  sockFrom.sin6_len = sizeof(sockFrom);
  memcpy(&sockFrom.sin6_addr, pIp6Hdr->src, sizeof(L7_in6_addr_t));
  fromLen = sizeof(L7_sockaddr_in6_t);

  memset(&sockTo, 0, sizeof(sockTo));
  sockTo.sin6_family = L7_AF_INET6;
  sockTo.sin6_len = sizeof(sockTo);

  memcpy(&sockTo.sin6_addr, pIp6Hdr->dst, sizeof(L7_in6_addr_t));

  toLen = sizeof(L7_sockaddr_in6_t);

  trafficClass = L7_IP6_GET_CLASS(pIp6Hdr);
  hopLimit = pIp6Hdr->hoplim - 1;
  nextHdr = pIp6Hdr->next;
  flags = 0;

  if (ipstkIntIfNumToStackIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
             " Failed to convert to stack interface number for intIfNum = %d",
             intIfNum);  
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n Failed to convert to stack interface number for intIfNum = %d ", 
              intIfNum);  
    if ((pBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
    }
    rtm_freem(pBuf);
    return L7_FAILURE;
  }

  if ((ret = osapiPktInfoSend(mfcIp6SendSockId, pBuf->rtm_data + L7_IP6_HEADER_LEN,
                              pBuf->rtm_len - L7_IP6_HEADER_LEN, flags,
                              (L7_sockaddr_t *)&sockFrom, fromLen,
                              (L7_sockaddr_t *)&sockTo, toLen, intIfNum,
                              &bytesSent, trafficClass, hopLimit, nextHdr,
                              L7_TRUE, L7_TRUE, L7_TRUE, L7_FALSE)) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
             " Failed to transmit pkt from socket = %d",mfcIp6SendSockId);   
    MFC_DEBUG(MFC_DEBUG_TX,"\n Failed to transmit pkt from socket = %d",
              mfcIp6SendSockId);
    if ((pBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
    }
    rtm_freem(pBuf);
    return L7_FAILURE;
  }
  if ((pBuf->rtm_bufhandle) != L7_NULLPTR)
  {
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(pBuf->rtm_bufhandle));
  }
  rtm_freem(pBuf);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process all the IPv6 Multicast Data Packets received.
*
* @param    sockFd    @b{(input)}Socket Fd to receive the packets from
*
* @returns  L7_SUCCESS Packet was received and processed
* @returns  L7_FAILURE No Packet was received.
*
* @comments    Be it success or failure, the given buffer is preserved
*           intact. It is freed-up irrespective of return value
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6PktRecv(void  *pktMsg)
{
  L7_uchar8         *bufferAddr = L7_NULLPTR;
  L7_uint32         bufferPoolId;
  L7_uint32         rtrIfNum;
  mfcEntry_t        mfcEntry;
  struct rtmbuf     *recvBuf = L7_NULLPTR;
  L7_uchar8         *pDataStart = L7_NULLPTR;
  L7_uchar8         *pTmpPtr = L7_NULLPTR;
  L7_uint32         recvFrame;
  L7_uint32         inBytes;
  L7_ushort16       protoType = L7_ETYPE_IPV6 ;
  L7_ip6Header_t    ip6Hdr;
  mcastControlPkt_t *pktInfo;
  L7_uchar8         *buffer;
  L7_uint32         currentMcastProtocol = L7_NULL;
  L7_uchar8         src[IPV6_DISP_ADDR_LEN];

  pktInfo = (mcastControlPkt_t *)pktMsg;
  buffer = (L7_uchar8 *)pktInfo->payLoad;

  rtrIfNum = pktInfo->rtrIfNum;
  memset(&mfcEntry, 0, sizeof(mfcEntry_t));

  mcastDebugPacketRxTxTrace(L7_AF_INET6, L7_TRUE, rtrIfNum, buffer, pktInfo->length);

  MFC_DEBUG(MFC_DEBUG_RX,"\n Entry, mfcIp6SockId = %d ",mfcIp6SockId);

  /* If multicast is not enabled, do nothing */
  if (mfcIsEnabled(L7_AF_INET6) != L7_TRUE)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n MFC is disabled");
    return L7_SUCCESS;
  }
  if ((mcastMapIpCurrentMcastProtocolGet (L7_AF_INET6, &currentMcastProtocol)
                                       == L7_SUCCESS) &&
      currentMcastProtocol == L7_MCAST_IANA_MROUTE_UNASSIGNED)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n No MRP is configured");
    return L7_SUCCESS;
  }

  if (mcastMapMcastProtocolIsOperational (L7_AF_INET6, currentMcastProtocol)
                                       != L7_TRUE)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n MRP (%d) is not operational", 
            currentMcastProtocol);
    return L7_SUCCESS;
  }
      
  bufferPoolId = rtMemPoolIdGet();
  if (bufferPoolId == 0)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES, " Failed to retreive buffer pool id ");
    return L7_FAILURE;
  }

  /* Allocate the buffer */
  if (bufferPoolAllocate(bufferPoolId, &bufferAddr) != L7_SUCCESS)
  {
    MFC_LOG_MSG (L7_AF_INET6, "RTM Buffer Pool Allocation Failed for Multicast IPv6 "
                 "Data Packet; rtmPoolID - %d", bufferPoolId);
    MFC_DEBUG(MFC_DEBUG_FAILURES,
              "\n Failed to allocate a buffer from pool id = %d",bufferPoolId);
    return L7_FAILURE;
  }

  recvBuf = (struct rtmbuf *)bufferAddr;
  memset(recvBuf, 0, sizeof(struct rtmbuf));

  recvFrame= sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_NORMAL, L7_MBUF_FRAME_ALIGNED);
  if (recvFrame== L7_NULL)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Failed to get free MBuf ");
    bufferPoolFree(bufferPoolId, bufferAddr);
    return L7_FAILURE;
  }
  recvBuf->rtm_bufhandle = (void *)recvFrame;

  SYSAPI_NET_MBUF_GET_DATASTART((L7_netBufHandle)(recvBuf->rtm_bufhandle),
                                pDataStart); 
  
  memset(pDataStart, 0, L7_ENET_HDR_SIZE + 2);
  memcpy(pDataStart + L7_ENET_HDR_SIZE, &protoType, sizeof(protoType));

 /* inBytes = L7_PORT_ENET_ENCAP_MAX_MTU;*/
  inBytes = pktInfo->length;

  memcpy((pDataStart + L7_ENET_HDR_SIZE + 2), buffer, inBytes);

  recvBuf->rtm_type = MT_DATA;  
  recvBuf->rtm_data = pDataStart + sysNetDataOffsetGet(pDataStart);
  /* this set the rtm_len */
  SYSAPI_NET_MBUF_SET_DATALENGTH((L7_netBufHandle)(recvBuf->rtm_bufhandle), 
                                    inBytes+ L7_ENET_HDR_SIZE + 2);

  recvBuf->rtm_pkthdr.rcvif = L7_NULLPTR;

  /* Extract the IP Header from the packet and fill-up the internal structure */
  pTmpPtr = recvBuf->rtm_data;
  memset(&ip6Hdr, 0, sizeof(L7_ip6Header_t));
  MCAST_GET_LONG(ip6Hdr.ver_class_flow, pTmpPtr);
  MCAST_GET_SHORT(ip6Hdr.paylen, pTmpPtr);
  MCAST_GET_BYTE(ip6Hdr.next, pTmpPtr);
  MCAST_GET_BYTE(ip6Hdr.hoplim, pTmpPtr);
  memcpy(ip6Hdr.src, pTmpPtr, sizeof(ip6Hdr.src));
  pTmpPtr += sizeof(ip6Hdr.src);
  memcpy(ip6Hdr.dst, pTmpPtr, sizeof(ip6Hdr.dst));

  /* Set-up the Cache Info structure */
  if (inetAddressSet(L7_AF_INET6, &(ip6Hdr.src), &(mfcEntry.source)) != L7_SUCCESS)
  {
    if ((recvBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(recvBuf->rtm_bufhandle));
    }
    bufferPoolFree(bufferPoolId, bufferAddr);  
    return L7_FAILURE;
  }

  /* Do not allow the Multicast Source Address to be Link Local */
  if (L7_IP6_IS_ADDR_LINK_LOCAL(&(mfcEntry.source)) != 0)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES,"Source Address - %s is Link Local",
               inetAddrPrint(&(mfcEntry.source),src));
    if ((recvBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(recvBuf->rtm_bufhandle));
    }
    bufferPoolFree(bufferPoolId, bufferAddr);  
    return L7_FAILURE;
  }

  if (inetAddressSet(L7_AF_INET6, &(ip6Hdr.dst), &(mfcEntry.group)) != L7_SUCCESS)
  {
    if ((recvBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(recvBuf->rtm_bufhandle));
    }
    bufferPoolFree(bufferPoolId, bufferAddr);  
    return L7_FAILURE;
  }
  MFC_DEBUG_ADDR(MFC_DEBUG_RX, "Rx'ed pkt for group : ", &mfcEntry.group);
  MFC_DEBUG_ADDR(MFC_DEBUG_RX, "Rx'ed pkt for source: ", &mfcEntry.source);
  MFC_DEBUG(MFC_DEBUG_RX, "Rx'ed pkt on iface  :  %d", rtrIfNum);
  if (inetIsInMulticast(&mfcEntry.group) != L7_TRUE)
  {
    MFC_DEBUG_ADDR(MFC_DEBUG_RX, "Rx'ed pkt for non- multicast group : ", &mfcEntry.group);
    if ((recvBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(recvBuf->rtm_bufhandle));
    }
    bufferPoolFree(bufferPoolId, bufferAddr);  
    return L7_FAILURE;
  }

  mfcEntry.iif = rtrIfNum;
  mfcEntry.mcastProtocol = L7_MRP_UNKNOWN;
  mfcEntry.dataTTL = (L7_uint32)(ip6Hdr.hoplim);
  mfcEntry.m   = recvBuf;

  if (L7_IP6_IS_ADDR_MULTICAST_LOC_SCOPE(&(mfcEntry.group.addr.ipv6)))
  {
    MFC_DEBUG(MFC_DEBUG_RX,"\n Received link local multicast packet,ignoring it");
    if ((recvBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(recvBuf->rtm_bufhandle));
    }
    bufferPoolFree(bufferPoolId, bufferAddr);
    return L7_FAILURE;
  }

  if (mfcMroutePktForward(&mfcEntry) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,"\n Failed to forward packet ");
    if ((recvBuf->rtm_bufhandle) != L7_NULLPTR)
    {
      SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(recvBuf->rtm_bufhandle));
    }
    bufferPoolFree(bufferPoolId, bufferAddr);
    return L7_FAILURE;
  }
  if ((recvBuf->rtm_bufhandle) != L7_NULLPTR)
  {
    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)(recvBuf->rtm_bufhandle));
  }
  bufferPoolFree(bufferPoolId, bufferAddr);
  return L7_SUCCESS;
}  

/*********************************************************************
* @purpose  Updates interface status in IP stack
*
* @param    rtrIfNum  @b{(input)} interface whose statuc has changed       
* @param    mode      @b{(input)} L7_ENABLE/ L7_DISABLE
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments    As of now, we only have IPv6 socket support. 
*
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6HwInterfaceStatusChange(L7_uint32 rtrIfNum, L7_BOOL mode)
{
  L7_uint32     intIfNum;

  if (ip6MapRtrIntfToIntIfNum(rtrIfNum,&intIfNum) != L7_SUCCESS)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES,
             " Failed to convert to internal interface number for rtrIfNum : %d",
             rtrIfNum);  
    return L7_FAILURE;
  }

  /* Enable/Disable the IPv6 multicast functionality
     on an interface in the underlying IP stack */

  if (osapiIpIpv6MRTMifSet(mfcIp6SockId,
                           IPPROTO_IPV6,
                           (mode == L7_ENABLE) ? 
                           L7_TRUE : L7_FALSE,
                           intIfNum) == L7_FAILURE)
  {
    MFC_DEBUG(MFC_DEBUG_FAILURES, " Failed to enable multicast on intIfNum = %d , "
                "socket = %d ",intIfNum,mfcIp6SockId);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check for invalid/rogue multicast data packets
*
* @param    hookId      @b{(input)} hook location
* @param    bufHandle   @b{(input)} frame buffer handle
* @param    *pduInfo    @b{(input)} ptr to frame info
* @param    contFuncPtr @b{(input)} optional continuation function ptr
*
* @returns  SYSNET_PDU_RC_DISCARD frame to be discarded (stop further processing)
* @returns  SYSNET_PDU_RC_IGNORED frame has been ignored (continue processing it)
*
* @comments    The IPv6 multicast MAC address should match the IPv6 address,
*           failing which we shall discard the packet as it is invalid.
*       
* @end
*
*********************************************************************/
static
SYSNET_PDU_RC_t mfcIpv6DataIntercept(L7_uint32 hookId,
                                     L7_netBufHandle bufHandle,
                                     sysnet_pdu_info_t *pduInfo,
                                     L7_FUNCPTR_t contFuncPtr)
{
  /* This function is called for IPv6 Multicast packets */

  L7_char8          *data;
  L7_uint32         offset;
  L7_ip6Header_t    *ip6_header;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  offset = sysNetDataOffsetGet(data);
  ip6_header = (L7_ip6Header_t *)(data + offset);

  /* Conditions:-
   * (a) The MAC DA should have first 2 octets as 0x3333
   * (b) The last 4 octets of MAC DA should match the IPv6 DA
   */
  if (!((memcmp(data, MCAST_MAC_IP6_DA_PREFIX, MCAST_MAC_IP6_DA_PREFIX_LEN) == 0)
        && (memcmp(&ip6_header->dst[L7_IP6_ADDR_LEN - MCAST_MAC_IP6_DA_MATCH_BYTES],
                   &data[L7_MAC_ADDR_LEN - MCAST_MAC_IP6_DA_MATCH_BYTES],
                   MCAST_MAC_IP6_DA_MATCH_BYTES) == 0)))
  {
    MFC_DEBUG(MFC_DEBUG_RX, "Addr-mismatch. Data packet discarded");    
    return SYSNET_PDU_RC_DISCARD;
  }
    
 return SYSNET_PDU_RC_IGNORED;  
}

/*********************************************************************
* @purpose  Register with sysnet to intercept Ipv6 packets before 
*           copied onto stack
*
* @param    none
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE 
*
* @comments  MFC registers with Sysnet to intercept IPv6 packets and verifies 
*            whether the mapping between the MAC destination address and IPV6 
*            destination group address and allows only valid packets to be 
*            copied onto the stack.
*            
*       
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SysnetPduInterceptRegister(void)
{
  sysnetPduIntercept_t sysnetPduIntercept;

  /*-----------------------------------------------------------------------*/
  /*    Register for SysNet packet intercept for IPv6 multicast packets    */
  /*-----------------------------------------------------------------------*/
  sysnetPduIntercept.addressFamily = L7_AF_INET6;
  sysnetPduIntercept.hookId = SYSNET_INET6_MCAST_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_MFC_FILTER_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = mfcIpv6DataIntercept;
  osapiStrncpy(sysnetPduIntercept.interceptFuncName, "mfcIpv6DataIntercept",
               sizeof(sysnetPduIntercept.interceptFuncName));
  if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    MFC_LOG_MSG (L7_AF_INET6, "Sysnet PDU Intercept Registration Failed for "
                 "IPv6 Data packets reception");
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  DeRegister with sysnet to intercept Ipv6 packets  
*
* @param    none
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE 
*
* @comments    
*       
* @end
*
*********************************************************************/
L7_RC_t mfcIpv6SysnetPduInterceptDeregister(void)
{
  sysnetPduIntercept_t sysnetPduIntercept;

  /*-----------------------------------------------------------------------*/
  /*    Register for SysNet packet intercept for IPv6 multicast packets    */
  /*-----------------------------------------------------------------------*/
  sysnetPduIntercept.addressFamily = L7_AF_INET6;
  sysnetPduIntercept.hookId = SYSNET_INET6_MCAST_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_MFC_FILTER_PRECEDENCE;
  sysnetPduIntercept.interceptFunc = mfcIpv6DataIntercept;
  osapiStrncpy(sysnetPduIntercept.interceptFuncName, "mfcIpv6DataIntercept",
               sizeof(sysnetPduIntercept.interceptFuncName));
  if (sysNetPduInterceptDeregister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    MFC_DEBUG (MFC_DEBUG_FAILURES, " Failed to deregister to sysnet to intercept IPv6 data pkts\n");
  }
  return L7_SUCCESS;
}


