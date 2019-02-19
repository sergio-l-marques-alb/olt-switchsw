/*
 * $Id:$
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:       phy8806x_syms.h
 */

#ifndef   _PHY8806X_SYMS_H_
#define   _PHY8806X_SYMS_H_

#include <sal/types.h>


#define MT2_CONFIG_INCLUDE_CHIP_SYMBOLS 1
#define MT2_CONFIG_INCLUDE_FIELD_NAMES 1
#define MT2_CONFIG_INCLUDE_FIELD_INFO 1
#define MT2_CONFIG_INCLUDE_RESET_VALUES 1
#define MT2_CONFIG_INCLUDE_TEST_MASK 1

typedef struct mt2_sym_s {
        uint32 addr;
#if MT2_CONFIG_INCLUDE_FIELD_INFO == 1
        uint32* fields;
#endif
        uint32 index;   /* "size" for registers */
        uint32 flags;
        uint32 blktypes;
        uint32 block;
        const char *name;
#if MT2_CONFIG_INCLUDE_RESET_VALUES == 1
        uint32 rstval;
        uint32 rstval_hi;
#endif
#if MT2_CONFIG_INCLUDE_TEST_MASK == 1
        uint32 test_mask;
        uint32 test_mask_hi;
#endif
} mt2_sym_t;

/* Flags */
#define MT2_SYMBOL_FLAG_REGISTER 0x00000001
#define MT2_SYMBOL_FLAG_R64      0x00000002
#define MT2_SYMBOL_FLAG_PORT     0x00000004
#define MT2_SYMBOL_FLAG_SOFT_PORT 0x00000008
#define MT2_SYMBOL_FLAG_MEMORY   0x00000010
#define MT2_SYMBOL_FLAG_COUNTER  0x00000020
#define MT2_SYMBOL_FLAG_MEMMAPPED 0x00000040
#define MT2_SYMBOL_FLAG_NOTEST   0x00000080
#define MT2_SYMBOL_FLAG_READONLY 0x00000100

/* Size & Index.. Encodingt TBD */
#define MT2_SYMBOL_INDEX_SIZE_ENCODE(x) (x)
#define MT2_SYMBOL_INDEX_MIN_ENCODE(x) (x << 8)
#define MT2_SYMBOL_INDEX_MAX_ENCODE(x) (x<<16)

#define MT2_SYMBOL_INDEX_SIZE_DECODE(x) (x & 0xff)
#define MT2_SYMBOL_INDEX_MIN_DECODE(x) ((x >> 8) & 0xff)
#define MT2_SYMBOL_INDEX_MAX_DECODE(x) (x >> 16)

#define CDK_SYMBOL_FIELD_FLAG_LAST (1 << 31)
#define CDK_SYMBOL_FIELD_ENCODE(x, y, z)  ((x << 16) | (y << 8) | z)

extern const mt2_sym_t phy8806x_syms[];
extern const uint32 phy8806x_syms_numels;
extern const char* phy8806x_fields[];


/* forward definitions of the access functions */
extern int mt2_xgsd_reg32_block_read(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg32_block_write(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_block_read(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_block_write(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);

extern int mt2_xgsd_reg32_port_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg32_port_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_port_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_port_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);

extern int mt2_xgsd_reg32_port_speed_read(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);
extern int mt2_xgsd_reg32_port_speed_write(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);
extern int mt2_xgsd_reg64_port_speed_read(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);
extern int mt2_xgsd_reg64_port_speed_write(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);

extern int mt2_xgsd_reg32_blocks_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg32_blocks_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);

extern int mt2_xgsd_mem_blocks_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *entry_data,
				int size);
extern int mt2_xgsd_mem_blocks_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *entry_data,
				int size);
extern int mt2_xgsd_mem_read(int unit, uint32 block, uint32 addr,
			uint32 idx, void *vptr, int size);
extern int mt2_xgsd_mem_write(int unit, uint32 block, uint32 addr,
			uint32 idx, void *vptr, int size);

extern uint32 *mt2_field_get(const uint32 * entbuf, int sbit, int ebit,
			uint32 * fbuf);
extern void mt2_field_set(uint32 * entbuf, int sbit, int ebit,
			uint32 * fbuf);
extern uint32 mt2_field32_get(const uint32 * entbuf, int sbit, int ebit);
extern void mt2_field32_set(uint32 * entbuf, int sbit, int ebit,
			uint32 fval);

extern int mt2_sbus_reg_read(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, uint32 * data);
extern int mt2_sbus_reg_write(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, uint32 * data);
extern int mt2_sbus_mem_read(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, int idx, uint32 * data);
extern int mt2_sbus_mem_write(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, int idx, uint32 * data);
extern int mt2_sbus_to_tsc_read(int unit, uint16 phyaddr, uint32 addr,
			uint32 * data);
extern int mt2_sbus_to_tsc_write(int unit, uint16 phyaddr, uint32 addr,
			uint32 * data);
extern int mt2_axi_read(int unit, uint16 phyaddr, uint32 regaddr,
			uint32 * data);
extern int mt2_axi_write(int unit, uint16 phyaddr, uint32 regaddr,
			uint32 * data);

#endif  /* _PHY8806X_SYMS_H_ */

