/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   relay_main.c
*
* @purpose    DHCP Relay Implementation
*
* @component  DHCP Relay Component
*
* @comments   Significant revision May 2008, rrice.
*
*             This component started out as just DHCP relay. It has been updated
*             to relay protocols other than DHCP, too. The commands to configure
*             server addresses are called "IP helper." So a number of the new
*             functions have an "ih" prefix. Since this component now relays
*             multiple protocols that run over UDP, it is sometimes referred to 
*             as a "UDP relay" agent. 
*
* @create     11/30/2001
*
* @author     rrice May 2008 revision
* @end
*
**********************************************************************/
#define L7_MAC_ENET_BCAST                    

#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "l7_relayinclude.h"
#include "relaymap_cnfgr.h"
#include "osapi_support.h"
#include "defaultconfig.h"
#include "l7_dhcps_api.h"
#include "buff_api.h"
#include "l7_ip_api.h"
#include "l3_comm_structs.h"
#include "dtlapi.h"
#include "l7utils_api.h"

extern dhcpRelayCfg_t     *pDhcpRelayCfgData;
extern dhcpRelayInfo_t *ihInfo; 

/* Set aside a scratch pad for computing UDP checksum */
extern pseudo_header_t *pseudo_packet;

L7_RC_t ihRelayDhcpClientMessage(L7_uint32 intIfNum, L7_uint32 ttl, 
                                 L7_dhcp_packet *dhcpPacket, L7_uint32 payloadLength);
L7_RC_t ihRelayDhcpServerMessage(L7_dhcp_packet *dhcpPacket, L7_uint32 dhcpPacketLength);

#define DHCP_OPTIONS_COOKIE_LEN   4

/*to check whether it's DHCP or BootP Packet  */
L7_uchar8 DHCP_OPTIONS_COOKIE[]= {99, 130, 83, 99, 255};

/*********************************************************************
* @purpose  Main UDP relay task function.
*
* @param
*
* @returns
*
* @notes    Executes all configurator commands (other than going into INIT_1).
*           Also, handles the "clear config" functionality
*           using dhcpRelayRestoreProcess(), NIM events, and packet processing.
*
* @end
*********************************************************************/
void udpRelay_Task(void)
{
  udpRelayMsg_t   Message;
  L7_uint32    status;

  osapiTaskInitDone(L7_DHCP_RELAY_TASK_SYNC);

  while (1)
  {
    /* zero the message */
    memset(&Message, 0x00, sizeof(udpRelayMsg_t));

    /*  Get a Message From The Queue */
    status = osapiMessageReceive(dhcpRelayQueue,
                                 &Message,
                                 sizeof(udpRelayMsg_t),
                                 L7_WAIT_FOREVER);

    if (status == L7_SUCCESS)
    {
      switch (Message.msgId)
      {
        case DHCPRELAYMAP_CNFGR:
          if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dhcpRelayCnfgrParse(&(Message.type.cnfgrCmdData));
            osapiSemaGive(ihInfo->ihLock);
          }
          break;
        case DHCPRELAYMAP_RESTORE_EVENT:
          if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dhcpRelayRestoreProcess();
            osapiSemaGive(ihInfo->ihLock);
          }
          break;
        case DHCPRELAYMAP_RELAY_PKT:
          if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            relayUdpPacket(Message.type.relayPacket.pktBuffer,
                           Message.type.relayPacket.intIfNum,
                           Message.type.relayPacket.rxPort,
                           Message.type.relayPacket.vlanId);
            bufferPoolFree(ihInfo->packetPoolId, (L7_uchar8*) Message.type.relayPacket.pktBuffer);
            osapiSemaGive(ihInfo->ihLock);
          }
          break;
        case DHCPRELAYMAP_INTF_EVENT:
          if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            dhcpRelayIntfChangeProcess(Message.type.intfEvent.intIfNum,
                                       Message.type.intfEvent.event,
                                       Message.type.intfEvent.correlator);
            osapiSemaGive(ihInfo->ihLock);
          }
          break;
        default:
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_RELAY_COMPONENT_ID,
                  "Received invalid message ID %d on message queue.", 
                  Message.msgId);
          break;
      }
    }
  }  
}

/*********************************************************************
* @purpose  Relay a DHCP client message to the configured DHCP servers.
*
* @param    intIfNum         @b{(input)}   Ingress interface for client msg
* @param    ttl              @b{(input)}   TTL to put in relayed msg  
* @param    dhcpPacket       @b{(input)}   Start of DHCP packet
* @param    dhcpPacketLength @b{(input)}   Length of DHCP packet.   
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    Here we go through the IP stack. This is because, a) there's 
*           no reason not to, and b) the stack will pick the source IP 
*           address for us. 
*
* @end
*********************************************************************/
L7_RC_t ihRelayDhcpClientMessage(L7_uint32 intIfNum, L7_uint32 ttl, 
                                 L7_dhcp_packet *dhcpPacket, L7_uint32 payloadLength)
{
  L7_uint32 rc;
  L7_IP_ADDR_t serverAddr;
  L7_uint32 opt = 1;
  L7_uint32 bytesSent;
  L7_sockaddr_in_t from, to;
  L7_uint32 relaySock;
  L7_uint32 localIntf;

  serverAddr = ihServerAddrGetNext(intIfNum, UDP_PORT_BOOTP_SERV, 0, L7_TRUE);
  if (serverAddr == 0)
  {
    ihInfo->ihStats.noServer++;
    return L7_FAILURE;
  }

  rc = osapiSocketCreate(L7_AF_INET, L7_SOCK_DGRAM, 0, &relaySock);
  if (rc != L7_SUCCESS)
  {
    ihInfo->ihStats.noSocket++;
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
            "Failed to open socket.");
    return L7_FAILURE;
  }

  /* Need to set SO_REUSEADDR because local DHCP client (for service port and 
   * network port) also binds a socket to UDP port 68 for INADDR_ANY. SO_REUSEADDR
   * lets us reuse the port number since we are not binding to a specific 
   * unicast address. */
  rc = osapiSetsockopt(relaySock, L7_SOL_SOCKET, L7_SO_REUSEADDR, (L7_uchar8 *)&opt, 
                       sizeof(opt));
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
            "Failed to set SO_REUSEADDR on DHCP relay socket %u.", relaySock);
    /* continue */
  }

  from.sin_family = L7_AF_INET;
  from.sin_port = UDP_PORT_BOOTP_CLNT;
  from.sin_addr.s_addr = L7_INADDR_ANY;
  if (osapiSocketBind(relaySock, (L7_sockaddr_t*) &from, sizeof(from)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
            "Failed to bind socket");
    ihInfo->ihStats.socketBindFailed++;
    osapiSocketClose(relaySock);
    return L7_FAILURE;
  }

  /* Set TTL value which was previously decremented */
  rc = osapiSetsockopt(relaySock, IPPROTO_IP, L7_IP_TTL, (L7_uchar8*) &ttl, sizeof(ttl));
  if (rc != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
            "Failed to set TTL to %u on DHCP relay socket %u.", ttl, relaySock);
    /* continue */
  }

  while (serverAddr)
  {
    /* Don't send if serverAddr is local or is on the interface where the 
     * client message came in. */
    if ((ipMapIpAddressToIntf(serverAddr, &localIntf) != L7_SUCCESS) &&
        !ipMapIpAddrIsLocal(intIfNum, serverAddr))
    {
      if (ihInfo->ihTraceFlags & IH_TRACE_PACKET)
      {
        L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiInetNtoa(serverAddr, destAddrStr);
        osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX, 
                      "IP Helper relaying %u byte DHCP message on interface %s to DHCP server at %s:%u.",
                      payloadLength, ifName, destAddrStr, UDP_PORT_BOOTP_SERV);
        ihTraceWrite(traceBuf);
      }
  
      to.sin_family = L7_AF_INET;
      to.sin_port = UDP_PORT_BOOTP_SERV;
      to.sin_addr.s_addr = osapiHtonl(serverAddr);
      rc = osapiSocketSendto(relaySock, (L7_uchar8*) dhcpPacket, payloadLength, 0, 
                             (L7_sockaddr_t *)&to, sizeof(to), &bytesSent);
      if (rc != L7_SUCCESS)
      {
        L7_uchar8 serverAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        ihInfo->ihStats.socketSendFailed++;
        osapiInetNtoa(serverAddr, serverAddrStr);
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
                "Failed to send packet to DHCP server at %s. errno is %s", 
                serverAddrStr, strerror(osapiErrnoGet()));
      }
      else
      {
        ihInfo->ihStats.publicStats.udpClientMsgsRelayed++;
        ihInfo->ihStats.publicStats.dhcpClientMsgsRelayed++;
      }
    }
    serverAddr = ihServerAddrGetNext(intIfNum, UDP_PORT_BOOTP_SERV, 
                                     serverAddr, L7_TRUE);
  }
    
   osapiSocketClose(relaySock);
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Relay a DHCP server message to the client.
*
* @param    dhcpPacket       @b{(input)}   Start of DHCP packet
* @param    dhcpPacketLength @b{(input)}   Length of DHCP packet.   
*
* @returns  L7_SUCCESS    
* @returns  L7_FAILURE    
*
* @notes    There are a couple reasons why we bypass the IP stack here. 
*           First, when we relay an OFFER, the client won't respond to
*           an ARP request if we are unicasting. Could work around by installing
*           a temporary static ARP entry. Second, if client asks for a 
*           broadcast response (CentOS and Windows XP default to unicast
*           response, so bcast response may be rare), we can limit the tx
*           to just the client's physical port, rather than broadcasting
*           on all ports in the VLAN. Of course, the relay agent has to use
*           option 82, CID suboption, to know the client's phy port. By 
*           bypassing the IP stack, we incur the hassle of building the 
*           L2, IP, and UDP headers ourself, we lose the possibility of 
*           fragmenting large replies, and we lose packet stats on the
*           interface. Tough choice.
*
* @end
*********************************************************************/
L7_RC_t ihRelayDhcpServerMessage(L7_dhcp_packet *dhcpPacket, 
                                 L7_uint32 dhcpPacketLength)
{
  L7_uint32 intIfNum = 0;           /* routing interface where client is attached */
  L7_uint32 clientPort = 0;         /* physical port where client is attached */
  L7_uint32 giaddr;
  L7_uint32 vlanId = 0;             /* VLAN client is on. Default to 0 for port-base if */
  L7_RC_t rc;

  if (pDhcpRelayCfgData->circuitIDOptionMode == L7_ENABLE)
  {
    /* parse option 82 to get physical port to client. Continue even if 
     * this fails */
    dhcpRelayOption82Strip(dhcpPacket, &dhcpPacketLength, &clientPort);
  }

  /* Even if we got option 82 back, we still need to grab giaddr so
   * we know the routing interface to the client. 
   * Check 'giaddr' and compare with one of our interfaces.
   * If matches one of interfaces, client is on that interface. */
  giaddr = osapiNtohl(dhcpPacket->giaddr);
  if (ipMapIpAddressToIntf(giaddr, &intIfNum) != L7_SUCCESS)
  {
    /* 'giaddr' not one of our interface addresses. Drop pkt. */
    ihInfo->ihStats.unknownGiaddr++;
    return L7_FAILURE;
  }
  /* This fails if a port based routing interface. That's ok. Leave vlanId = 0. */
  (void) ipMapVlanRtrIntIfNumToVlanId(intIfNum, &vlanId);


  /* check for BROADCAST field in the packet.. if it's ON, 
   * broadcast the reply, otherwise Unicast it. */
  if (osapiNtohs(dhcpPacket->flags) & BOOTP_BROADCAST)
  {
    rc = dhcpPacketBroadcast(intIfNum, clientPort, vlanId, dhcpPacket, dhcpPacketLength);
  }
  else
  {
    rc = dhcpPacketUnicast(intIfNum, clientPort, vlanId, dhcpPacket, dhcpPacketLength);
  }
  if (rc == L7_SUCCESS)
  {
    ihInfo->ihStats.publicStats.dhcpServerMsgsRelayed++;
  }
  return rc;
}

/*********************************************************************
* @purpose  Check a DHCP client message for validity and update it as
*           required before sending to DHCP server.
*
* @param    dhcpPacket       @b{(input)}   Start of DHCP packet
* @param    dhcpPacketLength @b{(in/out)}  Length of DHCP packet. May change.  
* @param    intIfNum         @b{(input)}   ingress routing interface 
* @param    rxPort           @b{(input)}   ingress physical port
* @param    vlanId           @b{(input)}   ingress VLAN ID
*
* @returns  L7_SUCCESS    packet should be sent to server
* @returns  L7_FAILURE    discard packet
*
* @notes    Check/update hop count, secs, and giaddr. Add relay agent options (opt 82).
*
* @end
*********************************************************************/
L7_RC_t ihModifyDhcpClientMessage(L7_dhcp_packet *dhcpPacket, 
                                  L7_uint32 *dhcpPacketLength,
                                  L7_uint32 intIfNum, L7_uint32 rxPort, 
                                  L7_uint32 vlanId)
{
  L7_IP_ADDR_t intfIpAddr = 0;
  L7_IP_MASK_t mask;
  L7_uint32 giAddrIntIfNum;
  L7_BOOL   addedGiAddr = L7_FALSE;

  if (dhcpPacket->hops > pDhcpRelayCfgData->maxHopCount)
  {
    /* discard packet... increment Relay Statistics...*/
    L7_uchar8 portName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(rxPort, L7_SYSNAME, portName);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_RELAY_COMPONENT_ID,
            "DHCP client message received on port %s VLAN %u has hops set to %u. "
            "Value exceeds maximum %u. Dropping.",
            portName, vlanId, dhcpPacket->hops, pDhcpRelayCfgData->maxHopCount);

    ihInfo->ihStats.publicStats.tooManyHops++;
    return L7_FAILURE;
  }

  dhcpPacket->hops++;

  /* the secs config knob allows the relay agent to wait a configurable amount 
   * of time for a local DHCP server to respond to the client before relaying 
   * the client's message to a remove server. Usefulness is doubtful, however. */
  if (osapiNtohs(dhcpPacket->secs) < pDhcpRelayCfgData->minWaitTime)
  {
    L7_uchar8 portName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 clientMacStr[18];
    nimGetIntfName(rxPort, L7_SYSNAME, portName);
    l7utilsMacAddrHexToString(dhcpPacket->chaddr, 18, clientMacStr);
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_RELAY_COMPONENT_ID,
            "DHCP client message received on port %s VLAN %u from client %s has "
            "seconds field set to %u. Value is less than the minimum %u. Dropping.",
            portName, vlanId, clientMacStr, dhcpPacket->secs, 
            pDhcpRelayCfgData->minWaitTime);

    ihInfo->ihStats.publicStats.tooEarly++;
    return L7_FAILURE;
  }

  /* Get local address on ingress interface to be used as giaddr. */
  if (ipMapRtrIntfIpAddressGet(intIfNum, &intfIpAddr, &mask) != L7_SUCCESS)
  {
    ihInfo->ihStats.noLocalIpAddress++;
    return L7_FAILURE;
  }

  /* If packet does not already have a giaddr, insert ours. If giaddr already set, 
   * leave it, unless it's one of our own addresses. */
  if ((dhcpPacket->giaddr) == 0)
  {
    dhcpPacket->giaddr = osapiHtonl(intfIpAddr);
    addedGiAddr = L7_TRUE;
  }
  else
  {
    /* check for a spoofed giaddr */
    if (ipMapIpAddressToIntf(osapiNtohl(dhcpPacket->giaddr), &giAddrIntIfNum) == L7_SUCCESS)
    {
      L7_uchar8 portName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 clientMacStr[18];
      L7_uchar8 giaddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(osapiNtohl(dhcpPacket->giaddr), giaddrStr);
      nimGetIntfName(rxPort, L7_SYSNAME, portName);
      l7utilsMacAddrHexToString(dhcpPacket->chaddr, 18, clientMacStr);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_RELAY_COMPONENT_ID,
              "DHCP client message received on port %s VLAN %u from client %s has "
              "giaddr set to local address %s. Dropping.",
              portName, vlanId, clientMacStr, giaddrStr);
      /* drop the packet */
      ihInfo->ihStats.publicStats.spoofedGiaddr++;
      return L7_FAILURE;
    }
  }

  /* Now check if option 82 circuit ID suboption is needed */
  if (addedGiAddr && 
      (pDhcpRelayCfgData->circuitIDOptionMode == L7_ENABLE))
  {
    if (ihAddDhcpRelayOptions(dhcpPacket, dhcpPacketLength, intIfNum, 
                              rxPort, vlanId) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Relay a UDP packet. Could be a DHCP packet.
*
* @param    ipPkt     @b{(input)}   Start of IPv4 packet header
* @param    intIfNum  @b{(input)}   ingress routing interface
* @param    rxPort    @b{(input)}   ingress physical port
* @param    vlanId    @b{(input)}   ingress VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    on own thread. Does not verify checksums in IP header 
*           or in UDP header. Assuming errors will be caught at final 
*           destination.
*
*           For non-DCHP packets, we have to retain the client's IP address
*           as the source IP address. 
*
* @end
*********************************************************************/
L7_RC_t relayUdpPacket(L7_ipHeader_t *ipPkt, L7_uint32 intIfNum, 
                       L7_uint32 rxPort, L7_uint32 vlanId)
{
    L7_uint32 rc;
    L7_uint32 localIfNum;
    L7_IP_ADDR_t serverAddr;
    L7_dhcp_packet *dhcpPacket;
    L7_uint32 payloadLength;
    L7_uint32 ipHdrLen;                  /* in 4-byte words */
    L7_udp_header_t *udpHeader;
    L7_uint32 ttl = ipPkt->iph_ttl - 1;
    L7_uchar8 *payload;
    L7_uint32 localIntf;

    ipHdrLen = 0x0F & ipPkt->iph_versLen;
    udpHeader = (L7_udp_header_t*) (((L7_uint32*) ipPkt) + ipHdrLen);
    payloadLength = ipPkt->iph_len - (4 * ipHdrLen + sizeof(L7_udp_header_t));

    if (osapiNtohs(udpHeader->destPort) == UDP_PORT_BOOTP_SERV)
    {
      dhcpPacket = (L7_dhcp_packet*) (udpHeader + 1);
      if (dhcpPacket->op == BOOTPREQUEST)
      {
        /* check and update hops, secs, giaddr, and relay agent options */
        if (ihModifyDhcpClientMessage(dhcpPacket, &payloadLength, intIfNum, 
                                      rxPort, vlanId) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }

        return ihRelayDhcpClientMessage(intIfNum, ttl, dhcpPacket, payloadLength); 
      }
      else if (dhcpPacket->op == BOOTPREPLY)
      {
        /* Verify that destination IP address is a local IP address */
        if (ipMapIpAddressToIntf(osapiNtohl(ipPkt->iph_dst), &localIfNum) != L7_SUCCESS)
        {
          ihInfo->ihStats.dhcpServerMsgToUnknownAddr++;
          return L7_FAILURE;
        }
        /* relay DHCP server message to client */
        return ihRelayDhcpServerMessage(dhcpPacket, payloadLength);
      }
      else
      {
        /* Unknown DHCP packet type */
        L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 portName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(rxPort, L7_SYSNAME, portName);
        osapiInetNtoa(osapiNtohl(ipPkt->iph_src), srcAddrStr);
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_RELAY_COMPONENT_ID,
                "Invalid DHCP packet type %u in packet from %s on port %s VLAN %u.", 
                dhcpPacket->op, srcAddrStr, portName, vlanId);
        return L7_FAILURE;
      }
    }

    /* Packet is UDP (not DHCP) broadcast packet from client. Relay to servers. 
     * Have to bypass the IP stack in order to retain the client's address as
     * the source IP address. Modify the destination IP address and re-insert 
     * in the software forwarding path. */
    payload = ((L7_uchar8*) udpHeader) + sizeof(L7_udp_header_t);
    serverAddr = ihServerAddrGetNext(intIfNum, osapiNtohs(udpHeader->destPort), 
                                     0, L7_TRUE);
    if (serverAddr == 0)
    {
        ihInfo->ihStats.noServer++;
        return L7_FAILURE;
    }

    while (serverAddr)
    {
      /* Don't send if serverAddr is local or is on the interface where the 
       * client message came in. */
     if ((ipMapIpAddressToIntf(serverAddr, &localIntf) != L7_SUCCESS) &&
         !ipMapIpAddrIsLocal(intIfNum, serverAddr))
     {
        if (ihInfo->ihTraceFlags & IH_TRACE_PACKET)
        {
          L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          L7_uchar8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          osapiInetNtoa(osapiNtohl(ipPkt->iph_src), srcAddrStr);
          osapiInetNtoa(serverAddr, destAddrStr);
          osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX, 
                        "IP Helper relaying %u byte message from interface %s from %s:%u to %s:%u.",
                        payloadLength, ifName, srcAddrStr, osapiNtohs(udpHeader->sourcePort),
                        destAddrStr, osapiNtohs(udpHeader->destPort));
          ihTraceWrite(traceBuf);
        }
  
        rc = ihUdpMessageForward(ipPkt, serverAddr, intIfNum, rxPort, vlanId);
        if (rc != L7_SUCCESS)
        {
          L7_uchar8 serverAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
          ihInfo->ihStats.socketSendFailed++;
          osapiInetNtoa(serverAddr, serverAddrStr);
          L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
                  "Failed to send packet to server at %s for UDP destination port %u.", 
                  serverAddrStr, osapiNtohs(udpHeader->destPort));
        }
        else
        {
          ihInfo->ihStats.publicStats.udpClientMsgsRelayed++;
        }
      }
      serverAddr = ihServerAddrGetNext(intIfNum, osapiNtohs(udpHeader->destPort), 
                                       serverAddr, L7_TRUE);
    }
    
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Transfer a UDP packet to the IP MAP forwarding thread
*
* @param    ipPkt     @b{(input)}   Start of IPv4 packet header
* @param    destAddr  @b{(input)}   Destination IPv4 address
* @param    intIfNum  @b{(input)}   ingress routing interface
* @param    rxPort    @b{(input)}   ingress physical port
* @param    vlanId    @b{(input)}   ingress VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    We are really forwarding this packet. Source IP address
*           stays the same. Destination IP address becomes the helper
*           address. So put the packet on the software forwarding path,
*           which already handles routing table lookup, ARP resolution
*           for next hop, MTU checking, directed broadcast forwarding, 
*           etc, etc.
*
* @end
*********************************************************************/
L7_RC_t ihUdpMessageForward(L7_ipHeader_t *ipPkt, L7_uint32 destAddr,
                            L7_uint32 intIfNum, L7_uint32 rxPort, L7_uint32 vlanId)
{
  L7_netBufHandle bufHandle;
  L7_ushort16 ethertype;
  L7_uchar8 *dataStart;
  L7_uint32 dataLength;   /* length of ethernet frame */
  L7_uchar8 *pos;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uchar8 *origPayload;        /* UDP payload in original packet */
  L7_uchar8 *payload;            /* UDP payload in new mbuf */
  L7_uint32 payloadLength;
  L7_uint32 ipHeaderLen = 0x0F & ipPkt->iph_versLen;   /* 4 byte words */
  L7_udp_header_t *udpHeader;
  L7_ipHeader_t *newIpHeader;
  /* Always building a 14 byte MAC header here */
  L7_uint32 ethHeaderLen = 2 * L7_MAC_ADDR_LEN + sizeof(L7_ushort16);
  L7_ushort16 srcPort, destPort;   /* host byte order */
  L7_uint32 alignmentShift;

  /* IP MAP requires packet be in an mbuf. So allocate a new one. Not optimal, but 
   * better than rewriting all the sw forwarding code. */
  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    /* Don't log this. mbuf alloc failures happen occasionally. */
    ihInfo->ihStats.mbufFailures++;
    return L7_FAILURE;
  }
  SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_UDP_RELAY_TX);
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);

  /* Be picky about alignment. We know we are going to use a 14-byte
   * ethernet header. Assuming dataStart is 4-byte aligned, this will
   * make the IP header not 4-byte aligned. So move dataStart by 2 bytes. */
  alignmentShift = ((L7_uint32)dataStart) % 4; 
  dataStart += alignmentShift;
  SYSAPI_NET_MBUF_SET_DATASTART(bufHandle, dataStart);

  /* Work from the inside out. Copy payload and build UDP header. */
  payloadLength = osapiNtohs(ipPkt->iph_len) - (4 * ipHeaderLen + sizeof(L7_udp_header_t));
  udpHeader = (L7_udp_header_t*) (((L7_uint32*) ipPkt) + ipHeaderLen);
  origPayload = (L7_uchar8*) (udpHeader + 1);
  payload = dataStart + ethHeaderLen + 4 * ipHeaderLen + sizeof(L7_udp_header_t);
  memcpy(payload, origPayload, payloadLength);
  
  /* Append a new UDP header with checksum modified for new dest IP address */
  srcPort = osapiNtohs(udpHeader->sourcePort);
  destPort = osapiNtohs(udpHeader->destPort);
  if (ihUdpHeaderBuild(osapiNtohl(ipPkt->iph_src), destAddr, srcPort, destPort,
                       payload, payloadLength) != L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  /* Copy old IP header (including options if any). */
  memcpy(dataStart + ethHeaderLen, (L7_uchar8*) ipPkt, 4 * ipHeaderLen);
  newIpHeader = (L7_ipHeader_t*) (dataStart + ethHeaderLen);   /* should be 4-byte aligned */

  /* And update destination */
  newIpHeader->iph_dst = osapiHtonl(destAddr); 
  newIpHeader->iph_csum = 0;
  newIpHeader->iph_csum= dhcpCheckSum((L7_ushort16*) newIpHeader, 4 * ipHeaderLen, 0);

  /* We have to kind of fudge the ethernet header. It only matters as the packet
   * runs the gauntlet through the IP MAP receive code. It is obviously replaced
   * before tx. Say the destination MAC address is us, as if packet arrived on a 
   * routing interface for forwarding. Set source MAC address to the same since 
   * we have lost the client's MAC at this point. It's value probably doesn't matter. */
  if (nimGetIntfAddress(intIfNum, L7_SYSMAC_BIA, mac) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];   
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_RELAY_COMPONENT_ID,
            "Failed to get MAC address for interface %s.", ifName);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }

  pos = dataStart;
  memcpy(dataStart, mac, L7_MAC_ADDR_LEN);
  pos += L7_MAC_ADDR_LEN;
  memcpy(pos, mac, L7_MAC_ADDR_LEN);
  pos += L7_MAC_ADDR_LEN;
  ethertype = osapiHtons(L7_ETYPE_IP);
  memcpy(pos, &ethertype, 2);


  /* update mbuf */
  dataLength = osapiNtohl(ipPkt->iph_len) + ethHeaderLen;
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, dataLength);

  if (ipMapPacketForward(bufHandle, rxPort, intIfNum, vlanId) != L7_SUCCESS)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Is packet a valid DHCP Relay destination?
*
* @param    destAddr      destination IP address
*
* @returns  L7_TRUE       If a valid destination
* @returns  L7_FALSE      Otherwise
*
* @notes    Does not apply to protocols other than DHCP.
*
* @end
*********************************************************************/
L7_BOOL bootpDhcpRelayIsValidDest(L7_uint32 destAddr)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  rc = ipMapIpAddressToIntf(destAddr, &intIfNum);
  /* RFC 1542 section 4.1 says mcast packets can be relayed. So we allow that
   * here although I don't know how common this is. */
  if ((rc == L7_SUCCESS) ||         /* to one of our IP addresses */
      (destAddr == L7_IP_LTD_BCAST_ADDR) ||     /* broadcast */
      ((destAddr & L7_CLASS_D_ADDR_NETWORK) == L7_CLASS_D_ADDR_NETWORK))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}


/*********************************************************************
* @purpose  Sysnet intercept function for UDP relay. Decide whether to 
*           relay each packet.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_COPIED   we are relaying, but others can handle, too
* @returns  SYSNET_PDU_RC_IGNORED  frame has been ignored; continue processing it
*
* @notes    By the time this packet is intercepted, IP MAP has set the 
*           receive interface (pduInfo->intIfNum) to the routing interface
*           rather than the physical interface. This intercept point only sees
*           packets received on routing interfaces. 
*
*           Relay doesn't consume packets. It allows other local apps to see
*           every packet, even those that are forwarded. 
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t ihUdpRelayIntercept(L7_uint32 hookId,
                                    L7_netBufHandle bufHandle,
                                    sysnet_pdu_info_t *pduInfo,
                                    L7_FUNCPTR_t continueFunc)
{
  L7_enetHeader_t *ethHeader;
  L7_ipHeader_t *ipHeader;
  L7_udp_header_t *udpHeader;
  L7_uint32 rc = L7_SUCCESS;
  L7_uint32 frameLen, ipLen, ethHeaderSize;
  L7_uchar8 *data;
  L7_uint32 ipHeaderLen;    /* in 4 byte words */
  L7_uint32 ifState;

  /* Even though we are not holding the mbuf, update its location */
  SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_DHCP_RELAY_RX);

  if (osapiSemaTake(ihInfo->ihLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;


  /* if UDP relay disabled globally, ignore packet */
  if (pDhcpRelayCfgData->adminMode != L7_ENABLE)
  {
    osapiSemaGive(ihInfo->ihLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Should only be intercepting pkts received on routing interfaces, but
   * best to make sure. */
  if ((ipMapRtrIntfOperModeGet(pduInfo->intIfNum, &ifState) != L7_SUCCESS) || 
      (ifState != L7_ENABLE))
  {
    osapiSemaGive(ihInfo->ihLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  ihInfo->ihStats.packetsIntercepted++;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, frameLen);
  ethHeader = (L7_enetHeader_t*) data;
  ethHeaderSize = sysNetDataOffsetGet(data);
  ipHeader = (L7_ipHeader_t *)(data + ethHeaderSize);
  ipLen = frameLen - ethHeaderSize;
  if ((ipHeader->iph_versLen != ((L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN) ) ||
      (ipHeader->iph_prot != IP_PROT_UDP))
  {
    osapiSemaGive(ihInfo->ihLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* UDP/IPv4 */
  ipHeaderLen = 0x0F & ipHeader->iph_versLen;
  udpHeader = (L7_udp_header_t *)((L7_uint32 *)ipHeader + ipHeaderLen);

  if (osapiNtohs(udpHeader->sourcePort) != UDP_PORT_BOOTP_SERV)
  {
    /* Message from client. If destination MAC address is not FF:FF:FF:FF:FF:FF, ignore packet */
    if (memcmp(ethHeader->dest.addr, L7_ENET_BCAST_MAC_ADDR.addr, L7_MAC_ADDR_LEN))
    {
      osapiSemaGive(ihInfo->ihLock);
      return SYSNET_PDU_RC_IGNORED;
    }
  }

  if (((osapiNtohl(ipHeader->iph_src) & L7_CLASS_D_ADDR_NETWORK) == L7_CLASS_D_ADDR_NETWORK) ||
      ((osapiNtohl(ipHeader->iph_src) & L7_CLASS_E_ADDR_NETWORK) == L7_CLASS_E_ADDR_NETWORK))
  {
    /* Illegal src address. Ignore. */
    osapiSemaGive(ihInfo->ihLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Note that server unicasts reply msg to relay agent with source and dest
   * ports set to 67. */
  if (osapiNtohs(udpHeader->destPort) == UDP_PORT_BOOTP_SERV)
  {
    /* for DHCP, accept packets whose dest IP address is bcast, mcast (why?), 
     * or unicast to me. */
    if (!bootpDhcpRelayIsValidDest(osapiNtohl(ipHeader->iph_dst)))
    {
      ihInfo->ihStats.notToMe++;
      osapiSemaGive(ihInfo->ihLock);
      return SYSNET_PDU_RC_IGNORED;
    }
    if (osapiNtohs(udpHeader->sourcePort) == UDP_PORT_BOOTP_CLNT)
      ihInfo->ihStats.publicStats.dhcpClientMsgsReceived++;
    else if (osapiNtohs(udpHeader->sourcePort) == UDP_PORT_BOOTP_SERV)
      ihInfo->ihStats.publicStats.dhcpServerMsgsReceived++;
  }
  else
  {
    /* for other protocols, only relay packets whose dest IP address is 255.255.255.255
     * or a network directed broadcast address on the ingress interface. */
    L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
    if ((osapiNtohl(ipHeader->iph_dst) != L7_IP_LTD_BCAST_ADDR) &&
        (ipMapNetDirBcastMatchCheck(pduInfo->intIfNum, osapiNtohl(ipHeader->iph_dst), 
                                    macAddr) != L7_SUCCESS))
    {
      ihInfo->ihStats.notToMe++;
      osapiSemaGive(ihInfo->ihLock);
      return SYSNET_PDU_RC_IGNORED;
    }
  }

  if (osapiNtohs(udpHeader->sourcePort) != UDP_PORT_BOOTP_SERV)
    ihInfo->ihStats.publicStats.udpClientMsgsReceived++;

  /* Packets from a DHCP server back to the client have src and dest port = 67. Relay these. */
  if (((osapiNtohs(udpHeader->destPort) != UDP_PORT_BOOTP_SERV) || 
       (osapiNtohs(udpHeader->sourcePort) != UDP_PORT_BOOTP_SERV)) &&
      (ihServerAddrGetNext(pduInfo->intIfNum, osapiNtohs(udpHeader->destPort), 
                           0, L7_FALSE) == 0))
  {
    /* No IP helper address configured for packets on this interface to this UDP port */
    ihInfo->ihStats.noServer++;
    osapiSemaGive(ihInfo->ihLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  if (ipHeader->iph_ttl <= 1)
  {
    /* TTL exceeded. Don't relay. But let it be delivered locally. Check
     * this after other checks so we don't count TTLs expired for packets
     * we would not relay anyway. */
    ihInfo->ihStats.publicStats.ttlExpired++;
    osapiSemaGive(ihInfo->ihLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  if (ihInfo->ihTraceFlags & IH_TRACE_PACKET)
  {
    L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 srcAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    nimGetIntfName(pduInfo->intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(osapiNtohl(ipHeader->iph_src), srcAddrStr);
    osapiInetNtoa(osapiNtohl(ipHeader->iph_dst), destAddrStr);
    osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX, 
                  "IP Helper received %u byte packet received on interface %s from %s:%u to %s:%u.",
                  ipLen, ifName, srcAddrStr, osapiNtohs(udpHeader->sourcePort),
                  destAddrStr, osapiNtohs(udpHeader->destPort));
    ihTraceWrite(traceBuf);
  }

  /* Should relay this packet */
  rc = ihUdpPacketQueue(ipHeader, ipLen, pduInfo->intIfNum, pduInfo->rxPort, pduInfo->vlanId);

  /* If packet from DHCP server, consume it. Otherwise it goes to IP stack, who 
   * finds no sockets bound to this UDP port and sends a dest unreachable 
   * (port unreachable) message. If DHCP snooping is active, it should have 
   * already snooped this packet. */
  if (osapiNtohs(udpHeader->sourcePort) == UDP_PORT_BOOTP_SERV)
  {
    osapiSemaGive(ihInfo->ihLock);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return SYSNET_PDU_RC_CONSUMED;
  }

  /* let others have a crack at this packet */
  osapiSemaGive(ihInfo->ihLock);
  return SYSNET_PDU_RC_COPIED;
}

/*********************************************************************
* @purpose  Queue a DHCP packet for processing
*
* @param    ipHeader     @b{(input)} IP packet
* @param    ipLen        @b{(input)} IP packet len
* @param    intIfNum     @b{(input)} Interface num. through which packet arrived
* @param    rxPort       @b{(input)} Ingress physical port
* @param    vlanId       @b{(input)} Ingress VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihUdpPacketQueue(L7_ipHeader_t *ipHeader, L7_uint32 ipLen, 
                         L7_uint32 intIfNum, L7_uint32 rxPort, L7_uint32 vlanId)
{
  udpRelayMsg_t ihMessage;
  L7_ipHeader_t *pktBuffer;          /* should be 4-byte aligned */

  /* Allocate a buffer to hold the packet. Don't want to hold the mbuf on the msg queue. */
  if (bufferPoolAllocate(ihInfo->packetPoolId, (L7_uchar8**) &pktBuffer) != L7_SUCCESS)
  {
    /* Not catastrophic. Sender should retx. */
    ihInfo->ihStats.noPacketBuffer++;
    return L7_FAILURE;
  }
  memcpy(pktBuffer, ipHeader, ipLen);

  ihMessage.msgId = DHCPRELAYMAP_RELAY_PKT; 
  ihMessage.type.relayPacket.intIfNum = intIfNum;
  ihMessage.type.relayPacket.rxPort = rxPort;
  ihMessage.type.relayPacket.vlanId = vlanId;
  ihMessage.type.relayPacket.pktBuffer = pktBuffer;

  if (osapiMessageSend(dhcpRelayQueue, &ihMessage, sizeof(udpRelayMsg_t), 
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    bufferPoolFree(ihInfo->packetPoolId, (L7_uchar8*) pktBuffer);
    ihInfo->ihStats.pktEnqueueFailed++;
    return L7_FAILURE;
  }

  ihInfo->ihStats.packetsQueued++;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Adds DHCP relay agent options when sending to server
*
*
* @param    pDhcpPacket      @b{(input)} Pointer to Dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
* @param    intIfNum         @b{(input)} ingress interface
* @param    rxPort           @b{(input)} ingress physical port
* @param    vlanId           @b{(input)} ingress VLAN ID
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihAddDhcpRelayOptions(L7_dhcp_packet* pDhcpPacket, 
                              L7_uint32 *dhcpPacketLength, 
                              L7_uint32 intIfNum, L7_uint32 rxPort,
                              L7_uint32 vlanId)
{
    L7_BOOL   stopProcessing = L7_FALSE;
    L7_uchar8 *dhcpOption, *pktEnd, *end_pad = 0;
    L7_uchar8 *pos;
    nimUSP_t  nimUsp;
    L7_ushort16 vidNbo;    /* VLAN ID in network byte order */

    memset(&nimUsp, L7_NULL, sizeof(nimUSP_t));

    if (memcmp(pDhcpPacket->options, DHCP_OPTIONS_COOKIE, DHCP_OPTIONS_COOKIE_LEN))
      /* BOOTP packet */
      return L7_SUCCESS;

    /* DHCP packet */
    pktEnd = (L7_uchar8*)(pDhcpPacket) + *dhcpPacketLength; /* end of DHCP packet */
    dhcpOption = &pDhcpPacket->options[DHCP_OPTIONS_COOKIE_LEN];

    while ((dhcpOption < pktEnd) && !stopProcessing)
    {
        switch (*dhcpOption)
        {
        /* Skip zeroes padded by client ... */
        case DHO_PAD:
            end_pad = dhcpOption;
            ++dhcpOption;
            break;

        case DHO_END:
            stopProcessing = L7_TRUE; /* while loop breaks */
            break;

        case DHO_DHCP_AGENT_OPTIONS:
            /* There's already a Relay Agent Information option
            in this packet. So we are not going to modify it */
            return L7_SUCCESS;

        default:
            end_pad = 0;
            dhcpOption += dhcpOption [DHCP_OPTION_LEN_FIELD_INDEX] + DHCP_OPTION_SUBOPTION_OFFSET;
            break;
        }
    }

    /* If the packet is padded, store the agent option
    at the beginning of the padding. */
    if (end_pad)
        dhcpOption = end_pad;

    /* setting Relay Agent Information option. */
    pos = dhcpOption;
    *pos++ = DHO_DHCP_AGENT_OPTIONS;
    *pos++ = 0;

    /* Copy  the Circuit Id Sub option specifier 1... */
    *pos++ = RAI_CIRCUIT_ID;
    *pos++ = DHCP_SUB_OPTION_TYPE_1_LENGTH;
    *pos++ = DHCP_CIRCUIT_ID_TYPE;
    *pos++ = DHCP_CIRCUIT_ID_LENGTH;
    vidNbo = osapiHtons(vlanId);
    memcpy(pos, &vidNbo, sizeof(L7_ushort16));   
    pos += sizeof(L7_ushort16);

    if (nimGetUnitSlotPort(rxPort, &nimUsp) == L7_SUCCESS)
    {
      *pos++ = nimUsp.unit;
      *pos++ = nimUsp.slot;
      *pos++ = (L7_uchar8)nimUsp.port;
    }
    else
    {
      *pos++ = 0;
      *pos++ = 0;
      *pos++ = 0;
    }
    *dhcpPacketLength += DHO_CIRCUIT_ID_OPTION_LEN;

    /* set length of Relay Agent option */
    dhcpOption[DHCP_OPTION_LEN_FIELD_INDEX] = DHO_CIRCUIT_ID_OPTION_LEN - DHCP_OPTION_SUBOPTION_OFFSET;    

    /* Deposit an END token. */
    *pos++ = DHO_END;
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the client's interface from the DHCP relay agent option
*           circuit ID suboption and strip option 82 from the packet.
*
* @param    packet           @b{(input)} Pointer to DHCP Packet
* @param    dhcpPacketLength @b{(in/out)} length of the DHCP Packet
* @param    intIfNum         @b{(output)} interface number
*
* @returns  L7_SUCCESS  found cid suboption and set intIfNum
* @returns  L7_FAILURE  cid suboption of option 82 not found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpRelayOption82Strip(L7_dhcp_packet *packet, 
                               L7_uint32 *dhcpPacketLength,
                               L7_uint32 *intIfNum)
{
  L7_uchar8 *dhcpOption, *pktEnd;
  L7_uchar8 *nextOption;            /* start of option following option 82 */
  L7_BOOL atEnd = L7_FALSE;
  L7_BOOL option82Found = L7_FALSE;
  nimUSP_t nimUsp;

  /* Maybe this is overkill, but I'd like to log servers that don't reflect 
   * option 82. But I don't want to flood the log with these. So keep track 
   * of servers that don't reflect and only log the first time we notice. */
  /* max number of naughty servers we remember */
#define IH_MAX_NS 8   
  static L7_uint32 failures = 0;    /* number of times we failed to find option 82 */
  static L7_uint32 naughtyServers[IH_MAX_NS];

  /* Intializing nimUSP. */
  memset(&nimUsp, L7_NULL, sizeof(nimUSP_t));

  /* If there's no cookie, it's a bootp packet, so we should just
     forward it unchanged. */
  if (memcmp (packet->options, DHCP_OPTIONS_COOKIE, DHCP_OPTIONS_COOKIE_LEN))
  {
    /* Curious if BOOTP is still out there.... */
    ihInfo->ihStats.bootpMsgToClient++;
    return L7_SUCCESS;
  }

  pktEnd = (L7_uchar8*)(packet) + *dhcpPacketLength;

  /* start after the magic cookie, which is always the first option */
  dhcpOption = &packet->options[DHCP_OPTIONS_COOKIE_LEN];

  /* walk options until we find option 82. */
  while ((dhcpOption < pktEnd) && !atEnd && !option82Found)
  {
    switch (*dhcpOption)
    {
      /* one byte option. */
      case DHO_PAD:
        ++dhcpOption;
        break;

      case DHO_END:
        /* no more options follow */
        atEnd = L7_TRUE; /* while loop breaks */
        break;

      case DHO_DHCP_AGENT_OPTIONS:    /* option 82 */
        /* Get the u/s/p values from sub option format */
        nimUsp.port = *(dhcpOption + DHCP_PORT_SUBOPTION_VALUE_OFFSET);
        nimUsp.slot = *(dhcpOption + DHCP_SLOT_SUBOPTION_VALUE_OFFSET);
        nimUsp.unit = *(dhcpOption + DHCP_UNIT_SUBOPTION_VALUE_OFFSET);

        option82Found = L7_TRUE;     /* no need to look any further */
        break;

      default:
        dhcpOption += dhcpOption[DHCP_OPTION_LEN_FIELD_INDEX] + DHCP_OPTION_SUBOPTION_OFFSET;
        break;
    }
  }

  if (!option82Found)
  {
    L7_BOOL alreadyReported = L7_FALSE;
    L7_uint32 i;

    failures++;
    for (i = 0; i < IH_MAX_NS; i++)
    {
      if ((failures % 1000) == 0)
      {
        /* Every 1000 failures, clear the naughty servers list */
        naughtyServers[i] = 0;
      }
      else if (naughtyServers[i] == osapiNtohl(packet->siaddr))
      {
        alreadyReported = L7_TRUE;
        break;
      }
      else if (naughtyServers[i] == 0)
      {
        naughtyServers[i] = osapiNtohl(packet->siaddr);
      }
    }
    if (!alreadyReported)
    {
      /* did not find circuit ID suboption of option 82. */
      L7_uchar8 serverAddr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(osapiNtohl(packet->siaddr), serverAddr);
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_RELAY_COMPONENT_ID,
              "DHCP relay configured to insert option 82, but option not found "
              "in message returned from server %s.", serverAddr);
    }
    return L7_FAILURE;
  }

  if (nimGetIntIfNumFromUSP(&nimUsp, intIfNum) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
            "Circuit ID option contains unknown interface %u/%u/%u",
            nimUsp.unit, nimUsp.slot, nimUsp.port);
    return L7_FAILURE;
  }

  /* Remove option 82. dhcpOption was left pointing to the beginning of option 82. */
  nextOption = dhcpOption + DHO_CIRCUIT_ID_OPTION_LEN;
  memmove(dhcpOption, nextOption, pktEnd - nextOption);
  *dhcpPacketLength -= DHO_CIRCUIT_ID_OPTION_LEN;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add IP and UDP headers and send to 255.255.255.255 and 
*           broadcast Ethernet address on the specified interface.
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    clientPort       @b{(input)} physical port where client is attached
* @param    vlanId           @b{(input)} If client on a VLAN routing interface, 
*                                        the interface's VLAN ID. 0 otherwise. 
* @param    dhcpPacket       @b{(input)} Pointer to the DHCP packet
* @param    dhcpPacketLength @b{(input)} length of the DHCP packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only used when sending DHCP server message back to client.
*
*           The start of dhcpPacket is at least 28 bytes from the start
*           of the buffer from the buffer pool; therefore, we can add 
*           new IP and UDP headers in place w/o copying to a temp buffer.
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketBroadcast(L7_uint32 intIfNum, L7_uint32 clientPort,
                            L7_uint32 vlanId,
                            L7_dhcp_packet *dhcpPacket,
                            L7_uint32 dhcpPacketLength)
{
  L7_ushort16 etype;
  L7_ushort16 flag = 0;
  L7_netBufHandle pBufHandle;
  L7_uint32 ipPktLen = L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + dhcpPacketLength;
  L7_uchar8 *ip_packet;
  DTL_CMD_TX_INFO_t dtlCmd;
  L7_uint32 outIntf;

  /* If we know the physical port the client is on (from option 82), 
   * send the response on that port. Otherwise, send on the routing 
   * interface. */
  if (clientPort)
    outIntf = clientPort;
  else
    outIntf = intIfNum;

  /* Add UDP and IP headers */
  if (dhcpPacketBuild(intIfNum, dhcpPacket, dhcpPacketLength) == L7_FAILURE)
  {
    return L7_FAILURE;
  }

  /* Add ethernet header */
  ip_packet = ((L7_uchar8*) dhcpPacket) - (L7_IP_HDR_LEN + sizeof(L7_udp_header_t));
  flag |= L7_LL_ENCAP_ENET;
  etype = L7_ETYPE_IP;
  if (sysNetPduHdrEncapsulate(L7_ENET_BCAST_MAC_ADDR.addr, outIntf, &flag,
                              ipPktLen, 0, etype, &pBufHandle, ip_packet) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (ihInfo->ihTraceFlags & IH_TRACE_PACKET)
  {
    L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(outIntf, L7_SYSNAME, ifName);
    osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX, 
                  "IP Helper broadcasting %u byte DHCP server message to client on interface %s vlan %u.",
                  dhcpPacketLength, ifName, vlanId);
    ihTraceWrite(traceBuf);
  }

  dtlCmd.intfNum = outIntf;
  dtlCmd.priority = 0;
  dtlCmd.typeToSend = DTL_NORMAL_UNICAST;
  dtlCmd.cmdType.L2.domainId = vlanId;

  /* Limit broadcast to client's port */
  return dtlPduTransmit(pBufHandle, DTL_CMD_TX_L2, &dtlCmd);
}

/*********************************************************************
* @purpose  Add UDP and IP headers to a DHCP server message and unicast
*           it on a given interface.
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    clientPort       @b{(input)} physical port where client is attached
* @param    vlanId           @b{(input)} If client on a VLAN routing interface, 
*                                        the interface's VLAN ID. 0 otherwise. 
* @param    dhcpPacket       @b{(input)} Pointer to the dhcp Packet
* @param    dhcpPacketLength @b{(input)} length of the Dhcp Packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketUnicast(L7_uint32 intIfNum, L7_uint32 clientPort, 
                          L7_uint32 vlanId, L7_dhcp_packet *dhcpPacket, 
                          L7_uint32 dhcpPacketLength)
{
  L7_ushort16 etype;
  L7_ushort16 flag = 0;
  L7_netBufHandle pBufHandle;
  L7_uchar8 destMacAddr[L7_MAC_ADDR_LEN];
  L7_uint32 ipPktLen = L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + dhcpPacketLength;
  L7_uchar8 *ip_packet;
  DTL_CMD_TX_INFO_t dtlCmd;
  L7_uint32 outIntf;

  if (ipPktLen > DHCP_BUFFER_LEN)
  {
    ihInfo->ihStats.msgTooBig++;
    return L7_FAILURE;
  }

  /* If we know the physical port the client is on (from option 82), 
   * send the response on that port. Otherwise, send on the routing 
   * interface. */
  if (clientPort)
    outIntf = clientPort;
  else
    outIntf = intIfNum;

  if (dhcpPacketBuild(intIfNum, dhcpPacket, dhcpPacketLength) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ip_packet = ((L7_uchar8*) dhcpPacket) - (L7_IP_HDR_LEN + sizeof(L7_udp_header_t));

  /* Fill in the dest MAC with the unicast MAC of the client */
  memcpy(destMacAddr, dhcpPacket->chaddr, L7_MAC_ADDR_LEN);

  flag |= L7_LL_ENCAP_ENET;
  etype = L7_ETYPE_IP;
  if (sysNetPduHdrEncapsulate(destMacAddr, outIntf, &flag,
                              ipPktLen, 0, etype, &pBufHandle, 
                              ip_packet) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (ihInfo->ihTraceFlags & IH_TRACE_PACKET)
  {
    L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 clientMacStr[18];
    l7utilsMacAddrHexToString(destMacAddr, 18, clientMacStr);
    nimGetIntfName(outIntf, L7_SYSNAME, ifName);
    osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX, 
                  "IP Helper unicasting %u byte DHCP server message to client %s on interface %s vlan %u.",
                  dhcpPacketLength, clientMacStr, ifName, vlanId);
    ihTraceWrite(traceBuf);
  }

  dtlCmd.intfNum = outIntf;
  dtlCmd.priority = 0;
  dtlCmd.typeToSend = DTL_NORMAL_UNICAST;
  dtlCmd.cmdType.L2.domainId = vlanId;

  /* Limit broadcast to client's port */
  return dtlPduTransmit(pBufHandle, DTL_CMD_TX_L2, &dtlCmd);
}

/*********************************************************************
* @purpose  Builds dhcp packet by adding UDP and IP headers
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    dhcpPacket       @b{(input)} Pointer to the DHCP packet
* @param    dhcpPacketLength @b{(input)} length of the DHCP packet
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The start of dhcpPacket is at least 28 bytes from the start
*           of the buffer from the buffer pool; therefore, we can add 
*           new IP and UDP headers in place w/o copying to a temp buffer.
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketBuild(L7_uint32 intIfNum, L7_dhcp_packet *dhcpPacket, 
                        L7_uint32 dhcpPacketLength)
{
  L7_uint32 srcAddr,destAddr;
  L7_IP_MASK_t mask;

  if (osapiNtohl(dhcpPacket->flags) & BOOTP_BROADCAST)
    destAddr = L7_IP_LTD_BCAST_ADDR;
  else
    destAddr = osapiNtohl(dhcpPacket->yiaddr);

  if (dhcpPacketIpHeaderBuild(intIfNum, (L7_uchar8*) dhcpPacket, dhcpPacketLength, destAddr) != L7_SUCCESS)
    return L7_FAILURE;

  if (ipMapRtrIntfIpAddressGet(intIfNum, &srcAddr, &mask) != L7_SUCCESS)
    return L7_FAILURE;

  ihUdpHeaderBuild(srcAddr, destAddr, UDP_PORT_BOOTP_SERV, UDP_PORT_BOOTP_CLNT, 
                   (L7_uchar8*) dhcpPacket, dhcpPacketLength);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Builds IP header
*
* @param    intIfnum         @b{(input)} Interface num. on which data to be sent
* @param    dhcpPacket       @b{(input/output)} beginning of DHCP packet
* @param    dhcpPacketLength @b{(input)} length of the DHCP packet
* @param    destAddr         @b{(input)} dest IP address (unicast or broadcast)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Used when relaying a DHCP server message to the client
*
* @end
*********************************************************************/
L7_RC_t dhcpPacketIpHeaderBuild(L7_uint32 intIfNum, L7_uchar8 *dhcpPacket,
                                L7_uint32 dhcpPacketLength, L7_uint32 destAddr)
{
    L7_uint32 srcAddr;
    L7_IP_MASK_t mask;
    static L7_uint32 iph_ident = 1;
    L7_ipHeader_t *ip = (L7_ipHeader_t *)(dhcpPacket - (L7_IP_HDR_LEN + sizeof(L7_udp_header_t)));

    /* source address is primary IP address on interface to client */
    if (ipMapRtrIntfIpAddressGet(intIfNum, &srcAddr, &mask) != L7_SUCCESS)
    {
      ihInfo->ihStats.noIpAddrOnOutIf++;
      return L7_FAILURE;
    }

    ip->iph_versLen = (L7_IP_VERSION << 4) | L7_IP_HDR_VER_LEN;
    ip->iph_tos = 0;
    ip->iph_len = osapiHtons(L7_IP_HDR_LEN + sizeof(L7_udp_header_t) + dhcpPacketLength);
    ip->iph_ident = osapiHtons(iph_ident++);
    ip->iph_flags_frag = 0;
    ip->iph_ttl = IP_BOOTP_DHCP_RELAY_TTL;
    ip->iph_prot = IP_PROT_UDP;
    ip->iph_src = osapiHtonl(srcAddr);   /* as far as client knows, relay agent is the server */
    ip->iph_dst = osapiHtonl(destAddr);  
    ip->iph_csum = 0;
    ip->iph_csum= dhcpCheckSum( (L7_ushort16*)ip, L7_IP_HDR_LEN, 0 );
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build a UDP header
*
* @param    srcAddr          @b{(input)} source IP address
* @param    destAddr         @b{(input)} dest IP address
* @param    srcPort          @b{(input)} source UDP port number
* @param    destPort         @b{(input)} destination UDP port number
* @param    payload          @b{(input)} Pointer to payload of UDP packet
* @param    payloadLength    @b{(input)} length of the payload
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ihUdpHeaderBuild(L7_uint32 srcAddr, L7_uint32 destAddr, 
                         L7_ushort16 srcPort, L7_ushort16 destPort,
                         L7_uchar8 *payload, L7_uint32 payloadLength)
{
    L7_udp_header_t *udp;
    pseudo_header_t *pseudohdr; 

    /* avoid wriing past end of buffer */
    if ((payloadLength + sizeof(L7_udp_header_t) + sizeof(pseudohdr)) > DHCP_UDP_PSEUDO_PACKET_LEN)
    {
      ihInfo->ihStats.msgTooBig++;
      return L7_FAILURE;
    }

    udp = (L7_udp_header_t *) (payload - sizeof(L7_udp_header_t));
    udp->destPort = osapiHtons(destPort);
    udp->sourcePort = osapiHtons(srcPort);
    udp->length = osapiHtons(sizeof(L7_udp_header_t) + payloadLength);
    udp->checksum = 0;

    pseudohdr = pseudo_packet;
    pseudohdr->protocol = IPPROTO_UDP;
    pseudohdr->length = osapiHtons(sizeof(L7_udp_header_t) + payloadLength);
    pseudohdr->place_holder = 0;
    pseudohdr->source_address = srcAddr;
    pseudohdr->dest_address = destAddr;

    memcpy((pseudo_packet + 1), udp, payloadLength + sizeof(L7_udp_header_t));
    udp->checksum = dhcpCheckSum((L7_ushort16*) pseudo_packet, 
                                 sizeof(pseudo_header_t) + sizeof(L7_udp_header_t) + payloadLength, 0);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Calculates checksum on a buffer
*
* @param    addr         @b{(input)} data buffer
* @param    len          @b{(input)} length of data
* @param    csum         @b{(input)} checksum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_ushort16 dhcpCheckSum( L7_ushort16 *addr, L7_ushort16 len, L7_ushort16 csum)
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
        sum += osapiHtons(*(u_char *)w << 8);
    sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */
    sum += (sum >> 16);                     /* add carry */
    answer = ~sum;                          /* truncate to 16 bits */
    return(answer);
}

/*********************************************************************
* @purpose  Determine if the interface type is valid for configuration
*           of relay entries. 
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE if valid interface type
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dhcpRelayIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
    case L7_LOGICAL_VLAN_INTF:
      return L7_TRUE;
      /*break;*/

    default:
      return L7_FALSE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for configuration of
*           relay entries. 
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE  if relay entry can be configured on this interface
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL dhcpRelayIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return dhcpRelayIsValidIntfType(sysIntfType);
  }
  return L7_FALSE;
}



/*********************************************************************
*
* @purpose  To process NIM events.
*
* @param    L7_uint32  intIfNum   internal interface number
* @param    L7_uint32  event      event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Enqueues the interface event onto the processing queue.
*           The processing task handles the event through
*           dhcpRelayIntfChangeProcess()
*
* @end
*
*********************************************************************/
L7_RC_t dhcpRelayIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc;
  NIM_EVENT_COMPLETE_INFO_t status;
  udpRelayMsg_t dhcpMessage;

  if ((event != L7_CREATE) && 
      (event != L7_DELETE)) /* No need to process any other NIM event than these two */
  {
    status.intIfNum     = intIfNum;
    status.response.rc  = L7_SUCCESS;
    status.event        = event;
    status.component    = L7_DHCP_RELAY_COMPONENT_ID;
    status.correlator   = correlator;
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  memset(&dhcpMessage, 0, sizeof(udpRelayMsg_t) );

  dhcpMessage.type.intfEvent.intIfNum = intIfNum;
  dhcpMessage.type.intfEvent.event  = event; 
  dhcpMessage.type.intfEvent.correlator = correlator; 
  dhcpMessage.msgId = DHCPRELAYMAP_INTF_EVENT;

  if ((rc = osapiMessageSend(dhcpRelayQueue, &dhcpMessage, sizeof(udpRelayMsg_t),
                              L7_NO_WAIT,
                              L7_MSG_PRIORITY_NORM )) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_RELAY_COMPONENT_ID,
            "NIM event %s enqueue failed.", nimGetIntfEvent(event));
  }

  return rc;
}

/*********************************************************************
* @purpose  Process interface-related events
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    When an interface is deleted, all the relay entries configured
*           on the interface get deleted.
*
* @end
*********************************************************************/
L7_RC_t dhcpRelayIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event,
                                 NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum     = intIfNum;
  status.component    = L7_DHCP_RELAY_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;

  if (dhcpRelayIsValidIntf(intIfNum) != L7_TRUE)
  {
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  if (ihInfo->ihTraceFlags & IH_TRACE_NIM_EVENTS)
  {
    L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX, 
                  "IP Helper processing NIM event %s for interface %s.",
                  nimGetIntfEvent(event), ifName);
    ihTraceWrite(traceBuf);
  }

  if (dhcpRelay_is_ready() == L7_TRUE)
  {
    switch (event)
    {
      case L7_CREATE:
        rc = dhcpRelayIntfCreate(intIfNum);
        break;
      case L7_DELETE:
        rc = dhcpRelayIntfDelete(intIfNum);
        break;
      default:
        rc = L7_SUCCESS; /* If its an event we dont care about, return SUCCESS */
        break;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_RELAY_COMPONENT_ID,
            "Received an interface callback while outside the EXECUTE state");
    rc = L7_FAILURE;
  }

  status.response.rc = rc;
  nimEventStatusCallback(status);
  return rc;
}








