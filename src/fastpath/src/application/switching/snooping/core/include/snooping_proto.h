/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename snooping_proto.h
*
* @purpose   Snooping Protocol Information
*
* @component Snooping
*
* @comments none
*
* @create 08-Dec-2006
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SNOOPING_PROTO_H
#define SNOOPING_PROTO_H

#include "snooping.h"


#define MGMD_REPORT_NUM_SOURCES_OFFSET 6

#define SNOOP_IGMPv1v2_HEADER_LENGTH    8
#define SNOOP_IGMPV3_HEADER_MIN_LENGTH  12

#define SNOOP_IGMPV3_RECORD_GROUP_HEADER_MIN_LENGTH 8

#define SNOOP_MLDV1_HEADER_LENGTH      24
#define SNOOP_MLDV2_HEADER_MIN_LENGTH  28

#define SNOOP_MLD_VERSION_1 1
#define SNOOP_MLD_VERSION_2 2

#define SNOOP_IGMP_VERSION_1 1
#define SNOOP_IGMP_VERSION_2 2
#define SNOOP_IGMP_VERSION_3 3

#define SNOOP_TTL_VALID_VALUE 1
#define SNOOP_TOS_VALID_VALUE 0xC0


#define SNOOP_IGMP_FP_DIVISOR 10
#define SNOOP_MLD_FP_DIVISOR  1000

#define SNOOP_IP_HDR_NEXT_PROTO_OFFSET   9
#define SNOOP_IP6_HDR_NEXT_PROTO_OFFSET  6

#define SNOOP_IP6_IPPROTO_HOPOPTS  0
#define SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN_CODE  0
#define SNOOP_IP6_RTR_ALERT_IPPROTO_HOPOPTS_LEN  8
#define SNOOP_IP6_IPPROTO_OPT_TYPE_PAD  1
#define SNOOP_IP6_IPPROTO_OPT_TYPE_RTR_ALERT  5

#define SNOOP_IP_TTL        1
#define SNOOP_IP6_HOP_LIMIT 1
#define SNOOP_IP_TOS        0xC0
#define SNOOP_IP6_TOS       0xC000
#define SNOOP_IP6_TOS       0xC000

#define IGMP_PKT_MIN_LENGTH       8
#define IGMP_V3_PKT_MIN_LENGTH    12
#define MLD_PKT_MIN_LENGTH        24
#define MLD_V2_PKT_MIN_LENGTH     28
#define IGMPV3_MAX_QUERY_DATA_LEN ((MGMD_MAX_QUERY_SOURCES * sizeof(L7_IP_ADDR_t)) + \
                                    sizeof(L7_mgmdv3Query_t) - \
                                    sizeof(L7_mgmdMsg_t) - sizeof(L7_uint32))
#define SNOOP_MGMD_RBST_VAR_GET(x) ((x) & 0x07)
#define SNOOP_MGMD_OTHER_QRYR_INTRVL_GET(x,y,z) (((x)*(y))+((z)/2))
#define SNOOP_IP6_HOPBHOP_LEN_GET(x) (((x) * 8) + 8)
#define SNOOP_MGMD_DEFAULT_RBST_VAR    2
#define SNOOP_MGMD_DEFAULT_QRY_INTRVL  125
#define SNOOP_MGMD_DEFAULT_MAX_RESP_TIME 10

#define SNOOP_IP6_ALL_HOSTS_ADDR       "FF02::1"
#define SNOOP_IP6_ALL_ROUTERS_ADDR     "FF02::2"
#define SNOOP_IP6_MLDV2_REPORT_ADDR    "FF02::16"

#define IGMP_FRAME_SIZE  (L7_ENET_HDR_SIZE+4+L7_ENET_ENCAPS_HDR_SIZE+L7_IP_HDR_LEN+SNOOP_IGMPv1v2_HEADER_LENGTH)      /* PTin modified: IGMP Snooping */
#define IGMPv3_FRAME_SIZE  (L7_ENET_HDR_SIZE+4+L7_ENET_ENCAPS_HDR_SIZE+L7_IP_HDR_LEN+SNOOP_IGMPV3_HEADER_MIN_LENGTH)  /* PTin modified: IGMP Snooping */

#define MLD_FRAME_SIZE  (L7_ENET_HDR_SIZE+4+L7_ENET_ENCAPS_HDR_SIZE+L7_IP6_HEADER_LEN+SNOOP_MLDV1_HEADER_LENGTH)
#define MLDv2_FRAME_SIZE  (L7_ENET_HDR_SIZE+4+L7_ENET_ENCAPS_HDR_SIZE+L7_IP6_HEADER_LEN+SNOOP_MLDV2_HEADER_MIN_LENGTH)

#define SNOOP_IP6_ADDR_BUFF_SIZE 42
#define SNOOP_MAC_ADDR_BUFF_SIZE 20
 

/*****************************************************************************
 PDU Message Structures
 ****************************************************************************/
/* MGMD message */
typedef struct L7_mgmdMsg_s
{
  L7_uchar8         mgmdType;         /* message type */
  L7_uchar8         igmpMaxRespTime;  /* maximum query response time */
  L7_ushort16       mgmdChecksum;     /* checksum */
  L7_ushort16       mldMaxRespTime;   /* maximum query response time */
  L7_ushort16       mgmdReserved;     /* Reserved */
  L7_inet_addr_t    mgmdGroupAddr;    /* group address */
} L7_mgmdMsg_t;


/* MGMD message */
typedef struct L7_mgmdQueryMsg_s
{
  L7_uchar8         mgmdType;         /* message type */
  L7_uchar8         igmpMaxRespTime;  /* maximum query response time */
  L7_ushort16       mgmdChecksum;     /* checksum */
  L7_ushort16       mldMaxRespTime;   /* maximum query response time */
  L7_ushort16       mgmdReserved;     /* Reserved */
  L7_inet_addr_t    mgmdGroupAddr;    /* group address */
  L7_uchar8         sQRV;
  L7_uchar8         qqic;
  L7_ushort16       numSources;
} L7_mgmdQueryMsg_t;

typedef struct ipv6pkt_s
{
  L7_in6_addr_t srcAddr;        /* Source Address */
  L7_in6_addr_t dstAddr;        /* Destination Address */
  L7_uint32     dataLen;        /* Upper-Layer Packet Length */
  L7_uchar8     zero[3];        /* Zero's */
  L7_uchar8     nxtHdr;         /* Next Header */
  L7_uchar8     icmpv6pkt[L7_MAX_FRAME_SIZE];/* ICMPv6 Packet */
} ipv6pkt_t;

typedef struct mgmdIpv6PseudoHdr_s
{
  L7_in6_addr_t srcAddr;        /* Source Address */
  L7_in6_addr_t dstAddr;        /* Destination Address */
  L7_uint32     dataLen;        /* Upper-Layer Packet Length */
  L7_uchar8     zero[3];        /* Zero's */
  L7_uchar8     nxtHdr;         /* Next Header */
  L7_uchar8     icmpv6pkt[SNOOP_MLDV2_HEADER_MIN_LENGTH];  /* ICMPv6-MLD Packet */
} mgmdIpv6PseudoHdr_t;


/* Place holder for the snooped IGMP/MLD MCAST Control Packet */
typedef struct mgmdSnoopControlPkt_s
{
  snoop_cb_t      *cbHandle;
  L7_uint32        intIfNum;         /* interface that received the packet  */
  L7_uint32        vlanId;
  L7_uint32        innerVlanId;
  L7_uchar8        destMac[L7_MAC_ADDR_LEN];
  L7_uint32        client_idx;       /* Client index */     /* PTin added: IGMP snooping */
  L7_uchar8        msgType;
  L7_inet_addr_t   srcAddr;
  L7_inet_addr_t   destAddr;
  L7_uchar8       *ip_payload;
  L7_uint32        ip_payload_length;
  L7_uchar8        tosByte;
  L7_char8         routerAlert;
  L7_uchar8        payLoad[L7_MAX_FRAME_SIZE];
  L7_uint32        length;           /* payload length */
} mgmdSnoopControlPkt_t;

/* Start of Function Prototype */
L7_RC_t snoopPacketHandle(L7_netBufHandle netBufHandle, 
                          sysnet_pdu_info_t *pduInfo,
                          L7_uchar8 family);
L7_RC_t snoopPacketProcess(snoopPDU_Msg_t *msg);
void    snoopPacketSend(L7_uint32 intIfNum,
                     L7_uint32 vlanId, L7_uint32 innerVlanId,
                     L7_uchar8 *payload,
                     L7_uint32 payloadLen, L7_uchar8 family);
/* MGMD Specific */
SYSNET_PDU_RC_t snoopIGMPPktIntercept(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t continueFunc);
SYSNET_PDU_RC_t snoopMLDPktIntercept(L7_uint32 hookId,
                                      L7_netBufHandle bufHandle,
                                      sysnet_pdu_info_t *pduInfo,
                                      L7_FUNCPTR_t continueFunc);
L7_RC_t snoopMgmdMembershipQueryProcess(mgmdSnoopControlPkt_t *mcastPacket);
L7_RC_t snoopMgmdSrcSpecificMembershipQueryProcess(mgmdSnoopControlPkt_t *mcastPacket); //PTIN: IGMPv3&MLDv2 Support
L7_RC_t snoopMgmdMembershipReportProcess(mgmdSnoopControlPkt_t *mcastPacket);
L7_RC_t snoopMgmdSrcSpecificMembershipReportProcess(mgmdSnoopControlPkt_t 
                                                    *mcastPacket);
L7_RC_t snoopPimv1v2DvmrpProcess(mgmdSnoopControlPkt_t *mcastPacket,
                              L7_inet_addr_t *grpAddr, L7_uint32 ipProtoType);
L7_RC_t snoopMgmdLeaveGroupProcess(mgmdSnoopControlPkt_t *mcastPacket);

L7_RC_t igmp_reuse_packet_and_send(mgmdSnoopControlPkt_t *mcastPacket);

/* End of function prototypes */
#endif /* SNOOPING_PROTO_H */

