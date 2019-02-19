/* $Id: register.h,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        register.h
 * Purpose:     Base definitions for SBX register types
 * Requires:    
 */

#ifndef _SOC_SBX_REGISTER_H
#define _SOC_SBX_REGISTER_H

#define SBX_DEV_NAME_LEN_MAX 64
#define SBX_REG_NAME_LEN_MAX 128
#define SBX_REG_FIELDS_MAX  64
#define SBX_REG_NUM_MAX 6400

#define DUMP_TABLE_RAW          0x01
#define DUMP_TABLE_HEX          0x02
#define DUMP_TABLE_ALL          0x04
#define DUMP_TABLE_CHANGED      0x08
#define DUMP_TABLE_VERTICAL     0x10

#ifndef BCM_CALADAN3_SUPPORT
#include <soc/sbx/sbTypesGlue.h>
#endif
#include <appl/diag/shell.h>
#include <appl/diag/sbx/field.h>

typedef int soc_sbx_reg_idx;
#define SOC_SBX_REG_IS_INDIRECT 0x80000000
typedef struct soc_sbx_reg_info_s {
    char    name[SBX_REG_NAME_LEN_MAX];
    uint32  offset;
    uint32  ioffset;
    uint32  nt_mask;
    uint32  mask;
    uint8   msb;
    uint8   lsb;
    uint8   intreg;
    uint8   nfields;
    soc_sbx_field_info_t *fields[SBX_REG_FIELDS_MAX];
} soc_sbx_reg_info_t;

typedef struct soc_sbx_reg_info_list_s {
    int     count;
    soc_sbx_reg_idx idx[SBX_REG_NUM_MAX];
} soc_sbx_reg_info_list_t;

typedef struct soc_sbx_chip_info_s {
    char    name[SBX_DEV_NAME_LEN_MAX];
    int     id;
    int     nregs;
    int     maxreg;
    soc_sbx_reg_info_t** regs;
} soc_sbx_chip_info_t;

extern cmd_result_t sbx_chip_info_get(int chip_id,
                                      soc_sbx_chip_info_t **chip_info,
				      int create);

extern cmd_result_t
cmd_sbx_cmic_do_dump_table(int unit, soc_mem_t mem,
              int copyno, int index, int count, int flags);

#endif	/* !_SOC_SBX_REGISTER_H */
