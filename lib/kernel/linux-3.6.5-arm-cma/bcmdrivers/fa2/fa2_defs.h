/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This file consists of common definitions which are shared by modules using
 * fa+ driver
 *
 */
#ifndef __FA2_DEFS_H__
#define __FA2_DEFS_H__

#define FA2_VERSION		"0.1"
#define FA2_VER_STR		"v" FA2_VERSION " " __DATE__ " " __TIME__
#define FA2_MODNAME		"Broadcom Flow Accelerator Plus (Experimental)"

#define FA2_NAME		"FA2"

#define FA2_RTMACTBL_SIZE     8
#define FA2_MTUTBL_SIZE       8
#define FA2_TUNLTBL_SIZE     16
#define FA2_PORTYPTBL_SIZE   32
#define FA2_NEXTHOPTBL_SIZE 128
#define FA2_FLOWTBL_SIZE   4096

/* Width in 32-bit words */
#define FA2_MTUTBL_ENTRY_SIZE     1
#define FA2_PORTYPTBL_ENTRY_SIZE  1
#define FA2_RTMACTBL_ENTRY_SIZE   2
#define FA2_TUNLTBL_ENTRY_SIZE    3
#define FA2_NEXTHOPTBL_ENTRY_SIZE 3
#define FA2_FLOWTBL_ENTRY_SIZE    7
#define FA2_INVALID_ENTRY_SIZE    255

#define FA2_SUCCESS		0
#define FA2_ERROR		(-1)
#define FA2_ABORT		1 /* Problem with an internal function call. Caller uses
                           * it to abort further processing
                           */

#define FA2_MAC_ADDR_SIZE 6


typedef enum {
	FA2_PKT_DONE,
	FA2_PKT_NORM,
} fa2_action_t;

#define FA2_MAX_TBL_ENT_WIDTH 7

#define FA2_INVALID_MTU_VAL 0xFFFF
#define FA2_GETBITS(val, mask, start_bit) (((val) & (mask)) >> (start_bit))
#define FA2_SETBITS(val, fld_val, mask, start_bit) val = \
                                                      ((val) & ~(mask)) | \
                                                      ((fld_val) << (start_bit))

/* skb stores msbyte at location 0 of a mac addr. The h/w indirect access
 * needs msbyte in highest location (example, reg data0 stores bits 31-0,
 * reg data1 stores bits 63-32). So we swap the data bytes before passing to h/w
 */
#define FA2_SWAP_MAC_FOR_HW(to, from)       to[0] = from[5]; \
                                            to[1] = from[4]; \
                                            to[2] = from[3]; \
                                            to[3] = from[2]; \
                                            to[4] = from[1]; \
                                            to[5] = from[0];

/* Used for 'pkt_type' field in struct fa2_pkt_info */
#define FA2_FWD_PKT      1
#define FA2_LOCAL_TX_PKT 2
#if (defined(CONFIG_IPROC_FA2) && defined(CONFIG_IPROC_FA2_CS_OFFLOAD_SMALL_PKT_WA))
#define FA2_LOCAL_SMALL_TX_PKT 3
#endif
#define FA2_INVALID_PKT 255

/* Used for 'proto' in 'fa2_pkt_info' */
#define FA2_PROTO_NOT_SUPPORTED 255

/* Mask for the bit indicating whether a SPU transaction belongs to this driver */
#define FA2_SPU_TRNS_ID_DRIVER_MASK (0x8000)
/* Mask for the bits indicating the packet index */
#define FA2_SPU_TRNS_ID_INDEX_MASK (0x7fff)

/* Enum used by driver. Can be used by other modules. Do not change enum
 * values
 */
enum fa2_tbl_num {
    FA2_FLTBL_V6_W1 = 1, /* IPv6 flow table - word1 */
    FA2_FLTBL_V6_W0, /* IPv6 flow table - word0 */
    FA2_FLTBL_V4, /* IPv4 flow table */
    FA2_MTUTBL, /* MTU table */
    FA2_PORTTYPTBL, /* Port type table */
    FA2_NXTHOPTBL, /* Router MAC table */
    FA2_RTMACTBL, /* Tunnel table */
    FA2_TUNLTBL, /* Tunnel table */
    FA2_INVALIDTBL = 255 /* Invalid value for table number */
};

/* Used for h/w table access. Values of enums match hardware's definitions.
 * Do not modify values
 */
enum fa2_tbl_op {
    FA2_TBL_WR = 0,
    FA2_TBL_RD = 1,
    FA2_TBL_INVAL_OP = 0xFF
};

struct fa2_rtmac_tbl_entry {
    uint8_t host;
    uint8_t l4_checksum_check;
    uint8_t external;
    uint8_t mac_addr[6]; /* remapped smac address */
};

struct fa2_tunnel_tbl_entry {
    uint8_t action;
    uint8_t dmac_idx;
    uint8_t smac[6];
    uint16_t session_id;
};

struct fa2_nh_tbl_entry {
    uint16_t vlan;
    uint8_t op;
    uint8_t l2_frmtype;
    uint8_t da[6]; /* Dest mac adddress */
};

struct fa2_flow_entry_ipv4 {
    uint8_t ipv4_key_type; /* 1 bit */
    uint8_t valid; /* 1 bit */
    uint32_t sip;
    uint32_t dip;
    uint8_t protocol; /* 8 bits */
    uint16_t sport;
    uint16_t dport;
    uint16_t rev_flow_ptr; /* 12 bits */
    uint8_t brcm_tag_opcode; /* 3 bits */
    uint8_t brcm_tag_tc; /* 3 bits */
    uint8_t brcm_tag_te; /* 2 bits */
    uint8_t brcm_tag_ts; /* 1 bit */
    uint16_t brcm_tag_destmap; /* 10 bits */
    uint8_t direction;
    uint8_t l4_chksum_chk;
    uint8_t ppp_tunnel_en;
    uint8_t ppp_tunnel_idx; /* 4 bits */
    uint8_t mtu_idx; /* 3 bits */
    uint8_t next_hop_idx; /* 7 bits */
    uint8_t remap_sa_idx; /* 3 bits */
    uint8_t dest_dma_chan; /* 2 bits */
    uint8_t action; /* 3 bits */
    uint32_t hits;
    uint8_t tcp_fin; /* 1 bit */
    uint8_t tcp_rst; /* 1 bit */
    uint8_t tcp_ack_after_close; /* 1 bit */
    uint8_t hit_after_close; /* 1 bit */
    uint8_t flow_state; /* 3 bits */
    uint8_t flow_timer; /* 8 bits */
};

struct fa2_flow_entry_ipv6_w0 {
    uint8_t ipv4_key_type; /* 1 bit */
    uint8_t valid; /* 1 bit */
    uint8_t dip_lower[8]; /* Lower 64 bits of ipv6 addr */
    uint8_t protocol; /* 8 bits */
    uint16_t sport;
    uint16_t dport;
    uint16_t rev_flow_ptr; /* 12 bits */
    uint8_t brcm_tag_opcode; /* 3 bits */
    uint8_t brcm_tag_tc; /* 3 bits */
    uint8_t brcm_tag_te; /* 2 bits */
    uint8_t brcm_tag_ts; /* 1 bit */
    uint16_t brcm_tag_destmap; /* 10 bits */
    uint8_t direction;
    uint8_t l4_chksum_chk;
    uint8_t ppp_tunnel_en;
    uint8_t ppp_tunnel_idx; /* 4 bits */
    uint8_t mtu_idx; /* 3 bits */
    uint8_t next_hop_idx; /* 7 bits */
    uint8_t remap_sa_idx; /* 3 bits */
    uint8_t dest_dma_chan; /* 2 bits */
    uint8_t action; /* 3 bits */
    uint32_t hits;
    uint8_t tcp_fin; /* 1 bit */
    uint8_t tcp_rst; /* 1 bit */
    uint8_t tcp_ack_after_close; /* 1 bit */
    uint8_t hit_after_close; /* 1 bit */
    uint8_t flow_state; /* 3 bits */
    uint8_t flow_timer; /* 8 bits */
};

struct fa2_flow_entry_ipv6_w1 {
    uint8_t ipv4_key_type;
    uint8_t valid;
    uint8_t sip[16];
    uint8_t dip_upper[8]; /* Upper 64 bits of ipv6 addr */
};

struct fa2_bcm_hdr3_info {
    uint8_t op;
    uint8_t ing_bh_op;
    uint8_t class_id;
    uint8_t reason_code;
    uint8_t tc;
    uint8_t src_pid;
    uint8_t te;
    uint8_t ts;
    uint8_t l4_off;
    uint8_t proc_op;
    uint8_t fwd_op;
    uint8_t rsvd1;
    uint16_t dst_map;
    uint16_t rsvd2;
};

struct fa2_pkt_info {
    struct ethhdr *mac;
    void *ipv4_or_ipv6;
    void *tcp_or_udp;
    uint32_t vlan_tag;
    uint32_t vlan_tag_next;
    uint32_t et_type; /*L3 protocol. example, ipv4(0x800) or ipv6(0x86DD) */
    uint8_t proto; /* If valid, specifies L4 protocol, TCP(1) or UDP(0).
                      Else invalid value (255) */
    uint8_t eth_snapllc;
    uint8_t need_hdr_bytes;
#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    uint8_t pkt_type;
#else
    uint8_t rsvd;
#endif
    uint32_t hdr_words[2];
};

extern volatile void __iomem *fa2_virt_addr;

extern int  fa2_load_init(void);
extern void fa2_unload_exit(void);

extern int fa2_indirect_write(enum fa2_tbl_num t, int index, uint32_t *val);
extern int fa2_set_tbl_entry(enum fa2_tbl_num t, int index, void *tbl_data);
extern int fa2_indirect_read(enum fa2_tbl_num t, int index, uint32_t *val);
extern int fa2_get_tbl_entry(enum fa2_tbl_num t, int index, void *tbl_data);
extern void fa2_display_rtmac_entry(struct fa2_rtmac_tbl_entry *rtmp);
extern void fa2_display_nh_entry(struct fa2_nh_tbl_entry *nhp);
extern void fa2_display_tunltbl_entry(struct fa2_tunnel_tbl_entry *ttp);
extern int fa2_add_rtmac_entry(struct fa2_rtmac_tbl_entry *rtm, int *tbl_idx,
                        uint8_t dir_write);
extern int fa2_add_nh_entry(struct fa2_nh_tbl_entry *nh, int *tbl_idx,
                     uint8_t dir_write);
extern int fa2_search_tunl_tbl(uint8_t *ma, uint16_t sid);
extern int fa2_add_tunl_entry(struct fa2_tunnel_tbl_entry *tt, int *tbl_idx,
                       uint8_t dir_write);
extern int fa2_del_tunl_entry(uint8_t *smac, uint16_t session_id, int index);
extern uint8_t fa2_dbg_log_lvl;
extern int fa2_set_mtu_entry(uint32_t val, int index);
extern int fa2_set_port_type_entry(uint8_t val, int index, int op);

#endif /* __FA2_DEFS_H__ */
