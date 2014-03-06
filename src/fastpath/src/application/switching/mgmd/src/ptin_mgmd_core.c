
/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @filename   snooping.c
*
* @purpose    To Process IGMP and MLD Multicast Control Packets
*
* @component  Mgmd
*
* @comments   Behaves as Router in the downlink (i.e. leaf) ports ,
* @comments   and as a Host in the uplink (i.e. root) ports. 
*
* @create    21/10/2013 
*
* @author    Daniel Filipe Figueira
* @author    Márcio Daniel Melo
* @end
*
**********************************************************************/
#include "ptin_mgmd_core.h"
#include "ptin_mgmd_util.h"

#include "ptin_mgmd_db.h"
#include "ptin_mgmd_logger.h" 
#include "ptin_mgmd_osapi.h"
#include "ptin_mgmd_features.h"

#include "ptin_mgmd_inet_defs.h"
#include "ptin_utils_inet_addr_api.h"

#include "ptin_mgmd_statistics.h"
#include "ptin_mgmd_cfg.h"
#include "ptin_mgmd_ctrl.h"

#include "ptin_mgmd_grouptimer.h"
#include "ptin_mgmd_sourcetimer.h"
#include "ptin_mgmd_proxytimer.h"
#include "ptin_mgmd_querytimer.h"
#include "ptin_mgmd_groupsourcespecifictimer.h"
#include "ptin_mgmd_routercmtimer.h"
#include "ptin_mgmd_proxycmtimer.h"

#include "ptin_mgmd_cnfgr.h"

#include "ptin_mgmd_whitelist.h"

#include "ptin_mgmd_service_api.h"


/*********************Global Variables******************/
BOOL                     ptin_mgmd_extended_debug = FALSE;
extern unsigned long     ptin_mgmd_memory_allocation;
/*********************End Global Variables******************/



/*********************Static Variables******************/
static ptin_mgmd_inet_addr_t sourceList[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD];
/*********************End Static Variables******************/

/*********************Static Routines******************/
static RC_t ptin_mgmd_igmp_packet_process(ptinMgmdControlPkt_t *mcastPacket);
static RC_t ptin_mgmd_mld_packet_process(void);
/*********************End Statid Routines******************/

/************************************************************************************************************/


void ptin_mgmd_core_memory_allocation(void)
{
  ptin_mgmd_memory_allocation+=sizeof(sourceList[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD]);  
}

RC_t ptin_mgmd_igmp_packet_process(ptinMgmdControlPkt_t *mcastPacket)
{
  uchar8              *buffPtr;
  uchar8               igmpType;
  RC_t                 rc      = SUCCESS;
  
  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  }
  
  buffPtr = mcastPacket->ipPayload;
  PTIN_MGMD_GET_BYTE(igmpType, buffPtr);
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"IGMP Type:[0x%X] NetworkVersion:[%u] ClientVersion:[%u]", igmpType, mcastPacket->cbHandle->mgmdProxyCfg.networkVersion, mcastPacket->cbHandle->mgmdProxyCfg.clientVersion);
  PTIN_MGMD_UNUSED_PARAM(buffPtr);  

  //Validate total length value
  if ((igmpType == PTIN_IGMP_V3_MEMBERSHIP_REPORT && mcastPacket->ipPayloadLength < IGMP_V3_PKT_MIN_LENGTH) ||
      ((igmpType == PTIN_IGMP_MEMBERSHIP_QUERY  || igmpType == PTIN_IGMP_V2_MEMBERSHIP_REPORT || igmpType == PTIN_IGMP_V1_MEMBERSHIP_REPORT) && (mcastPacket->ipPayloadLength < IGMP_PKT_MIN_LENGTH)))
  {
    ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdPacketType2IGMPStatField(mcastPacket->ipPayload[0], SNOOP_STAT_FIELD_INVALID_RX));
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid packet: Invalid IGMP header length[%u]", mcastPacket->ipPayloadLength);
    return FAILURE;
  }

  if (mcastPacket->cbHandle->mgmdProxyCfg.host.tos_rtr_alert_check == TRUE)
  {
    //Reference: RFC3376 - Section 4. Do not accept IGMPv3 Report or Query messages without the IP TOS set to 0xC0. However this check is performed only if MGMD is configured for it.
    // @todo: Currently this field is not supported on the GL, Therefore we need to decide in compile time wether we check it or not
    if (((mcastPacket->tosByte) & (MGMD_IP_TOS)) != MGMD_IP_TOS)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid packet: Invalid ToS - %u", mcastPacket->tosByte);
      return FAILURE;
    }
  }

  //Process the IGMP message
  switch (igmpType)
  {
    case PTIN_IGMP_MEMBERSHIP_QUERY:     
      if(mcastPacket->cbHandle->mgmdProxyCfg.networkVersion!=PTIN_IGMP_VERSION_3 && mcastPacket->ipPayloadLength>IGMP_PKT_MIN_LENGTH)
      { 
        PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"MEMBERSHIP_QUERYv3: Silently ignored...we are configured to operate at IGMPv2 only!");        
        rc=ERROR;//We are configured to operate at IGMPv2         
      }
      else
      {
        rc = ptin_mgmd_membership_query_process(mcastPacket);
      }      
      break;
    case PTIN_IGMP_V3_MEMBERSHIP_REPORT:
      if (mcastPacket->cbHandle->mgmdProxyCfg.clientVersion!=PTIN_IGMP_VERSION_3)//Drop the packet
      { 
        PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"MEMBERSHIP_REPORTv3: Silently ignored...we are configured to operate at IGMPv2 only!");
        rc=ERROR;//We are configured to operate at IGMPv2               
      }
      else
      {
        rc = ptin_mgmd_membership_report_v3_process(mcastPacket);         
      }      
      break;

    case PTIN_IGMP_V1_MEMBERSHIP_REPORT: //Should we support these?
    case PTIN_IGMP_V2_MEMBERSHIP_REPORT:
    case PTIN_IGMP_V2_LEAVE_GROUP:
      rc = ptin_mgmd_membership_report_v2_process(mcastPacket);
      break; 

    default:
      PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unknown IGMP packet: Silently ignored...");
      break;
  }              

  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
  }
  return rc;
}

RC_t ptin_mgmd_mld_packet_process(void)
{
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "MLD not yet supported");
#ifdef PTIN_MGMD_MLD_SUPPORT
  else /* IPv6 Multicast Control Packet */
  {
    switch (mcastPacket.msgType)
    {
    case L7_MLD_MEMBERSHIP_QUERY: /* Query */
      rc = snoopMgmdMembershipQueryProcess(mcastPacket);
      break;

    case L7_MLD_V1_MEMBERSHIP_REPORT: /* Report */
      rc = snoopMgmdMembershipReportProcess(mcastPacket);
      break;

    case L7_MLD_V2_MEMBERSHIP_REPORT:
      rc = SUCCESS; //snoopMgmdSrcSpecificMembershipReportProcess(&mcastPacket);
      break;

    default:
      PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unknown IGMP packet: Silently ignored...");
      break;
    } /* end of switch case */
  }/* end of IPv6 Multicast Control Packet processing */
#endif

  return SUCCESS;
}


/*****************************************************************
* @purpose  Calculates the max response time from the max response code 
* @purpose     and to convert to mS unit
*
* @param    max_resp_time @b{ (input) } the maximum response time
*
* @returns
* @notes
*
   If Max Resp Code < 128, Max Resp Time = Max Resp Code

   If Max Resp Code >= 128, Max Resp Code represents a floating-point
   value as follows:

       0 1 2 3 4 5 6 7
      +-+-+-+-+-+-+-+-+
      |1| exp | mant  |
      +-+-+-+-+-+-+-+-+

   Max Resp Time = (mant | 0x10) << (exp + 3)
* @end
*********************************************************************/
static int32  ptin_mgmd_fp_decode_max_resp_code(uchar8 family, int32 max_resp_code)
{
  int32           max_resp_time = 0;

#if  !SNOOP_PTIN_CISCO_MAX_RESPONSE_CODE_BUG
  if (max_resp_code < 0x80)
  {
    max_resp_time= max_resp_code;
  }
  else
  {
   
    max_resp_time= ((max_resp_code & 0x0F) | 0x10) << (((max_resp_code & 0x70) >>4)+3);
  }
#else
  //Since Cisco equiments consider Max Response Code=Max Response Time, we will behave accordingly 
  max_resp_time= max_resp_code;
#endif

  if (family == PTIN_MGMD_AF_INET)
  {
    //Convert from dS to mS
    max_resp_time = max_resp_time*100;
  }
  else if (family == PTIN_MGMD_AF_INET6)
  {
    max_resp_time = max_resp_time;
  }
  return max_resp_time;
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
* @returns SUCCESS  Valid packet
* @returns FAILURE  Packet to be dropped
*
* @notes Only for IGMP packets
*
* @end
*
*************************************************************************/
static RC_t ptin_mgmd_igmp_packet_parse(uchar8 *framePayload, uint32 framePayloadLength, ptinMgmdControlPkt_t *mcastPacket)
{
  uint32               ipHdrLen;  
  ptin_mgmd_ipHeader_t ip_header;
  uchar8               *startPtr;
  uchar8               *buffPtr;

  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  }

   /* Get Mgmd Control Block */
  if ((mcastPacket->cbHandle = mgmdCBGet(PTIN_MGMD_AF_INET)) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Error getting pMgmdCB");
    return FAILURE;
  }

  //Get proxy configurations
  if (ptin_mgmd_igmp_proxy_config_get(&mcastPacket->cbHandle->mgmdProxyCfg) != SUCCESS)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Error getting IGMP Proxy configurations");
    return FAILURE;
  }

  memset(mcastPacket->framePayload, 0x00, PTIN_MGMD_EVENT_PACKET_DATA_SIZE_MAX * sizeof(uchar8));
  memcpy(mcastPacket->framePayload, framePayload, framePayloadLength);
  mcastPacket->frameLength = framePayloadLength;
  buffPtr             = framePayload;
  mcastPacket->family = PTIN_MGMD_AF_INET;
  startPtr            = buffPtr;

  if (mcastPacket->frameLength < PTIN_IP_HDR_LEN + MGMD_IGMPv1v2_HEADER_LENGTH)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid packet: Packet length too small [%u]", mcastPacket->frameLength);
    return FAILURE;
  }
  
  //Parse IP header
  PTIN_MGMD_GET_BYTE(ip_header.iph_versLen, buffPtr);
  PTIN_MGMD_GET_BYTE(ip_header.iph_tos, buffPtr);
  PTIN_MGMD_GET_SHORT(ip_header.iph_len, buffPtr);
  PTIN_MGMD_GET_SHORT(ip_header.iph_ident, buffPtr);
  PTIN_MGMD_GET_SHORT(ip_header.iph_flags_frag, buffPtr);
  PTIN_MGMD_GET_BYTE(ip_header.iph_ttl, buffPtr);
  PTIN_MGMD_GET_BYTE(ip_header.iph_prot, buffPtr);
  PTIN_MGMD_GET_SHORT(ip_header.iph_csum, buffPtr);
  PTIN_MGMD_GET_ADDR(&ip_header.iph_src, buffPtr);
  PTIN_MGMD_GET_ADDR(&ip_header.iph_dst, buffPtr);

  //Required as per RFC 3376
  if (ip_header.iph_ttl != PTIN_TTL_VALID_VALUE)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid packet: Invalid TTL[%u]", ip_header.iph_ttl);
    return FAILURE;
  }

  //Save tos field Required as per RFC 3376
  mcastPacket->tosByte = ip_header.iph_tos;

  //Calculate the IP header length, including any IP options
  ipHdrLen = ((ip_header.iph_versLen & 0x0f) * 4);

  //check for router alert option  Required as per RFC 3376
  if ((ipHdrLen > PTIN_IP_HDR_LEN) && (*buffPtr ==IGMP_IP_ROUTER_ALERT_TYPE))
  {
    mcastPacket->routerAlert =TRUE;
  }
  else
  {
    mcastPacket->routerAlert =FALSE;
  }

  buffPtr += (ipHdrLen - PTIN_IP_HDR_LEN);
  mcastPacket->ipPayload = buffPtr;    

  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &ip_header.iph_src, &(mcastPacket->srcAddr));
  ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &ip_header.iph_dst, &(mcastPacket->destAddr));
  if ((PTIN_MGMD_INET_IS_ADDR_BROADCAST(&(mcastPacket->srcAddr))) || (PTIN_MGMD_INET_IS_ADDR_EXPERIMENTAL(&(mcastPacket->srcAddr))) || ptin_mgmd_inetIsInMulticast(&(mcastPacket->srcAddr)) == TRUE)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid packet: Invalid src_addr[%s]", mcastPacket->srcAddr);      
    return FAILURE;
  }
  if ( ((ip_header.iph_dst & PTIN_MGMD_CLASS_D_ADDR_NETWORK) != PTIN_MGMD_CLASS_D_ADDR_NETWORK) || (ip_header.iph_dst <= PTIN_MGMD_IP_MCAST_BASE_ADDR) )
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid packet: Invalid dst_addr[%s]", ip_header.iph_dst);      
    return FAILURE;
  }

  mcastPacket->msgType = ip_header.iph_prot;
  if (mcastPacket->msgType == PTIN_MGMD_IP_PROT_IGMP)
  {
    //Validate IP total length value
    if ((ip_header.iph_len - ipHdrLen) < MGMD_IGMPv1v2_HEADER_LENGTH)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid packet: Invalid IP header length [%u]", ip_header.iph_len - ipHdrLen);      
      return FAILURE;
    }

    //Verify IGMP checksum
    if (ptinMgmdCheckSum((ushort16 *)buffPtr, ip_header.iph_len - ipHdrLen, 0) != 0)
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid packet: Invalid IGMP header checksum");      
      return FAILURE;
    }
  }
  else
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Received an Invalid packet: [%u]",mcastPacket->msgType);      
    return FAILURE;
  }
  mcastPacket->ipPayloadLength = ip_header.iph_len - ipHdrLen;

  //Validate IP header checksum
  if (ptinMgmdCheckSum((ushort16 *)startPtr, ipHdrLen, 0) != 0)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid packet: Invalid IP header checksum");      
    return FAILURE;
  }
  
  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
  }
  return SUCCESS;
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
* @returns SUCCESS  Valid packet
* @returns FAILURE  Packet to be dropped
*
* @notes Only for MGMD packets
*
* @end
*
*************************************************************************/
static RC_t ptin_mgmd_mld_packet_parse(uchar8 *payLoad, uint32 payloadLength, ptinMgmdControlPkt_t *mcastPacket)
{
  _UNUSED_(payLoad);
  _UNUSED_(payloadLength);
  _UNUSED_(mcastPacket);
#if 0
  uint32        ipHdrLen;  
  L7_ipHeader_t ip_header;
  uchar8        *startPtr;
  uchar8        *buffPtr;
 
  static ipv6pkt_t ipv6pkt; /* packet with pseudo header for checksum calculation */
  uint32      lenIcmpData;
  L7_ip6Header_t ip6_header;
  ptin_in6_addr_t  ipv6Addr;
  uchar8     *data, byteVal;
  uchar8 tempArr[L7_MAC_ADDR_LEN - 2] = { 0, 0, 0, 0};

  memset(mcastPacket->payLoad, 0x00, PTIN_MGMD_EVENT_PACKET_DATA_SIZE_MAX * sizeof(uchar8));
  memcpy(mcastPacket->payLoad, payLoad, payloadLength);
  mcastPacket->length = payloadLength;

  buffPtr=payLoad;

  mcastPacket.family=PTIN_MGMD_AF_INET6;
  
  /* Check for invalid IPv6 MCAST address */
  if (memcmp(mcastPacket->destMac+2, tempArr, sizeof(tempArr)) == 0)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Invalid Packet: Invalid IPv6 MCAST address");
    return FAILURE;
  }

  if (mcastPacket->length < L7_ENET_HDR_SIZE + L7_ENET_HDR_TYPE_LEN_SIZE + L7_IP6_HEADER_LEN + SNOOP_MLDV1_HEADER_LENGTH)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Received pkt is too small %d", mcastPacket->length);
    return FAILURE;
  }

  SNOOP_GET_LONG(ip6_header.ver_class_flow, buffPtr);
  SNOOP_GET_SHORT(ip6_header.paylen, buffPtr);
  SNOOP_GET_BYTE(ip6_header.next, buffPtr);
  SNOOP_GET_BYTE(ip6_header.hoplim, buffPtr);

  /* Not received with an IPv6 hop limit as 1, discard. RFC3810 section 5  */
  if (ip6_header.hoplim != SNOOP_TTL_VALID_VALUE)
  {
     LOG_WARNING(LOG_CTX_PTIN_IGMP,"Received pkt with hop limit other than one %d",\
                ip6_header.hoplim);
    return FAILURE;
  }

  SNOOP_GET_ADDR6(&ipv6Addr, buffPtr);
  if (!PTIN_IP6_IS_ADDR_LINK_LOCAL(&ipv6Addr))
  {
    /* Not received with an IPv6 link local address.
       Discard RFC3810 5.1.14, 5.2.13 */
     LOG_WARNING(LOG_CTX_PTIN_IGMP,
                "Not received with an IPv6 link local address");
    return FAILURE;
  }

  inetAddressSet(PTIN_MGMD_AF_INET6, &ipv6Addr, &(mcastPacket->srcAddr));
  SNOOP_GET_ADDR6(&ipv6Addr, buffPtr);
  inetAddressSet(PTIN_MGMD_AF_INET6, &ipv6Addr, &(mcastPacket->destAddr));

  xtenHdrLen = 0;
  mcastPacket->routerAlert = FALSE;
  if (ip6_header.next == SNOOP_IP6_IPPROTO_HOPOPTS)
  {
    SNOOP_GET_BYTE(ip6_header.next, buffPtr); /* Next Header */
    SNOOP_GET_BYTE(byteVal, buffPtr); /* Xtension hdr length */
    xtenHdrLen = SNOOP_IP6_HOPBHOP_LEN_GET(byteVal);
    buffPtr += xtenHdrLen - 2;
    mcastPacket->routerAlert = TRUE;
  }

  if (ip6_header.next == IP_PROT_ICMPV6)
  {
    mcastPacket->ip_payload = buffPtr;
    SNOOP_GET_BYTE(mcastPacket->msgType, buffPtr);
    SNOOP_UNUSED_PARAM(buffPtr);
    /* Verify ICMPv6 checksum */
    inetAddressGet(PTIN_MGMD_AF_INET6, &(mcastPacket->srcAddr), &(ipv6pkt.srcAddr));
    inetAddressGet(PTIN_MGMD_AF_INET6, &(mcastPacket->destAddr), &(ipv6pkt.dstAddr));
    lenIcmpData = ip6_header.paylen - xtenHdrLen;
    /*datalen should be in big endian for snoopcheckSum to succeed*/
    if (snoopGetEndianess() == SNOOP_LITTLE_ENDIAN)
    {
      ipv6pkt.dataLen = htonl(lenIcmpData);
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
    if (snoopCheckSum ((ushort16 *)&ipv6pkt,
                       (L7_IP6_HEADER_LEN + lenIcmpData), 0)!=0)
    {
      return FAILURE;
    }

    mcastPacket->ip_payload_length = ip6_header.paylen - xtenHdrLen;
  }
  else
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid next header %d",ip6_header.next);
    return FAILURE;
  }
#endif
  PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Not implemented...");
  return SUCCESS;
}

/*********************************************************************
* @purpose Process incoming snoop control packets
*
* @param   msg       @b{(input)}  Pointer to received snoop control
*                                 message
*
* @returns SUCCESS
* @returns FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
RC_t ptin_mgmd_packet_process(uchar8 *payload, uint32 payloadLength, uint32 serviceId, uint32 portId, uint32 clientId)
{
  static ptinMgmdControlPkt_t mcastPacket;
  RC_t                         rc         = SUCCESS;
  uchar8                       version; 
  ptin_mgmd_externalapi_t      externalApi;
  ptin_mgmd_port_type_t        portType;

  if(ptin_mgmd_extended_debug)
  {
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  //If advanced debugging is enabled, dump packet in output  
    uint32 i;
    printf("Rx (%u bytes):\n", payloadLength);
    for (i=0; i<payloadLength; ++i)
      printf("%02x ", payload[i]);
    printf("\n");
  }

  if (SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Unable to get external API");    
    return FAILURE;
  }

  //Validate packet
  if ( (PTIN_NULLPTR == payload) || (0 == payloadLength) )
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid packet payload:[%08X] length:[%u]", payload, payloadLength);    
    return FAILURE;
  }

  //Validate portId
  if ( (portId==0) || (portId > PTIN_MGMD_MAX_PORTS) )
  {   
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid portID [%u]", portId);    
    return FAILURE;
  }

  //Validate serviceId
  if (serviceId > PTIN_MGMD_MAX_SERVICE_ID)
  {    
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid serviceID [%u]", serviceId);    
    return FAILURE;
  }

  //Validate clientId (only for leaf ports)
  if( (SUCCESS == externalApi.portType_get(serviceId, portId, &portType)) && (PTIN_MGMD_PORT_TYPE_LEAF == portType) )
  {
    if (/*clientId==0  ||*/ clientId >= PTIN_MGMD_MAX_CLIENTS)
    {    
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid clientID [%u]", clientId);
      return FAILURE;
    }
  }

  memset(&mcastPacket, 0x00, sizeof(ptinMgmdControlPkt_t));
  mcastPacket.portId    = portId;
  mcastPacket.serviceId = serviceId;
  mcastPacket.clientId  = clientId;  

  /* Get Mgmd Executation Block */
  if ((mcastPacket.ebHandle = mgmdEBGet()) == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Error getting mgmdEBGet");
    return FAILURE;
  }

  //Parse and process the IGMP/MLD packet
  version=((payload[0]&0xf0)>>4);
  if(version == PTIN_IP_VERSION) 
  {
    if ( (rc=ptin_mgmd_igmp_packet_parse(payload, payloadLength, &mcastPacket)) != SUCCESS)
    {
      if (mcastPacket.ipPayload!=PTIN_NULLPTR)
      {
        ptin_mgmd_stat_increment_field(mcastPacket.portId, mcastPacket.serviceId, mcastPacket.clientId, ptinMgmdPacketType2IGMPStatField(mcastPacket.ipPayload[0], SNOOP_STAT_FIELD_DROPPED_RX));
      }
      else
      {
        ptin_mgmd_stat_increment_field(mcastPacket.portId, mcastPacket.serviceId, mcastPacket.clientId, SNOOP_STAT_FIELD_IGMP_DROPPED);
      }
      if(ptin_mgmd_extended_debug)
      {
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
      }
      return SUCCESS;
    }
    
    rc = ptin_mgmd_igmp_packet_process(&mcastPacket);
  } 
  else if(version == PTIN_IPv6_VERSION)
  {
    rc = ptin_mgmd_mld_packet_parse(payload, payloadLength, &mcastPacket);
    rc = ptin_mgmd_mld_packet_process();

    if(ptin_mgmd_extended_debug)
    {
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
    }
    return SUCCESS;
  }
  else
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Unknown IP header version");
    return SUCCESS;
  }

  if (rc==SUCCESS)
  {
    ptin_mgmd_stat_increment_field(mcastPacket.portId, mcastPacket.serviceId, mcastPacket.clientId, ptinMgmdPacketType2IGMPStatField(mcastPacket.ipPayload[0],SNOOP_STAT_FIELD_VALID_RX));
  }
  else if (rc==FAILURE)
  {
    ptin_mgmd_stat_increment_field(mcastPacket.portId, mcastPacket.serviceId, mcastPacket.clientId, ptinMgmdPacketType2IGMPStatField(mcastPacket.ipPayload[0],SNOOP_STAT_FIELD_INVALID_RX));
  }
  else
  {
    ptin_mgmd_stat_increment_field(mcastPacket.portId, mcastPacket.serviceId, mcastPacket.clientId, ptinMgmdPacketType2IGMPStatField(mcastPacket.ipPayload[0],SNOOP_STAT_FIELD_DROPPED_RX));
  }

  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
  }
  return rc;
}

/*********************************************************************
* @purpose Process IGMPv3/MLDv2 Membership query message
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns SUCCESS
* @returns FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
RC_t ptin_mgmd_membership_query_process(ptinMgmdControlPkt_t *mcastPacket)
{
  ptin_mgmd_port_type_t        portType;
  ptinMgmdGroupInfoData_t     *avlTreeEntry=PTIN_NULLPTR;
  uchar8                      *dataPtr = PTIN_NULL;
  uint32                       ipv4Addr, 
                               incomingVersion = 0, 
                               timeout = 0;
  uchar8                       byteVal,
                               robustnessVariable;
  ushort16                     maxRespCode = 0, 
                               recdChecksum;
  char8                        queryType=-1;
  char                         debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN], 
                               debug_buf2[PTIN_MGMD_IPV6_DISP_ADDR_LEN];
  uint32                       selectedDelay;
  uint32                       maxRespTime;
  uint16                       noOfSources = 0;
  uint16                       sourceIdx;
  void                        *ptr = PTIN_NULLPTR;
  BOOL                         isInterface = FALSE;
  BOOL                         sendReport = FALSE;
  ptin_mgmd_inet_addr_t        groupAddr,
                               sourceList[PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD]     = {};
  ptin_mgmd_externalapi_t externalApi; 

  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  }

  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Unable to get external API");
    return FAILURE;
  }

  /* Port must be root */  
  if (externalApi.portType_get(mcastPacket->serviceId,mcastPacket->portId, &portType)!=SUCCESS || portType!=PTIN_MGMD_PORT_TYPE_ROOT)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} This is not a root port (ServiceId=%u portId=%u portType=%u)! Packet silently discarded.",mcastPacket->serviceId,mcastPacket->portId,portType);
    return ERROR;
  }

   /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ipPayload;

  if (mcastPacket->family == PTIN_MGMD_AF_INET) /* IGMP Message */
  {
    PTIN_MGMD_GET_BYTE(byteVal, dataPtr);       /* Version/Type */
    PTIN_MGMD_GET_BYTE(maxRespCode, dataPtr);   /* Max Response Code - 8 Bits IGMP*/
    PTIN_MGMD_GET_SHORT(recdChecksum, dataPtr); /* Checksum */
    PTIN_MGMD_GET_ADDR(&ipv4Addr, dataPtr);     /* Group Address */

    ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &ipv4Addr, &groupAddr);
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Dst Addr:[%s], Group Addr:[%s]", ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr,debug_buf), ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf2));

    if (mcastPacket->ipPayload==PTIN_NULLPTR ||  mcastPacket->ipPayloadLength==0)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Either the IP payload is a null pointer or the ip payload length is 0");
      return ERROR;      
    }

    if (mcastPacket->ipPayloadLength > MGMD_IGMPv1v2_HEADER_LENGTH)
    {
#ifdef PTIN_MGMD_ROUTER_ALERT_CHECK
      if (mcastPacket->cbHandle->snoopCfgData->igmpv3_tos_rtr_alert_check == TRUE)
      {
        if (mcastPacket->tosByte != PTIN_TOS_VALID_VALUE)
        {
          ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX);
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Packet rec'd with TOS invalid, packet silently discarded");
          return FAILURE;
        }

        if (mcastPacket->routerAlert != TRUE)
        {
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Packet rec'd with Router Alert Option not Active, packet silently discarded");
          return FAILURE;
        }
      }
#endif

      PTIN_MGMD_GET_BYTE(byteVal, dataPtr);  /*Resv+SFlag+QRV - 4 bits + 1 bit + 3 bits*/   
      robustnessVariable = byteVal & 0x07;         
      PTIN_MGMD_GET_BYTE(byteVal, dataPtr);       /* QQIC */
      PTIN_MGMD_GET_SHORT(noOfSources, dataPtr);  /* Number of sources */
      PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Robustness:[%u], QQIC:[%u], noOfSources:[%u]", robustnessVariable, byteVal, noOfSources);
      PTIN_MGMD_UNUSED_PARAM(dataPtr);

      if (mcastPacket->ipPayloadLength != (MGMD_IGMPV3_HEADER_MIN_LENGTH + noOfSources * sizeof(ptin_mgmd_in_addr_t)))
      {
        PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv2 Membership Query Message Length %u, packet silently discarded", mcastPacket->ipPayloadLength);
        return FAILURE;
      }
      incomingVersion = PTIN_IGMP_VERSION_3;

      //Switch interface compatibility mode
      if(mcastPacket->cbHandle->proxyCM[mcastPacket->serviceId].compatibilityMode == PTIN_MGMD_COMPATIBILITY_V2 && 
         ptin_mgmd_proxycmtimer_isRunning(&mcastPacket->cbHandle->proxyCM[mcastPacket->serviceId])==PTIN_MGMD_FALSE)      
      {
        //If we are here, it means that we were configured to operate at IGMPv3.
        mcastPacket->cbHandle->proxyCM[mcastPacket->serviceId].compatibilityMode=PTIN_MGMD_COMPATIBILITY_V3;        
      }   
    }
    else 
    {
      if (maxRespCode == 0)
      {
        if (mcastPacket->ipPayloadLength != MGMD_IGMPv1v2_HEADER_LENGTH)
        {
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv1 Membership Query Message Length: %u, packet silently discarded", mcastPacket->ipPayloadLength);
          return FAILURE;
        }
        incomingVersion = PTIN_IGMP_VERSION_1;   
        //RFC 2236 - Section 4 - Compatibility with IGMPv1 Routers
        //The IGMPv1 router will send General Queries with the Max Response Time set to 0. This MUST be interpreted as a value of 100 (10 seconds).
        maxRespCode=PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL;
      }
      else
      {
        if (mcastPacket->ipPayloadLength != MGMD_IGMPv1v2_HEADER_LENGTH)
        {
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv2 Membership Query Message Length: %u, packet silently discarded", mcastPacket->ipPayloadLength);
          return FAILURE;
        }
  #ifdef PTIN_MGMD_ROUTER_ALERT_CHECK
        if (mcastPacket->cbHandle->snoopCfgData->igmpv3_tos_rtr_alert_check == TRUE)
        {
          if (mcastPacket->routerAlert != TRUE)
          {
            PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} IGMPv2 Membership Query Message rec'd with Router Alert Option not Active, packet silently discarded");
            return FAILURE;
          }
        }
  #endif
        incomingVersion = PTIN_IGMP_VERSION_2;      
      }
        
      //Switch proxy interface compatibility mode
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Setting compatibility mode to IGMPv2 on service [%u]",mcastPacket->serviceId);
      mcastPacket->cbHandle->proxyCM[mcastPacket->serviceId].compatibilityMode=PTIN_MGMD_COMPATIBILITY_V2;  
      if(mcastPacket->cbHandle->mgmdProxyCfg.networkVersion==PTIN_IGMP_VERSION_3)
      {
        ptin_mgmd_proxycmtimer_start(mcastPacket->serviceId, mcastPacket->cbHandle, &mcastPacket->cbHandle->mgmdProxyCfg);
      }
      else
      {
        ptin_mgmd_proxycmtimer_stop(&mcastPacket->cbHandle->proxyCM[mcastPacket->serviceId]);
      }
    }     
  }/* End IGMP pkt check */
#if 0//Snooping
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
    inetAddressSet(PTIN_MGMD_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
  }
#else//Proxy
  else /* MLD Message */
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"IPv6 currently not supported, packet silently discarded");
    return SUCCESS;
#if 0 //We have disable IPv6 processing
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdQueryMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);   /* Version/Type */
    SNOOP_GET_BYTE(byteVal, dataPtr);  /* Code */
    SNOOP_UNUSED_PARAM(byteVal);
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_SHORT(maxRespCode, dataPtr);  /* Max response time */
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Resp Code=%d",maxRespCode);
    SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);/* rserved */

    SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
    inetAddressSet(PTIN_MGMD_AF_INET6, &ipv6Addr, &groupAddr);
    if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH) /* MIN MLD qry length */
    {
      SNOOP_GET_BYTE(mgmdMsg.qqic, dataPtr);  /* QQIC */
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.qqic=%d",mgmdMsg.qqic);
      SNOOP_GET_SHORT(noOfSources, dataPtr);  /* Number of sources */
      SNOOP_UNUSED_PARAM(dataPtr);
      if (mcastPacket->ip_payload_length !=
          (noOfSources * sizeof(ptin_in6_addr_t) + SNOOP_MLDV2_HEADER_MIN_LENGTH))
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Illegal MLDv2 packet length = %d", mcastPacket->ip_payload_length);
        return FAILURE;
      }
      incomingVersion = SNOOP_MLD_VERSION_2;
    }
    else
    {
      if (mcastPacket->ip_payload_length != SNOOP_MLDV1_HEADER_LENGTH)
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Illegal MLDv1 packet length = %d", mcastPacket->ip_payload_length);
        return FAILURE;
      }
      incomingVersion = SNOOP_MLD_VERSION_1;
    }
#endif
  }/* End of MLD Message check */
#endif

  /*Let us verify if we do have any MGMD Host*/
  if (ptin_mgmd_avlTreeCount(&mcastPacket->ebHandle->ptinMgmdGroupAvlTree)==0)
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Membership Query Packet silently ignored: We do not have active MGMD Hosts");
    return SUCCESS;
  }
   
  /* As the packet has the max-respons-time in 1/10 of secs, convert it to milliseconds for further processing */
  maxRespTime = ptin_mgmd_fp_decode_max_resp_code(mcastPacket->family, maxRespCode);
#if 0
  if (maxRespTime == 0)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Max Response Time equal to zero, packet silently discarded");
    return FAILURE;
  }
#endif
  
  /* Calculate the Selected delay */
  selectedDelay = ptinMgmd_generate_random_response_delay(maxRespTime);
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Max_Response_Time:[%u]mS, Selected_Delay:[%u]mS", maxRespTime, selectedDelay);
 
  if (mcastPacket->family == PTIN_MGMD_AF_INET)
  {
    switch (incomingVersion)
    {
    case PTIN_IGMP_VERSION_1:
      {
#if 0
        /* Check if it is general query address or group specific */
        if (inetIsAddressZero(&groupAddr) == TRUE)
        {
          /* Check if IPv4 destination address is same as 224.0.0.1 */
          inetAddressGet(mcastPacket->family, &mcastPacket->destAddr,
                         &ipv4Addr);
          if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
          {
            LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IPv4 dest addr %s!=224.0.0.1",snoopPTinIPv4AddrPrint(mcastPacket->destAddr,debug_buf));
            return FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IGMPv1 General Query" );
          ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
        }
        else /* Should be group specific query */
        {
          if (inetIsInMulticast(&groupAddr) == TRUE)
          {
            if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &groupAddr) != 0)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess:  Ipv4 dst addr != Group Addr %s!=%s"\,
                        snoopPTinIPv4AddrPrint(mcastPacket->destAddr,debug_buf),snoopPTinIPv4AddrPrint(groupAddr,debug_buf));
              return FAILURE;
            }
          }
          else
          {
            LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: Multicast Ipv4 Group Addr Invalid =%s"\,
                      ,snoopPTinIPv4AddrPrint(groupAddr,debug_buf));
            return FAILURE;
          }
          LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IGMPv1 Group Specific Query )";
                   ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);

                   }
                   }
#else
        PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} IGMPv1 Query Rec'd, Packet Silently Ignored");
        return NOT_SUPPORTED;
#endif
        break;
      }
    case PTIN_IGMP_VERSION_2:
      {
#if 1
        /* Check if it is general query address or group specific */
        if (ptin_mgmd_inetIsAddressZero(&groupAddr) == TRUE)
        {
          /* Check if IPv4 destination address is same as 224.0.0.1 */
          ptin_mgmd_inetAddressGet(mcastPacket->family, &mcastPacket->destAddr,
                         &ipv4Addr);
          if (ipv4Addr != PTIN_MGMD_IGMP_ALL_HOSTS_ADDR)
          {
            PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv2 General Query Rec'd:: IPv4 dest addr %s!=224.0.0.1, packet silently discarded",ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr,debug_buf));                        
            return FAILURE;
          }
          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"IGMPv2 General Query Rec'd");    
          queryType=PTIN_IGMP_MEMBERSHIP_QUERY;        
        }
        else /* Should be group specific query */
        {
          if (ptin_mgmd_inetIsInMulticast(&groupAddr) == TRUE)
          {
            if (PTIN_MGMD_INET_IS_ADDR_EQUAL(&mcastPacket->destAddr, &groupAddr) == FALSE)
            {
              PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv2 Group Specific Query Rec'd: IPv4 dst addr != Group Addr %s!=%s, packet silently discarded",
                          ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr,debug_buf),ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf2));                        
              return FAILURE;
            }
          }
          else
          {
            PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv2 Group Specific Query Rec'd: Multicast IPv4 Group Addr Invalid =%s, packet silently discarded",
                        ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf));            
            return FAILURE;
          }
          /*Let us verify if this group is registered by any IGMP Host*/            
          if ((avlTreeEntry=ptinMgmdL3EntryFind(mcastPacket->serviceId,&groupAddr,AVL_EXACT))==PTIN_NULLPTR || 
              avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].active==FALSE)               
          {
            PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Failed to find group for which grp-query is rx'ed: %s. Packet silently ignored.",ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf));
            return SUCCESS;
          }
          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"IGMPv2 Group Specific Query Rec'd");
          queryType=PTIN_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY; 
        }
#else
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"IGMPv2 Query Rec'd, Packet Silently Ignored");
        return NOT_IMPLEMENTED_YET;
#endif
        break;          
      }
    case PTIN_IGMP_VERSION_3:
      {           
        /* Check if it is general query address or group specific */
        if (ptin_mgmd_inetIsAddressZero(&groupAddr) == TRUE)
        {          
          if (noOfSources!=0)
          {
            PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv3 General Query Rec'd: MGroupAddr=0 & NSources=%d, packet silently discarded",noOfSources);            
            return FAILURE;
          }
          /* Check if IPv4 destination address is same as 224.0.0.1 */
          ptin_mgmd_inetAddressGet(mcastPacket->family, &mcastPacket->destAddr,
                         &ipv4Addr);
          if (ipv4Addr != PTIN_MGMD_IGMP_ALL_HOSTS_ADDR)
          {
            PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv3 General Query Rec'd: IPv4 dest addr %s!=224.0.0.1, packet silently discarded",ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr,debug_buf));            
            return FAILURE;
          }
          PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"IGMPv3 General Query Rec'd" );
          queryType=PTIN_IGMP_MEMBERSHIP_QUERY;            
        }
        else /* Should be group or group & source specific query */
        {
          if (ptin_mgmd_inetIsInMulticast(&groupAddr) == TRUE)
          {
            if (PTIN_MGMD_INET_IS_ADDR_EQUAL(&mcastPacket->destAddr, &groupAddr) == TRUE)
            {
              PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv3 Group Specific Query Rec'd: Ipv4 dst addr != Group Addr - %s!=%s, packet silently discarded",
                          ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr,debug_buf),ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf));              
              return FAILURE;
            }
            if (noOfSources==0)
            {
              PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"IGMPv3 Group Specific Query Rec'd");
              queryType=PTIN_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY;              
            }
            else
            {
              PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"IGMPv3 Group & Source Specific Query Rec'd");              
              queryType=PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY;             
            }
          }
          else
          {
            PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Invalid IGMPv3 Group Specific Query Rec'd: Multicast Ipv4 Group Addr Invalid =%s, packet silently discarded",
                        ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf));            
            return FAILURE;
          }

          /*Let us verify if this group is registered by any IGMPv3 Host*/            
          if ((avlTreeEntry=ptinMgmdL3EntryFind(mcastPacket->serviceId,&groupAddr,AVL_EXACT))==PTIN_NULLPTR || 
              avlTreeEntry->ports[PTIN_MGMD_ROOT_PORT].active==FALSE)              
          {
            PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Failed to find group for which grp-query is rx'ed: %s. Packet silently ignored.",ptin_mgmd_inetAddrPrint(&groupAddr,debug_buf));
            return SUCCESS;
          }
        }
        break;
      }
    }
  }
  else/*IPv6*/
  {
#if 0
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&groupAddr) == TRUE)
    {
      uchar8 mldQryAddr[L7_IP6_ADDR_LEN];
      uchar8 ipBuf[L7_IP6_ADDR_LEN];

      memset(mldQryAddr, 0x00, L7_IP6_ADDR_LEN);
      osapiInetPton(PTIN_MGMD_AF_INET6, SNOOP_IP6_ALL_HOSTS_ADDR, mldQryAddr);

      /* Check if it is equal to the all hosts address FF02::1 */
      inetAddressGet(PTIN_MGMD_AF_INET6, &mcastPacket->destAddr, ipBuf);
      if (memcmp(ipBuf, mldQryAddr, L7_IP6_ADDR_LEN) != 0)
      {
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"Invalid Packet");
        return FAILURE;
      }
      queryType=SNOOP_PTIN_GENERAL_QUERY;
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX);
    }
    else /* Should be group specific query */
    {
      if (inetIsInMulticast(&groupAddr) == TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &groupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->family, "Invalid Packet");
          return FAILURE;
        }

      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->family, "Invalid Packet");
        return FAILURE;
      }
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX);
    }
#endif
  }

  if (mcastPacket->family == PTIN_MGMD_AF_INET)
  {
    /* Add new source */
    for (sourceIdx=0;sourceIdx<noOfSources;sourceIdx++)
    {
      memset(&sourceList[sourceIdx], 0x00, sizeof(ptin_mgmd_inet_addr_t));             
      {
        PTIN_MGMD_GET_ADDR(&ipv4Addr, dataPtr);
        ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &ipv4Addr, &sourceList[sourceIdx]);
        if (ptin_mgmd_inetIpAddressValidityCheck(PTIN_MGMD_AF_INET,&sourceList[sourceIdx])!=SUCCESS)
        {
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid Source IP Address %s. Packet silently ignored.",ptin_mgmd_inetAddrPrint(&sourceList[sourceIdx], debug_buf));
          return FAILURE;
        }
      }
    }
  }
  else
  {
#if 0
    /* Add new source */
    for (sourceIdx=0;sourceIdx<noOfSources;sourceIdx++)
    {
      memset(&sourceList[sourceIdx], 0x00, sizeof(ptin_inet_addr_t));
      /* IPv6 MCAST Address */
      SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);
      inetAddressSet(PTIN_MGMD_AF_INET6, &ipv6Addr, &sourceList[sourceIdx]);

      if (inetIpAddressValidityCheck(PTIN_MGMD_AF_INET,&sourceList[sourceIdx])!=SUCCESS)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP, "Invalid Source IP Address %s",inetAddrPrint(&sourceList[sourceIdx], debug_buf));
        return FAILURE;
      }
    }
#endif
  }    

  switch (queryType)
  {
    case PTIN_IGMP_MEMBERSHIP_QUERY://General Query
      {
//Cleanup mechanism to remove any pending State Change Records
        ptinMgmdCleanUpGroupRecordAvlTree(mcastPacket->serviceId);
//End cleanup

        if(mcastPacket->cbHandle->proxyCM[mcastPacket->serviceId].compatibilityMode!=PTIN_MGMD_COMPATIBILITY_V3)
        {        
          if (mgmdBuildIgmpv2CSR(mcastPacket->serviceId,maxRespTime)!=SUCCESS)
          {
            PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Failed mgmdBuildIgmpv2CSR() ");
            return ERROR;
          }
          sendReport=FALSE;
          timeout=maxRespTime;
        }
        else
        {
          ptr=ptinMgmdGeneralQueryProcess(mcastPacket->serviceId, selectedDelay, &sendReport, &timeout);
          isInterface=TRUE;      
        }
        break;
      }

    case PTIN_IGMP_MEMBERSHIP_GROUP_SPECIFIC_QUERY :
      {
        ptr=ptinMgmdGroupSpecifcQueryProcess(avlTreeEntry, selectedDelay,&sendReport, &timeout);      
        break;
      }
    case PTIN_IGMP_MEMBERSHIP_GROUP_AND_SOURCE_SCPECIFC_QUERY:
      {
        ptr=ptinMgmdGroupSourceSpecifcQueryProcess(avlTreeEntry, PTIN_MGMD_ROOT_PORT, noOfSources, sourceList, selectedDelay, &sendReport, &timeout);
        break;
      }
    default:
    {      
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} Invalid Query type");    
      return FAILURE;
    }   
  }  

  if (ptr !=PTIN_NULLPTR && sendReport==TRUE && timeout>=PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL)
  {
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Scheduling Membership Report Message with timeout: %u ",timeout); 

    if (ptinMgmdScheduleReportMessage(mcastPacket->serviceId,&groupAddr,queryType,selectedDelay,isInterface,1, ptr)!=SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"} Failed snoopPTinReportSchedule()");
      return FAILURE;
    }
  }
  else if (ptr ==PTIN_NULLPTR && sendReport==TRUE)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "sendReport Flag is equal to TRUE, while groupPtr=PTIN_NULLPTR");      
  }
  else if (timeout<PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Selected delay smaller than allowed value (%u<1)",timeout);
  }
  else
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "sendReport Flag is equal to FALSE");      
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "} snoopMgmdSrcSpecificMembershipQueryProcess: Query was processed");         
  return SUCCESS;
}


uint8 ptinMgmdRecordType2IGMPStatField(uint8 recordType,uint8 fieldType)
{
  switch (recordType)
  {
  case PTIN_MGMD_MODE_IS_INCLUDE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_MGMD_MODE_IS_EXCLUDE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_MGMD_ALLOW_NEW_SOURCES:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_MGMD_BLOCK_OLD_SOURCES:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_MGMD_CHANGE_TO_INCLUDE_MODE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }

  case PTIN_MGMD_CHANGE_TO_EXCLUDE_MODE:
    switch (fieldType)
    {
    case SNOOP_STAT_FIELD_TX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX;   
    case SNOOP_STAT_FIELD_TOTAL_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX;   
    case SNOOP_STAT_FIELD_VALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX;   
    case SNOOP_STAT_FIELD_INVALID_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX;   
    case SNOOP_STAT_FIELD_DROPPED_RX:
      return SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX;   
    default:
      return SNOOP_STAT_FIELD_ALL;
    }
  default:
    return SNOOP_STAT_FIELD_ALL;
  }
}

/****************************************************************************
* @purpose Process IGMPv3 Group Membership Report
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns SUCCESS
* @returns FAILURE, if invalid group address
*
* @notes none
*
* @end
*
*****************************************************************************/
RC_t ptin_mgmd_membership_report_v3_process(ptinMgmdControlPkt_t *mcastPacket)
{
  ptin_mgmd_port_type_t     portType;
  uchar8                   *dataPtr;
  uchar8                    recType;
  uchar8                    auxDataLen;
  RC_t                      rc = SUCCESS;
  ptinMgmdGroupInfoData_t    *snoopEntry;
  uint32                    ipv4Addr;
  ushort16                  noOfGroups, noOfSources, numberOfClients=0;  
  uint32                    i;
  char                      debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN];
#if 0
  char                      debug_buf2[PTIN_MGMD_IPV6_DISP_ADDR_LEN];  
#endif
  BOOL                      flagNewGroup = FALSE, 
                            flagAddClient = FALSE, 
                            flagRemoveClient = FALSE;
  BOOL                      groupAlreadyExists=FALSE;
  ptin_mgmd_externalapi_t   externalApi; 

  memset(&sourceList, 0x00, sizeof(sourceList));

  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Membership_Report_v3 type:[0x%X] serviceId:[%u] portId:[%u] clientId:[%u]", mcastPacket->ipPayload[0], mcastPacket->serviceId, mcastPacket->portId, mcastPacket->clientId);

  //Get configurations
  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return ERROR;
  }
  
  //Port must be leaf
  if (externalApi.portType_get(mcastPacket->serviceId,mcastPacket->portId, &portType) != SUCCESS || portType!=PTIN_MGMD_PORT_TYPE_LEAF)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid packet: Received a membership report on a root interface");
    return FAILURE;
  }

  //Validate destination address
  ptin_mgmd_inetAddressGet(PTIN_MGMD_AF_INET, &mcastPacket->destAddr, &ipv4Addr);
  if (ipv4Addr != PTIN_MGMD_IGMPV3_REPORT_ADDR && ipv4Addr!=mcastPacket->cbHandle->mgmdProxyCfg.ipv4_addr)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid packet: Invalid destination address [%s]", ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr, debug_buf));
    return FAILURE;
  }

  //Initialize Group Records Context
  mcastPacket->ebHandle->noOfGroupRecordsToBeSent=0;
  mcastPacket->ebHandle->interfacePtr=PTIN_NULLPTR;
  mcastPacket->ebHandle->groupRecordPtr=PTIN_NULLPTR;
  //End Initialization

  //Move dataPtr to the 'Number of Group Records' field (6 bytes offset)
  dataPtr = mcastPacket->ipPayload + 6;
  PTIN_MGMD_GET_SHORT(noOfGroups, dataPtr);
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Number of Group records [%u]", noOfGroups);

  //We need to fix this in the near future: since it can be used to prevent DOS attacks
  if ( (noOfGroups > mcastPacket->cbHandle->mgmdProxyCfg.host.max_records_per_report) || (noOfGroups == 0) )
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid packet: Received a membership report with an invalid number of group records [%u]", noOfGroups);
    return FAILURE;
  }

  //Check if it is a malformed packet
  if ((uint32)(dataPtr - mcastPacket->ipPayload) > mcastPacket->ipPayloadLength)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid packet: Malformed packet");
    return FAILURE;
  }

  while (noOfGroups > 0)
  {
    ptin_mgmd_inet_addr_t groupAddr = {0}; 

    PTIN_MGMD_GET_BYTE(recType, dataPtr); //Record type    
      
    PTIN_MGMD_GET_BYTE(auxDataLen, dataPtr);   //AuxData Len
    PTIN_MGMD_GET_SHORT(noOfSources, dataPtr); //Number of sources 
    PTIN_MGMD_GET_ADDR(&ipv4Addr, dataPtr);    //Multicast Address
    ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &ipv4Addr, &groupAddr);    
    
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Group Record: recordType:[%u] auxDataLen:[%u] numberOfSources:[%u] groupAddr:[%s]", recType,auxDataLen, noOfSources, ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf));

    if (recType==0 || recType>=MGMD_GROUP_REPORT_TYPE_MAX)
    {
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX);
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid packet: Unknown group record type [%u]", recType);
      return SUCCESS;
    }

    //If we receive a Block{} or an Allow{} without sources we silently discard the packet
    if (noOfSources==0 && (recType==PTIN_MGMD_ALLOW_NEW_SOURCES || recType==PTIN_MGMD_BLOCK_OLD_SOURCES))
    {
      PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Received a group record type:[%u] without any source address. Silently discarded.", recType);
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_INVALID_RX));
      return SUCCESS;
    }

    //If number of sources in this record is higher than PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD we ignore the group record
    //Todo: We need to replace PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD with a configuration parameter!
    if (noOfSources>PTIN_MGMD_MAX_SOURCES || noOfSources > PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD)
    {
      PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Number of sources [%u] is higher than the maximum configured value [%u]. Silently discarded.",noOfSources,PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD);
      //Decrement the number of Group Records 
      --noOfGroups;
      /* Point to the next record */                
      dataPtr += (noOfSources)*4+(auxDataLen * 4);             
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX)); 
      continue;
    }  

    //We assume the behaviour preconized in RFC 5790 Lightweight IGMPv3/MLDv2 
    if(recType==PTIN_MGMD_MODE_IS_EXCLUDE || recType==PTIN_MGMD_CHANGE_TO_EXCLUDE_MODE)
    {
      dataPtr+= (noOfSources)*4;  
      noOfSources=0;                                     
    }    
    else     
    {
      uint16 validSources;
      uint16 srcIdx;
      
      /* Add new source */
      validSources = noOfSources;
      for (i=0, srcIdx=0; i<noOfSources; ++i)
      {
        memset(&sourceList[i], 0x00, sizeof(ptin_mgmd_inet_addr_t));             
        PTIN_MGMD_GET_ADDR(&ipv4Addr, dataPtr);
        ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &ipv4Addr, &sourceList[srcIdx]);

#if 0//Whitelist Support disabled for now!
        //If the white-list filtering is enabled, we must ensure that this is a valid channel
        if(igmpCfg.whitelist == PTIN_MGMD_ENABLE)
        {
          if(PTIN_NULLPTR == ptinMgmdWhitelistSearch(mcastPacket->serviceId, &groupAddr, &sourceList[srcIdx], AVL_EXACT))
          {
            PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Requested channel not found in the white-list [service:%u groupIp:%s sourceIp:%s]", 
                                  mcastPacket->serviceId, ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf), ptin_mgmd_inetAddrPrint(&sourceList[srcIdx], debug_buf2));
            continue;
          }
          else
          {
            PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Requested channel found in the white-list [service:%u groupIp:%s sourceIp:%s]", 
                                mcastPacket->serviceId, ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf), ptin_mgmd_inetAddrPrint(&sourceList[srcIdx], debug_buf2));
          }
        }
#endif

        //I'm not sure that the parsing of the packet should end here if the sourceIp is invalid..
        //However, if in the future we decide to just continue to the next source, we have to ensure that the INVALID counter only increments once for the group-record
        if (ptin_mgmd_inetIpAddressValidityCheck(PTIN_MGMD_AF_INET,&sourceList[i])!=SUCCESS)
        {
          ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_INVALID_RX)); 
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid packet: Invalid Source IP Address [%s]", ptin_mgmd_inetAddrPrint(&sourceList[srcIdx], debug_buf));
          return FAILURE;
        }

        ++srcIdx;
      }    

      //If the white-list is enabled, here is where we check if the group-record contained any invalid channel. If yes, we increment the rx_invalid
      if(mcastPacket->cbHandle->mgmdProxyCfg.whitelist == PTIN_MGMD_ENABLE)
      {
        //If no channel in this group-record is valid, we mark it as dropped and continue to the next group-record
        if( (0 != noOfSources) && (0 == validSources) )
        {
          dataPtr += (noOfSources)*4+(auxDataLen*4);             
          ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX)); 
          continue;
        }

        //If any of the channels is invalid, mark the group-record as invalid. However, continue to process the valid ones
        if(validSources != noOfSources)
        {
          noOfSources = validSources;
          ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_INVALID_RX)); 
        }
      }
    }
   
    /* Create new entry in AVL tree for VLAN+IP if necessary */
    if (PTIN_NULLPTR == (snoopEntry = ptinMgmdL3EntryFind(mcastPacket->serviceId, &groupAddr, AVL_EXACT)))
    {
      //If the Record Type is equal to ToIn{} or Block{S} we ignore this Group Record
      if ( (recType==PTIN_MGMD_CHANGE_TO_INCLUDE_MODE && noOfSources==0) || (recType==PTIN_MGMD_BLOCK_OLD_SOURCES) )
      {   
        PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Received a group record type:[%u], without having any subscribed multicast group. Group record silently discarded.", recType);      
        //Decrement the number of Group Records 
        --noOfGroups;
        /* Point to the next record */                
        dataPtr += (auxDataLen * 4);             
        ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_VALID_RX)); 
        continue;
      }

      if (SUCCESS != ptinMgmdL3EntryAdd(mcastPacket->serviceId,&groupAddr))
      {
        /*Let us verify if we do have space in the AVL Tree full*/
        if (ptin_mgmd_avlTreeCount(&mcastPacket->ebHandle->ptinMgmdGroupAvlTree)<PTIN_MGMD_MAX_GROUPS)
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add new group[%s]/service[%u] entry!", ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf), mcastPacket->serviceId);
          ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_VALID_RX)); 
          return ERROR;          
         }
        else
        {
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add new group[%s]/service[%u]. AVL Tree is full: number of group entries [%u]", ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf), mcastPacket->serviceId,ptin_mgmd_avlTreeCount(&mcastPacket->ebHandle->ptinMgmdGroupAvlTree));
           //Decrement the number of Group Records 
          --noOfGroups;
          /* Point to the next record */                
          dataPtr += (auxDataLen * 4);          
          continue;
        }   
      }
      else
      {
        flagNewGroup=TRUE;
        PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Added new group[%s]/service[%u])", ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf), mcastPacket->serviceId);
      }
      if (PTIN_NULLPTR == (snoopEntry = ptinMgmdL3EntryFind(mcastPacket->serviceId, &groupAddr, AVL_EXACT)))
      {
        ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX)); 
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Something went wrong..Unable to find the newly added group[%s]/service[%u] entry!", ptin_mgmd_inetAddrPrint(&groupAddr, debug_buf), mcastPacket->serviceId);
        return ERROR;
      }
      groupAlreadyExists=FALSE;  
    }
    else
    { 
      if((numberOfClients=snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients)>0 && 
            (mcastPacket->cbHandle->proxyCM[mcastPacket->serviceId].compatibilityMode!=PTIN_MGMD_COMPATIBILITY_V3))
        groupAlreadyExists=TRUE;
      else    
        groupAlreadyExists=FALSE;
      
      //If the Record Type is equal to ToIn{} or Block{S} we ignore this Group Record. 
      //If we are in v2 compatibility mode in this interface and the record type is Block{S} we also ignore this Group Record.
      if ( ((snoopEntry->ports[mcastPacket->portId].numberOfClients==0) && ((recType==PTIN_MGMD_CHANGE_TO_INCLUDE_MODE && noOfSources==0) || (recType==PTIN_MGMD_BLOCK_OLD_SOURCES))) ||
           ((snoopEntry->ports[mcastPacket->portId].groupCMTimer.compatibilityMode == PTIN_MGMD_COMPATIBILITY_V2) && (recType==PTIN_MGMD_BLOCK_OLD_SOURCES)) )
      {         
        PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Received a group record type:[%u] (clients=%u routerCM=%u). Silently discarded.", 
                   recType, snoopEntry->ports[mcastPacket->portId].numberOfClients, snoopEntry->ports[mcastPacket->portId].groupCMTimer.compatibilityMode);      

        //Decrement the number of Group Records 
        --noOfGroups;
        /* Point to the next record */                
        dataPtr += (auxDataLen * 4);     
        ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_VALID_RX)); 
        continue;
      }
    }
    
    /* If Leaf interface is not used, initialize it */
    if (snoopEntry->ports[mcastPacket->portId].active == FALSE)
    {
      ptinMgmdInitializeInterface(snoopEntry, mcastPacket->portId);            
    }
    
    //Switch interface compatibility mode
    if(snoopEntry->ports[mcastPacket->portId].groupCMTimer.compatibilityMode == PTIN_MGMD_COMPATIBILITY_V2 &&  
     ptin_mgmd_routercmtimer_isRunning(&snoopEntry->ports[mcastPacket->portId].groupCMTimer)==PTIN_MGMD_FALSE) 
    {
      snoopEntry->ports[mcastPacket->portId].groupCMTimer.compatibilityMode=PTIN_MGMD_COMPATIBILITY_V3;      
    }  
    
    switch (recType)
    {
    case PTIN_MGMD_MODE_IS_INCLUDE:
      {
#if 0
        if (SUCCESS != (rc=snoopPTinMembershipReportIsIncludeProcess(snoopEntry, mcastPacket->portId, mcastPacket->client_idx, noOfSources, sourceList,&noOfRecords, groupPtr)))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportIsIncludeProcess()");              
        }
        break;
#else //We assume the behaviour preconized in RFC 5790 Lightweight IGMPv3/MLDv2 
        //Once a Is_In{S} is equal to Allow_New_Source{S}. We use the same routine to process both.
        if (SUCCESS != (rc=ptinMgmdMembershipReportAllowProcess(mcastPacket->ebHandle, snoopEntry, mcastPacket->portId, mcastPacket->clientId, noOfSources, sourceList, &mcastPacket->cbHandle->mgmdProxyCfg)))
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportAllowProcess()");              
        }
        break;
#endif
        
      }
    case PTIN_MGMD_MODE_IS_EXCLUDE:
      {
#if 0                    
        if (SUCCESS != (rc=snoopPTinMembershipReportIsExcludeProcess(snoopEntry, mcastPacket->portId, mcastPacket->client_idx, noOfSources, sourceList,&noOfRecords, groupPtr)))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportIsExcludeProcess()");              
        }
        break;
#else//We assume the same behaviour preconized in RFC 5790 Lightweight IGMPv3/MLDv2 
       //Is_Ex{S} -> To_Ex{0} = Join(G)       
        if (SUCCESS != (rc=ptinMgmdMembershipReportToExcludeProcess(mcastPacket->ebHandle, snoopEntry, mcastPacket->portId, mcastPacket->clientId, 0, PTIN_NULLPTR, &mcastPacket->cbHandle->mgmdProxyCfg)))
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportToExcludeProcess()");              
        }
        break;
#endif
      }
    case PTIN_MGMD_CHANGE_TO_INCLUDE_MODE:
      {            
        if (SUCCESS != (rc=ptinMgmdMembershipReportToIncludeProcess(mcastPacket->ebHandle, snoopEntry, mcastPacket->portId, mcastPacket->clientId, noOfSources, sourceList, &mcastPacket->cbHandle->mgmdProxyCfg)))
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportIsIncludeProcess()");              
        }
        break;
      }
    case PTIN_MGMD_CHANGE_TO_EXCLUDE_MODE:
      {    
#if 0  
        if (SUCCESS != (rc=snoopPTinMembershipReportToExcludeProcess(snoopEntry, mcastPacket->portId, mcastPacket->client_idx, noOfSources, sourceList,&noOfRecords, groupPtr)))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportToExcludeProcess()");              
        }
        break;
#else  //We assume the behaviour preconized in RFC 5790 Lightweight IGMPv3/MLDv2 
       //To_Ex{S} -> To_Ex{0} = Join(G)
        if (SUCCESS != (rc=ptinMgmdMembershipReportToExcludeProcess(mcastPacket->ebHandle, snoopEntry, mcastPacket->portId, mcastPacket->clientId, 0, PTIN_NULLPTR, &mcastPacket->cbHandle->mgmdProxyCfg)))
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportToExcludeProcess()");              
        }
        break;
#endif
      }
    case PTIN_MGMD_ALLOW_NEW_SOURCES:
      {            
        if (SUCCESS != (rc=ptinMgmdMembershipReportAllowProcess(mcastPacket->ebHandle, snoopEntry, mcastPacket->portId, mcastPacket->clientId, noOfSources, sourceList, &mcastPacket->cbHandle->mgmdProxyCfg)))
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportAllowProcess()");              
        }
        break;
      }
    case PTIN_MGMD_BLOCK_OLD_SOURCES:
      {            
        if (SUCCESS != (rc=ptinMgmdMembershipReportBlockProcess(snoopEntry, mcastPacket->portId, mcastPacket->clientId, noOfSources, sourceList)))
        {
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportBlockProcess()");
        }
        break;
      }
    default:
      {
        PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unknown record type. Silently ignored");        
      }
    }

    if (rc!=SUCCESS)
    {
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_DROPPED_RX));         

      return rc;
    }
    else
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(recType,SNOOP_STAT_FIELD_VALID_RX)); 

    /* Point to the next record */
    /* RFC 3376 4.2.6, RFC 3810 5.2.6 */
    if (mcastPacket->family == PTIN_MGMD_AF_INET)
    {
      dataPtr += (auxDataLen * 4);
    }
    else
    {
      dataPtr += (auxDataLen * 4);
    }    

    if (flagNewGroup==TRUE && snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients>0)
    {
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId,SNOOP_STAT_FIELD_ACTIVE_GROUPS);
      flagNewGroup=FALSE;
      if (flagAddClient==FALSE)
        flagAddClient=TRUE;
    }
    else if (snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients==0)
    {
      ptin_mgmd_stat_decrement_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId,SNOOP_STAT_FIELD_ACTIVE_GROUPS);
    }
    else if (numberOfClients<snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients)
    {
      if (flagAddClient==FALSE)
        flagAddClient=TRUE;
    }
    else if (numberOfClients>snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients)
    {
      if (flagRemoveClient==FALSE)
        flagRemoveClient=TRUE;
    }

    noOfGroups-=1;    

  } /* end of while loop */

  if (mcastPacket->ebHandle->noOfGroupRecordsToBeSent>0)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
    if (ptinMgmdScheduleReportMessage(mcastPacket->serviceId,&mcastPacket->ebHandle->groupRecordPtr->key.groupAddr,PTIN_IGMP_V3_MEMBERSHIP_REPORT,0,FALSE,mcastPacket->ebHandle->noOfGroupRecordsToBeSent, mcastPacket->ebHandle->groupRecordPtr)!=SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Failed snoopPTinReportSchedule()");
      return ERROR;
    }
  }  
  if (flagAddClient!=flagRemoveClient)
  {
    if (flagAddClient==TRUE)
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId,SNOOP_STAT_FIELD_ACTIVE_CLIENTS);
    else
      ptin_mgmd_stat_decrement_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId,SNOOP_STAT_FIELD_ACTIVE_CLIENTS);
  }

  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"}");
  return SUCCESS;
}


/****************************************************************************
* @purpose Process IGMPv2 Group Membership Report
*
* @param   mcastPacket  @b{(input)} Pointer to data structure to hold
*                                   received packet
*
* @returns SUCCESS
* @returns FAILURE, if invalid group address
*
* @notes none
*
* @end
*
*****************************************************************************/
RC_t ptin_mgmd_membership_report_v2_process(ptinMgmdControlPkt_t *mcastPacket)
{
  ptin_mgmd_port_type_t     portType;
  uchar8                    *dataPtr;
  uchar8                    igmpType;
  RC_t                      rc = SUCCESS;
  ptinMgmdGroupInfoData_t     *snoopEntry = PTIN_NULLPTR;
  uint32                    ipDstAddr, groupAddr;
  ushort16                  numberOfClients = 0;
  ptin_mgmd_inet_addr_t     groupInetAddr;
  char                      debug_buf[PTIN_MGMD_IPV6_DISP_ADDR_LEN];  
  BOOL                      flagNewGroup = FALSE, flagAddClient = FALSE, flagRemoveClient = FALSE;
  ptin_mgmd_externalapi_t   externalApi;   

  igmpType = mcastPacket->ipPayload[0];
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Membership_Report_v2 type:[0x%X] serviceId:[%u] portId:[%u] clientId:[%u]", igmpType, mcastPacket->serviceId, mcastPacket->portId, mcastPacket->clientId);

  //Get configurations
  if(SUCCESS != ptin_mgmd_externalapi_get(&externalApi))
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to get external API");
    return FAILURE;
  }
  
  //Port must be leaf
  if (externalApi.portType_get(mcastPacket->serviceId,mcastPacket->portId, &portType) != SUCCESS || portType!=PTIN_MGMD_PORT_TYPE_LEAF)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid packet: Received a membership report on a root interface");
    return ERROR;
  }

  //Validate destination address (224.0.0.2 for Leave reports; group address for others)
  dataPtr = mcastPacket->ipPayload + 4;
  PTIN_MGMD_GET_ADDR(&groupAddr, dataPtr);
  ptin_mgmd_inetAddressGet(PTIN_MGMD_AF_INET, &mcastPacket->destAddr, &ipDstAddr);
  if(PTIN_IGMP_V2_LEAVE_GROUP == igmpType)
  {
    if( (PTIN_MGMD_IGMP_ALL_ROUTERS_ADDR != ipDstAddr) && (groupAddr != ipDstAddr) && (ipDstAddr!=mcastPacket->cbHandle->mgmdProxyCfg.ipv4_addr))
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid packet: Invalid destination address [%s]", ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr, debug_buf));
      return FAILURE;
    }
  }
  else if(PTIN_IGMP_V2_MEMBERSHIP_REPORT == igmpType)
  {
    if( (groupAddr != ipDstAddr) && (ipDstAddr!=mcastPacket->cbHandle->mgmdProxyCfg.ipv4_addr))
    {
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid packet: Invalid destination address [%s]", ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr, debug_buf));
      return FAILURE;
    }
  }

  //Check if it is a malformed packet
  if ((uint32)(dataPtr - mcastPacket->ipPayload) > mcastPacket->ipPayloadLength)
  {
    PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Invalid packet: Malformed packet");
    return FAILURE;
  }

  //Initialize Group Records Context
  mcastPacket->ebHandle->noOfGroupRecordsToBeSent=0;
  mcastPacket->ebHandle->interfacePtr=PTIN_NULLPTR;
  mcastPacket->ebHandle->groupRecordPtr=PTIN_NULLPTR;
  //End Initialization

  switch(igmpType)
  {
    case PTIN_IGMP_V2_LEAVE_GROUP:
    {
      /*
       * IGMPv2 Leave-Reports are converted to IGMPv3 To-Include Reports without sources (see RFC 3376 7.3.2) 
       */      

      ptin_mgmd_inetAddressSet(PTIN_MGMD_AF_INET, &groupAddr, &groupInetAddr);
      if (PTIN_NULLPTR == (snoopEntry = ptinMgmdL3EntryFind(mcastPacket->serviceId, &groupInetAddr, AVL_EXACT)))
      {
        PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a Leave-Report for a non-existing group[%s]. Silently ignored", ptin_mgmd_inetAddrPrint(&groupInetAddr, debug_buf));
        return SUCCESS;
      }

      numberOfClients = snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients;
    
      if (SUCCESS != (rc = ptinMgmdMembershipReportToIncludeProcess(mcastPacket->ebHandle, snoopEntry, mcastPacket->portId, mcastPacket->clientId, 0, PTIN_NULLPTR, &mcastPacket->cbHandle->mgmdProxyCfg)))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportIsIncludeProcess()");
      }

      break;
    }
    case PTIN_IGMP_V2_MEMBERSHIP_REPORT:
    {
      /*
       * IGMPv2 Membership-Reports are converted to IGMPv3 To-Exclude Reports without sources (see RFC 3376 7.3.2) 
       */      

      if (PTIN_NULLPTR == (snoopEntry = ptinMgmdL3EntryFind(mcastPacket->serviceId, &mcastPacket->destAddr, AVL_EXACT)))
      {
        if (SUCCESS != ptinMgmdL3EntryAdd(mcastPacket->serviceId, &mcastPacket->destAddr))
        {
          ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(igmpType, SNOOP_STAT_FIELD_DROPPED_RX));
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unable to add new group[%s]/service[%u] entry!", ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr, debug_buf), mcastPacket->serviceId);
          return ERROR;
        }
        else
        {
          flagNewGroup = TRUE;
          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Added new group[%s]/service[%u])", ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr, debug_buf), mcastPacket->serviceId);
        }
        if (PTIN_NULLPTR == (snoopEntry = ptinMgmdL3EntryFind(mcastPacket->serviceId, &mcastPacket->destAddr, AVL_EXACT)))
        {
          ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(igmpType, SNOOP_STAT_FIELD_DROPPED_RX));
          PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Something went wrong..Unable to find the newly added group[%s]/service[%u] entry!", ptin_mgmd_inetAddrPrint(&mcastPacket->destAddr, debug_buf), mcastPacket->serviceId);
          return ERROR;
        }
      }
      else
      {
        numberOfClients = snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients;
      }

      /* If Leaf interface is not used, initialize it */
      if (snoopEntry->ports[mcastPacket->portId].active == FALSE)
      {
        ptinMgmdInitializeInterface(snoopEntry, mcastPacket->portId);
      }

      if (SUCCESS != (rc = ptinMgmdMembershipReportToExcludeProcess(mcastPacket->ebHandle, snoopEntry, mcastPacket->portId, mcastPacket->clientId, 0, PTIN_NULLPTR, &mcastPacket->cbHandle->mgmdProxyCfg)))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "snoopPTinMembershipReportToExcludeProcess()");
      }

      break;
    }
    default:
      PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid packet: Unknown IGMP type [%u]...Packet silently discarded",igmpType);
      return ERROR;
  }

  //Switch interface compatibility mode
  if (PTIN_NULLPTR != snoopEntry)
  {
    snoopEntry->ports[mcastPacket->portId].groupCMTimer.compatibilityMode = PTIN_MGMD_COMPATIBILITY_V2;
    snoopEntry->ports[mcastPacket->portId].groupCMTimer.groupKey=snoopEntry->ptinMgmdGroupInfoDataKey;
    if(mcastPacket->cbHandle->mgmdProxyCfg.clientVersion==PTIN_IGMP_VERSION_3)
    {
      ptin_mgmd_routercmtimer_start(snoopEntry, mcastPacket->portId, &mcastPacket->cbHandle->mgmdProxyCfg);
    }
    else
    {
      ptin_mgmd_routercmtimer_stop(&snoopEntry->ports[mcastPacket->portId].groupCMTimer);
    }
    PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Setting compatibility mode to v%u", snoopEntry->ports[mcastPacket->portId].groupCMTimer.compatibilityMode);
  }
  else
  {
    PTIN_MGMD_LOG_NOTICE(PTIN_MGMD_LOG_CTX_PTIN_IGMP,"Unable to correctly set compatibility mode");
  }
  
  if (rc != SUCCESS)
  {
    ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(igmpType, SNOOP_STAT_FIELD_DROPPED_RX));    
    return rc;
  }
  else 
  {
    ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, ptinMgmdRecordType2IGMPStatField(igmpType, SNOOP_STAT_FIELD_VALID_RX));
  }

  if (flagNewGroup == TRUE && snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients > 0)
  {
    ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, SNOOP_STAT_FIELD_ACTIVE_GROUPS);
    flagNewGroup = FALSE;
    if (flagAddClient == FALSE) flagAddClient = TRUE;
  }
  else if (snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients == 0)
  {
    ptin_mgmd_stat_decrement_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, SNOOP_STAT_FIELD_ACTIVE_GROUPS);
  }
  else if (numberOfClients < snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients)
  {
    if (flagAddClient == FALSE) flagAddClient = TRUE;
  }
  else if (numberOfClients > snoopEntry->ports[PTIN_MGMD_ROOT_PORT].numberOfClients)
  {
    if (flagRemoveClient == FALSE) flagRemoveClient = TRUE;
  }

  if (mcastPacket->ebHandle->noOfGroupRecordsToBeSent > 0)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Schedule Membership Report Message");
    if (ptinMgmdScheduleReportMessage(mcastPacket->serviceId, &mcastPacket->ebHandle->groupRecordPtr->key.groupAddr, PTIN_IGMP_V3_MEMBERSHIP_REPORT, 0, FALSE, mcastPacket->ebHandle->noOfGroupRecordsToBeSent, mcastPacket->ebHandle->groupRecordPtr) != SUCCESS)
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Failed snoopPTinReportSchedule()");
      return ERROR;
    }
  } 

  if (flagAddClient != flagRemoveClient)
  {
    if (flagAddClient == TRUE)
    {
      ptin_mgmd_stat_increment_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, SNOOP_STAT_FIELD_ACTIVE_CLIENTS);
    }
    else
    {
      ptin_mgmd_stat_decrement_field(mcastPacket->portId, mcastPacket->serviceId, mcastPacket->clientId, SNOOP_STAT_FIELD_ACTIVE_CLIENTS);
    }
  }

  return rc;
}


RC_t ptin_mgmd_event_packet(PTIN_MGMD_EVENT_PACKET_t* eventData)
{
  RC_t res = SUCCESS;

  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "{");
  }
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Context [Length:%u ServiceId:%u PortId:%u ClientId:%u]", eventData->payloadLength, eventData->serviceId, eventData->portId, eventData->clientId);
  res = ptin_mgmd_packet_process(eventData->payload, eventData->payloadLength, eventData->serviceId, eventData->portId, eventData->clientId);

  if(ptin_mgmd_extended_debug)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "}");
  }
  return res;
}

RC_t ptin_mgmd_event_timer(PTIN_MGMD_EVENT_TIMER_t* eventData)
{
  switch(eventData->type)
  {
    case PTIN_MGMD_EVENT_TIMER_TYPE_GROUP:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a group timer event type");
      ptin_mgmd_event_grouptimer((ptinMgmdGroupTimer_t*)eventData->data);

      break;
    }
    case PTIN_MGMD_EVENT_TIMER_TYPE_SOURCE:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a source timer event type");
      ptin_mgmd_event_sourcetimer((ptinMgmdSourcetimer_t*)eventData->data);

      break;
    }
    case PTIN_MGMD_EVENT_TIMER_TYPE_PROXY:
    {     
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a proxy timer event type");
      ptin_mgmd_event_proxytimer((mgmdProxyInterfaceTimer_t*)eventData->data);
      break;
    }
    case PTIN_MGMD_EVENT_TIMER_TYPE_QUERY:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a query timer event type");
      ptin_mgmd_event_querytimer((mgmdPtinQuerierTimerKey_t*)eventData->data);

      break;
    }
    case PTIN_MGMD_EVENT_TIMER_TYPE_GROUPSOURCEQUERY:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a group-source specific timer event type");
      ptin_mgmd_event_groupsourcespecifictimer((groupSourceSpecificQueriesAvlKey_t*)eventData->data);

      break;
    }
    case PTIN_MGMD_EVENT_TIMER_TYPE_ROUTERCM:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a router compatibility mode timer event type");
      ptin_mgmd_event_routercmtimer((snoopPTinCMtimer_t**)eventData->data);

      break;
    }
    case PTIN_MGMD_EVENT_TIMER_TYPE_PROXYCM:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received a proxy compatibility mode timer event type");
      ptin_mgmd_event_proxycmtimer((snoopPTinCMtimer_t**)eventData->data);

      break;
    }    
    default:
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unknown timer event type received");
    }
  }

  return SUCCESS;
}

RC_t ptin_mgmd_event_ctrl(PTIN_MGMD_EVENT_CTRL_t* eventData)
{
  RC_t res = SUCCESS;

  switch (eventData->msgCode)
  {
    case PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_GET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_GET]       [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_mgmd_config_get(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_SET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_PROXY_CONFIG_SET]        [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_MGMD_CONFIG_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_mgmd_config_set(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_GET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_GET]    [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_clientstats_get(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_CLEAR:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_CLIENT_STATS_CLEAR]  [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_clientstats_clear(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_INTF_STATS_GET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_INTF_STATS_GET]      [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_intfstats_get(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_INTF_STATS_CLEAR:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_INTF_STATS_CLEAR]    [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_STATS_REQUEST_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_intfstats_clear(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_ADD:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_ADD]    [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_STATICGROUP_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_STATICGROUP_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_staticChannel_add(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_REMOVE:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_STATIC_GROUP_REMOVE] [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_STATICGROUP_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_STATICGROUP_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_staticChannel_remove(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_GROUPS_GET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_GROUPS_GET]          [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_activegroups_get(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_CLIENT_GROUPS_GET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_CLIENT_GROUPS_GET]   [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_ACTIVEGROUPS_REQUEST_t));
        res = FAILURE;
      }

      if(SUCCESS == res)
      {
        res = ptin_mgmd_ctrl_client_activegroups_get(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_GENERAL_QUERY_ADMIN:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "GL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_GENERAL_QUERY_ADMIN]   [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_QUERY_CONFIG_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_QUERY_CONFIG_t));
        res = FAILURE;
      }
      if(SUCCESS == res)
      {        
        res = ptinMgmdQuerierAdminModeApply(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_GENERAL_QUERY_RESET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "GL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_GENERAL_QUERY_RESET]   [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_QUERY_CONFIG_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_QUERY_CONFIG_t));
        res = FAILURE;
      }
      if(SUCCESS == res)
      {        
        res = ptinMgmdGeneralQuerierReset(eventData);
      }
      break;
    }
    case PTIN_MGMD_EVENT_CTRL_GROUP_CLIENTS_GET:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "GL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_GROUP_CLIENTS_GET]     [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_REQUEST_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_GROUPCLIENTS_REQUEST_t));
        res = FAILURE;
      }
      if(SUCCESS == res)
      {        
        res = ptin_mgmd_ctrl_clientList_get(eventData);
      }
      break;
    }  
    case PTIN_MGMD_EVENT_CTRL_WHITELIST_ADD:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "GL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_WHITELIST_ADD]         [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));
        res = FAILURE;
      }
      if(SUCCESS == res)
      {        
        res = ptin_mgmd_ctrl_whitelist_add(eventData);
      }
      break;
    } 
    case PTIN_MGMD_EVENT_CTRL_WHITELIST_REMOVE:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "GL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_WHITELIST_REMOVE]      [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_WHITELIST_CONFIG_t));
        res = FAILURE;
      }
      if(SUCCESS == res)
      {        
        res = ptin_mgmd_ctrl_whitelist_remove(eventData);
      }
      break;
    } 
    case PTIN_MGMD_EVENT_CTRL_SERVICE_REMOVE:
    {
      PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "GL Msg [Code: 0x%04X - PTIN_MGMD_EVENT_CTRL_SERVICE_REMOVE]      [ID: 0x%08X]", eventData->msgCode, eventData->msgId);

      //Validate message size
      if(eventData->dataLength != sizeof(PTIN_MGMD_CTRL_SERVICE_REMOVE_t))
      {
        PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Received message[%u bytes] does not have the expected size[%u bytes]", eventData->dataLength, sizeof(PTIN_MGMD_CTRL_SERVICE_REMOVE_t));
        res = FAILURE;
      }
      if(SUCCESS == res)
      {        
        res = ptin_mgmd_ctrl_service_remove(eventData);
      }
      break;
    } 
    default:
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "CTRL Msg [Code: 0x%04X] [ID: 0x%08X] - Unknown message", eventData->msgCode, eventData->msgId);
      res = FAILURE;
      break;
    }
  }

  eventData->res = res;
  PTIN_MGMD_LOG_DEBUG(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Returning to CTRL with res[%u]", eventData->res);
  return res;
}

RC_t ptin_mgmd_event_debug(PTIN_MGMD_EVENT_DEBUG_t* eventData)
{
  RC_t res = SUCCESS;

  switch(eventData->type)
  {
    case PTIN_MGMD_EVENT_IGMP_DEBUG_LOG_LVL:
    {
      ptin_mgmd_logseverity_set(PTIN_MGMD_LOG_CTX_PTIN_IGMP, eventData->param[0]);
      if(ptin_mgmd_extended_debug!= (BOOL) (eventData->param[1]))
      {
        if((BOOL)eventData->param[1]==TRUE || (BOOL)(eventData->param[1])==FALSE)
        {
          PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Setting ptin_mgmd_extendedDebug to :%u", eventData->param[1]);
          ptin_mgmd_extended_debug = (BOOL)(eventData->param[1]);
        }
        else
        {
          PTIN_MGMD_LOG_WARNING(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Invalid input parameter for ptin_mgmd_extendedDebug :%u", (BOOL)(eventData->param[1]));
        }
      }
      break;
    }
    case PTIN_MGMD_EVENT_TIMER_DEBUG_LOG_LVL:
    {
      ptin_mgmd_log_sev_set(1 << PTIN_MGMD_LOG_CTX_PTIN_TIMER, eventData->param[0]);      
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_MCAST_GROUP_PRINT:
    {
      ptinMgmdMcastgroupPrint(eventData->param[0], eventData->param[1]);
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_MCAST_GROUP_DUMP:
    {
      ptinMgmdDumpL3AvlTree();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_MCAST_GROUP_CLEAN:
    {
      ptinMgmdCleanAllGroupAvlTree();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_GROUP_RECORDS_DUMP:
    {
      ptinMgmdDumpGroupRecordAvlTree();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_GROUP_RECORDS_CLEAN:
    {
      ptinMgmdCleanAllGroupRecordAvlTree();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_WHITELIST_DUMP:
    {      
      ptinMgmdWhitelistDump();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_WHITELIST_CLEAN:
    {     
      ptinMgmdWhitelistClean();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_MEASUREMENT_TIMERS_DUMP:
    {
      ptin_mgmd_measurement_timer_dump();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_MEMORY_REPORT_DUMP:
    {
      ptin_mgmd_memory_report();
      break;
    }
    case PTIN_MGMD_EVENT_DEBUG_GENERAL_QUERY_DUMP:
    {
      ptinMgmdDumpGeneralQuery();
      break;
    }    
    default:
    {
      PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_IGMP, "Unknown request received");

      break;
    }
  }

  return res;
}

