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
 * Utility functions and prototypes
 *
 */

#ifndef _UTIL_H
#define _UTIL_H

#include <linux/kernel.h>
#include <linux/delay.h>
#include <crypto/scatterwalk.h>


extern int xor_debug_logging;
extern int xor_logging_sleep;


#define DEBUG_ON 1

#if DEBUG_ON
#define xor_log(...)                           \
    do {                                        \
        if (xor_debug_logging) {               \
            printk(__VA_ARGS__);                \
            if (xor_logging_sleep)            \
                msleep(xor_logging_sleep);    \
        }                                       \
    } while (0)
#define xor_dump(msg, var, var_len)                                    \
    do {                                                                \
        if (xor_debug_logging) {                                       \
            print_hex_dump(KERN_ALERT, msg, DUMP_PREFIX_NONE, 4, 1, var, var_len, false); \
            if (xor_logging_sleep)                                    \
                msleep(xor_logging_sleep);                            \
        }                                                               \
    } while (0)

void __xor_dump_sg(struct scatterlist *sg, unsigned len);

#define dump_sg(sg, len)     __xor_dump_sg(sg, len)

#else /* !DEBUG_ON */

#define xor_log(...)
#define xor_dump(msg, var, var_len)

#define xor_dump_sg(sg, len)

#endif /* DEBUG_ON */

/* Utility macros */
#define RETURN_ON_FAIL(do_func)                                         \
    do {                                                                \
        int _tmp = do_func;                                             \
        if (_tmp) {                                                     \
            if (msg[0]) printk(KERN_ERR "%s : %s() ret %d\n",           \
                               __func__, do_func , _tmp);               \
            return _tmp;                                                \
        }                                                               \
    } while (0)

#define NO_MSG ("")


static inline
uint32_t
xor_reg_get32(volatile void __iomem *base, unsigned offset) {
    return ioread32(base + offset);
}


static inline
void
xor_reg_set32(volatile void __iomem *base, unsigned offset, uint32_t value) {
    iowrite32(value, base + offset);
}


static inline
void
xor_reg_set_bit32(volatile void __iomem *base, unsigned offset, uint32_t value, uint32_t mask) {
    uint32_t val = ioread32(base + offset);

    val |= (value & mask);

    iowrite32(val, base + offset);
}


static inline
void
xor_reg_clr_bit32(volatile void __iomem *base, unsigned offset, uint32_t value, uint32_t mask) {
    uint32_t val = ioread32(base + offset);

    val &= ~(value & mask);

    iowrite32(val, base + offset);
}


/* Copy sg data, from skip, length len, to dest */
void xor_copy_part_to_buf(struct scatterlist *src, u8 *dest,
                         unsigned int len, unsigned skip);
/* Copy src into scatterlist from offset, length len */
void xor_copy_part_from_buf(struct scatterlist *dest, u8 *src,
                           unsigned len, unsigned skip);

#endif /* _UTIL_H */
