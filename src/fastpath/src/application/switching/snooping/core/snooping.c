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
#include "l7_mgmd_api.h"

/* PTin added: IGMP snooping */
#if 1
#include "snooping_ptin_db.h"
#include "snooping_ptin_defs.h"
#include "logger.h" // DFF
#include "ptin_igmp.h"
#include "ptin_debug.h"

/* PTin Add: IGMPv3 */
#if SNOOP_PTIN_IGMPv3_GLOBAL

#include"rng_api.h"//MMELO
#include"osapi.h"//MMELO

#if SNOOP_PTIN_IGMPv3_PROXY
#include "snooping_ptin_proxy_grouptimer.h"//MMELO
#include "snooping_ptin_proxy_sourcetimer.h"//MMELO
#include "snooping_ptin_proxy_interfacetimer.h"//MMELO
//#include "snooping_ptin_util.h"//MMELO
#endif 


#endif


extern ptin_debug_pktTimer_t debug_pktTimer;
#endif




/* PTin added: IGMPv3 snooping */
#if 0/*SNOOP_PTIN_IGMPv3_PROXY*/

#if 0

void timer_expired(L7_uint32 mine, L7_uint32 other)
{
  LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Callback do timer foi chamado");
}
#endif



/************************************************************************************************************/
/*PTin Added: MGMD Proxy*/


//TODO: MMELO
//static L7_uint8 MulticastRouterVersion;

/*****************************************************************
* @purpose  calculates the max response time from the max response code
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

static L7_int32  snoopPTinProxy_decode_max_resp_code(L7_uchar8 family, L7_int32 max_resp_code)
{
  L7_int32           max_resp_time = 0;

  if (max_resp_code < 0x80)
  {
    max_resp_time= max_resp_code;
  }
  else
  {
    max_resp_time= ((max_resp_code & 0x0F) | 0x10) << (((max_resp_code & 0x70) >>4)+3);
  }

  if (family == L7_AF_INET)
  {
   max_resp_time = max_resp_time/10;
  }
  else if (family == L7_AF_INET6)
  {
    max_resp_time = max_resp_time/1000;
  }
  return max_resp_time;
}

/*****************************************************************
* @purpose  calculates the selected delay from the max response time
*
* @param    max_resp_time @b{ (input) } the maximum response time
*
* @returns  the selected delay for a
   response is randomly selected in the range (0, [Max Resp Time]) where
   Max Resp Time is derived from Max Resp Code in the received Query
   message.
* @notes  
*

* @end
*********************************************************************/


static L7_int32 snoopPTinProxy_selected_delay_calculate (L7_int32 max_resp_time)
{
  L7_int32 selectedDelay = 0;
  extern L7_double64 L7_Random(void);
  L7_double64 randval;

 /* Initialize the random number generator */

  srand(time(NULL));

  randInitRNG((L7_long32)(rand()));

  randval =  L7_Random();

  selectedDelay= max_resp_time *randval;
  LOG_DEBUG(LOG_CTX_PTIN_IGMP,
    "selectedDelay %d  randval %e ",selectedDelay, randval);
  /* report suppression override*/
  if (selectedDelay == 0)
    selectedDelay = 1;
  return (selectedDelay);
}

/*End PTin Added: MGMD Proxy*/
/************************************************************************************************************/
#endif 

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

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopIGMPPktIntercept");

  debug_pktTimer.pkt_intercept_counter++;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  protPtr = (data + sysNetDataOffsetGet(data)) + SNOOP_IP_HDR_NEXT_PROTO_OFFSET;

  if (*protPtr == IP_PROT_IGMP || *protPtr == IP_PROT_PIM)
  {
    if (snoopPacketHandle(bufHandle, pduInfo, L7_AF_INET) == L7_SUCCESS)
    {
      return SYSNET_PDU_RC_COPIED;
    }
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
  if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
  {
    return SYSNET_PDU_RC_IGNORED;
  }

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

  if (pSnoopEB->ipv6OptionsSupport == L7_TRUE)
  {

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
  }
  else
  {
    if ((*protoPtr == IP_PROT_ICMPV6 && 
         (*dataPtr == L7_MLD_MEMBERSHIP_QUERY || 
          *dataPtr == L7_MLD_V1_MEMBERSHIP_REPORT || 
          *dataPtr == L7_MLD_V1_MEMBERSHIP_DONE ||
          *dataPtr == L7_MLD_V2_MEMBERSHIP_REPORT))||
        (*protoPtr == SNOOP_IP6_IPPROTO_HOPOPTS &&
         (*dataPtr == L7_MLD_MEMBERSHIP_QUERY ||
          *dataPtr == L7_MLD_V2_MEMBERSHIP_REPORT))
       )
    {
      if (snoopPacketHandle(bufHandle, pduInfo, L7_AF_INET6) == L7_SUCCESS)
      { 
        return SYSNET_PDU_RC_COPIED;
      }
    } /* End of valid MLD Packet check */
  }

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
                          sysnet_pdu_info_t *pduInfo,
                          L7_uchar8 family)
{
  snoopPDU_Msg_t   msg;
  L7_uchar8       *data;
  L7_uint32        dataLength;
  L7_uint32        dot1qMode;
  L7_INTF_TYPES_t  sysIntfType;
  L7_RC_t          rc;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_uint32        client_idx;              /* PTin added: IGMP snooping */

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
#if SNOOP_PTIN_IGMPv3_PROXY
  char             debug_buf[IPV6_DISP_ADDR_LEN];  
#endif
#endif  
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopPacketHandle");

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

#if 1
  LOG_TRACE(LOG_CTX_PTIN_IGMP,
                "Packet intercepted vlan %d, innerVlan=%u, intIfNum %d, rx_port=%d",
              pduInfo->vlanId, pduInfo->innerVlanId, pduInfo->intIfNum, pduInfo->rxPort);
#else
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Packet intercepted vlan %d, innerVlan=%u, intIfNum %d",
              pduInfo->vlanId, pduInfo->innerVlanId, pduInfo->intIfNum);
#endif

  /* Ensure snooping is enabled on the switch */
  if (pSnoopCB->snoopCfgData->snoopAdminMode != L7_ENABLE)
  {
    return L7_FAILURE; /* In case of failure, the buffer is freed by DTL */
  }

  /* Validate intIfNum */
  if (pduInfo->intIfNum >= PTIN_SYSTEM_MAXINTERFACES_PER_GROUP)
  {
    if (ptin_debug_igmp_snooping)
      LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid intIfNum %u: out or range (max=%u)", pduInfo->intIfNum, PTIN_SYSTEM_MAXINTERFACES_PER_GROUP-1);
    return L7_FAILURE;
  }

  /* Get interface type */
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
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
      return L7_FAILURE;
    }

    /* Verify that the receiving interface is valid */
    if (snoopIntfCanBeEnabled(pduInfo->intIfNum, pduInfo->vlanId) != L7_TRUE)
    {
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);
      ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
      return L7_FAILURE;
    }
  }

  /* Get start and length of incoming frame */
  SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
              data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11]);

  /* PTin added: IGMP snooping */
  #if 1
  L7_uint16 McastRootVlan;

  /* Internal vlan will be converted to MC root vlan */

  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_inet_addr_t srcAddr, grpAddr;

  memset(&srcAddr, 0x00, sizeof(srcAddr));
  memset(&srcAddr, 0x00, sizeof(grpAddr));

  /* Default is IPv4 */
  srcAddr.family = L7_AF_INET;
  grpAddr.family = L7_AF_INET;

  /* IGMP */
  if (pSnoopCB->family == L7_AF_INET)
  {
    L7_uchar8 *buffPtr;
    L7_uint16 ipHdrLen;

    /* Validate minimum size of packet */
    if (dataLength < L7_ENET_HDR_SIZE + L7_ENET_HDR_TYPE_LEN_SIZE + L7_IP_HDR_LEN + SNOOP_IGMPv1v2_HEADER_LENGTH)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Received pkt is too small %d",dataLength);
      return L7_FAILURE;
    }

    /* Extract source and group address from packet */

    /* Point to the start of ethernet payload */
    buffPtr = (L7_uchar8 *)(data + sysNetDataOffsetGet(data));

    ipHdrLen = (buffPtr[0] & 0x0f)*4;
    if ( ipHdrLen < L7_IP_HDR_LEN )
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "IP Header Len is invalid %d",ipHdrLen);
      return L7_FAILURE;
    }

    /* Source address */
    srcAddr.family = L7_AF_INET;
    srcAddr.addr.ipv4.s_addr = *((L7_uint32 *) &buffPtr[12]);

    /* Group address */
    grpAddr.family = L7_AF_INET;
    grpAddr.addr.ipv4.s_addr = *(L7_uint32 *) ((L7_uint8 *) &buffPtr[24] + (ipHdrLen - L7_IP_HDR_LEN));
       }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: IPv6 not supported yet!");
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_FAILURE;
  }

  /* Get multicast root vlan */
  if (ptin_igmp_McastRootVlan_get(&grpAddr, &srcAddr, pduInfo->vlanId, &McastRootVlan)==L7_SUCCESS)
  {
#if SNOOP_PTIN_IGMPv3_PROXY 
    LOG_TRACE(LOG_CTX_PTIN_IGMP,
                "snoopPacketHandle: Vlan=%u converted to %u (grpAddr=%s srcAddr=%s)",
                pduInfo->vlanId, McastRootVlan, inetAddrPrint(&grpAddr,debug_buf),inetAddrPrint(&srcAddr,debug_buf));
#else
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family,
                "snoopPacketHandle: Vlan=%u converted to %u (grpAddr=0x%08x srcAddr=0x%08x)",
                pduInfo->vlanId, McastRootVlan,grpAddr.addr.ipv4.s_addr, srcAddr.addr.ipv4.s_addr);
#endif
    
    pduInfo->vlanId = McastRootVlan;
  }
  else
  {
#if SNOOP_PTIN_IGMPv3_PROXY 
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,
                "snoopPacketHandle: Can't get McastRootVlan for vlan=%u (grpAddr=%s srcAddr=%s)",
                pduInfo->vlanId, inetAddrPrint(&grpAddr,debug_buf) , inetAddrPrint(&srcAddr,debug_buf));
#else
    SNOOP_DEBUG(SNOOP_DEBUG_PROTO, pSnoopCB->family,
                "snoopPacketHandle: Can't get McastRootVlan for vlan=%u (grpAddr=0x%08x srcAddr=0x%08x)",
                pduInfo->vlanId, grpAddr.addr.ipv4.s_addr, srcAddr.addr.ipv4.s_addr);
#endif
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_FAILURE;
  }
  #else
  /* !IGMPASSOC_MULTI_MC_SUPPORTED */
  if (ptin_igmp_McastRootVlan_get(pduInfo->vlanId, &McastRootVlan)==L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Vlan=%u converted to %u",pduInfo->vlanId,McastRootVlan);
    pduInfo->vlanId = McastRootVlan;
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Can't get McastRootVlan for vlan=%u",pduInfo->vlanId);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, (L7_uint16)-1, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_FAILURE;
  }
  #endif

  /* Validate interface and vlan */
  if (ptin_igmp_intfVlan_validate(pduInfo->intIfNum,pduInfo->vlanId)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: intIfNum=%u,vlan=%u are not accepted",pduInfo->intIfNum,pduInfo->vlanId);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: intIfNum=%u,vlan=%u accepted",pduInfo->intIfNum,pduInfo->vlanId);

  client_idx = (L7_uint32) -1;

  /* Validate client information */
  #if ( !PTIN_BOARD_IS_MATRIX )
  L7_BOOL   unstacked_service = L7_FALSE;

  #ifndef IGMPASSOC_MULTI_MC_SUPPORTED
  /* Check if MC service is unstacked. If it is, clients will be dynamic */
  if (ptin_igmp_vlan_UC_is_unstacked(pduInfo->vlanId, &unstacked_service)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: intIfNum=%u,vlan=%u are not accepted",pduInfo->intIfNum,pduInfo->vlanId);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, SNOOP_STAT_FIELD_IGMP_DROPPED);
    return L7_FAILURE;
  }
  #endif

  /* Only for linecards, clients are identified with the inner vlan (matrix are ports) */
  if ( (ptin_igmp_clientIntfVlan_validate( pduInfo->intIfNum, pduInfo->vlanId) == L7_SUCCESS) &&
       (!unstacked_service) && (pduInfo->innerVlanId != 0) )
  {
    ptin_client_id_t client;
    L7_uchar8 *smac = &data[L7_MAC_ADDR_LEN];

    /* Search for the static client */
    /* Client information */
    client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
    client.outerVlan = pduInfo->vlanId;
    client.innerVlan = pduInfo->innerVlanId;
    client.ipv4_addr = 0;
    memcpy(client.macAddr,smac,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
    client.mask = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

    if (ptin_igmp_clientIndex_get(pduInfo->intIfNum, pduInfo->vlanId, &client, &client_idx)!=L7_SUCCESS)
    {
      client_idx = (L7_uint) -1;
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: ptin_igmp_clientIndex_get failed");
    }
  }
  #endif

  /* Validate client index */
  if (client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    client_idx = (L7_uint) -1;
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Client not provided!");
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Client index is %u",client_idx);
  }

  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet intercepted at intIfNum=%u, oVlan=%u, iVlan=%u",
            pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);

  /* Intercepted packet */
  ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_INTERCEPTED);

  /* Change Internal vlan with the MC root vlan inside message */
  if ((*(L7_ushort16 *)&data[12] == 0x8100) ||
      (*(L7_ushort16 *)&data[12] == 0x88A8) ||
      (*(L7_ushort16 *)&data[12] == 0x9100))
  {
    data[14] &= 0xf0;
    data[14] |= (msg.vlanId>>8) & 0x0f;
    data[15]  =  msg.vlanId & 0xff;
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: vlan changed inside packet");
  }
  #endif

  memset((L7_uchar8 *)&msg, 0, sizeof(msg));
  msg.msgId    = snoopPDUMsgRecv;
  msg.intIfNum = pduInfo->intIfNum;
  msg.innerVlanId = pduInfo->innerVlanId;
  msg.vlanId   = /*osapiNtohl already done by hapiRx function*/(pduInfo->vlanId);
  msg.cbHandle = pSnoopCB;
  msg.client_idx = client_idx;          /* PTin added: IGMP snooping */

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
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Insufficient buffers");
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    ptin_igmp_dynamic_client_flush(pduInfo->vlanId,client_idx);
    return L7_FAILURE;
  }

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Going to send message to queue");

  L7_int32 n_msg = -1;
  if (osapiMsgQueueGetNumMsgs(pSnoopCB->snoopExec->snoopIGMPQueue, &n_msg)==L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Size of IGMP queue = %u messages",n_msg);
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Error reading IGMP queue size");
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
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: osapiMessageSend failed\n");
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, pSnoopCB->family, "snoopPacketHandle: Message sent to queue");
    if (osapiSemaGive(pSnoopCB->snoopExec->snoopMsgQSema) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
              "snoopPacketHandle: Failed to give msgQueue semaphore");
    }
  }

  /* If any error, packet will be dropped */
  if (rc!=L7_SUCCESS)
  {
#if SNOOP_PTIN_IGMPv3_PROXY
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,
                "If any error, packet will be dropped");
#endif
    ptin_igmp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    ptin_igmp_dynamic_client_flush(pduInfo->vlanId,client_idx);
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
  L7_uint32      /*intfMode,*/ xtenHdrLen;
  L7_uint32      messageLen, ipHdrLen/*,  vlanMode*/;
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
SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
    return L7_FAILURE;
  }

  /* Flood the packet if snooping is not enabled in either the VLAN or on the
     interface */
  if (msg->vlanId < L7_DOT1Q_MIN_VLAN_ID || msg->vlanId > L7_DOT1Q_MAX_VLAN_ID)
  {
    snoopDebugPacketRxTrace(mcastPacket, SNOOP_PKT_DROP_BAD_VLAN);
    return L7_FAILURE;
  }
  else
  {
    /* PTin removed */
    #if 0
    vlanMode = (mcastPacket->cbHandle->snoopCfgData->snoopVlanMode[msg->vlanId] & SNOOP_VLAN_MODE) 
               ? L7_ENABLE : L7_DISABLE ;
    if (vlanMode != L7_ENABLE)
    {
      if (snoopIntfModeGet(msg->intIfNum, msg->vlanId, &intfMode,
                           mcastPacket->cbHandle->family) == L7_SUCCESS)
      {
        if (intfMode != L7_ENABLE)
        {
SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
          snoopPacketFlood(mcastPacket);
          return L7_FAILURE;
        }
      }
      else
      {
        return L7_FAILURE;
      } /* End of else of intf mode get */
    } /* End of VLAN Mode check */
    #endif
  }/* End of VLAN Mode Get */

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

    /* Required as per RFC 3376*/
    if (ip_header.iph_ttl != SNOOP_TTL_VALID_VALUE)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Received pkt with TTL other than one %d",\
                  ip_header.iph_ttl);
      return L7_FAILURE;
    }

    /* Save tos field Required as per RFC 3376*/
    mcastPacket->tosByte = ip_header.iph_tos;

    /* Calculate the IP header length, including any IP options */
    ipHdrLen = ((ip_header.iph_versLen & 0x0f) * 4);

    /*check for router alert option  Required as per RFC 3376**/
    if ((ipHdrLen > L7_IP_HDR_LEN) &&  (*buffPtr ==IGMP_IP_ROUTER_ALERT_TYPE))
    {
      mcastPacket->routerAlert =L7_TRUE;
    }
    else
    {
      mcastPacket->routerAlert =L7_FALSE;
    }

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
  else
  { /* MLD */
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

    /* Not received with an IPv6 hop limit as 1, discard.
       RFC3810 section 5  */
    if (ip6_header.hoplim != SNOOP_TTL_VALID_VALUE)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "Received pkt with hop limit other than one %d",\
                  ip6_header.hoplim);
      return L7_FAILURE;
    }

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
    mcastPacket->routerAlert = L7_FALSE;
    if (ip6_header.next == SNOOP_IP6_IPPROTO_HOPOPTS)
    {
      SNOOP_GET_BYTE(ip6_header.next, buffPtr); /* Next Header */
      SNOOP_GET_BYTE(byteVal, buffPtr); /* Xtension hdr length */
      xtenHdrLen = SNOOP_IP6_HOPBHOP_LEN_GET(byteVal);
      buffPtr += xtenHdrLen - 2;
      mcastPacket->routerAlert = L7_TRUE;
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
  ptin_IgmpProxyCfg_t          igmpCfg;
  //L7_BOOL                      checkForTOS = L7_FALSE;

  #if 1
  if (msg->client_idx == (L7_uint32) -1)
  {
    /* Apenas se a interface e LEAF */
    if (ptin_igmp_clientIntfVlan_validate(msg->intIfNum,msg->vlanId)==L7_SUCCESS)
    {
      #if ( PTIN_BOARD_IS_MATRIX )
      ptin_client_id_t  client;

      SNOOP_TRACE(SNOOP_DEBUG_PROTO, msg->cbHandle->family, "snoopPacketHandle: Going to add dynamically a client");

      /* Search for the dynamic client */
        /* Client information */
      client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
      client.outerVlan = msg->vlanId;
      client.innerVlan = msg->innerVlanId;
      client.ipv4_addr = 0;
      memcpy(client.macAddr,&msg->snoopBuffer[L7_MAC_ADDR_LEN],sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
      client.mask = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_MACADDR;

      /* If the client does not exist, it will be created in dynamic mode */
      if (ptin_igmp_dynamic_client_add(msg->intIfNum, msg->vlanId, &client, &msg->client_idx)!=L7_SUCCESS)
      {
        msg->client_idx = (L7_uint) -1;
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, msg->cbHandle->family, "snoopPacketHandle: intIfNum=%u,vlan=%u are not accepted",msg->intIfNum,msg->vlanId);
      }
      #else
      if (ptin_debug_igmp_snooping)
        LOG_ERR(LOG_CTX_PTIN_IGMP, "No dynamic clients allowed for leafs of linecards");
      return L7_FAILURE;
      #endif
    }
  }

  /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }
  #endif

  memset(&mcastPacket, 0x00, sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle = msg->cbHandle;
  mcastPacket.intIfNum = msg->intIfNum;
  mcastPacket.vlanId   = msg->vlanId;
  mcastPacket.innerVlanId = msg->innerVlanId;
  mcastPacket.client_idx = msg->client_idx;       /* PTin added: IGMP snooping */

  /*Check to see whether the interface is still attached or whether it has been
    detached/deleted by the time this packet is up for processing (now)*/
  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;
  nimQueryData.intIfNum = msg->intIfNum;

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket.cbHandle->family, "snoopPacketProcess: Packet copied... OuterVlan %d InnerVlan:%d intIfNum %d",
              mcastPacket.vlanId, mcastPacket.innerVlanId, mcastPacket.intIfNum);

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS ||
      (nimQueryData.data.state != L7_INTF_ATTACHED &&
       nimQueryData.data.state != L7_INTF_ATTACHING))
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    snoopDebugPacketRxTrace(&mcastPacket, SNOOP_PKT_DROP_NOT_READY);
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);
    return L7_SUCCESS;
  }

  /* Validate incoming VLAN ID */
  if ((pSnoopOperEntry = snoopOperEntryGet(msg->vlanId, msg->cbHandle, L7_MATCH_EXACT))
      == L7_NULLPTR)
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    snoopDebugPacketRxTrace(&mcastPacket, SNOOP_PKT_DROP_BAD_VLAN);
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);
    return L7_SUCCESS;
  }

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket.cbHandle->family, "snoopPacketProcess: Packet is valid");

  /*------------------------------------*/
  /*   Process the buffer               */
  /*------------------------------------*/
  if (snoopPacketParse(msg, &mcastPacket) != L7_SUCCESS)
  {
    /* Free the buffer */
    bufferPoolFree(msg->snoopBufferPoolId, msg->snoopBuffer);
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_DROPPED);
    ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);
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
       
#if SNOOP_PTIN_IGMPv3_PROXY  //PTin Added
      if(igmpType==L7_IGMP_MEMBERSHIP_QUERY || igmpType==L7_IGMP_V3_MEMBERSHIP_REPORT || igmpType==L7_IGMP_V2_MEMBERSHIP_REPORT || igmpType==L7_IGMP_V1_MEMBERSHIP_REPORT)
      {
          /* Validate total length value */
        if (mcastPacket.length < IGMP_PKT_MIN_LENGTH)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopPacketProcess: IGMP Packet Length is invalid =%d",mcastPacket.length);
          return L7_FAILURE;;
        }
      }
//We need to decide either if we do this verification or if we enable it to be configureable via the Management Interface
//    /* "Reference: RFC3376 - Section 4. Do not accept IGMPv3 Report or Query
//     *  messages without the IP TOS set to 0xC0."
//     *  However this check is performed only if IGMP is configured to.
//     */
//    if (fixme->checkForTOS == L7_TRUE)
//    {
//      if (type == L7_IGMP_V3_MEMBERSHIP_REPORT)
//      {
//        checkForTOS = L7_TRUE;
//      }
//      else if (type == L7_IGMP_MEMBERSHIP_QUERY)
//      {
//        if (message->length >= IGMP_V3_PKT_MIN_LENGTH)
//        {
//          checkForTOS = L7_TRUE;
//        }
//      }
//      else
//      {
//        /* Do Nothing */
//      }
//    }
//
//    if (checkForTOS == L7_TRUE)
//    {
//      if (((mcastPacket.tosByte) & (SNOOP_IP_TOS)) != SNOOP_IP_TOS)
//      {
//        LOG_DEBUG (LOG_CTX_PTIN_IGMP, "IGMPv3 Message received with Invalid ToS - 0x%x, "
//                    "Discarding it.\n", message->ipTypeOfService);
//        return;
//      }
//    }
#endif//End PTin Added
      switch (igmpType)
      {
      case L7_IGMP_MEMBERSHIP_QUERY:
#if SNOOP_PTIN_IGMPv3_PROXY 
        if (igmpCfg.networkVersion!=3)
        {
#endif
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_QUERY: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.innerVlanId,
                  mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.destMac[4],mcastPacket.payLoad[5],
                  mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
          rc = snoopMgmdMembershipQueryProcess(&mcastPacket);
#if SNOOP_PTIN_IGMPv3_PROXY 
          }
        else if (igmpCfg.networkVersion==3)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_QUERYv3: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.innerVlanId,
                  mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.destMac[4],mcastPacket.payLoad[5],
                  mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
          rc = snoopMgmdSrcSpecificMembershipQueryProcess(&mcastPacket);
        }
#endif
        break;      
        case L7_IGMP_V3_MEMBERSHIP_REPORT:
          
#if SNOOP_PTIN_IGMPv3_ROUTER
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_REPORTv3: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                    mcastPacket.intIfNum,mcastPacket.vlanId,mcastPacket.innerVlanId,
                    mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.payLoad[4],mcastPacket.payLoad[5],
                    mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
          rc = snoopMgmdSrcSpecificMembershipReportProcess(&mcastPacket);
          break;
#else
        LOG_WARNING(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_REPORTv3 Rec'd: Operating on v2 Only!");
#endif
        case L7_IGMP_V1_MEMBERSHIP_REPORT:
        case L7_IGMP_V2_MEMBERSHIP_REPORT:
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"MEMBERSHIP_REPORT: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                    mcastPacket.intIfNum,mcastPacket.vlanId,mcastPacket.innerVlanId,
                    mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.payLoad[4],mcastPacket.payLoad[5],
                    mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
          ptin_timer_start(0,"snoopPacketProcess-snoopMgmdMembershipReportProcess");
          rc = snoopMgmdMembershipReportProcess(&mcastPacket);
          ptin_timer_stop(0);
          break;
        case L7_IGMP_V2_LEAVE_GROUP:
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"LEAVE_GROUP: intIfNum=%u, oVlan=%u, iVlan=%u, DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                    mcastPacket.intIfNum,mcastPacket.vlanId,mcastPacket.innerVlanId,
                    mcastPacket.payLoad[0],mcastPacket.payLoad[1],mcastPacket.payLoad[2],mcastPacket.payLoad[3],mcastPacket.payLoad[4],mcastPacket.payLoad[5],
                    mcastPacket.payLoad[6],mcastPacket.payLoad[7],mcastPacket.payLoad[8],mcastPacket.payLoad[9],mcastPacket.payLoad[10],mcastPacket.payLoad[11]);
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
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
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
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
          rc = snoopPacketFlood(&mcastPacket);
        }
      }
      else
      {
        /* Unknown packet type flood on the VLAN */
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
        rc = snoopPacketFlood(&mcastPacket);
      }
    } /* end of non igmp packet processing */
  } /* End of IPv4 multicast control packet processing */
  else
  { /* IPv6 Multicast Control Packet */
    switch (mcastPacket.msgType)
    {
      case L7_MLD_MEMBERSHIP_QUERY: /* Query */
        rc = snoopMgmdMembershipQueryProcess(&mcastPacket);
        break;

      case L7_MLD_V1_MEMBERSHIP_REPORT: /* Report */
        rc = snoopMgmdMembershipReportProcess(&mcastPacket);
        break;

      case L7_MLD_V2_MEMBERSHIP_REPORT:
        rc = L7_SUCCESS; //snoopMgmdSrcSpecificMembershipReportProcess(&mcastPacket);
        break;

      case L7_MLD_V1_MEMBERSHIP_DONE: /* Done */
        rc = snoopMgmdLeaveGroupProcess(&mcastPacket);
        break;

      default:
        /* Snooping switch MUST forward all unrecognized
          messages... so, flood on all ports in the VLAN except
          the incoming intf */
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
        rc = snoopPacketFlood(&mcastPacket);
        break;
    } /* end of switch case */
  }/* end of IPv6 Multicast Control Packet processing */

  /* PTin added: IGMP snooping */
  #if 1
  if (rc==L7_SUCCESS)
    debug_pktTimer.pktOk_counter++;
  else
    debug_pktTimer.pktEr_counter++;

  debug_pktTimer.time_end = osapiTimeMillisecondsGet();
  #endif

  if (rc==L7_SUCCESS)
  {
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID);
  }
  else
  {
    ptin_igmp_stat_increment_field(mcastPacket.intIfNum, mcastPacket.vlanId, mcastPacket.client_idx, SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID);
  }

  /* If client is dynamic, and no channels are associated, flush it */
  ptin_igmp_dynamic_client_flush(mcastPacket.vlanId, mcastPacket.client_idx);

  return rc;
}

/* PTin removed: IGMP snooping */
#if 0
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
  L7_BOOL         generalQry = L7_FALSE;

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
  else
  { /* MLD Message */
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
      generalQry = L7_TRUE;
    }
    else
    { /* Should be group specific query */
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
      /* Check if IPv4 destination address is same as 224.0.0.1 */
      inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                     &ipv4Addr);
      if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
    else
    { /* Should be group specific query */
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

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }
  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  if (inetIsAddressZero(&mcastPacket->srcAddr) != L7_TRUE)
  {
    /* If ipv6 options header is not supported in hardware.
       In such case we only process MLDv2 General queries with hop options */
    if (mcastPacket->cbHandle->family == L7_AF_INET6 &&
        mcastPacket->cbHandle->snoopExec->ipv6OptionsSupport == L7_FALSE &&
        mcastPacket->routerAlert == L7_TRUE)
    {
      if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH &&
          generalQry == L7_TRUE)
      {
        if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
        {
          /* PTin removed: IGMP snooping */
          #if 0
          snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                               SNOOP_DYNAMIC, mcastPacket->cbHandle);
          #endif
        }
      }
      else if (generalQry == L7_FALSE)
      {
        /* Do not flood group specific queries as they are switched in 
           hardware and are not copied to CPU. If we flood then two copies
           are created for the packet. */
        rc = L7_FAILURE;
      }
    }
    else
    {
      if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
      {
        /* PTin removed: IGMP snooping */
        #if 0
        snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                             SNOOP_DYNAMIC, mcastPacket->cbHandle);
        #endif
      }
    }
  }
  /* Forward membership query on all interfaces on this VLAN, except the
     incoming interface */
  if (rc == L7_SUCCESS)
  {
SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
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
  L7_BOOL          fwdFlag = L7_TRUE;
  L7_mgmdMsg_t     mgmdMsg;
  L7_uchar8        dmac[L7_MAC_ADDR_LEN], byteVal;
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_in6_addr_t    ipv6Addr;

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);

    if (mgmdMsg.mgmdType == L7_IGMP_V2_MEMBERSHIP_REPORT &&
        pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  { /* MLD Message */
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
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Check to see if the group address is in the 224.0.0.X range.  If it is, don't
     process it... just forward to all ports in the vlan except incoming, otherwise
     it to the table */
  fwdFlag = inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr);

  if (fwdFlag == L7_FALSE)
  {
    /* Does an entry with the same MAC addr and VLAN ID already exist? */
    if (snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,
                       L7_MATCH_EXACT) == L7_NULLPTR)
    {
      /* Entry does not exist... give back the semaphore and create new entry */
      rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family);
      if (rc == L7_SUCCESS)
      {
        /* Add all multicast router interfaces to this entry */
        snoopAllMcastRtrIntfsAdd(dmac, mcastPacket->vlanId,
                                 mcastPacket->intIfNum, mcastPacket->cbHandle);
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopEntryCreate failed. Table full\n");
      }
    }

    /* If the entry already existed or it was just successfully created, add the interface */
    if (rc == L7_SUCCESS)
    {
      rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                        SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                "snoopMgmdMembershipReportProcess: snoopIntfAdd failed");
      }
    }
    /* Forward membership report to multicast routers we've detected */
    snoopPacketRtrIntfsForward(mcastPacket);
  }
  else
  {
    /* Flood it in the VLAN as it is for link local multicast address */
SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
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
  L7_uint32               ipv4Addr, timerValue, intfFastLeaveMode;
  L7_BOOL   fastLeaveMode;
  L7_ushort16             noOfGroups, noOfSources;
  L7_inet_addr_t          groupAddr;
  L7_BOOL                 fwdFlag = L7_FALSE, floodFlag = L7_TRUE;
  snoop_cb_t             *pSnoopCB = L7_NULLPTR;
  L7_uchar8               ipBuf[16];

  /* Set pointer to IGMPv3 Membership Report */
  dataPtr = mcastPacket->ip_payload + MGMD_REPORT_NUM_SOURCES_OFFSET;
  SNOOP_GET_SHORT(noOfGroups, dataPtr);
  /* Data ptr will point to the start if group records */

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  if (mcastPacket->cbHandle->family == L7_AF_INET)
  {
    /* IGMP */
    inetAddressGet(L7_AF_INET, &mcastPacket->destAddr, &ipv4Addr);
    if (ipv4Addr != L7_IP_IGMPV3_REPORT_ADDR)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Invalid destination IPv4 address\n");
      return L7_FAILURE;
    }

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->tosByte != SNOOP_TOS_VALID_VALUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }

      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
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
          fastLeaveMode = (mcastPacket->cbHandle->snoopCfgData->snoopVlanMode[mcastPacket->vlanId] &
                           SNOOP_VLAN_FAST_LEAVE_MODE) ? L7_ENABLE : L7_DISABLE;

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
              if (snoopIntfRemove(dmac, mcastPacket->vlanId,
                                  mcastPacket->intIfNum,SNOOP_GROUP_MEMBERSHIP,
                                  mcastPacket->cbHandle) != L7_SUCCESS)
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                        "snoopMgmdV3MembershipReportProcess: Failed to remove group membership");
              }
            }
          }
          else
          {
            if (snoop_is_timer_running(snoopEntry, mcastPacket->intIfNum,
                                       mcastPacket->vlanId, SNOOP_GROUP_MEMBERSHIP,
                                       mcastPacket->cbHandle) == L7_TRUE)
            {
              if (snoopQuerySend(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->cbHandle,
                                 SNOOP_LEAVE_PROCESS_QUERY, L7_NULLPTR) != L7_SUCCESS)
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                        "snoopMgmdV3MembershipReportProcess: Failed to send group specific query");
              }
            }

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
        if (snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,
                           L7_MATCH_EXACT) == L7_NULLPTR)
        {
          /* Entry does not exist... give back the semaphore and create new entry */
          rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family);
          if (rc == L7_SUCCESS)
          {
            /* Add all multicast router interfaces to this entry */
            snoopAllMcastRtrIntfsAdd(dmac, mcastPacket->vlanId,
                                     mcastPacket->intIfNum,
                                     mcastPacket->cbHandle);
          }
          else
          {
            SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopEntryCreate failed. Table full\n");
          }
        }

        /* If the entry already existed or it was just successfully created, add the interface */
        if (rc == L7_SUCCESS)
        {
          rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                            SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
        }
      }/* End of record type check */
    } /* end of if process packet */

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
    snoopPacketRtrIntfsForward(mcastPacket);
  }
  else
  {
    /* Flood it in the VLAN as it is for link local multicast address */
SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
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
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  L7_BOOL  fastLeaveMode;
  L7_uint32        timerValue, intfFastLeaveMode;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_mgmdMsg_t     mgmdMsg;
  L7_in6_addr_t    ipv6Addr;
  L7_BOOL          fwdFlag = L7_TRUE;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

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

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  { /* MLD Message */
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
                   &ipv4Addr);
    if (ipv4Addr != L7_IP_ALL_RTRS_ADDR)
    {
      if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
      {
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

  /* check if it is a leave for a valid mcast group address */
  if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_FALSE)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* check if frame dmac is same as IP Dest Addr equivalent dmac */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }


  fwdFlag = inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr);

  if (fwdFlag == L7_FALSE)
  {

    /* Does an entry with the same MAC addr and VLAN ID already exist? */
    snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId,
                                mcastPacket->cbHandle->family, L7_MATCH_EXACT);

    if (snoopEntry)
    {
      fastLeaveMode = (mcastPacket->cbHandle->snoopCfgData->snoopVlanMode[mcastPacket->vlanId] &
                       SNOOP_VLAN_FAST_LEAVE_MODE) ? L7_ENABLE : L7_DISABLE;
      if (snoopIntfFastLeaveAdminModeGet(mcastPacket->intIfNum, &intfFastLeaveMode,
                                         mcastPacket->cbHandle->family) != L7_SUCCESS)
      {
        intfFastLeaveMode = L7_DISABLE;
      }

      /* check whether the interface or the VLAN has
       * fast-leave admin mode enabled
       */
      if ((intfFastLeaveMode == L7_ENABLE) ||(fastLeaveMode == L7_ENABLE))
      {
        if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
                                 mcastPacket->intIfNum))
        {
          if (snoopIntfRemove(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                              SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle)
              != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                    "snoopMgmdLeaveGroupProcess: Failed to remove group membership");
          }
        }
      }
      else
      {
        if (snoop_is_timer_running(snoopEntry, mcastPacket->intIfNum,
                                   mcastPacket->vlanId, SNOOP_GROUP_MEMBERSHIP,
                                   mcastPacket->cbHandle) == L7_TRUE)
        {
          if (snoopQuerySend(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->cbHandle,
                             SNOOP_LEAVE_PROCESS_QUERY, L7_NULLPTR) != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                    "snoopMgmdLeaveGroupProcess: Failed to send group specific query");
          }
        }

        /* Entry exists... give back semaphore, set timers and transmit query */
        timerValue = snoopCheckPrecedenceParamGet(mcastPacket->vlanId,
                                                  mcastPacket->intIfNum,
                                                  SNOOP_PARAM_MAX_RESPONSE_TIME,
                                                  snoopEntry->snoopInfoDataKey.family);
        if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
                                 mcastPacket->intIfNum))
        {
          snoopTimerUpdate(snoopEntry, mcastPacket->intIfNum, mcastPacket->vlanId,
                           SNOOP_MAX_RESPONSE, timerValue, mcastPacket->cbHandle);

        }/* End of membership check */
      }
    }
  } /* end of group address validity check */

  if (fwdFlag == L7_FALSE)
    snoopPacketRtrIntfsForward(mcastPacket);
  else
{
SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
}

  /* Update stats */
  pSnoopCB->counters.controlFramesProcessed++;
  return rc;
}
#endif

//static L7_RC_t igmp_packet_respond(mgmdSnoopControlPkt_t *mcastPacket);
static L7_RC_t igmp_packet_general_query_respond(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupIP);

#if 0
static L7_RC_t igmp_generate_packet_and_send(L7_uint32 vlan, L7_uint8 type, L7_uint32 groupIP);
#endif

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
//L7_RC_t         rc = L7_SUCCESS;
  L7_uchar8      *dataPtr = L7_NULL;
  L7_uint32       ipv4Addr;
  L7_in6_addr_t   ipv6Addr;
  L7_mgmdMsg_t    mgmdMsg;
  L7_uchar8       byteVal;
  L7_uchar8       dmac[L7_MAC_ADDR_LEN];
  L7_BOOL         generalQry = L7_FALSE;


  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess");

  /* Interface must be root */
  if (ptin_igmp_rootIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipQueryProcess: This is not a root interface (intIfNum=%u)!",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

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
  else
  { /* MLD Message */
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
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }
      generalQry = L7_TRUE;
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
    }
    else
    { /* Should be group specific query */
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
          return L7_FAILURE;
        }
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);
    }
  }
  else
  {
    /* Check if it is general query address or group specific */
    if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
    {
      /* Check if IPv4 destination address is same as 224.0.0.1 */
      inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                     &ipv4Addr);
      if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
    }
    else
    { /* Should be group specific query */
      if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
      {
        if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
          return L7_FAILURE;
        }
      }
      else
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
        return L7_FAILURE;
      }
      ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);
    }
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
    return L7_FAILURE;
  }
  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Invalid Packet");
    return L7_FAILURE;
  }

  #if 0
  if (inetIsAddressZero(&mcastPacket->srcAddr) != L7_TRUE)
  {
    /* If ipv6 options header is not supported in hardware.
       In such case we only process MLDv2 General queries with hop options */
    if (mcastPacket->cbHandle->family == L7_AF_INET6 &&
        mcastPacket->cbHandle->snoopExec->ipv6OptionsSupport == L7_FALSE &&
        mcastPacket->routerAlert == L7_TRUE)
    {
      if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH &&
          generalQry == L7_TRUE)
      {
        if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
        {
          /* PTin removed: IGMP snooping */
          snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                               SNOOP_DYNAMIC, mcastPacket->cbHandle);
        }
      }
      else if (generalQry == L7_FALSE)
      {
        /* Do not flood group specific queries as they are switched in 
           hardware and are not copied to CPU. If we flood then two copies
           are created for the packet. */
        rc = L7_FAILURE;
      }
    }
    else
    {
      if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
      {
        /* PTin removed: IGMP snooping */
        snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum, 
                             SNOOP_DYNAMIC, mcastPacket->cbHandle);
      }
    }
  }
  #endif

  pSnoopCB->counters.controlFramesProcessed++;

  if (igmp_packet_general_query_respond(mcastPacket, &mgmdMsg.mgmdGroupAddr)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Error responding with channel list");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipQueryProcess: Query was processed");

  #if 0
  /* Forward membership query on all interfaces on this VLAN, except the
     incoming interface */
  if (rc == L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopMgmdMembershipQueryProcess: Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }
  #endif

  return L7_SUCCESS;
}



#if SNOOP_PTIN_IGMPv3_PROXY 
/*********************************************************************
* @purpose Process IGMPv3/MLDv2 Membership query message
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
L7_RC_t snoopMgmdSrcSpecificMembershipQueryProcess(mgmdSnoopControlPkt_t *mcastPacket)
{
  #if 0

  snoop_cb_t                  *pSnoopCB = L7_NULLPTR;

  snoopPTinProxyDBInfoData_t    *snoopEntryProxyDB= L7_NULLPTR;
  snoopPTinProxyGrouprecord_t snoopPTinProxyGrouprecord;
//  snoopPTinProxyGrouprecordInfoData_t *snoopEntryProxyGrouprecord= L7_NULLPTR;

    snoopPTinProxyInterfacetimerInfoData_t *snoopEntryProxyInterfacetimer= L7_NULLPTR;
    snoopPTinProxyGrouptimerInfoData_t *snoopEntryProxyGrouptimer= L7_NULLPTR;
    //snoopPTinProxySourcetimerInfoData_t *snoopEntryProxySourcetimer= L7_NULLPTR;

  
      //L7_RC_t                      rc = L7_SUCCESS;
    L7_uchar8                   *dataPtr = L7_NULL;
    L7_uint32                    ipv4Addr, incomingVersion = 0;
    L7_in6_addr_t                ipv6Addr;
    L7_inet_addr_t               sourceAddr[64];//IGMPv3/MLDv2
    L7_mgmdQueryMsg_t            mgmdMsg;
    L7_uchar8                    byteVal;
    L7_ushort16                  maxRespCode=0,noOfSources=0;
    L7_uchar8                    dmac[L7_MAC_ADDR_LEN];
    L7_BOOL                      generalQry = L7_FALSE;
    L7_uint32                    maxRespTime=0,selectedDelay = 0;
    L7_char8                     queryType=-1/*,mRouterVersion=-1*/;
    char                         debug_buf[IPV6_DISP_ADDR_LEN];

    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Membership Query Message Rec'd");

    /* Interface must be root */
    if (ptin_igmp_rootIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"This is not a root interface (intIfNum=%u)!",mcastPacket->intIfNum);
      return L7_FAILURE;
    }

    /* Get Snoop Control Block */
    pSnoopCB = mcastPacket->cbHandle;
    /* If the src IP is non-zero, add this interface to the multicast router list
       and to all snooping entries */

    /* Set pointer to IGMP message */
    dataPtr = mcastPacket->ip_payload;

    if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Dst Addr %s",snoopPTinIPv4AddrPrint(mcastPacket->destAddr.addr.ipv4.s_addr,debug_buf));

      memset(&mgmdMsg, 0x00, sizeof(L7_mgmdQueryMsg_t));

      SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */

      SNOOP_GET_BYTE(maxRespCode, dataPtr);  /*Max Response Code - 8 Bits IGMP*/
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Resp Code=%d",maxRespCode);

      SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */

      inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Group Addr %s",snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr,debug_buf));

      /* Validate checksum of the recd. packet */
      if (inetChecksum(mcastPacket->ip_payload, mcastPacket->ip_payload_length) != 0)
      {
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet rec'd with invalid checksum: Rec'd Checksum != Calculated Checksum %d=!%d \n", mgmdMsg.mgmdChecksum,inetChecksum(mcastPacket->ip_payload, mcastPacket->ip_payload_length));
        return L7_FAILURE;
      }

      if (mcastPacket->ip_payload_length > SNOOP_IGMPv1v2_HEADER_LENGTH)
      {

  #if 1//igmpv3_tos_rtr_alert_check
        if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
        {
          if (mcastPacket->tosByte != SNOOP_TOS_VALID_VALUE)
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet rec'd with TOS invalid");
            return L7_FAILURE;
          }

          if (mcastPacket->routerAlert != L7_TRUE)
          {
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"Packet rec'd with Router Alert Option not Active");
            return L7_FAILURE;
          }
        }
  #endif

  #if 1
        SNOOP_GET_BYTE(mgmdMsg.sQRV, dataPtr);  /* Robustness Var */
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.sQRV=%d",mgmdMsg.sQRV);
  #else
        SNOOP_PTIN_GET_QRV(mgmdMsg.sQRV, dataPtr);          /*Resv+SFlag+QRV - 4 bits + 1 bit + 3 bits*/
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.sQRV=%d",mgmdMsg.sQRV);
  #endif
        SNOOP_GET_BYTE(mgmdMsg.qqic, dataPtr);  /* QQIC */
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.qqic=%d",mgmdMsg.qqic);
        SNOOP_GET_SHORT(mgmdMsg.numSources, dataPtr);  /* Number of sources */
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.numSources=%d",mgmdMsg.numSources);
        SNOOP_UNUSED_PARAM(dataPtr);
        if (mcastPacket->ip_payload_length !=
            (SNOOP_IGMPV3_HEADER_MIN_LENGTH +
             mgmdMsg.numSources * sizeof(L7_in_addr_t)))
        {
           LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Membership Query Message Length %d", mcastPacket->ip_payload_length);
            return L7_FAILURE;
        }
        incomingVersion = SNOOP_IGMP_VERSION_3;
      }
      else if (maxRespCode == 0)
      {
        if (mcastPacket->ip_payload_length != SNOOP_IGMPv1v2_HEADER_LENGTH)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv1 Membership Query Message Length: %d", mcastPacket->ip_payload_length);
          return L7_FAILURE;
        }
        incomingVersion = SNOOP_IGMP_VERSION_1;
        return L7_SUCCESS;//Fixme: remove me!
      }
      else
      {
        if (mcastPacket->ip_payload_length != SNOOP_IGMPv1v2_HEADER_LENGTH)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Membership Query Message Length: %d", mcastPacket->ip_payload_length);
          return L7_FAILURE;
        }
  #if 1//igmpv3_tos_rtr_alert_check
        if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
        {
          if (mcastPacket->routerAlert != L7_TRUE)
          {
            LOG_DEBUG(LOG_CTX_PTIN_IGMP,"IGMPv2 Membership Query Message rec'd with Router Alert Option not Active");
            return L7_FAILURE;
          }
        }
  #endif
        incomingVersion = SNOOP_IGMP_VERSION_2;
        return L7_SUCCESS;//Fixme: remove me!
      }
    }/* Is IGMP pkt check */
  #if 0//Snooping
    else
    { /* MLD Message */
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
  #else//Proxy
   else /* MLD Message */
    {
      memset(&mgmdMsg, 0x00, sizeof(L7_mgmdQueryMsg_t));
      SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);   /* Version/Type */
      SNOOP_GET_BYTE(byteVal, dataPtr);  /* Code */
      SNOOP_UNUSED_PARAM(byteVal);
      SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
      SNOOP_GET_SHORT(maxRespCode, dataPtr);  /* Max response time */
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Resp Code=%d",maxRespCode);
      SNOOP_GET_SHORT(mgmdMsg.mgmdReserved, dataPtr);               /* rserved */

      SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
      inetAddressSet(L7_AF_INET6, &ipv6Addr, &mgmdMsg.mgmdGroupAddr);
      if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH) /* MIN MLD qry length */
      {
        SNOOP_GET_BYTE(mgmdMsg.sQRV, dataPtr);  /* Robustness Var */
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.sQRV=%d",mgmdMsg.sQRV);
        SNOOP_GET_BYTE(mgmdMsg.qqic, dataPtr);  /* QQIC */
        LOG_TRACE(LOG_CTX_PTIN_IGMP,"mgmdMsg.qqic=%d",mgmdMsg.qqic);
        SNOOP_GET_SHORT(mgmdMsg.numSources, dataPtr);  /* Number of sources */
        SNOOP_UNUSED_PARAM(dataPtr);
        if (mcastPacket->ip_payload_length !=
            (mgmdMsg.numSources * sizeof(L7_in6_addr_t) + SNOOP_MLDV2_HEADER_MIN_LENGTH))
        {
         LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Illegal MLDv2 packet length = %d", mcastPacket->ip_payload_length);
          return L7_FAILURE;
        }
        incomingVersion = SNOOP_MLD_VERSION_2;
      }
      else
      {
        if (mcastPacket->ip_payload_length != SNOOP_MLDV1_HEADER_LENGTH)
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Illegal MLDv1 packet length = %d", mcastPacket->ip_payload_length);
          return L7_FAILURE;
        }
        incomingVersion = SNOOP_MLD_VERSION_1;
      }
    }/* End of MLD Message check */
  #endif

   /* As the packet has the max-respons-time in 1/10 of secs, convert it to seconds
       for further processing */
    maxRespTime = snoopPTinProxy_decode_max_resp_code(mcastPacket->cbHandle->family,maxRespCode);
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Max Response Time=%d",maxRespTime);
    /* Calculate the Selected delay */
    selectedDelay = snoopPTinProxy_selected_delay_calculate((L7_int32) maxRespTime);
    selectedDelay =maxRespTime;
    LOG_TRACE(LOG_CTX_PTIN_IGMP,"Selected Delay=%d",selectedDelay);

    if (mcastPacket->cbHandle->family == L7_AF_INET)/*IPv4*/
      {
      switch (incomingVersion)
      {
        case SNOOP_IGMP_VERSION_1:
        {
  #if 0
          /* Check if it is general query address or group specific */
          if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
          {
            /* Check if IPv4 destination address is same as 224.0.0.1 */
            inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                           &ipv4Addr);
            if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IPv4 dest addr %s!=224.0.0.1",snoopPTinIPv4AddrPrint(mcastPacket->destAddr,debug_buf));
              return L7_FAILURE;
            }
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IGMPv1 General Query" );
            ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
          }
          else
          { /* Should be group specific query */
            if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
            {
              if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
              {
                LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess:  Ipv4 dst addr != Group Addr %s!=%s"\,
                          snoopPTinIPv4AddrPrint(mcastPacket->destAddr,debug_buf),snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr,debug_buf));
                return L7_FAILURE;
              }
            }
            else
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: Multicast Ipv4 Group Addr Invalid =%s"\,
                          ,snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr,debug_buf));
              return L7_FAILURE;
            }
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"snoopMgmdSrcSpecificMembershipQueryProcess: IGMPv1 Group Specific Query )";
            ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);

          }
        }
  #else
          LOG_DEBUG(LOG_CTX_PTIN_IGMP,"IGMPv1 Query Rec'd");
          return L7_FAILURE;

  #endif
        break;
        }
        case SNOOP_IGMP_VERSION_2:
        {
          /* Check if it is general query address or group specific */
          if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
          {
            /* Check if IPv4 destination address is same as 224.0.0.1 */
            inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                           &ipv4Addr);
            if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 General Query Rec'd:: IPv4 dest addr %s!=224.0.0.1",snoopPTinIPv4AddrPrint(mcastPacket->destAddr.addr.ipv4.s_addr,debug_buf));
              return L7_FAILURE;
            }
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMPv2 General Query Rec'd");
            ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
          }
          else
          { /* Should be group specific query */
            if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
            {
              if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
              {
                LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Group Specific Query Rec'd: IPv4 dst addr != Group Addr %s!=%s",
                          snoopPTinIPv4AddrPrint(mcastPacket->destAddr.addr.ipv4.s_addr,debug_buf),snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr,debug_buf));
                return L7_FAILURE;
              }
            }
            else
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv2 Group Specific Query Rec'd: Multicast IPv4 Group Addr Invalid =%s",
                          snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr,debug_buf));
              return L7_FAILURE;
            }
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMPv2 Group Specific Query Rec'd");
            ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);

          }
          break;
        }
        case SNOOP_IGMP_VERSION_3:
        {          
            /* Check if it is general query address or group specific */
          if (inetIsAddressZero(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
          {
            if(mgmdMsg.numSources!=0)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 General Query Rec'd: MGroupAddr=0 & NSources=%d",mgmdMsg.numSources);
              return L7_FAILURE;
            }
            /* Check if IPv4 destination address is same as 224.0.0.1 */
            inetAddressGet(mcastPacket->cbHandle->family, &mcastPacket->destAddr,
                           &ipv4Addr);
            if (ipv4Addr != L7_IP_ALL_HOSTS_ADDR)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 General Query Rec'd: IPv4 dest addr %s!=224.0.0.1",snoopPTinIPv4AddrPrint(mcastPacket->destAddr.addr.ipv4.s_addr,debug_buf));
              return L7_FAILURE;
            }
            LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMPv3 General Query Rec'd" );
            ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);

            /*Let us verify if we do have any IGMPv3 Host*/
            snoop_eb_t         *pSnoopEB;
            if ((pSnoopEB = snoopEBGet()) == L7_NULLPTR)
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Failed to obtain pointer to  snoopEBGet()");
              return L7_FAILURE;
            }
            else if (avlTreeCount(&(pSnoopEB->snoopPTinProxyDBAvlTree))==0)
            {
              LOG_TRACE(LOG_CTX_PTIN_IGMP,"Membership Query Packet silently ignored: We do not have active IGMPv3 Hosts");
              return L7_SUCCESS;
            }           
          }
          else
          { /* Should be group or group & source specific query */
            if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
            {
              if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
              {
                LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 Group Specific Query Rec'd: Ipv4 dst addr != Group Addr - %s!=%s",
                          snoopPTinIPv4AddrPrint(mcastPacket->destAddr.addr.ipv4.s_addr,debug_buf),snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr,debug_buf));
                return L7_FAILURE;
              }
              if (mgmdMsg.numSources==0)
              {
                LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMPv3 Group Specific Query Rec'd");
                ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_GROUP_QUERIES_RECEIVED);

              }
              else
              {
                LOG_TRACE(LOG_CTX_PTIN_IGMP,"IGMPv3 Group & Source Specific Query Rec'd");
                ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_SOURCE_QUERIES_RECEIVED);
              }

            }
            else
            {
              LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Invalid IGMPv3 Group Specific Query Rec'd: Multicast Ipv4 Group Addr Invalid =%s",
                          snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr,debug_buf));
              return L7_FAILURE;
            }

            /*Let us verify if this group is registered by any IGMPv3 Host*/            
            if (L7_NULLPTR == (snoopEntryProxyDB = snoopPTinProxyDBEntryFind(mcastPacket->intIfNum, mcastPacket->vlanId,mgmdMsg.mgmdGroupAddr, L7_MATCH_EXACT)))
            {
              LOG_TRACE(LOG_CTX_PTIN_IGMP,"Failed to find group for which grp-query is rx'ed: %s. Packet silently ignored.",snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr,debug_buf));
              return L7_SUCCESS;
            }
          }
          break;
        }
      }
    }
    else/*IPv6*/
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
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdSrcSpecificMembershipQueryProcess: Invalid Packet");
          return L7_FAILURE;
        }
        generalQry = L7_TRUE;
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED);
      }
      else
      { /* Should be group specific query */
        if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_TRUE)
        {
          if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
          {
            SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdSrcSpecificMembershipQueryProcess: Invalid Packet");
            return L7_FAILURE;
          }
        }
        else
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdSrcSpecificMembershipQueryProcess: Invalid Packet");
          return L7_FAILURE;
        }
        ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED);
      }
    }




    
 L7_uint32 TimeLeft=0;
 L7_BOOL isRunning=L7_FALSE;
 if ((snoopEntryProxyInterfacetimer=snoopPTinProxyInterfacetimerEntryFind(mcastPacket->intIfNum, mcastPacket->vlanId, L7_MATCH_EXACT))!=L7_NULLPTR)
 {
   isRunning=snoop_ptin_proxy_Interfacetimer_isRunning(snoopEntryProxyInterfacetimer->interfacetimer);
   if (isRunning==L7_TRUE && snoopEntryProxyInterfacetimer->interfacetimer->isRunning==L7_TRUE)
   {
       TimeLeft=snoop_ptin_proxy_Interfacetimer_timeleft(snoopEntryProxyInterfacetimer->interfacetimer);   
   }
 }  
  
 
 /**RFC 3376 - 5.2. Action on Reception of a Query*/
     /*     1. If there is a pending response to a previous General Query
  scheduled sooner than the selected delay, no additional response
  needs to be scheduled*/
 if (isRunning==L7_TRUE && selectedDelay>TimeLeft)
 {
   LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",TimeLeft,selectedDelay);
   return L7_SUCCESS;
 }
 switch (queryType)
 {
  /*     2. If the received Query is a General Query, the interface timer is
  used to schedule a response to the General Query after the
  selected delay. Any previously pending response to a General
  Query is canceled.*/
   case SNOOP_PTIN_GENERAL_QUERY:
   {
     if (isRunning==L7_TRUE && selectedDelay<TimeLeft)
     {
       LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft higher then selected delay  %d>%d",TimeLeft,selectedDelay);
       return L7_SUCCESS;/*TODO*/
     }
     else
     {
       LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is no Pending Response to a General Query General, going to generate the CSR List");
       return L7_SUCCESS;/*TODO*/
     }
     if (L7_SUCCESS != snoop_ptin_proxy_Interfacetimer_start(snoopEntryProxyInterfacetimer->interfacetimer, selectedDelay,mgmdMsg.sQRV,snoopEntryProxyInterfacetimer))
     {
       LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to start Interface Timer");
       return L7_FAILURE;
     }
     break;
   }

/*     3. If the received Query is a Group-Specific Query or a Group-and-Source-Specific Query and there is no pending response to a
previous Query for this group, then the group timer is used to
schedule a report. If the received Query is a Group-and-Source-Specific Query, the list of queried sources is recorded to be used
when generating a response.*/

/*     4. If there already is a pending response to a previous Query
scheduled for this group, and either the new Query is a Group-Specific Query or the recorded source-list associated with the
group is empty, then the group source-list is cleared and a single
response is scheduled using the group timer. The new response is
scheduled to be sent at the earliest of the remaining time for the
pending report and the selected delay.*/
   case SNOOP_PTIN_GROUP_SPECIFIC_QUERY:
   {
     /* Find entry in AVL tree for VLAN+IP if necessary */
     if (L7_NULLPTR == (snoopEntryProxyDB = snoopPTinProxyDBEntryFind(rootInterface, mcastPacket->vlanId,mgmdMsg.mgmdGroupAddr, L7_MATCH_EXACT)))
     {
       LOG_TRACE(LOG_CTX_PTIN_IGMP,"Failed to find group for which grp-query is rx'ed: %s. Packet silently ignored.",snoopPTinIPv4AddrPrint(mgmdMsg.mgmdGroupAddr.addr.ipv4.s_addr,debug_buf));
       return L7_SUCCESS;
     }

     if ((snoopEntryProxyGrouptimer=snoopPTinProxyGrouptimerEntryFind(rootInterface,mcastPacket->vlanId, mgmdMsg.mgmdGroupAddr, L7_MATCH_EXACT))!=L7_NULLPTR)
     {
       isRunning=snoop_ptin_proxy_Grouptimer_isRunning(snoopEntryProxyGrouptimer->grouptimer);
       if (isRunning==L7_TRUE && snoopEntryProxyGrouptimer->grouptimer->isRunning==L7_TRUE)
       {
         TimeLeft=snoop_ptin_proxy_Grouptimer_timeleft(snoopEntryProxyGrouptimer->grouptimer);   
       }
     }
     if (isRunning==L7_TRUE && selectedDelay>TimeLeft)
     {
       LOG_TRACE(LOG_CTX_PTIN_IGMP,"There is a Pending Response to a General Query General with timeleft smaller then selected delay  %d<%d: Packet silently ignored",TimeLeft,selectedDelay);
       return L7_SUCCESS;
     }     

     else
     {
       noOfSources=mgmdMsg.numSources;
//     L7_uchar8 activeSources=0x00;
       L7_uint8  numOfActiveSources=0,i;
       
       for (i=0;i<noOfSources;i++)
       {
         /* IGMP Source Address*/
         if (mcastPacket->cbHandle->family == L7_AF_INET)
         {
           SNOOP_GET_ADDR(&ipv4Addr, dataPtr);
           inetAddressSet(L7_AF_INET, &ipv4Addr, &sourceAddr[i]);

           L7_uint8  sourceIdx = 0;           
           if (snoopPTinProxyDBSourceFind(snoopEntryProxyDB->clients,sourceAddr[i],sourceIdx)==L7_SUCCESS)
           {
//           //Add this source to membership reponse
             numOfActiveSources++;
//           activeSources[i]=1;
           }
             else
           {
             continue;
           }
          

         }
         else
         {
         /* MLD Source Address */
           SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);
           inetAddressSet(L7_AF_INET6, &ipv6Addr,&sourceAddr);
         }       
       }
       if (numOfActiveSources>0)
       {
              
        
              
         snoopPTinProxyGrouprecord.recordType=
//       L7_RC_t snoop_ptin_proxy_Sourcetimer_start(snoopPTinProxySourcetimer_t* pTimer, L7_uint32 timeout, L7_uchar8 mgmdsQRV, snoopPTinProxySourcetimerInfoData_t* groupData)
//       L7_RC_t snoop_ptin_proxy_Grouptimer_start(snoopPTinProxyGrouptimer_t* pTimer, L7_uint32 timeout, L7_uchar8 mgmdsQRV, snoopPTinProxyGrouptimerInfoData_t* groupData)
         snoopPTinProxyGrouptimerInfoData_t *snoopPTinProxyGrouptimerInfoData;
         
         if(snoopPTinProxyGrouptimerEntryAdd(rootInterface,mcastPacket->vlanId, mgmdMsg.mgmdGroupAddr)!=L7_SUCCESS)
         {
             //erro
         }
         else
         {
           if((snoopPTinProxyGrouptimerInfoData=snoopPTinProxyGrouptimerEntryFind(rootInterface,mcastPacket->vlanId, mgmdMsg.mgmdGroupAddr,L7_MATCH_EXACT))==L7_NULLPTR)
           {
             //erro
           }
           else
           {
             
             snoopPTinProxyGrouptimerInfoData.numberOfSources=
             for (i=0;i<numOfActiveSources;i++)
             {
               snoopPTinProxyGrouptimerInfoData.sourceAddr[i]=sourceAddr[i];
             }
            snoopPTinProxyGrouptimerInfoData.numberOfSources=numOfActiveSources;             
            snoopPTinProxyGrouptimerInfoData.recordType=snoopEntryProxyDB->filtermode;/*PTIN_SNOOP_FILTERMODE_INCLUDE=L7_IGMP_MODE_IS_INCLUDE & PTIN_SNOOP_FILTERMODE_EXCLUDE=L7_IGMP_MODE_IS_EXCLUDE*/   
         }

               
       }
     }
     break;
   }
/*5. If the received Query is a Group-and-Source-Specific Query and
there is a pending response for this group with a non-empty
source-list, then the group source list is augmented to contain
the list of sources in the new Query and a single response is
scheduled using the group timer. The new response is scheduled to
be sent at the earliest of the remaining time for the pending
report and the selected delay.*/
  case SNOOP_PTIN_GROUP_AND_SOURCE_SPECIFIC_QUERY:
  {
      //StartSourceTimer
    //           snoopPTinProxyDBSourceEntryFind(L7_uint16 interfaceIdx, L7_uint32 vlanId, L7_inet_addr_t groupAddr, L7_inet_addr_t sourceAddr,  L7_uint32 flag);
    break;
  }
 }











  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdSrcSpecificMembershipQueryProcess: Invalid Packet");
    return L7_FAILURE;
  }
  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdSrcSpecificMembershipQueryProcess: Invalid Packet");
    return L7_FAILURE;
  }













  #if 0
  if (inetIsAddressZero(&mcastPacket->srcAddr) != L7_TRUE)
  {
    /* If ipv6 options header is not supported in hardware.
       In such case we only process MLDv2 General queries with hop options */
    if (mcastPacket->cbHandle->family == L7_AF_INET6 &&
        mcastPacket->cbHandle->snoopExec->ipv6OptionsSupport == L7_FALSE &&
        mcastPacket->routerAlert == L7_TRUE)
    {
      if (mcastPacket->ip_payload_length > SNOOP_MLDV1_HEADER_LENGTH &&
          generalQry == L7_TRUE)
      {
        if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
        {
          /* PTin removed: IGMP snooping */
          snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum,
                               SNOOP_DYNAMIC, mcastPacket->cbHandle);
        }
      }
      else if (generalQry == L7_FALSE)
      {
        /* Do not flood group specific queries as they are switched in
           hardware and are not copied to CPU. If we flood then two copies
           are created for the packet. */
        rc = L7_FAILURE;
      }
    }
    else
    {
      if ((rc = snoopQuerierQueryProcess(mcastPacket)) == L7_SUCCESS)
      {
        /* PTin removed: IGMP snooping */
        snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum,
                             SNOOP_DYNAMIC, mcastPacket->cbHandle);
      }
    }
  }
  #endif

  pSnoopCB->counters.controlFramesProcessed++;

  if (igmp_packet_general_query_respond(mcastPacket, &mgmdMsg.mgmdGroupAddr)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdSrcSpecificMembershipQueryProcess: Error responding with channel list");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdSrcSpecificMembershipQueryProcess: Query was processed");

  #if 0
  /* Forward membership query on all interfaces on this VLAN, except the
     incoming interface */
  if (rc == L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopMgmdMembershipQueryProcess: Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }
  #endif
#endif

  return L7_SUCCESS;
}

/*End MGMD Proxy*/
/************************************************************************************************************/
#endif

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
  L7_mgmdMsg_t     mgmdMsg;
  L7_BOOL          fwdFlag = L7_FALSE;
  L7_uchar8        dmac[L7_MAC_ADDR_LEN], byteVal;
  L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_in6_addr_t    ipv6Addr;
  #if 0
  snoopInfoData_t *snoopEntry;
  ptin_McastClient_id_t clientId;
  L7_uint               client_index;
  #endif

  ptin_timer_start(1,"snoopMgmdMembershipReportProcess-start");

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess");

  /* Interface must be client */
  if (ptin_igmp_clientIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: This is not a client interface (intIfNum=%u)!",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

  /* Set pointer to IGMP message */
  dataPtr = mcastPacket->ip_payload;

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

  if (mcastPacket->cbHandle->family == L7_AF_INET) /* IGMP Message */
  {
    memset(&mgmdMsg, 0x00, sizeof(L7_mgmdMsg_t));
    SNOOP_GET_BYTE(mgmdMsg.mgmdType, dataPtr);         /* Version/Type */
    SNOOP_GET_BYTE(mgmdMsg.igmpMaxRespTime, dataPtr);  /* Max response time */
    SNOOP_GET_SHORT(mgmdMsg.mgmdChecksum, dataPtr);    /* Checksum */
    SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
    SNOOP_UNUSED_PARAM(dataPtr);
    inetAddressSet(L7_AF_INET, &ipv4Addr, &mgmdMsg.mgmdGroupAddr);

    if (mgmdMsg.mgmdType == L7_IGMP_V2_MEMBERSHIP_REPORT &&
        pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  { /* MLD Message */
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
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
    return L7_FAILURE;
  }

  /* Match the destination mac and the mac resloved from ipv4 dest addr */
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Invalid Packet");
    return L7_FAILURE;
  }

  #if 0
  /* Client information */
  clientId.ptin_intf.intf_type = clientId.ptin_intf.intf_id = 0;
  clientId.outerVlan = mcastPacket->vlanId;
  clientId.innerVlan = mcastPacket->innerVlanId;
  clientId.ipv4_addr = 0;
  memcpy(clientId.macAddr,dmac,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  clientId.mask = MCAST_CLIENT_MASK_FIELD_INTF | MCAST_CLIENT_MASK_FIELD_INNERVLAN;
  if (ptin_igmp_clientIndex_get(mcastPacket->intIfNum, mcastPacket->vlanId, &clientId, &client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: ptin_igmp_clientIndex_get failed");
    return L7_FAILURE;
  }
  #else
  if (mcastPacket->client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: Client not provided");
    return L7_FAILURE;
  }
  #endif

  pSnoopCB->counters.controlFramesProcessed++;

  /* Check to see if the group address is in the 224.0.0.X range.  If it is, don't
     process it... just forward to all ports in the vlan except incoming, otherwise
     it to the table */
  if (inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Packet is in the 224.0.0.x range... don't process it!");
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED);
    return L7_SUCCESS;
  }

  #if 0
  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  if ((snoopEntry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family, L7_MATCH_EXACT)) == L7_NULLPTR)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoop entry does not exist");
    /* Entry does not exist... give back the semaphore and create new entry */
    rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,L7_FALSE);
    if (rc != L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoopEntryCreate failed. Table full");
      return L7_FAILURE;
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoop entry successfully created!");

    if ((snoopEntry=snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family, L7_MATCH_EXACT)) == L7_NULLPTR)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoopEntryFind failed.");
      return L7_FAILURE;
    }
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: snoop entry now exists");
  }

  /* Only add interfaces, for dynamic entries */
  if (!snoopEntry->staticGroup)
  {
    /* If the entry already existed or it was just successfully created, add the interface */
    rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                      SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
    if (rc != L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoopIntfAdd failed");
      return L7_FAILURE;
    }

    /* Check if channel exists... mark to forward join to network */
    if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Message will be sent to network");
      fwdFlag = L7_TRUE;
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: channel already exists");
    }

    rc = snoopChannelIntfAdd(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr);
    if (rc != L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoopChannelIntfAdd failed");
      return L7_FAILURE;
    }
  }
  /* If is a static group, check if this interface is part of the group */
  else
  {
    if (!snoopEntry->port_list[mcastPacket->intIfNum].active)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: This interface is not part of the static group");
      return L7_FAILURE;
    }
    /* Check if channel exists... It should exist for static entries */
    if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Static channel does not exist");
      return L7_FAILURE;
    }
  }

  /* Add client */
  if (snoopChannelClientAdd(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr,client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoopChannelClientAdd failed");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: client added");
  #endif

  ptin_timer_stop(1);

  ptin_timer_start(2,"snoopMgmdMembershipReportProcess-snoop_client_add_procedure");
  /* Add client */
  if ((rc=snoop_client_add_procedure(dmac, mcastPacket->vlanId,
                                     &mgmdMsg.mgmdGroupAddr,
                                     mcastPacket->client_idx,
                                     mcastPacket->intIfNum,
                                     &fwdFlag))!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdMembershipReportProcess: snoop_client_add_procedure failed");
  }
  ptin_timer_stop(2);

  #if 0
  /* Only if IP channel is new, we forward the JOIN packet */
  if (fwdFlag)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdMembershipReportProcess: Forwarding message to network");
    /* Forward membership report to multicast routers we've detected */
    igmp_packet_respond(mcastPacket);
  }
  #endif

  if (rc==L7_SUCCESS)
  {
    /* Restart client timer */
    ptin_igmp_client_timer_start(mcastPacket->vlanId, mcastPacket->client_idx);
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS);
  }
  else
  {
    ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED);
  }

  return rc;
}

#if SNOOP_PTIN_IGMPv3_ROUTER
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
  snoopPTinL3InfoData_t  *snoopEntry;  
  L7_uint32               ipv4Addr;
  L7_ushort16             noOfGroups, noOfSources;
  L7_inet_addr_t          groupAddr;
  L7_BOOL                 fwdFlag = L7_FALSE, floodFlag = L7_TRUE;
  snoop_cb_t             *pSnoopCB = L7_NULLPTR;
  L7_uchar8               ipBuf[16];

#if 0
  snoopPTinProxyDBInfoData_t          *snoopEntryProxyDB;//PROXY SUPPORTED
  snoopPTinProxyGrouprecordInfoData_t *snoopEntryProxygroupRecord;//PROXY SUPPORTED
  L7_BOOL                              reportFlag=L7_FALSE;//PROXY SUPPORTED
#endif
  /* Set pointer to IGMPv3 Membership Report */
  dataPtr = mcastPacket->ip_payload + MGMD_REPORT_NUM_SOURCES_OFFSET;
  SNOOP_GET_SHORT(noOfGroups, dataPtr);
  /* Data ptr will point to the start if group records */

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

#if SNOOP_PTIN_IGMPv3_PROXY
  L7_INTF_MASK_t rootIntfList;
  L7_uint32      rootIntIfNum;
  L7_uchar8     *dataPtrTmp;
    /* Get Multicast Root Interface */
  if (ptin_igmp_rootIntfs_getList(mcastPacket->vlanId, &rootIntfList) !=L7_SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Failed to get ptin_igmp_rootIntfs_getList() :%d",mcastPacket->vlanId);
    return L7_FAILURE;
  }
  else
  {
    L7_BOOL result;
    L7_INTF_NONZEROMASK(rootIntfList, result);
    if (result==L7_FALSE)
    {
      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Empty root interface list :ptin_igmp_rootIntfs_getList()");
      return L7_FAILURE;
    }
    else
    {
      LOG_TRACE(LOG_CTX_PTIN_IGMP,"ptin_igmp_rootIntfs_getList() :%d",rootIntfList.value);
      L7_INTF_FHMASKBIT(rootIntfList, rootIntIfNum);
      if (rootIntIfNum==0)
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Unable to get root interface Idx: L7_INTF_FHMASKBIT");
        return L7_FAILURE;
      }      
    }
  }
#endif

  if (mcastPacket->cbHandle->family == L7_AF_INET)
  {
    /* IGMP */
    inetAddressGet(L7_AF_INET, &mcastPacket->destAddr, &ipv4Addr);
    if (ipv4Addr != L7_IP_IGMPV3_REPORT_ADDR)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid destination IPv4 address");
      return L7_FAILURE;
    }

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->tosByte != SNOOP_TOS_VALID_VALUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }

      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid Packet");
        return L7_FAILURE;
      }
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
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid destination IPv6 address");
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

  /* Validate client index */
  if (mcastPacket->client_idx >= PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP, "Client not provided  :%d",mcastPacket->client_idx);
    return L7_FAILURE;
  }

  /* Check if it is a malformed packet */
  if ((dataPtr - mcastPacket->ip_payload) > mcastPacket->ip_payload_length)
  {
    LOG_WARNING(LOG_CTX_PTIN_IGMP,"Dropped malformed packet :%d",mcastPacket->cbHandle->family);
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
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Invalid destination multicast mac");
        return L7_FAILURE;
      }
      fwdFlag = inetIsLinkLocalMulticastAddress(&groupAddr);
    }
    else
    {
      fwdFlag = L7_TRUE;
    }

    if (fwdFlag == L7_FALSE)
    {
      /* Create new entry in AVL tree for VLAN+IP if necessary */
      if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(rootIntfList, mcastPacket->vlanId, groupAddr,L7_MATCH_EXACT)))
      {
        if (L7_SUCCESS != snoopPTinL3EntryAdd(rootIntfList, mcastPacket->vlanId,groupAddr))
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Failed to Add L3 Entry");
          return L7_FAILURE;
        }
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopPTinL3EntryAdd(%d,%d,%d)",rootIntfList,groupAddr,mcastPacket->vlanId);
        }
        if (L7_NULLPTR == (snoopEntry = snoopPTinL3EntryFind(rootIntfList, mcastPacket->vlanId, groupAddr,L7_MATCH_EXACT)))
        {
          return L7_FAILURE;
        }
      }
#if 0
      /*Create new entry in ProxyDB for INT+VLAN+GroupAddr if necessary */      
      if (L7_NULLPTR == (snoopEntryProxyDB = snoopPTinProxyDBEntryFind(intIdxUpstream,  mcastPacket->vlanId,groupAddr, L7_MATCH_EXACT)))
      {
        if (L7_SUCCESS != snoopPTinProxyDBEntryAdd(intIdxUpstream,mcastPacket->vlanId, groupAddr))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Add Entry to Proxy DB");
          rc = L7_FAILURE;
        }
        if (L7_NULLPTR == (snoopEntryProxyDB = snoopPTinProxyDBEntryFind(intIdxUpstream,mcastPacket->vlanId,  groupAddr, L7_MATCH_EXACT)))
        {
          LOG_ERR(LOG_CTX_PTIN_IGMP, "Failed to Find Entry to Proxy DB");
          rc = L7_FAILURE;
        }
        snoopEntry->proxyDB=snoopEntryProxyDB;                  
        if (recType==L7_IGMP_MODE_IS_INCLUDE ||  recType==L7_IGMP_CHANGE_TO_INCLUDE_MODE  || recType==L7_IGMP_ALLOW_NEW_SOURCES)
        {
            snoopEntryProxyDB->filtermode=PTIN_SNOOP_FILTERMODE_INCLUDE;
            LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopEntryProxyDB->filtermode=PTIN_SNOOP_FILTERMODE_INCLUDE");
        }
        else if (recType==L7_IGMP_MODE_IS_EXCLUDE ||  recType==L7_IGMP_CHANGE_TO_EXCLUDE_MODE  || recType==L7_IGMP_BLOCK_OLD_SOURCES)
        {
          snoopEntryProxyDB->filtermode=PTIN_SNOOP_FILTERMODE_EXCLUDE;
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopEntryProxyDB->filtermode=PTIN_SNOOP_FILTERMODE_EXCLUDE");
        }
        else
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP, "Unknown record type. Silently ignored");
          return L7_SUCCESS;
        }

        PTIN_PROXY_SET_MASKBIT(snoopEntryProxyDB->clients,mcastPacket->intIfNum);
//Todo        
        //Open Switch port for this interface                     

        LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopPTinProxyDBEntryFind(%d,%d)",groupAddr,mcastPacket->vlanId);
        
        
        else
        {
          LOG_TRACE(LOG_CTX_PTIN_IGMP, "snoopPTinProxyGrouprecordEntryFind(%d)",snoopEntryProxygroupRecord->snoopPTinProxyGrouprecordInfoDataKey.groupAddr);
        }
        /*Forward a Membership Report Message*/
        if (reportFlag==L7_FALSE)
          reportFlag=L7_TRUE;
      }
      else//This Group Address Already exists in the Proxy DB
      {    
        if ((snoopEntryProxyDB->filtermode==PTIN_SNOOP_FILTERMODE_EXCLUDE) &&(recType==L7_IGMP_MODE_IS_INCLUDE ||  recType==L7_IGMP_CHANGE_TO_INCLUDE_MODE  || recType==L7_IGMP_ALLOW_NEW_SOURCES))
        {
          LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Changing Filter Mode :PTIN_SNOOP_FILTERMODE_INCLUDE");
          snoopEntryProxyDB->filtermode=PTIN_SNOOP_FILTERMODE_INCLUDE;
        }
                                
        /*We need to check if the client is active for this interface or not*/ 
        if (PTIN_PROXY_IS_MASKBITSET(snoopEntryProxyDB->clients,mcastPacket->intIfNum)==L7_TRUE)
        {
          continue;/*Don't do anything*/
        }
        else
        {
          PTIN_PROXY_SET_MASKBIT(snoopEntryProxyDB->clients,mcastPacket->intIfNum);
          //Open Switch port for this interface
        }
      }
#endif

      /* If interface is not used, initialize it */
      if (snoopEntry->interfaces[mcastPacket->intIfNum].active == L7_FALSE)
      {
        LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Initializing interface idx: %u", mcastPacket->intIfNum);
        snoopPTinInitializeInterface(&snoopEntry->interfaces[mcastPacket->intIfNum]);         
      }

      floodFlag = L7_FALSE;        

#if SNOOP_PTIN_IGMPv3_PROXY
          dataPtrTmp=dataPtr; 
#endif
  
      switch(recType)
      {
        case L7_IGMP_MODE_IS_INCLUDE:
        { 
#if SNOOP_PTIN_IGMPv3_PROXY              
          snoopPTinMembershipReportIsIncludeProcess(snoopEntry, rootIntIfNum, mcastPacket->client_idx, noOfSources, &dataPtrTmp,L7_TRUE);
          snoopPTinMembershipReportIsIncludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr,L7_FALSE);          
#else
          snoopPTinMembershipReportIsIncludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources,dataPtr);
#endif
          break;
        }
        case L7_IGMP_MODE_IS_EXCLUDE:
        {
#if SNOOP_PTIN_IGMPv3_PROXY              
          snoopPTinMembershipReportIsExcludeProcess(snoopEntry, rootIntIfNum, mcastPacket->client_idx, noOfSources, &dataPtrTmp,L7_TRUE);
          snoopPTinMembershipReportIsExcludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr,L7_FALSE);          
#else
          snoopPTinMembershipReportIsExcludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr);
#endif
          break;
        }
        case L7_IGMP_CHANGE_TO_INCLUDE_MODE:
        {
#if SNOOP_PTIN_IGMPv3_PROXY              
          snoopPTinMembershipReportToIncludeProcess(snoopEntry, rootIntIfNum, mcastPacket->client_idx, noOfSources, &dataPtrTmp,L7_TRUE);
          snoopPTinMembershipReportToIncludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr,L7_FALSE);          
#else          
          snoopPTinMembershipReportToIncludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr);
#endif
          break;
        }
        case L7_IGMP_CHANGE_TO_EXCLUDE_MODE:
        {
#if SNOOP_PTIN_IGMPv3_PROXY              
          snoopPTinMembershipReportToExcludeProcess(snoopEntry, rootIntIfNum, mcastPacket->client_idx, noOfSources, &dataPtrTmp,L7_TRUE);
          snoopPTinMembershipReportToExcludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr,L7_FALSE);          
#else
          snoopPTinMembershipReportToExcludeProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr);
#endif
          break;
        }
        case L7_IGMP_ALLOW_NEW_SOURCES:
        {
#if SNOOP_PTIN_IGMPv3_PROXY              
          snoopPTinMembershipReportAllowProcess(snoopEntry, rootIntIfNum, mcastPacket->client_idx, noOfSources, &dataPtrTmp,L7_TRUE);
          snoopPTinMembershipReportAllowProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr,L7_FALSE);          
#else
          snoopPTinMembershipReportAllowProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr);
#endif
          break;
        }
        case L7_IGMP_BLOCK_OLD_SOURCES:
        {
#if SNOOP_PTIN_IGMPv3_PROXY              
          snoopPTinMembershipReportBlockProcess(snoopEntry, rootIntIfNum, mcastPacket->client_idx, noOfSources, &dataPtrTmp,L7_TRUE);
          snoopPTinMembershipReportBlockProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr,L7_FALSE);          
#else
          snoopPTinMembershipReportBlockProcess(snoopEntry, mcastPacket->intIfNum, mcastPacket->client_idx, noOfSources, &dataPtr);
#endif
          break;
        }
        default:
        {
          LOG_WARNING(LOG_CTX_PTIN_IGMP, "Unknown record type. Silently ignored");
          return L7_SUCCESS;
        }
      }       

#if 0
      if ((noOfSources == 0) && (recType == L7_IGMP_CHANGE_TO_INCLUDE_MODE))
      {
        /* Treat TO_IN( {} ) like an IGMPv2 Leave / MLDv1 Done*/

        /* Does an entry with the same MAC addr and VLAN ID already exist? */
        snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId,
                                    mcastPacket->cbHandle->family, L7_MATCH_EXACT);
        if (snoopEntry)
        {
          fastLeaveMode = (mcastPacket->cbHandle->snoopCfgData->snoopVlanMode[mcastPacket->vlanId] &
                           SNOOP_VLAN_FAST_LEAVE_MODE) ? L7_ENABLE : L7_DISABLE;

          if (snoopIntfFastLeaveAdminModeGet(mcastPacket->intIfNum, &intfFastLeaveMode,
                                             mcastPacket->cbHandle->family) != L7_SUCCESS)
          {
            intfFastLeaveMode = L7_DISABLE;
          }

          /* check whether the interface or the VLAN has fast-leave admin mode enabled */
          if ((intfFastLeaveMode == L7_ENABLE) ||(fastLeaveMode == L7_ENABLE))
          {
//            if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
//                                     mcastPacket->intIfNum))
//            {
//              if (snoopIntfRemove(dmac, mcastPacket->vlanId,
//                                  mcastPacket->intIfNum,SNOOP_GROUP_MEMBERSHIP,
//                                  mcastPacket->cbHandle) != L7_SUCCESS)
//              {
//                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
//                        "snoopMgmdV3MembershipReportProcess: Failed to remove group membership");
//              }
//            }
          }
          else
          {
            if (snoop_is_timer_running(snoopEntry, mcastPacket->intIfNum,
                                       mcastPacket->vlanId, SNOOP_GROUP_MEMBERSHIP,
                                       mcastPacket->cbHandle) == L7_TRUE)
            {
              if (snoopQuerySend(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->cbHandle,
                                 SNOOP_LEAVE_PROCESS_QUERY, L7_NULLPTR) != L7_SUCCESS)
              {
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
                        "snoopMgmdV3MembershipReportProcess: Failed to send group specific query");
              }
            }

//            timerValue = snoopCheckPrecedenceParamGet(mcastPacket->vlanId,mcastPacket->intIfNum,
//                                                      SNOOP_PARAM_MAX_RESPONSE_TIME,
//                                                      snoopEntry->snoopInfoDataKey.family);
//            if (L7_INTF_ISMASKBITSET(snoopEntry->snoopGrpMemberList,
//                                     mcastPacket->intIfNum))
//            {
//              snoopTimerUpdate(snoopEntry, mcastPacket->intIfNum,
//                               mcastPacket->vlanId,
//                               SNOOP_MAX_RESPONSE, timerValue,
//                               mcastPacket->cbHandle);
//            } /* End of group membership check */
            /* Entry exists... give back semaphore, set timers and transmit query */
          }
        }
      }
      else
      {

        /* Does an entry with the same MAC addr and VLAN ID already exist? */
        if (snoopEntryFind(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,
                           L7_MATCH_EXACT) == L7_NULLPTR)
        {
          /* Entry does not exist... give back the semaphore and create new entry */
          rc = snoopEntryCreate(dmac, mcastPacket->vlanId, mcastPacket->cbHandle->family,L7_FALSE);
          if (rc == L7_SUCCESS)
          {
            /* Add all multicast router interfaces to this entry */
            snoopAllMcastRtrIntfsAdd(dmac, mcastPacket->vlanId,
                                     mcastPacket->intIfNum,
                                     mcastPacket->cbHandle);
          }
          else
          {
            SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopEntryCreate failed. Table full");
          }
        }

        /* If the entry already existed or it was just successfully created, add the interface */
        if (rc == L7_SUCCESS)
        {
          rc = snoopIntfAdd(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                            SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle);
        }
      }/* End of record type check */
#endif
    } /* end of if process packet */

    /* Point to the next record */
    /* RFC 3376 4.2.6, RFC 3810 5.2.6 */
    if (mcastPacket->cbHandle->family == L7_AF_INET)
    {
      dataPtr += (auxDataLen * 4);
    }
    else
    {
      dataPtr += (auxDataLen * 4);
    }

    
    noOfGroups -= 1;
  } /* end of while loop */

  /* Forward membership report to multicast routers we've detected */
  if (floodFlag == L7_FALSE)
  {
    snoopPacketRtrIntfsForward(mcastPacket, L7_IGMP_MEMBERSHIP_QUERY);
  }
  else
  {
    /* Flood it in the VLAN as it is for link local multicast address */
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "Flooding packet...");
    snoopPacketFlood(mcastPacket);
  }

  pSnoopCB->counters.controlFramesProcessed++;

  ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3);

  return rc;
}
#endif

#if SNOOP_PTIN_IGMPv3_PROXY 
/**
 * Generate an IGMP packet from scratch, and send it to network 
 * interfaces 
 * 
 * @param vlan    : Internal Vlan
 * @param type    : IGMP packet type
 * @param groupIP : IP channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t igmp_reuse_packet_and_send(mgmdSnoopControlPkt_t *mcastPacket)
{

  L7_RC_t                rc = L7_SUCCESS;
#if 0
  mgmdSnoopControlPkt_t  mcastPacketReport;
  char                         debug_buf[IPV6_DISP_ADDR_LEN];

//L7_INTF_MASK_t intfList;
//L7_uint16 McastRootVlan, intIfNum=0;
  ptin_IgmpProxyCfg_t igmpCfg;

  L7_uchar8 groupsCnt=1;//We are forwarding the first report to this group 
  L7_uchar8            igmpFrame[L7_MAX_FRAME_SIZE]={0};
  L7_uint32             igmpFrameLength=0;

  snoopPTinProxyGrouprecordInfoData_t* groupRecordData=L7_NULLPTR;


  /* Validate arguments */
  if (mcastPacket->vlanId<PTIN_VLAN_MIN || mcastPacket->vlanId>PTIN_VLAN_MAX || mcastPacket==L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "igmp_generate_packet_and_send: Invalid arguments");
    return L7_FAILURE;
  }

  /* Get Proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

  // Initialize mcastPacket structure
#if 0
  memset(&mcastPacketReport,0x00,sizeof(mgmdSnoopControlPkt_t));
#else
  memcpy(&mcastPacketReport,mcastPacket,sizeof(mgmdSnoopControlPkt_t));
#endif
  
#if 0
  snoopPTinProxyGrouprecordInfoData_t* groupRecordData=L7_NULLPTR;

  if ((groupRecordData=snoopPTinProxyGrouprecordEntryFind(mcastPacket->destAddr,recType,L7_MATCH_EXACT)==L7_NULLPTR)
  {
    snoopPTinProxyGrouprecordEntryAdd(mcastPacket->destAddr,recordType)==L7_NULLPTR)


    LOG_ERR(LOG_CTX_PTIN_IGMP, "groupRecordData=NULLPTR :%s",inetAddrPrint(&groupAddr,debug_buf));
    return L7_FAILURE;
  }
  else
  {
    continue;

  }
#endif   

///* Get Snoop Control Block */
//if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
//{
//  LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting pSnoopCB");
//  return;
//}
//  /* Only allow IPv4 for now */
//if (pSnoopCB->family != L7_AF_INET)
//{
//  LOG_ERR(LOG_CTX_PTIN_IGMP, "Not IPv4 packet");
//  return;
//}

 

if (mcastPacket->srcAddr.family==L7_AF_INET)
{
#if 0
 mcastPacket->srcAddr.addr.ipv4=igmpCfg.ipv4_addr.s_addr;
#else
  inetAddressSet(L7_AF_INET, L7_NULL_IP_ADDR, &(mcastPacketReport.srcAddr));
#endif
}
else if (mcastPacket->srcAddr.family==L7_AF_INET6)
{
  inetAddressSet(L7_AF_INET6, L7_NULL_IP_ADDR, &(mcastPacketReport.srcAddr));
}
else
{
  LOG_WARNING(LOG_CTX_PTIN_IGMP,"Unknown IP Address family :%x. Silently ignored",mcastPacket->srcAddr.family);
  return L7_SUCCESS;
}


   /* Get proxy configurations */
  if (ptin_igmp_proxy_config_get(&igmpCfg) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP, "Error getting IGMP Proxy configurations");
    return L7_FAILURE;
  }

//
///* Build header frame for IGMPv3 Report */
//rc = snoopPTinReportFrameV3Build(groupRecordData,  &groupsCnt, igmpFrame, (L7_uint32 *) igmpFrameLength);
//if (rc != L7_SUCCESS)
//{
//  LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Report frame");
//  return L7_FAILURE;
//}
//
///* Build MAC+IP frames and add the IGMP frame to the same packet */
//rc = snoopPTinPacketBuild(mcastPacketReport.vlanId,mcastPacketReport.cbHandle , 0, mcastPacketReport.payLoad, &mcastPacketReport.length, igmpFrame, igmpFrameLength);
//if (rc != L7_SUCCESS)
//{
//  LOG_ERR(LOG_CTX_PTIN_IGMP, "Error building IGMPv3 Report frame");
//  return L7_FAILURE;
//}

  
#if 0
    L7_uint32 i;
  printf("PayloadLength:%d\n",mcastPacket->ip_payload_length);
  for (i=0;i<payloadLen;i++)
    printf("0x%x",mcastPacket->ip_payload[i]);
  printf("\n");
#endif




/////* Get Multicast Root VLAN */
//if (ptin_igmp_McastRootVlan_get(mcastPacket->vlanId, &McastRootVlan)!=L7_SUCCESS)
//{
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Failed to get ptin_igmp_McastRootVlan_get() :%d",mcastPacket->vlanId);
//   return L7_FAILURE;
//}
//else
//{
//  LOG_TRACE(LOG_CTX_PTIN_IGMP,"ptin_igmp_McastRootVlan_get() :%d",McastRootVlan);
//  mcastPacketReport.vlanId=(L7_uint32) McastRootVlan;
//}
//  LOG_TRACE(LOG_CTX_PTIN_IGMP,"Inner vlanID :%d",mcastPacketReport.innerVlanId);

  /* Get Multicast Root Interface */
//if (ptin_igmp_rootIntfs_getList(mcastPacket->vlanId, &intfList) !=L7_SUCCESS)
//{
//  LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Failed to get ptin_igmp_rootIntfs_getList() :%d",mcastPacket->vlanId);
//  return L7_FAILURE;
//}
//else
//{
//  L7_BOOL result;
//  L7_INTF_NONZEROMASK(intfList, result);
//  if (result==L7_FALSE)
//  {
//    LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Empty root interface list :ptin_igmp_rootIntfs_getList()");
//    return L7_FAILURE;
//  }
//  else
//  {
//    LOG_TRACE(LOG_CTX_PTIN_IGMP,"ptin_igmp_rootIntfs_getList() :%d",intfList.value);
//    L7_INTF_FHMASKBIT(intfList, intIfNum);
//    if (intIfNum==0)
//    {
//      LOG_DEBUG(LOG_CTX_PTIN_IGMP,"Unable to get root interface Idx: L7_INTF_FHMASKBIT");
//      return L7_FAILURE;
//    }
//    else
//    {
//      LOG_TRACE(LOG_CTX_PTIN_IGMP,"Root Interface Idx:%d",intIfNum);
//      mcastPacketReport.intIfNum=(L7_uint32) intIfNum;
//
//    }
//  }
//}
  
    
    
    

//// Build answer packet
//ptin_timer_start(27,"igmp_generate_packet_and_send-igmp_rebuild_packet");
//rc=igmp_rebuild_packet(&mcastPacketReport, channel, type);
//ptin_timer_stop(27);
//if (rc==L7_SUCCESS)
//{
    // Send Packet to mrouter ports
  ptin_timer_start(28,"igmp_generate_packet_and_send-snoopPacketRtrIntfsForward");
  rc=snoopPacketRtrIntfsForward(&mcastPacketReport, mcastPacketReport.msgType);
  ptin_timer_stop(28);
  if (rc==L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_PTIN_IGMP,  "snoopPacketRtrIntfsForward Packet transmitted to router interfaces");
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_IGMP, "snoopPacketRtrIntfsForward: Error transmitting to router interfaces");
    return L7_FAILURE;
  }
//}
//else
//{
//  SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Error rebuilding packet");
//}
#endif

  return rc;
}
#endif

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
  //L7_RC_t          rc = L7_SUCCESS;
  L7_uint32        ipv4Addr;
  //L7_BOOL  fastLeaveMode;
  //L7_uint32        timerValue, intfFastLeaveMode;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  L7_mgmdMsg_t     mgmdMsg;
  L7_in6_addr_t    ipv6Addr;
  L7_BOOL          fwdFlag = L7_FALSE;
  #if 0
  snoopInfoData_t *snoopEntry;
  ptin_McastClient_id_t clientId;
  L7_uint               client_index;
  #endif

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess");

  /* Interface must be client */
  if (ptin_igmp_clientIntfVlan_validate(mcastPacket->intIfNum, mcastPacket->vlanId)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: This is not a client interface (intIfNum=%u)!",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

  /* Get Snoop Control Block */
  pSnoopCB = mcastPacket->cbHandle;

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

    if (pSnoopCB->snoopCfgData->igmpv3_tos_rtr_alert_check == L7_TRUE)
    {
      if (mcastPacket->routerAlert != L7_TRUE)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
        return L7_FAILURE;
      }
    }
  }
  else
  { /* MLD Message */
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
                   &ipv4Addr);
    if (ipv4Addr != L7_IP_ALL_RTRS_ADDR)
    {
      if (L7_INET_ADDR_COMPARE(&mcastPacket->destAddr, &mgmdMsg.mgmdGroupAddr) != 0)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
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
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET6, "snoopMgmdLeaveGroupProcess: Invalid destination IPv6 address");
      return L7_FAILURE;
    }
  }

  /* check if it is a leave for a valid mcast group address */
  if (inetIsInMulticast(&mgmdMsg.mgmdGroupAddr) == L7_FALSE)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
    return L7_FAILURE;
  }

  /* check if frame dmac is same as IP Dest Addr equivalent dmac */
  memset(dmac, 0x00, sizeof(dmac));
  snoopMulticastMacFromIpAddr(&mcastPacket->destAddr, dmac);
  if (memcmp(mcastPacket->destMac, dmac, L7_MAC_ADDR_LEN) != 0)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
    return L7_FAILURE;
  }

  memset(dmac, 0x00, sizeof(dmac));
  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&mgmdMsg.mgmdGroupAddr, dmac);
  if (snoopMacAddrCheck(dmac, mcastPacket->cbHandle->family) != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "snoopMgmdLeaveGroupProcess: Invalid Packet");
    return L7_FAILURE;
  }

  if (inetIsLinkLocalMulticastAddress(&mgmdMsg.mgmdGroupAddr))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: Packet is in the 224.0.0.x range... don't process it!");
    return L7_SUCCESS;
  }

  #if 0
  /* Client information */
  clientId.ptin_intf.intf_type = clientId.ptin_intf.intf_id = 0;
  clientId.outerVlan = mcastPacket->vlanId;
  clientId.innerVlan = mcastPacket->innerVlanId;
  clientId.ipv4_addr = 0;
  memcpy(clientId.macAddr,dmac,sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
  clientId.mask = MCAST_CLIENT_MASK_FIELD_INTF | MCAST_CLIENT_MASK_FIELD_INNERVLAN;
  if (ptin_igmp_clientIndex_get(mcastPacket->intIfNum, mcastPacket->vlanId, &clientId, &client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: ptin_igmp_clientIndex_get failed");
    return L7_FAILURE;
  }
  #else
  if (mcastPacket->client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_IGMP_INSTANCE)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Client not provided");
    return L7_FAILURE;
  }
  #endif

  /* Update stats */
  pSnoopCB->counters.controlFramesProcessed++;

  ptin_igmp_stat_increment_field(mcastPacket->intIfNum, mcastPacket->vlanId, mcastPacket->client_idx, SNOOP_STAT_FIELD_LEAVES_RECEIVED);

  #if 0
  /* Does an entry with the same MAC addr and VLAN ID already exist? */
  snoopEntry = snoopEntryFind(dmac, mcastPacket->vlanId,
                              mcastPacket->cbHandle->family, L7_MATCH_EXACT);
  if ( snoopEntry == L7_NULLPTR )
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: snoopEntryFind failed!");
    return L7_FAILURE;
  }

  /* If interface is not active for this group, do nothing */
  if (!snoopEntry->port_list[mcastPacket->intIfNum].active)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: Interface %u is not active.",mcastPacket->intIfNum);
    return L7_FAILURE;
  }

  /* Verify if this channel exists */
  if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: This channel does not exist");
    return L7_SUCCESS;
  }

  /* Is this client consuming this channel? If not, there is nothing to be done */
  if (!snoopChannelClientExists(snoopEntry,&mgmdMsg.mgmdGroupAddr,client_index))
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: This client does not exist!");
    return L7_SUCCESS;
  }

  /* Remove client */
  if (snoopChannelClientRemove(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr,client_index)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                "snoopMgmdLeaveGroupProcess: snoopChannelClientRemove failed");
    return L7_FAILURE;
  }
  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: client removed");

  /* Only send leave and remove interface, for dynamic entries */
  if (!snoopEntry->staticGroup)
  {
    /* Procedures when no one is watching a channel in a particular interface... */
    /* Is there no clients for this channel+interface? */
    if (snoopChannelClientsIntfNone(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr))
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: No clients for this channel and interface");
      /* Remove interface from this channel */
      if (snoopChannelIntfRemove(snoopEntry,mcastPacket->intIfNum,&mgmdMsg.mgmdGroupAddr)!=L7_SUCCESS)
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                    "snoopMgmdLeaveGroupProcess: snoopChannelIntfRemove failed");
        return L7_FAILURE;
      }
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: interface removed for this interface");

      /* If channel was removed, send LEAVE to network */
      if (!snoopChannelExist(snoopEntry,&mgmdMsg.mgmdGroupAddr))
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: Message will be forward to network");
        fwdFlag = L7_TRUE;
      }

      /* If there is no channels for this interface, remove interface from group */
      if (snoopChannelsIntfNone(snoopEntry,mcastPacket->intIfNum))
      {
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: No channels for this interface");
        if (snoopIntfRemove(dmac, mcastPacket->vlanId, mcastPacket->intIfNum,
                            SNOOP_GROUP_MEMBERSHIP, mcastPacket->cbHandle) != L7_SUCCESS)
        {
          SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,
                      "snoopMgmdLeaveGroupProcess: Failed to remove group membership");
          return L7_FAILURE;
        }
        SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: interface was removed from group");
      }
    }
  }
  #endif

  /* Add client */
  if (snoop_client_remove_procedure(dmac, mcastPacket->vlanId,
                                    &mgmdMsg.mgmdGroupAddr,
                                    mcastPacket->client_idx,
                                    mcastPacket->intIfNum,
                                    &fwdFlag)!=L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: snoop_client_remove_procedure failed");
    return L7_FAILURE;
  }

  #if 0
  // Only if IP channel was removed, we forward the LEAVE packet */
  if (fwdFlag)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"snoopMgmdLeaveGroupProcess: Forwarding message to network");
    igmp_packet_respond(mcastPacket);
  }
  #endif

  return L7_SUCCESS;
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
    /* PTin removed: IGMP snooping */
    #if 0
    /* Add this interface to mcast router list and to all snooping entries */
    snoopMcastRtrListAdd(mcastPacket->vlanId, mcastPacket->intIfNum,
                         SNOOP_DYNAMIC, mcastPacket->cbHandle);
    #endif
  }

  /* Forward on all interfaces on this VLAN, except the incoming interface only
     if the packet is of type IGMP. Other packets are forwarded by silicon*/
  pSnoopCB->counters.controlFramesProcessed++;
  if (ipProtType == IP_PROT_IGMP)
    return snoopPacketFlood(mcastPacket);
  else
    return L7_SUCCESS;
}


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

/*********************************************************************
* @purpose  Build IGMP Message
*
* @param    vlanId    @b{(input)}  Vlan id
* @param    type      @b{(input)}  IGMP Packet type
* @param    version   @b{(input)}  IGMP Version
* @param    groupAddr @b{(input)}  IGMP Group address
* @param    buffer    @b{(output)} Buffer to hold the packet
* @param    length    @b{(input)}  Message size
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
static L7_RC_t snoopIGMPFrameBuildv2(L7_uint32      vlanId,
                                    L7_uchar8       type,
                                    L7_uint32       version,
                                    L7_inet_addr_t *groupAddr,
                                    L7_uchar8      *buffer,
                                    L7_uint32      *length,
                                    snoop_cb_t     *pSnoopCB)
{
  L7_uchar8      *dataPtr, *tempPtr, *startPtr;
  L7_uchar8       baseMac[L7_MAC_ADDR_LEN], byteVal;
  L7_uchar8       destMac[L7_MAC_ADDR_LEN];
  L7_ushort16     shortVal;
  static L7_ushort16 iph_ident = 1;
  L7_inet_addr_t  destIp;
  L7_uint32       ipv4Addr;
  L7_uint32       frame_length = IGMP_FRAME_SIZE;

  dataPtr = buffer;

  // Determine Destination IP
  switch (type)
  {
    case L7_IGMP_V1_MEMBERSHIP_REPORT:
    case L7_IGMP_V2_MEMBERSHIP_REPORT:
      memcpy(&destIp,groupAddr,sizeof(L7_inet_addr_t));
      break;

    case L7_IGMP_V2_LEAVE_GROUP:
      destIp.family = L7_AF_INET;
      destIp.addr.ipv4.s_addr = L7_IP_ALL_RTRS_ADDR;
      break;

    case L7_IGMP_MEMBERSHIP_QUERY:
      destIp.family = L7_AF_INET;
      destIp.addr.ipv4.s_addr = L7_IP_ALL_HOSTS_ADDR;
      break;

    case L7_IGMP_V3_MEMBERSHIP_REPORT:
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopIGMPFrameBuildv2: L7_IGMP_V3_MEMBERSHIP_REPORT not supported yet");
      return L7_FAILURE;

    default:
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "snoopIGMPFrameBuildv2: Message type (%u) not supported",type);
      return L7_FAILURE;
  }

  /* Validate the group address being reported */
  snoopMulticastMacFromIpAddr(&destIp, destMac);
  /* Validate MAC address */
  if (snoopMacAddrCheck(destMac, L7_AF_INET) != L7_SUCCESS)
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
  shortVal = L7_ETYPE_8021Q;
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes
  shortVal = ((pSnoopCB->snoopCfgData->snoopAdminIGMPPrio & 0x07)<<13) | (vlanId & 0x0fff);
  SNOOP_PUT_SHORT(shortVal, dataPtr);                   // 2 bytes

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
  //if (inetAddressGet(L7_AF_INET, &pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierAddress, &ipv4Addr)!=L7_SUCCESS)  {
  if (inetAddressGet(L7_AF_INET, &pSnoopCB->snoopCfgData->snoopVlanCfgData[vlanId].snoopVlanQuerierCfgData.snoopQuerierVlanAddress, &ipv4Addr)!=L7_SUCCESS)
  {
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

    SNOOP_UNUSED_PARAM(dataPtr);

    shortVal = snoopCheckSum((L7_ushort16 *)startPtr, SNOOP_IGMPv1v2_HEADER_LENGTH, 0);
    SNOOP_PUT_SHORT(shortVal, tempPtr); /* Copy the calculated checksum
                                          to stored checksum ptr */
    SNOOP_UNUSED_PARAM(tempPtr);

    frame_length = IGMP_FRAME_SIZE;
  }
  else
  {
    return L7_FAILURE;
  }

  // Return length of packet
  if (length!=L7_NULLPTR)  *length = frame_length;
  return L7_SUCCESS;
}

/**
 * Rebuild packet structure
 * 
 * @param mcastPacket : Packet info
 * @param groupAddr   : Group address (IP channel) to use
 * @param type        : IGMP Message type
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t igmp_rebuild_packet(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupAddr, L7_uchar8 type)
{
  L7_uchar8       *dataStart;
  L7_uint32        version, frameLength = 0, vlanId;
  L7_RC_t          rc = L7_SUCCESS;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;

  /* Get Snoop Control Block */
  if ((pSnoopCB = snoopCBGet(mcastPacket->cbHandle->family)) == L7_NULLPTR)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_rebuild_packet: Error getting pSnoopCB");
    return L7_FAILURE;
  }

  /* Get vlan id */
  vlanId  = mcastPacket->vlanId;

  /* USe always version 2 */
  version = SNOOP_IGMP_VERSION_2; // /*(type==SNOOP_IGMP_QUERY) ? igmp_querier_version :*/ pSnoopCB->snoopCfgData->snoopQuerierCfgData.snoopQuerierVersion;

  /* Datastart points to the beginning of data */
  dataStart = mcastPacket->payLoad;

  /* Only allow IPv4 for now */
  if (pSnoopCB->family != L7_AF_INET)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_rebuild_packet: Not IPv4 packet");
    return L7_FAILURE;
  }

  /* Build packet */
  rc = snoopIGMPFrameBuildv2(vlanId, type, version, groupAddr, dataStart, &frameLength, pSnoopCB);

  /* Everything went went well? */
  if (rc != L7_SUCCESS)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_rebuild_packet: Something went wrong");
    return L7_FAILURE;
  }

  /* Update frame length */
  mcastPacket->length = frameLength;

  return rc;
}

#if 0
static L7_RC_t igmp_packet_respond(mgmdSnoopControlPkt_t *mcastPacket)
{
  L7_inet_addr_t         groupAddr;
  L7_uchar8              type;
  L7_RC_t                rc = L7_SUCCESS;
  L7_uint32              intf, vlan, innervlan;

  intf      = mcastPacket->intIfNum;
  vlan      = mcastPacket->vlanId;
  innervlan = mcastPacket->innerVlanId;

  // get message type
  type = mcastPacket->ip_payload[0];

  // Get Group channel
  groupAddr.family           = L7_AF_INET;
  groupAddr.addr.ipv4.s_addr = ((struct_IGMPv2 *) mcastPacket->ip_payload)->groupIP;

  // Build answer packet
  if ((rc=igmp_rebuild_packet(mcastPacket, &groupAddr, type))==L7_SUCCESS)
  {
    // Send Packet to mrouter ports
    if ((rc=snoopPacketRtrIntfsForward(mcastPacket))==L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_packet_respond: Packet transmitted to router interfaces");
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_packet_respond: Error transmitting to router interfaces");
    }
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family, "igmp_packet_respond: Error rebuilding packet");
  }

  mcastPacket->intIfNum    = intf;
  mcastPacket->vlanId      = vlan;
  mcastPacket->innerVlanId = innervlan;

  return rc;
}
#endif

static L7_RC_t igmp_packet_general_query_respond(mgmdSnoopControlPkt_t *mcastPacket, L7_inet_addr_t *groupIP)
{
  L7_uint16 channel;
  L7_uint16 number_of_channels;
  L7_inet_addr_t channel_list[1024];
  L7_uint intf, vlanId, client_vlan;

  intf        = mcastPacket->intIfNum;
  vlanId      = mcastPacket->vlanId;
  client_vlan = mcastPacket->innerVlanId;

  /* Initialize number of channels */
  number_of_channels = 0;

  if (groupIP->family != L7_AF_INET)
    return L7_FAILURE;

  if (groupIP->addr.ipv4.s_addr != L7_NULL) {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Respostas ao Specific query (0x%08X) (UNI Vlan=%u)",groupIP->addr.ipv4.s_addr, vlanId);

    /* If this IP exists, send join */

    if (!snoopChannelExist4VlanId(vlanId,groupIP,L7_NULLPTR)) {  
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Canal 0x%08X existe!", groupIP->addr.ipv4.s_addr);
      number_of_channels = 1;
      memcpy(&channel_list[0], groupIP, sizeof(L7_inet_addr_t) );
    }
    else {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Canal 0x%08X NAO existe!", groupIP->addr.ipv4.s_addr);
    }
  }
  else  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"UNI Vlan=%u",vlanId);

    /* Note: the vlan that is supplied to the query is the uniVlan and not the MC one!
       The client_vlan must be ignored in this case */
    number_of_channels = L7_MAX_GROUP_REGISTRATION_ENTRIES;
    snoopChannelsListGet(vlanId,(L7_uint16)-1,channel_list,&number_of_channels);
  }

  /* Send channels reports */
  for (channel=0; channel<number_of_channels; channel++) {
    /* Prepare packet */
    if (igmp_rebuild_packet(mcastPacket, &channel_list[channel], L7_IGMP_V2_MEMBERSHIP_REPORT) == L7_SUCCESS) {
      /* Send packet to all mrouter ports */
      snoopPacketRtrIntfsForward(mcastPacket, L7_IGMP_V2_MEMBERSHIP_REPORT);
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Error transmitting Query #%u",channel);
    }
  }

  SNOOP_TRACE(SNOOP_DEBUG_PROTO, mcastPacket->cbHandle->family,"Respostas ao Query=%u",number_of_channels);

  mcastPacket->intIfNum    = intf;
  mcastPacket->vlanId      = vlanId;
  mcastPacket->innerVlanId = client_vlan;

  return L7_SUCCESS;
}

/**
 * Generate an IGMP packet from scratch, and send it to network 
 * interfaces 
 * 
 * @param vlan    : Internal Vlan
 * @param type    : IGMP packet type
 * @param groupIP : IP channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t igmp_generate_packet_and_send(L7_uint32 vlan, L7_uint8 type, L7_inet_addr_t *channel)
{
  L7_RC_t                rc = L7_SUCCESS;
  mgmdSnoopControlPkt_t  mcastPacket;

  /* Validate arguments */
  if (vlan<PTIN_VLAN_MIN || vlan>PTIN_VLAN_MAX || channel==L7_NULLPTR)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Invalid arguments");
  }
  /* Valkidate family */
  if (channel->family!=L7_AF_INET)
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: IP family not supported");
  }

  // Initialize mcastPacket structure
  memset(&mcastPacket,0x00,sizeof(mgmdSnoopControlPkt_t));
  mcastPacket.cbHandle    = snoopCBGet(L7_AF_INET);
  mcastPacket.vlanId      = vlan;
  mcastPacket.innerVlanId = 0;
  mcastPacket.client_idx  = (L7_uint32)-1;
  mcastPacket.msgType     = IP_PROT_IGMP;
  mcastPacket.srcAddr.family            = L7_AF_INET;
  mcastPacket.srcAddr.addr.ipv4.s_addr  = L7_NULL_IP_ADDR;
  mcastPacket.destAddr.family           = L7_AF_INET;
  mcastPacket.destAddr.addr.ipv4.s_addr = L7_NULL_IP_ADDR;

  // Build answer packet
  ptin_timer_start(27,"igmp_generate_packet_and_send-igmp_rebuild_packet");
  rc=igmp_rebuild_packet(&mcastPacket, channel, type);
  ptin_timer_stop(27);
  if (rc==L7_SUCCESS)
  {
    // Send Packet to mrouter ports
    ptin_timer_start(28,"igmp_generate_packet_and_send-snoopPacketRtrIntfsForward");
    rc=snoopPacketRtrIntfsForward(&mcastPacket, type);
    ptin_timer_stop(28);
    if (rc==L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Packet transmitted to router interfaces");
    }
    else
    {
      SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Error transmitting to router interfaces");
    }
  }
  else
  {
    SNOOP_TRACE(SNOOP_DEBUG_PROTO, L7_AF_INET, "igmp_generate_packet_and_send: Error rebuilding packet");
  }

  return rc;
}
