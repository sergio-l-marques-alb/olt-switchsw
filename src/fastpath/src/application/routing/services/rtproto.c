/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename rtproto.c
*
* @purpose The purpose of this file is to forward IP traffic.
*          This code rplaces VxWorks IP forwarding function.
*
* @component VxWorks IPM
*
* @comments none
*
* @create 03/19/2001
*
* @author alt
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include <log.h>
#include <l7_common.h>
#include <l3end_api.h>
#include <l7_packet.h>
#include <dtlapi.h>
#include <l7_ipmap_arp_api.h>
#include <rto_api.h>
#include <sysapi.h>
#include "sysnet_api.h"
#include "sysnet_api_ipv4.h"
#include "simapi.h"
#include <nimapi.h>
#include <l7_ip_api.h>
#include "rtiprecv.h"
#include "rtmbuf.h"
#include "rtip.h"
#include "rtip_icmp.h"
#include "rtmisc.h"
#include "ip_util.h"
#include <buff_api.h>
#include <stdlib.h>
#include <l3_defaultconfig.h>
#ifdef _L7_OS_LINUX_
    #define __FAVOR_BSD 1
#endif


#define RTPROTO_DEBUG   0     /* change this definition to 1 for debugging */

extern L7_uint32 ipForwardRtrIntfInvalid;
extern L7_uint32 ipForwardConsumed;
extern L7_uint32 ipForwardConsumedMcast;
extern L7_uint32 ipForwardBufAllocFail;
extern L7_uint32 ipForwardPktsDropped;
extern L7_uint32 ipForwardPktsOut;
extern L7_uint32 ipForwardPktsToStack;
extern L7_uint32 ipMapTraceFlags;
extern ipMapDebugIcmpCtlStats_t icmpCtlStats;
extern void ipMapTraceWrite(L7_uchar8 *traceMsg);

struct rtm_ifnet *gRtmIfnet;  /* Global array of ifnet structures */

/* Static variables.
*/
static L7_uint32 rtBufferPoolId = 0; /* Buffer pool used by the IP forwarding code */

/*********************************************************************
*
* @purpose  Set new destination MAC address in the frame and send it out.
*
* @param    bufHandle     Network Buffer
* @param    intIfNum   Internal Interface Number
* @param    mac           Destination MAC address.
*
*
* @notes    none
*
* @end
*********************************************************************/

static void
rtLvl7TxIPForward (L7_netBufHandle bufHandle,
                   L7_uint32 intIfNum, L7_uchar8 * mac)
{
  L7_uchar8 *data;

  ipForwardPktsOut++;


  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  /* Set new SA and DA in the MAC header. The LLC header should still be valid.
  */
  memcpy (&data[0], mac, 6); /* New DA address */
  ipMapRouterIfMacAddressGet (intIfNum, &data[6]);

#if RTPROTO_DEBUG

  printf("rtLvl7TxIP: Sending to mac: %x:%x:%x:%x:%x:%x\n",
         mac[0],
         mac[1],
         mac[2],
         mac[3],
         mac[4],
         mac[5]);
#endif

  ipmRouterIfBufSend (intIfNum, bufHandle);

};


/*********************************************************************
*
* @purpose  Handle ARP frames.
*
* @param    L7_netBufHandle     Name of the END device.
* @param    intIfNum            Internal Interface Number of the receiving port.
*
*
* @returns  L7_SUCCESS  Frame has been handled and consumed.
* @returns  L7_ERROR    Redirect frame to system CPU via IP stack.
* @returns  L7_FAILURE  Frame should be forwarded to the IP stack.
*
* @end
*********************************************************************/
static L7_RC_t
rtLvl7ArpHandle (L7_netBufHandle bufHandle,
                 L7_uint32 intIfNum)
{
  L7_uchar8 * data;
  L7_uint32 nbytes;
  L7_ether_arp_t *arp_header;  /* points to ARP packet within mbuf */
  L7_ether_arp_t arp_pkt;     /* copy of incoming ARP packet. No eth header. */
  sysnet_pdu_info_t pduInfo;
  L7_ulong32 tmp;

  /* Call interceptors who are interested in all incoming ARP frames.  If L7_TRUE is returned,
  ** the frame was either discarded or consumed, which means that the network buffer has been
  ** freed by the intercept call, or will be freed by the consumer.
  */
  memset(&pduInfo, 0, sizeof(sysnet_pdu_info_t));
  pduInfo.intIfNum = intIfNum;
  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_ARP_IN, bufHandle, &pduInfo,
                           L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
    return L7_SUCCESS;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, nbytes);

  /* Get the ARP header... this will handle VLAN tagged frames as well as Ethernet and LLC&SNAP encap */
  arp_header = (L7_ether_arp_t  *)(data + sysNetDataOffsetGet(data));

  /* Check if the packet is destined for the system IP address.
   * If so, do not claim it so that it can be forwarded to the
   * kernel's IP stack.
   */
  memcpy(&tmp,arp_header->arp_tpa,sizeof(tmp));
  if(osapiNtohl(tmp) == simGetSystemIPAddr())
    return L7_ERROR;

  /* Make a copy of the ARP packet so we can free the mbuf before processing
   * the ARP packet. Hey, it's only 28 bytes. */
  memcpy(&arp_pkt, arp_header, sizeof(L7_ether_arp_t));
  SYSAPI_NET_MBUF_FREE(bufHandle);

  /* This can fail if we receive an ARP packet on an IP interface enabled
   * for DHCP before the interface gets its IP address. That's ok. Don't 
   * check rc here and let ipMapArpReceive() log any error conditions. */
  (void) ipMapArpReceive (intIfNum, (L7_uchar8 *) &arp_pkt);

  /* Since the caller of this function, rtoIPv4Handle, will free the buf handle upon success,
  ** claim the frame here by returning a success, even if ipMapArpReceive was unsuccessful
  */
  return L7_SUCCESS;
}




/*********************************************************************
*
* @purpose  Handle IP frames.
*
* @param    L7_netBufHandle     Name of the END device.
* @param    local_ip_addr       IP address associated with receiving interface
* @param    local_ip_netmask    network mask associated with receiving interface
* @param    *pduInfo            pointer to PDU info structure which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS  Frame has been handled and consumed.
* @returns  L7_ERROR    Redirect frame to system CPU via kernel IP stack.
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
rtLvl7IPHandle (L7_netBufHandle bufHandle,
                L7_uint32 local_ip_addr, L7_uint32 local_ip_netmask,
                sysnet_pdu_info_t *pduInfo
               )
{
  struct ip *ip_header;
  L7_uchar8 * data;
  L7_uint32 dest_ip_addr;

  /* Call interceptors who are interested in all IP frames, prior to any validity checks.
  ** If L7_TRUE is returned, the frame was either discarded or consumed, which means
  ** that the network buffer has been freed by the intercept call, or will be freed by
  ** the consumer.
  */
  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_IN, bufHandle, pduInfo, L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
  {
    ipForwardConsumed++;
    return L7_SUCCESS;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  /* Get the IP header... this will handle VLAN tagged frames as well as Ethernet and LLC&SNAP encap */
  ip_header = (struct ip *)(data + sysNetDataOffsetGet(data));
  dest_ip_addr = osapiNtohl(ip_header->ip_dst.s_addr);

  if (IN_MULTICAST(dest_ip_addr))
  {
    /* Call interceptors who are interested in all IP multicast frames.  If L7_TRUE is
    ** returned, the frame was either discarded or consumed, which means that the network
    ** buffer has been freed by the intercept call, or will be freed by the consumer.
    */
    if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_MCAST_IN, bufHandle, pduInfo,
                             L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
    {
      ipForwardConsumedMcast++;
      return L7_SUCCESS;
    }
  }

  /* Check if the packet is destined for the system IP address.
   * If so, do not claim it so that it can be forwarded to the
   * kernel's IP stack.
   */
  if (dest_ip_addr == simGetSystemIPAddr())
    return L7_ERROR;

  rtLvl7IPRecv(bufHandle, pduInfo);
  return L7_SUCCESS;
}

void sysapiIpStatsShow(void)
{
  extern SYSAPI_IP_STATS_t sysapiIPStats;              \

  printf("\nipInDiscards: %u", sysapiIPStats.ipInDiscards);
  printf("\nipInDelivers: %u", sysapiIPStats.ipInDelivers);
  printf("\nipOutRequests: %u", sysapiIPStats.ipOutRequests);
  printf("\nipOutDiscards: %u", sysapiIPStats.ipOutDiscards);
  printf("\nipOutNoRoutes: %u", sysapiIPStats.ipOutNoRoutes);
}

/*********************************************************************
* @purpose  Route IP frames and process ARPs.
*
* @param    bufHandle Network Buffer containg incoming IP or ARP packet.
* @param    *pduInfo    pointer to PDU info structure which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS  Frame has been consumed by the routing process.
* @returns  L7_ERROR    Redirect frame to system CPU via kernel IP stack.
* @returns  L7_FAILURE  Frame has not been consumed.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
rtoIPv4Handle (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8  *data;
  L7_uint32   protocolTypeOffset = 0;
  L7_ushort16 netProtocolType;
  L7_ushort16 protocolType;
  L7_uint32 ipAddr = 0;
  L7_uint32 netMask = 0;
  L7_RC_t     rc;

  /* Determine start of packet data. */
  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

#if RTPROTO_DEBUG
  printf("rtLvl7Handle: Got frame.\n");
#endif

  protocolTypeOffset = sysNetDataOffsetGet(data) - L7_ENET_ENCAPS_HDR_SIZE;
  bcopy ((data + protocolTypeOffset), (L7_char8 *) &netProtocolType, 2);
  protocolType = ntohs(netProtocolType);

  /* Check if it is an ARP frame. */
  if (protocolType == L7_ETYPE_ARP)
  {
#if RTPROTO_DEBUG
    printf("rtoIPv4Handle: Got ARP frame.\n");
#endif

    return rtLvl7ArpHandle (bufHandle, pduInfo->intIfNum);
  }

  /* If protocol is not IP then we have some kind of problem. */
  if (protocolType != L7_ETYPE_IP)
  {
    SYSAPI_NET_MBUF_FREE(bufHandle);
    SYSAPI_IP_STATS_INCREMENT(FD_CNFGR_NIM_MIN_CPU_INTF_NUM,
                              L7_PLATFORM_CTR_RX_IP_IN_DISCARDS);
    return L7_SUCCESS;
  }

  /* Handle IP frame. */
  rc = ipMapRtrIntfIpAddressGet(pduInfo->intIfNum, &ipAddr, &netMask);
  if (rc == L7_SUCCESS)
  {
    rc = rtLvl7IPHandle(bufHandle, ipAddr, netMask, pduInfo);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  ARP code calls this function when ARP resolution is done.
*
* @param    mbufHandle   IP buffer.
* @param    callbackparam   various call back parameters
* @param    mac         Resolved MAC address.
* @param    rc          Resolution status.
*
*
* @notes    none
*
* @end
*********************************************************************/
static void
rtIPForwardArpCallback (L7_uint32 mbufHandle,
                        L7_uint32 callbackparam,
                        L7_uchar8 *mac,
                        L7_RC_t  rc)
{
  L7_int32 type = 0, code;
  struct rtmbuf *m, *mcopy;
  cbparam_t *p_callbackparam;
  L7_uint32 dest = 0;
  L7_uchar8 *buffer_addr;
  L7_RC_t rc1;
  L7_uint32 tosEnable;
  L7_uint32 icmpUnreachablesMode = L7_DISABLE;
  L7_uint32 icmpRedirectsMode = L7_DISABLE;

  m = (struct rtmbuf *)mbufHandle;
  p_callbackparam = (cbparam_t *)callbackparam;

#if RTPROTO_DEBUG
  printf("rtIPForwardArpCallback: rc = %d, intIfNum = %d, bufHandle = 0x%x\n",
         rc, p_callbackparam->intIfNum, (L7_netBufHandle)m->rtm_bufhandle);
#endif

  /* Can't resolve. Delete the packet.
  */
  if (rc != L7_SUCCESS)
  {
    if (m->rtm_flags & M_EXT)
    {
      /* check unreachables mode */
      ipMapRtrICMPUnreachablesModeGet(m->rtm_pkthdr.rcvif->if_index, &icmpUnreachablesMode);
      if(L7_ENABLE == icmpUnreachablesMode)
      {
        type = ICMP_UNREACH;
        tosEnable = ipMapRtrTosForwardingModeGet();
        if (tosEnable == L7_ENABLE)

          code = ICMP_UNREACH_TOSHOST;
        else
          code = ICMP_UNREACH_HOST;


        mcopy = 0;
        rc1 = bufferPoolAllocate (rtBufferPoolId, &buffer_addr);
        if (rc1 == L7_SUCCESS)
        {
          mcopy = (struct rtmbuf *) buffer_addr;
          memset (mcopy, 0, sizeof (struct rtmbuf));
          mcopy->rtm_pkthdr.rcvif = m->rtm_pkthdr.rcvif;
          rtcreateIcmpErrPckt(m, mcopy);
          icmp_error(mcopy, type, code, 0, 0);
        }
      }
    }

    SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
    bufferPoolFree (rtBufferPoolId, (L7_uchar8 *) m);
    bufferPoolFree (rtBufferPoolId, (L7_uchar8 *) p_callbackparam);
    return;
  }

  /* Send ICMP Redirect if necessary. Do this before sending the packet,
   * because sending the packet frees the packet's mbuf and Redirect needs
   * to copy from the mbuf. */
  if (m->rtm_flags & M_EXT)
  {
    if (p_callbackparam->intIfNum == m->rtm_pkthdr.rcvif->if_index && !p_callbackparam->srcrt)
    {
      (void) ipMapIfICMPRedirectsModeGet(m->rtm_pkthdr.rcvif->if_index, &icmpRedirectsMode);
      if ((ipMapRtrICMPRedirectModeGet() != L7_ENABLE) || (icmpRedirectsMode != L7_ENABLE))
      {
        icmpCtlStats.totalSuppressedRedirects++;
      }
      else
      {
        struct ip *ip = rtmtod(m, struct ip *);
        u_long src = osapiNtohl(ip->ip_src.s_addr);
        if ((src & rtGetSubnetMaskOfIntf(p_callbackparam->intIfNum)) ==
          (p_callbackparam->routerIP & rtGetSubnetMaskOfIntf(p_callbackparam->intIfNum)))
        {
          dest = p_callbackparam->routerIP;
          type = ICMP_REDIRECT;
          code = ICMP_REDIRECT_HOST;
          mcopy = 0;
          rc1 = bufferPoolAllocate (rtBufferPoolId, &buffer_addr);
          if (rc1 == L7_SUCCESS)
          {
            mcopy = (struct rtmbuf *) buffer_addr;
            memset (mcopy, 0, sizeof (struct rtmbuf));
            mcopy->rtm_pkthdr.rcvif = m->rtm_pkthdr.rcvif;
            rtcreateIcmpErrPckt(m, mcopy);
            icmp_error(mcopy, type, code, dest, 0);
          }
        }
      }
    }
  }
  /* Send the data packet */
  rtLvl7TxIPForward ( (L7_netBufHandle)m->rtm_bufhandle, p_callbackparam->intIfNum, mac);

  bufferPoolFree(rtBufferPoolId, (L7_uchar8 *) m);
  bufferPoolFree(rtBufferPoolId, (L7_uchar8 *) p_callbackparam);
}


/*********************************************************************
*
* @purpose  Get ARP Address for destination IP Address.
*
* @param    dest_ip_addr   Destination IP address.
* @param    struct rtmbuf*   IP frame.
* @param    intIfNum        internal interface number
* @param    srcrt        Parameter to check if ICMP_REDIRECT messages needs
*                           to be sent.
*
* @returns  L7_SUCCESS  ARP entry exists.
* @returns  L7_FAILURE  ARP entry is not found.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32
rtInIPForwardArpTable (L7_uint32 dest_ip_addr, L7_uint32 netMask,
                       struct rtmbuf *m,
                       L7_uint32 intIfNum, L7_uint32 srcrt)
{
  L7_RC_t rc;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_netBufHandle netMbufHandle;
  L7_char8 *pdataStart, *p;
  cbparam_t *callbackparam;
  L7_uchar8 *buffer_addr;
  struct ip * ip;

  if (ipMapTraceFlags & IPMAP_TRACE_FORWARDING)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    if (nimGetIntfName(intIfNum, L7_SYSNAME, ifName) != L7_SUCCESS)
      strcpy(ifName, "invalid");
    osapiInetNtoa(ntohl(dest_ip_addr), destStr);
    sprintf(traceBuf, "Attempting to resolve next hop %s on interface %s",
            destStr, ifName);
    ipMapTraceWrite(traceBuf);
  }

  ip = rtmtod(m, struct ip *);
  if (m->rtm_bufhandle == NULL)
  {
    netMbufHandle = sysapiRxNetMbufGet(L7_MBUF_RX_PRIORITY_NORMAL,
                                       L7_MBUF_IP_ALIGNED);
    if (netMbufHandle == L7_NULL)
    {
      ipForwardPktsDropped++;
      bufferPoolFree (rtBufferPoolId, (L7_uchar8 *) m);
      return ENOBUFS;
    }

    SYSAPI_NET_MBUF_GET_DATASTART(netMbufHandle, pdataStart);

    p = &pdataStart[0];
    memset(p,0,sizeof(L7_enetHeader_t));
    p += sizeof(L7_enetHeader_t);
    *(L7_ushort16 *)p = osapiHtons(L7_ETYPE_IP);
    p += 2;
    memcpy(p, m->rtm_data, m->rtm_len);
    SYSAPI_NET_MBUF_SET_DATALENGTH(netMbufHandle, m->rtm_len + (p - pdataStart));
    m->rtm_bufhandle = (void *)netMbufHandle;
  }

  callbackparam = 0;
  rc = bufferPoolAllocate (rtBufferPoolId, &buffer_addr);
  if (rc != L7_SUCCESS)
  {
    ipForwardBufAllocFail++;
    rc = L7_FAILURE;
  }
  else
  {
    callbackparam = (cbparam_t *) buffer_addr;
    memset(callbackparam,0,sizeof(callbackparam));
    callbackparam->intIfNum = intIfNum;
    callbackparam->routerIP = dest_ip_addr;
    callbackparam->srcrt = srcrt;

    /* Try to resolve the ARP entry. If it is net directed broadcast packet,
     there is no need to resolve the arp because, mac is a broadcast mac.
    */
    if ((netMask != 0xffffffff)
        && ((osapiNtohl(ip->ip_dst.s_addr) & ~netMask) == ~netMask))
    {
      rc = nimGetBroadcastMACAddr(intIfNum,mac);
    }
    else
    {
      rc = ipMapArpAddrResolve (intIfNum,
                                dest_ip_addr,
                                mac,
                                rtIPForwardArpCallback,
                                (L7_uint32)m,
                                (L7_uint32)callbackparam);
    }
  }

  if (rc == L7_SUCCESS)
  {
    /* ARP entry already resolved. So why did hw not forward the packet?
     * Maybe L2 table entry has aged out. That can happen with asymmetric
     * traffic, where we forward to a neighbor but he doesn't send to us.
     * Update swFwdCount on ARP entry and possibly send an out-of-cycle
     * ARP request to repopulate the L2 table. */
    ipMapArpAddrUsed(dest_ip_addr, intIfNum);

    rtLvl7TxIPForward ( (L7_netBufHandle)m->rtm_bufhandle, intIfNum, mac);
    bufferPoolFree (rtBufferPoolId, (L7_uchar8 *) m);
    bufferPoolFree (rtBufferPoolId, (L7_uchar8 *) callbackparam);
    return L7_SUCCESS;
  }

  /* ARP entry is in progress. ARP will call rtIPForwardArpCallback() when the
   * IP address becomes resolved or resolution fails. */
  if (rc == L7_ASYNCH_RESPONSE)
  {
    if (ipMapTraceFlags & IPMAP_TRACE_FORWARDING)
    {
      L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
      L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(ntohl(dest_ip_addr), destStr);
      sprintf(traceBuf, "ARP in progress for IP address %s", destStr);
      ipMapTraceWrite(traceBuf);
    }
    SYSAPI_NET_MBUF_SET_LOC((L7_netBufHandle)m->rtm_bufhandle, MBUF_LOC_ARP_PEND_Q);
    return EARPINPROGRESS;
  }

  /* Can't resolve. Delete the packet. */
  if (ipMapTraceFlags & IPMAP_TRACE_FORWARDING)
  {
    L7_uchar8 traceBuf[IPMAP_TRACE_LEN_MAX];
    L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];
    osapiInetNtoa(ntohl(dest_ip_addr), destStr);
    sprintf(traceBuf, "rtInIPForwardArpTable() failed to resolve IP address %s",
            destStr);
    ipMapTraceWrite(traceBuf);
  }

  SYSAPI_NET_MBUF_FREE((L7_netBufHandle)m->rtm_bufhandle);
  bufferPoolFree (rtBufferPoolId, (L7_uchar8 *) m);
  if (callbackparam)
  {
    bufferPoolFree (rtBufferPoolId, (L7_uchar8 *) callbackparam);
  }
  return EHOSTUNREACH;
}

/*********************************************************************
*
* @purpose  Process incoming IP and ARP frames.
*
* @param    bufHandle   Network buffer handle for the incoming message.
* @param    *pduInfo    pointer to PDU info structure which stores intIfNum and vlanId
*
* @returns  L7_SUCCESS  Frame has been consumed.
* @returns  L7_FAILURE  Frame has not been consumed.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ipmRouterIfFrameProcess (L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uint32 nbytes;
  L7_uchar8 *data;
  L7_uint32 ifState = L7_DISABLE;
  L7_RC_t   rc;

  if ((ipMapRtrIntfOperModeGet(pduInfo->intIfNum, &ifState) != L7_SUCCESS) ||
      (ifState != L7_ENABLE))
  {
    ipForwardRtrIntfInvalid++;
    return L7_FAILURE;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, nbytes);

  /* Check whether LVL 7 forwarding stack or ARP code wants to handle this frame.
  ** If frame is handled then we have nothing else to do. Otherwise, return a failure
  ** to indicate the frame should be given to the local IP stack.
  */
  if ((rc = rtoIPv4Handle (bufHandle, pduInfo)) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  /* Special Case: An L7_ERROR return means the packet is destined to the
   *               system CPU.  Return an L7_FAILURE from here so that the
   *               packet is presented to the CPU via the DTL end device.
   *               Do not free the packet.
   */
  if (rc == L7_ERROR)
  {
    return L7_FAILURE;
  }

  /* If we get here, a failure needs to be returned which indicates that the packet
  ** needs to be sent to the local IP stack.
  **
  ** On Linux, any packets that would have gotten here have already been discarded.
  ** This is accomplished by adding a sysnet registration to the Linux code for all
  ** packets coming into the IP mapping layer and filtering out the ones that would
  ** have gotten this far. These packets are also sent to the IP stack in the kernel
  ** which allows for management packets arriving at a network port.
  */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Receives IP packets
*
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    intIfNum @b     {(input)} Internal Interface Number
*                           on which the packet was received
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR    - Frame has not been consumed.
*
* @notes    This routine frees buffer when it is not passed to IP stack
*
* @end
*********************************************************************/
L7_RC_t ipmRecvLocal(L7_netBufHandle netBufHandle,
                     L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  sysnet_pdu_info_t pduInfo;

  /* Call interceptors who are interested in IP frames to be delivered to the local IP stack.
  ** If L7_TRUE is returned, the frame was either discarded or consumed, which means that the
  ** network buffer has been freed by the intercept call, or will be freed by the consumer.
  */
  memset(&pduInfo, 0, sizeof(sysnet_pdu_info_t));
  pduInfo.intIfNum = intIfNum;
  if (SYSNET_PDU_INTERCEPT(L7_AF_INET, SYSNET_INET_LOCAL_IN, netBufHandle, &pduInfo,
                           L7_NULLPTR, L7_NULLPTR) == L7_TRUE)
    return L7_SUCCESS;

  ipForwardPktsToStack++;
  if ((rc = ipmLocalPacketProcess(intIfNum, netBufHandle)) != L7_SUCCESS)
    SYSAPI_NET_MBUF_FREE(netBufHandle);

  return rc;
}


/*********************************************************************
* @purpose  Initialize Buffer pool and ifnet structures for the IP forwarding component.
*           This function executes Phase 1 requirements for rtIpRecv
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t rtIpRecvCnfgrInitPhase1Process(void)
{
  L7_uint32 buffer_size;
  L7_uint32 num_buffers = FD_RTR_MAX_NUM_BUFFERS; /* Number of buffer requested in the pool */
  L7_RC_t rc;
  L7_uint32 pool_size;
  L7_uchar8 *pool;
  L7_uint32 buff_count;

  /* Determine the biggest buffer size that we will need.
  ** We use buffers to store the following structures:
  **         rtmbuf, ifnet, cbparam_t
  */
  buffer_size = sizeof (struct rtmbuf);
  if (buffer_size < sizeof (cbparam_t))
  {
    buffer_size = sizeof (cbparam_t);
  }

  /* Determine how much memory we need for the desired number of buffers.
  */
  pool_size = bufferPoolSizeCompute (num_buffers, buffer_size);

  /* Allocate memory for the buffer pool.
  */
  pool = osapiMalloc (L7_IP_MAP_COMPONENT_ID, pool_size);


  if (pool == L7_NULLPTR)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
              "Out of memory\n");
      return L7_FAILURE;
  }

  /* Create the buffer pool.
  */
  rc = bufferPoolCreate (pool,
                         pool_size,
                         buffer_size,
                         "IP Forward",
                         &rtBufferPoolId,
                         &buff_count);
  if (rc != L7_SUCCESS)
  {

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Buffer pool create failed\n");
    return L7_FAILURE;
  }

  /* Since we use bufferPoolSizeCompute function to determine pool size, the
  ** number of buffers in the pool should be equal to the number of buffers we want.
  */
  if (buff_count != num_buffers)
  {

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Invalid buffer pool size: buff_count = %d, num_buffers = %d\n",
             buff_count, num_buffers);
    return L7_FAILURE;
  }

  /* Allocate memory for the ifnet structures.  This memory is never freed. */
  gRtmIfnet = osapiMalloc ( L7_IP_MAP_COMPONENT_ID, sizeof(struct rtm_ifnet) * (L7_RTR_MAX_RTR_INTERFACES+1) );
  if (gRtmIfnet == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IP_MAP_COMPONENT_ID,
            "Unable to malloc memory for network interfaces\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}




/*********************************************************************
* @purpose  Free Buffer pool and ifnet structures for the IP forwarding component.
*           This function executes Phase 1 Fini requirements for rtIpRecv
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*********************************************************************/
void rtIpRecvCnfgrFiniPhase1Process(void)
{
  bufferPoolTerminate(rtBufferPoolId);

  if (gRtmIfnet != L7_NULLPTR)
  {
    OSAPI_FREE_MEMORY(gRtmIfnet);
  }
}




/*********************************************************************
* @purpose  Return buffer pool ID for the IP forwarding code.
*
*
* @returns  buffer pool ID.
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 rtMemPoolIdGet (void)
{
  return rtBufferPoolId;

}

/*********************************************************************
* @purpose  Fill in the ifnet structure for the corresponding router interface
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE if rtrIfNum is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rtIfNetCreate(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  gRtmIfnet[rtrIfNum].if_index = (L7_ushort16)intIfNum;
  gRtmIfnet[rtrIfNum].if_flags = IFF_BROADCAST | IFF_MULTICAST;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Zero-out in the ifnet structure for the corresponding router interface
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE if rtrIfNum is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rtIfNetDelete(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  memset(&gRtmIfnet[rtrIfNum], 0, sizeof(struct rtm_ifnet));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Mark ifnet if_flags up
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE if rtrIfNum is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rtIfNetUp(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  gRtmIfnet[rtrIfNum].if_flags |= IFF_UP;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Mark ifnet if_flags down
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE if rtrIfNum is invalid
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rtIfNetDown(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum;

  if (_ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  gRtmIfnet[rtrIfNum].if_flags &= ~IFF_UP;

  return L7_SUCCESS;
}
