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
 */
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <net/xfrm.h>

#include "pae_shared.h"
#include "pae_log.h"
#include "pae_regs.h"
#include "paemsg_int.h"

static struct dentry *fw_debug_dir_dentry = 0;
static struct dentry *fw_debug_dentry = 0;

static ssize_t debugfs_read(struct file *f, char __user *user_buf, size_t count, loff_t *offset)
{
    int rv = 0;
    int num_copied = 0;
    u32 cur_head;
    
    if (pae_status == PAE_STOPPED) {
        return -EINVAL;
    }

	if (!user_buf) {
		return -EINVAL;
    }

	if (count == 0) {
		return 0;
    }

	if (!access_ok(VERIFY_WRITE, user_buf, count)) {
		return -EFAULT;
    }

    cur_head = pae_get_tcm_u32(PAE_DEBUG_BUF_HEAD);

    /* If the debug buffer has wrapped so that *offset is no longer valid,  
     * update *offset to the oldest data in the buffer
     */
    if (*offset + PAE_DEBUG_BUF_SIZE <= (loff_t)cur_head) {
        *offset = cur_head - PAE_DEBUG_BUF_SIZE + 1;
    }

    while (!rv && num_copied < count && ((u32)*offset != cur_head)) {
        u8 c = pae_get_tcm_u8(PAE_DEBUG_BUF_BASE + (*offset % PAE_DEBUG_BUF_SIZE));
        rv = __put_user(c, user_buf + num_copied);
        ++num_copied;
        ++*offset;
    }
    
    if (!rv) {
        return num_copied;
    } else {
        return rv;
    }
}


static const struct file_operations debugfs_fops = {
    .read = debugfs_read
};


int pae_debug_setup()
{
    int rv = 0;

    fw_debug_dir_dentry = debugfs_create_dir("bcmiproc-pae", NULL);
    if (!fw_debug_dir_dentry) {
        printk("Failed to make PAE debug log\n");
        rv = -ENOMEM;
        goto failed_create_debug_dir;
    }

    fw_debug_dentry = debugfs_create_file("log", 0x600, fw_debug_dir_dentry, 0, &debugfs_fops);

    if (!fw_debug_dentry) {
        printk("Failed to make PAE debug log\n");
        rv = -ENOMEM;
        goto failed_create_debug;
    }

    return 0;

 failed_create_debug:
    debugfs_remove(fw_debug_dir_dentry);
    
 failed_create_debug_dir:
    return rv;
}


int pae_debug_exit()
{
    debugfs_remove(fw_debug_dentry);
    debugfs_remove(fw_debug_dir_dentry);

    return 0;
}
