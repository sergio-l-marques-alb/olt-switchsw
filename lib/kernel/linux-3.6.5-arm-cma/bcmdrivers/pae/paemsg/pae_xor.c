/**
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
#include <linux/cpu.h>
#include <asm/io.h>
#include <linux/kobject.h>
#include <asm/processor.h>
#include <asm/cpu.h>
#include <linux/module.h>

#include "pae_shared.h"
#include "pae_log.h"
#include "pae_regs.h"
#include "pae_cmds.h"
#include "paemsg_int.h"

#include "pae_xor.h"

/**
 *   Raid XOR
 *   Computes XOR checksum over list of Source buffers.
 *   Writes result in Destination buffer.
 *
 * - raid_xor
 * - raid_copy
 * - raid_compare
 * - raid_zero
 *
 * Message Buffer
 *  offset     XOR          COPY          COMPARE       ZERO
 *  -------------------------------------------------------------
 *   0x00    Destination  Destination   Destination   Destination
 *   0x04    Flags        Flags         Flags         Flags
 *   0x08    Length       Length        Length        Length
 *   0x0C    Source[0]    Source        Source[0]      -
 *    ..      ...          -              ...          -
 *   0x1C    Source[3]     -            Source[3]      -
 */


int
pae_do_xor(unsigned int src_count, unsigned int bytes, dma_addr_t dest, dma_addr_t *srcs)
{
    uint32_t raid_msg_buf[8];
    unsigned i;
 
    if (pae_status == PAE_STOPPED) {
        printk(KERN_WARNING "PAE is stopped, cannot execute command\n");
        return -ENXIO;
    }

    raid_msg_buf[0] = (uint32_t) dest;
    raid_msg_buf[1] = src_count;
    raid_msg_buf[2] = bytes;

    for (i = 0; i < src_count; i++)
        raid_msg_buf[3 + i] = (uint32_t) srcs[i];
    
    for (i = 0; i < 8; i++)
        pae_set_tcm_u32(PAE_CMD_BUF_ADDR + (i * 4), raid_msg_buf[i]);

    return pae_do_cmd(PAE_XOR_OP);
}

EXPORT_SYMBOL(pae_do_xor);
