/*
 * $Id: dcmn_mem.h,v 1.2 Broadcom SDK $
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DCMN_MEM_H
#define _SOC_DCMN_MEM_H
#include <soc/mcm/allenum.h>

/*  check if dnx table is dynamic */
int dcmn_tbl_is_dynamic(int unit,soc_mem_t mem);

/* check if a given mem contain one of the fields apear in given list*/
int dcmn_mem_contain_one_of_the_fields(int unit,const soc_mem_t mem,soc_field_t *fields);

/* 
 * This function reads from all cached memories in order to detect and fix SER errors
 */
uint32
soc_dcmn_cache_table_update_all(int unit);

int
dcmn_mem_array_wide_access(int unit, uint32 flags, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,unsigned operation);

/*
 * Structures and prototypes related to PEM block access.
 * {
 */
int
dpp_do_read_table(int unit, soc_mem_t mem, unsigned array_index,
                  int index, int count, uint32 *entry_ptr) ;

/*
 * }
 */

/* SBUS defines */
#ifdef BCM_SBUSDMA_SUPPORT
#define SOC_DCMN_MAX_SBUSDMA_CHANNELS    3
#define SOC_DCMN_TDMA_CHANNEL            0
#define SOC_DCMN_TSLAM_CHANNEL           1
#define SOC_DCMN_DESC_CHANNEL            2
#define SOC_DCMN_MEM_CLEAR_CHUNK_SIZE    4 /* Use one entry buffers for SLAM DMA */
#endif /*BCM_SBUSDMA_SUPPORT*/

#endif /*_SOC_DCMN_MEM_H*/
