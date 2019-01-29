/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_util.c
*
* @purpose   Utilities for captive portal wired interface owner.
*
* @component captive portal wired interface owner
*
* @comments  none
*
* @create    2/25/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#include "nimapi.h"
#include "captive_portal_commdefs.h"
#include "log.h"
#include "osapi_support.h"
#include "l7_packet.h"
#include "sysapi.h"
#include "simapi.h"
#include "dot1q_api.h"

#include "wio_util.h"
#include "wio_outcalls.h"

#include "cli_txt_cfg_api.h"

/* rather than put this on the stack or make it a static global, let's
 * allocate a pseudo packet buffer once at startup and just use it over
 * and over again. */
L7_uchar8 *pseudo_pkt = NULL;

extern wioInfo_t *wioInfo;

static L7_BOOL wioIsValidIntfType(L7_uint32 sysIntfType);


/*********************************************************************
* @purpose  Calculates checksum.
*
* @param    addr         @b{(input)} data buffer
* @param    len          @b{(input)} length of data
* @param    csum         @b{(input)} checksum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Set csum to 0 to compute checksum for IP or TCP.
*
* @end
*********************************************************************/
L7_ushort16 wioCheckSum(L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum)
{
  register L7_uint32 nleft = len;
  const    L7_ushort16 *w = addr;
  register L7_ushort16 answer;
  register L7_uint32 sum = csum;

  while(nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }
  if(nleft == 1)
    sum += osapiHtons(*(L7_uchar8*)w << 8);
  sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
  sum += (sum >> 16);                     /* add carry */
  answer = ~sum;                          /* truncate to 16 bits */
  return(answer);
}

/*********************************************************************
* @purpose  Update TCP checksum in a redirected packet.
*
* @param    ipHeader    @b{(input)}  beginning of IP header, followed by TCP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioUpdateTcpChecksum(L7_ipHeader_t *ipHeader)
{
  L7_tcpHeader_t *tcp;
  pseudo_header_t pseudoHeader;
  L7_ushort16 ipHeaderLen = wioIpHdrLen(ipHeader);
  L7_ushort16 tcpPacketLen = ipHeader->iph_len - ipHeaderLen;  /* sans IP header */

  /* Punt if not a TCP packet */
  if (ipHeader->iph_prot != IP_PROT_TCP)
    return L7_FAILURE;

  /* If packet is too big for our pseudo packet buffer, bail out. */
  if ((sizeof(pseudo_header_t) + tcpPacketLen) > WIO_TCP_PSEUDO_PKT_LEN)
  {
    wioInfo->debugStats.messageTooBig++;
    return L7_FAILURE;
  }

  tcp = (L7_tcpHeader_t *)((L7_char8 *)ipHeader + ipHeaderLen);
  tcp->checksum = 0;

  /* Build pseudo header */
  pseudoHeader.protocol = IP_PROT_TCP;
  pseudoHeader.length = tcpPacketLen;                /* pseudo header length not incl here */
  pseudoHeader.place_holder = 0;
  pseudoHeader.source_address = ipHeader->iph_src;   /* net byte order */
  pseudoHeader.dest_address = ipHeader->iph_dst;     /* ditto */

  /* copy pseudo header and TCP packet into pseudo packet buffer */
  memcpy(pseudo_pkt, &pseudoHeader, sizeof(pseudo_header_t));
  memcpy(pseudo_pkt + sizeof(pseudo_header_t), tcp, tcpPacketLen);
  tcp->checksum = wioCheckSum((L7_ushort16*)pseudo_pkt, sizeof(pseudo_header_t) + tcpPacketLen, 0);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the interface type is valid for captive portal
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
static L7_BOOL wioIsValidIntfType(L7_uint32 sysIntfType)
{
  return (sysIntfType == L7_PHYSICAL_INTF);
}

/*********************************************************************
* @purpose  Determine if the interface is valid for participation in captive portal
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL wioIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return wioIsValidIntfType(sysIntfType);
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get the IPv4 address of the local authentication server.
*
* @param    void
*
* @returns  IPv4 address
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 wioAuthServerAddrGet(void)
{
  return wioInfo->authServerAddr;
}

/*********************************************************************
* @purpose  Get the interface info structure for a given interface.
*
* @param    intIfNum  @b((input)) the interface in question
*
* @returns  pointer to the interface structure
*
* @notes    valid intIfNum range is [1, WIO_MAX_INTERFACES]
*
* @end
*********************************************************************/
wioIntfInfo_t *wioIntfInfoGet(L7_uint32 intIfNum)
{
  if (!wioIsValidIntf(intIfNum))
    return NULL;

  if ((intIfNum < 1) || (intIfNum > WIO_MAX_INTERFACES))
    return NULL;

  return &wioInfo->wioIntfInfo[intIfNum];
}

/*********************************************************************
* @purpose  Determine whether captive portal is enabled on a given wired interface.
*
* @param    intIfNum  @b((input)) the interface in question
*
* @returns  L7_TRUE if captive portal is enabled
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL wioIntfIsEnabled(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);
  if (intfInfo)
    return (intfInfo->wioIntfState == WIO_INTF_STATE_ENABLED);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether an interface has been administratively blocked.
*
* @param    intIfNum  @b((input)) the interface in question
*
* @returns  L7_TRUE if interface is blocked
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL wioIntfIsBlocked(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);
  if (intfInfo)
    return (intfInfo->wioIntfBlock == WIO_INTF_BLOCKED);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether captive portal is active on one or more wired interfaces.
*
* @param    void
*
* @returns  L7_TRUE if CP is active on a wired interface
*
* @notes    Interface must be enabled and not blocked
*
* @end
*********************************************************************/
L7_BOOL wioCpIsActive(void)
{
  L7_uint32 i;

  for (i = 1; i <= WIO_MAX_INTERFACES; i++)
  {
    if (wioIntfIsEnabled(i) && !wioIntfIsBlocked(i))
    {
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get the port number of the optional web server port for an interface.
*
* @param    intIfNum  @b((input)) the interface in question
*
* @returns  L7_TRUE if interface is blocked
*
* @notes
*
* @end
*********************************************************************/
L7_ushort16 wioOptWebServerPortGet(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);
  if (intfInfo)
    return (intfInfo->webServerPort);
  return 0;
}

/*********************************************************************
* @purpose  Get the first optional https port number for an interface.
*
* @param    intIfNum  @b((input)) the interface in question
*
* @returns  L7_TRUE if interface is blocked
*
* @notes
*
* @end
*********************************************************************/
L7_ushort16 wioOptSecureWebServerPort1Get(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);
  if (intfInfo)
    return (intfInfo->secureWebServerPort1);
  return 0;
}

/*********************************************************************
* @purpose  Get the second optional https port number for an interface.
*
* @param    intIfNum  @b((input)) the interface in question
*
* @returns  L7_TRUE if interface is blocked
*
* @notes
*
* @end
*********************************************************************/
L7_ushort16 wioOptSecureWebServerPort2Get(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);
  if (intfInfo)
    return (intfInfo->secureWebServerPort2);
  return 0;
}

/*********************************************************************
* @purpose  Determine whether a given interface has a given CP capability.
*
* @param    intIfNum    @b((input))  the interface in question
* @param    capability  @b((input))  the capability in question
*
* @returns  L7_TRUE if interface is blocked
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL wioIntfIsCapable(L7_uint32 intIfNum, L7_INTF_PARM_CP_TYPES_t capability)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);
  if (intfInfo)
    return (intfInfo->capabilities & capability);
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Get the set of CP capabilities supported on a wired interface.
*
* @param    intIfNum    @b((input))  the interface in question
*
* @returns  Bit mask of capabilities
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 wioIntfCapabilities(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);
  if (intfInfo)
    return intfInfo->capabilities;
  return 0;
}

/*********************************************************************
* @purpose  Set the default capabilities on all wired interfaces.
*
* @param    void
*
* @returns
*
* @notes    Only the session timeout is supported.
*           Doesn't use wioIntfInfoGet(), because when this is called,
*           interfaces are not created yet, and NIM calls will fail.
*
* @end
*********************************************************************/
L7_RC_t wioDefaultCapabilitiesSet(void)
{
  L7_uint32 i;
  wioIntfInfo_t *intfInfo;

  for (i = 1; i <= WIO_MAX_INTERFACES; i++)
  {
    intfInfo = &wioInfo->wioIntfInfo[i];
    intfInfo->capabilities = L7_INTF_PARM_CP_SESTIMEOUT;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether the source IPv4 address in an incoming packet
*           is a valid client IP address.
*
* @param    srcIPAddr  @b((input)) source IPv4 address from received packet
*
* @returns  L7_TRUE if address is invalid
*
* @notes    Allow source address of 0.0.0.0, which is used when client
*           sends DHCP Discover.
*
* @end
*********************************************************************/
L7_BOOL wioSourceIpAddressInvalid(L7_uint32 srcIpAddr)
{
  return (((srcIpAddr & L7_IP_LOOPBACK_ADDR_MASK) == L7_IP_LOOPBACK_ADDR_NETWORK) ||
          ((srcIpAddr & L7_CLASS_D_ADDR_NETWORK) == L7_CLASS_D_ADDR_NETWORK) ||
          ((srcIpAddr & L7_CLASS_E_ADDR_NETWORK) == L7_CLASS_E_ADDR_NETWORK));
}

/*********************************************************************
* @purpose  Returns the length of an IP header in bytes.
*
* @param    ipHeader  @b{(input)} IP header
*
* @returns  length of IP header in bytes
*
* @notes
*
* @end
*********************************************************************/
L7_ushort16 wioIpHdrLen(L7_ipHeader_t *ipHeader)
{
  return 4 * (0x0F & *((L7_uchar8*)ipHeader));       /* bytes */
}

/*********************************************************************
* @purpose  Get the protocol ID from an IPv4 packet and if TCP or UDP,
*           get the destination TCP or UDP port number.
*
* @param    ipHeader  @b((input)) received packet, starting with IPv4 header
* @param    protocol  @b((output)) IP protocol ID
* @param    destPort  @b((output)) destination port number
*
* @returns  L7_SUCCESS
*           L7_ERROR if either protocol or destPort are NULL
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioIpPacketInfoGet(L7_ipHeader_t *ipHeader,
                           L7_uchar8 *protocol, L7_ushort16 *destPort)
{
  L7_udp_header_t *udpHeader;

  if (!protocol || !destPort)
    return L7_ERROR;

  *protocol = ipHeader->iph_prot;
  if ((*protocol == IP_PROT_UDP) || (*protocol == IP_PROT_TCP))
  {
    /* TCP and UDP headers both start with src port and dest port. So we can
     * cast both to a udp header for our purposes here. */
    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader + wioIpHdrLen(ipHeader));
    *destPort = osapiNtohs(udpHeader->destPort);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether CP should discard a given packet sent by
*           an unauthenticated client.
*
* @param    ipHeader  @b((input)) received packet, starting with IPv4 header
*
* @returns  L7_TRUE if address is invalid
*
* @notes    Let DHCP and DNS packets through. Discard others. DNS can be TCP or UDP.
*
* @end
*********************************************************************/
L7_BOOL wioPassUnauthClientPacket(L7_ipHeader_t *ipHeader)
{
  L7_uchar8 protocol;
  L7_ushort16 destPort;

  if (wioIpPacketInfoGet(ipHeader, &protocol, &destPort) != L7_SUCCESS)
    return L7_FALSE;

  if (((protocol == IP_PROT_UDP) &&
       ((destPort == UDP_PORT_DHCP_SERV) || (destPort == UDP_PORT_DNS))) ||
      ((protocol == IP_PROT_TCP) && (destPort == TCP_PORT_DNS)))
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether an incoming packet is one that should be
*           redirected to the authentication server.
*
* @param    intIfNum  @b((input)) interface where packet arrived
* @param    ipHeader  @b((input)) received packet, starting with IPv4 header
*
* @returns  L7_TRUE if packet should be redirected
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL wioRedirectPacketType(L7_uint32 intIfNum, L7_ipHeader_t *ipHeader)
{
  L7_uchar8 protocol;
  L7_ushort16 destPort;
  L7_ushort16 httpPort = wioOptWebServerPortGet(intIfNum);
  L7_ushort16 httpsPort1 = wioOptSecureWebServerPort1Get(intIfNum);
  L7_ushort16 httpsPort2 = wioOptSecureWebServerPort2Get(intIfNum);

  if (wioIpPacketInfoGet(ipHeader, &protocol, &destPort) != L7_SUCCESS)
    return L7_FALSE;

  if (protocol != IP_PROT_TCP)
    return L7_FALSE;
  if ((destPort == TCP_PORT_HTTP) || (destPort == TCP_PORT_HTTPS))
    return L7_TRUE;

  /* Does destination port number match a user-configured port? */
  if (((httpPort != 0) && (destPort == httpPort)) ||
      ((httpsPort1 != 0) && (destPort == httpsPort1)) ||
      ((httpsPort2 != 0) && (destPort == httpsPort2)))
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}


/*********************************************************************
* @purpose  Redirect an incoming IPv4 packet to the local captive portal
*           authentication server.
*
* @param    ethHeader @b((input)) start of ethernet header
* @param    ipHeader  @b((input)) received packet, starting with IPv4 header
* @param    pduInfo   @b((input)) This function may modify the ingress
*                                 interface and VLAN ID.
*
* @returns  L7_SUCCESS if all goes well
*
* @notes    Changes the destination IPv4 address and affected checksums.
*           May change destination MAC address and fake the ingress
*           interface and VLAN ID.
*
* @end
*********************************************************************/
L7_RC_t wioPacketRedirect(L7_enetHeader_t *ethHeader, L7_ipHeader_t *ipHeader,
                          sysnet_pdu_info_t *pduInfo)
{
  L7_uint32 authServerAddr = wioAuthServerAddrGet();
  L7_uchar8 localMac[L7_MAC_ADDR_LEN];    /* MAC address on ingress interface */
  L7_uint32 i;
  NIM_INTF_MASK_t portMask;

  if (authServerAddr == 0)
  {
    wioInfo->debugStats.noAuthServerAddr++;
    return L7_FAILURE;
  }

  ipHeader->iph_dst = osapiHtonl(authServerAddr);
  ipHeader->iph_csum = 0;
  ipHeader->iph_csum = wioCheckSum((L7_ushort16*)ipHeader, L7_IP_HDR_LEN, 0);

  if (wioUpdateTcpChecksum(ipHeader) != L7_SUCCESS)
    return L7_FAILURE;

  if (authServerAddr == simGetSystemIPAddr())
  {
    /* Since auth server is using the IPv4 address from the network port, there
     * cannot be any routing interfaces on this box. Therefore, the packet must
     * have arrived on a layer 2 port. Pretend packet came in mgmt VLAN destined
     * for network port IP addr. */
    pduInfo->vlanId = simMgmtVlanIdGet();
    simGetSystemIPBurnedInMac(ethHeader->dest.addr);

    /* Make sure ingress interface is a member of the mgmt VLAN. If it's not
     * ipMapPduRcv() won't send the packet to the end driver. */
    if (!dot1qIntfIsVlanMember(pduInfo->vlanId, pduInfo->intIfNum))
    {
       if (dot1qVlanEgressPortsGet(pduInfo->vlanId, &portMask) == L7_SUCCESS)
       {
         for (i = 1; i <= WIO_MAX_INTERFACES; i++)
         {
           if (L7_INTF_ISMASKBITSET(portMask, i))
           {
             pduInfo->intIfNum = i;
             break;
           }
         }
       }
    }
  }
  else
  {
    /* authentication server is using an IPv4 address on a routing interface. */
    if (nimGetIntfL3MacAddress(pduInfo->intIfNum, 0, localMac) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    if (memcmp(localMac, ethHeader->dest.addr, L7_MAC_ADDR_LEN))
    {
      /* destination MAC wasn't us. Pretend packet arrived on routing interface
       * where authentication server IPv4 address is configured. Don't think
       * we need to change the ingress VLAN ID, since IP MAP would use this
       * to determine the ingress routing interface, but we already have that. */
      memcpy(ethHeader->dest.addr, localMac, L7_MAC_ADDR_LEN);
      pduInfo->intIfNum = wioInfo->authServerIntf;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Modify the source IP address in an outgoing packet to a client.
*
* @param    clientConn  @b((input)) client connection params
* @param    bufHandle   @b((input)) mbuf containing packet being sent
*
* @returns  L7_SUCCESS if all goes well
*
* @notes    Changes the source IPv4 address and affected checksums.
*
*           Note that transmitted packets can have an OS buffer
*           attached to them, so calling SYSAPI_NET_MBUF_GET_DATASTART
*           doesn't return a pointer to the real data.  So we must use
*           sysapiNetMbufGetNextBuffer() instead. Since packet can be
*           spread across multiple os buffers, need to first copy packet
*           to a contiguous buffer. We'll recompute TCP checksum on that
*           and then copy the new checksum into the original os buffer.
*
* @end
*********************************************************************/
L7_RC_t wioModifySourceAddress(wioClientConn_t *clientConn,
                               L7_netBufHandle bufHandle)
{
  L7_ipHeader_t *ipHeader;
  L7_tcpHeader_t *tcpHeader;
  L7_uint32 ethHeaderSize;
  L7_uint32 bytesCopied = 0;
  L7_uchar8 pktCopy[2048];
  L7_netBlockHandle blockHandle = L7_NULL;
  L7_uchar8 *data;
  L7_uchar8 *dataStart;   /* start of first os buffer */
  L7_uint32 len;
  L7_ushort16 newTcpCsum;

  /* Copy data to temporary buffer */
  sysapiNetMbufGetNextBuffer(bufHandle, &blockHandle, &data, &len);
  ethHeaderSize = sysNetDataOffsetGet(data);
  if ((data == NULL) || (len < ethHeaderSize))
    return L7_FAILURE;
  dataStart = data;     /* remember start of packet in original buffer */
  while (blockHandle)
  {
    if (bytesCopied + len > 2048)
      return L7_FAILURE;
    memcpy(pktCopy + bytesCopied, data, len);
    bytesCopied += len;
    sysapiNetMbufGetNextBuffer(bufHandle, &blockHandle, &data, &len);
  }
  if (bytesCopied == 0)
    return L7_FAILURE;

  /* Update TCP checksum in copy */
  ipHeader = (L7_ipHeader_t*) (pktCopy + ethHeaderSize);
  ipHeader->iph_src = osapiHtonl(clientConn->remoteIpAddr);
  wioUpdateTcpChecksum(ipHeader);
  tcpHeader = (L7_tcpHeader_t*) ((L7_uchar8*) ipHeader + wioIpHdrLen(ipHeader));
  newTcpCsum = tcpHeader->checksum;

  /* Now update original packet, again assuming 1st os buffer large enough to
   * contain the TCP header. */
  ipHeader = (L7_ipHeader_t*) (dataStart + ethHeaderSize);
  ipHeader->iph_src = osapiHtonl(clientConn->remoteIpAddr);
  ipHeader->iph_csum = 0;
  ipHeader->iph_csum = wioCheckSum((L7_ushort16*)ipHeader, L7_IP_HDR_LEN, 0);
  tcpHeader = (L7_tcpHeader_t*) ((L7_uchar8*)ipHeader + wioIpHdrLen(ipHeader));
  tcpHeader->checksum = newTcpCsum;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given an incoming packet from a client in the AUTH PENDING state,
*           see if the packet is part of an existing connection with the
*           local auth server. If not, add the connection to the connection
*           list.
*
* @param    client    @b((input)) entry on client list
* @param    intIfNum  @b((input)) interface where packet was received
* @param    ipHeader  @b((input)) received packet, starting with IPv4 header
*
* @returns  L7_SUCCESS if all goes well
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioUpdateClientConnList(wioClient_t *client,
                                L7_uint32 intIfNum, L7_ipHeader_t *ipHeader)
{
  wioClientConn_t *conn;
  L7_uint32 clientIpAddr = osapiNtohl(ipHeader->iph_src);
  L7_uint32 remoteIpAddr = osapiNtohl(ipHeader->iph_dst);
  L7_tcpHeader_t *tcpHeader;
  L7_ushort16 clientTcpPort;

  /* sanity check */
  if (ipHeader->iph_prot != IP_PROT_TCP)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
           "Attempt to use non-TCP packet to update captive portal wired connection list.");
    return L7_FAILURE;
  }

  tcpHeader = (L7_tcpHeader_t *)((L7_char8 *)ipHeader + wioIpHdrLen(ipHeader));
  clientTcpPort = osapiNtohs(tcpHeader->srcPort);

  conn = wioClientConnFind(intIfNum, clientIpAddr, clientTcpPort);
  if (conn)
  {
    /* see if client TCP port now talking to a different remote address */
    if (conn->remoteIpAddr != remoteIpAddr)
    {
      wioInfo->debugStats.remoteAddrChange++;
      if (wioInfo->wioTraceFlags & WIO_TRACE_CONN_LIST)
      {
        L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 remIpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiInetNtoa(conn->clientIpAddr, ipAddrStr);
        osapiInetNtoa(remoteIpAddr, remIpAddrStr);
        osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                      "Updated connection on interface %s from client %s:%u to %s",
                      ifName, ipAddrStr, clientTcpPort, remIpAddrStr);
        wioTraceWrite(wioTrace);
      }

      conn->remoteIpAddr = remoteIpAddr;
    }
  }
  else
  {
    wioClientConnAdd(intIfNum, client, clientTcpPort, remoteIpAddr);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the IPv4 address of the local authentication server.
*
* @param    authServer  @b((input)) IPv4 address of authentication server
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioAuthServerAddrSet(L7_uint32 authServer)
{
  wioInfo->authServerAddr = authServer;

  if (wioInfo->wioTraceFlags & WIO_TRACE_AUTH_SERVER)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(authServer, ipAddrStr);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                  "Captive portal authentication server address set to %s.",
                  ipAddrStr);
    wioTraceWrite(wioTrace);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle captive portal enable event on a wired interface.
*
* @param    intIfNum  @b((input)) interface where CP has been enabled
* @param    authServer  @b((input)) IPv4 address of authentication server
*
* @returns  L7_SUCCESS
*           L7_FAILURE if interface not a valid captive portal wired interface
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioIntfEnable(L7_uint32 intIfNum, L7_uint32 authServer)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);

  if (!intfInfo)
    return L7_FAILURE;

  if (wioInfo->wioTraceFlags & WIO_TRACE_CP_STATE)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                  "Captive portal enabled on interface %s.", ifName);
    wioTraceWrite(wioTrace);
  }

  /* Auth server uses a local IP address. If this is an address on a
   * routing interface, get the internal interface number of that
   * routing interface. */
  if ((wioInfo->authServerAddr != authServer) &&
      (authServer != simGetSystemIPAddr()))
  {
    wioInfo->authServerIntf = wioAuthServerIntfGet(authServer);
    if (wioInfo->authServerIntf == 0)
    {
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(authServer, ipAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Failed to find routing interface for authentication server address %s.",
            ipAddrStr);
    }
  }

  intfInfo->wioIntfState = WIO_INTF_STATE_ENABLED;
  wioAuthServerAddrSet(authServer);
  intfInfo->capabilities |= L7_INTF_PARM_CP_MODE;

  /* If this is the first interface where CP is active, register
   * sysnet intercepts. */
  if ((!wioInfo->isRegWithSysnet) && !wioIntfIsBlocked(intIfNum))
  {
    wioSysnetRegister();
  }

  /* Notify hardware */
  if (dtlCaptivePortalConfig(intIfNum, L7_TRUE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Failed to enable captive portal on interface %s in the hardware.",
            ifName);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle captive portal disable event on a wired interface.
*
* @param    intIfNum  @b((input)) interface where CP has been disabled
*
* @returns  L7_SUCCESS if all goes well
*           L7_FAILURE if interface not a valid captive portal wired interface
*
* @notes    All state on this interface is cleared.
*
* @end
*********************************************************************/
L7_RC_t wioIntfDisable(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);

  if (!intfInfo)
    return L7_FAILURE;

  if (wioInfo->wioTraceFlags & WIO_TRACE_CP_STATE)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                  "Captive portal disabled on interface %s.", ifName);
    wioTraceWrite(wioTrace);
  }

  intfInfo->wioIntfState = WIO_INTF_STATE_DISABLED;
  if (intfInfo->wioIntfBlock == WIO_INTF_BLOCKED)
  {
    intfInfo->wioIntfBlock = WIO_INTF_UNBLOCKED;
    if (dtlCaptivePortalBlock(intIfNum, L7_FALSE) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Failed to un-block the interface %s in the hardware.", ifName);
    }
  }
  intfInfo->webServerPort = 0;
  intfInfo->secureWebServerPort1 = 0;
  intfInfo->secureWebServerPort2 = 0;
  intfInfo->capabilities &= ~L7_INTF_PARM_CP_MODE;

  /* Delete all clients on this interface */
  wioClientsOnIntfRemove(intIfNum);

  /* Delete all client connections on this interface */
  wioClientConnectionsRemove(intIfNum, 0);   /* all clients */

  wioIntfStatsClear(intIfNum);

  /* If this was the only wired interface where CP was active, stop
   * intercepting packets. */
  if (wioInfo->isRegWithSysnet && !wioCpIsActive())
  {
    wioSysnetDeregister();
  }

  /* Notify hardware. */
  if (dtlCaptivePortalConfig(intIfNum, L7_FALSE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Failed to disable captive portal on interface %s (%u) in the hardware.",
            ifName, intIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set an interface to be administratively blocked
*
* @param    intIfNum  @b((input)) interface now blocked
*
* @returns  L7_SUCCESS if all goes well
*           L7_FAILURE if interface not a valid captive portal wired interface
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioIntfBlock(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);

  if (!intfInfo)
    return L7_FAILURE;

  intfInfo->wioIntfBlock = WIO_INTF_BLOCKED;

  if (wioInfo->wioTraceFlags & WIO_TRACE_BLOCK)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                  "Interface %s now administratively blocked.", ifName);
    wioTraceWrite(wioTrace);
  }

  /* If this was the only wired interface where CP was active, stop
   * intercepting packets. */
  if (wioInfo->isRegWithSysnet && !wioCpIsActive())
  {
    wioSysnetDeregister();
  }

  /* tell hardware interface is blocked */
  if (dtlCaptivePortalBlock(intIfNum, L7_TRUE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Failed to block interface %s in the hardware.", ifName);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restore an interface that was administratively blocked
*
* @param    intIfNum  @b((input)) interface where CP has been disabled
*
* @returns  L7_SUCCESS if all goes well
*           L7_FAILURE if interface not a valid captive portal wired interface
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioIntfUnblock(L7_uint32 intIfNum)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);

  if (!intfInfo)
    return L7_FAILURE;

  intfInfo->wioIntfBlock = WIO_INTF_UNBLOCKED;

  if (wioInfo->wioTraceFlags & WIO_TRACE_BLOCK)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN,
                  "Interface %s no longer admistratively blocked.", ifName);
    wioTraceWrite(wioTrace);
  }

  /* If this is the first interface where CP is active, register
   * sysnet intercepts. */
  if ((!wioInfo->isRegWithSysnet) && wioIntfIsEnabled(intIfNum))
  {
    wioSysnetRegister();
  }

  /* tell hardware interface is no longer blocked */
  if (dtlCaptivePortalBlock(intIfNum, L7_FALSE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Failed to un-block interface %s in the hardware.", ifName);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle notice from captive portal that a client has been authenticated.
*
* @param    clientMac  @b((input)) MAC address of newly authenticated client
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioClientAuth(L7_enetMacAddr_t *clientMac)
{
  return wioClientStateSet(clientMac, WIO_CLIENT_AUTH);
}

/*********************************************************************
* @purpose  Handle notice from captive portal that a client is unauthenticated.
*
* @param    clientMac  @b((input)) MAC address of newly authenticated client
*
* @returns  L7_SUCCESS
*
* @notes    This is generally a result of telling CP about a new client. It is
*           an indication that authentication is in process, not that the
*           client was authenticated and has been deauthenticated.
*
* @end
*********************************************************************/
L7_RC_t wioClientUnauth(L7_enetMacAddr_t *clientMac)
{
  return wioClientStateSet(clientMac, WIO_CLIENT_PENDING);
}

/*********************************************************************
* @purpose  Handle notice from captive portal that CP has deauthenticated a
*           previously authenticated client.
*
* @param    clientMac  @b((input)) MAC address of newly authenticated client
*
* @returns  L7_SUCCESS
*
* @notes    When client is deauthenticated, we delete him.
*
* @end
*********************************************************************/
L7_RC_t wioClientDeauth(L7_enetMacAddr_t *clientMac)
{
  return wioClientRemove(clientMac);
}

/*********************************************************************
* @purpose  Set the optional web server TCP port for a given interface.
*
* @param    intIfNum       @b((input))  interface
* @param    tcpPortNum     @b((input))  TCP port number
* @param    optPortFlavor  @b((input))  TCP port number
*
* @returns  L7_SUCCESS
*
* @notes    Packets arriving on this interface with this destination TCP
*           port number are to be treated like http and https for
*           captive portal authentication purposes.
*
* @end
*********************************************************************/
L7_RC_t wioOptWebServerPortSet(L7_uint32 intIfNum, L7_ushort16 tcpPortNum,
                               e_WioOptPortFlavor flavor)
{
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(intIfNum);

  if (!intfInfo)
    return L7_FAILURE;

  switch (flavor)
  {
    case WIO_PORT_FLAVOR_HTTP:
      intfInfo->webServerPort = tcpPortNum;
      break;

    case WIO_PORT_FLAVOR_HTTPS_1:
      intfInfo->secureWebServerPort1 = tcpPortNum;
      break;

    case WIO_PORT_FLAVOR_HTTPS_2:
      intfInfo->secureWebServerPort2 = tcpPortNum;
      break;

    default:
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Unknown optional port type %d", flavor);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  WIO callback to process configuration apply completion event
*
* @param    L7_uint32 event @b{(input)} txtCfgApplyEvent_t type event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioTxtCfgApplyCompleteCallback(L7_uint32 event)
{
  wioEventMsg_t msg;

  if (!(WIO_IS_READY))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CP_WIO_COMPONENT_ID,
            "Received cfg apply completion event %u in init state %s.",
            event, wioInfo->cnfgrState);
  }

  if ((event != TXT_CFG_APPLY_FAILURE) && (event != TXT_CFG_APPLY_SUCCESS))
  {
    return L7_SUCCESS;
  }

  msg.msgType = WIO_TXT_CFG_COMPLETE;
  msg.wioMsgData.txtCfgMsg.event = event;
  if (osapiMessageSend(wioInfo->wioMsgQ, &msg, sizeof(wioEventMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CP_WIO_COMPONENT_ID,
            "Failed to send cfg complete event %u msg.", event);
    return L7_FAILURE;
  }

  osapiSemaGive(wioInfo->msgQSema);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process WIO configuration apply completion event
*
* @param    L7_uint32 event @b{(input)} txtCfgApplyEvent_t type event
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t wioTxtCfgApplyComplete(L7_uint32 event)
{
  cnfgrApiComponentHwUpdateDone(L7_CP_WIO_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  return L7_SUCCESS;
}

