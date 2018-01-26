/*
 * $Id: $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        soc/shared/dcbformats/type36.h
 * Purpose:     Define dma control block (DCB) format for a type36 DCB
 *              used by the 56870 (Trident 3)
 *
 *              This file is shared between the SDK and the embedded applications.
 */

#ifndef _SOC_SHARED_DCBFORMATS_TYPE36_H
#define _SOC_SHARED_DCBFORMATS_TYPE36_H

/*
 * DMA Control Block - Type 36
 * Used on 56870 devices
 * 16 words
 */
typedef struct {
    uint32  addr;                       /* T36.0: physical address */

#ifdef  LE_HOST
                                        /* T36.1: Control 0 */
    uint32  c_count:16,                 /* Requested byte count */
            c_chain:1,                  /* Chaining */
            c_sg:1,                     /* Scatter Gather */
            c_reload:1,                 /* Reload */
            c_hg:1,                     /* Higig (TX) */
            c_stat:1,                   /* Update stats (TX) */
            c_pause:1,                  /* Pause packet (TX) */
            c_purge:1,                  /* Purge packet (TX) */
            c_intr:2,                   /* Desc Intr Mode */
            :7;                         /* Don't care */
#else
    uint32  :7,                         /* Don't care */
            c_intr:2,                   /* Desc Intr Mode */
            c_purge:1,                  /* Purge packet (TX) */
            c_pause:1,                  /* Pause packet (TX) */
            c_stat:1,                   /* Update stats (TX) */
            c_hg:1,                     /* Higig (TX) */
            c_reload:1,                 /* Reload */
            c_sg:1,                     /* Scatter Gather */
            c_chain:1,                  /* Chaining */
            c_count:16;                 /* Requested byte count */
#endif  /* LE_HOST */

                                        /* T36.2 (word 14 in EP_TO_CPU_HDR) */
    uint32  mh0;                        /* Module Header word 0 */
                                        /* T36.3 (word 13 in EP_TO_CPU_HDR) */
    uint32  mh1;                        /* Module Header word 1 */
                                        /* T36.4 (word 12 in EP_TO_CPU_HDR) */
    uint32  mh2;                        /* Module Header word 2 */
                                        /* T36.5 (word 11 in EP_TO_CPU_HDR) */
    uint32  mh3;                        /* Module Header word 3 */

                                        /* T36.6 (word 10 in EP_TO_CPU_HDR) */
                                        /* word 9 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  reason_hi;                  /* CPU opcode (hi bits) */
                                        /* T36.7 (word 9 in EP_TO_CPU_HDR) */
                                        /* word 8 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  reason;                     /* CPU opcode (low bits) */

#ifdef  LE_HOST
                                        /* T36.8 (word 8 in EP_TO_CPU_HDR) */
                                        /* word 7 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  srcport:8,          /* Source port number */
                    pkt_len:14,         /* Packet length */
                    cpu_cos:6,          /* CPU COS*/
                    cpu_opcode_type:4;  /* CPU opcode type */
        } overlay1;
        struct {
            uint32  srcport:8,          /* Source port number */
                    pkt_len:14,         /* Packet length */
                    queue_num:6,        /* MMU COS*/
                    cpu_opcode_type:4;  /* CPU opcode type */
        } overlay2;
    } word8;

#else
                                        /* T36.8 (word 8 in EP_TO_CPU_HDR) */
                                        /* word 7 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  cpu_opcode_type:4,  /* CPU opcode type */
                    cpu_cos:6,          /* CPU COS*/
                    pkt_len:14,         /* Packet length */
                    srcport:8;          /* Source port number */
        } overlay1;
        struct {
            uint32  cpu_opcode_type:4,  /* CPU opcode type */
                    queue_num:6,        /* MMU COS*/
                    pkt_len:14,         /* Packet length */
                    srcport:8;          /* Source port number */
        } overlay2;
    } word8;
#endif


#ifdef  LE_HOST
                                        /* T36.9 (word 7 in EP_TO_CPU_HDR) */
                                        /* word 6 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  nhi_a_hi:7,                 /* NEXT_HOP_INDEX_A (Underlay) */
            nhi_type:2,                 /* Type for NEXT_HOP_INDEX_A and B */
            match_rule:8,               /* Matched FP rule */
            forwarding_type:2,          /* Packet forwarding type */
            ucq:1,                      /* Unicast queue */
            outer_vid:12;               /* Outer VLAN ID */

                                        /* T36.10 (word 6 in EP_TO_CPU_HDR) */
                                        /* word 5 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  outer_pri:3,                /* Outer priority */
            inner_cfi:1,                /* Inner Canoncial Format Indicator */
            inner_pri:3,                /* Inner priority */
            nhi_b:17,                   /* NEXT_HOP_INDEX_B (Overlay) */
            nhi_a_lo:8;                 /* NEXT_HOP_INDEX_A (Underlay) */

                                        /* T36.11 (word 5 in EP_TO_CPU_HDR) */
                                        /* word 4 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  mtp_index:5,                /* MTP index */
            dscp:6,                     /* New DSCP */
            chg_tos:1,                  /* DSCP has been changed by HW */
            vntag_action:2,             /* VN tag action */
            outer_cfi:1,                /* Outer Canoncial Format Indicator */
            decap_tunnel_type:5,        /* Tunnel type that was decapsulated */
            inner_vid:12;               /* Inner VLAN ID */

#else
                                        /* T36.9 (word 7 in EP_TO_CPU_HDR) */
                                        /* word 6 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  outer_vid:12,               /* Outer VLAN ID */
            ucq:1,                      /* Unicast queue */
            forwarding_type:2,          /* Packet forwarding type */
            match_rule:8,               /* Matched FP rule */
            nhi_type:2,                 /* Type for NEXT_HOP_INDEX_A and B */
            nhi_a_hi:7;                 /* NEXT_HOP_INDEX_A (Underlay) */

                                        /* T36.10 (word 6 in EP_TO_CPU_HDR) */
                                        /* word 5 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  nhi_a_lo:8,                 /* NEXT_HOP_INDEX_A (Underlay) */
            nhi_b:17,                   /* NEXT_HOP_INDEX_B (Overlay) */
            inner_pri:3,                /* Inner priority */
            inner_cfi:1,                /* Inner Canoncial Format Indicator */
            outer_pri:3;                /* Outer priority */

                                        /* T36.11 (word 5 in EP_TO_CPU_HDR) */
                                        /* word 4 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  inner_vid:12,               /* Inner VLAN ID */
            decap_tunnel_type:5,        /* Tunnel type that was decapsulated */
            outer_cfi:1,                /* Outer Canoncial Format Indicator */
            vntag_action:2,             /* VN tag action */
            chg_tos:1,                  /* DSCP has been changed by HW */
            dscp:6,                     /* New DSCP */
            mtp_index:5;                /* MTP index */
#endif

                                        /* T36.12 (word 4 in EP_TO_CPU_HDR) */
                                        /* word 3 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  timestamp;                  /* Timestamp */

#ifdef  LE_HOST
                                        /* T36.13 (word 3 in EP_TO_CPU_HDR) */
                                        /* word 2 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  bpdu:1,                     /* BPDU Packet */
            timestamp_type:2,           /* Timestamp type */
            loopback_pkt_type:3,        /* Loopback packet type */
            ecn:2,                      /* New ECN value */
            chg_ecn:1,                  /* ECN changed */
            :23;

                                        /* T36.14 (word 2 in EP_TO_CPU_HDR) */
                                        /* word 1 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  match_id_hi:18,             /* Packet MATCH_ID upper bits*/
            otag_action:2,              /* Ingress outer tag action */
            itag_action:2,              /* Ingress inner tag action */
            service_tag:1,              /* SD tag present */
            switch_pkt:1,               /* Switched packet */
            uc_switch_drop:1,           /* Unicast switched copies dropped */
            src_hg:1,                   /* Source is Higig */
            l3routed:3,                 /* Any IP routed packet */
            l3only:1,                   /* L3 only IPMC packet */
            replicated:1,               /* Replicated copy */
            do_not_change_ttl:1;        /* Do not change TTL */

                                        /* T36.15 (word 1 in EP_TO_CPU_HDR) */
                                        /* word 0 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  match_id_lo;                /* Packet MATCH_ID lower bits*/

    union {                             /* T36.16 (word 0 in EP_TO_CPU_HDR) */
        uint32  timestamp_hi;           /* Timestamp upper bits */
        struct {                        /* EH_TYPE_0 */
            uint32  eh_queue_tag:16,
                    :3,
                    eh_tag_type:2,
                    eh_seg_sel:3,
                    :3,
                    os1588_done:1,
                    eh_type:4;
        } overlay1;
        struct {                        /* EH_TYPE_1 */
            uint32  classid: 9,
                    l3_iif:14,
                    classid_type:4,
                    os1588_done:1,
                    eh_type:4;
        } overlay2;
        struct {                        /* EH_TYPE 2 */
            uint32  eh_queue_tag: 14,
                    classid:9,
                    classid_type:4,
                    os1588_done:1,
                    eh_type:4;
        } overlay3;
        struct {
            uint32  rx_bfd_session_index:12,    /* Rx BFD pkt session ID */
                    rx_bfd_start_offset_type:2, /* Rx BFD pkt st offset type */
                    rx_bfd_start_offset:8,      /* Rx BFD pkt start offset */
                    :10;                        /* Reserved */
        } overlay4;
    } word16;
                                        /* T36.17: DMA Status 0 */
    uint32  count:16,                   /* Transferred byte count */
            end:1,                      /* End bit (RX) */
            start:1,                    /* Start bit (RX) */
            error:1,                    /* Cell Error (RX) */
            :12,                        /* Reserved */
            done:1;                     /* Descriptor Done */

#else
                                        /* T36.13 (word 3 in EP_TO_CPU_HDR) */
                                        /* word 2 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  :23,
            chg_ecn:1,                  /* ECN changed */
            ecn:2,                      /* New ECN value */
            loopback_pkt_type:3,        /* Loopback packet type */
            timestamp_type:2,           /* Timestamp type */
            bpdu:1;                     /* BPDU Packet */

                                        /* T36.14 (word 2 in EP_TO_CPU_HDR) */
                                        /* word 1 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  do_not_change_ttl:1,        /* Do not change TTL */
            replicated:1,               /* Replicated copy */
            l3only:1,                   /* L3 only IPMC packet */
            l3routed:3,                 /* Any IP routed packet */
            src_hg:1,                   /* Source is Higig */
            uc_switch_drop:1,           /* Unicast switched copies dropped */
            switch_pkt:1,               /* Switched packet */
            service_tag:1,              /* SD tag present */
            itag_action:2,              /* Ingress inner tag action */
            otag_action:2,              /* Ingress outer tag action */
            match_id_hi:18;             /* Packet MATCH_ID upper bits*/

                                        /* T36.15 (word 1 in EP_TO_CPU_HDR) */
                                        /* word 0 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  match_id_lo;                /* Packet MATCH_ID lower bits*/

    union {                             /* T36.16 (word 0 in EP_TO_CPU_HDR) */
        uint32  timestamp_hi;           /* Timestamp upper bits */
        struct {                        /* EH_TYPE_0 */
            uint32  eh_type:4,
                    os1588_done:1,
                    :3,
                    eh_seg_sel:3,
                    eh_tag_type:2,
                    :3,
                    eh_queue_tag:16;
        } overlay1;
        struct {                        /* EH_TYPE_1 */
            uint32  eh_type:4,
                    os1588_done:1,
                    classid_type:4,
                    l3_iif:14,
                    classid: 9;
        } overlay2;
        struct {                        /* EH_TYPE 2 */
            uint32  eh_type:4,
                    os1588_done:1,
                    classid_type:4,
                    classid:9,
                    eh_queue_tag:14;
        } overlay3;
        struct {
            uint32  :10,                        /* Reserved */
                    rx_bfd_start_offset:8,      /* Rx BFD pkt start offset */
                    rx_bfd_start_offset_type:2, /* Rx BFD pkt st offset type */
                    rx_bfd_session_index:12;    /* Rx BFD pkt session ID */
        } overlay4;
    } word16;

                                        /* T36.17: DMA Status 0 */
    uint32  done:1,                     /* Descriptor Done */
            :12,                        /* Reserved */
            error:1,                    /* Cell Error (RX) */
            start:1,                    /* Start bit (RX) */
            end:1,                      /* End bit (RX) */
            count:16;                   /* Transferred byte count */
#endif
} dcb36_t;


/* Trident 3 CMICX DCB */
typedef struct {
    uint32  addr_lo;                    /* T36.0: physical address, lower 32 bits */
    uint32  addr_hi;                    /* T36.0: physical address, upper 32 bits */

#ifdef  LE_HOST
    uint32  c_count:16,                 /* Requested byte count */
            c_chain:1,                  /* Chaining */
            c_sg:1,                     /* Scatter Gather */
            c_reload:1,                 /* Reload */
            c_hg:1,                     /* Higig (TX) */
            :2,                         /* reserved */
            c_purge:1,                  /* Purge packet (TX) */
            c_intr:2,                   /* Desc Intr Mode */
            c_desc:4,                   /* descriptors remaining */
            c_desc_status:1,            /* disable descriptor status write */
            :2;                         /* reserved */
#else
    uint32  :2,                         /* resered */
            c_desc_status:1,            /* disable descriptor status write */
            c_desc:4,                   /* descriptors remaining */
            c_intr:2,                   /* Desc Intr Mode */
            c_purge:1,                  /* Purge packet (TX) */
            :2,                         /* reserved */
            c_hg:1,                     /* Higig (TX) */
            c_reload:1,                 /* Reload */
            c_sg:1,                     /* Scatter Gather */
            c_chain:1,                  /* Chaining */
            c_count:16;                 /* Requested byte count */
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  count:16,                   /* Transferred byte count */
            end:1,                      /* End bit (RX) */
            start:1,                    /* Start bit (RX) */
            error:1,                    /* Cell Error (RX) */
            ecc_error:1,                /* packet ECC Error (RX) */
            :11,                        /* Reserved */
            done:1;                     /* Descriptor Done */
#else
    uint32  done:1,                     /* Descriptor Done */
            :11,                        /* Reserved */
            ecc_error:1,                /* Packet ECC Error (RX) */
            error:1,                    /* Cell Error (RX) */
            start:1,                    /* Start bit (RX) */
            end:1,                      /* End bit (RX) */
            count:16;                   /* Transferred byte count */
#endif
} dcb36_cmicx_t;

typedef struct {
    uint32  mh0;                        /* Module Header word 0 */
                                        /* (word 14 in EP_TO_CPU_HDR) */
    uint32  mh1;                        /* Module Header word 1 */
                                        /* (word 13 in EP_TO_CPU_HDR) */
    uint32  mh2;                        /* Module Header word 2 */
                                        /* (word 12 in EP_TO_CPU_HDR) */
    uint32  mh3;                        /* Module Header word 3 */
                                        /* (word 11 in EP_TO_CPU_HDR) */

                                        /* (word 10 in EP_TO_CPU_HDR) */
                                        /* word 9 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  reason_hi;                  /* CPU opcode (hi bits) */

                                        /* (word 9 in EP_TO_CPU_HDR) */
                                        /* word 8 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  reason;                     /* CPU opcode (low bits) */

#ifdef  LE_HOST
                                        /* (word 8 in EP_TO_CPU_HDR) */
                                        /* word 7 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  srcport:8,          /* Source port number */
                    pkt_len:14,         /* Packet length */
                    cpu_cos:6,          /* CPU COS*/
                    ecn:2,              /* New ECN value */
                    chg_ecn:1,          /* ECN changed */
                    bfd_session_index_valid:1;  /* Indicate a BFD terminated packet */
        } overlay1;
        struct {
            uint32  srcport:8,          /* Source port number */
                    pkt_len:14,         /* Packet length */
                    queue_num:6,        /* MMU COS*/
                    ecn:2,              /* New ECN value */
                    chg_ecn:1,          /* ECN changed */
                    bfd_session_index_valid:1;  /* Indicate a BFD terminated packet */
        } overlay2;
    } word8;

#else
                                        /* (word 8 in EP_TO_CPU_HDR) */
                                        /* word 7 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  bfd_session_index_valid:1,  /* Indicate a BFD terminated packet */
                    chg_ecn:1,          /* ECN changed */
                    ecn:2,              /* New ECN value */
                    cpu_cos:6,          /* CPU COS*/
                    pkt_len:14,         /* Packet length */
                    srcport:8;          /* Source port number */
        } overlay1;
        struct {
            uint32  bfd_session_index_valid:1,  /* Indicate a BFD terminated packet */
                    chg_ecn:1,          /* ECN changed */
                    ecn:2,              /* New ECN value */
                    queue_num:6,        /* MMU COS*/
                    pkt_len:14,         /* Packet length */
                    srcport:8;          /* Source port number */
        } overlay2;
    } word8;
#endif


#ifdef  LE_HOST
                                        /* (word 7 in EP_TO_CPU_HDR) */
                                        /* word 6 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  nhi_a_hi:7,                 /* NEXT_HOP_INDEX_A (Underlay) */
            nhi_type:2,                 /* Type for NEXT_HOP_INDEX_A and B */
            match_rule:8,               /* Matched FP rule */
            forwarding_type:2,          /* Packet forwarding type */
            ucq:1,                      /* Unicast queue */
            outer_vid:12;               /* Outer VLAN ID */

                                        /* (word 6 in EP_TO_CPU_HDR) */
                                        /* word 5 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  outer_pri:3,                /* Outer priority */
            inner_cfi:1,                /* Inner Canoncial Format Indicator */
            inner_pri:3,                /* Inner priority */
            nhi_b:16,                   /* NEXT_HOP_INDEX_B (Overlay) */
            nhi_a_lo:9;                 /* NEXT_HOP_INDEX_A (Underlay) */

                                        /* (word 5 in EP_TO_CPU_HDR) */
                                        /* word 4 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  mtp_index:5,                /* MTP index */
            dscp:6,                     /* New DSCP */
            chg_tos:1,                  /* DSCP has been changed by HW */
            vntag_action:2,             /* VN tag action */
            outer_cfi:1,                /* Outer Canoncial Format Indicator */
            decap_tunnel_type:5,        /* Tunnel type that was decapsulated */
            inner_vid:12;               /* Inner VLAN ID */

#else
                                        /* (word 7 in EP_TO_CPU_HDR) */
                                        /* word 6 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  outer_vid:12,               /* Outer VLAN ID */
            ucq:1,                      /* Unicast queue */
            forwarding_type:2,          /* Packet forwarding type */
            match_rule:8,               /* Matched FP rule */
            nhi_type:2,                 /* Type for NEXT_HOP_INDEX_A and B */
            nhi_a_hi:7;                 /* NEXT_HOP_INDEX_A (Underlay) */

                                        /* (word 6 in EP_TO_CPU_HDR) */
                                        /* word 5 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  nhi_a_lo:9,                 /* NEXT_HOP_INDEX_A (Underlay) */
            nhi_b:16,                   /* NEXT_HOP_INDEX_B (Overlay) */
            inner_pri:3,                /* Inner priority */
            inner_cfi:1,                /* Inner Canoncial Format Indicator */
            outer_pri:3;                /* Outer priority */

                                        /* (word 5 in EP_TO_CPU_HDR) */
                                        /* word 4 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  inner_vid:12,               /* Inner VLAN ID */
            decap_tunnel_type:5,        /* Tunnel type that was decapsulated */
            outer_cfi:1,                /* Outer Canoncial Format Indicator */
            vntag_action:2,             /* VN tag action */
            chg_tos:1,                  /* DSCP has been changed by HW */
            dscp:6,                     /* New DSCP */
            mtp_index:5;                /* MTP index */
#endif

                                        /* (word 4 in EP_TO_CPU_HDR) */
                                        /* word 3 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  timestamp;                  /* Timestamp */

#ifdef  LE_HOST
                                        /* (word 3 in EP_TO_CPU_HDR) */
                                        /* word 2 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  bpdu:1,                     /* BPDU Packet */
            :2,                         /* Reserved */
            hg_lookup_pkt:1,            /* Higig Lookup Packet */
            vrf:12,                     /* VRF */
            timestamp_type:2,           /* Timestamp type */
            l3_iif:14;                  /* L3 IIF */

                                        /* (word 2 in EP_TO_CPU_HDR) */
                                        /* word 1 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  match_id_hi:18,             /* Packet MATCH_ID upper bits*/
            otag_action:2,              /* Ingress outer tag action */
            itag_action:2,              /* Ingress inner tag action */
            service_tag:1,              /* SD tag present */
            switch_pkt:1,               /* Switched packet */
            uc_switch_drop:1,           /* Unicast switched copies dropped */
            src_hg:1,                   /* Source is Higig */
            l3routed:3,                 /* Any IP routed packet */
            l3only:1,                   /* L3 only IPMC packet */
            replicated:1,               /* Replicated copy */
            do_not_change_ttl:1;        /* Do not change TTL */

                                        /* (word 1 in EP_TO_CPU_HDR) */
                                        /* word 0 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  match_id_lo;                /* Packet MATCH_ID lower bits*/

    union {                             /* (word 0 in EP_TO_CPU_HDR) */
        struct {                        /* EH_TYPE_0 */
            uint32  eh_queue_tag:16,
                    :3,
                    eh_tag_type:2,
                    eh_seg_sel:3,
                    :3,
                    os1588_done:1,
                    eh_type:4;
        } overlay1;
        struct {                        /* EH_TYPE_1 */
            uint32  classid: 9,
                    l3_iif:14,
                    classid_type:4,
                    os1588_done:1,
                    eh_type:4;
        } overlay2;
        struct {                        /* EH_TYPE 2 */
            uint32  eh_queue_tag: 14,
                    classid:9,
                    classid_type:4,
                    os1588_done:1,
                    eh_type:4;
        } overlay3;
    } word16;

#else
                                        /* (word 3 in EP_TO_CPU_HDR) */
                                        /* word 2 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  l3_iif:14,                  /* L3 IIF */
            timestamp_type:2,           /* Timestamp type */
            vrf:12,                     /* VRF */
            hg_lookup_pkt:1,            /* Higig Lookup Packet */
            :2,                         /* Reserved */
            bpdu:1;                     /* BPDU Packet */

                                        /* (word 2 in EP_TO_CPU_HDR) */
                                        /* word 1 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  do_not_change_ttl:1,        /* Do not change TTL */
            replicated:1,               /* Replicated copy */
            l3only:1,                   /* L3 only IPMC packet */
            l3routed:3,                 /* Any IP routed packet */
            src_hg:1,                   /* Source is Higig */
            uc_switch_drop:1,           /* Unicast switched copies dropped */
            switch_pkt:1,               /* Switched packet */
            service_tag:1,              /* SD tag present */
            itag_action:2,              /* Ingress inner tag action */
            otag_action:2,              /* Ingress outer tag action */
            match_id_hi:18;             /* Packet MATCH_ID upper bits*/

                                        /* (word 1 in EP_TO_CPU_HDR) */
                                        /* word 0 in EP_TO_CPU_COMPOSITES_BUS*/
    uint32  match_id_lo;                /* Packet MATCH_ID lower bits*/

    union {                             /* (word 0 in EP_TO_CPU_HDR) */
        struct {                        /* EH_TYPE_0 */
            uint32  eh_type:4,
                    os1588_done:1,
                    :3,
                    eh_seg_sel:3,
                    eh_tag_type:2,
                    :3,
                    eh_queue_tag:16;
        } overlay1;
        struct {                        /* EH_TYPE_1 */
            uint32  eh_type:4,
                    os1588_done:1,
                    classid_type:4,
                    l3_iif:14,
                    classid: 9;
        } overlay2;
        struct {                        /* EH_TYPE 2 */
            uint32  eh_type:4,
                    os1588_done:1,
                    classid_type:4,
                    classid:9,
                    eh_queue_tag:14;
        } overlay3;
    } word16;
#endif

    uint32 rsvd;                        /* padding to get header to 16 words */
} ep_to_cpu_hdr36_t;

#define SOC_CPU_OPCODE_TYPE_IP_0        0
#define SOC_CPU_OPCODE_TYPE_IP_1        1
#define SOC_CPU_OPCODE_TYPE_EP          2
#define SOC_CPU_OPCODE_TYPE_NLF         3

#endif /* _SOC_SHARED_DCBFORMATS_TYPE36_H */
