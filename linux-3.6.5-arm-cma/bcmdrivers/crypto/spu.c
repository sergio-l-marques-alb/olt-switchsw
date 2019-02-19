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
 * This file generates and parses SPU request packets based on a given context
 *
 */


#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/random.h>
#include <linux/in.h>
#include <linux/delay.h>
#include <linux/if_ether.h>
#include <crypto/scatterwalk.h>

#include "util.h"
#include "spu.h"


struct sk_buff *spu_create_request(bool pae_chaining, uint8_t pae_streamid,
                                   unsigned isInbound, unsigned authFirst,
                                   CIPHER_ALG cipher_alg, CIPHER_MODE cipher_mode, CIPHER_TYPE cipher_type,
                                   uint8_t *cipher_key_buf, unsigned cipher_key_len,
                                   uint8_t *cipher_iv_buf, unsigned cipher_iv_len,
                                   HASH_ALG auth_alg, HASH_MODE auth_mode, HASH_TYPE auth_type,
                                   uint8_t digestsize,
                                   uint8_t *auth_key_buf, unsigned auth_key_len,
                                   uint16_t prev_length_blocks,
                                   struct scatterlist *assoc, unsigned assoc_start, unsigned assoc_size,
                                   uint8_t *prebuf, unsigned prebuf_len,
                                   struct scatterlist *data, unsigned data_start, unsigned data_size,
                                   unsigned hash_pad_len, unsigned total_sent,
                                   uint8_t *icv_buf, unsigned icv_buf_len,
                                   uint8_t *iv_buf, unsigned iv_buf_len)
{
    SPUHEADER *spuh;
    BDESC_HEADER *bdesc;
    BD_HEADER *bd;

    struct sk_buff *skb;

    uint8_t *ptr;
    pae_ctx_type context_optype = CTX_NONE;
    uint32_t protocol_bits = 0;
    uint32_t cipher_bits = 0;
    uint32_t ecf_bits = 0;
    uint8_t  sctx_words = 0;
    int8_t delta_size = 0;
    unsigned buf_len = 0;

    /* size of the padding added in the BD when needed to finalize a hash */
    unsigned padding_size = (1 + hash_pad_len + sizeof(uint64_t));
    /* size of the BD */
    unsigned real_data_size = (hash_pad_len) ? (data_size + prebuf_len + padding_size) : (data_size + prebuf_len);
    /* padding to align the status word */
    unsigned padding = ((real_data_size + 3) & ~3) - real_data_size;

    flow_log("%s()\n", __FUNCTION__);
    flow_log("  pae: chain:%u streamid:%u\n", pae_chaining, pae_streamid);
    flow_log("  in:%u authFirst:%u\n", isInbound, authFirst);
    flow_log("  cipher alg:%u mode:%u type %u\n", cipher_alg, cipher_mode, cipher_type);
    flow_log("  key: %d\n", cipher_key_len);
    flow_dump("  key: ", cipher_key_buf, cipher_key_len);
    flow_log("  iv: %d\n", cipher_iv_len);
    flow_dump("  iv: ", cipher_iv_buf, cipher_iv_len);
    flow_log("  auth alg:%u mode:%u type %u\n", auth_alg, auth_mode, auth_type);
    flow_log("  digestsize: %u\n", digestsize);
    flow_log("  authkey: %d\n", auth_key_len);
    flow_dump("  authkey: ", auth_key_buf, auth_key_len);
    flow_log("  prev_length_blocks: %u\n", prev_length_blocks);
    flow_log("  assoc:%p, assoc_start:%u, assoc_size:%u\n", assoc, assoc_start, assoc_size);
    flow_log("  prebuf:%p, prebuf_len:%u\n", prebuf, prebuf_len);
    flow_log("  data:%p, data_start:%u data_size:%u\n", data, data_start, data_size);
    flow_log("    hash_pad:%u padsz:%u total_sent:%u real_data_size:%u\n",
             hash_pad_len, padding_size, total_sent, real_data_size);
    flow_log("  icv: %u\n", icv_buf_len);
    flow_dump("  icv: ", icv_buf, icv_buf_len);
    flow_log("  iv: %u\n", iv_buf_len);
    flow_dump("  iv: ", iv_buf, iv_buf_len);

    skb = alloc_skb(ETH_FRAME_LEN, GFP_KERNEL);
    if (!skb) {
        printk(KERN_WARNING "make_spu_request(): skb memory squeeze, dropping.\n");
        return NULL;
    }

    /* reserve space for bcm header type 3 */
    skb_reserve(skb, 8);
    /* reserve space for SPU header + data + 0-3 padding + status + hash padding */
    skb_put(skb, sizeof(SPUHEADER));
    ptr = skb->data;
    
    /* starting out: zero the header (plus some) */
    memset(ptr, 0, sizeof(SPUHEADER));
    
    /* format master header word */
    /* Do not set the next bit even though the datasheet says to */
    spuh = (SPUHEADER *)ptr;
    ptr += sizeof(SPUHEADER); buf_len += sizeof(SPUHEADER);

    spuh->mh.opCode = SPU_CRYPTO_OPERATION_GENERIC;
    spuh->mh.flags.SCTX_PR = 1;
    spuh->mh.flags.BDESC_PR = 1;
    spuh->mh.flags.BD_PR = 1;

    /* EMH is blank here, but the format is:
     * Byte 0: 0x80 is FA2_SPU_TRNS_ID_DRIVER_MASK bit showing this is one of our packets
     *       : 0x40 Load context bit
     *       : 0x3F Next 6 bits are for the index
     * Byte 1: 0xF0 stream ID
     *         0x0F operation type
     *         0x00 None
     *         0x01 AES128     0x02 AES192     0x03 AES256    0x04 DES     0x05 3DES
     *         0x06 MD5        0x07 SHA1       0x08 SHA224    0x09 SHA256
     * Byte 2: 0x80 Final hash packet
     *         0x40 Unused bit
     *         0x3F Length of packet padding used to ensure pkt_len > 64
     * Byte 3: Response size delta from actual request size without padding
     */
    /* TODO: optimize chaining, don't always set optype */
    /* if (pae_chaining) { */
    switch (cipher_alg) {
    case CIPHER_ALG_DES:  context_optype = CTX_DES; break;
    case CIPHER_ALG_3DES: context_optype = CTX_3DES; break;
    case CIPHER_ALG_AES:
        switch (cipher_type) {
        case CIPHER_TYPE_AES128: context_optype = CTX_AES128; break;
        case CIPHER_TYPE_AES192: context_optype = CTX_AES192; break;
        case CIPHER_TYPE_AES256: context_optype = CTX_AES256; break;
        }
        break;
    case CIPHER_ALG_RC4:
    case CIPHER_ALG_NONE:
        break;
    }
    
    switch (auth_alg) {
    case HASH_ALG_MD5:    context_optype = CTX_MD5; break;
    case HASH_ALG_SHA1:   context_optype = CTX_SHA1; break;
    case HASH_ALG_SHA224: context_optype = CTX_SHA224; break;
    case HASH_ALG_SHA256: context_optype = CTX_SHA256; break;
    case HASH_ALG_AES:
    case HASH_ALG_NONE:
        break;
    }
    /* } */

    skb->data[4] |= (pae_chaining << 6);
    skb->data[5] = (pae_streamid << 4) + context_optype;
    flow_log("  pae ctl: 0x%x 0x%x\n", skb->data[4], skb->data[5]);

    /* Format sctx word 0 (protocol_bits) */
    protocol_bits = SPU_SCTX_TYPE_GENERIC << SCTX_TYPE_SHIFT;
    sctx_words = 3; /* size so far in words */

    /* Format sctx word 1 (cipher_bits) */
    if (isInbound) {
        cipher_bits |= 1 << CIPHER_INBOUND_SHIFT;
    }
    if (authFirst) {
        cipher_bits |= 1 << CIPHER_ORDER_SHIFT;
    }

    /* Set the crypto parameters in the cipher.flags */
    cipher_bits |= cipher_alg << CIPHER_ALG_SHIFT;
    cipher_bits |= cipher_mode << CIPHER_MODE_SHIFT;
    cipher_bits |= cipher_type << CIPHER_TYPE_SHIFT;

    /* Set the auth parameters in the cipher.flags */
    cipher_bits |= auth_alg << HASH_ALG_SHIFT;
    cipher_bits |= auth_mode << HASH_MODE_SHIFT;
    cipher_bits |= auth_type << HASH_TYPE_SHIFT;

    /* Format sctx word 2 (ecf_bits) */
    if (cipher_alg) {
        /* if encrypting then set IV size, use SCTX IV unless no IV given here */
        if (cipher_iv_buf && cipher_iv_len) {
            /* Use SCTX IV */
            ecf_bits |= 1 << SCTX_IV_SHIFT;
        }
    }

    /* Format sctx extensions if required, and update main fields if required) */
    if (auth_alg) {
        /* Write the authentication key material */
        skb_put(skb, auth_key_len);
        memcpy(ptr, auth_key_buf, auth_key_len);

        ptr += auth_key_len; buf_len += auth_key_len;
        sctx_words += auth_key_len/4;
        
        ecf_bits |= (digestsize / 4) << ICV_SIZE_SHIFT;
    }        

    /* copy the encryption keys in the SAD entry */
    if (cipher_alg) {
        skb_put(skb, cipher_key_len);
        memcpy(ptr, cipher_key_buf, cipher_key_len);

        ptr += cipher_key_len; buf_len += cipher_key_len;
        sctx_words += cipher_key_len/4;

        if (cipher_iv_buf && cipher_iv_len) {
            /* cipher iv provided so put it in here */
            skb_put(skb, cipher_iv_len);
            memcpy(ptr, cipher_iv_buf, cipher_iv_len);

            ptr += cipher_iv_len;  buf_len += cipher_iv_len;
            sctx_words += cipher_iv_len/4;
        }
    }

    /* write in the total sctx length now that we know it */
    protocol_bits |= sctx_words;

    /* Endian adjust the SCTX */
    spuh->sa.protocol.bits = htonl(protocol_bits);
    spuh->sa.cipher.bits = htonl(cipher_bits);
    spuh->sa.ecf.bits = htonl(ecf_bits);

    delta_size -= sctx_words * 4;  /* SCTX is not present in output */

    /* === create the BDESC section === */
    skb_put(skb, sizeof(BDESC_HEADER));
    bdesc = (BDESC_HEADER *)ptr;

    bdesc->offsetMAC  = htons(0);
    bdesc->lengthMAC = htons(0);
    bdesc->offsetCrypto = htons(assoc_size + icv_buf_len + iv_buf_len);
    bdesc->lengthCrypto = htons(real_data_size);
    bdesc->offsetICV = htons(0);
    bdesc->offsetIV = htons(icv_buf_len);
        
    ptr += sizeof(BDESC_HEADER); buf_len += sizeof(BDESC_HEADER);
    delta_size -= sizeof(BDESC_HEADER);  /* BDESC is not present in output */

    /* === no MFM section === */

    /* === create the BD section === */

    /* add the header */
    skb_put(skb, sizeof(BD_HEADER) + assoc_size + icv_buf_len + iv_buf_len + real_data_size);
    bd = (BD_HEADER*)ptr;

    bd->size = htons(assoc_size + icv_buf_len + iv_buf_len + real_data_size);
    bd->PrevLength = htons(prev_length_blocks);

    ptr += sizeof(BD_HEADER); buf_len += sizeof(BD_HEADER);

    /* copy in assoc data, if present */
    if (assoc_size) {
        sg_copy_part_to_buf(assoc, ptr, assoc_size, assoc_start);
        ptr += assoc_size; buf_len += assoc_size;
    }

    /* copy in ICV, if present */
    if (icv_buf_len) {
        memcpy(ptr, icv_buf, icv_buf_len);
        ptr += icv_buf_len; buf_len += icv_buf_len;
    }
    
    /* copy in IV, if present */
    if (iv_buf_len) {
        memcpy(ptr, iv_buf, iv_buf_len);
        ptr += iv_buf_len; buf_len += iv_buf_len;
    }

    /* copy in the prepend buffer */
    memcpy(ptr, prebuf, prebuf_len);
    ptr += prebuf_len; buf_len += prebuf_len;

    /* copy in the data */
    sg_copy_part_to_buf(data, ptr, data_size, data_start);
    ptr += data_size; buf_len += data_size;
    
    /* add hash padding if requested */
    if (hash_pad_len) {
        /* indicate that this is a final hash */
        skb->data[6] |= 0x80;

        *ptr = 0x80;
        memset(ptr+1, 0, hash_pad_len);
        ptr += (hash_pad_len + 1); buf_len += (hash_pad_len + 1);

        /* add the size in as required per alg */
        if (auth_alg == HASH_ALG_MD5)
            *(uint64_t*)ptr = cpu_to_le64((u64)total_sent * 8);
        else  /* SHA1, SHA2-224, SHA2-256 */
            *(uint64_t*)ptr = cpu_to_be64((u64)total_sent * 8);
        ptr += sizeof(uint64_t); buf_len += sizeof(uint64_t);
    }

    /* pad to a 4byte alignment */
    if (padding) {
        flow_log("  padding to 4byte alignment: %u bytes\n", padding);

        skb_put(skb, padding);
        memset(ptr, 0, padding);
        ptr += padding; buf_len += padding;
    }

    /* === no HASH section from the host (only received) === */
    if (auth_alg) {
        delta_size += digestsize;  /* but HASH section will be added */
    }

    /* === add the STAT section === */
    skb_put(skb, 4);
    memset(ptr, 0, 4);
    ptr += 4; buf_len += 4;

    /* EMH: Use the emh to store the change in size due to SPU processing */
    skb->data[7] = delta_size;
    
    /* Enforce a minimum frame size */
    if (buf_len < MIN_SPU_PKT_SIZE) {
        if ((MIN_SPU_PKT_SIZE - buf_len) > 63) {
            printk("%s() Error: oversized packet padding.\n", __FUNCTION__);
        } else {
            skb_put(skb, MIN_SPU_PKT_SIZE - buf_len);
            memset(skb->data + buf_len, 0, (MIN_SPU_PKT_SIZE - buf_len));
            /* store padding size in third byte of EMH.  PAE FW will extract it to remove padding pre-SPU */
            skb->data[6] |= (MIN_SPU_PKT_SIZE - buf_len) & 0x3f;
            buf_len += (MIN_SPU_PKT_SIZE - buf_len);
        }
    }

    packet_log("  skb:%d\n", skb->len);
    packet_dump("  skb: ", skb->data, buf_len);

    return skb;
}
