/*
 * $Id: wb_db_counters.c,v 1.8 Broadcom SDK $
 *
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
 * Warmboot - Level 2 support (COUNTERS Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/wb_db_counters.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/sw_db.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/drv.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
bcm_dpp_wb_counters_info_t   *_dpp_wb_counters_info_p[BCM_MAX_NUM_UNITS];


#define BCM_DPP_WB_COUNTERS_INFO(unit)             (_dpp_wb_counters_info_p[unit])
#define BCM_DPP_WB_COUNTERS_INFO_INIT(unit)        ( (_dpp_wb_counters_info_p[unit] != NULL) &&          \
                                                 (_dpp_wb_counters_info_p[unit]->init_done == TRUE) )
#endif /* BCM_WARM_BOOT_SUPPORT */


#define BCM_DPP_WB_COUNTERS_SET_AND_UPDATE_ARR(_dest, _var, _var_name, arr_indx1)        \
    BCM_DPP_WB_SET_AND_UPDATE_ARR(_dest, _var, COUNTERS, counters, _var_name, arr_indx1, 0x0)

#define BCM_DPP_WB_COUNTERS_SET_AND_UPDATE_DOUBLE_ARR(_dest, _var, _var_name, arr_indx1, arr_indx2)        \
    BCM_DPP_WB_SET_AND_UPDATE_DOUBLE_ARR (_dest, _var, COUNTERS, counters, _var_name, arr_indx1, arr_indx2, 0x0)


#define BCM_DPP_WB_COUNTERS_GET(_dest, _var)        \
    BCM_DPP_WB_GET(_dest, _var, 0x0)

_bcm_dpp_wb_counter_state_t *_bcm_dpp_wb_counter_state[SOC_MAX_NUM_DEVICES];

/*
 * Global functions 
 */
int
_bcm_dpp_wb_counters_counter_state_allocate(int unit, uint32 nof_procs)
{    
    uint32 allocSize;
    BCMDNX_INIT_FUNC_DEFS;

    allocSize = sizeof(_bcm_dpp_wb_counter_state_t) + nof_procs*sizeof(_bcm_dpp_wb_counter_proc_info_t);
    BCMDNX_ALLOC(_bcm_dpp_wb_counter_state[unit], sizeof(_bcm_dpp_wb_counter_state_t), "DPP wb counter state");
    if (_bcm_dpp_wb_counter_state[unit]) {
        sal_memset(_bcm_dpp_wb_counter_state[unit],
                   0x00,
                   sizeof(_bcm_dpp_wb_counter_state_t));        
    } else { 
        return BCM_E_MEMORY;
    } 
    BCMDNX_ALLOC(_bcm_dpp_wb_counter_state[unit]->proc, allocSize, "DPP wb counter state procs");
    if (_bcm_dpp_wb_counter_state[unit]->proc) {
        sal_memset(_bcm_dpp_wb_counter_state[unit]->proc,
                   0x00,
                   allocSize);        
    } else { 
        return BCM_E_MEMORY;
    } 
       
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_counters_proc_free(int unit)
{
    if (_bcm_dpp_wb_counter_state[unit]) {
        BCM_FREE(_bcm_dpp_wb_counter_state[unit]->proc);   
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_wb_counters_dealloc(int unit)
{
    _bcm_dpp_wb_counters_proc_free(unit);    
    BCM_FREE(_bcm_dpp_wb_counter_state[unit]);
    return BCM_E_NONE;
}

int
_bcm_dpp_wb_counters_proc_allocated_set(int unit, unsigned int allocated, uint32 proc_indx)
{
    BCM_DPP_WB_COUNTERS_SET_AND_UPDATE_ARR(_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].allocated, allocated, allocated, proc_indx);
}

int
_bcm_dpp_wb_counters_proc_allocated_get(int unit, unsigned int *allocated, uint32 proc_indx)
{
    BCM_DPP_WB_COUNTERS_GET(_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].allocated, allocated);    
}

int
_bcm_dpp_wb_counters_proc_inUse_is_allocated(int unit, uint8 *is_alloc, uint32 proc_indx)
{
    *is_alloc = !(_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse == NULL);
    return BCM_E_NONE;
}

int
_bcm_dpp_wb_counters_proc_inUse_allocate(int unit, long unsigned int allocSize, uint32 proc_indx)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse, allocSize, "alloc bitmap");
    if (_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse) {
        sal_memset(_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse,
                   0x00,
                   allocSize);
        if (0 == proc_indx) {
            /* zero in counter pipeline from PP == nop/unset */
            _bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse[0] |= 1;
        }
    } else { 
        return BCM_E_MEMORY;
    } 
       
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_counters_proc_inUse_free(int unit, uint32 proc_indx)
{
    BCM_FREE(_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse);    
    return BCM_E_NONE;
}

int
_bcm_dpp_wb_counters_proc_inUse_set(int unit, uint32 inUse, uint32 proc_indx, uint32 inUse_indx)
{
    BCM_DPP_WB_COUNTERS_SET_AND_UPDATE_DOUBLE_ARR(
        _bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse[inUse_indx], inUse, inUse, proc_indx, inUse_indx);
}

int
_bcm_dpp_wb_counters_proc_inUse_get(int unit, uint32 *inUse, uint32 proc_indx, uint32 inUse_indx)
{
    BCM_DPP_WB_COUNTERS_GET(_bcm_dpp_wb_counter_state[unit]->proc[proc_indx].inUse[inUse_indx], inUse);    
}



#if defined(BCM_WARM_BOOT_SUPPORT)


#define BCM_DPP_WB_COUNTERS_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_COUNTERS_CURRENT_VERSION        BCM_DPP_WB_COUNTERS_VERSION_1_0


/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */
#define BCM_DPP_WB_COUNTERS_BG_COLLECTION_ENABLE_NUM        \
    (1)
#define BCM_DPP_WB_COUNTERS_BGWAIT_NUM        \
    (1)

#define BCM_DPP_WB_COUNTERS_BACKGROUND_DISABLE_NUM        \
    (1)

#define BCM_DPP_WB_COUNTERS_FIFO_READ_BACKGROUND_NUM        \
    (1)

#define BCM_DPP_WB_COUNTERS_FIFO_READ_DEFERRED_NUM        \
    (1)

#define BCM_DPP_WB_COUNTERS_FIFO_READ_PASSES_NUM(unit)  \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_FIFO_READ_FAILS_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_FIFO_READ_ITEMS_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_FIFO_READ_MAX_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_FIFO_READ_LAST_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_DIRECT_READ_PASSES_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_DIRECT_READ_FAILS_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_CACHE_UPDATES_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_CACHE_READS_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_CACHE_WRITES_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_API_READS_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_API_WRITES_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_API_MISS_READS_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_API_MISS_WRITES_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_ALLOCATED_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_MODE_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#define BCM_DPP_WB_COUNTERS_VALID_NUM(unit)        \
    SOC_DPP_DEFS_GET(unit, nof_counter_processors)

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
/* double dimention arrays are saved in scache as spread single dimention array */
#define BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_COUNTER(unit) \
     \
    (SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * _SUB_COUNT)
#define BCM_DPP_WB_COUNTERS_COUNTER_NUM(unit)        \
    (SOC_DPP_DEFS_GET(unit, nof_counter_processors) * BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_COUNTER(unit))
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

/* double dimention arrays are saved in scache as spread single dimention array */
#define BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_INUSE(proc_idx)                     \
     \
    ((wb_info->counters_config->counter_state_ptr->proc[proc_idx].num_sets + 7) / 8)
#define BCM_DPP_WB_COUNTERS_INUSE_NUM(unit)        \
    (_bcm_dpp_wb_compute_in_use_num(unit))



typedef struct bcm_dpp_wb_counters_background_collection_enable_config_s {
    uint32     background_collection_enable;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_background_collection_enable_config_t;
                        
typedef struct bcm_dpp_wb_counters_bgWait_config_s {
    int32     bgWait;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_bgWait_config_t;


typedef struct bcm_dpp_wb_counters_background_disable_config_s {
    uint8     background_disable;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_background_disable_config_t;


typedef struct bcm_dpp_wb_counters_fifo_read_background_config_s {
    uint32     fifo_read_background;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_fifo_read_background_config_t;


typedef struct bcm_dpp_wb_counters_fifo_read_deferred_config_s {
    uint32     fifo_read_deferred;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_fifo_read_deferred_config_t;


typedef struct bcm_dpp_wb_counters_fifo_read_passes_config_s {
    uint32     fifo_read_passes;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_fifo_read_passes_config_t;


typedef struct bcm_dpp_wb_counters_fifo_read_fails_config_s {
    uint32     fifo_read_fails;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_fifo_read_fails_config_t;


typedef struct bcm_dpp_wb_counters_fifo_read_items_config_s {
    uint32     fifo_read_items;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_fifo_read_items_config_t;


typedef struct bcm_dpp_wb_counters_fifo_read_max_config_s {
    uint32     fifo_read_max;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_fifo_read_max_config_t;


typedef struct bcm_dpp_wb_counters_fifo_read_last_config_s {
    uint32     fifo_read_last;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_fifo_read_last_config_t;


typedef struct bcm_dpp_wb_counters_direct_read_passes_config_s {
    uint32     direct_read_passes;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_direct_read_passes_config_t;


typedef struct bcm_dpp_wb_counters_direct_read_fails_config_s {
    uint32     direct_read_fails;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_direct_read_fails_config_t;


typedef struct bcm_dpp_wb_counters_cache_updates_config_s {
    uint32     cache_updates;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_cache_updates_config_t;


typedef struct bcm_dpp_wb_counters_cache_reads_config_s {
    uint32     cache_reads;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_cache_reads_config_t;


typedef struct bcm_dpp_wb_counters_cache_writes_config_s {
    uint32     cache_writes;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_cache_writes_config_t;


typedef struct bcm_dpp_wb_counters_api_reads_config_s {
    uint32     api_reads;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_api_reads_config_t;


typedef struct bcm_dpp_wb_counters_api_writes_config_s {
    uint32     api_writes;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_api_writes_config_t;


typedef struct bcm_dpp_wb_counters_api_miss_reads_config_s {
    uint32     api_miss_reads;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_api_miss_reads_config_t;


typedef struct bcm_dpp_wb_counters_api_miss_writes_config_s {
    uint32     api_miss_writes;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_api_miss_writes_config_t;


typedef struct bcm_dpp_wb_counters_allocated_config_s {
    uint32     allocated;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_allocated_config_t;

typedef struct bcm_dpp_wb_counters_mode_config_s {
    SOC_TMC_CNT_COUNTERS_INFO mode;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_mode_config_t;

typedef struct bcm_dpp_wb_counters_valid_config_s {
    uint8 valid;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_valid_config_t;

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
typedef struct bcm_dpp_wb_counters_counter_config_s {
    uint64     counter;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_counter_config_t;
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */


typedef struct bcm_dpp_wb_counters_inUse_config_s {
    uint8     inUse;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_counters_inUse_config_t;


#define BCM_DPP_WB_DEV_COUNTERS_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        BCM_DPP_WB_COUNTERS_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_COUNTERS_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        BCM_DPP_WB_COUNTERS_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_COUNTERS_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_COUNTERS_INFO(unit)->is_dirty == 1)





#define BCM_DPP_WB_COUNTERS_UPDATE_SINGLE_STATE(_var, _sw_db_var)      \
    BCM_DPP_WB_UPDATE_SINGLE_STATE(COUNTERS, counters, _var, _sw_db_var)
 
#define BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(_var, _sw_db_var, _indx1)     \
    BCM_DPP_WB_UPDATE_ARR_STATE(COUNTERS, counters, _var, _sw_db_var, _indx1)

#define BCM_DPP_WB_COUNTERS_UPDATE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size, _indx1, _indx2) \
    BCM_DPP_WB_UPDATE_DBL_ARR_STATE(COUNTERS, counters, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2)
 


#define BCM_DPP_WB_COUNTERS_RESTORE_SINGLE_STATE(_var, _sw_db_var)     \
    BCM_DPP_WB_RESTORE_SINGLE_STATE(COUNTERS, counters, _var, _sw_db_var)

#define BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(_var, _sw_db_var)        \
    BCM_DPP_WB_RESTORE_ARR_STATE(COUNTERS, counters, _var, _sw_db_var)

#define BCM_DPP_WB_COUNTERS_RESTORE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size) \
    BCM_DPP_WB_RESTORE_DBL_ARR_STATE(COUNTERS, counters, _var, _sw_db_var, _arr_iner_size)




#define BCM_DPP_WB_COUNTERS_SAVE_SINGLE_STATE(_var, _sw_db_var)        \
    BCM_DPP_WB_SAVE_SINGLE_STATE(COUNTERS, counters, _var, _sw_db_var)

#define BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(_var, _sw_db_var)       \
    BCM_DPP_WB_SAVE_ARR_STATE(COUNTERS, counters, _var, _sw_db_var)

#define BCM_DPP_WB_COUNTERS_SAVE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size) \
    BCM_DPP_WB_SAVE_DBL_ARR_STATE(COUNTERS, counters, _var, _sw_db_var, _arr_iner_size)




#define BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(_var, _var_num)        \
    BCM_DPP_WB_DB_LAYOUT_INIT_VARIABLE(counters, _var, _var_num)

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_compute_in_use_num(int unit)
{
    int i;
    int ret = 0;
    bcm_dpp_wb_counters_info_t  *wb_info;

    wb_info = BCM_DPP_WB_COUNTERS_INFO(unit);

    for (i=0;i<SOC_DPP_DEFS_GET(unit, nof_counter_processors);i++) {
        ret = ret + BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_INUSE(i);
    }

    return ret;
}

STATIC int
_bcm_dpp_wb_counters_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_counters_info_t  *wb_info; 
    int                      entry_size, total_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COUNTERS_INFO(unit);

    wb_info->version = version;

    switch (version) {

        case BCM_DPP_WB_COUNTERS_VERSION_1_0:

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(background_collection_enable, 
                                                        BCM_DPP_WB_COUNTERS_BG_COLLECTION_ENABLE_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(bgWait, 
                                                        BCM_DPP_WB_COUNTERS_BGWAIT_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(background_disable, 
                                                        BCM_DPP_WB_COUNTERS_BACKGROUND_DISABLE_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(fifo_read_background, 
                                                        BCM_DPP_WB_COUNTERS_FIFO_READ_BACKGROUND_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(fifo_read_deferred, 
                                                        BCM_DPP_WB_COUNTERS_FIFO_READ_DEFERRED_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(fifo_read_passes, 
                                                        BCM_DPP_WB_COUNTERS_FIFO_READ_PASSES_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(fifo_read_fails, 
                                                        BCM_DPP_WB_COUNTERS_FIFO_READ_FAILS_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(fifo_read_items, 
                                                        BCM_DPP_WB_COUNTERS_FIFO_READ_ITEMS_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(fifo_read_max, 
                                                        BCM_DPP_WB_COUNTERS_FIFO_READ_MAX_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(fifo_read_last, 
                                                        BCM_DPP_WB_COUNTERS_FIFO_READ_LAST_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(direct_read_passes, 
                                                        BCM_DPP_WB_COUNTERS_DIRECT_READ_PASSES_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(direct_read_fails, 
                                                        BCM_DPP_WB_COUNTERS_DIRECT_READ_FAILS_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(cache_updates, 
                                                        BCM_DPP_WB_COUNTERS_CACHE_UPDATES_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(cache_reads, 
                                                        BCM_DPP_WB_COUNTERS_CACHE_READS_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(cache_writes, 
                                                        BCM_DPP_WB_COUNTERS_CACHE_WRITES_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(api_reads, 
                                                        BCM_DPP_WB_COUNTERS_API_READS_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(api_writes, 
                                                        BCM_DPP_WB_COUNTERS_API_WRITES_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(api_miss_reads, 
                                                        BCM_DPP_WB_COUNTERS_API_MISS_READS_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(api_miss_writes, 
                                                        BCM_DPP_WB_COUNTERS_API_MISS_WRITES_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(allocated, 
                                                        BCM_DPP_WB_COUNTERS_ALLOCATED_NUM(unit)); 

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(mode,
                                                        BCM_DPP_WB_COUNTERS_MODE_NUM(unit));
    
            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(valid,
                                                        BCM_DPP_WB_COUNTERS_VALID_NUM(unit));

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(counter, 
                                                        BCM_DPP_WB_COUNTERS_COUNTER_NUM(unit)); 
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

            BCM_DPP_WB_DB_LAYOUT_INIT_COUNTERS_VARIABLE(inUse, 
                                                        BCM_DPP_WB_COUNTERS_INUSE_NUM(unit)); 


            wb_info->size = total_size;
            
            break;

        default:
            /* no other version supported */
            rc = BCM_E_INTERNAL;
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_counters_restore_background_collection_enable_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_SINGLE_STATE(background_collection_enable, 
                                             counter_state_ptr->running);
}

STATIC int
_bcm_dpp_wb_counters_restore_bgWait_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_SINGLE_STATE(bgWait, 
                                             counter_state_ptr->bgWait);
}

STATIC int
_bcm_dpp_wb_counters_restore_background_disable_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_SINGLE_STATE(background_disable, 
                                             counter_state_ptr->background_disable);
}

STATIC int
_bcm_dpp_wb_counters_restore_fifo_read_background_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_SINGLE_STATE(fifo_read_background, 
                                             counter_state_ptr->fifo_read_background);
}

STATIC int
_bcm_dpp_wb_counters_restore_fifo_read_deferred_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_SINGLE_STATE(fifo_read_deferred, 
                                             counter_state_ptr->fifo_read_deferred);
}

STATIC int
_bcm_dpp_wb_counters_restore_fifo_read_passes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(fifo_read_passes, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_passes);
}

STATIC int
_bcm_dpp_wb_counters_restore_fifo_read_fails_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(fifo_read_fails, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_fails);
}

STATIC int
_bcm_dpp_wb_counters_restore_fifo_read_items_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(fifo_read_items, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_items);
}

STATIC int
_bcm_dpp_wb_counters_restore_fifo_read_max_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(fifo_read_max, 
                                          counter_state_ptr->proc[arr_indx].fifo_read_max);
}

STATIC int
_bcm_dpp_wb_counters_restore_fifo_read_last_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(fifo_read_last, 
                                          counter_state_ptr->proc[arr_indx].fifo_read_last);
}

STATIC int
_bcm_dpp_wb_counters_restore_direct_read_passes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(direct_read_passes, 
                                             counter_state_ptr->proc[arr_indx].direct_read_passes);
}

STATIC int
_bcm_dpp_wb_counters_restore_direct_read_fails_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(direct_read_fails, 
                                             counter_state_ptr->proc[arr_indx].direct_read_fails);
}

STATIC int
_bcm_dpp_wb_counters_restore_cache_updates_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(cache_updates, 
                                             counter_state_ptr->proc[arr_indx].cache_updates);
}

STATIC int
_bcm_dpp_wb_counters_restore_cache_reads_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(cache_reads, 
                                             counter_state_ptr->proc[arr_indx].cache_reads);
}

STATIC int
_bcm_dpp_wb_counters_restore_cache_writes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(cache_writes, 
                                             counter_state_ptr->proc[arr_indx].cache_writes);
}

STATIC int
_bcm_dpp_wb_counters_restore_api_reads_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(api_reads, 
                                             counter_state_ptr->proc[arr_indx].api_reads);
}

STATIC int
_bcm_dpp_wb_counters_restore_api_writes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(api_writes, 
                                             counter_state_ptr->proc[arr_indx].api_writes);
}

STATIC int
_bcm_dpp_wb_counters_restore_api_miss_reads_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(api_miss_reads, 
                                             counter_state_ptr->proc[arr_indx].api_miss_reads);
}

STATIC int
_bcm_dpp_wb_counters_restore_api_miss_writes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(api_miss_writes, 
                                          counter_state_ptr->proc[arr_indx].api_miss_writes);
}

STATIC int
_bcm_dpp_wb_counters_restore_allocated_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(allocated, 
                                             proc[arr_indx].allocated);
}
STATIC int
_bcm_dpp_wb_counters_restore_mode_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(mode, 
                                          counter_state_ptr->proc[arr_indx].mode);
}
STATIC int
_bcm_dpp_wb_counters_restore_valid_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_ARR_STATE(valid, 
                                          counter_state_ptr->proc[arr_indx].valid);
}

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
STATIC int
_bcm_dpp_wb_counters_restore_counter_state(int unit)
{
    BCM_DPP_WB_COUNTERS_RESTORE_DBL_ARR_STATE(counter, 
                                              counter_state_ptr->proc[arr_indx1].counter[arr_indx2],
                                              BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_COUNTER(unit));
}
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

STATIC int
_bcm_dpp_wb_counters_restore_inUse_state(int unit)
{
    int                                   arr_indx = 0;
    int                                   rc = BCM_E_NONE;
    bcm_dpp_wb_counters_info_t            *wb_info;
    bcm_dpp_wb_counters_inUse_config_t    *inUse_state;
    counters_cntl_t                       *counters_config;
    int                                   proc_idx = 0;
    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COUNTERS_INFO(unit);
    counters_config = wb_info->counters_config;

    switch(wb_info->version) {

        case BCM_DPP_WB_COUNTERS_VERSION_1_0:

        inUse_state = (bcm_dpp_wb_counters_inUse_config_t *)(wb_info->scache_ptr + wb_info->inUse_off);

        for (proc_idx = 0; proc_idx < SOC_DPP_DEFS_GET(unit, nof_counter_processors); proc_idx++) {
            for (arr_indx = 0; arr_indx < BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_INUSE(proc_idx); arr_indx++, inUse_state++) {
                if(NULL != counters_config->proc[proc_idx].inUse) {
                    counters_config->proc[proc_idx].inUse[arr_indx] = inUse_state->inUse;
                }
            }
        }
        break;

        default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown scache data version")));
        break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_bcm_dpp_wb_counters_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_counters_restore_background_collection_enable_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_bgWait_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_background_disable_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_fifo_read_background_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_fifo_read_deferred_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_fifo_read_passes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_fifo_read_fails_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_fifo_read_items_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_fifo_read_max_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_fifo_read_last_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_direct_read_passes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_direct_read_fails_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_cache_updates_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_cache_reads_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_cache_writes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_api_reads_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_api_writes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_api_miss_reads_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_api_miss_writes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_allocated_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_mode_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_restore_valid_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
    rc = _bcm_dpp_wb_counters_restore_counter_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

    rc = _bcm_dpp_wb_counters_restore_inUse_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_counters_save_bg_collection_enable_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_SINGLE_STATE(background_collection_enable, 
                                            counter_state_ptr->running);
}

STATIC int
_bcm_dpp_wb_counters_save_bgWait_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_SINGLE_STATE(bgWait, 
                                          counter_state_ptr->bgWait);
}

STATIC int
_bcm_dpp_wb_counters_save_background_disable_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_SINGLE_STATE(background_disable, 
                                             counter_state_ptr->background_disable);
}

STATIC int
_bcm_dpp_wb_counters_save_fifo_read_background_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_SINGLE_STATE(fifo_read_background, 
                                             counter_state_ptr->fifo_read_background);
}

STATIC int
_bcm_dpp_wb_counters_save_fifo_read_deferred_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_SINGLE_STATE(fifo_read_deferred, 
                                             counter_state_ptr->fifo_read_deferred);
}

STATIC int
_bcm_dpp_wb_counters_save_fifo_read_passes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(fifo_read_passes, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_passes);
}

STATIC int
_bcm_dpp_wb_counters_save_fifo_read_fails_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(fifo_read_fails, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_fails);
}

STATIC int
_bcm_dpp_wb_counters_save_fifo_read_items_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(fifo_read_items, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_items);
}

STATIC int
_bcm_dpp_wb_counters_save_fifo_read_max_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(fifo_read_max, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_max);
}

STATIC int
_bcm_dpp_wb_counters_save_fifo_read_last_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(fifo_read_last, 
                                             counter_state_ptr->proc[arr_indx].fifo_read_last);
}

STATIC int
_bcm_dpp_wb_counters_save_direct_read_passes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(direct_read_passes, 
                                             counter_state_ptr->proc[arr_indx].direct_read_passes);
}

STATIC int
_bcm_dpp_wb_counters_save_direct_read_fails_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(direct_read_fails, 
                                             counter_state_ptr->proc[arr_indx].direct_read_fails);
}

STATIC int
_bcm_dpp_wb_counters_save_cache_updates_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(cache_updates, 
                                             counter_state_ptr->proc[arr_indx].cache_updates);
}

STATIC int
_bcm_dpp_wb_counters_save_cache_reads_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(cache_reads, 
                                             counter_state_ptr->proc[arr_indx].cache_reads);
}

STATIC int
_bcm_dpp_wb_counters_save_cache_writes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(cache_writes, 
                                             counter_state_ptr->proc[arr_indx].cache_writes);
}

STATIC int
_bcm_dpp_wb_counters_save_api_reads_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(api_reads, 
                                             counter_state_ptr->proc[arr_indx].api_reads);
}

STATIC int
_bcm_dpp_wb_counters_save_api_writes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(api_writes, 
                                             counter_state_ptr->proc[arr_indx].api_writes);
}

STATIC int
_bcm_dpp_wb_counters_save_api_miss_reads_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(api_miss_reads, 
                                             counter_state_ptr->proc[arr_indx].api_miss_reads);
}

STATIC int
_bcm_dpp_wb_counters_save_api_miss_writes_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(api_miss_writes, 
                                          counter_state_ptr->proc[arr_indx].api_miss_writes);
}

STATIC int
_bcm_dpp_wb_counters_save_allocated_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(allocated, 
                                       proc[arr_indx].allocated);
}
STATIC int
_bcm_dpp_wb_counters_save_mode_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(mode, 
                                      counter_state_ptr->proc[arr_indx].mode);
}

STATIC int
_bcm_dpp_wb_counters_save_valid_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_ARR_STATE(valid, 
                                      counter_state_ptr->proc[arr_indx].valid);
}

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
STATIC int
_bcm_dpp_wb_counters_save_counter_state(int unit)
{
    BCM_DPP_WB_COUNTERS_SAVE_DBL_ARR_STATE(counter, 
                                              counter_state_ptr->proc[arr_indx1].counter[arr_indx2],
                                              BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_COUNTER(unit));
}
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

STATIC int
_bcm_dpp_wb_counters_save_inUse_state(int unit)
{
    int                                   arr_indx;
    int                                   proc_idx = 0;
    int                                   rc = BCM_E_NONE;
    bcm_dpp_wb_counters_info_t            *wb_info;
    bcm_dpp_wb_counters_inUse_config_t    *inUse_state;
    counters_cntl_t                       *counters_config;
    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COUNTERS_INFO(unit);
    counters_config = wb_info->counters_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COUNTERS_VERSION_1_0:

            inUse_state = (bcm_dpp_wb_counters_inUse_config_t *)(wb_info->scache_ptr + wb_info->inUse_off);
            for (proc_idx = 0; proc_idx < SOC_DPP_DEFS_GET(unit, nof_counter_processors); proc_idx++) {
                for (arr_indx = 0; arr_indx < BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_INUSE(proc_idx); arr_indx++, inUse_state++) {
                    if(NULL != counters_config->proc[proc_idx].inUse) {
                        inUse_state->inUse = counters_config->proc[proc_idx].inUse[arr_indx];
                    }
                }
            }
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown scache data version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_counters_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (_dpp_wb_counters_info_p[unit] == NULL) {
        _dpp_wb_counters_info_p[unit] = sal_alloc(sizeof(bcm_dpp_wb_counters_info_t), "wb counters");
        if (_dpp_wb_counters_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Out of memory")));
        }
    }

    sal_memset(_dpp_wb_counters_info_p[unit], 0x00, sizeof(bcm_dpp_wb_counters_info_t));

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_counters_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (_dpp_wb_counters_info_p[unit] != NULL) {
        sal_free(_dpp_wb_counters_info_p[unit]);
        _dpp_wb_counters_info_p[unit] = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Global functions
 */

int
_bcm_dpp_wb_counters_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_counters_info_t  *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_COUNTERS_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if (!BCM_DPP_WB_COUNTERS_IS_DIRTY(unit) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }
    rc = _bcm_dpp_wb_counters_save_bg_collection_enable_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_bgWait_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_background_disable_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_fifo_read_background_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_fifo_read_deferred_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_fifo_read_passes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_fifo_read_fails_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_fifo_read_items_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_fifo_read_max_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_fifo_read_last_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_direct_read_passes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_direct_read_fails_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_cache_updates_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_cache_reads_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_cache_writes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_api_reads_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_api_writes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_api_miss_reads_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_api_miss_writes_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_allocated_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_mode_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_counters_save_valid_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
    rc = _bcm_dpp_wb_counters_save_counter_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

    rc = _bcm_dpp_wb_counters_save_inUse_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_COUNTERS_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_counters_state_init(int unit, _bcm_dpp_counter_state_t *counter_state)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_COUNTERS_CURRENT_VERSION, recovered_ver;
    uint8                    *scache_ptr;
    bcm_dpp_wb_counters_info_t  *wb_info; 

    BCMDNX_INIT_FUNC_DEFS;

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COUNTERS, 0);

    rc = _bcm_dpp_wb_counters_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_COUNTERS_INFO(unit);
    wb_info->counters_config = _bcm_dpp_wb_counter_state[unit];
    wb_info->counters_config->counter_state_ptr = counter_state;

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetrieve, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_counters_layout_init(unit, recovered_ver);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_counters_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_counters_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_counters_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_counters_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* state should have been removed by Host. Stale State */
        rc = _bcm_dpp_wb_counters_sync(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* writing to persistent storage initiated by bcm_petra_init() */
        BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
    }

    wb_info->init_done = TRUE;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_counters_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_counters_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_counters_update_background_collection_enable_state(int unit)
{
    BCM_DPP_WB_COUNTERS_UPDATE_SINGLE_STATE(background_collection_enable, 
                                            counter_state_ptr->running);
}


int
_bcm_dpp_wb_counters_update_bgWait_state(int unit)
{
    BCM_DPP_WB_COUNTERS_UPDATE_SINGLE_STATE(bgWait, 
                                            counter_state_ptr->bgWait);
}

int
_bcm_dpp_wb_counters_update_background_disable_state(int unit)
{
    BCM_DPP_WB_COUNTERS_UPDATE_SINGLE_STATE(background_disable, 
                                            counter_state_ptr->background_disable);
}

int
_bcm_dpp_wb_counters_update_fifo_read_background_state(int unit)
{
    BCM_DPP_WB_COUNTERS_UPDATE_SINGLE_STATE(fifo_read_background, 
                                            counter_state_ptr->fifo_read_background);
}

int
_bcm_dpp_wb_counters_update_fifo_read_deferred_state(int unit)
{
    BCM_DPP_WB_COUNTERS_UPDATE_SINGLE_STATE(fifo_read_deferred, 
                                            counter_state_ptr->fifo_read_deferred);
}

int
_bcm_dpp_wb_counters_update_fifo_read_passes_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(fifo_read_passes, 
                                         counter_state_ptr->proc[arr_indx].fifo_read_passes, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_fifo_read_fails_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(fifo_read_fails, 
                                         counter_state_ptr->proc[arr_indx].fifo_read_fails, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_fifo_read_items_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(fifo_read_items, 
                                         counter_state_ptr->proc[arr_indx].fifo_read_items, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_fifo_read_max_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(fifo_read_max, 
                                         counter_state_ptr->proc[arr_indx].fifo_read_max, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_fifo_read_last_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(fifo_read_last, 
                                         counter_state_ptr->proc[arr_indx].fifo_read_last, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_direct_read_passes_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(direct_read_passes, 
                                         counter_state_ptr->proc[arr_indx].direct_read_passes, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_direct_read_fails_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(direct_read_fails, 
                                         counter_state_ptr->proc[arr_indx].direct_read_fails, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_cache_updates_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(cache_updates, 
                                         counter_state_ptr->proc[arr_indx].cache_updates, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_cache_reads_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(cache_reads, 
                                         counter_state_ptr->proc[arr_indx].cache_reads, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_cache_writes_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(cache_writes, 
                                         counter_state_ptr->proc[arr_indx].cache_writes, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_api_reads_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(api_reads, 
                                         counter_state_ptr->proc[arr_indx].api_reads, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_api_writes_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(api_writes, 
                                         counter_state_ptr->proc[arr_indx].api_writes, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_api_miss_reads_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(api_miss_reads, 
                                         counter_state_ptr->proc[arr_indx].api_miss_reads, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_api_miss_writes_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(api_miss_writes, 
                                         counter_state_ptr->proc[arr_indx].api_miss_writes, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_allocated_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(allocated, 
                                         proc[arr_indx].allocated, 
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_mode_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(mode, 
                                         counter_state_ptr->proc[arr_indx].mode,
                                         arr_indx);
}

int
_bcm_dpp_wb_counters_update_valid_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_COUNTERS_UPDATE_ARR_STATE(valid, 
                                         counter_state_ptr->proc[arr_indx].valid,
                                         arr_indx);
}

#if defined(BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE)
int
_bcm_dpp_wb_counters_update_counter_state(int unit, uint32 arr_indx1, uint32 arr_indx2)
{
    BCM_DPP_WB_COUNTERS_UPDATE_DBL_ARR_STATE(counter, 
                                             counter_state_ptr->proc[arr_indx1].counter[arr_indx2], 
                                             BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_COUNTER(unit), 
                                             arr_indx1, 
                                             arr_indx2);
}
#endif /* BCM_DPP_WARM_BOOT_SAVE_COUNTERS_CACHE */

int
_bcm_dpp_wb_counters_update_inUse_state(int unit, uint32 arr_indx1, uint32 arr_indx2)
{
    int                                  rc = BCM_E_NONE;
    int                                  proc_idx;
    bcm_dpp_wb_counters_info_t           *wb_info;
    bcm_dpp_wb_counters_inUse_config_t   *inUse_state;
    counters_cntl_t                      *counters_config;
    uint32                               data_size;
    uint8                                *data;
    int                                  offset;
    soc_scache_handle_t                  wb_handle;
    uint32                               sd_arr_indx;  /* single dimention array index */

    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_COUNTERS_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated with updating the */
        /* persistent storage                                             */
        BCM_DPP_WB_DEV_COUNTERS_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_COUNTERS, 0);

    wb_info = BCM_DPP_WB_COUNTERS_INFO(unit);
    counters_config = wb_info->counters_config;

    switch(wb_info->version) {
        case BCM_DPP_WB_COUNTERS_VERSION_1_0:

            /* update framework cache and peristent storage                       */
            inUse_state = (bcm_dpp_wb_counters_inUse_config_t *)(wb_info->scache_ptr + wb_info->inUse_off);

            sd_arr_indx = 0;
            for (proc_idx = 0; proc_idx < arr_indx1; proc_idx++) {
                sd_arr_indx += BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_INUSE(proc_idx);
            }

            sd_arr_indx += arr_indx2;

            if(sd_arr_indx >= wb_info->inUse_num ||
               ((BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_INUSE(arr_indx1)) > 0 &&
                (arr_indx2)         >= (BCM_DPP_WB_COUNTERS_DBL_ARR_INER_SIZE_INUSE(arr_indx1)))) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Parameter error, index out of bound")));
            }

            inUse_state += sd_arr_indx;

            inUse_state->inUse = counters_config->proc[arr_indx1].inUse[arr_indx2];


            data_size = sizeof(bcm_dpp_wb_counters_inUse_config_t);
            data = (uint8 *)inUse_state;
            offset = (uint32)(data - wb_info->scache_ptr);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown scache data version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
BCMDNX_FUNC_RETURN;
}

#endif /* BCM_WARM_BOOT_SUPPORT */
