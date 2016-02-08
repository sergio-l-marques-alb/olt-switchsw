/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping.c
*
* @purpose    Contains definitions to packet processing routines to
*             support multicast snooping 
*             
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#include <unistd.h>

#include "datatypes.h"
#include "l7_product.h"
#include "buff_api.h"
#include "sysnet_api_ipv6.h"
#include "osapi_support.h"
#include "l7utils_inet_addr_api.h"

#include "snooping_defs.h"
#include "snooping_proto.h"
#include "snooping_outcalls.h"
#include "snooping_util.h"
#include "snooping_debug.h"
#include "snooping_api.h"
#include "snooping_db.h"
#include "zlib.h"
#include "ipv6_commdefs.h"
#include "simapi.h"
#include "ptin_translate_api.h"
#include "ptin_snoop_stat_api.h"

#include "ptin/logger.h"
#include "ptin/globaldefs.h"

#define SEM_LOCK(multicast)
#define SEM_UNLOCK(multicast)

static L7_inet_addr_t stored_channel[L7_MAX_GROUP_REGISTRATION_ENTRIES];
static L7_uint16 number_of_channels=0;

static L7_RC_t snoopIGMPFrameBuild2(L7_uint32 intIfNum,
                                    L7_uchar8        main_type,
                                    L7_inet_addr_t  *groupAddr,
                                    L7_uchar8       *buffer,
                                    snoop_cb_t      *pSnoopCB,
                                    L7_uint32        version,
                                    L7_uint32        vlanId,
                                    L7_uint32        number_of_groups,
                                    L7_uint32       *length);

// PTin added
int igmp_runtime_debug = 0;

void IgmpRuntimeDebug(int enable)
{
  igmp_runtime_debug = enable;
  return;
}


/*****************************************************************************
* @purpose  Callback function to Copy and process IGMP and PIM control frames
*
* @param    hookId        @b{(input)} The hook location
* @param    bufHandle     @b{(input)} Handle to the frame to be processed
* @param    pduInfo       @b{(input)} Pointer to info about this frame
* @param    continueFunc  @b{(input)} Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_COPIED   If frame has been copied; 
*                                  continue processing it
* @returns  SYSNET_PDU_RC_IGNORED  If frame has been ignored 
*                                  continue processing it
*
* @notes    none
*
* @end
****************************************************************************/
SYSNET_PDU_RC_t snoopIGMPPktIntercept(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t continueFunc)
{
  L7_uchar8     *data, *protPtr;
  L7_uint16     i;
  L7_uint16     original_vlan=0;
  L7_int        port;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  // Check if client exists
  // innervlan==0, correspond to packets with innervlan null, or without tag. The null client should also be added!
  #if 0
  if (ptin_client_exists(pduInfo->intIfNum,original_vlan,pduInfo->innerVlanId)!=L7_SUCCESS)
  {
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, original_vlan, pduInfo->innerVlanId, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return SYSNET_PDU_RC_IGNORED;
  }
  #endif

  protPtr = (data + sysNetDataOffsetGet(data)) + SNOOP_IP_HDR_NEXT_PROTO_OFFSET;

  // IGMP packets
  if (*protPtr == IP_PROT_IGMP || *protPtr == IP_PROT_PIM)
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d) IGMP Packet intercepted: intIfNum=%u, vlanId=%u, innerVlanId=%u, destIntIfNum=%u, destVlanId=%u\n", __FUNCTION__, __LINE__,
             pduInfo->intIfNum,pduInfo->vlanId,pduInfo->innerVlanId,pduInfo->destIntIfNum,pduInfo->destVlanId);

      printf("%s(%d)\n", __FUNCTION__, __LINE__);
      for (i=0; i<64; i++)
      {
        if (i%0x10==0)  printf("\n0x%04x    ",i);
        printf("%02x ",data[i]);
      }
      printf("\n");
    }
    // Get original vlan for snooping counters
    if (ptin_intif_oldvlan_translate_get(pduInfo->intIfNum, &original_vlan, pduInfo->vlanId)!=L7_SUCCESS ||
        original_vlan<=L7_DOT1Q_MIN_VLAN_ID || original_vlan>L7_DOT1Q_MAX_VLAN_ID)  {
      original_vlan = 0;
    }

    // Validate clientVlan, only for the PON interfaces (this vlan only exists for these interfaces
    if (ptin_translate_intf2port(pduInfo->intIfNum, &port)==L7_SUCCESS && port<L7_SYSTEM_PON_PORTS &&
        !ptin_igmp_stat_client_exists(pduInfo->intIfNum, original_vlan, pduInfo->innerVlanId))
    {
      // Increment number of IGMP dropped packets
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, original_vlan, pduInfo->innerVlanId, SNOOP_STAT_FIELD_IGMP_DROPPED);
      if (igmp_intercept_debug)
      {
        printf("%s(%d) Client %u does not exist\n", __FUNCTION__, __LINE__,pduInfo->innerVlanId);
      }
    }
    else if (snoopPacketHandle(bufHandle, pduInfo, L7_AF_INET) == L7_SUCCESS)
    {
      // Increment Intercepted IGMPs
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, original_vlan, pduInfo->innerVlanId, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);

      return SYSNET_PDU_RC_COPIED;
    }
    else  {
      // Increment received IGMPs with invalid parameters
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, original_vlan, pduInfo->innerVlanId, SNOOP_STAT_FIELD_IGMP_DROPPED);
      if (igmp_intercept_debug)  {
        printf("%s(%d) Error with snoopPacketHandle\n", __FUNCTION__, __LINE__);
      }
    }
  }
  else {
    // Not an IGMP packet
  }

  return SYSNET_PDU_RC_IGNORED;
}

/*********************************************************************
* @purpose  Callback funtion to Copy and process MLD control frames
*
* @param    hookId        @b{(input)} The hook location
* @param    bufHandle     @b{(input)} Handle to the frame to be processed
* @param    pduInfo       @b{(input)} Pointer to info about this frame
* @param    continueFunc  @b{(input)} Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_COPIED   If frame has been copied;
*                                  continue processing it
* @returns  SYSNET_PDU_RC_IGNORED  If frame has been ignored
*                                  continue processing it
*
* @notes    none
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t snoopMLDPktIntercept(L7_uint32 hookId,
                                     L7_netBufHandle bufHandle,
                                     sysnet_pdu_info_t *pduInfo,
                                     L7_FUNCPTR_t continueFunc)
{
  L7_uchar8          *data, *dataPtr;
  L7_uint32           dataOffSet;
  L7_uchar8          *protoPtr;
  L7_ip6ExtHeader_t   ip6ExtHdr;
  L7_uint32           extLength = 0;
  snoop_eb_t         *pSnoopEB;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  dataOffSet = sysNetDataOffsetGet(data);

  protoPtr = data + dataOffSet + SNOOP_IP6_HDR_NEXT_PROTO_OFFSET;
  dataPtr = data + dataOffSet + (sizeof(L7_uchar8) * L7_IP6_HEADER_LEN);
  /* Support for MLD packets with IPv6 options header is not supported
     by all hardwares.
  */
  pSnoopEB = snoopEBGet();
  if (pSnoopEB != L7_NULLPTR && pSnoopEB->ipv6OptionsSupport == L7_TRUE)
  {
    if (*protoPtr == SNOOP_IP6_IPPROTO_HOPOPTS)
    { 
      memcpy(&ip6ExtHdr, dataPtr, sizeof(L7_ip6ExtHeader_t));
      if (ip6ExtHdr.xnext != IP_PROT_ICMPV6)
      {
        return SYSNET_PDU_RC_IGNORED;
      }
    
      extLength = SNOOP_IP6_HOPBHOP_LEN_GET(ip6ExtHdr.xlen);
      dataPtr   = data + dataOffSet + (sizeof(L7_uchar8) * L7_IP6_HEADER_LEN)
                  + extLength;
    }
    else if (*protoPtr != IP_PROT_ICMPV6)
    { 
      return SYSNET_PDU_RC_IGNORED;
    }
  }
  else if (*protoPtr != IP_PROT_ICMPV6)
  {
    return SYSNET_PDU_RC_IGNORED;
  }
  
  if (*dataPtr == L7_MLD_MEMBERSHIP_QUERY || 
      *dataPtr == L7_MLD_V1_MEMBERSHIP_REPORT || 
      *dataPtr == L7_MLD_V1_MEMBERSHIP_DONE ||
      *dataPtr == L7_MLD_V2_MEMBERSHIP_REPORT)
  { 
    if (snoopPacketHandle(bufHandle, pduInfo, L7_AF_INET6) == L7_SUCCESS)
    { 
      return SYSNET_PDU_RC_COPIED;
    }
  } /* End of valid MLD Packet check */  
  return SYSNET_PDU_RC_IGNORED;
}

/***********************************************************************
* @purpose Function to handle incoming snoop control packets
*
* @param   netBufHandle  @b{(input)}  Buffer handle to the snoop control 
*                                     frame received
* @param   pduInfo       @b{(input)}  Pointer to PDU info
* @param   family        @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                     L7_AF_INET6 => MLD Snooping
*
* @returns L7_SUCCESS  Packet copied into local buffer
* @returns L7_FAILURE  Packet ignored
*
* @notes none
*
* @end
*
***********************************************************************/
L7_RC_t snoopPacketHandle(L7_netBufHandle netBufHandle, 
                          sysnet_pdu_info_t *pduInfo, L7_uchar8 family)
{
  snoopPDU_Msg_t   msg;
  L7_uchar8       *data;
  L7_uint32        dataLength;
  L7_uint32        dot1qMode;
  L7_INTF_TYPES_t  sysIntfType;
  L7_RC_t          rc;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  //L7_uint32        mcVlanId;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* If message queue is not yet created, return error. */
  if (family == L7_AF_INET)
  {
    if (!pSnoopCB->snoopExec->snoopIGMPQueue)
    {
      return L7_FAILURE; /* In case of failure, the buffer is freed by DTL */
    }
  }
  else
  {
    if (!pSnoopCB->snoopExec->snoopMLDQueue)
    {
      return L7_FAILURE; /* In case of failure, the buffer is freed by DTL */
    }
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "Packet intercepted vlan %d intIfNum %d",
              pduInfo->vlanId, pduInfo->intIfNum);

  /* Ensure snooping is enabled on the switch */
  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    return L7_FAILURE; /* In case of failure, the buffer is freed by DTL */
  }

  if (nimGetIntfType(pduInfo->intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* If not received on CPU interface, verify the incoming interface */
  if (sysIntfType != L7_CPU_INTF)
  {
    /* Verify that the receiving interface is a member of the receiving VLAN */
    if ( (dot1qOperVlanMemberGet(pduInfo->vlanId, pduInfo->intIfNum, 
                                 &dot1qMode) != L7_SUCCESS) || 
         (dot1qMode != L7_DOT1Q_FIXED) )
    {
      return L7_FAILURE;
    }

    /* Verify that the receiving interface is valid */
    if (snoopIntfCanBeEnabled(pduInfo->intIfNum, pduInfo->vlanId) != L7_TRUE)
    {
      return L7_FAILURE;
    }
  }

  memset((L7_uchar8 *)&msg, 0, sizeof(msg));
  msg.msgId    = snoopPDUMsgRecv;
  msg.intIfNum = pduInfo->intIfNum;
  msg.innerVlanId = pduInfo->innerVlanId;
  msg.vlanId   = /*osapiNtohl already done by hapiRx function*/(pduInfo->vlanId);
  msg.cbHandle = pSnoopCB;

  /* Get start and length of incoming frame */
  SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);
  /* Put a copy of the incoming frame into one of Snooping's buffer pools.
   * This will allow other components to process the original MBUF
   */
  if (dataLength <= SNOOP_SMALL_BUFFER_SIZE)
  {
    rc = bufferPoolAllocate(pSnoopCB->snoopExec->snoopSmallBufferPoolId, 
                            &(msg.snoopBuffer));
    msg.snoopBufferPoolId = pSnoopCB->snoopExec->snoopSmallBufferPoolId;
  }
  else if (dataLength <= SNOOP_MED_BUFFER_SIZE)
  {
    rc = bufferPoolAllocate(pSnoopCB->snoopExec->snoopMedBufferPoolId, 
                            &(msg.snoopBuffer));
    msg.snoopBufferPoolId = pSnoopCB->snoopExec->snoopMedBufferPoolId;
  }
  else
  {
    rc = bufferPoolAllocate(pSnoopCB->snoopExec->snoopLargeBufferPoolId, 
                            &(msg.snoopBuffer));
    msg.snoopBufferPoolId = pSnoopCB->snoopExec->snoopLargeBufferPoolId;
  }

  if (rc != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "Insufficient buffers");
    return L7_FAILURE;
  }

  memcpy(msg.snoopBuffer, data, dataLength);
  msg.dataLength = dataLength;

  if (pSnoopCB->family == L7_AF_INET)
  {
    rc = osapiMessageSend(pSnoopCB->snoopExec->snoopIGMPQueue, 
                          &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT, 
                          L7_MSG_PRIORITY_NORM);
  }
  else
  {
    rc = osapiMessageSend(pSnoopCB->snoopExec->snoopMLDQueue, 
                          &msg, SNOOP_PDU_MSG_SIZE, L7_NO_WAIT, 
                          L7_MSG_PRIORITY_NORM);
  } 

  if (rc != L7_SUCCESS)
  {
    bufferPoolFree(msg.snoopBufferPoolId, msg.snoopBuffer);
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "osapiMessageSend failed\n");
  }
  else
  {
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
       LOG_MSG("snoopPacketHandle: Failed to give msgQueue semaphore\n"); 
    }
  }

  return rc;
}
/*************************************************************************
* @purpose Parse and place the information in a data structure for
*          further processing
*
* @param   msg          @b{(input)}  Pointer to received snoop control 
*                                    message 
* @param   mcastPacket  @b{(output)} Pointer to data structure used 
*                                    internally for processing
*
* @returns L7_SUCCESS  Valid packet 
* @returns L7_FAILURE  Packet to be dropped
*
* @notes none
*
* @end
*
*************************************************************************/
static L7_RC_t snoopPacketParse(snoopPDU_Msg_t *msg, 
                                mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_uint32      intfMode, xtenHdrLen;
  L7_uint32      messageLen, ipHdrLen,  vlanMode;
  L7_INTF_TYPES_t sysIntfType;
  L7_uchar8     *data, byteVal, *startPtr;
  L7_uchar8     *pEtype;
  L7_ushort16    protocolType;
  L7_uchar8     *buffPtr;
  L7_ipHeader_t  ip_header;
  L7_ip6Header_t ip6_header;
  L7_in6_addr_t  ipv6Addr;
  L7_uint32      lenIcmpData;
  static ipv6pkt_t ipv6pkt; /* packet with pseudo header for checksum 
                             calculation */

  /* Determine the protocol type and verify that it is type IP.
  ** Also, strip any VLAN tag.  The tag will be added on egress if necessary.
  */
  data       = msg->snoopBuffer;
  messageLen = msg->dataLength;
  pEtype     = msg->snoopBuffer + L7_ENET_HDR_SIZE;


  /* Copy the dmac of the frame for future validation */
  memcpy(mcastPacket->destMac, msg->snoopBuffer, L7_MAC_ADDR_LEN);

  SNOOP_GET_SHORT(protocolType, pEtype);
  if (protocolType == L7_ETYPE_8021Q)
  {
  #ifndef L7_METRO_FLEX_PACKAGE
    L7_int32 index;

    /* This is a VLAN tagged Frame */
    /* Strip the VLAN header */
    for (index = L7_ENET_HDR_SIZE-1; index >= 0; index--)
    {
      /* Shift the Destination MAC Address and the Source MAC Address by 4 bytes.
      ** These 4 bytes are the VLAN header.
      */
      *(msg->snoopBuffer + index + L7_8021Q_ENCAPS_HDR_SIZE) = *(msg->snoopBuffer + index);
    }
    messageLen -= L7_8021Q_ENCAPS_HDR_SIZE;
    data = msg->snoopBuffer + L7_8021Q_ENCAPS_HDR_SIZE;
    pEtype = data + L7_ENET_HDR_SIZE; /* The Ethernet SA DA fields are moved */
  #else
    /* The VLAN TAG is not stripped in this case */
    pEtype = data + L7_ENET_HDR_SIZE + L7_8021Q_ENCAPS_HDR_SIZE;
  #endif
    SNOOP_GET_SHORT(protocolType, pEtype);
  }
  SNOOP_UNUSED_PARAM(pEtype);
  /* Verify that this is an IPv4/IPv6 frame */
  if (!((protocolType == L7_ETYPE_IP   && msg->cbHandle->family == L7_AF_INET)||
        (protocolType == L7_ETYPE_IPV6 && msg->cbHandle->family == L7_AF_INET6))
      )
  {
    snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_ETYPE);
    return L7_FAILURE;
  }

  memset(mcastPacket->payLoad, 0x00, L7_MAX_FRAME_SIZE * sizeof(L7_uchar8));
  if (messageLen > L7_MAX_FRAME_SIZE)
  {
    messageLen = L7_MAX_FRAME_SIZE;
  }

  memcpy(mcastPacket->payLoad, data, messageLen);
  mcastPacket->length   = messageLen;

  /* now operate only on the local buffer */
  data = mcastPacket->payLoad;

  /* If received on CPU interface, flood it within the VLAN */
  if ( (nimGetIntfType(msg->intIfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_CPU_INTF) )
  {
    snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_RX_CPU);
    snoopPacketFlood(mcastPacket);
    return L7_FAILURE;
  }

  /* Flood the packet if snooping is not enabled in either the VLAN or on the 
     interface */
  if(snoopVlanModeGet(msg->vlanId, &vlanMode, mcastPacket->cbHandle->family) == L7_SUCCESS)
  {
    if (vlanMode != L7_ENABLE)
    {
      if (snoopIntfModeGet(msg->intIfNum, msg->vlanId, &intfMode, 
                           mcastPacket->cbHandle->family) == L7_SUCCESS)
      {
        if (intfMode != L7_ENABLE)
        {
          snoopPacketFlood(mcastPacket);
          return L7_FAILURE;
        }
      }
      else
      {
        return L7_FAILURE;
      } /* End of else of intf mode get */
    } /* End of VLAN Mode check */
  }/* End of VLAN Mode Get */
  else
  {
    snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_VLAN);
    return L7_FAILURE;
  }
  
  /* Point to the start of ethernet payload */
  buffPtr = (L7_uchar8 *)(data + sysNetDataOffsetGet(data));
  /* IGMP */
  if (mcastPacket->cbHandle->family == L7_AF_INET)
  { 
    if (mcastPacket->length < L7_ENET_HDR_SIZE + 
                              L7_ENET_HDR_TYPE_LEN_SIZE + 
                              L7_IP_HDR_LEN + SNOOP_IGMPv1v2_HEADER_LENGTH)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Received pkt is too small %d",\
                  mcastPacket->length);
      return L7_FAILURE;
    }

    startPtr = buffPtr;
    SNOOP_GET_BYTE(ip_header.iph_versLen, buffPtr);
    SNOOP_GET_BYTE(ip_header.iph_tos, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_len, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_ident, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_flags_frag, buffPtr);
    SNOOP_GET_BYTE(ip_header.iph_ttl, buffPtr);
    SNOOP_GET_BYTE(ip_header.iph_prot, buffPtr);
    SNOOP_GET_SHORT(ip_header.iph_csum, buffPtr);
    SNOOP_GET_ADDR(&ip_header.iph_src, buffPtr);   
    SNOOP_GET_ADDR(&ip_header.iph_dst, buffPtr);

    /* Calculate the IP header length, including any IP options */
    ipHdrLen = ((ip_header.iph_versLen & 0x0f) * 4);
    buffPtr += (ipHdrLen - L7_IP_HDR_LEN);
    mcastPacket->ip_payload = buffPtr;

    inetAddressSet(L7_AF_INET, &ip_header.iph_src, &(mcastPacket->srcAddr));
    inetAddressSet(L7_AF_INET, &ip_header.iph_dst, &(mcastPacket->destAddr));

    if ((L7_INET_IS_ADDR_BROADCAST(&(mcastPacket->srcAddr))) ||
        (L7_INET_IS_ADDR_EXPERIMENTAL(&(mcastPacket->srcAddr))) ||
        inetIsInMulticast(&(mcastPacket->srcAddr)) == L7_TRUE)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Invalid src IP address");
      return L7_FAILURE;
    }

    mcastPacket->msgType = ip_header.iph_prot;

    if (mcastPacket->msgType == IP_PROT_IGMP)
    {
      /* Validate IP total length value */
      if ((ip_header.iph_len - ipHdrLen) < SNOOP_IGMPv1v2_HEADER_LENGTH)
      {
         snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IPHDR_LEN);
         return L7_FAILURE;
      }

      /* Verify IGMP checksum */
      if (snoopCheckSum((L7_ushort16 *)buffPtr, ip_header.iph_len - ipHdrLen, 
                        0) != 0)
      {
        snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IGMP_CKSUM);
        return L7_FAILURE;
      }
    }
    mcastPacket->ip_payload_length = ip_header.iph_len - ipHdrLen;

    /* Validate IP header checksum */
    if (snoopCheckSum((L7_ushort16 *)startPtr, ipHdrLen, 0) != 0)
    {
      snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IP_CKSUM);
      return L7_FAILURE;
    }
    
    /* Verify that this is an IP multicast frame */
    /*    snoopValidatePacket(); */
     if ((ip_header.iph_dst & L7_CLASS_D_ADDR_NETWORK) != 
                              L7_CLASS_D_ADDR_NETWORK)
    {
      snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IPV4_DEST);
      return L7_FAILURE;
    }

    /* Should not receive a frame destined to <= 224.0.0.0 */
    if (ip_header.iph_dst <= L7_IP_MCAST_BASE_ADDR)
    {
      snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_IPV4_DEST);
      return L7_FAILURE;
    }   
  }
  else /* MLD */
  {
    L7_uchar8 tempArr[L7_MAC_ADDR_LEN - 2] = { 0, 0, 0, 0};

    /* Check for invalid IPv6 MCAST address */
    if (memcmp(mcastPacket->destMac+2, tempArr, sizeof(tempArr)) == 0)
    {
       SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
       return L7_FAILURE;
    }
  
    if (mcastPacket->length < L7_ENET_HDR_SIZE + 
                              L7_ENET_HDR_TYPE_LEN_SIZE + 
                              L7_IP6_HEADER_LEN + SNOOP_MLDV1_HEADER_LENGTH)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Received pkt is too small %d",\
                  mcastPacket->length);
      return L7_FAILURE;
    }

    SNOOP_GET_LONG(ip6_header.ver_class_flow, buffPtr);
    SNOOP_GET_SHORT(ip6_header.paylen, buffPtr);
    SNOOP_GET_BYTE(ip6_header.next, buffPtr);
    SNOOP_GET_BYTE(ip6_header.hoplim, buffPtr);
    SNOOP_GET_ADDR6(&ipv6Addr, buffPtr);   
    if (!L7_IP6_IS_ADDR_LINK_LOCAL(&ipv6Addr))
    {
      /* Not received with an IPv6 link local address. 
         Discard RFC3810 5.1.14, 5.2.13 */
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, 
                  "Not received with an IPv6 link local address");
      return L7_FAILURE;
    }
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &(mcastPacket->srcAddr));
    SNOOP_GET_ADDR6(&ipv6Addr, buffPtr);
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &(mcastPacket->destAddr));

    xtenHdrLen = 0; 
    if (ip6_header.next == SNOOP_IP6_IPPROTO_HOPOPTS)
    {
      SNOOP_GET_BYTE(ip6_header.next, buffPtr); /* Next Header */
      SNOOP_GET_BYTE(byteVal, buffPtr); /* Xtension hdr length */
      xtenHdrLen = SNOOP_IP6_HOPBHOP_LEN_GET(byteVal);
      buffPtr += xtenHdrLen - 2; 
    }
    
    if (ip6_header.next == IP_PROT_ICMPV6)
    {
      mcastPacket->ip_payload = buffPtr;
      SNOOP_GET_BYTE(mcastPacket->msgType, buffPtr);
      SNOOP_UNUSED_PARAM(buffPtr);
      /* Verify ICMPv6 checksum */
      inetAddressGet(L7_AF_INET6, &(mcastPacket->srcAddr), &(ipv6pkt.srcAddr));
      inetAddressGet(L7_AF_INET6, &(mcastPacket->destAddr), &(ipv6pkt.dstAddr));
      lenIcmpData = ip6_header.paylen - xtenHdrLen;
      /*datalen should be in big endian for snoopcheckSum to succeed*/
      if (snoopGetEndianess() == SNOOP_LITTLE_ENDIAN)
      {
       ipv6pkt.dataLen = osapiHtonl(lenIcmpData);
      }
      else
      {
       ipv6pkt.dataLen = lenIcmpData; 
      } 
 
      ipv6pkt.zero[0] = 0;
      ipv6pkt.zero[1] = 0;
      ipv6pkt.zero[2] = 0;
      ipv6pkt.nxtHdr  = IP_PROT_ICMPV6;

      memcpy(&(ipv6pkt.icmpv6pkt), mcastPacket->ip_payload, lenIcmpData);
      if (snoopCheckSum ((L7_ushort16 *)&ipv6pkt, 
                         (L7_IP6_HEADER_LEN + lenIcmpData), 0)!=0)
      {
        snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_ICMP_CKSUM);
        return L7_FAILURE;
      }
      
      mcastPacket->ip_payload_length = ip6_header.paylen - xtenHdrLen;
    }
    else
    { 
      /* Not an ICMPv6 packet */
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Invalid next header %d",\
                  ip6_header.next);
      return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose Process incoming snoop control packets
*
* @param   msg       @b{(input)}  Pointer to received snoop control 
*                                 message 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopPacketProcess(snoopPDU_Msg_t *msg)
{
  static mgmdSnoopControlPkt_t mcastPacket;  /* A place holder for received 
                                                packet */
  L7_NIM_QUERY_DATA_t          nimQueryData;
  L7_uchar8                   *buffPtr;
  L7_uchar8                    igmpType;
  L7_uint32                    ipv4Addr;
  L7_RC_t                      rc;
  snoopOperData_t             *pSnoopOperEntry;
  L7_inet_addr_t               addr;
  //L7_uint32                    vlanMode, mcVlanId;
  L7_uint16                    original_vlan, switch_vlan;

  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle = msg->cbHandle;
  mcastPacket.intIfNum = msg->intIfNum;
  mcastPacket.vlanId   = msg->vlanId;
  mcastPacket.innerVlanId = msg->innerVlanId;

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Processing IGMP packet with vlan=%u and innervlan=%u, from interface %u",
         msg->vlanId,msg->innerVlanId,msg->intIfNum);

  if (igmp_intercept_debug)
  {
    printf("%s(%d) Processing IGMP packet with vlan=%u and innervlan=%u, from interface %u\n", __FUNCTION__, __LINE__,
           msg->vlanId,msg->innerVlanId,msg->intIfNum);
  }
  // Save this vlan (it could be changed)
  switch_vlan = msg->vlanId;

  /*Check to see whether the interface is still attached or whether it has been
    detached/deleted by the time this packet is up for processing (now)*/
  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;
  nimQueryData.intIfNum = msg->intIfNum;

  // Get original vlan, if there is translation
  if (ptin_intif_oldvlan_translate_get(msg->intIfNum, &original_vlan, msg->vlanId)==L7_SUCCESS &&
      original_vlan>L7_DOT1Q_MIN_VLAN_ID && original_vlan<=L7_DOT1Q_MAX_VLAN_ID)  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d) Incoming vlan=%u, original vlan=%u\n", __FUNCTION__, __LINE__,msg->vlanId,original_vlan);
    }
    msg->vlanId = original_vlan;
    mcastPacket.vlanId = original_vlan;
  }
  else  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d) No vlan translation detected (incoming vlan=%u)\n", __FUNCTION__, __LINE__,msg->vlanId);
    }
  }

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket.cbHandle->family, "Packet copied: OuterVlan %d InnerVlan:%d intIfNum %d",
              mcastPacket.vlanId, mcastPacket.innerVlanId, mcastPacket.intIfNum);

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS ||
      (nimQueryData.data.state != L7_INTF_ATTACHED &&
      nimQueryData.data.state != L7_INTF_ATTACHING))
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    snoopDebugPacketRxTrace(&mcastPacket, SNOOP_PKT_DROP_NOT_READY);
    if (igmp_intercept_debug)
    {
      printf("%s(%d): something went wrong\n", __FUNCTION__, __LINE__);
    }
    // Increment received IGMPs with invalid parameters
    ptin_igmp_stat_increment_field(msg->intIfNum, msg->vlanId, msg->innerVlanId, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_SUCCESS;
  }

  /* Validate incoming VLAN ID */
  if ((pSnoopOperEntry = snoopOperEntryGet(msg->vlanId, msg->cbHandle, L7_MATCH_EXACT)) 
                       == L7_NULLPTR)
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    snoopDebugPacketRxTrace(&mcastPacket, SNOOP_PKT_DROP_BAD_VLAN);
    if (igmp_intercept_debug)  {
      printf("%s(%d): something went wrong\n", __FUNCTION__, __LINE__);
    }
    // Increment received IGMPs with invalid parameters
    ptin_igmp_stat_increment_field(msg->intIfNum, msg->vlanId, msg->innerVlanId, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_SUCCESS;
  }

  /*------------------------------------*/
  /*   Process the buffer               */
  /*------------------------------------*/
  if (snoopPacketParse(msg, &mcastPacket) != L7_SUCCESS)
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    if (igmp_intercept_debug)
    {
      printf("%s(%d): something went wrong\n", __FUNCTION__, __LINE__);
    }
    // Increment received IGMPs with invalid parameters
    ptin_igmp_stat_increment_field(msg->intIfNum, msg->vlanId, msg->innerVlanId, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_SUCCESS;
  }
  /* Free the buffer */
  bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);

  snoopDebugPacketRxTrace(&mcastPacket, 0);
  /* IPv4 Multicast Control Packet */
  if (mcastPacket.cbHandle->family == L7_AF_INET)
  {
    if (mcastPacket.msgType == IP_PROT_IGMP)
    {
      /* Point to the start of IP Payload location in the payload */
      buffPtr = mcastPacket.ip_payload;
      SNOOP_GET_BYTE(igmpType, buffPtr); /* Get the IGMP Type */
      SNOOP_UNUSED_PARAM(buffPtr);
      switch (igmpType)
      {
        case L7_IGMP_MEMBERSHIP_QUERY:
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"L7_IGMP_MEMBERSHIP_QUERY received");

          // Interface should be a router port
          if (!L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, mcastPacket.intIfNum ))
           rc = L7_FAILURE;
          else  {
           // Store querier version for later usage
           rc=snoopMgmdMembershipQueryProcess(&mcastPacket);
          }
          break;

        case L7_IGMP_V1_MEMBERSHIP_REPORT:
        case L7_IGMP_V2_MEMBERSHIP_REPORT:
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"L7_IGMP_Vx_MEMBERSHIP_REPORT received (MAC=%02x:%02x:%02x:%02x:%02x:%02x)",
                    mcastPacket.destMac[0],mcastPacket.destMac[1],mcastPacket.destMac[2],mcastPacket.destMac[3],mcastPacket.destMac[4],mcastPacket.destMac[5]);
          if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, mcastPacket.intIfNum))
          {  
            rc = L7_FAILURE;
            if (igmp_intercept_debug)
            {
              printf("%s(%d): This is a router port... dropping packet\n", __FUNCTION__, __LINE__);
            }
          }
          else  {
            if (igmp_intercept_debug)
            {
              printf("%s(%d): This is a client port... processing packet\n", __FUNCTION__, __LINE__);
            }
            rc=snoopMgmdMembershipReportProcess(&mcastPacket);
          }
          break;

        case L7_IGMP_V3_MEMBERSHIP_REPORT:
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"L7_IGMP_V3_MEMBERSHIP_REPORT received");
          if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, mcastPacket.intIfNum))
            rc = L7_FAILURE;
          else
            rc = snoopMgmdSrcSpecificMembershipReportProcess(&mcastPacket);
          break;

        case L7_IGMP_V2_LEAVE_GROUP:
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"L7_IGMP_V2_LEAVE_GROUP received");
          if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, mcastPacket.intIfNum))
            rc = L7_FAILURE;
          else
            rc = snoopMgmdLeaveGroupProcess(&mcastPacket);
          break;

      case L7_IGMP_DVMRP:
           ipv4Addr = L7_IP_ALL_DVMRP_RTRS_ADDR;
           inetAddressSet(L7_AF_INET, &ipv4Addr , &addr);
           rc = snoopPimv1v2DvmrpProcess(&mcastPacket, &addr, IP_PROT_IGMP);
           break;
      case L7_IGMP_PIM_V1:
           ipv4Addr = L7_IP_ALL_PIM_RTRS_ADDR;
           inetAddressSet(L7_AF_INET, &ipv4Addr , &addr);
           rc = snoopPimv1v2DvmrpProcess(&mcastPacket, &addr, IP_PROT_IGMP);
           break;

        default:
          /* Per the IGMP Snooping draft, an IGMP Snooping switch MUST forward all unrecognized
             IGMP messages... so, flood on all ports in the VLAN except the incoming intf */
           rc = snoopPacketFlood(&mcastPacket);
           break;
      }
    }
    else
    {
      inetAddressGet(L7_AF_INET, &mcastPacket.destAddr, &ipv4Addr);
      /* Non-IGMP/multicast data frame */
      /* Check to see if the dest IP addr falls in the 224.0.0.X range. */
      if ( (ipv4Addr > L7_IP_MCAST_BASE_ADDR) && 
           (ipv4Addr < L7_IP_MAX_LOCAL_MULTICAST) )
      {
          /* If this is a PIMv2 packet, it is handled the same as an IGMP Query barring the 
             query packet parsing.
           * This will flood the packet on all ports */
        if ((mcastPacket.msgType == IP_PROT_PIM) && 
            ipv4Addr == L7_IP_ALL_PIM_RTRS_ADDR)
        {
          inetAddressSet(L7_AF_INET, &ipv4Addr , &addr);
          rc = snoopPimv1v2DvmrpProcess(&mcastPacket, &addr, IP_PROT_PIM);
        }
        else
        {
          /* Multicast data frame in the reserved range */
          rc = snoopPacketFlood(&mcastPacket);
        }
      }
      else
      {
        /* Unknown packet type flood on the VLAN */
        rc = snoopPacketFlood(&mcastPacket);
      }
    } /* end of non igmp packet processing */
  } /* End of IPv4 multicast control packet processing */
  else /* IPv6 Multicast Control Packet */
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d): What am i doing here?\n", __FUNCTION__, __LINE__);
    }
    switch (mcastPacket.msgType)
    {
    case L7_MLD_MEMBERSHIP_QUERY: /* Query */
      rc = snoopMgmdMembershipQueryProcess(&mcastPacket);
      break;

    case L7_MLD_V1_MEMBERSHIP_REPORT: /* Report */
      rc = snoopMgmdMembershipReportProcess(&mcastPacket);
      break;

    case L7_MLD_V2_MEMBERSHIP_REPORT:
      rc = snoopMgmdSrcSpecificMembershipReportProcess(&mcastPacket);
      break;

    case L7_MLD_V1_MEMBERSHIP_DONE: /* Done */
      rc = snoopMgmdLeaveGroupProcess(&mcastPacket);
      break;

    default:
      /* Snooping switch MUST forward all unrecognized
        messages... so, flood on all ports in the VLAN except 
        the incoming intf */
        rc = snoopPacketFlood(&mcastPacket);
        break;
    } /* end of switch case */
  }/* end of IPv6 Multicast Control Packet processing */

  // Update received igmp packet counters
  if (rc==L7_SUCCESS)  {
    ptin_igmp_stat_increment_field(msg->intIfNum, msg->vlanId, msg->innerVlanId, SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID);
  }
  else  {
    ptin_igmp_stat_increment_field(msg->intIfNum, msg->vlanId, msg->innerVlanId, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
  }

  if (igmp_intercept_debug)
  {
    printf("%s(%d): success\n", __FUNCTION__, __LINE__);
  }

  if (igmp_runtime_debug)
  {
    printf("Received and processed an IGMP packet (%u): %u ticks",mcastPacket.msgType,osapiTimeMillisecondsGetOffset(time_run));
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Received and processed an IGMP packet (%u)",mcastPacket.msgType);

  return rc;
}


// PTin added
typedef struct {
  L7_uchar8   type;
  L7_uchar8   responseTime;
  L7_ushort16 chksum;
  L7_uint32   groupIP;
} __attribute__ ((packed)) struct_IGMPv2;

typedef struct {
  L7_uchar8   type;
  L7_uchar8   reserved1;
  L7_ushort16 chksum;
  L7_ushort16 reserved2;
  L7_ushort16 number_of_group_records;
  L7_uchar8   recType;
  L7_uchar8   auxDataLength;
  L7_ushort16 number_of_sources;
  L7_uint32   multicastAddr;
} __attribute__ ((packed)) struct_IGMPv3;

#define IGMP_QUERY  0
#define IGMP_JOIN   1
#define IGMP_LEAVE  2

static L7_RC_t igmp_rebuild_packet(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupAddr, L7_uchar8 type, L7_uint32 number_of_groups)
{
  L7_uchar8       *dataStart;
  L7_uint32        version, frameLength = 0, vlanId;
  L7_RC_t          rc = L7_SUCCESS;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(mcastPacket->cbHandle->family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  osapiSemaTake(pSnoopCB->snoopExec->snoopAvlTreeSema, L7_WAIT_FOREVER);

  vlanId   = mcastPacket->vlanId;

  version = /*(type==SNOOP_IGMP_QUERY) ? igmp_querier_version :*/ pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion;
  if (version == 0 || version > SNOOP_IGMP_VERSION_3)
  {  
    // Check for a valid version
    osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
    return L7_FAILURE;
  }

  // Datastart points to the beginning of data
  dataStart = mcastPacket->payLoad;

  if (pSnoopCB->family != L7_AF_INET)
  {
    osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
    return L7_FAILURE;
  }

  rc = snoopIGMPFrameBuild2(L7_NULL,
                            type,
                            groupAddr,    // Group address for General query
                            dataStart,
                            pSnoopCB,
                            version,
                            vlanId,
                            number_of_groups,
                            &frameLength);

  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
    return L7_FAILURE;
  }

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  snoopPacketFloodInsertInnerTag(vlanId, bufHandle);
#else
  // PTin added
  //mcastPacket->intIfNum  = L7_NULL;
  mcastPacket->length    = frameLength;
#endif

  osapiSemaGive(pSnoopCB->snoopExec->snoopAvlTreeSema);
  return rc;
}


L7_RC_t igmp_generate_packet_and_send(L7_uint32 vlan, L7_uint8 source_type, L7_uint32 groupIP)
{
  L7_inet_addr_t         groupAddr;
  L7_uchar8              type=SNOOP_IGMP_NONE, version;
  L7_RC_t                rc = L7_SUCCESS;
  mgmdSnoopControlPkt_t  mcastPacket;

  // Initialize mcastPacket structure
  memset(&mcastPacket,0x00,sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle    = snoopCBGet(L7_AF_INET);
  mcastPacket.vlanId      = vlan;
  mcastPacket.msgType     = IP_PROT_IGMP;
  mcastPacket.srcAddr.family            = L7_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr  = L7_NULL_IP_ADDR;
  mcastPacket.destAddr.family           = L7_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;

  if (igmp_intercept_debug)
  {
    printf("%s(%d): Responding to router ports\n", __FUNCTION__, __LINE__);
  }

  switch (source_type)
  {
    case L7_IGMP_V1_MEMBERSHIP_REPORT:
      type = SNOOP_IGMP_JOIN;
      version = SNOOP_IGMP_VERSION_1;
      break;

    case L7_IGMP_V2_MEMBERSHIP_REPORT:
      type = SNOOP_IGMP_JOIN;
      version = SNOOP_IGMP_VERSION_2;
      break;

    case L7_IGMP_V2_LEAVE_GROUP:
      type = SNOOP_IGMP_LEAVE;
      version = SNOOP_IGMP_VERSION_2;
      break;

    case L7_IGMP_MEMBERSHIP_QUERY:
      type = SNOOP_IGMP_QUERY;
      version = 0;
      break;

    default:
      rc = L7_FAILURE;
  }

  // Get Group channel
  if (type==SNOOP_IGMP_NONE)
  {
    rc = L7_FAILURE;
  }
  else if (type==SNOOP_IGMP_QUERY)
  {
    groupAddr.family           = L7_AF_INET;
    groupAddr.addr.ipv4.s_addr = groupIP;
    type= SNOOP_IGMP_JOIN;
  }
  else
  {
    groupAddr.family           = L7_AF_INET;
    switch (version)
    {
      case SNOOP_IGMP_VERSION_1:
      case SNOOP_IGMP_VERSION_2:
        groupAddr.addr.ipv4.s_addr = groupIP;
        break;
      default:
        rc = L7_FAILURE;
    }
  }

  if (rc == L7_SUCCESS)
  {
    // Build answer packet
    if ((rc=igmp_rebuild_packet(&mcastPacket, &groupAddr, type, 0))==L7_SUCCESS)
    {
      //mcastPacket->intIfNum=0;

      // Send Packet to mrouter ports
      if ((rc=snoopPacketRtrIntfsForward(&mcastPacket,type))==L7_SUCCESS)
      {
        if (igmp_intercept_debug)
        {
          printf("%s(%d): Transmited %u bytes to Router ports\n", __FUNCTION__, __LINE__,mcastPacket.length);
        }
      }
    }
  }

  return rc;
}


static L7_RC_t igmp_packet_respond(mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_inet_addr_t         groupAddr;
  L7_uchar8              source_type, type=SNOOP_IGMP_NONE, version;
  struct_IGMPv3          *igmp_v3;
  L7_RC_t                rc = L7_SUCCESS;
  L7_uint32              intf, vlan, innervlan;

  intf      = mcastPacket->intIfNum;
  vlan      = mcastPacket->vlanId;
  innervlan = mcastPacket->innerVlanId;

  if (igmp_intercept_debug)
  {
    printf("%s(%d): Responding to router ports\n", __FUNCTION__, __LINE__);
  }

  // get message type
  source_type = mcastPacket->ip_payload[0];

  switch (source_type)
  {
    case L7_IGMP_V1_MEMBERSHIP_REPORT:
      type = SNOOP_IGMP_JOIN;
      version = SNOOP_IGMP_VERSION_1;
      break;

    case L7_IGMP_V2_MEMBERSHIP_REPORT:
      type = SNOOP_IGMP_JOIN;
      version = SNOOP_IGMP_VERSION_2;
      break;

    case L7_IGMP_V3_MEMBERSHIP_REPORT:
      {
        igmp_v3 = (struct_IGMPv3 *) mcastPacket->ip_payload;

        if (igmp_v3->number_of_group_records==0)
        {
          rc = L7_FAILURE;
        }
        else
        {
          if (igmp_v3->recType==SNOOP_IGMP_V3_RECORD_TYPE_INCLUDE)
            type = SNOOP_IGMP_JOIN;
          else if (igmp_v3->recType==SNOOP_IGMP_V3_RECORD_TYPE_EXCLUDE)
            type = SNOOP_IGMP_LEAVE;
          else
            rc = L7_FAILURE;
        }
        version = SNOOP_IGMP_VERSION_3;
      }
      break;

    case L7_IGMP_V2_LEAVE_GROUP:
      type = SNOOP_IGMP_LEAVE;
      version = SNOOP_IGMP_VERSION_2;
      break;

    case L7_IGMP_MEMBERSHIP_QUERY:
      type = SNOOP_IGMP_QUERY;
      version = 0;
      break;

    default:
      rc = L7_FAILURE;
  }

  // Get Group channel
  if (type==SNOOP_IGMP_NONE)
  {
    rc = L7_FAILURE;
  }
  else if (type==SNOOP_IGMP_QUERY)
  {
    groupAddr.family           = L7_AF_INET;
    groupAddr.addr.ipv4.s_addr = ((struct_IGMPv2 *) mcastPacket->ip_payload)->groupIP;
    type= SNOOP_IGMP_JOIN;
  }
  else
  {
    groupAddr.family           = L7_AF_INET;
    switch (version)
    {
      case SNOOP_IGMP_VERSION_1:
      case SNOOP_IGMP_VERSION_2:
        groupAddr.addr.ipv4.s_addr = ((struct_IGMPv2 *) mcastPacket->ip_payload)->groupIP;
        break;
      case SNOOP_IGMP_VERSION_3:
      {
        igmp_v3 = (struct_IGMPv3 *) mcastPacket->ip_payload;
        groupAddr.addr.ipv4.s_addr = igmp_v3->multicastAddr;
      }
      default:
        rc = L7_FAILURE;
    }
  }

  if (rc == L7_SUCCESS)
  {
    // Build answer packet
    if ((rc=igmp_rebuild_packet(mcastPacket, &groupAddr, type, 0))==L7_SUCCESS)
    {
      //mcastPacket->intIfNum=0;

      // Send Packet to mrouter ports
      if ((rc=snoopPacketRtrIntfsForward(mcastPacket,type))==L7_SUCCESS)
      {
        if (igmp_intercept_debug)
        {
          printf("%s(%d): Transmited %u bytes to Router ports\n", __FUNCTION__, __LINE__,mcastPacket->length);
        }
      }
    }
  }

  mcastPacket->intIfNum    = intf;
  mcastPacket->vlanId      = vlan;
  mcastPacket->innerVlanId = innervlan;

  return rc;
}


static L7_RC_t igmp_packet_general_query_respond(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupIP)
{
  L7_uint channel;
  L7_uint igmp_version = mcastPacket->cbHandle->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion;
  L7_uint intf, mc_vlan, vlanId, client_vlan;

  intf        = mcastPacket->intIfNum;
  mc_vlan     = mcastPacket->vlanId;
  client_vlan = mcastPacket->innerVlanId;
  vlanId      = snoop_mcast_vlan_inv[mc_vlan];

  /* Initialize number of channels */
  number_of_channels = 0;

  if (groupIP->family != L7_AF_INET)
    return L7_FAILURE;

  if (groupIP->addr.ipv4.s_addr != L7_NULL) {
    if (igmp_intercept_debug)
      printf("%s(%d): Respostas ao Specific query (0x%08X) (UNI Vlan=%u)\n", __FUNCTION__, __LINE__, groupIP->addr.ipv4.s_addr, vlanId);

    /* If this IP exists, send join */
    if (snoopIpEntryFind(vlanId, groupIP->addr.ipv4.s_addr) != L7_NULLPTR) {  
      if (igmp_intercept_debug)
        printf("%s(%d): Canal 0x%08X existe!\n", __FUNCTION__, __LINE__, groupIP->addr.ipv4.s_addr);

      number_of_channels = 1;
      memcpy(&stored_channel[0], groupIP, sizeof(L7_inet_addr_t) );
    }
    else {
      if (igmp_intercept_debug)
        printf("%s(%d): Canal 0x%08X NAO existe!\n", __FUNCTION__, __LINE__, groupIP->addr.ipv4.s_addr);
    }
  }
  else  {
    SEM_LOCK(mcastPacket);

    if (igmp_intercept_debug)
      printf("%s(%d): MC Vlan=%u UNI Vlan=%u\n", __FUNCTION__, __LINE__, mc_vlan, vlanId);

    /* Note: the vlan that is supplied to the query is the uniVlan and not the MC one!
       The client_vlan must be ignored in this case */
    igmp_get_channels_recursive(NULL, vlanId, -1 /* -1 because the client_vlan must be ignored */,
                                stored_channel, &number_of_channels);

    SEM_UNLOCK(mcastPacket);
  }

  if (igmp_version == SNOOP_IGMP_VERSION_3) {
    /* Prepare packet */
    if (igmp_rebuild_packet(mcastPacket, &stored_channel[0], SNOOP_IGMP_JOIN, number_of_channels) == L7_SUCCESS) {
      //mcastPacket->intIfNum=0;
      /* Send packet to all mrouter ports */
      snoopPacketRtrIntfsForward(mcastPacket, SNOOP_IGMP_JOIN);
    }
  }
  else if (igmp_version == SNOOP_IGMP_VERSION_1 || igmp_version == SNOOP_IGMP_VERSION_2) {
    /* Send channels reports */
    for (channel=0; channel<number_of_channels; channel++) {
      /* Prepare packet */
      if (igmp_rebuild_packet(mcastPacket, &stored_channel[channel], SNOOP_IGMP_JOIN, 1) == L7_SUCCESS) {
        //mcastPacket->intIfNum=0;
        /* Send packet to all mrouter ports */
        snoopPacketRtrIntfsForward(mcastPacket, SNOOP_IGMP_JOIN);
      }
    }
  }

  if (igmp_intercept_debug)
    printf("%s(%d): Respostas ao Query=%u\n", __FUNCTION__, __LINE__, number_of_channels);

  mcastPacket->intIfNum    = intf;
  mcastPacket->vlanId      = mc_vlan;
  mcastPacket->innerVlanId = client_vlan;

  return L7_SUCCESS;
}


// ptin_snooping_activeChannels_get
//  Gets the list of active channels
// 
// Arguments:
//  channels: Pointer to what will be the list of channel addresses
//  nChannels: Pointer to what will be the number of active channels
// Returns:
//  L7_SUCCESS: List has at least one active channels
//  L7_FAILURE: No active channels
//

L7_inet_addr_t active_channels_ip[L7_MAX_GROUP_REGISTRATION_ENTRIES];

L7_RC_t ptin_snooping_activeChannels_get(L7_uint16 vlanId, L7_uint16 client_vlanId, L7_inet_addr_t **channels, L7_uint16 *nChannels)
{
  L7_uint16 n=0;

  if (channels==L7_NULLPTR || nChannels==L7_NULLPTR)
    return L7_FAILURE;

  SEM_LOCK(mcastPacket);
  igmp_get_channels_recursive(NULL, vlanId, client_vlanId, active_channels_ip, &n);
  SEM_UNLOCK(mcastPacket);

  *channels  = active_channels_ip;
  *nChannels = n;

  return L7_SUCCESS;
}


L7_RC_t ptin_snooping_clientsList_get(L7_uint32 channelIP, L7_uint16 sVlan, L7_uint32 *client_list_bmp, L7_uint16 *number_of_clients)
{
  snoopIpInfoData_t *ipEntry;

  if (channelIP==L7_NULL || sVlan<2 || sVlan>4093 || client_list_bmp==L7_NULLPTR || number_of_clients==L7_NULLPTR)
    return L7_FAILURE;

  // Search for channel
  if ((ipEntry=snoopIpEntryFind(sVlan,channelIP))==L7_NULLPTR)
    return L7_FAILURE;

  // Copy clients bitmap
  memcpy(client_list_bmp,ipEntry->clientList.clientVlan,sizeof(L7_uint32)*4096/(sizeof(L7_uint32)*8));
  *number_of_clients = ipEntry->clientList.number_of_clients;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Process IGMP/MLD Membership query message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold 
*                                   received packet 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdMembershipQueryProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  snoop_cb_t     *pSnoopCB = L7_NULLPTR;
  L7_RC_t         rc = L7_SUCCESS;
  L7_uchar8      *dataPtr = L7_NULL;
  L7_uint32       ipv4Addr;
  L7_in6_addr_t   ipv6Addr;
  L7_mgmdMsg_t    mgmdMsg;
  L7_uchar8       byteVal;
  L7_uchar8       dmac[L7_MAC_ADDR_LEN];
  snoopInfoData_t *entry;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;
  /* If the src IP is non-zero, add this interface to the multicast router list 
     and to all snooping entries */

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Max response time */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (mcastPacket->cbHandle->family == L7_AF_INET6)
  {
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
    {
      L7_uchar8 mldQryAddr[L7_IP6_ADDR_LEN];
      L7_uchar8 ipBuf[L7_IP6_ADDR_LEN];

      memset(mldQryAddr, 0x00, L7_IP6_ADDR_LEN);
      osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, mldQryAddr);

       /* Check if it is equal to the all hosts address FF02::1 */
      inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
      if (memcmp(ipBuf, mldQryAddr, L7_IP6_ADDR_LEN) != 0)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
          return L7_FAILURE;
        }
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  {
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
    {
      if (igmp_intercept_debug)
      {
        printf("%s(%d): general query received\n", __FUNCTION__, __LINE__);
      }

      /* Check if IPv4 destination address is same as 224.0.0.1 */
      inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr, 
                     &ipv4Addr);
      if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR) 
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }

      // Increment general queries reception
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->innerVlanId, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
          return L7_FAILURE;
        }

        // PTin added - Answer to a specific group query
        if (igmp_intercept_debug)
        {
          printf("%s(%d): group query received\n", __FUNCTION__, __LINE__);
        }

        memset(dmac, 0x00, sizeof(dmac));
        /* Validate the group address being reported */
        snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
        if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
        {
          if (igmp_intercept_debug)
            printf("%s(%d): Invalid Packet\n", __FUNCTION__, __LINE__);
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
          return L7_FAILURE;
        }

        // Increment Specific queries recption!
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->innerVlanId, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);

        SEM_LOCK(mcastPacket);
        entry=snoopEntryFind(dmac, snoop_mcast_vlan_inv[mcastPacket->vlanId], mcastPacket->cbHandle->family,L7_MATCH_EXACT);
        SEM_UNLOCK(mcastPacket);

        if (entry == L7_NULLPTR)  {
          if (igmp_intercept_debug)
            printf("%s(%d): This MFDB entry does not exist! (dmac=%02X:%02X:%02X:%02X:%02X:%02X, vid=%u (%u), family=%u)\n", __FUNCTION__, __LINE__,
                   dmac[0],dmac[1],dmac[2],dmac[3],dmac[4],dmac[5],snoop_mcast_vlan_inv[mcastPacket->vlanId],mcastPacket->vlanId,mcastPacket->cbHandle->family);
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
          return L7_SUCCESS;
        }
      } 
    }
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    if (igmp_intercept_debug)
      printf("%s(%d): Invalid Packet\n", __FUNCTION__, __LINE__);
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }
  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    if (igmp_intercept_debug)
      printf("%s(%d): Invalid Packet\n", __FUNCTION__, __LINE__);
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

//if (inetIsAddressZero(&mcastPacket->srcAddr) != L7_TRUE)
//{
//  if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
//  {
//    snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum,
//                         SNOOP_DYNAMIC, mcastPacket->cbHandle);
//  }
//}
  /* Forward membership query on all interfaces on this VLAN, except the
     incoming interface */
  if (rc == L7_SUCCESS)
  {
    if (mcastPacket->cbHandle->family==L7_AF_INET )
    {
      igmp_packet_general_query_respond(mcastPacket, &mgmdMsg.mgmdGroupAddr);
    }
    else  {
      snoopPacketFlood(mcastPacket);
    }
  }

  pSnoopCB->counters.controlFramesProcessed++;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Process IGMPv1/IGMPv2/MLDv1/MLDv2 Group Membership Reports
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold 
*                                   received packet 
*
* @returns L7_SUCCESS
* @returns L7_FAILURE  Invalid packet
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdMembershipReportProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_uchar8       *dataPtr;
  L7_BOOL          fwdFlag = L7_TRUE, fwdFlag2=L7_FALSE;
  L7_mgmdMsg_t     mgmdMsg;
  L7_uchar8        dmac[L7_MAC_ADDR_LEN], byteVal;
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_in6_addr_t    ipv6Addr;
  snoopInfoData_t *entry;
  snoopIpInfoData_t *ipentry;

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    if (inetIsAddressZero(&mcastPacket->srcAddr) == L7_TRUE)
    {
      if (igmp_intercept_debug)
      {
        printf("%s(%d): srcAddr is NULL\n", __FUNCTION__, __LINE__);
      }
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
      return L7_FAILURE;
    }

    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Max response time */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d): Invalid packet\n", __FUNCTION__, __LINE__);
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  #ifdef IGMP_WHITELIST_FEATURE
  /* Is this group Address valid? (Use Multicast vlan) */
  if (!(igmp_wlist_channel_exists(mcastPacket->intIfNum, snoop_mcast_vlan[mcastPacket->vlanId], &mgmdMsg.mgmdGroupAddr)))
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d): GroupAddr %03x.%03x.%03x.%03x is not in white list (intIfNum=%u vlan=%u)\n", __FUNCTION__, __LINE__,
             (mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr>>24) & 0xff,
             (mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr>>16) & 0xff,
             (mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr>> 8) & 0xff,
             (mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr) & 0xff,
             mcastPacket->intIfNum, mcastPacket->vlanId );
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }
  #endif

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS) 
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d): Invalid packet\n", __FUNCTION__, __LINE__);
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d): Invalid packet\n", __FUNCTION__, __LINE__);
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;
  
  /* Check to see if the group address is in the 224.0.0.X range.  If it is, don't
     process it... just forward to all ports in the vlan except incoming, otherwise
     it to the table */
  fwdFlag = inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr);

  if (igmp_intercept_debug)
  {
    printf("%s(%d): Group Address is in the range 224.0.0.X? %d\n", __FUNCTION__, __LINE__,fwdFlag);
  }

  if (fwdFlag == L7_FALSE)
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d): fwdFlag is FALSE\n", __FUNCTION__, __LINE__);
    }

    SEM_LOCK(mcastPacket);
    /* Does an entry with the same MAC addr and VLAN ID already exist? */
    entry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,L7_MATCH_EXACT);
    if (entry == L7_NULLPTR)
    {
      if (igmp_intercept_debug)
      {
        printf("%s(%d): First client\n", __FUNCTION__, __LINE__);
      }

      /* Entry does not exist... give back the semaphore and create new entry */
      rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family, L7_FALSE);
      if (rc == L7_SUCCESS)
      {
        if (igmp_intercept_debug)
        {
          printf("%s(%d): Channel added\n", __FUNCTION__, __LINE__);
        }
        //fwdFlag2 = L7_TRUE;

        // PTin added
        entry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,L7_MATCH_EXACT);
        if (entry!=L7_NULLPTR) {
          memset(&entry->ipList,0x00,sizeof(st_ptin_ip_list_t));

          /* Add all multicast router interfaces to this entry */
          snoopAllMcastRtrIntfsAdd(dmac, mcastPacket->vlanId, 
                                   mcastPacket->intIfNum, mcastPacket->cbHandle);
        }
        else  rc = L7_FAILURE;
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopEntryCreate failed. Table full\n"); 
      }
    }

    /* If the entry already existed or it was just successfully created, add the interface */
    if (rc == L7_SUCCESS)
    {
      if (entry!=L7_NULLPTR) {
        if (igmp_intercept_debug)
        {
          printf("%s(%d): Setting IP 0x%08X\n", __FUNCTION__, __LINE__,ipv4Addr);
        }

        // Register this IP to this entry
        snoopIpAdd(entry, ipv4Addr);

        // Check if this IP exists in AvlTree
        if ((ipentry=snoopIpEntryFind(mcastPacket->vlanId,ipv4Addr))==L7_NULLPTR)  {
          if (igmp_intercept_debug)
          {
            printf("%s(%d): IP 0x%08X does not exist in IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
          }
          // Don't exist: add it
          if ((rc=snoopIpEntryAdd(mcastPacket->vlanId,ipv4Addr,L7_FALSE))==L7_SUCCESS)
          {
            if (igmp_intercept_debug)
            {
              printf("%s(%d): IP 0x%08X added to IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
            }
            if ((ipentry=snoopIpEntryFind(mcastPacket->vlanId,ipv4Addr))==L7_NULLPTR)  {
              if (igmp_intercept_debug)
              {
                printf("%s(%d): Strange... IP 0x%08X still does not exist in IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
              }
              rc = L7_FAILURE;
            }
            else  {
              if (igmp_intercept_debug)
              {
                printf("%s(%d): This packet will be forwarded\n", __FUNCTION__, __LINE__);
              }
              fwdFlag2 = L7_TRUE;
            }
          }
        }

        // Add client to this ipEntry
        if (ipentry!=L7_NULLPTR)
        {
          if (igmp_intercept_debug)
          {
            printf("%s(%d): Adding client %u to IP 0x%08X\n", __FUNCTION__, __LINE__,mcastPacket->innerVlanId,ipv4Addr);
          }
          snoopClientVlanAdd(ipentry, mcastPacket->innerVlanId, mcastPacket->intIfNum);
        }
      }

      // Only add interface if entry is dynamic
      if (!entry->staticEntry)
      {
        rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum, 
                          SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
        if (rc != L7_SUCCESS)
        {
          LOG_MSG("snoopMgmdMembershipReportProcess: snoopIntfAdd failed\n"); 
        }
      }
    }

    SEM_UNLOCK(mcastPacket);

    /* Forward membership report to multicast routers we've detected */
    if (fwdFlag2)  {
      rc = igmp_packet_respond(mcastPacket);
    }
//      snoopPacketRtrIntfsForward(mcastPacket);
  }
  else
  {
    /* Flood it in the VLAN as it is for link local multicast address */
    snoopPacketFlood(mcastPacket);
  }

  if (rc==L7_SUCCESS)  {
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->innerVlanId, SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS);
  }


  pSnoopCB->counters.controlFramesProcessed++;

  return rc;
}

/****************************************************************************
* @purpose Process IGMPv3/MLDv2 Group Membership Report
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, if invalid group address
*
* @notes none
*
* @end
*
*****************************************************************************/
L7_RC_t snoopMgmdSrcSpecificMembershipReportProcess(mgmdSnoopControlPkt_t 
                                                    *mcastPacket)
{
  L7_uchar8               dmac[L7_MAC_ADDR_LEN], *dataPtr, recType, auxDataLen;
  L7_RC_t                 rc = L7_SUCCESS;
  snoopInfoData_t        *snoopEntry;
  L7_uint32               ipv4Addr, group_ipv4Addr, timerValue, intfFastLeaveMode;
  L7_BOOL   fastLeaveMode;
  L7_ushort16             noOfGroups, noOfSources;
  L7_inet_addr_t          groupAddr;
  L7_BOOL                 fwdFlag = L7_FALSE, floodFlag = L7_TRUE, fwdFlag2=L7_FALSE;
  snoop_cb_t             *pSnoopCB = L7_NULLPTR;
  L7_uchar8               ipBuf[16];
  snoopInfoData_t        *entry;
  snoopIpInfoData_t      *ipentry;

  /* Set pointer to IGMPv3 Membership Report */
  dataPtr = mcastPacket->ip_payload + MGMD_REPORT_NUM_SOURCES_OFFSET;
  SNOOP_GET_SHORT(noOfGroups, dataPtr);
  /* Data ptr will point to the start if group records */
  
  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  if (mcastPacket->cbHandle->family == L7_AF_INET)
  {
    if (inetIsAddressZero(&mcastPacket->srcAddr) == L7_TRUE)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
      return L7_FAILURE;
    }

    /* IGMP */
    inetAddressGet(L7_AF_INET, &mcastPacket->destAddr, &group_ipv4Addr);
    if (group_ipv4Addr != L7_IP_IGMPV3_REPORT_ADDR)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Invalid destination IPv4 address\n");
      return L7_FAILURE;
    }
  }
  else
  {
    L7_uchar8 mldv3ReportAddr[L7_IP6_ADDR_LEN];

    memset(mldv3ReportAddr, 0x00, L7_IP6_ADDR_LEN);
    osapiInetPton(L7_AF_INET6, SNOOP_IP6_MLDV2_REPORT_ADDR, mldv3ReportAddr);
    
     /* MLD */
    inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
    if (memcmp(ipBuf, mldv3ReportAddr, L7_IP6_ADDR_LEN) != 0)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Invalid destination IPv6 address\n");
      return L7_FAILURE;
    }
  }

  /* Validate the dmac and the destination Ipv4 address */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  while (noOfGroups > 0)
  { 
   /* IGMP Membership records */
    SNOOP_GET_BYTE(recType, dataPtr); /* Record type */
    SNOOP_GET_BYTE(auxDataLen, dataPtr); /* AuxData Len */
    SNOOP_GET_SHORT(noOfSources, dataPtr); /* Number of sources */
    if (mcastPacket->cbHandle->family == L7_AF_INET)
    {
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);
      inetAddressSet(L7_AF_INET, &ipv4Addr, &groupAddr);
    }
    else
    {
      /* IPv6 MCAST Address */ 
      SNOOP_GET_ADDR6(ipBuf, dataPtr);
      inetAddressSet(L7_AF_INET6, ipBuf, &groupAddr);
    }              
    
    if (inetIsInMulticast(&groupAddr) == L7_TRUE)
    {
      memset(dmac, 0x00, sizeof(dmac));
      /* Validate the group address being reported */
      snoopMulticastMacFromIpAddr(&groupAddr, dmac);
      if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "Invalid destination multicast mac\n");
        return L7_FAILURE;
      }
      fwdFlag = inetIsLinkLocalMulticastAddress(&groupAddr);
    }
    else
    {
      fwdFlag = L7_TRUE;
    }

    SEM_LOCK(mcastPacket);

    if (fwdFlag == L7_FALSE)
    {
      floodFlag = L7_FALSE;

      if ((noOfSources == 0) && (recType == L7_IGMP_CHANGE_TO_INCLUDE_MODE))
      {
        /* Treat TO_IN( {} ) like an IGMPv2 Leave / MLDv1 Done*/

        /* Does an entry with the same MAC addr and VLAN ID already exist? */
        snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId,
                                    mcastPacket->cbHandle->family, L7_MATCH_EXACT);
        if (snoopEntry)
        {
          if (snoopVlanFastLeaveModeGet(mcastPacket->vlanId, &fastLeaveMode, 
                                          mcastPacket->cbHandle->family) != L7_SUCCESS)
          {
            fastLeaveMode = L7_DISABLE;
          }

          if (snoopIntfFastLeaveAdminModeGet(mcastPacket->intIfNum, &intfFastLeaveMode, 
                                             mcastPacket->cbHandle->family) != L7_SUCCESS)
          {
            intfFastLeaveMode = L7_DISABLE;
          }

          /* check whether the interface or the VLAN has fast-leave admin mode enabled */
          if ((intfFastLeaveMode == L7_ENABLE) ||(fastLeaveMode == L7_ENABLE))
          {
            if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, 
                                     mcastPacket->intIfNum))
            {
              // if this IP exists in AvlTree
              if ((ipentry=snoopIpEntryFind(mcastPacket->vlanId,ipv4Addr))!=L7_NULLPTR)  {

                if (igmp_intercept_debug)
                {
                  printf("%s(%d): IP 0x%08X exists in IP tree... going to remove client %u\n", __FUNCTION__, __LINE__,ipv4Addr,mcastPacket->innerVlanId);
                }

                // Remove client
                snoopClientVlanRemove(ipentry, mcastPacket->innerVlanId, mcastPacket->intIfNum);

                // If there is no clients associated to this ip, remove the IP
                // Only do this to dynamic entries
                if (!ipentry->staticEntry && snoopNoClientVlans(ipentry,L7_ALL_INTERFACES))
                {
                  if (igmp_intercept_debug)
                  {
                    printf("%s(%d): No clients in IP 0x%08X... going to remove ip entry and ip in snoop entry... packet will be forwarded\n", __FUNCTION__, __LINE__,ipv4Addr);
                  }

                  fwdFlag2 = L7_TRUE;
                  // Remove the IP entry from the IP tree
                  if (snoopIpEntryDelete(mcastPacket->vlanId,ipv4Addr)!=L7_SUCCESS)  {
                    if (igmp_intercept_debug)
                    {
                      printf("%s(%d): Error removing ip entry\n", __FUNCTION__, __LINE__);
                    }
                  }
                  else  {
                    if (igmp_intercept_debug)
                    {
                      printf("%s(%d): ip entry removed\n", __FUNCTION__, __LINE__);
                    }
                  }
                  // Clear IP bit in snoop tree
                  snoopIpRemove(snoopEntry,ipv4Addr);

                  if (igmp_intercept_debug)
                  {
                    printf("%s(%d): ip cleared in snoop entry\n", __FUNCTION__, __LINE__);
                  }

                  // Only remove interface from snoopEntry, if there is no IPs associated to it
                  if (snoopNoIPs(snoopEntry))
                  {
                    // Remove the interfave from snoop tree
                    if (snoopEntryRemove(dmac,mcastPacket->vlanId,mcastPacket->cbHandle->family)!=L7_SUCCESS)
//                  if (snoopIntfRemove(dmac, mcastPacket->vlanId,
//                                      mcastPacket->intIfNum,SNOOP_GROUP_MEMBERSHIP,
//                                      mcastPacket->cbHandle) != L7_SUCCESS)
                    {
                      LOG_MSG("snoopMgmdV3MembershipReportProcess: Failed to remove group membership \n");
                    }
                    if (igmp_intercept_debug)
                    {
                      printf("%s(%d): interface removed\n", __FUNCTION__, __LINE__);
                    }
                  }

                  // Only remove interface from snoopEntry, if there is no clients associated to this interface
                }
              }
              else  {
                if (igmp_intercept_debug)
                {
                  printf("%s(%d): IP 0x%08X does not exist in IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
                }
              }
            }
          }
          else
          { 
            timerValue = snoopCheckPrecedenceParamGet(mcastPacket->vlanId,mcastPacket->intIfNum,
                                                      SNOOP_PARAM_MAX_RESPONSE_TIME,
                                                      snoopEntry->snoopInfoDataKey.family);
            if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, 
                                     mcastPacket->intIfNum))
            {
              snoopTimerUpdate(snoopEntry, mcastPacket->intIfNum, 
                               mcastPacket->vlanId,
                               SNOOP_MAX_RESPONSE, timerValue, 
                               mcastPacket->cbHandle);
            } /* End of group membership check */
            /* Entry exists... give back semaphore, set timers and transmit query */
          }
        }
      }
      else
      {
        /* Does an entry with the same MAC addr and VLAN ID already exist? */
        entry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family, L7_MATCH_EXACT);
        if (entry == L7_NULLPTR)
        {
          /* Entry does not exist... give back the semaphore and create new entry */
          rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family, L7_FALSE);
          if (rc == L7_SUCCESS)
          {
            //fwdFlag2 = L7_TRUE;
            // PTin added
            entry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,L7_MATCH_EXACT);
            if (entry!=L7_NULLPTR) {
              memset(&entry->ipList,0x00,sizeof(st_ptin_ip_list_t));

              /* Add all multicast router interfaces to this entry */
              snoopAllMcastRtrIntfsAdd(dmac, mcastPacket->vlanId, 
                                     mcastPacket->intIfNum, 
                                     mcastPacket->cbHandle);
            }
            else  rc = L7_FAILURE;
          }
          else
          {
            SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopEntryCreate failed. Table full\n");  
          }
        }

        /* If the entry already existed or it was just successfully created, add the interface */
        if (rc == L7_SUCCESS)
        {
          // PTin added
          if (entry!=L7_NULLPTR) {
            if (igmp_intercept_debug)
            {
              printf("%s(%d): Setting IP 0x%08X\n", __FUNCTION__, __LINE__,ipv4Addr);
            }

            // Register this IP to this entry
            snoopIpAdd(entry, ipv4Addr);

            // Check if this IP exists in AvlTree
            if ((ipentry=snoopIpEntryFind(mcastPacket->vlanId,ipv4Addr))==L7_NULLPTR)  {
              if (igmp_intercept_debug)
              {
                printf("%s(%d): IP 0x%08X does not exist in IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
              }
              // Don't exist: add it
              if ((rc=snoopIpEntryAdd(mcastPacket->vlanId,ipv4Addr,L7_FALSE))==L7_SUCCESS)
              {
                if (igmp_intercept_debug)
                {
                  printf("%s(%d): IP 0x%08X added to IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
                }
                if ((ipentry=snoopIpEntryFind(mcastPacket->vlanId,ipv4Addr))==L7_NULLPTR)  {
                  if (igmp_intercept_debug)
                  {
                    printf("%s(%d): Strange... IP 0x%08X still does not exist in IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
                  }
                  rc = L7_FAILURE;
                }
                else  {
                  if (igmp_intercept_debug)
                  {
                    printf("%s(%d): This packet will be forwarded\n", __FUNCTION__, __LINE__);
                  }
                  fwdFlag2 = L7_TRUE;
                }
              }
            }

            // Add client to this ipEntry
            if (ipentry!=L7_NULLPTR)
            {
              if (igmp_intercept_debug)
              {
                printf("%s(%d): Adding client %u to IP 0x%08X\n", __FUNCTION__, __LINE__,mcastPacket->innerVlanId,ipv4Addr);
              }
              snoopClientVlanAdd(ipentry, mcastPacket->innerVlanId, mcastPacket->intIfNum);
            }
          }
          // Only add interface if entry is dynamic
          if (!entry->staticEntry)
          {
            rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum, 
                              SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
          }
        }
      }/* End of record type check */
    } /* end of if process packet */
    SEM_UNLOCK(mcastPacket);

    /* Point to the next record */
    /* RFC 3376 4.2.6, RFC 3810 5.2.6 */
    if (mcastPacket->cbHandle->family == L7_AF_INET)
    {
      dataPtr += ((auxDataLen * 4) + (noOfSources * sizeof(L7_in_addr_t)));
    }
    else
    {
      dataPtr += ((auxDataLen * 4) + (noOfSources * sizeof(L7_in6_addr_t)));
    }

    /* Check if it is a malformed packet */
    if ((dataPtr - mcastPacket->ip_payload) > mcastPacket->ip_payload_length)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Dropped malformed packet\n");
      return L7_FAILURE;
    }

    noOfGroups -= 1;
  } /* end of while loop */

  /* Forward membership report to multicast routers we've detected */
  if (floodFlag == L7_FALSE)
  {
    if ( fwdFlag2 )  {
      rc = igmp_packet_respond(mcastPacket);
    }
    //snoopPacketRtrIntfsForward(mcastPacket);
  }
  else
  {
   /* Flood it in the VLAN as it is for link local multicast address */
    snoopPacketFlood(mcastPacket);
  }

  if (rc==L7_SUCCESS)  {
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->innerVlanId, SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3);
  }

  pSnoopCB->counters.controlFramesProcessed++;
  return rc;
}
/*********************************************************************
* @purpose Process IGMPv2 Leave/ MLDv1 Done Group message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t snoopMgmdLeaveGroupProcess(mgmdSnoopControlPkt_t *mcastPacket)
{ 
  L7_uchar8        dmac[L7_MAC_ADDR_LEN], *dataPtr, byteVal;
  L7_uchar8        ipBuf[L7_IP6_ADDR_LEN];
  snoopInfoData_t *snoopEntry;
  snoopIpInfoData_t *ipentry;
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr, group_ipv4Addr;
  L7_BOOL  fastLeaveMode;
  L7_uint32        timerValue, intfFastLeaveMode;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_mgmdMsg_t     mgmdMsg;
  L7_in6_addr_t    ipv6Addr;
  L7_BOOL          fwdFlag = L7_TRUE, fwdFlag2 = L7_FALSE;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  if (igmp_intercept_debug)
  {
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
  }

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    if (inetIsAddressZero(&mcastPacket->srcAddr) == L7_TRUE)
    {
      if (igmp_intercept_debug)
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
      return L7_FAILURE;
    }

    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);
  }
  else /* MLD Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);                  /* Code */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(mgmdMsg.mldMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }

  if (pSnoopCB->family == L7_AF_INET)
  {
    /* Check if IPv4 destination address is same as 224.0.0.2 */
    inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                   &group_ipv4Addr);
    if (group_ipv4Addr != L7_IP_ALL_RTRS_ADDR)
    {
      if (igmp_intercept_debug)
        printf("%s(%d)\n", __FUNCTION__, __LINE__);

      if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
      {
        if (igmp_intercept_debug)
          printf("%s(%d)\n", __FUNCTION__, __LINE__);

        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else 
  {
    L7_uchar8 mldv3ReportAddr[L7_IP6_ADDR_LEN];

    memset(mldv3ReportAddr, 0x00, L7_IP6_ADDR_LEN);
    osapiInetPton(L7_AF_INET6, SNOOP_IP6_ALL_ROUTERS_ADDR, mldv3ReportAddr);

     /* MLD */
    inetAddressGet(L7_AF_INET6, &mcastPacket->destAddr, ipBuf);
    if (memcmp(ipBuf, mldv3ReportAddr, L7_IP6_ADDR_LEN) != 0)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Invalid destination IPv6 address\n");
      return L7_FAILURE;
    }
  }

  if (igmp_intercept_debug)
    printf("%s(%d)\n", __FUNCTION__, __LINE__);

  /* check if it is a leave for a valid mcast group address */
  if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_FALSE)
  {
    if (igmp_intercept_debug)
      printf("%s(%d)\n", __FUNCTION__, __LINE__);
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* check if frame dmac is same as IP Dest Addr equivalent dmac */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    if (igmp_intercept_debug)
      printf("%s(%d)\n", __FUNCTION__, __LINE__);
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    if (igmp_intercept_debug)
      printf("%s(%d)\n", __FUNCTION__, __LINE__);

    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  fwdFlag = inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr);

  if (igmp_intercept_debug)
    printf("%s(%d)\n", __FUNCTION__, __LINE__);

  if (fwdFlag == L7_FALSE)
  {
    if (igmp_intercept_debug)
    {
      printf("%s(%d) dmac=%02X:%02X:%02X:%02X:%02X:%02X, vlanId=%u, family=%u, cbHandle=%u\n", __FUNCTION__, __LINE__,
             dmac[0],dmac[1],dmac[2],dmac[3],dmac[4],dmac[5],mcastPacket->vlanId,mcastPacket->cbHandle->family,(L7_uint32) mcastPacket->cbHandle);
    }

    SEM_LOCK(mcastPacket);

    /* Does an entry with the same MAC addr and VLAN ID already exist? */
    snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId, 
                                mcastPacket->cbHandle->family, L7_MATCH_EXACT);
    if (snoopEntry)
    {
      if (igmp_intercept_debug)
        printf("%s(%d)\n", __FUNCTION__, __LINE__);

      if (snoopVlanFastLeaveModeGet(mcastPacket->vlanId, &fastLeaveMode, 
                                    mcastPacket->cbHandle->family) != L7_SUCCESS)
      {
        if (igmp_intercept_debug)
          printf("%s(%d)\n", __FUNCTION__, __LINE__);

        fastLeaveMode = L7_DISABLE;
      }

      if (snoopIntfFastLeaveAdminModeGet(mcastPacket->intIfNum, &intfFastLeaveMode,
                                         mcastPacket->cbHandle->family) != L7_SUCCESS)
      {
        if (igmp_intercept_debug)
          printf("%s(%d)\n", __FUNCTION__, __LINE__);

        intfFastLeaveMode = L7_DISABLE;
      }

      /* check whether the interface or the VLAN has 
       * fast-leave admin mode enabled 
       */
      if ((intfFastLeaveMode == L7_ENABLE) ||(fastLeaveMode == L7_ENABLE))
      {
        if (igmp_intercept_debug)
          printf("%s(%d)\n", __FUNCTION__, __LINE__);

        if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, 
                                 mcastPacket->intIfNum))
        {
          if (igmp_intercept_debug)
            printf("%s(%d)\n", __FUNCTION__, __LINE__);

          // if this IP exists in AvlTree
          if ((ipentry=snoopIpEntryFind(mcastPacket->vlanId,ipv4Addr))!=L7_NULLPTR)  {

            if (igmp_intercept_debug)
              printf("%s(%d): IP 0x%08X exists in IP tree... going to remove client %u\n", __FUNCTION__, __LINE__,ipv4Addr,mcastPacket->innerVlanId);

            // Remove client
            snoopClientVlanRemove(ipentry, mcastPacket->innerVlanId, mcastPacket->intIfNum);

            // If there is no clients associated to this ip...
            if (!ipentry->staticEntry && snoopNoClientVlans(ipentry,L7_ALL_INTERFACES))
            {
              if (igmp_intercept_debug)
                printf("%s(%d): No clients in IP 0x%08X... going to remove ip entry and ip in snoop entry... packet will be forwarded\n", __FUNCTION__, __LINE__,ipv4Addr);

              fwdFlag2 = L7_TRUE;
              // Remove the IP entry from the IP tree
              if (snoopIpEntryDelete(mcastPacket->vlanId,ipv4Addr)!=L7_SUCCESS)  {
                if (igmp_intercept_debug)
                  printf("%s(%d): Error removing ip entry\n", __FUNCTION__, __LINE__);
              }
              else  {
                if (igmp_intercept_debug)
                  printf("%s(%d): ip entry removed\n", __FUNCTION__, __LINE__);
              }
              // Clear IP bit in snoop tree
              snoopIpRemove(snoopEntry,ipv4Addr);

              if (igmp_intercept_debug)
                printf("%s(%d): ip cleared in snoop entry\n", __FUNCTION__, __LINE__);

              // Only remove interface from snoopEntry, if there is no IPs associated to it
              if (snoopNoIPs(snoopEntry))
              {
                // Remove the interfave from snoop tree
                if (snoopEntryRemove(dmac,mcastPacket->vlanId,mcastPacket->cbHandle->family)!=L7_SUCCESS)
//              if (snoopIntfRemove(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
//                                  SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle)
//                                  != L7_SUCCESS)
                {
                  LOG_MSG("snoopMgmdLeaveGroupProcess: Failed to remove group membership \n");
                }
                if (igmp_intercept_debug)
                  printf("%s(%d): interface removed\n", __FUNCTION__, __LINE__);
              }
            }
          }
          else  {
            if (igmp_intercept_debug)
              printf("%s(%d): IP 0x%08X does not exist in IP tree\n", __FUNCTION__, __LINE__,ipv4Addr);
          }
          if (igmp_intercept_debug)
            printf("%s(%d)\n", __FUNCTION__, __LINE__);
        }
        if (igmp_intercept_debug)
          printf("%s(%d)\n", __FUNCTION__, __LINE__);
      }
      else
      {
        if (igmp_intercept_debug)
          printf("%s(%d)\n", __FUNCTION__, __LINE__);

        /* Entry exists... give back semaphore, set timers and transmit query */
        timerValue = snoopCheckPrecedenceParamGet(mcastPacket->vlanId,
                                                  mcastPacket->intIfNum,
                                                  SNOOP_PARAM_MAX_RESPONSE_TIME,
                                                  snoopEntry->snoopInfoDataKey.family);
        if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList, 
                                 mcastPacket->intIfNum))
        {        
          if (igmp_intercept_debug)
            printf("%s(%d)\n", __FUNCTION__, __LINE__);

          snoopTimerUpdate(snoopEntry, mcastPacket->intIfNum, mcastPacket->vlanId,
                           SNOOP_MAX_RESPONSE, timerValue, mcastPacket->cbHandle);

        }/* End of membership check */
      }
    }
    SEM_UNLOCK(mcastPacket);
  } /* end of group address validity check */

  if (fwdFlag == L7_FALSE)  {
    if (igmp_intercept_debug)
      printf("%s(%d)\n", __FUNCTION__, __LINE__);

    if ( fwdFlag2 )  rc = igmp_packet_respond(mcastPacket);
    //snoopPacketRtrIntfsForward(mcastPacket);
  } else  {
    if (igmp_intercept_debug)
      printf("%s(%d)\n", __FUNCTION__, __LINE__);

    snoopPacketFlood(mcastPacket);
  }

  if (rc==L7_SUCCESS)  {
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->innerVlanId, SNOOP_STAT_FIELD_LEAVES_RECEIVED);
  }

  if (igmp_intercept_debug)
    printf("%s(%d) rc=%u\n", __FUNCTION__, __LINE__,rc);

  /* Update stats */
  pSnoopCB->counters.controlFramesProcessed++;
  return rc;
}

/*********************************************************************
* @purpose Process IGMP PIMv1 / DVMRP message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
* @param   grpAddr      @b{(input)} Multicast Group address
* @param   ipProtType   @b{(input)} IP Protocol payload type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   PIMv1,DVMRP control messages are of type IGMP.  So, process them 
*          by adding the incoming interface to the list of multicast 
*          routers and then forward the frame to interfaces with 
*          multicast routers attached. We do this to identify the
*          multicast routers currently in non-querier state. For PIMv2 
*          packets forwarding is done in hardware, hence only snoop them.
*
* @end
*
*********************************************************************/
L7_RC_t snoopPimv1v2DvmrpProcess(mgmdSnoopControlPkt_t *mcastPacket,
                                 L7_inet_addr_t *grpAddr, 
                                 L7_uint32 ipProtType)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Verify this frame was sent to the the "all routers" address */

  pSnoopCB = mcastPacket->cbHandle;

  if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, grpAddr) == 0)
  {
    /* Add this interface to mcast router list and to all snooping entries */
    snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                         SNOOP_DYNAMIC, mcastPacket->cbHandle);
  }

  /* Forward on all interfaces on this VLAN, except the incoming interface only
     if the packet is of type IGMP. Other packets are forwarded by silicon*/
  pSnoopCB->counters.controlFramesProcessed++;
  if (ipProtType == IP_PROT_IGMP)
    return snoopPacketFlood(mcastPacket);
  else
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build IGMP Message
*
* @param    intIfNum         @b{(input)}  Internal interface number
* @param    destIp           @b{(input)}  Destination IP Address
* @param    type             @b{(input)}  IGMP Packet type
* @param    groupAddr        @b{(input)}  IGMP Group address
* @param    buffer           @b{(output)} Buffer to hold the packet
* @param    pSnoopCB         @b{(input)}  Control Block
* @param    version          @b{(input)}  IGMP Version
* @param    pSnoopOperEntry  @b{(input)}  Operational data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIGMPFrameBuild2(L7_uint32 intIfNum,
                            L7_uchar8        main_type,
                            L7_inet_addr_t  *groupAddr,
                            L7_uchar8       *buffer,
                            snoop_cb_t      *pSnoopCB,
                            L7_uint32        version,
                            L7_uint32        vlanId,
                            L7_uint32        number_of_groups,
                            L7_uint32       *length)
{
  L7_uchar8       type;
  L7_uchar8      *dataPtr, *tempPtr, *startPtr;
  L7_uchar8       baseMac[L7_MAC_ADDR_LEN], byteVal;
  L7_uchar8       destMac[L7_MAC_ADDR_LEN];
  L7_ushort16     shortVal;
  static L7_ushort16 iph_ident = 1;
  L7_inet_addr_t  destIp;
  L7_uint32       ipv4Addr;
  L7_uint32       index, frame_length = IGMP_FRAME_SIZE;
  dataPtr = buffer;

  // Determine IGMP packet type
  switch (main_type)
  {
    case SNOOP_IGMP_JOIN:
      {
        switch (version)
        {
          case SNOOP_IGMP_VERSION_1:
            type = L7_IGMP_V1_MEMBERSHIP_REPORT;
            break;
          case SNOOP_IGMP_VERSION_2:
            type = L7_IGMP_V2_MEMBERSHIP_REPORT;
            break;
          case SNOOP_IGMP_VERSION_3:
            type = L7_IGMP_V3_MEMBERSHIP_REPORT;
            break;
          default:
            return L7_FAILURE;
        }
      }
      break;

    case SNOOP_IGMP_LEAVE:
      {
        switch (version)
        {
          case SNOOP_IGMP_VERSION_1:
          case SNOOP_IGMP_VERSION_2:
            type = L7_IGMP_V2_LEAVE_GROUP;
            break;
          case SNOOP_IGMP_VERSION_3:
            type = L7_IGMP_V3_MEMBERSHIP_REPORT;
            break;
          default:
            return L7_FAILURE;
        }
      }
      break;

    case SNOOP_IGMP_QUERY:
      type = L7_IGMP_MEMBERSHIP_QUERY;
      break;

    default:
      return L7_FAILURE;
  }

  // Determine Destination IP
  switch (type)
  {
    case L7_IGMP_V1_MEMBERSHIP_REPORT:
    case L7_IGMP_V2_MEMBERSHIP_REPORT:
      memcpy(&destIp,groupAddr,sizeof(L7_inet_addr_t));
      break;

    case L7_IGMP_V3_MEMBERSHIP_REPORT:
    case L7_IGMP_V2_LEAVE_GROUP:
      destIp.family = L7_AF_INET;
      destIp.addr.ipv4.s_addr = L7_IP_ALL_RTRS_ADDR;
      break;

    case L7_IGMP_MEMBERSHIP_QUERY:
    default:
      destIp.family = L7_AF_INET;
      destIp.addr.ipv4.s_addr = L7_IP_ALL_HOSTS_ADDR;
      break;
  }

  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&destIp, destMac);
  /* Validate MAC address */
  if (snoopMacAddrCheck(destMac, pSnoopCB->family) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Get base MAC address (could be BIA or LAA) and use it as src MAC */
  if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
  {
    simGetSystemIPBurnedInMac(baseMac);
  }
  else
  {
    simGetSystemIPLocalAdminMac(baseMac);
  }

  /* Set source and dest MAC in ethernet header */
  SNOOP_PUT_DATA(destMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes
  SNOOP_PUT_DATA(baseMac, L7_MAC_ADDR_LEN, dataPtr);    // 6 bytes

  /* OuterVlan */
//shortVal = L7_ETYPE_8021Q;
//SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
//shortVal = (vlanId<4096) ? snoop_mcast_vlan[vlanId] : vlanId;
//SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* IP Ether type */
  shortVal = L7_ETYPE_IP;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  
  /* Start of IP Header */
  startPtr = dataPtr; 
  /* IP Version */
  byteVal    = (L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte

  /* TOS */
  byteVal     = SNOOP_IP_TOS;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Payload Length */
  if (version > SNOOP_IGMP_VERSION_2)
  {
    shortVal = L7_IP_HDR_LEN + SNOOP_IGMPV3_HEADER_MIN_LENGTH;
  }
  else
  {
    shortVal = L7_IP_HDR_LEN + SNOOP_IGMPv1v2_HEADER_LENGTH;
  }
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* Identified */
  shortVal = iph_ident++;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* Fragment flags */
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  /* TTL */
  byteVal = SNOOP_IP_TTL;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Protocol */
  byteVal = IGMP_PROT;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Checksum = 0*/
  shortVal = 0;
  tempPtr = dataPtr;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  /* Source Address - Snooping Switches send it with null source address */
  if (inetAddressGet(pSnoopCB->family, &pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress, &ipv4Addr)!=L7_SUCCESS)  {
    ipv4Addr = 0;
  }
  memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN); dataPtr += L7_IP_ADDR_LEN;  // 4 bytes
  /* Destination Address */
  inetAddressGet(L7_AF_INET, &destIp, &ipv4Addr);
  memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN); dataPtr += L7_IP_ADDR_LEN;  // 4 bytes

  shortVal = snoopCheckSum((L7_ushort16 *)startPtr, L7_IP_HDR_LEN, 0);
  SNOOP_PUT_SHORT(shortVal, tempPtr);                   // 2 bytes
  SNOOP_UNUSED_PARAM(tempPtr);
  /* End of IP Header */

  /* Start IGMP Header */
  startPtr = dataPtr;

  /* IGMP Type */
  byteVal = type;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte
  /* Max response code */
  byteVal = 0;
  SNOOP_PUT_BYTE(byteVal, dataPtr);                     // 1 byte 
  /* Checksum = 0*/
  tempPtr = dataPtr;
  shortVal = 0;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

  if (type==L7_IGMP_V1_MEMBERSHIP_REPORT ||
      type==L7_IGMP_V2_MEMBERSHIP_REPORT ||
      type==L7_IGMP_V2_LEAVE_GROUP ||
      type==L7_IGMP_MEMBERSHIP_QUERY)
  {  
    // Group Address
    inetAddressGet(L7_AF_INET, groupAddr, &ipv4Addr);
    memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN);
    dataPtr += L7_IP_ADDR_LEN;                       // 4 bytes  

    if (type==L7_IGMP_MEMBERSHIP_QUERY && version==SNOOP_IGMP_VERSION_3)
    {
      /* QRV */
      byteVal = 2; // pSnoopOperEntry->snoopQuerierInfo.sFlagQRV;  // Do not supress router side processing
      SNOOP_PUT_BYTE(byteVal, dataPtr);                                   // 1 byte
      /* QQIC */
      byteVal = 60; // pSnoopOperEntry->snoopQuerierInfo.qqic;
      SNOOP_PUT_BYTE(byteVal, dataPtr);                                   // 1 byte
      // Number of sources
      shortVal = 0;
      SNOOP_PUT_SHORT(shortVal, dataPtr);                                 // 2 byte

      SNOOP_UNUSED_PARAM(dataPtr);

      shortVal = snoopCheckSum((L7_ushort16 *)startPtr, SNOOP_IGMPV3_HEADER_MIN_LENGTH, 0);
      SNOOP_PUT_SHORT(shortVal, tempPtr);/* Copy the calculated checksum 
                                            to stored checksum ptr */
      SNOOP_UNUSED_PARAM(tempPtr);

      frame_length = IGMPv3_FRAME_SIZE;
    }
    else  {
      SNOOP_UNUSED_PARAM(dataPtr);

      shortVal = snoopCheckSum((L7_ushort16 *)startPtr, SNOOP_IGMPv1v2_HEADER_LENGTH, 0);
      SNOOP_PUT_SHORT(shortVal, tempPtr); /* Copy the calculated checksum
                                            to stored checksum ptr */
      SNOOP_UNUSED_PARAM(tempPtr);

      frame_length = IGMP_FRAME_SIZE;
    }
  }
  else if (type==L7_IGMP_V3_MEMBERSHIP_REPORT)
  {
    /* Reserved = 0*/
    shortVal = 0;
    SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

    /* Number of group ips*/
    shortVal = number_of_groups;
    SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
    
    if (number_of_groups>255)  number_of_groups=255;
    for (index=0; index<number_of_groups; index++)
    {
      /* Record type */
      byteVal = (main_type==SNOOP_IGMP_JOIN) ? SNOOP_IGMP_V3_RECORD_TYPE_INCLUDE : SNOOP_IGMP_V3_RECORD_TYPE_EXCLUDE;
      SNOOP_PUT_BYTE(byteVal, dataPtr);                                   // 1 byte
      /* Auxiliary data length */
      byteVal = 0;
      SNOOP_PUT_BYTE(byteVal, dataPtr);                                   // 1 byte
      /* Number of sources*/
      shortVal = 0;
      SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
      // Group Address
      inetAddressGet(L7_AF_INET, &groupAddr[index], &ipv4Addr);
      memcpy(dataPtr, &ipv4Addr, L7_IP_ADDR_LEN);
      dataPtr += L7_IP_ADDR_LEN;                            // 4 bytes
    }

    SNOOP_UNUSED_PARAM(dataPtr);

    shortVal = snoopCheckSum((L7_ushort16 *)startPtr, 8 + number_of_groups*8, 0);
    SNOOP_PUT_SHORT(shortVal, tempPtr);/* Copy the calculated checksum 
                                          to stored checksum ptr */
    SNOOP_UNUSED_PARAM(tempPtr);

    frame_length = IGMPv3_FRAME_SIZE - 4 + number_of_groups*8;
  }

  // Return length of packet
  if (length!=L7_NULLPTR)  *length = frame_length;
  return L7_SUCCESS;
}

