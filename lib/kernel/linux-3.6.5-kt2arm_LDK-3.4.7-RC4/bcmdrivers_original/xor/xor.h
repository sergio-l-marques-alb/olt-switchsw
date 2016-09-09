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

#ifndef _IPROC_XOR_H
#define _IPROC_XOR_H

#include <linux/types.h>
#include <linux/workqueue.h>

#define XOR_MIN_BYTE_COUNT   (128)
#define XOR_MAX_BYTE_COUNT   ((16 * 1024 * 1024) - 1)

#define XOR_MAX_DESCRIPTORS  32
#define XOR_MAX_SOURCES      4

#define XOR_SELF_TEST_NUM_SRCS   4     /* <= XOR_MAX_SOURCES */


/**
 * struct xor_chan_s - iproc-xor XOR channel
 * @common: the corresponding xor channel in async_tx               
 * @desc_lock: lock for tx queue
 * @total_desc: number of descriptors allocated
 * @submit_q: queue of submitted descriptors
 * @pending_q: queue of pending descriptors
 * @in_progress_q: queue of descriptors in progress
 * @free_q: queue of unused descriptors
 */
struct xor_chan_s {
    struct dma_chan  common;

    struct list_head submit_q;
    struct list_head pending_q;
    struct list_head in_progress_q;
    struct list_head free_q;

    int              total_desc;

    spinlock_t       desc_lock;
};


/**
 * xor_desc_s - XOR descriptor
 * @async_tx: the referring async_tx descriptor
 * @node:
 * @htx_list:
 * @unmap_src_cnt: number of xor sources
 * @unmap_len: transaction byte count
 * @idx: index of xor sources
 */
struct xor_desc_s {
    struct dma_async_tx_descriptor async_tx;

    struct list_head node;
    struct list_head tx_list;

    dma_addr_t       dest;
    dma_addr_t       srcs[XOR_MAX_SOURCES];
    unsigned int     src_cnt;
    unsigned int     len;
    unsigned int     idx;
};


struct xor_device_private_s {
    struct platform_device *pdev;

    struct dma_device       dma_dev;
    struct xor_chan_s       xor_chan;
    volatile void __iomem  *base_ptr;
    unsigned long           flags;
    bool                    enabled;
};

extern struct xor_device_private_s xor_priv;

#endif /* _IPROC_XOR_H */
