/*
 * $Id: dnxc_mem.h,v 1.2 Broadcom SDK $
 * 
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DNXC_MEM_H
#define _SOC_DNXC_MEM_H

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jr2) and DNXF family only!"
#endif

#include <soc/mcm/allenum.h>

/*
 * fill the memory each entry value will be entry_id 
 * In a case field != INVALIDf the entry will be set to 0 and just the spcified field will be set to entry_id. 
 * This function use DMA doe efficency. 
 */
int dnxc_fill_table_with_index_val(
    int unit,
    soc_mem_t mem,              /* memory/table to fill */
    soc_field_t field           /* field to set */
    );

/*
 * }
 */

/* SBUS defines */
#ifdef BCM_SBUSDMA_SUPPORT
#define SOC_DNXC_MAX_SBUSDMA_CHANNELS    3
#define SOC_DNXC_TDMA_CHANNEL            0
#define SOC_DNXC_TSLAM_CHANNEL           1
#define SOC_DNXC_DESC_CHANNEL            2
#define SOC_DNXC_MEM_CLEAR_CHUNK_SIZE    4      /* Use one entry buffers for SLAM DMA */
#endif /* BCM_SBUSDMA_SUPPORT */

#ifdef BCM_DNX_SUPPORT
int dnx_mem_read_eps_protect_start(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 *eps_protect_flag,
    soc_reg_above_64_val_t * eps_shaper_set);

int dnx_mem_read_eps_protect_end(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 *eps_protect_flag,
    soc_reg_above_64_val_t * eps_shaper_set);
#endif

#endif /*_SOC_DNXC_MEM_H*/
