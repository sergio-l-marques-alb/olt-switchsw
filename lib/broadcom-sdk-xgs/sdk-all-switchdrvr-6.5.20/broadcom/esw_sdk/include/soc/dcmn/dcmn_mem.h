/*
 * $Id: dcmn_mem.h,v 1.2 Broadcom SDK $
 * 
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _SOC_DCMN_MEM_H
#define _SOC_DCMN_MEM_H
#include <soc/mcm/allenum.h>


int
dcmn_mem_array_wide_access(int unit, uint32 flags, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,unsigned operation);

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
