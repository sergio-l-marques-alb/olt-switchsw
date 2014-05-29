/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename l3_intf.c
*
* @purpose Linux network adaptation for the router interfaces.
*
* @component Linux IPM
*
* @comments none
*
* @create 02/08/2002
*
* @author JWL
* @end
*
**********************************************************************/
/*********************************************************************
 *
 *********************************************************************/
#include <string.h>
#include "l3end_api.h" /* JWL -- need to change name of this file */
#include "l3_intf.h"
#include "sysapi.h"
#include "dtlapi.h"
#include "dtl_tap_monitor.h"
#include "l7_ipmap_arp_api.h"
#include "netinet/ip.h"
#include "rto_api.h"

#include "l7_ip_api.h"
#include "l7_vrrp_api.h"
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "ipstk_api.h"
#include "sim_debug_api.h"
#include "osapi_support.h"
#include "usmdb_nim_api.h"

#define ETHERTYPE_IP 0x800
#define IPPROTO_OSPF 0x59

#define	ICMP_ECHOREPLY		0		/* echo reply */

extern L3INTF_INFO *l3intfInfo;

extern L7_uint32 ip6ForwardNotRtrIntf;    /* packets received on non-routing interface */
extern L7_uint32 ip6ForwardRxIfDown;      /* packets received on down interface */
extern L7_uint32 ip6ForwardPktsToStack;   /* packets to IP stack */

extern L7_RC_t ptin_routing_intf_physicalport_get(L7_uint16 routingIntfNum, L7_uint16 *physicalIntfNum); /* I am unable to include ptin_routing here */


/*********************************************************************
*
* @purpose  Transmit OSAPI network buffer on the specified IP interface.
*
* @param    intIfNum      internal interface number
* @param    bufHandle     mbuf containing the frame.
*
* @notes    This function is used by routing code and ARP code to
*           send frames on the network.
*
* @end
*********************************************************************/
L7_RC_t ipmRouterIfBufSend(L7_uint32 intIfNum, L7_netBufHandle  bufHandle) {

   L3INTF_INFO *ifInfo;
   DTL_CMD_TX_INFO_t dtlCmd;
   L7_uint32 len;
   L7_netBufHandle  newBufHandle;
   L7_uchar8 destMac[6];
   L7_ushort16 flag = 0;
   L7_ushort16 protocol_type;
   L7_uint32 datalen;
   L7_uchar8 *pdataStart;
   L7_uint32 offset = 0;
   L7_uint32 commonHeaderLen = L7_ENET_HDR_SIZE + 2;
   L7_uint32 encapType;
   L7_uint32 rtrIntf;
   struct ip *ip_header;
   L7_uint32 hlen;
   L7_ushort16 cksum = 0;
   L7_ushort16 packet_type;
   struct ip *p;
   L7_uint32 word32;
   L7_uint32 vlanId;
   sysnet_pdu_info_t pduInfo;
   L7_ushort16 protocolType;
   L7_uchar8 *pEtype;
   L7_INTF_TYPES_t ifType;

   if (nimGetIntfType(intIfNum, &ifType) != L7_SUCCESS)
   {
      SYSAPI_NET_MBUF_FREE(bufHandle);                   
      return L7_FAILURE;
   }
  
   if (ifType == L7_LOOPBACK_INTF)
   {
     /*
     * Loopbacks are implemented in the IP Stack as virtual
     * ethernet devices, so drop anything that is attempted
     * to be sent from a loopback here.
     */
      SYSAPI_NET_MBUF_FREE(bufHandle);                   
      return L7_SUCCESS;
   }

   if (ipstkIntIfNumToRtrIntf(intIfNum, &rtrIntf) != L7_SUCCESS) {

      SYSAPI_NET_MBUF_FREE(bufHandle);
      return L7_FAILURE;

   }

   if (ipstkRtrIfIsUp(intIfNum) != L7_SUCCESS)
   {

      SYSAPI_NET_MBUF_FREE(bufHandle);
      return L7_FAILURE;

   }

   ifInfo = &l3intfInfo[rtrIntf];

#if 1 /* PTin Added - Routing support (ensure uplink routing interfaces send ARPs through a single (previously determined) physical interface) */
{
  L7_uint16 physicalIntfNum;

  /* To avoid broadcast on uplink routing interfaces we get here the physical interface associated with this routing interface. */
  ptin_routing_intf_physicalport_get(intIfNum, &physicalIntfNum);

  if(physicalIntfNum != (L7_uint16)-1)
  {
    intIfNum = physicalIntfNum;
  }
  else
  {
    intIfNum = ifInfo->intIfNum;
  }

  dtlCmd.intfNum = intIfNum;
  dtlCmd.priority = 0;
  dtlCmd.typeToSend = DTL_NORMAL_UNICAST;

  if (ipMapVlanRtrIntIfNumToVlanId(ifInfo->intIfNum, &vlanId) == L7_SUCCESS) {
    dtlCmd.cmdType.L2.domainId = vlanId;
  } else {
    dtlCmd.cmdType.L2.domainId = 0;
  }
  dtlCmd.cmdType.L2.flags = 0;
}
#else
   dtlCmd.intfNum = ifInfo->intIfNum;
   dtlCmd.priority = 0;
   dtlCmd.typeToSend = DTL_NORMAL_UNICAST;

   if (ipMapVlanRtrIntIfNumToVlanId(dtlCmd.intfNum, &vlanId) == L7_SUCCESS) {

      dtlCmd.cmdType.L2.domainId = vlanId;

   } else {

      dtlCmd.cmdType.L2.domainId = 0;

   }
   dtlCmd.cmdType.L2.flags = 0;
#endif

   SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, datalen);

   if (!datalen) {

      return L7_FAILURE;

   }

   SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pdataStart);

   /* Strip VLAN tag if present... let lower layer insert tag if appropriate */
   pEtype = pdataStart + L7_ENET_HDR_SIZE;
   bcopy (pEtype, (L7_uchar8 *)&protocolType, sizeof(L7_ushort16));
   protocolType = ntohs(protocolType);
#if 0 /* Ptin removed - routing support (Removed this because the ARP packet now arrives here with the dtl0 vlan) */
   if (protocolType == L7_ETYPE_8021Q)
   {
     L7_int32 index;
     L7_uint32 messageLen;

     /* Strip the VLAN header */
     for (index = L7_ENET_HDR_SIZE-1; index >= 0; index--)
     {
       /* Shift the Destination MAC Address and the Source MAC Address by 4 bytes.
       ** These 4 bytes are the VLAN header.
       */
       *(pdataStart + index + L7_8021Q_ENCAPS_HDR_SIZE) = *(pdataStart + index);
     }

     /* Set the new data start */
     SYSAPI_NET_MBUF_SET_DATASTART(bufHandle, pdataStart + L7_8021Q_ENCAPS_HDR_SIZE);
     SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, pdataStart); /* get the new data start */

     SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, messageLen);
     messageLen -= L7_8021Q_ENCAPS_HDR_SIZE;
     /* Set the new data length */
     SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, messageLen);
   }
#endif

   offset = sysNetDataOffsetGet(pdataStart);

   ip_header = (struct ip *)(pdataStart + offset);
   bcopy ((pdataStart + offset - 2), (L7_char8 *) &packet_type, 2);
   packet_type = ntohs(packet_type);

   if (packet_type == ETHERTYPE_IP)
   {
      if (ip_header->ip_p == IPPROTO_OSPF)
      {
         /* align ip header on 32 bits for manipulation */
         p=(struct ip *)&word32;
         memcpy(p,ip_header,sizeof(L7_uint32));
         p->ip_tos |= 0xc0;
         hlen = p->ip_hl << 2;
         memcpy(ip_header,p,sizeof(L7_uint32));

         ip_header->ip_sum = 0;
         ipMapIpHeaderCksumCalculate((void *)ip_header, hlen, &cksum);
         ip_header->ip_sum = cksum;

      }

      if(ip_header->ip_p == IP_PROT_ICMP)
      {
         L7_enetHeader_t     *enetHdr;
         L7_ipHeader_t       *ipHdr;
         L7_enet_encaps_t    *encap;				
         L7_uchar8 *buff = L7_NULL;				
         L7_uchar8 icmp_type;						

         buff = (char *)pdataStart;	
         enetHdr = (L7_enetHeader_t *)(&buff[0]);
         encap = (L7_enet_encaps_t *)((L7_uchar8 *)enetHdr + L7_ENET_HDR_SIZE);
         ipHdr = (L7_ipHeader_t *)((L7_uchar8 *)encap + L7_ENET_ENCAPS_HDR_SIZE);
         icmp_type = *(L7_uchar8 *)((L7_uchar8 *)ipHdr + L7_IP_HDR_LEN);

         if(icmp_type == ICMP_ECHOREPLY)
         {
            L7_uchar8 vmac[L7_ENET_MAC_ADDR_LEN];
            L7_uchar8 srcIp[L7_IP_ADDR_LEN];
            L7_uchar8 vrid;		

            memcpy(srcIp, &(ipHdr->iph_src), L7_IP_ADDR_LEN);

            if((vrrpFuncTable.vrrpMapGetVMac) &&
               (vrrpFuncTable.vrrpMapGetVMac(srcIp, vmac, &vrid) == L7_SUCCESS))
            {
               enetHdr->src.addr[0] = 0x00;
               enetHdr->src.addr[1] = 0x00;
               enetHdr->src.addr[2] = 0x5e;
               enetHdr->src.addr[3] = 0x00;
               enetHdr->src.addr[4] = 0x01;
               enetHdr->src.addr[5] = vrid;
            }
         }
         pingDebugPacketTxTrace(intIfNum, pdataStart);
      }
   }

   /* Call interceptors who are interested in outgoing IP frames prior to L2 encapsualtion.
   ** If L7_TRUE is returned, the frame was either discarded or consumed, which means that
   ** the network buffer has been freed by the intercept call, or will be freed by the consumer.
   */
   memset(&pduInfo, 0x00, sizeof(sysnet_pdu_info_t));
   pduInfo.destIntIfNum = intIfNum;
   pduInfo.destVlanId = vlanId;
   if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_PRECAP_OUT, bufHandle, &pduInfo,
                            L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
     return L7_SUCCESS;

   if (usmDbIfEncapsulationTypeGet(intIfNum, &encapType) == L7_SUCCESS) {

      if (((encapType == L7_ENCAP_802) && (offset == commonHeaderLen))
       || ((encapType == L7_ENCAP_ETHERNET)
        && (offset == commonHeaderLen + 8))) {

         memcpy(destMac, pdataStart, 6);
         bcopy (pdataStart + offset - sizeof(L7_ushort16),
                (L7_uchar8 *) &protocol_type, sizeof(L7_ushort16));

         if (encapType == L7_ENCAP_802) {

            flag |= L7_LL_ENCAP_802;

         } else {

            flag |= L7_LL_ENCAP_ENET;

         }

         flag |= L7_LL_USE_L3_SRC_MAC;
         if (sysNetPduHdrEncapsulate(destMac, intIfNum, &flag, datalen - offset,
                                     0, protocol_type, &newBufHandle,
                                     pdataStart + offset) == L7_SUCCESS) 
         {
            SYSAPI_NET_MBUF_GET_DATALENGTH (newBufHandle, len);
            SYSAPI_NET_MBUF_FREE(bufHandle);

            /* Call interceptors who are interested in outgoing IP frames post L2 encapsualtion.  If
            ** L7_TRUE is returned, the frame was either discarded or consumed, which means that the
            ** network buffer has been freed by the intercept call, or will be freed by the consumer.
            */
            if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_POSTCAP_OUT, newBufHandle, &pduInfo,
                                     L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
              return L7_SUCCESS;

            dtlPduTransmit(newBufHandle, DTL_CMD_TX_L2, &dtlCmd);
            return(L7_SUCCESS);
         }
         else
         {
           SYSAPI_NET_MBUF_FREE(bufHandle);
           return L7_FAILURE;
         }
      }
   }

   /* Call interceptors who are interested in outgoing IP frames post L2 encapsualtion.  If
   ** L7_TRUE is returned, the frame was either discarded or consumed, which means that the
   ** network buffer has been freed by the intercept call, or will be freed by the consumer.
   */
   if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_POSTCAP_OUT, bufHandle, &pduInfo,
                            L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
     return L7_SUCCESS;

   SYSAPI_NET_MBUF_GET_DATALENGTH (bufHandle, len);

   dtlPduTransmit(bufHandle, DTL_CMD_TX_L2, &dtlCmd);

   return(L7_SUCCESS);

}

/*********************************************************************
*
* @purpose  Transmit OSAPI network buffer on the specified IP interface.
*
* @param    fd            tap fd
* @param    intIfNum      internal interface number
* @param    bufHandle     mbuf containing the frame.
*
* @notes    called from dtl_tap_monitor
*
* @end
*********************************************************************/
#if defined(L3INTF_USE_TAP)
void l3intf_tap_send(int fd,L7_uint32 intIfNum, L7_netBufHandle handle, tapDtlInfo *info)
{
  /* no need to walk tap table */

  ipmRouterIfBufSend(intIfNum, handle);
  return;
}

#endif

/*********************************************************************
*
* @purpose  Process local non-unicast IP packets
*
* @param    intIfNum       internal Interface Number
* @param    bufHandle      mbuf containing the frame.
*
* @returns  L7_SUCCESS      if packet is consumed by the local IP stack
* @returns  L7_FAILURE      if packet is not consumed by the local IP stack
*
* @notes    On VxWorks, this function is used to send multicast and broadcast
*           packets to the underlying stack.  On Linux, the networking stack
*           has already seen the packet, so just discard it.
*           Unless L3INTF_USE_TAP is set.  If it is set then
*           the L3 interfaces are using a TAP driver to send
*           data to the kernel.  This implies LVL7 sees the frame
*           first and here we need to write it to the apprpriate
*           file descriptor
*
* @end
*********************************************************************/
L7_RC_t ipmLocalPacketProcess(L7_uint32 intIfNum, L7_netBufHandle bufHandle) 
{
   L7_uint32 ifIndex;
   L3INTF_INFO *ifInfo;
   L7_uchar8 *data;
   L7_uint32 data_length;
#ifdef L3INTF_USE_TAP
   unsigned char tmp_buf[12];
   L7_ushort16 ether_type;
   L7_uchar8 vrrp_mac_da[6] = {0x00,0x00,0x5e,0x00,0x01,0x00};
   L7_uchar8 rtrif_mac_addr[6];
   struct ip *ip_header;
   L7_uint32 headerOffset;
#endif

   SYSAPI_NET_MBUF_GET_DATASTART(bufHandle,data);
   SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle,data_length);

#ifdef L3INTF_USE_TAP
   /* Defect LVL700026620. If the destination MAC address if VRRP mac address,
    * swap the VRRP mac address with the interface mac address
    */
   if (memcmp(data, vrrp_mac_da, 5) == 0)
   {
     nimGetIntfL3MacAddress(intIfNum, 0, (L7_uchar8 *)&(rtrif_mac_addr));
     memcpy (data, rtrif_mac_addr, 6);
   }

   memcpy(&ether_type, (L7_ushort16 *)&data[12], sizeof(L7_ushort16));
   ether_type = osapiNtohs(ether_type);
   if(ether_type == L7_ETYPE_8021Q)
   {
      /*
       *I don't think we need to worry about the vlan here,
       *as it should have been rx on the appropriate net
       *interface and therefore have the corresponding fd below
       */
      memcpy(tmp_buf,data,12);
      memcpy(data+4,tmp_buf,12);
      data +=4;
      data_length-=4;
      memcpy(&ether_type, (L7_ushort16 *)&data[12], sizeof(L7_ushort16));
      ether_type = osapiNtohs(ether_type);
      SYSAPI_NET_MBUF_SET_DATASTART(bufHandle, data);
      SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, data_length);
   }

   if (ipstkIntIfNumToRtrIntf(intIfNum, &ifIndex) != L7_SUCCESS) 
   {
     if (ether_type == L7_ETYPE_IPV6)
     {
       ip6ForwardNotRtrIntf++;
     }
     return L7_FAILURE;
   }

   if (ipstkRtrIfIsUp(intIfNum) != L7_SUCCESS) 
   {
     if (ether_type == L7_ETYPE_IPV6)
     {
       ip6ForwardRxIfDown++;
     }
     return L7_FAILURE;
   }

   if (ether_type == L7_ETYPE_IPV6)
   {
     ip6ForwardPktsToStack++;
   }

   headerOffset = sysNetDataOffsetGet(data);
   ip_header = (struct ip *)(data + headerOffset);

   if(ip_header->ip_p == IP_PROT_ICMP)
   {
     pingDebugPacketRxTrace(intIfNum,(char *)ip_header);
   }

   ifInfo = &l3intfInfo[ifIndex];
   if(0 > write(ifInfo->tap_fd,(void *)data,data_length)){}
#endif

   SYSAPI_NET_MBUF_FREE(bufHandle);
   return(L7_SUCCESS);
}
