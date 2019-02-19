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
#ifndef PAE_REGS_H
#define PAE_REGS_H

#include <linux/kernel.h>

/* Utility macros */
#define RETURN_ON_FAIL(x, msg) if (0) ; else { int _tmp = x;  if (_tmp) { if (msg[0]) printk(KERN_ERR "%s : %d\n",msg,_tmp); return _tmp; } }

#define NO_MSG ("")


/* Slow register access functions: they do an ioremap / unmap for every access */
/* Should only be used in rarely-used code (like init/shutdown functions */

int pae_register_get(u32 addr, u32 *val);
int pae_register_set(u32 addr, u32 val);

void pae_reg_field_set(u32 *regval, u32 field_val, u32 mask_val, unsigned shift);
u32 pae_reg_field_get(u32 regval, u32 mask_val, unsigned shift);

int pae_tcm_init(void);
void pae_tcm_exit(void);

void pae_set_tcm_u32(u32 r5_addr, u32 word);
u32  pae_get_tcm_u32(u32 r5_addr);

void pae_set_tcm_u16(u32 r5_addr, u16 halfword);
u16  pae_get_tcm_u16(u32 r5_addr);

void pae_set_tcm_u8(u32 r5_addr, u8 byte);
u8   pae_get_tcm_u8(u32 r5_addr);

void pae_clear_tcms(void);

/* Note that register definition file uses a different format for single bit
 * fields and "wide" fields.  The single bit fields don't have an _R, so we
 * need one macro for "wide" fields, one for single-bit fields
 */

#define REG_FIELD_OFFSET(reg, field) (reg ## __ ## field ## _R)
#define REG_FIELD_OFFSET_BIT(reg, field) (reg ## __ ## field)

#define REG_FIELD_WIDTH(reg, field) (reg ## __ ## field ## _WIDTH)

#define REG_FIELD_SET(old_val, regname, fieldname, field_val) pae_reg_field_set(old_val, field_val, 1 << (regname ## __ ## fieldname ## _WIDTH - 1), regname ## __ ## fieldname ## _R)
#define REG_FIELD_SET_BIT(old_val, regname, fieldname, field_val) pae_reg_field_set(old_val, field_val, 1, regname ## __ ## fieldname)

#define REG_FIELD_GET_BIT(val, regname, fieldname) pae_reg_field_get(val, 1, regname ## __ ## fieldname)


#endif /* PAE_REGS_H */
