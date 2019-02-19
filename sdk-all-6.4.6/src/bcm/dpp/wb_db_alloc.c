/*
 * $Id: wb_db_alloc.c,v 1.61 Broadcom SDK $
 *
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
 * Warmboot - Level 2 support (ALLOC Library)
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/module.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm/vlan.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/wb_db_gport.h>
#include <bcm_int/dpp/wb_db_alloc.h>
#include <bcm_int/dpp/wb_db_cosq.h>
#include <bcm_int/dpp/policer.h>
#include <soc/dpp/soc_sw_db.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/sw_db.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_WARM_BOOT_SUPPORT)
/**********************************************************************
 * Overview   This module saves the pool and template information for *
 *            during coldboot when BCM_WARM_BOOT_SUPPORT is defined   *
 *            and restores the data during startup during warmboot    *
 *            initialization.                                         *
 *                                                                    *
 * Resource   Resource pools are stored in 2 different ways.  If only *
 *    Pools   a single resource is allocated at a time, the pool type *
 *            is designated as SINGLE, otherwise it is MULTI.  The    *
 *            SINGLE type stores each resource as a single bit in a   *
 *            32 bit word.  The MULTI type stores a 1 bit Valid       *
 *            indication and a 2 bit count.                           *
 *                                                                    *
 *                     31     24        15         7         0        *
 *            single   0000 0000 0000 0000 0000 0000 0000 0000        *
 *                     1 in bit means in use, 0 means not in use      *
 *                                                                    *
 *                     31     24        15         7         0        *
 *            multi    0000 0000 0000 0000 0000 0000 0000 0000        *
 *            entry      9   8   7  6   5   4   3  2   1   0          *
 *                       vc cvcc vccv ccvc cvcc vccv ccvc cvcc        *
 *                      v indicates valid bit, c indicates count      *
 *                                                                    *
 * Template   Template pools consist of a template id a reference     *
 *    Pools   count and the template data as follows                  *
 *                                                                    *
 *                     31     24        15         7         0        *
 *                     0000 0000 0000 0000 0000 0000 0000 0000        *
 *                     [              template_id            ]        *
 *                     [             reference cnt           ]        *
 *                     [            associated data          ]        *                               
 **********************************************************************/

/* Warmboot allocation manager state */
bcm_dpp_wb_alloc_info_t *_dpp_wb_alloc_info_p[BCM_MAX_NUM_UNITS] = {0};

/* Warmboot pool information */
#define BCM_DPP_WB_POOL_VALID(pool_info) (((pool_info) != NULL) && (pool_info->count > 0))
/* Debug defines */
/* #define SHR_DEBUG_WB_ALLOC 1 */
/* #define SHR_DEBUG_POOL_ID 0 to debug a particular resource pool */
/* #define SHR_DEBUG_WB_TEMPLATE_ALLOC 1 */
/* #define SHR_DEBUG_TEMPLATE_POOL_ID 9 */
/* #define SHR_DEBUG_WB_ALLOC_SYNC_INIT */
/* static functions */
STATIC int
_bcm_dpp_wb_alloc_info_alloc(int unit, int nof_resources_pools, int nof_template_pools);
STATIC int
_bcm_dpp_wb_alloc_layout_init(int unit,
                              int version, 
                              bcm_dpp_am_pool_info_t *pool_info, 
                              int nof_resource_pools,
                              bcm_dpp_am_template_info_t *template_info,
                              int num_template_pools);
STATIC int
_bcm_dpp_wb_alloc_restore_state(int unit, int nof_resource_pools, int nof_template_pools);
STATIC int
_bcm_dpp_wb_alloc_info_dealloc(int unit);
/* RES POOL Low layer shared functions */
STATIC int _shr_wb_alloc_res_pool_info_init(int unit, 
                                            bcm_dpp_am_pool_info_t *pool_info, 
                                            shr_wb_alloc_pool_desc_t *pool_desc);
STATIC int
_shr_wb_alloc_pool_entry_alloc(int unit, 
                               shr_wb_alloc_pool_desc_t* pool_desc, 
                               int elem_count, 
                               int elem_start, 
                               shr_wb_alloc_pool_entry_t* pool_entry,
                               int *pool_offset);
STATIC int
_shr_wb_alloc_pool_entry_dealloc(int unit, 
                                 shr_wb_alloc_pool_desc_t* pool_desc, 
                                 int elem_count, 
                                 int elem_start, 
                                 shr_wb_alloc_pool_entry_t* pool_entry,
                                 int *pool_offset);
STATIC int
_shr_wb_alloc_template_pool_entry_is_alloc(int unit,
                                        int template_type,
                                        int template_id);
STATIC int 
_shr_wb_alloc_res_pool_restore_entry(int unit, 
                                     shr_wb_alloc_pool_desc_t *pool_desc, 
                                     int entry_id);

STATIC int 
_shr_wb_alloc_res_pool_restore(int unit, 
                               shr_wb_alloc_pool_desc_t *pool_desc);

STATIC int _shr_wb_alloc_res_pool_size_in_bytes_get(int unit, 
                                                    shr_wb_alloc_pool_desc_t *pool_desc,
                                                    int *pool_size_in_bytes);

/* TEMPLATE Low layer shared functions */
STATIC int _shr_wb_alloc_template_pool_size_in_bytes_get(int unit, 
                                                         shr_wb_alloc_template_desc_t *template_desc,
                                                         int *template_pool_size_in_bytes);
STATIC int _shr_wb_alloc_template_pool_info_init(int unit, 
                                                 bcm_dpp_am_template_info_t *template_info, 
                                                 shr_wb_alloc_template_desc_t *template_desc);

STATIC int _shr_wb_alloc_template_pool_entry_size_in_bytes_get(int unit, 
                                                               shr_wb_alloc_template_desc_t *template_desc,
                                                               int *template_entry_size_in_bytes);
STATIC int
_shr_wb_alloc_template_pool_entry_alloc(int unit,
                                        shr_wb_alloc_template_desc_t* template_desc,
                                        int template_id,
                                        int ref_cnt,
                                        const void *data,
                                        void *template_wb_data,
                                        int   *template_wb_offset,
                                        int   template_wb_data_size_in_bytes);

STATIC int 
_shr_wb_alloc_template_pool_entry_scache_header_get(int unit,
                                                    shr_wb_alloc_template_desc_t *template_desc, 
                                                    int *template_id, 
                                                    int *template_wb_offset, 
                                                    shr_wb_alloc_template_entry_header_t **scache_header_ptr);

STATIC int
_shr_wb_alloc_template_pool_entry_dealloc(int unit, 
                                          shr_wb_alloc_template_desc_t* template_desc, 
                                          int template_id, 
                                          void *template_wb_data,
                                          int *template_wb_offset,
                                          int template_wb_data_size_in_bytes,
                                          uint8 clear_entry);
STATIC int 
_shr_wb_alloc_template_pool_restore_entry(int unit, 
                                          shr_wb_alloc_template_desc_t *template_desc, 
                                          int entry_id);
STATIC int 
_shr_wb_alloc_template_pool_restore(int unit, 
                                    shr_wb_alloc_template_desc_t *template_desc);


/* Single bit per entry defines */
#define SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRY_MASK        0x1
#define SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD   32

/* Multiple bits per entry defines 1 bit valid 2 bits count */
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_PER_ENTRY       3
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRY_MASK         0x7
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_VALID_MASK         0x4
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_COUNT_MASK         0x3
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_COUNT_PER_ENTRY 2
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD    10
#define SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY      (1 << SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_COUNT_PER_ENTRY)


/* Global functions */
/*
 * The save functionality has been implemented
 *   - some consistency checks rather then blindly copying data structures
 *     Thus easier to debug, catch errors.
 *   - The implementation also make its symmetric with the the per
 *     API update.
 */

int
_bcm_dpp_wb_alloc_state_init(int unit, 
                             bcm_dpp_am_pool_info_t *pool_info, 
                             int nof_resource_pools,
                             bcm_dpp_am_template_info_t *template_info,
                             int nof_template_pools)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_ALLOC_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_alloc_info_t *wb_info; 

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * BCM_MODULE_COUNT + 2 is unique number used to generate key.
     * normally it would be fabric, cosq, stack, etc... but for shared
     * modules these aren't defined.  No other shared module can use this #. 
     */
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, (BCM_MODULE_WB_ALOOC_MNGR), 0);

    /* Allocate warmboot state */
    rc = _bcm_dpp_wb_alloc_info_alloc(unit, nof_resource_pools, nof_template_pools);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);
    wb_info->scache_handle = wb_handle;

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetrieve, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_alloc_layout_init(unit, recovered_ver, pool_info, nof_resource_pools,
                                           template_info, nof_template_pools);
        BCMDNX_IF_ERR_EXIT(rc);

        size = 0;

        /* restore state */
        rc = _bcm_dpp_wb_alloc_restore_state(unit, nof_resource_pools, nof_template_pools);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {

            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_alloc_layout_init(unit, version, pool_info, nof_resource_pools,
                                               template_info, nof_template_pools);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->total_size_in_bytes;

            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_ALLOC_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* Calculate size to allocate scache */
        rc = _bcm_dpp_wb_alloc_layout_init(unit, version, pool_info, nof_resource_pools, 
                                           template_info, nof_template_pools);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->total_size_in_bytes;

        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* Ensure that scache is cleared - ref_cnts are incremented and decremented so memory must be 0 initially */
        sal_memset(scache_ptr, 0, size);

        /* layout corresponding to recovered version and update scache ptrs in pools */
        rc = _bcm_dpp_wb_alloc_layout_init(unit, version, pool_info, nof_resource_pools, 
                                           template_info, nof_template_pools);
        BCMDNX_IF_ERR_EXIT(rc);

        /* writing to persistent storage initiated by bcm_petra_init() */
        BCM_DPP_WB_DEV_ALLOC_DIRTY_BIT_SET(unit);
    }

    wb_info->init_done = TRUE;

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_alloc_state_deinit(int unit)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_alloc_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Update the warmboot database for the given pool 
 */
STATIC int
_bcm_dpp_wb_alloc_update(int unit, int pool_id, int elem_count, int elem_start)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_alloc_info_t *wb_info;
    soc_scache_handle_t wb_handle;
    shr_wb_alloc_pool_entry_t pool_entry;
    int pool_offset=0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);

    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */

    if (!(BCM_DPP_WB_ALLOC_INFO_INIT(unit))) {
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: warning call to alloc_update before init. pool(%d)\n"), pool_id)); 
        BCM_EXIT;
    }

    /* updating framework cache regardless of immediate sync status (enabled or disabled)  */
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, (BCM_MODULE_WB_ALOOC_MNGR), 0);

    switch(wb_info->version) {
        case BCM_DPP_WB_ALLOC_VERSION_1_0:
        case BCM_DPP_WB_ALLOC_VERSION_1_1:

            LOG_DEBUG(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "\nwb: alloc_mngr - update pool(%d) elem(%d) cnt(%d)\n"), 
                       pool_id, elem_start, elem_count));
            
            rc = _shr_wb_alloc_pool_entry_alloc(unit, &wb_info->pool_desc[pool_id], 
                                                elem_count, elem_start, &pool_entry,
                                                &pool_offset);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done. */
        /* upon later sync request (bcmSwitchControlSync) the dirty framework cache will be writen to persistent storage */
        BCM_DPP_WB_DEV_ALLOC_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    /* only when immediate sync is enabled, updating persistent storage 'on the fly'  */
    rc = soc_scache_commit_specific_data(unit, wb_handle, sizeof(shr_wb_alloc_pool_entry_t),
                                         (uint8*)&pool_entry, pool_offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Update the warmboot database for the given pool 
 * WB infrastructure supports on each update up to elem_count <= 4. 
 * To overcome it in minimal changes, run a for loop to internal function 
 */
int
bcm_dpp_wb_alloc_update(int unit, int pool_id, int elem_count, int elem_start)
{
    int elem_index, elem_cur_cnt;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    for (elem_index = elem_start; elem_index < (elem_start+elem_count); elem_index+=SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY) {
      /* Each call update 4 entries at a time instead in one chunk */
      /* WB supports up to 4 entries allocation in one call */
      elem_cur_cnt = (elem_start+elem_count-elem_index);
      rc = _bcm_dpp_wb_alloc_update(unit, pool_id, ((elem_cur_cnt > SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY) ? SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY:(elem_cur_cnt)), elem_index);
      BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Update the warmboot database for the given pool 
 */
STATIC int
_bcm_dpp_wb_dealloc_update(int unit, int pool_id, int elem_count, int elem_start)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_alloc_info_t *wb_info;
    soc_scache_handle_t wb_handle;
    shr_wb_alloc_pool_entry_t pool_entry;
    int pool_offset=0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);

    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */

    if (!(BCM_DPP_WB_ALLOC_INFO_INIT(unit))) {
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: warning call to dealloc_update before init. pool(%d)\n"), pool_id)); 
        BCM_EXIT;
    }

    /* updating framework cache regardless of immediate sync status (enabled or disabled)  */
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, (BCM_MODULE_WB_ALOOC_MNGR), 0);

    switch(wb_info->version) {
        case BCM_DPP_WB_ALLOC_VERSION_1_0:
        case BCM_DPP_WB_ALLOC_VERSION_1_1:

            LOG_DEBUG(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "\nwb: alloc_mngr - update pool(%d) elem(%d) cnt(%d)\n"), 
                       pool_id, elem_start, elem_count));
            
            rc = _shr_wb_alloc_pool_entry_dealloc(unit, &wb_info->pool_desc[pool_id], 
                                                  elem_count, elem_start, &pool_entry,
                                                  &pool_offset);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done. */
        /* upon later sync request (bcmSwitchControlSync) the dirty framework cache will be writen to persistent storage */
        BCM_DPP_WB_DEV_ALLOC_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    /* only when immediate sync is enabled, updating persistent storage 'on the fly'  */
    rc = soc_scache_commit_specific_data(unit, wb_handle, sizeof(shr_wb_alloc_pool_entry_t), 
                                         (uint8 *)&pool_entry, pool_offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Update the warmboot database for the given pool 
 * WB infrastructure supports on each update up to elem_count <= 4. 
 * To overcome it in minimal changes, run a for loop to internal function 
 */
int
bcm_dpp_wb_dealloc_update(int unit, int pool_id, int elem_count, int elem_start)
{
  int elem_index, elem_cur_cnt;
  int rc = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  for (elem_index = elem_start; elem_index < (elem_start+elem_count); elem_index+=SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY) {
    /* Each call update 4 entries at a time instead in one chunk */
    /* WB supports up to 4 entries deallocation in one call */
    elem_cur_cnt = (elem_start+elem_count-elem_index);
    rc = _bcm_dpp_wb_dealloc_update(unit, pool_id, ((elem_cur_cnt > SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY) ? SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY:(elem_cur_cnt)), elem_index);
    BCMDNX_IF_ERR_EXIT(rc);
  }

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Update the warmboot template database for the given pool 
 */
int
bcm_dpp_wb_template_alloc_update(int unit, int template_type, const void *data, int template_id, int ref_cnt)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_alloc_info_t *wb_info;
    soc_scache_handle_t wb_handle;
    uint8 *template_wb_data = NULL;
    int template_wb_data_size_in_bytes;
    int template_wb_offset;
#ifdef SHR_DEBUG_WB_ALLOC_SYNC_INIT
    static int dbg_count = 0;
    static int dbg_previous_template_type = -1;
#endif

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);

    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */

    if (!(BCM_DPP_WB_ALLOC_INFO_INIT(unit))) {
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: warning call to template_alloc_update before init. template(%d)\n"), template_id)); 
        BCM_EXIT;
    }

    /* updating framework cache regardless of immediate sync status (enabled or disabled)  */
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, (BCM_MODULE_WB_ALOOC_MNGR), 0);
    
    switch(wb_info->version) {
        
       case BCM_DPP_WB_ALLOC_VERSION_1_0:
       case BCM_DPP_WB_ALLOC_VERSION_1_1:
        
            LOG_DEBUG(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "\nwb: alloc_mngr - update template type(%d) id(%d)\n"), 
                       template_type, template_id));
        
            rc = _shr_wb_alloc_template_pool_entry_size_in_bytes_get(unit, &wb_info->template_desc[template_type],
                                                                     &template_wb_data_size_in_bytes);

            BCMDNX_ALLOC(template_wb_data, template_wb_data_size_in_bytes, "wb alloc temp template data");

            if (template_wb_data == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("wb alloc temp template data allocation failed")));
            }

            sal_memset(template_wb_data, 0x00, template_wb_data_size_in_bytes);


            rc = _shr_wb_alloc_template_pool_entry_alloc(unit, &wb_info->template_desc[template_type], 
                                                         template_id, ref_cnt, data, template_wb_data,
                                                         &template_wb_offset, template_wb_data_size_in_bytes);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }


#ifdef SHR_DEBUG_WB_ALLOC_SYNC_INIT
    if (template_type != dbg_previous_template_type) {
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "template pool(%d) allocate start cnt(%d)\n"), template_type, dbg_count));
        dbg_previous_template_type = template_type;
        dbg_count=0;
    } else {
        dbg_count++;
    }
#endif


    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* upon later sync request (bcmSwitchControlSync) the dirty framework cache will be writen to persistent storage */
        BCM_DPP_WB_DEV_ALLOC_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    /* only when immediate sync is enabled, updating persistent storage 'on the fly'  */
    rc = soc_scache_commit_specific_data(unit, wb_handle, template_wb_data_size_in_bytes,
                                         template_wb_data, 
                                         template_wb_offset);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if (template_wb_data != NULL) {
        BCM_FREE(template_wb_data);
    }

    BCMDNX_FUNC_RETURN;
}

/* 
 * Update the warmboot template database for the given pool 
 */
int
bcm_dpp_wb_template_dealloc_update(int unit, int template_type, int template_id, uint8 clear_entry)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_alloc_info_t *wb_info;
    soc_scache_handle_t wb_handle;
    uint8 *template_wb_data = NULL;
    int template_wb_data_size_in_bytes;
    int template_wb_offset;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);

    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */

    if (!(BCM_DPP_WB_ALLOC_INFO_INIT(unit))) {
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: warning call to template_dealloc_update before init. template(%d)\n"), template_id)); 
        BCM_EXIT;
    }


    /* updating framework cache regardless of immediate sync status (enabled or disabled)  */
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, (BCM_MODULE_WB_ALOOC_MNGR), 0);

    switch(wb_info->version) {
        case BCM_DPP_WB_ALLOC_VERSION_1_0:
        case BCM_DPP_WB_ALLOC_VERSION_1_1:

            LOG_DEBUG(BSL_LS_BCM_INIT,
                      (BSL_META_U(unit,
                                  "\nwb: alloc_mngr - update template pool(%d) deallocate template_id(%d)\n"), 
                       template_type, template_id));

            rc = _shr_wb_alloc_template_pool_entry_size_in_bytes_get(unit, &wb_info->template_desc[template_type],
                                                                     &template_wb_data_size_in_bytes);

            BCMDNX_ALLOC(template_wb_data, template_wb_data_size_in_bytes, "wb alloc temp template data");

            if (template_wb_data == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("wb alloc temp template data allocation failed")));
            }

            sal_memset(template_wb_data, 0x00, template_wb_data_size_in_bytes);

            rc = _shr_wb_alloc_template_pool_entry_dealloc(unit, &wb_info->template_desc[template_type], 
                                                           template_id, template_wb_data, &template_wb_offset,
                                                           template_wb_data_size_in_bytes, clear_entry);
            BCMDNX_IF_ERR_EXIT(rc);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* upon later sync request (bcmSwitchControlSync) the dirty framework cache will be writen to persistent storage */
        BCM_DPP_WB_DEV_ALLOC_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    /* only when immediate sync is enabled, updating persistent storage 'on the fly'  */
    rc = soc_scache_commit_specific_data(unit, wb_handle, template_wb_data_size_in_bytes,
                                         template_wb_data, 
                                         template_wb_offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if (template_wb_data != NULL) {
        BCM_FREE(template_wb_data);
    }

    BCMDNX_FUNC_RETURN;
}

int 
bcm_dpp_wb_template_clear_update(int unit, int template_type){
    int rv;
    int template_start, template_id, nof_templates;
    bcm_dpp_am_template_info_t   *template_info;
    BCMDNX_INIT_FUNC_DEFS;
    

    /* Get nof template ids for this template, and the first template. */
    template_info = &BCM_DPP_WB_ALLOC_INFO(unit)->template_desc[template_type].template_info;
    template_start = template_info->start;
    nof_templates = template_info->count;

    /* Iterate over the ids and clear them. */
    for (template_id = template_start ; template_id < template_start + nof_templates ; template_id++) {

        /* Only clear the entry if it exists. */
        rv = _shr_wb_alloc_template_pool_entry_is_alloc(unit, template_type, template_id);
        if (rv == BCM_E_EXISTS) {

            rv = bcm_dpp_wb_template_dealloc_update(unit, template_type, template_id, TRUE);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/******************
 * local functions
 ******************/
STATIC int
_bcm_dpp_wb_alloc_restore_resource_pools_state(int unit, int nof_resource_pools)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_alloc_info_t *wb_info;
    int pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);
    
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\n wb: alloc_mngr - restore Pool State\n")));
               
    switch(wb_info->version) {
        case BCM_DPP_WB_ALLOC_VERSION_1_0:
            for (pool_id=0; pool_id < (nof_resource_pools - 1); pool_id++) {
                /* Pool is not in use */
                if (wb_info->pool_desc[pool_id].cb != NULL) {
                    rc = _shr_wb_alloc_res_pool_restore(unit, &wb_info->pool_desc[pool_id]);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            }
            break;

        case BCM_DPP_WB_ALLOC_VERSION_1_1:
            for (pool_id=0; pool_id < nof_resource_pools; pool_id++) {
                /* Pool is not in use */
                if (wb_info->pool_desc[pool_id].cb != NULL) {
                    rc = _shr_wb_alloc_res_pool_restore(unit, &wb_info->pool_desc[pool_id]);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            }
            break;
            
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("wb error: version unsupported")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}

STATIC int
_bcm_dpp_wb_alloc_restore_template_state(int unit, int nof_template_pools)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_alloc_info_t *wb_info;
    int template_pool_id;

    BCMDNX_INIT_FUNC_DEFS;


    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);
    
    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\n wb: alloc_mngr - restore Template State\n")));
               
    switch(wb_info->version) {
        case BCM_DPP_WB_ALLOC_VERSION_1_0:
        case BCM_DPP_WB_ALLOC_VERSION_1_1:

            for (template_pool_id=0; template_pool_id < nof_template_pools; template_pool_id++) {
                /* Pool is not in use */
                if (wb_info->template_desc[template_pool_id].cb != NULL) {
                    rc = _shr_wb_alloc_template_pool_restore(unit, &wb_info->template_desc[template_pool_id]);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            }
            break;
            
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("wb error: version unsupported")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}

/**********************
 * Callback functions *
 **********************/
/* For pools which cannot easily call alloc_mngr functions due to multiple table access */
STATIC int
_bcm_wb_alloc_restore_pool(int unit, 
                           bcm_dpp_am_pool_info_t *pool_info, 
                           uint32 entry_id, 
                           uint32 count)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = shr_res_alloc(unit, pool_info->pool_id, SHR_RES_ALLOC_WITH_ID, count,
                       (int*)(&entry_id));
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_wb_alloc_restore_pool_eep_local(int unit, 
                                     bcm_dpp_am_pool_info_t *pool_info, 
                                     uint32 entry_id, 
                                     uint32 count)
{
    int rc = BCM_E_NONE;
    int lif_term_types = 0; /* ARAD only */

    BCMDNX_INIT_FUNC_DEFS;
    rc = bcm_dpp_am_l2_vpn_pwe_alloc(unit, lif_term_types, SHR_RES_ALLOC_WITH_ID, count,
                                     NULL, (int*)&entry_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_wb_alloc_restore_pool_mc_dynamic(int unit,  
                                      bcm_dpp_am_pool_info_t *pool_info, 
                                      uint32 entry_id, 
                                      uint32 count)
{
    int rc = BCM_E_NONE;
    SOC_TMC_MULT_ID mc_id;
    uint32 mc_id_rsvd_lo, mc_id_rsvd_hi;
    BCMDNX_INIT_FUNC_DEFS;
    mc_id = (SOC_TMC_MULT_ID)entry_id;

    rc = bcm_petra_multicast_group_reserved_range_get(unit, &mc_id_rsvd_lo, &mc_id_rsvd_hi);
    BCMDNX_IF_ERR_EXIT(rc);

    /*
     * Only allocate if outside of the reserved range.
     * The reserved range is set up during bcm init.
     * Allow this allocation to be done every time during init.
     */
    if ((mc_id < mc_id_rsvd_lo) || (mc_id >= mc_id_rsvd_hi)) {
        rc = bcm_dpp_am_mc_alloc(unit, SHR_RES_ALLOC_WITH_ID, &mc_id, FALSE);
        BCMDNX_IF_ERR_EXIT(rc);
    }
exit:
    BCMDNX_FUNC_RETURN;

}
STATIC int 
_bcm_wb_alloc_restore_pool_eep_global(int unit,  
                                      bcm_dpp_am_pool_info_t *pool_info, 
                                      uint32 entry_id, 
                                      uint32 count)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dpp_am_l3_eep_alloc(unit, _BCM_DPP_AM_L3_EEP_TYPE_DEFAULT, SHR_RES_ALLOC_WITH_ID, (int*)&entry_id);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}
STATIC int 
_bcm_wb_alloc_restore_pool_eep_ip_tnl(int unit,  
                                      bcm_dpp_am_pool_info_t *pool_info, 
                                      uint32 entry_id, 
                                      uint32 count)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dpp_am_ip_tunnel_eep_alloc(unit, _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_DEFAULT, SHR_RES_ALLOC_WITH_ID, (int*)&entry_id);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}
STATIC int 
_bcm_wb_alloc_restore_pool_eep_mpls_tunnel(int unit,  
                                           bcm_dpp_am_pool_info_t *pool_info, 
                                           uint32 entry_id, 
                                           uint32 count)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dpp_am_mpls_eep_alloc(unit, 0 /* type */, SHR_RES_ALLOC_WITH_ID, count, (int*)&entry_id);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}

STATIC int 
_bcm_wb_alloc_restore_pool_eep_trill(int unit, 
                                     bcm_dpp_am_pool_info_t *pool_info, 
                                     uint32 entry_id, 
                                     uint32 count
                                     ) 
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dpp_am_trill_eep_alloc(unit, _BCM_DPP_AM_TRILL_EEP_TYPE_ROO /* type */, SHR_RES_ALLOC_WITH_ID, (int*)&entry_id);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}

STATIC int 
_bcm_wb_alloc_restore_pool_eg_out_ac(int unit,  
                                     bcm_dpp_am_pool_info_t *pool_info, 
                                     uint32 entry_id, 
                                     uint32 count)
{
    int rc = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = bcm_dpp_am_out_ac_alloc(unit, _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT, SHR_RES_ALLOC_WITH_ID,(SOC_PPD_AC_ID*)&entry_id);
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}

static int 
_bcm_wb_alloc_restore_pool_vlan_edit_action_ingress(int unit,  
                                            bcm_dpp_am_pool_info_t *pool_info, 
                                            uint32 entry_id, 
                                            uint32 count)
{
    int rc = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = bcm_dpp_am_vlan_edit_action_id_alloc(unit, BCM_VLAN_ACTION_SET_WITH_ID | BCM_VLAN_ACTION_SET_INGRESS, (int*)&entry_id);
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int 
_bcm_wb_alloc_restore_pool_vlan_edit_action_egress(int unit,  
                                            bcm_dpp_am_pool_info_t *pool_info, 
                                            uint32 entry_id, 
                                            uint32 count)
{
    int rc = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = bcm_dpp_am_vlan_edit_action_id_alloc(unit, BCM_VLAN_ACTION_SET_WITH_ID | BCM_VLAN_ACTION_SET_EGRESS, (int*)&entry_id);
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_wb_alloc_restore_pool_meter_a(int unit,  
                                   bcm_dpp_am_pool_info_t *pool_info, 
                                   uint32 entry_id, 
                                   uint32 count)
{
    int rc = BCM_E_NONE;
    int meter_group = 0; /* meter group 0 is meter_a */
	int meter_id = entry_id;
	int meter_core = _DPP_POLICER_ID_TO_CORE(unit, entry_id);
    int act_meter_group, act_meter_core;
    
    BCMDNX_INIT_FUNC_DEFS;
   
    rc = bcm_dpp_am_meter_alloc(unit, SHR_RES_ALLOC_WITH_ID, meter_core, meter_group, (int)count, &act_meter_core, &act_meter_group, &meter_id); 
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int 
_bcm_wb_alloc_restore_pool_meter_b(int unit,  
                                   bcm_dpp_am_pool_info_t *pool_info, 
                                   uint32 entry_id, 
                                   uint32 count)
{
    int rc = BCM_E_NONE;
    int meter_group = 1; /* meter group 1 is meter_a */
    int meter_id = entry_id;
	int meter_core = _DPP_POLICER_ID_TO_CORE(unit, entry_id);
    int act_meter_group, act_meter_core;

    BCMDNX_INIT_FUNC_DEFS;
   
    rc = bcm_dpp_am_meter_alloc(unit, SHR_RES_ALLOC_WITH_ID, meter_core, meter_group, (int)count, &act_meter_core, &act_meter_group, &meter_id); 
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_wb_alloc_pool_callback_function_get(int unit, 
                                         int res_id, 
                                         shr_wb_alloc_restore_cb_t *cb)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    switch (res_id) 
    {
        case dpp_am_res_mc_dynamic:
            *cb = _bcm_wb_alloc_restore_pool_mc_dynamic;
            break;
        case dpp_am_res_eep_global:
            *cb = _bcm_wb_alloc_restore_pool_eep_global;
            break;
        case dpp_am_res_eep_ip_tnl: /* used for ip tunnels*/
            *cb = _bcm_wb_alloc_restore_pool_eep_ip_tnl;
            break;
        case dpp_am_res_eep_mpls_tunnel:
            *cb = _bcm_wb_alloc_restore_pool_eep_mpls_tunnel;
            break;
        case dpp_am_res_eep_local:
            *cb = _bcm_wb_alloc_restore_pool_eep_local;
            break;
        case dpp_am_res_eep_trill: 
            *cb = _bcm_wb_alloc_restore_pool_eep_trill; 
            break;
        case dpp_am_res_eg_out_ac:
            *cb = _bcm_wb_alloc_restore_pool_eg_out_ac;
            break;
        case dpp_am_res_meter_a:
            *cb = _bcm_wb_alloc_restore_pool_meter_a;
            break;
        case dpp_am_res_meter_b:
            *cb = _bcm_wb_alloc_restore_pool_meter_b;
            break;
        case dpp_am_res_vlan_edit_action_ingress:
            *cb = _bcm_wb_alloc_restore_pool_vlan_edit_action_ingress;
            break;
        case dpp_am_res_vlan_edit_action_egress:
            *cb = _bcm_wb_alloc_restore_pool_vlan_edit_action_egress;
            break;

        /* Unused tables */
        case dpp_am_res_eg_out_rif:
        case dpp_am_res_eg_data_erspan:
        case dpp_am_res_ipv6_tunnel:
        case dpp_am_res_eg_data_trill_invalid_entry:
        case dpp_am_res_trap_reserved_mc:
        case dpp_am_res_trap_prog:
        case dpp_am_res_pon_channel_profile:
            *cb = NULL;
            break;
       /* Resources only used in ARADPLUS. */
#ifdef BCM_88660
        case dpp_am_res_qos_egr_dscp_exp_marking:
            if (SOC_IS_ARADPLUS(unit)) {
                *cb = _bcm_wb_alloc_restore_pool;
            }
            break;
#endif /* BCM_88660 */

        /*All of these use the default restore function. */
        case dpp_am_res_qos_ing_elsp:
        case dpp_am_res_qos_ing_lif_cos:
        case dpp_am_res_qos_ing_pcp_vlan:    
        case dpp_am_res_qos_ing_cos_opcode:
        case dpp_am_res_qos_egr_remark_id:
        case dpp_am_res_qos_egr_mpls_php_id:
        case dpp_am_res_qos_egr_pcp_vlan:
        case dpp_am_res_ether_policer:
        case dpp_am_res_trap_single_instance:
        case dpp_am_res_trap_user_define:
        case dpp_am_res_trap_virtual:
        case dpp_am_res_trap_egress:
        case dpp_am_res_snoop_cmd:
        case dpp_am_res_lif_dynamic:
        case dpp_am_res_lif_pwe:
        case dpp_am_res_lif_ip_tnl:
        case dpp_am_res_fec_global:
        case dpp_am_res_failover_common_id:
        case dpp_am_res_failover_ingress_id:
        case dpp_am_res_failover_fec_id:
        case dpp_am_res_failover_egress_id:
        case dpp_am_res_glbl_src_ip:
        case dpp_am_res_glbl_ttl:
        case dpp_am_res_glbl_tos:
        case dpp_am_res_rp_id:
        case dpp_am_res_oam_ma_index:
        case dpp_am_res_oam_mep_id_short:
        case dpp_am_res_oam_mep_id_long:
        case dpp_am_res_oam_rmep_id:
        case dpp_am_res_oam_trap_code_upmep_ftmh_header:
        case dpp_am_res_trill_virtual_nick_name:
        case dpp_am_res_qos_egr_l2_i_tag:
        case dpp_am_res_local_inlif_common:
        case dpp_am_res_local_inlif_wide:
        case dpp_am_res_local_outlif:
        case dpp_am_res_global_inlif:
        case dpp_am_res_global_outlif:
        case dpp_am_res_obs_inlif:
        case dpp_am_res_obs_eg_encap:
        case dpp_am_res_vsi_vlan:
        case dpp_am_res_vsi_mstp:
        case dpp_am_res_vsq_src_port:
        case dpp_am_res_vsq_pg:
#ifdef BCM_ARAD_SUPPORT
        case dpp_am_res_ecmp_id:
#endif 
            *cb = _bcm_wb_alloc_restore_pool;
            break;
        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_wb_alloc_template_restore_pool(int unit,  
                                    bcm_dpp_am_template_info_t *template_info, 
                                    int entry_id,
                                    int ref_cnt,
                                    void *data)
{
    int rc = BCM_E_NONE;
    int is_allocated = 0;
    BCMDNX_INIT_FUNC_DEFS;

    rc = shr_template_allocate_group(unit, template_info->pool_id, SHR_TEMPLATE_MANAGE_SET_WITH_ID, 
                                     data, (int)ref_cnt, &is_allocated, (int*)&entry_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_wb_alloc_template_pool_callback_function_get(int unit, 
                                                  int template_id, 
                                                  shr_wb_alloc_template_restore_cb_t *cb)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    switch(template_id) {
    case dpp_am_template_user_defined_traps:
    case dpp_am_template_snoop_cmd:
    case dpp_am_template_mirror_action_profile:
    case dpp_am_template_vsi_egress_profile:
    case dpp_am_template_vsi_ingress_profile:
    case dpp_am_template_reserved_mc:
    case dpp_am_template_tpid_class:
    case dpp_am_template_prog_trap:
    case dpp_am_template_trap_reserved_mc:
    case dpp_am_template_lif_term_profile:
    case dpp_am_template_port_mact_sa_drop_profile:
    case dpp_am_template_port_mact_da_unknown_profile:
    case dpp_am_template_meter_profile_a_low:
    case dpp_am_template_meter_profile_a_high:
    case dpp_am_template_meter_profile_b_low:
    case dpp_am_template_meter_profile_b_high:
    case dpp_am_template_fabric_tdm_link_ptr:
    case dpp_am_template_fc_generic_pfc_mapping:
    case dpp_am_template_fc_generic_pfc_mapping_c0:
    case dpp_am_template_fc_generic_pfc_mapping_c1:
    case dpp_am_template_fc_generic_pfc_mapping_c2:
    case dpp_am_template_fc_generic_pfc_mapping_c3:
    case dpp_am_template_fc_generic_pfc_mapping_c4:
    case dpp_am_template_fc_generic_pfc_mapping_c5:
    case dpp_am_template_fc_generic_pfc_mapping_c6:
    case dpp_am_template_fc_generic_pfc_mapping_c7: 
    case dpp_am_template_l2_event_handle:    
    case dpp_am_template_l2_vsi_learn_profile:
    case dpp_am_template_l2_flooding_profile:
    case dpp_am_template_vlan_port_protocol_profile:
    case dpp_am_template_ip_tunnel_src_ip:
    case dpp_am_template_ip_tunnel_ttl:
    case dpp_am_template_ip_tunnel_tos:
    case dpp_am_template_ttl_scope_index:
    case dpp_am_template_ingress_uc_tc_mapping:
    case dpp_am_template_ingress_flow_tc_mapping:
    /* added following 3 templates that were missing  */
    case dpp_am_template_egress_interface_unicast_thresh:
    case dpp_am_template_egress_interface_multicast_thresh:
    case dpp_am_template_system_red_dp_pr:
    /* removed following 5 from cosq and moved them here because did nod work in wb_db_cosq.c module  */
    case dpp_am_template_egress_port_discount_cls_type_raw:
    case dpp_am_template_egress_port_discount_cls_type_cpu:
    case dpp_am_template_egress_port_discount_cls_type_eth:
    case dpp_am_template_egress_port_discount_cls_type_tm:
    case dpp_am_template_egress_thresh:
    case dpp_am_template_ptp_port_profile:
    case dpp_am_template_oam_icc_map:
    case dpp_am_template_oam_mep_profile_non_accelerated:
    case dpp_am_template_oam_mep_profile_accelerated:
    case dpp_am_template_bfd_mep_profile_non_accelerated:
    case dpp_am_template_bfd_mep_profile_accelerated:
    case dpp_am_template_oam_tx_priority:
    case dpp_am_template_bfd_ip_dip:
    case dpp_am_template_oam_sa_mac:
    case dpp_am_template_mpls_pwe_push_profile:
    case dpp_am_template_bfd_req_interval_pointer:
    case dpp_am_template_bfd_tos_ttl_profile:
    case dpp_am_template_bfd_src_ip_profile:
    case dpp_am_template_bfd_tx_rate_profile:
    case dpp_am_template_bfd_flags_profile:
    case dpp_am_template_oam_punt_event_hendling_profile:
    case dpp_am_template_oam_lmm_nic_tables_profile:
    case dpp_am_template_oam_lmm_oui_tables_profile:
    case dpp_am_template_oam_eth1731_profile:
    case dpp_am_template_oam_local_port_2_system_port:
    case dpp_am_template_oam_oamp_pe_gen_mem:
    case dpp_am_template_cnm_queue_profile:
    case dpp_am_template_port_tpid_class_egress_acceptable_frame_type:
    case dpp_am_template_tpid_profile:
    case dpp_am_template_vlan_edit_profile_mapping:
    case dpp_am_template_vlan_edit_profile_mapping_eg:
    case dpp_am_template_mpls_push_profile:
    case dpp_am_template_trap_egress:
    case dpp_am_template_egress_queue_mapping:
    case dpp_am_template_eedb_roo_ll_format_eth_type_index:
    case dpp_am_template_l3_rif_mac_termination_combination:
    case dpp_am_template_nrdy_threshold:
        *cb = _bcm_wb_alloc_template_restore_pool;
        break;
    case dpp_am_template_l3_vrrp:
        if (SOC_IS_ARADPLUS(unit)) {
            *cb = _bcm_wb_alloc_template_restore_pool;
        } else {
            *cb = NULL;
        }
        break;
   /* COSQ related not handled here, handled in wb_db_cosq.c */
    case dpp_am_template_queue_rate_cls:
    case dpp_am_template_vsq_pg_tc_mapping:
    case dpp_am_template_vsq_rate_cls_ct:
    case dpp_am_template_vsq_rate_cls_ctcc:
    case dpp_am_template_vsq_rate_cls_cttc:
    case dpp_am_template_vsq_rate_cls_pp:
    case dpp_am_template_vsq_rate_cls_src_port:
    case dpp_am_template_vsq_rate_cls_pg:
    case dpp_am_template_queue_discount_cls:
    case dpp_am_template_cosq_port_hr_flow_control:
    case dpp_am_template_cosq_sched_class:
        *cb = NULL;
        break; 
   default:
        *cb = NULL;
        rc = BCM_E_INTERNAL;
        break;
    }
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Layout of memory is as follows:
 * scache_ptr + 
 *    Resource pools    ************************
 *                      *      Pool 0 Data     *
 *                      ************************
 *                      *      Pool 1 Data     *
 *                      ************************
 *      ...             *          ...         *
 *                      ************************
 *    pool n offset     *      Pool n Data     *
 *                      ************************
 *    Template pools    *  template pool 0     *
 *                      ************************
 *                      *  template pool 1     *
 *                      ************************
 *      ...             *          ...         *
 *                      ************************
 *                      *  template pool n     *
 *                      ************************
 */
STATIC int
_bcm_dpp_wb_alloc_layout_init(int unit,
                              int version, 
                              bcm_dpp_am_pool_info_t *pool_info, 
                              int nof_resource_pools,
                              bcm_dpp_am_template_info_t *template_info,
                              int num_template_pools)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_alloc_info_t *wb_info; 
    int last_offset = 0;
    int pool_id;
    int template_pool_id;
    int pool_size_in_bytes;
    int template_pool_size_in_bytes;
    shr_wb_alloc_pool_desc_t *pool_desc;
    shr_wb_alloc_template_desc_t *template_desc;
    shr_wb_alloc_template_restore_cb_t template_cb_function = 0;
    shr_wb_alloc_restore_cb_t cb_function = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_ALLOC_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_ALLOC_VERSION_1_0:
        case BCM_DPP_WB_ALLOC_VERSION_1_1:

            wb_info->total_size_in_bytes = 0;

            if (version == BCM_DPP_WB_ALLOC_VERSION_1_0) {
                nof_resource_pools = nof_resource_pools + 1;
            }
            for (pool_id = 0; pool_id < nof_resource_pools; pool_id++) {
                rc = _bcm_wb_alloc_pool_callback_function_get(unit, 
                      pool_info[pool_id].res_id, /* Since there is more than one pool per resource, the resource determines the pool callback. */
                      &cb_function);
                
                if (rc == BCM_E_INTERNAL) {
                    /* Making sure each pool has a defined callback, if not return error */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG("wb alloc mngr element cb for pool_id(%d) not exist"), pool_id));
                    break;
                }
                BCMDNX_IF_ERR_EXIT(rc);
                wb_info->pool_desc[pool_id].cb = cb_function;

                rc = _shr_wb_alloc_res_pool_info_init(unit, &pool_info[pool_id], 
                                                      &wb_info->pool_desc[pool_id]);                
                BCMDNX_IF_ERR_EXIT(rc);
            }

            last_offset = 0;

            for (pool_id=0; pool_id < nof_resource_pools; pool_id++) {
                pool_desc = &wb_info->pool_desc[pool_id];
                /* 
                 * This may at a later time be set to it's own scache region.
                 * this will allow the pool region to change in size by deallocating
                 * and reallocating an scache region.  Will need to change offset calculation
                 * if and when we switch to this method.  Offset will always be 0.
                 */
                pool_desc->scache_handle = 0;
                pool_desc->scache_ptr = wb_info->scache_ptr;

                pool_desc->scache_offset = last_offset;

                rc = _shr_wb_alloc_res_pool_size_in_bytes_get(unit, pool_desc, &pool_size_in_bytes);
                BCMDNX_IF_ERR_EXIT(rc);

                wb_info->total_size_in_bytes += pool_size_in_bytes;
                last_offset += pool_size_in_bytes;
            }

            for (template_pool_id = 0; template_pool_id < num_template_pools; template_pool_id++) {
                rc = _bcm_wb_alloc_template_pool_callback_function_get(unit, template_info[template_pool_id].template_id, &template_cb_function);

                if (rc == BCM_E_INTERNAL) {
                    /* Making sure each pool has a defined callback, if not return error */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                                     (_BSL_BCM_MSG("wb alloc tempalte element cb for pool_id(%d) not exist"), pool_id));
                    break;
                }
                BCMDNX_IF_ERR_EXIT(rc);
                wb_info->template_desc[template_pool_id].cb = template_cb_function;

                rc = _shr_wb_alloc_template_pool_info_init(unit, &template_info[template_pool_id], 
                                                           &wb_info->template_desc[template_pool_id]);
                BCMDNX_IF_ERR_EXIT(rc);
            }
            for (template_pool_id=0; template_pool_id < num_template_pools; template_pool_id++) {
                
                template_desc = &wb_info->template_desc[template_pool_id];
                template_desc->scache_ptr = wb_info->scache_ptr;

                template_desc->scache_offset = last_offset;
                
                rc = _shr_wb_alloc_template_pool_size_in_bytes_get(unit, template_desc, 
                                                                   &template_pool_size_in_bytes);
                
                BCMDNX_IF_ERR_EXIT(rc);

                wb_info->total_size_in_bytes += template_pool_size_in_bytes;
                last_offset += template_pool_size_in_bytes;
            }
            break;

        default:
            /* no other version supported */
            rc = BCM_E_INTERNAL;
            break;
    }

#ifdef SHR_DEBUG_WB_ALLOC
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: init layout total size in bytes for alloc (%d)\n"),
                             wb_info->total_size_in_bytes)); 
#endif

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_alloc_info_alloc(int unit, int nof_resources_pools, int nof_template_pools)
{
    shr_wb_alloc_pool_desc_t    *pool_desc = NULL;
    shr_wb_alloc_template_desc_t *template_desc = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    /* Allocate struct */
    if (BCM_DPP_WB_ALLOC_INFO(unit) == NULL) {
        BCMDNX_ALLOC(_dpp_wb_alloc_info_p[unit], sizeof(bcm_dpp_wb_alloc_info_t), "wb alloc");
        if (BCM_DPP_WB_ALLOC_INFO(unit) == NULL) {
            goto err;
        }

        BCMDNX_ALLOC(pool_desc, nof_resources_pools * sizeof(shr_wb_alloc_pool_desc_t), "wb res pool alloc");
        if (pool_desc == NULL) {
            goto err;
        }

        BCMDNX_ALLOC(template_desc, nof_template_pools * sizeof(shr_wb_alloc_template_desc_t), "wb template pool alloc");
        if (template_desc == NULL) {
            goto err;
        } 
    } else {
        /* Backup the pointers because memset will override them. */
        pool_desc = BCM_DPP_WB_ALLOC_INFO(unit)->pool_desc;
        template_desc = BCM_DPP_WB_ALLOC_INFO(unit)->template_desc;
    }

    sal_memset(BCM_DPP_WB_ALLOC_INFO(unit), 0x00, sizeof(bcm_dpp_wb_alloc_info_t));
    sal_memset(pool_desc, 0x00, nof_resources_pools * sizeof(shr_wb_alloc_pool_desc_t));
    sal_memset(template_desc, 0x00, nof_template_pools * sizeof(shr_wb_alloc_template_desc_t));

    BCM_DPP_WB_ALLOC_INFO(unit)->pool_desc = pool_desc;
    BCM_DPP_WB_ALLOC_INFO(unit)->template_desc = template_desc;

    BCM_EXIT; /* Skip error label. */

err:
    BCM_FREE(pool_desc);
    BCM_FREE(template_desc);
    BCM_FREE(BCM_DPP_WB_ALLOC_INFO(unit));
    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("WB Alloc Mngr Allocation failed")));
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_alloc_info_dealloc(int unit)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    if (BCM_DPP_WB_ALLOC_INFO(unit) != NULL) {
       BCM_FREE(BCM_DPP_WB_ALLOC_INFO(unit)->pool_desc);
       BCM_FREE(BCM_DPP_WB_ALLOC_INFO(unit)->template_desc);
       BCM_FREE(BCM_DPP_WB_ALLOC_INFO(unit));
       BCM_DPP_WB_ALLOC_INFO(unit) = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;

}

/* 
 * Restore state from persistent storage 
 */
STATIC int
_bcm_dpp_wb_alloc_restore_state(int unit, int nof_resource_pools, int nof_template_pools)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    
    rc = _bcm_dpp_wb_alloc_restore_resource_pools_state(unit, nof_resource_pools);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_alloc_restore_template_state(unit, nof_template_pools);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*2 formats single element per allocation or multiple (1, 2, or 4 elements) */
#define SHR_WB_ALLOC_POOL_TYPE_SINGLE(pool_info) \
    (((bcm_dpp_am_pool_info_t*)pool_info)->max_elements_per_allocation == 1)

#define SHR_WB_ALLOC_POOL_TYPE_MULTI(pool_info) \
    (((bcm_dpp_am_pool_info_t*)pool_info)->max_elements_per_allocation > 1)

/* Given the element count, return the translated count for warmboot */
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_COUNT_SET(elem_count, wb_elem_count)    \
{                                                                            \
   if (elem_count > (SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY)) {   \
       BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,                                        \
       (_BSL_BCM_MSG("wb alloc mngr element count(%d) invalid"), elem_count));   \
   } else {                                                                  \
       wb_elem_count = elem_count -1;                                        \
   }                                                                         \
}

/* Given the warmboot element count, return the translated count for normal operation */
#define SHR_WB_ALLOC_POOL_TYPE_MULTI_COUNT_GET(wb_elem_count, elem_count)    \
{                                                                            \
   elem_count = wb_elem_count + 1;                                           \
                                                                             \
   if (elem_count > (SHR_WB_ALLOC_POOL_TYPE_MAX_COUNT_PER_ENTRY)) {   \
       BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,                                        \
       (_BSL_BCM_MSG("wb alloc mngr element count(%d) invalid"), elem_count));   \
   }                                                                         \
}

/*
 * Calculate the size of the pool data to be written in persistent storage.
 */
STATIC int _shr_wb_alloc_res_pool_info_init(int unit, 
                                            bcm_dpp_am_pool_info_t *pool_info, 
                                            shr_wb_alloc_pool_desc_t *pool_desc)
{
    int rc = BCM_E_NONE;
    int pool_size_in_32_bit_words;

    BCMDNX_INIT_FUNC_DEFS;

    pool_desc->pool_info = *pool_info;

    if (!BCM_DPP_WB_POOL_VALID(pool_info)) {
        /* nothing to be done for this empty pool */
        goto exit;
    }

    if (SHR_WB_ALLOC_POOL_TYPE_SINGLE(pool_info)) {
        /* 1 bit per entry */
        pool_size_in_32_bit_words =  pool_info->count/SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD;
        if (pool_info->count%SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD) {
            pool_size_in_32_bit_words++;
        }
        pool_desc->pool_size_in_bytes = pool_size_in_32_bit_words*4;
        
    } else {
        /* 1 valid bit per entry */
        pool_size_in_32_bit_words =  pool_info->count/SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD;
       if (pool_info->count%SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD) {
            pool_size_in_32_bit_words++;
        }
        pool_desc->pool_size_in_bytes = pool_size_in_32_bit_words*4;
    }

#ifdef SHR_DEBUG_WB_ALLOC
#ifdef SHR_DEBUG_POOL_ID
    if (pool_info->pool_id == SHR_DEBUG_POOL_ID) {
#endif
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: init pool(%d) pool_start(%d) num_entries(%d) size_bytes(%d)\n"),
                             pool_info->pool_id, 
                  pool_info->start,
                  pool_info->count, 
                  pool_desc->pool_size_in_bytes)); 
#ifdef SHR_DEBUG_POOL_ID
    }
#endif
#endif
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Warmboot template information */
#define BCM_DPP_WB_TEMPLATE_VALID(template_info) (((template_info) != NULL) && (template_info->count > 0))

/*
 * Calculate the size of the template data to be written in persistent storage.
 */
STATIC int _shr_wb_alloc_template_pool_info_init(int unit, 
                                                 bcm_dpp_am_template_info_t *template_info, 
                                                 shr_wb_alloc_template_desc_t *template_desc)
{
    int rc = BCM_E_NONE;
    int template_pool_size_in_bytes;

    BCMDNX_INIT_FUNC_DEFS;

    template_desc->template_info = *template_info;

    if (!BCM_DPP_WB_TEMPLATE_VALID(template_info)) {
        /* nothing to be done for this empty pool */
        goto exit;
    }

    template_pool_size_in_bytes =  sizeof(shr_wb_alloc_template_entry_header_t) * template_info->count;
    template_pool_size_in_bytes += (template_info->data_size) * template_info->count;
    /* round to next 32 bit word */
    template_pool_size_in_bytes += (template_info->data_size % 4) * template_info->count;

    template_desc->template_pool_size_in_bytes = template_pool_size_in_bytes;

#ifdef SHR_DEBUG_WB_TEMPLATE_ALLOC
#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    if (template_info->pool_id == SHR_DEBUG_TEMPLATE_POOL_ID) {
#endif
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: init template pool(%d) pool_start(%d) num_entries(%d) size_bytes(%d)\n"),
                             template_info->pool_id, 
                  template_info->start,
                  template_info->count, 
                  template_desc->template_pool_size_in_bytes)); 
#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    }
#endif
#endif

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;

}
/*
 * Update the cached pool with data allocated and return pool_entry and pool_offset
 * to be written to persistent storage.
 */
STATIC int
_shr_wb_alloc_pool_entry_alloc(int unit, 
                               shr_wb_alloc_pool_desc_t* pool_desc, 
                               int elem_count, 
                               int elem_start, 
                               shr_wb_alloc_pool_entry_t* pool_entry,
                               int *pool_offset)
{
    int rc = BCM_E_NONE;
    bcm_dpp_am_pool_info_t *pool_info;   
    int word_id, bit_id;
    int wb_elem_count;
    shr_wb_alloc_pool_entry_t *scache_ptr;
    
    BCMDNX_INIT_FUNC_DEFS;
    pool_info = &pool_desc->pool_info;

    
    /* 
     * wb allocation is ZERO based so reducing the alloc mngr start index
     */
    if(pool_info->start != 0) {
        elem_start -= pool_info->start;
    }

    if (SHR_WB_ALLOC_POOL_TYPE_SINGLE((bcm_dpp_am_pool_info_t *)(&pool_desc->pool_info))) {
        if (elem_count > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("wb alloc error updating pool, invalid elem count for pool type")));
        }
        /* 
         * Each bit represents one entry - read scache entry, update local cache and save entry and value to
         * be returned to caller to update the persistent cache with.
         */
        word_id = elem_start/SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD;
        bit_id = elem_start % SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD;
        
        scache_ptr = (shr_wb_alloc_pool_entry_t *)(pool_desc->scache_ptr + pool_desc->scache_offset + (word_id * 4));

        *scache_ptr |= SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRY_MASK << bit_id;
        *pool_entry = *scache_ptr;

    } else {
        /* 
         * Format is 1 bit indicating valid and 2 bits indicating elem count - update local cache and save
         * entry and value to be returned to caller to update the persistent cache with.
         */
        word_id = elem_start/SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD;
        bit_id = elem_start % SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD;
        scache_ptr = (shr_wb_alloc_pool_entry_t *)(pool_desc->scache_ptr + pool_desc->scache_offset + (word_id * 4));
        
        /* Read entry and clear entry field */
        *scache_ptr &= ~( SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRY_MASK << bit_id 
                          * SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_PER_ENTRY);
        *scache_ptr |= (SHR_WB_ALLOC_POOL_TYPE_MULTI_VALID_MASK << bit_id * SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_PER_ENTRY); /* valid bit */

        SHR_WB_ALLOC_POOL_TYPE_MULTI_COUNT_SET(elem_count, wb_elem_count); 
        *scache_ptr |= (( SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRY_MASK & wb_elem_count) << 
                        (bit_id * SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_PER_ENTRY));
        *pool_entry = *scache_ptr;
    }

    *pool_offset = pool_desc->scache_offset + (word_id * 4);

#ifdef SHR_DEBUG_WB_ALLOC

#ifdef SHR_DEBUG_POOL_ID
    if (pool_info->pool_id == SHR_DEBUG_POOL_ID) {
#endif
       LOG_INFO(BSL_LS_BCM_INIT,
                (BSL_META_U(unit,
                            "wb_alloc: allocate entry pool(%d) pool_start(%d) elem_start(%d) elem_count(%d) offset(%d) scache(0x%08x) value(0x%08x)\n"),
                            pool_info->pool_id,
                 pool_info->start,
                 elem_start,
                 elem_count,
                 *pool_offset,
                 (int)scache_ptr,
                 *pool_entry)); 

#ifdef SHR_DEBUG_POOL_ID
   }
#endif

#endif
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *   Function
 *      _shr_wb_alloc_template_pool_entry_scache_header_get
 *   Purpose
 *      Given the template decs, will normalize the template_id, calculate the template_wb_offset,
 *      and return a pointer to the entry's scach header. 
 *  
 *   Parameters
 *      unit                            -(IN) unit id where the template is allocated.
 *      template_desc                   -(IN) Template desc for the enry.
 *      template_id                     -(INOUT) Template id for the entry, will be normalized to the WB's template id.
 *      template_wb_offset              -(OUT) Will be filled with the WB offset for the entry.
 *      scache_header_ptr               -(OUT) Will be filled with the scache header pointer.
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int 
_shr_wb_alloc_template_pool_entry_scache_header_get(int unit,
                                                    shr_wb_alloc_template_desc_t *template_desc, 
                                                    int *template_id, 
                                                    int *template_wb_offset, 
                                                    shr_wb_alloc_template_entry_header_t **scache_header_ptr
                                                    ) {
    bcm_dpp_am_template_info_t *template_info;

    BCMDNX_INIT_FUNC_DEFS;
    template_info = &template_desc->template_info;

    /* 
     * wb allocation is ZERO based so reducing the template mngr start index
     */
    if (template_info->start != 0) {
        *template_id -= template_info->start;
    }
    /* 
     * Format is template header (template_id and ref_cnt)
     */
    *template_wb_offset = (*template_id) * sizeof(shr_wb_alloc_template_entry_header_t);
    /*
     * Add data_size offset and round up data size to nearest 32 bit word 
     */
    *template_wb_offset += ((*template_id) * template_info->data_size) + ((*template_id) * (template_info->data_size % 4));
    *template_wb_offset += template_desc->scache_offset;

    *scache_header_ptr = (shr_wb_alloc_template_entry_header_t *)(template_desc->scache_ptr + *template_wb_offset);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * 
 *   Function
 *      _shr_wb_alloc_template_pool_entry_is_alloc
 *   Purpose
 *      Given the template decs, will normalize the template_id, calculate the template_wb_offset,
 *      and return a pointer to the entry's scach header. 
 *  
 *   Parameters
 *      unit                            -(IN) unit id where the template is allocated.
 *      template_desc                   -(IN) Template desc for the enry.
 *      template_id                     -(INOUT) Template id for the entry, will be normalized to the WB's template id.
 *      template_wb_offset              -(OUT) Will be filled with the WB offset for the entry.
 *      scache_header_ptr               -(OUT) Will be filled with the scache header pointer.
 *  
 *   Results
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise
 */
STATIC int
_shr_wb_alloc_template_pool_entry_is_alloc(int unit,
                                        int template_type,
                                        int template_id)
{
    int rv = BCM_E_NONE;
    shr_wb_alloc_template_entry_header_t *scache_header_ptr;
    int template_wb_offset;
    BCMDNX_INIT_FUNC_DEFS;

    
    rv = _shr_wb_alloc_template_pool_entry_scache_header_get(unit,
                                                             &(BCM_DPP_WB_ALLOC_INFO(unit)->template_desc[template_type]),
                                                             &template_id,
                                                             &template_wb_offset,
                                                             &scache_header_ptr);
    BCMDNX_IF_ERR_EXIT(rv);

    if (scache_header_ptr->template_id != template_id
        || scache_header_ptr->ref_cnt == 0) {
        rv = BCM_E_NOT_FOUND;
    } else {
        rv = BCM_E_EXISTS;
    }

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Determine offset to be written and fill the associated data, increment reference count
 * and update template id in template_wb_data.  Return offset to be written to and data
 * size.
 */
STATIC int
_shr_wb_alloc_template_pool_entry_alloc(int unit,
                                        shr_wb_alloc_template_desc_t* template_desc,
                                        int template_id,
                                        int ref_cnt,
                                        const void *data,
                                        void *template_wb_data,
                                        int   *template_wb_offset,
                                        int   template_wb_data_size_in_bytes)
{
    int rc = BCM_E_NONE;
    bcm_dpp_am_template_info_t *template_info;   
    shr_wb_alloc_template_entry_header_t *scache_header_ptr;
    uint8 *scache_data_ptr;
    uint8 *val = NULL;
    shr_template_manage_hash_compare_extras_t *extra;
    
    BCMDNX_INIT_FUNC_DEFS;
    template_info = &template_desc->template_info;

    rc = _shr_wb_alloc_template_pool_entry_scache_header_get(unit, 
                                                             template_desc, 
                                                             &template_id, 
                                                             template_wb_offset, 
                                                             &scache_header_ptr);
    BCMDNX_IF_ERR_EXIT(rc);
        
    scache_header_ptr->template_id = template_id;

    scache_header_ptr->ref_cnt += ref_cnt;

    scache_data_ptr = (uint8*)scache_header_ptr + sizeof(shr_wb_alloc_template_entry_header_t);

    /* convert data to actual stream saved for the template in alloc_mngr
       (data is not saved in its entirety)*/
    extra = &(template_info->hashExtra);
    val = sal_alloc(template_info->data_size,"wb_db_alloc Data buffer");

    if (!val) {
        /* alloc failed */
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unable to allocate enough memory")));
    } else {
        sal_memset(val,0x0,(template_info->data_size));

        /* move to buffer */
        rc = extra->to_stream(data, val, template_info->data_size);
        if (rc != BCM_E_NONE) {
            goto exit;
        }
    }
    
    sal_memcpy(scache_data_ptr, val, template_info->data_size);
    sal_memcpy(template_wb_data, scache_header_ptr, template_wb_data_size_in_bytes);


#ifdef SHR_DEBUG_WB_TEMPLATE_ALLOC

#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    if (template_info->pool_id == SHR_DEBUG_TEMPLATE_POOL_ID) {
#endif
       LOG_INFO(BSL_LS_BCM_INIT,
                (BSL_META_U(unit,
                            "wb_alloc: allocate template entry pool(%d) pool_start(%d) template_id(%d) ref_cnt(0x%08x) startdata(0x%08x) offset(0x%08x)\n"),
                            template_info->pool_id,
                 template_info->start,
                 template_id,
                 scache_header_ptr->ref_cnt,
                 *(uint32*)(scache_data_ptr),
                 (uint32)*template_wb_offset)); 

#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
   }
#endif

#endif

    BCMDNX_IF_ERR_EXIT(rc);    
exit:
    if (val) {
        sal_free(val);
    }

    BCMDNX_FUNC_RETURN;
}

/*
 * Update the cached pool with data deallocated and return pool_entry and pool_offset to be written
 * to persistent storage.
 */
STATIC int
_shr_wb_alloc_pool_entry_dealloc(int unit, 
                                 shr_wb_alloc_pool_desc_t* pool_desc, 
                                 int elem_count, 
                                 int elem_start, 
                                 shr_wb_alloc_pool_entry_t* pool_entry,
                                 int *pool_offset)
{
    int rc = BCM_E_NONE;
    int word_id, bit_id;
    shr_wb_alloc_pool_entry_t *scache_ptr;
    
    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * wb allocation is ZERO based so reducing the alloc mngr start index
     */
    if(pool_desc->pool_info.start != 0) {
        elem_start -= pool_desc->pool_info.start;
    }


    if (SHR_WB_ALLOC_POOL_TYPE_SINGLE((bcm_dpp_am_pool_info_t *)(&pool_desc->pool_info))) {
        if (elem_count > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("wb alloc error updating pool, invalid elem count for pool type")));
        }
        /* 
         * Each bit represents one entry - deallocate
         */
        word_id = elem_start/SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD;
        bit_id = elem_start % SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD;

        scache_ptr = (shr_wb_alloc_pool_entry_t *)(pool_desc->scache_ptr + pool_desc->scache_offset + (word_id * 4));
        *scache_ptr &= ~(SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRY_MASK << bit_id);
        *pool_entry = *scache_ptr;
        
    } else {
        /* 
         * Format is 1 bit indicating valid and 2 bits indicating elem count - update entry
         */
        word_id = elem_start/SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD;
        bit_id = elem_start % SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD;        
        scache_ptr = (shr_wb_alloc_pool_entry_t *)(pool_desc->scache_ptr + pool_desc->scache_offset + (word_id * 4));
        *scache_ptr &= ~( SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRY_MASK << bit_id 
                          * SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_PER_ENTRY);
        *pool_entry = *scache_ptr;
    }
    
    *pool_offset = pool_desc->scache_offset + (word_id * 4);
    
    BCMDNX_IF_ERR_EXIT(rc);

#ifdef SHR_DEBUG_WB_ALLOC
#ifdef SHR_DEBUG_POOL_ID
    if (pool_desc->pool_info.pool_id == SHR_DEBUG_POOL_ID) {
#endif
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: free entry pool(%d) elem_start(%d) elem_count(%d) offset(%d) scache(0x%08x) value(0x%08x)\n"),
                             pool_desc->pool_info.pool_id,
                  elem_start,
                  elem_count,
                  *pool_offset,
                  (int)scache_ptr,
                  *pool_entry)); 
#ifdef SHR_DEBUG_POOL_ID
    }
#endif
#endif
    
exit:
    BCMDNX_FUNC_RETURN;
}
#define BCM_WB_ALLOC_TEMPLATE_ID_INVALID 0xffff


/*
 * Update the cached pool with data deallocated and return pool_entry and pool_offset to be written
 * to persistent storage.
 */

STATIC int
_shr_wb_alloc_template_pool_entry_dealloc(int unit, 
                                          shr_wb_alloc_template_desc_t* template_desc, 
                                          int template_id, 
                                          void *template_wb_data,
                                          int *template_wb_offset,
                                          int template_wb_data_size_in_bytes,
                                          uint8 clear_entry)
{
    int rc = BCM_E_NONE;
    bcm_dpp_am_template_info_t *template_info;
    shr_wb_alloc_template_entry_header_t *scache_header_ptr;
    uint8 *scache_data_ptr;

    BCMDNX_INIT_FUNC_DEFS;
    template_info = &template_desc->template_info;

    rc = _shr_wb_alloc_template_pool_entry_scache_header_get(unit, 
                                                             template_desc, 
                                                             &template_id, 
                                                             template_wb_offset, 
                                                             &scache_header_ptr);
    BCMDNX_IF_ERR_EXIT(rc);

    if (scache_header_ptr->template_id != template_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("template id mismatch on wb dealloc")));
    }
    if (scache_header_ptr->ref_cnt == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("template ref_cnt=0 on wb dealloc, template_id=%d"),template_id));
    } 

    if (clear_entry){
        scache_header_ptr->ref_cnt = 0;
    } else {
        scache_header_ptr->ref_cnt--;
    }

    if (scache_header_ptr->ref_cnt == 0) {
        scache_header_ptr->template_id = BCM_WB_ALLOC_TEMPLATE_ID_INVALID;              
        scache_data_ptr = (uint8*)scache_header_ptr + sizeof(shr_wb_alloc_template_entry_header_t);  
        sal_memset(scache_data_ptr, 0x00, template_info->data_size);
    }    
    
    sal_memcpy(template_wb_data, scache_header_ptr, template_wb_data_size_in_bytes);

#ifdef SHR_DEBUG_WB_TEMPLATE_ALLOC

#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    if (template_info->pool_id == SHR_DEBUG_TEMPLATE_POOL_ID) {
#endif
       LOG_INFO(BSL_LS_BCM_INIT,
                (BSL_META_U(unit,
                            "wb_alloc: deallocate template entry pool(%d) template_id(%d) ref_cnt(0x%08x) startdata(0x%08x) \n"),
                            template_info->pool_id,
                 template_id,
                 scache_header_ptr->ref_cnt,
                 *(uint32*)((uint8*)scache_header_ptr + sizeof(shr_wb_alloc_template_entry_header_t)))); 

#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
   }
#endif

#endif

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Determine the size of the external memory required for the pool.
 * This function returns the pool size in bytes but also rounds the size 
 * to the next 32 bit word boundary.
 */
STATIC int 
_shr_wb_alloc_res_pool_size_in_bytes_get(int unit, 
                                         shr_wb_alloc_pool_desc_t *pool_desc,
                                         int *pool_size_in_bytes)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    *pool_size_in_bytes = pool_desc->pool_size_in_bytes;
    
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_shr_wb_alloc_template_pool_size_in_bytes_get(int unit, 
                                              shr_wb_alloc_template_desc_t *template_desc,
                                              int *template_pool_size_in_bytes)
{
    int rc = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    *template_pool_size_in_bytes = template_desc->template_pool_size_in_bytes;
    
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}
/* 
 * Return the size of the data object associated with the template
 */
STATIC int 
_shr_wb_alloc_template_pool_entry_size_in_bytes_get(int unit, 
                                                    shr_wb_alloc_template_desc_t *template_desc,
                                                    int *template_entry_size_in_bytes)
{
    int rc = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    *template_entry_size_in_bytes = template_desc->template_info.data_size + sizeof( shr_wb_alloc_template_entry_header_t) + (int)(template_desc->template_info.data_size % 4);
    
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int 
_shr_wb_alloc_res_pool_restore_entry(int unit, 
                                     shr_wb_alloc_pool_desc_t *pool_desc, 
                                     int entry_id)
{
    int rc = BCM_E_NONE;
    bcm_dpp_am_pool_info_t *pool_info;   
    int word_id, bit_id;
    int wb_elem_count;
    int elem_start = entry_id;
    int in_use;
    shr_wb_alloc_pool_entry_t pool_entry;
    int elem_count = 0;
#ifdef SHR_DEBUG_WB_ALLOC
    int pool_offset;
#endif

    BCMDNX_INIT_FUNC_DEFS;
    pool_info = &pool_desc->pool_info;
    
    if (SHR_WB_ALLOC_POOL_TYPE_SINGLE((bcm_dpp_am_pool_info_t *)(&pool_desc->pool_info))) {
        /* 
         * Each bit represents one entry - get entry
         */
        word_id = elem_start/SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD;
        bit_id = elem_start % SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRIES_PER_WORD;

        pool_entry = *(shr_wb_alloc_pool_entry_t *)(pool_desc->scache_ptr + pool_desc->scache_offset + (word_id * 4));
        
        if (((pool_entry) & (SHR_WB_ALLOC_POOL_TYPE_SINGLE_ENTRY_MASK << bit_id)) != 0) {
            in_use = TRUE;
            elem_count = 1;
        } else {
            in_use = FALSE;
            elem_count = 0;
        }
    } else {
        /* 
         * Format is 1 bit indicating valid and 2 bits indicating elem count - get entry
         */
        word_id = elem_start/SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD;
        bit_id = elem_start % SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRIES_PER_WORD;

        pool_entry = *(shr_wb_alloc_pool_entry_t *)(pool_desc->scache_ptr + pool_desc->scache_offset + (word_id * 4));

        pool_entry &= (SHR_WB_ALLOC_POOL_TYPE_MULTI_ENTRY_MASK << bit_id 
                         * SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_PER_ENTRY);

        /* shift */
        pool_entry = pool_entry >> bit_id * SHR_WB_ALLOC_POOL_TYPE_MULTI_BITS_PER_ENTRY;

        if ((pool_entry) & SHR_WB_ALLOC_POOL_TYPE_MULTI_VALID_MASK) {
            in_use = TRUE;
            wb_elem_count = pool_entry & SHR_WB_ALLOC_POOL_TYPE_MULTI_COUNT_MASK;
            SHR_WB_ALLOC_POOL_TYPE_MULTI_COUNT_GET(wb_elem_count, elem_count); 
        } else {
            in_use = FALSE;
        }
    }
#ifdef SHR_DEBUG_WB_ALLOC
        pool_offset = pool_desc->scache_offset + (word_id * 4);    
#ifdef SHR_DEBUG_POOL_ID
    if (pool_info->pool_id == SHR_DEBUG_POOL_ID) {
#endif
        if (in_use) {
            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,
                                 "wb_alloc: restore entry pool(%d) elem_start(%d) elem_count(%d) offset(%d) scache(0x%08x) value(0x%08x)\n"),
                                 pool_info->pool_id,
                      elem_start,
                      elem_count,
                      pool_offset,
                      (uint32)pool_desc->scache_ptr,
                      (uint32)pool_entry)); 
        }
#ifdef SHR_DEBUG_POOL_ID
    }
#endif
#endif

    if (in_use == TRUE) {

        /* 
         * wb allocation is ZERO based so adding the alloc mngr start index
         */
        if(pool_info->start != 0) {
            entry_id += pool_info->start;
        }

        /* Call callback function to allocate */
      rc = ((pool_desc->cb))(unit, &pool_desc->pool_info, entry_id, elem_count);
    }
    
    BCMDNX_IF_ERR_EXIT(rc);
    
 exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_shr_wb_alloc_template_pool_restore_entry(int unit, 
                                          shr_wb_alloc_template_desc_t *template_desc, 
                                          int entry_id)
{
    uint8 *val = NULL;
    shr_template_manage_hash_compare_extras_t *extra;
    int rc = BCM_E_NONE;
    bcm_dpp_am_template_info_t *template_info;
    shr_wb_alloc_template_entry_header_t *scache_header_ptr;
    uint8 *scache_data_ptr;
    int template_wb_offset;
    int in_use;
    int template_id = entry_id;

    BCMDNX_INIT_FUNC_DEFS;
    template_info = &template_desc->template_info;
    /* 
     * Format is template header (template_id and ref_cnt)
     */
    template_wb_offset = template_id * sizeof(shr_wb_alloc_template_entry_header_t);
    /*
     * Add data_size offset and round up data size to nearest 32 bit word 
     */
    template_wb_offset += (template_id * template_info->data_size) + (template_id * (template_info->data_size % 4));

    template_wb_offset += template_desc->scache_offset;

    scache_header_ptr = (shr_wb_alloc_template_entry_header_t*)(template_desc->scache_ptr + template_wb_offset);
    
    if ((scache_header_ptr->template_id == template_id) && 
        (scache_header_ptr->ref_cnt > 0)) {

        in_use = TRUE;
    } else {
        in_use = FALSE;
    }
    scache_data_ptr = (uint8*)scache_header_ptr + sizeof(shr_wb_alloc_template_entry_header_t);

#ifdef SHR_DEBUG_WB_TEMPLATE_ALLOC
    
#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    if (template_info->pool_id == SHR_DEBUG_TEMPLATE_POOL_ID) {
#endif
        if (in_use) {
            LOG_INFO(BSL_LS_BCM_INIT,
                     (BSL_META_U(unit,
                                 "wb_alloc: restore entry pool(%d) template_id(%d) ref_cnt(%d) startdata(0x%08x) template_wb_offset(0x%08x)\n"),
                                 template_info->pool_id,
                      template_id,
                      scache_header_ptr->ref_cnt,
                      *(uint32*)(scache_data_ptr),
                      (uint32)template_wb_offset)); 
        }

#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    }
#endif
#endif

    if (in_use == TRUE) {

        /* 
         * wb allocation is ZERO based so adding the alloc mngr start index
         */
        if(template_info->start != 0) {
            entry_id += template_info->start;
        }

        /* convert data to actual stream saved for the template in alloc_mngr
           (data is not saved in its entirety)*/
        extra = &(template_info->hashExtra);
        val = sal_alloc(extra->orig_data_size ,"wb_db_alloc Data buffer");

        if (!val) {
            /* alloc failed */
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unable to allocate enough memory")));
        } else {
            rc = (extra->from_stream(val,scache_data_ptr,(template_info->data_size)));
            if (rc != BCM_E_NONE) {
                /* Problem with from stream */
                rc =  BCM_E_INTERNAL;
            }
        }

        /* Call callback function to allocate */
        rc = ((shr_wb_alloc_template_restore_cb_t) (template_desc->cb))(unit, 
                                                                        &template_desc->template_info, 
                                                                        entry_id, 
                                                                        scache_header_ptr->ref_cnt,
                                                                        val);
    }
    
    BCMDNX_IF_ERR_EXIT(rc);
    
 exit:
    if (val) {
        sal_free(val);
    }

    BCMDNX_FUNC_RETURN;
}


STATIC int 
_shr_wb_alloc_res_pool_restore(int unit, 
                               shr_wb_alloc_pool_desc_t *pool_desc)
{
    int rc = BCM_E_NONE;
    int entry_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef SHR_DEBUG_WB_ALLOC
    
#ifdef SHR_DEBUG_POOL_ID
    if (pool_desc->pool_info.pool_id == SHR_DEBUG_POOL_ID) {
#endif
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: restore pool(%d)\n"),
                             pool_desc->pool_info.pool_id));
#ifdef SHR_DEBUG_POOL_ID
    }
#endif
#endif

    /* read each entry from scache decipher and call appropropriate allocate function */
    for (entry_id = 0; entry_id < pool_desc->pool_info.count; entry_id++) {
        rc = _shr_wb_alloc_res_pool_restore_entry(unit, pool_desc, entry_id);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_shr_wb_alloc_template_pool_restore(int unit, 
                                    shr_wb_alloc_template_desc_t *template_desc)
{
    int rc = BCM_E_NONE;
    int entry_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef SHR_DEBUG_WB_TEMPLATE_ALLOC
    
#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    if (template_desc->template_info.pool_id == SHR_DEBUG_TEMPLATE_POOL_ID) {
#endif
        LOG_INFO(BSL_LS_BCM_INIT,
                 (BSL_META_U(unit,
                             "wb_alloc: restore pool(%d)\n"),
                             template_desc->template_info.pool_id));
#ifdef SHR_DEBUG_TEMPLATE_POOL_ID
    }
#endif
#endif

    /* read each entry from scache decipher and call appropropriate allocate function */
    for (entry_id = 0; entry_id < template_desc->template_info.count; entry_id++) {
        rc = _shr_wb_alloc_template_pool_restore_entry(unit, template_desc, entry_id);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
