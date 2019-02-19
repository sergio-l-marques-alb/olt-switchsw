/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  l7_packet.h
*
* @purpose   Common defines, enums and data structures for packets
* @component System Common
*
* @comments  none
*
* @create    03/15/2001
*
* @author    wjacobs
*
* @end
*
**********************************************************************/
#ifndef _INCLUDE_L7_PACKET_
#define _INCLUDE_L7_PACKET_

#include "datatypes.h"
#include "commdefs.h"
#include "system_exports.h"

#define L7_ENET_MAC_ADDR_LEN  L7_MAC_ADDR_LEN
#define L7_OUIVAL_LEN  3

/* Start of Link Layer Defines, Enums, and Data Structures */
/* Start of Link Layer Defines, Enums, and Data Structures */
/* Start of Link Layer Defines, Enums, and Data Structures */
/* Start of Link Layer Defines, Enums, and Data Structures */
/* Start of Link Layer Defines, Enums, and Data Structures */


/* Ethernet MAC Address */
typedef struct L7_mac_addr_s
{
  L7_uchar8  addr[L7_ENET_MAC_ADDR_LEN];
}L7_enetMacAddr_t;


/* Link Layer Address Description */
typedef struct L7_linkLayerAddr_s
{
  L7_uchar8  type;      /* defined in L7_LINK_LAYER_t */
  L7_uchar8  len;       /* length of address */
  union
  {
    L7_enetMacAddr_t  enetAddr;
  }addr;
}L7_linkLayerAddr_t;


/* Ethernet Header */
typedef struct L7_enetHeader_s
{
  L7_enetMacAddr_t    dest;
  L7_enetMacAddr_t    src;
}L7_enetHeader_t;

/* Another Ethernet header, but with an ethertype field. Note that the
 * ethertype field can either be an ethertype (Eth II) or a length (802.3). */
typedef struct L7_ethHeader_s
{
  L7_enetMacAddr_t  dest;
  L7_enetMacAddr_t  src;
  L7_ushort16  ethertype;   /* type or length */
} L7_ethHeader_t;

/* OUI Address */
typedef struct L7_OUIVAL_s
{
  L7_uchar8  addr[L7_OUIVAL_LEN];
}L7_OUIVal_t;


#define L7_ENET_HDR_SIZE    sizeof(L7_enetHeader_t)
#define L7_ETHERNET_CHECKSUM_LEN  4
#define L7_MIN_ENET_PKT_SIZE    64
#define L7_EFFECTIVE_MIN_ENET_PKT_SIZE    L7_MIN_ENET_PKT_SIZE - L7_ETHERNET_CHECKSUM_LEN
#define L7_ENET_HDR_TYPE_LEN_SIZE 2

/*------------------------------------*
     ENCAPSULATION TYPES
 *------------------------------------*/

/* Ethernet Encapsulation Overlay */
typedef struct L7_enet_encaps_s
{
  L7_ushort16       type;   /* ether type */
}L7_enet_encaps_t;


#define L7_ENET_ENCAPS_HDR_SIZE    sizeof(L7_enet_encaps_t)

/* 802 Encapsulation Overlay */
typedef struct L7_802_encaps_s
{
  L7_ushort16       length;
  L7_uchar8         dsap;
  L7_uchar8         ssap;
  L7_uchar8         cntl;
  L7_uchar8         org[3];
  L7_ushort16       type;  /* ether type */
}L7_802_encaps_t;

#define L7_802_ENCAPS_HDR_SIZE    sizeof(L7_802_encaps_t)

typedef enum
{
  L7_ENCAP_NONE = 0,
  L7_ENCAP_ETHERNET,
  L7_ENCAP_802

} L7_ENCAPSULATION_t;

/* Flags for link layer encapsulation */
#define L7_LL_ENCAP_ENET                    0x01
#define L7_LL_ENCAP_802                     0x02
#define L7_LL_ENCAP_8021Q_VLAN_TAGGED       0x04
#define L7_LL_BROADCAST_VLAN                0x08


/*------------------------------------*
     VLAN TAGGING STRUCTURES
 *------------------------------------*/

/* Ethernet Tagged */

typedef struct L7_enetTag_s
{
  L7_ushort16       tpid;                            /* Tag Protocol ID - 0x8100 if 802.1Q Tagged */
  L7_ushort16       tci;                             /* Tag Control Information */
}L7_8021QTag_t;

#define L7_8021Q_VLAN_PROTOCOL_ID                 0x8100

#define L7_8021Q_ENCAPS_HDR_SIZE    sizeof(L7_8021QTag_t)

/* Tag Control Information Masks */

#define L7_VLAN_TAG_PRIORITY_MASK           0xE000
#define L7_VLAN_TAG_CFI_MASK                0x1000
#define L7_VLAN_TAG_VLAN_ID_MASK            0x0FFF


typedef struct L7_enetTagERIF_s
{
  L7_enetHeader_t  macHdr;                           /* MAC Header */
  L7_ushort16      rc;                               /* Routing control */
}L7_enetTagERIF_t;


/*------------------------------------*
     STP BPDU TYPES
 *------------------------------------*/

#define L7_CONFIG_BPDU_TYPE                 0x00
#define L7_TCN_BPDU_TYPE                    0x80
#define L7_BPDU_TYPE_OFFSET                 20

#define L7_BPDU_MIN_CONFIG_SIZE             52       /*17 octets BPDU hdr + 35 octets payload*/
#define L7_BPDU_MIN_TCN_SIZE                21       /*17 octets BPDU hdr + 4 octets payload*/


/*------------------------------------*
     LINK LAYER TYPES
 *------------------------------------*/
typedef enum
{
  L7_LL_ETHERNET = 1,
  L7_LL_ATM
} L7_LINK_LAYER_t;


/* Start of Common Ethernet MAC Addresses */
/* Start of Common Ethernet MAC Addresses */
/* Start of Common Ethernet MAC Addresses */
/* Start of Common Ethernet MAC Addresses */
/* Start of Common Ethernet MAC Addresses */
/* Start of Common Ethernet MAC Addresses */

#ifdef L7_MAC_STP_BPDU                                /* Spanning Tree BPDU */
static L7_enetMacAddr_t L7_STP_BPDU_MAC =
{{0x01, 0x80, 0xC2, 0x00, 0x00, 0x00}};
#endif

#ifdef L7_MAC_ENET_BCAST
static L7_enetMacAddr_t L7_ENET_BCAST_MAC_ADDR =
{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
#endif

#ifdef L7_MAC_802_3X_PAUSE                            /* Flow control       */
static L7_enetMacAddr_t L7_MAC_802_3X_PAUSE_MAC =
{{0x01, 0x80, 0xC2, 0x00, 0x00, 0x01}};
#endif

#ifdef L7_MAC_EAPOL_PDU                              /* EAPOL PDU (802.1X)   */
static L7_enetMacAddr_t L7_EAPOL_PDU_MAC_ADDR =
{{0x01, 0x80, 0xC2, 0x00, 0x00, 0x03}};
#endif

#ifdef L7_MAC_LAC_PDU                                 /* LAC PDU            */
static L7_enetMacAddr_t L7_LAC_PDU_MAC =
{{0x01, 0x80, 0xC2, 0x00, 0x00,0x02}};
#endif

#ifdef L7_MAC_GMRP_PDU                                /* GMRP PDU           */
static L7_enetMacAddr_t L7_GMRP_PDU_MAC =
{{0x01, 0x80, 0xC2, 0x00, 0x00,0x20}};
#endif

#ifdef L7_MAC_GVRP_PDU                                /* GVRP PDU           */
static L7_enetMacAddr_t L7_GVRP_PDU_MAC =
{{0x01, 0x80, 0xC2, 0x00, 0x00, 0x21}};
#endif

#ifdef L7_MAC_IP_LTD_BCAST
static L7_enetMacAddr_t L7_IP_LTD_BCAST_MAC_ADDR =
{{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
#endif

#ifdef L7_MAC_IP_ALL_HOSTS                            /* IP ALL HOSTS       */
static L7_enetMacAddr_t L7_IP_ALL_HOSTS_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x01}};
#endif

#ifdef L7_MAC_IP_ALL_ROUTERS                          /* IP ALL ROUTERS     */
static L7_enetMacAddr_t L7_IP_ALL_RTRS_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x02}};
#endif

#ifdef L7_MAC_IP_DVMRP_RTRS                           /* IP ALL DVMRP RTRS  */
static L7_enetMacAddr_t L7_IP_DVMRP_RTRS_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x04}};
#endif

#ifdef L7_MAC_IP_OSPF_ALL_RTRS                        /* IP OSPF RTRS       */
static L7_enetMacAddr_t L7_IP_ALL_OSPF_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x05}};
#endif

#ifdef L7_MAC_IP_OSPF_DR_RTRS                         /* IP OSPF DESIG RTRS */
static L7_enetMacAddr_t L7_IP_OSPF_DESIG_RTRS_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x06}};
#endif

#ifdef L7_MAC_IP_RIPV2_RTRS                           /* IP RIPV2 RTRS      */
static L7_enetMacAddr_t L7_IP_RIPV2_RTRS_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x09}};
#endif

#ifdef L7_MAC_IP_ALL_PIM_RTRS                           /* IP ALL PIM RTRS      */
static L7_enetMacAddr_t L7_IP_ALL_PIM_RTRS_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x0d}};
#endif

#ifdef L7_MAC_IP_ALL_DVMRP_RTRS                           /* IP ALL DVMRP RTRS      */
static L7_enetMacAddr_t L7_IP_ALL_DVMRP_RTRS_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x04}};
#endif

#ifdef L7_MAC_IP_DVMRP_ON_MOSPF                       /* IP DVMRP ON MOSPF  */
static L7_enetMacAddr_t L7_IP_DVMRP_MOSPF_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x01, 0x15}};
#endif

#ifdef L7_MAC_ENET_VRRP                              /* vrrp mac addr */
static L7_enetMacAddr_t L7_ENET_VRRP_MAC_ADDR =
{{0x00, 0x00, 0x5E, 0x00, 0x01, 0x00}};
#endif

#ifdef L7_MAC_IP_VRRP_ADVT                           /* VRRP advertisement dest MAC */
static L7_enetMacAddr_t L7_IP_VRRP_ADVT_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x12}};
#endif

#ifdef L7_MAC_IP_IGMPV3_REPORT                       /* IGMPv3 Membership Report dest MAC */
static L7_enetMacAddr_t L7_IP_IGMPV3_REPORT_MAC_ADDR =
{{0x01, 0x00, 0x5E, 0x00, 0x00, 0x16}};
#endif

#ifdef L7_MAC_IP6_ALL_NODES                          /* IPv6 All Nodes dest MAC */
static L7_enetMacAddr_t L7_IP6_ALL_NODES_MAC_ADDR =
{{0x33, 0x33, 0x00, 0x00, 0x00, 0x01}};
#endif

#ifdef L7_MAC_IP6_ALL_ROUTERS                        /* IPv6 All Routers dest MAC */
static L7_enetMacAddr_t L7_IP6_ALL_ROUTERS_MAC_ADDR =
{{0x33, 0x33, 0x00, 0x00, 0x00, 0x2}};
#endif

#ifdef L7_MAC_IP6_SOL_NODE                           /* IPv6 Solicited Node dest MAC */
static L7_enetMacAddr_t L7_IP6_SOL_NODE_MAC_ADDR =
{{0x33, 0x33, 0xFF, 0x17, 0xFC, 0x0F}};
#endif

#ifdef L7_MAC_LLDP_PDU
static L7_enetMacAddr_t L7_ENET_LLDP_MULTICAST_MAC_ADDR =
{{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}};
#endif

#ifdef L7_MAC_ISDP_PDU
static L7_enetMacAddr_t L7_ENET_ISDP_MULTICAST_MAC_ADDR =
{{0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc}};
#endif /* L7_MAC_ISDP_PDU */

/* Start of ETHER TYPES */
/* Start of ETHER TYPES */
/* Start of ETHER TYPES */
/* Start of ETHER TYPES */
/* Start of ETHER TYPES */
/* Start of ETHER TYPES */

#define L7_ETYPE_IP       0x0800
#define L7_ETYPE_CDP      0x2000
#define L7_ETYPE_IPV6     0x86DD
#define L7_ETYPE_ARP      0x0806
#define L7_ETYPE_RARP     0x8035
#define L7_ETYPE_8021Q    0x8100
#define L7_ETYPE_IPX      0x8137
#define L7_ETYPE_EAPOL    0x888E
#define L7_ETYPE_VBAS     0x8200
#define L7_ETYPE_LLDP     0x88CC
#define L7_ETYPE_ISDP     0x2000
#define L7_ETYPE_EFMOAM   0x8809
#define L7_ETYPE_EFMOAM_DATA 0x9000
#define L7_ETYPE_CFM      0x8902

/* Start of IP Header   */
/* Start of IP Header   */
/* Start of IP Header   */
/* Start of IP Header   */
/* Start of IP Header   */
/* Start of IP Header   */


typedef struct L7_ipHeader_s
{
  L7_uchar8   iph_versLen;      /* version/hdr len */
  L7_uchar8   iph_tos;          /* type of service */
  L7_ushort16 iph_len;          /* pdu len */
  L7_ushort16 iph_ident;        /* identifier */
  L7_ushort16 iph_flags_frag;   /* flags/ fragment offset */
  L7_uchar8   iph_ttl;          /* lifetime */
  L7_uchar8   iph_prot;         /* protocol ID */
  L7_ushort16 iph_csum;         /* checksum */
  L7_uint32   iph_src;          /* src add */
  L7_uint32   iph_dst;          /* dst add */
}L7_ipHeader_t;

#define L7_IP_VERS4     0x40
#define L7_IP_HDR_LEN     20
#define L7_IP_HDR_MAX_LEN 60
#define L7_IP_ADDR_LEN    4

/* Changes by TCS 13/12 */
#define L7_IP_HDR_VER_LEN   5
#define L7_IP_VERSION       4


/* Start of IP Priority Classifcation Defines */
/* Start of IP Priority Classifcation Defines */
/* Start of IP Priority Classifcation Defines */
/* Start of IP Priority Classifcation Defines */
/* Start of IP Priority Classifcation Defines */
/* Start of IP Priority Classifcation Defines */

#if 0

/* type of service */
#define IPTOS_PMASK       0xE0      /* precedence mask */
#define IPTOS_TMASK       0x1E      /* tos mask */
#define IPTOS_DELAY       0x10      /* delay */
#define IPTOS_TPUT        0x08      /* throughput */
#define IPTOS_REL         0x04      /* reliability */
#define IPTOS_MON         0x02      /* monetary cost */

#endif



/* Start of Common IP Multicast Addresses     */
/* Start of Common IP Multicast Addresses     */
/* Start of Common IP Multicast Addresses     */
/* Start of Common IP Multicast Addresses     */
/* Start of Common IP Multicast Addresses     */
/* Start of Common IP Multicast Addresses     */

#define L7_IP_MCAST_BASE_ADDR             0xE0000000ul
#define L7_IP_MAX_LOCAL_MULTICAST         0xE00000FFul /* Highest local multicast addr */
#define L7_IP_MCAST_TO_MAC_MASK           0x007FFFFFul /* lower 23-bits of IP map to L2 address */

#define L7_IP_LOOPBACK_ADDR_NETWORK       0x7F000000ul /* Loopback group Network */
#define L7_IP_LOOPBACK_ADDR_MASK          0xFF000000ul /* Loopback group mask */

#define L7_CLASS_B_ADDR_NETWORK           0x80000000ul /* Class B */
#define L7_CLASS_C_ADDR_NETWORK           0xC0000000ul /* Class C */
#define L7_CLASS_D_ADDR_NETWORK           0xE0000000ul /* Class D (Multicast) */

#define L7_CLASS_E_ADDR_NETWORK           0xF0000000ul /* Class E Network     */
#define L7_CLASS_E_ADDR_MASK              0xF0000000ul /* Class E Network Mask*/
#define L7_IP_SUBMASK_EXACT_MATCH_MASK    0xFFFFFFFFul


#define L7_IP_LTD_BCAST_ADDR              0xFFFFFFFFul /* 255.255.255.255 */
#define L7_IP_ALL_HOSTS_ADDR              0xE0000001ul /* 224.0.0.1 */
#define L7_IP_ALL_RTRS_ADDR               0xE0000002ul /* 224.0.0.2 */
#define L7_IP_ALL_DVMRP_RTRS_ADDR         0xE0000004ul /* 224.0.0.4 */
#define L7_IP_ALL_OSPF_RTRS_ADDR          0xE0000005ul /* 224.0.0.5 */
#define L7_IP_ALL_OSPF_DESIG_RTRS_ADDR    0xE0000006ul /* 224.0.0.6 */
#define L7_IP_RIPV2_RTRS_ADDR             0xE0000009ul /* 224.0.0.9 */
#define L7_IP_ALL_PIM_RTRS_ADDR           0xE000000Dul /* 224.0.0.13 */
#define L7_IP_DVMRP_ON_MOSPF_ADDR         0xE0000015ul /* 224.0.0.15 */
#define L7_IP_VRRP_ADDR                   0xE0000012ul /* 224.0.0.18 */
#define L7_IP_IGMPV3_REPORT_ADDR          0xE0000016ul /* 224.0.0.22 */
#define L7_IP_SSM_BASE_ADDR               0xE8000000ul /* 232.0.0.0 */

/*------------------------------------*
   COMMON DATA TYPES
*------------------------------------*/
#define IP_MAX_CLIENT         8
#define ICMP_PROT             1
#define IP_PROT_ICMP          1
#define IP_PROT_ICMPV6        58
#define MAX_ICMP_LEN          256
#define IP_PING_TTL           10  /* seconds */
#define IP_MAX_SIMUL_PING     16
#define IGMP_PROT             2
#define IP_PROT_IGMP          2
#define IP_PROT_IP            4   /* ip in ip encap */
#define IP_PROT_TCP           6
#define IP_PROT_UDP           17
#define IP_PROT_ESP           50  /* encryption */
#define IP_PROT_AUTH          51  /* authentication */
#define IP_PROT_IPV6          41  /* ipv6 in ipv4 encap */
#define IP_PROT_RSVP          46
#define IP_PROT_OSPFIGP       89  /* OSPF */
#define IP_PROT_PIM           103

#define IP_PROT_VRRP        112  /* vrrp protocol type */
#define IP_VRRP_TTL           255 /* seconds */


#define IP_BOOTP_DHCP_RELAY_TTL 64
#define L7_DHCP_PACKET_LEN    548

/* TCP/UDP port numbers */
#define UDP_PORT_TIME           37
#define UDP_PORT_NAMESERVER     42
#define UDP_PORT_TACACS         49
#define UDP_PORT_DNS            53
#define UDP_PORT_BOOTP_SERV     67
#define UDP_PORT_BOOTP_CLNT     68
#define UDP_PORT_DHCP_SERV      67
#define UDP_PORT_DHCP_CLNT      68
#define UDP_PORT_TFTP           69
#define UDP_PORT_HTTP           80
#define UDP_PORT_NTP           123
#define UDP_PORT_NETBIOS_NS    137
#define UDP_PORT_NETBIOS_DGM   138
#define UDP_PORT_MOBILE_IP     434
#define UDP_PORT_PIM_AUTO_RP   496
#define UDP_PORT_ISAKMP        500
#define UDP_PORT_RIP           520

#define TCP_PORT_DNS            53
#define TCP_PORT_HTTP           80
#define TCP_PORT_BGP           179
#define TCP_PORT_HTTPS         443


/* TCP/UDP port range */
#define L7_TCP_UDP_MIN_PORT     1
#define L7_TCP_UDP_MAX_PORT     65535
#define L7_TCP_UDP_DEFAULT_PORT 514

#define L7_INADDR_ANY           0x00000000
#define L7_INADDR_BROADCAST     0xffffffff

/* TBD: Check */
typedef struct L7_udp_header_s
{
  L7_ushort16  sourcePort;     /* source port */
  L7_ushort16  destPort;       /* dest port */
  L7_ushort16  length;         /* length    */
  L7_ushort16  checksum;       /* checksum  */
}L7_udp_header_t;

#define L7_UDP_HDR_LEN    8

typedef struct
{
  L7_ushort16  srcPort;                 /* source port */    
  L7_ushort16  destPort;                /* dest port */       
  L7_uint32    seqNumber;               /* sequence number */      
  L7_uint32    ackNumber;               /* acknowledgement number */       
  L7_ushort16  dataOffsetCtrlFlags;     /* offset(4 bits) and control flags(6 bits) */
  L7_ushort16  window;                  /* window size */
  L7_ushort16  checksum;                /* checksum  */
  L7_ushort16  urgentPtr;               /* urgent pointer */
  L7_uint32    options;                 /* TCP options */
} L7_tcpHeader_t;

#define TCP_HEADER_LENGTH(__tcpHdrPtr__) \
  (((osapiNtohs(__tcpHdrPtr__->dataOffsetCtrlFlags) & 0xf000) >> 12)*4)

/*ARP Header*/
typedef struct  L7_arphdr_s
{
  L7_ushort16 ar_hrd;       /* format of hardware address */
#define L7_ARPHRD_ETHER   1 /* ethernet hardware format */
#define L7_ARPHRD_FRELAY  15  /* frame relay hardware format */
  L7_ushort16 ar_pro;       /* format of protocol address */
  L7_uchar8     ar_hln;       /* length of hardware address */
  L7_uchar8     ar_pln;       /* length of protocol address */
  L7_ushort16 ar_op;        /* one of: */
#define L7_ARPOP_REQUEST  1 /* request to resolve address */
#define L7_ARPOP_REPLY  2     /* response to previous request */
#define L7_ARPOP_REVREQUEST 3 /* request protocol address given hardware */
#define L7_ARPOP_REVREPLY 4 /* response giving protocol address */
#define L7_ARPOP_INVREQUEST 8   /* request to identify peer */
#define L7_ARPOP_INVREPLY 9 /* response identifying peer */
/*
 * The remaining fields are variable in size,
 * according to the sizes above.
 */
#ifdef COMMENT_ONLY
  L7_uchar8 ar_sha[]; /* sender hardware address */
  L7_uchar8 ar_spa[]; /* sender protocol address */
  L7_uchar8 ar_tha[]; /* target hardware address */
  L7_uchar8 ar_tpa[]; /* target protocol address */
#endif
}L7_arphdr_t;

typedef struct  L7_ether_arp_s
{
  L7_arphdr_t   ea_hdr;     /* fixed-size header */
  L7_uchar8     arp_sha[6]; /* sender hardware address */
  L7_uchar8     arp_spa[4]; /* sender protocol address */
  L7_uchar8     arp_tha[6]; /* target hardware address */
  L7_uchar8     arp_tpa[4]; /* target protocol address */
}L7_ether_arp_t;
#define arp_hrd ea_hdr.ar_hrd
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op  ea_hdr.ar_op

 /*end of ARP Header*/

/* IGMP message */
typedef struct L7_igmpMsg_s
{
  L7_uchar8    igmpType;         /* message type */
  L7_uchar8    igmpMaxRespTime;  /* maximum query response time */
  L7_ushort16  igmpChecksum;     /* checksum */
  L7_uint32    igmpGroupAddr;    /* group address */
}L7_igmpMsg_t;

/* IGMPv3 Membership Query */
typedef struct L7_igmpv3Query_s
{
  L7_igmpMsg_t igmpMsg;          /* contains L7_igmpMsg_t above */
  L7_uchar8    igmpResvSflagQRV; /* 4 bits reserved,  1 bit S Flag, 3 bits Querier's Robustness Variable */
  L7_uchar8    igmpQQIC;         /* Querier's Query Interval */
  L7_ushort16  igmpQueryNumSrcs; /* Number of source addresses in this frame */
  L7_uint32    igmpQuerySrcs[1]; /* igmpQueryNumSrcs source addresses */
} L7_igmpv3Query_t;

/* IGMPv3 Group Record */
typedef struct L7_igmpv3GroupRecord_s
{
  L7_uchar8    igmpGrpRecType;       /* Group Record type (see below) */
  L7_uchar8    igmpGrpRecAuxDataLen; /* Aux Data Len */
  L7_ushort16  igmpGrpRecNumSrcs;    /* Number of source addresses in this group record */
  L7_uint32    igmpGrpRecGroupAddr;  /* Group address */
  L7_uint32    igmpGrpRecSrcs[1];    /* igmpGrpRecNumSrcs source addresses */
  /* Followed potentially by auxiliary data */
} L7_igmpv3GroupRecord_t;

/* IGMPv3 Membership Report */
typedef struct L7_igmpv3Report_s
{
  L7_uchar8              igmpReportType;          /* Message type (v3 report = 0x22) */
  L7_uchar8              igmpReportReserved1;     /* Reserved */
  L7_ushort16            igmpReportChecksum;      /* checksum */
  L7_ushort16            igmpReportReserved2;     /* Reserved */
  L7_ushort16            igmpReportNumGroupRecs;  /* Number of group records in this frame */
  L7_igmpv3GroupRecord_t igmpReportGroupRecs[1];  /* igmpReportNumGroupRecs group records */
} L7_igmpv3Report_t;

/* IGMP message types */
#define L7_IGMP_MEMBERSHIP_QUERY           0x11
#define L7_IGMP_V1_MEMBERSHIP_REPORT       0x12
#define L7_IGMP_DVMRP                      0x13
#define L7_IGMP_PIM_V1                     0x14
#define L7_IGMP_V2_MEMBERSHIP_REPORT       0x16
#define L7_IGMP_V2_LEAVE_GROUP             0x17
#define L7_IGMP_V3_MEMBERSHIP_REPORT       0x22

/* IGMPv3 group record types */
typedef enum
{
  L7_IGMP_MODE_IS_INCLUDE = 1,
  L7_IGMP_MODE_IS_EXCLUDE,
  L7_IGMP_CHANGE_TO_INCLUDE_MODE,
  L7_IGMP_CHANGE_TO_EXCLUDE_MODE,
  L7_IGMP_ALLOW_NEW_SOURCES,
  L7_IGMP_BLOCK_OLD_SOURCES,
  MGMD_GROUP_REPORT_TYPE_MAX
} MGMD_GROUP_REPORT_TYPE_t;

#define L7_IGMP_PROXY_GRP_TIMER_EXPIRY     7
#define L7_IGMP_PROXY_SRC_TIMER_EXPIRY     8

/* MLD message types */
#define L7_MLD_MEMBERSHIP_QUERY           130
#define L7_MLD_V1_MEMBERSHIP_REPORT       131
#define L7_MLD_V1_MEMBERSHIP_DONE         132
#define L7_MLD_V2_MEMBERSHIP_REPORT       143

/* MLDv2 group record types */
#define L7_MLD_MODE_IS_INCLUDE            1
#define L7_MLD_MODE_IS_EXCLUDE            2
#define L7_MLD_CHANGE_TO_INCLUDE_MODE     3
#define L7_MLD_CHANGE_TO_EXCLUDE_MODE     4
#define L7_MLD_ALLOW_NEW_SOURCES          5
#define L7_MLD_BLOCK_OLD_SOURCES          6

/* DHCP packet format */
#define L7_DHCP_HARDWARE_ADDR_MAXLEN  16
#define L7_DHCP_SNAME_LEN  64
#define L7_DHCP_FILE_LEN   128
#define L7_DHCP_HDR_LEN    236

#define L7_DHCP_BOOTP_REQUEST     1
#define L7_DHCP_BOOTP_REPLY       2

/* DHCP packet types. Values match those in packet. BOOTP packet considered
 * to have UNKNOWN DHCP packet type. */
typedef enum 
{
  L7_DHCP_UNKNOWN = 0,
  L7_DHCP_DISCOVER,
  L7_DHCP_OFFER,
  L7_DHCP_REQUEST,
  L7_DHCP_DECLINE,
  L7_DHCP_ACK,
  L7_DHCP_NACK,
  L7_DHCP_RELEASE,
  L7_DHCP_INFORM

} L7_dhcp_pkt_type_t;

#define L7_DHCP_HTYPE_ETHER 1

#define L7_DHCP_MAGIC_COOKIE_LEN  4

typedef struct L7_dhcp_packet_s
{
  L7_uchar8      op;      /* Message opcode/type */
  L7_uchar8      htype;   /* Hardware addr type, ethernet - 1 */
  L7_uchar8      hlen;    /* Hardware addr length ethernet - 6 */
  L7_uchar8      hops;    /* Number of relay agent hops from client */
  L7_uint32      xid;     /* Transaction ID - 4 Bytes*/
  L7_ushort16    secs;    /* Sec */
  L7_ushort16    flags;   /* Flag bits */
  L7_uint32      ciaddr;  /* Client IP address */
  L7_uint32      yiaddr;  /* Your IP address */
  L7_uint32      siaddr;  /* Server IP address */
  L7_uint32      giaddr;  /* Gateway IP address */
  L7_uchar8      chaddr[L7_DHCP_HARDWARE_ADDR_MAXLEN]; /* Client hwaddr */
  L7_uchar8      sname[L7_DHCP_SNAME_LEN]; /*  Server name */
  L7_uchar8      file[L7_DHCP_FILE_LEN];   /*  Boot filename */
}L7_dhcp_packet_t;

/* DHCP Option types */
typedef enum
{
  L7_DHCP_OPT_PAD = 0,
  L7_DHCP_OPT_SUBNET_MASK = 1,
  L7_DHCP_OPT_TIME_OFFSET = 2,
  L7_DHCP_OPT_ROUTER = 3,
  L7_DHCP_OPT_TIME_SERVER = 4,
  L7_DHCP_OPT_NAME_SERVER = 5,
  L7_DHCP_OPT_DNS_SERVER = 6,
  L7_DHCP_OPT_LOG_SERVER = 7,
  L7_DHCP_OPT_COOKIE_SERVER = 8,
  L7_DHCP_OPT_LPR_SERVER = 9,
  L7_DHCP_OPT_IMPRESS_SERVER = 10,
  L7_DHCP_OPT_RESLOC_SERVER = 11,
  L7_DHCP_OPT_HOST_NAME = 12,
  L7_DHCP_OPT_BOOT_FILE_SIZE = 13,
  L7_DHCP_OPT_MERIT_DUMP_FILE = 14,
  L7_DHCP_OPT_DOMAIN_NAME = 15,
  L7_DHCP_OPT_SWAP_SERVER = 16,
  L7_DHCP_OPT_ROOT_PATH = 17,
  L7_DHCP_OPT_EXTENS_PATH = 18,
  L7_DHCP_OPT_IP_FORWARDING = 19,
  L7_DHCP_OPT_NL_SRC_ROUTING = 20,
  L7_DHCP_OPT_POLICY_FILTER = 21,
  L7_DHCP_OPT_MAX_DGRAM_REASSEM_SIZE = 22,
  L7_DHCP_OPT_DEFAULT_IP_TTL = 23,
  L7_DHCP_OPT_PATH_MTU_AGING_TIMEOUT = 24,
  L7_DHCP_OPT_PATH_MTU_PLATEAU_TABLE = 25,
  L7_DHCP_OPT_INTERFACE_MTU = 26,
  L7_DHCP_OPT_ALL_SUBNETS_LOCAL = 27,
  L7_DHCP_OPT_BROADCAST_ADDR = 28,
  L7_DHCP_OPT_PERF_MASK_DISCOVERY = 29,
  L7_DHCP_OPT_MASK_SUPPLIER = 30,
  L7_DHCP_OPT_PERF_ROUTER_DISCOVERY = 31,
  L7_DHCP_OPT_ROUTER_SOLICIT_ADDR = 32,
  L7_DHCP_OPT_STATIC_ROUTE = 33,
  L7_DHCP_OPT_TRAILER_ENCAP = 34,
  L7_DHCP_OPT_ARP_CACHE_TIMEOUT = 35,
  L7_DHCP_OPT_ETHERNET_ENCAP = 36,
  L7_DHCP_OPT_TCP_DEFAULT_TTL = 37,
  L7_DHCP_OPT_TCP_KEEPALIVE_INTERVAL = 38,
  L7_DHCP_OPT_TCP_KEEPALIVE_GARBAGE = 39,
  L7_DHCP_OPT_NETINFO_SERVICE_DOMAIN = 40,
  L7_DHCP_OPT_NETINFO_SERVERS = 41,
  L7_DHCP_OPT_NTP_SERVERS = 42,
  L7_DHCP_OPT_VENDOR_SPECINFO = 43,
  L7_DHCP_OPT_NETBIOS_NAME_SERVER = 44,
  L7_DHCP_OPT_NETBIOS_DGRAM_DIST_SERVER = 45,
  L7_DHCP_OPT_NETBIOS_NODE_TYPE = 46,
  L7_DHCP_OPT_NETBIOS_SCOPE = 47,
  L7_DHCP_OPT_XWIN_FONT_SERVER = 48,
  L7_DHCP_OPT_XWINDOW_DISPMGR = 49,
  L7_DHCP_OPT_REQ_IPADDR = 50,
  L7_DHCP_OPT_IPADDR_LEASE_TIME = 51,
  L7_DHCP_OPT_OPTION_OVERLOAD = 52,
  L7_DHCP_OPT_DHCP_MSG_TYPE = 53,
  L7_DHCP_OPT_SERVER_ID = 54,
  L7_DHCP_OPT_PARAMETER_REQLIST = 55,
  L7_DHCP_OPT_MSG = 56,
  L7_DHCP_OPT_MAX_DHCP_MSG_SIZE = 57,
  L7_DHCP_OPT_RENEWAL_TIMEVAL = 58,
  L7_DHCP_OPT_REBINDING_TIMEVAL = 59,
  L7_DHCP_OPT_VENDOR_CLASS_ID = 60,
  L7_DHCP_OPT_CLIENT_ID = 61,
  L7_DHCP_OPT_UNASSIGNED1 = 62,
  L7_DHCP_OPT_UNASSIGNED2 = 63,
  L7_DHCP_OPT_NETINFO_SERVPLUS_DOMAIN = 64,
  L7_DHCP_OPT_NETINFO_SERVPLUS_SERVERS = 65,
  L7_DHCP_OPT_TFTP_SERVER_NAME = 66,
  L7_DHCP_OPT_BOOTFILE_NAME = 67,
  L7_DHCP_OPT_MOBILE_IP_HOME_AGENT = 68,
  L7_DHCP_OPT_SMTP_SERVER = 69,
  L7_DHCP_OPT_POP3_SERVER = 70,
  L7_DHCP_OPT_NNTP_SERVER = 71,
  L7_DHCP_OPT_DEFAULT_WWW_SERVER = 72,
  L7_DHCP_OPT_DEFAULT_FINGER_SERVER = 73,
  L7_DHCP_OPT_DEFAULT_IRC_SERVER = 74,
  L7_DHCP_OPT_STREETTALK_SERVER = 75,
  L7_DHCP_OPT_STREETTALK_STDA_SERVER = 76,
  L7_DHCP_OPT_IP_PHONE = 176,
  L7_DHCP_OPT_END = 255
}L7_dhcp_opttype_t;

typedef struct L7_dhcp_option_s
{
  L7_uchar8     type;
  L7_uchar8     length;
  L7_uchar8     value[1];
}L7_dhcp_option_t;

/*DHCP Server packet stucture add- used for voice vlan*/
/*
** DHCP server well known port numbers and broadcast address
*/
#define L7_DHCPS_SERVER_PORT        67
#define L7_DHCPS_CLIENT_PORT        68

#define L7_DHCPS_MESSAGE_TYPE_START     L7_DHCP_DISCOVER      
#define L7_DHCPS_MESSAGE_TYPE_END       L7_DHCP_INFORM
 
typedef struct L7_dhcpsPacket_s
{
  L7_dhcp_packet_t  header;
  L7_uchar8         magicCookie[L7_DHCP_MAGIC_COOKIE_LEN];
  L7_uchar8         options[1];
}L7_dhcpsPacket_t;

/*****************************************************
 *
 *   IPV6 section
 *
 *****************************************************/

/** IPV6 packet header **/
typedef struct L7_ip6Header_s
{
  L7_uint32    ver_class_flow;
  L7_ushort16  paylen;
  L7_uchar8    next;
  L7_uchar8    hoplim;
  L7_uchar8    src[16];
  L7_uchar8    dst[16];
} L7_ip6Header_t;
#define L7_IP6_HEADER_LEN 40



/* IPV6 extensions header */
typedef struct L7_ip6ExtHeader_s
{
    L7_uchar8  xnext;
    L7_uchar8  xlen;      /* not including hdr */
}L7_ip6ExtHeader_t;

#define L7_ICMPV6_HEADER_LEN 8
typedef struct L7_ip6IcmpHeader_s
{
    L7_uchar8   icmp6_type;   /* type field */
    L7_uchar8   icmp6_code;   /* code field */
    L7_ushort16 icmp6_cksum;  /* checksum field */
    L7_uint32   icmp6_data;
}L7_ip6IcmpHeader_t;

/* type field */
#define L7_ICMPV6_DEST_UNREACH        1
#define L7_ICMPV6_TIME_EXCEED         3
#define L7_ICMPV6_ECHO_REQUEST        128
#define L7_ICMPV6_ECHO_REPLY          129
#define L7_ICMPV6_ROUTER_ADVERT       134
#define L7_ICMPV6_ROUTER_SOLICIT      133

/* code field */
/* Destination unreachable codes */
#define L7_ICMPV6_NOROUTE                0
#define L7_ICMPV6_ADMIN_PROHIBITED       1
#define L7_ICMPV6_ADDR_UNREACH           3
#define L7_ICMPV6_PORT_UNREACH           4

/* Time Exceeded codes */
#define L7_ICMPV6_EXC_HOPLIMIT      0

#define L7_IP6RA_HEADER_LEN 16
typedef struct L7_ip6RouterAdvHeader_s
{
    L7_uchar8   ra6_type;   /* type field */
    L7_uchar8   ra6_code;   /* code field */
    L7_ushort16 ra6_cksum;  /* checksum field */
    L7_uchar8   ra6_choplim;
    L7_uchar8   ra6_flags;
    L7_ushort16 ra6_rtr_lifetime;
    L7_uint32   ra6_reachable;
    L7_uint32   ra6_retransmit;
}L7_ip6RouterAdvHeader_t;

#define L7_IP6RA_FLAG_OTHER       0x40
#define L7_IP6RA_FLAG_MANAGED     0x80

/* src/target lla option: ether specific */
#define L7_IP6RA_LLA_OPT_LEN   8
#define L7_IP6RA_LLA_OPT_SRC_TYPE 1
#define L7_IP6RA_LLA_OPT_TARGET_TYPE 2
typedef struct L7_ip6RadvdLLA_s
{
    L7_uchar8   lla_type;
    L7_uchar8   lla_len;
    L7_uchar8   lla_data[6];
}L7_ip6RadvdLLA_t;

/* radv prefix info option */
#define L7_IP6RA_PREFIX_OPT_LEN 32
#define L7_IP6RA_PREFIX_OPT_TYPE 3
typedef struct L7_ip6RadvPrefix_s
{
    L7_uchar8   rap_type;
    L7_uchar8   rap_len;
    L7_uchar8   rap_prefix_len;
    L7_uchar8   rap_flags;
    L7_uint32   rap_valid_time;
    L7_uint32   rap_preferred_time;
    L7_uint32   rap_reserved;
    L7_uchar8   rap_prefix[16];
}L7_ip6RadvPrefix_t;

#define L7_IP6RA_PREFIX_FLAG_AUTO    0x40
#define L7_IP6RA_PREFIX_FLAG_ONLINK  0x80

/* nd mtu option */
#define L7_IP6ND_MTU_OPT_LEN 8
#define L7_IP6ND_MTU_OPT_TYPE 5
typedef struct L7_ip6NdMtu_s
{
    L7_uchar8   ndmtu_type;
    L7_uchar8   ndmtu_len;
    L7_ushort16 ndmtu_reserved;
    L7_uint32   ndmtu_mtu;
}L7_ip6NdMtu_t;

#endif
