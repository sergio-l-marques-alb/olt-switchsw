/*
 *  linux/drivers/mmc/card/queue.c
 *
 *  Copyright (C) 2003 Russell King, All Rights Reserved.
 *  Copyright 2006-2007 Pierre Ossman
 *  Copyright (C) 2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 */
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/freezer.h>
#include <linux/kthread.h>
#include <linux/scatterlist.h>

#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include "queue.h"

#ifdef CONFIG_OPTIMIZE_SD_PERFORMANCE
#define MMC_QUEUE_BOUNCESZ	262144
#else
#define MMC_QUEUE_BOUNCESZ	65536
#endif

#define MMC_QUEUE_SUSPENDED	(1 << 0)

/*
 * Prepare a MMC request. This just filters out odd stuff.
 */
static int mmc_prep_request(struct request_queue *q, struct request *req)
{
	/*
	 * We only like normal block requests.
	 */
	if (!blk_fs_request(req)) {
		blk_dump_rq_flags(req, "MMC bad request");
		return BLKPREP_KILL;
	}

	req->cmd_flags |= REQ_DONTPREP;

	return BLKPREP_OK;
}

static int mmc_queue_thread(void *d)
{
	struct mmc_queue *mq = d;
	struct request_queue *q = mq->queue;

	current->flags |= PF_MEMALLOC;

	down(&mq->thread_sem);
	do {
		struct request *req = NULL;

		spin_lock_irq(q->queue_lock);
		set_current_state(TASK_INTERRUPTIBLE);
		if (!blk_queue_plugged(q))
			req = blk_fetch_request(q);
		mq->mqrq_cur->req = req;
		spin_unlock_irq(q->queue_lock);

		mq->issue_fn(mq, req);
		if (!req) {
			if (kthread_should_stop()) {
				set_current_state(TASK_RUNNING);
				break;
			}
			up(&mq->thread_sem);
			schedule();
			down(&mq->thread_sem);
			continue;
		}

		set_current_state(TASK_RUNNING);
	} while (1);
	up(&mq->thread_sem);

	return 0;
}

/*
 * Generic MMC request handler.  This is called for any queue on a
 * particular host.  When the host is not busy, we look for a request
 * on any queue on this host, and attempt to issue it.  This may
 * not be the queue we were asked to process.
 */
static void mmc_request(struct request_queue *q)
{
	struct mmc_queue *mq = q->queuedata;
	struct request *req;

	if (!mq) {
		while ((req = blk_fetch_request(q)) != NULL) {
			req->cmd_flags |= REQ_QUIET;
			__blk_end_request_all(req, -EIO);
		}
		return;
	}

	if (!mq->mqrq_cur->req)
		wake_up_process(mq->thread);
}

struct scatterlist *mmc_alloc_sg(int sg_len, int *err)
{
	struct scatterlist *sg;

	sg = kmalloc(sizeof(struct scatterlist)*sg_len, GFP_KERNEL);
	if (!sg)
		*err = -ENOMEM;
	else {
		*err = 0;
		sg_init_table(sg, sg_len);
	}

	return sg;
}

/**
 * mmc_init_queue - initialise a queue structure.
 * @mq: mmc queue
 * @card: mmc card to attach this queue
 * @lock: queue lock
 *
 * Initialise a MMC card request queue.
 */
int mmc_init_queue(struct mmc_queue *mq, struct mmc_card *card, spinlock_t *lock)
{
	struct mmc_host *host = card->host;
	u64 limit = BLK_BOUNCE_HIGH;
	int ret;
	struct mmc_queue_req *mqrq_cur = &mq->mqrq[0];
	struct mmc_queue_req *mqrq_prev = &mq->mqrq[1];

	if (mmc_dev(host)->dma_mask && *mmc_dev(host)->dma_mask)
		limit = *mmc_dev(host)->dma_mask;

	mq->card = card;
	mq->queue = blk_init_queue(mmc_request, lock);
	if (!mq->queue)
		return -ENOMEM;

	memset(&mq->mqrq_cur, 0, sizeof(mq->mqrq_cur));
	memset(&mq->mqrq_prev, 0, sizeof(mq->mqrq_prev));
	mq->mqrq_cur = mqrq_cur;
	mq->mqrq_prev = mqrq_prev;
	mq->queue->queuedata = mq;

	blk_queue_prep_rq(mq->queue, mmc_prep_request);
	blk_queue_ordered(mq->queue, QUEUE_ORDERED_DRAIN, NULL);
	queue_flag_set_unlocked(QUEUE_FLAG_NONROT, mq->queue);

#ifdef CONFIG_MMC_BLOCK_BOUNCE
	if (host->max_hw_segs == 1) {
		unsigned int bouncesz;

		bouncesz = MMC_QUEUE_BOUNCESZ;

		if (bouncesz > host->max_req_size)
			bouncesz = host->max_req_size;
		if (bouncesz > host->max_seg_size)
			bouncesz = host->max_seg_size;
		if (bouncesz > (host->max_blk_count * 512))
			bouncesz = host->max_blk_count * 512;

		if (bouncesz > 512) {
			mqrq_cur->bounce_buf = kmalloc(bouncesz, GFP_KERNEL);
			if (!mqrq_cur->bounce_buf) {
				printk(KERN_WARNING "%s: unable to "
					"allocate bounce curr buffer\n",
					mmc_card_name(card));
			}
			mqrq_prev->bounce_buf = kmalloc(bouncesz, GFP_KERNEL);
			if (!mqrq_prev->bounce_buf) {
				printk(KERN_WARNING "%s: unable to "
					"allocate bounce prev buffer\n",
					mmc_card_name(card));
				kfree(mqrq_cur->bounce_buf);
				mqrq_cur->bounce_buf = NULL;
			}
		}

		if (mqrq_cur->bounce_buf && mqrq_prev->bounce_buf) {
			blk_queue_bounce_limit(mq->queue, BLK_BOUNCE_ANY);
			blk_queue_max_hw_sectors(mq->queue, bouncesz / 512);
			blk_queue_max_segments(mq->queue, bouncesz / 512);
			blk_queue_max_segment_size(mq->queue, bouncesz);

			mqrq_cur->sg = mmc_alloc_sg(1, &ret);
			if (ret)
				goto cleanup_queue;

			mqrq_cur->bounce_sg =
				mmc_alloc_sg(bouncesz / 512, &ret);
			if (ret)
				goto cleanup_queue;
			mqrq_prev->sg = mmc_alloc_sg(1, &ret);
			if (ret)
				goto cleanup_queue;

			mqrq_prev->bounce_sg =
				mmc_alloc_sg(bouncesz / 512, &ret);
			if (ret)
				goto cleanup_queue;
		}
	}
#endif

	if (!mqrq_cur->bounce_buf && !mqrq_prev->bounce_buf) {
		blk_queue_bounce_limit(mq->queue, limit);
		blk_queue_max_hw_sectors(mq->queue,
			min(host->max_blk_count, host->max_req_size / 512));
		blk_queue_max_segments(mq->queue, host->max_hw_segs);
		blk_queue_max_segment_size(mq->queue, host->max_seg_size);

		mqrq_cur->sg = mmc_alloc_sg(host->max_phys_segs, &ret);
		if (ret)
			goto cleanup_queue;

		mqrq_prev->sg = mmc_alloc_sg(host->max_phys_segs, &ret);
		if (ret)
			goto cleanup_queue;
	}

	init_MUTEX(&mq->thread_sem);

	mq->thread = kthread_run(mmc_queue_thread, mq, "mmcqd");
	if (IS_ERR(mq->thread)) {
		ret = PTR_ERR(mq->thread);
		goto free_bounce_sg;
	}

	return 0;
 free_bounce_sg:
	kfree(mqrq_cur->bounce_sg);
	mqrq_cur->bounce_sg = NULL;
	kfree(mqrq_prev->bounce_sg);
	mqrq_prev->bounce_sg = NULL;
 cleanup_queue:
	kfree(mqrq_cur->sg);
	mqrq_cur->sg = NULL;
	kfree(mqrq_cur->bounce_buf);
	mqrq_cur->bounce_buf = NULL;
	kfree(mqrq_prev->sg);
	mqrq_prev->sg = NULL;
	kfree(mqrq_prev->bounce_buf);
	mqrq_prev->bounce_buf = NULL;
	blk_cleanup_queue(mq->queue);
	return ret;
}

void mmc_cleanup_queue(struct mmc_queue *mq)
{
	struct request_queue *q = mq->queue;
	unsigned long flags;
	struct mmc_queue_req *mqrq_cur = mq->mqrq_cur;
	struct mmc_queue_req *mqrq_prev = mq->mqrq_prev;

	/* Make sure the queue isn't suspended, as that will deadlock */
	mmc_queue_resume(mq);

	/* Then terminate our worker thread */
	kthread_stop(mq->thread);

	/* Empty the queue */
	spin_lock_irqsave(q->queue_lock, flags);
	q->queuedata = NULL;
	blk_start_queue(q);
	spin_unlock_irqrestore(q->queue_lock, flags);

	kfree(mqrq_cur->bounce_sg);
	mqrq_cur->bounce_sg = NULL;

	kfree(mqrq_cur->sg);
	mqrq_cur->sg = NULL;

	kfree(mqrq_cur->bounce_buf);
	mqrq_cur->bounce_buf = NULL;

	kfree(mqrq_prev->bounce_sg);
	mqrq_prev->bounce_sg = NULL;

	kfree(mqrq_prev->sg);
	mqrq_prev->sg = NULL;

	kfree(mqrq_prev->bounce_buf);
	mqrq_prev->bounce_buf = NULL;

	mq->card = NULL;
}
EXPORT_SYMBOL(mmc_cleanup_queue);

/**
 * mmc_queue_suspend - suspend a MMC request queue
 * @mq: MMC queue to suspend
 *
 * Stop the block request queue, and wait for our thread to
 * complete any outstanding requests.  This ensures that we
 * won't suspend while a request is being processed.
 */
void mmc_queue_suspend(struct mmc_queue *mq)
{
	struct request_queue *q = mq->queue;
	unsigned long flags;

	if (!(mq->flags & MMC_QUEUE_SUSPENDED)) {
		mq->flags |= MMC_QUEUE_SUSPENDED;

		spin_lock_irqsave(q->queue_lock, flags);
		blk_stop_queue(q);
		spin_unlock_irqrestore(q->queue_lock, flags);

		down(&mq->thread_sem);
	}
}

/**
 * mmc_queue_resume - resume a previously suspended MMC request queue
 * @mq: MMC queue to resume
 */
void mmc_queue_resume(struct mmc_queue *mq)
{
	struct request_queue *q = mq->queue;
	unsigned long flags;

	if (mq->flags & MMC_QUEUE_SUSPENDED) {
		mq->flags &= ~MMC_QUEUE_SUSPENDED;

		up(&mq->thread_sem);

		spin_lock_irqsave(q->queue_lock, flags);
		blk_start_queue(q);
		spin_unlock_irqrestore(q->queue_lock, flags);
	}
}

/*
 * Prepare the sg list(s) to be handed of to the host driver
 */
unsigned int mmc_queue_map_sg(struct mmc_queue *mq, struct mmc_queue_req *mqrq)
{
	unsigned int sg_len;
	size_t buflen;
	struct scatterlist *sg;
	int i;

	if (!mqrq->bounce_buf)
		return blk_rq_map_sg(mq->queue, mqrq->req, mqrq->sg);

	BUG_ON(!mqrq->bounce_sg);

	sg_len = blk_rq_map_sg(mq->queue, mqrq->req, mqrq->bounce_sg);

	mqrq->bounce_sg_len = sg_len;

	buflen = 0;
	for_each_sg(mqrq->bounce_sg, sg, sg_len, i)
		buflen += sg->length;

	sg_init_one(mqrq->sg, mqrq->bounce_buf, buflen);

	return 1;
}

/*
 * If writing, bounce the data to the buffer before the request
 * is sent to the host driver
 */
void mmc_queue_bounce_pre(struct mmc_queue_req *mq)
{
	unsigned long flags;

	if (!mq->bounce_buf)
		return;

	if (rq_data_dir(mq->req) != WRITE)
		return;

	local_irq_save(flags);
	sg_copy_to_buffer(mq->bounce_sg, mq->bounce_sg_len,
		mq->bounce_buf, mq->sg[0].length);
	local_irq_restore(flags);
}

/*
 * If reading, bounce the data from the buffer after the request
 * has been handled by the host driver
 */
void mmc_queue_bounce_post(struct mmc_queue_req *mq)
{
	unsigned long flags;

	if (!mq->bounce_buf)
		return;

	if (rq_data_dir(mq->req) != READ)
		return;

	local_irq_save(flags);
	sg_copy_from_buffer(mq->bounce_sg, mq->bounce_sg_len,
		mq->bounce_buf, mq->sg[0].length);
	local_irq_restore(flags);
}

