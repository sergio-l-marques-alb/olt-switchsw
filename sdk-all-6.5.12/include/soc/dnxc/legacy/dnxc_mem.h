/*
 * $Id: dnxc_mem.h,v 1.2 Broadcom SDK $
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DNXC_MEM_H
#define _SOC_DNXC_MEM_H

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jer2) and DNXF family only!" 
#endif

#include <soc/mcm/allenum.h>

/*  check if dnx table is dynamic */
int dnxc_tbl_is_dynamic(int unit,soc_mem_t mem);

/* check if a given mem contain one of the fields apear in given list*/
int dnxc_mem_contain_one_of_the_fields(int unit,const soc_mem_t mem,soc_field_t *fields);


/*
 * fill the memory each entry value will be entry_id 
 * In a case field != INVALIDf the entry will be set to 0 and just the spcified field will be set to entry_id. 
 * This function use DMA doe efficency. 
 */
int dnxc_fill_table_with_index_val(
    int       unit,
    soc_mem_t mem,        /* memory/table to fill */
    soc_field_t field    /*field to set*/
  );

/* 
 * This function reads from all cached memories in order to detect and fix SER errors
 */
uint32
soc_dnxc_cache_table_update_all(int unit);

/*
 * Structures and prototypes related to PEM block access.
 * {
 */
int
dnx_do_read_table(int unit, soc_mem_t mem, unsigned array_index,
                  int index, int count, uint32 *entry_ptr) ;

/*
 * }
 */

/* SBUS defines */
#ifdef BCM_SBUSDMA_SUPPORT
#define SOC_DNXC_MAX_SBUSDMA_CHANNELS    3
#define SOC_DNXC_TDMA_CHANNEL            0
#define SOC_DNXC_TSLAM_CHANNEL           1
#define SOC_DNXC_DESC_CHANNEL            2
#define SOC_DNXC_MEM_CLEAR_CHUNK_SIZE    4 /* Use one entry buffers for SLAM DMA */
#endif /*BCM_SBUSDMA_SUPPORT*/

#endif /*_SOC_DNXC_MEM_H*/
