/*
 * $Id: pstats.h $
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
 * File for SOC Pktstats structures and routines
 */

#ifndef __SOC_PSTATS_H__
#define __SOC_PSTATS_H__

typedef struct soc_pstats_mem_desc {
    soc_mem_t mem;
    uint32 width;
    uint32 entries;
    uint32 shift;
} soc_pstats_mem_desc_t;

#define MAX_PSTATS_MEM_PER_BLK  10

typedef struct soc_pstats_tbl_desc {
    soc_block_t blk;
    soc_mem_t bmem;
    int pipe_enum;
    int mor_dma;
    soc_pstats_mem_desc_t desc[MAX_PSTATS_MEM_PER_BLK];
    int pattern_set;
    soc_mem_t mem;
    uint32 index;
    uint32 count;
} soc_pstats_tbl_desc_t;

typedef struct soc_pstats_tbl_ctrl {
    soc_block_t blk;
    uint32 tindex;
    uint32 entries;
    uint8 *buff;
    uint32 size;
    int flags;
    sal_sem_t dma_done;
} soc_pstats_tbl_ctrl_t;

extern int soc_pstats_init(int unit);
extern int soc_pstats_deinit(int unit);
extern int soc_pstats_tbl_pattern_get(int unit, soc_pstats_tbl_desc_t *tdesc,
                                      soc_mem_t *mem, uint32 *index, uint32 *count);
extern int soc_pstats_tbl_pattern_set(int unit, soc_pstats_tbl_desc_t *tdesc,
                                      soc_mem_t mem, uint32 index, uint32 count);
extern int soc_pstats_sync(int unit);
extern int soc_pstats_mem_get(int unit, soc_mem_t mem, uint8 *buf, int sync);

#endif /* __SOC_PSTATS_H__ */

