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
 * This file implements xor offload (through the PAE)
 *
 */

#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/rtnetlink.h>
#include <linux/raid/xor.h>

#include <linux/dmaengine.h>
#include <drivers/dma/dmaengine.h>

#include "xor_util.h"
#include "../pae/paemsg/pae_xor.h"

#include "xor.h"


#define PAE_REG_BASE                                  0x18049000
#define PAE_INTERRUPT                                 PAE_REG_BASE
#define PAE_INTERRUPT_OFFSET                          0x0
#define PAE_INTERRUPT__PAE_MESSAGE_AVAIL_TO_HOST      (1 << 12)
#define PAE_INTERRUPT_MASK                            0x18049004
#define PAE_INTERRUPT_MASK_OFFSET                     0x4
#define PAE_INTERRUPT_MASK__PAE_MESSAGE_AVAIL_TO_HOST (1 << 12)
#define PAE_MESSAGE_OUT_MAILBOX                       0x18049008
#define PAE_MESSAGE_OUT_MAILBOX_OFFSET                0x8
#define PAE_MESSAGE_OUT_MAILBOX__MESSAGE_READY        (1 << 31)
#define PAE_MESSAGE_OUT_MAILBOX__MESSAGE              (1 << 0)
#define PAE_MESSAGE_OUT_MAILBOX__MESSAGE_WIDTH        30
#define PAE_MESSAGE_IN_MAILBOX                         0x1804900C
#define PAE_MESSAGE_IN_MAILBOX_OFFSET                  0xC
#define PAE_MESSAGE_IN_MAILBOX__MESSAGE_READY         (1 << 31)
#define PAE_MESSAGE_IN_MAILBOX__MESSAGE               (1 << 0)
#define PAE_MESSAGE_IN_MAILBOX__MESSAGE_WIDTH         30


/* ================= Device Structure ================== */

struct xor_device_private_s xor_priv;

static void xor_process_pending(struct xor_chan_s *xor_chan);
static dma_cookie_t xor_async_tx_submit(struct dma_async_tx_descriptor *tx);
static int xor_xor_self_test(void);

int xor_enable_device(bool full);
void xor_disable_device(void);


/* ================= Utility ================== */

void
dump_qs(unsigned lock)
{
    struct xor_chan_s *xor_chan = &xor_priv.xor_chan;
    struct xor_desc_s *desc;
    unsigned long flags;

    xor_log("Q-dump descs:%d", xor_chan->total_desc);

    if (lock) spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        xor_log("\n  subm_q:");
        list_for_each_entry(desc, &xor_chan->submit_q, node) {
            xor_log(" %p", desc);
        }
        xor_log("\n  pend_q:");
        list_for_each_entry(desc, &xor_chan->pending_q, node) {
            xor_log(" %p", desc);
        }
        xor_log("\n  prog_q:");
        list_for_each_entry(desc, &xor_chan->in_progress_q, node) {
            xor_log(" %p", desc);
        }
        xor_log("\n  free_q:");
        list_for_each_entry(desc, &xor_chan->free_q, node) {
            xor_log(" %p", desc);
        }
        xor_log("\n");        
    }
    if (lock) spin_unlock_irqrestore(&xor_chan->desc_lock, flags);
}


/* ==================== Parameters ===================== */

struct xor_attr_s {
    struct attribute attr;
    int value;
};

static struct xor_attr_s xor_enable = {
    .attr.name="enable",
    .attr.mode = 0644,
    .value = 0,
};

static struct attribute * xor_attrs[] = {
    &xor_enable.attr,
    NULL
};


static ssize_t
xor_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    struct xor_attr_s *a = container_of(attr, struct xor_attr_s, attr);

    return scnprintf(buf, PAGE_SIZE, "%d\n", a->value);
}


/**
 * 0 - disable
 * 1 - enable
 * 2 - enable all DMA functionality (slower)
 **/
static ssize_t
xor_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len)
{
    struct xor_attr_s *a = container_of(attr, struct xor_attr_s, attr);

    sscanf(buf, "%d", &a->value);

    if (a->value) {
        xor_enable_device(a->value == 2);
    } else {
        pr_warn("iproc_xor: cannot disable, must remove the module.\n");
    }

    return len;
}


int xor_debug_logging = 0;
int xor_logging_sleep = 0;

module_param(xor_debug_logging, int, 0644);
MODULE_PARM_DESC(xor_debug_logging, "Enable XOR Debug Logging");

module_param(xor_logging_sleep, int, 0644);
MODULE_PARM_DESC(xor_logging_sleep, "XOR Debug Logging Sleep");

unsigned xor_op_counts[6] = {0, 0, 0, 0, 0, 0};
unsigned long xor_bytes_through = 0;

module_param_array(xor_op_counts, uint, NULL, 0644);
MODULE_PARM_DESC(xor_op_counts, "XOR Operation Counts: xor");

module_param(xor_bytes_through, ulong, 0644);
MODULE_PARM_DESC(xor_bytes_through, "XOR Bytes Processed");


/* ==================== Queue Tasks  ==================== */

static
irqreturn_t
xor_irq_handler(int irq, void *dev_id)
{
    struct xor_chan_s     *xor_chan = &xor_priv.xor_chan;
    struct xor_desc_s     *desc = NULL;
    unsigned long flags;

    xor_log("%s\n", __func__);

    /* reset the message and notify flag */
    xor_reg_set32(xor_priv.base_ptr, PAE_MESSAGE_OUT_MAILBOX_OFFSET, 0x0);
    barrier();

    /* reset the irq reg */
    xor_reg_set32(xor_priv.base_ptr, PAE_INTERRUPT_OFFSET, 0x1000);

    /* pull the finised entry */
    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        if  (!list_empty(&xor_chan->in_progress_q)) {
            desc = list_entry(xor_chan->in_progress_q.next, struct xor_desc_s, node);
            list_del(&desc->node);
        }

        dma_cookie_complete(&desc->async_tx);
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    /* Handle this processed desc if there is one */
    if (desc) {
        xor_log("%s finished desc:%p cb:%p cb_p:%p\n",
                __func__, desc, desc->async_tx.callback, desc->async_tx.callback_param);

        if (desc->async_tx.callback) {
            xor_log("%s callback\n", __func__);
            desc->async_tx.callback(desc->async_tx.callback_param);
        }

        dma_run_dependencies(&desc->async_tx);

        if (!(desc->async_tx.flags & DMA_COMPL_SKIP_DEST_UNMAP)) {
            enum dma_data_direction dir;

            if (desc->src_cnt > 1) /* is xor ? */
                dir = DMA_BIDIRECTIONAL;
            else
                dir = DMA_FROM_DEVICE;

            dma_unmap_page(&xor_priv.pdev->dev, desc->dest, desc->len, dir);
        }

        if (!(desc->async_tx.flags & DMA_COMPL_SKIP_SRC_UNMAP)) {
            while (desc->src_cnt--) {
                if (desc->srcs[desc->src_cnt] == desc->dest)
                    continue;

                dma_unmap_page(&xor_priv.pdev->dev, desc->srcs[desc->src_cnt], desc->len, DMA_TO_DEVICE);
            }
        }

        /* move desc to the free pool */
        spin_lock_irqsave(&xor_chan->desc_lock, flags);
        {
            list_add_tail(&desc->node, &xor_chan->free_q);
            xor_bytes_through += desc->len;
        }
        spin_unlock_irqrestore(&xor_chan->desc_lock, flags);
    }

    /* Kick off the next if present */
    xor_process_pending(xor_chan);

    xor_log("%s done\n", __func__);

    return IRQ_HANDLED;
}


/* ==================== Helper Functions ===================== */

static
struct xor_desc_s *
xor_alloc_descriptor(struct xor_chan_s *xor_chan, gfp_t flags)
{   
    struct xor_desc_s *desc;

    desc = kmalloc(sizeof(*desc), flags);
    if (desc) {
        xor_chan->total_desc++;
        desc->async_tx.tx_submit = xor_async_tx_submit;
    }

    return desc;
}


/**
 * @device_alloc_chan_resources: allocate resources and return the
 *  number of allocated descriptors
 */
static
int
xor_alloc_chan_resources(struct dma_chan *chan)
{
    struct xor_chan_s *xor_chan = container_of(chan, struct xor_chan_s, common);
    struct xor_desc_s *desc;
    LIST_HEAD(tmp_list);
    int i;
    unsigned long flags;

    xor_log("%s chan:%p\n", __func__, chan);

    if (!list_empty(&xor_chan->free_q))
        return xor_chan->total_desc;

    for (i = 0; i < XOR_MAX_DESCRIPTORS; i++) {
        desc = xor_alloc_descriptor(xor_chan, GFP_KERNEL | GFP_DMA);
        if (!desc) {
            dev_err(xor_chan->common.device->dev, "Only %d initial descriptors\n", i);
            break;
        }
        list_add_tail(&desc->node, &tmp_list);
    }

    if (!i)
        return -ENOMEM;

    /* At least one desc is allocated */
    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        list_splice_init(&tmp_list, &xor_chan->free_q);
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    xor_log("%s returning %d total alloc'd\n", __func__, xor_chan->total_desc);

    return xor_chan->total_desc;
}


/**
 * @device_free_chan_resources: release DMA channel's resources       
 */
static
void
xor_free_chan_resources(struct dma_chan *chan)
{
    struct xor_chan_s *xor_chan = container_of(chan, struct xor_chan_s, common);
    struct xor_desc_s *desc, *_desc;
    unsigned long flags;

    xor_log("%s chan:%p\n", __func__, chan);

    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        list_for_each_entry_safe(desc, _desc, &xor_chan->submit_q, node) {
            list_del(&desc->node);
            xor_chan->total_desc--;
            kfree(desc);
        }
        list_for_each_entry_safe(desc, _desc, &xor_chan->pending_q, node) {
            list_del(&desc->node);
            xor_chan->total_desc--;
            kfree(desc);
        }
        list_for_each_entry_safe(desc, _desc, &xor_chan->in_progress_q, node) {
            list_del(&desc->node);
            xor_chan->total_desc--;
            kfree(desc);
        }
        list_for_each_entry_safe(desc, _desc, &xor_chan->free_q, node) {
            list_del(&desc->node);
            xor_chan->total_desc--;
            kfree(desc);
        }
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    BUG_ON(xor_chan->total_desc < 0);

    /* Some descriptor not freed? */
    if (unlikely(xor_chan->total_desc))
        pr_warn("iproc_xor: Failed to free xor channel resource\n");
}


/**
 * xor_status: poll for transaction completion, the optional
 *  txstate parameter can be supplied with a pointer to get a
 *  struct with auxiliary transfer status information, otherwise the call
 *  will just return a simple status code
 * @chan: XOR channel handle
 * @cookie: XOR transaction identifier
 * @txstate: XOR transactions state holder (or NULL)
 */
static
enum dma_status
xor_status(struct dma_chan *chan, dma_cookie_t cookie, struct dma_tx_state *txstate)
{
    enum dma_status ret;

    xor_log("%s chan:%p cookie:%u txstate:%p chan->cookie:%u chan->completed:%u\n",
            __func__, chan, cookie, txstate, chan->cookie, chan->completed_cookie);

    ret = dma_cookie_status(chan, cookie, txstate);

    xor_log("%s returning %d\n", __func__, (int) ret);

    return ret;
}


/**
 * xor_process_pending: push pending_q transactions to hardware.
 * @chan: DMA channel                           
 */
static
void
xor_process_pending(struct xor_chan_s *xor_chan)
{
    struct xor_desc_s *desc = NULL;
    unsigned long flags;

    xor_log("%s chan:%p\n", __func__, xor_chan);

    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        /* if we have an element to process and are not already processing then do it */
        if (!list_empty(&xor_chan->pending_q) && list_empty(&xor_chan->in_progress_q)) {
            desc = list_entry(xor_chan->pending_q.next, struct xor_desc_s, node);
            list_move_tail(&desc->node, &xor_chan->in_progress_q);
        }
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    if (desc) {
        xor_log("%s moved desc:%p dest:%08x src_cnt:%d\n", __func__, desc, desc->dest, desc->src_cnt);
        pae_do_xor(desc->src_cnt, desc->len, desc->dest, desc->srcs);
    }
}


/**
 * xor_issue_pending: push the submited transactions to "hardware".  Moves
 * descriptors in submit_q to pending_q and starts the processing
 * @chan: DMA channel                           
 */
static
void
xor_issue_pending(struct dma_chan *chan)
{
    struct xor_chan_s *xor_chan = container_of(chan, struct xor_chan_s, common);
    unsigned long flags;

    xor_log("%s chan:%p\n", __func__, chan);

    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        list_splice_tail_init(&xor_chan->submit_q, &xor_chan->pending_q);
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    xor_process_pending(xor_chan);
}


/**
 * xor_async_tx_submit: set the prepared descriptor(s) to be executed
 * by the engine.
 * @tx: descriptor to move to submit_q
 */
static
dma_cookie_t
xor_async_tx_submit(struct dma_async_tx_descriptor *tx)
{   
    struct xor_desc_s *desc = container_of(tx, struct xor_desc_s, async_tx);
    struct xor_chan_s *xor_chan = container_of(tx->chan, struct xor_chan_s, common);
    dma_cookie_t cookie;
    unsigned long flags;

    xor_log("%s chan:%p tx:%p desc:%p\n", __func__, xor_chan, tx, desc);

    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        cookie = xor_chan->common.cookie + 1;
        if (cookie < 0)
            cookie = 1;

        desc->async_tx.cookie = cookie;
        xor_chan->common.cookie = desc->async_tx.cookie;

        list_splice_tail_init(&desc->tx_list, &xor_chan->submit_q);
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    return cookie;
}


/**
 * xor_prep_dma_xor: prepares a xor operation
 */
static
struct dma_async_tx_descriptor *
xor_prep_dma_xor(struct dma_chan *chan, dma_addr_t dest, dma_addr_t *src,
                 unsigned int src_cnt, size_t len, unsigned long dma_flags)
{
    struct xor_chan_s *xor_chan = container_of(chan, struct xor_chan_s, common);
    struct xor_desc_s *new = NULL;
    int i;
    unsigned long flags;

    xor_log("%s chan:%p dest:%08x (virt:%p) src_cnt:%d len:%u dma_flags:%ld\n",
            __func__, chan, dest, bus_to_virt(dest), src_cnt, len, dma_flags);
    for (i = 0; i < src_cnt; i++)
        xor_log("  src[%d]: phy:%08x (virt:%p)\n", i, src[i], bus_to_virt(src[i]));

    if (unlikely(len < XOR_MIN_BYTE_COUNT))
        return NULL;

    BUG_ON(len > XOR_MAX_BYTE_COUNT);

    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        if (!list_empty(&xor_chan->free_q)) {
            new = container_of(xor_chan->free_q.next, struct xor_desc_s, node);
            list_del(&new->node);
        }
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    if (!new) {
        new = xor_alloc_descriptor(xor_chan, GFP_KERNEL | GFP_DMA);
    }

    dma_async_tx_descriptor_init(&new->async_tx, &xor_chan->common);

    INIT_LIST_HEAD(&new->node);
    INIT_LIST_HEAD(&new->tx_list);

    new->src_cnt = src_cnt;
    new->len = len;
    new->dest = dest;
    for (i = 0; i < src_cnt; i++) {
        new->srcs[i] = src[i];
    }

    new->async_tx.parent = NULL;
    new->async_tx.next = NULL;
    new->async_tx.cookie = 0;
    new->async_tx.callback = NULL;
    new->async_tx.callback_param = NULL;

    async_tx_ack(&new->async_tx);

    spin_lock_irqsave(&xor_chan->desc_lock, flags);
    {
        xor_op_counts[0] += 1;
        list_add_tail(&new->node, &new->tx_list);
    }
    spin_unlock_irqrestore(&xor_chan->desc_lock, flags);

    new->async_tx.flags = dma_flags;
    new->async_tx.cookie = -EBUSY;

    return &new->async_tx;
}


static struct dma_async_tx_descriptor *
xor_prep_dma_interrupt(struct dma_chan *chan, unsigned long flags)
{
    xor_log("%s\n", __func__);
    return NULL;
}
static struct dma_async_tx_descriptor *
xor_prep_dma_memcpy(struct dma_chan *chan, dma_addr_t dest, dma_addr_t src,
                    size_t len, unsigned long flags)
{
    xor_log("%s\n", __func__);
    return NULL;
}
static struct dma_async_tx_descriptor *
xor_prep_dma_memset(struct dma_chan *chan, dma_addr_t dest, int value,
                       size_t len, unsigned long flags)
{
    xor_log("%s\n", __func__);
    return NULL;
}
static struct dma_async_tx_descriptor *
xor_prep_dma_xor_val(struct dma_chan *chan, dma_addr_t *src, unsigned int src_cnt,
                     size_t len, enum sum_check_flags *result, unsigned long flags)
{
    xor_log("%s\n", __func__);
    return NULL;
}
static struct dma_async_tx_descriptor *
xor_prep_dma_pq(struct dma_chan *chan, dma_addr_t *dst, dma_addr_t *src,
                unsigned int src_cnt, const unsigned char *scf,
                size_t len, unsigned long flags)
{
    xor_log("%s\n", __func__);
    return NULL;
}
static struct dma_async_tx_descriptor *
xor_prep_dma_pq_val(struct dma_chan *chan, dma_addr_t *pq, dma_addr_t *src,
                        unsigned int src_cnt, const unsigned char *scf, size_t len,
                        enum sum_check_flags *pqres, unsigned long flags)
{
    xor_log("%s\n", __func__);
    return NULL;
}


/* ==================== Self Tests ==================== */

static
int xor_do_test(unsigned n_srcs, struct page** srcs, struct page *dest)
{
    int err = 0;
    int i, src_idx;
    u8 cmp_byte = 0;
    u32 cmp_word;
    dma_addr_t dma_srcs[n_srcs];
    dma_addr_t dest_dma;
    struct dma_async_tx_descriptor *tx;
    dma_cookie_t cookie;
    struct xor_chan_s *xor_chan = &xor_priv.xor_chan;
    struct dma_chan *dma_chan = &xor_chan->common;

    xor_log("%s n_srcs:%d\n", __func__, n_srcs);

    /* set srcs and zero data */
    for (src_idx = 0; src_idx < n_srcs; src_idx++) {
        u8 *ptr = page_address(srcs[src_idx]);
        for (i = 0; i < PAGE_SIZE; i++)
            ptr[i] = (1 << src_idx);
    }
    memset(page_address(dest), 0, PAGE_SIZE);

    /* generate the known answer */
    for (src_idx = 0; src_idx < n_srcs; src_idx++)
        cmp_byte ^= (u8) (1 << src_idx);
    cmp_word = (cmp_byte << 24) | (cmp_byte << 16) | (cmp_byte << 8) | cmp_byte;

    /* create the dma ptrs */
    for (i = 0; i < n_srcs; i++) {
        dma_srcs[i] = dma_map_page(&xor_priv.pdev->dev, srcs[i], 0, PAGE_SIZE, DMA_TO_DEVICE);
    }

    dest_dma = dma_map_page(&xor_priv.pdev->dev, dest, 0, PAGE_SIZE, DMA_BIDIRECTIONAL);

    tx = xor_prep_dma_xor(dma_chan, dest_dma, dma_srcs, n_srcs, PAGE_SIZE, 0);
    cookie = xor_async_tx_submit(tx);
    xor_issue_pending(dma_chan);
    async_tx_ack(tx);

    /* wait a bit for the result, and longer if debug output is on */
    msleep(1 + (xor_debug_logging * (xor_logging_sleep + 1) * 100));

    if (xor_status(dma_chan, cookie, NULL) != DMA_SUCCESS) {
        dev_printk(KERN_ERR, dma_chan->device->dev, "Self-test xor timed out, disabling\n");
        err++;
        goto do_test_free_resources;
    }

    for (i = 0; i < (PAGE_SIZE / sizeof(u32)); i++) {
        u32 *ptr = page_address(dest);
        if (ptr[i] != cmp_word) {
            dev_printk(KERN_ERR, &xor_priv.pdev->dev,
                       "Self-test failed compare: srcs:%d dest:%p index %d, data %08x, expected %08x\n",
                       n_srcs, ptr, i, ptr[i], cmp_word);
            err++;
            goto do_test_free_resources;
        }
    }

do_test_free_resources:
    xor_free_chan_resources(dma_chan);
    src_idx = n_srcs;
    while (src_idx--) {
        dma_unmap_page(&xor_priv.pdev->dev, dma_srcs[src_idx], PAGE_SIZE, DMA_TO_DEVICE);
    }

    if (err) {
            dev_printk(KERN_ERR, &xor_priv.pdev->dev,
                       "Self-test failed compare: %d times\n", err);
    }

    return err;
}


static
int
xor_xor_self_test(void)
{
    int i, j, src_idx;
    int err = 0;
    struct page *dest;
    struct page *xor_srcs[XOR_SELF_TEST_NUM_SRCS];

    xor_log("%s\n", __func__);

    for (src_idx = 0; src_idx < XOR_SELF_TEST_NUM_SRCS; src_idx++) {
        xor_srcs[src_idx] = alloc_page(GFP_ATOMIC);
        if (!xor_srcs[src_idx]) {
            while (src_idx--)
                __free_page(xor_srcs[src_idx]);
            return -ENOMEM;
        }
    }

    dest = alloc_page(GFP_ATOMIC);
    if (!dest) {
        while (src_idx--)
            __free_page(xor_srcs[src_idx]);
        return -ENOMEM;
    }

    for (j = 0; j < 5; j++) {
        for (i = 2; i <= XOR_SELF_TEST_NUM_SRCS; i++) {
            err += xor_do_test(i, xor_srcs, dest);
        }
    }

    if (!err)
        xor_log("%s self-test passed.\n", __func__);

    src_idx = XOR_SELF_TEST_NUM_SRCS;
    while (src_idx--) {
        __free_page(xor_srcs[src_idx]);
    }
    __free_page(dest);
    return err;
}


/* ==================== Kernel Platform API ==================== */

int
xor_enable_device(bool full)
{
    struct dma_device *dma_dev = &xor_priv.dma_dev;
    const char *name           = xor_priv.pdev->name;
    unsigned irq = 204;
    int rc = 0;
    uint32_t irq_mask = PAE_INTERRUPT_MASK__PAE_MESSAGE_AVAIL_TO_HOST;

    xor_log("%s\n", __func__);

    xor_priv.base_ptr = ioremap(PAE_REG_BASE, 32);
    if (!xor_priv.base_ptr) {
        printk(KERN_ERR "%s: ioremap of register space failed\n", __func__);
        rc = -ENOMEM;
        goto xor_enable_unmap;
    }
    xor_log("%s got base addr:%p\n", __func__, xor_priv.base_ptr);    

    if (full) {
        xor_log("%s adding full (slow) callbacks.\n", __func__);

        dma_cap_set(DMA_INTERRUPT, dma_dev->cap_mask);
        dma_dev->device_prep_dma_interrupt = xor_prep_dma_interrupt;
        dma_cap_set(DMA_MEMCPY, dma_dev->cap_mask);
        dma_dev->device_prep_dma_memcpy = xor_prep_dma_memcpy;
        dma_cap_set(DMA_MEMSET, dma_dev->cap_mask);
        dma_dev->device_prep_dma_memset = xor_prep_dma_memset;
        dma_cap_set(DMA_XOR_VAL, dma_dev->cap_mask);
        dma_dev->device_prep_dma_xor_val = xor_prep_dma_xor_val;
        dma_cap_set(DMA_PQ, dma_dev->cap_mask);
        dma_dev->device_prep_dma_pq = xor_prep_dma_pq;
        dma_cap_set(DMA_PQ_VAL, dma_dev->cap_mask);
        dma_dev->device_prep_dma_pq_val = xor_prep_dma_pq_val;
    }

    xor_log("%s enabling dma registration.\n", __func__);
    rc = dma_async_device_register(&xor_priv.dma_dev);
    if (rc) {
        pr_err("iproc_xor: failed to register slave DMA engine device: %d\n", rc);
        goto xor_enable_unmap;
    }

    /* clear the message & irq regs, then set mask */
    xor_reg_set32(xor_priv.base_ptr, PAE_MESSAGE_OUT_MAILBOX_OFFSET, 0x0);
    xor_reg_set32(xor_priv.base_ptr, PAE_MESSAGE_IN_MAILBOX_OFFSET, 0x0);
    xor_reg_set32(xor_priv.base_ptr, PAE_INTERRUPT_OFFSET, 0x0);
    barrier();
    xor_reg_set32(xor_priv.base_ptr, PAE_INTERRUPT_MASK_OFFSET, irq_mask);
    barrier();

    /* enable interrupt handler for XOR communications */
    rc = request_irq(irq, xor_irq_handler, 0, name, NULL);
    if (rc) {
        printk(KERN_ERR "%s: Failed to register %s irq %d\n", __func__, name, irq);
        goto xor_enable_unreg;
    }

    xor_priv.enabled = true;

    return xor_xor_self_test();

xor_enable_unmap:
    iounmap(xor_priv.base_ptr);
xor_enable_unreg:
    xor_priv.dma_dev.chancnt = 0;
    dma_async_device_unregister(&xor_priv.dma_dev);
    return rc;
}


static
int
__devinit iproc_xor_probe(struct platform_device *pdev)
{
    struct dma_device *dma_dev  = &xor_priv.dma_dev;
    struct xor_chan_s *xor_chan = &xor_priv.xor_chan;
    int rc = 0;

    pr_debug("iproc_xor: %s() pdev:%p\n", __func__, pdev);

    xor_priv.enabled = false;

    INIT_LIST_HEAD(&dma_dev->channels);

    /* set base routines and capabilities*/
    dma_dev->dev = &pdev->dev;
    dma_dev->device_alloc_chan_resources = xor_alloc_chan_resources;
    dma_dev->device_free_chan_resources = xor_free_chan_resources;
    dma_dev->device_tx_status = xor_status;
    dma_dev->device_issue_pending = xor_issue_pending;

    dma_cap_set(DMA_XOR, dma_dev->cap_mask);
    dma_dev->device_prep_dma_xor = xor_prep_dma_xor;
    dma_dev->max_xor = XOR_MAX_SOURCES;

    xor_chan->common.device = dma_dev;

    INIT_LIST_HEAD(&xor_chan->submit_q);
    INIT_LIST_HEAD(&xor_chan->pending_q);
    INIT_LIST_HEAD(&xor_chan->in_progress_q);
    INIT_LIST_HEAD(&xor_chan->free_q);

    spin_lock_init(&xor_chan->desc_lock);
    
    dma_cookie_init(&xor_chan->common);

    list_add_tail(&xor_chan->common.device_node, &dma_dev->channels);

    return rc;
}


static
int
__devexit iproc_xor_remove(struct platform_device *pdev)
{
    struct xor_chan_s *xor_chan;
    struct dma_chan *chan, *_chan;

    pr_debug("iproc_xor: remove() pdev:%p\n", pdev);

    if (xor_priv.enabled) {
        xor_priv.enabled = false;

        free_irq(204, NULL);

        iounmap(xor_priv.base_ptr);

        list_for_each_entry_safe(chan, _chan, &xor_priv.dma_dev.channels, device_node) {
            xor_chan = container_of(chan, struct xor_chan_s, common);
            list_del(&chan->device_node);

            printk("  found chan:%p with %d descs\n", chan, xor_chan->total_desc);

            xor_free_chan_resources(chan);
            dma_release_channel(chan);
            kfree(xor_chan);
        }
        xor_priv.dma_dev.chancnt = 0;

        dma_async_device_unregister(&xor_priv.dma_dev);
    }

    pr_debug("iproc_xor: remove() done\n");

    return 0;
}


/* ===== Kernel Module API ===== */

static struct platform_driver xor_pdriver = { 
    .driver = {
        .name   = "iproc_xor",
        .owner  = THIS_MODULE,
    },
    .probe  = iproc_xor_probe,
    .remove = iproc_xor_remove,
    .suspend = NULL,
    .resume  = NULL,
};

static struct sysfs_ops xor_ops = {
    .show = xor_show,
    .store = xor_store,
};

static struct kobj_type xor_type = {
    .sysfs_ops = &xor_ops,
    .default_attrs = xor_attrs,
};

struct kobject *xor_kobj;


static
int
__init iproc_xor_init(void)
{
    int rc;

    pr_debug("iproc_xor: loading driver\n");
 
    xor_kobj = kzalloc(sizeof(*xor_kobj), GFP_KERNEL);
    if (!xor_kobj) {
        pr_err("%s: Sysfs alloc failed\n", __func__);
        rc = -ENOMEM;
        goto alloc_failed;
    }
            
    kobject_init(xor_kobj, &xor_type);
    if (kobject_add(xor_kobj, NULL, "%s", "iproc_xor")) {
        pr_err("%s: Sysfs creation failed\n", __func__);
        rc = -1;
        goto sysfs_failed;
    }   

    rc = platform_driver_register(&xor_pdriver);

    if (rc < 0) {
        pr_err("%s: Driver registration failed, error %d\n", __func__, rc);
        goto driver_reg_failed;
    }

    xor_priv.pdev = platform_device_register_simple("iproc_xor", -1, NULL, 0);

    if (!xor_priv.pdev) {
        rc = -EINVAL;
        goto device_reg_failed;
    }
 
    platform_set_drvdata(xor_priv.pdev, &xor_priv);

    return 0;

device_reg_failed:
    platform_driver_unregister(&xor_pdriver);
driver_reg_failed:
sysfs_failed:
    kobject_put(xor_kobj);
    xor_kobj = NULL;
alloc_failed:
    return rc;
}


static
void
__exit iproc_xor_exit(void)
{
    platform_device_unregister(xor_priv.pdev);
    platform_driver_unregister(&xor_pdriver);

    if (xor_kobj) {
        kobject_put(xor_kobj);
        kfree(xor_kobj);
    }
}


module_init(iproc_xor_init);
module_exit(iproc_xor_exit);

MODULE_DESCRIPTION("iProc XOR offload support.");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Broadcom Corporation");
