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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/io.h>

#include "pae_regs.h"

#define PAE_IS_LITTLE_ENDIAN (1)

#define PAE_TCM_A_BASE    (0x2a000000)
#define PAE_TCM_A_BASE_R5 (0x00000000)
#define PAE_TCM_A_SIZE    (0x00018000)

#define PAE_TCM_B_BASE    (0x2a040000)
#define PAE_TCM_B_BASE_R5 (0x40000000)
#define PAE_TCM_B_SIZE    (0x00018000)

static void* pae_tcm_a_base;
static void* pae_tcm_b_base;

void pae_reg_field_set(uint32_t *regval, uint32_t field_val, uint32_t mask_val, unsigned shift)
{
    uint32_t masked_reg = (*regval & ~(mask_val << shift));
    uint32_t new_reg = (masked_reg | (field_val << shift));
    *regval = new_reg;
}


uint32_t pae_reg_field_get(uint32_t regval, uint32_t mask_val, unsigned shift)
{
    return ((regval >> shift) & mask_val);
}


int pae_register_get(u32 addr, u32 *val)
{
    void *base = ioremap(addr, 4);
    if (!base) {
        return -ENOMEM;
    }
    *val = ioread32(base);

    iounmap(base);

    return 0;
}


int pae_register_set(u32 addr, u32 val)
{
    void *base = ioremap(addr, 4);
    if (!base) {
        return -ENOMEM;
    }
    iowrite32(val, base);
    wmb();
    iounmap(base);

    return 0;
}


int pae_tcm_init(void)
{
    pae_tcm_a_base = ioremap_nocache(PAE_TCM_A_BASE, PAE_TCM_A_SIZE);
    if (!pae_tcm_a_base) {
        printk(KERN_ERR "Failed to remap TCMA (%08x %08x)\n", PAE_TCM_A_BASE, PAE_TCM_A_SIZE);
        return -ENOMEM;
    }

    pae_tcm_b_base = ioremap_nocache(PAE_TCM_B_BASE, PAE_TCM_B_SIZE);
    if (!pae_tcm_b_base) {
        printk(KERN_ERR "Failed to remap TCMB (%08x %08x)\n", PAE_TCM_B_BASE, PAE_TCM_B_SIZE);
        iounmap(pae_tcm_a_base);
        pae_tcm_a_base = 0;
        return -ENOMEM;
    }

    return 0;
}


void pae_tcm_exit(void)
{
    if (pae_tcm_a_base) {
        iounmap(pae_tcm_a_base);
        pae_tcm_a_base = 0;
    }
    if (pae_tcm_b_base) {
        iounmap(pae_tcm_b_base);
        pae_tcm_b_base = 0;
    }
}


void pae_set_tcm_u32(u32 r5_addr, u32 word)
{
    int i;
    static u32 first_loc_val;

    static int primed = 0;
    if (r5_addr == 0) {
        primed = 1;
    }
    if (r5_addr >= PAE_TCM_A_BASE_R5 && r5_addr < PAE_TCM_A_BASE_R5 + PAE_TCM_A_SIZE) {
        iowrite32(word, r5_addr - PAE_TCM_A_BASE_R5 + pae_tcm_a_base);
        wmb();
        udelay(1);
        if (r5_addr == 0) {
            first_loc_val = word;
            for (i = 0; i < 20; ++i) {
                if (ioread32(pae_tcm_a_base) != word) {
                    iowrite32(word, pae_tcm_a_base);
                    printk("Re-Wrote\n");
                }
            }
        }
        if (primed && ioread32(pae_tcm_a_base) != first_loc_val) {
            printk("\nWas %08x after writing %08x to %08x\n", ioread32(pae_tcm_a_base), word, r5_addr);
            iowrite32(first_loc_val, pae_tcm_a_base);
        }
    } else if (r5_addr >= PAE_TCM_B_BASE_R5 && r5_addr < PAE_TCM_B_BASE_R5 + PAE_TCM_B_SIZE) {
        iowrite32(word, r5_addr - PAE_TCM_B_BASE_R5 + pae_tcm_b_base);
    } else {
        printk(KERN_ERR "PAE R5 Address %08x not in TCM space\n", r5_addr);
    }
}

u32 pae_get_tcm_u32(u32 r5_addr)
{
    if (r5_addr >= PAE_TCM_A_BASE_R5 && r5_addr < PAE_TCM_A_BASE_R5 + PAE_TCM_A_SIZE) {
        return ioread32(r5_addr - PAE_TCM_A_BASE_R5 + pae_tcm_a_base);
    } else if (r5_addr >= PAE_TCM_B_BASE_R5 && r5_addr < PAE_TCM_B_BASE_R5 + PAE_TCM_B_SIZE) {
        return ioread32(r5_addr - PAE_TCM_B_BASE_R5 + pae_tcm_b_base);
    } else {
        if (printk_ratelimit() == 0) {
            printk(KERN_ERR "PAE R5 Address %08x not in TCM space\n", r5_addr);
        }
        return 0;
    }

}


u8 pae_get_tcm_u8(u32 r5_addr)
{
    u32 word = pae_get_tcm_u32(r5_addr & ~0x3);
    
    if (PAE_IS_LITTLE_ENDIAN) {
        return (word >> (8 * (r5_addr & 3)));
    } else {
        return (word >> (8 * (3 - (r5_addr & 3))));
    }
}


void pae_set_tcm_u8(u32 r5_addr, u8 byte)
{
    u32 orig_word = pae_get_tcm_u32(r5_addr & ~0x3);
    u32 masked_word, new_word;

    if (PAE_IS_LITTLE_ENDIAN) {
        masked_word = orig_word & ~(0xff << (8 * (r5_addr & 3)));
        new_word = masked_word | (((u32)byte) << (8 * (r5_addr & 3)));
    } else {
        masked_word = orig_word & ~(0xff << (3 - (8 * (r5_addr & 3))));
        new_word = masked_word | (((u32)byte) << (8 * (3 - (r5_addr & 3))));
    }

    pae_set_tcm_u32((r5_addr & ~0x3), new_word);
}


u16  pae_get_tcm_u16(u32 r5_addr)
{
    u32 word = pae_get_tcm_u32(r5_addr & ~0x3);
    
    if (PAE_IS_LITTLE_ENDIAN) {
        return (word >> (8 * (r5_addr & 3)));
    } else {
        return (word >> (8 * (3 - (r5_addr & 3))));
    }
}


void pae_set_tcm_u16(u32 r5_addr, u16 halfword)
{
    u32 orig_word = pae_get_tcm_u32(r5_addr & ~0x3);
    u32 masked_word, new_word;

    if (PAE_IS_LITTLE_ENDIAN) {
        masked_word = orig_word & ~(0xffff << (8 * (r5_addr & 3)));
        new_word = masked_word | (((u32)halfword) << (8 * (r5_addr & 3)));
    } else {
        masked_word = orig_word & ~(0xffff << (3 - (8 * (r5_addr & 3))));
        new_word = masked_word | (((u32)halfword) << (8 * (3 - (r5_addr & 3))));
    }

    pae_set_tcm_u32((r5_addr & ~0x3), new_word);
}



void pae_clear_tcms(void)
{
    memset_io(pae_tcm_a_base, 0, PAE_TCM_A_SIZE);
    memset_io(pae_tcm_b_base, 0, PAE_TCM_B_SIZE);
    wmb();
}
