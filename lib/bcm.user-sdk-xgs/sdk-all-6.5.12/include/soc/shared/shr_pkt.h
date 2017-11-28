
/* 
 * $Id: shr_pkt.h,v 1.16 2017/02/23 14:48 sraghave Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        shr_pkt.h
 * Purpose:     Common Packet Format definitions shared by
 *              SDK and UKERNEL
 */

#ifndef   _SOC_SHARED_SHR_PKT_H_
#define   _SOC_SHARED_SHR_PKT_H_

#include <shared/pack.h>

#include <bcm/types.h>

/* UDP Header */
#define SHR_UDP_HEADER_LENGTH            8

/* IPv4 Header */
#define SHR_IPV4_VERSION                 4
#define SHR_IPV4_HEADER_LENGTH           20    /* Bytes */
#define SHR_IPV4_HEADER_LENGTH_WORDS     5     /* Words */

/* L2 Header */
#define SHR_L2_ETYPE_IPV4                  0x0800
#define SHR_MAC_ADDR_LENGTH    (sizeof(bcm_mac_t))

/* IP Protocols */
#define SHR_IP_PROTOCOL_UDP              17

/* UDP Header */
typedef struct shr_udp_header_s {  /* <num bits>: ... */
    uint16  src;         /* 16: Source Port Number */
    uint16  dst;         /* 16: Destination Port Number */
    uint16  length;      /* 16: Length */
    uint16  sum;         /* 16: Checksum */
} shr_udp_header_t;

/* IPv4 Header */
typedef struct shr_ipv4_header_s {
    uint8   version;     /*  4: Version */
    uint8   h_length;    /*  4: Header length */
    uint8   dscp;        /*  6: Differentiated Services Code Point */
    uint8   ecn;         /*  2: Explicit Congestion Notification */
    uint16  length;      /* 16: Total Length bytes (header + data) */
    uint16  id;          /* 16: Identification */
    uint8   flags;       /*  3: Flags */
    uint16  f_offset;    /* 13: Fragment Offset */
    uint8   ttl;         /*  8: Time to Live */
    uint8   protocol;    /*  8: Protocol */
    uint16  sum;         /* 16: Header Checksum */
    uint32  src;         /* 32: Source IP Address */
    uint32  dst;         /* 32: Destination IP Address */
} shr_ipv4_header_t;

/* IPv6 Header */
typedef struct shr_ipv6_header_s {
    uint8      version;        /*   4: Version */
    uint8      t_class;        /*   8: Traffic Class (6:dscp, 2:ecn) */
    uint32     f_label;        /*  20: Flow Label */
    uint16     p_length;       /*  16: Payload Length */
    uint8      next_header;    /*   8: Next Header */
    uint8      hop_limit;      /*   8: Hop Limit */
    bcm_ip6_t  src;            /* 128: Source IP Address */
    bcm_ip6_t  dst;            /* 128: Destination IP Address */
} shr_ipv6_header_t;

/* GRE - Generic Routing Encapsulation */
typedef struct shr_gre_header_s {
    uint8   checksum;           /*  1: Checksum present */
    uint8   routing;            /*  1: Routing present */
    uint8   key;                /*  1: Key present */
    uint8   sequence_num;       /*  1: Sequence number present */
    uint8   strict_src_route;   /*  1: Strict Source Routing */
    uint8   recursion_control;  /*  3: Recursion Control */
    uint8   flags;              /*  5: Flags */
    uint8   version;            /*  3: Version */
    uint16  protocol;           /* 16: Protocol Type */
} shr_gre_header_t;

/* ACH - Associated Channel Header */
typedef struct shr_ach_header_s {
    uint8   f_nibble;        /*  4: First nibble, must be 1 */
    uint8   version;         /*  4: Version */
    uint8   reserved;        /*  8: Reserved */
    uint16  channel_type;    /* 16: Channel Type */
} shr_ach_header_t;

/* MPLS - Multiprotocol Label Switching Label */
typedef struct shr_mpls_label_s {
    uint32  label;    /* 20: Label */
    uint8   exp;      /*  3: Experimental, Traffic Class, ECN */
    uint8   s;        /*  1: Bottom of Stack */
    uint8   ttl;      /*  8: Time to Live */
} shr_mpls_label_t;

/* VLAN Tag - 802.1Q */
typedef struct shr_vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint8   prio;    /*  3: Priority Code Point */
        uint8   cfi;     /*  1: Canonical Format Indicator */
        uint16  vid;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} shr_vlan_tag_t;

/* L2 Header */
typedef struct shr_l2_header_t {
    bcm_mac_t       dst_mac;      /* 48: Destination MAC */
    bcm_mac_t       src_mac;      /* 48: Source MAC */
    shr_vlan_tag_t  outer_vlan_tag;    /* VLAN Tag */
    shr_vlan_tag_t  inner_vlan_tag;    /* inner VLAN Tag */
    uint16          etype;        /* 16: Ether Type */
} shr_l2_header_t;


/*******************************************
 * Pack/Unpack Macros
 * 
 * Data is packed/unpacked in/from Network byte order
 */
#define SHR_PKT_PACK_U8(_buf, _var)     _SHR_PACK_U8(_buf, _var)
#define SHR_PKT_PACK_U16(_buf, _var)    _SHR_PACK_U16(_buf, _var)
#define SHR_PKT_PACK_U32(_buf, _var)    _SHR_PACK_U32(_buf, _var)
#define SHR_PKT_UNPACK_U8(_buf, _var)   _SHR_UNPACK_U8(_buf, _var)
#define SHR_PKT_UNPACK_U16(_buf, _var)  _SHR_UNPACK_U16(_buf, _var)
#define SHR_PKT_UNPACK_U32(_buf, _var)  _SHR_UNPACK_U32(_buf, _var)

/* Pack routines used to pack these header information into
 * provided buffers in network byte order.
 */
uint8 *
shr_udp_header_pack(uint8 *buffer, shr_udp_header_t *udp);
uint8 *
shr_ipv4_header_pack(uint8 *buffer, shr_ipv4_header_t *ip);
uint8 *
shr_ipv6_header_pack(uint8 *buffer, shr_ipv6_header_t *ip);
uint8 *
shr_gre_header_pack(uint8 *buffer, shr_gre_header_t *gre);
uint8 *
shr_ach_header_pack(uint8 *buffer, shr_ach_header_t *ach);
uint8 *
shr_mpls_label_pack(uint8 *buffer, shr_mpls_label_t *mpls);
uint8 *
shr_l2_header_pack(uint8 *buffer, shr_l2_header_t *l2);

/* Utility functions */
uint32
shr_initial_chksum_get(uint16 length, uint8 *data);

uint32
shr_udp_initial_checksum_get(int v6,
                             shr_ipv4_header_t *ipv4,
                             shr_ipv6_header_t *ipv6,
                             shr_udp_header_t  *udp);

#endif/* _SOC_SHARED_SHR_PKT_H_ */
