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
 * This file implements definitions used by fa+ device driver 
 *
 */
#ifndef __FA2_IF_H__
#define __FA2_IF_H__

#define FA2_BCMHDR_OPCODE_0               0
#define FA2_BCMHDR_OPCODE_1               1
#define FA2_BCMHDR_OPCODE_2               2
#define FA2_BCMHDR_OPCODE_3               3

/* Broadcom header bit definitions */
#define FA2_BCMHDR_OPCODE                 0xE0000000
#define FA2_BCMHDR_OPCODE_SHIFT           29

/* Broadcom header opcode 0 bits */
#define FA2_BCMHDR_OP_0_RSVD              0x1F000000
#define FA2_BCMHDR_OP_0_RSVD_SHIFT        24

#define FA2_BCMHDR_OP_0_CLASS_ID          0x00FF0000
#define FA2_BCMHDR_OP_0_CLASS_ID_SHIFT    16

#define FA2_BCMHDR_OP_0_REASON_CODE       0x0000FF00
#define FA2_BCMHDR_OP_0_REASON_CODE_SHIFT 8

#define FA2_BCMHDR_OP_0_TC                0x000000E0
#define FA2_BCMHDR_OP_0_TC_SHIFT          5

#define FA2_BCMHDR_OP_0_SRC_PID           0x0000001F
#define FA2_BCMHDR_OP_0_SRC_PID_SHIFT     0

/* Broadcom header opcode 1 bits */
#define FA2_BCMHDR_OP_1_RSVD 0x1FFFFFC0
#define FA2_BCMHDR_OP_1_RSVD_SHIFT 6

#define FA2_BCMHDR_OP_1_TR 0x00000020
#define FA2_BCMHDR_OP_1_TR_SHIFT 5

#define FA2_BCMHDR_OP_1_TR_PID 0x0000001F
#define FA2_BCMHDR_OP_1_TR_PID_SHIFT 0

/* Next word (word1) in opcode #1 */
#define FA2_BCMHDR_OP_1_TIMESTAMP 0xFFFFFFFF
#define FA2_BCMHDR_OP_1_TIMESTAMP_SHIFT 0

/* Broadcom header opcode 2 bits */
#define FA2_BCMHDR_OP_2_ALL_BKTS_FULL 0x10000000
#define FA2_BCMHDR_OP_2_ALL_BKTS_FULL_SHIFT 28

#define FA2_BCMHDR_OP_2_NAPT_FLOW_ADDR 0x0FFF0000
#define FA2_BCMHDR_OP_2_NAPT_FLOW_ADDR_SHIFT 16

#define FA2_BCMHDR_OP_2_HDR_CHK_RESULT 0x0000FF00
#define FA2_BCMHDR_OP_2_HDR_CHK_RESULT_SHIFT 8

#define FA2_BCMHDR_OP_2_TC 0x000000E0
#define FA2_BCMHDR_OP_2_TC_SHIFT 5

#define FA2_BCMHDR_OP_2_SRC_PID 0x0000001F
#define FA2_BCMHDR_OP_2_SRC_PID_SHIFT 0

/* Broadcom header opcode 3 bits */
#define FA2_BCMHDR_OP_3_OPCODE 0xE0000000
#define FA2_BCMHDR_OP_3_OPCODE_SHIFT 29

#define FA2_BCMHDR_OP_3_RSVD 0x18000000
#define FA2_BCMHDR_OP_3_RSVD_SHIFT 27

#define FA2_BCMHDR_OP_3_ING_BH_OP 0x07000000
#define FA2_BCMHDR_OP_3_ING_BH_OP_SHIFT 24

#define FA2_BCMHDR_OP_3_CLASS_ID 0x00FF0000
#define FA2_BCMHDR_OP_3_CLASS_ID_SHIFT 16

#define FA2_BCMHDR_OP_3_REASON_CODE 0x0000FF00
#define FA2_BCMHDR_OP_3_REASON_CODE_SHIFT 8

#define FA2_BCMHDR_OP_3_TC 0x000000E0
#define FA2_BCMHDR_OP_3_TC_SHIFT 5

#define FA2_BCMHDR_OP_3_SRC_PID 0x0000001F
#define FA2_BCMHDR_OP_3_SRC_PID_SHIFT 0

/* Next word (word1) in opcode #3 */
#define FA2_BCMHDR_OP_3_W1_RSVD 0xF8000000
#define FA2_BCMHDR_OP_3_W1_RSVD_SHIFT 27

#define FA2_BCMHDR_OP_3_W1_TE 0x06000000
#define FA2_BCMHDR_OP_3_W1_TE_SHIFT 25

#define FA2_BCMHDR_OP_3_TS 0x01000000
#define FA2_BCMHDR_OP_3_TS_SHIFT 24

#define FA2_BCMHDR_OP_3_DST_MAP 0x00FFC000
#define FA2_BCMHDR_OP_3_DST_MAP_SHIFT 14

#define FA2_BCMHDR_OP_3_L4_OFFSET 0x00003FC0
#define FA2_BCMHDR_OP_3_L4_OFFSET_SHIFT 6

#define FA2_BCMHDR_OP_3_FWD_OP 0x00000030
#define FA2_BCMHDR_OP_3_FWD_OP_SHIFT 4

#define FA2_BCMHDR_OP_3_PROC_OP 0x0000000F
#define FA2_BCMHDR_OP_3_PROC_OP_SHIFT 0

/* Definitions used for flow entry fields maintained within the driver */
/* Used with word0, word1 fields in fa2_drv_flentry_t */

/* Flow entry's word0 bit defs */
#define FA2_DRV_VALID 0x80000000

#define FA2_DRV_VALID_SHIFT 31

#define FA2_DRV_REMAP_SA_IDX 0x70000000

#define FA2_DRV_REMAP_SA_IDX_SHIFT 28

#define FA2_DRV_NXT_HOP_IDX 0x0FE00000

#define FA2_DRV_NXT_HOP_IDX_SHIFT 21

#define FA2_DRV_MTU_IDX 0x001C0000

#define FA2_DRV_MTU_IDX_SHIFT 18

#define FA2_DRV_PPPOE_TUNL_IDX 0x0003C000

#define FA2_DRV_PPPOE_TUNL_IDX_SHIFT 14

#define FA2_DRV_PPPOE_TUNL_EN 0x00002000

#define FA2_DRV_PPPOE_TUNL_EN_SHIFT 13

#define FA2_DRV_DIRN 0x00001000

#define FA2_DRV_DIRN_SHIFT 12

#define FA2_DRV_REV_FLOW_PTR 0x00000FFF

#define FA2_DRV_REV_FLOW_PTR_SHIFT 0

/* Flow entry's word1 bit defs */
#define FA2_DRV_RCVD_SRC_PID 0x01F80000

#define FA2_DRV_RCVD_SRC_PID_SHIFT 19

#define FA2_DRV_CONN_ASSURED 0x00040000

#define FA2_DRV_CONN_ASSURED_SHIFT 18

#define FA2_DRV_REV_FL_VALID 0x00020000

#define FA2_DRV_REV_FL_VALID_SHIFT 17

#define FA2_DRV_IPV4_ENTRY 0x00010000

#define FA2_DRV_IPV4_ENTRY_SHIFT 16

#define FA2_DRV_FLOW_TIMER 0x0000FF00

#define FA2_DRV_FLOW_TIMER_SHIFT 8

#define FA2_DRV_FLOW_STATE 0x000000E0

#define FA2_DRV_FLOW_STATE_SHIFT 5

#define FA2_DRV_ACTION 0x0000001C

#define FA2_DRV_ACTION_SHIFT 2

#define FA2_DRV_DEST_DMA_CH 0x00000003

#define FA2_DRV_DEST_DMA_CH_SHIFT 0

#define FA2_ATOH(c) ((c) < 'a') ? ((c) - '0') : ((c) - 'a' + 10)

#define FA2_DRV_INVALID_SRC_PID 0x3F

/* Used as arg with ioctl from user space; used by driver as well */
#define FA2_GET_OPT_COUNTS               1
#define FA2_GET_OPT_CACHE_ENTRIES        2
#define FA2_GET_OPT_HW_ENTRIES           3
#define FA2_GET_OPT_CACHE_AND_HW_ENTRIES 4
#define FA2_GET_OPT_RTMAC                5
#define FA2_GET_OPT_NHT                  6

#define FA2_SUCCESS		0
#define FA2_ERROR		(-1)
#define FA2_ABORT		1 /* Problem with an internal function call. Caller uses
                           * it to abort further processing
                           */

#define FA2DRV_MAJOR	232
#define FA2DRV_NAME		"fa2"
#define FA2DRV_DEVICE_NAME	"/dev/" FA2DRV_NAME


#define FA2_VLAN_TAG1     0x81000000
#define FA2_VLAN_TAG2     0x91000000
#define FA2_VLAN_TAG_MASK 0xFFFF0000
#define FA2_VLAN_ID_MASK  0x0FFF
#define FA2_SNAP_ID1      0xAAAA
#define FA2_SNAP_ID1_MASK 0xFFFF
#define FA2_SNAP_ID2      0x03000000
#define FA2_TYPE_MASK     0xFFFF0000
#define FA2_TYPE_SHIFT    16

/* Valid vlan tags can be 0x8100 or 0x9100; so we check for both types */
#define FA2_IS_VLAN_TAGID(val) ((((val) & FA2_VLAN_TAG_MASK) ==\
                                  FA2_VLAN_TAG1) || (((val) &\
                                  FA2_VLAN_TAG_MASK) == FA2_VLAN_TAG2))

#define FA2_FL_INVALID   0
#define FA2_FL_IN_CACHE  1
#define FA2_FL_IN_HW     2

/* Default flow aging check timer */
#define FA2_FL_AGE_CHK_INT 10

/* Default values of flow timeouts used in reg 0x60 and flow entry */ 
#define FA2_TCP_EST_FL_DEF_TOUT 28
#define FA2_UDP_EST_FL_DEF_TOUT 21
#define FA2_TCP_FIN_FL_DEF_TOUT 4

#define FA2_NEW_FLOW  1
#define FA2_AGED_FLOW 2

/* Per flow information maintained by driver */
typedef struct fa2_drv_flentry {
    union {
        struct {
            uint32_t sip;
            uint32_t dip;
        } v4;

        struct {
            uint8_t sip[16];
            uint8_t dip[16];
        } v6;
    } ip;

    uint16_t sport;
    uint16_t dport;

    /* Word0 format:
     * 0:11  rev. flow ptr, 
     * 12    direction
     * 13    ppp0e enable
     * 14:17 ppp tunnel index
     * 18:20 mtu idx 
     * 21:27 next hop idx
     * 28:30 remap_da_idx
     * 31    valid (can be used to replace 'location' fld of ns)
     * 
     */
    uint32_t word0;

    /*
     * Word1 format:
     * 0:1 dest dma channel
     * 2:4 action
     * 5:7 flow state
     * 8:15 flow timer
     * 16:16 L3 proto (1 = ipv4, 0 = ipv6)
     * 17:17 1 = rev flow ptr in word0 is valid, 0 = rev flow not valid
     * 18:18 1 = stack saw traffic in both directions on this connection and 
     *           marked it assured, 0 = connection not yet assured
     * 19:24 source port id obtained from brcm header. 0x3F, if invalid. Defined
     *       6 bits to accomodate future expansion
     * 25:31 unused (may be used in future)
     */
    uint32_t word1;

    uint8_t protocol; /* 8-bit value in fa+ */

    //uint8_t valid; not needed
    uint8_t location; /* Track which flows are actually added to hardware.
                         Some flows like those for FTP control connections
                         are not added to hw since the stack needs to process
                         information within FTP packets (example, PORT
                         command) */

    uint8_t state;    /* This variable is used by aging logic to ignore eviction
                         if the flow was added in the same interval as this
                         polling cycle; it should be checked in later polling
                         cycles for aging.
                       */

    uint8_t entry_state; /* To avoid duplicate packets from coming in before
                          * entries get added to flow table
                          */

    uint16_t flid;     /* Index (location) of this flow in the cache */
	struct nf_conn		*ct;
	struct nf_conntrack	*nfct;
    
} fa2_flentry_t;

typedef enum fa2_ioctl {
	FA2CMDIOCTL_STATUS,
	FA2CMDIOCTL_RESET,
	FA2CMDIOCTL_INIT = 3,
	FA2CMDIOCTL_ENABLE,
	FA2CMDIOCTL_DISABLE,
	FA2CMDIOCTL_REGDUMP,
	FA2CMDIOCTL_DUMPPT,
	FA2CMDIOCTL_DUMPNHT,
	FA2CMDIOCTL_CFGLOG,
	FA2CMDIOCTL_ADDFE,
    FA2CMDIOCTL_ADDPT,
    FA2CMDIOCTL_ADDNHT,
	FA2CMDIOCTL_INVALID,
} fa2_ioctl_t;

enum {
    FA2_DBG_DEFAULT_LVL = 1,
    FA2_DBG_LOW_LVL = 2,
    FA2_DBG_MEDIUM_LVL = 3,
    FA2_DBG_HIGH_LVL = 4,
    FA2_DBG_PKT_DUMP = 5,
    FA2_DBG_MAX_LVL = 5
};

typedef struct {
	unsigned read_or_clear;
} fa2_status_t;

typedef struct {
	unsigned reset_flags;
} fa2_reset_t;

typedef struct {
	unsigned init_flags;
} fa2_init_t;

typedef struct {
	unsigned enable_flags;
} fa2_enable_t;

typedef struct {
	unsigned disable_flags;
} fa2_disable_t;

typedef struct {
	unsigned regdump_flags;
} fa2_regdump_t;

typedef struct {
	unsigned ptdump_flags;
} fa2_ptdump_t;

typedef struct {
	unsigned nhtdump_flags;
} fa2_nhtdump_t;

typedef struct {
	unsigned cfglog_flags;
} fa2_cfglog_t;

typedef struct {
	unsigned addfe_flags;
} fa2_addfe_t;

typedef struct {
	unsigned dummy;
} fa2_dummy_t;

/* Note the numbers below selected; check later for any conflicts */
#define FA2IOCTL_STATUS		_IOR('v', 179, fa2_status_t)
#define FA2IOCTL_RESET		_IOR('v', 180, fa2_reset_t)
#define FA2IOCTL_INIT		_IOR('v', 181, fa2_init_t)
#define FA2IOCTL_ENABLE		_IOR('v', 182, fa2_enable_t)
#define FA2IOCTL_DISABLE	_IOR('v', 183, fa2_disable_t)
#define FA2IOCTL_REGDUMP	_IOR('v', 184, fa2_regdump_t)
#define FA2IOCTL_DUMPPT		_IOR('v', 185, fa2_ptdump_t)
#define FA2IOCTL_DUMPNHT	_IOR('v', 186, fa2_nhtdump_t)
#define FA2IOCTL_CFGLOG 	_IOR('v', 187, fa2_cfglog_t)
#define FA2IOCTL_ADDFE  	_IOR('v', 188, fa2_addfe_t)
#define FA2IOCTL_ADDPT  	_IOR('v', 189, fa2_dummy_t) /* change later */
#define FA2IOCTL_ADDNHT  	_IOR('v', 190, fa2_dummy_t) /* change later */
#define FA2IOCTL_INVALID  	_IOR('v', 255, fa2_dummy_t)


extern void fa2_dump_flow_cache(int index, int in_hw);
extern int fa2_dbg_get_fltable(void);

extern int fa2_tx_to_spu(struct sk_buff *skb,
                         void (*cb)(unsigned char* data, size_t len, void *cb_data_ptr, unsigned cb_data_idx),
                         void *cb_data_ptr, unsigned cb_data_idx);

#endif /* __FA2_IF_H__ */
