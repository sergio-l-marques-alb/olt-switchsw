/*
 * $Id$
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
 * File:        wb_db_alloc.h
 * Purpose:     WarmBoot - Level 2 support (ALLOC Module)
 */

#ifndef _BCM_DPP_WB_DB_ALLOC_H_
#define _BCM_DPP_WB_DB_ALLOC_H_


#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/wb_db_cosq.h>
#include <bcm_int/dpp/alloc_mngr.h>


#if defined(BCM_WARM_BOOT_SUPPORT)

#define BCM_DPP_WB_ALLOC_VERSION_1_0           SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_ALLOC_VERSION_1_1           SOC_SCACHE_VERSION(1,1)

#define BCM_DPP_WB_ALLOC_CURRENT_VERSION       BCM_DPP_WB_ALLOC_VERSION_1_1

#define BCM_DPP_WB_ALLOC_INFO(unit)            (_dpp_wb_alloc_info_p[unit])
#define BCM_DPP_WB_ALLOC_INFO_INIT(unit)       ((BCM_DPP_WB_ALLOC_INFO(unit) != NULL) && \
                                                (BCM_DPP_WB_ALLOC_INFO(unit)->init_done == TRUE) )

#define BCM_DPP_WB_DEV_ALLOC_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                        \
                        SOC_CONTROL(unit)->scache_dirty = 1;                           \
                        BCM_DPP_WB_ALLOC_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);



/* 
 * This is the base entry which can be either of the following:
 * 
 * 1. each bit indicates whether an entry is allocated or not (valid when only 1 entry allocated at a time).
 *    this gives 32 valid entries.
 *
 * 2. 1 bit indicates whether an entry is valid and the next 2 bits indicate the number of entries allocated.
 *    In this case, 1 valid bit and 3 size bits are used in each nibble.  10 entries in each 32 bit word.
 */ 
typedef uint32 shr_wb_alloc_pool_entry_t;

typedef int (*shr_wb_alloc_restore_cb_t)(int unit,  
                                         bcm_dpp_am_pool_info_t *pool_info, 
                                         uint32 entry_id, 
                                         uint32 count);

typedef int (*shr_wb_alloc_template_restore_cb_t)(int unit,  
                                                  bcm_dpp_am_template_info_t *template_info, 
                                                  int entry_id,  
                                                  int ref_cnt,
                                                  void *data);

typedef struct shr_wb_alloc_pool_desc_s {
    soc_scache_handle_t          scache_handle;       /* handle for cache storage for r/w persistent store   */
    uint8                        *scache_ptr;         /* pointer to cache storage for r/w persistent store   */
    int                          scache_offset;       /* offset to cache storate from ptr for this pool      */
    int                          pool_size_in_bytes;  /* size of compressed pool in bytes                    */
    shr_wb_alloc_restore_cb_t    cb;                 /* callback function to reallocate entry for this pool */
    bcm_dpp_am_pool_info_t       pool_info;

} shr_wb_alloc_pool_desc_t;

typedef struct bcm_dpp_wb_alloc_global_desc_s {
    soc_scache_handle_t          scache_handle;       /* handle for cache storage for r/w persistent store */
    uint8                        *scache_ptr;         /* pointer to cache storage for r/w persistant       */
    int                          scache_offset;       /* offset to cache storate from ptr for this pool    */
    int                          global_size_in_bytes; /* sum of all global data                           */
    int                          rsrc_db_size_in_bytes;
    int                          rsrc_db_offset;
    int                          rsrc_sz_size_in_bytes;
    int                          rsrc_sz_offset;
} bcm_dpp_wb_alloc_global_desc_t;


typedef struct shr_wb_alloc_template_entry_header_s {
    int template_id;
    int ref_cnt;
} shr_wb_alloc_template_entry_header_t;

typedef struct shr_wb_alloc_template_desc_s {
    soc_scache_handle_t          scache_handle;       /* handle for cache storage for r/w persistent store   */
    uint8                        *scache_ptr;         /* pointer to cache storage for r/w persistent store   */
    int                          scache_offset;       /* offset to cache storate from ptr for this pool      */
    int                          template_pool_size_in_bytes;/* size of pool                                 */
    shr_wb_alloc_template_restore_cb_t cb;           /* callback function to reallocate entry for this pool */
    bcm_dpp_am_template_info_t   template_info;

} shr_wb_alloc_template_desc_t;


/*
 * Alloc_mngr warmboot state
 */
typedef struct bcm_dpp_wb_alloc_info_s {
    int                          init_done;           /* bcm_init() is complete                            */
    int                          is_dirty;            /* cache has not been written to persistent storage  */
    uint16                       version;             /* warmboot version                                  */
    soc_scache_handle_t          scache_handle;       /* handle for non-persistent cache storage           */
    uint8                       *scache_ptr;          /* pointer to non-persistent cache storage           */
    int                          total_size_in_bytes; /* total size of alloc_mngr wb state                 */
    shr_wb_alloc_pool_desc_t     *pool_desc;          /* descriptor for each pool                          */
    bcm_dpp_wb_alloc_global_desc_t global_desc;       /* Descriptor for global data.                       */
    shr_wb_alloc_template_desc_t *template_desc;      /* desc for each template pool                       */
} bcm_dpp_wb_alloc_info_t;

extern bcm_dpp_wb_alloc_info_t *_dpp_wb_alloc_info_p[BCM_MAX_NUM_UNITS];

/*
 * Functions
 */
extern int
_bcm_dpp_wb_alloc_state_init(int unit, 
                             bcm_dpp_am_pool_info_t *pool_info, 
                             int num_pools, 
                             bcm_dpp_am_template_info_t *template_info, 
                             int num_template_pools);

extern int
bcm_dpp_wb_alloc_update(int unit, int pool_id, int elem_count, int elem_start);

extern int
bcm_dpp_wb_dealloc_update(int unit, int pool_id, int elem_count, int elem_start);

extern int
bcm_dpp_wb_template_dealloc_update(int unit, int template_type, int template_id, uint8 clear_entry);

extern int
bcm_dpp_wb_template_alloc_update(int unit, int template_type, const void *data, int template_id, int ref_cnt);

int 
bcm_dpp_wb_template_clear_update(int unit, int template_type);
 
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_ALLOC_H_ */
