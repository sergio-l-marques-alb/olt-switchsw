/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_main.c
*
* @purpose   Infrastructure for captive portal wired interface owner
*
* @component captive portal wired interface owner
*
* @comments  The Captive Portal feature is a software implementation that blocks
*            both wired and wireless clients from accessing the network until user
*            verification has been established. The feature is implemented as 3
*            components: 1. captive portal itself, 2. the wireless interface
*            owner, 3. the wired interface owner (WIO). The code in this directory
*            implements the wired interface owner. See the functional spec,
*            "FASTPATH Captive Portal Functional Specification." Section 7.8.2
*            of the FS describes the wired interface owner.
*
*            The wired interface owner is its own component so that it is
*            separable from the other two captive portal components. A product
*            could be built with captive portal only for wireless interfaces.
*            WIO has its own thread, so that events (e.g., CP enabled on a
*            wired interface) can be handled w/o blocking the main CP component.
*
*            WIO is useless without captive portal. Even so, it was decided to
*            wrap the new client notification in the outcalls pattern. CP can
*            exist w/o WIO. To avoid a dependency in CP on WIO, WIO conditionally
*            registers a set of function pointers (wioInfo->wioCallbacks) with CP.
*
*            WIO code is run on several threads. The CP main thread invokes some
*            WIO callbacks. The WIO thread runs most code. Incoming and outgoing
*            packets are intercepted and some are modified on the
*            thread running at the sysnet intercept point. In order to synchronize
*            these threads, WIO has a single binary semaphore, taken in the APIs,
*            packet intercept routines, and wioTask().
*
*            WIO maintains an AVL tree of wired clients. The key to the tree is
*            the client MAC address. A client is added to the tree the first time
*            WIO intercepts an incoming IPv4 packet from a client. A client is
*            removed from the tree when CP deauthenticates the client, or CP
*            becomes disabled on the client's interface.
*
*            WIO redirects incoming "web server" packets to a local authentication
*            server. A "web server" packet is a TCP packet whose destination TCP port
*            is HTTP (80), HTTPS (443), or an optional, configurable web server port.
*            The hardware traps these packets to the CPU for unauthenticated clients.
*            For each interface, WIO maintains an unordered linked list of these
*            connections in order to retain the remote IP address that the client
*            originally intended to open a connection with. The client's IP address
*            and TCP port number uniquely identify elements on this list. Because WIO
*            shares a buffer pool for connection entries on all interfaces, the total
*            number of hijacked connections at any given time may be no more than
*            WIO_MAX_INTERCEPTED_CONNECTIONS.
*
*            WIO may be included in a switch that never enables CP on wired interfaces.
*            Since the WIO packet intercept code at takes our semaphore and does some
*            minimal inspection on every packet, it is desirable to avoid
*            intercepting packets unless CP is actually enabled on one or more
*            wired interfaces. For this reason, sysnet registration is done
*            when the first wired interface is enabled for CP, and the intercepts
*            are deregistered when CP is no longer enabled on any wired interface.
*            In this context, CP is only considered "active" on a wired interface
*            if CP is enabled and the interface is not blocked.
*
*            The user can administratively block a captive portal port. The blocked
*            state of an interface is independent of whether CP is enabled on the
*            interface.
*
*            The WIO component has no persistent data. So WIO does not register with
*            nvStore. All CP configuration is maintained by CP and communicated to
*            WIO. All WIO state is maintained in a top level global structure,
*            wioInfo, and its array of interfaces, wioIntfInfo[]. The interface array
*            is indexed (for now) by internal interface number, since the only wired
*            interfaces that can be enabled for CP are physical ports, and the
*            physical port intIfNums are [1, n]. If support for other types of wired
*            interfaces is added, it may be necessary to add a mapping from intIfNum
*            to array index and update wioIntfInfoGet(). All code needing to refer to
*            interface data should go through wioIntfInfoGet() in case this indexing
*            changes.
*
*            WIO is not a NIM client. CP interprets interface events for WIO.
*
*            All IPv4 addresses stored by WIO are stored in host byte order.
*
*            For debugging, you can selectively turn on console traces using
*            "devshell wioTraceFlagsSet." See e_WioTraceFlags for a list of trace flags.
*            wio_debug.c has additional debug commands for printing state data, debug
*            stats and so forth.
*
*
* @create    2/25/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#define L7_MAC_ENET_VRRP

#include "log.h"
#include "sysnet_api.h"
#include "osapi_support.h"
#include "simapi.h"
#include "dtlapi.h"

#include "wio_util.h"
#include "wio_outcalls.h"
#include "wio_cnfgr.h"
#include "simapi.h"
#include "l7_ip_api.h"
#include "dot1q_api.h"
#include "fdb_api.h"
#include "l7_packet.h"
#include "mfdb_exports.h"

wioInfo_t *wioInfo = NULL;


/*********************************************************************
* @purpose  Main function for the captive portal wired interface owner.
*           Read incoming messages and process accordingly.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void wioTask(void)
{
  wioEventMsg_t eventMsg;
  wioFrameMsg_t frameMsg;
  L7_uint32 qLen;

  /* Loop forever, processing incoming messages */
  while (L7_TRUE)
  {
    /* Since we are reading from multiple queues, we cannot wait forever
     * on the message receive from each queue. Use a semaphore to indicate
     * whether any queue has data. Wait until data is available. */
    if (osapiSemaTake(wioInfo->msgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
             "Failure taking message queue semaphore.");
      continue;
    }

    memset(&eventMsg, 0, sizeof(wioEventMsg_t));
    memset(&frameMsg, 0, sizeof(wioFrameMsg_t));

    /* track high water mark of msg queues */
    if (osapiMsgQueueGetNumMsgs(wioInfo->wioMsgQ, &qLen) == L7_SUCCESS)
    {
      if (wioInfo->debugStats.msgQHighWater < qLen)
      {
        wioInfo->debugStats.msgQHighWater = qLen;
      }
    }

    if (osapiMsgQueueGetNumMsgs(wioInfo->wioFrameMsgQ, &qLen) == L7_SUCCESS)
    {
      if (wioInfo->debugStats.frameQHighWater < qLen)
      {
        wioInfo->debugStats.frameQHighWater = qLen;
      }
    }

    /* Always give priority to the event queue. Don't want to overflow the event queue. */
    if (osapiMessageReceive(wioInfo->wioMsgQ, &eventMsg,
                                 sizeof(wioEventMsg_t), L7_NO_WAIT) == L7_SUCCESS)
    {
      switch (eventMsg.msgType)
      {
        case WIO_CNFGR_INIT:
          if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            wioCnfgrHandle(&eventMsg.wioMsgData.cmdData);
            osapiSemaGive(wioInfo->wioLock);
          }
          break;

        case WIO_CP_INTF_STATE:
          if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            if (eventMsg.wioMsgData.cpEnableMsg.cpEvent == WIO_INTF_EVENT_ENABLE)
            {
              wioIntfEnable(eventMsg.wioMsgData.cpEnableMsg.intIfNum,
                            eventMsg.wioMsgData.cpEnableMsg.authServer);
            }
            else
            {
              wioIntfDisable(eventMsg.wioMsgData.cpEnableMsg.intIfNum);
            }
            osapiSemaGive(wioInfo->wioLock);
          }
          break;

        case WIO_CP_BLOCK:
          if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            if (eventMsg.wioMsgData.blockMsg.block == WIO_INTF_BLOCK)
            {
              wioIntfBlock(eventMsg.wioMsgData.blockMsg.intIfNum);
            }
            else if (eventMsg.wioMsgData.blockMsg.block == WIO_INTF_UNBLOCK)
            {
              wioIntfUnblock(eventMsg.wioMsgData.blockMsg.intIfNum);
            }
            else
            {
              L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
                      "Bad captive portal block value %d.",
                      eventMsg.wioMsgData.blockMsg.block);
            }
            osapiSemaGive(wioInfo->wioLock);
          }
          break;

        case WIO_CP_AUTH:
          if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            if (eventMsg.wioMsgData.clientAuthMsg.auth == WIO_CLIENT_EVENT_AUTH)
            {
              wioClientAuth(&eventMsg.wioMsgData.clientAuthMsg.clientMac);
            }
            else if (eventMsg.wioMsgData.clientAuthMsg.auth == WIO_CLIENT_EVENT_UNAUTH)
            {
              wioClientUnauth(&eventMsg.wioMsgData.clientAuthMsg.clientMac);
            }
            else
            {
              wioClientDeauth(&eventMsg.wioMsgData.clientAuthMsg.clientMac);
            }
            osapiSemaGive(wioInfo->wioLock);
          }
          break;

        case WIO_AUTH_PORT:
          if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) == L7_SUCCESS)
          {
            wioOptWebServerPortSet(eventMsg.wioMsgData.authPortMsg.intIfNum,
                                   eventMsg.wioMsgData.authPortMsg.tcpPortNum,
                                   eventMsg.wioMsgData.authPortMsg.portFlavor);
            osapiSemaGive(wioInfo->wioLock);
          }
          break;

        default:
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
                  "Captive portal wired interface owner got unknown event %d",
                  eventMsg.msgType);
      }
    }
    /* event queue was empty. Look for an ARP packet. */
    else if (osapiMessageReceive(wioInfo->wioFrameMsgQ, &frameMsg,
                                 sizeof(wioFrameMsg_t), L7_NO_WAIT) == L7_SUCCESS)
    {
      if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) == L7_SUCCESS)
      {
        wioFrameForward(frameMsg.rxIntf, frameMsg.vlanId,
                        frameMsg.frameBuf, frameMsg.dataLen);
        osapiSemaGive(wioInfo->wioLock);
      }
    }
    else
    {
      /* Someone gave the msg queue semaphore, but didn't put a message on
       * either queue. Count it. */
      wioInfo->debugStats.msgQRxError++;
    }
  }
}


/*********************************************************************
* @purpose  Take a peek at incoming IPv4 packets. If packet is from
*           a new client, inform captive portal. If packet is http(s)
*           or destination TCP port is the optional web server port for
*           captive portal, redirect the packet to the local captive
*           portal authentication server.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED
*           SYSNET_PDU_RC_IGNORED
*           SYSNET_PDU_RC_DISCARD   if ingress interface is blocked,
*                                   or if IPv4 packet from unauth client
*
* @notes    This function executed on DTL thread.
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t wioPacketInterceptIn(L7_uint32 hookId,
                                     L7_netBufHandle bufHandle,
                                     sysnet_pdu_info_t *pduInfo,
                                     L7_FUNCPTR_t continueFunc)
{
  L7_enetHeader_t *ethHeader;
  L7_ipHeader_t *ipHeader;
  L7_uint32 clientIpAddr;   /* host byte order */
  L7_uint32 len, ethHeaderSize;
  L7_uchar8 *data;
  wioClient_t *client = NULL;
  wioIntfInfo_t *intfInfo;
  L7_enetMacAddr_t *destMac;

  /* If ingress interface is not an interface that is valid for captive portal,
   * ignore the packet. */
  if (!wioIsValidIntf(pduInfo->intIfNum))
  {
    return SYSNET_PDU_RC_IGNORED;
  }

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Ignore packets received on interfaces not enabled for captive portal */
  if (!wioIntfIsEnabled(pduInfo->intIfNum))
  {
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_CP_WIO_RX);

  intfInfo = wioIntfInfoGet(pduInfo->intIfNum);
  intfInfo->debugStats.packetsIn++;

  /* Drop packets received on interfaces administratively blocked */
  if (wioIntfIsBlocked(pduInfo->intIfNum))
  {
    intfInfo->debugStats.packetsInBlocked++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_DISCARD;
  }

  /* Captive portal is enabled on this interface. Dig deeper. */
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);
  ethHeader = (L7_enetHeader_t*) data;
  ethHeaderSize = sysNetDataOffsetGet(data);
  ipHeader = (L7_ipHeader_t*)(data + ethHeaderSize);

  /* Make sure this is IPv4 */
  if ((ipHeader->iph_versLen & 0xF0) != (L7_IP_VERSION << 4))
  {
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  clientIpAddr = osapiNtohl(ipHeader->iph_src);
  if (wioSourceIpAddressInvalid(clientIpAddr))
  {
    /* Let system treat invalid source as it normally would. */
    intfInfo->debugStats.invalidSrcAddr++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* ignore packets sent directly to the authentication server, unless
   * the server address is the network port address. In the latter case
   * we may need to tweek the ingress VLAN and interface. */
  if ((osapiNtohl(ipHeader->iph_dst) == wioInfo->authServerAddr) &&
      (wioInfo->authServerAddr != simGetSystemIPAddr()))
  {
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Determine if client is an existing or new client, or if an existing
   * client has moved to a new interface. Update client list as necessary
   * and inform captive portal, as necessary. */
  if ((wioClientFind(&ethHeader->src, L7_MATCH_EXACT, &client) != L7_SUCCESS) ||
      (client->intIfNum != pduInfo->intIfNum) ||
      (client->clientIpAddr != clientIpAddr))
  {
    if (client)
    {
      /* Either client has moved to a new interface or his IP address has changed.
       * In either case, delete the existing client and recreate with updated
       * information. */
      if (client->intIfNum != pduInfo->intIfNum)
      {
        /* Client has moved to a new interface */
        wioInfo->debugStats.clientMoves++;
      }
      wioClientRemove(&ethHeader->src);
      client = NULL;
    }

    /* Create (or recreate) client */
    client = wioClientAdd(&ethHeader->src, clientIpAddr, pduInfo->intIfNum,
                          pduInfo->vlanId);
    if (client && (clientIpAddr != 0))
    {
      /* Tell captive portal. If client address is 0 (we created client
       * in response to seeing his DHCP Discover), don't tel CP yet. Wait until
       * client has an IP address. */
      intfInfo->debugStats.newClients++;
      wioClientAuthReq(pduInfo->intIfNum, &ethHeader->src, clientIpAddr);
    }
  }

  if (!client)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
    nimGetIntfName(pduInfo->intIfNum, L7_SYSNAME, ifName);
    osapiInetNtoa(clientIpAddr, ipAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Failed to create captive portal client for packet on %s from %s.",
            ifName, ipAddrStr);
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Now determine whether packet should be redirected to local authentication
   * server, and if so, set the packet's destination IP address to the auth
   * server's address (and do checksum housekeeping).  */

  if (client->clientState == WIO_CLIENT_AUTH)
  {
    /* Client is authenticated. Let packet pass unmolested. */
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Client is not authenticated. Let certain packets (e.g., DHCP, DNS) through */
  if (wioPassUnauthClientPacket(ipHeader))
  {
    /* Unauthenticated client allowed to send this packet. */
    destMac = &ethHeader->dest;
    if (wioDestMacIsLocal(destMac, pduInfo->intIfNum, pduInfo->vlanId))
    {
      /* Packet can be routed. Ignore and let continue to software forwarding code. */
      intfInfo->debugStats.unauthPacketsPassed++;
      osapiSemaGive(wioInfo->wioLock);
      return SYSNET_PDU_RC_IGNORED;
    }

    /* dest MAC not local unicast. Could be broadcast or mcast. If so, process
     * locally and flood. */
    if (destMac->addr[0] & 0x01)
    {
      if (wioPacketQueue(data, len, pduInfo) == L7_SUCCESS)
      {
        intfInfo->debugStats.arpPacketsForward++;
        osapiSemaGive(wioInfo->wioLock);
        return SYSNET_PDU_RC_COPIED;
      }
      /* if we failed to queue the broadcast packet, go ahead and flood it. */
    }

    /* IP packet intercepted on switching interface. We'll have to switch it. */
    if (wioPacketQueue(data, len, pduInfo) == L7_SUCCESS)
    {
      intfInfo->debugStats.unauthPacketsPassed++;
    }
    osapiSemaGive(wioInfo->wioLock);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return SYSNET_PDU_RC_CONSUMED;
  }

  if ((client->clientState == WIO_CLIENT_PENDING) ||
      (client->clientState == WIO_CLIENT_AUTH))
  {
    /* Captive portal knows about this client. Client is in the process of
     * being authenticated. Redirect http(s) packets to auth server. */
    if (wioRedirectPacketType(pduInfo->intIfNum, ipHeader))
    {
      /* update list of client connections for authenticating host */
      wioUpdateClientConnList(client, pduInfo->intIfNum, ipHeader);

      /* Set destination IP address to auth server and update checksums */
      if (wioPacketRedirect(ethHeader, ipHeader, pduInfo) == L7_SUCCESS)
      {
        intfInfo->debugStats.redirectedPacketsIn++;
        /* Pretend we ignored the packet */
        osapiSemaGive(wioInfo->wioLock);
        return SYSNET_PDU_RC_IGNORED;
      }
    }
  }

  intfInfo->debugStats.unauthPacketsDropped++;
  osapiSemaGive(wioInfo->wioLock);
  /* Discard IPv4 packet from unauthenticated client */
  return SYSNET_PDU_RC_DISCARD;
}

/*********************************************************************
* @purpose  Forward a packet on the WIO task
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} ingress VLAN ID
* @param    frame    @b{(input)} start of ethernet frame
* @param    dataLen  @b{(input)} frame length in bytes (incl. ethernet header)
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wioFrameForward(L7_uint32 intIfNum, L7_ushort16 vlanId,
                        L7_uchar8 *frame, L7_uint32 dataLen)
{
  L7_enetMacAddr_t *destMac = (L7_enetMacAddr_t*) frame;

  if (destMac->addr[0] & 0x01)
    return wioFrameFlood(intIfNum, vlanId, frame, dataLen);
  else
    return wioFrameUnicast(intIfNum, vlanId, frame, dataLen);
}

/*********************************************************************
* @purpose  Flood an ARP packet in a given VLAN
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} ingress VLAN ID
* @param    frame    @b{(input)} start of ethernet frame
* @param    dataLen  @b{(input)} frame length in bytes (incl. ethernet header)
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wioFrameFlood(L7_uint32 intIfNum, L7_ushort16 vlanId,
                      L7_uchar8 *frame, L7_uint32 dataLen)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i, activeState = L7_INACTIVE;
  L7_RC_t rc = L7_SUCCESS;

  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  {
    for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
    {
      if (L7_INTF_ISMASKBITSET(portMask, i) &&
          (i != intIfNum))
      {
        if ((nimGetIntfActiveState(i, &activeState) == L7_SUCCESS) &&
            (activeState == L7_ACTIVE))
        {
          /* Send on an interface that is link up and in forwarding state */
          if (wioFrameSend(i, vlanId, frame, dataLen) != L7_SUCCESS)
          {
            wioInfo->debugStats.frameTxFailures++;
            rc = L7_FAILURE;
          }
        }
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Unicast an packet
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} ingress VLAN ID
* @param    frame    @b{(input)} start of ethernet frame
* @param    dataLen  @b{(input)} frame length in bytes (incl. ethernet header)
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wioFrameUnicast(L7_uint32 intIfNum, L7_ushort16 vlanId,
                        L7_uchar8 *frame, L7_uint32 dataLen)
{
  L7_uchar8 vidMac[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t fdbEntry;
  L7_enetMacAddr_t *destMac = (L7_enetMacAddr_t*) frame;
  L7_uint32 outIntf;         /* internal interface number of egress port */

  /* Look up {VLAN ID, dest MAC} pair in fdb  */
  memset(vidMac, 0, L7_FDB_KEY_SIZE);
  memset(&fdbEntry, 0, sizeof(fdbEntry));
  memcpy(vidMac, &vlanId, L7_MFDB_VLANID_LEN);
  memcpy(vidMac + L7_MFDB_VLANID_LEN, destMac->addr, L7_MAC_ADDR_LEN);
  if (fdbFind(vidMac, L7_MATCH_EXACT, &fdbEntry) != L7_SUCCESS)
  {
    /* Flood in VLAN */
    return wioFrameFlood(intIfNum, vlanId, frame, dataLen);
  }

  outIntf = fdbEntry.dot1dTpFdbPort;
  if (wioFrameSend(outIntf, vlanId, frame, dataLen) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(outIntf, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Failure sending ARP message on interface %s in VLAN %u",
            ifName, vlanId);
  }
  return L7_SUCCESS;
}

/***********************************************************************
* @purpose Send an ARP packet on a given interface
*
* @param    intIfNum   @b{(input)} outgoing interface
* @param    vlanId     @b{(input)} VLAN ID
* @param    frame      @b{(input)} ethernet frame
* @param    frameLen   @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes    Assumes ethernet encapsultation on outgoing port. Apparently we don't really
*           support SNAP encapsulation.
*
* @end
*
***********************************************************************/
L7_RC_t wioFrameSend(L7_uint32 intIfNum, L7_ushort16 vlanId,
                     L7_uchar8 *frame, L7_ushort16 frameLen)
{
  DTL_CMD_TX_INFO_t dtlCmd;
  L7_ethHeader_t *ethHeader;
  L7_netBufHandle bufHandle;       /* mbuf allocated here to tx packet */
  L7_ushort16 ethertype;           /* ethertype */
  L7_uchar8 *dataStart;            /* start of ethernet header in mbuf we create */

  dtlCmd.intfNum = intIfNum;
  dtlCmd.priority = 0;
  dtlCmd.typeToSend = DTL_NORMAL_UNICAST;
  dtlCmd.cmdType.L2.domainId = vlanId;

  /* Allocate an mbuf to copy frame into */
  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    /* Don't log this. mbuf alloc failures happen occasionally. */
    wioInfo->debugStats.mbufFailures++;
    return L7_FAILURE;
  }
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);

  /* copy packet to new mbuf */
  memcpy(dataStart, frame, frameLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLen);

  ethHeader = (L7_ethHeader_t*) dataStart;
  ethertype = osapiNtohs(ethHeader->ethertype);
  if (ethertype == L7_ETYPE_8021Q)
  {
    /* Strip VLAN tag.  Let driver insert tag if appropriate. */
    L7_int32 index;
    for (index = L7_ENET_HDR_SIZE - 1; index >= 0; index--)
    {
      /* Shift the Destination MAC Address and the Source MAC Address by 4 bytes.
      ** These 4 bytes are the VLAN header.
      */
      *(dataStart + index + L7_8021Q_ENCAPS_HDR_SIZE) = *(dataStart + index);
    }

    dataStart = dataStart + L7_8021Q_ENCAPS_HDR_SIZE;
    frameLen -= L7_8021Q_ENCAPS_HDR_SIZE;
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLen);
    SYSAPI_NET_MBUF_SET_DATASTART(bufHandle, dataStart);
  }

  /* Existing encapsulation ok for egress port. Slap it in mbuf and send. */
  return dtlPduTransmit(bufHandle, DTL_CMD_TX_L2, &dtlCmd);
}

/*********************************************************************
* @purpose  Intercept incoming ARP packets and forward those received
*           from unauthenticated clients.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if packet to be forwarded
*           SYSNET_PDU_RC_IGNORED   if packet should continue normal processing
*           SYSNET_PDU_RC_DISCARD   if packet falls through the cracks
*           SYSNET_PDU_RC_COPIED    if broadcast destination MAC
*
* @notes    All IP and ARP packets from unauthenticated clients come to the CPU.
*           CP has no use for ARP packets, but we're stuck forwarding them
*           on L2 boxes. We have to let ARP packets through, or else the client may
*           never send http packets. If an ARP packet is from an authenticated client,
*           ignore it. If the destination MAC address is a local MAC address,
*           ignore the ARP packet. If the destination is a non-local unicast
*           address or a broadcast address, forward the ARP packet in the VLAN.
*
*           This function is executed on the DTL thread.
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t wioArpIntercept(L7_uint32 hookId,
                                L7_netBufHandle bufHandle,
                                sysnet_pdu_info_t *pduInfo,
                                L7_FUNCPTR_t continueFunc)
{
  L7_uchar8 *data;
  L7_uint32 dataLen;
  L7_enetHeader_t *ethHeader;
  L7_enetMacAddr_t *srcMac;
  L7_enetMacAddr_t *destMac;
  wioClient_t *client;
  wioIntfInfo_t *intfInfo;

  /* If ingress interface is not an interface that is valid for captive portal,
   * ignore the packet. */
  if (!wioIsValidIntf(pduInfo->intIfNum))
  {
    return SYSNET_PDU_RC_IGNORED;
  }

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Ignore packets received on interfaces not enabled for captive portal */
  if (!wioIntfIsEnabled(pduInfo->intIfNum))
  {
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  intfInfo = wioIntfInfoGet(pduInfo->intIfNum);
  intfInfo->debugStats.arpPacketsIn++;

  /* Determine whether sender is an authenticated client */
  SYSAPI_NET_MBUF_SET_LOC(bufHandle, MBUF_LOC_CP_WIO_ARP_RX);
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLen);
  ethHeader = (L7_enetHeader_t*) data;
  srcMac = &ethHeader->src;

  if (wioClientFind(srcMac, L7_MATCH_EXACT, &client) == L7_SUCCESS)
  {
    if (client->clientState == WIO_CLIENT_AUTH)
    {
      /* If the sender is an authenticated client, the packet should only come
       * to the CPU for reasons unrelated to CP (e.g., this box is the destination
       * of the ARP packet). Let this ARP packet continue on its merry way */
      intfInfo->debugStats.arpPacketsAuthClient++;
      osapiSemaGive(wioInfo->wioLock);
      return SYSNET_PDU_RC_IGNORED;
    }
  }

  /* ARP packet is from unauthenticated client on CP interface. If destination
   * MAC is local (router interface, network port interface, or VRRP MAC), then
   * ignore packet and let it be processed locally. */
  destMac = &ethHeader->dest;
  if (wioDestMacIsLocal(destMac, pduInfo->intIfNum, pduInfo->vlanId))
  {
    intfInfo->debugStats.arpPacketsLocal++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* ARP packet destination is not local unicast. If it's a broadcast or multicast,
   * process locally and flood. */
   if (destMac->addr[0] & 0x01)
  {
    if (wioPacketQueue(data, dataLen, pduInfo) == L7_SUCCESS)
    {
      intfInfo->debugStats.arpPacketsForward++;
      osapiSemaGive(wioInfo->wioLock);
      return SYSNET_PDU_RC_COPIED;
    }
    /* if we failed to queue the broadcast packet, go ahead and flood it. */
  }

  /* Do transmit processing on own thread */
  if (wioPacketQueue(data, dataLen, pduInfo) == L7_SUCCESS)
  {
    intfInfo->debugStats.arpPacketsForward++;
    osapiSemaGive(wioInfo->wioLock);
    SYSAPI_NET_MBUF_FREE(bufHandle);
    return SYSNET_PDU_RC_CONSUMED;
  }

  intfInfo->debugStats.arpPacketsDiscarded++;
  osapiSemaGive(wioInfo->wioLock);
  return SYSNET_PDU_RC_DISCARD;
}

/*********************************************************************
* @purpose  Determine whether a MAC address is local to this box.
*
* @param    destMac    @b{(input)} destination MAC address
* @param    intIfNum   @b{(input)} ingress interface
* @param    vlanId     @b{(input)} VLAN ID of incoming packet
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    Dest MAC local if it is
*            1. network port MAC
*            2. routing interface MAC
*            3. VRRP MAC of local virtual router
*
* @end
*********************************************************************/
L7_BOOL wioDestMacIsLocal(L7_enetMacAddr_t *destMac,
                          L7_uint32 intIfNum, L7_ushort16 vlanId)
{
  L7_enetMacAddr_t networkPortMac;
  L7_enetMacAddr_t routingIntfMac;
  L7_uint32 rif = 0;    /* If ARP packet received on a routing interface, this is it */
  L7_uint32 vlanRoutingIf;
  L7_uint32 routingEnabled;


  /* Check if destination is network port */
  simGetSystemIPBurnedInMac(networkPortMac.addr);
  if (memcmp(destMac, networkPortMac.addr, L7_MAC_ADDR_LEN) == 0)
  {
    /* Destination MAC is network port MAC. */
    return L7_TRUE;
  }

  /* Check if ARP packet is to a local routing interface */
  if ((ipMapRtrIntfModeGet(intIfNum, &routingEnabled) == L7_SUCCESS) &&
      (routingEnabled == L7_ENABLE))
  {
    /* arrived on port based routing interface */
    rif = intIfNum;
  }
  else if ((dot1qVlanIntfVlanIdToIntIfNum(vlanId, &vlanRoutingIf) == L7_SUCCESS) &&
           (ipMapRtrIntfModeGet(vlanRoutingIf, &routingEnabled) == L7_SUCCESS) &&
           (routingEnabled == L7_ENABLE))
  {
    /* arrived on VLAN routing interface */
    rif = vlanRoutingIf;
  }
  if ((rif != 0) &&
      (nimGetIntfAddress(rif, L7_SYSMAC_BIA, routingIntfMac.addr) == L7_SUCCESS) &&
      (memcmp(destMac, routingIntfMac.addr, L7_MAC_ADDR_LEN) == 0))
  {
    /* Destination MAC is for local routing interface. */
    return L7_TRUE;
  }

  /* If VRRP is running on ingress interface, we could receive an ARP Request
   * whose destination MAC address is the VRRP MAC address (if the sender
   * is renewing its ARP cache entry for us). ARP replies should come back with
   * our real MAC address. If the destination MAC address looks like a VRRP
   * MAC address and the ingress interface is a routing interface, then process
   * ARP packet locally. */
  if (rif)
  {
    /* Packet received on a routing interface. VRRP MAC can only be local
     * if packet arrived on a routing interface. Don't worry whether last byte
     * of MAC address (virtual router ID) matches. VRRP will check this. */
    if (memcmp(destMac, L7_ENET_VRRP_MAC_ADDR.addr, L7_MAC_ADDR_LEN - 1) == 0)
      return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Queue an ethernet frame for processing on our own thread
*
* @param    ethHeader    @b{(input)} start of ethernet header
* @param    dataLen      @b{(input)} length of ethernet frame
* @param    pduInfo      @b{(input)} pointer to pdu info structure
*                                    which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS if successfully queued
*           L7_FAILURE if queuing failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t wioPacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                       sysnet_pdu_info_t *pduInfo)
{
  wioFrameMsg_t wioFrameMsg;
  wioIntfInfo_t *intfInfo = wioIntfInfoGet(pduInfo->intIfNum);

  if (dataLen > WIO_MAX_FRAME_SIZE)
  {
    intfInfo->debugStats.frameTooLong++;
    return L7_FAILURE;
  }

  memcpy(&wioFrameMsg.frameBuf, ethHeader, dataLen);
  wioFrameMsg.dataLen = dataLen;
  wioFrameMsg.rxIntf = pduInfo->intIfNum;
  wioFrameMsg.vlanId = pduInfo->vlanId;

  if (osapiMessageSend(wioInfo->wioFrameMsgQ, &wioFrameMsg, sizeof(wioFrameMsg_t), L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(wioInfo->msgQSema);
  }
  else
  {
    wioInfo->debugStats.frameMsgQFull++;
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Take a peek at outgoing IPv4 packets. If packet matches a
*           captive portal client connection on a wired interface,
*           replace the source IP address with the remote IP address
*           the client thinks it is talking to. If the packet is to
*           a destination MAC address and egress interface other
*           than those of the client, change those, too.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_IGNORED
*           SYSNET_PDU_RC_DISCARD   if ingress interface is blocked
*
* @notes    Packets are intercepted after ethernet header is added.
*
*           Box could be a L2 box whose only IP address is the IP address
*           on the network port. Clients could have IP addresses in a
*           different subnet that the network port. In this case, the
*           IP stack will route outgoing packets based on the default
*           route associated with the network port. We reroute these
*           directly to the client. Amazing.
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t wioPacketInterceptOut(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t continueFunc)
{
  L7_enetHeader_t *ethHeader;
  L7_ipHeader_t *ipHeader;
  L7_tcpHeader_t *tcpHeader;
  L7_uint32 destIpAddr;        /* destination IP address of packet. host byte order */
  L7_netBlockHandle blockHandle = L7_NULL; /* important to initialize NULL */
  L7_uint32 len = 0, ethHeaderSize = 0;
  L7_ushort16 *pEtype;
  L7_ushort16 protocol_type;
  L7_uchar8 *data = NULL;
  wioIntfInfo_t *intfInfo;
  L7_ushort16 clientTcpPort;
  wioClientConn_t *clientConn;
  wioClient_t *client;
  DTL_CMD_TX_INFO_t  dtlCmd;

  /* Egress interface may not be the interface the client is on. So we can't
   * ignore packets whose egress interface isn't a CP wired interface. */

  if (osapiSemaTake(wioInfo->wioLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return SYSNET_PDU_RC_IGNORED;
  }


  wioInfo->debugStats.packetsOut++;

  /* Captive portal is enabled on this interface. Dig deeper. */
  /* Outgoing packets can be in OS-specific buffers. So need to take some care here.
   * Calling SYSAPI_NET_MBUF_GET_DATASTART doesn't return a pointer to the real data.
   * First we'll just peek at the headers and see if this is a packet we want
   * to mess with. Try to avoid copying data for packets we ignore. */
  sysapiNetMbufGetNextBuffer(bufHandle, &blockHandle, &data, &len);
  if (data == NULL)
  {
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  ethHeader = (L7_enetHeader_t*) data;
  ethHeaderSize = sysNetDataOffsetGet(data);
  if (len < ethHeaderSize + L7_IP_HDR_LEN)
  {
    /* I am assuming the buffer will always include at least the ethernet
     * and IP headers. If that's not a good assumption, this counter
     * will be non-zero. */
    wioInfo->debugStats.outBufferTooSmall++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Make sure this is an IP packet */
  pEtype = (L7_ushort16 *)(data + ethHeaderSize - 2);
  protocol_type = osapiNtohs(*pEtype);
  if (protocol_type != L7_ETYPE_IP)
  {
    /* sysnet shouldn't give us non-IP packets here */
    wioInfo->debugStats.nonIpPacketsOut++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  ipHeader = (L7_ipHeader_t*)(data + ethHeaderSize);

  /* Make sure this is IPv4 */
  if ((ipHeader->iph_versLen & 0xF0) != (L7_IP_VERSION << 4))
  {
    /* Ditto to previous comment */
    wioInfo->debugStats.nonIpPacketsOut++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  if (ipHeader->iph_prot != IP_PROT_TCP)
  {
    /* This is ok */
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  wioInfo->debugStats.tcpOut++;

  /* Make sure our buffer is large enough to include a TCP header. */
  tcpHeader = (L7_tcpHeader_t*)(data + ethHeaderSize + wioIpHdrLen(ipHeader));
  if (len < (ethHeaderSize + wioIpHdrLen(ipHeader) + TCP_HEADER_LENGTH(tcpHeader)))
  {
    wioInfo->debugStats.outBufferTooSmall++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  /* Now see if this is a reply to one of our clients in auth pending state */
  destIpAddr = osapiNtohl(ipHeader->iph_dst);
  client = wioClientFindByIpAddr(destIpAddr);
  if (!client)
  {
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_IGNORED;
  }

  intfInfo = wioIntfInfoGet(client->intIfNum);
  intfInfo->debugStats.packetsOut++;

  /* Drop packets to be sent on interfaces administratively blocked */
  if (wioIntfIsBlocked(client->intIfNum))
  {
    intfInfo->debugStats.packetsOutBlocked++;
    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_DISCARD;
  }

  /* Find matching connection to this client */
  clientTcpPort = osapiNtohs(tcpHeader->destPort);
  clientConn = wioClientConnFind(client->intIfNum, client->clientIpAddr, clientTcpPort);
  if (clientConn)
  {
    intfInfo->debugStats.sourceAddrChange++;
    if (wioModifySourceAddress(clientConn, bufHandle) != L7_SUCCESS)
    {
      intfInfo->debugStats.failedToModSrcAddr++;
    }
    /* If IP stack chose a next hop other than the client itself,
     * redirect packet directly to the client. */
    if (memcmp(ethHeader->dest.addr, client->clientMacAddr.addr, L7_MAC_ADDR_LEN))
    {
      memcpy(ethHeader->dest.addr, client->clientMacAddr.addr, L7_MAC_ADDR_LEN);
    }

    /* Since outgoing interface and VLAN may have changed, consume packet and
     * send directly. */
    dtlCmd.intfNum = client->intIfNum;
    dtlCmd.priority = 0;
    dtlCmd.typeToSend = DTL_NORMAL_UNICAST;
    dtlCmd.cmdType.L2.domainId = client->clientVlan;
    dtlPduTransmit(bufHandle, DTL_CMD_TX_L2, &dtlCmd);   /* takes ownership of mbuf */

    osapiSemaGive(wioInfo->wioLock);
    return SYSNET_PDU_RC_CONSUMED;
  }

  osapiSemaGive(wioInfo->wioLock);
  return SYSNET_PDU_RC_IGNORED;
}


