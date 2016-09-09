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
 * This file implements symmetric crypto acceleration (through the SPU and FA+)
 *
 */

#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/crypto.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/rtnetlink.h>

#include <crypto/algapi.h>
#include <crypto/aead.h>
#include <crypto/aes.h>
#include <crypto/des.h>
#include <crypto/sha.h>
#include <crypto/md5.h>
#include <crypto/authenc.h>
#include <crypto/skcipher.h>
#include <crypto/hash.h>
#include <crypto/scatterwalk.h>
#include <crypto/aes.h>

#include "../fa2/fa2_if.h"
#include "util.h"
#include "spu.h"

#include "cipher.h"

/* ================= Device Structure ================== */

struct device_private iproc_priv;


/* ==================== Parameters ===================== */

int flow_debug_logging = 0;
int packet_debug_logging = 0;
int debug_logging_sleep = 0;

module_param(flow_debug_logging, int, 0644);
MODULE_PARM_DESC(flow_debug_logging, "Enable Flow Debug Logging");

module_param(packet_debug_logging, int, 0644);
MODULE_PARM_DESC(packet_debug_logging, "Enable Packet Debug Logging");

module_param(debug_logging_sleep, int, 0644);
MODULE_PARM_DESC(debug_logging_sleep, "Packet Debug Logging Sleep");

unsigned op_counts[4] = {0, 0, 0, 0};
unsigned long bytes_out = 0;
unsigned long bytes_in = 0;
unsigned max_inflight = 0;

module_param_array(op_counts, uint, NULL, 0644);
MODULE_PARM_DESC(op_counts, "Crypto Operation Counts: cipher hash hmac aead");

module_param(bytes_out, ulong, 0644);
MODULE_PARM_DESC(bytes_out, "Crypto Bytes Sent");

module_param(bytes_in, ulong, 0644);
MODULE_PARM_DESC(bytes_in, "Crypto Bytes Received");

module_param(max_inflight, uint, 0644);
MODULE_PARM_DESC(bytes_in, "Max packets inflight at once");


/* ==================== Queue Tasks and Helpers ==================== */

/* Called back from FA2 driver when a response is received */
static void iproc_cb(unsigned char* data, size_t len, void* cb_data_ptr, unsigned cb_data_idx);
/* finish_req() is used to notify that the current request has been completed */
static void finish_req(struct iproc_reqctx_s *rctx, int err);


/* handle_ablkcipher_req() will process the current request as an ablkcipher */
static unsigned handle_ablkcipher_req(struct ablkcipher_request *req)
{
    struct iproc_reqctx_s *rctx = ablkcipher_request_ctx(req);
    struct iproc_ctx_s *ctx = crypto_ablkcipher_ctx(crypto_ablkcipher_reqtfm(req));

	int err = 0;
    unsigned chunksize = 0;
    int remaining = 0;
    int chunk_start;
    unsigned local_total_sent;
    u8 local_iv_ctr[MAX_IV_SIZE];
    struct sk_buff *skb;
    bool pae_chain = false;
    unsigned long flags;
    
    spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        chunk_start = rctx->src_sent;
        remaining = rctx->total_todo - chunk_start;

        /* make sure we didn't try and send because of a race */
        if (!remaining) {
            printk("%s() on finished req\n", __FUNCTION__);

            rctx->can_send = false;  
            spin_unlock_irqrestore(&iproc_priv.lock, flags);
            return 0;
        }

        /* enable PAE chaining for CBC encrypt follow-on packets */
        if ((chunk_start) && (ctx->cipher.mode == CIPHER_MODE_CBC) && (rctx->isEncrypt)) {
                pae_chain = true;
        }

        /* determine the chunk we are breaking off */
        chunksize = (remaining > MAX_SPU_DATA_SIZE) ? MAX_SPU_DATA_SIZE : remaining;
        local_total_sent = rctx->total_sent += chunksize;
        rctx->src_sent += chunksize;

        /* get our local copy of the iv */
        __builtin_memcpy(local_iv_ctr, rctx->iv_ctr, MAX_IV_SIZE);

        /* generate the next iv if possible */
        if ((ctx->cipher.mode == CIPHER_MODE_CBC) && !rctx->isEncrypt) {
            /* CBC Decrypt: next IV is the last ciphertext block in this chunk */
            sg_copy_part_to_buf(req->src, rctx->iv_ctr, rctx->iv_ctr_len, rctx->src_sent - rctx->iv_ctr_len);
        } else if (ctx->cipher.mode == CIPHER_MODE_CTR) {
            /* CTR mode, increment counter for next block*/
            /* Assumes 16-byte block (AES).  SPU does not support CTR mode for DES/3DES anyhow. */
            add_to_ctr(rctx->iv_ctr, chunksize);
        }

        if (rctx->total_sent == rctx->total_todo) {
            /* we are sending the last chunk... then we can't send more */
            rctx->can_send = false;
        } 
    }
    spin_unlock_irqrestore(&iproc_priv.lock, flags);

    flow_log("handle_ablkcipher_req() req:%p totsent:%u chunkstart:%u remaining:%u chunksize:%u\n",
             req, local_total_sent, chunk_start, remaining, chunksize);

    skb = spu_create_request(pae_chain, rctx->streamID,                      /* PAEaccel, streamID */
                             !(rctx->isEncrypt), 0,                          /* inbound, auth */
                             ctx->cipher.alg, ctx->cipher.mode, ctx->cipher_type, /* cipher info */
                             (uint8_t *)ctx->enckey, ctx->enckeylen,         /* key */
                             local_iv_ctr, rctx->iv_ctr_len,                 /* iv */
                             ctx->auth.alg, ctx->auth.mode, HASH_TYPE_NONE,  /* auth info */
                             0,                                              /* digest size */
                             NULL, 0,                                        /* auth key */
                             0,                                              /* prev_length_blocks */
                             NULL, 0, 0,                                     /* assoc */
                             NULL, 0,                                        /* prepend data */
                             req->src, chunk_start, chunksize,               /* data */
                             0, 0,                                           /* hash_pad, total_sent */
                             NULL, 0,                                        /* ICV */
                             NULL, 0);                                       /* IV */

    err = fa2_tx_to_spu(skb, iproc_cb, rctx, chunk_start);
	if (err) {
        printk("%s() Error: send failed. err:%d\n", __FUNCTION__, err);

        finish_req(rctx, err);
        return 0;
    }

	return skb->len;
}


/* handle_ablkcipher_resp() will handle an ablkcipher SPU response and either */
/* fire off the next chained SPU message or finish the request */
static void handle_ablkcipher_resp(struct ablkcipher_request *req, u8 *buf, unsigned buflen, unsigned offset)
{
    struct iproc_reqctx_s *rctx = ablkcipher_request_ctx(req);
    unsigned long flags;

    flow_log("handle_ablkcipher_resp() req:%p buflen:%u\n", req, buflen);
    packet_dump("  resp: ", buf, buflen);

    /* load the answer (or part thereof) into the req->dest */
    sg_copy_part_from_buf(req->dst, buf, buflen, offset);

    spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        rctx->total_received += buflen;

        if (rctx->total_received != rctx->total_todo) {
            spin_unlock_irqrestore(&iproc_priv.lock, flags);
            return;
        }

        /* We just finished a CIPHER so up the count */
        ++op_counts[0];
    }
    spin_unlock_irqrestore(&iproc_priv.lock, flags);

    finish_req(rctx, 0);
}


/* handle_ahash_req() will process the current request as an ahash */
static int handle_ahash_req(struct ahash_request *req)
{
    struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
    struct iproc_ctx_s *ctx = crypto_ahash_ctx(tfm);
    struct iproc_reqctx_s *rctx = ahash_request_ctx(req);
    uint8_t *authkey = (uint8_t *)ctx->authkey; 
    unsigned authkeylen = ctx->authkeylen; 
    unsigned nbytes_to_hash = 0;
    HASH_TYPE hash_type = HASH_TYPE_NONE;
	int err = 0;
    unsigned chunksize = 0, chunk_start = 0;
    int pad_len = 0;                         /* signed, needs to handle roll-overs */
    struct sk_buff *skb;
    unsigned local_nbuf;
    unsigned long flags;
 
    /* leave space for padding, and round down to HASH_BLOCK_SIZE */
    unsigned max_chunk_size = (MAX_SPU_DATA_SIZE - HASH_BLOCK_SIZE) / HASH_BLOCK_SIZE * HASH_BLOCK_SIZE;
   
    flow_log("%s() req:%p\n", __FUNCTION__, req);

    spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        /* compute the amount remaining to hash (used often) */
        nbytes_to_hash = rctx->total_todo - rctx->total_sent;
        chunksize = min(nbytes_to_hash, max_chunk_size);

        /* pull off remaining portion of request if we don't have enough for a full chunk and we aren't done */
        if ((nbytes_to_hash < max_chunk_size) && (!rctx->last)) {
            sg_copy_part_to_buf(req->src, rctx->buf + rctx->nbuf, nbytes_to_hash - rctx->nbuf, rctx->src_sent);
            rctx->nbuf = nbytes_to_hash;

            spin_unlock_irqrestore(&iproc_priv.lock, flags);

            flow_log("  Exiting with stored remnant. nbuf:%u\n", rctx->nbuf);
            packet_dump("  buf: ", rctx->buf, rctx->nbuf);

            finish_req(rctx, 0);
            return 0;
        }

        /* if we have an nbuf then it's ours */
        local_nbuf = rctx->nbuf;
        rctx->nbuf = 0;

        chunk_start = rctx->src_sent;
        rctx->total_sent += chunksize;
        hash_type = (rctx->src_sent) ?  HASH_TYPE_UPDT : HASH_TYPE_INIT;
        rctx->src_sent += chunksize - local_nbuf;  /* if you sent a prebuf then that wasn't from this req->src */

        if (rctx->total_sent == rctx->total_todo) {
            if (rctx->last) {
                /* do padding.  Reserve space for 1 byte (0x80) and the total length as u64 */
                unsigned used_space_last_block = chunksize % HASH_BLOCK_SIZE + 1 + sizeof(uint64_t);
                pad_len = HASH_BLOCK_SIZE - used_space_last_block;
                if (pad_len < 0) {
                    pad_len += HASH_BLOCK_SIZE;
                }

                /* we are sending the last chunk... then we can't send more */
                rctx->can_send = false;
            }
        }

        /* if we have a stored context then override and use it */
        if (hash_type == HASH_TYPE_UPDT) {
            authkey = req->result;
            authkeylen = ctx->digestsize;
        }
    }
    spin_unlock_irqrestore(&iproc_priv.lock, flags);

    flow_log("  sent last:%u nbuf:%u chunk_start:%u chunk_size:%u\n",
             rctx->last, local_nbuf, chunk_start, chunksize);

    skb = spu_create_request(chunk_start, rctx->streamID,                         /* chain non-first, streamID */
                             0, 0,                                                /* inbound, auth */
                             ctx->cipher.alg, ctx->cipher.mode, CIPHER_TYPE_NONE, /* cipher info */
                             (uint8_t *)ctx->enckey, ctx->enckeylen,              /* key */
                             NULL, 0,                                             /* iv & iv len */
                             ctx->auth.alg, ctx->auth.mode, hash_type,            /* auth info */
                             ctx->digestsize,                                     /* digest size */
                             authkey, authkeylen,                                 /* auth key */
                             0,                                                   /* prev_length_blocks */
                             NULL, 0, 0,                                          /* assoc */
                             rctx->buf, local_nbuf,                               /* prepend data */
                             req->src, chunk_start, chunksize - local_nbuf,       /* data */
                             pad_len, rctx->total_sent,                           /* hash_pad, total_sent */
                             NULL, 0,                                             /* ICV */
                             NULL, 0);                                            /* IV */

    err = fa2_tx_to_spu(skb, iproc_cb, rctx, chunk_start);
 	if (err) {
        printk("%s() Error: send failed. err:%d\n", __FUNCTION__, err);

		finish_req(rctx, err);
        return 0;
	}
        
    return skb->len;
}


/* handle_ahash_resp() will handle an ablkcipher SPU response and either */
/* fire off the next chained SPU message or finish the request */
static void handle_ahash_resp(struct ahash_request *req, u8 *buf, unsigned buflen, bool isFinal)
{
    struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
    struct iproc_ctx_s *ctx = crypto_ahash_ctx(ahash);
    struct iproc_reqctx_s *rctx = ahash_request_ctx(req);
    unsigned blocksize = crypto_tfm_alg_blocksize(crypto_ahash_tfm(ahash));
    unsigned digestsize = ctx->digestsize;

    /* skip to the digest */
    buf += (buflen - digestsize); buflen -= (buflen - digestsize);

    flow_log("%s() req:%p buflen:%u blocksize:%u digestsize:%u isFinal:%u\n",
             __FUNCTION__, req, buflen, blocksize, digestsize, isFinal);
    packet_dump("  resp: ", buf, buflen);

    /* We don't need lock protection because only one response gets past here */
    if (!isFinal) 
        return;

    /* get context/result and store it */
    memcpy(req->result, buf, digestsize);

    /* byte swap the output from the UPDT function to network byte order */
    if (ctx->auth.alg == HASH_ALG_MD5) {
        __swab32s((u32*)req->result);
        __swab32s(((u32*)req->result) + 1);
        __swab32s(((u32*)req->result) + 2);
        __swab32s(((u32*)req->result) + 3);
        __swab32s(((u32*)req->result) + 4);
    }

    /* if this an HMAC then do the outer hash */
    if (rctx->is_sw_hmac) {
        switch (ctx->auth.alg) {
        case HASH_ALG_MD5:
            do_shash("md5", req->result, ctx->opad, blocksize, req->result, ctx->digestsize);
            break;
        case HASH_ALG_SHA1:
            do_shash("sha1", req->result, ctx->opad, blocksize, req->result, ctx->digestsize);
            break;
        case HASH_ALG_SHA256:
            do_shash("sha256", req->result, ctx->opad, blocksize, req->result, ctx->digestsize);
            break;
        default:
            printk("%s() Error : unknown hmac type\n", __FUNCTION__);
            finish_req(rctx, -EINVAL);
            return;
        }

        flow_dump("  hmac: ", req->result, ctx->digestsize);

        /* We just finished an HMAC so up the count */
        ++op_counts[2];
    } else {
        /* We just finished a HASH so up the count */
        ++op_counts[1];
    }

    finish_req(rctx, 0);
    return;
} 


/* handle_aead_req() will process the current request as an aead */
static int handle_aead_req(struct aead_request *req)
{
    struct iproc_reqctx_s *rctx = aead_request_ctx(req);
	struct iproc_ctx_s *ctx = crypto_aead_ctx(crypto_aead_reqtfm(req));
    uint8_t *authkey = (uint8_t *)ctx->authkey; 

	int err;
    unsigned chunksize;
    int chunk_start;
    struct sk_buff *skb;
    unsigned long flags;
    uint8_t *predata = NULL;
    unsigned predata_len = 0;
    HASH_TYPE hash_type = HASH_TYPE_NONE;
    int pad_len = 0;                         /* signed, needs to handle roll-overs */
    
    spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        /* make sure we didn't try and send because of a race */
        if (!rctx->total_todo) {
            printk("handle_aeadr_req on finished req\n");

            rctx->can_send = false;  
            spin_unlock_irqrestore(&iproc_priv.lock, flags);
            return false;
        }

        /* doing the whole thing at once */
        chunk_start = 0;
        chunksize = rctx->total_todo;
        rctx->src_sent = rctx->total_sent = chunksize = rctx->total_todo;

        /* we are sending the only chunk */
        rctx->can_send = false;
    }
    spin_unlock_irqrestore(&iproc_priv.lock, flags);

    flow_log("%s() req:%p tot_todo:%u chunksize:%u tot_sent:%u\n",
             __FUNCTION__, req, rctx->total_todo, chunksize, chunk_start);

    skb = spu_create_request(0, rctx->streamID,                                     /* PAEaccel, streamID */
                             !(rctx->isEncrypt), ctx->authFirst,                    /* inbound, auth */
                             ctx->cipher.alg, ctx->cipher.mode, ctx->cipher_type,   /* cipher info */
                             (uint8_t *)ctx->enckey, ctx->enckeylen,                /* key */
                             rctx->iv_ctr, rctx->iv_ctr_len,                        /* iv */
                             ctx->auth.alg, ctx->auth.mode, hash_type,              /* auth info */
                             ctx->digestsize,                                       /* digest size */
                             authkey, ctx->authkeylen,                              /* auth key */
                             0,                                                     /* prev_length_blocks */
                             req->assoc, 0, req->assoclen,                          /* assoc */
                             predata, predata_len,                                  /* prepend data */
                             req->src, chunk_start, chunksize - rctx->nbuf,         /* data */
                             pad_len, rctx->total_sent,                             /* hash_pad, total_sent */
                             NULL, 0,                                               /* ICV */
                             rctx->iv_ctr, rctx->iv_ctr_len);                       /* IV */

    bytes_out += skb->len;
    err = fa2_tx_to_spu(skb, iproc_cb, rctx, chunk_start);
	if (err) {
        printk("%s() Error: send failed. err:%d\n", __FUNCTION__, err);

        finish_req(rctx, err);
        return false;
    }

	return true;
}


/* handle_aead_resp() will handle an aead SPU response and either */
/* fire off the next chained SPU message or finish the request */
static void handle_aead_resp(struct aead_request *req, u8 *buf, unsigned buflen, unsigned offset)
{
    struct iproc_reqctx_s *rctx = aead_request_ctx(req);
	struct iproc_ctx_s *ctx = crypto_aead_ctx(crypto_aead_reqtfm(req));
    unsigned long flags;

    /* skip over the header and truncate the SPU status */
    buf += (rctx->iv_ctr_len + req->assoclen); buflen -= (rctx->iv_ctr_len + req->assoclen);

    flow_log("%s() req:%p buflen:%u\n", __FUNCTION__, req, buflen);
    packet_dump("  resp: ", buf, buflen);

    sg_copy_part_from_buf(req->dst, buf, buflen, offset);

    spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        rctx->total_received += buflen;

        if (rctx->total_sent != rctx->total_todo)
            rctx->can_send = true;

        if (rctx->total_received != rctx->total_todo + ctx->digestsize) {
            spin_unlock_irqrestore(&iproc_priv.lock, flags);
            return;
        }

        /* We just finished an AEAD so up the count */
        ++op_counts[3];
    }
    spin_unlock_irqrestore(&iproc_priv.lock, flags);

    finish_req(rctx, 0);
}


static void handle_active_list(void)
{
	unsigned long flags;
	struct iproc_reqctx_s *curr_rctx;
    int did_send;

    flow_log("%s()\n", __FUNCTION__);
    dump_active_list();

    /* Lock before traversing list, as a receive can complete a transaction and modify the list */
	spin_lock_irqsave(&iproc_priv.lock, flags);

    curr_rctx = iproc_priv.active_list_start;

    while (curr_rctx && iproc_priv.remaining_slots) {
        if (!curr_rctx->in_use && curr_rctx->can_send && iproc_priv.remaining_slots) {
            curr_rctx->in_use = true;
            spin_unlock_irqrestore(&iproc_priv.lock, flags);

            /* determine the type of the current request and process it appropriately */
            switch (crypto_tfm_alg_type(curr_rctx->parent->tfm)) {
            case CRYPTO_ALG_TYPE_AHASH:
                did_send = handle_ahash_req(ahash_request_cast(curr_rctx->parent));
                break;
            case CRYPTO_ALG_TYPE_AEAD:
                did_send = handle_aead_req(container_of(curr_rctx->parent, struct aead_request, base));
                break;
            case CRYPTO_ALG_TYPE_ABLKCIPHER:
                did_send = handle_ablkcipher_req(ablkcipher_request_cast(curr_rctx->parent));
                break;
            default:
                did_send = 0;
                curr_rctx->can_send = false;

                /* This should never happen.  If it does, the rctx will leak, as we won't process it */
                printk(KERN_ERR "%s() Error: unknown req type\n", __FUNCTION__);
            }

            spin_lock_irqsave(&iproc_priv.lock, flags);

            curr_rctx->in_use = false;

            if (did_send) {
                bytes_out += did_send;
                --iproc_priv.remaining_slots;
                max_inflight = max(max_inflight, MAX_INFLIGHT - iproc_priv.remaining_slots);
            } else {
                /* may indicate that the curr_rctx is now invalid.  reset and start over */
                curr_rctx = iproc_priv.active_list_start;
            }
        } else {
            curr_rctx = curr_rctx->next_active;
        }
    }

    spin_unlock_irqrestore(&iproc_priv.lock, flags);
}


/* finish_req() is used to notify that the current request has been completed */
static void finish_req(struct iproc_reqctx_s *rctx, int err)
{
	unsigned long flags;
    struct crypto_async_request *areq = rctx->parent;

    flow_log("%s() rctx:%p err:%d\n", __FUNCTION__, rctx, err);
        
	spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        /* shouldn't be sending on this anymore */
        rctx->can_send = false;

        /* remove rctx from active list */
        if (rctx->prev_active)
            rctx->prev_active->next_active = rctx->next_active;
        if (rctx->next_active)
            rctx->next_active->prev_active = rctx->prev_active;
        if (rctx == iproc_priv.active_list_start)
            iproc_priv.active_list_start = rctx->next_active;
        if (rctx == iproc_priv.active_list_end)
            iproc_priv.active_list_end = rctx->prev_active;
    }    
    spin_unlock_irqrestore(&iproc_priv.lock, flags);

    /* Mark the request as complete, unsafe to use now */
    areq->complete(areq, err);
}


static void iproc_cb(unsigned char* data, size_t len, void* cb_data_ptr, unsigned cb_data_idx)
{
    struct iproc_reqctx_s *rctx = (struct iproc_reqctx_s*) cb_data_ptr;
    struct crypto_async_request *areq = rctx->parent;

    u32 spu_status;
    int err = 0;
    bool isFinal;
    unsigned long flags;

    flow_log("%s() areq:%p len:%u\n", __FUNCTION__, areq, len);
    packet_dump("  data: ", data, len);

    /* allow the message thread to spawn another request message for this req */
	spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        iproc_priv.remaining_slots++;
    }
    spin_unlock_irqrestore(&iproc_priv.lock, flags);

    /* sanity check on length: need to have space for header & status at the least */
    if (len < 12 + 4) {
        printk(KERN_ERR "%s() Warning: Runt packet from SPU, len %d. Dropped.\n",
               __FUNCTION__, len);

        err = -EINVAL;
        goto cb_error;
    }

    /* Pull the isFinal bit, accounting for endianness */
    isFinal = data[6] & 0x80;
    
    /* process the SPU status */
    spu_status = __be32_to_cpu(*(__be32*)(data + len - 4));
    if (spu_status) { 
        printk(KERN_ERR "%s() Warning: Error result from SPU: 0x%08x\n",
               __FUNCTION__, spu_status);
        err = -EINVAL;
        goto cb_error;
    }

    /* update the module statistics */
    bytes_in += len;

    /* skip over the header and truncate the SPU status */
    data += 12; len -= (12 + 4);

    /* return the data to the proper subsystem */
    switch (crypto_tfm_alg_type(areq->tfm)) {
    case CRYPTO_ALG_TYPE_ABLKCIPHER:
        handle_ablkcipher_resp(ablkcipher_request_cast(areq), data, len, cb_data_idx);
        break;
    case CRYPTO_ALG_TYPE_AHASH:
        handle_ahash_resp(ahash_request_cast(areq), data, len, isFinal);
        break;
    case CRYPTO_ALG_TYPE_AEAD:
        handle_aead_resp(container_of(areq, struct aead_request, base), data, len, cb_data_idx);
        break;
    default:
        err = -EINVAL;
        goto cb_error;
    }

    handle_active_list();
    return;
    
cb_error:
    /* If we got something from the SPU then assume it should have */
    /* finished the current outstanding req, handle next one too */
    finish_req(rctx, err);

    handle_active_list();
}


/* ==================== Kernel Cryptographic API ==================== */

/* ablkcipher helpers */

static int ablkcipher_enqueue(struct ablkcipher_request *req, uint8_t encrypt)
{
 	struct iproc_reqctx_s *rctx = ablkcipher_request_ctx(req);
	struct iproc_ctx_s *ctx = crypto_ablkcipher_ctx(crypto_ablkcipher_reqtfm(req));
	unsigned long flags;

    flow_log("%s() req:%p enc:%u\n", __FUNCTION__, req, encrypt);

    rctx->in_use = false;
    rctx->isEncrypt = encrypt;
    rctx->total_todo = req->nbytes;
    rctx->total_sent = 0;
    rctx->src_sent = 0;    
    rctx->total_received = 0;
    
    if (ctx->cipher.mode == CIPHER_MODE_CBC || ctx->cipher.mode == CIPHER_MODE_CTR) {
        rctx->iv_ctr_len = crypto_ablkcipher_ivsize(crypto_ablkcipher_reqtfm(req));
    } else {
        rctx->iv_ctr_len = 0;
    }
    memcpy(rctx->iv_ctr, req->info, rctx->iv_ctr_len);

	spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        rctx->parent = &req->base;
        rctx->prev_active = iproc_priv.active_list_end;
        rctx->next_active = NULL;
        if (iproc_priv.active_list_end)
            iproc_priv.active_list_end->next_active = rctx;
        if (!iproc_priv.active_list_start)
            iproc_priv.active_list_start = rctx;
        iproc_priv.active_list_end = rctx;

        rctx->can_send = true;
    }
	spin_unlock_irqrestore(&iproc_priv.lock, flags);

    handle_active_list();

    return -EINPROGRESS;
}


static int ablkcipher_setkey(struct crypto_ablkcipher *cipher, const u8 *key, unsigned keylen)
{
    struct iproc_ctx_s *ctx = crypto_ablkcipher_ctx(cipher);

    flow_log("ablkcipher_setkey() keylen: %d\n", keylen);
    flow_dump("  key: ", key, keylen);

    switch (ctx->cipher.alg) {
    case CIPHER_ALG_DES:
        if (keylen == DES_KEY_SIZE) {
            u32 tmp[DES_EXPKEY_WORDS];

            if (des_ekey(tmp, key) == 0) {
                if (crypto_ablkcipher_get_flags(cipher) & CRYPTO_TFM_REQ_WEAK_KEY) {
                    crypto_ablkcipher_set_flags(cipher, CRYPTO_TFM_RES_WEAK_KEY);
                    return -EINVAL;
                }
            }

            ctx->cipher_type = CIPHER_TYPE_DES;
        } else {
            crypto_ablkcipher_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
            return -EINVAL;
        }
        break;
    case CIPHER_ALG_3DES:
        if (keylen == (DES_KEY_SIZE * 3)) {
            const u32 *K = (const u32 *)key;
            if ( !((K[0] ^ K[2]) | (K[1] ^ K[3])) ||
                 !((K[2] ^ K[4]) | (K[3] ^ K[5])) ) {
                crypto_ablkcipher_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_SCHED);
                return -EINVAL;
            }

            ctx->cipher_type = CIPHER_TYPE_3DES;
        } else {
            crypto_ablkcipher_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
            return -EINVAL;
        }
        break;
    case CIPHER_ALG_AES:
        switch (keylen) {
        case AES_KEYSIZE_128: ctx->cipher_type = CIPHER_TYPE_AES128; break;
        case AES_KEYSIZE_192: ctx->cipher_type = CIPHER_TYPE_AES192; break;
        case AES_KEYSIZE_256: ctx->cipher_type = CIPHER_TYPE_AES256; break;
        default:
            crypto_ablkcipher_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
            return -EINVAL;
        }
        break;
    default:
        printk("%s() Error: unknown cipher alg\n", __FUNCTION__);
        return -EINVAL;
    }

    memcpy(ctx->enckey, key, keylen);
    ctx->enckeylen = keylen;
    
    return 0;
}


static int ablkcipher_encrypt(struct ablkcipher_request *req)
{
    flow_log("ablkcipher_encrypt() alkb_req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    return ablkcipher_enqueue(req, 1);
}


static int ablkcipher_decrypt(struct ablkcipher_request *req)
{
    flow_log("ablkcipher_decrypt() alkb_req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    return ablkcipher_enqueue(req, 0);
}


/* ahash helpers */

static int ahash_enqueue(struct ahash_request *req)
{
	struct iproc_reqctx_s *rctx = ahash_request_ctx(req);
	unsigned long flags;
  
    flow_log("ahash_enqueue() req:%p base:%p nbytes:%u\n", req, &req->base, req->nbytes);

    rctx->in_use = false;

    /* lock the active list */
	spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        rctx->parent = &req->base;
        rctx->prev_active = iproc_priv.active_list_end;
        rctx->next_active = NULL;
        if (iproc_priv.active_list_end)
            iproc_priv.active_list_end->next_active = rctx;
        if (!iproc_priv.active_list_start)
            iproc_priv.active_list_start = rctx;
        iproc_priv.active_list_end = rctx;

        rctx->can_send = true;
    }
	spin_unlock_irqrestore(&iproc_priv.lock, flags);

    handle_active_list();

    return -EINPROGRESS;
}


static int ahash_init(struct ahash_request *req)
{
    struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
    struct iproc_reqctx_s *rctx = ahash_request_ctx(req);
    struct iproc_ctx_s *ctx = crypto_ahash_ctx(tfm);

    flow_log("ahash_init() req:%p\n", req);

    /* Initialize the context */
    rctx->nbuf = 0;
    rctx->last = 0;

    rctx->total_todo = 0;
    rctx->total_sent = 0;
    rctx->src_sent = 0;
    rctx->total_received = 0;

    ctx->digestsize = crypto_ahash_digestsize(tfm);

    rctx->is_sw_hmac = false;

    return 0;
}


static int ahash_update(struct ahash_request *req)
{
    struct iproc_reqctx_s *rctx = ahash_request_ctx(req);

    flow_log("ahash_update() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    if (!req->nbytes)
        return 0;

    rctx->total_todo += req->nbytes;

    return ahash_enqueue(req);
}


static int ahash_final(struct ahash_request *req)
{
    struct iproc_reqctx_s *rctx = ahash_request_ctx(req);

    flow_log("ahash_final() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */
    
    rctx->last = 1;
    
    return ahash_enqueue(req);
}


static int ahash_finup(struct ahash_request *req)
{
    struct iproc_reqctx_s *rctx = ahash_request_ctx(req);

    flow_log("ahash_finup() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    rctx->total_todo += req->nbytes;
    rctx->last = 1;

    return ahash_enqueue(req);
}


static int ahash_digest(struct ahash_request *req)
{
    struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
    int err = 0;

    flow_log("ahash_digest() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    /* whole thing at once */
    err = ahash->init(req);
    if (!err) {
        err = ahash->finup(req);
    }

    return err;
}


/*  HMAC ahash functions */

static int ahash_hmac_setkey(struct crypto_ahash *ahash, const u8 *key, unsigned keylen)
{
    struct iproc_ctx_s *ctx = crypto_ahash_ctx(ahash);
    unsigned blocksize = crypto_tfm_alg_blocksize(crypto_ahash_tfm(ahash));
    unsigned digestsize = crypto_ahash_digestsize(ahash);
    unsigned index;

    flow_log("%s() ahash:%p key:%p keylen:%u blksz:%u digestsz:%u\n",
             __FUNCTION__, ahash, key, keylen, blocksize, digestsize);
    flow_dump("  key: ", key, keylen);

    if (keylen > blocksize) {
        switch (ctx->auth.alg) {
        case HASH_ALG_MD5:
            do_shash("md5", ctx->ipad, key, keylen, NULL, 0);
            break;
        case HASH_ALG_SHA1:
            do_shash("sha1", ctx->ipad, key, keylen, NULL, 0);
            break;
        case HASH_ALG_SHA256:
            do_shash("sha256", ctx->ipad, key, keylen, NULL, 0);
            break;
        default:
            printk("%s() Error: unknown hash alg\n", __FUNCTION__);
            return -EINVAL;
        }

        keylen = digestsize;

        flow_log("  keylen > digestsize... hashed\n");
        flow_dump("  newkey: ", ctx->ipad, keylen);
    } else {
        memcpy(ctx->ipad, key, keylen);
    }

    ctx->digestsize = digestsize;
    ctx->authkeylen = 0;          /* not "keylen" since we are using hash only operation */

    memset(ctx->ipad + keylen, 0, blocksize - keylen);
    memcpy(ctx->opad, ctx->ipad, blocksize);

    for (index = 0; index < blocksize; index++) {
        ctx->ipad[index] ^= 0x36;
        ctx->opad[index] ^= 0x5c;
    }

    flow_dump("  ipad: ", ctx->ipad, HASH_BLOCK_SIZE);
    flow_dump("  opad: ", ctx->opad, HASH_BLOCK_SIZE);

    return 0;
}


static int ahash_hmac_init(struct ahash_request *req)
{
    struct iproc_reqctx_s *rctx = ahash_request_ctx(req);
    struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
    struct iproc_ctx_s *ctx = crypto_ahash_ctx(tfm);
    
    flow_log("ahash_hmac_init() req:%p\n", req);

    /* init the context as a hash, but */
    ahash_init(req);

    /* start with a prepended ipad */
    memcpy(rctx->buf, ctx->ipad, HASH_BLOCK_SIZE);
    rctx->nbuf = HASH_BLOCK_SIZE;
    rctx->total_todo += HASH_BLOCK_SIZE;

    ctx->auth.mode = HASH_MODE_HASH;

    rctx->is_sw_hmac = true;

    return 0;
}


static int ahash_hmac_update(struct ahash_request *req)
{
    flow_log("ahash_hmac_update() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    if (!req->nbytes)
        return 0;

    return ahash_update(req);
}


static int ahash_hmac_final(struct ahash_request *req)
{
    flow_log("ahash_hmac_final() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */
    
    return ahash_final(req);
}


static int ahash_hmac_finup(struct ahash_request *req)
{
    flow_log("ahash_hmac_finupl() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    return ahash_finup(req);
}


static int ahash_hmac_digest(struct ahash_request *req)
{
    struct crypto_ahash *ahash = crypto_ahash_reqtfm(req);
    int err = 0;

    flow_log("ahash_hmac_digest() req:%p nbytes:%u\n", req, req->nbytes);
    /* dump_sg(req->src, req->nbytes); */

    /* whole thing at once */
    err = ahash->init(req);
    if (!err) {
        err = ahash->finup(req);
    }

    return err;
}


/* aead helpers */

static int aead_need_fallback(struct aead_request *req)
{
 	struct iproc_reqctx_s *rctx = aead_request_ctx(req);
	struct iproc_ctx_s *ctx = crypto_aead_ctx(crypto_aead_reqtfm(req));

    unsigned packetlen = (ctx->authkeylen + ctx->enckeylen + rctx->iv_ctr_len + 
                          req->assoclen + rctx->iv_ctr_len + req->cryptlen + 40);

    flow_log("%s() packetlen:%u\n", __FUNCTION__, packetlen);

    return (packetlen > MAX_SPU_DATA_SIZE);
}


static void aead_complete(struct crypto_async_request *areq, int err)
{
    struct aead_request *req = container_of(areq, struct aead_request, base);
 	struct iproc_reqctx_s *rctx = aead_request_ctx(req);

    flow_log("%s() req:%p err:%d\n", __FUNCTION__, areq, err);
     
    areq->tfm = crypto_aead_tfm(crypto_aead_crt(__crypto_aead_cast(rctx->old_tfm))->base);

    areq->complete = rctx->old_complete;
    areq->data = rctx->old_data;

    areq->complete(areq, err);
}


static int aead_do_fallback(struct aead_request *req, bool isEncrypt)
{
    struct crypto_tfm *tfm = crypto_aead_tfm(crypto_aead_reqtfm(req));
 	struct iproc_reqctx_s *rctx = aead_request_ctx(req);
    struct iproc_ctx_s *ctx = crypto_tfm_ctx(tfm);
    int err;

    flow_log("%s() req:%p enc:%u\n", __FUNCTION__, req, isEncrypt);

    if (ctx->fallback_cipher) {
        /* Store the cipher tfm and then use the fallback tfm */
        rctx->old_tfm = tfm;
        aead_request_set_tfm(req, ctx->fallback_cipher);
        /* Save the callback and chain ourselves in, so we can restore the tfm */
        rctx->old_complete = req->base.complete;
        rctx->old_data = req->base.data;
        aead_request_set_callback(req, aead_request_flags(req), aead_complete, req);

        err = isEncrypt ? crypto_aead_encrypt(req) : crypto_aead_decrypt(req);
    } else
        err = -EINVAL;

    return err;
}


static int aead_enqueue(struct aead_request *req, uint8_t isEncrypt)
{
 	struct iproc_reqctx_s *rctx = aead_request_ctx(req);
	struct iproc_ctx_s *ctx = crypto_aead_ctx(crypto_aead_reqtfm(req));
	unsigned long flags;

    flow_log("%s() req:%p enc:%u\n", __FUNCTION__, req, isEncrypt);

    if (req->assoclen > 512) {
        printk("%s() Error: associated data too long. (%u > 512bytes)\n",
               __FUNCTION__, req->assoclen);
        return -EINVAL;
    }

    rctx->in_use         = false;
    rctx->isEncrypt      = isEncrypt;
    rctx->total_todo     = req->cryptlen;
    rctx->total_sent     = 0;
    rctx->src_sent       = 0;
    rctx->total_received = 0;
    rctx->is_sw_hmac     = false;
    rctx->streamID       = iproc_priv.stream_count++ % MAX_STREAMS;

    if (ctx->cipher.mode == CIPHER_MODE_CBC || ctx->cipher.mode == CIPHER_MODE_CTR) {
        rctx->iv_ctr_len = crypto_aead_ivsize(crypto_aead_reqtfm(req));
    } else {
        rctx->iv_ctr_len = 0;
    }

    rctx->nbuf = 0;

    flow_log("  iv_ctr_len:%u\n", rctx->iv_ctr_len);
    flow_dump("  iv: ", req->iv, rctx->iv_ctr_len);
    
    memcpy(rctx->iv_ctr, req->iv, rctx->iv_ctr_len);

    /* If we need authenc.c to handle the request then do it... */
    if (unlikely(aead_need_fallback(req)))
        return aead_do_fallback(req, isEncrypt);

	spin_lock_irqsave(&iproc_priv.lock, flags);
    {
        rctx->parent = &req->base;
        rctx->prev_active = iproc_priv.active_list_end;
        rctx->next_active = NULL;
        if (iproc_priv.active_list_end)
            iproc_priv.active_list_end->next_active = rctx;
        if (!iproc_priv.active_list_start)
            iproc_priv.active_list_start = rctx;
        iproc_priv.active_list_end = rctx;

        rctx->can_send = true;
    }
	spin_unlock_irqrestore(&iproc_priv.lock, flags);

    handle_active_list();

    return -EINPROGRESS;
}


static int aead_setkey(struct crypto_aead *cipher,
                       const u8 *key, unsigned keylen)
{
    struct iproc_ctx_s *ctx = crypto_aead_ctx(cipher);
    struct crypto_tfm *tfm = crypto_aead_tfm(cipher);
    struct rtattr *rta = (void *)key;
    struct crypto_authenc_key_param *param;
    const u8 *origkey = key;
    const unsigned origkeylen = keylen;
    
    int ret = 0;
    
    flow_log("%s() aead:%p key:%p keylen:%u\n", __FUNCTION__, cipher, key, keylen);
    flow_dump("  key: ", key, keylen);

    if (!RTA_OK(rta, keylen))                       goto badkey;
    if (rta->rta_type != CRYPTO_AUTHENC_KEYA_PARAM) goto badkey;
    if (RTA_PAYLOAD(rta) < sizeof(*param))          goto badkey;

    param = RTA_DATA(rta);
    ctx->enckeylen = be32_to_cpu(param->enckeylen);

    key += RTA_ALIGN(rta->rta_len);
    keylen -= RTA_ALIGN(rta->rta_len);

    if (keylen < ctx->enckeylen)        goto badkey;
    if (ctx->enckeylen > MAX_KEY_SIZE)  goto badkey;

    ctx->authkeylen = keylen - ctx->enckeylen;

    if (ctx->authkeylen > MAX_KEY_SIZE) goto badkey;

    /* TODO need to set this right for all, and handle short/long authkeys */
    ctx->digestsize = ctx->authkeylen;

    memcpy(ctx->enckey, key + ctx->authkeylen, ctx->enckeylen);
    memcpy(ctx->authkey, key, ctx->authkeylen);

    switch (ctx->cipher.alg) {
    case CIPHER_ALG_DES:
        if (ctx->enckeylen == DES_KEY_SIZE) {
            u32 tmp[DES_EXPKEY_WORDS];

            if (des_ekey(tmp, key) == 0) {
                if (crypto_aead_get_flags(cipher) & CRYPTO_TFM_REQ_WEAK_KEY) {
                    crypto_aead_set_flags(cipher, CRYPTO_TFM_RES_WEAK_KEY);
                    return -EINVAL;
                }
            }

            ctx->cipher_type = CIPHER_TYPE_DES;
        } else {
            crypto_aead_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
            return -EINVAL;
        }
        break;
    case CIPHER_ALG_3DES:
        if (ctx->enckeylen == (DES_KEY_SIZE * 3)) {
            const u32 *K = (const u32 *)key;
            if ( !((K[0] ^ K[2]) | (K[1] ^ K[3])) ||
                 !((K[2] ^ K[4]) | (K[3] ^ K[5])) ) {
                crypto_aead_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_SCHED);
                return -EINVAL;
            }

            ctx->cipher_type = CIPHER_TYPE_3DES;
        } else {
            crypto_aead_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
            return -EINVAL;
        }
        break;
    case CIPHER_ALG_AES:
        switch (ctx->enckeylen) {
        case AES_KEYSIZE_128: ctx->cipher_type = CIPHER_TYPE_AES128; break;
        case AES_KEYSIZE_192: ctx->cipher_type = CIPHER_TYPE_AES192; break;
        case AES_KEYSIZE_256: ctx->cipher_type = CIPHER_TYPE_AES256; break;
        default:
            crypto_aead_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
            return -EINVAL;
        }
        break;
    default:
        printk("%s() Error: Unknown cipher alg\n", __FUNCTION__);
        return -EINVAL;
    }

    flow_log("  enckeylen:%u authkeylen:%u\n", ctx->enckeylen, ctx->authkeylen);
    flow_dump("  enc: ", ctx->enckey, ctx->enckeylen);
    flow_dump("  auth: ", ctx->authkey, ctx->authkeylen);

    /* setkey the fallback just in case we needto use it */
    if (ctx->fallback_cipher) {
        flow_log("  running fallback setkey()\n");

        ctx->fallback_cipher->base.crt_flags &= ~CRYPTO_TFM_REQ_MASK;
        ctx->fallback_cipher->base.crt_flags |= tfm->crt_flags & CRYPTO_TFM_REQ_MASK;
        ret = crypto_aead_setkey(ctx->fallback_cipher, origkey, origkeylen);
        if (ret) {
            flow_log("  fallback setkey() returned:%d\n", ret);
            tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
            tfm->crt_flags |= (ctx->fallback_cipher->base.crt_flags & CRYPTO_TFM_RES_MASK);
        }
    }

    return ret;

badkey:
    ctx->enckeylen = 0;
    ctx->authkeylen = 0;
    ctx->digestsize = 0;

    crypto_aead_set_flags(cipher, CRYPTO_TFM_RES_BAD_KEY_LEN);
    return -EINVAL;
}


static int aead_setauthsize(struct crypto_aead *cipher,
                            unsigned authsize)
{
    struct iproc_ctx_s *ctx = crypto_aead_ctx(cipher);
    int ret = 0;

    flow_log("%s() aead:%p prev:%u authsize:%u\n",
             __FUNCTION__, cipher, ctx->authkeylen, authsize);

    ctx->authkeylen = authsize;

    /* TODO need to set this right for all, and handle short/long authkeys */
    ctx->digestsize = ctx->authkeylen;

    /* setkey the fallback just in case we needto use it */
    if (ctx->fallback_cipher) {
        flow_log("  running fallback setauth()\n");

        ret = crypto_aead_setauthsize(ctx->fallback_cipher, authsize);
        if (ret) {
            flow_log("  fallback setauth() returned:%d\n", ret);
        }
    }

    return ret;
}


static int aead_encrypt(struct aead_request *req)
{
    flow_log("%s() aead_req:%p nbytes:%u\n", __FUNCTION__, req, req->cryptlen);
    /* dump_sg(req->src, req->nbytes); */

    return aead_enqueue(req, true);
}


static int aead_decrypt(struct aead_request *req)
{
    flow_log("%s() aead_req:%p nbytes:%u\n", __FUNCTION__, req, req->cryptlen);
    /* dump_sg(req->src, req->nbytes); */

    return aead_enqueue(req,false);
}


/* ==================== Supported Cipher Algorithms ==================== */

static struct iproc_alg_s driver_algs[] = {
/* AEAD algorithms. */
    /* AES-GCM */
    /* { */
    /*     .type = CRYPTO_ALG_TYPE_AEAD, */
    /*     .alg.crypto = { */
    /*         .cra_name = "gcm(aes)", */
    /*         .cra_driver_name = "gcm-aes-iproc", */
    /*         .cra_aead = { */
    /*             .ivsize = AES_BLOCK_SIZE, */
    /*             .maxauthsize = AES_BLOCK_SIZE, */
    /*         } */
    /*     }, */
    /*     .cipher_info = { */
    /*         .alg = CIPHER_ALG_AES, */
    /*         .mode = CIPHER_MODE_CBC, */
    /*     }, */
    /*     .auth_info = { */
    /*         .alg = HASH_ALG_AES, */
    /*         .mode = HASH_MODE_GCM, */
    /*     }, */
    /*     .authFirst = false, */
    /* }, */
    /* enc -> hash - aes */
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(md5),cbc(aes))",
            .cra_driver_name = "authenc-hmac-md5-cbc-aes-iproc",
            .cra_blocksize = AES_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = AES_BLOCK_SIZE,
                .maxauthsize = MD5_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_AES,
            .mode = CIPHER_MODE_GCM,
        },
        .auth_info = {
            .alg = HASH_ALG_MD5,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(sha1),cbc(aes))",
            .cra_driver_name = "authenc-hmac-sha1-cbc-aes-iproc",
            .cra_blocksize = AES_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = AES_BLOCK_SIZE,
                .maxauthsize = SHA1_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_AES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA1,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(sha256),cbc(aes))",
            .cra_driver_name = "authenc-hmac-sha256-cbc-aes-iproc",
            .cra_blocksize = AES_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = AES_BLOCK_SIZE,
                .maxauthsize = SHA256_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_AES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA256,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    /* enc -> hash - des */
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(md5),cbc(des))",
            .cra_driver_name = "authenc-hmac-md5-cbc-des-iproc",
            .cra_blocksize = DES_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = DES_BLOCK_SIZE,
                .maxauthsize = MD5_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_MD5,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(sha1),cbc(des))",
            .cra_driver_name = "authenc-hmac-sha1-cbc-des-iproc",
            .cra_blocksize = DES_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = DES_BLOCK_SIZE,
                .maxauthsize = SHA1_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA1,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(sha256),cbc(des))",
            .cra_driver_name = "authenc-hmac-sha256-cbc-des-iproc",
            .cra_blocksize = DES_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = DES_BLOCK_SIZE,
                .maxauthsize = SHA256_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA256,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    /* enc -> hash - 3des */
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(md5),cbc(des3_ede))",
            .cra_driver_name = "authenc-hmac-md5-cbc-des3-iproc",
            .cra_blocksize = DES3_EDE_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = DES3_EDE_BLOCK_SIZE,
                .maxauthsize = MD5_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_3DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_MD5,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(sha1),cbc(des3_ede))",
            .cra_driver_name = "authenc-hmac-sha1-cbc-des3-iproc",
            .cra_blocksize = DES3_EDE_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = DES3_EDE_BLOCK_SIZE,
                .maxauthsize = SHA1_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA1,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },
    {
        .type = CRYPTO_ALG_TYPE_AEAD,
        .alg.crypto = {
            .cra_name = "authenc(hmac(sha256),cbc(des3_ede))",
            .cra_driver_name = "authenc-hmac-sha256-cbc-des3-iproc",
            .cra_blocksize = DES3_EDE_BLOCK_SIZE,
            .cra_aead = {
                .ivsize = DES3_EDE_BLOCK_SIZE,
                .maxauthsize = SHA256_DIGEST_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA256,
            .mode = HASH_MODE_HMAC,
        },
        .authFirst = false,
    },

/* ABLKCIPHER algorithms. */
    /* { */
    /*     .type = CRYPTO_ALG_TYPE_ABLKCIPHER, */
    /*     .alg.crypto = { */
    /*         .cra_name = "arc4", */
    /*         .cra_driver_name = "arc4-iproc", */
    /*         .cra_blocksize = ARC4_BLOCK_SIZE, */
    /*         .cra_ablkcipher = { */
    /*             .min_keysize = ARC4_MIN_KEY_SIZE, */
    /*             .max_keysize = ARC4_MIN_KEY_SIZE, */
    /*             .ivsize = ARC4_BLOCK_SIZE, */
    /*         } */
    /*     }, */
    /*     .cipher_info = { */
    /*         .alg = CIPHER_ALG_RC4, */
    /*         .mode = CIPHER_MODE_NONE, */
    /*     }, */
    /*     .auth_info = { */
    /*         .alg = HASH_ALG_NONE, */
    /*         .mode = HASH_MODE_NONE, */
    /*     }, */
    /* }, */
    {   
        .type = CRYPTO_ALG_TYPE_ABLKCIPHER,
        .alg.crypto = {
            .cra_name = "cbc(des)",
            .cra_driver_name = "cbc-des-iproc",
            .cra_blocksize = DES_BLOCK_SIZE,
            .cra_ablkcipher = {
                .min_keysize = DES_KEY_SIZE,
                .max_keysize = DES_KEY_SIZE,
                .ivsize = DES_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_NONE,
            .mode = HASH_MODE_NONE,
        },
    },
    {   
        .type = CRYPTO_ALG_TYPE_ABLKCIPHER,
        .alg.crypto = {
            .cra_name = "ecb(des)",
            .cra_driver_name = "ecb-des-iproc",
            .cra_blocksize = DES_BLOCK_SIZE,
            .cra_ablkcipher = {
                .min_keysize = DES_KEY_SIZE,
                .max_keysize = DES_KEY_SIZE,
                .ivsize = DES_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_DES,
            .mode = CIPHER_MODE_ECB,
        },
        .auth_info = {
            .alg = HASH_ALG_NONE,
            .mode = HASH_MODE_NONE,
        },
    },
    {   
        .type = CRYPTO_ALG_TYPE_ABLKCIPHER,
        .alg.crypto = {
            .cra_name = "cbc(des3_ede)",
            .cra_driver_name = "cbc-des3-iproc",
            .cra_blocksize = DES3_EDE_BLOCK_SIZE,
            .cra_ablkcipher = {
                .min_keysize = DES3_EDE_KEY_SIZE,
                .max_keysize = DES3_EDE_KEY_SIZE,
                .ivsize = DES3_EDE_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_3DES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_NONE,
            .mode = HASH_MODE_NONE,
        },
    },
    {   
        .type = CRYPTO_ALG_TYPE_ABLKCIPHER,
        .alg.crypto = {
            .cra_name = "ecb(des3_ede)",
            .cra_driver_name = "ecb-des3-iproc",
            .cra_blocksize = DES3_EDE_BLOCK_SIZE,
            .cra_ablkcipher = {
                .min_keysize = DES3_EDE_KEY_SIZE,
                .max_keysize = DES3_EDE_KEY_SIZE,
                .ivsize = DES3_EDE_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_3DES,
            .mode = CIPHER_MODE_ECB,
        },
        .auth_info = {
            .alg = HASH_ALG_NONE,
            .mode = HASH_MODE_NONE,
        },
    },
    {   
        .type = CRYPTO_ALG_TYPE_ABLKCIPHER,
        .alg.crypto = {
            .cra_name = "cbc(aes)",
            .cra_driver_name = "cbc-aes-iproc",
            .cra_blocksize = AES_BLOCK_SIZE,
            .cra_ablkcipher = {
                .min_keysize = AES_MIN_KEY_SIZE,
                .max_keysize = AES_MAX_KEY_SIZE,
                .ivsize = AES_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_AES,
            .mode = CIPHER_MODE_CBC,
        },
        .auth_info = {
            .alg = HASH_ALG_NONE,
            .mode = HASH_MODE_NONE,
        },
    },
    {   
        .type = CRYPTO_ALG_TYPE_ABLKCIPHER,
        .alg.crypto = {
            .cra_name = "ecb(aes)",
            .cra_driver_name = "ecb-aes-iproc",
            .cra_blocksize = AES_BLOCK_SIZE,
            .cra_ablkcipher = {
                .min_keysize = AES_MIN_KEY_SIZE,
                .max_keysize = AES_MAX_KEY_SIZE,
                .ivsize = 0,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_AES,
            .mode = CIPHER_MODE_ECB,
        },
        .auth_info = {
            .alg = HASH_ALG_NONE,
            .mode = HASH_MODE_NONE,
        },
    },
    {   
        .type = CRYPTO_ALG_TYPE_ABLKCIPHER,
        .alg.crypto = {
            .cra_name = "ctr(aes)",
            .cra_driver_name = "ctr-aes-iproc",
            .cra_blocksize = AES_BLOCK_SIZE,
            .cra_ablkcipher = {
                /* .geniv = "chainiv", */
                .min_keysize = AES_MIN_KEY_SIZE,
                .max_keysize = AES_MAX_KEY_SIZE,
                .ivsize = AES_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_AES,
            .mode = CIPHER_MODE_CTR,
        },
        .auth_info = {
            .alg = HASH_ALG_NONE,
            .mode = HASH_MODE_NONE,
        },
    },

/* AHASH algorithms. */
    {
        .type = CRYPTO_ALG_TYPE_AHASH,
        .alg.hash = {
            .halg.digestsize = MD5_DIGEST_SIZE,
            .halg.base = {
                .cra_name = "md5",
                .cra_driver_name = "md5-iproc",
                .cra_blocksize = MD5_BLOCK_WORDS * 4,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_NONE,
            .mode = CIPHER_MODE_NONE,
        },
        .auth_info = {
            .alg = HASH_ALG_MD5,
            .mode = HASH_MODE_HASH,
        },
    },
    {
        .type = CRYPTO_ALG_TYPE_AHASH,
        .alg.hash = {
            .halg.digestsize = MD5_DIGEST_SIZE,
            .halg.base = {
                .cra_name = "hmac(md5)",
                .cra_driver_name = "hmac-md5-iproc",
                .cra_blocksize = MD5_BLOCK_WORDS * 4,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_NONE,
            .mode = CIPHER_MODE_NONE,
        },
        .auth_info = {
            .alg = HASH_ALG_MD5,
            .mode = HASH_MODE_HMAC,
        },
    },
    {   .type = CRYPTO_ALG_TYPE_AHASH,
        .alg.hash = {
            .halg.digestsize = SHA1_DIGEST_SIZE,
            .halg.base = {
                .cra_name = "sha1",
                .cra_driver_name = "sha1-iproc",
                .cra_blocksize = SHA1_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_NONE,
            .mode = CIPHER_MODE_NONE,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA1,
            .mode = HASH_MODE_HASH,
        },
    },
    {   .type = CRYPTO_ALG_TYPE_AHASH,
        .alg.hash = {
            .halg.digestsize = SHA1_DIGEST_SIZE,
            .halg.base = {
                .cra_name = "hmac(sha1)",
                .cra_driver_name = "hmac-sha1-iproc",
                .cra_blocksize = SHA1_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_NONE,
            .mode = CIPHER_MODE_NONE,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA1,
            .mode = HASH_MODE_HMAC,
        },
    },
    {   .type = CRYPTO_ALG_TYPE_AHASH,
        .alg.hash = {
            .halg.digestsize = SHA256_DIGEST_SIZE,
            .halg.base = {
                .cra_name = "sha256",
                .cra_driver_name = "sha256-iproc",
                .cra_blocksize = SHA256_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_NONE,
            .mode = CIPHER_MODE_NONE,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA256,
            .mode = HASH_MODE_HASH,
        },
    },
    {   .type = CRYPTO_ALG_TYPE_AHASH,
        .alg.hash = {
            .halg.digestsize = SHA256_DIGEST_SIZE,
            .halg.base = {
                .cra_name = "hmac(sha256)",
                .cra_driver_name = "hmac-sha256-iproc",
                .cra_blocksize = SHA256_BLOCK_SIZE,
            }
        },
        .cipher_info = {
            .alg = CIPHER_ALG_NONE,
            .mode = CIPHER_MODE_NONE,
        },
        .auth_info = {
            .alg = HASH_ALG_SHA256,
            .mode = HASH_MODE_HMAC,
        },
    },
};


static int generic_cra_init(struct crypto_tfm *tfm)
{
    struct crypto_alg *alg = tfm->__crt_alg;
    struct iproc_alg_s *cipher_alg;
    struct iproc_ctx_s *ctx = crypto_tfm_ctx(tfm);

    if ((alg->cra_flags & CRYPTO_ALG_TYPE_MASK) == CRYPTO_ALG_TYPE_AHASH)
        cipher_alg = container_of(__crypto_ahash_alg(alg), struct iproc_alg_s, alg.hash);
    else
        cipher_alg = container_of(alg, struct iproc_alg_s, alg.crypto);

    ctx->cipher = cipher_alg->cipher_info;
    ctx->auth   = cipher_alg->auth_info;
    ctx->authFirst = cipher_alg->authFirst;
    
    ctx->fallback_cipher = NULL;

    ctx->enckeylen = 0;
    ctx->authkeylen = 0;

    return 0;
}


static int ablkcipher_cra_init(struct crypto_tfm *tfm)
{
    generic_cra_init(tfm);

    /* set ablkcipher_request_ctx size */
    tfm->crt_ablkcipher.reqsize = sizeof(struct iproc_reqctx_s);

    return 0;
}


static int ahash_cra_init(struct crypto_tfm *tfm)
{
    generic_cra_init(tfm);

    /* set ahash_request_ctx size */
    crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm), sizeof(struct iproc_reqctx_s));

    return 0;
}


static int aead_cra_init(struct crypto_tfm *tfm)
{
    struct iproc_ctx_s *ctx = crypto_tfm_ctx(tfm);
    struct crypto_alg *alg = tfm->__crt_alg;

    generic_cra_init(tfm);

    if (alg->cra_flags & CRYPTO_ALG_NEED_FALLBACK) {
        flow_log("%s() creating fallback cipher\n", __FUNCTION__);

        ctx->fallback_cipher = crypto_alloc_aead(alg->cra_name, 0,
                                                 CRYPTO_ALG_ASYNC |
                                                 CRYPTO_ALG_NEED_FALLBACK);
        if (IS_ERR(ctx->fallback_cipher)) {
            printk("%s() Error: failed to allocate fallback for %s\n",
                   __FUNCTION__, alg->cra_name);
            ctx->fallback_cipher = NULL;
        }
    }

    /* set aead_request_ctx size */
    tfm->crt_aead.reqsize = sizeof(struct iproc_reqctx_s);

    return 0;
}


static void aead_cra_exit(struct crypto_tfm *tfm)
{
    struct iproc_ctx_s *ctx = crypto_tfm_ctx(tfm);

    if (ctx->fallback_cipher)
        crypto_free_aead(ctx->fallback_cipher);

    ctx->fallback_cipher = NULL;
}


/* ==================== Kernel Platform API ==================== */

static int __devinit
iproc_crypto_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    int err = 0, i, j;

    flow_log("iproc-crypto: %s()\n", __FUNCTION__);

    spin_lock_init(&iproc_priv.lock);

    iproc_priv.remaining_slots = MAX_INFLIGHT;
    iproc_priv.stream_count = 0;

    iproc_priv.active_list_start = iproc_priv.active_list_end = NULL;

    /* register crypto algorithms the device supports */
    for (i = 0; i < ARRAY_SIZE(driver_algs); i++) {
        char *name = NULL;

        switch (driver_algs[i].type) {
        case CRYPTO_ALG_TYPE_ABLKCIPHER:
            driver_algs[i].alg.crypto.cra_module = THIS_MODULE;
            driver_algs[i].alg.crypto.cra_priority = 300;
            driver_algs[i].alg.crypto.cra_alignmask = 0;
            driver_algs[i].alg.crypto.cra_ctxsize = sizeof(struct iproc_ctx_s);
            INIT_LIST_HEAD(&driver_algs[i].alg.crypto.cra_list);
            break;
        case CRYPTO_ALG_TYPE_AHASH:
            driver_algs[i].alg.hash.halg.base.cra_module = THIS_MODULE;
            driver_algs[i].alg.hash.halg.base.cra_priority = 300;
            driver_algs[i].alg.hash.halg.base.cra_alignmask = 0;
            driver_algs[i].alg.hash.halg.base.cra_ctxsize = sizeof(struct iproc_ctx_s);
            break;
        case CRYPTO_ALG_TYPE_AEAD:
            driver_algs[i].alg.crypto.cra_module = THIS_MODULE;
            driver_algs[i].alg.crypto.cra_priority = 1500;
            driver_algs[i].alg.crypto.cra_alignmask = 0;
            driver_algs[i].alg.crypto.cra_ctxsize = sizeof(struct iproc_ctx_s);
            INIT_LIST_HEAD(&driver_algs[i].alg.crypto.cra_list);
            break;
        default:
            dev_err(dev, "iproc-crypto: unknown alg name: %s type: %d\n", name, driver_algs[i].type);
            err = -EINVAL;
        }

        if (!err) {
            switch (driver_algs[i].type) {
            case CRYPTO_ALG_TYPE_ABLKCIPHER:
                driver_algs[i].alg.crypto.cra_init = ablkcipher_cra_init;
                driver_algs[i].alg.crypto.cra_type = &crypto_ablkcipher_type;
                driver_algs[i].alg.crypto.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC;

                driver_algs[i].alg.crypto.cra_ablkcipher.setkey = ablkcipher_setkey,
                driver_algs[i].alg.crypto.cra_ablkcipher.encrypt = ablkcipher_encrypt,
                driver_algs[i].alg.crypto.cra_ablkcipher.decrypt = ablkcipher_decrypt,

                err = crypto_register_alg(&driver_algs[i].alg.crypto);
                name = driver_algs[i].alg.crypto.cra_driver_name;
                pr_info("  registered ablkcipher %s\n", name);
                break;
            case CRYPTO_ALG_TYPE_AHASH:
                driver_algs[i].alg.hash.halg.base.cra_init = ahash_cra_init;
                driver_algs[i].alg.hash.halg.base.cra_type = &crypto_ahash_type;
                driver_algs[i].alg.hash.halg.base.cra_flags = CRYPTO_ALG_TYPE_AHASH | CRYPTO_ALG_ASYNC;

                if (driver_algs[i].auth_info.mode != HASH_MODE_HMAC) {
                    driver_algs[i].alg.hash.init = ahash_init;
                    driver_algs[i].alg.hash.update = ahash_update;
                    driver_algs[i].alg.hash.final = ahash_final;
                    driver_algs[i].alg.hash.finup = ahash_finup;
                    driver_algs[i].alg.hash.digest = ahash_digest;
                } else {
                    driver_algs[i].alg.hash.setkey = ahash_hmac_setkey;
                    driver_algs[i].alg.hash.init = ahash_hmac_init;
                    driver_algs[i].alg.hash.update = ahash_hmac_update;
                    driver_algs[i].alg.hash.final = ahash_hmac_final;
                    driver_algs[i].alg.hash.finup = ahash_hmac_finup;
                    driver_algs[i].alg.hash.digest = ahash_hmac_digest;
                }

                err = crypto_register_ahash(&driver_algs[i].alg.hash);
                name = driver_algs[i].alg.hash.halg.base.cra_driver_name;
                pr_info("  registered ahash %s\n", name);
                break;
            case CRYPTO_ALG_TYPE_AEAD:
                driver_algs[i].alg.crypto.cra_init = aead_cra_init;
                driver_algs[i].alg.crypto.cra_exit = aead_cra_exit;
                driver_algs[i].alg.crypto.cra_type = &crypto_aead_type;
                driver_algs[i].alg.crypto.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK;
                driver_algs[i].alg.crypto.cra_aead.setkey = aead_setkey;
                driver_algs[i].alg.crypto.cra_aead.setauthsize = aead_setauthsize;
                driver_algs[i].alg.crypto.cra_aead.encrypt = aead_encrypt;
                driver_algs[i].alg.crypto.cra_aead.decrypt = aead_decrypt;

                err = crypto_register_alg(&driver_algs[i].alg.crypto);
                name = driver_algs[i].alg.crypto.cra_driver_name;
                pr_info("  registered aead %s\n", name);
                break;
            }
        }

        if (err) {
            dev_err(dev, "%s alg registration failed\n", name);
            goto err_algs;
        }
    }
    
    return 0;

err_algs:
    for (j = 0; j < i; j++) {
        switch (driver_algs[i].type) {
        case CRYPTO_ALG_TYPE_ABLKCIPHER:        
        case CRYPTO_ALG_TYPE_AEAD:
            crypto_unregister_alg(&driver_algs[j].alg.crypto);
            break;
        case CRYPTO_ALG_TYPE_AHASH:
            crypto_unregister_ahash(&driver_algs[i].alg.hash);
            break;
        }
    }

    dev_err(dev, "iproc-crypto: probe/init failed.\n");

    return err;
}


static int __devexit
iproc_crypto_remove(struct platform_device *pdev)
{
    int i;

    pr_info("iproc-crypto: remove()\n");

    for (i = 0; i < ARRAY_SIZE(driver_algs); i++) {        
        switch (driver_algs[i].type) {
        case CRYPTO_ALG_TYPE_ABLKCIPHER:        
        case CRYPTO_ALG_TYPE_AEAD:
            crypto_unregister_alg(&driver_algs[i].alg.crypto);
            pr_info("  unregistered cipher %s\n", driver_algs[i].alg.crypto.cra_driver_name);
            break;
        case CRYPTO_ALG_TYPE_AHASH:
            crypto_unregister_ahash(&driver_algs[i].alg.hash);
            pr_info("  unregistered hash %s\n", driver_algs[i].alg.hash.halg.base.cra_driver_name);
            break;
        }
    }

    pr_info("iproc-crypto: remove() done\n");

    return 0;
}


/* ===== Kernel Module API ===== */

static struct platform_driver crypto_pdriver = { 
    .driver = {
        .name   = "iproc-crypto",
        .owner  = THIS_MODULE,
    },
    .probe  = iproc_crypto_probe,
    .remove = iproc_crypto_remove,
    .remove = NULL,
    .suspend = NULL,
    .resume  = NULL,
};


static int __init iproc_crypto_init(void)
{
    int rc;

    pr_info("iproc-crypto: loading driver\n");

    if (!1) {
        pr_err("platform unsupported\n");
        return -ENODEV;
    } 
 
    rc = platform_driver_register(&crypto_pdriver);

    if (rc < 0) {
        printk(KERN_ERR "%s: Driver registration failed, error %d\n", __func__, rc);
        goto driver_reg_failed;
    }

    iproc_priv.pdev = platform_device_register_simple("iproc-crypto", -1, NULL, 0);

    if (!iproc_priv.pdev) {
        goto device_reg_failed;
    }
 
    platform_set_drvdata(iproc_priv.pdev, &iproc_priv);

    return 0;

device_reg_failed:
    platform_driver_unregister(&crypto_pdriver);

driver_reg_failed:
    return rc;
}

static void __exit iproc_crypto_exit(void)
{
    iproc_crypto_remove(iproc_priv.pdev);

    platform_device_unregister(iproc_priv.pdev);
    platform_driver_unregister(&crypto_pdriver);
}

module_init(iproc_crypto_init);
module_exit(iproc_crypto_exit);

MODULE_DESCRIPTION("iProc symmetric crypto acceleration support.");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Broadcom Corporation");
