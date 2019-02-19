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
#include <crypto/hash.h>
#include <crypto/scatterwalk.h>


extern int flow_debug_logging;
extern int packet_debug_logging;

extern int debug_logging_sleep;


#define DEBUG_ON 1

#if DEBUG_ON
#define flow_log(...)                           \
    do {                                        \
        if (flow_debug_logging) {               \
            printk(__VA_ARGS__);                \
            if (debug_logging_sleep)            \
                msleep(debug_logging_sleep);    \
        }                                       \
    } while (0)
#define flow_dump(msg, var, var_len)                                    \
    do {                                                                \
        if (flow_debug_logging) {                                       \
            print_hex_dump(KERN_ALERT, msg, DUMP_PREFIX_NONE, 4, 1, var, var_len, false); \
            if (debug_logging_sleep)                                    \
                msleep(debug_logging_sleep);                            \
        }                                                               \
    } while (0)

#define packet_log(...)                         \
    do {                                        \
        if (packet_debug_logging) {             \
            printk(__VA_ARGS__);                \
            if (debug_logging_sleep)            \
                msleep(debug_logging_sleep);    \
        }                                       \
    } while (0)
#define packet_dump(msg, var, var_len)                                  \
    do {                                                                \
        if (packet_debug_logging) {                                     \
            print_hex_dump(KERN_ALERT, msg, DUMP_PREFIX_NONE, 4, 1, var, var_len, false); \
            if (debug_logging_sleep)                                    \
                msleep(debug_logging_sleep);                            \
        }                                                               \
    } while (0)


void __dump_active_list(void);
void __dump_sg(struct scatterlist *sg, unsigned len);

#define dump_active_list()   __dump_active_list()
#define dump_sg(sg, len)     __dump_sg(sg, len)

#else /* !DEBUG_ON */

#define flow_log(...)
#define flow_dump(msg, var, var_len)
#define packet_log(...)
#define packet_dump(msg, var, var_len)

#define dump_active_list()
#define dump_sg(sg, len)

#endif /* DEBUG_ON */

/* Copy sg data, from skip, length len, to dest */
void sg_copy_part_to_buf(struct scatterlist *src, u8 *dest,
                         unsigned int len, unsigned skip);
/* Copy src into scatterlist from offset, length len */
void sg_copy_part_from_buf(struct scatterlist *dest, u8 *src,
                           unsigned len, unsigned skip);

void add_to_ctr(uint8_t* ctr_pos, unsigned increment);

/* produce a message digest from data of length n bytes */
int do_shash(unsigned char *name, unsigned char *result,
             const uint8_t *data1, unsigned data1_len,
             const uint8_t *data2, unsigned data2_len);

#endif /* _UTIL_H */
