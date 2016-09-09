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
 *
 */
/******************************************************************************/
/*                                                                            */
/* File Name : ctf_regs.h						      */
/*									      */
/* Defined interface for use by CTF configuration layer			      */
/*									      */
/******************************************************************************/
#ifndef __CTF_REGS_H_
#define __CTF_REGS_H_

#include <mach/iproc_regs.h>
#include <mach/io_map.h>

/*
 * CTF Control Register
 * Base Addr: 0x18027c00
 */
#define CTFCTL_REG_OFFSET					0x00
#define CTFCTL_CFG_SW_ACC_MODE					(1 << 0)
#define CTFCTL_CFG_BYPASS_CTF					(1 << 1)
#define CTFCTL_CFG_CRC_FWD					(1 << 2)
#define CTFCTL_CFG_CRC_OWRT					(1 << 3)
#define CTFCTL_CFG_HWQ_THRESHLD_MASK				(0x1FF << 4)
#define CTFCTL_CFG_NAPT_FLOW_INIT				(1 << 13)
#define CTFCTL_CFG_NEXT_HOP_INIT				(1 << 14)
#define CTFCTL_CFG_HWQ_INIT					(1 << 15)
#define CTFCTL_CFG_LAB_INIT					(1 << 16)
#define CTFCTL_CFG_HB_INIT					(1 << 17)
#define CTFCTL_CFG_DSBL_MAC_DA_CHECK				(1 << 18)

#define CTFCTL_HWQ_DEF_THRESHLD					0x140

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  13;  /* 31:19*/
			uint32_t disable_mac_da_check	:  1;   /* 18	*/
			uint32_t hb_init		:  1;   /* 17	*/
			uint32_t lab_init		:  1;   /* 16	*/
			uint32_t hwq_init		:  1;   /* 15	*/
			uint32_t next_hop_init          :  3;   /* 14	*/
			uint32_t napt_flow_init		:  1;   /* 13	*/
			uint32_t hwq_thresh		:  9;   /* 12:4	*/
			uint32_t crc_owrt_cfg		:  1;   /* 3	*/
			uint32_t crc_fwd_cfg		:  1;   /* 2	*/
			uint32_t bypass_ctf		:  1;   /* 1 	*/
			uint32_t accl_mode		:  1;   /* 0 	*/
		} f;
#else
		struct {
			uint32_t accl_mode		:  1;   /* 0 	*/
			uint32_t bypass_ctf		:  1;   /* 1 	*/
			uint32_t crc_fwd_cfg		:  1;   /* 2	*/
			uint32_t crc_owrt_cfg		:  1;   /* 3	*/
			uint32_t hwq_thresh		:  9;   /* 12:4	*/
			uint32_t napt_flow_init		:  1;   /* 13	*/
			uint32_t next_hop_init          :  3;   /* 14	*/
			uint32_t hwq_init		:  1;   /* 15	*/
			uint32_t lab_init		:  1;   /* 16	*/
			uint32_t hb_init		:  1;   /* 17	*/
			uint32_t disable_mac_da_check	:  1;   /* 18	*/
			uint32_t reserved		:  13;  /* 31:19*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_reg_t;

/*
 * CTF Memory Access Control Register
 * Base Addr: 0x18027c04
 */
#define CTFCTL_MEMACC_REG_OFFSET				0x04
#define CTFCTL_MEMACC_TAB_INDEX_MASK				(0x3FF << 0)
#define CTFCTL_MEMACC_TAB_SEL_MASK				(0x3 << 10)
#define CTFCTL_MEMACC_NAPT_FLOW_TAB				(~(3 << 10))
#define CTFCTL_MEMACC_NAPT_POOL_TAB				(1 << 10)
#define CTFCTL_MEMACC_NEXT_HOP_TAB				(2 << 10)
#define CTFCTL_MEMACC_RD_WR_N					(1 << 12)
#define CTFCTL_MEMACC_CUR_TBL_INDEX_MASK			(0x3FF << 13)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  9;   /* 31:19*/
			uint32_t cur_tab_index		:  10;  /* 22:13*/
			uint32_t rd_wr_n		:  1;   /* 12	*/
			uint32_t table_sel		:  2;   /* 10:11*/
			uint32_t table_index		:  10;  /* 9:0 	*/
		} f;
#else
		struct {
			uint32_t table_index		:  10;  /* 9:0 	*/
			uint32_t table_sel		:  2;   /* 10:11*/
			uint32_t rd_wr_n		:  1;   /* 12	*/
			uint32_t cur_tab_index		:  10;  /* 22:13*/
			uint32_t reserved		:  9;   /* 31:19*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_memacc_reg_t;

/*
 * CTF BRCM Header Control Register
 * Base Addr: 0x18027c08
 */
#define CTFCTL_BRCMHDR_REG_OFFSET				0x08
#define CTFCTL_BRCM_HDR_HW_EN					(1 << 0)
#define CTFCTL_BRCM_HDR_SW_RX_EN				(1 << 1)
#define CTFCTL_BRCM_HDR_SW_TX_EN				(1 << 2)
#define CTFCTL_BRCM_HDR_PARSE_IGN_EN				(1 << 3)
#define CTFCTL_BRCM_HDR_TE					(0x3 << 4)
#define CTFCTL_BRCM_HDR_TC					(0x7 << 6)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  23; /* 31:9	*/
			uint32_t brcm_hdr_tc		:  3;  /* 8:6	*/
			uint32_t brcm_hdr_te		:  2;  /* 5:4	*/
			uint32_t hdr_parse_ign_excep	:  1;  /* 3	*/
			uint32_t hdr_sw_tx_en		:  1;  /* 2 	*/
			uint32_t hdr_sw_rx_en		:  1;  /* 1 	*/
			uint32_t hdr_hw_en		:  1;  /* 0 	*/
		} f;
#else
		struct {
			uint32_t hdr_hw_en		:  1;  /* 0 	*/
			uint32_t hdr_sw_rx_en		:  1;  /* 1 	*/
			uint32_t hdr_sw_tx_en		:  1;  /* 2 	*/
			uint32_t hdr_parse_ign_excep	:  1;  /* 3	*/
			uint32_t brcm_hdr_te		:  2;  /* 5:4	*/
			uint32_t brcm_hdr_tc		:  3;  /* 8:6	*/
			uint32_t reserved		:  23; /* 31:9	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_brcmhdr_reg_t;

/*
 * CTF L2 Skip Control Register
 * Base Addr: 0x18027c0c
 */
#define CTFCTL_L2SKIP_REG_OFFSET				0x0C
#define CTFCTL_L2SKIP_ET_SKIP_TYPE_MASK				(0xFFFF << 0)
#define CTFCTL_L2SKIP_ET_SKIP_BYTES_MASK			(0x7 << 16)
#define CTFCTL_L2SKIP_ET_SKIP_ENABLE				(1 << 19)
#define CTFCTL_L2SKIP_ET_TO_SNAP_CONV				(1 << 20)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  11; /* 31:21	*/
			uint32_t eth2_to_snap_conv	:  1;  /* 20	*/
			uint32_t et_skp_en		:  1;  /* 19	*/
			uint32_t et_skip_bytes		:  3;  /* 18:16	*/
			uint32_t et_skip_type		:  16;  /* 15:0	*/
		} f;
#else
		struct {
			uint32_t et_skip_type		:  16;  /* 15:0	*/
			uint32_t et_skip_bytes		:  3;  /* 18:16	*/
			uint32_t et_skp_en		:  1;  /* 19	*/
			uint32_t eth2_to_snap_conv	:  1;  /* 20	*/
			uint32_t reserved		:  11; /* 31:21	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_l2skip_reg_t;

/*
 * CTF L2 Tag Type Register
 * Base Addr: 0x18027c10
 */
#define CTFCTL_L2TAG_REG_OFFSET					0x10
#define CTFCTL_L2TAG_ET_TAG_TYPE0_MASK				(0xFFFF << 0)
#define CTFCTL_L2TAG_ET_TAG_TYPE1_MASK				(0xFFFF << 16)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t et_tag_type1		:  16;  /* 31:16*/
			uint32_t et_tag_type0		:  16;  /* 15:0	*/
		} f;
#else
		struct {
			uint32_t et_tag_type0		:  16;  /* 15:0	*/
			uint32_t et_tag_type1		:  16;  /* 31:16*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_l2tag_reg_t;

/*
 * CTF L2 LLC Max Length Register
 * Base Addr: 0x18027c14
 */
#define CTFCTL_L2LLC_REG_OFFSET					0x14
#define CTFCTL_L2LLC_MAX_LENGTH_MASK				(0xFFFF << 0)
#define CTFCTL_LLC_MAX_LENGTH_DEF				0x5DC

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  16;  /* 31:16*/
			uint32_t llc_max_len		:  16;  /* 15:0	*/
		} f;
#else
		struct {
			uint32_t llc_max_len		:  16;  /* 15:0	*/
			uint32_t reserved		:  16;  /* 31:16*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_l2llc_maxlen_reg_t;

/*
 * CTF L2 Ether Type Register
 * Base Addr: 0x18027c14
 */
#define CTFCTL_L2ET_REG_OFFSET					0x20
#define CTFCTL_L2ET_IPV6					(0xFFFF << 16)
#define CTFCTL_L2ET_IPV4					(0xFFFF << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t l2et_ipv6		:  16;  /* 31:16*/
			uint32_t l2et_ipv4		:  16;  /* 15:0	*/
		} f;
#else
		struct {
			uint32_t l2et_ipv4		:  16;  /* 15:0	*/
			uint32_t l2et_ipv6		:  16;  /* 31:16*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_l2et_reg_t;

/*
 * CTF L3 IPv6 Type Register
 * Base Addr: 0x18027c24
 */
#define CTFCTL_L3IPV6_REG_OFFSET				0x24
#define CTFCTL_L3_IPV6_NEXT_HDR_TCP				(0xFF << 0)
#define CTFCTL_L3_IPV6_HDR_DEF_TCP				0x6
#define CTFCTL_L3_IPV6_NEXT_HDR_UDP				(0xFF << 8)
#define CTFCTL_L3_IPV6_HDR_DEF_UDP				0x11

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  16; /* 31:16	*/
			uint32_t ipv6_nxt_hdr_udp	:  8;  /* 15:8	*/
			uint32_t ipv6_nxt_hdr_tcp	:  8;  /* 7:0	*/
		} f;
#else
		struct {
			uint32_t ipv6_nxt_hdr_tcp	:  8;  /* 7:0	*/
			uint32_t ipv6_nxt_hdr_udp	:  8;  /* 15:8	*/
			uint32_t reserved		:  16; /* 31:16	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_l3ipv6_type_reg_t;

/*
 * CTF L3 IPv4 Type Register
 * Base Addr: 0x18027c28
 */
#define CTFCTL_L3IPV4_REG_OFFSET				0x28
#define CTFCTL_L3_IPV4_NEXT_HDR_TCP				(0xFF << 0)
#define CTFCTL_L3_IPV4_HDR_DEF_TCP				0x6
#define CTFCTL_L3_IPV4_NEXT_HDR_UDP				(0xFF << 8)
#define CTFCTL_L3_IPV4_HDR_DEF_UDP				0x11
#define CTFCTL_L3_IPV4_CKSUM_EN					(1 << 16)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  15; /* 31:17	*/
			uint32_t parse_cksum_en		:  1;  /* 16	*/
			uint32_t parse_proto_udp	:  8;  /* 15:8	*/
			uint32_t parse_proto_tcp	:  8;  /* 7:0	*/
		} f;
#else
		struct {
			uint32_t parse_proto_tcp	:  8;  /* 7:0	*/
			uint32_t parse_proto_udp	:  8;  /* 15:8	*/
			uint32_t parse_cksum_en		:  1;  /* 16	*/
			uint32_t reserved		:  15; /* 31:17	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_l3ipv4_type_reg_t;

/*
 * CTF L3 NAPT Control Register
 * Base Addr: 0x18027c2c
 */
#define CTFCTL_L3NAPT_REG_OFFSET				0x2C
#define CTFCTL_L3NAPT_HDR_DEC_TTL				(1 << 21)
#define CTFCTL_L3NAPT_HASH_SEL					(1 << 20)
#define CTFCTL_L3NAPT_HITS_CLR_ON_RD_EN				(1 << 19)
#define CTFCTL_L3NAPT_TIMESTAMP					(0x7 << 16)
#define CTFCTL_L3NAPT_HASH_SEED					(0xFFFF << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  10; /* 31:22	*/
			uint32_t dec_ttl		:  1;  /* 21	*/
			uint32_t hash_sel		:  1;  /* 20	*/
			uint32_t hits_clr_on_rd_en	:  1;  /* 19	*/
			uint32_t napt_timestamp		:  3;  /* 18:16	*/
			uint32_t napt_hash_seed		:  16; /* 15:0	*/
		} f;
#else
		struct {
			uint32_t napt_hash_seed		:  16; /* 15:0	*/
			uint32_t napt_timestamp		:  3;  /* 18:16	*/
			uint32_t hits_clr_on_rd_en	:  1;  /* 19	*/
			uint32_t hash_sel		:  1;  /* 20	*/
			uint32_t dec_ttl		:  1;  /* 21	*/
			uint32_t reserved		:  10; /* 31:22	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctfctl_l3napt_reg_t;

#define CTFCTL_L3NAPT_TIMESTAMP_NUM_BITS    3
#define CTFCTL_MAX_TIMESTAMP_VAL ((1 << CTFCTL_L3NAPT_TIMESTAMP_NUM_BITS) - 1)
#define CTFCTL_TIMESTAMP_MASK    ((1 << CTFCTL_L3NAPT_TIMESTAMP_NUM_BITS) - 1)
#define CTFCTL_TIMESTAMP_NUM_STATES (1 << 3)

/*
 * CTF Interrupt Status Register
 * Base Addr: 0x18027c30
 */
#define CTFCTL_INTSTAT_REG_OFFSET				0x30
#define CTF_INTSTAT_HB_INIT_DONE				(1 << 9)
#define CTF_INTSTAT_LAB_INIT_DONE				(1 << 8)
#define CTF_INTSTAT_HWQ_INIT_DONE				(1 << 7)
#define CTF_INTSTAT_NXT_HOP_INIT_DONE				(1 << 6)
#define CTF_INTSTAT_NAPT_FLOW_INIT_DONE				(1 << 5)
#define CTF_INTSTAT_BRCM_HDR_INIT_DONE				(1 << 4)
#define CTF_INTSTAT_IPV4_CKSUM_ERR				(1 << 3)
#define CTF_INTSTAT_L3_PARSE_INCOMP				(1 << 2)
#define CTF_INTSTAT_L2_PARSE_INCOMP				(1 << 1)
#define CTF_INTSTAT_BRCM_HDR_PARSE_INCOMP			(1 << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  22; /* 31:10	*/
			uint32_t hb_init_done		:  1;  /* 9	*/
			uint32_t lab_init_done		:  1;  /* 8	*/
			uint32_t hwq_init_done		:  1;  /* 7	*/
			uint32_t next_hop_init_done	:  1;  /* 6	*/
			uint32_t napt_flow_init_done	:  1;  /* 5	*/
			uint32_t brcm_hdr_invalid	:  1;  /* 4	*/
			uint32_t l3_ipv4_cksum_err	:  1;  /* 3	*/
			uint32_t l3_parse_incomp	:  1;  /* 2	*/
			uint32_t l2_parse_incomp	:  1;  /* 1	*/
			uint32_t brcm_hdr_parse_incomp	:  1;  /* 0	*/
		} f;
#else
		struct {
			uint32_t brcm_hdr_parse_incomp	:  1;  /* 0	*/
			uint32_t l2_parse_incomp	:  1;  /* 1	*/
			uint32_t l3_parse_incomp	:  1;  /* 2	*/
			uint32_t l3_ipv4_cksum_err	:  1;  /* 3	*/
			uint32_t brcm_hdr_invalid	:  1;  /* 4	*/
			uint32_t napt_flow_init_done	:  1;  /* 5	*/
			uint32_t next_hop_init_done	:  1;  /* 6	*/
			uint32_t hwq_init_done		:  1;  /* 7	*/
			uint32_t lab_init_done		:  1;  /* 8	*/
			uint32_t hb_init_done		:  1;  /* 9	*/
			uint32_t reserved		:  22; /* 31:10	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctf_intstat_reg_t;

/*
 * CTF Interrupt Status Mask Register
 * Base Addr: 0x18027c34
 */
#define CTFCTL_INTMASK_REG_OFFSET				0x34
#define CTF_INTMASK_HB_INIT_DONE				~(1 << 9)
#define CTF_INTMASK_LAB_INIT_DONE				~(1 << 8)
#define CTF_INTMASK_HWQ_INIT_DONE				~(1 << 7)
#define CTF_INTMASK_NXT_HOP_INIT_DONE				~(1 << 6)
#define CTF_INTMASK_NAPT_FLOW_INIT_DONE				~(1 << 5)
#define CTF_INTMASK_BRCM_HDR_INIT_DONE				~(1 << 4)
#define CTF_INTMASK_IPV4_CKSUM_ERR				~(1 << 3)
#define CTF_INTMASK_L3_PARSE_INCOMP				~(1 << 2)
#define CTF_INTMASK_L2_PARSE_INCOMP				~(1 << 1)
#define CTF_INTMASK_BRCM_HDR_PARSE_INCOMP			~(1 << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  22; /* 31:10	*/
			uint32_t hb_init_done		:  1;  /* 9	*/
			uint32_t lab_init_done		:  1;  /* 8	*/
			uint32_t hwq_init_done		:  1;  /* 7	*/
			uint32_t next_hop_init_done	:  1;  /* 6	*/
			uint32_t napt_flow_init_done	:  1;  /* 5	*/
			uint32_t brcm_hdr_invalid	:  1;  /* 4	*/
			uint32_t l3_ipv4_cksum_err	:  1;  /* 3	*/
			uint32_t l3_parse_incomp	:  1;  /* 2	*/
			uint32_t l2_parse_incomp	:  1;  /* 1	*/
			uint32_t brcm_hdr_parse_incomp	:  1;  /* 0	*/
		} f;
#else
		struct {
			uint32_t brcm_hdr_parse_incomp	:  1;  /* 0	*/
			uint32_t l2_parse_incomp	:  1;  /* 1	*/
			uint32_t l3_parse_incomp	:  1;  /* 2	*/
			uint32_t l3_ipv4_cksum_err	:  1;  /* 3	*/
			uint32_t brcm_hdr_invalid	:  1;  /* 4	*/
			uint32_t napt_flow_init_done	:  1;  /* 5	*/
			uint32_t next_hop_init_done	:  1;  /* 6	*/
			uint32_t hwq_init_done		:  1;  /* 7	*/
			uint32_t lab_init_done		:  1;  /* 8	*/
			uint32_t hb_init_done		:  1;  /* 9	*/
			uint32_t reserved		:  22; /* 31:10	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctf_intmask_reg_t;

/*
 * CTF Receive Status Mask Register
 * Base Addr: 0x18027c38
 */
#define CTF_RXMASK_REG_OFFSET					0x38
#define CTF_RXMASK_L3PROTO_EXT_FAIL				~(1 << 7)
#define CTF_RXMASK_L3PROTO_IPV4_HDR_LEN_FAIL			~(1 << 6)
#define CTF_RXMASK_L3PROTO_IPV4_OPT_FAIL			~(1 << 5)
#define CTF_RXMASK_L3PROTO_IPV4_CKSUM_FAIL			~(1 << 4)
#define CTF_RXMASK_L3PROTO_FRAG_FAIL				~(1 << 3)
#define CTF_RXMASK_L3PROTO_VER_FAIL				~(1 << 2)
#define CTF_RXMASK_L3PROTO_L2ETYPE_FAIL				~(1 << 1)
#define CTF_RXMASK_L3PROTO_L2SNAP_FAIL				~(1 << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  24; /* 31:8	*/
			uint32_t ext_fail		:  1;  /* 7	*/
			uint32_t ipv4_hdr_len_fail	:  1;  /* 6	*/
			uint32_t ipv4_opt_fail		:  1;  /* 5	*/
			uint32_t ipv4_cksum_fail	:  1;  /* 4	*/
			uint32_t l3_frag_fail		:  1;  /* 3	*/
			uint32_t l3_ver_fail		:  1;  /* 2	*/
			uint32_t l2_etype_fail		:  1;  /* 1	*/
			uint32_t l2_snap_fail		:  1;  /* 0	*/
		} f;
#else
		struct {
			uint32_t l2_snap_fail		:  1;  /* 0	*/
			uint32_t l2_etype_fail		:  1;  /* 1	*/
			uint32_t l3_ver_fail		:  1;  /* 2	*/
			uint32_t l3_frag_fail		:  1;  /* 3	*/
			uint32_t ipv4_cksum_fail	:  1;  /* 4	*/
			uint32_t ipv4_opt_fail		:  1;  /* 5	*/
			uint32_t ipv4_hdr_len_fail	:  1;  /* 6	*/
			uint32_t ext_fail		:  1;  /* 7	*/
			uint32_t reserved		:  24; /* 31:8	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctf_rxstatmask_reg_t;

/*
 * CTF Hit Count Register
 * Base Addr: 0x18027c3c
 */

#define CTF_HITCOUNT_REG_OFFSET					0x3C
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_hitcount_reg_t;

/*
 * CTF Miss Count Register
 * Base Addr: 0x18027c40
 */

#define CTF_MISSCOUNT_REG_OFFSET				0x40
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_misscount_reg_t;

/*
 * CTF SNAP Fail Count Register
 * Base Addr: 0x18027c44
 */

#define CTF_SNAPFAILCOUNT_REG_OFFSET				0x44
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_snapfailcount_reg_t;

/*
 * CTF EType Fail Count Register
 * Base Addr: 0x18027c48
 */

#define CTF_ETYPEFAILCOUNT_REG_OFFSET				0x48
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_etypefailcount_reg_t;

/*
 * CTF Version Fail Count Register
 * Base Addr: 0x18027c4c
 */

#define CTF_VERFAILCOUNT_REG_OFFSET				0x4C
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_verfailcount_reg_t;

/*
 * CTF Frag Fail Count Register
 * Base Addr: 0x18027c50
 */

#define CTF_FRAGFAILCOUNT_REG_OFFSET				0x50
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_fragfailcount_reg_t;

/*
 * CTF Protocol Ext Fail Count Register
 * Base Addr: 0x18027c54
 */

#define CTF_PROTEXTFAILCOUNT_REG_OFFSET				0x54
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_protoextfailcount_reg_t;

/*
 * CTF IPv4 Checksum Fail Count Register
 * Base Addr: 0x18027c58
 */

#define CTF_IPV4CKSUMFAILCOUNT_REG_OFFSET			0x58
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_ipv4cksumfailcount_reg_t;

/*
 * CTF IPv4 Options Fail Count Register
 * Base Addr: 0x18027c5c
 */

#define CTF_IPV4OPTFAILCOUNT_REG_OFFSET				0x5C
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_ipv4optfailcount_reg_t;

/*
 * CTF IPv4 Header Length Fail Count Register
 * Base Addr: 0x18027c60
 */

#define CTF_IPV4HDRLENFAILCOUNT_REG_OFFSET			0x60
typedef struct {
	uint32_t count;
} __attribute__((packed)) ctf_ipv4hdrlenfailcount_reg_t;

/*
 * CTF Error Status Register
 * Base Addr: 0x18027c64
 */
#define CTF_ERR_STATUS_REG_OFFSET				0x64
#define CTF_ERR_HWQ_OVFLOW					(1 << 8)
#define CTF_ERR_HB_OVFLOW					(1 << 7)
#define CTF_ERR_RXQ_OVFLOW					(1 << 6)
#define CTF_ERR_SOP_EOP						(1 << 5)
#define CTF_ERR_SPB_OVFLOW					(1 << 4)
#define CTF_ERR_LAB_OVFLOW					(1 << 3)
#define CTF_ERR_INT_MERGE					(1 << 2)
#define CTF_ERR_TXQ_OVFLOW					(1 << 1)
#define CTF_ERR_RB_OVFLOW					(1 << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  23; /* 31:9	*/
			uint32_t hwq_ovflow_err		:  1;  /* 8	*/
			uint32_t hb_ovflow_err		:  1;  /* 7	*/
			uint32_t rxq_ovflow_err		:  1;  /* 6	*/
			uint32_t sop_eop_err		:  1;  /* 5	*/
			uint32_t spb_ovflow_err		:  1;  /* 4	*/
			uint32_t lab_ovflow_err		:  1;  /* 3	*/
			uint32_t int_merge_err		:  1;  /* 2	*/
			uint32_t txq_ovflow_err		:  1;  /* 1	*/
			uint32_t rb_ovflow_err		:  1;  /* 0	*/
		} f;
#else
		struct {
			uint32_t rb_ovflow_err		:  1;  /* 0	*/
			uint32_t txq_ovflow_err		:  1;  /* 1	*/
			uint32_t int_merge_err		:  1;  /* 2	*/
			uint32_t lab_ovflow_err		:  1;  /* 3	*/
			uint32_t spb_ovflow_err		:  1;  /* 4	*/
			uint32_t sop_eop_err		:  1;  /* 5	*/
			uint32_t rxq_ovflow_err		:  1;  /* 6	*/
			uint32_t hb_ovflow_err		:  1;  /* 7	*/
			uint32_t hwq_ovflow_err		:  1;  /* 8	*/
			uint32_t reserved		:  23; /* 31:9	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctf_errstat_reg_t;

/*
 * CTF Error Status Mask Register
 * Base Addr: 0x18027c68
 */
#define CTF_ERR_STATUS_MASK_REG_OFFSET				0x68
#define CTF_ERR_HWQ_OVFLOW_MASK					~(1 << 8)
#define CTF_ERR_HB_OVFLOW_MASK					~(1 << 7)
#define CTF_ERR_RXQ_OVFLOW_MASK					~(1 << 6)
#define CTF_ERR_SOP_EOP_MASK					~(1 << 5)
#define CTF_ERR_SPB_OVFLOW_MASK					~(1 << 4)
#define CTF_ERR_LAB_OVFLOW_MASK					~(1 << 3)
#define CTF_ERR_INT_MERGE_MASK					~(1 << 2)
#define CTF_ERR_TXQ_OVFLOW_MASK					~(1 << 1)
#define CTF_ERR_RB_OVFLOW_MASK					~(1 << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  23; /* 31:9	*/
			uint32_t hwq_ovflow_err		:  1;  /* 8	*/
			uint32_t hb_ovflow_err		:  1;  /* 7	*/
			uint32_t rxq_ovflow_err		:  1;  /* 6	*/
			uint32_t sop_eop_err		:  1;  /* 5	*/
			uint32_t spb_ovflow_err		:  1;  /* 4	*/
			uint32_t lab_ovflow_err		:  1;  /* 3	*/
			uint32_t int_merge_err		:  1;  /* 2	*/
			uint32_t txq_ovflow_err		:  1;  /* 1	*/
			uint32_t rb_ovflow_err		:  1;  /* 0	*/
		} f;
#else
		struct {
			uint32_t rb_ovflow_err		:  1;  /* 0	*/
			uint32_t txq_ovflow_err		:  1;  /* 1	*/
			uint32_t int_merge_err		:  1;  /* 2	*/
			uint32_t lab_ovflow_err		:  1;  /* 3	*/
			uint32_t spb_ovflow_err		:  1;  /* 4	*/
			uint32_t sop_eop_err		:  1;  /* 5	*/
			uint32_t rxq_ovflow_err		:  1;  /* 6	*/
			uint32_t hb_ovflow_err		:  1;  /* 7	*/
			uint32_t hwq_ovflow_err		:  1;  /* 8	*/
			uint32_t reserved		:  23; /* 31:9	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctf_errstatmask_reg_t;

/*
 * CTF Debug Control Register
 * Base Addr: 0x18027c6c
 */
#define CTF_DBG_CTL_REG_OFFSET					0x6C
#define CTF_DBG_OK_TO_SEND					(0xF << 6)
#define CTF_DBG_FORCE_ALL_HIT					(1 << 2)
#define CTF_DBG_FORCE_ALL_MISS					(1 << 1)
#define CTF_DBG_REG						(1 << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  25; /* 31:7	*/
			uint32_t ok_to_send		:  1;  /* 6:3	*/
			uint32_t force_all_hit		:  1;  /* 2	*/
			uint32_t force_all_miss		:  1;  /* 1	*/
			uint32_t reg_dbg		:  1;  /* 0	*/
		} f;
#else
		struct {
			uint32_t reg_dbg		:  1;  /* 0	*/
			uint32_t force_all_miss		:  1;  /* 1	*/
			uint32_t force_all_hit		:  1;  /* 2	*/
			uint32_t ok_to_send		:  1;  /* 6:3	*/
			uint32_t reserved		:  25; /* 31:7	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctf_dbgctl_reg_t;

/*
 * CTF Debug Control Register
 * Base Addr: 0x18027c70
 */
#define CTF_DBG_MEMACC_REG_OFFSET				0x70
#define CTF_DBG_MEM_ACC_BUSY					(1 << 0)

typedef struct {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t reserved		:  31; /* 31:1	*/
			uint32_t mem_acc_busy		:  1;  /* 0	*/
		} f;
#else
		struct {
			uint32_t mem_acc_busy		:  1;  /* 0	*/
			uint32_t reserved		:  31; /* 31:1	*/
		} f;
#endif
		uint32_t word;
	};
} __attribute__((packed)) ctf_dbgstat_reg_t;

/*
 * CTF Memory Access Data Register(s)
 * Base Addr: 0x18027ca0
 */
#define CTF_MEM_ACC_DATA0_REG_OFFSET			0xA0
#define CTF_MEM_ACC_DATA1_REG_OFFSET			0xA4
#define CTF_MEM_ACC_DATA2_REG_OFFSET			0xA8
#define CTF_MEM_ACC_DATA3_REG_OFFSET			0xAC
#define CTF_MEM_ACC_DATA4_REG_OFFSET			0xB0
#define CTF_MEM_ACC_DATA5_REG_OFFSET			0xB4
#define CTF_MEM_ACC_DATA6_REG_OFFSET			0xB8
#define CTF_MEM_ACC_DATA7_REG_OFFSET			0xBC
#define CTF_DATA_SIZE					8
#define CTF_MAX_POOL_TABLE_INDEX			4
#define CTF_MAX_NEXTHOP_TABLE_INDEX			128

typedef struct {
	uint32_t data[CTF_DATA_SIZE];
} __attribute__((packed)) ctf_data_reg_t;

#endif  /* defined(__CTF_REGS_H_) */
