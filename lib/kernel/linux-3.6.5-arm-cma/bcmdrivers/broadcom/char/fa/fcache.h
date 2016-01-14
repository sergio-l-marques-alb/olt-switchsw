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
/* File Name : fcache.h							      */
/* Description: This file implements the Broadcom Flow Cache.		      */
/*                                                                            */
/* Flow Cache:								      */
/* ----------------------                                                     */
/*                                                                            */
/* For details of the design please refer:                                    */
/*      BCM53010 iProc FA Driver Software Design 0.3                          */
/******************************************************************************/
#ifndef __FCACHE_H
#define __FCACHE_H
#include <linux/if_ether.h>

/* Flow cache table is 1024X256 */
#define FCACHE_HTABLE_SIZE		256  /* Hash Table */
#define FCACHE_MAX_ENTRIES		1024 /* Max. table entries 256 * 4 */
#define FCACHE_MAX_BKT_ENTRIES		4

#define FCACHE_JHASH_RAND		0xAB0ADCEE

/* Flow cache system periodic timer */
#define FCACHE_REFRESH			30 /* 30-second timer */
#define FCACHE_REFRESH_INTERVAL		FCACHE_REFRESH


/* Refresh based on layer-4 protocol */
#define FCACHE_REFRESH_TCP		60
#define FCACHE_REFRESH_UDP		180
#define FCACHE_REFRESH_TCP_INTERVAL	FCACHE_REFRESH_TCP
#define FCACHE_REFRESH_UDP_INTERVAL	FCACHE_REFRESH_UDP

/* Flow cache hash table IP-Tuple lookup result */
#define FCACHE_MISS			0
#define FCACHE_HIT			1

/* Functional interface return status */
#define FCACHE_ERROR			(-1)
#define FCACHE_SUCCESS			0

#define FLOW_NULL			((napt_flow_t *)NULL)
#define FCACHE_FA_OPCODE		2 /* b'010x */
#define FCACHE_SA_OPCODE		0 /* b'000x */
#define FCACHE_OPCODE_MASK		(7 << 29)

#define NAPT_FLOW_TABLE			0
#define NAPT_POOL_TABLE			1
#define NEXT_HOP_TABLE			2
#define CTF_ALL_TABLES			3

#define MAC_ADDR_LEN			6
#define BRCM_TAG_LEN			4

#define FCACHE_INVAL_INDX 0xFFFF

#define IN_FLOW_TABLE           1
#define IN_FCACHE               0

#define FC_NOT_NEW_FLOW         0
#define FC_NEW_FLOW             1
#define FC_AGED_FLOW            2

/* To check if a flow was checked for aging by its peer */ 
#define FC_NUM_LINE_BITS        5
#define FC_FLOWS_PER_LINE       32
#define FC_LINE_MASK            0x1F

#define BCM_FA_VLAN_TAG1     0x81000000
#define BCM_FA_VLAN_TAG2     0x91000000
#define BCM_FA_VLAN_TAG_MASK 0xFFFF0000
#define BCM_FA_VLAN_ID_MASK  0x0FFF
#define BCM_FA_SNAP_ID1      0xAAAA
#define BCM_FA_SNAP_ID1_MASK 0xFFFF
#define BCM_FA_SNAP_ID2      0x03000000
#define BCM_FA_TYPE_MASK     0xFFFF0000
#define BCM_FA_TYPE_SHIFT    16

/* Valid vlan tags can be 0x8100 or 0x9100; so we check for both types */
#define BCM_FA_IS_VLAN_TAGID(val) ((((val) & BCM_FA_VLAN_TAG_MASK) ==\
                                   BCM_FA_VLAN_TAG1) || (((val) &\
                                   BCM_FA_VLAN_TAG_MASK) == BCM_FA_VLAN_TAG2))
/*
 * IP Flow learning status
 */
extern void fc_status(void);

/*
 * Flush all learnt entries in flow cache
 */
extern void  fc_flush(void);

struct bcmhdr {
	union {
#ifdef BIG_ENDIAN
		struct {
			uint32_t	op_code		:3; /* 31:29 */
			uint32_t	reserved	:5; /* 28:24 */
			uint32_t	cl_id		:8; /* 23:16 */
			uint32_t	reason_code	:8; /* 15:8  */
			uint32_t	tc		:3; /* 7:5   */
			uint32_t	src_pid		:5; /* 4:0   */
		} oc0;
		struct {
			uint32_t	op_code		:3; /* 31:29 */
			uint32_t	reserved	:2; /* 28:27 */
			uint32_t	all_bkts_full	:1; /* 26    */
			uint32_t	bkt_id		:2; /* 25:24 */
			uint32_t	napt_flow_id	:8; /* 23:16 */
			uint32_t	hdr_chk_result	:8; /* 15:8  */
			uint32_t	tc		:3; /* 7:5   */
			uint32_t	src_pid		:5; /* 4:0   */
		} oc1;
#else
		struct {
			uint32_t	src_pid		:5; /* 4:0   */
			uint32_t	tc		:3; /* 7:5   */
			uint32_t	reason_code	:8; /* 15:8  */
			uint32_t	cl_id		:8; /* 23:16 */
			uint32_t	reserved	:5; /* 28:24 */
			uint32_t	op_code		:3; /* 31:29 */
		} oc0;
		struct {
			uint32_t	src_pid		:5; /* 4:0   */
			uint32_t	tc		:3; /* 7:5   */
			uint32_t	hdr_chk_result	:8; /* 15:8  */
			uint32_t	napt_flow_id	:8; /* 23:16 */
			uint32_t	bkt_id		:2; /* 25:24 */
			uint32_t	all_bkts_full	:1; /* 26    */
			uint32_t	reserved	:2; /* 28:27 */
			uint32_t	op_code		:3; /* 31:29 */
		} oc1;
#endif
		uint32_t word;
	};
};

struct bcm_ethhdr {
	unsigned char	h_dest[MAC_ADDR_LEN];
	unsigned char	h_source[MAC_ADDR_LEN];
	struct bcmhdr	bcm;
	uint16_t	h_proto;
};

struct ethbcmhdr {
	struct bcmhdr	bcm;
	struct ethhdr	eth;
};

/*
 * Flow Cache Entry Key:
 */
typedef struct ipv4 {
	uint32_t	l3_sip	: 32;
	uint32_t	l3_dip	: 32;
	uint32_t	l3_proto: 1;
	uint32_t	l4_sport: 16;
	uint32_t	l4_dport: 16;
} ipv4_t;

typedef struct ipv6 {
	uint8_t		l3_sip[16];
	uint8_t		l3_dip[16];
	uint32_t	l3_proto: 1;
	uint32_t	l4_sport: 16;
	uint32_t	l4_dport: 16;
} ipv6_t;

typedef struct {
	uint32_t	hash_index	: 8;
	uint32_t	bkt_index	: 2;
	
	union {
		ipv4_t	ipv4;
		ipv6_t	ipv6;
		uint32_t word;
	}f;
} napt_flow_key_t;

/*
 * Flow Cache IPv4 Table Entry
 */
//# pragma pack(push, 2 )
typedef struct flow_ipv4_t {
#ifdef BIG_ENDIAN
	uint32_t	ipv4_entry	:1; /* 255:255 */
	uint32_t	reserved_1	:10;/* 254:246 */
	uint32_t	reserved_0;	    /* 245:213 */
	uint32_t	valid		:1; /* 212:212 */
	uint32_t	tag_oc		:3; /* 211:209 */
	uint32_t	tag_tc		:3; /* 208:206 */
	uint32_t	tag_te		:2; /* 205:204 */
	uint32_t	tag_ts		:1; /* 203:203 */
	uint32_t	tag_dest_map	:10;/* 202:193 */
	uint32_t	direction	:1; /* 192:192 */
	uint32_t	sip :32;		    /* 191:160 */
	uint32_t	dip :32;		    /* 159:128 */
	uint32_t	proto		:1; /* 127:127 */
	uint32_t	sport		:16;/* 126:111 */
	uint32_t	dport		:16;/* 110:95  */
	uint32_t	remap_port	:16;/* 94:79   */
	uint32_t	remap_ip;	    /* 47:78   */
	uint32_t	next_hop_idx	:7; /* 46:40   */
	uint32_t	remap_sa_idx	:2; /* 39:38   */
	uint32_t	dest_dma	:1; /* 37:37   */
	uint32_t	action		:2; /* 36:35   */
	uint32_t	hits;		    /* 34:3    */
	uint32_t	timestamp	:3; /* 2:0     */
#else
	uint32_t	timestamp	:3; /* 2:0     */
	uint32_t	hits;		    /* 34:3    */
	uint32_t	action		:2; /* 36:35   */
	uint32_t	dest_dma	:1; /* 37:37   */
	uint32_t	remap_sa_idx	:2; /* 39:38   */
	uint32_t	next_hop_idx	:7; /* 46:40   */
	uint32_t	remap_ip;	    /* 47:78   */
	uint32_t	remap_port	:16;/* 94:79   */
	uint32_t	dport		:16;/* 110:95  */
	uint32_t	sport		:16;/* 126:111 */
	uint32_t	proto		:1; /* 127:127 */
	uint32_t	dip;		    /* 159:128 */
	uint32_t	sip;		    /* 191:160 */
	uint32_t	direction	:1; /* 192:192 */
	uint32_t	tag_dest_map	:10;/* 202:193 */
	uint32_t	tag_ts		:1; /* 203:203 */
	uint32_t	tag_te		:2; /* 205:204 */
	uint32_t	tag_tc		:3; /* 208:206 */
	uint32_t	tag_oc		:3; /* 211:209 */
	uint32_t	valid		:1; /* 212:212 */
	uint32_t	reserved_0;	    /* 245:213 */
	uint32_t	reserved_1	:10;/* 254:246 */
	uint32_t	ipv4_entry	:1; /* 255:255 */
#endif
} napt_flow_ipv4_t;

/*
 * Flow Cache IPv6 Table Entry
 */
typedef struct flow_ipv6_t {
#ifdef BIG_ENDIAN
	uint8_t		sip[16];
	uint8_t		dip[16];
	uint32_t	ipv4_entry	:1;  /* 255:255 */
	uint32_t	reserved	:10; /* 254:245 */
	uint32_t	valid		:1;  /* 244:244 */
	uint32_t	tag_oc		:3;  /* 243:241 */
	uint32_t	tag_tc		:3;  /* 240:238 */
	uint32_t	tag_te		:2;  /* 237:236 */
	uint32_t	tag_ts		:1;  /* 235:235 */
	uint32_t	tag_dest_map	:10; /* 234:225 */
	uint32_t	direction	:1;  /* 224:224 */
	uint32_t	proto		:1;  /* 223:223 */
	uint32_t	sport		:16; /* 222:207 */
	uint32_t	dport		:16; /* 206:191 */
	uint32_t	remap_port	:16; /* 190:175 */
	uint8_t		remap_ip[16];	     /* 174:47  */
	uint32_t	next_hop_idx	:7;  /* 46:40   */
	uint32_t	remap_sa_idx	:2;  /* 39:38   */
	uint32_t	dest_dma	:1;  /* 37:37   */
	uint32_t	action		:2;  /* 36:35   */
	uint32_t	hits;		     /* 34:3    */
	uint32_t	timestamp	:3;  /* 2:0     */
#else
	uint32_t	timestamp	:3;  /* 2:0     */
	uint32_t	hits;		     /* 34:3    */
	uint32_t	action		:2;  /* 36:35   */
	uint32_t	dest_dma	:1;  /* 37:37   */
	uint32_t	remap_sa_idx	:2;  /* 39:38   */
	uint32_t	next_hop_idx	:7;  /* 46:40   */
	uint8_t		remap_ip[16];	     /* 174:47  */
	uint32_t	remap_port	:16; /* 190:175 */
	uint32_t	dport		:16; /* 206:191 */
	uint32_t	sport		:16; /* 222:207 */
	uint32_t	proto		:1;  /* 223:223 */
	uint32_t	direction	:1;  /* 224:224 */
	uint32_t	tag_dest_map	:10; /* 234:225 */
	uint32_t	tag_ts		:1;  /* 235:235 */
	uint32_t	tag_te		:2;  /* 237:236 */
	uint32_t	tag_tc		:3;  /* 240:238 */
	uint32_t	tag_oc		:3;  /* 243:241 */
	uint32_t	valid		:1;  /* 244:244 */
	uint32_t	reserved	:10; /* 254:245 */
	uint32_t	ipv4_entry	:1;  /* 255:255 */
	uint8_t		dip[16];
	uint8_t		sip[16];
#endif
} napt_flow_ipv6_t;
//#pragma pack(pop)

typedef struct flow_t {
	napt_flow_key_t		key;
	struct nf_conn		*ct;
	struct nf_conntrack	*nfct;
	struct flow_t		*chain_p;
	uint32_t		dot1q_tag;
	uint16_t		l3_proto;
	uint16_t		l4_proto;
	union {
		napt_flow_ipv4_t	ipv4;
		napt_flow_ipv6_t	ipv6;
		uint32_t		word;
	} fe;
    uint32_t valid;
    uint32_t location; /* Track which flows are actually added to hardware.
                          Some flows like those for FTP control connections
                          are not added to hw since the stack needs to process
                          information within FTP packets (example, PORT
                          command) */
    uint16_t pair_entry_index;
    uint8_t state;    /* This variable is used by aging logic to ignore eviction
                       * if the flow was added in the same interval as this
                       * polling cycle; it should be checked in later polling
                       * cycles for aging.
                       */
    uint32_t entry_state; /* To avoid duplicate packets from coming in before
                           * entries get added to flow table
                           */
} napt_flow_t;

/*
 * NAPT Flow Table
 */
typedef struct {
	napt_flow_t		*htable[FCACHE_HTABLE_SIZE];
	napt_flow_t		etable[FCACHE_MAX_ENTRIES];
	struct	timer_list 	timer; /* Refresh/GarbageCollect timer */
} flow_cache_t;

/*
 * Next Hop Table
 */
typedef struct {
#ifdef BIG_ENDIAN
	uint8_t		da[6];
	uint32_t	vlan		:16;
	uint32_t	op		:2;
	uint32_t	l2_fr_type	:1;
#else
	uint32_t	l2_fr_type	:1;
	uint32_t	op		:2;
	uint32_t	vlan		:16;
	uint8_t		da[6];
#endif
} next_hop_t;

/*
 * NAPT Pool Table
 */
typedef struct {
#ifdef BIG_ENDIAN
	uint8_t		remap_sa[6];
	uint32_t	ext		:1;
#else
	uint32_t	ext		:1;
	uint8_t		remap_sa[6];
#endif
} napt_pool_t;

#endif  /* defined(__FCACHE_H) */
