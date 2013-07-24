/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_proto.h
*
* @purpose   SFlow Protocol Datagram format definitions
*
* @component sflow
*
* @comments  This section is taken from sFlow v5 specification seciton
*            5. sFlow Datagram Format. All the listed formats may not be 
*            supported
*
* @create 19-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SFLOW_PROTO_H
#define SFLOW_PROTO_H

#include "l3_addrdefs.h"
#include "datatypes.h"
#include "sflow_exports.h"

#include "sflow_defs.h"
/* sFlow Address Type */
typedef enum  
{
  L7_SFLOW_ADDR_TYPE_UNKNW = 0, /* Unknown */
  L7_SFLOW_ADDR_TYPE_IP_V4 = 1, /* IPv4    */
  L7_SFLOW_ADDR_TYPE_IP_V6 = 2  /* IPv6    */
} L7_SFLOW_ADDR_TYPE_t;

/* Opaque representaion for network address */
typedef struct  SFLOW_addr_s 
{
  L7_SFLOW_ADDR_TYPE_t type;          /*  sFlow Address type */
  L7_inet_addr_t       sflAddr;        /*  Network Address */
} SFLOW_addr_t;


/* sFlow Packet header data */
/* Flow Datatypes */
/* The header protocol describes the format of the sampled header */
typedef enum  
{
  SFLOW_HDR_ETHERNET_ISO8023     = 1,
  SFLOW_HDR_ISO88024_TOKENBUS    = 2,
  SFLOW_HDR_ISO88025_TOKENRING   = 3,
  SFLOW_HDR_FDDI                 = 4,
  SFLOW_HDR_FRAME_RELAY          = 5,
  SFLOW_HDR_X25                  = 6,
  SFLOW_HDR_PPP                  = 7,
  SFLOW_HDR_SMDS                 = 8,
  SFLOW_HDR_AAL5                 = 9,
  SFLOW_HDR_AAL5_IP              = 10, /* e.g. Cisco AAL5 mux */
  SFLOW_HDR_IPv4                 = 11,
  SFLOW_HDR_IPv6                 = 12,
  SFLOW_HDR_MPLS                 = 13,
  SFLOW_HDR_POS                 = 14  /* RFC 1662, 2615 */
} SFLOW_HDR_PROTOCOL_t;

/* Raw packet header */
typedef struct SFLOW_sampled_header_s 
{
  SFLOW_HDR_PROTOCOL_t  header_protocol; /* SFLOW_HDR_PROTOCOL_t */
  L7_uint32             frame_length;    /* Original length of packet before sampling */
  L7_uint32             stripped;        /* header/trailer bytes stripped by sender */
  L7_uint32             header_length;   /* length of sampled header bytes to follow */
  L7_uchar8             header_bytes[SFLOW_MAX_HEADER_SIZE];  /* Header bytes */
} SFLOW_sampled_header_t;
#define SFLOW_FLOW_RECORD_SAMPLED_HEADER_OVERHEAD 16

/* Parsed ethernet header */
typedef struct SFLOW_sampled_ethernet_s 
{
  L7_uint32         length;     /* Length of the ethernet pkt */
  L7_enetMacAddr_t  src_mac;    /* Source MAC Address */
  L7_enetMacAddr_t  dst_mac;    /* Destination MAC Address */ 
  L7_uint32         eth_type;   /* Ethernet packet type */
} SFLOW_sampled_ethernet_t;

/* Parsed IPv4 header */
typedef struct SFLOW_sampled_ipv4_s
{
  L7_uint32    length;      /* The length of the IP packet
                               excluding lower layer encapsulations */
  L7_uint32    protocol;    /* IP Protocol type (for example, TCP = 6, UDP = 17) */
  L7_in_addr_t src_ip;      /* Source IP Address */
  L7_in_addr_t dst_ip;      /* Destination IP Address */
  L7_uint32    src_port;    /* TCP/UDP source port number or equivalent */
  L7_uint32    dst_port;    /* TCP/UDP destination port number or equivalent */
  L7_uint32    tcp_flags;   /* TCP flags */
  L7_uint32    tos;         /* IP type of service */
} SFLOW_sampled_ipv4_t;

/* Parsed IPv6 header */
typedef struct SFLOW_sampled_ipv6_s 
{
  L7_uint32 length;          /* The length of the IP packet
                                excluding lower layer encapsulations */
  L7_uint32     protocol;     /* IP next header (for example, TCP = 6, UDP = 17) */
  L7_in6_addr_t src_ip;       /* Source IP Address */
  L7_in6_addr_t dst_ip;       /* Destination IP Address */
  L7_uint32     src_port;     /* TCP/UDP source port number or equivalent */
  L7_uint32     dst_port;     /* TCP/UDP destination port number or equivalent */
  L7_uint32     tcp_flags;    /* TCP flags */
  L7_uint32     priority;     /* IP priority */
} SFLOW_sampled_ipv6_t;

/* Extended Flow Data */
/* Extended switch data */
typedef struct SFLOW_extended_switch_s 
{
  L7_uint32 src_vlan;       /* The 802.1Q VLAN id of incomming frame */
  L7_uint32 src_priority;   /* The 802.1p priority */
  L7_uint32 dst_vlan;       /* The 802.1Q VLAN id of outgoing frame */
  L7_uint32 dst_priority;   /* The 802.1p priority */
} SFLOW_extended_switch_t;

/* Extended router data */
typedef struct SFLOW_extended_router_s 
{
  L7_inet_addr_t nexthop;          /* IP address of next hop router */
  L7_uint32      src_mask;         /* Source address prefix mask bits */
  L7_uint32      dst_mask;         /* Destination address prefix mask bits */
} SFLOW_extended_router_t;

/* Extended gateway data */
typedef enum
{
  SFLOW_EXTENDED_AS_SET = 1,      /* Unordered set of ASs */
  SFLOW_EXTENDED_AS_SEQUENCE = 2  /* Ordered sequence of ASs */
} SFLOW_EXTENDED_AS_PATH_SEG_TYPE_t;

typedef struct SFLOW_extended_as_path_segment_s
{
  SFLOW_EXTENDED_AS_PATH_SEG_TYPE_t type; 
  L7_uint32 length; /* number of AS numbers in set/sequence */
  union 
  {
    L7_uint32 *set;
    L7_uint32 *seq;
  } as;
} SFLOW_extended_as_path_segment_t;

typedef struct SFLOW_extended_gateway_s 
{
  L7_inet_addr_t nexthop;             /* Address of the border router that should
                                         be used for the destination network */
  L7_uint32      as;                  /* AS number for this gateway */
  L7_uint32      src_as;              /* AS number of source (origin) */
  L7_uint32      src_peer_as;         /* AS number of source peer */
  L7_uint32      dst_as_path_segments;/* number of segments in path */
  SFLOW_extended_as_path_segment_t  *dst_as_path; /* list of seqs or sets */
  L7_uint32      communities_length;  /* number of communities */
  L7_uint32     *communities;         /* set of communities */
  L7_uint32      localpref;           /* LocalPref associated with this route */
} SFLOW_extended_gateway_t;

typedef struct SFLOW_string_s 
{
  L7_uint32      len;
  L7_uchar8     *str;
} SFLOW_string_t;

/* Extended user data */
typedef struct SFLOW_extended_user_s
{
  L7_uint32      src_charset;  /* MIBEnum value of character set used to encode a string - See RFC 2978
                                  Where possible UTF-8 encoding (MIBEnum=106) should be used. A value
                                  of zero indicates an unknown encoding. */
  SFLOW_string_t  src_user;
  L7_uint32       dst_charset;
  SFLOW_string_t  dst_user;
} SFLOW_extended_user_t;

/* Extended URL data */
typedef enum 
{
  SFLOW_EXTENDED_URL_SRC = 1, /* Source address is server */
  SFLOW_EXTENDED_URL_DST = 2  /* Destination address is server*/
} SFLOW_EXTENDED_URL_DIRECTION_t;

typedef struct SFLOW_extended_url_s 
{
  SFLOW_EXTENDED_URL_DIRECTION_t  direction; 
  SFLOW_string_t   url;       /* URL associated with the packet flow.  
                                Must be URL encoded */
  SFLOW_string_t   host;      /* The host field from the HTTP header */
} SFLOW_extended_url_t;

/* Extended MPLS data */
typedef struct SFLOW_label_stack_s
{
  L7_uint32   depth;
  L7_uint32  *stack; /* first entry is top of stack - see RFC 3032 for encoding */
} SFLOW_label_stack_t;

typedef struct SFLOW_extended_mpls_s 
{
  L7_inet_addr_t        nextHop;     /* Address of the next hop */
  SFLOW_label_stack_t   in_stack;
  SFLOW_label_stack_t   out_stack;
} SFLOW_extended_mpls_t;

  /* Extended NAT data
     Packet header records report addresses as seen at the sFlowDataSource.
     The extended_nat structure reports on translated source and/or destination
     addesses for this packet. If an address was not translated it should
     be equal to that reported for the header. */

typedef struct SFLOW_extended_nat_s 
{
  L7_inet_addr_t  src_address;    /* Source address */
  L7_inet_addr_t  dst_address;    /* Destination address */
} SFLOW_extended_nat_t;

/* Additional Extended MPLS stucts */
typedef struct SFLOW_extended_mpls_tunnel_s 
{
   SFLOW_string_t tunnel_lsp_name;  /* Tunnel name */
   L7_uint32      tunnel_id;        /* Tunnel ID */
   L7_uint32      tunnel_cos;       /* Tunnel COS value */
}  SFLOW_extended_mpls_tunnel_t;

typedef struct SFLOW_extended_mpls_vc_s 
{
   SFLOW_string_t vc_instance_name;  /* VC instance name */
   L7_uint32     vll_vc_id;         /* VLL/VC instance ID */
   L7_uint32     vc_label_cos;      /* VC Label COS value */
}  SFLOW_extended_mpls_vc_t;

/* Extended MPLS FEC
    - Definitions from MPLS-FTN-STD-MIB mplsFTNTable */

typedef struct SFLOW_extended_mpls_FTN_s
{
   SFLOW_string_t mplsFTNDescr;
   L7_uint32      mplsFTNMask;
}  SFLOW_extended_mpls_FTN_t;

/* Extended MPLS LVP FEC
    - Definition from MPLS-LDP-STD-MIB mplsFecTable
    Note: mplsFecAddrType, mplsFecAddr information available
          from packet header */
typedef struct SFLOW_extended_mpls_LDP_FEC_s
{
   L7_uint32 mplsFecAddrPrefixLength;
}  SFLOW_extended_mpls_LDP_FEC_t;

/* Extended VLAN tunnel information
   Record outer VLAN encapsulations that have
   been stripped. extended_vlantunnel information
   should only be reported if all the following conditions are satisfied:
     1. The packet has nested vlan tags, AND
     2. The reporting device is VLAN aware, AND
     3. One or more VLAN tags have been stripped, either
        because they represent proprietary encapsulations, or
        because switch hardware automatically strips the outer VLAN
        encapsulation.
   Reporting extended_vlantunnel information is not a substitute for
   reporting extended_switch information. extended_switch data must
   always be reported to describe the ingress/egress VLAN information
   for the packet. The extended_vlantunnel information only applies to
   nested VLAN tags, and then only when one or more tags has been
   stripped. */

typedef SFLOW_label_stack_t SFLOW_vlan_stack_t;
typedef struct SFLOW_extended_vlan_tunnel_s 
{
  SFLOW_vlan_stack_t stack;  /* List of stripped 802.1Q TPID/TCI layers. Each
                               TPID,TCI pair is represented as a single 32 bit
                               integer. Layers listed from outermost to
                               innermost. */
} SFLOW_extended_vlan_tunnel_t;

/* Data format */
typedef enum 
{
  /* enterprise = 0, format = ... */
  SFLOW_FLOW_HEADER    = 1,      /* Packet headers are sampled */
#define SFLOW_FLOW_HEADER_SAMPLE_OVERHEAD 56 /* A flow header sample will contain 
                                             (56 + sampled_header.header_length) Bytes */
  SFLOW_FLOW_ETHERNET  = 2,      /* MAC layer information */
  SFLOW_FLOW_IPV4      = 3,      /* IP version 4 data */
  SFLOW_FLOW_IPV6      = 4,      /* IP version 6 data */
  SFLOW_FLOW_EX_SWITCH    = 1001,      /* Extended switch information */
  SFLOW_FLOW_EX_ROUTER    = 1002,      /* Extended router information */
  SFLOW_FLOW_EX_GATEWAY   = 1003,      /* Extended gateway router information */
  SFLOW_FLOW_EX_USER      = 1004,      /* Extended TACAS/RADIUS user information */
  SFLOW_FLOW_EX_URL       = 1005,      /* Extended URL information */
  SFLOW_FLOW_EX_MPLS      = 1006,      /* Extended MPLS information */
  SFLOW_FLOW_EX_NAT       = 1007,      /* Extended NAT information */
  SFLOW_FLOW_EX_MPLS_TUNNEL  = 1008,   /* additional MPLS information */
  SFLOW_FLOW_EX_MPLS_VC      = 1009,
  SFLOW_FLOW_EX_MPLS_FTN     = 1010,
  SFLOW_FLOW_EX_MPLS_LDP_FEC = 1011,
  SFLOW_FLOW_EX_VLAN_TUNNEL  = 1012,   /* VLAN stack */
} SFLOW_FlOW_TYPE_TAG_t;

typedef union SFLOW_flow_type_s 
{
  SFLOW_sampled_header_t        header;
  SFLOW_sampled_ethernet_t      ethernet;
  SFLOW_sampled_ipv4_t          ipv4;
  SFLOW_sampled_ipv6_t          ipv6;
  SFLOW_extended_switch_t       sw;
  SFLOW_extended_router_t       router;
  SFLOW_extended_gateway_t      gateway;
  SFLOW_extended_user_t         user;
  SFLOW_extended_url_t          url;
  SFLOW_extended_mpls_t         mpls;
  SFLOW_extended_nat_t          nat;
  SFLOW_extended_mpls_tunnel_t  mpls_tunnel;
  SFLOW_extended_mpls_vc_t      mpls_vc;
  SFLOW_extended_mpls_FTN_t     mpls_ftn;
  SFLOW_extended_mpls_LDP_FEC_t mpls_ldp_fec;
  SFLOW_extended_vlan_tunnel_t  vlan_tunnel;
} SFLOW_flow_type_t;

typedef struct SFLOW_flow_sample_record_s 
{
  L7_uint32                         tag;  /* SFLOW_FLOW_TYPE_TAG_t */
  L7_uint32                         length;
  SFLOW_flow_type_t                 flow_data;
} SFLOW_flow_sample_record_t;

typedef enum
{
  SFLOW_FLOW_SAMPLE              = 1, /* enterprise = 0 : format = 1 */
  SFLOW_COUNTERS_SAMPLE          = 2, /* enterprise = 0 : format = 2 */
  SFLOW_FLOW_SAMPLE_EXPANDED     = 3, /* enterprise = 0 : format = 3 */
  SFLOW_COUNTERS_SAMPLE_EXPANDED = 4  /* enterprise = 0 : format = 4 */
} SFLOW_SAMPLE_TAG_t;

/* Format of a single flow sample datagram */
typedef struct SFLOW_flow_sample_s 
{
  L7_uint32 sequence_number;      /* Incremented with each flow sample
                                     generated */
  L7_uint32 source_id;            /* fsSourceId */
  L7_uint32 sampling_rate;        /* fsPacketSamplingRate */
  L7_uint32 sample_pool;          /* Total number of packets that could have been
                                     sampled (i.e. packets skipped by sampling
                                     process + total number of samples) */
  L7_uint32 drops;                /* Number of times a packet was dropped due to
                                     lack of resources */
  L7_uint32 input;                /* SNMP ifIndex of input interface.
                                     0 if interface is not known. */
  L7_uint32 output;               /* SNMP ifIndex of output interface,
                                     0 if interface is not known.
                                     Set most significant bit to indicate
                                     multiple destination interfaces
                                     (i.e. in case of broadcast or multicast)
                                     and set lower order bits to indicate
                                     number of destination interfaces.
                                     Examples:
                                     0x00000002  indicates ifIndex = 2
                                     0x00000000  ifIndex unknown.
                                     0x80000007  indicates a packet sent
                                     to 7 interfaces.
                                     0x80000000  indicates a packet sent to
                                     an unknown number of
                                     interfaces greater than 1.*/
  L7_uint32 num_records;
  SFLOW_flow_sample_record_t flow_records[SFLOW_FLOW_RECORDS_PER_SAMPLE];
} SFLOW_flow_sample_t;

typedef struct SFLOW_flow_sample_expanded_s 
{
  L7_uint32 sequence_number;      /* Incremented with each flow sample
                                     generated */
  L7_uint32 source_id_type;       /* EXPANDED */
  L7_uint32 source_id_index;      /* EXPANDED */
  L7_uint32 sampling_rate;        /* fsPacketSamplingRate */
  L7_uint32 sample_pool;          /* Total number of packets that could have been
                                     sampled (i.e. packets skipped by sampling
                                     process + total number of samples) */
  L7_uint32 drops;                /* Number of times a packet was dropped due to
                                     lack of resources */
  L7_uint32 inputFormat;          /* EXPANDED */
  L7_uint32 input;                /* SNMP ifIndex of input interface.
                                     0 if interface is not known. */
  L7_uint32 outputFormat;         /* EXPANDED */
  L7_uint32 output;               /* SNMP ifIndex of output interface,
                                     0 if interface is not known. */
  L7_uint32 num_records;
  SFLOW_flow_sample_record_t flow_records[SFLOW_FLOW_RECORDS_PER_SAMPLE];
} SFLOW_flow_sample_expanded_t;

/* Counters Data Types */
/* Generic interface counters - see RFC 1573, 2233 */
typedef struct SFLOW_if_counters_s 
{
  L7_uint32 ifIndex;
  L7_uint32 ifType;
  L7_uint64 ifSpeed;
#define SFLOW_ONE_MBPS               1000000
#define SFLOW_TEN_MBPS               10000000
#define SFLOW_HUNDRED_MBPS           100000000
#define SFLOW_THOUSAND_MBPS          1000000000
#define SFLOW_2P5THOUSAND_MBPS       2500000000UL   /* PTin added (2.5G) */
#define SFLOW_TENTHOUSAND_MBPS       10000000000
#define SFLOW_IFSPEEDMAX_MBPS        4294967295UL  /* max allowed ifSpeed value */
  L7_uint32 ifDirection;        /* Derived from MAU MIB (RFC 2668)
                                   0 = unknown, 1 = full-duplex,
                                   2 = half-duplex, 3 = in, 4 = out */
#define SFLOW_UNKNWN                   0
#define SFLOW_FULL_DPLX                1
#define SFLOW_HALF_DPLX                2
#define SFLOW_IN                       3
#define SFLOW_OUT                      4
  L7_uint32 ifStatus;           /* bit field with the following bits assigned:
                                   bit 0 = ifAdminStatus (0 = down, 1 = up)
                                   bit 1 = ifOperStatus (0 = down, 1 = up) */
  L7_uint64 ifInOctets;
  L7_uint32 ifInUcastPkts;
  L7_uint32 ifInMulticastPkts;
  L7_uint32 ifInBroadcastPkts;
  L7_uint32 ifInDiscards;
  L7_uint32 ifInErrors;
  L7_uint32 ifInUnknownProtos;
  L7_uint64 ifOutOctets;
  L7_uint32 ifOutUcastPkts;
  L7_uint32 ifOutMulticastPkts;
  L7_uint32 ifOutBroadcastPkts;
  L7_uint32 ifOutDiscards;
  L7_uint32 ifOutErrors;
  L7_uint32 ifPromiscuousMode;
#define SFLOW_IF_PROMISCOUS_MODE_TRUE  1
#define SFLOW_IF_PROMISCOUS_MODE_FALSE 2
} SFLOW_if_counters_t;
#define SFLOW_IF_COUNTERS_LEN  88
typedef enum 
{
  SFLOW_IF_INDEX = 0,
  SFLOW_IF_TYPE,
  SFLOW_IF_SPEED,
  SFLOW_IF_DIRECTION,
  SFLOW_IF_STATUS,
  SFLOW_IF_IN_OCTETS,
  SFLOW_IF_IN_UCAST_PKTS,
  SFLOW_IF_IN_MCAST_PKTS,
  SFLOW_IF_IN_BCAST_PKTS,
  SFLOW_IF_IN_DISCARDS,
  SFLOW_IF_IN_ERRORS,
  SFLOW_IF_IN_UNKNWN_PROTOS,
  SFLOW_IF_OUT_OCTETS,
  SFLOW_IF_OUT_UCAST_PKTS,
  SFLOW_IF_OUT_MCAST_PKTS,
  SFLOW_IF_OUT_BCAST_PKTS,
  SFLOW_IF_OUT_DISCARDS,
  SFLOW_IF_OUT_ERRORS,
  SFLOW_IF_PROMISCOUS_MODE,

  SFLOW_IF_COUNTERS
} SFLOW_IF_COUNTERS_t;

/* Ethernet interface counters - see RFC 2358 */
typedef struct SFLOW_ethernet_counters_s 
{
  L7_uint32 dot3StatsAlignmentErrors;
  L7_uint32 dot3StatsFCSErrors;
  L7_uint32 dot3StatsSingleCollisionFrames;
  L7_uint32 dot3StatsMultipleCollisionFrames;
  L7_uint32 dot3StatsSQETestErrors;
  L7_uint32 dot3StatsDeferredTransmissions;
  L7_uint32 dot3StatsLateCollisions;
  L7_uint32 dot3StatsExcessiveCollisions;
  L7_uint32 dot3StatsInternalMacTransmitErrors;
  L7_uint32 dot3StatsCarrierSenseErrors;
  L7_uint32 dot3StatsFrameTooLongs;
  L7_uint32 dot3StatsInternalMacReceiveErrors;
  L7_uint32 dot3StatsSymbolErrors;
} SFLOW_ethernet_counters_t;
#define SFLOW_ETH_COUNTERS_LEN 52
typedef enum
{
  SFLOW_ETH_ALIGNMENT_ERRORS = SFLOW_IF_COUNTERS + 1,
  SFLOW_ETH_FCS_ERRORS,
  SFLOW_ETH_SINGLE_COLLISION_FRAMES,
  SFLOW_ETH_MULTIPLE_COLLISION_FRAMES,
  SFLOW_ETH_SQE_TEST_ERRORS,
  SFLOW_ETH_DEFERRED_TX,
  SFLOW_ETH_LATE_COLLISIONS,
  SFLOW_ETH_EXCESSIVE_COLLISIONS,
  SFLOW_ETH_INTERNAL_MAC_TX_ERRORS,
  SFLOW_ETH_CARIER_SENSE_ERRORS,
  SFLOW_ETH_FRAME_TOO_LONGS,
  SFLOW_ETH_INTERNAL_MAC_RX_ERRORS,
  SFLOW_ETH_SYMBOL_ERRORS,

  SFLOW_ETH_COUNTERS
} SFLOW_ETH_COUNTERS_t;
/* Token ring counters - see RFC 1748 */
typedef struct SFLOW_tokenring_counters_s 
{
  L7_uint32 dot5StatsLineErrors;
  L7_uint32 dot5StatsBurstErrors;
  L7_uint32 dot5StatsACErrors;
  L7_uint32 dot5StatsAbortTransErrors;
  L7_uint32 dot5StatsInternalErrors;
  L7_uint32 dot5StatsLostFrameErrors;
  L7_uint32 dot5StatsReceiveCongestions;
  L7_uint32 dot5StatsFrameCopiedErrors;
  L7_uint32 dot5StatsTokenErrors;
  L7_uint32 dot5StatsSoftErrors;
  L7_uint32 dot5StatsHardErrors;
  L7_uint32 dot5StatsSignalLoss;
  L7_uint32 dot5StatsTransmitBeacons;
  L7_uint32 dot5StatsRecoverys;
  L7_uint32 dot5StatsLobeWires;
  L7_uint32 dot5StatsRemoves;
  L7_uint32 dot5StatsSingles;
  L7_uint32 dot5StatsFreqErrors;
} SFLOW_tokenring_counters_t;

/* 100 BaseVG interface counters - see RFC 2020 */
typedef struct SFLOW_vg_counters_s
{
  L7_uint32 dot12InHighPriorityFrames;
  L7_uint64 dot12InHighPriorityOctets;
  L7_uint32 dot12InNormPriorityFrames;
  L7_uint64 dot12InNormPriorityOctets;
  L7_uint32 dot12InIPMErrors;
  L7_uint32 dot12InOversizeFrameErrors;
  L7_uint32 dot12InDataErrors;
  L7_uint32 dot12InNullAddressedFrames;
  L7_uint32 dot12OutHighPriorityFrames;
  L7_uint64 dot12OutHighPriorityOctets;
  L7_uint32 dot12TransitionIntoTrainings;
  L7_uint64 dot12HCInHighPriorityOctets;
  L7_uint64 dot12HCInNormPriorityOctets;
  L7_uint64 dot12HCOutHighPriorityOctets;
} SFLOW_vg_counters_t;

typedef struct SFLOW_vlan_counters_s 
{
  L7_uint32 vlan_id;
  L7_uint64 octets;
  L7_uint32 ucastPkts;
  L7_uint32 multicastPkts;
  L7_uint32 broadcastPkts;
  L7_uint32 discards;
} SFLOW_vlan_counters_t;

/* Counters data */
typedef enum
{
  /* enterprise = 0, format = ... */
  SFLOW_COUNTERS_GENERIC      = 1,
  SFLOW_COUNTERS_ETHERNET     = 2,
  SFLOW_COUNTERS_TOKENRING    = 3,
  SFLOW_COUNTERS_VG           = 4,
  SFLOW_COUNTERS_VLAN         = 5
} SFLOW_COUNTERS_TYPE_TAG_t;

typedef union SFLOW_counters_type_s 
{
  SFLOW_if_counters_t        generic;
  SFLOW_ethernet_counters_t  ethernet;
  SFLOW_tokenring_counters_t tokenring;
  SFLOW_vg_counters_t        vg;
  SFLOW_vlan_counters_t      vlan;
} SFLOW_counters_type_t;

typedef struct SFLOW_counters_sample_record_s 
{
  SFLOW_COUNTERS_TYPE_TAG_t              tag;
  L7_uint32                              length;
  SFLOW_counters_type_t                  counter_data;
} SFLOW_counters_sample_record_t;

typedef struct SFLOW_counters_sample_s 
{
  L7_uint32 sequence_number;    /* Incremented with each counters sample
                                   generated by this source_id */
  L7_uint32 source_id;          /* fsSourceId */
  L7_uint32 num_records;
  SFLOW_counters_sample_record_t counter_records[SFLOW_COUNTER_RECORDS_PER_SAMPLE];
} SFLOW_counters_sample_t;

/* same thing, but the expanded version, so ds_index can be a full 32 bits */
typedef struct SFLOW_counters_sample_expanded_s 
{
  L7_uint32 sequence_number;    /* Incremented with each counters sample
                                   generated by this source_id */
  L7_uint32 source_id_type;     /* EXPANDED */
  L7_uint32 source_id_index;    /* EXPANDED */
  L7_uint32 num_records;
  SFLOW_counters_sample_record_t counter_records[SFLOW_COUNTER_RECORDS_PER_SAMPLE];
} SFLOW_counters_sample_expanded_t;

typedef enum  
{
  SFLOW_DATAGRAM_VERSION2 = 2,
  SFLOW_DATAGRAM_VERSION4 = 4,
  SFLOW_DATAGRAM_VERSION5 = 5
} SFLOW_DATAGRAM_VERSION_t;


typedef union SFLOW_sample_type_s
{
  SFLOW_flow_sample_t              flow_sample;
  SFLOW_flow_sample_expanded_t     flow_sample_ext;
  SFLOW_counters_sample_t          counter_sample;
  SFLOW_counters_sample_expanded_t counter_sample_ext;
} SFLOW_sample_type_t;

/* sample data */
typedef struct SFLOW_sample_data_s
{
   SFLOW_SAMPLE_TAG_t    data_format;
   L7_uint32             length; /* sample length */ 
   SFLOW_sample_type_t   sample_data;  
} SFLOW_sample_data_t;

/* sFlow Datagram v5 header */
typedef struct SFLOW_sample_datagram_hdr_s 
{
  L7_uint32       datagram_version;/* (enum SFLDatagram_version) = VERSION5 = 5 */
  L7_inet_addr_t  agent_address;   /* IP address of sampling agent */
  L7_uint32       sub_agent_id;    /* Used to distinguishing between datagram
                                      streams from separate agent sub entities
                                      within an device. */
  L7_uint32       sequence_number; /* Incremented with each sample datagram
                                      generated */
  L7_uint32       uptime;          /* Current time (in milliseconds since device
                                      last booted). Should be set as close to
                                      datagram transmission time as possible.*/
  L7_uint32       num_records;     /* Number of tag-len-val flow/counter records to follow */
} SFLOW_sample_datagram_hdr_t;
#define SFLOW_SAMPLE_DATAGRAM_IP6_LEN (24 + L7_IP6_ADDR_LEN)
#define SFLOW_SAMPLE_DATAGRAM_IP4_LEN (24 + L7_IP_ADDR_LEN)
#endif /* SFLOW_PROTO_H */
