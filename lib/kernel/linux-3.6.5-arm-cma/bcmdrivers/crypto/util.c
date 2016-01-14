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
 * This file implements utility functions
 *
 */


#include "cipher.h"

#include "util.h"


/* Copy sg data, from skip, length len, to dest */
void sg_copy_part_to_buf(struct scatterlist *src, u8 *dest,
                         unsigned int len, unsigned skip)
{
    unsigned index = 0;
    unsigned next_index;
    unsigned copied_len = 0;
    unsigned end = skip + len;
    unsigned long flags;

    local_irq_save(flags);

    next_index = src->length;

    while (src && next_index < skip) {
        src = scatterwalk_sg_next(src);
        index = next_index;

        if (!src) break;
        next_index += src->length;
    }

    while (src && index < end) {
        unsigned offset_from_page_start = skip - index;
        unsigned page_copy_len = min(end - index, src->length) - offset_from_page_start;

        memcpy(dest + copied_len, (u8 *) sg_virt(src) + offset_from_page_start, page_copy_len);

        copied_len += page_copy_len;
        src = scatterwalk_sg_next(src);
        skip = index = next_index;

        if (!src) break;
        next_index += src->length;
    }

    local_irq_restore(flags);
}


/* Copy src into scatterlist from offset, length len */
void sg_copy_part_from_buf(struct scatterlist *dest, u8 *src,
                           unsigned len, unsigned skip)
{
    unsigned index = 0;
    unsigned next_index;
    unsigned end = skip + len;
    unsigned copied_len = 0;
    unsigned long flags;

    local_irq_save(flags);

    next_index = dest->length;

    while (dest && next_index < skip) {
        dest = scatterwalk_sg_next(dest);
        index = next_index;

        if (!dest) break;
        next_index += dest->length;
    }

    while (dest && index < end) {
        unsigned offset_from_page_start = skip - index;
        unsigned page_copy_len = min(end - index, dest->length) - offset_from_page_start;

        memcpy((u8 *) sg_virt(dest) + offset_from_page_start, src + copied_len, page_copy_len);

        copied_len += page_copy_len;
        dest = scatterwalk_sg_next(dest);
        skip = index = next_index;

        if (!dest) break;
        next_index += dest->length;
    }

    local_irq_restore(flags);
}


void add_to_ctr(uint8_t* ctr_pos, unsigned increment)
{
    __be64 *high_be = (__be64*)ctr_pos;
    __be64 *low_be = high_be + 1;
    uint64_t orig_low = __be64_to_cpu(*low_be);
    uint64_t new_low = orig_low + (uint64_t)increment;

    *low_be = __cpu_to_be64(new_low);
    if (new_low < orig_low) {
        /* there was a carry from the low 8 bytes */
        *high_be = __cpu_to_be64(__be64_to_cpu(*high_be) + 1);
    }
}

struct sdesc {
         struct shash_desc shash;
         char ctx[];
};

/* produce a message digest from data of length n bytes */
int do_shash(unsigned char *name, unsigned char *result,
             const uint8_t *data1, unsigned data1_len,
             const uint8_t *data2, unsigned data2_len)
{
    int rc;
    unsigned size;
    struct crypto_shash *hash;
    struct sdesc *sdesc;

    hash = crypto_alloc_shash(name, 0, 0);
    if (IS_ERR(hash)) {
        rc = PTR_ERR(hash);
        printk("%s: Crypto %s allocation error %d", __func__, name, rc);
        return rc;
    }

    size = sizeof(struct shash_desc) + crypto_shash_descsize(hash);
    sdesc = kmalloc(size, GFP_KERNEL);
    if (!sdesc) {
        rc = -ENOMEM;
        printk("%s: Memory allocation failure", __func__);
        goto do_shash_err;
    }
    sdesc->shash.tfm = hash;
    sdesc->shash.flags = 0x0;
 
    rc = crypto_shash_init(&sdesc->shash);
    if (rc) {
        printk("%s: Could not init %s shash", __func__, name);
        goto do_shash_err;
    }
    rc = crypto_shash_update(&sdesc->shash, data1, data1_len);
    if (rc) {
        printk("%s: Could not update1", __func__);
        goto do_shash_err;
    }
    if (data2 && data2_len) {
        rc = crypto_shash_update(&sdesc->shash, data2, data2_len);
        if (rc) {
            printk("%s: Could not update2", __func__);
            goto do_shash_err;
        }
    }
    rc = crypto_shash_final(&sdesc->shash, result);
    if (rc)
        printk("%s: Could not genereate %s hash", __func__, name);
 
do_shash_err:
    crypto_free_shash(hash);
    kfree(sdesc);
 
    return rc;
}


void __dump_active_list(void)
{
    struct iproc_reqctx_s *c = iproc_priv.active_list_start, *prev = 0;
    int idx = 0;

    if (flow_debug_logging) {
        printk(KERN_ALERT "start: %p, end: %p\n", iproc_priv.active_list_start, iproc_priv.active_list_end);
        while (c) {
            if (c->prev_active != prev) {
                printk(KERN_ALERT "  BAD PREV PTR ON NEXT\n");
            }
            printk(KERN_ALERT "  %d : %p (can_send:%u, remaining:%u totsent:%u, srcsent:%u recv=%u, tot:%u\n",
                   idx++, c, c->can_send, iproc_priv.remaining_slots, c->total_sent, c->src_sent, c->total_received, c->total_todo);
            prev = c;
            c = c->next_active;
        }
    }
    if (debug_logging_sleep)
        msleep(debug_logging_sleep);
}


void __dump_sg(struct scatterlist *sg, unsigned len)
{
    uint8_t dbuf[16];
    unsigned idx = 0;
    unsigned count;
   
    if (flow_debug_logging) {
        while (idx < len) {
            count = (len - idx > 16) ? 16 : len - idx;
            sg_copy_part_to_buf(sg, dbuf, count , idx);
            print_hex_dump(KERN_ALERT, "  sg: ", DUMP_PREFIX_NONE, 4, 1, dbuf, count, false);
            idx += 16;
        }
    }
    if (debug_logging_sleep)
        msleep(debug_logging_sleep);
}
