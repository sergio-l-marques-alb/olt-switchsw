/*
 * $Id: wb_db_counters.h,v 1.5 Broadcom SDK $
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
 * File:        wb_db_counters.h
 * Purpose:     WarmBoot - Level 2 support (COUNTERS Module)
 */

#ifndef _BCM_DPP_WB_DB_COUNTERS_H_
#define _BCM_DPP_WB_DB_COUNTERS_H_


#include <bcm_int/dpp/counters.h>

/*
 *  Per counter processor information
 *
 *  this struct is a complementary struct for _bcm_dpp_counter_proc_info_t
 *  found in counters.h. it holds data that needs to be stored in non-volatile
 *  memory for warm boot recovery.
 */
typedef struct _bcm_dpp_wb_counter_proc_info_s {
    unsigned int allocated;                 /* number of allocated sets */
    uint32 *inUse;                           /* bits indicating in use sets */
} _bcm_dpp_wb_counter_proc_info_t;

/*
 *  Per unit information
 *
 *  this struct is a complementary struct for _bcm_dpp_counter_state_t
 *  found in counters.h. it holds data that needs to be stored in non-volatile
 *  memory for warm boot recovery.
 */
typedef struct _bcm_dpp_wb_counter_state_s {
    _bcm_dpp_wb_counter_proc_info_t *proc;     /* per processor information */
#if defined(BCM_WARM_BOOT_SUPPORT)
    _bcm_dpp_counter_state_t        *counter_state_ptr; /* pointer for the non-wb data held in counters.c*/
#endif
} _bcm_dpp_wb_counter_state_t;

extern _bcm_dpp_wb_counter_state_t *_bcm_dpp_wb_counter_state[SOC_MAX_NUM_DEVICES];

extern int
_bcm_dpp_wb_counters_proc_allocate(int unit, long unsigned int allocSize);

extern int
_bcm_dpp_wb_counters_counter_state_allocate(int unit, uint32 nof_procs);

extern int
_bcm_dpp_wb_counters_proc_free(int unit);

extern int
_bcm_dpp_wb_counters_dealloc(int unit);

extern int
_bcm_dpp_wb_counters_proc_allocated_set(int unit, unsigned int allocated, uint32 proc_indx);

extern int
_bcm_dpp_wb_counters_proc_allocated_get(int unit, unsigned int *allocated, uint32 proc_indx);

extern int
_bcm_dpp_wb_counters_proc_inUse_allocate(int unit, long unsigned int allocSize, uint32 proc_indx);

extern int
_bcm_dpp_wb_counters_proc_inUse_free(int unit, uint32 proc_indx);

extern int
_bcm_dpp_wb_counters_proc_inUse_is_allocated(int unit, uint8 *is_alloc, uint32 proc_indx);

extern int
_bcm_dpp_wb_counters_proc_inUse_set(int unit, uint32 inUse, uint32 proc_indx, uint32 inUse_indx);

extern int
_bcm_dpp_wb_counters_proc_inUse_get(int unit, uint32 *inUse, uint32 proc_indx, uint32 inUse_indx);

#if defined(BCM_WARM_BOOT_SUPPORT)

/* saving counters cache can be quit big (2MB for ARAD, 512KB for SOC_PETRA-B) */
/* Enable this flag when: */
/* There is a requirement (from a customer) to keep persistent counters persistent, */
/* even if some data will be lost across a restart/reload/upgrade.   */
/* #define BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

typedef _bcm_dpp_wb_counter_state_t counters_cntl_t;

/*
 * runtime information
 */
typedef struct bcm_dpp_wb_counters_info_s {
    int                      init_done;
    counters_cntl_t          *counters_config;

    int                      is_dirty;

    uint16                   version;
    uint8                   *scache_ptr;
    int                      size;

    uint32                   background_collection_enable_num;
    uint32                   background_collection_enable_off;

    uint32                   bgWait_num;
    uint32                   bgWait_off;

    uint32                   background_disable_num;
    uint32                   background_disable_off;

    uint32                   fifo_read_background_num;
    uint32                   fifo_read_background_off;

    uint32                   fifo_read_deferred_num;
    uint32                   fifo_read_deferred_off;

    uint32                   fifo_read_passes_num;
    uint32                   fifo_read_passes_off;

    uint32                   fifo_read_fails_num;
    uint32                   fifo_read_fails_off;

    uint32                   fifo_read_items_num;
    uint32                   fifo_read_items_off;

    uint32                   fifo_read_max_num;
    uint32                   fifo_read_max_off;

    uint32                   fifo_read_last_num;
    uint32                   fifo_read_last_off;

    uint32                   direct_read_passes_num;
    uint32                   direct_read_passes_off;

    uint32                   direct_read_fails_num;
    uint32                   direct_read_fails_off;

    uint32                   cache_updates_num;
    uint32                   cache_updates_off;

    uint32                   cache_reads_num;
    uint32                   cache_reads_off;

    uint32                   cache_writes_num;
    uint32                   cache_writes_off;

    uint32                   api_reads_num;
    uint32                   api_reads_off;

    uint32                   api_writes_num;
    uint32                   api_writes_off;

    uint32                   api_miss_reads_num;
    uint32                   api_miss_reads_off;

    uint32                   api_miss_writes_num;
    uint32                   api_miss_writes_off;

    uint32                   allocated_num;
    uint32                   allocated_off;

    uint32                   mode_num;
    uint32                   mode_off;

    uint32                   valid_num;
    uint32                   valid_off;

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
    uint32                   counter_num;
    uint32                   counter_off;
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

    uint32                   inUse_num;
    uint32                   inUse_off;

} bcm_dpp_wb_counters_info_t;


extern bcm_dpp_wb_counters_info_t   *_dpp_wb_counters_info_p[BCM_MAX_NUM_UNITS];

/*
 * Functions
 */
extern int
_bcm_dpp_wb_counters_state_init(int unit, _bcm_dpp_counter_state_t *counter_state);

extern int
_bcm_dpp_wb_counters_state_deinit(int unit);

extern int
_bcm_dpp_wb_counters_sync(int unit);


extern int
_bcm_dpp_wb_counters_update_background_collection_enable_state(int unit);

extern int
_bcm_dpp_wb_counters_update_bgWait_state(int unit);

extern int
_bcm_dpp_wb_counters_update_background_disable_state(int unit);

extern int
_bcm_dpp_wb_counters_update_fifo_read_background_state(int unit);

extern int
_bcm_dpp_wb_counters_update_fifo_read_deferred_state(int unit);

extern int
_bcm_dpp_wb_counters_update_fifo_read_passes_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_fifo_read_fails_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_fifo_read_items_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_fifo_read_max_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_fifo_read_last_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_direct_read_passes_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_direct_read_fails_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_cache_updates_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_cache_reads_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_cache_writes_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_api_reads_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_api_writes_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_api_miss_reads_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_api_miss_writes_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_allocated_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_mode_state(int unit, uint32 arr_indx);

extern int
_bcm_dpp_wb_counters_update_valid_state(int unit, uint32 arr_indx);

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
extern int
_bcm_dpp_wb_counters_update_counter_state(int unit, uint32 arr_indx1, uint32 arr_indx2);
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

extern int
_bcm_dpp_wb_counters_update_inUse_state(int unit, uint32 arr_indx1, uint32 arr_indx2);



#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_COUNTERS_H_ */
