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
 * This file contains the main device structures
 *
 */

#ifndef _CIPHER_H
#define _CIPHER_H

#include <crypto/aes.h>
#include <crypto/internal/hash.h>

#include "spu.h"

#define ARC4_MIN_KEY_SIZE   1
#define ARC4_MAX_KEY_SIZE   256
#define ARC4_BLOCK_SIZE     1

#define MAX_KEY_SIZE    (AES_KEYSIZE_256) 
#define MAX_IV_SIZE     (AES_BLOCK_SIZE)
#define MAX_DIGEST_SIZE (SHA256_DIGEST_SIZE)

/* MD5, SHA1, SHA224, SHA256 all have the same block size of 64 bytes */
#define HASH_BLOCK_SIZE (64)

#define MAX_FA_PACKET_SIZE     (2000)
#define MAX_INFLIGHT           (10)    /* Max concurrent in_flight packets (all streams) */
                                       /* Need to protect against streamID overflow if > MAX_STREAMS */
#define MAX_STREAMS            (12)    /* Need to change EMH bitfields to make larger than 12 */

#define REQUEST_QUEUE_LENGTH   (1)

#define FLAGS_BUSY          BIT(0)


typedef struct {
    CIPHER_ALG alg;
    CIPHER_MODE mode;
} CipherOp;

typedef struct {
    HASH_ALG alg;
    HASH_MODE mode;
} AuthOp;


struct iproc_ctx_s {
    u8 enckey[MAX_KEY_SIZE];
    unsigned int enckeylen;

    u8 authkey[MAX_KEY_SIZE];
    unsigned int authkeylen;

    unsigned int digestsize;

    CipherOp    cipher;
    CIPHER_TYPE cipher_type;

    AuthOp auth;
    bool authFirst;

    struct crypto_aead *fallback_cipher;

    /* auth_type is determined during processing of request */

    uint8_t ipad[HASH_BLOCK_SIZE];
    uint8_t opad[HASH_BLOCK_SIZE];
};


struct iproc_reqctx_s {
    /* general context */
    struct crypto_async_request *parent;
    struct iproc_reqctx_s *next_active, *prev_active;
    
    unsigned total_todo;
    unsigned total_sent;
    unsigned src_sent;
    unsigned total_received;
    bool in_use;
    bool can_send;
    u8 streamID;

    /* cipher context */
    unsigned    isEncrypt;
    u8 iv_ctr[MAX_IV_SIZE];    /* CBC mode: IV.  CTR mode: counter.  Else empty. */
    unsigned iv_ctr_len;       /* = block_size if either an IV or CTR is present, else 0 */
    
    uint8_t hash_ctx[HASH_BLOCK_SIZE];
    unsigned hash_ctx_len;

    /* auth context */
    u8 buf[MAX_SPU_DATA_SIZE];
    unsigned nbuf;

    unsigned int last;

    /* hmac context */
    bool is_sw_hmac;

    /* aead context */
    struct crypto_tfm *old_tfm;
    crypto_completion_t old_complete;
    void *old_data;
};


struct device_private {
    struct platform_device *pdev;

    unsigned long          flags;

    spinlock_t             lock;

    struct task_struct     *kthread;

    struct iproc_reqctx_s   *active_list_start;
    struct iproc_reqctx_s   *active_list_end;

    unsigned remaining_slots;
    unsigned stream_count;
};


struct iproc_alg_s {
    u32 type;
    union {
        struct crypto_alg crypto;
        struct ahash_alg hash;
    } alg;
    CipherOp cipher_info;
    AuthOp auth_info;
    bool authFirst;
};

extern struct device_private iproc_priv;

#endif /* _CIPHER_H */
