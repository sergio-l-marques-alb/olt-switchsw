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
 */

#ifndef PAE_SHARED_H
#define PAE_SHARED_H

/* Shared values with firmware */

#define PAE_COMPAT_VERSION (0x02)        /* incremented when an incompatible change is made */
#define PAE_FW_VERSION (0x000900)        /* 0xaabbcc : version aa.bb.cc */
#define PAE_IPSEC_COMPAT_VERSION (0x02)
#define PAE_IPSEC_VERSION (0x020000)  


#define PAE_NUM_ACTIONS (1024) 
#define PAE_NUM_ERROR_BUFFERS (32) 
#define PAE_ERROR_BUF_MASK (0x000000ff) 

#define PAE_FEATURE_BIT_FULL_TUPLE_HASH   (1 << 0)
#define PAE_FEATURE_BIT_MASKED_TUPLE_HASH (1 << 1)
#define PAE_FEATURE_BIT_IPSEC             (1 << 2)

#define PAE_FEATURE_BIT_ANY_TUPLE_HASH (PAE_FEATURE_BIT_FULL_TUPLE_HASH | PAE_FEATURE_BIT_MASKED_TUPLE_HASH)

typedef enum {
    PAE_NO_MESSAGE             = 0,
    PAE_ACTION_ADD             = 1,
    PAE_ACTION_DELETE          = 2,
    PAE_ACTION_UPDATE          = 3,
    PAE_ACTION_ERROR_CLEAR     = 4,
    PAE_FA_RULE_SET            = 5,
    PAE_LUE_RULE_ADD           = 7,
    PAE_LUE_RULE_DELETE        = 8,

    PAE_MEMORY_READ            = 9,
    PAE_MEMORY_WRITE           = 10,
    PAE_TUPLE_RULE_ADD_IPV4    = 11,
    PAE_TUPLE_RULE_ADD_IPV6    = 12,
    PAE_TUPLE_RULE_DELETE_IPV4 = 13,
    PAE_TUPLE_RULE_DELETE_IPV6 = 14,
    PAE_TUPLE_MASK_SET_IPV4    = 15,
    PAE_TUPLE_MASK_SET_IPV6    = 16,

    PAE_XOR_OP                 = 17,
} pae_msg_type_t;

typedef enum {
    PAE_ERR_NONE               = 0,
    PAE_ERR_BAD_STATE          = 1,  /* Signals unexpected/corrupted state in FW */
    PAE_ERR_INVALID_ACTION_IDX = 2,
    PAE_ERR_ACTION_IDX_IN_USE  = 3,
    PAE_ERR_MEMORY             = 4,
    PAE_ERR_INVALID_DATA       = 5,
    PAE_ERR_INVALID_DB         = 6,
    PAE_ERR_INVALID_DB_IDX     = 7,
    PAE_ERR_INVALID_FA_IDX     = 8,
    PAE_ERR_INVALID_TUPLE_RULE = 9,
	PAE_ERR_ACTION_IPSEC_MEMORY= 10,
	PAE_ERR_VERSION_COMPAT     = 11,
    PAE_ERR_BAD_CMD            = 12
} pae_resp_type_t;

typedef enum {
    PAE_ACTION_TYPE_NULL           = 0,
    PAE_ACTION_TYPE_FORWARD        = 1,
    PAE_ACTION_TYPE_HEADER_REWRITE = 2,
    PAE_ACTION_TYPE_IPSEC          = 3,
    PAE_ACTION_TYPE_SNOOP          = 4
} pae_action_type_t;

#define R5_BTCM_BASE            (0x40000000)  /* should match PAE_TCM_B_BASE_R5 from pae_regs.h */
#define R5_VERSION              (R5_BTCM_BASE)
#define R5_CUR_TIME             (R5_BTCM_BASE + 0x04)
#define R5_IDLE_TIME            (R5_BTCM_BASE + 0x08)
#define R5_IS_IDLE              (R5_BTCM_BASE + 0x0c)
#define R5_IDLE_START           (R5_BTCM_BASE + 0x10)
#define R5_ENDIAN_CHECK_WORD    (R5_BTCM_BASE + 0x14)
#define R5_ENDIAN_CHECK_BYTES   (R5_BTCM_BASE + 0x18)
#define R5_IPSEC_VERSION        (R5_BTCM_BASE + 0x1C)
#define R5_FEATURES             (R5_BTCM_BASE + 0x20)

#define PAE_CMD_TYPE_ADDR (R5_BTCM_BASE + 0x60)
#define PAE_CMD_RESP_ADDR (PAE_CMD_TYPE_ADDR + 0x04)
#define PAE_CMD_BUF_ADDR  (PAE_CMD_TYPE_ADDR + 0x08)
#define PAE_CMD_BUF_SIZE     (512)
#define PAE_CMD_RESP_PENDING (0xffffffff)
#define PAE_MAX_CMD_RESP_ITER (2000)      /* host-side: time to wait for a response */

//#define PAE_XOR_BUF_ADDR  (PAE_CMD_BUF_ADDR + PAE_CMD_BUF_SIZE)
//#define PAE_XOR_BUF_SIZE  (4096)

#define PAE_DEBUG_BUF_SIZE (1024)
//#define PAE_DEBUG_BUF_HEAD (PAE_XOR_BUF_ADDR + PAE_XOR_BUF_SIZE)
#define PAE_DEBUG_BUF_HEAD (PAE_CMD_BUF_ADDR + PAE_CMD_BUF_SIZE)
#define PAE_DEBUG_BUF_BASE (PAE_DEBUG_BUF_HEAD + 4)

#define ACTION_PERF_BASE (PAE_DEBUG_BUF_BASE + PAE_DEBUG_BUF_SIZE)
#define ACTION_PERF_SIZE_PER (4 * sizeof(uint32_t))
#define ACTION_PERF_BYTES_OFFSET (0)
#define ACTION_PERF_PACKETS_OFFSET (4)
#define ACTION_PERF_HIT_TIME_OFFSET (8)
#define ACTION_PERF_ERROR_STATE_OFFSET (c)



#endif /* PAE_SHARED_H */
