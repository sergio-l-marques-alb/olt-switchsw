/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    snooping_debug.c
* @purpose     snooping debug functions
* @component   snooping
* @comments    none
* @create      08/25/2006
* @author      ssuvarna
*              drajendra
* @end
*
*********************************************************************/
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "osapi_support.h"

#include "log.h"
#include "support_api.h"
#include "l7utils_inet_addr_api.h"

#include "snooping_defs.h"
#include "snooping_util.h"
#include "snooping_debug.h"
#include "snooping_api.h"
#include "snooping_outcalls.h"
#include "snooping_proto.h"
#include "snooping_db.h"

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
static L7_uchar8 snoopDebugFlags[SNOOP_INSTANCES_MAX];
static L7_BOOL snoopIsDebugEnabled = L7_FALSE;


#define SNOOP_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d,%s), Vlan_Id:%d SrcMac: %s DestMac: %s SrcIP: %s DestIP: %s Type: %s Group: %s\n"

#define SNOOP_DEBUG_PACKET_RX_DROP_FORMAT "Pkt RX - %s\n"
#define SNOOP_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf: %d/%d/%d(%d,%s), Vlan_Id:%d SrcMac: %s DestMac: %s SrcIP: %s DestIP: %s Type: %s Group: %s\n"
#define SNOOP_DEBUG_PACKET_VLAN_TX_FORMAT "Pkt TX -  Vlan_Id:%d SrcMac: %s DestMac: %s SrcIP: %s DestIP: %s Type: %s Group: %s\n"

void snoopDebugParseIgmpv3GroupRecords(L7_uchar8 *pIpPayload,
                                       L7_uint32 ip_payload_length,
                                       L7_uchar8 *buff, L7_uint32 buffLen,
                                       L7_uchar8 family)
{
  L7_uchar8 *dataPtr, recType, auxDataLen;
  L7_ushort16 noOfGroups, noOfSources;
  L7_inet_addr_t          groupAddr;
  L7_uchar8               ipBuf[16];
  L7_uchar8               groupIp[SNOOP_IP6_ADDR_BUFF_SIZE];
  L7_uchar8               tmpBuff[SNOOP_IP6_ADDR_BUFF_SIZE+10];
  L7_uint32               ipv4Addr, count = 0;

  dataPtr = pIpPayload;
  dataPtr +=  MGMD_REPORT_NUM_SOURCES_OFFSET;
  SNOOP_GET_SHORT(noOfGroups, dataPtr);

  osapiSnprintf(tmpBuff, sizeof(tmpBuff), "NumGroups: %d ", noOfGroups);
  count += strlen(tmpBuff);
  osapiStrncat(buff, tmpBuff, strlen(tmpBuff));
  while (noOfGroups > 0)
  {
    SNOOP_GET_BYTE(recType, dataPtr); /* Record type */
    SNOOP_GET_BYTE(auxDataLen, dataPtr); /* AuxData Len */
    SNOOP_GET_SHORT(noOfSources, dataPtr); /* Number of sources */
    if (family == L7_AF_INET)
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
    if ((noOfSources == 0) && (recType == L7_IGMP_CHANGE_TO_INCLUDE_MODE))
    {
      snoopInetHtoP(&groupAddr, groupIp);
      osapiSnprintf(tmpBuff, sizeof(tmpBuff), "Leave: %s ", groupIp);
    }
    else
    {
      snoopInetHtoP(&groupAddr, groupIp);
      osapiSnprintf(tmpBuff, sizeof(tmpBuff), "Join: %s ", groupIp);
    }
    count += strlen(tmpBuff);

    if (count < buffLen)
    {
      osapiStrncat(buff, tmpBuff, strlen(tmpBuff));
    }
    else
    {
      return;
    }

    if (family == L7_AF_INET)
    {
      dataPtr += ((auxDataLen * 4) + (noOfSources * sizeof(L7_in_addr_t)));
    }
    else
    {
      dataPtr += ((auxDataLen * 4) + (noOfSources * sizeof(L7_in6_addr_t)));
    }

    /* Check if it is a malformed packet */
    if (ip_payload_length != 0 && ((dataPtr - pIpPayload) > ip_payload_length))
    {
      return;
    }
    noOfGroups -= 1;
  }/* End of while loop */
}
/*********************************************************************
* @purpose Trace snoop packets received
*
* @param   mcastPacket     @b{(input)} holder for snooped packet info
* @param   dropFlag        @b{(input)} Drop flag to indicate if packet
*                                      is being dropped
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugPacketRxTrace(mgmdSnoopControlPkt_t *mcastPacket,
                             L7_uchar8 dropFlag)
{
  snoop_cb_t       *pSnoopCB = L7_NULLPTR;
  L7_uchar8         type[25], *dataPtr;
  L7_uint32         ipv4Addr, unit, slot, port;
  L7_in6_addr_t     ipv6Addr;
  L7_inet_addr_t    groupAddr;
  static L7_uchar8  groupIp[12*SNOOP_IP6_ADDR_BUFF_SIZE];
  L7_uchar8         srcMac[SNOOP_MAC_ADDR_BUFF_SIZE];
  L7_uchar8         destMac[SNOOP_MAC_ADDR_BUFF_SIZE];
  L7_uchar8         srcIp[SNOOP_IP6_ADDR_BUFF_SIZE];
  L7_uchar8         destIp[SNOOP_IP6_ADDR_BUFF_SIZE], igmpType;
  L7_enetHeader_t  *enetHdr;

  pSnoopCB = mcastPacket->cbHandle;
  if (pSnoopCB->snoopDebugPacketTraceRxFlag == L7_TRUE)
  {
    if (dropFlag)
    {
      switch(dropFlag)
      {
      case SNOOP_PKT_DROP_NOT_READY:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reason: System not ready ");
        break;
      case SNOOP_PKT_DROP_BAD_VLAN:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reson: Invalid vlan ID");
        break;
      case SNOOP_PKT_DROP_BAD_ETYPE:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reason: Invalid Ether Type");
        break;
      case SNOOP_PKT_DROP_RX_CPU:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Flood within the VLAN  Reason: Received on CPU interface");
        break;
      case SNOOP_PKT_DROP_BAD_IPHDR_LEN:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reason: Invalid IP header length");
        break;
      case SNOOP_PKT_DROP_BAD_IGMP_CKSUM:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reason: Invalid IGMP Checksum");
        break;
      case SNOOP_PKT_DROP_BAD_IP_CKSUM:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reason: Invalid IPv4 Header Checksum");
        break;
      case SNOOP_PKT_DROP_BAD_IPV4_DEST:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reason: Invalid IPv4 Multicast Destination Address");
        break;

      case SNOOP_PKT_DROP_BAD_ICMP_CKSUM:
        SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_DROP_FORMAT,
                            "Action: Drop  Reason: Invalid ICMPv6 checksum");
        break;

      default:
        break;
    }
     return;
   } /* end of drop flag check */

    enetHdr = (L7_enetHeader_t *)(&mcastPacket->payLoad[0]);
    osapiSnprintf(srcMac, sizeof(srcMac), "%02X:%02X:%02X:%02X:%02X:%02X",
                  enetHdr->src.addr[0],enetHdr->src.addr[1],
                  enetHdr->src.addr[2],enetHdr->src.addr[3],
                  enetHdr->src.addr[4],enetHdr->src.addr[5]);
    osapiSnprintf(destMac, sizeof(destMac), "%02X:%02X:%02X:%02X:%02X:%02X",
                  enetHdr->dest.addr[0],enetHdr->dest.addr[1],
                  enetHdr->dest.addr[2],enetHdr->dest.addr[3],
                  enetHdr->dest.addr[4],enetHdr->dest.addr[5]);
    memset(srcIp, 0x00, sizeof(srcIp));
    memset(destIp, 0x00, sizeof(destIp));

    snoopInetHtoP(&mcastPacket->srcAddr, srcIp);
    snoopInetHtoP(&mcastPacket->destAddr, destIp);

    if (pSnoopCB->family == L7_AF_INET)
    {
      memset(groupIp, 0x00, sizeof(groupIp));
      memset(type, 0x00, sizeof(type));
      /* Point to the start of IP Payload location in the payload */
      dataPtr = mcastPacket->ip_payload;
      SNOOP_GET_BYTE(igmpType, dataPtr); /* Get the IGMP Type */
      SNOOP_UNUSED_PARAM(dataPtr);
      dataPtr = mcastPacket->ip_payload + 4;
      switch (igmpType)
      {
      case L7_IGMP_MEMBERSHIP_QUERY:

        osapiSnprintf(type, sizeof(type), "%s", "IGMP Membership_Query");
        SNOOP_GET_ADDR(&ipv4Addr, dataPtr);  /* Group Address */
        osapiInetNtoa(ipv4Addr, groupIp);
        break;

      case L7_IGMP_V1_MEMBERSHIP_REPORT:
        osapiSnprintf(type, sizeof(type), "%s", "IGMP V1_Membership_Report");
        SNOOP_GET_ADDR(&ipv4Addr, dataPtr); /* Group Address */
        osapiInetNtoa(ipv4Addr, groupIp);
        break;

      case L7_IGMP_V2_MEMBERSHIP_REPORT:
        osapiSnprintf(type, sizeof(type), "%s", "IGMP V2_Membership_Report");
        SNOOP_GET_ADDR(&ipv4Addr, dataPtr); /* Group Address */
        osapiInetNtoa(ipv4Addr, groupIp);
        break;

      case L7_IGMP_V3_MEMBERSHIP_REPORT:
        osapiSnprintf(type, sizeof(type), "%s", "IGMP V3_Membership_Report");
        snoopDebugParseIgmpv3GroupRecords(mcastPacket->ip_payload, mcastPacket->ip_payload_length,
                                          groupIp, sizeof(groupIp), L7_AF_INET);
        break;

      case L7_IGMP_V2_LEAVE_GROUP:
        osapiSnprintf(type, sizeof(type), "%s", "IGMP V2_Leave_Group");
        SNOOP_GET_ADDR(&ipv4Addr, dataPtr);  /* Group Address */
        osapiInetNtoa(ipv4Addr, groupIp);
        break;

      case L7_IGMP_DVMRP:
        osapiSnprintf(type, sizeof(type), "%s", "DVMRP");
        break;

      case L7_IGMP_PIM_V1:
        osapiSnprintf(type, sizeof(type), "%s", "PIM_V1");
        break;

      default:
        break;
      }
    }/* end of igmp packet parse */
    else
    {
      /* MLD Packet */
      memset(groupIp, 0x00, sizeof(groupIp));
      memset(type, 0x00, sizeof(type));
      /* Point to the start of IP Payload location in the payload */
      dataPtr = mcastPacket->ip_payload + 8;
      switch (mcastPacket->msgType)
      {
      case L7_MLD_MEMBERSHIP_QUERY:
        osapiSnprintf(type, sizeof(type), "%s", "MLD Membership_Query");
        SNOOP_GET_ADDR6(&ipv6Addr, dataPtr); /* Group Address */
        inetAddressSet(L7_AF_INET6,&ipv6Addr, &groupAddr);
        snoopInetHtoP(&groupAddr, groupIp);
        break;

      case L7_MLD_V1_MEMBERSHIP_REPORT:
        osapiSnprintf(type, sizeof(type), "%s", "MLD V1_Membership_Report");
        SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);  /* Group Address */
        inetAddressSet(L7_AF_INET6, &ipv6Addr, &groupAddr);
        snoopInetHtoP(&groupAddr, groupIp);
        break;

      case L7_MLD_V1_MEMBERSHIP_DONE:
        osapiSnprintf(type, sizeof(type), "%s", "MLD V1_Membership_Done");
        SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);  /* Group Address */
        inetAddressSet(L7_AF_INET6, &ipv6Addr, &groupAddr);
        snoopInetHtoP(&groupAddr, groupIp);
        break;

      case L7_MLD_V2_MEMBERSHIP_REPORT:
        osapiSnprintf(type, sizeof(type), "%s", "MLD V2_Membership_Report");
        snoopDebugParseIgmpv3GroupRecords(mcastPacket->ip_payload, mcastPacket->ip_payload_length,
                                          groupIp, sizeof(groupIp), L7_AF_INET6);
        break;

      default:
        break;
      }
    } /* end of mld packet check */
    SNOOP_UNUSED_PARAM(dataPtr);
    if (usmDbUnitSlotPortGet(mcastPacket->intIfNum, &unit, &slot, &port)
        == L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(mcastPacket->intIfNum, L7_SYSNAME, ifName);

      SNOOP_USER_TRACE_RX(SNOOP_DEBUG_PACKET_RX_FORMAT,
                          unit, slot, port, mcastPacket->intIfNum, ifName,
                          mcastPacket->vlanId, srcMac, destMac,
                          srcIp, destIp, type, groupIp);
    }/* end of slotport get */
  }/* End of trace flag enable check */
}
/*********************************************************************
* @purpose Trace snoop packets transmitted
*
* @param   intIfNum       @b{(input)} Internal Interface Number
* @param   vlanId         @b{(input)} Vlan ID
* @param   payload        @b{(input)} pointer to packet
* @param   family         @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugPacketTxTrace(L7_uint32 intIfNum, L7_uint32 vlanId,
                             L7_uchar8 *payload, L7_uchar8 family)
{
  snoop_cb_t       *pSnoopCB;
  L7_uchar8         type[25], *dataPtr, *ipPayLoad, byteVal;
  L7_uint32         ipv4Addr, unit, slot, port, xtenHdrLen;
  L7_in6_addr_t     ipv6Addr;
  static L7_uchar8  groupIp[12*SNOOP_IP6_ADDR_BUFF_SIZE];
  L7_uchar8         srcMac[SNOOP_MAC_ADDR_BUFF_SIZE];
  L7_uchar8         destMac[SNOOP_MAC_ADDR_BUFF_SIZE];
  L7_uchar8         srcIp[SNOOP_IP6_ADDR_BUFF_SIZE];
  L7_uchar8         destIp[SNOOP_IP6_ADDR_BUFF_SIZE], igmpType;
  L7_enetHeader_t  *enetHdr;
  L7_ipHeader_t     ip_header;
  L7_ip6Header_t    ip6_header;
  L7_inet_addr_t    groupAddr;

  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return;
  }

  if (pSnoopCB->snoopDebugPacketTraceTxFlag != L7_TRUE)
  {
    return;
  }

  enetHdr = (L7_enetHeader_t *)(payload);
  osapiSnprintf(srcMac, sizeof(srcMac), "%02X:%02X:%02X:%02X:%02X:%02X",
                enetHdr->src.addr[0],enetHdr->src.addr[1],
                enetHdr->src.addr[2],enetHdr->src.addr[3],
                enetHdr->src.addr[4],enetHdr->src.addr[5]);
  osapiSnprintf(destMac, sizeof(destMac), "%02X:%02X:%02X:%02X:%02X:%02X",
                enetHdr->dest.addr[0],enetHdr->dest.addr[1],
                enetHdr->dest.addr[2],enetHdr->dest.addr[3],
                enetHdr->dest.addr[4],enetHdr->dest.addr[5]);

  memset(groupIp, 0x00, sizeof(groupIp));
  memset(type, 0x00, sizeof(type));
  memset(srcIp, 0x00, sizeof(srcIp));
  memset(destIp, 0x00, sizeof(destIp));

  if (pSnoopCB->family == L7_AF_INET)
  {
    dataPtr = payload + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE;
    SNOOP_GET_BYTE(ip_header.iph_versLen, dataPtr);
    dataPtr = payload + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + 12;
    SNOOP_GET_ADDR(&ip_header.iph_src, dataPtr);
    SNOOP_GET_ADDR(&ip_header.iph_dst, dataPtr);
    osapiInetNtoa(ip_header.iph_src, srcIp);
    osapiInetNtoa(ip_header.iph_dst, destIp);
    dataPtr += ((ip_header.iph_versLen & 0x0f) * 4) - L7_IP_HDR_LEN;
    ipPayLoad = dataPtr;
    SNOOP_GET_BYTE(igmpType, dataPtr); /* Get the IGMP Type */
    dataPtr = dataPtr + 3;
    switch(igmpType)
    {
    case L7_IGMP_MEMBERSHIP_QUERY:
      osapiSnprintf(type, sizeof(type), "%s", "Membership_Query");
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
      osapiInetNtoa(ipv4Addr, groupIp);
      break;

    case L7_IGMP_V1_MEMBERSHIP_REPORT:
      osapiSnprintf(type, sizeof(type), "%s", "V1_Membership_Report");
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
      osapiInetNtoa(ipv4Addr, groupIp);
      break;

    case L7_IGMP_V2_MEMBERSHIP_REPORT:
      osapiSnprintf(type, sizeof(type), "%s", "V2_Membership_Report");
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
      osapiInetNtoa(ipv4Addr, groupIp);
      break;

    case L7_IGMP_V3_MEMBERSHIP_REPORT:
      osapiSnprintf(type, sizeof(type), "%s", "V3_Membership_Report");
      snoopDebugParseIgmpv3GroupRecords(ipPayLoad, 0, groupIp, sizeof(groupIp), L7_AF_INET);
      break;

    case L7_IGMP_V2_LEAVE_GROUP:
      osapiSnprintf(type, sizeof(type), "%s", "V2_Leave_Group");
      SNOOP_GET_ADDR(&ipv4Addr, dataPtr);    /* Group Address */
      osapiInetNtoa(ipv4Addr, groupIp);
      break;

    case L7_IGMP_DVMRP:
      osapiSnprintf(type, sizeof(type), "%s", "DVMRP");
      break;

    case L7_IGMP_PIM_V1:
      osapiSnprintf(type, sizeof(type), "%s", "PIM_V1");
      break;

    default:
      break;
    }
  }
  else
  {
    /* MLD Parse */
    dataPtr = payload + L7_ENET_HDR_SIZE + L7_ENET_ENCAPS_HDR_SIZE + 6;
    SNOOP_GET_BYTE(ip6_header.next, dataPtr);
    SNOOP_GET_BYTE(ip6_header.hoplim, dataPtr);
    SNOOP_GET_ADDR6(ip6_header.src, dataPtr);
    SNOOP_GET_ADDR6(ip6_header.dst, dataPtr);
    inetAddressSet(L7_AF_INET6, ip6_header.src, &groupAddr);
    snoopInetHtoP(&groupAddr, srcIp);
    inetAddressSet(L7_AF_INET6, ip6_header.dst, &groupAddr);
    snoopInetHtoP(&groupAddr, destIp);
    if (ip6_header.next == IP_PROT_ICMPV6)
    {
      ipPayLoad = dataPtr;
      igmpType =  *dataPtr;
      dataPtr = dataPtr + 8;
    }
    else
    {
      SNOOP_GET_BYTE(ip6_header.next, dataPtr);
      SNOOP_GET_BYTE(byteVal, dataPtr);
      xtenHdrLen = SNOOP_IP6_HOPBHOP_LEN_GET(byteVal);
      ipPayLoad = dataPtr + xtenHdrLen - 2;
      igmpType =  *(dataPtr + xtenHdrLen - 2);
      dataPtr  =  dataPtr + xtenHdrLen - 2 + 8;
    }

    switch (igmpType)
    {
    case L7_MLD_MEMBERSHIP_QUERY:
      osapiSnprintf(type, sizeof(type), "%s", "MLD Membership_Query");
      SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);    /* Group Address */
      inetAddressSet(L7_AF_INET6,&ipv6Addr, &groupAddr);
      snoopInetHtoP(&groupAddr, groupIp);
      break;

    case L7_MLD_V1_MEMBERSHIP_REPORT:
      osapiSnprintf(type, sizeof(type), "%s", "MLD V1_Membership_Report");
      SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);    /* Group Address */
      inetAddressSet(L7_AF_INET6, &ipv6Addr, &groupAddr);
      snoopInetHtoP(&groupAddr, groupIp);
      break;

    case L7_MLD_V1_MEMBERSHIP_DONE:
      osapiSnprintf(type, sizeof(type), "%s", "MLD V1_Membership_Done");
      SNOOP_GET_ADDR6(&ipv6Addr, dataPtr);    /* Group Address */
      inetAddressSet(L7_AF_INET6, &ipv6Addr, &groupAddr);
      snoopInetHtoP(&groupAddr, groupIp);
      break;

    case L7_MLD_V2_MEMBERSHIP_REPORT:
      osapiSnprintf(type, sizeof(type), "%s", "MLD V2_Membership_Report");
      snoopDebugParseIgmpv3GroupRecords(ipPayLoad, 0, groupIp, sizeof(groupIp), L7_AF_INET6);
      break;

    default:
      break;
    }
  }
  SNOOP_UNUSED_PARAM(dataPtr);
  if (intIfNum && usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    SNOOP_USER_TRACE_TX(SNOOP_DEBUG_PACKET_TX_FORMAT,
                        unit, slot, port, intIfNum, ifName, vlanId,
                        srcMac, destMac, srcIp, destIp, type,groupIp);
  }
  else
  {
    SNOOP_USER_TRACE_TX(SNOOP_DEBUG_PACKET_VLAN_TX_FORMAT,
                        vlanId, srcMac, destMac, srcIp, destIp, type,groupIp);
  }
}
/************************************************************************
* @purpose  Get the current status of displaying snoop packet debug info
*
* @param    transmitFlag  @b{(output)}  Tx Debug Flag
* @param    receiveFlag   @b{(output)}  Rx Debug Flag
* @param    family        @b{(input)}   L7_AF_INET  => IGMP Snooping
*                                       L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
************************************************************************/
L7_RC_t snoopDebugPacketTraceFlagGet(L7_BOOL *transmitFlag, L7_BOOL *receiveFlag,
                                     L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get a valid control block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  *transmitFlag = pSnoopCB->snoopDebugPacketTraceTxFlag;
  *receiveFlag  = pSnoopCB->snoopDebugPacketTraceRxFlag;
  return L7_SUCCESS;
}
/*****************************************************************************
* @purpose  Turns on/off the displaying of snoop packet debug info of
*           a snoop instance
*
* @param    transmitFlag  @b{(input)}  Tx Debug Flag
* @param    receiveFlag   @b{(input)}  Rx Debug Flag
* @param    family        @b{(input)}  L7_AF_INET  => IGMP Snooping
*                                      L7_AF_INET6 => MLD Snooping
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*****************************************************************************/
L7_RC_t snoopDebugPacketTraceFlagSet(L7_BOOL transmitFlag, L7_BOOL receiveFlag,
                                     L7_uchar8 family)
{
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  /* Get a valid control block */
  if ((pSnoopCB = snoopCBGet(family)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  pSnoopCB->snoopDebugPacketTraceTxFlag = transmitFlag;
  pSnoopCB->snoopDebugPacketTraceRxFlag = receiveFlag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
  */

  pSnoopCB->snoopDebugCfg->hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Save configuration settings for snoop trace data of a s
*           snoop instance
*
* @param    pSnoopCB   @b{(input)} Control block
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t snoopDebugPacketTraceFlagSave(snoop_cb_t *pSnoopCB)
{
  pSnoopCB->snoopDebugCfg->cfg.snoopDebugPacketTraceTxFlag =
                               pSnoopCB->snoopDebugPacketTraceTxFlag;
  pSnoopCB->snoopDebugCfg->cfg.snoopDebugPacketTraceRxFlag =
                               pSnoopCB->snoopDebugPacketTraceRxFlag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    pSnoopCB   @b{(input)} Control block
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopDebugCfgUpdate(snoop_cb_t *pSnoopCB)
{
  (void)snoopDebugPacketTraceFlagSave(pSnoopCB);
}
/*********************************************************************
* @purpose  Read and apply the debug config of a snoop instance
*
* @param    pSnoopCB   @b{(input)} Control block
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopDebugCfgRead(snoop_cb_t *pSnoopCB)
{
  /* reset the debug cfg data */
  memset((void*)pSnoopCB->snoopDebugCfg, 0x00 ,sizeof(snoopDebugCfg_t));

  if (pSnoopCB->family == L7_AF_INET)
  {

    (void)sysapiSupportCfgFileGet(L7_SNOOPING_COMPONENT_ID,
                            SNOOP_IGMP_DEBUG_CFG_FILENAME,
                           (L7_char8 *)pSnoopCB->snoopDebugCfg,
                           (L7_uint32)sizeof(snoopDebugCfg_t),
                           &(pSnoopCB->snoopDebugCfg->checkSum),
                           SNOOP_IGMP_DEBUG_CFG_VER_CURRENT,
                           snoopIGMPDebugBuildDefaultConfigData, L7_NULL);
  }
  else if(pSnoopCB->family == L7_AF_INET6)
  {
    (void)sysapiSupportCfgFileGet(L7_SNOOPING_COMPONENT_ID,
                           SNOOP_MLD_DEBUG_CFG_FILENAME,
                           (L7_char8 *)pSnoopCB->snoopDebugCfg,
                           (L7_uint32)sizeof(snoopDebugCfg_t),
                           &(pSnoopCB->snoopDebugCfg->checkSum),
                           SNOOP_MLD_DEBUG_CFG_VER_CURRENT,
                           snoopMLDDebugBuildDefaultConfigData, L7_NULL);
  }

  pSnoopCB->snoopDebugCfg->hdr.dataChanged = L7_FALSE;
}
/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopDebugRegister(void)
{
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_SNOOPING_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = snoopDebugSave;
    supportDebugDescr.userControl.hasDataChanged = snoopDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = snoopDebugRestore;

    /* Register Internal Parms */

    /* Submit registration */

    (void) supportDebugRegister(supportDebugDescr);
}
/*********************************************************************
* @purpose  Print the current Snooping config values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopCfgDump(void)
{
  L7_char8 buf[32];
  L7_uint32 i, cbIndex;
  L7_uint32 intIfNum, mode, maxInstances;
  snoopIntfCfgData_t *pCfg;
  nimConfigID_t configIdNull;
  nimUSP_t usp;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  pSnoopCB = snoopCBFirstGet();
  maxInstances = pSnoopCB->snoopExec->maxSnoopInstances;
  memset(&configIdNull, 0, sizeof(nimConfigID_t));
  for (cbIndex = L7_NULL; cbIndex < maxInstances;
        cbIndex++, pSnoopCB++)
  {
    sysapiPrintf("\n");
    if (pSnoopCB->family == L7_AF_INET)
    {
      sysapiPrintf("IGMP Snooping\n");
    }
    else
    {
      sysapiPrintf("MLD Snooping\n");
    }

    sysapiPrintf("=============\n");

    if (pSnoopCB->snoopCfgData->snoopAdminMode == L7_ENABLE)
      osapiSnprintf(buf, sizeof(buf), "%s", "Enable");
    else
      osapiSnprintf(buf, sizeof(buf), "%s", "Disable");
    sysapiPrintf("Admin Mode - %s\n", buf);

    sysapiPrintf("Intfs configured - ");
    for (i = 1; i < L7_IGMP_SNOOPING_MAX_INTF; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pSnoopCB->snoopCfgData->snoopIntfCfgData[i].configId, &configIdNull))
      {
        continue;
      }
      if (nimIntIfFromConfigIDGet(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[i].configId), &intIfNum) != L7_SUCCESS)
      {
        continue;
      }
      if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) == L7_TRUE)
      {
        if ((snoopIntfCfgModeGet(intIfNum, &mode, pSnoopCB->family) == L7_SUCCESS)
            && (mode == L7_ENABLE))
        {
          if (nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), "%d/%d/%d", usp.unit, usp.slot, usp.port);
            sysapiPrintf("%s ", buf);
          }
        }
      }
    }
    sysapiPrintf("\n\n");

    sysapiPrintf("Intf   Group Membership Interval   Max Response Time   Mcast Router Expiry Time\n");
    sysapiPrintf("====   =========================   =================   ========================\n");
    for (i = 1; i < L7_IGMP_SNOOPING_MAX_INTF; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pSnoopCB->snoopCfgData->snoopIntfCfgData[i].configId, &configIdNull))
      {
        continue;
      }
      if (nimIntIfFromConfigIDGet(&(pSnoopCB->snoopCfgData->snoopIntfCfgData[i].configId), &intIfNum) != L7_SUCCESS)
      {
        continue;
      }
      if (snoopMapIntfIsConfigurable(intIfNum, &pCfg, pSnoopCB) == L7_TRUE)
      {
        memset(&usp, 0, sizeof(nimUSP_t));
        (void)nimGetUnitSlotPort(intIfNum, &usp);
        sysapiPrintf("%d/%d/%d    %4d                       %4d                  %2d\n",
               usp.unit, usp.slot, usp.port,
               pCfg->groupMembershipInterval, pCfg->responseTime, pCfg->mcastRtrExpiryTime);
      }
    }
    sysapiPrintf("\n");
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @functions snoopInfoShow
*
* @purpose   Displays all operation info for Snooping and querier
*
* @param     none
*
* @returns   L7_SUCCESS
*
* @comments  Used for debug
*
* @end
*
*********************************************************************/
void snoopInfoShow()
{
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  snoopInfoData_t *snoopEntry;
  L7_uint32 i, cbIndex, time, maxInstances;
  nimUSP_t usp;
  L7_RC_t rc;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;
  snoopGrpTimerData_t *pSnoopTimerData;
  snoopMrtrTimerData_t *pmrtrTimerData;
  snoopMrtrTimerDataKey_t key;
  snoopOperData_t        *pSnoopOperEntry  = L7_NULLPTR;

  pSnoopCB = snoopCBFirstGet();
  maxInstances = pSnoopCB->snoopExec->maxSnoopInstances;
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "Operational Info:\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n=============\n");

  for (cbIndex = L7_NULL; cbIndex < maxInstances;
        cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->family == L7_AF_INET)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "IGMP Snooping:\n");
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "MLD Snooping:\n");
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n=============\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "Forwarding entries:\n");
    rc = snoopFirstGet(mac, &vlanId, pSnoopCB->family);
    while (rc == L7_SUCCESS)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vlanId);
      snoopEntry = snoopEntryFind(mac, vlanId, pSnoopCB->family, L7_MATCH_EXACT);
      if (snoopEntry == L7_NULLPTR)
      {
       break;
      }
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "    Intfs      ExpTime \n");
      pSnoopTimerData = (snoopGrpTimerData_t *)SLLFirstGet(&snoopEntry->ll_timerList);
      while (pSnoopTimerData != L7_NULLPTR)
      {
        memset(&usp, 0, sizeof(nimUSP_t));
        (void)nimGetUnitSlotPort(pSnoopTimerData->intIfNum, &usp);
        if (appTimerTimeLeftGet(snoopEntry->timerCB,
                                  pSnoopTimerData->grpTimer, &time) != L7_SUCCESS)
        {
           time = 0;
        }
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "    %d/%d/%d     %4d  \n",
                       usp.unit, usp.slot, usp.port, time);
        pSnoopTimerData = (snoopGrpTimerData_t *)
                        SLLNextGet(&snoopEntry->ll_timerList,
                                   (L7_sll_member_t *)pSnoopTimerData);
      }
      rc = snoopNextGet(mac, vlanId, mac, &vlanId, pSnoopCB->family);
    }

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\nOperational Interfaces:\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "    VLAN   Intf\n");
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;
      rc = nimFirstValidIntfNumber(&i);
      while (rc == L7_SUCCESS)
      {
        if (snoopIsValidIntf(i) == L7_TRUE)
        {
          if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->snoopIntfMode, i))
          {
            memset(&usp, 0, sizeof(nimUSP_t));
            (void)nimGetUnitSlotPort(i, &usp);
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "    %4d   %d/%d/%d\n",
                          pSnoopOperEntry->vlanId, usp.unit,
                          usp.slot, usp.port);
          }
        }
        rc = nimNextValidIntfNumber(i, &i);
      }
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    }

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\nPorts with Multicast Routers attached:\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "    VLAN   Intf      ExpTime\n");
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;

      rc = nimFirstValidIntfNumber(&i);
      while (rc == L7_SUCCESS)
      {
        if (snoopIsValidIntf(i) == L7_TRUE)
        {
          if (L7_INTF_ISMASKBITSET(pSnoopOperEntry->mcastRtrAttached, i))
          {
            memset(&usp, 0, sizeof(nimUSP_t));
            (void)nimGetUnitSlotPort(i, &usp);
            key.intIfNum = i;
            key.vlanId   = pSnoopOperEntry->vlanId;

            pmrtrTimerData = (snoopMrtrTimerData_t *)
                      avlSearchLVL7(&pSnoopCB->snoopMrtrTimerAvlTree, &key,
                                     L7_MATCH_EXACT);
            if (pmrtrTimerData != L7_NULLPTR)
            {
              if (appTimerTimeLeftGet(pSnoopCB->snoopExec->timerCB,
                                      pmrtrTimerData->mrtrTimer, &time)!= L7_SUCCESS)
              {
                time = 0;
              }
            }
            else
            {
              time = 0;
            }
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "    %4d   %d/%d/%d     %4d\n",
                          pSnoopOperEntry->vlanId,
                          usp.unit, usp.slot, usp.port, time);
          }
        }
        rc = nimNextValidIntfNumber(i, &i);
      }
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\nSnooping VLAN Map:\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
" VLAN   Enabled    QuerierState   OperVersion  QueryTimer QuerierTimer   MaxRespCode\n");
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while(pSnoopOperEntry)
    {
      L7_uint32 queryTimer, querierTimer;
        vlanId = pSnoopOperEntry->vlanId;

        if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer != L7_NULL)
        {
          if (appTimerTimeLeftGet(pSnoopCB->snoopExec->timerCB,
                                  pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.querierExpiryTimer,
                                  &querierTimer)!= L7_SUCCESS)
          {
            querierTimer = 0;
          }
        }
        else
        {
          querierTimer = 0;
        }

        if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer != L7_NULL)
        {
          if (appTimerTimeLeftGet(pSnoopCB->snoopExec->timerCB,
                                  pSnoopOperEntry->snoopQuerierInfo.snoopQuerierTimerData.queryIntervalTimer,
                                  &queryTimer)!= L7_SUCCESS)
          {
            queryTimer = 0;
          }
        }
        else
        {
          queryTimer = 0;
        }

      sysapiPrintf(" %4d     %s          %d             %d           %4u        %4u           %d\n",
                    vlanId,
                    pSnoopCB->snoopCfgData->snoopVlanMode[vlanId]
               & SNOOP_VLAN_MODE ? "Y" : "N",
               pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState,
               pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperVersion,
               queryTimer, querierTimer,
               pSnoopOperEntry->snoopQuerierInfo.maxResponseCode);
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB, L7_MATCH_GETNEXT);
    }
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");


    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n=============\n");
  }
  return;
}

/*********************************************************************
* @purpose  Enable Debug Tracing in Snooping
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
* @returns  L7_FAILURE  if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
snoopDebugTraceEnable (void)
{
  if (snoopIsDebugEnabled == L7_TRUE)
  {
    SNOOP_DEBUG_PRINTF ("\nSNOOP Debug Tracing is already Enabled.\n");
    return (L7_FAILURE);
  }

  snoopIsDebugEnabled = L7_TRUE;
  SNOOP_DEBUG_PRINTF ("\nSNOOP Debug Tracing is Enabled.\n");

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Disable Debug Tracing in SNOOP
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
* @returns  L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
snoopDebugTraceDisable (void)
{
  if (snoopIsDebugEnabled != L7_TRUE)
  {
    SNOOP_DEBUG_PRINTF ("SNOOP Debug Tracing is already Disabled.\n");
    return (L7_FAILURE);
  }

  snoopIsDebugEnabled = L7_FALSE;
  SNOOP_DEBUG_PRINTF ("SNOOP Debug Tracing is Disabled.\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Shows the usage of the Snoop Debug Trace Utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void
snoopDebugTraceHelp (void)
{
  SNOOP_DEBUG_PRINTF ("snoopDebugTraceEnable()  - Enable Debug Tracing in Snooping.\n");
  SNOOP_DEBUG_PRINTF ("snoopDebugTraceDisable() - Disable Debug Tracing in Snooping.\n");
  SNOOP_DEBUG_PRINTF ("snoopDebugTraceAllFlagsReset(family) - Disable Debug Tracing for All Events.\n");
  SNOOP_DEBUG_PRINTF ("snoopDebugTraceFlagsShow(family) - Show the status of Trace Flags.\n");
  SNOOP_DEBUG_PRINTF ("snoopDebugTraceFlagsSet(flag,family) - Enable Debug Tracing for the Specified Flag.\n");
  SNOOP_DEBUG_PRINTF ("snoopDebugTraceFlagsReset(flag,family) - Disable Debug Tracing for the Specified Flag.\n");
  SNOOP_DEBUG_PRINTF ("     Flags  ....\n");
  SNOOP_DEBUG_PRINTF ("       0   -> To Trace SNOOP Timer Events.\n");
  SNOOP_DEBUG_PRINTF ("       1   -> To Trace SNOOP Querier Events.\n");
  SNOOP_DEBUG_PRINTF ("       2   -> To Trace the Protocol Control Packets Path.\n");
  SNOOP_DEBUG_PRINTF ("       3   -> To Trace the SNOOP Internal Events Path.\n");
  SNOOP_DEBUG_PRINTF ("       4   -> To Trace the SNOOP Checkpoint Service (NSF Only).\n");
  SNOOP_DEBUG_PRINTF ("       5   -> To Trace the SNOOP Checkpoint Service VERBOSE (NSF Only).\n");
  SNOOP_DEBUG_PRINTF ("     Family  ....\n");
  if (snoopProtocolGet(L7_AF_INET) == L7_TRUE)
  {
    SNOOP_DEBUG_PRINTF ("     0   -> To Trace IGMP Snooping.\n");
  }
  if (snoopProtocolGet(L7_AF_INET6) == L7_TRUE)
  {
    SNOOP_DEBUG_PRINTF ("     1   -> To Trace MLD Snooping.\n");
  }

  return;
}
/*********************************************************************
* @purpose  Clear all trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugTraceAllFlagsReset(L7_uint32 family)
{
  if (family < SNOOP_INSTANCES_MAX)
  {
    snoopDebugFlags[family] = 0;
  }
}
/*********************************************************************
* @purpose  Shows the Enabled/Disabled Trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugTraceFlagsShow(L7_uint32 family)
{
  if (family < SNOOP_INSTANCES_MAX)
  {
    SNOOP_DEBUG_PRINTF ("  Debug Trace Enabled                      ->   %s\n", (snoopIsDebugEnabled) ? "Y" : "N");
    SNOOP_DEBUG_PRINTF ("  Trace SNOOP Timer Events                 ->   %s\n", (snoopDebugFlags[family] & 1) ? "Y" : "N");
    SNOOP_DEBUG_PRINTF ("  Trace SNOOP Querier Events               ->   %s\n", (snoopDebugFlags[family] & 2) ? "Y" : "N");
    SNOOP_DEBUG_PRINTF ("  Trace the Protocol Control Packets Path  ->   %s\n", (snoopDebugFlags[family] & 4) ? "Y" : "N");
    SNOOP_DEBUG_PRINTF ("  Trace the SNOOP Internal Events Path     ->   %s\n", (snoopDebugFlags[family] & 8) ? "Y" : "N");
    SNOOP_DEBUG_PRINTF ("  Trace the SNOOP Checkpoint service       ->   %s\n", (snoopDebugFlags[family] & 16) ? "Y" : "N");
    SNOOP_DEBUG_PRINTF ("  Trace the SNOOP Checkpoint service VERB  ->   %s\n", (snoopDebugFlags[family] & 32) ? "Y" : "N");
  }
}

/*********************************************************************
* @purpose  Set a particular tracelevel
*
* @param    flag   {(input)} Trace Level
* @param    family {(input)} Snoop Instance
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugTraceFlagsSet(L7_uint32 flag, L7_uint32 family)
{
  if (family < SNOOP_INSTANCES_MAX && flag < SNOOP_DEBUG_FLAGS_BITS)
  {
    snoopDebugFlags[family] |= (1 << flag);
  }
}

/*********************************************************************
* @purpose  Set a particular tracelevel for all families
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugTraceFlagsSetAF(L7_uint32 flag)
{
  L7_uint32 family;

  for (family = 0; family < SNOOP_INSTANCES_MAX; family++)
  {
    if (flag < SNOOP_DEBUG_FLAGS_BITS)
    {
      snoopDebugFlags[family] |= (1 << flag);
    }
  }
}
/*********************************************************************
* @purpose  Reset a particular tracelevel
*
* @param    flag   {(input)} Trace Level
* @param    family {(input)} Snoop Instance
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugTraceFlagsReset(L7_uint32 flag, L7_uint32 family)
{
  if (family < SNOOP_INSTANCES_MAX && flag < SNOOP_DEBUG_FLAGS_BITS)
  {
    snoopDebugFlags[family] &= ~(1 << flag);
  }
}

/*********************************************************************
* @purpose  Reset a particular tracelevel for all families
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void snoopDebugTraceFlagsResetAF(L7_uint32 flag)
{
  L7_uint32 family;

  for (family = 0; family < SNOOP_INSTANCES_MAX; family++)
  {
    if (flag < SNOOP_DEBUG_FLAGS_BITS)
    {
      snoopDebugFlags[family] &= ~(1 << flag);
    }
  }
}

/*********************************************************************
*
* @purpose
*
* @param    flag   {(input)} Trace Level
* @param    family {(input)} Snoop Instance
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL
snoopDebugTraceFlagCheck (L7_uint32 traceFlag, L7_uchar8 family)
{
  if (snoopIsDebugEnabled != L7_TRUE)
  {
   return L7_FALSE;
  }

  if (family == L7_AF_INET)
  {
    family = 0;
  }
  else
  {
    family = 1;
  }

  if (snoopDebugFlags[family] & (1 << traceFlag))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}
/*********************************************************************
*
* @purpose To dump the free list used to maintain snoopEntry ctrl blks
*
* @param   none
*
* @returns  0 - Success
*           1 - Error
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dumpSnoopEntryTimerFreeList(void)
{
  L7_uint32 idx;
  snoop_eb_t  *pSnoopEB = snoopEBGet();

 if (pSnoopEB == L7_NULLPTR)
 {
   return 1;
 }

 SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\nFree Index: %4d\nFree List: ", pSnoopEB->snoopEntryTimerFreeIdx);
  for (idx = 0; idx < SNOOP_ENTRY_TIMER_BLOCKS_COUNT; idx++)
  {
    if (idx % 20 == 0)
     SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4d ", pSnoopEB->snoopEntryTimerFreeList[idx]);
  }
  return 0;
}

