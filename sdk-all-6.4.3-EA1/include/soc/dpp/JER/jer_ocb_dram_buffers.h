/*
 * $Id: jer_ocb_dram_buffers.h Exp $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 */

#ifndef _JER_OCB_DRAM_BUFFERS_H
#define _JER_OCB_DRAM_BUFFERS_H

/*
 * Includes
 */

/*
 * TypeDefs
 */

typedef struct soc_jer_ocb_dram_dbuf_val_s {
    uint32 start;
    uint32 end;
    uint32 size;
} soc_jer_ocb_dram_dbuf_val_t;


typedef struct jer_init_dbuffs_bdry_s {
    soc_jer_ocb_dram_dbuf_val_t ocb_0;
    soc_jer_ocb_dram_dbuf_val_t ocb_1;
    soc_jer_ocb_dram_dbuf_val_t fmc_0;
    soc_jer_ocb_dram_dbuf_val_t fmc_1;
    soc_jer_ocb_dram_dbuf_val_t mnmc_0;
    soc_jer_ocb_dram_dbuf_val_t mnmc_1;
    soc_jer_ocb_dram_dbuf_val_t fbc_fmc;
    soc_jer_ocb_dram_dbuf_val_t fbc_mnmc;
} soc_jer_ocb_dram_dbuffs_bound_t;

typedef struct jer_init_dbuffs_s {
    soc_jer_ocb_dram_dbuffs_bound_t  dbuffs_bdries;
    uint32                           max_nof_dram_buffers;
} soc_jer_ocb_dram_dbuffs_t;

/*
 * Defines
 */

#define JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE 0
#define JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE 1
#define JERICHO_INIT_FMC_NO_REP_DBUFF_MODE      2

#define JER_OCB_BANK_NUM 16
#define JER_OCB_BANK_SIZE (4 * 1024)

/*
 * Functions
 */
int soc_jer_str_prop_mc_nbr_full_dbuff_get(int unit, int *p_val);
int soc_jer_ocb_dram_buffer_conf_calc(int unit);
int soc_jer_ocb_dram_buffer_conf_set(int unit);
int soc_jer_ocb_conf_set(int unit);


#endif /* _JER_OCB_DRAM_BUFFERS_H */
