/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_main.c
*
* @purpose   DHCP snooping
*
* @component DHCP snooping
*
* @comments none
*
* @create 3/26/2007
*
* @author Rob Rice (rrice)
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "log.h"
#include "nimapi.h"
#include "comm_mask.h"
#include "trapapi.h"

#include "ptin_dhcp.h"

#include "dot1q_api.h"
#include "dot3ad_api.h"

#include "pppoe_cnfgr.h"
#include "pppoe_util.h"

extern pppoeCnfgrState_t pppoeCnfgrState;
extern osapiRWLock_t pppoeCfgRWLock;

extern void *pppoeMsgQSema;
extern void *pppoe_Event_Queue;
extern void *pppoe_Packet_Queue;


/*********************************************************************
* @purpose  Main function for the DHCP snooping thread. Read incoming
*           messages (events and DHCP packets) and process accordingly.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void pppoeTask(void)
{
   pppoeEventMsg_t eventMsg;
   pppoeFrameMsg_t frameMsg;

   /* Loop forever, processing incoming messages */
   while (L7_TRUE)
   {
     /* Since we are reading from multiple queues, we cannot wait forever
      * on the message receive from each queue. Use a semaphore to indicate
      * whether any queue has data. Wait until data is available.
      */
     if (osapiSemaTake(pppoeMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, PTIN_PPPOE_COMPONENT_ID,
              "Failure taking PPPoE message queue semaphore.");
       continue;
     }
     memset(&eventMsg, 0, sizeof(pppoeEventMsg_t));
     memset(&frameMsg, 0, sizeof(pppoeFrameMsg_t));

     /* Always try to read from the event queue first. No sense processing
      * DHCP packets with stale state. */
     if (osapiMessageReceive(pppoe_Event_Queue, &eventMsg,
                             sizeof(pppoeEventMsg_t), L7_NO_WAIT) == L7_SUCCESS)
     {
       /* Configuration actions are all done on the UI thread.
        * Synchronize that processing with event processing by
        * taking the same semaphore here. */
       switch (eventMsg.msgType)
       {
         case PPPOE_CNFGR_INIT:
           if (osapiWriteLockTake(pppoeCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
           {
             pppoeCnfgrHandle(&eventMsg.pppoeMsgData.cmdData);
             osapiWriteLockGive(pppoeCfgRWLock);
           }
           break;
         default:
           L7_LOGF(L7_LOG_SEVERITY_ERROR, PTIN_PPPOE_COMPONENT_ID,
                  "Unknown event message type received in the DHCP snooping thread.");
           break;
       }
     }

     /* If the event queue was empty, see if a DHCP message has arrived. */
     else if (osapiMessageReceive(pppoe_Packet_Queue, &frameMsg,
                                  sizeof(pppoeFrameMsg_t), L7_NO_WAIT) == L7_SUCCESS)
     {
       if (osapiWriteLockTake(pppoeCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
       {
         pppoeFrameProcess(frameMsg.rxIntf, frameMsg.vlanId, frameMsg.frameBuf,
                           frameMsg.frameLen, frameMsg.innerVlanId, frameMsg.client_idx);     /* PTin modified: DHCP snooping */
         osapiWriteLockGive(pppoeCfgRWLock);
       }
     }
   } /* end while true */

  return;
}

/*********************************************************************
* @purpose  Take a peek at incoming IP packets. If DHCP, snoop them.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if frame has been consumed; stop processing it
* @returns  SYSNET_PDU_RC_IGNORED   if frame has been ignored; continue processing it
*
* @notes    don't barf on IP packets with options
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t pppoePacketIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
//  L7_ipHeader_t *ipHeader;
//  L7_udp_header_t *udpHeader;
//  L7_dhcp_packet_t *dhcpPacket;
//  L7_uint32 rc;
//  L7_uint32 len, ethHeaderSize;
//  L7_uchar8 *data, sysMacAddr[L7_MAC_ADDR_LEN];
//  L7_uint32 vlanId, innerVlanId = 0;
//#if defined(L7_ROUTING_PACKAGE) || defined(L7_DHCPS_PACKAGE)
//  L7_uint32 mode;
//#endif
//  L7_uint client_idx = (L7_uint)-1;   /* PTin added: DHCP snooping */
//
//  if (ptin_debug_dhcp_snooping)
//    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Packet intercepted: intIfNum=%u, vlanId=%u, innerVlanId=%u",
//              pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);
//
//  if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//  {
//    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                  "(%s)Packet rx'ed at DHCP intercept, intIfNum=%u, vlanId = %d, inner vlanId = %d",
//                  __FUNCTION__, pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);
//    dsTraceWrite(traceMsg);
//  }
//
//  if (! pppoeInfo->l2HwUpdateComplete)
//  {
//    /* DHCP packet received before control plane is ready. Drop such packet */
//    pppoeInfo->debugStats.msgsDroppedControlPlaneNotReady++;
//    return SYSNET_PDU_RC_DISCARD;
//  }
//
//  /* Shouldn't get packets if DHCP snooping disabled, but if we do, ignore them. */
//  if (pppoeCfgData->dsGlobalAdminMode != L7_ENABLE )
//  {
//    #ifdef L7_DHCP_L2_RELAY_PACKAGE
//    if (pppoeCfgData->dsL2RelayAdminMode != L7_ENABLE)
//    {
//      if (pppoeCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS)
//      {
//        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                    "(%s)Packet rx'ed is ignored as neither GBL Snooping nor L2 Relay is enabled at DHCP intercept.",
//                      __FUNCTION__);
//        dsTraceWrite(traceMsg);
//      }
//      return SYSNET_PDU_RC_IGNORED;
//    }
//    #else
//    return SYSNET_PDU_RC_IGNORED;
//    #endif
//  }
//
//  /* If either DHCP snooping or the L2 Relay is not enabled on
//     rx interface, ignore packet. */
//  if (dsVlanIntfIsSnooping(pduInfo->vlanId,pduInfo->intIfNum) /*dsIntfIsSnooping(pduInfo->intIfNum)*/ == L7_FALSE )   /* PTin modified: DHCP snooping */
//  {
//    #ifdef L7_DHCP_L2_RELAY_PACKAGE
//    if ( _dsVlanIntfL2RelayGet(pduInfo->vlanId,pduInfo->intIfNum) /*_dsIntfL2RelayGet(pduInfo->intIfNum)*/ == L7_FALSE) /* PTin modified: DHCP snooping */
//    {
//      if (pppoeCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS)
//      {
//        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                    "(%s)Packet rx'ed is ignored as neither INTF Snooping nor L2 Relay is enabled at DHCP intercept.",
//                      __FUNCTION__);
//        dsTraceWrite(traceMsg);
//      }
//      return SYSNET_PDU_RC_IGNORED;
//    }
//    #else
//    return SYSNET_PDU_RC_IGNORED;
//    #endif
//  }
//
//  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
//  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);
//  ethHeaderSize = sysNetDataOffsetGet(data);
//  ipHeader = (L7_ipHeader_t*)(data + ethHeaderSize);
//
//  if (((ipHeader->iph_versLen & 0xF0) == (L7_IP_VERSION << 4)) &&
//      (ipHeader->iph_prot == IP_PROT_UDP))
//  {
//    if (((osapiNtohl(ipHeader->iph_src) & L7_CLASS_D_ADDR_NETWORK) == L7_CLASS_D_ADDR_NETWORK) ||
//        ((osapiNtohl(ipHeader->iph_src) & L7_CLASS_E_ADDR_NETWORK) == L7_CLASS_E_ADDR_NETWORK))
//    {
//      /* Illegal src IP in DHCP packet. Ignore it. */
//      pppoeInfo->debugStats.badSrcAddr++;
//      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
//      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
//
//      if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//      {
//        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                      "(%s) Packet ignored, because of ip source (0x%08x)",
//                      __FUNCTION__, ipHeader->iph_src);
//        dsTraceWrite(traceMsg);
//      }
//      return SYSNET_PDU_RC_IGNORED;
//    }
//
//    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader + dsIpHdrLen(ipHeader));
//    if ((osapiNtohs(udpHeader->destPort) == UDP_PORT_DHCP_SERV) ||
//        (osapiNtohs(udpHeader->destPort) == UDP_PORT_DHCP_CLNT))
//
//    {
//      /* This is used only when the packet comes double tagged.*/
//      vlanId = pduInfo->vlanId;
//      innerVlanId = pduInfo->innerVlanId;
//      if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//      {
//        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                      "(%s)Packet rx'ed Dot1q VLAN Id (%d) length(%d)",
//                      __FUNCTION__, vlanId, len);
//        dsTraceWrite(traceMsg);
//      }
//      if ( dsRateLimitFilter(pduInfo))
//      {
//        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
//        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
//        SYSAPI_NET_MBUF_FREE(bufHandle);
//        if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//        {
//          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                        "(%s) Packet rate limited",
//                        __FUNCTION__);
//          dsTraceWrite(traceMsg);
//        }
//        return SYSNET_PDU_RC_CONSUMED;
//      }
//
//      /* PTin added: DHCP snooping */
//      /* Only search and validate client for non CXP360G and untrusted interfaces */
//      #if ( !PTIN_BOARD_IS_MATRIX )
//
//      ptin_client_id_t client;
//
//      if (!_dsVlanIntfTrustGet(pduInfo->vlanId,pduInfo->intIfNum))
//      {
//        #if 0
//        /* Validate inner vlan */
//        if (innerVlanId==0 || innerVlanId>=4095)
//        {
//          if (ptin_debug_dhcp_snooping)
//            LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not referenced! (intIfNum=%u, innerVlanId=%u, intVlanId=%u)",
//                  pduInfo->intIfNum, innerVlanId, vlanId);
//          ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
//          ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
//          return SYSNET_PDU_RC_IGNORED;
//        }
//        #endif
//
//        /* Client information */
//        client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
//        client.innerVlan = innerVlanId;
//        client.mask = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_INNERVLAN;
//
//        /* Only search for a client, if inner vlan is valid */
//        /* Otherwise, use dynamic DHCP */
//        if (innerVlanId>0 && innerVlanId<4096)
//        {
//          /* Find client index, and validate it */
//          if (ptin_dhcp_clientIndex_get(pduInfo->intIfNum, vlanId, &client, &client_idx)!=L7_SUCCESS ||
//              client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
//          {
//            if (ptin_debug_dhcp_snooping)
//              LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not found! (intIfNum=%u, ptin_intf=%u/%u, innerVlanId=%u, intVlanId=%u)",
//                    pduInfo->intIfNum, client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, vlanId);
//            ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
//            ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
//            return SYSNET_PDU_RC_IGNORED;
//            //client_idx = (L7_uint)-1;
//          }
//        }
//        else
//        {
//          client_idx = (L7_uint) -1;
//        }
//      }
//      #endif
//
        LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Packet intercepted at intIfNum=%u, oVlan=%u, iVlan=%u",
                  pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);

        rc = pppoePacketQueue(data, len, vlanId, pduInfo->intIfNum, innerVlanId, &client_idx);    /* PTin modified: DHCP snooping */
//
//      /* Packet intercepted */
//      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_INTERCEPTED);
//
//      if (rc == L7_REQUEST_DENIED) /* DHCP Message got filtered, So Ignore
//                                      For further processing */
//      {
//        pppoeInfo->debugStats.msgsIntercepted++;
//        pppoeInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;
//        if (ptin_debug_dhcp_snooping)
//          LOG_TRACE(LOG_CTX_PTIN_DHCP,"Incremented DHCP_STAT_FIELD_RX_FILTERED");
//        ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_FILTERED);
//        SYSAPI_NET_MBUF_FREE(bufHandle);
//        if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//        {
//          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                        "(%s) Packet denied to be stored in packet queue",
//                        __FUNCTION__);
//          dsTraceWrite(traceMsg);
//        }
//        return SYSNET_PDU_RC_CONSUMED;
//      }
//      if (rc == L7_SUCCESS)
//      {
//        pppoeInfo->debugStats.msgsIntercepted++;
//        pppoeInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;
//
//        /* If the DHCP reply message is destined for the DHCP CLIENT running on the
//           switch itself, then ignore the packet.*/
//        dhcpPacket = (L7_dhcp_packet_t*)((L7_char8 *)udpHeader + sizeof(L7_udp_header_t));
//
//        simGetSystemIPBurnedInMac(sysMacAddr);
//        if (memcmp(dhcpPacket->chaddr, sysMacAddr, L7_MAC_ADDR_LEN) == L7_NULL)
//        {
//          return SYSNET_PDU_RC_IGNORED;
//        }
//
//#ifdef L7_ROUTING_PACKAGE
//        if ((ihAdminModeGet(&mode) == L7_SUCCESS) && (mode == L7_ENABLE))
//        {
//          return SYSNET_PDU_RC_IGNORED;
//        }
//#endif /* if Relay agent is there */
//#ifdef L7_DHCPS_PACKAGE
//        if ((usmDbDhcpsAdminModeGet(0, &mode) == L7_SUCCESS) &&
//            (mode == L7_ENABLE))
//        {
//          return SYSNET_PDU_RC_IGNORED;
//        }
//#endif /* If DHCP server is there */
//        if (_dsVlanIntfTrustGet(pduInfo->vlanId,pduInfo->intIfNum) /*_dsIntfTrustGet(pduInfo->intIfNum)*/) /* Trusted port */   /* PTin modified: DHCP snooping */
//        {
//          return SYSNET_PDU_RC_IGNORED;
//        }
//        SYSAPI_NET_MBUF_FREE(bufHandle);
//        return SYSNET_PDU_RC_CONSUMED;
//      }
//    }
//    else
//    {
//      if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//      {
//        L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//        osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                      "(%s) Packet is neither server nor client (%u)",
//                      __FUNCTION__, udpHeader->destPort);
//        dsTraceWrite(traceMsg);
//      }
//    }
//  }
//  else
//  {
//    if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//    {
//      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                    "(%s) Invalid protocol received (%u), or invalid versLen (0x%02x)",
//                    __FUNCTION__, ipHeader->iph_prot, ipHeader->iph_versLen);
//      dsTraceWrite(traceMsg);
//    }
//  }

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Queue a DHCP packet for processing on our own thread
*
*
* @param    ethHeader    @b{(input)} ethernet frame
* @param    dataLen      @b{(input)} length of ethernet frame
* @param    vlanId       @b{(input)} Vlan ID
* @param    intIfNum     @b{(input)} port where packet arrived
* @param    innerVlanId  @b{(input)} port where packet arrived
*
* @returns  L7_SUCCESS
*
* @notes    vlanId represents the existing VLANID if single tagged,
*                  represents the outer tag if double tagged.
*           innerVlanId represents the inner tag if double tagged.
*
* @end
*********************************************************************/
L7_RC_t pppoePacketQueue(L7_uchar8 *ethHeader, L7_uint32 dataLen,
                      L7_ushort16 vlanId, L7_uint32 intIfNum,
                      L7_ushort16 innerVlanId, L7_uint *client_idx)    /* PTin modified: DHCP snooping && DHCPv6 */
{
//  dsFrameMsg_t dsFrameMsg;
//
//  L7_udp_header_t *udp_header;
//  L7_dhcp_packet_t *dhcpPacket;
//  L7_ushort16 ipPktLen;
//  L7_ushort16 dhcpPktLen;
//  L7_ushort16 ethHdrLen;
//  L7_ipHeader_t *ipHeader;
//  L7_ushort16 ipHdrLen;
//  L7_uchar8 ipVersion;
//
//  ethHdrLen = sysNetDataOffsetGet(ethHeader);
//  ipVersion = (*(L7_uchar8*)(ethHeader + ethHdrLen) & 0xF0) >> 4;
//
// if (ipVersion != L7_IP6_VERSION)
// {
//    ipHeader = (L7_ipHeader_t*) (ethHeader + ethHdrLen);
//    ipHdrLen = dsIpHdrLen(ipHeader);
//
//    /* Filter the DHCP messages based on rules */
//
//    udp_header = (L7_udp_header_t *) ((L7_char8 *) ipHeader + ipHdrLen);
//    dhcpPacket = (L7_dhcp_packet_t*) ((L7_char8 *) udp_header + sizeof(L7_udp_header_t));
//    ipPktLen = osapiNtohs(ipHeader->iph_len);
//    dhcpPktLen = ipPktLen - ipHdrLen - sizeof(L7_udp_header_t);
//
//    LOG_DEBUG(LOG_CTX_PTIN_DHCP, "Packet %s received at intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
//          dhcpMsgTypeNames[dsPacketType(dhcpPacket, dhcpPktLen)], intIfNum, vlanId, innerVlanId, dhcpPacket->chaddr[0], dhcpPacket->chaddr[1], dhcpPacket->chaddr[2], dhcpPacket->chaddr[3], dhcpPacket->chaddr[4], dhcpPacket->chaddr[5]);
//
//    if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//    {
//       L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
//       L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
//       L7_dhcp_pkt_type_t dhcpPktType = dsPacketType(dhcpPacket, dhcpPktLen);
//
//       memset(ifName, 0, sizeof(ifName));
//       nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
//       osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN, "(%s)DHCP snooping received %s on interface %s in VLAN %u.", __FUNCTION__, dhcpMsgTypeNames[dhcpPktType], ifName, vlanId);
//       dsTraceWrite(dsTrace);
//    }
//    if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX_DETAIL)
//    {
//       dsLogEthernetHeader((L7_enetHeader_t*) ethHeader, DS_TRACE_CONSOLE);
//       dsLogIpHeader(ipHeader, DS_TRACE_CONSOLE);
//       dsLogDhcpPacket(dhcpPacket, DS_TRACE_CONSOLE);
//    }
//
//    /* Filter DHCP packet based on security rules */
//    if (dsFrameFilter(intIfNum, vlanId, ethHeader, ipHeader, innerVlanId, client_idx)) /* PTin modified: DHCP snooping */
//    {
//       pppoeInfo->debugStats.msgsFiltered++;
//
//       if (ptin_debug_dhcp_snooping)
//          LOG_TRACE(LOG_CTX_PTIN_DHCP, "Incremented DHCP_STAT_FIELD_RX_FILTERED");
//       //ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_FILTERED);
//       return L7_REQUEST_DENIED;
//    }
// }
// pppoeInfo->debugStats.msgsReceived++;
// ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX);
//
// if (pppoeCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
// {
//   L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//   osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                 "(%s)Packet frameLen = %d ",__FUNCTION__, dataLen );
//   dsTraceWrite(traceMsg);
// }
//
// memcpy(&dsFrameMsg.frameBuf, ethHeader, dataLen);
// dsFrameMsg.rxIntf = intIfNum;
// dsFrameMsg.vlanId = vlanId;
// dsFrameMsg.frameLen = dataLen;
// /* Useful only when processing DHCP replies in Metro networks. */
// dsFrameMsg.innerVlanId = innerVlanId;
// /* PTin added: DHCP snooping */
// dsFrameMsg.client_idx  = *client_idx;
//
// if (osapiMessageSend(pppoe_Packet_Queue, &dsFrameMsg, sizeof(dsFrameMsg_t), L7_NO_WAIT,
//                       L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
// {
//  osapiSemaGive(pppoeMsgQSema);
// }
// else
// {
//   /* This may be fairly normal, so don't log. DHCP should recover. */
//   pppoeInfo->debugStats.frameMsgTxError++;
// }
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a DHCP packet on DHCP snooping thread
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pppoeFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                          L7_uchar8 *frame, L7_uint32 frameLen,
                          L7_ushort16 innerVlanId, L7_uint client_idx)
{

   L7_uchar8 *eth_header_ptr, *pppoe_header_ptr, *tlv_header_ptr;

   LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "PPPoE: Received new message");

   //Parse the received frame
   eth_header_ptr       = frame;
   mac_header           = (L7_enetHeader_t*) frame;
   ethHdrLen            = sysNetDataOffsetGet(frame);
   ipv6_header_ptr      = eth_header_ptr + ethHdrLen;
   ipv6_header          = (L7_ip6Header_t*) ipv6_header_ptr;
   udp_header_ptr       = ipv6_header_ptr + L7_IP6_HEADER_LEN;
   udp_header           = (L7_udp_header_t *) udp_header_ptr;
   dhcp_header_ptr      = udp_header_ptr + sizeof(L7_udp_header_t);
   relay_agent_header   = (L7_dhcp6_relay_agent_packet_t*) dhcp_header_ptr;

   return ret;
}

/*********************************************************************
* @purpose  Apply security filtering rules to received DHCP packet.
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  L7_TRUE if frame is filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL pppoeFrameFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
                      L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
                      L7_ushort16 innerVlanId, L7_uint *client_idx)      /* PTin modified: DHCP snooping */
{
///* Discard server packets received on untrusted ports */
//if (dsFilterServerMessage(intIfNum, vlanId, frame, ipHeader, innerVlanId, client_idx))    /* PTin modified: DHCP snooping */
//{
//  if (ptin_debug_dhcp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: server filter");
//  return L7_TRUE;
//}
//
///* Discard certain client messages based on rx interface */
//if (dsFilterClientMessage(intIfNum, vlanId, frame, ipHeader, innerVlanId, client_idx))    /* PTin modified: DHCP snooping */
//{
//  if (ptin_debug_dhcp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: client filter");
//   return L7_TRUE;
//}
//
///* Verify that the source MAC matches the client hw address */
//if (dsFilterVerifyMac(intIfNum, vlanId, frame, ipHeader))
//{
//  if (ptin_debug_dhcp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: verify MAC");
//  return L7_TRUE;
//}

  return L7_FALSE;
}
