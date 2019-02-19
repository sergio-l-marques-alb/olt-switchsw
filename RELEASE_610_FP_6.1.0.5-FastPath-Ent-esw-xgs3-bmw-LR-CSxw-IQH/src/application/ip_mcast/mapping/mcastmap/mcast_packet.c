/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  mcast_packet.c
*
* @purpose   MCAST Mapping system infrastructure
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    01/31/2006
*
* @author    gkiran
* @end
*
**********************************************************************/

#include "l7_ip_api.h"
#include "l7_socket.h"
#include "l7_mgmd_api.h"
#include "l7_dvmrp_api.h"
#include "l7_pimdm_api.h"
#include "l7_pimsm_api.h"
#include "buff_api.h"
#include "l3end_api.h"
#include "osapi_support.h"
#include "mcast_util.h"
#include "mcast_packet.h"
#include "mcast_debug.h"
#include "mcast_wrap.h"

#define   MCAST_HEADER_LEN_MASK   0x0f
#define   MCAST_HEADER_LEN_SHIFT  2

#define IGMP_DVMRP        0x13    /* DVMRP routing message    */
#define IGMP_RESP         0x1e    /* traceroute resp.(to sender) */

#define  MCAST_ETHER_MAX_LEN                    1518


static void processMulticastPacket(L7_uchar8 *ipHdr, 
                                      L7_uint32 intIfNum);
static L7_BOOL mcastIsInterfaceUp(L7_uint32 rtrIfNum);
static L7_RC_t mcastMapEventSend(mcastControlPkt_t *pMessage, 
                                 L7_uchar8 *pBuffer,
                                 L7_uint32 len, L7_uint32 protoType,
                                 L7_uint32 eventCmd);

/*********************************************************************
* @purpose  Consume IGMP and PIM control frames
*
* @param    hookId        @b{(input)} The hook location
* @param    bufHandle     @b{(input)} Handle to the frame to be processed
* @param    pduInfo      @b{(input)} Pointer to info about this frame
* @param    continueFunc  @b{(input)} Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_CONSUMED  if frame has been consumed; stop processing it
* @returns  SYSNET_PDU_RC_IGNORED   if frame has been ignored; continue processing it
*
* @comments none
*       
* @end
*********************************************************************/
SYSNET_PDU_RC_t mcastMapIntercept(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
  L7_uchar8 *data,pkt[MCAST_ETHER_MAX_LEN],*ipPkt;
  L7_ipHeader_t *ipHeader;
  L7_BOOL localAddr = L7_FALSE, isMulticast = L7_FALSE;
  L7_uint32 i, intf;
  L7_IP_ADDR_t localIPAddr = 0;
  L7_IP_ADDR_t localMask = 0;
  L7_char8 *ipDestPtr;
  L7_uint32 dataLength;
  L7_uint32 tmp;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, dataLength);

  ipHeader = (L7_ipHeader_t *)(data + sysNetDataOffsetGet(data));
  if ((ipHeader->iph_prot != IP_PROT_PIM ) &&
      (ipHeader->iph_prot != IP_PROT_IGMP ))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          "Protocol is not IGMP or PIM\r\n");
    return SYSNET_PDU_RC_IGNORED;

  }
  if (dataLength > MCAST_ETHER_MAX_LEN)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          "dataLength is greater than 1518\r\n");
    return SYSNET_PDU_RC_IGNORED;

  }
  memcpy(&pkt,data,dataLength);
  SYSAPI_NET_MBUF_FREE(bufHandle);
  ipPkt = pkt;

  ipHeader = (L7_ipHeader_t *)(ipPkt + sysNetDataOffsetGet(ipPkt));

  

  /* Both IGMP and PIM packets need to be handled by Multicast so check 
   * for both protocols in the IP Header.
   */     

  memcpy(&tmp,&(ipHeader->iph_dst),sizeof(tmp));
  if (!L7_IP4_IN_MULTICAST(osapiNtohl(tmp)))
   {
     localAddr = L7_FALSE;
     for (i = 1; i <= L7_RTR_MAX_RTR_INTERFACES; i++ )
     {
       intf = 0;
       if (ipMapRtrIntfToIntIfNum(i, &intf) != L7_SUCCESS)
       {
         continue;
       }
       if ((ipMapRtrIntfIpAddressGet(intf, &localIPAddr, &localMask) != L7_SUCCESS) ||
             (localIPAddr == L7_NULL_IP_ADDR))
       {
         continue;
       }
       memcpy(&tmp,&(ipHeader->iph_dst),sizeof(tmp));
       if (osapiNtohl(tmp) == localIPAddr)
       {
         localAddr = L7_TRUE;
         break;
       }
     }
   }  
   else
   {
     /* 
      * Check the MAC dest'n and IP dest'n mapping for IGMP  and PIM packets 
      * Conditions:-
      * (a) The MAC DA should have first 3 octets as 0x 01005E
      * (b) The last 3 octets of MAC DA should match the IPv4 DA
      */
      ipDestPtr = (L7_char8 *) &(ipHeader->iph_dst); 
      if(mcastMacAddressCheck(L7_AF_INET,ipDestPtr,ipPkt)== L7_FAILURE)
      {
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          " MAC dest'n and IP dest'n does not map correctly");
        return SYSNET_PDU_RC_CONSUMED;
      }
        
      isMulticast = L7_TRUE;
   }

   if ((isMulticast == L7_TRUE)||
       (localAddr == L7_TRUE) )
   {
      /* process Multicast packets */
     processMulticastPacket((L7_uchar8 *)ipHeader, pduInfo->intIfNum);
   }
   
  /* Handle multicast packets as well as unicast IGMP and PIM packets */
   return SYSNET_PDU_RC_CONSUMED;
}
/*********************************************************************
* @purpose  Prepares the multicast packets from bufHandle and interface num.
*
* @param    bufHandle    @b{(input)} Handle which contains incoming DHCP data
* @param    intIfNum     @b{(input)} Interface num through which packet arrived
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/

static void processMulticastPacket(L7_uchar8 *ipHeader, L7_uint32 intIfNum)
{
  L7_uint32          rtrIfNum =1;
  L7_ushort16        ipLength = L7_NULL;
  L7_int32           headerLen = L7_NULL;
  L7_uchar8         *payLoad;
  L7_BOOL            localSubnet = L7_FALSE;
  mcastControlPkt_t  message;
  L7_ipHeader_t     *ip_header;
  L7_igmpMsg_t      *igmpHdr;

  if(ipHeader == L7_NULL)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"Invalid input\r\n");
    return ;

  }

  /*memset Control Packet to zero*/
  memset(&message, 0, sizeof(mcastControlPkt_t));
  /* copy the message */
  ip_header = (L7_ipHeader_t *)ipHeader;

  if ((ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum)) != L7_SUCCESS )
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    " Conversion to rtrIfNum failed for intIfNum = %d\n",
                    intIfNum);
    return ;
  }

  /* store interface number in message. */
  message.rtrIfNum =  rtrIfNum;

  /* store family type in message. */
  message.family =  L7_AF_INET;

  if (ip_header->iph_ttl <= L7_MCAST_TTL_THRESHOLD_MIN)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
               " Packet received with TTL less than min ttl\n");
    return;
  }

  /* Check minimum IP header length */
  headerLen = (ip_header->iph_versLen & 
               MCAST_HEADER_LEN_MASK) << MCAST_HEADER_LEN_SHIFT;
  if (headerLen < sizeof(L7_ipHeader_t))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          " Packet received with Header Length Mismatch.\n");
    return ;
  }

  /* Validate IP header checksum */
  if (inetChecksum(ip_header,headerLen) != 0)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    "Packet received with invalid IP checksum \n");
    return ;
  }

  if (inetAddressSet(L7_AF_INET, 
                     &ip_header->iph_src, &message.srcAddr) != L7_SUCCESS)
  {
    return ;
  }

  if (inetAddressSet(L7_AF_INET, 
                     &ip_header->iph_dst, &message.destAddr) != L7_SUCCESS)
  {
    return ;
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                      " Checking for unnumbered, rtrIfNum =%d", rtrIfNum);
  if (mcastIpMapIsRtrIntfUnnumbered(message.family,rtrIfNum) == L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                        " Received packet on unnumbered iface = %d", rtrIfNum);
    localSubnet = L7_TRUE;
  }
  else 
  {
    /* This check is done as currently no mcast protocols process loop-back packets. */
    if (inetIsLocalAddress(&message.srcAddr, message.rtrIfNum) == L7_TRUE)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                          "Packet received with local sourceAddr on iface = %d address ", rtrIfNum);
      return ;
    }
    localSubnet = inetIsDirectlyConnected(&message.srcAddr, message.rtrIfNum);
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                      " localSubnet = %d", localSubnet);

  /* If the dst ip address is link local multicast, make sure that the
   * src ip address is from a local subnet. If not discard it.
   */
  if ((L7_FALSE == localSubnet) && 
      (inetIsLinkLocalMulticastAddress(&message.destAddr)) == L7_TRUE)
  {
    {
      if ( (localSubnet == L7_FALSE) &&
           (inetIsAddressZero(&message.srcAddr)) &&
           (ip_header->iph_prot == IP_PROT_IGMP) )
      {
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                        "Receive packet with zero source address for IGMP \n");
        localSubnet = L7_TRUE;
      }
      else
      {
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                        "Receive packet with non local source address \n");
        return ;
      }
    }


  }

  message.ipTypeOfService = ip_header->iph_tos;
  ipLength = ip_header->iph_len;

  payLoad = ((L7_uchar8 *)ip_header + headerLen);

  /* Storing length in control pakcet */
  message.length = ipLength - headerLen;

  if (ip_header->iph_prot == IP_PROT_IGMP)
  {
    igmpHdr = (L7_igmpMsg_t *)((L7_uchar8 *)ip_header + headerLen);

    if (igmpHdr->igmpType == IGMP_DVMRP)
    {
       if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
      {
        if (mcastMapEventSend(&message,payLoad,message.length,
                              L7_MCAST_IANA_MROUTE_DVMRP,
                              MCAST_EVENT_DVMRP_CONTROL_PKT_RECV) 
            != L7_SUCCESS)
        {
          MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                         " Failed to send packet reception event to DVMRP\n"); 
        }
      }
    }
    /* If IGMP packet or it is from a local
     * subnet, go ahead and process it.
     */
    else if (localSubnet == L7_TRUE)
    {

      if(headerLen > L7_IP_HDR_LEN)
      {
        if(*(L7_uchar8 *)(((L7_uchar8 *)ip_header) +L7_IP_HDR_LEN)
          == IGMP_IP_ROUTER_ALERT_TYPE)  
        {
          message.ipRtrAlert = L7_TRUE;

        }
      }
     if (mgmdMapMgmdIsOperational(message.family) == L7_TRUE)
     {
       /* Reference: RFC3376 - Section 4 */
       if (ip_header->iph_ttl != 1)
       {
         MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "IGMP Packet received with "
                          "Invalid TTL - %d, Discarding it.\n", ip_header->iph_ttl); 
       }
       else
       {
         if (mcastMapEventSend(&message,payLoad,message.length,
                                L7_MCAST_IANA_MROUTE_IGMP,
                                MCAST_EVENT_IGMP_CONTROL_PKT_RECV) 
              != L7_SUCCESS)
          {
            MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                           " Failed to send packet reception event to MGMD\n"); 
          }
       }
     }
    }
  }
  else if (ip_header->iph_prot == IP_PROT_PIM)
  {
    if (pimdmMapPimDmIsOperational(message.family) == L7_TRUE)
    {
      if (mcastMapEventSend(&message,payLoad,message.length,
                            L7_MCAST_IANA_MROUTE_PIM_DM,
                            MCAST_EVENT_PIMDM_CONTROL_PKT_RECV) 
          != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                       "Failed to send packet reception event to PIMDM\n"); 
      }
    }
    else if (pimsmMapPimsmIsOperational(message.family) == L7_TRUE)
    {
      if (mcastMapEventSend(&message,payLoad,message.length,
                            L7_MCAST_IANA_MROUTE_PIM_SM,
                            MCAST_EVENT_PIMSM_CONTROL_PKT_RECV) 
          != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                        " Failed to send packet reception event to PIMSM\n"); 
      }
    }
  }
  return ;
}

/************************************************************************
* @purpose   Sends the multicast control packet over the specified interface
*
* @param     pSendPkt  @b{(input)}  structure containing the buffer 
                                    to sendpacket out
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
************************************************************************/

L7_RC_t mcastMapPacketSend(mcastSendPkt_t *pSendPkt)
{
  L7_int32 bytesSent=-1;
  L7_uchar8 addrBuffStr[IPV6_DISP_ADDR_LEN];
  L7_RC_t ret;
  L7_uint32 intIfNum;

  if (L7_NULLPTR == pSendPkt)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                     " NULL packet info to send\n");
    return L7_FAILURE;
  }

  /*Check If the Interface is UP .Will the OS not be able to decide 
    this in the sendto() function and return appropriately.-TBD*/
  if (mcastIsInterfaceUp(pSendPkt->rtrIfNum) == L7_FALSE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    "Interface = %d is not up to send out the packet\n");
    return L7_FAILURE;
  }

  if(ipMapRtrIntfToIntIfNum(pSendPkt->rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
      "ipMapRtrIntfToIntIfNum() failed rtrIfNum = %d", pSendPkt->rtrIfNum);
    return L7_FAILURE;
  }

  if (L7_AF_INET == pSendPkt->family)
  {
    L7_sockaddr_in_t saddr, fromaddr;
    memset(&saddr,0,sizeof(L7_sockaddr_in_t));
    memset(&fromaddr,0,sizeof(L7_sockaddr_in_t));
    if (inetIsInMulticast(&pSendPkt->destAddr) == L7_TRUE)
    {
      /*Multicast Send Fails if MSG_DONTROUTE option is set */
      pSendPkt->flags &= ~MSG_DONTROUTE;
    }

    saddr.sin_family = L7_AF_INET;
    saddr.sin_port = osapiHtons(pSendPkt->port);
    saddr.sin_addr.s_addr =
    osapiHtonl(pSendPkt->destAddr.addr.ipv4.s_addr);
    fromaddr.sin_family = L7_AF_INET;
    /* Send from same port we're sending to */
    fromaddr.sin_port = osapiHtons(pSendPkt->port);
    
    ret = osapiPktInfoSend(pSendPkt->sockFd, pSendPkt->payLoad, 
      pSendPkt->length, pSendPkt->flags, 
      (L7_sockaddr_t *)&fromaddr, sizeof(fromaddr),
      (L7_sockaddr_t *)&saddr, sizeof(saddr), 
      intIfNum, &bytesSent, 0, 0, 0, L7_FALSE, 
      L7_FALSE, L7_FALSE, pSendPkt->rtrAlert);

    if (L7_SUCCESS != ret)
    {
      L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_FLEX_MCAST_MAP_COMPONENT_ID, "Mcast IPv4 Control Packet Send Failed; "
			   "rtrIfNum - %d, DestIP - %s, sockFD - %d, Length - %d, Flags - 0x%x. IPv4 Multicast Control Packet"
			   " Transmission Failed. This is because of the Socket sendto() failure. This message can be seen "
			   "when a load of control packets are sent at once and it results in the non-availability of socket "
			   "buffers.", intIfNum, inetAddrPrint(&pSendPkt->destAddr,addrBuffStr),
               pSendPkt->sockFd, pSendPkt->length, pSendPkt->flags);
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n Failed to send v4 multicast packet over"
              "interface = %d  and dest = %s \n",pSendPkt->rtrIfNum,
              inetAddrPrint(&pSendPkt->destAddr,addrBuffStr));
      return L7_FAILURE;
    }
  } /*end of if(L7_AF_INET == pSendPkt->family) */
  else if (L7_AF_INET6 == pSendPkt->family )
  {
    if (mcastMapV6PacketSend(pSendPkt) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP-v6 : Failed to send v6 multicast packet over"
              "interface = %d  and dest = %s \n",pSendPkt->rtrIfNum,
              inetAddrPrint(&pSendPkt->destAddr,addrBuffStr));  
      return L7_FAILURE;
    }

  }/*end of if(L7_AF_INET6 == pSendPkt->family) */

  return L7_SUCCESS;
}


/********************************************************
* @purpose   This API is a dummy API to check if the interface is UP.
*            This should be replaced by a common API
*
* @param     rtrIfNum      @b{(input)}  router interface number
*
* @returns   L7_TRUE
*
* @comments  none
*
* @end
*********************************************************/

static L7_BOOL mcastIsInterfaceUp(L7_uint32 rtrIfNum)
{
  return L7_TRUE;
}

/***************************************************************************
* @purpose   This function invokes the appropriate MRP event Send function
*            based on the protoType and the eventCmd type.
*
* @param     pMessage      @b{(input)} message to be sent to the MRP
* @param     pBuffer       @b((input)) input buffer received
* @param     len           @b((input)) buffer length 
* @param     protoType     @b((input)) protocol Type(IGMP/PIMSM/PIMDM/DVMRP/MLD) 
* @param     eventCmd      @b((input)) Event Cmd Type 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  none
*
* @end
****************************************************************************/
static L7_RC_t mcastMapEventSend(mcastControlPkt_t *pMessage,
                                 L7_uchar8 *pBuffer,
                                 L7_uint32 len, L7_uint32 protoType,
                                 L7_uint32 eventCmd)
{
  L7_int32 rc = L7_FAILURE;

  if (L7_NULLPTR == pMessage || L7_NULLPTR == pBuffer)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n NULL info to post packet reception event \n");
    return L7_FAILURE;
  }

  rc = bufferPoolAllocate(mcastGblVariables_g.mcastV4CtrlPktPoolId,
                          &(pMessage->payLoad));
  if (rc == L7_SUCCESS)
  {
    memcpy(pMessage->payLoad,pBuffer,len);

    switch (protoType)
    {
      case L7_MCAST_IANA_MROUTE_DVMRP:

        rc = dvmrpMapEventChangeCallback(pMessage->family, eventCmd, 
                                       sizeof(mcastControlPkt_t), (void *)pMessage);
        break;
      case L7_MCAST_IANA_MROUTE_IGMP:
        rc = mgmdMapComponentCallback(pMessage->family, eventCmd, 
                                      sizeof(mcastControlPkt_t), (void *)pMessage);
        break;
      case L7_MCAST_IANA_MROUTE_PIM_DM:
        rc = pimdmMapEventChangeCallback (pMessage->family, eventCmd,
                                          sizeof (mcastControlPkt_t),
                                          (void*) pMessage);
        break;
      case L7_MCAST_IANA_MROUTE_PIM_SM:
        rc = pimsmMapCommonCallback(pMessage->family, eventCmd, 
                                    sizeof(mcastControlPkt_t), (void *)pMessage);
        break;
      default:
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n Failed notification to MRPs of packet reception "
                "with invalid Protocoltype of %d.\n", protoType);
        break;
    }
    if (rc != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\n Failed to notify MRPs of packet reception\n");
      bufferPoolFree(mcastGblVariables_g.mcastV4CtrlPktPoolId, 
                     pMessage->payLoad);
      return L7_FAILURE;
    }
  }
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nFailed to allocate packet buffer from PoolId = %d\n",
            mcastGblVariables_g.mcastV4CtrlPktPoolId);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
