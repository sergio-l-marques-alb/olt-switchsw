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


#include "xor_util.h"


/* Copy sg data, from skip, length len, to dest */
void xor_copy_part_to_buf(struct scatterlist *src, u8 *dest,
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
void xor_copy_part_from_buf(struct scatterlist *dest, u8 *src,
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


void __xor_dump_sg(struct scatterlist *sg, unsigned len)
{
    uint8_t dbuf[16];
    unsigned idx = 0;
    unsigned count;
   
    if (xor_debug_logging) {
        while (idx < len) {
            count = (len - idx > 16) ? 16 : len - idx;
            xor_copy_part_to_buf(sg, dbuf, count , idx);
            print_hex_dump(KERN_ALERT, "  sg: ", DUMP_PREFIX_NONE, 4, 1, dbuf, count, false);
            idx += 16;
        }
    }
    if (xor_logging_sleep)
        msleep(xor_logging_sleep);
}
